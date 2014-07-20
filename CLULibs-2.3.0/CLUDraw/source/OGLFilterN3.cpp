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


// OGLFilterN3.cpp: Implementierung der Klasse COGLFilterN3.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OGLFilterN3.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLFilterN3::COGLFilterN3()
{
	ResetModes();
}

COGLFilterN3::~COGLFilterN3()
{

}


//////////////////////////////////////////////////////////////////////
/// Reset Modes

bool COGLFilterN3::ResetModes()
{
	COGLMVFilterBase::ResetModes();

	m_bSolidObjects = true;
	m_bDrawOPNS = true;

	m_bMVIsBlade = false;
	m_bMVIsVersor = false;

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Set Mode

bool COGLFilterN3::SetMode(int iMode)
{
	if (COGLMVFilterBase::SetMode(iMode))
		return true;

	switch(iMode)
	{
	case N3M_AUTO:
		m_bMVIsBlade = m_bMVIsVersor = false;
		break;

	case N3M_BLADE:
		m_bMVIsVersor = false;
		m_bMVIsBlade = true;
		break;

	case N3M_VERSOR:
		m_bMVIsVersor = true;
		m_bMVIsBlade = false;
		break;

	case MODE_OPNS:
	case N3M_OPNS:
		m_bDrawOPNS = true;
		break;

	case MODE_IPNS:
	case N3M_IPNS:
		m_bDrawOPNS = false;
		break;

	case N3M_SOLID:
		m_bSolidObjects = true;
		break;

	case N3M_WIRE:
		m_bSolidObjects = false;
		break;

	case N3M_SHOW_IMAG:
		m_bShowImaginaryObjects = true;
		break;

	case N3M_HIDE_IMAG:
		m_bShowImaginaryObjects = false;
		break;

	default:
		return false;
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Draw Multivector

bool COGLFilterN3::DrawMV(const MultiV<float>& vX, bool bAnalyzeOnly)
{
	MultiV<float> vA(vX);

	//vA.Round(m_fSensitivity);

	m_MVInfo.Reset();
	m_dMVInfo.Reset();

	if (!m_bMVIsBlade && !m_bMVIsVersor)
	{
		if (!m_N3Base.AnalyzeMV(vA, m_MVInfo, m_bDrawOPNS, m_fSensitivity) || 
			m_MVInfo.m_eType == GA_NONE)
			return false;
	}
	else if (m_bMVIsBlade)
	{
		if (!m_N3Base.AnalyzeBlade(vA, m_MVInfo, m_bDrawOPNS, m_fSensitivity) || 
			m_MVInfo.m_eType == GA_NONE)
			return false;
	}
	else if (m_bMVIsVersor)
	{
		if (!m_N3Base.AnalyzeVersor(vA, m_MVInfo, m_fSensitivity) || 
			m_MVInfo.m_eType == GA_NONE)
			return false;
	}

	if (bAnalyzeOnly)
		return true;
	else
		return DrawMVInfo(m_MVInfo);
}

//////////////////////////////////////////////////////////////////////
/// Draw Multivector

bool COGLFilterN3::DrawMV(const MultiV<double>& vX, bool bAnalyzeOnly)
{
	MultiV<double> vA(vX);

	//vA.Round(m_dSensitivity);

	m_MVInfo.Reset();
	m_dMVInfo.Reset();

	if (!m_bMVIsBlade && !m_bMVIsVersor)
	{
		if (!m_dN3Base.AnalyzeMV(vA, m_dMVInfo, m_bDrawOPNS, m_dSensitivity) || 
			m_dMVInfo.m_eType == GA_NONE)
			return false;
	}
	else if (m_bMVIsBlade)
	{
		if (!m_dN3Base.AnalyzeBlade(vA, m_dMVInfo, m_bDrawOPNS, m_dSensitivity) || 
			m_dMVInfo.m_eType == GA_NONE)
			return false;
	}
	else if (m_bMVIsVersor)
	{
		if (!m_dN3Base.AnalyzeVersor(vA, m_dMVInfo, m_dSensitivity) || 
			m_dMVInfo.m_eType == GA_NONE)
			return false;
	}

	if (!CastMVInfoToFloat(m_MVInfo, m_dMVInfo))
		return false;

	if (bAnalyzeOnly)
		return true;
	else
		return DrawMVInfo(m_MVInfo);
}


//////////////////////////////////////////////////////////////////////
/// Draw MVInfo

bool COGLFilterN3::DrawMVInfo(CMVInfo<float>& rInfo)
{
	COGLVertex pVex[4], pActVex[3];
	float fMag;
//	float pfNull[] = { 0.0f, 0.0f, 0.0f };
	COGLColor Dark, Arrow;

	Dark = m_fBackFaceColFac * m_ActiveColor;
	Arrow = m_fArrowColFac * m_ActiveColor;

	m_VexList.Reset();

	if (rInfo.m_eType == GA_POINT)
	{
		pVex[0] = rInfo.m_mVex[0].Data();

		DrawPoint(pVex[0]);
	}
	else if (rInfo.m_eType == GA_POINTPAIR)
	{
		pVex[0] = rInfo.m_mVex[0].Data();
		pVex[1] = rInfo.m_mVex[1].Data();
		fMag = rInfo.m_mVal[0];
	
		if (fMag > 0.0 || m_bShowImaginaryObjects)
		{
			pActVex[0] = pVex[0] + pVex[1] * fMag;
			pActVex[1] = pVex[0] - pVex[1] * fMag;

			DrawPoint(pActVex[0]);
			DrawPoint(pActVex[1]);

			if (fMag < 0.0)
			{
				DrawLine(pActVex[0], pActVex[1], true, 0x5555);
			}
		}
	}
	else if (rInfo.m_eType == GA_DIR1D)
	{
		pVex[0].Set(0.0, 0.0, 0.0);
		pVex[1] = rInfo.m_mVex[0].Data();

		DrawVector(pVex[0], pVex[1], 0x3333);
	}
	else if (rInfo.m_eType == GA_LINE)
	{
		pVex[0] = rInfo.m_mVex[0].Data();
		pVex[1] = rInfo.m_mVex[1].Data();

		DrawLine(pVex[0], pVex[1] * rInfo.m_mVal[0]);
	}
	else if (rInfo.m_eType == GA_INF_POINTPAIR)
	{
		if (m_bShowImaginaryObjects)
		{
			pVex[0] = rInfo.m_mVex[0].Data();
			pVex[1] = rInfo.m_mVex[1].Data();
			pVex[2].Set(0,0,0);

			float fMag = pVex[0].Mag();
			if (fMag != 0)
			{
				pVex[0] = pVex[0] / fMag;

				Arrow.Apply();
				DrawVector(pVex[2], pVex[0], 0x5555);
			}

			Arrow.Apply();
			DrawVector(pVex[0], pVex[0]+pVex[1], 0x5555);
			Arrow.Apply();
			DrawVector(pVex[0], pVex[0]-pVex[1], 0x5555);

			m_ActiveColor.Apply();
			DrawPoint(pVex[0]+pVex[1]);
			DrawPoint(pVex[0]-pVex[1]);
		}
	}
	else if (rInfo.m_eType == GA_CIRCLE)
	{
		pVex[0] = rInfo.m_mVex[0].Data();
		pVex[1] = rInfo.m_mVex[1].Data();

		DrawCircle(pVex[0], pVex[1], rInfo.m_mVal[0]);
	}
	else if (rInfo.m_eType == GA_DIR2D)
	{
		pVex[0] = rInfo.m_mVex[0].Data();
		pVex[1] = rInfo.m_mVex[1].Data();
		pVex[2].Set(0.0, 0.0, 0.0);
		fMag = 0.5f * rInfo.m_mVal[0];

		pActVex[0] = pVex[0] * fMag;
		pActVex[1] = pVex[1] * fMag;
		
		Arrow.Apply();
		DrawVector(-pActVex[0], pActVex[0], 0x3333);
		Arrow.Apply();
		DrawVector(-pActVex[1], pActVex[1], 0x3333);
		
		pActVex[0] = pVex[0] * (2.0f * fMag - 0.12f*1.8f);
		pActVex[1] = pVex[1] * (2.0f * fMag - 0.12f*1.8f);

		float fA = m_ActiveColor.Alpha();
		m_ActiveColor.Alpha(0.5f);
		DrawPlane(pVex[2], pActVex[0], pActVex[1], true);
		m_ActiveColor.Alpha(fA);
	}
	else if (rInfo.m_eType == GA_PLANE)
	{
		fMag = (float) sqrt(rInfo.m_mVal[0] / m_fPi);

		pVex[0] = rInfo.m_mVex[0].Data();
		pVex[1] = rInfo.m_mVex[1].Data();
		pVex[2] = rInfo.m_mVex[2].Data();

		//DrawPlane(pVex[0], pVex[1]*fMag, pVex[2]*fMag);
		DrawDisk(pVex[0], pVex[1], pVex[2], fMag);
	}
	else if (rInfo.m_eType == GA_INF_LINE)
	{
		//fMag = (float) sqrt(rInfo.m_mVal[0] / m_fPi);

		if (m_bShowImaginaryObjects)
		{
			pVex[0] = rInfo.m_mVex[0].Data();
			pVex[1] = rInfo.m_mVex[1].Data();
			pVex[2] = rInfo.m_mVex[2].Data();

			fMag = pVex[0].Mag();
			if (fMag != 0)
			{
				pVex[0] = pVex[0] / fMag;
				pVex[3].Set(0,0,0);

				Arrow.Apply();
				DrawVector(pVex[3], pVex[0], 0x3333);
			}

			pVex[3][0] = pVex[1][1]*pVex[2][2] - pVex[1][2]*pVex[2][1];
			pVex[3][1] = pVex[1][2]*pVex[2][0] - pVex[1][0]*pVex[2][2];
			pVex[3][2] = pVex[1][0]*pVex[2][1] - pVex[1][1]*pVex[2][0];

			DrawCircle(pVex[0], pVex[3], -1);

			Arrow.Apply();
			DrawVector(pVex[0], pVex[0]+pVex[1], 0x3333);
			Arrow.Apply();
			DrawVector(pVex[0], pVex[0]+pVex[2], 0x3333);
		}
	}
	else if (rInfo.m_eType == GA_SPHERE)
	{
		pVex[0] = rInfo.m_mVex[0].Data();

		DrawSphere(pVex[0], rInfo.m_mVal[0], m_bSolidObjects);
	}
	else if (rInfo.m_eType == GA_DIR3D)
	{
		//float fA = m_ActiveColor.Alpha();
		//m_ActiveColor.Alpha(0.5f);

		pVex[0] = rInfo.m_mVex[0].Data();
		pVex[1] = rInfo.m_mVex[1].Data();
		pVex[2] = rInfo.m_mVex[2].Data();

		fMag = 0.5f * rInfo.m_mVal[0];

		pActVex[0] = pVex[0] * fMag;
		pActVex[1] = pVex[1] * fMag;
		pActVex[2] = pVex[2] * fMag;

		Arrow.Apply();
		DrawVector(-pActVex[0], pActVex[0], 0x3333);
		Arrow.Apply();
		DrawVector(-pActVex[1], pActVex[1], 0x3333);
		Arrow.Apply();
		DrawVector(-pActVex[2], pActVex[2], 0x3333);

//		glEnable(GL_POLYGON_OFFSET_FILL);
//		glPolygonOffset(1.0, 1.0);

		fMag = float( pow(double(rInfo.m_mVal[0]), 1.0/3.0) ) - 0.12f*1.8f;
		DrawRhombus(fMag);

//		glDisable(GL_POLYGON_OFFSET_FILL);
		//m_ActiveColor.Alpha(fA);
	}
	else if (rInfo.m_eType == GA_SPACE)
	{
		fMag = float( pow(double(rInfo.m_mVal[0]), 1.0/3.0) );

//		glEnable(GL_POLYGON_OFFSET_FILL);
//		glPolygonOffset(1.0, 1.0);

		DrawCube(fMag);

//		glDisable(GL_POLYGON_OFFSET_FILL);
	}
	else if (rInfo.m_eType == GA_MOTOR)
	{
		pVex[0] = rInfo.m_mVex[0].Data(); // plumb vector to rot. axis
		pVex[1] = rInfo.m_mVex[1].Data(); // Rot. Axis

		DrawRotor(pVex[0], pVex[1], rInfo.m_mVal[0]);
	}
	else if (rInfo.m_eType == GA_TWIST)
	{
		pVex[0] = rInfo.m_mVex[0].Data(); // plumb vector to rot. axis
		pVex[1] = rInfo.m_mVex[1].Data(); // Trans. along Rot. Axis
		pVex[2] = rInfo.m_mVex[2].Data(); // Rot. Axis

		DrawTwist(pVex[0], pVex[1], pVex[2], rInfo.m_mVal[0]);
	}
	else if (rInfo.m_eType == GA_TRANSLATOR)
	{
		pVex[0].Set(0.0, 0.0, 0.0);
		pVex[1] = rInfo.m_mVex[0].Data(); // translation vector

		DrawVector(pVex[0], pVex[1]);
	}

	return true;
}


