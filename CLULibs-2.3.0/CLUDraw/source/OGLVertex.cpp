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


// OGLVertex.cpp: Implementierung der Klasse COGLVertex.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OGLVertex.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLVertex::COGLVertex()
{
	memset(m_pfVex, 0, 3*sizeof(float));
}

COGLVertex::COGLVertex(const COGLVertex& rVex)
{
	*this = rVex;
}




//////////////////////////////////////////////////////////////////////
/// COGLVertex Functions

COGLVertex& COGLVertex::Clamp()
{
	int i;

	for(i=0;i<3;i++)
	{
		if (m_pfVex[i] > OGL_VEX_COORD_MAX)
			m_pfVex[i] = OGL_VEX_COORD_MAX;
		else if (m_pfVex[i] < -OGL_VEX_COORD_MAX)
			m_pfVex[i] = -OGL_VEX_COORD_MAX;
	}

	return *this;
}


COGLVertex operator+ (const COGLVertex& rVex1, const COGLVertex& rVex2)
{
	int i;
	COGLVertex Vex;

	for(i=0;i<3;i++)
	{
		Vex.m_pfVex[i] = rVex1.m_pfVex[i] + rVex2.m_pfVex[i];
	}

	return Vex;
}

COGLVertex operator- (const COGLVertex& rVex1, const COGLVertex& rVex2)
{
	int i;
	COGLVertex Vex;

	for(i=0;i<3;i++)
	{
		Vex.m_pfVex[i] = rVex1.m_pfVex[i] - rVex2.m_pfVex[i];
	}

	return Vex;
}

COGLVertex operator* (const float fVal, const COGLVertex& rVex)
{
	int i;
	COGLVertex Vex;

	for(i=0;i<3;i++)
	{
		Vex.m_pfVex[i] = fVal * rVex.m_pfVex[i];
	}

	return Vex;
}

COGLVertex operator* (const COGLVertex& rVex, const float fVal)
{
	int i;
	COGLVertex Vex;

	for(i=0;i<3;i++)
	{
		Vex.m_pfVex[i] = fVal * rVex.m_pfVex[i];
	}

	return Vex;
}

COGLVertex operator/ (const COGLVertex& rVex, const float fVal)
{
	if (fVal == 0.0f)
		return rVex;

	int i;
	COGLVertex Vex;

	for(i=0;i<3;i++)
	{
		Vex.m_pfVex[i] = rVex.m_pfVex[i] / fVal;
	}

	return Vex;
}

COGLVertex operator- (const COGLVertex& rVex)
{
	int i;
	COGLVertex Vex;

	for(i=0;i<3;i++)
	{
		Vex.m_pfVex[i] = -rVex.m_pfVex[i];
	}

	return Vex;
}


// Vector scalar product
float operator* (const COGLVertex& rVex1, const COGLVertex& rVex2)
{
	return ( rVex1[0]*rVex2[0] + rVex1[1]*rVex2[1] + rVex1[2]*rVex2[2] );
}

// Vector cross product
COGLVertex operator^ (const COGLVertex& rVex1, const COGLVertex& rVex2)
{
	COGLVertex Vex;

	Vex[0] = rVex1[1]*rVex2[2] - rVex1[2]*rVex2[1];
	Vex[1] = rVex1[2]*rVex2[0] - rVex1[0]*rVex2[2];
	Vex[2] = rVex1[0]*rVex2[1] - rVex1[1]*rVex2[0];

	return Vex;
}

