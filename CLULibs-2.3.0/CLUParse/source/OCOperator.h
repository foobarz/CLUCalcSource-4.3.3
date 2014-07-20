/// LICENSE START
////////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the CLUParse Library.
//
// 
// Copyright (C) 2001-2004  Christian B.U. Perwass
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

// OCOperator.h: Schnittstelle für die Klasse COCOperator.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCOPERATOR_H__A6F486BC_6252_4A90_821A_1D4014F7F303__INCLUDED_)
#define AFX_OCOPERATOR_H__A6F486BC_6252_4A90_821A_1D4014F7F303__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ObjectClass.h"
#include "ParseBase.h"
#include "CodeElement.h"
#include "CodeElementList.h"



class COCOperator : public CObjectClass
{
public:
	COCOperator();
	virtual ~COCOperator();

	void SetVoidOp(bool bVal) { m_bVoidOp = bVal; }
	void SetOrderRightToLeft(bool bRightToLeft) { m_bRightToLeft = bRightToLeft; }

	void SetOpLevel(int iLevelBinary, int iLevelUnary = -1) 
	{ m_iOpLevelBinary = iLevelBinary; m_iOpLevelUnary = iLevelUnary; }

	void SetOpSymbol(const char *pcSym) 
	{ if (pcSym) m_csIDSymbolSpace = pcSym; }

	virtual bool Parse(CParseBase& rParse, CCodeElementList& rCode);

protected:
	// Pointer to Class derived from CCodeElement that
	// implements functionality of Operator.
	// Has to be set by derived class.
	CCodeElement *m_pCodeOp;

	// Level of Binary Operator. 0 binds strongest.
	int m_iOpLevelBinary;

	// Level of Unary Operator. 0 binds strongest.
	// An operator is assumed to be unary, if it is 
	// the first element in a CodeElement list.
	// if this variable is -1 then operator cannot
	// be a unary operator.
	int m_iOpLevelUnary;

	// if this operator is of same strength as current
	// operator level then this operation is performed
	// first if m_bRightToLeft == true. 
	// Otherwise previous operation is performed first.
	bool m_bRightToLeft;

	// if this operator does not return a value
	// then set m_bVoidOp = true
	bool m_bVoidOp;

	// Current Operator level. -1 indicates no previous operator.
	// Current operator level is saved in rParse.
	// static int m_iCurOpLevel;
};

#endif // !defined(AFX_OCOPERATOR_H__A6F486BC_6252_4A90_821A_1D4014F7F303__INCLUDED_)
