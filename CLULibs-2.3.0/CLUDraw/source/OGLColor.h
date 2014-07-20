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


// OGLColor.h: Schnittstelle für die Klasse COGLColor.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLCOLOR_H__ECC38C81_0DD0_4C4F_B77D_86F72B5AA0DE__INCLUDED_)
#define AFX_OGLCOLOR_H__ECC38C81_0DD0_4C4F_B77D_86F72B5AA0DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "OGLBaseElement.h"
class COGLColor  : public COGLBaseElement
{
public:
	COGLColor();
	COGLColor(const COGLColor& rCol);

	COGLColor& operator= (const COGLColor& rCol);
	COGLColor& operator= (const float *pfCol);

	void Set(float fR, float fG, float fB, float fA = 1.0f)
	{ m_pfCol[0] = fR; m_pfCol[1] = fG; m_pfCol[2] = fB; m_pfCol[3] = fA; }

	void Alpha(float fA) 
	{ m_pfCol[3] = fA; }

	float Alpha() const
	{ return m_pfCol[3]; }

	bool operator== (const COGLColor& rCol)
	{ 
		if (m_pfCol[0] == rCol.m_pfCol[0] &&
			m_pfCol[1] == rCol.m_pfCol[1] &&
			m_pfCol[2] == rCol.m_pfCol[2])
			return true;
		return false;
	}

	friend COGLColor operator* (float fVal, const COGLColor &rCol);
	friend COGLColor operator+ (const COGLColor& rCol1, const COGLColor& rCol2);
	friend COGLColor operator- (const COGLColor& rCol1, const COGLColor& rCol2);

	COGLColor& operator+= (const COGLColor& rCol);
	COGLColor& operator-= (const COGLColor& rCol);

	COGLColor& operator/= (float fVal);

	float operator[] (int iCol)
	{ 
		if (iCol >= 0 && iCol <= 3) return m_pfCol[iCol];
		else return 0.0f;
	}

	void Renorm();
	void Clamp();

	bool Apply(int iMode = 0, void *pvData = 0);

	float* Data() { return m_pfCol; }
	const float* Data() const { return m_pfCol; }

protected:
	float m_pfCol[4];
};

#endif // !defined(AFX_OGLCOLOR_H__ECC38C81_0DD0_4C4F_B77D_86F72B5AA0DE__INCLUDED_)
