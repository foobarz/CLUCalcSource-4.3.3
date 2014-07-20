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


// OGLDisplayList.cpp: Implementierung der Klasse COGLDisplayList.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OGLDisplayList.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLDisplayList::COGLDisplayList()
{
	m_uIdx = 0;
}

COGLDisplayList::~COGLDisplayList()
{
	if (m_uIdx) glDeleteLists(m_uIdx, 1);
}


//////////////////////////////////////////////////////////////////////
/// Create Display List

bool COGLDisplayList::Create()
{
	if (!(m_uIdx = glGenLists(1)))
		return false;

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Destroy Display List

bool COGLDisplayList::Destroy()
{
	if (!m_uIdx)
		return false;

	glDeleteLists(m_uIdx, 1);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Begin Draw

bool COGLDisplayList::BeginDraw(bool bDraw)
{
	if (!m_uIdx) return false;

	if (bDraw)
		glNewList(m_uIdx, GL_COMPILE_AND_EXECUTE);
	else
		glNewList(m_uIdx, GL_COMPILE);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// End Draw

bool COGLDisplayList::EndDraw()
{
	if (!m_uIdx) return false;

	glEndList();

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Apply

bool COGLDisplayList::Apply(int iMode, void *pvData)
{
	if (!m_uIdx) return false;

	glCallList(m_uIdx);

	return true;
}

