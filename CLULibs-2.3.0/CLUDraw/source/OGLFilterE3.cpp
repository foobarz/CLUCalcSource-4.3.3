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


// OGLFilterE3GA.cpp: Implementierung der Klasse COGLFilterE3.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OGLFilterE3.h"
#include "math.h"

//MultiV<float> vDummy;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLFilterE3::COGLFilterE3()
{
	ResetModes();
}

COGLFilterE3::~COGLFilterE3()
{

}

//////////////////////////////////////////////////////////////////////
/// Reset Modes

bool COGLFilterE3::ResetModes()
{
	COGLMVFilterBase::ResetModes();
	
	m_bDrawOPNS = true;

	m_eDrawVecType = StdVec;
	m_eDrawPlaneType = StdPlane;

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Set Mode

bool COGLFilterE3::SetMode(int iMode)
{
	if (COGLMVFilterBase::SetMode(iMode))
		return true;

	switch(iMode)
	{
	case MODE_OPNS:
		m_bDrawOPNS = true;
		break;

	case MODE_IPNS:
		m_bDrawOPNS = false;
		break;

	case E3M_DRAW_VEC_AS_LINE:
		m_eDrawVecType = Line;
		break;

	case E3M_DRAW_VEC_AS_ARROW:
		m_eDrawVecType = Arrow;
		break;

	case E3M_DRAW_VEC_AS_POINT:
		m_eDrawVecType = Point;
		break;

	case E3M_DRAW_PLANE_AS_RECT:
		m_eDrawPlaneType = Rectangle;
		break;

	case E3M_DRAW_PLANE_AS_DISK:
		m_eDrawPlaneType = Disk;
		break;

	default:
		return false;
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Draw Multivector

bool COGLFilterE3::DrawMV(const MultiV<float>& vX, bool bAnalyzeOnly)
{
	MultiV<float> vA(vX);

	vA.TinyToZero(m_fSensitivity);

	m_MVInfo.Reset();
	m_dMVInfo.Reset();

	if (!m_E3Base.AnalyzeMV(vA, m_MVInfo, m_bDrawOPNS, m_fSensitivity) || 
		m_MVInfo.m_eType == GA_NONE)
		return false;

	if (bAnalyzeOnly)
		return true;
	else
		return DrawMVInfo(m_MVInfo);
}


//////////////////////////////////////////////////////////////////////
/// Draw Multivector

bool COGLFilterE3::DrawMV(const MultiV<double>& vX, bool bAnalyzeOnly)
{
	MultiV<double> vA(vX);

	vA.TinyToZero((double) m_fSensitivity);

	m_MVInfo.Reset();
	m_dMVInfo.Reset();

	if (!m_dE3Base.AnalyzeMV(vA, m_dMVInfo, m_bDrawOPNS, m_fSensitivity) || 
		m_dMVInfo.m_eType == GA_NONE)
		return false;

	if (!CastMVInfoToFloat(m_MVInfo, m_dMVInfo))
		return false;

	if (bAnalyzeOnly)
		return true;
	else
		return DrawMVInfo(m_MVInfo);
}

//////////////////////////////////////////////////////////////////////
/// Draw Multivector from Info

bool COGLFilterE3::DrawMVInfo(CMVInfo<float>& rInfo)
{
	COGLVertex pVex[3];
	COGLVertex xN;
	float fMag;
	COGLColor Dark;

	m_VexList.Reset();
	Dark = m_fBackFaceColFac * m_ActiveColor;

	if (rInfo.m_eType == GA_POINT)
	{
		pVex[0].Set(0.0, 0.0, 0.0);
		pVex[1] = rInfo.m_mVex[0].Data();

		switch(m_eDrawVecType)
		{
		case Line:
			DrawPoint(pVex[1]);
			DrawLine(pVex[0], pVex[1], true);
			break;

		case Arrow:
			DrawVector(pVex[0], pVex[1]);
			break;

		case Point:
			DrawPoint(pVex[1]);
			break;
		}
	}
	else if (rInfo.m_eType == GA_PLANE)
	{
		pVex[0].Set(0.0f, 0.0f, 0.0f);
		pVex[1] = rInfo.m_mVex[0].Data();
		pVex[2] = rInfo.m_mVex[1].Data();

		switch(m_eDrawPlaneType)
		{
		case Rectangle:
			fMag = (float) sqrt(rInfo.m_mVal[0]);
			DrawPlane(pVex[0], pVex[1]*fMag, pVex[2]*fMag);
			break;

		case Disk:
			fMag = (float) sqrt(rInfo.m_mVal[0] / m_fPi);
			DrawDisk(pVex[0], pVex[1], pVex[2], fMag);
			break;
		}
	}
	else if (rInfo.m_eType == GA_SPACE)
	{
		fMag = float( pow(double(rInfo.m_mVal[0]), 1.0/3.0) );

		DrawCube(fMag);
	}
	else if (rInfo.m_eType == GA_ROTOR)
	{
		pVex[0].Set(0.0f, 0.0f, 0.0f);
		pVex[1] = rInfo.m_mVex[0].Data(); // Rot. Axis

		DrawRotor(pVex[0], pVex[1], rInfo.m_mVal[0]);
	}

	return true;
}



