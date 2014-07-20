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

// OGLFilterE3GA.h: Schnittstelle für die Klasse COGLFilterE3GA.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLFILTERE3GA_H__2557741E_7C0C_48B2_9343_592EAF6B11C0__INCLUDED_)
#define AFX_OGLFILTERE3GA_H__2557741E_7C0C_48B2_9343_592EAF6B11C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OGLMVFilterBase.h"
#include "OGLVexListE3.h"


#define E3M_DRAW_VEC_AS_LINE	(ID_E3GA << 16) + 0x0100
#define E3M_DRAW_VEC_AS_ARROW	(ID_E3GA << 16) + 0x0101
#define E3M_DRAW_VEC_AS_POINT	(ID_E3GA << 16) + 0x0102
#define E3M_DRAW_VEC_AS_STD		E3M_DRAW_VEC_AS_LINE

#define E3M_DRAW_PLANE_AS_RECT	(ID_E3GA << 16) + 0x0200
#define E3M_DRAW_PLANE_AS_DISK	(ID_E3GA << 16) + 0x0201
#define E3M_DRAW_PLANE_AS_STD	E3M_DRAW_PLANE_AS_DISK


class COGLFilterE3 : public COGLMVFilterBase  
{
public:
	enum TDrawVecType
	{
		StdVec = 0, Line = 0, Arrow, Point
	};

	enum TDrawPlaneType
	{
		StdPlane = 0, Disk = 0, Rectangle
	};

public:
	COGLFilterE3();
	virtual ~COGLFilterE3();

	virtual bool SetMode(int iMode);
	virtual bool ResetModes();

	virtual bool DrawMV(const MultiV<float>& vA, bool bAnalyzeOnly = false);
	virtual bool DrawMV(const MultiV<double>& vA, bool bAnalyzeOnly = false);

	bool DrawMVInfo(CMVInfo<float>& rInfo);

protected:
	bool m_bDrawOPNS;
	TDrawVecType m_eDrawVecType;
	TDrawPlaneType m_eDrawPlaneType;

	COGLVexListE3 m_VexList;
};

#endif // !defined(AFX_OGLFILTERE3GA_H__2557741E_7C0C_48B2_9343_592EAF6B11C0__INCLUDED_)
