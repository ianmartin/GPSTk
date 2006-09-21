#pragma ident "$Id$"



/**
 * @file Stats.hpp
 * One and two-sample statistics
 */
 
#ifndef INCLUDE_GPSTK_STATS_HPP
#define INCLUDE_GPSTK_STATS_HPP

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

#include "MiscMath.hpp"
#include "Vector.hpp"
#include "Exception.hpp"

namespace gpstk
{
   /** @addtogroup math */
   //@{
 
/** Conventional statistics for one sample.  Constructor does the same as
 * Reset(); use this when starting a new series of input samples.
 * Results are available at any time by calling N(), Minimum(), Maximum(),
 * Average(), Variance() and StdDev().
 */
   template <class T>
   class Stats
   {
   public:
      /// constructor
      explicit Stats() { n=0; weighted=false; }

      /// constructor given a vector of data
      Stats(Vector<T>& X, Vector<T>& W)
      {
         n = 0;
         weighted = false;
         Add(X,W);
      }

      /// reset, i.e. ignore earlier data and restart sampling
      inline void Reset(void) { n=0; weighted=false; W = 0; }

      /// access the sample size
      inline unsigned int N(void) const { return n; }

      /// return minimum value
      inline T Minimum(void) const { if(n) return min; else return T(); }

      /// return maximum value
      inline T Maximum(void) const { if(n) return max; else return T(); }

      /// return computed average
      inline T Average(void) const
      {
         // normalization constant is (W=sum wts)/n, -> 1 when all wts=1
         if(n == 0)
            return T();
         if(weighted)
            return (T(n)*ave/W);
         return ave;
      }

      /// return computed variance
      inline T Variance(void) const
      {
         if(n == 0)
            return T();
         if(weighted) {
            T wn=W/T(n);
            return (var/wn/wn/wn/wn);
         }
         return var;
      }

      /// return computed standard deviation
      inline T StdDev(void) const
      {
         if(n == 0)
            return T();
         if(weighted) {
            T wn=W/T(n);
            return SQRT(ABS(var))/wn/wn;
         }
         return SQRT(ABS(var));
      }

      /// return the normalization constant = sum weights
      inline T Normalization(void) const
      { return W; }

      /// return weight flag
      inline bool Weighted(void) const
      { return weighted; }

      /// add a single sample to the computation of statistics, with optional weight
      void Add(const T& x, const T& wt=T())
      {
         if(wt != T()) weighted=true;

         T xx(x);
         n++;
         if(n == 1) {
            min = max = ave = xx;
            if(weighted) ave *= wt;
            var = T();
            W = T();
         }
         else {
            if(xx < min) min=xx;
            if(xx > max) max=xx;
         }

         if(weighted) { xx *= wt;  W += wt; }
         ave += (xx-ave)/T(n);
         if(n > 1)
            var = (var*T(n-2) + T(n)*(xx-ave)*(xx-ave)/T(n-1))/T(n-1);
      }

      /// add a Vector<T> of samples to the computation of statistics,
      /// with optional weights
      inline void Add(Vector<T>& X, Vector<T> w = Vector<T>())
      {
         if(w.size() > 0 && w.size() < X.size()) {
            Exception e("Inconsistent input: weight vector short");
            GPSTK_THROW(e);
         }

         size_t i;
         if(w.size() > 0) 
            for(i=0; i<X.size(); i++) Add(X(i),w(i));
         else
            for(i=0; i<X.size(); i++) Add(X(i));
      }

      /// remove a sample from the computation of statistics (can't do min and max).
      void Subtract(T x)
      {
         T dn=T(n);
         if(n > 2) var=(var*(dn-T(1))-dn*(x-ave)*(x-ave)/(dn-T(1)))/(dn-T(2));
         else var=T();
	
         if(n > 1)
	 { 
	 	ave=(ave*dn-x)/(dn-T(1));
		W--;
	 }
         else if(n==1) ave=x;
         else ave=T();

         n--;
      }

