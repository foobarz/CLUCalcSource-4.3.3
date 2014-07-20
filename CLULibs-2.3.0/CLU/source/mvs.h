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


//  Clifford Geometric Algebra Class Definitions


#ifndef _MULTIVSTYLE_H_
#define _MULTIVSTYLE_H_


#include "base.h"
#include "blade.h"
#include "bladelist.h"

#include"mem.h"

//#define BLADE (uint[])    // Used for casting array initialization

#define MVS_LIST 0x0001    // Multi-Vector Display Style as [1,..., ]
#define MVS_SUM  0x0002    // Multi-Vector Display Style as [ 1*g0 + ... + ]

#ifndef _MAXSTRSIZE_
#define _MAXSTRSIZE_ 1024   // Maximum String Size for String output
#endif

#ifdef _GNUCPP3_
	#ifndef _TMPL_
	#define _TMPL_ <>
	#endif
#	ifndef _FRIEND_
#		define _FRIEND_  friend 
#	endif
#else
	#ifndef _TMPL_
	#define _TMPL_
	#endif
#	ifndef _FRIEND_
#		define _FRIEND_  friend 
#	endif
#endif



typedef unsigned uint;
typedef unsigned long ulong;

template<class CType> class MultiV;
template<class CType> class MultiVStyle;

template<class CType> int operator==(const MultiVStyle<CType> &, const Blade<CType> &);

// Multi-Vector-Style Class Declaration ---------------------

template<class CType>
class MultiVStyle
{
public:
	MultiVStyle(BladeList<CType>& nstyle);
	MultiVStyle();
	virtual ~MultiVStyle();
	
	MultiVStyle<CType>& Init(BladeList<CType>& BList, 
		int *_grades = 0, int *_gpTable = 0, int *_ipTable = 0, int *_opTable = 0, 
		int *_viTable = 0, int *_dualTable = 0, int *_invoTable = 0);
	
	_FRIEND_ int operator== _TMPL_(const MultiVStyle<CType> &, const Blade<CType> &);
	
	uint GADim() const { return gadim; }
	int* GPTable() const { return gpTable; }
	int* IPTable() const { return ipTable; }
	int* OPTable() const { return opTable; }
	int* ReverseTable() const { return revTable; }
	int* DualTable() const { return dualTable; }
	int* InvoTable() const { return invoTable; }

	int* VecInvTable() const { return viTable; }
	int* Grades() const { return grades; }
	char* BladeName(uint pos);
	Base<CType>* BasePtr() const { return base; }
	BladeList<CType>* Style() const { return style; }
	
protected:
	Base<CType> *base;
	uint dim;
	uint gadim;
	uint tblsize;
	BladeList<CType> *style;
	cStr str;
	
	int *grades;
	int *viTable;  // Vector Inverse Table
	int *gpTable, *ipTable, *opTable, *dualTable, *invoTable, *revTable;
	
	void makeGPTable();
	void makeIPTable();
	void makeOPTable();
	void makeReverseTable();
	void makeDualTable();
	void makeInvoTable();
	void makeVecInvTable();
	void getGrades();
};



#endif // _MULTIVSTYLE_H_


