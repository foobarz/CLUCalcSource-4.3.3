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


// OGLVertexList.h: Schnittstelle für die Klasse COGLVertexList.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLVERTEXLIST_H__C262712E_1840_43F3_A75E_F76904823DE9__INCLUDED_)
#define AFX_OGLVERTEXLIST_H__C262712E_1840_43F3_A75E_F76904823DE9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OGLVertex.h"
#include "OGLColor.h"
#include "OGLBaseElement.h"

typedef CPointND<float, 4> TColor;

class COGLVertexList : public COGLBaseElement
{
public:
	COGLVertexList();
	COGLVertexList(const COGLVertexList& rVexList);
	virtual ~COGLVertexList();

	COGLVertexList& operator= (const COGLVertexList& rVexList);

	void Reset() { m_mVexList.Set(0); m_mTexList.Set(0); m_mNormalList.Set(0); m_mColList.Set(0); m_mIdxList.Set(0); }

	COGLVertexList& operator<< (const COGLVertex& rVex)
	{ AddVex(rVex); return *this; }

	COGLVertexList& operator<< (const COGLColor& rCol)
	{ AddCol(rCol); return *this; }

	COGLVertexList& operator<< (Mem<int>& mIdx)
	{ SetIdxList(mIdx); return *this; }

	COGLVertexList& operator<< (GLenum eMode)
	{ SetMode(eMode); return *this; }

	COGLVertex& operator[] (int i) { return m_mVexList[(uint)i]; }
	COGLVertex& GetTex(int iPos) { return m_mTexList[(uint)iPos]; }
	COGLVertex& GetNormal(int iPos) { return m_mNormalList[(uint)iPos]; }
	TColor& GetColor(int iPos) { return m_mColList[(uint)iPos]; }
	int& GetIndex(int iPos) { return m_mIdxList[(uint)iPos]; }

	bool AddVex(const COGLVertex& rVex);
	bool AddVex(const float *pfVex);
	bool AddVex(float fX, float fY, float fZ);

	bool AddTex(const COGLVertex& rVex);
	bool AddTex(const float *pfVex);
	bool AddTex(float fX, float fY, float fZ);

	bool AddCol(const COGLColor& rCol);
	bool AddCol(const float *pfCol);
	bool AddCol(float fR, float fG, float fB, float fA = 0.0f);

	bool AddNormal(const COGLVertex& rVex);
	bool AddNormal(const float *pfVex);
	bool AddNormal(float fX, float fY, float fZ);

	bool SetIdxList(int iNo, int *pIdx);
	bool SetIdxList(Mem<int>& mIdx);

	void SetMode(GLenum eMode) { m_eMode = eMode; }
	GLenum GetMode() const { return m_eMode; }

	void InvertNormals(float fFac = 1.0f);

	int Count() const { return m_mVexList.Count(); }
	bool Apply(int iMode = 0, void *pvData = 0);

	Mem<COGLVertex>& GetVexList() { return m_mVexList; }
	Mem<COGLVertex>& GetNormList() { return m_mNormalList; }
	Mem<COGLVertex>& GetTexList() { return m_mTexList; }
	Mem<TColor>& GetColList() { return m_mColList; }
	Mem<int>& GetIdxList() { return m_mIdxList; }

protected:
	GLenum m_eMode;
	Mem<COGLVertex> m_mVexList;
	Mem<COGLVertex> m_mNormalList;
	Mem<COGLVertex> m_mTexList;
	Mem<TColor> m_mColList;
	Mem<int> m_mIdxList;
};

#endif // !defined(AFX_OGLVERTEXLIST_H__C262712E_1840_43F3_A75E_F76904823DE9__INCLUDED_)
