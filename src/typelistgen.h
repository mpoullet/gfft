/***************************************************************************
 *   Copyright (C) 2012-2015 by Vladimir Mirnyy                            *
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


#ifndef __typelistgen_h
#define __typelistgen_h

/** \file
    \brief Typelist generation classes
*/

#include "gfftpolicy.h"

namespace GFFT {

template<typename T>
struct getID {
   static const id_t ID = T::ID;
};

template<ulong_t N>
struct getID<ulong_<N> > {
   static const id_t ID = N-1;
};

/** \class {GFFT::ListGenerator}
    \brief Generates all different combinations of given parameters.
\tparam TList is one- or two-dimensional (an entry can be a Typelist too) TypeList.
\tparam TLenList is Typelist of \a ulong_<N>, where N is maximum possible
        lengths of every Typelist in TList. This array of numbers is used 
        to compute unique ID for each generated unique set of parameters.
\tparam DefTrans is transform definition class. 
        DefineTransform class is substituted here, but also definitions of 
        other template classes with suited parameters are potentially possible.
\tparam WorkingList is the working Typelist, which accumulates a unique set of
        parameters. It is being passed as parameter to the class DefineTransform,
        when the set is complete. This parameter must not be set explicitely.
\tparam ID unique id for each set of parameters. This static constant is generated 
        at compile-time and must not be set explicitely.

The parameters are given in the two-dimensional compile-time array.
This metaprogram takes one parameter from every TList's entry
and generates Typelist of unique sets of parameters to define Transform.
The entry may be either a type or a Typelist.
*/
template<class TList, class TLenList, 
         template<class,id_t> class DefTrans,
         class WorkingList=Loki::NullType, id_t ID=0>
struct ListGenerator;

// H is a simple type
template<class H, class Tail, ulong_t N, class NTail,
         template<class,id_t> class DefTrans, class WorkingList, id_t ID>
struct ListGenerator<Loki::Typelist<H,Tail>,Loki::Typelist<ulong_<N>,NTail>,DefTrans,WorkingList,ID>
{
   static const id_t nextID = (ID*N) + getID<H>::ID;
   typedef Loki::Typelist<H,WorkingList> WList;
   typedef typename ListGenerator<Tail,NTail,DefTrans,WList,nextID>::Result Result;
};

// Typelist is in the head
template<class H, class T, class Tail, ulong_t N, class NTail,
         template<class,id_t> class DefTrans, class WorkingList, id_t ID>
struct ListGenerator<Loki::Typelist<Loki::Typelist<H,T>,Tail>,Loki::Typelist<ulong_<N>,NTail>,DefTrans,WorkingList,ID>
{
   static const id_t nextID = (ID*N) + getID<H>::ID;
   typedef Loki::Typelist<H,WorkingList> WList;
   typedef typename ListGenerator<Loki::Typelist<T,Tail>,Loki::Typelist<ulong_<N>,NTail>,DefTrans,WorkingList,ID>::Result L1;
   typedef typename ListGenerator<Tail,NTail,DefTrans,WList,nextID>::Result L2;
   typedef typename Loki::TL::Append<L1,L2>::Result Result;
};

template<class H, class Tail, ulong_t N, class NTail,
         template<class,id_t> class DefTrans, class WorkingList, id_t ID>
struct ListGenerator<Loki::Typelist<Loki::Typelist<H,Loki::NullType>,Tail>,Loki::Typelist<ulong_<N>,NTail>,DefTrans,WorkingList,ID>
{
   static const id_t nextID = (ID*N) + getID<H>::ID;
   typedef Loki::Typelist<H,WorkingList> WList;
   typedef typename ListGenerator<Tail,NTail,DefTrans,WList,nextID>::Result Result;
};

template<template<class,id_t> class DefTrans, class WorkingList, id_t ID>
struct ListGenerator<Loki::NullType,Loki::NullType,DefTrans,WorkingList,ID>
{
   typedef Loki::Typelist<typename DefTrans<WorkingList,ID>::Result,Loki::NullType> Result;
};



/// Generates Typelist with types Holder<N>, N = Begin,...,End
template<long_t Begin, long_t End,
template<long_t> class Holder = long_>
struct GenNumList {
   typedef Loki::Typelist<Holder<Begin>,
      typename GenNumList<Begin+1,End,Holder>::Result> Result;
};

template<long_t End,
template<long_t> class Holder>
struct GenNumList<End,End,Holder> {
   typedef Loki::Typelist<Holder<End>,Loki::NullType> Result;
};


template<long_t M, long_t P>
struct PowerHolder {
   static const long_t N = IPow<M,P>::value;
   static const long_t ID = N-1;
   static const long_t value = N;
};

template<long_t P>
struct PowerHolder<2,P> {
   static const long_t N = 1<<P;
   static const long_t ID = N-1;
   static const long_t value = N;
};

template<long_t P>
struct Power2holder : public PowerHolder<2,P> {};

template<long_t P>
struct Power3holder : public PowerHolder<3,P> {};


/// Generates Typelist with types Holder<N>, N = Begin,...,End
template<long_t PowerBegin, long_t PowerEnd, long_t M>
struct GenPowerList {
   typedef Loki::Typelist<PowerHolder<M,PowerBegin>,
      typename GenPowerList<PowerBegin+1,PowerEnd,M>::Result> Result;
};

template<long_t PowerEnd, long_t M>
struct GenPowerList<PowerEnd,PowerEnd,M> {
   typedef Loki::Typelist<PowerHolder<M,PowerEnd>,Loki::NullType> Result;
};

}  //namespace

#endif
