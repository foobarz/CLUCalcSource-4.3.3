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


//--------------- Multi-Vector-Style Library ---------------------

#include "mvs.h"

#include "makestr.h"
#include "mathelp.h"


////////////////////////////////////////////////////////////////////////////////////
// Constructor

template<class CType>
MultiVStyle<CType>::MultiVStyle(BladeList<CType> &nstyle)
{
	
	base = nstyle.BasePtr();
	dim = base->VSDim();
	gadim = base->GADim();
	tblsize = gadim * gadim;
	
	if (nstyle.Count() == gadim)
	{ 
		style = new BladeList<CType>(nstyle);
		gpTable = new int[tblsize];
		ipTable = new int[tblsize];
		opTable = new int[tblsize];
		revTable = new int[gadim];
		dualTable = new int[gadim];
		invoTable = new int[gadim];
		viTable = new int[gadim];
		grades = new int[gadim];
		
		makeGPTable();
		makeIPTable();
		makeOPTable();
		makeDualTable();
		makeInvoTable();
		makeVecInvTable();
		getGrades();
	}
	else
	{ 
		style = 0;
		gpTable = 0;
		ipTable = 0;
		opTable = 0;
		dualTable = 0;
		invoTable = 0;
		viTable = 0;
		grades = 0;
	}
}


////////////////////////////////////////////////////////////////////////////////////
// Default Constructor

template<class CType>
MultiVStyle<CType>::MultiVStyle()
{
	
	base = 0;
	dim = 0;
	gadim = 0;
	tblsize = 0;
	
	style = 0;
	gpTable = 0;
	ipTable = 0;
	opTable = 0;
	dualTable = 0;
	invoTable = 0;
	viTable = 0;
	grades = 0;
}


////////////////////////////////////////////////////////////////////////////////////
// Destructor

template<class CType>
MultiVStyle<CType>::~MultiVStyle()
{
	if (style) delete style;
	if (gpTable) delete[] gpTable;
	if (ipTable) delete[] ipTable;
	if (opTable) delete[] opTable;
	if (revTable) delete[] revTable;
	if (dualTable) delete[] dualTable;
	if (invoTable) delete[] invoTable;
	if (viTable) delete[] viTable;
	if (grades) delete[] grades;
}



////////////////////////////////////////////////////////////////////////////////////
// Assignment Operator 
template<class CType>
MultiVStyle<CType>& MultiVStyle<CType>::Init(BladeList<CType>& nstyle, int *_grades, 
		int *_gpTable, int *_ipTable, int *_opTable, 
		int *_viTable, int *_dualTable, int *_invoTable)
{
	base = nstyle.BasePtr();
	dim = base->VSDim();
	gadim = base->GADim();
	tblsize = gadim * gadim;
	
	if (nstyle.Count() == gadim)
	{ 
		if (style) delete style;
		if (gpTable) delete[] gpTable;
		if (ipTable) delete[] ipTable;
		if (opTable) delete[] opTable;
		if (revTable) delete[] revTable;
		if (dualTable) delete[] dualTable;
		if (invoTable) delete[] invoTable;
		if (viTable) delete[] viTable;
		if (grades) delete[] grades;
	
		style = new BladeList<CType>(nstyle);
		gpTable = new int[tblsize];
		ipTable = new int[tblsize];
		opTable = new int[tblsize];
		revTable = new int[gadim];
		dualTable = new int[gadim];
		invoTable = new int[gadim];
		viTable = new int[gadim];
		grades = new int[gadim];
    
		if (_gpTable) memcpy(gpTable, _gpTable, tblsize*sizeof(int));
		else makeGPTable();

		if (_ipTable) memcpy(ipTable, _ipTable, tblsize*sizeof(int));
		else makeIPTable();

		if (_opTable) memcpy(opTable, _opTable, tblsize*sizeof(int));
		else makeOPTable();

		if (_dualTable) memcpy(dualTable, _dualTable, gadim*sizeof(int));
		else makeDualTable();

		if (_invoTable) memcpy(invoTable, _invoTable, gadim*sizeof(int));
		else makeInvoTable();

	    if (_viTable) memcpy(viTable, _viTable, gadim*sizeof(int));
		else makeVecInvTable();

	    if (_grades) memcpy(grades, _grades, gadim*sizeof(int));
		else getGrades();

		makeReverseTable();
	}
	else
	{ 
		if (style) delete style;
		if (gpTable) delete[] gpTable;
		if (ipTable) delete[] ipTable;
		if (opTable) delete[] opTable;
		if (revTable) delete[] revTable;
		if (dualTable) delete[] dualTable;
		if (invoTable) delete[] invoTable;
		if (viTable) delete[] viTable;
		if (grades) delete[] grades;
	
		style = 0;
		gpTable = 0;
		ipTable = 0;
		opTable = 0;
		revTable = 0;
		dualTable = 0;
		invoTable = 0;
		viTable = 0;
		grades = 0;
	}
	
	return this[0];
}


////////////////////////////////////////////////////////////////////////////////////
// Make Reverse Table

