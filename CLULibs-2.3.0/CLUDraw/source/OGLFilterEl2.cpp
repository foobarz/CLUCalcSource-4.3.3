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


// OGLFilterN3.cpp: Implementierung der Klasse COGLFilterEl2.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OGLFilterEl2.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLFilterEl2::COGLFilterEl2()
{
	ResetModes();
}

COGLFilterEl2::~COGLFilterEl2()
{

}


//////////////////////////////////////////////////////////////////////
/// Reset Modes

bool COGLFilterEl2::ResetModes()
{
	COGLMVFilterBase::ResetModes();

	m_bDrawOPNS = true;

	m_bMVIsBlade = false;
	m_bMVIsVersor = false;

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Set Mode

bool COGLFilterEl2::SetMode(int iMode)
{
	if (COGLMVFilterBase::SetMode(iMode))
		return true;

	switch(iMode)
	{
	case EL2M_AUTO:
		m_bMVIsBlade = m_bMVIsVersor = false;
		break;

	case EL2M_BLADE:
		m_bMVIsVersor = false;
		m_bMVIsBlade = true;
		break;

	case EL2M_VERSOR:
		m_bMVIsVersor = true;
		m_bMVIsBlade = false;
		break;

	case MODE_OPNS:
	case EL2M_OPNS:
		m_bDrawOPNS = true;
		break;

	case MODE_IPNS:
	case EL2M_IPNS:
		m_bDrawOPNS = false;
		break;

	case EL2M_SHOW_IMAG:
		m_bShowImaginaryObjects = true;
		break;

	case EL2M_HIDE_IMAG:
		m_bShowImaginaryObjects = false;
		break;

	default:
		return false;
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Draw Multivector

bool COGLFilterEl2::DrawMV(const MultiV<float>& vX, bool bAnalyzeOnly)
{
	MultiV<float> vA(vX);

	//vA.TinyToZero(m_fSensitivity);

	m_MVInfo.Reset();
	m_dMVInfo.Reset();

	if (!m_bMVIsBlade && !m_bMVIsVersor)
	{
		if (!m_El2Base.AnalyzeMV(vA, m_MVInfo, m_bDrawOPNS, m_fSensitivity) || 
			m_MVInfo.m_eType == GA_NONE)
			return false;
	}
	else if (m_bMVIsBlade)
	{
		if (!m_El2Base.AnalyzeBlade(vA, m_MVInfo, m_bDrawOPNS, m_fSensitivity) || 
			m_MVInfo.m_eType == GA_NONE)
			return false;
	}
	else if (m_bMVIsVersor)
	{
		if (!m_El2Base.AnalyzeVersor(vA, m_MVInfo, m_fSensitivity) || 
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

bool COGLFilterEl2::DrawMV(const MultiV<double>& vX, bool bAnalyzeOnly)
{
	MultiV<double> vA(vX);

	//vA.Round(m_dSensitivity);

	m_MVInfo.Reset();
	m_dMVInfo.Reset();

	if (!m_bMVIsBlade && !m_bMVIsVersor)
	{
		if (!m_dEl2Base.AnalyzeMV(vA, m_dMVInfo, m_bDrawOPNS, m_dSensitivity) || 
			m_dMVInfo.m_eType == GA_NONE)
			return false;
	}
	else if (m_bMVIsBlade)
	{
		if (!m_dEl2Base.AnalyzeBlade(vA, m_dMVInfo, m_bDrawOPNS, m_dSensitivity) || 
			m_dMVInfo.m_eType == GA_NONE)
			return false;
	}
	else if (m_bMVIsVersor)
	{
		if (!m_dEl2Base.AnalyzeVersor(vA, m_dMVInfo, m_dSensitivity) || 
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

bool COGLFilterEl2::DrawMVInfo(CMVInfo<float>& rInfo)
{
	COGLVertex pVex[4], pActVex[3];
	float fMag, fMag2;
//	float pfNull[] = { 0.0f, 0.0f, 0.0f };
//	COGLColor Dark, Arrow;

//	Dark = m_fBackFaceColFac * m_ActiveColor;
//	Arrow = m_fArrowColFac * m_ActiveColor;

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
	else if (rInfo.m_eType == GA_POINTSET)
	{
		int i, iCount = rInfo.m_mVex.Count();

		for ( i = 0; i < iCount; i++)
		{
			pVex[0] = rInfo.m_mVex[i].Data();
			DrawPoint(pVex[0]);
		}
	}
	else if (rInfo.m_eType == GA_ELLIPSE)
	{
		pVex[0] = rInfo.m_mVex[0].Data();
		pVex[1] = rInfo.m_mVex[1].Data();
		pVex[2] = rInfo.m_mVex[2].Data();

		DrawEllipse(pVex[2], pVex[0], pVex[1]);
	}
	else if (rInfo.m_eType == GA_HYPERBOLA)
	{
		pVex[0] = rInfo.m_mVex[0].Data();
		pVex[1] = rInfo.m_mVex[1].Data();
		pVex[2] = rInfo.m_mVex[2].Data();

		DrawHyperbola(pVex[2], pVex[0], pVex[1]);
	}
	else if (rInfo.m_eType == GA_LINE)
	{
		pVex[0] = rInfo.m_mVex[0].Data();
		pVex[1] = rInfo.m_mVex[1].Data();
		fMag = rInfo.m_mVal[0];

		DrawLine(pVex[0], pVex[1] * fMag);
	}
	else if (rInfo.m_eType == GA_LINEPAIR)
	{
		pVex[0] = rInfo.m_mVex[0].Data();
		pVex[1] = rInfo.m_mVex[1].Data();
		pVex[2] = rInfo.m_mVex[2].Data();
		fMag = rInfo.m_mVal[0];

/*
		printf("Center: %lg, %lg, %lg\n", pVex[0][0], pVex[0][1], pVex[0][2]);
		printf("Dir 1: %lg, %lg, %lg\n", pVex[1][0], pVex[1][1], pVex[1][2]);
		printf("Dir 2: %lg, %lg, %lg\n", pVex[2][0], pVex[2][1], pVex[2][2]);
*/
		DrawLine(pVex[0], pVex[1] * fMag);
		DrawLine(pVex[0], pVex[2] * fMag);
	}
	else if (rInfo.m_eType == GA_PARLINEPAIR)
	{
		pVex[0] = rInfo.m_mVex[0].Data();
		pVex[1] = rInfo.m_mVex[1].Data();
		pVex[2] = rInfo.m_mVex[2].Data();
		fMag = rInfo.m_mVal[0];

		if (fMag == 0.0f)
		{
			DrawLine(pVex[2], pVex[1] * 100.0f);
		}
		else
		{
			DrawLine(pVex[2] + pVex[0] * fMag, pVex[1] * 100.0f);
			DrawLine(pVex[2] - pVex[0] * fMag, pVex[1] * 100.0f);
		}
	}
	else if (rInfo.m_eType == GA_SPACE)
	{
		fMag = (float) pow(double(rInfo.m_mVal[0]), 1.0/3.0);

//		glEnable(GL_POLYGON_OFFSET_FILL);
//		glPolygonOffset(1.0, 1.0);

		DrawCube(fMag);

//		glDisable(GL_POLYGON_OFFSET_FILL);
	}

	return true;
}


