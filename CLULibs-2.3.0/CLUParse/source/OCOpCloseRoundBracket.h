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

// OCOpCloseRoundBracket.h: Schnittstelle für die Klasse COCOpCloseRoundBracket.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCOPCLOSEROUNDBRACKET_H__237A6855_4D4B_4672_B7D4_45F4F579CB6D__INCLUDED_)
#define AFX_OCOPCLOSEROUNDBRACKET_H__237A6855_4D4B_4672_B7D4_45F4F579CB6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ObjectClass.h"

class COCOpCloseRoundBracket : public CObjectClass
{
public:
	COCOpCloseRoundBracket();
	virtual ~COCOpCloseRoundBracket();

	virtual bool Parse(CParseBase& rParse, CCodeElementList& rCode);

};

#endif // !defined(AFX_OCOPCLOSEROUNDBRACKET_H__237A6855_4D4B_4672_B7D4_45F4F579CB6D__INCLUDED_)
