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


// Make String helping functions

#ifndef _MAKESTR_HH_
#define _MAKESTR_HH_

#include<string.h>
#include<stdio.h>

#include "cstr.h"

// Functions giving type-string of parameter
char* StrType(int a);
char* StrType(unsigned int a);
char* StrType(long int a);
char* StrType(unsigned long int a);
char* StrType(float a);
char* StrType(double a);
char* StrType(long double a);


cStr MakeStr(const int &a);
cStr MakeStr(const unsigned int &a);
cStr MakeStr(const long int &a);
cStr MakeStr(const unsigned long int &a);
cStr MakeStr(const float &a);
cStr MakeStr(const double &a);


#endif
