#pragma ident "$Id: SatID.hpp 105 2006-08-31 15:53:26Z ocibu $"

#ifndef GPSTK_SP3_SATID_HPP
#define GPSTK_SP3_SATID_HPP

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

#include <iostream>
#include <sstream>
#include <iomanip>

#include "Exception.hpp"
#include "SatID.hpp"

/**
 * @file SP3SatID.hpp
 * gpstk::SP3SatID - navigation system-independent representation of a satellite
 * as defined by the SP3 specification.
 */

namespace gpstk
{
   class SP3SatID : public SatID
   {
   public:

      /// empty constructor, creates an invalid object
      SP3SatID() throw() { id=-1; system=systemGPS; }

      /// explicit constructor, no defaults, SP3 systems only
      SP3SatID(int p, SatelliteSystem s) throw()
      {
         id = p; system = s;
         switch(system) {
            case systemGPS:
            case systemGlonass:
            case systemGalileo:
            case systemLEO: break;
            // invalidate anything non-SP3
            default:
               system = systemGPS;
               id = -1;
         }
      }

      /// constructor from string
      SP3SatID(std::string& str) throw(Exception)
         try { this->fromString(str); }
         catch(Exception& e) { GPSTK_RETHROW(e); }

      /// cast SatID to SP3SatID
      SP3SatID(const SatID& sat) throw()
         { *this = SP3SatID(sat.id,sat.system); }

      /// set the fill character used in output
      /// return the current fill character
      char setfill(char c) throw()
         { char csave=fillchar; fillchar=c; return csave; }

      /// get the fill character used in output
      char getfill() throw()
         { return fillchar; }

      // operator=, copy constructor and destructor built by compiler

      /// return a character based on the system
      /// return the single-character system descriptor
      /// @note return only SP3 types, for non-SP3 systems return '?'
      char systemChar() const throw()
      {
         switch(system) {
            case systemGPS: return 'G';
            case systemGalileo: return 'E';
            case systemGlonass: return 'R';
            case systemLEO: return 'L';
            // non-SP3
            default: return '?';
         }
      };

      /// read from string
      /// @note GPS is default system (no or unknown system char)
      void fromString(const std::string s) throw(Exception)
      {
         char c;
         std::istringstream iss(s);

         id = -1; system = systemGPS;  // default
         if(s.find_first_not_of(std::string(" \t\n"), 0) == std::string::npos)
            return;                    // all whitespace yields the default

         iss >> c;                     // read one character (non-whitespace)
         switch(c)
         {
                                       // no leading system character
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
               iss.putback(c);
               system = SatID::systemGPS;
               break;
            case ' ': case 'G': case 'g':
               system = SatID::systemGPS;
               break;
            case 'R': case 'r':
               system = SatID::systemGlonass;
               break;
            //case 'T': case 't':        // not in SP3 specification
               //system = SatID::systemTransit;
               //break;
            //case 'S': case 's':        // not in SP3 specification
               //system = SatID::systemGeosync;
               //break;
            case 'E': case 'e':
               system = SatID::systemGalileo;
               break;
            case 'L': case 'l':
               system = SatID::systemLEO;
               break;
            default:                   // non-SP3 system character
               Exception e(std::string("Invalid system character \"")
                           + c + std::string("\""));
               GPSTK_THROW(e);
         }
         iss >> id;
         if(id <= 0) id = -1;
      }

      /// convert to string
      std::string toString() const throw()
      {
         std::ostringstream oss;
         char savechar=oss.fill(fillchar);
         oss << systemChar()
             << std::setw(2) << id
             << std::setfill(savechar);
          return oss.str();
      }

   private:

      static char fillchar;  ///< fill character used during stream output

   }; // class SP3SatID

   /// stream output for SP3SatID
   inline std::ostream& operator<<(std::ostream& s, const SP3SatID& sat)
   {
      s << sat.toString();
      return s;
   }

} // namespace gpstk

#endif