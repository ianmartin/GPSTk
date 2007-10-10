
/**
 * @file SolverBase.hpp
 * Abstract base class for solver algorithms.
 */

#ifndef GPSTK_SOLVER_BASE_HPP
#define GPSTK_SOLVER_BASE_HPP

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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2006, 2007
//
//============================================================================



#include "Exception.hpp"
#include "Matrix.hpp"
#include "Vector.hpp"


namespace gpstk
{
    /// Thrown when some problem appeared when solving a given equation set
    /// @ingroup exceptiongroup
    NEW_EXCEPTION_CLASS(InvalidSolver, gpstk::Exception);


    /** @addtogroup GPSsolutions */
    /// @ingroup math
    //@{

    /**
     * Abstract base class for solver algorithms.
     */
    class SolverBase
    {
    public:

        /// Implicit constructor
        SolverBase() : valid(false) {};

        /// Return validity of results
        bool isValid(void)
            { return valid; }

        /// Solution
        Vector<double> solution;

        /// Postfit-residuals.
        Vector<double> postfitResiduals;

        /// Covariance matrix
        Matrix<double> covMatrix;

        /// Destructor
        virtual ~SolverBase() {};


    protected:
        bool valid;         // true only if results are valid

   }; // end class SolverBase
   

   //@}
   
}

#endif
