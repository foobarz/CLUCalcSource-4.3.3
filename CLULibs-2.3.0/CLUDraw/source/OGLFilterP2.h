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


// OGLFilterP2.h: Schnittstelle für die Klasse COGLFilterP2.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLFILTERP2_H__FC9B6027_9BD4_4328_A631_60D53503E919__INCLUDED_)
#define AFX_OGLFILTERP2_H__FC9B6027_9BD4_4328_A631_60D53503E919__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OGLMVFilterBase.h"
#include "OGLVexListP2.h"

class COGLFilterP2 : public COGLMVFilterBase  
{
public:
	COGLFilterP2();
	virtual ~COGLFilterP2();

	virtual bool DrawMV(const MultiV<double>& vA, bool bAnalyzeOnly = false) { return false; }
	virtual bool DrawMV(const MultiV<float>& vA, bool bAnalyzeOnly = false);

	bool DrawE2Plane(float fSize, const COGLColor& rCol);
	MultiV<float> Rotor(const MultiV<float>& vA, float fRad, bool bAxis = true);

	bool DrawBlade(const MultiV<float>& vA);
	bool DrawRotor(const MultiV<float>& vA);

protected:

	COGLVexListP2 m_VexList;
};

#endif // !defined(AFX_OGLFILTERP2_H__FC9B6027_9BD4_4328_A631_60D53503E919__INCLUDED_)
