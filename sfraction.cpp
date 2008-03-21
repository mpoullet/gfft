//
// C++ Implementation: sfraction
//
// Description:
//
//
// Author: Volodymyr Myrnyy <vm@scientificcpp.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <iostream>

//#include "sfraction.h"
#include "sbigint.h"

static const int AN = 1000;
static const int BN = 20;

 typedef SInt<AN> C1;
 typedef SInt<BN> C2;
//
// typedef Add<A,B>::Result AddAB;
// typedef Mult<A,B>::Result MultAB;
//
// typedef SFraction<A,B> SF;
// typedef Simplify<SF>::Result Simp;
static const unsigned A = 65000;
static const unsigned B = 6500;
static const unsigned int Base = 1<<(sizeof(IntT)*4);
typedef NUMLIST_2(A,A) L1;
typedef NUMLIST_2(A+1,A) L11;
typedef NUMLIST_2(B,B) L2;

typedef SBigInt<true,L1,Base> BI1;
typedef SBigInt<true,L2,Base> BI2;
typedef SBigInt<true,L11,Base> BI11;
typedef Negate<BI11>::Result BI3;
typedef Mult<BI2,C1>::Result Sum;
/*typedef Add<Sum1,BI1>::Result Sum2;
typedef Sub<Sum2,BI1>::Result Sum3;
typedef Sub<Sum3,BI1>::Result Sum;*/
//typedef Negate<Sum1>::Result Sum;

//typedef NL::Print<Sum::Num>::Result deb;
typedef Translate<BI2,10>::Result SumT;
typedef NL::Print<SumT>::Result deb1;

using namespace std;

int main(int argc, char *argv[])
{

/*  cout<<"Add: "<<AddAB::Value<<endl;
  cout<<"Mult: "<<MultAB::Value<<endl;*/
  cout<<"1: "<<Sum::isPositive<<endl;
  cout<<"1: "<<Sum::Num::value<<endl;
  cout<<"2: "<<Sum::Num::Tail::value<<endl;
  cout<<"2: "<<NL::Length<Sum::Num>::value<<endl;
  cout<<A*Base+A<<"-"<<B*Base+B<<"="<<Sum::Num::Tail::value*Base+Sum::Num::value<<endl;
  cout<<"Dif: "<<(A*Base+A-B*Base-B)-(Sum::Num::Tail::value*Base+Sum::Num::value)<<endl;
//  cout<<"3: "<<Sum::Num::Tail::Tail::value<<endl;

  return 0;
}