      /// remove a Vector<T> of samples to the computation of statistics
      inline void Subtract(Vector<T>& X)
      {
         for(size_t i=0; i<X.size(); i++) Subtract(X(i));
      }

      /// define private members directly; useful in continuing with an object
      /// that was earlier saved (e.g. to a file).
      void Load(unsigned int in_n, T in_min, T in_max, T in_ave, T in_var,
                  bool wtd=false, T norm=1.0)
      {
         n = in_n;
         min = in_min;
         max = in_max;
         var = in_var;
         ave = in_ave;
         weighted = wtd;
         W = norm;
      }
      
      /// combine two Stats (assumed taken from the same or equivalent samples);
      /// both must be either weighted or unweighted.
      Stats<T>& operator+=(const Stats<T>& S)
      {
         if(S.n == 0) return *this;
         if(n==0) {
            *this = S;
            return *this;
         }
         if((weighted && !S.weighted) || (!weighted && S.weighted)) {
            Exception e("Stats::operator+= : operands have inconsistent weighting");
            GPSTK_THROW(e);
         }

         if(S.min < min) min=S.min;
         if(S.max > max) max=S.max;

         if(weighted) {
            T W1 = W/n;
            T W2 = S.W/S.n;
            var  = W1*W1*((n-T(1))*var + n*ave*ave)
                 + W2*W2*((S.n-T(1))*S.var + S.n*S.ave*S.ave);
            var *= (n+S.n)/(W+S.W);
            var *= (n+S.n)/(W+S.W);
            W += S.W;
         }
         else {
            var  = ((n-T(1))*var + n*ave*ave);
            var += ((S.n-T(1))*S.var + S.n*S.ave*S.ave);
         }
         ave = (n*ave + S.n*S.ave)/(n+S.n);
         n += S.n;
         var -= n*ave*ave;
         var /= (n-T(1));

         return *this;

      }  // end Stats operator+=

      /// dump the data stored in the class

   private:
      /// Number of samples added to the statistics so far
      unsigned int n;
      /// Minimum value
      T min;
      /// Maximum value
      T max;
      /// Average value
      T var;
      /// Variance (square of the standard deviation)
      T ave;
      /// Normalization constant = sum weights
      T W;
      /// Flag weighted input; ALL input must be consistently weighted or not
      bool weighted;
   }; // end class Stats

   /// Output operator for Stats class
   template <class T>
   std::ostream& operator<<(std::ostream& s, const Stats<T>& ST) 
   {
      std::ofstream savefmt;
      savefmt.copyfmt(s);
      s << " N       = " << ST.N() << (ST.Weighted() ? " ":" not") << " weighted\n";
      s << " Minimum = "; s.copyfmt(savefmt); s << ST.Minimum();
      s << " Maximum = "; s.copyfmt(savefmt); s << ST.Maximum() << "\n";
      s << " Average = "; s.copyfmt(savefmt); s << ST.Average();
      s << " Std Dev = "; s.copyfmt(savefmt); s << ST.StdDev();
      return s;
   }

/** Conventional statistics for two samples.  Constructor does the same as
 * Reset(); use this when starting a new series of input samples.
 * Results are available at any time by calling N(), Minimum(), Maximum(),
 * Average(), Variance() and StdDev().
 */
   template <class T>
   class TwoSampleStats
   {
   public:
      /// constructor
      TwoSampleStats() { n=0; }

      /// constructor given two Vector of data
      TwoSampleStats(Vector<T>& X, Vector<T>& Y)
      {
         n = 0;
         Add(X,Y);
      }

