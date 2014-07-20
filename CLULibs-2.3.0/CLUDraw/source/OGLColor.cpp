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


// OGLColor.cpp: Implementierung der Klasse COGLColor.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OGLColor.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLColor::COGLColor()
{
	for(int i=0;i<4;i++)
		m_pfCol[i] = 0.0f;
}

COGLColor::COGLColor(const COGLColor& rCol)
{
	*this = rCol;
}

//////////////////////////////////////////////////////////////////////
// Copy Operator

COGLColor& COGLColor::operator= (const COGLColor& rCol)
{
	memcpy(m_pfCol, rCol.m_pfCol, 4*sizeof(float));

	return *this;
}

//////////////////////////////////////////////////////////////////////
// Copy Operator

COGLColor& COGLColor::operator= (const float *pfCol)
{
	memcpy(m_pfCol, pfCol, 4*sizeof(float));

	return *this;
}


//////////////////////////////////////////////////////////////////////
/// Apply Color

bool COGLColor::Apply(int iMode , void *pvData )
{
	glColor4fv(m_pfCol);
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Renorm Color
void COGLColor::Renorm()
{
	float fMax = 0.0;

	for(int i=0;i<3;i++) 
	{
		if (fMax < m_pfCol[i])
			fMax = m_pfCol[i]; 
	}
	
	if (fMax > 1.0f)
	{
		for(int i=0;i<3;i++) 
		{
			m_pfCol[i] /= fMax;
		}
	}

	if (m_pfCol[3] < 0.0)
		m_pfCol[3] = 0.0;
	else if (m_pfCol[3] > 1.0)
		m_pfCol[3] = 1.0;

}


//////////////////////////////////////////////////////////////////////
/// Clamp Color
void COGLColor::Clamp()
{
	float fMax = 0.0;

	for(int i=0;i<4;i++) 
	{
		if (m_pfCol[i] < 0.0f)
			m_pfCol[i] = 0.0f; 
		else if (m_pfCol[i] > 1.0f)
			m_pfCol[i] = 1.0f;
	}
}

//////////////////////////////////////////////////////////////////////
/// Scale Colours

COGLColor operator* (float fVal, const COGLColor &rCol)
{ 
//	if (fVal < 0.0)
//		return rCol;

	COGLColor NewCol;

//	float fMax = 0.0;
	if (rCol.m_pfCol[0] == 0 &&
		rCol.m_pfCol[1] == 0 &&
		rCol.m_pfCol[2] == 0)
	{
		NewCol.Set(0, 0, 0, rCol.m_pfCol[3] * fVal);
	}
	else
	{
		for(int i=0;i<3;i++) 
		{
			NewCol.m_pfCol[i] = rCol.m_pfCol[i] * fVal;
		}

		NewCol.m_pfCol[3] = rCol.m_pfCol[3];
	}

	return NewCol; 
}

//////////////////////////////////////////////////////////////////////
/// Add Colors

COGLColor operator+ (const COGLColor& rCol1, const COGLColor& rCol2)
{ 
	int i;
	COGLColor NewCol;

	for(i=0;i<4;i++) 
		NewCol.m_pfCol[i] = rCol1.m_pfCol[i] + rCol2.m_pfCol[i]; 

	return NewCol; 
}

//////////////////////////////////////////////////////////////////////
/// Subtract Colors

COGLColor operator- (const COGLColor& rCol1, const COGLColor& rCol2)
{
	int i;
	COGLColor NewCol;

	for(i=0;i<4;i++) 
		NewCol.m_pfCol[i] = rCol1.m_pfCol[i] - rCol2.m_pfCol[i]; 

	return NewCol; 
}


//////////////////////////////////////////////////////////////////////
// Add

COGLColor& COGLColor::operator+= (const COGLColor& rCol)
{
	int i;

	for(i=0;i<4;i++) 
		m_pfCol[i] += rCol.m_pfCol[i];

	return *this; 
}


//////////////////////////////////////////////////////////////////////
// Subtract

COGLColor& COGLColor::operator-= (const COGLColor& rCol)
{
	int i;

	for(i=0;i<4;i++) 
		m_pfCol[i] -= rCol.m_pfCol[i];

	return *this; 
}


//////////////////////////////////////////////////////////////////////
// Divide

COGLColor& COGLColor::operator/= (float fVal)
{
	int i;

	for(i=0;i<3;i++) 
		m_pfCol[i] /= fVal;

	return *this; 
}