template<class CType>
void MultiVStyle<CType>::makeReverseTable()
{
	if (!revTable) return;

	int iGrade;
	for(uint i = 0; i < gadim; i++)
	{
		iGrade = grades[i];
		if ( (( (iGrade * (iGrade-1)) >> 1) % 2) )
			revTable[i] = -int(i + 1);
		else
			revTable[i] = int(i + 1);
	}
}




////////////////////////////////////////////////////////////////////////////////////
// Make Involution Table

template<class CType>
void MultiVStyle<CType>::makeInvoTable()
{
	if (!invoTable) return;
	
	
	Blade<CType> a(base[0]);
	uint i;
	int r;
	
	//  printf("Make Dual Table ------------------------");
	//  printf("\nb: %s\n\n", b.Str());
	
	for(i=0;i<gadim;i++)
	{ 
		a = style[0][i] & (~style[0][i]);
		r = (a.Factor() < 0);
		invoTable[i] = r;
    //        printf("\na: %s, s: %s, b: %s, r: %d, dt: %d\n",a.Str(), 
    //               style[0][i].Str(), b.Str(), r, dualTable[i]);
	   
	}
	
}




////////////////////////////////////////////////////////////////////////////////////
// Make Dual Table

template<class CType>
void MultiVStyle<CType>::makeDualTable()
{
	if (!dualTable) return;
	
	
	Blade<CType> a(base[0]);
	Blade<CType> b(base[0]);
	uint i;
	int r;
	
	b.makePS();
	b = !b; // Inverse of Pseudo Scalar
	
	//  printf("Make Dual Table ------------------------");
	//  printf("\nb: %s\n\n", b.Str());
	
	for(i=0;i<gadim;i++)
	{ 
		a = style[0][i] & b;
		r = (style[0] == a);
		dualTable[i] = r * int(style[0][((r<0)?(-r):(r))-1].Factor()) * int(a.Factor());
    //        printf("\na: %s, s: %s, b: %s, r: %d, dt: %d\n",a.Str(), 
    //               style[0][i].Str(), b.Str(), r, dualTable[i]);
	   
	}
	
}



////////////////////////////////////////////////////////////////////////////////////
// Make GP Table

template<class CType>
void MultiVStyle<CType>::makeGPTable()
{
	if (!gpTable) return;
	
	Blade<CType> a(base[0]);
	uint i,j,k;
	int r;
	
	for(i=0,k=0;i<gadim;i++)
	{
		for(j=0;j<gadim;j++,k++)
		{ 
			a = style[0][i] & style[0][j];
			r = (style[0] == a);
			gpTable[k] = r * int(style[0][((r<0)?(-r):(r))-1].Factor()) 
					       * int(a.Factor());
		}
	}	
}



////////////////////////////////////////////////////////////////////////////////////
// Make IP Table

template<class CType>
void MultiVStyle<CType>::makeIPTable()
{
	if (!ipTable) return;
	
	Blade<CType> a(base[0]);
	uint i,j,k;
	int r;
	
	for(i=0,k=0;i<gadim;i++)
	{
		for(j=0;j<gadim;j++,k++)
		{ 
			a = style[0][i] * style[0][j];
			if (a.Factor() == CType(0)) ipTable[k] = 0;
			else
			{ 
				r = (style[0] == a);
				ipTable[k] = r * int(style[0][((r<0)?(-r):(r))-1].Factor()) 
								* int(a.Factor());
			}
		}
	}
}



////////////////////////////////////////////////////////////////////////////////////
// Make OP Table

template<class CType>
void MultiVStyle<CType>::makeOPTable()
{
	if (!opTable) return;
	
	Blade<CType> a(base[0]);
	uint i,j,k;
	int r;
	
	for(i=0,k=0;i<gadim;i++)
	{
		for(j=0;j<gadim;j++,k++)
		{ 
			a = style[0][i] ^ style[0][j];
			if (a.Factor() == CType(0)) opTable[k] = 0;
			else
			{ 
				r = (style[0] == a);
				opTable[k] = r * int(style[0][((r<0)?(-r):(r))-1].Factor()) 
								* int(a.Factor());
			}
		}
	}
}



////////////////////////////////////////////////////////////////////////////////////
// Make Vector Inverse Table

template<class CType>
void MultiVStyle<CType>::makeVecInvTable()
{
	if (!viTable) return;
	
	Blade<CType> a(base[0]);
	
	for(uint i=0;i<gadim;i++)
	{ 
		a = (style[0][i] * (~style[0][i]));
		viTable[i] = (int) a.Factor();
	}
}



////////////////////////////////////////////////////////////////////////////////////
// Make Grade List

template<class CType>
void MultiVStyle<CType>::getGrades()
{
	if (!grades) return;
	
	for(uint i=0;i<gadim;i++)
	{ 
		grades[i] = style[0][i].Grade();
	}
}



////////////////////////////////////////////////////////////////////////////////////
// Name of Blade at pos

template<class CType>
char* MultiVStyle<CType>::BladeName(uint pos)
{
	if (pos < gadim)
		return style[0][pos].Str();
	else
		return str = "(ERROR)";
}


////////////////////////////////////////////////////////////////////////////////////
// Finding Blade in MVStyle

template<class CType>
int operator==(const MultiVStyle<CType> &amvs, const Blade<CType> &b)
{
	return (amvs.style[0] == b);
}

