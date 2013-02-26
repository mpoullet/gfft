/***************************************************************************
 *   Copyright (C) 2006-2013 by Volodymyr Myrnyy                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 ***************************************************************************/

#ifndef __gfftalg_h
#define __gfftalg_h

/** \file
    \brief Recursive algorithms and short-radix FFT specifications
*/

#include "gfftspec.h"


namespace GFFT {

using namespace MF;


template<typename T>
struct TempTypeTrait;

template<>
struct TempTypeTrait<float> {
   typedef double Result;
};

template<>
struct TempTypeTrait<double> {
   typedef long double Result;
};

template<typename T,
template<typename> class Complex>
struct TempTypeTrait<Complex<T> > {
   typedef typename TempTypeTrait<T>::Result Result;
};

template<typename T, typename A,
template<typename,typename> class Complex>
struct TempTypeTrait<Complex<T,A> > {
   typedef typename TempTypeTrait<T>::Result Result;
};

// template<typename T, typename A,
// template<typename,typename> class Complex>
// struct TempTypeTrait<Complex<T,A> > {
//    typedef T Result;
// };

// Look here for a small prime factor using 6k+1, 6k+5 algorithm
// until some relative small limit (e.g. 100)
// then rely on some prime factor algorithm like Rader (only for primes), Winograd or Bluestein (for any n)
// then come back to factoring
template<unsigned long N>
struct GetNextFactor {
  // dummy trial: assume multiple of 2 and 3 only
  static const int value = (N%2 == 0) ? 2 : 3; 
};


/// Danielson-Lanczos section of the decimation-in-time FFT version
/**
\tparam N current transform length
\tparam T value type of the data array
\tparam S sign of the transform: 1 - forward, -1 - backward

This template class implements resursive metaprogram, which
runs funciton apply() twice recursively at the beginning of the function apply()
with the half of the transform length N
until the simplest case N=2 has been reached. Then function \a _spec2 is called.
Therefore, it has two specializations for N=2 and N=1 (the trivial and empty case).
\sa InFreq
*/
template<unsigned long N, typename T, int S, int LastK = 1>
class InTime {
   typedef typename TempTypeTrait<T>::Result LocalVType;
   static const unsigned int K = GetNextFactor<N>::value;
   static const unsigned int M = N/K;
   static const unsigned int M2 = M*2;
   InTime<M,T,S,K*LastK> next;
public:
   void apply(T* data) {

      next.apply(data);
      next.apply(data + M2);

      LocalVType wtemp,tempr,tempi,wr,wi,t;

//    Change dynamic calculation to the static one
//      wtemp = sin(S*M_PI/N);
      t = Sin<N,1,LocalVType>::value();
      const LocalVType wpr = -2.0*t*t;
//      wpi = -sin(2*M_PI/N);
      const LocalVType wpi = -S*Sin<N,2,LocalVType>::value();
      wr = 1.0;
      wi = 0.0;
      for (unsigned long i=0; i<M2; i+=K) {
	const int i2 = i+M2;
        tempr = data[i2]*wr - data[i2+1]*wi;
        tempi = data[i2]*wi + data[i2+1]*wr;
        data[i2] = data[i]-tempr;
        data[i2+1] = data[i+1]-tempi;
        data[i] += tempr;
        data[i+1] += tempi;

        wtemp = wr;
        wr += wr*wpr - wi*wpi;
        wi += wi*wpr + wtemp*wpi;
      }
   }

