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


// ExMinFunc.h: Schnittstelle für die Klasse ExMinFunc.
//
// Example Minimisation function.
//
// Use this example as template for your own functions that
// are to be minimised.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXMINFUNC_H__36567181_50D4_11D3_9065_00E098180C06__INCLUDED_)
#define AFX_EXMINFUNC_H__36567181_50D4_11D3_9065_00E098180C06__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MinFuncBase.h"


class ExMinFunc : public MinFuncBase  
{
public:
	ExMinFunc(double a1 = 1.0, double a2 = 1.0, double a3 = 1.0);
	virtual ~ExMinFunc();

	// returns true if no error occured, otherwise false.
	// deriv: derivative order, i.e. 0 is function, 1 is 1st deriv. etc
	// p: pointer to function parameters
	// v: return value or values
	bool Func(uint deriv, double *p, double *v);

protected:
	bool MinFunc(double *p, double *v);
	bool MinFuncD1(double *p, double *v);

protected:
	double a[2];
};

#endif // !defined(AFX_EXMINFUNC_H__36567181_50D4_11D3_9065_00E098180C06__INCLUDED_)
