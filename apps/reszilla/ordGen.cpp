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


#include <MSCData.hpp>
#include <MSCStream.hpp>

#include "OrdApp.hpp"
#include "OrdEngine.hpp"
#include "ObsReader.hpp"
#include "EphReader.hpp"
#include "MetReader.hpp"


using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;

class OrdGen : public OrdApp
{
public:
   OrdGen() throw();
   
   bool initialize(int argc, char *argv[]) throw();

protected:
   virtual void spinUp();
   virtual void process();

private:
   CommandOptionWithAnyArg obsFileOption, ephFileOption, mscFileOption,
      antennaPosOption, metFileOption, ordModeOption;
   CommandOptionWithNumberArg msidOption;
   CommandOptionNoArg svTimeOption, keepWartsOption;

   string ordMode;
   bool svTime;
   bool keepWarts;
   Triple antennaPos;
   unsigned msid;

   static const string defaultOrdMode;
};

const string OrdGen::defaultOrdMode("p1p2");

//-----------------------------------------------------------------------------
// The constructor basically just sets up all the command line options
//-----------------------------------------------------------------------------
OrdGen::OrdGen() throw()
   : OrdApp("ordGen", "Generates observed range deviations."),
     ordMode(defaultOrdMode), svTime(false), keepWarts(false), msid(0),
   
   obsFileOption('o', "obs", 
                 "Where to get the obs data.", true),
   
   ephFileOption('e', "eph",  "Where to get the ephemeris data. Can be "
                 " rinex, fic, or sp3", true),
   
   mscFileOption('c', "msc", "Station coordinate file"),
   
   msidOption('m', "msid", "Station to process data for. Used to select "
              "a station position from the msc file or data from a SMODF file."),
   
   ordModeOption('\0', "omode", "ORD mode: p1p2, c1p2, c1, p1, c2, p2, smo. "
                 "Note that the smo mode often requires the --svtime "
                 "option to be specified. The default is " + defaultOrdMode),
   
   antennaPosOption('p', "pos", "Location of the antenna in meters ECEF."
                    , false),
   
   metFileOption('w', "weather", "Weather data file name (RINEX met "
                 "format only)."),
   
   svTimeOption('s', "svtime", "Observation data is in SV time frame. "
                "The default is RX time frame."),
   
   keepWartsOption('k', "keep-warts", "Keep any warts that are in "
                   "the data. The defailt is to remove them.")
{
}


//-----------------------------------------------------------------------------
// Here the command line options parsed and used to configure the program
//-----------------------------------------------------------------------------
bool OrdGen::initialize(int argc, char *argv[]) throw()
{
   if (!OrdApp::initialize(argc,argv)) return false;

   if (ordModeOption.getCount())
      ordMode = lowerCase(ordModeOption.getValue()[0]);

   svTime = svTimeOption.getCount();

   if (keepWartsOption.getCount())
      keepWarts=true;
      
   if (msidOption.getCount())
      msid = asUnsigned(msidOption.getValue().front());

   return true;
}


//-----------------------------------------------------------------------------
// General program setup
//-----------------------------------------------------------------------------
void OrdGen::spinUp()
{
   // Get the station position
   if (antennaPosOption.getCount())
   {
      string aps = antennaPosOption.getValue()[0];
      if (numWords(aps) != 3)
      {
         cerr << "Please specify three coordinates in the antenna postion." << endl;
         exit(-1);
      }
      else
         for (int i=0; i<3; i++)
            antennaPos[i] = asDouble(word(aps, i));
   }
   else if (msid && mscFileOption.getCount() > 0)
   {
      string mscfn = (mscFileOption.getValue())[0];
      MSCStream msc(mscfn.c_str(), ios::in);
      MSCData mscd;
      while (msc >> mscd && mscd.station != msid)
         ;
      if (mscd.station == msid)
         antennaPos = mscd.coordinates;
   }
   else
   {
      string fn = (obsFileOption.getValue())[0];
      ObsReader obsReader(fn, verboseLevel);
      if (obsReader.inputType == FFIdentifier::tRinexObs)
         antennaPos = obsReader.roh.antennaPosition;
   }
      
   if (RSS(antennaPos[0], antennaPos[1], antennaPos[2]) < 1)
   {
      cerr << "Warning! The antenna appears to be within one meter of the" << endl
           << "center of the geoid. This program is not capable of" << endl
           << "accurately estimating the propigation of GNSS signals" << endl
           << "through solids such as a planetary crust or magma. Also," << endl
           << "if this location is correct, your antenna is probally" << endl
           << "no longer in the best of operating condition." << endl;
      exit(-1);
   }

   if (verboseLevel)
   {
      if (msid)
         cout << "# msid: " << msid << endl;
      cout << "# Antenna Position: " << antennaPos << endl;
      cout << "# Observed Rage Deviation (ORD) mode: " << ordMode << endl;
   }
}


//-----------------------------------------------------------------------------
void OrdGen::process()
{
   // Get the ephemeris data
   EphReader ephReader;
   ephReader.verboseLevel = verboseLevel;
   for (int i=0; i<ephFileOption.getCount(); i++)
      ephReader.read(ephFileOption.getValue()[i]);
   gpstk::EphemerisStore& eph = *ephReader.eph;

   // Get the weather data...
   MetReader metReader;
   metReader.verboseLevel = verboseLevel;
   for (int i=0; i<ephFileOption.getCount(); i++)
      ephReader.read(ephFileOption.getValue()[i]);
   WxObsData& wod = metReader.wx;

   // Use a New Brunswick trop model.
   NBTropModel tm;

   // Now set up the function object that is used to compute the ords.
   OrdEngine ordEngine(eph, wod, antennaPos, tm);
   ordEngine.svTime = svTimeOption;
   ordEngine.keepWarts = keepWarts;
   ordEngine.setMode(ordMode);
   ordEngine.verbosity = verboseLevel;
   ORDEpochMap ordEpochMap;

   // Walk through each obs file, reading and computing ords along the way.
   for (int i=0; i<obsFileOption.getCount(); i++)
   {
      string fn = (obsFileOption.getValue())[i];
      ObsReader obsReader(fn, verboseLevel);
      obsReader.msid = msid;

      while (obsReader())
      {
         ObsEpoch obs(obsReader.getObsEpoch());

         if (!obsReader())
            break;

         ORDEpoch oe = ordEngine(obs);

         write(output, oe);
      }
   }
}


//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   try
   {
      OrdGen crap;
      if (!crap.initialize(argc, argv))
         exit(0);
      crap.run();
   }
   catch (gpstk::Exception &exc)
   { cout << exc << endl; }
   catch (std::exception &exc)
   { cerr << "Caught std::exception " << exc.what() << endl; }
   catch (...)
   { cerr << "Caught unknown exception" << endl; }
}
