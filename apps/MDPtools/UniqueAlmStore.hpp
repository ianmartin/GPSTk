#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/MDPtools/UniqueAlmStore.hpp#4 $"

/**
 * @file UniqueAlmStore.hpp
 *
 *  Almanac uniqueness has always be a challenge.  Here's the working definition
 *  for this module:
 *    1. Collected within a single 12.5 min cycle
 *    2. Collected from the same PRN
 *    3. SF5,Pg25 - complete cycle - SF4,Pg1 all must have the same Toa
 *    4. SVID available for each PRN for which almanac health is other than
 *          "6 ones"  (see IS-GPS-200, 20.3.????)
 */

#ifndef GPSTK_UNIQUEALMSTORE_HPP
#define GPSTK_UNIQUEALMSTORE_HPP

//lgpl-license START
//lgpl-license END

//dod-release-statement START
//dod-release-statement END
#include "FICStream.hpp"
#include "DayTime.hpp"
#include "EngAlmanac.hpp"

   // Project
#include "miscdefs.hpp"
#include "MDPNavSubframe.hpp"

namespace gpstk
{
      //  The int in the following map is the SVID
      //  The MDPNavSubframe is the corresponding almanac page
   typedef std::map <short,gpstk::MDPNavSubframe> PageMap;
   typedef PageMap::iterator pmI;
   typedef PageMap::const_iterator pmCI;
   
   class UniqueAlmStore
   {
   public:
         /// Default constructor
      UniqueAlmStore( gpstk::NavIndex ni, gpstk::NavCode nc );
      
         /// Destructor
      virtual ~UniqueAlmStore( ) {}

      pmCI begin() const;
      pmCI end() const; 
      void newSubframe( gpstk::MDPNavSubframe sf );
      bool readyToWrite() const;
      void write(gpstk::FICStream& out);
      
   protected:
      bool completeSetOfPages() const;
      short fullWeekFrom8Bit( const short full, const short eightBit );
      bool isToaPage( const short SVID );
      long getToa( const MDPNavSubframe& nav );
   
      int state;              /// Current state of the collection process
                              ///    See table of static const below.
      bool written;           /// Has this almanac been written out since last collection?
      long numPagesExamined;
      
      int prn;                /// < The SV's PRN
      CarrierCode carrier;    /// < This almanac's carrier frequency code
      RangeCode range;        /// < This alamanc's range code
      NavCode navCode;            /// < This almanac's nav code
      
      short SVIDOrderNdx;
      
      long startingSOW;
      long candidateToa;
      gpstk::DayTime ToaTime;    // Not valid until state==COMPLETE | WRITTEN
      PageMap pageMap; 
      
         // Possible values for member state
      static const int WAITING = 0;
      static const int START_ON_NEXT_FRAME = 3;
      static const int COLLECTING = 1;
      static const int COMPLETE = 2;
      
         // Other useful "magic numbers"
      static const long ALMANAC_PERIOD = 750;      // 12.5 minutes in seconds
      static const long SF4_OFFSET = 24;
      static const short HALF_8BITS = 128;
      static const short DEAD_HEALTH = 0x3F;
      
   }; // class UniqueAlmStore
} // namespace

#endif
