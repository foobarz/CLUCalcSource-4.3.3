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


// Class written by Vladimir Banarer, 3.12.2001.


// OGLBaseObject.cpp: Implementierung der Klasse COGLBaseObject.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OGLBaseObject.h"


//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLBaseObject::COGLBaseObject()
{

}

COGLBaseObject::~COGLBaseObject()
{

}


bool COGLBaseObject::Apply(int iMode, void *pvData)
{
	int i, n;

	n = m_moIdxListList.Count();

			for (i = 0; i <n; i++)
			{
				SetIdxList(m_moIdxListList[i]);
				SetMode((GLenum)m_mModeList[i]);
				COGLVertexList::Apply();
			}


	return true;
}




bool COGLBaseObject::AddIdxList(int iNo, int *pIdx, int iMode)
{
	int i = m_moIdxListList.Count();

	m_moIdxListList.Add(1);
	if (iNo < 0 || !m_moIdxListList[i].Set(iNo))
		return false;


	memcpy(m_moIdxListList[i].Data(), pIdx, iNo*sizeof(int));
	m_mModeList.Add(1);
	m_mModeList[i] = iMode;


	return true;
}

bool COGLBaseObject::AddIdxList(Mem<int>& mIdx, int iMode)
{
	int i = m_moIdxListList.Count();

	m_moIdxListList.Add(1);
	m_moIdxListList[i] = mIdx;

	m_mModeList.Add(1);
	m_mModeList[i] = iMode;

	return true;

}


void COGLBaseObject::Reset()
{
	COGLVertexList::Reset();

	m_mModeList.Set(0);
	m_moIdxListList.Set(0);

}
