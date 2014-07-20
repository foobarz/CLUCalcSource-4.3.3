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

// OGLObjColorCube.cpp: Implementierung der Klasse COGLObjColorCube.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OGLObjColorCube.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLObjColorCube::COGLObjColorCube()
{
	m_iListIdx = 0;
}

COGLObjColorCube::~COGLObjColorCube()
{
	glDeleteLists(m_iListIdx, 1);
}


//////////////////////////////////////////////////////////////////////
/// Init

void COGLObjColorCube::Init()
{
	int i, j, k;
	float fR, fG, fB;

	if (!m_iListIdx)
		m_iListIdx = glGenLists(1);
	
	m_VexList.Reset();
	m_VexList.SetMode(GL_POINTS);
	
	for(i=0,fR=0.0;i<10;i++,fR+=0.1f)
	{
		for(j=0,fG=0.0;j<10;j++, fG+=0.1f)
		{
			for(k=0,fB=0.0;k<10;k++,fB+=0.1f)
			{
				m_VexList.AddVex(fR, fG, fB);
				m_VexList.AddCol(fR, fG, fB);
			}
		}
	}
	
	glNewList(m_iListIdx, GL_COMPILE);
	m_VexList.Apply();
	glEndList();
	
}

//////////////////////////////////////////////////////////////////////
/// Apply

bool COGLObjColorCube::Apply()
{
	glCallList(m_iListIdx);

	return true;
}

