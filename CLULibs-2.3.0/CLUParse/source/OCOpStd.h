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

// OCOpStd.h: Schnittstelle für die Klasse COCOpStd.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCOPSTD_H__19593133_8CC9_4FD2_98B9_FDE6822C5E96__INCLUDED_)
#define AFX_OCOPSTD_H__19593133_8CC9_4FD2_98B9_FDE6822C5E96__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OCOperator.h"
#include "CodeOperator.h"

class COCOpStd : public COCOperator  
{
public:
	COCOpStd();
	virtual ~COCOpStd();

	bool Init(const char *pcSym, int iOpLevelUnary, int iOpLevelBinary, 
			  bool bRightToLeft,  bool bVoidOp, bool bRecurseLists,
			  TOpUnaryPtr UFunc, TOpBinaryPtr BFunc,
			  TOpAUnaryPtr AUFunc, TOpABinaryPtr ABFunc);

protected:
	CCodeOperator m_Operator;
};

#endif // !defined(AFX_OCOPSTD_H__19593133_8CC9_4FD2_98B9_FDE6822C5E96__INCLUDED_)
