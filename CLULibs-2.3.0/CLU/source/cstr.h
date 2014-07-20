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


// String Library Declarations

/*
Operators + and << can be used concurrently. 
The former for sums including at least one element at the first or second
position of type cStr.
The latter for streams that do not necessarily include vars of type cStr.
*/

#ifndef _CSTR_HH_
#define _CSTR_HH_

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "mem.h"

class cStr;

#include "makestr.h"

#ifndef _MAXSTRSIZE_
#define _MAXSTRSIZE_ 1024
#endif

#define _CSTR_FORMAT_CHARS_ "iouoxXfeEgGcsp"

typedef unsigned uint;
typedef unsigned long ulong;

class cStr
{
public:
	cStr(const char *nstr = 0);
	cStr(const cStr& nstr);
	~cStr();
	
	//  char& operator[] (uint pos) { return str[pos]; }
	
    cStr& ToUpperCase();
	cStr& ToLowerCase();

	// Sub String extraction
	cStr operator() (uint fpos, uint lpos, uint cut = 0);
	cStr Last(uint no = 1, uint cut = 0);
	cStr First(uint no = 1, uint cut = 0);
	
	cStr& operator= (const cStr& nstr);
	cStr& operator= (const char *nstr);
	cStr& operator= (const char a);
	cStr& operator= (const int a);
	cStr& operator= (const unsigned int a);
	cStr& operator= (const long a);
	cStr& operator= (const double a);
	cStr& operator= (const long double a);
	cStr& operator= (const void* a);
	
	friend int operator== (const cStr& a, const cStr& b);
	friend int operator!= (const cStr& a, const cStr& b);
	
	friend int operator== (const cStr& a, const char* b);
	friend int operator!= (const cStr& a, const char* b);

	friend int operator== (const char* a, const cStr& b);
	friend int operator!= (const char* a, const cStr& b);

	// Returns position of a in b +1.
	// Returns 0 if a is not in b.
	friend int operator> (const char a, const cStr& b); // first occurence
	friend int operator< (const char a, const cStr& b); // last occurence
	
	friend cStr operator! (const cStr& a); // Capitalize
	friend cStr operator- (const cStr& a); // Change to lower case
	
	friend cStr operator~ (const cStr& a); // Reverse String
	
	friend cStr operator+ (const cStr& a, const cStr& b);
	friend cStr& operator+= (cStr& a, const cStr& b);
	
	friend cStr operator+ (const cStr& a, const char *b);
	friend cStr operator+ (const char *b, const cStr& a);
	friend cStr& operator+= (cStr& a, const char *b);
	
	friend cStr operator+ (const cStr& a, const char b);
	friend cStr operator+ (const char b, const cStr& a);
	friend cStr& operator+= (cStr& a, const char b);
	
	friend cStr operator+ (const cStr& a, const uint b);
	friend cStr operator+ (const uint b, const cStr& a);
	friend cStr& operator+= (cStr& a, const uint b);
	
	friend cStr operator+ (const cStr& a, const ulong b);
	friend cStr operator+ (const ulong b, const cStr& a);
	friend cStr& operator+= (cStr& a, const ulong b);
	
	friend cStr operator+ (const cStr& a, const int b);
	friend cStr operator+ (const int b, const cStr& a);
	friend cStr& operator+= (cStr& a, const int b);
	
	friend cStr operator+ (const cStr& a, const long b);
	friend cStr operator+ (const long b, const cStr& a);
	friend cStr& operator+= (cStr& a, const long b);
	
	friend cStr operator+ (const cStr& a, const double b);
	friend cStr operator+ (const double b, const cStr& a);
	friend cStr& operator+= (cStr& a, const double b);
	
	friend cStr operator+ (const cStr& a, const long double b);
	friend cStr operator+ (const long double b, const cStr& a);
	friend cStr& operator+= (cStr& a, const long double b);
	
	friend cStr& operator<< (cStr& a, const cStr& b);
	friend cStr& operator<< (cStr& a, const char *b);
	friend cStr& operator<< (cStr& a, const char b);
	friend cStr& operator<< (cStr& a, const uint b);
	friend cStr& operator<< (cStr& a, const ulong b);
	friend cStr& operator<< (cStr& a, const int b);
	friend cStr& operator<< (cStr& a, const long b);
	friend cStr& operator<< (cStr& a, const double b);
	friend cStr& operator<< (cStr& a, const long double b);
	
	friend cStr operator% (const int a, const cStr& b);
	friend cStr operator% (const long a, const cStr& b);
	friend cStr operator% (const double a, const cStr& b);
	friend cStr operator% (const long double a, const cStr& b);
	
	friend cStr operator% (const unsigned a, const cStr& b);
	friend cStr operator% (const unsigned long a, const cStr& b);
	
	friend cStr operator* (const cStr& a, int n);
	friend cStr operator* (int n, const cStr& a);
	friend cStr& operator*= (cStr& a, int n);
	
	// Add or sub n characters to string. Char given by cfill
	friend cStr operator| (const cStr& a, int n);
	friend cStr operator| (int n, const cStr& a);
	friend cStr& operator|= (cStr& a, int n);
	
	friend int operator>> (const cStr& a, uint& b);
	friend int operator>> (const cStr& a, ulong& b);
	friend int operator>> (const cStr& a, int& b);
	friend int operator>> (const cStr& a, long& b);
	friend int operator>> (const cStr& a, float& b);
	friend int operator>> (const cStr& a, double& b);
	friend int operator>> (const cStr& a, long double& b);
	friend int operator>> (const cStr& a, void*& b);
	
	
	
	uint Len() const { return (str.Count() - 1); }
	char *Str() const { return str.Data(); }
	operator char*() const { return str.Data(); }
	
	char FillChar() const { return cfill; }
	char FillChar(char ncfill) { cfill = ncfill; return cfill; }
	
	// Returns 1 if c is a printf format character
	// i.e. it is in _CSTR_FORMAT_CHARS_.
	// Otherwise zero is returned.
	friend int isFormat(char c);
	
protected:
	Mem<char> str;
	char cfill;
	
	cStr& MakeFormatStr(char *ts);
};



#endif
