/// LICENSE START
////////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the CLUDraw Library.
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


// OGLVexListN3.h: Schnittstelle für die Klasse COGLVexListN3.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLVEXLISTN3_H__DC24CFDC_277E_4483_9AFC_D41068494DE9__INCLUDED_)
#define AFX_OGLVEXLISTN3_H__DC24CFDC_277E_4483_9AFC_D41068494DE9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OGLVertexList.h"

class COGLVexListN3 : public COGLVertexList  
{
public:
	COGLVexListN3();
	virtual ~COGLVexListN3();

	COGLVertexList& operator<< (const MultiV<float>& vA)
	{ AddMV(vA); return *this; }

	bool AddMV(const MultiV<float>& vA)
	{ return AddVex(&vA[1]); }

};

#endif // !defined(AFX_OGLVEXLISTN3_H__DC24CFDC_277E_4483_9AFC_D41068494DE9__INCLUDED_)
