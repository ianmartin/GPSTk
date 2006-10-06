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
#include <iostream>
#include <iomanip>

#include "FFIdentifier.hpp"

#include "SP3EphemerisStore.hpp"
#include "RinexEphemerisStore.hpp"
#include "GPSGeoid.hpp"

#include "RinexObsStream.hpp"
#include "RinexObsData.hpp"

#include "RinexNavStream.hpp"
#include "RinexNavData.hpp"
#include "RinexMetStream.hpp"
#include "RinexMetData.hpp"

#include "FICStream.hpp"
#include "FICData.hpp"

#include "SMODFStream.hpp"
#include "SMODFData.hpp"

#include "MSCData.hpp"
#include "MSCStream.hpp"

#include "MDPObsEpoch.hpp"
#include "MDPPVTSolution.hpp"
#include "MDPNavSubframe.hpp"
#include "MDPSelftestStatus.hpp"
#include "MDPStream.hpp"

namespace gpstk
{
   FFIdentifier::FFIdentifier(const std::string& fn)
   {
      int verbosity = 0;
      const int recCount = 2; 
      fileType=tUnknown;

      while (true)
      {
         try
         {
            RinexObsStream ros(fn.c_str(), std::ios::in);
            ros.exceptions(std::fstream::failbit);
            if (verbosity>2)
               std::cout << "Trying " << fn << " as RINEX obs."<< std::endl;

            RinexObsHeader temp_roh;
            ros >> temp_roh;
            RinexObsData rod;
            ros >> rod;
            fileType = tRinexObs;
            break;
         } 
         catch (FFStreamError& e)
         {
            if (verbosity > 3) 
               std::cout << e << std::endl;
         }

         try
         {
            SMODFStream smo(fn.c_str(), std::ios::in);
            smo.exceptions(std::fstream::failbit);
            if (verbosity>2)
               std::cout << "Trying " << fn << " as SMODF."<< std::endl;
         
            SMODFData smodata;       
            smo >> smodata;
            smo >> smodata;
            fileType = tSMODF;
            break;
         }
         catch (FFStreamError& e)
         {
            if (verbosity > 3)
               std::cout << e << std::endl;
         }

         try
         {
            MDPStream mdps(fn.c_str(), std::ios::in);
            mdps.exceptions(std::fstream::failbit);
            if (verbosity>2)
               std::cout << "Trying " << fn << " as MDP."<< std::endl;

            MDPHeader header;
            mdps >> header;
            mdps >> header;
            fileType = tMDP;
            break;
         }
         catch (FFStreamError& e)
         {
            if (verbosity > 3)
               std::cout << e << std::endl;
         }

         try
         {
            RinexNavStream rns(fn.c_str(), std::ios::in);
            rns.exceptions(std::ifstream::failbit);
            if (verbosity>2)
               std::cout << "Trying " << fn << " as RINEX nav."<< std::endl;

            RinexNavData rnd;
            rns >> rnd;
            rns >> rnd;
            fileType = tRinexNav;
            break;
         }
         catch (FFStreamError& e)
         {
            if (verbosity > 3)
               std::cout << e << std::endl;
         }

         try
         {
            FICStream fs(fn.c_str(), std::ios::in);
            if (verbosity>2)
               std::cout << "Trying " << fn << " as FIC nav."<< std::endl;
      
            FICHeader header;
            fs >> header;      
            FICData data;
            fs >> data;
            fileType = tFIC;
            break;
         }
         catch (FFStreamError& e)
         {
            if (verbosity > 3)
               std::cout << e <<std:: endl;
         }

         try
         {

            SP3Stream pefile(fn.c_str(), std::ios::in);
            pefile.exceptions(std::ifstream::failbit);
            if (verbosity>2)
               std::cout << "Trying " << fn << " as SP3 ephemeris."<<std:: endl;
            
            SP3Header header;
            pefile >> header;
            SP3Data data;
            pefile >> data;
            fileType = tSP3;
            break;
         }
         catch (FFStreamError& e)
         {
            if (verbosity > 3)
               std::cout << e << std::endl;
         }

         try
         {
            MSCStream msc(fn.c_str(), std::ios::in);
            msc.exceptions(std::ifstream::failbit);
            if (verbosity>2)
               std::cout << "Trying " << fn << " as MSC."<<std:: endl;

            MSCData mscd;
            msc >> mscd;
            fileType = tMSC;
            break;
         }
         catch (FFStreamError& e)
         {
            if (verbosity > 3)
               std::cout << e << std::endl;
         }
         

         break;
      } // end of while (true)
   }

   FFIdentifier::operator FFType()
   {
      return fileType;
   }
}