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

// CLUParse.cpp: Implementierung der Klasse CCLUParse.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CLUParse.h"
#include <algorithm>
#include <string>

#ifdef _GNUCPP3_
#   include <unistd.h>
#   define _getcwd getcwd
#   define _chdir chdir
#else
#   include <direct.h>
#endif

using namespace std;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCLUParse::CCLUParse()
{

}

CCLUParse::~CCLUParse()
{

}



//////////////////////////////////////////////////////////////////////
// Init

bool CCLUParse::Init()
{
	//printf("CLUParse:Init: Start\n");
	
	// ';' is line separation symbol
	SetLineSepSym(';');

	// Set Code Base
	SetCodeBase(&m_CodeBase);


	// Register Object Class Object Separator Symbol ' '
	if (!AddObjectClass(&m_ocObjSepSym))
		return false;

	// Initialize Object Class 'Label'
	cStr csSet;

	// ID Symbols
	csSet = "abcdefghijklmnopqrstuvwxyzäöüß";
	csSet << "ABCDEFGHIJKLMNOPQRSTUVWXYZÄÖÜ_";
	csSet << OC_IDSYM_GLOBAL_STR;
	m_ocLabel.m_csIDSymbolSpace = csSet;

	// Symbol Subspace
	csSet << "1234567890";
	m_ocLabel.SetSymbolSpace(csSet);

	// Register Object Class 'Label'
	if (!AddObjectClass(&m_ocLabel))
		return false;

	// Register Object Class 'Number'
	if (!AddObjectClass(&m_ocNumber))
		return false;

	// Register Object Class 'String'
	if (!AddObjectClass(&m_ocString))
		return false;

	// Register Object Class Operator '('
	if (!AddObjectClass(&m_ocOpOpenRoundBracket))
		return false;

	// Register Object Class Operator ')'
	if (!AddObjectClass(&m_ocOpCloseRoundBracket))
		return false;

	// Register Object Class Operator '['
	if (!AddObjectClass(&m_ocOpOpenSquareBracket))
		return false;

	// Register Object Class Operator ']'
	if (!AddObjectClass(&m_ocOpCloseSquareBracket))
		return false;

	// Register Object Class Operator ','
	if (!AddObjectClass(&m_ocOpComma))
		return false;

	// Register Object Class Operator '{'
	if (!AddObjectClass(&m_ocOpOpenCurlyBracket))
		return false;

	// Register Object Class Operator '}'
	if (!AddObjectClass(&m_ocOpCloseCurlyBracket))
		return false;

	// Register Object Class Keyword IF
	if (!AddObjectClass(&m_ocKeywordIF))
		return false;

	// Register Object Class Keyword THEN
	if (!AddObjectClass(&m_ocKeywordTHEN))
		return false;

	// Register Object Class Keyword ELSE
	if (!AddObjectClass(&m_ocKeywordELSE))
		return false;

	// Register Object Class Keyword LOOP
	if (!AddObjectClass(&m_ocKeywordLOOP))
		return false;

	// Register Object Class Keyword BREAK
	if (!AddObjectClass(&m_ocKeywordBREAK))
		return false;

	
	// Initialize and Register standard Operators.
	int iOpNo = 44;
	if (!m_mOps.Set(iOpNo))
		return false;

	m_mOps[0].Init(
		"=",										// Op. Symbol
		-1,											// Unary level
		OPLEVEL_ASSIGN,								// binary level
		true,										// Right to Left
		false,										// Void Operator
		false,										// recurse lists
		0,											// Unary Func. Ptr.
		0,											// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		OPFUNC(TOpABinaryPtr,OpAssign));	// Binary Assign Func Ptr.

	m_mOps[1].Init(
		"^",										// Op. Symbol
		-1,											// Unary level
		OPLEVEL_WEDGE,								// binary level
		false,										// Right to Left
		false,										// Void Operator
		true,										// recurse lists
		0,											// Unary Func. Ptr.
		OPFUNC(TOpBinaryPtr,OpWedge),		// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[2].Init(
		".",										// Op. Symbol
		-1,											// Unary level
		OPLEVEL_POINT,								// binary level
		false,										// Right to Left
		false,										// Void Operator
		true,										// recurse lists
		0,											// Unary Func. Ptr.
		OPFUNC(TOpBinaryPtr,OpInnerProd),	// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[3].Init(
		"*",										// Op. Symbol
		OPLEVEL_DUAL,								// Unary level
		OPLEVEL_TIMES,								// binary level
		false,										// Right to Left
		false,										// Void Operator
		true,										// recurse lists
		OPFUNC(TOpUnaryPtr,OpDual),			// Unary Func. Ptr.
		OPFUNC(TOpBinaryPtr,OpProd),		// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[4].Init(
		"/",										// Op. Symbol
		-1,											// Unary level
		OPLEVEL_DIV,								// binary level
		false,										// Right to Left
		false,										// Void Operator
		true,										// recurse lists
		0,											// Unary Func. Ptr.
		OPFUNC(TOpBinaryPtr,OpDiv),		// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[5].Init(
		"-",										// Op. Symbol
		OPLEVEL_NEGATE,								// Unary level
		OPLEVEL_MINUS,								// binary level
		false,										// Right to Left
		false,										// Void Operator
		true,										// recurse lists
		OPFUNC(TOpUnaryPtr,OpNegate),		// Unary Func. Ptr.
		OPFUNC(TOpBinaryPtr, OpSubtract),	// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[6].Init(
		"+",										// Op. Symbol
		-1,											// Unary level
		OPLEVEL_PLUS,								// binary level
		false,										// Right to Left
		false,										// Void Operator
		true,										// recurse lists
		0,											// Unary Func. Ptr.
		OPFUNC(TOpBinaryPtr, OpAdd),		// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[7].Init(
		"|",										// Op. Symbol
		-1,											// Unary level
		OPLEVEL_JOIN,								// binary level
		false,										// Right to Left
		false,										// Void Operator
		true,										// recurse lists
		0,											// Unary Func. Ptr.
		OPFUNC(TOpBinaryPtr, OpJoin),		// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[8].Init(
		"&",										// Op. Symbol
		-1,											// Unary level
		OPLEVEL_MEET,								// binary level
		false,										// Right to Left
		false,										// Void Operator
		true,										// recurse lists
		0,											// Unary Func. Ptr.
		OPFUNC(TOpBinaryPtr, OpMeet),		// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[9].Init(
		"%",										// Op. Symbol
		-1,											// Unary level
		OPLEVEL_MODULUS,							// binary level
		false,										// Right to Left
		false,										// Void Operator
		true,										// recurse lists
		0,											// Unary Func. Ptr.
		OPFUNC(TOpBinaryPtr, OpMod),			// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[10].Init(
		OC_IDSYM_CALL_STR,							// Op. Symbol
		-1,											// Unary level
		OPLEVEL_FUNCTION,							// binary level
		false,										// Right to Left
		false,										// Void Operator
		false,										// recurse lists
		0,											// Unary Func. Ptr.
		OPFUNC(TOpBinaryPtr,OpFuncCall),			// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

/*
	m_mOps[10].Init(
		"#",										// Op. Symbol
		-1,											// Unary level
		OPLEVEL_GRADE,								// binary level
		false,										// Right to Left
		false,										// Void Operator
		false,										// recurse lists
		0,											// Unary Func. Ptr.
		OPFUNC(TOpBinaryPtr,OpElementSelect),		// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.
*/

	m_mOps[11].Init(
		"~",										// Op. Symbol
		OPLEVEL_REVERSE,							// Unary level
		OPLEVEL_STEPLIST,							// binary level
		true,										// Right to Left
		false,										// Void Operator
		true,										// recurse lists
		OPFUNC(TOpUnaryPtr, OpReverse),				// Unary Func. Ptr.
		OPFUNC(TOpBinaryPtr, OpStepList),			// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[12].Init(
		"!",										// Op. Symbol
		OPLEVEL_INVERSE,							// Unary level
		-1,											// binary level
		true,										// Right to Left
		false,										// Void Operator
		true,										// recurse lists
		OPFUNC(TOpUnaryPtr,OpInverse),		// Unary Func. Ptr.
		0,											// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[13].Init(
		"?",										// Op. Symbol
		OPLEVEL_PRINT,								// Unary level
		-1,											// binary level
		true,										// Right to Left
		true,										// Void Operator
		false,										// recurse lists
		OPFUNC(TOpUnaryPtr, OpPrint),		// Unary Func. Ptr.
		0,											// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[14].Init(
		":",										// Op. Symbol
		OPLEVEL_OGLDRAW,							// Unary level
		-1,											// binary level
		true,										// Right to Left
		true,										// Void Operator
		true,										// recurse lists
		OPFUNC(TOpUnaryPtr, OpOGLDraw),		// Unary Func. Ptr.
		0,											// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[15].Init(
		OC_IDSYM_EQUAL_STR,							// Op. Symbol
		-1,											// Unary level
		OPLEVEL_EQUAL,								// binary level
		false,										// Right to Left
		false,										// Void Operator
		true,										// recurse lists
		0,											// Unary Func. Ptr.
		OPFUNC(TOpBinaryPtr, OpEqual),				// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[16].Init(
		OC_IDSYM_NOTEQUAL_STR,						// Op. Symbol
		-1,											// Unary level
		OPLEVEL_EQUAL,								// binary level
		false,										// Right to Left
		false,										// Void Operator
		true,										// recurse lists
		0,											// Unary Func. Ptr.
		OPFUNC(TOpBinaryPtr, OpNotEqual),			// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[17].Init(
		OC_IDSYM_AND_STR,							// Op. Symbol
		-1,											// Unary level
		OPLEVEL_AND,								// binary level
		false,										// Right to Left
		false,										// Void Operator
		true,										// recurse lists
		0,											// Unary Func. Ptr.
		OPFUNC(TOpBinaryPtr, OpAND),				// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[18].Init(
		OC_IDSYM_OR_STR,							// Op. Symbol
		-1,											// Unary level
		OPLEVEL_OR,									// binary level
		false,										// Right to Left
		false,										// Void Operator
		true,										// recurse lists
		0,											// Unary Func. Ptr.
		OPFUNC(TOpBinaryPtr, OpOR),				// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[19].Init(
		OC_IDSYM_NOT_STR,							// Op. Symbol
		OPLEVEL_NOT,								// Unary level
		-1,											// binary level
		false,										// Right to Left
		false,										// Void Operator
		true,										// recurse lists
		OPFUNC(TOpUnaryPtr, OpNOT),					// Unary Func. Ptr.
		0,											// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[20].Init(
		">",										// Op. Symbol
		-1,											// Unary level
		OPLEVEL_EQUAL,								// binary level
		false,										// Right to Left
		false,										// Void Operator
		true,										// recurse lists
		0,											// Unary Func. Ptr.
		OPFUNC(TOpBinaryPtr, OpGreater),			// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[21].Init(
		"<",										// Op. Symbol
		-1,											// Unary level
		OPLEVEL_EQUAL,								// binary level
		false,										// Right to Left
		false,										// Void Operator
		true,										// recurse lists
		0,											// Unary Func. Ptr.
		OPFUNC(TOpBinaryPtr, OpLess),				// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[22].Init(
		OC_IDSYM_LSHIFT_STR,						// Op. Symbol
		-1,											// Unary level
		OPLEVEL_LSHIFT,								// binary level
		false,										// Right to Left
		false,										// Void Operator
		false,										// recurse lists
		0,											// Unary Func. Ptr.
		0,											// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		OPFUNC(TOpABinaryPtr, OpLShift));			// Binary Assign Func Ptr.

	m_mOps[23].Init(
		OC_IDSYM_GE_STR,							// Op. Symbol
		-1,											// Unary level
		OPLEVEL_EQUAL,								// binary level
		false,										// Right to Left
		false,										// Void Operator
		true,										// recurse lists
		0,											// Unary Func. Ptr.
		OPFUNC(TOpBinaryPtr, OpGreaterEqual),		// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[24].Init(
		OC_IDSYM_LE_STR,							// Op. Symbol
		-1,											// Unary level
		OPLEVEL_EQUAL,								// binary level
		false,										// Right to Left
		false,										// Void Operator
		true,										// recurse lists
		0,											// Unary Func. Ptr.
		OPFUNC(TOpBinaryPtr, OpLessEqual),			// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[25].Init(
		"°",										// Op. Symbol
		-1,											// Unary level
		OPLEVEL_GRADE,								// binary level
		false,										// Right to Left
		false,										// Void Operator
		true,										// recurse lists
		0,											// Unary Func. Ptr.
		OPFUNC(TOpBinaryPtr,OpGradeProj),			// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[26].Init(
		OC_IDSYM_REF_STR,							// Op. Symbol
		-1,											// Unary level
		OPLEVEL_ASSIGN,								// binary level
		true,										// Right to Left
		false,										// Void Operator
		false,										// recurse lists
		0,											// Unary Func. Ptr.
		0,											// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		OPFUNC(TOpABinaryPtr,OpReference));			// Binary Assign Func Ptr.

	m_mOps[27].Init(
		"'",										// Op. Symbol
		OPLEVEL_INVOLUTE,							// Unary level
		-1,											// binary level
		true,										// Right to Left
		false,										// Void Operator
		true,										// recurse lists
		OPFUNC(TOpUnaryPtr, OpInvolute),			// Unary Func. Ptr.
		0,											// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[28].Init(
		OC_IDSYM_PAND_STR,							// Op. Symbol
		-1,											// Unary level
		OPLEVEL_PAND,								// binary level
		false,										// Right to Left
		false,										// Void Operator
		false,										// recurse lists
		0,											// Unary Func. Ptr.
		OPFUNC(TOpBinaryPtr, OpPointAND),			// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[29].Init(
		OC_IDSYM_PDIV_STR,							// Op. Symbol
		-1,											// Unary level
		OPLEVEL_PDIV,								// binary level
		false,										// Right to Left
		false,										// Void Operator
		false,										// recurse lists
		0,											// Unary Func. Ptr.
		OPFUNC(TOpBinaryPtr, OpPointDiv),			// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[30].Init(
		OC_IDSYM_PMUL_STR,							// Op. Symbol
		-1,											// Unary level
		OPLEVEL_PMUL,								// binary level
		false,										// Right to Left
		false,										// Void Operator
		false,										// recurse lists
		0,											// Unary Func. Ptr.
		OPFUNC(TOpBinaryPtr, OpPointProd),			// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[31].Init(
		OC_IDSYM_PIP_STR,							// Op. Symbol
		-1,											// Unary level
		OPLEVEL_PIP,								// binary level
		false,										// Right to Left
		false,										// Void Operator
		false,										// recurse lists
		0,											// Unary Func. Ptr.
		OPFUNC(TOpBinaryPtr, OpPointInnerProd),		// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[32].Init(
		OC_IDSYM_PMINUS_STR,						// Op. Symbol
		-1,											// Unary level
		OPLEVEL_PMINUS,								// binary level
		false,										// Right to Left
		false,										// Void Operator
		false,										// recurse lists
		0,											// Unary Func. Ptr.
		OPFUNC(TOpBinaryPtr, OpPointSubtract),		// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[33].Init(
		OC_IDSYM_PGT_STR,							// Op. Symbol
		-1,											// Unary level
		OPLEVEL_EQUAL,								// binary level
		false,										// Right to Left
		false,										// Void Operator
		false,										// recurse lists
		0,											// Unary Func. Ptr.
		OPFUNC(TOpBinaryPtr, OpPointGreater),		// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[34].Init(
		OC_IDSYM_PPLUS_STR,							// Op. Symbol
		-1,											// Unary level
		OPLEVEL_PPLUS,								// binary level
		false,										// Right to Left
		false,										// Void Operator
		false,										// recurse lists
		0,											// Unary Func. Ptr.
		OPFUNC(TOpBinaryPtr, OpPointAdd),			// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[35].Init(
		OC_IDSYM_POP_STR,							// Op. Symbol
		-1,											// Unary level
		OPLEVEL_POP,								// binary level
		false,										// Right to Left
		false,										// Void Operator
		false,										// recurse lists
		0,											// Unary Func. Ptr.
		OPFUNC(TOpBinaryPtr, OpPointWedge),			// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[36].Init(
		OC_IDSYM_PLSHIFT_STR,						// Op. Symbol
		-1,											// Unary level
		OPLEVEL_LSHIFT,								// binary level
		false,										// Right to Left
		false,										// Void Operator
		false,										// recurse lists
		0,											// Unary Func. Ptr.
		0,											// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		OPFUNC(TOpABinaryPtr, OpPointLShift));		// Binary Assign Func Ptr.

	m_mOps[37].Init(
		OC_IDSYM_POR_STR,							// Op. Symbol
		-1,											// Unary level
		OPLEVEL_POR,								// binary level
		false,										// Right to Left
		false,										// Void Operator
		false,										// recurse lists
		0,											// Unary Func. Ptr.
		OPFUNC(TOpBinaryPtr, OpPointOR),			// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[38].Init(
		OC_IDSYM_PLT_STR,							// Op. Symbol
		-1,											// Unary level
		OPLEVEL_EQUAL,								// binary level
		false,										// Right to Left
		false,										// Void Operator
		false,										// recurse lists
		0,											// Unary Func. Ptr.
		OPFUNC(TOpBinaryPtr, OpPointLess),			// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[39].Init(
		OC_IDSYM_PGTE_STR,							// Op. Symbol
		-1,											// Unary level
		OPLEVEL_EQUAL,								// binary level
		false,										// Right to Left
		false,										// Void Operator
		false,										// recurse lists
		0,											// Unary Func. Ptr.
		OPFUNC(TOpBinaryPtr, OpPointGreaterEqual),	// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[40].Init(
		OC_IDSYM_PLTE_STR,							// Op. Symbol
		-1,											// Unary level
		OPLEVEL_EQUAL,								// binary level
		false,										// Right to Left
		false,										// Void Operator
		false,										// recurse lists
		0,											// Unary Func. Ptr.
		OPFUNC(TOpBinaryPtr, OpPointLessEqual),		// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[41].Init(
		OC_IDSYM_PEQUAL_STR,						// Op. Symbol
		-1,											// Unary level
		OPLEVEL_EQUAL,								// binary level
		false,										// Right to Left
		false,										// Void Operator
		false,										// recurse lists
		0,											// Unary Func. Ptr.
		OPFUNC(TOpBinaryPtr, OpPointEqual),			// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[42].Init(
		OC_IDSYM_PNEQUAL_STR,						// Op. Symbol
		-1,											// Unary level
		OPLEVEL_EQUAL,								// binary level
		false,										// Right to Left
		false,										// Void Operator
		false,										// recurse lists
		0,											// Unary Func. Ptr.
		OPFUNC(TOpBinaryPtr, OpPointNotEqual),		// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	m_mOps[43].Init(
		OC_IDSYM_POW_STR,							// Op. Symbol
		-1,											// Unary level
		OPLEVEL_POWER,								// binary level
		false,										// Right to Left
		false,										// Void Operator
		false,										// recurse lists
		0,											// Unary Func. Ptr.
		OPFUNC(TOpBinaryPtr, OpPow),				// Binary Func. Ptr.
		0,											// Unary Assign Func Ptr
		0);											// Binary Assign Func Ptr.

	// Register Operators
	for(int i=0;i<iOpNo;i++)
	{
		if (!AddObjectClass(&m_mOps[i]))
			return false;
	}


	///////////////////////////////////////////////////////
	/// Functions

	////////////////////////////////////////////////////////////
	/// Mathematical functions
	AddFunc("sum", STDFUNC(TFuncPtr,SumFunc));
	AddFunc("sub", STDFUNC(TFuncPtr,SubFunc));
	AddFunc("prod", STDFUNC(TFuncPtr,ProdFunc));
	AddFunc("oprod", STDFUNC(TFuncPtr,OuterProdFunc));
	AddFunc("iprod", STDFUNC(TFuncPtr,InnerProdFunc));
	//AddFunc("arg", STDFUNC(TFuncPtr,ArgFunc));
	AddFunc("min", STDFUNC(TFuncPtr,MinFunc));
	AddFunc("max", STDFUNC(TFuncPtr,MaxFunc));
	AddFunc("argmin", STDFUNC(TFuncPtr,ArgMinFunc));
	AddFunc("argmax", STDFUNC(TFuncPtr,ArgMaxFunc));

	AddFunc("abs", STDFUNC(TFuncPtr,AbsFunc));
	AddFunc("pow", STDFUNC(TFuncPtr,PowFunc));
	AddFunc("sqrt", STDFUNC(TFuncPtr,SqrtFunc));
	AddFunc("inv", STDFUNC(TFuncPtr,InvFunc));
	AddFunc("fact", STDFUNC(TFuncPtr,FactFunc));
	AddFunc("sign", STDFUNC(TFuncPtr,SignFunc));
	AddFunc("sin", STDFUNC(TFuncPtr,SinFunc));
	AddFunc("cos", STDFUNC(TFuncPtr,CosFunc));
	AddFunc("tan", STDFUNC(TFuncPtr,TanFunc));
	AddFunc("asin", STDFUNC(TFuncPtr,AsinFunc));
	AddFunc("acos", STDFUNC(TFuncPtr,AcosFunc));
	AddFunc("atan", STDFUNC(TFuncPtr,AtanFunc));

	AddFunc("floor", STDFUNC(TFuncPtr,FloorFunc));
	AddFunc("ceil", STDFUNC(TFuncPtr,CeilFunc));
	AddFunc("round", STDFUNC(TFuncPtr,RoundFunc));
	AddFunc("exp", STDFUNC(TFuncPtr,ExpFunc));
	AddFunc("log", STDFUNC(TFuncPtr,LogFunc));

	////////////////////////////////////////////////////////////
	/// Geometric algebra functions
	AddFunc("proj", STDFUNC(TFuncPtr,ProjectFunc));
	AddFunc("rej", STDFUNC(TFuncPtr,RejectFunc));
	AddFunc("cp", STDFUNC(TFuncPtr,CPFunc));
	AddFunc("acp", STDFUNC(TFuncPtr,ACPFunc));

	AddFunc("VecE3", STDFUNC(TFuncPtr,VecE3Func));
	AddFunc("VecP3", STDFUNC(TFuncPtr,VecP3Func));
	AddFunc("VecN3", STDFUNC(TFuncPtr,VecN3Func));
	AddFunc("VecC2", STDFUNC(TFuncPtr,VecEl2Func));

	AddFunc("DegConic", STDFUNC(TFuncPtr,MakeConicDegenerateFunc));
	AddFunc("FindAllDegConics", STDFUNC(TFuncPtr,FindAllDegConicsFunc));

	AddFunc("RotorC2", STDFUNC(TFuncPtr,RotorC2Func));
	AddFunc("ReflectorC2", STDFUNC(TFuncPtr,ReflectorC2Func));

	AddFunc("RotorE3", STDFUNC(TFuncPtr,RotorE3Func));
	AddFunc("RotorP3", STDFUNC(TFuncPtr,RotorP3Func));
	AddFunc("RotorN3", STDFUNC(TFuncPtr,RotorN3Func));

	AddFunc("SphereN3", STDFUNC(TFuncPtr,SphereN3));
	AddFunc("TranslatorN3", STDFUNC(TFuncPtr,TranslatorN3));
	AddFunc("DilatorN3", STDFUNC(TFuncPtr,DilatorN3));

	AddFunc("DirVecN3", STDFUNC(TFuncPtr,DirVecN3));
	AddFunc("DirVecP3", STDFUNC(TFuncPtr,DirVecP3Func));

	AddFunc("DefVarsE3", STDFUNC(TFuncPtr,DefVarsE3));
	AddFunc("DefVarsP3", STDFUNC(TFuncPtr,DefVarsP3));
	AddFunc("DefVarsN3", STDFUNC(TFuncPtr,DefVarsN3));
	AddFunc("DefVarsC2", STDFUNC(TFuncPtr,DefVarsEl2));

	AddFunc("AffineVecP3", STDFUNC(TFuncPtr,AffineVecP3Func));
	AddFunc("FactorizeBlade", STDFUNC(TFuncPtr,FactorBladeFunc));

	AddFunc("GradeList", STDFUNC(TFuncPtr,GradeListFunc));
	AddFunc("BladeIdxList", STDFUNC(TFuncPtr,GetMVIndicesFunc));
	AddFunc("MV2Mask", STDFUNC(TFuncPtr,GenerateMaskFunc));
	AddFunc("AnalyzeMV", STDFUNC(TFuncPtr,AnalyzeMVFunc));

	////////////////////////////////////////////////////////////
	/// Random Number Generator Functions
	AddFunc("Ran", STDFUNC(TFuncPtr,RandomFunc));
	AddFunc("SetRanSeed", STDFUNC(TFuncPtr,RandomSeedFunc));
	AddFunc("Gauss", STDFUNC(TFuncPtr,GaussFunc));
	AddFunc("SetGaussPars", STDFUNC(TFuncPtr,GaussParametersFunc));

	////////////////////////////////////////////////////////////
	/// Drawing Functions
	AddFunc("SetMode", STDFUNC(TFuncPtr,SetModeFunc));
	AddFunc("Offset", STDFUNC(TFuncPtr,OffsetFunc));

	AddFunc("Plot", STDFUNC(TFuncPtr,PlotFunc));
	AddFunc("SetPlotMode", STDFUNC(TFuncPtr,SetPlotModeFunc));

	AddFunc("Color", STDFUNC(TFuncPtr, ColorFunc));

	AddFunc("DrawPlane", STDFUNC(TFuncPtr,DrawPlaneFunc));
	AddFunc("DrawBox", STDFUNC(TFuncPtr,DrawBoxFunc));
	AddFunc("DrawLine", STDFUNC(TFuncPtr,DrawLineFunc));
	AddFunc("DrawPoint", STDFUNC(TFuncPtr,DrawPointFunc));
	AddFunc("DrawPointSurface", STDFUNC(TFuncPtr,DrawPointSurfaceFunc));
	AddFunc("DrawPointGrid", STDFUNC(TFuncPtr,DrawPointGridFunc));
	AddFunc("DrawPointList", STDFUNC(TFuncPtr,DrawPointListFunc));

	AddFunc("SetArrowShape", STDFUNC(TFuncPtr,SetArrowShapeFunc));
	AddFunc("DrawArrow", STDFUNC(TFuncPtr,DrawArrowFunc));
	AddFunc("DrawArc", STDFUNC(TFuncPtr,DrawArcFunc));
	AddFunc("DrawDisk", STDFUNC(TFuncPtr,DrawDiskFunc));
	AddFunc("DrawEllipse", STDFUNC(TFuncPtr,DrawEllipseFunc));
	AddFunc("DrawEllipsoid", STDFUNC(TFuncPtr,DrawEllipsoidFunc));
	AddFunc("DrawCircle", STDFUNC(TFuncPtr,DrawCircleFunc));
	AddFunc("DrawCircleSurface", STDFUNC(TFuncPtr,DrawCircleSurfaceFunc));
	AddFunc("DrawLineSurface", STDFUNC(TFuncPtr,DrawLineSurfaceFunc));
	AddFunc("DrawTriangle", STDFUNC(TFuncPtr,DrawTriangleFunc));
	AddFunc("DrawCone", STDFUNC(TFuncPtr,DrawConeFunc));
	AddFunc("DrawSphere", STDFUNC(TFuncPtr,DrawSphereFunc));
	AddFunc("DrawCylinder", STDFUNC(TFuncPtr,DrawCylinderFunc));
	AddFunc("DrawIcosahedron", STDFUNC(TFuncPtr,DrawIcosahedronFunc));
	AddFunc("DrawQuantumFractal", STDFUNC(TFuncPtr,DrawQuantumFractalFunc));

	AddFunc("EnableFog", STDFUNC(TFuncPtr,EnableFogFunc));
	AddFunc("SetFogDensity", STDFUNC(TFuncPtr,SetFogDensityFunc));
	AddFunc("SetFogColor", STDFUNC(TFuncPtr,SetFogColorFunc));

	AddFunc("StartOverlay", STDFUNC(TFuncPtr,StartOverlayFunc));
	AddFunc("EndOverlay", STDFUNC(TFuncPtr,EndOverlayFunc));

	AddFunc("StartView", STDFUNC(TFuncPtr,StartViewFunc));
	AddFunc("EndView", STDFUNC(TFuncPtr,EndViewFunc));

	AddFunc("PushFrame", STDFUNC(TFuncPtr,PushFrameFunc));
	AddFunc("PopFrame", STDFUNC(TFuncPtr,PopFrameFunc));

	AddFunc("SetPointSize", STDFUNC(TFuncPtr,SetPointSizeFunc));
	AddFunc("GetPointSize", STDFUNC(TFuncPtr,GetPointSizeFunc));
	AddFunc("SetLineWidth", STDFUNC(TFuncPtr,SetLineWidthFunc));
	AddFunc("GetLineWidth", STDFUNC(TFuncPtr,GetLineWidthFunc));

	AddFunc("EnableSmoothPoint", STDFUNC(TFuncPtr,SetSmoothPointFunc));
	AddFunc("IsSmoothPoint", STDFUNC(TFuncPtr,GetSmoothPointFunc));
	AddFunc("EnableSmoothLine", STDFUNC(TFuncPtr,SetSmoothLineFunc));
	AddFunc("IsSmoothLine", STDFUNC(TFuncPtr,GetSmoothLineFunc));
	AddFunc("EnableSmoothPolygon", STDFUNC(TFuncPtr,SetSmoothPointFunc));
	AddFunc("IsSmoothPolygon", STDFUNC(TFuncPtr,GetSmoothPointFunc));

	AddFunc("SetFrameOrigin", STDFUNC(TFuncPtr,SetFrameOriginFunc));
	AddFunc("SetFrameRotor", STDFUNC(TFuncPtr,SetFrameRotorFunc));

	AddFunc("AddFrameOrigin", STDFUNC(TFuncPtr,AddFrameOriginFunc));
	AddFunc("AddFrameRotor", STDFUNC(TFuncPtr,AddFrameRotorFunc));

	AddFunc("TranslateFrame", STDFUNC(TFuncPtr,TranslateFrameFunc));
	AddFunc("RotateFrame", STDFUNC(TFuncPtr,RotateFrameFunc));

	AddFunc("TranslateView", STDFUNC(TFuncPtr,TranslateViewFunc));
	AddFunc("RotateView", STDFUNC(TFuncPtr,RotateViewFunc));
	AddFunc("SetViewAngle", STDFUNC(TFuncPtr,SetViewAngleFunc));

	////////////////////////////////////////////////////////////
	/// Matrix Functions
	AddFunc("Matrix", STDFUNC(TFuncPtr,MatrixFunc));
	AddFunc("ResizeMatrix", STDFUNC(TFuncPtr,ResizeMatrixFunc));
	AddFunc("ReshapeMatrix", STDFUNC(TFuncPtr,ReshapeMatrixFunc));
	AddFunc("AddRows", STDFUNC(TFuncPtr,AddMatrixRowsFunc));
	AddFunc("AddCols", STDFUNC(TFuncPtr,AddMatrixColsFunc));
	AddFunc("RowSpace", STDFUNC(TFuncPtr,ReduceMatrixToRowSpaceFunc));
	AddFunc("FindMainNRows", STDFUNC(TFuncPtr,ReduceMatrixToMajorRowsFunc));
	AddFunc("GetRows", STDFUNC(TFuncPtr,ExtractMatrixRowsFunc));

	AddFunc("EigenValues", STDFUNC(TFuncPtr,EigenValuesFunc));
	AddFunc("Eigen", STDFUNC(TFuncPtr,EigenFunc));
	AddFunc("SVD", STDFUNC(TFuncPtr,SVDFunc));
	AddFunc("det", STDFUNC(TFuncPtr,DetFunc));

	AddFunc("Diag2Row", STDFUNC(TFuncPtr,DiagToVectorFunc));
	AddFunc("Row2Diag", STDFUNC(TFuncPtr,VectorToDiagFunc));
	AddFunc("DiagToRow", STDFUNC(TFuncPtr,DiagToVectorFunc));
	AddFunc("RowToDiag", STDFUNC(TFuncPtr,VectorToDiagFunc));
	AddFunc("IdMatrix", STDFUNC(TFuncPtr,IdentityMatrixFunc));

	AddFunc("GetMVProductMatrix", STDFUNC(TFuncPtr,GetMVProductMatrixFunc));
	AddFunc("MV2Matrix", STDFUNC(TFuncPtr,GetMVMatrixFunc));
	AddFunc("Matrix2MV", STDFUNC(TFuncPtr,GetMatrixMVFunc));

	AddFunc("Sym2Col", STDFUNC(TFuncPtr,Sym2ColMatrixFunc));
	AddFunc("Col2Sym", STDFUNC(TFuncPtr,Col2SymMatrixFunc));

	////////////////////////////////////////////////////////////
	/// Tensor Functions
	AddFunc("Tensor", STDFUNC(TFuncPtr,TensorFunc));
	AddFunc("MV2Tensor", STDFUNC(TFuncPtr,GetMVTensorFunc));
	AddFunc("Tensor2MV", STDFUNC(TFuncPtr,GetTensorMVFunc));
	AddFunc("GAOpTensor", STDFUNC(TFuncPtr,GetGAOpTensorFunc));

	////////////////////////////////////////////////////////////
	/// Miscellaneous Functions

	AddFunc("SetTextPrec", STDFUNC(TFuncPtr,SetTextPrecFunc));
	AddFunc("SetEvalPrec", STDFUNC(TFuncPtr,SetEvalPrecFunc));
	AddFunc("SetVisPrec", STDFUNC(TFuncPtr,SetVisPrecFunc));

	AddFunc("SetVisWinSize", STDFUNC(TFuncPtr,SetOGLSizeFunc));

	AddFunc("SetMouseMode", STDFUNC(TFuncPtr,SetMouseModeFunc));
	AddFunc("GetMouseMode", STDFUNC(TFuncPtr,GetMouseModeFunc));

	AddFunc("SetMouseValue", STDFUNC(TFuncPtr,SetMouseValueFunc));
	AddFunc("GetMouseValue", STDFUNC(TFuncPtr,GetMouseValueFunc));

	AddFunc("SetMouseSens", STDFUNC(TFuncPtr,SetMouseSensFunc));
	AddFunc("GetMouseSens", STDFUNC(TFuncPtr,GetMouseSensFunc));
	
	AddFunc("Mouse", STDFUNC(TFuncPtr,MouseFunc));
	AddFunc("Len", STDFUNC(TFuncPtr,LenFunc));
	AddFunc("Size", STDFUNC(TFuncPtr,SizeFunc));
	AddFunc("Str", STDFUNC(TFuncPtr,StringFunc));
	AddFunc("String", STDFUNC(TFuncPtr,StringFunc));
	AddFunc("Scalar", STDFUNC(TFuncPtr,ScalarFunc));
	AddFunc("Type", STDFUNC(TFuncPtr,TypeFunc));
	AddFunc("argtrue", STDFUNC(TFuncPtr,ArgTrueFunc));
	AddFunc("sort", STDFUNC(TFuncPtr,SortFunc));

	////////////////////////////////////////////////////////////
	/// List Function

	AddFunc("List", STDFUNC(TFuncPtr,ListFunc));
	AddFunc("SubList", STDFUNC(TFuncPtr,ExtractListElementsFunc));
	AddFunc("RemList", STDFUNC(TFuncPtr,RemoveListElementsFunc));
	AddFunc("InsList", STDFUNC(TFuncPtr,InsertListElementsFunc));
	AddFunc("TransList", STDFUNC(TFuncPtr,TransposeListElementsFunc));

	AddFunc("CombIdxList", STDFUNC(TFuncPtr,CombinationIndexListFunc));
	AddFunc("PermIdxList", STDFUNC(TFuncPtr,PermutationIndexListFunc));

	////////////////////////////////////////////////////////////
	/// File Functions

	AddFunc("WriteVar", STDFUNC(TFuncPtr,WriteVariableFunc));
	AddFunc("ReadVar", STDFUNC(TFuncPtr,ReadVariableFunc));
	AddFunc("ReadXML", STDFUNC(TFuncPtr,ReadXMLFunc));
	AddFunc("ReadText", STDFUNC(TFuncPtr,ReadTextFunc));
	AddFunc("WriteText", STDFUNC(TFuncPtr,WriteTextFunc));
	AddFunc("ReadData", STDFUNC(TFuncPtr,ReadDataFunc));
	AddFunc("WriteData", STDFUNC(TFuncPtr,WriteDataFunc));
	AddFunc("ReadMatrix", STDFUNC(TFuncPtr,ReadMatrixFunc));
	AddFunc("ShowFile", STDFUNC(TFuncPtr,ShowFileFunc));
	AddFunc("FileChooser", STDFUNC(TFuncPtr,FileChooserFunc));
	AddFunc("SaveScreen", STDFUNC(TFuncPtr,SaveScreenFunc));

	////////////////////////////////////////////////////////////
	/// Latex Functions

	AddFunc("Latex", STDFUNC(TFuncPtr,LatexStringFunc));
	AddFunc("DrawLatex", STDFUNC(TFuncPtr,DrawLatexFunc));
	AddFunc("GetLatexImg", STDFUNC(TFuncPtr,GetLatexImageFunc));
	AddFunc("SetLatexMagStep", STDFUNC(TFuncPtr,SetLatexMagStepFunc));
	AddFunc("EnableRenderLatex", STDFUNC(TFuncPtr,SetRenderLatexFunc));
	AddFunc("SetLatexAlign", STDFUNC(TFuncPtr,SetLatexAlignFunc));

	////////////////////////////////////////////////////////////
	/// Image Functions

	AddFunc("Image", STDFUNC(TFuncPtr,NewBMPFunc));
	AddFunc("DrawImg", STDFUNC(TFuncPtr,DrawBMPFunc));
	AddFunc("ReadImg", STDFUNC(TFuncPtr,LoadBMPFunc));
	AddFunc("WriteImg", STDFUNC(TFuncPtr,SaveBMPFunc));
	AddFunc("EnableImgTransparency", STDFUNC(TFuncPtr,EnableBMPTransparencyFunc));
	AddFunc("SetImgTransparentColor", STDFUNC(TFuncPtr,SetBMPTransparentColorFunc));
	AddFunc("SetImgScale", STDFUNC(TFuncPtr,SetBMPScaleFunc));
	AddFunc("GetImgCoords", STDFUNC(TFuncPtr,GetBMPCoordsFunc));
	AddFunc("SetImgAlign", STDFUNC(TFuncPtr,SetBMPAlignFunc));
	AddFunc("SetImgPos", STDFUNC(TFuncPtr,SetBMPPosFunc));
	AddFunc("Img2Matrix", STDFUNC(TFuncPtr,BMP2MatrixFunc));
	AddFunc("Matrix2Img", STDFUNC(TFuncPtr,Matrix2BMPFunc));
	//AddFunc("RotateImg", STDFUNC(TFuncPtr,RotateBMPFunc));
	AddFunc("CopyImg", STDFUNC(TFuncPtr,CopyBMPFunc));
	AddFunc("ResizeImg", STDFUNC(TFuncPtr,ResizeBMPFunc));
	AddFunc("ReshapeImg", STDFUNC(TFuncPtr,ReshapeBMPFunc));
	AddFunc("InsertImg", STDFUNC(TFuncPtr,InsertBMPFunc));
	AddFunc("SetTexture", STDFUNC(TFuncPtr,SetBMPTextureFunc));
	AddFunc("SetTextureRepeat", STDFUNC(TFuncPtr,SetBMPTextureRepeatFunc));
	AddFunc("EnableAbsTexCoords", STDFUNC(TFuncPtr,EnableBMPAbsTexCoordsFunc));

	////////////////////////////////////////////////////////////
	/// Presentation Functions

	AddFunc("_ClearScriptList", STDFUNC(TFuncPtr,ClearScriptListFunc));
	AddFunc("_AddScriptToList", STDFUNC(TFuncPtr,AddScriptToListFunc));

	////////////////////////////////////////////////////////////
	/// Error Propagation

	AddFunc("EPOp", STDFUNC(TFuncPtr,ErrorPropagationOperationFunc));
	AddFunc("EPVecN3", STDFUNC(TFuncPtr,ErrorPropVecN3Func));
	AddFunc("EPCCVecN3", STDFUNC(TFuncPtr,ErrorPropCrossCovVecN3Func));
	AddFunc("EPPrjVecN3ToE3", STDFUNC(TFuncPtr,ErrorPropPrjVecN3ToE3Func));

	AddFunc("EPVecC2", STDFUNC(TFuncPtr,ErrorPropVecC2Func));

	AddFunc("EPsqrt", STDFUNC(TFuncPtr,ErrorPropSQRTFunc));
	AddFunc("EPinv", STDFUNC(TFuncPtr,ErrorPropINVFunc));
	AddFunc("EPmul", STDFUNC(TFuncPtr,ErrorPropMULFunc));


	////////////////////////////////////////////////////////////
	/// Lighting Functions

	AddFunc("ResetLight", STDFUNC(TFuncPtr,ResetLightFunc));
	AddFunc("SetAmbientLight", STDFUNC(TFuncPtr,SetAmbientLightFunc));
	AddFunc("EnableLight", STDFUNC(TFuncPtr,EnableLightFunc));
	AddFunc("EnableLightFixed", STDFUNC(TFuncPtr,EnableLightFixedFunc));
	AddFunc("SetLightPos", STDFUNC(TFuncPtr,SetLightPosFunc));

	AddFunc("SetLightAmbientCol", STDFUNC(TFuncPtr,SetLightAmbientColFunc));
	AddFunc("SetLightDiffuseCol", STDFUNC(TFuncPtr,SetLightDiffuseColFunc));
	AddFunc("SetLightSpecularCol", STDFUNC(TFuncPtr,SetLightSpecularColFunc));

	AddFunc("SetLightSpotDir", STDFUNC(TFuncPtr,SetLightSpotDirFunc));
	AddFunc("SetLightSpotExp", STDFUNC(TFuncPtr,SetLightSpotExpFunc));
	AddFunc("SetLightSpotCutOff", STDFUNC(TFuncPtr,SetLightSpotCutOffFunc));
	AddFunc("SetLightAttenuation", STDFUNC(TFuncPtr,SetLightAttenuationFunc));


	////////////////////////////////////////////////////////////
	/// Tool Functions

	AddFunc("Slider", STDFUNC(TFuncPtr,ToolSliderFunc));
	AddFunc("Stepper", STDFUNC(TFuncPtr,ToolCounterFunc));
	AddFunc("Input", STDFUNC(TFuncPtr,ToolInputFunc));
	AddFunc("InputText", STDFUNC(TFuncPtr,ToolInputTextFunc));
	AddFunc("CheckBox", STDFUNC(TFuncPtr,ToolCheckBoxFunc));
	AddFunc("Button", STDFUNC(TFuncPtr,ToolButtonFunc));
	AddFunc("Choice", STDFUNC(TFuncPtr,ToolChoiceFunc));


	////////////////////////////////////////////////////////////
	/// Info Window Functions

	AddFunc("SetInfoText", STDFUNC(TFuncPtr,SetInfoTextFunc));
	AddFunc("SetInfoWidth", STDFUNC(TFuncPtr,SetInfoWidthFunc));


	////////////////////////////////////////////////////////////
	/// Serial IO Port Functions
#ifdef WIN32
	AddFunc("OpenSP", STDFUNC(TFuncPtr,OpenSerialPortFunc));
	AddFunc("WriteSP", STDFUNC(TFuncPtr,WriteToSerialPortFunc));
	AddFunc("ReadSP", STDFUNC(TFuncPtr,ReadFromSerialPortFunc));
#endif

	////////////////////////////////////////////////////////////
	/// Animation Functions
	AddFunc("EnableAnimate", STDFUNC(TFuncPtr,EnableAnimationFunc));
	AddFunc("SetAnimateTimeStep", STDFUNC(TFuncPtr,SetAnimationTimeStepFunc));
	AddFunc("GetTime", STDFUNC(TFuncPtr,GetTimeFunc));
	

	////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////


	//AddFunc("Repeat", STDFUNC(TFuncPtr, RepeatFunc));

	AddFunc("DrawText", STDFUNC(TFuncPtr,DrawTextFunc));
	AddFunc("SetTextSize", STDFUNC(TFuncPtr,SetBitmapFontSizeFunc));
/*
	AddFunc("SetFont", STDFUNC(TFuncPtr,SetBitmapFontFunc));
	AddFunc("SetFontScale", STDFUNC(TFuncPtr,SetBitmapFontScaleFunc));
*/

	///////////////////////////////////////////////////////
	/// Predefined Variables

	/// Colors
	DefineColor("Red",   1.0f, 0.0f, 0.0f);
	DefineColor("Green", 0.0f, 1.0f, 0.0f);
	DefineColor("Blue",  0.0f, 0.0f, 1.0f);

	DefineColor("Cyan",   0.0f, 1.0f, 1.0f);
	DefineColor("Yellow", 1.0f, 1.0f, 0.0f);
	DefineColor("Magenta",  1.0f, 0.0f, 1.0f);

	DefineColor("Orange",   1.0f, 0.7f, 0.0f);
	DefineColor("White", 1.0f, 1.0f, 1.0f);
	DefineColor("Black",  0.0f, 0.0f, 0.0f);

	DefineColor("MRed",   0.6f, 0.0f, 0.0f);
	DefineColor("MGreen", 0.0f, 0.6f, 0.0f);
	DefineColor("MBlue",  0.0f, 0.0f, 0.6f);

	DefineColor("MCyan",   0.0f, 0.6f, 0.6f);
	DefineColor("MYellow", 0.6f, 0.6f, 0.0f);
	DefineColor("MMagenta",  0.6f, 0.0f, 0.6f);

	DefineColor("MOrange",   0.6f, 0.1f, 0.0f);
	DefineColor("MGray", 0.6f, 0.6f, 0.6f);
	DefineColor("MWhite", 0.6f, 0.6f, 0.6f);

	DefineColor("DRed",   0.3f, 0.0f, 0.0f);
	DefineColor("DGreen", 0.0f, 0.3f, 0.0f);
	DefineColor("DBlue",  0.0f, 0.0f, 0.3f);

	DefineColor("DCyan",   0.0f, 0.3f, 0.3f);
	DefineColor("DYellow", 0.3f, 0.3f, 0.0f);
	DefineColor("DMagenta",  0.3f, 0.0f, 0.3f);

	DefineColor("DOrange",   0.3f, 0.1f, 0.0f);
	DefineColor("DGray", 0.3f, 0.3f, 0.3f);
	DefineColor("DWhite", 0.3f, 0.3f, 0.3f);
	
	DefineColor("Alpha", 0.0f, 0.0f, 0.0f, -0.2f);
	DefineColor("MAlpha", 0.0f, 0.0f, 0.0f, -0.4f);
	DefineColor("DAlpha", 0.0f, 0.0f, 0.0f, -0.7f);
	

	DefineString("ToolName", "None");
	DefineString("LinkData", "None");

	DefineInt("true", 1);
	DefineInt("false", 0);

	DefineInt("ExecMode", 0);
	DefineInt("EM_NEW", EXEC_MODE_NEW);
	DefineInt("EM_CHANGE", EXEC_MODE_CHANGE);
	DefineInt("EM_ANIMATE", EXEC_MODE_ANIM);
	DefineInt("EM_USER", EXEC_MODE_USER);
	DefineInt("EM_TOOL", EXEC_MODE_TOOL);
	DefineInt("EM_AGAIN", EXEC_MODE_AGAIN);
	DefineInt("EM_KEYUP", EXEC_MODE_KEYUP);
	DefineInt("EM_KEYDOWN", EXEC_MODE_KEYDOWN);
	DefineInt("EM_RESHAPE", EXEC_MODE_RESHAPE);
	DefineInt("EM_LINK", EXEC_MODE_LINK);

	DefineInt("MVOP_GEO", int(MVOP_GEO));
	DefineInt("MVOP_INNER", int(MVOP_INNER));
	DefineInt("MVOP_OUTER", int(MVOP_OUTER));
	DefineInt("MVOP_ADD", int(MVOP_ADD));
	DefineInt("MVOP_SUB", int(MVOP_SUB));
	DefineInt("MVOP_DUAL", int(MVOP_DUAL));
	DefineInt("MVOP_REVERSE", int(MVOP_REVERSE));
	DefineInt("MVOP_INVOLUTE", int(MVOP_INVOLUTE));

	DefineInt("GA_E3", int(ID_E3GA));
	DefineInt("GA_P3", int(ID_PGA));
	DefineInt("GA_N3", int(ID_CONFGA));
	DefineInt("GA_C2", int(ID_EL2GA));

	DefineInt("OPNS", MODE_OPNS);
	DefineInt("IPNS", MODE_IPNS);

	DefineInt("E3_DRAW_VEC_AS_LINE", E3M_DRAW_VEC_AS_LINE);
	DefineInt("E3_DRAW_VEC_AS_ARROW", E3M_DRAW_VEC_AS_ARROW);
	DefineInt("E3_DRAW_VEC_AS_POINT", E3M_DRAW_VEC_AS_POINT);

	DefineInt("E3_DRAW_PLANE_AS_RECT", E3M_DRAW_PLANE_AS_RECT);
	DefineInt("E3_DRAW_PLANE_AS_DISK", E3M_DRAW_PLANE_AS_DISK);

	DefineInt("N3_OPNS", N3M_OPNS);
	DefineInt("N3_IPNS", N3M_IPNS);
	DefineInt("N3_SOLID", N3M_SOLID);
	DefineInt("N3_WIRE", N3M_WIRE);
	DefineInt("N3_SHOW_IMAG", N3M_SHOW_IMAG);
	DefineInt("N3_HIDE_IMAG", N3M_HIDE_IMAG);

	DefineInt("DRAW_POINT_AS_DOT", DRAW_POINT_AS_DOT);
	DefineInt("DRAW_POINT_AS_SPHERE", DRAW_POINT_AS_SPHERE);

	// Plot Modes
	DefineInt("POINTS", PLOT_POINTS);
	DefineInt("CONNECTED", PLOT_CONNECTED);

	// Fonts
/*
	DefineInt("FONT_FixedSys_Std_9", OGL_BMPFONT_FixedSys_Std_9);
	DefineInt("FONT_FixedSys_Bold_9", OGL_BMPFONT_FixedSys_Bold_9);
	DefineInt("FONT_Times_Std_12", OGL_BMPFONT_Times_Std_12);
	DefineInt("FONT_Sylfaen_Bold_24", OGL_BMPFONT_Sylfaen_Bold_24);
	DefineInt("FONT_Symbol_Std_12", OGL_BMPFONT_Symbol_Std_12);
	DefineInt("FONT_Symbol_Bold_12", OGL_BMPFONT_Symbol_Bold_12);
	DefineInt("FONT_Symbol_Std_24", OGL_BMPFONT_Symbol_Std_24);
	DefineInt("FONT_T2_Std_12", OGL_BMPFONT_T2_Std_12);
	DefineInt("FONT_T2_Std_24", OGL_BMPFONT_T2_Std_24);
*/

	DefineDouble("dTime", 0.0);
	DefineDouble("Time", 0.0);
	DefineDouble("Pi", 3.14159265359);
	DefineDouble("RadPerDeg", 3.14159265359 / 180.0);

	//printf("CLUParse:Init: End\n");
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Define Color

bool CCLUParse::DefineColor(const char *pcName, float fR, float fG, float fB, float fA, bool bProtect)
{
	CCodeVar *pVar;

	if (!pcName)
		return false;

	m_CodeBase.DeleteConstVar(pcName);

	pVar = &m_CodeBase.NewConstVar(pcName, PDT_COLOR);
	((COGLColor *) pVar->Val())->Set(fR, fG, fB, fA);

	if (bProtect)
		pVar->EnableProtect();

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Define Float

bool CCLUParse::DefineFloat(const char *pcName, float fVal, bool bProtect)
{
	CCodeVar *pVar;

	if (!pcName)
		return false;

	m_CodeBase.DeleteConstVar(pcName);
	pVar = &m_CodeBase.NewConstVar(pcName, PDT_FLOAT);
	*((float *) pVar->Val()) = fVal;

	if (bProtect)
		pVar->EnableProtect();

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Define Double

bool CCLUParse::DefineDouble(const char *pcName, double dVal, bool bProtect)
{
	CCodeVar *pVar;

	if (!pcName)
		return false;

	m_CodeBase.DeleteConstVar(pcName);
	pVar = &m_CodeBase.NewConstVar(pcName, PDT_DOUBLE);
	*((double *) pVar->Val()) = dVal;

	if (bProtect)
		pVar->EnableProtect();

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Define Integer

bool CCLUParse::DefineInt(const char *pcName, int iVal, bool bProtect)
{
	CCodeVar *pVar;

	if (!pcName)
		return false;

	m_CodeBase.DeleteConstVar(pcName);
	pVar = &m_CodeBase.NewConstVar(pcName, PDT_INT);
	*((int *) pVar->Val()) = iVal;

	if (bProtect)
		pVar->EnableProtect();

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Define String

bool CCLUParse::DefineString(const char *pcName, const char *pcVal, bool bProtect)
{
	CCodeVar *pVar;

	if (!pcName)
		return false;

	m_CodeBase.DeleteConstVar(pcName);
	pVar = &m_CodeBase.NewConstVar(pcName, PDT_STRING);
	*((TString *) pVar->Val()) = pcVal;

	if (bProtect)
		pVar->EnableProtect();

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Define Version

bool CCLUParse::DefineVersion()
{
	CCodeVar *pVar;

	m_CodeBase.DeleteConstVar("VERSION");
	pVar = &m_CodeBase.NewConstVar("VERSION", PDT_VARLIST);
	TVarList &rList = *(pVar->GetVarListPtr());

	int iMaj, iMin, iRev;
	GetVersion(iMaj, iMin, iRev);

	rList.Set(3);
	rList[0] = iMaj;
	rList[1] = iMin;
	rList[2] = iRev;

	pVar->EnableProtect();

	return true;
}



//////////////////////////////////////////////////////////////////////
/// Print Parse Errors

cStr CCLUParse::PrintParseErrors()
{
	cStr csLine, csError, csStr = "", csText = "";
	int iErrNo, iLen, iTextLine, iRelPos;
	
/*
	if (!GetLineCount())
		return csText;
*/

	iErrNo = GetError();
	if (iErrNo == CPB_NOERROR)
		return csText;

	csError = GetErrorStr(iErrNo);
	
	csText << "Error No. " << iErrNo << ": " << csError << "<br>";

	if (iErrNo == CPB_NO_INCLUDEFILE)
	{
		if (m_sErrorFilename.size() > 0)
		{
			csText << "In file '<b>" << m_sErrorFilename.c_str() 
					<< "</b>' could not load '<b>" << m_sIncludeFilename.c_str() << "</b>'.<br>";
		}
		else
		{
			csText << "Could not load '<b>" << m_sIncludeFilename.c_str() << "</b>'.<br>";
		}
	}
	else if (iErrNo == CPB_RECURSIVE_INCLUDEFILE)
	{
		if (m_sErrorFilename.size() > 0)
		{
			csText << "In file '<b>" << m_sErrorFilename.c_str() 
					<< "</b>' the file '<b>" << m_sIncludeFilename.c_str() << "</b>' is included recursively.<br>";
		}
		else
		{
			csText << "File '<b>" << m_sIncludeFilename.c_str() << "</b>' is included recursively.<br>";
		}
	}
	else
	{
		STextLine &rLine = m_msText[m_iCurLine];
		iTextLine = rLine.iLine;

		if (rLine.csFilename.Len() > 0)
			csText << "In file '<b>" << rLine.csFilename << "</b>',<br>";

		csStr << "<i>Line " << (iTextLine+1) << " (" << (m_iCurPos+1) << ")</i>: ";

/*
		int i, iSize = rLine.vecPos.size();
		for(i=0;i<iSize;i++)
		{
			csStr << rLine.vecPos[i] << ", ";
		}
		csStr << "\n";
*/
		string sTextLine = rLine.csInputText.Str();
		iLen = sTextLine.size();

		if (m_iCurPos >= int(rLine.vecPos.size()) )
		{
			iRelPos = rLine.vecPos.back() + 1 - rLine.iInputTextStartPos;
			sTextLine += " ";
		}
		else
			iRelPos = rLine.vecPos[m_iCurPos] - rLine.iInputTextStartPos;

		int iStart = sTextLine.rfind('\n', iRelPos);
		if (iStart < 0)
			iStart = 0;
		else
			iStart++;

		int iEnd = sTextLine.find('\n', iRelPos);
		if (iEnd < 0)
			iEnd = sTextLine.size()-1;
		else
			iEnd--;

		csText << csStr << "<code><b>" << sTextLine.substr(iStart, iRelPos-iStart).c_str();
		csText << "@" << sTextLine.substr(iRelPos, iEnd-iRelPos+1).c_str() << "</b></code><br>";
	}

	return csText;
}

//////////////////////////////////////////////////////////////////////
/// Get position of parse error

bool CCLUParse::GetParseErrorPos(int& iLine, int& iPos, int &iInputPos, cStr &csFilename)
{
	cStr csLine, csError, csStr = "", csText = "";
	
	iLine = -1;
	iPos = -1;
	iInputPos = -1;

	if (!GetLineCount())
		return false;

	GetCurLine(csLine, iLine, iPos);

	STextLine &rLine = m_msText[iLine];
//	if (rLine.iMainFilePos < 0)
//	{
		if (iPos >= int(rLine.vecPos.size()))
			iInputPos = rLine.vecPos.back();
		else
			iInputPos = rLine.vecPos[iPos];
//	}
//	else
//	{
//		iInputPos = rLine.iMainFilePos;
//	}


	if (rLine.csFilename.Len())
		csFilename = rLine.csFilename;
	else
	{
		csFilename = m_sScriptName.c_str();
	}

	if (!strchr(csFilename.Str(), '\\') &&
		!strchr(csFilename.Str(), '/') &&
		!strchr(csFilename.Str(), ':'))
	{
		csFilename = m_sScriptPath.c_str() + csFilename;
	}

	iLine = GetTextLineNo(iLine) + 1;
	iPos++;


	return true;
}

//////////////////////////////////////////////////////////////////////
/// Print Code Errors

cStr CCLUParse::PrintCodeErrors()
{
	cStr csLine, csError, csStr = "", csText, csLevel;
	int iPos, iLine, iErrNo, iMsgCount, iMsg, iTextLine, iLen;
	
	CCodeErrorList &rErrList = m_CodeBase.m_ErrorList;

	iMsgCount = rErrList.Count();

	for(iMsg=0;iMsg<iMsgCount;iMsg++)
	{
		csText = "";
		csStr = "";

		SMsg& rMsg = rErrList[iMsg];
	
		iLine = rMsg.iLine;
		iPos = rMsg.iPos;
		iErrNo = rMsg.iNo;
		csError = rMsg.csText;

		switch(rMsg.iLevel)
		{
		case CEL_FATAL:
			csLevel = "Fatal Error";
			break;

		case CEL_ERROR:
			csLevel = "Error";
			break;

		case CEL_WARNING:
			csLevel = "Warning";
			break;

		default:
			csLevel = "Message";
			break;
		}

		if (GetLineCount())
		{
			csLine = GetLine(iLine);
			iTextLine = GetTextLineNo(iLine);
		}
		else
		{
			csLine = "";
			iTextLine = 0;
		}
		
		iLen = csLine.Len();
		csText << csLevel << " No. " << iErrNo << ": " << csError << "<br>";
		
		if (iTextLine >= 0 && iPos >= 0 && iLine < int(m_msText.Count()) )
		{
			int iRelPos;
			STextLine &rLine = m_msText[iLine];

			if (rLine.csFilename.Len() > 0)
				csText << "In file '<b>" << rLine.csFilename << "</b>',<br>";

			csStr << "<i>Line " << (iTextLine+1) << " (" << (iPos+1) << ")</i>: ";

			/*
			int i, iSize = rLine.vecPos.size();
			for(i=0;i<iSize;i++)
			{
			csStr << rLine.vecPos[i] << ", ";
			}
			csStr << "\n";
			*/
			string sTextLine = rLine.csInputText.Str();
			iLen = sTextLine.size();

			if (iPos >= int(rLine.vecPos.size()) )
			{
				iRelPos = rLine.vecPos.back() + 1 - rLine.iInputTextStartPos;
				sTextLine += " ";
			}
			else
				iRelPos = rLine.vecPos[iPos] - rLine.iInputTextStartPos;

			int iStart = sTextLine.rfind('\n', iRelPos);
			if (iStart < 0)
				iStart = 0;
			else
				iStart++;

			int iEnd = sTextLine.find('\n', iRelPos);
			if (iEnd < 0)
				iEnd = sTextLine.size()-1;
			else
				iEnd--;

			csText << csStr << "<code><b>" << sTextLine.substr(iStart, iRelPos-iStart+1).c_str();
			csText << "@" << sTextLine.substr(iRelPos+1, iEnd-iRelPos+1).c_str() << "</b></code><br>";
		}
	}

	return csText;
}


//////////////////////////////////////////////////////////////////////
/// Get Code Error Position

bool CCLUParse::GetCodeErrorPos(int& iLine, int& iPos, int& iInputPos, cStr &csFilename)
{
	cStr csLine, csError, csStr = "", csText, csLevel;
	int iMsgCount;
	
	iLine = -1;
	iPos = -1;

	CCodeErrorList &rErrList = m_CodeBase.m_ErrorList;

	if (m_msText.Count() == 0)
		return false;

	if ((iMsgCount = rErrList.Count()) <= 0)
		return false;

	SMsg& rMsg = rErrList[iMsgCount-1];
	
	iLine = GetTextLineNo(rMsg.iLine) + 1;
	iPos = rMsg.iPos + 1;

	STextLine& rLine = m_msText[rMsg.iLine];
//	if (rLine.iMainFilePos < 0)
//	{
		if (rMsg.iPos >= int(rLine.vecPos.size()) )
			iInputPos = rLine.vecPos.back();
		else
			iInputPos = rLine.vecPos[rMsg.iPos];
//	}
//	else
//	{
		//iInputPos = rLine.iMainFilePos;
//	}

	if (rLine.csFilename.Len())
		csFilename = rLine.csFilename;
	else
	{
		csFilename = m_sScriptName.c_str();
	}

	if (!strchr(csFilename.Str(), '\\') &&
		!strchr(csFilename.Str(), '/') &&
		!strchr(csFilename.Str(), ':'))
	{
		csFilename = m_sScriptPath.c_str() + csFilename;
	}


	return true;
}


//////////////////////////////////////////////////////////////////////
/// Print Vars

cStr CCLUParse::PrintVars()
{
	cStr csText;

	int iVar, iVarCount = m_CodeBase.VarCount();

	for(iVar=0;iVar<iVarCount;iVar++)
	{
		CCodeVar& rVar = m_CodeBase.GetVar(iVar);

		csText << rVar.Name() << ", "; 
		csText << rVar.TypeStr() << ", ";
		csText << rVar.ValStr() << "\n";
	}

	return csText;
}

//////////////////////////////////////////////////////////////////////
/// Print Stack

cStr CCLUParse::PrintStack()
{
	cStr csText;

	int iVar, iVarCount = m_CodeBase.StackCount();

	for(iVar=0;iVar<iVarCount;iVar++)
	{
		CCodeVar* pVar = m_CodeBase.GetStackVar(iVar);

		csText << "<b>" << iVar << "</b>: ";
		if (pVar->IsTypeValid())
		{
			csText << pVar->Name() << ", "; 
			csText << pVar->TypeStr() << ", ";
			csText << pVar->ValStr() << "<br>";
		}
		else
		{
			csText << "<b>Uninitialized variable</b><br>";
		}
	}

	return csText;
}


//////////////////////////////////////////////////////////////////////
// Run Code starting at line iStartLine for iLineCount lines.
// if iStartLine == 0 then start at beginning.
// if iLineCount == -1 then run all lines after iStartLine.

bool CCLUParse::RunCode(int iStartLine, int iLineCount)
{	
	// Reset Environment variables like font type and scale, etc.
	m_CodeBase.ResetEnvVars();

	//m_CodeBase.DrawScriptFileIndex();

	bool bRet = CParse::RunCode(iStartLine, iLineCount);

	// Clean up things the user might have forgotten.
	m_CodeBase.CleanUp();

	return bRet;
}


// Insert Text pcText at position iPos and parse it if bParse is true.
// If iPos == -1 then add text to end.
// Returns number of lines read. Returns -1 if error occured.

int CCLUParse::InsertText(const char *pcText, int iPos, bool bParse)
{
	m_vecIncludeFilename.resize(1);
	m_vecIncludeFilename[0] = m_sScriptName + ".clu";

	return CParse::InsertText(pcText, iPos, bParse);
}


//////////////////////////////////////////////////////////////////////
/// PreParse Text. Called by InsertText
/// Insert text separated into lines before iPos.
/// If iPos == -1 then lines are added to end.
/// Separate Text into lines using m_cSepSym as separation symbol.

int CCLUParse::PreParse(const char* _pcText, int iPos, 
						const char *pcFilename, int iMainFilePos)
{
	int iLineCount = m_msText.Count();
	int iAddedLines = 0;
	int iCurLine; // Line Text is to be inserted before.
	int iTextLine;
	int iCommentPos = 0;
	int iOrigTextPos = 0;
	int iOrigTextStartPos = 0;
	const int ciMaxLineLength = 2048;

	cStr csFilename;
	ifstream zFile;
	string sVal;
	char pcLine[500], pcCurPath[500];
	char *pcInputText;

	if (pcFilename)
		csFilename = pcFilename;
	else
		csFilename = "";

	_getcwd(pcCurPath, 499);
	_chdir(m_sScriptPath.c_str());

	//printf("CParseBase:InsertText: Start\n");
	if (!_pcText) return -1;

	int iLen = strlen(_pcText);
	if (!iLen) return -1;

	pcInputText = new char[iLen + 1];
	if (!pcInputText) return -1;

	strcpy(pcInputText, _pcText);

	char *pcText = new char[ciMaxLineLength];
	if (!pcText) return -1;

	// Make pcOrigText of same size as script, in case the whole
	// script is one big comment of type /* */.
	char *pcOrigText = new char[iLen + 1];
	if (!pcOrigText) return -1;

	char *pcCommentLine = new char[ciMaxLineLength];
	if (!pcCommentLine) return -1;

	if (iPos < 0)
		iCurLine = iLineCount;
	else
		iCurLine = iPos;

	int i, iLinePos, iBracketCount, iSqBraCount;
	int iInputPos, iInputPosAdj;
	unsigned char ucSym, ucPrevSym;
	bool bGotoReturn = false;		// if true then goto next return
	bool bGotoEOC = false;			// if true goto end of comment '*/'
	bool bInString = false;			// Assumes string is enclosed by ""
	bool bInIfCond = false;			// We are in IF condition, looking for closing bracket
	bool bStopAfterLine = false;

	m_sErrorFilename = "";
	m_sIncludeFilename = "";

	vector<int> vecFuncRBC, vecInputPos;

	iSqBraCount = 0;

	iInputPos = -1;
	iLinePos = 0;
	iTextLine = 0;
	ucSym = 0;

	for(i=0;i<iLen;i++)
	{
		// Check whether current line is too long
		if (iLinePos >= ciMaxLineLength - 1)
		{
			delete[] pcText;
			delete[] pcInputText;
			delete[] pcOrigText;
			delete[] pcCommentLine;

			return -1;
		}

		iInputPosAdj = 0;
		ucPrevSym = ucSym;
		ucSym = (unsigned char) pcInputText[i];

		pcOrigText[iOrigTextPos++] = ucSym;
		iInputPos++;

		if (ucSym == 10)
		{
			iTextLine++;
		
			if (bStopAfterLine)
				break;
		}
		//if (ucSym == 10 || ucSym == 13)
		//{
		//	if (ucPrevSym != 10 && ucPrevSym != 13)
		//		iTextLine++;
		//}

		if (bGotoReturn)
		{
			if (ucSym != 10 && ucSym != 13)
			{
				pcCommentLine[iCommentPos++] = ucSym;
				continue;
			}
			else
			{
				pcCommentLine[iCommentPos] = 0;
				bGotoReturn = false;

				// Check for PreParse Statements.
				// They need to start with a '#'.
				if (pcCommentLine[0] == '#')
				{
					char *pcVal, *pcVal2;
					// Check whether preprocessor statement is 'include'
					if (strstr(pcCommentLine, "include"))
					{
						// Look for left quotations
						if ((pcVal = strchr(pcCommentLine, '\"')))
						{
							pcVal++;
							// Look for right quotations
							if ((pcVal2 = strchr(pcVal, '\"')))
							{
								vector<string>::iterator it_El;
								*pcVal2 = 0;
								// Open Include File
								m_sIncludeFilename = pcVal;
								it_El = find(m_vecIncludeFilename.begin(), 
												m_vecIncludeFilename.end(), 
												m_sIncludeFilename);

								if (it_El != m_vecIncludeFilename.end())
								{
									SetError(CPB_RECURSIVE_INCLUDEFILE);

									delete pcInputText;
									delete pcText;
									delete pcOrigText;
									delete pcCommentLine;
									
									if (pcFilename)
									{
										m_sErrorFilename = pcFilename;
										m_sErrorPath = m_sScriptPath;
									}
									else
									{
										m_sErrorFilename = "";
										m_sErrorPath = "";
									}

									if (iMainFilePos < 0)
										return -(i-iCommentPos)-2;
									else
										return -iMainFilePos-2;
								}

								m_vecIncludeFilename.push_back(m_sIncludeFilename);

								sVal = "";
								zFile.clear();
								zFile.open(pcVal);
								if (zFile.is_open())
								{
									while(!zFile.eof())
									{
										zFile.getline(pcLine, 499);
										sVal += pcLine;
										sVal += "\n";
									}
									zFile.close();
								}
								else
								{
									SetError(CPB_NO_INCLUDEFILE);

									delete pcInputText;
									delete pcText;
									delete pcOrigText;
									delete pcCommentLine;
									
									if (pcFilename)
									{
										m_sErrorFilename = pcFilename;
										m_sErrorPath = m_sScriptPath;
									}
									else
									{
										m_sErrorFilename = "";
										m_sErrorPath = "";
									}

									if (iMainFilePos < 0)
										return -(i-iCommentPos)-2;
									else
										return -iMainFilePos-2;
								}

								//int iOldTextLen = strlen(&pcInputText[i]);
								int iNewTextLen = sVal.size();

								// If there is text to be included
								if (iNewTextLen)
								{
									int iIncludeLineCount = PreParse(sVal.c_str(), iCurLine, 
										pcVal, (iMainFilePos < 0 ? (i-iCommentPos) : iMainFilePos));

									m_vecIncludeFilename.pop_back();

									if (iIncludeLineCount < 0)
									{
										delete[] pcText;
										delete[] pcInputText;
										delete[] pcOrigText;
										delete[] pcCommentLine;

										return iIncludeLineCount;
									}

									iCurLine += iIncludeLineCount;
									iAddedLines += iIncludeLineCount;
									continue;
								}
							}
						}
					}
				}
			}
		}

		if (bGotoEOC)
		{
			if (ucSym == '*' && pcInputText[i+1] == '/')
			{
				pcOrigText[iOrigTextPos++] = pcInputText[i+1];
				iInputPos++;
				i++;
				bGotoEOC = false;
			}
			continue;
		}

		if (!bInString && (ucSym < 32 || ucSym == 255) && ucSym != '\t')
			continue;

		if (ucSym == '\"' &&
			(ucPrevSym != '\\' ||
			 (i > 1 && ucPrevSym == '\\' && pcInputText[i-2] == '\\')))
		{
			if (bInString)
				bInString = false;
			else
				bInString = true;
		}
/*
		if (bInString)
		{
			if (i+1 < iLen && ucSym == '\\' && pcInputText[i+1] == '\\')
			{
				pcOrigText[iOrigTextPos++] = pcInputText[i+1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
		}
		else
*/
		if (!bInString)
		{
			// Check for comments
			if (ucSym == '/' && pcInputText[i+1] == '/')
			{
				iCommentPos = 0;
				bGotoReturn = true;
				pcOrigText[iOrigTextPos++] = pcInputText[i+1];
				iInputPos++;
				i++;
				continue;
			}

			if (ucSym == '/' && pcInputText[i+1] == '*')
			{
				bGotoEOC = true;
				pcOrigText[iOrigTextPos++] = pcInputText[i+1];
				iInputPos++;
				i++;
				continue;
			}

			if (ucSym == m_cLineSepSym || ucSym == '}')
			{
				if (bInIfCond || iSqBraCount > 0)
				{
					SetError(CPB_MISSING_IFCOND);
					bStopAfterLine = true;
					m_iCurLine = iCurLine;
					m_iCurPos = iLinePos;

					break;
//					return -i-2;
				}

				if (ucSym == '}')
				{
					pcText[iLinePos++] = '}';
					vecInputPos.push_back(iInputPos);
				}

				pcText[iLinePos] = 0;
				pcOrigText[iOrigTextPos] = 0;

				m_msText.Insert(iCurLine);
				m_msText[iCurLine].csText = pcText;
				m_msText[iCurLine].csInputText = pcOrigText;
				m_msText[iCurLine].csFilename = csFilename;
				m_msText[iCurLine].iMainFilePos = iMainFilePos;
				m_msText[iCurLine].iInputTextStartPos = iOrigTextStartPos;
				m_msText[iCurLine].iLine = iTextLine;
				m_msText[iCurLine].vecPos = vecInputPos;

				iOrigTextStartPos += iOrigTextPos;
				iOrigTextPos = 0;
				iLinePos = 0;
				iCurLine++;
				iAddedLines++;
				vecInputPos.erase(vecInputPos.begin(), vecInputPos.end());

				continue;
			}

			if (bInIfCond)
			{
				if (ucSym == '(')
				{
					iBracketCount++;
				}
				else if (ucSym == ')')
				{
					// Is If clause finished here?
					if (--iBracketCount == 0)
					{	
						bInIfCond = false;
						pcText[iLinePos++] = ')';
						vecInputPos.push_back(iInputPos);

						pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
						vecInputPos.push_back(iInputPos);

						// Set THEN ID-symbol
						ucSym = OC_IDSYM_THEN_CHAR;
					}
				}
			}

			if (i+2 < iLen && 
				ucSym == 'i' && 
				pcInputText[i+1] == 'f' && 
				strchr(m_ocLabel.m_csIDSymbolSpace.Str(), pcInputText[i+2]) == 0)
			{
				// Are we already in if condition?
				if (bInIfCond)
				{
					SetError(CPB_MISSING_IFCOND);
					bStopAfterLine = true;
					m_iCurLine = iCurLine;
					m_iCurPos = iLinePos;

					break;
					//return -i-2;
				}

				// Set IF ID-symbol
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);
				pcText[iLinePos++] = OC_IDSYM_IF_CHAR;
				vecInputPos.push_back(iInputPos);

				// Look for (
				while (i < iLen && pcInputText[i] != '(')
				{
					i++;
					pcOrigText[iOrigTextPos++] = pcInputText[i];
					iInputPos++;
				}

				if (i == iLen)
				{
					SetError(CPB_MISSING_IFCOND);
					bStopAfterLine = true;
					m_iCurLine = iCurLine;
					m_iCurPos = iLinePos;

					break;
					//return -i-2;
				}

				// Initialize bracket count
				iBracketCount = 1;
				bInIfCond = true;
				ucSym = pcInputText[i];
			}
			else if (i+4 < iLen && ucSym == 'e' && 
					 pcInputText[i+1] == 'l' && 
					 pcInputText[i+2] == 's' && 
					 pcInputText[i+3] == 'e' && 
					 strchr(m_ocLabel.m_csIDSymbolSpace.Str(), pcInputText[i+4]) == 0)
			{
				// Set ELSE ID-symbol
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym = OC_IDSYM_ELSE_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i+1];
				pcOrigText[iOrigTextPos++] = pcInputText[i+2];
				pcOrigText[iOrigTextPos++] = pcInputText[i+3];
				i += 3;
				iInputPos += 3;
			}
			else if (i+4 < iLen && ucSym == 'l' && 
					 pcInputText[i+1] == 'o' && 
					 pcInputText[i+2] == 'o' && 
					 pcInputText[i+3] == 'p' && 
					 strchr(m_ocLabel.m_csIDSymbolSpace.Str(), pcInputText[i+4]) == 0)
			{
				// Set LOOP ID-symbol
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym = OC_IDSYM_LOOP_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i+1];
				pcOrigText[iOrigTextPos++] = pcInputText[i+2];
				pcOrigText[iOrigTextPos++] = pcInputText[i+3];
				i += 3;
				iInputPos += 3;
			}
			else if (i+5 < iLen && ucSym == 'b' && 
					 pcInputText[i+1] == 'r' && 
					 pcInputText[i+2] == 'e' && 
					 pcInputText[i+3] == 'a' && 
					 pcInputText[i+4] == 'k' && 
					 strchr(m_ocLabel.m_csIDSymbolSpace.Str(), pcInputText[i+5]) == 0)
			{
				// Set BREAK ID-symbol
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym = OC_IDSYM_BREAK_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i+1];
				pcOrigText[iOrigTextPos++] = pcInputText[i+2];
				pcOrigText[iOrigTextPos++] = pcInputText[i+3];
				pcOrigText[iOrigTextPos++] = pcInputText[i+4];
				i += 4;
				iInputPos += 4;
			}
			else if (ucSym == '(' && 
						(strchr(m_ocLabel.m_csIDSymbolSpace.Str(), pcInputText[i-1]) != 0 ||
						 strchr("1234567890", pcInputText[i-1]) != 0 ||
						 pcInputText[i-1] == ')' ||
						 pcInputText[i-1] == ']' ||
						 pcInputText[i-1] == '"') )
			{
				// Function call or component extraction of matrix or list, etc.
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				pcText[iLinePos++] = OC_IDSYM_CALL_CHAR;
				vecInputPos.push_back(iInputPos);
				vecFuncRBC.push_back(1);
			}
			else if (ucSym == '(')
			{
				int iSize = vecFuncRBC.size();
				if (iSize > 0)
					++vecFuncRBC[iSize-1];
			}
			else if (ucSym == ')')
			{
				int iSize = vecFuncRBC.size();
				if (iSize > 0)
				{
					if (--vecFuncRBC[iSize-1] == 0)
					{
						vecFuncRBC.pop_back();
					}
				}
			}
			else if (ucSym == ',')
			{
				int iSize = vecFuncRBC.size();
				if ((iSize > 0 && vecFuncRBC[iSize-1] != 1 && iSqBraCount == 0) ||
					(iSize == 0 && iSqBraCount == 0))
				{	
					SetError(CPB_UNEXPSYMCOMMA);
					m_iCurLine = iCurLine;
					m_iCurPos = iLinePos;
	
					bStopAfterLine = true;
				}
			}
			else if (ucSym == '[')
			{
				iSqBraCount++;
			}
			else if (ucSym == ']')
			{
				if (iSqBraCount == 0)
				{
					SetError(CPB_UNEXPECTEDSYM);
					m_iCurLine = iCurLine;
					m_iCurPos = iLinePos;
					bStopAfterLine = true;
				}
				else
				{
					iSqBraCount--;
				}
			}
			else if (i+1 < iLen && ucSym == '^' && pcInputText[i+1] == '^')
			{
				// Set POWER operator ID-symbol
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym = OC_IDSYM_POW_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i+1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			else if (i+1 < iLen && ucSym == '=' && pcInputText[i+1] == '=')
			{
				// Set EQUAL operator ID-symbol
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym = OC_IDSYM_EQUAL_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i+1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			else if (i+1 < iLen && ucSym == '!' && pcInputText[i+1] == '=')
			{
				// Set NOT EQUAL operator ID-symbol
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym = OC_IDSYM_NOTEQUAL_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i+1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			else if (i+1 < iLen && ucSym == '&' && pcInputText[i+1] == '&')
			{
				// Set AND operator ID-symbol
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym = OC_IDSYM_AND_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i+1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			else if (i+1 < iLen && ucSym == '|' && pcInputText[i+1] == '|')
			{
				// Set OR operator ID-symbol
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym = OC_IDSYM_OR_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i+1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			else if (i+1 < iLen && ucSym == '!' && pcInputText[i+1] == '!')
			{
				// Set NOT operator ID-symbol
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym = OC_IDSYM_NOT_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i+1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			else if (i+1 < iLen && ucSym == '<' && pcInputText[i+1] == '=')
			{
				// Set Less Equal operator ID-symbol
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym = OC_IDSYM_LE_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i+1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			else if (i+1 < iLen && ucSym == '>' && pcInputText[i+1] == '=')
			{
				// Set Greater Equal operator ID-symbol
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym = OC_IDSYM_GE_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i+1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			else if (i+1 < iLen && ucSym == '<' && pcInputText[i+1] == '<')
			{
				// Set Left Shift operator ID-symbol
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos-1);

				ucSym = OC_IDSYM_LSHIFT_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i+1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			else if (i+1 < iLen && ucSym == '-' && pcInputText[i+1] == '>')
			{
				// Set Reference Symbol ID
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym = OC_IDSYM_REF_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i+1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			else if (i+1 < iLen && ucSym == ':' && pcInputText[i+1] == ':')
			{
				// Set Reference Symbol ID
				ucSym = OC_IDSYM_GLOBAL_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i+1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			else if (i+2 < iLen && ucSym == '.' && pcInputText[i+1] == '&' && 
									pcInputText[i+2] == '&')
			{
				// Set Point AND Symbol ID
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym = OC_IDSYM_PAND_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i+1];
				pcOrigText[iOrigTextPos++] = pcInputText[i+2];
				i += 2;
				iInputPos += 2;
				iInputPosAdj = -2;
			}
			else if (i+2 < iLen && ucSym == '.' && pcInputText[i+1] == '|' && 
									pcInputText[i+2] == '|')
			{
				// Set Point OR Symbol ID
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym = OC_IDSYM_POR_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i+1];
				pcOrigText[iOrigTextPos++] = pcInputText[i+2];
				i += 2;
				iInputPos += 2;
				iInputPosAdj = -2;
			}
			else if (i+2 < iLen && ucSym == '.' && pcInputText[i+1] == '=' && 
									pcInputText[i+2] == '=')
			{
				// Set Point EQUAL Symbol ID
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym = OC_IDSYM_PEQUAL_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i+1];
				pcOrigText[iOrigTextPos++] = pcInputText[i+2];
				i += 2;
				iInputPos += 2;
				iInputPosAdj = -2;
			}
			else if (i+2 < iLen && ucSym == '.' && pcInputText[i+1] == '!' && 
									pcInputText[i+2] == '=')
			{
				// Set Point NOT EQUAL Symbol ID
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym = OC_IDSYM_PNEQUAL_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i+1];
				pcOrigText[iOrigTextPos++] = pcInputText[i+2];
				i += 2;
				iInputPos += 2;
				iInputPosAdj = -2;
			}
			else if (i+2 < iLen && ucSym == '.' && pcInputText[i+1] == '<' && 
									pcInputText[i+2] == '=')
			{
				// Set Point LESS EQUAL Symbol ID
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym = OC_IDSYM_PLTE_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i+1];
				pcOrigText[iOrigTextPos++] = pcInputText[i+2];
				i += 2;
				iInputPos += 2;
				iInputPosAdj = -2;
			}
			else if (i+2 < iLen && ucSym == '.' && pcInputText[i+1] == '>' && 
									pcInputText[i+2] == '=')
			{
				// Set Point GREATER EQUAL Symbol ID
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym = OC_IDSYM_PGTE_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i+1];
				pcOrigText[iOrigTextPos++] = pcInputText[i+2];
				i += 2;
				iInputPos += 2;
				iInputPosAdj = -2;
			}
			else if (i+2 < iLen && ucSym == '.' && pcInputText[i+1] == '<' && 
									pcInputText[i+2] == '<')
			{
				// Set Point LShift Symbol ID
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym = OC_IDSYM_PLSHIFT_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i+1];
				pcOrigText[iOrigTextPos++] = pcInputText[i+2];
				i += 2;
				iInputPos += 2;
				iInputPosAdj = -2;
			}
			else if (i+1 < iLen && ucSym == '.' && pcInputText[i+1] == '/')
			{
				// Set Point Div Symbol ID
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym = OC_IDSYM_PDIV_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i+1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			else if (i+1 < iLen && ucSym == '.' && pcInputText[i+1] == '*')
			{
				// Set Point Mul Symbol ID
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym = OC_IDSYM_PMUL_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i+1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			else if (i+1 < iLen && ucSym == '.' && pcInputText[i+1] == '.')
			{
				// Set Point Mul Symbol ID
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym = OC_IDSYM_PIP_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i+1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			else if (i+1 < iLen && ucSym == '.' && pcInputText[i+1] == '^')
			{
				// Set Point Mul Symbol ID
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym = OC_IDSYM_POP_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i+1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			else if (i+1 < iLen && ucSym == '.' && pcInputText[i+1] == '-')
			{
				// Set Point Mul Symbol ID
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym = OC_IDSYM_PMINUS_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i+1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			else if (i+1 < iLen && ucSym == '.' && pcInputText[i+1] == '+')
			{
				// Set Point Add Symbol ID
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym = OC_IDSYM_PPLUS_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i+1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			else if (i+1 < iLen && ucSym == '.' && pcInputText[i+1] == '>')
			{
				// Set Point GREATER Symbol ID
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym = OC_IDSYM_PGT_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i+1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			else if (i+1 < iLen && ucSym == '.' && pcInputText[i+1] == '<')
			{
				// Set Point LESS Symbol ID
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym = OC_IDSYM_PLT_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i+1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			// Replace e.g. ' .1' by ' 0.1'
			else if (i+1 < iLen && ucSym == '.' && 
				     strchr("1234567890", pcInputText[i+1]) != 0)
			{
				if (i == 0 ||
					( strchr("1234567890", pcInputText[i-1]) == 0 &&
					  strchr(m_ocLabel.m_csIDSymbolSpace.Str(), pcInputText[i-1]) == 0 ))
				{
					pcText[iLinePos++] = '0';
					vecInputPos.push_back(iInputPos);
				}
			}
		}

		pcText[iLinePos++] = (char) ucSym;
		vecInputPos.push_back(iInputPos + iInputPosAdj);
 	}

	if (iLinePos > 0)
	{
		pcText[iLinePos] = 0;
		pcOrigText[iOrigTextPos] = 0;
		
		m_msText.Insert(iCurLine);
		m_msText[iCurLine].csText = pcText;
		m_msText[iCurLine].csInputText = pcOrigText;
		m_msText[iCurLine].csFilename = csFilename;
		m_msText[iCurLine].iMainFilePos = iMainFilePos;
		m_msText[iCurLine].iInputTextStartPos = iOrigTextStartPos;
		m_msText[iCurLine].iLine = iTextLine;
		m_msText[iCurLine].vecPos = vecInputPos;

		iOrigTextStartPos += iOrigTextPos;
		iOrigTextPos = 0;
		
		iLinePos = 0;
		iCurLine++;
		iAddedLines++;
	}

	//printf("CParseBase:InsertText: End\n");

	_chdir(pcCurPath);

	delete[] pcText;
	delete[] pcInputText;
	delete[] pcOrigText;
	delete[] pcCommentLine;

	if (bStopAfterLine)
		return -1;

	return iAddedLines;
}


