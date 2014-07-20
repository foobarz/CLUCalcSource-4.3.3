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

// MinFuncBase.h: Schnittstelle für die Klasse MinFuncBase.
//
// This class defines the basis for functions which are to be
// minimised by one of the minimisers in this project.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MINFUNCBASE_H__36567180_50D4_11D3_9065_00E098180C06__INCLUDED_)
#define AFX_MINFUNCBASE_H__36567180_50D4_11D3_9065_00E098180C06__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef unsigned int uint;


class MinFuncBase  
{
public:
//	MinFuncBase();
	//virtual ~MinFuncBase();

	// returns true if no error occured, otherwise false.
	// deriv: derivative order, i.e. 0 is function, 1 is 1st deriv. etc
	// p: pointer to function parameters
	// v: return value or values
	virtual bool Func(uint deriv, double *p, double *v) = 0;

	int GetMaxDerivative() { return m_iMaxDeriv; }
	int GetDimension() { return m_iDim; }

protected:
	int m_iDim; // Dimensionality of function
	int m_iMaxDeriv; // maximal derivative available
};

#endif // !defined(AFX_MINFUNCBASE_H__36567180_50D4_11D3_9065_00E098180C06__INCLUDED_)
