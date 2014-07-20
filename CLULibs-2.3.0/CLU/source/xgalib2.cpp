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


// Creates Necessary Instances of template class library CGA

#include"cga.cxx"

// These are included from XUT Library
// Set include path appropriately
#include "dynlist.cxx"
#include "mem.cxx"
#include "memobj.cxx"

//#include "matrix.cpp"

////////////////////////////////////////////////////////////////////////////////////
// float instantiation
//template class MultiV<float>;

template class Mem<Blade<float>* >;
template class Mem<MultiV<float>* >;

template class DynList<Blade<float> >;
template class MemObj<MultiV<float> >;

Instantiate_CGA(float);


////////////////////////////////////////////////////////////////////////////////////
// double instantiation
template class Mem<Blade<double>* >;
template class Mem<MultiV<double>* >;

template class DynList<Blade<double> >;
template class MemObj<MultiV<double> >;


Instantiate_CGA(double);


////////////////////////////////////////////////////////////////////////////////////

// The first two number give major and minor version.
// The last three numbers give the date in the format day,month,year.
unsigned _xgalib_version[5] = {2,0,5,12,2000};

char *_xgalib_license_text = 
"This program uses the Extended Geometric Algebra (XGA) Library\n"
"Version 2.0, December 2000\n\n"
//
"The library implements Geometric Algebra in arbitrary dimension\n"
"and with an arbitrary basis signature.\n\n"
//
"Copyright (C) 1997-2001  Christian B.U. Perwass\n\n"
//
"This library is free software; you can redistribute it and/or\n"
"modify it under the terms of the GNU Library General Public\n"
"License as published by the Free Software Foundation; either\n"
"version 2 of the License, or (at your option) any later version.\n\n"
//
"This library is distributed in the hope that it will be useful,\n"
"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\n"
"Library General Public License for more details.\n\n"
//
"You should have received a copy of the GNU Library General Public\n"
"License along with this library; if not, write to the\n"
"Free Software Foundation, Inc., 59 Temple Place - Suite 330,\n"
"Boston, MA  02111-1307, USA.\n\n"
//
"The GNU Library General Public License can be found in the file\n"
"license.txt distributed with this library.\n\n"
//
"Please send all queries and comments regarding the XGA Library to\n\n"
//
"email: public@perwass.com\n"
"mail: Institut fuer Informatik, Preusserstr. 1-9, 24105 Kiel, Germany.\n\n";

