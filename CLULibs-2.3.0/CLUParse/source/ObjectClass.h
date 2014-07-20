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

// ObjectClass.h: Schnittstelle für die Klasse CObjectClass.
//
//	Base class for all parse classes.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECTCLASS_H__30982795_F6BD_4D30_A056_1D7F60A882BD__INCLUDED_)
#define AFX_OBJECTCLASS_H__30982795_F6BD_4D30_A056_1D7F60A882BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CParseBase;
class CCodeElementList;

class CObjectClass  
{
public:
	CObjectClass();
	virtual ~CObjectClass();

	virtual bool Parse(CParseBase& rParse, CCodeElementList& rCode) = 0;

	// Subspace of Symbols that identify Object class.
	// That is a text element is of this object class if its first
	// symbol is contained in m_csIDSymbolSpace. 
	// Between all object classes there has to be a unique identification possible.
	cStr m_csIDSymbolSpace; 
};

#endif // !defined(AFX_OBJECTCLASS_H__30982795_F6BD_4D30_A056_1D7F60A882BD__INCLUDED_)
