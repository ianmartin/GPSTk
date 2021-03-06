#pragma ident "$Id: xVector.cpp 1979 2009-06-24 18:05:07Z afarris $"
// file: xVector.cpp

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
//  Copyright 2009, The University of Texas at Austin
//
//============================================================================

#include "xVector.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION (xVector);

void xVector :: setUp (void) 
{ 
}	

void xVector :: getTest (void)
{
   gpstk::Vector<double> v1(250,1.0);
   gpstk::Vector<double> v2(1000,5.0);

   for(int i = 0; i < v1.size(); i++)
   {
      CPPUNIT_ASSERT_EQUAL(1.,v1[i]);
      CPPUNIT_ASSERT_EQUAL(1.,v1(i));
   }

   for(int i = 0; i < v2.size(); i++)
   {
      CPPUNIT_ASSERT_EQUAL(5.,v2[i]);
      CPPUNIT_ASSERT_EQUAL(5.,v2(i));
   }
}

void xVector :: operatorTest (void)
{
   gpstk::Vector<double> v1(3,1.0);
   gpstk::Vector<double> v2(12,3.0);
   gpstk::Vector<double> v3 = v2;
   gpstk::Vector<double> v4 = -v1;

   for(int i = 0; i < v2.size(); i++)
   {
      CPPUNIT_ASSERT_EQUAL(v2[i],v3[i]);
   }

   for(int i = 0; i < v1.size(); i++)
   {
      CPPUNIT_ASSERT_EQUAL(-v1[i],v4[i]);
   }
   v2 += v3; // 6 6 6 ...
   v2 += 2; // 8 8 8 ...
   for(int i = 0; i < v2.size(); i++)
   {
      CPPUNIT_ASSERT_EQUAL(8.,v2[i]);
   }

   v2 -= v3; // 5 5 5 ...
   v2 -= 4; // 1 1 1 ...
   
   for(int i = 0; i < v1.size(); i++)
   {
      CPPUNIT_ASSERT_EQUAL(v1[i],v2[i]); //sizes mismatch, check till v1 ends
   }

   v2 += 2; // 3 3 3 ...
   
   v1 = v1&&v2; // 3 3 3 3 3 3 ...
   CPPUNIT_ASSERT_EQUAL((size_t)15,v1.size());
   v1 = v1&&v3;
   CPPUNIT_ASSERT_EQUAL((size_t)27,v1.size());
}