   void apply(const T* src, T* dst) {

      next.apply(src, dst);
      next.apply(src + 2*LastK, dst + M2);
//      next.apply(src+M2, dst+M2);

      LocalVType wtemp,tempr,tempi,wr,wi,t;

      t = Sin<N,1,LocalVType>::value();
      const LocalVType wpr = -2.0*t*t;
      const LocalVType wpi = -S*Sin<N,2,LocalVType>::value();
      wr = 1.0;
      wi = 0.0;
      for (unsigned long i=0; i<M2; i+=K) {
	const int i2 = i+M2;
        tempr = dst[i2]*wr - dst[i2+1]*wi;
        tempi = dst[i2]*wi + dst[i2+1]*wr;
        dst[i2] = dst[i]-tempr;
        dst[i2+1] = dst[i+1]-tempi;
        dst[i] += tempr;
        dst[i+1] += tempi;

        wtemp = wr;
        wr += wr*wpr - wi*wpi;
        wi += wi*wpr + wtemp*wpi;
      }
   }
};

/*
/// Specialization for N=4, decimation-in-time
template<typename T, int S>
class InTime<4,T,S> {
public:
   void apply(T* data) {
      T tr = data[2];
      T ti = data[3];
      data[2] = data[0]-tr;
      data[3] = data[1]-ti;
      data[0] += tr;
      data[1] += ti;
      tr = data[6];
      ti = data[7];
      data[6] = S*(data[5]-ti);
      data[7] = S*(tr-data[4]);
      data[4] += tr;
      data[5] += ti;

      tr = data[4];
      ti = data[5];
      data[4] = data[0]-tr;
      data[5] = data[1]-ti;
      data[0] += tr;
      data[1] += ti;
      tr = data[6];
      ti = data[7];
      data[6] = data[2]-tr;
      data[7] = data[3]-ti;
      data[2] += tr;
      data[3] += ti;
   }
};
*/

// Specialization for N=3, decimation-in-time
template<typename T, int S, int LastK>
class InTime<3,T,S,LastK> {
public:
   void apply(T* data) { _spec3_fwd(data); }
   void apply(const T* src, T* dst) { _spec3_fwd(src, dst); }
};

// Specialization for N=2, decimation-in-time
template<typename T, int S, int LastK>
class InTime<2,T,S,LastK> {
  static const int K2 = LastK*2;
public:
  void apply(T* data) 
  { 
      _spec2(data); 
//       const T tr = data[K2];
//       const T ti = data[K2+1];
//       data[K2] = data[0]-tr;
//       data[K2+1] = data[1]-ti;
//       data[0] += tr;
//       data[1] += ti;
  }
  void apply(const T* src, T* dst) 
  { 
//    _spec2(src, dst); 
    const T v1(src[1]), v2(src[K2]), v3(src[K2+1]);
    dst[0] = (*src + v2);
    dst[1] = (v1 + v3);
    dst[2] = (*src - v2);
    dst[3] = (v1 - v3);
  }
};

// Specialization for N=1, decimation-in-time
template<typename T, int S, int LastK>
class InTime<1,T,S,LastK> {
public:
   void apply(T* data) { }
   void apply(const T* src, T* dst) { }
};


/// Danielson-Lanczos section of the decimation-in-frequency FFT version
/**
\tparam N current transform length
\tparam T value type of the data array
\tparam S sign of the transform: 1 - forward, -1 - backward

This template class implements resursive metaprogram, which
runs funciton apply() twice recursively at the end of the function apply()
with the half of the transform length N
until the simplest case N=2 has been reached. Then function \a _spec2 is called.
Therefore, it has two specializations for N=2 and N=1 (the trivial and empty case).
\sa InTime
*/
template<unsigned long N, typename T, int S>
class InFreq {
   typedef typename TempTypeTrait<T>::Result LocalVType;
   InFreq<N/2,T,S> next;
public:
   void apply(T* data) {

      LocalVType wtemp,tempr,tempi,wr,wi,wpr,wpi;
//    Change dynamic calculation to the static one
//      wtemp = sin(M_PI/N);
      wtemp = Sin<N,1,LocalVType>::value();
      wpr = -2.0*wtemp*wtemp;
//      wpi = -sin(2*M_PI/N);
      wpi = -S*Sin<N,2,LocalVType>::value();
      wr = 1.0;
      wi = 0.0;
      for (unsigned long i=0; i<N; i+=2) {
        tempr = data[i] - data[i+N];
        tempi = data[i+1] - data[i+N+1];
        data[i] += data[i+N];
        data[i+1] += data[i+N+1];
        data[i+N] = tempr*wr - tempi*wi;
        data[i+N+1] = tempi*wr + tempr*wi;

        wtemp = wr;
        wr += wr*wpr - wi*wpi;
        wi += wi*wpr + wtemp*wpi;
      }

      next.apply(data);
      next.apply(data+N);
   }

   void apply(const T*, T*) { }
};

// Specialization for N=4, decimation-in-frequency
// template<typename T, int S>
// class InFreq<4,T,S> {
// public:
//    void apply(T* data) {
//       T tr = data[4];
//       T ti = data[5];
//       data[4] = data[0]-tr;
//       data[5] = data[1]-ti;
//       data[0] += tr;
//       data[1] += ti;
//       tr = data[6];
//       ti = data[7];
//       data[6] = S*(data[3]-ti);
//       data[7] = S*(tr-data[2]);
//       data[2] += tr;
//       data[3] += ti;
// 
//       tr = data[2];
//       ti = data[3];
//       data[2] = data[0]-tr;
//       data[3] = data[1]-ti;
//       data[0] += tr;
//       data[1] += ti;
//       tr = data[6];
//       ti = data[7];
//       data[6] = data[4]-tr;
//       data[7] = data[5]-ti;
//       data[4] += tr;
//       data[5] += ti;
//    }
// };

// Specialization for N=2, decimation-in-frequency
template<typename T, int S>
class InFreq<2,T,S> {
public:
   void apply(T* data) { _spec2(data); }
   void apply(const T* src, T* dst) { _spec2(src, dst); }
};

// Specialization for N=1, decimation-in-frequency
template<typename T, int S>
class InFreq<1,T,S> {
public:
   void apply(T* data) { }
   void apply(const T*, T*) { }
};


/// Binary reordering of array elements
/*!
\tparam N length of the data
\tparam T value type

This is C-like implementation. It has been written very 
similar to the one presented in the book 
"Numerical recipes in C++".
\sa GFFTswap2
*/
template<unsigned long N, typename T>
class GFFTswap {
public:
   void apply(T* data) {
     unsigned long m, j = 0;
     for (unsigned long i=0; i<2*N-1; i+=2) {
        if (j>i) {
            std::swap(data[j], data[i]);
            std::swap(data[j+1], data[i+1]);
        }
        m = N;
        while (m>=2 && j>=m) {
            j -= m;
            m >>= 1;
        }
        j += m;
     }
   }