      /// Add data to the statistics
      void Add(const T& X, const T& Y)
      {
         if(n == 0) {
            sumx = sumy = sumx2 = sumy2 = sumxy = T();
            xmin = xmax = X;
            ymin = ymax = Y;
            scalex = scaley = T(1);
         }
         if(scalex==T(1) && X!=T()) scalex=ABS(X);
         if(scaley==T(1) && Y!=T()) scaley=ABS(Y);
         sumx += X/scalex;
         sumy += Y/scaley;
         sumx2 += (X/scalex)*(X/scalex);
         sumy2 += (Y/scaley)*(Y/scaley);
         sumxy += (X/scalex)*(Y/scaley);
         if(X < xmin) xmin=X;
         if(X > xmax) xmax=X;
         if(Y < ymin) ymin=Y;
         if(Y > ymax) ymax=Y;
         n++;
      }

      /// Add two Vectors of data to the statistics
      void Add(const Vector<T>& X, const Vector<T>& Y)
      {
         size_t m = (X.size() < Y.size() ? X.size() : Y.size());
         if(m==0) return;
         for(size_t i=0; i<m; i++) Add(X(i),Y(i));
      }

      void Subtract(const T& X, const T& Y)
      {
         if(n == 1) {
            sumx = sumy = sumx2 = sumy2 = sumxy = T();
            xmin = xmax = T();
            ymin = ymax = T();
            scalex = scaley = T(1);
            return;
         }

         sumx -= X/scalex;
         sumy -= Y/scaley;
         sumx2 -= (X/scalex)*(X/scalex);
         sumy2 -= (Y/scaley)*(Y/scaley);
         sumxy -= (X/scalex)*(Y/scaley);
         n--;
      }

      void Subtract(const Vector<T>& X, const Vector<T>& Y)
      {
         size_t m=(X.size()<Y.size()?X.size():Y.size());
         if(m==0) return;
         for(size_t i=0; i<m; i++) Subtract(X(i),Y(i));
      }

      /// reset, i.e. ignore earlier data and restart sampling
      inline void Reset(void) { n=0; }

      /// return the sample size
      inline unsigned int N(void) const { return n; }
      /// return minimum X value
      inline T MinimumX(void) const { if(n) return xmin; else return T(); }
      /// return maximum X value
      inline T MaximumX(void) const { if(n) return xmax; else return T(); }
      /// return minimum Y value
      inline T MinimumY(void) const { if(n) return ymin; else return T(); }
      /// return maximum Y value
      inline T MaximumY(void) const { if(n) return ymax; else return T(); }

      /// return computed X average
      inline T AverageX(void) const
         { if(n>0) return (scalex*sumx/T(n)); else return T(); }

      /// return computed Y average
      inline T AverageY(void) const
         { if(n>0) return (scaley*sumy/T(n)); else return T(); }

      /// return computed X variance
      inline T VarianceX(void) const
      {
         if(n>1) return scalex*scalex*(sumx2-sumx*sumx/T(n))/T(n-1);
         else return T();
      }

      /// return computed Y variance
      inline T VarianceY(void) const
      {
         if(n>1) return scaley*scaley*(sumy2-sumy*sumy/T(n))/T(n-1);
         else return T();
      }

      /// return computed X standard deviation
      inline T StdDevX(void) const { return SQRT(VarianceX()); }

      /// return computed Y standard deviation
      inline T StdDevY(void) const { return SQRT(VarianceY()); }

      /// return slope of best-fit line Y=slope*X + intercept
      inline T Slope(void) const
      {
         if(n>0)
            return ((scaley/scalex)*(sumxy-sumx*sumy/T(n))/(sumx2-sumx*sumx/T(n)));
         else
            return T();
      }

      /// return intercept of best-fit line Y=slope*X + intercept
      inline T Intercept(void) const
      {
         if(n>0)
            return (AverageY()-Slope()*AverageX());
         else
            return T();
      }

      /// return uncertainty in slope
      inline T SigmaSlope(void) const
      {
         if(n>2)
            return (SigmaYX()/(StdDevX()*SQRT(T(n-1))));
         else
            return T();
      }

      /// return correlation
      inline T Correlation(void) const
      {
         if(n>1)
         {
            return ( scalex * scaley * (sumxy-sumx*sumy/T(n))
               / (StdDevX()*StdDevY()*T(n-1)) );
         }
         else
            return T();
      }

