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

// OCObjSepSym.cpp: Implementierung der Klasse COCObjSepSym.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OCObjSepSym.h"
#include "ParseBase.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COCObjSepSym::COCObjSepSym()
{
	m_csIDSymbolSpace = " \t";
}

COCObjSepSym::~COCObjSepSym()
{

}


//////////////////////////////////////////////////////////////////////
/// Parse
///
/// Object separation symbol is typically ' '.
/// Simply advance a single step and return.

bool COCObjSepSym::Parse(CParseBase& rParse, CCodeElementList& rCodeList)
{
	rParse.AddToCurPos(1);

	return true;
}

