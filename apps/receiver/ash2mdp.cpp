#pragma ident "$Id$"

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//  
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

#include <string>
#include <sstream>

#include <list>
#include <map>

#include "DayTime.hpp"
#include "GPSWeekSecond.hpp"
#include "TimeConstants.hpp"
#include "Exception.hpp"
#include "CommandOption.hpp"
#include "CommandOptionParser.hpp"
#include "DeviceStream.hpp"

#include "StringUtils.hpp"
#include "InOutFramework.hpp"

#include "MDPStream.hpp"
#include "MDPNavSubframe.hpp"
#include "MDPObsEpoch.hpp"

#include "AshtechStream.hpp"
#include "AshtechMBEN.hpp"
#include "AshtechPBEN.hpp"
#include "AshtechALB.hpp"
#include "AshtechEPB.hpp"

#include "ObsUtils.hpp"

using namespace std;
using namespace gpstk;


class Ashtech2MDP : public InOutFramework<AshtechStream, MDPStream>
{
public:
   Ashtech2MDP(const string& applName)
      throw()
      : InOutFramework<AshtechStream,MDPStream>(
         applName, "Converts Ashtech Z(Y)-12 serial streaming format to "
         "MDP format.")
   {}

   bool initialize(int argc, char *argv[]) throw()
    {
       CommandOptionWithNumberArg weekOption(
          'w', "week",
          "The full GPS week in which this data starts");

      if (!InOutFramework<AshtechStream, MDPStream>::initialize(argc,argv))
         return false;

      DayTime now;
      time.week = now.GPSfullweek();
      time.sow = now.GPSsecond();

      if (weekOption.getCount())
      {
         time.week = StringUtils::asInt(weekOption.getValue()[0]);
         time.sow = HALFWEEK;
      }
         

      AshtechData::debugLevel = debugLevel;
      if (debugLevel>3)
         AshtechData::hexDump = true;

      if (debugLevel>3)
         MDPHeader::hexDump = true;

      if (debugLevel)
         cout << "Initial time: " << time.printf("%F %.1g") << endl;

      return true;
   }
   
protected:
   virtual void spinUp()
   {}

   virtual void process()
   {
      bool firstPBEN = false;

      AshtechData hdr;
      AshtechPBEN pben;
      AshtechMBEN mben;
      AshtechEPB  epb;
      AshtechALB  alb;
      unsigned short fc=0;
      vector<MDPObsEpoch> hint(33);
      short svCount = 0;

      while (input >> hdr)
      {
         if (pben.checkId(hdr.id) && (input >> pben) && pben)
         {
            if (debugLevel>2)
               pben.dump(cout);

            double dt = pben.sow - time.sow;
            time.sow = pben.sow;
            svCount = 0;

            if (std::abs(dt) > HALFWEEK && !firstPBEN)
               time.week++;

            firstPBEN = true;

            MDPPVTSolution pvt = makeMDPPVTSolution(pben, time.week);
            pvt.freshnessCount = fc++;
            if (debugLevel<2)
               output << pvt << flush;
            else if (debugLevel>2)
               pvt.dump(cout);
         }
         else if (mben.checkId(hdr.id) && (input >> mben) && mben)
         {
            if (debugLevel>2)
               mben.dump(cout);
            if (svCount==0)
               svCount = mben.left+1;

            if (firstPBEN == true)
            {
               hint[mben.svprn].time = DayTime(time.week, time.sow);
               hint[mben.svprn].numSVs = svCount;
               MDPObsEpoch moe = makeMDPObsEpoch(mben, hint[mben.svprn]);
               moe.freshnessCount = fc++;
               hint[mben.svprn] = moe;
            if (debugLevel<2)
               output << moe << flush;
            else if (debugLevel>2)
               moe.dump(cout);
            }
         }
         else if (epb.checkId(hdr.id) && (input >> epb))
         {
            if (debugLevel>2)
               epb.dump(cout);
            MDPNavSubframe sf[3];
            
         }
         else if (alb.checkId(hdr.id) && (input >> alb))
         {
            if (debugLevel>2)
               alb.dump(cout);
            MDPNavSubframe sf;
         }
      }

      timeToDie = true;
   }

   virtual void shutDown()
   {}

   GPSWeekSecond time;
};


int main(int argc, char *argv[])
{
   Ashtech2MDP crap(argv[0]);
   
   if (!crap.initialize(argc, argv))
      exit(0);
   
   crap.run();
}
