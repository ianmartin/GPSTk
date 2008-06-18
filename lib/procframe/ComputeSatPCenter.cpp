#pragma ident "$Id$"

/**
 * @file ComputeSatPCenter.cpp
 * This class computes the satellite antenna phase correction, in meters.
 */

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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2008
//
//============================================================================


#include "ComputeSatPCenter.hpp"


namespace gpstk
{

      // Index initially assigned to this class
   int ComputeSatPCenter::classIndex = 1500000;


      // Returns an index identifying this object.
   int ComputeSatPCenter::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string ComputeSatPCenter::getClassName() const
   { return "ComputeSatPCenter"; }


      /* Returns a satTypeValueMap object, adding the new data generated when
       * calling this object.
       *
       * @param time      Epoch corresponding to the data.
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& ComputeSatPCenter::Process(const DayTime& time,
                                           satTypeValueMap& gData)
   {

         // Compute Sun position at this epoch
      SunPosition sunPosition;
      Triple sunPos(sunPosition.getPosition(time));

         // Define a Triple that will hold satellite position, in ECEF
      Triple svPos(0.0, 0.0, 0.0);

      SatIDSet satRejectedSet;

         // Loop through all the satellites
      satTypeValueMap::iterator it;
      for (it = gData.begin(); it != gData.end(); ++it) 
      {

            // Use ephemeris if satellite position is not already computed
         if( ( (*it).second.find(TypeID::satX) == (*it).second.end() ) ||
             ( (*it).second.find(TypeID::satY) == (*it).second.end() ) ||
             ( (*it).second.find(TypeID::satZ) == (*it).second.end() ) )
         {
            if(pEphemeris==NULL)
            {
                  // If ephemeris is missing, then remove all satellites
               satRejectedSet.insert( (*it).first );
               continue;
            }
            else
            {
                  // Try to get satellite position
                  // if it is not already computed
               try
               {
                     // For our purposes, position at receive time 
                     // is fine enough
                  Xvt svPosVel(pEphemeris->getXvt( (*it).first, time ));

                     // If everything is OK, then continue processing.
                  svPos[0] = svPosVel.x.theArray[0];
                  svPos[1] = svPosVel.x.theArray[1];
                  svPos[2] = svPosVel.x.theArray[2];

               }
               catch(...)
               {
                     // If satellite is missing, then schedule it for removal
                  satRejectedSet.insert( (*it).first );
                  continue;
               }
            }

         }
         else
         {
               // Get satellite position out of GDS
            svPos[0] = (*it).second[TypeID::satX];
            svPos[1] = (*it).second[TypeID::satY];
            svPos[2] = (*it).second[TypeID::satZ];
         }

            // Let's get the satellite antenna phase correction value in
            // meters, and insert it in the GNSS data structure.
         (*it).second[TypeID::satPCenter] = 
            getSatPCenter((*it).first, time, svPos, sunPos);

      }
         // Remove satellites with missing data
      gData.removeSatID(satRejectedSet);

      return gData;
   }



      /* Sets name of "PRN_GPS"-like file containing satellite data.
       * @param name      Name of satellite data file.
       */
   ComputeSatPCenter& ComputeSatPCenter::setFilename(const string& name)
   {
      fileData = name;
      satData.open(fileData);

      return (*this);
   };



      /* Compute the value of satellite antenna phase correction, in meters.
       * @param sat       Satellite ID
       * @param time      Epoch of interest
       * @param satpos    Satellite position, as a Triple
       * @param sunpos    Sun position, as a Triple
       *
       * @return Satellite antenna phase correction, in meters.
       */
   double ComputeSatPCenter::getSatPCenter( const SatID& satid,
                                            const DayTime& time,
                                            const Triple& sat,
                                            const Triple& sunPosition )
   {

         // Unitary vector from satellite to Earth mass center
      Triple rk( ( (-1.0)*(sat.unitVector()) ) );

         // Unitary vector from Earth mass center to Sun
      Triple ri( sunPosition.unitVector() );

         // rj = rk x ri
      Triple rj(rk.cross(ri));

         // Redefine ri: ri = rj x rk
      ri = rj.cross(rk);

         // Let's convert ri to an unitary vector.
      ri = ri.unitVector();

         // Get vector from Earth mass center to receiver
      Triple rxPos(nominalPos.X(), nominalPos.Y(), nominalPos.Z());

         // Compute unitary vector vector from satellite to RECEIVER
      Triple rrho( (rxPos-sat).unitVector() );

         // If satellite belongs to block "IIR", its correction is 0.0,
         // else, it will depend on satellite model.
         // Variable that will hold the correction, 0.0 by default (IIR)
      double svPCcorr(0.0);

         // For satellites II and IIA:
      if( (satData.getBlock( satid, time ) == "II") ||
          (satData.getBlock( satid, time ) == "IIA") )
      {

            // First, build satellite antenna vector for models II/IIA
         Triple svAntenna(0.279*ri + 1.023*rk);

            // Projection of "svAntenna" vector to line of sight vector (rrho)
         svPCcorr =  (rrho.dot(svAntenna));

      }
      else
      {
         if( (satData.getBlock( satid, time ) == "I") )
         {

               // First, build satellite antenna vector for model I
            Triple svAntenna(0.210*ri + 0.854*rk);

               // Projection of "svAntenna" to line of sight vector (rrho)
            svPCcorr =  (rrho.dot(svAntenna));
         }
      }

         // This correction is interpreted as an "advance" in the signal,
         // instead of a delay. Therefore, it has negative sign
      return (-svPCcorr);

   } // End of ComputeSatPCenter::getSatPCenter()


} // end namespace gpstk