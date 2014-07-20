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


// OGLBaseTransform.cpp: Implementierung der Klasse COGLBaseTransform.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h" 
#include "OGLBaseTransform.h"
#include <math.h>


double COGLBaseTransform::m_dPi = acos(-1.0);
double COGLBaseTransform::m_dRadPerDeg = COGLBaseTransform::m_dPi / 180.0;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLBaseTransform::COGLBaseTransform()
{
	SetTransformation(0,0,0,0,0,0,0);
}

COGLBaseTransform::~COGLBaseTransform()
{
	
}

//////////////////////////////////////////////////////////////////////
/// Set Translation

bool COGLBaseTransform::SetTranslation(float TX, float TY, float TZ)
{
	m_fTX = TX; 
	m_fTY = TY; 
	m_fTZ = TZ;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Transformation

bool COGLBaseTransform::SetRotation(float RAngle, float RX, float RY, float RZ)
{
	m_fRAngle = RAngle; 
	m_fRX = RX; 
	m_fRY = RY; 
	m_fRZ = RZ; 

	return true;
}

bool COGLBaseTransform::SetRotation(float RXAngle, float RYAngle, float RZAngle)
{
	double ca, sa, cb, sb, cc, sc, t;
	double st;

	m_dPi = acos(-1.0);

	ca = cos(RXAngle*m_dRadPerDeg); sa = sin(RXAngle*m_dRadPerDeg);
	cb = cos(RYAngle*m_dRadPerDeg); sb = sin(RYAngle*m_dRadPerDeg);
	cc = cos(RZAngle*m_dRadPerDeg); sc = sin(RZAngle*m_dRadPerDeg);


	t = acos((cb*cc + (ca*cc + sa*sb*sc) + ca*cb -1)/2.0);
	m_fRAngle = float(t / m_dRadPerDeg);
	st = sin(t);
	if(st != 0)
	{
		m_fRX = (float) (0.5/st*(sa*cb +sa*cc - ca*sb*sc)); 
		m_fRY = (float) (0.5/st*(sa*sc + ca*sb*cc - sb));
		m_fRZ = (float) (0.5/st*(cb*sc + ca*sc - sa*sb*cc));
	}
	else
	{
		m_fRX = 0; m_fRY = 0; m_fRZ = 0;
	}

	return true;
}

bool COGLBaseTransform::Apply(int iMode, void *pvData)
{
	glRotatef(m_fRAngle, m_fRX, m_fRY, m_fRZ);
	glTranslatef(m_fTX, m_fTY, m_fTZ);

	return true;
}

