#pragma ident "$Id: TabularEphemerisStore.cpp 1430 2008-10-30 20:59:14Z architest $"

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

/**
 * @file TabularEphemerisStore.cpp
 * Store a tabular list of Xvt data (such as a table of precise ephemeris data
 * in an SP3 file) and compute Xvt from this table. A Lagrange interpolation
 * is used to compute the Xvt for times that are not in the table but do have
 * sufficient data.
 */

#include "TabularEphemerisStore.hpp"
#include "MiscMath.hpp"
#include "ECEF.hpp"
#include "icd_200_constants.hpp"

using namespace gpstk::StringUtils;

namespace gpstk
{


      /* A debugging function that outputs in human readable form,
       * all data stored in this object.
       *
       * @param[in] s the stream to receive the output; defaults to cout
       * @param[in] detail the level of detail to provide
       */
   void TabularEphemerisStore::dump( std::ostream& s,
                                     short detail )
      const throw()
   {

      s << "Dump of TabularEphemerisStore:" << std::endl;

      if(detail >= 0)
      {

         EphMap::const_iterator it;

         s << " Data stored for " << pe.size() << " satellites, over time span "
           << initialTime << " to " << finalTime << "." << std::endl;

         if(detail == 0) return;

         for(it=pe.begin(); it!=pe.end(); it++)
         {

            s << "  PRN " << it->first << " : "
              << it->second.size() << " records.";
            if(detail == 1) { s << std::endl; continue; }
            s << "  Data:" << std::endl;
            SvEphMap::const_iterator jt;

            for(jt=it->second.begin(); jt!=it->second.end(); jt++)
            {
               s << " " << jt->first << " P "
                 << std::fixed << std::setprecision(6)
                 << std::setw(13) << jt->second.x[0] << " "
                 << std::setw(13) << jt->second.x[1] << " "
                 << std::setw(13) << jt->second.x[2] << " "
                 << std::setw(13) << jt->second.dtime
                 << " V "
                 << std::setw(13) << jt->second.v[0] << " "
                 << std::setw(13) << jt->second.v[1] << " "
                 << std::setw(13) << jt->second.v[2] << " "
                 << std::setw(13) << jt->second.ddtime
                 << std::endl;
            }

         }

      }  // End of 'if(detail >= 0)...'

   }  // End of method 'TabularEphemerisStore::dump()'



      /* Edit the dataset, removing data outside the indicated time
       * interval.
       *
       * @param[in] tmin defines the beginning of the time interval
       * @param[in] tmax defines the end of the time interval
       */
   void TabularEphemerisStore::edit( const DayTime& tmin,
                                     const DayTime& tmax )
      throw()
   {

      EphMap::iterator kt;

      for(kt=pe.begin(); kt!=pe.end(); kt++)
      {

         SvEphMap::reverse_iterator jt=(kt->second).rbegin();

         while(jt != (kt->second).rend())
         {

            if(jt->first < tmin || jt->first > tmax)
            {
               (kt->second).erase(jt->first);
            }

            jt ++;

         }

      }

      initialTime = tmin;
      finalTime = tmax;

   }  // End of method 'TabularEphemerisStore::edit()'



      // Remove all data
   void TabularEphemerisStore::clear()
      throw()
   {

      pe.clear();
      initialTime = DayTime::END_OF_TIME;
      finalTime = DayTime::BEGINNING_OF_TIME;

   }  // End of method 'TabularEphemerisStore::clear()'



