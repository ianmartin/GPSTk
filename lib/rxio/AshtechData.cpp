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

#include "StringUtils.hpp"
#include "BinUtils.hpp"

#include "AshtechData.hpp"
#include "AshtechStream.hpp"

using namespace std;

namespace gpstk
 {
   //---------------------------------------------------------------------------
   //---------------------------------------------------------------------------
   // This is the string that preceeds every message from the receiver.
   const char* AshtechData::preamble = "$PASHR,";

   // Set to zero for no debugging output
   // set to 1 to output text messages about decode/format/range errors
   // set to 2 to add a hex dump of those messages
   // set to 3+ to add the tossed bytes whether or not they are bad
   int AshtechData::debugLevel = 0;

   // set true to print a hex dump of every message to cout
   bool AshtechData::hexDump = false;


   //---------------------------------------------------------------------------
   // Compute the CRC of the string and set the crcbit appropriately.
   void AshtechData::checkCRC(string str)
      throw()
   {
      // Nope, we don't do this. for now
      if (false)
      {
         clearstate(crcbit);
         return;
      }
   } // AshtechData::checkCRC()


   //---------------------------------------------------------------------------
   void AshtechData::reallyGetRecord(FFStream& ffs)
      throw(exception, FFStreamError, EndOfFile)
   {
      // Note that this will generate a bad_cast exception if it doesn't work.
      AshtechStream& stream=dynamic_cast<AshtechStream&>(ffs);

      // make sure the object is reset before starting the search
      clear(fmtbit | lenbit | crcbit);
      id.clear();
      stream.rawData.erase();

      readHeader(stream);
   } // AshtechData::reallyGetRecord()


   //---------------------------------------------------------------------------
   void AshtechData::readHeader(AshtechStream& stream)
      throw(FFStreamError, EndOfFile)
   {
      char buff[11];
      string& rawData = stream.rawData;
      while (stream.read(buff, sizeof(buff)) && id == "")
      {
         rawData.append(buff, stream.gcount());
         if (rawData.size()>=11 && 
             rawData.substr(0,7) == preamble &&
             rawData[10]==',')
         {
            id = rawData.substr(7,3);
            break;
         }

         rawData.erase(0, rawData.find(preamble[0]));
      }
   }

   //---------------------------------------------------------------------------
   void AshtechData::readBody(AshtechStream& stream)
      throw(FFStreamError, EndOfFile)
   {
      string& rawData = stream.rawData;
      const string term("\015\012");

      while (rawData.substr(rawData.length()-2,2) != term)
      {
         string buff;
         getline(stream, buff, term[1]);
         rawData.append(buff);
         rawData.append(term.substr(1,1));
      }
      if (debugLevel>2)
         StringUtils::hexDumpData(cout, rawData);

      decode(rawData);
      if (!good() && debugLevel>1)
         cout << "bad decode" << endl;

      if (hexDump || (debugLevel>1 && rdstate()))
      {
         cout << "Record Number:" << stream.recordNumber << endl;
         StringUtils::hexDumpData(cout, rawData);
      }
   }


   //---------------------------------------------------------------------------
   void AshtechData::dump(ostream& out) const throw()
   {
      ostringstream oss;
      oss << getName() << " : id:" << id << " rdstate:" << rdstate();
      if (crcerr())
         oss << "-crc";
      if (fmterr())
         oss << "-fmt";
      if (lenerr())
         oss << "-len";
      if (parerr())
         oss << "-par";

      out << oss.str() << endl;
   }  // AshtechData::dump()
} // namespace gpstk