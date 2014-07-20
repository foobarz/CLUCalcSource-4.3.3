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


// OGLVertex.h: Schnittstelle für die Klasse COGLVertex.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLVERTEX_H__B634C808_40D5_4BE1_8E64_8AC4B1B21B65__INCLUDED_)
#define AFX_OGLVERTEX_H__B634C808_40D5_4BE1_8E64_8AC4B1B21B65__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <math.h>

class COGLVertex  
{
public:
	COGLVertex();
	COGLVertex(const COGLVertex& rVex);

	operator float*() { return m_pfVex; }
	operator const float*() const { return m_pfVex; }
	float& operator[] (int i) { if (i >= 0 && i < 3) return m_pfVex[i]; else return m_pfVex[0]; }

	float Mag() const
	{ return (float) sqrt(m_pfVex[0]*m_pfVex[0] + m_pfVex[1]*m_pfVex[1] + m_pfVex[2]*m_pfVex[2]); }

	// Check that vertex coords. are within allowed bounds.
	COGLVertex& Clamp();

	// Normalize vector
	COGLVertex& Norm()
	{ float fMag = Mag(); if (fMag != 0.0f) *this /= fMag; return *this;}

	COGLVertex& operator= (const COGLVertex& rVex)
	{ memcpy(m_pfVex, rVex.m_pfVex, 3*sizeof(float)); return *this; }

	COGLVertex& operator= (const float* pfVal)
	{ memcpy(m_pfVex, pfVal, 3*sizeof(float)); return *this; }
	
	void Set(float fX, float fY, float fZ)
	{ m_pfVex[0] = fX; m_pfVex[1] = fY; m_pfVex[2] = fZ; }

	COGLVertex& operator-= (const float fVal)
	{ for(int i=0;i<3;i++) m_pfVex[i] -= fVal; return *this; }

	COGLVertex& operator+= (const float fVal)
	{ for(int i=0;i<3;i++) m_pfVex[i] += fVal; return *this; }

	COGLVertex& operator*= (const float fVal)
	{ for(int i=0;i<3;i++) m_pfVex[i] *= fVal; return *this; }

	COGLVertex& operator/= (const float fVal)
	{ for(int i=0;i<3;i++) m_pfVex[i] /= fVal; return *this; }

	friend COGLVertex operator+ (const COGLVertex& rVex1, const COGLVertex& rVex2);
	friend COGLVertex operator- (const COGLVertex& rVex1, const COGLVertex& rVex2);
	friend float operator* (const COGLVertex& rVex1, const COGLVertex& rVex2);
	friend COGLVertex operator* (const float fVal, const COGLVertex& rVex);
	friend COGLVertex operator* (const COGLVertex& rVex, const float fVal);
	friend COGLVertex operator/ (const COGLVertex& rVex, const float fVal);
	friend COGLVertex operator- (const COGLVertex& rVex);

	// Vector cross product
	friend COGLVertex operator^ (const COGLVertex& rVex1, const COGLVertex& rVex2);

protected:
	float m_pfVex[3];
};
	

#endif // !defined(AFX_OGLVERTEX_H__B634C808_40D5_4BE1_8E64_8AC4B1B21B65__INCLUDED_)