      /* Returns the position, velocity, and clock offset of the indicated
       * object in ECEF coordinates (meters) at the indicated time.
       *
       * @param[in] id the object's identifier
       * @param[in] t the time to look up
       *
       * @return the Xvt of the object at the indicated time
       *
       * @throw InvalidRequest If the request can not be completed for any
       *    reason, this is thrown. The text may have additional
       *    information as to why the request failed.
       */
   Xvt TabularEphemerisStore::getXvt( const SatID sat,
                                      const DayTime& t )
      const throw(InvalidRequest)
   {

      EphMap::const_iterator svmap = pe.find(sat);
      if (svmap==pe.end()) {
         InvalidRequest e("Ephemeris for satellite  " + asString(sat)
                            + " not found.");
         GPSTK_THROW(e);
      }

      const SvEphMap& sem=svmap->second;
      SvEphMap::const_iterator i=sem.find(t);
      Xvt sv;
      if (i!= sem.end() && haveVelocity) {      // exact match of t
         sv = i->second;
         sv.x[0] *= 1.e3;     // m
         sv.x[1] *= 1.e3;     // m
         sv.x[2] *= 1.e3;     // m
         sv.dtime *= 1.e-6;   // sec
         sv.v[0] *= 1.e-1;    // m/sec
         sv.v[1] *= 1.e-1;    // m/sec
         sv.v[2] *= 1.e-1;    // m/sec
         sv.ddtime *= 1.e-10; // sec/sec

         sv.dtime += -2*(sv.x[0]/C_GPS_M)*(sv.v[0]/C_GPS_M)
            -2*(sv.x[1]/C_GPS_M)*(sv.v[1]/C_GPS_M)
            -2*(sv.x[2]/C_GPS_M)*(sv.v[2]/C_GPS_M);
         return sv;
      }

         // Note that the order of the Lagrange interpolation
         // is twice this value
      const int half=5;

         //  i will be the lower bound, j the upper (in time).
      i = sem.lower_bound(t); // i points to first element with key >= t

      SvEphMap::const_iterator j=i;

      if(i == sem.begin() || --i == sem.begin()) {
         InvalidRequest e("Inadequate data before requested time, satellite "
                          + asString(sat));
         GPSTK_THROW(e);
      }
      if(j == sem.end()) {
         InvalidRequest e("Inadequate data after requested time, satellite "
                          + asString(sat));
         GPSTK_THROW(e);
      }

         // "t" is now just between "i" and "j"; therefore, it is time to check
         // for data gaps ("checkDataGap" must be enabled for this).
      if ( checkDataGap                               &&
           ( std::abs( t - i->first ) > gapInterval ) &&
           ( std::abs( j->first - t ) > gapInterval ) )
      {
            // There was a data gap
         InvalidRequest e( "Data gap too wide detected for satellite "
                           + asString(sat) );
         GPSTK_THROW(e);
      }

      for(int k=0; k<half-1; k++)
      {

         i--;

            // if k==half-2, this is last iteration
         if(i == sem.begin() && k<half-2)
         {
            InvalidRequest e("Inadequate data before requested time, satellite "
                             + asString(sat));
            GPSTK_THROW(e);
         }
         j++;
         if(j == sem.end() && k<half-2) {
            InvalidRequest e("Inadequate data after requested time, satellite "
                               + asString(sat));
            GPSTK_THROW(e);
         }
      }

         // Now that we have fully defined the i-j interval, let's check if
         // the interpolation interval is too wide ("checkInterval" must be
         // enabled for this).
      if ( checkInterval                                     &&
           ( std::abs( j->first - i->first ) > maxInterval ) )
      {
            // There was a data gap
         InvalidRequest e( "Interpolation interval too wide detected for SV "
                           + asString(sat) );
         GPSTK_THROW(e);
      }


         // pull data and interpolate
      SvEphMap::const_iterator itr;
      DayTime t0=i->first;
      double dt=t-t0,err;
      std::vector<double> times,X,Y,Z,T,VX,VY,VZ,F;

      for (itr=i; itr!=sem.end(); itr++)
      {
         times.push_back(itr->first - t0);      // sec
         X.push_back(itr->second.x[0]);         // km
         Y.push_back(itr->second.x[1]);         // km
         Z.push_back(itr->second.x[2]);         // km
         T.push_back(itr->second.dtime);        // microsec
         VX.push_back(itr->second.v[0]);        // decimeters/sec
         VY.push_back(itr->second.v[1]);        // decimeters/sec
         VZ.push_back(itr->second.v[2]);        // decimeters/sec
         F.push_back(itr->second.ddtime);       // 1.e-4 microsec/sec
         if(itr == j) break;
      }

      if (haveVelocity)
      {
         sv.x[0] = LagrangeInterpolation(times,X,dt,err);
         sv.x[1] = LagrangeInterpolation(times,Y,dt,err);
         sv.x[2] = LagrangeInterpolation(times,Z,dt,err);
         sv.dtime = LagrangeInterpolation(times,T,dt,err);
         sv.v[0] = LagrangeInterpolation(times,VX,dt,err);
         sv.v[1] = LagrangeInterpolation(times,VY,dt,err);
         sv.v[2] = LagrangeInterpolation(times,VZ,dt,err);
         sv.ddtime = LagrangeInterpolation(times,F,dt,err);
      }
      else {
         LagrangeInterpolation(times,X,dt,sv.x[0],sv.v[0]);
         LagrangeInterpolation(times,Y,dt,sv.x[1],sv.v[1]);
         LagrangeInterpolation(times,Z,dt,sv.x[2],sv.v[2]);
         LagrangeInterpolation(times,T,dt,sv.dtime,sv.ddtime);
         sv.v[0] *= 1.e4;              // decimeters/sec
         sv.v[1] *= 1.e4;              // decimeters/sec
         sv.v[2] *= 1.e4;              // decimeters/sec
         sv.ddtime *= 1.e4;            // 1.e-4 microsec/sec
      }

      sv.x[0] *= 1.e3;     // m
      sv.x[1] *= 1.e3;     // m
      sv.x[2] *= 1.e3;     // m
      sv.dtime *= 1.e-6;   // sec
      sv.v[0] *= 1.e-1;    // m/sec
      sv.v[1] *= 1.e-1;    // m/sec
      sv.v[2] *= 1.e-1;    // m/sec
      sv.ddtime *= 1.e-10; // sec/sec

      // add relativity correction to dtime
      // this only for consistency with GPSEphemerisStore::getSatXvt ....
      // dtr = -2*dot(R,V)/(c*c) = -4.4428e-10 * ecc * sqrt(A(m))*sinE
      // (do it this way for numerical reasons)
      sv.dtime += -2*(sv.x[0]/C_GPS_M)*(sv.v[0]/C_GPS_M)
         -2*(sv.x[1]/C_GPS_M)*(sv.v[1]/C_GPS_M)
         -2*(sv.x[2]/C_GPS_M)*(sv.v[2]/C_GPS_M);

      return sv;

   }  // end Xvt TabularEphemerisStore::getSatXvt


   //-----------------------------------------------------------------------------
   //-----------------------------------------------------------------------------
   void TabularEphemerisStore::addEphemeris(const SP3Data& data)
      throw()
   {
      DayTime t = data.time;
      SatID sat = data.sat;
      Xvt&  xvt = pe[sat][t];

      if (data.flag=='P')
      {
         xvt.x = ECEF(data.x[0], data.x[1], data.x[2]);
         xvt.dtime = data.clk;
         haveVelocity=false;
      }
      else if (data.flag=='V')
      {
         xvt.v = Triple(data.x[0],data.x[1],data.x[2]);
         xvt.ddtime = data.clk;
         haveVelocity=true;
      }
      
      if (t<initialTime)
         initialTime = t;
      else if (t>finalTime)
         finalTime = t;
   }

}  // namespace gpstk
