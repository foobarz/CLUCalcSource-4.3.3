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


#ifndef _BASE_H_
#define _BASE_H_


#include "cstr.h"

typedef unsigned uint;
typedef unsigned long ulong;


// Base Class -------------------------------------------------------

template<class CType>
class Base
{
public:
	
	Base(uint nvsdim, uint nfidx=1, const char* nvname = "e", unsigned nid=0);
	virtual ~Base();
	
	uint BaseID() const { return id; }
	uint VSDim() const { return vsdim; }
	uint GADim() const { return gadim; }
	char* VName() const { return vname; }
	uint FirstIdx() const { return firstIdx; }
	
	virtual CType signature(uint i) = 0;// Signature, to be defined by derived class
	
protected:
	uint id;				// Id of Basis
	uint vsdim;			// Dimensionality of Vector Space
	uint gadim;			// Dimensionality of corresponding algebra
	cStr vname;			// Name of basis vectors
	uint firstIdx;	// Index of first Basis Vector
	
};


#endif // _BASE_H_