   void apply(const T*, T*) { }
};


/// Binary reordering of array elements
/*!
\tparam N length of the data
\tparam T value type

This is second version of binary reordering.
It is based on template class recursion
similar to InTime and InFreq template classes,
where member function apply() is called twice recursively
building the parameters n and r, which are at last the
indexes of exchanged data values.

This version is slightly slower than GFFTswap, but 
allows parallelization of this algorithm, which is
implemented in template class GFFTswap2OMP.
\sa GFFTswap, GFFTswap2OMP
*/
template<unsigned int P, typename T,
unsigned int I=0>
class GFFTswap2 {
   static const unsigned long BN = 1<<(I+1);
   static const unsigned long BR = 1<<(P-I);
   GFFTswap2<P,T,I+1> next;
public:
   void apply(T* data, unsigned long n=0, unsigned long r=0) {
      next.apply(data,n,r);
      next.apply(data,n|BN,r|BR);
   }
};

template<unsigned int P, typename T>
class GFFTswap2<P,T,P> {
public:
   void apply(T* data, unsigned long n=0, unsigned long r=0) {
      if (n>r) {
        swap(data[n],data[r]);
        swap(data[n+1],data[r+1]);
      }
   }
};


/// Reordering of data for real-valued transforms
/*!
\tparam N length of the data
\tparam T value type
\tparam S sign of the transform: 1 - forward, -1 - backward
*/
template<unsigned long N, typename T, int S>
class Separate {
   typedef typename TempTypeTrait<T>::Result LocalVType;
   static const int M = (S==1) ? 2 : 1;
public:
   void apply(T* data) {
      unsigned long i,i1,i2,i3,i4;
      LocalVType wtemp,wr,wi,wpr,wpi;
      LocalVType h1r,h1i,h2r,h2i,h3r,h3i;
      wtemp = Sin<2*N,1,LocalVType>::value();
      wpr = -2.*wtemp*wtemp;
      wpi = -S*Sin<N,1,LocalVType>::value();
      wr = 1.+wpr;
      wi = wpi;
      for (i=1; i<N/2; ++i) {
        i1 = i+i;
        i2 = i1+1;
        i3 = 2*N-i1;
        i4 = i3+1;
        h1r = 0.5*(data[i1]+data[i3]);
        h1i = 0.5*(data[i2]-data[i4]);
        h2r = S*0.5*(data[i2]+data[i4]);
        h2i =-S*0.5*(data[i1]-data[i3]);
        h3r = wr*h2r - wi*h2i;
        h3i = wr*h2i + wi*h2r;
        data[i1] = h1r + h3r;
        data[i2] = h1i + h3i;
        data[i3] = h1r - h3r;
        data[i4] =-h1i + h3i;

        wtemp = wr;
        wr += wr*wpr - wi*wpi;
        wi += wi*wpr + wtemp*wpi;
      }
      h1r = data[0];
      data[0] = M*0.5*(h1r + data[1]);
      data[1] = M*0.5*(h1r - data[1]);

      if (N>1) data[N+1] = -data[N+1];
   }
   
   void apply(const T*, T*) { }
};

// Policy for a definition of forward FFT
template<unsigned long N, typename T>
struct Forward {
   enum { Sign = 1 };
   void apply(T*) { }
   void apply(const T*, T*) { }
};

template<unsigned long N, typename T,
template<typename> class Complex>
struct Forward<N,Complex<T> > {
   enum { Sign = 1 };
   void apply(Complex<T>*) { }
   void apply(const Complex<T>*, Complex<T>*) { }
};

// Policy for a definition of backward FFT
template<unsigned long N, typename T>
struct Backward {
   enum { Sign = -1 };
   void apply(T* data) {
      for (T* i=data; i<data+2*N; ++i) *i/=N;
   }
   void apply(const T*, T* dst) {
      for (T* i=dst; i<dst+2*N; ++i) *i/=N;
   }
};

template<unsigned long N, typename T,
template<typename> class Complex>
struct Backward<N,Complex<T> > {
   enum { Sign = -1 };
   void apply(Complex<T>* data) {
      for (unsigned long i=0; i<N; ++i) {
        data[i]/=N;
      }
   }
   void apply(const Complex<T>*, Complex<T>* dst) {
      for (unsigned long i=0; i<N; ++i) {
        dst[i]/=N;
      }
   }
};


}  //namespace DFT

#endif /*__gfftalg_h*/
