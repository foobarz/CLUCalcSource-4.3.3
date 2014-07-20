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



// Mathematical helping functions library
#include<math.h>

#include"mathelp.h"


// Magnitude functions
int Mag(int a) { return ( (a < 0) ? (-a) : (a) ); }
uint Mag(uint a) { return a; }
float Mag(float a) { return ( (a < float(0)) ? (-a) : (a) ); }
double Mag(double a) { return ( (a < double(0)) ? (-a) : (a) ); }
long int Mag(long int a) { return ( (a < 0) ? (-a) : (a) ); }

// Min Max Functions
int Min(int a, int b) { return (a < b ? (a) : (b)); }
uint Min(uint a, uint b) { return (a < b ? (a) : (b)); }
float Min(float a, float b) { return (a < b ? (a) : (b)); }
double Min(double a, double b) { return (a < b ? (a) : (b)); }

int Max(int a, int b) { return (a > b ? (a) : (b)); }
uint Max(uint a, uint b) { return (a > b ? (a) : (b)); }
float Max(float a, float b) { return (a > b ? (a) : (b)); }
double Max(double a, double b) { return (a > b ? (a) : (b)); }


// -------------------- Round float ----------------------
float Round(float x)
{
  float ip, fp, add = 1.0;
	double dip;

  fp = Mag(float(modf(x, &dip)));
  ip = float(dip);

  if (ip < 0.0) add = -1.0;

  if (fp >= 0.5) ip += add;
    
  return ip;
}


// -------------------- Round double ----------------------
double Round(double x)
{
  double ip, fp, add = 1.0;

  fp = Mag(modf(x, &ip));

  if (ip < 0.0) add = -1.0;

  if (fp >= 0.5) ip += add;
    
  return ip;
}


// ------------------- Factorial -------------------------
uint Fact(uint n)
{
  if (!n) return 1;
  if (n < 3) return n;

  uint r = n;
  for(uint i=2;i<n;i++)
    r *= i;

  return r;
}

// ------------------------- Calculate Combinations ------------------
uint Comb(uint n, uint k)
{
  return (Fact(n) / (k * Fact(n - k)));
}

// ----------------------- Calculate Variations -----------------------
uint Vari(uint n, uint k)
{
  return (Fact(n) /  Fact(n - k));
}



//*************************************************************************
// Find Even or Odd Permutation *******************************************
//*************************************************************************

int Perm3(uint i, uint &j, uint &k, uint even)
{
	if (even)
	{ 
		switch(i)
		{ 
		case 0: j = 1; k = 2; break;
		case 1: j = 2; k = 0; break;
		case 2: j = 0; k = 1; break;
		}
		return 1;
	}
	else
	{ 
		switch(i)
		{ 
		case 0: k = 1; j = 2; break;
		case 1: k = 2; j = 0; break;
		case 2: k = 0; j = 1; break;
		}
		return -1;
	}
}


////////////////////////////////////////////////////////////////////////////////////////
/// Tiny functions return limit of floating point precision

int Tiny(int& tiny) { return (tiny = 0); }
float Tiny(float& tiny) { return (tiny = float(1e-7)); }
double Tiny(double& tiny) { return (tiny = double(1e-12)); }

////////////////////////////////////////////////////////////////////////////////////////
/// Round with precision

int Round(int fVal, int fPrec)
{
	return fVal;
}

float Round(float fVal, float fPrec)
{ 
	if (fPrec == 0)
		Tiny(fPrec);

	if (fVal < 0.0)
		return float(ceil(double(fVal / fPrec) - 0.500001) * double(fPrec)); 
	else
		return float(floor(double(fVal / fPrec) + 0.500001) * double(fPrec)); 

/*
	if (fVal < 0.0f)
		return (fVal > -fPrec ? 0 : fVal);
	else
		return (fVal < fPrec ? 0 : fVal);
*/
}

double Round(double fVal, double fPrec)
{ 
/*
	if (fVal < 0.0)
		return (fVal > -fPrec ? 0 : fVal);
	else
		return (fVal < fPrec ? 0 : fVal);
*/
	if (fPrec == 0)
		Tiny(fPrec);

	if (fVal < 0.0)
		return ceil(fVal / fPrec - 0.500001) * fPrec; 
	else
		return floor(fVal / fPrec + 0.500001) * fPrec; 

}