      /// return conditional uncertainty = uncertainty y given x
      inline T SigmaYX(void) const
      {
         if(n>2)
         {
            return (StdDevY() * SQRT(T(n-1)/T(n-2))
                  * SQRT(T(1)-Correlation()*Correlation()) );
         }
         else return T();
      }

      /// combine two TwoSampleStats (assumed to be taken from the same or
      /// equivalent samples)
      TwoSampleStats<T>& operator+=(TwoSampleStats<T>& S)
      {
         if(n + S.n == 0) return *this;
         if(S.xmin < xmin) xmin=S.xmin;
         if(S.xmax > xmax) xmax=S.xmax;
         if(S.ymin < ymin) ymin=S.ymin;
         if(S.ymax > ymax) ymax=S.ymax;
         sumx += S.scalex*S.sumx/scalex;
         sumy += S.scaley*S.sumy/scaley;
         sumx2 += (S.scalex/scalex)*(S.scalex/scalex)*S.sumx2;
         sumy2 += (S.scaley/scaley)*(S.scaley/scaley)*S.sumy2;
         sumxy += (S.scalex/scalex)*(S.scaley/scaley)*S.sumxy;
         n += S.n;
         return *this;
      }  // end Stats operator+=

   private:
      /// Number of samples added to the statistics so far
      unsigned int n;
      T xmin, xmax, ymin, ymax, scalex, scaley;
      T sumx, sumy, sumx2, sumy2, sumxy;

   }; // end class TwoSampleStats

   /// Output operator for TwoSampleStats class
   template <class T>
   std::ostream& operator<<(std::ostream& s, const TwoSampleStats<T>& TSS) 
   {
      std::ofstream savefmt;
      savefmt.copyfmt(s);
      s << " N       = " << TSS.N() << "\n";
      s << " Minimum: X = "; s.copyfmt(savefmt); s << TSS.MinimumX();
      s << "  Y = "; s.copyfmt(savefmt); s << TSS.MinimumY();
      s << "  Maximum: X = "; s.copyfmt(savefmt); s << TSS.MaximumX();
      s << "  Y = "; s.copyfmt(savefmt); s << TSS.MaximumY() << "\n";
      s << " Average: X = "; s.copyfmt(savefmt); s << TSS.AverageX();
      s << "  Y = "; s.copyfmt(savefmt); s << TSS.AverageY();
      s << "  Std Dev: X = "; s.copyfmt(savefmt); s << TSS.StdDevX();
      s << "  Y = "; s.copyfmt(savefmt); s << TSS.StdDevY() << "\n";
      s << " Intercept = "; s.copyfmt(savefmt); s << TSS.Intercept();
      s << "  Slope = "; s.copyfmt(savefmt); s << TSS.Slope();
      s << " with uncertainty = "; s.copyfmt(savefmt); s << TSS.SigmaSlope() << "\n";
      s << " Conditional uncertainty (sigma y given x) = ";
      s.copyfmt(savefmt); s << TSS.SigmaYX();
      s << "  Correlation = "; s.copyfmt(savefmt); s << TSS.Correlation() << "\n";
      return s;
   }

   /** Compute the median of a vector */
   template <class T>
   inline T median(const Vector<T>& v)
   {
      if(v.size()==0) return T();
      if(v.size()==1) return v(0);
      if(v.size()==2) return (v(0)+v(1))/T(2);
      // insert sort
      size_t i,j;
      T x;
      Vector<T> w(v);
      for(i=0; i<v.size(); i++) {
         x = w[i] = v(i);
         j = i-1;
         while(j>=0 && x<w[j]) {
            w[j+1] = w[j];
            j--;
         }
         w[j+1] = x;
      }
      if(v.size() % 2)
         x=w[(v.size()+1)/2-1];
      else
         x=(w[v.size()/2-1]+w[v.size()/2])/T(2);

      return x;
   }  // end median(Vector)

   //@}

}  // namespace

#endif
