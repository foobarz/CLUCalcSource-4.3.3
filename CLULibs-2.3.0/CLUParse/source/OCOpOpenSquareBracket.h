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

//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCOPOPENSQUAREBRACKET_H__33EE4C70_DDB1_446C_AE35_F85D0F0D391B__INCLUDED_)
#define AFX_OCOPOPENSQUAREBRACKET_H__33EE4C70_DDB1_446C_AE35_F85D0F0D391B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ObjectClass.h"
#include "CodeCreateVarList.h"

class COCOpOpenSquareBracket : public CObjectClass
{
public:
	COCOpOpenSquareBracket();
	virtual ~COCOpOpenSquareBracket();

	virtual bool Parse(CParseBase& rParse, CCodeElementList& rCode);

protected:
	CCodeCreateVarList *m_pCreateVarList;

};

#endif // !defined(AFX_OCOPOPENSQUAREBRACKET_H__33EE4C70_DDB1_446C_AE35_F85D0F0D391B__INCLUDED_)
