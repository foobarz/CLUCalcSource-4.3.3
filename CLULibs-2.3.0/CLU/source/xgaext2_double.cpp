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

 
// Instantiation of needed Base Extention templates
#include "xgalib2.h"


#include "cga.cxx"
#include"pga.cxx"
#include"sta.cxx"
#include"e3ga.cxx"
#include "GA1p2m.cxx"
#include "confga.cxx"
#include "Cl22.cxx"
#include "el2ga.cxx"

#include "mem.cxx"
#include "memobj.cxx"
#include "MVInfo.cxx"


//#include "matrix.cpp"

////////////////////////////////////////////////////////////////////////////////////
// double instantiation

//Instantiate_CGA(double);

Instantiate_MVInfo(double);

Instantiate_PGA(double);
Instantiate_STA(double); 
Instantiate_E3GA(double);
Instantiate_GA1p2m(double);
Instantiate_ConfGA(double);
Instantiate_Cl22(double);
Instantiate_El2GA(double);

