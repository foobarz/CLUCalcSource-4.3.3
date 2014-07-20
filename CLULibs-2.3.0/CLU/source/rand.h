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

// Class Declaration for random number routines

#ifndef _RAND_HH_
#define _RAND_HH_

typedef long int lint;
typedef unsigned uint;

class Rand
{
  
public:
  Rand(lint nseed = 0);

  Rand& operator= (const Rand& R);

  void seed3(lint nseed);
  double ran3();
  inline double pmran3(double x) { return ((ran3()-0.5)*2.0*x); }
  inline double range3(double min, double max)
  { return (min + (max-min)*ran3()); }

  double gausdev() { return Gauss(); }
  void GausDev(double ngasdev) { gasdev = ngasdev; }
  void GausMean(double ngasmean) { gasmean = ngasmean; }

  double Gauss();
  void GaussDev(double ngasdev) { gasdev = ngasdev; }
  void GaussMean(double ngasmean) { gasmean = ngasmean; }

protected:

  // for ran3
  int ran3Inext;
  int ran3Inextp;
  lint ran3Ma[55];  // Do NOT change size

  lint mbig, mseed, mz;
  double fac;

  // for gausdev
  uint gasdevIset;
  double gasdevGset;
  double gasdev, gasmean;
};





#endif



