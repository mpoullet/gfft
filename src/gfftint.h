/***************************************************************************
 *   Copyright (C) 2009-2015 by Vladimir Mirnyy                            *
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

/** \file
    \brief Definition of holders for static integer types
*/

#ifndef gfftint_h
#define gfftint_h

#include "sint.h"

typedef long int_t;
//typedef unsigned long uint_t; // defined in sint.h
typedef unsigned short short_t;

/// Integer number metacontainer.
/**     Integer N is wrapped into container class to handle integers and other
        compile-time number-classes using the same operation classes specializing
        them for particular number-container.
 \param N an integer number
*/
template<int_t N>
struct long_ {
  typedef int_t value_type;
  static const int_t value = N;
};


#define STATIC_INTEGER_CLASS(Type, Name) \
template<Type N>                         \
struct s_##Name {                        \
   typedef Type value_type;              \
   static const Type value = N;          \
};

STATIC_INTEGER_CLASS(int, int)
STATIC_INTEGER_CLASS(unsigned int, uint)
STATIC_INTEGER_CLASS(long, long)
STATIC_INTEGER_CLASS(unsigned long, ulong)
#undef STATIC_INTEGER_CLASS


//template<typename T1, typename T2>
//struct Pair {
//  typedef T1 first;
//  typedef T2 second;
//};

#endif