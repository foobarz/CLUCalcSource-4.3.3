/// LICENSE START
////////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the CLU Library.
//
// 
// Copyright (C) 1997-2004  Christian B.U. Perwass
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// The GNU Library General Public License can be found in the file
// license.txt distributed with this library.
//
// Please send all queries and comments to
//
// email: help@clucalc.info
// mail: Institut fuer Informatik, Olshausenstr. 40, 24098 Kiel, Germany.
//
////////////////////////////////////////////////////////////////////////////////////
/// LICENSE END

// Class Library Definitions for Random number generators

#include <math.h>
#include <string.h>

#include "rand.h"


// Constructor
Rand::Rand(lint nseed)
{
  mbig = 1000000000;
  mseed = 161803398;
  mz = 0;
  fac = 1e-9;

  gasdevIset = 0;
  gasdev = 1.0;
  gasmean = 0.0;

  seed3(nseed);
}


Rand& Rand::operator= (const Rand& R)
{
  mbig = R.mbig;
  mseed = R.mseed;
  mz = R.mz;
  fac = R.fac;
  ran3Inext = R.ran3Inext;
  ran3Inextp = R.ran3Inextp;

  memcpy(ran3Ma, R.ran3Ma, 55*sizeof(lint));  


  gasdevIset = R.gasdevIset;
  gasdevGset = R.gasdevGset;
  gasdev = R.gasdev;
  gasmean = R.gasmean;

  return this[0];
}
// Seeding for Ran3 procedure
void Rand::seed3(lint seed)
{
  int i, ii, k;
  lint mj, mk;

  mj = mseed - seed;
  mj %= mbig;

  ran3Ma[54] = mj;
  mk = 1;

  for(i=0;i<54;i++)
  { ii = ((21*(i+1)) % 55) - 1;
    ran3Ma[ii] = mk;
    mk = mj - mk;
    if (mk < mz) mk += mbig;
    mj = ran3Ma[ii];
  }

  for(k=0;k<4;k++)
    for(i=0;i<55;i++)
    { ran3Ma[i] -= ran3Ma[1 + ((i+30) % 55)];
      if (ran3Ma[i] < mz) ran3Ma[i] += mbig;
    }

  ran3Inext = -1;
  ran3Inextp = 30;
}


// Ran3 procedure from Num.Rec. pp221-222
double Rand::ran3()
{
  lint mj;

  if (++ran3Inext == 55) ran3Inext = 0;
  if (++ran3Inextp == 55) ran3Inextp = 0;

  mj = ran3Ma[ran3Inext] - ran3Ma[ran3Inextp];

  if (mj < mz) mj += mbig;
  ran3Ma[ran3Inext] = mj;

  return (double(mj) * fac);
}



// Gaussian deviate
double Rand::Gauss()
{
  double f, r, v1, v2;

  if (!gasdevIset)
  { do
    { v1 = 2.0 * ran3() - 1.0;
      v2 = 2.0 * ran3() - 1.0;
      r = v1*v1 + v2*v2;
    } while (r >= 1.0 || r == 0.0);

    f = sqrt(-2.0 * log(r) / r) * gasdev;
    gasdevGset = v1 * f  + gasmean;
    gasdevIset = 1;

    return (v2*f + gasmean);
  }

  gasdevIset = 0;

  return gasdevGset;
}
  


