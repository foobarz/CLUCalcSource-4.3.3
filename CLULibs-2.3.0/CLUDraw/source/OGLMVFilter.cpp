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


// OGLMVFilter.cpp: Implementierung der Klasse COGLMVFilter.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OGLMVFilter.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLMVFilter::COGLMVFilter()
{
	m_bEnableP2 = false;
}

COGLMVFilter::~COGLMVFilter()
{

}


//////////////////////////////////////////////////////////////////////
/// Init

bool COGLMVFilter::Init()
{
	COGLMVFilterBase::Init();

	m_E3Filter.Init();
	m_P2Filter.Init();
	m_P3Filter.Init();
	m_N3Filter.Init();
	m_El2Filter.Init();

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Texture Repeat Factor

void COGLMVFilter::SetTextureRepeatFactor(float fVal)
{
	m_E3Filter.SetTextureRepeatFactor(fVal);
	m_P2Filter.SetTextureRepeatFactor(fVal);
	m_P3Filter.SetTextureRepeatFactor(fVal);
	m_N3Filter.SetTextureRepeatFactor(fVal);
	m_El2Filter.SetTextureRepeatFactor(fVal);
	COGLMVFilterBase::SetTextureRepeatFactor(fVal);
}

//////////////////////////////////////////////////////////////////////

void COGLMVFilter::EnableAbsTexCoords(bool bVal)
{
	m_E3Filter.EnableAbsTexCoords(bVal);
	m_P2Filter.EnableAbsTexCoords(bVal);
	m_P3Filter.EnableAbsTexCoords(bVal);
	m_N3Filter.EnableAbsTexCoords(bVal);
	m_El2Filter.EnableAbsTexCoords(bVal);
	COGLMVFilterBase::EnableAbsTexCoords(bVal);
}


//////////////////////////////////////////////////////////////////////
/// Set Arrow Shape

void COGLMVFilter::SetArrowShape(float fSize, float fAngle)
{
	m_E3Filter.SetArrowShape(fSize, fAngle);
	m_P2Filter.SetArrowShape(fSize, fAngle);
	m_P3Filter.SetArrowShape(fSize, fAngle);
	m_N3Filter.SetArrowShape(fSize, fAngle);
	m_El2Filter.SetArrowShape(fSize, fAngle);
	COGLMVFilterBase::SetArrowShape(fSize, fAngle);
}

//////////////////////////////////////////////////////////////////////
/// DrawMV

bool COGLMVFilter::DrawMV(const MultiV<float>& vA, bool bAnalyzeOnly)
{
	switch(vA.BasePtr()->BaseID())
	{
	case ID_E3GA:
		m_iLastMode = ID_E3GA;
		return m_E3Filter.DrawMV(vA, bAnalyzeOnly);

	case ID_PGA:
		m_iLastMode = ID_PGA;
		if (m_bEnableP2)
			return m_P2Filter.DrawMV(vA, bAnalyzeOnly);
		else
			return m_P3Filter.DrawMV(vA, bAnalyzeOnly);

	case ID_CONFGA:
		m_iLastMode = ID_CONFGA;
		return m_N3Filter.DrawMV(vA, bAnalyzeOnly);

	case ID_EL2GA:
		m_iLastMode = ID_EL2GA;
		return m_El2Filter.DrawMV(vA, bAnalyzeOnly);

	default:
		return false;
	}
}

//////////////////////////////////////////////////////////////////////
/// DrawMV

bool COGLMVFilter::DrawMV(const MultiV<double>& vA, bool bAnalyzeOnly)
{
	switch(vA.BasePtr()->BaseID())
	{
	case ID_E3GA:
		m_iLastMode = ID_E3GA;
		return m_E3Filter.DrawMV(vA, bAnalyzeOnly);

	case ID_PGA:
		m_iLastMode = ID_PGA;
		if (m_bEnableP2)
			return m_P2Filter.DrawMV(vA, bAnalyzeOnly);
		else
			return m_P3Filter.DrawMV(vA, bAnalyzeOnly);

	case ID_CONFGA:
		m_iLastMode = ID_CONFGA;
		return m_N3Filter.DrawMV(vA, bAnalyzeOnly);

	case ID_EL2GA:
		m_iLastMode = ID_EL2GA;
		return m_El2Filter.DrawMV(vA, bAnalyzeOnly);

	default:
		return false;
	}
}

//////////////////////////////////////////////////////////////////////
/// Reset Modes

bool COGLMVFilter::ResetModes()
{
	COGLMVFilterBase::ResetModes();
	
	m_E3Filter.ResetModes();
	m_P3Filter.ResetModes();
	m_N3Filter.ResetModes();
	m_El2Filter.ResetModes();

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Mode

bool COGLMVFilter::SetMode(int iMode)
{
	switch(iMode >> 16)
	{
	case ID_E3GA:
		return m_E3Filter.SetMode(iMode);

	case ID_PGA:
		if (m_bEnableP2)
			return m_P2Filter.SetMode(iMode);
		else
			return m_P3Filter.SetMode(iMode);

	case ID_CONFGA:
		return m_N3Filter.SetMode(iMode);

	case ID_EL2GA:
		return m_El2Filter.SetMode(iMode);

	case ID_ALL:
		m_E3Filter.SetMode(iMode);
		if (m_bEnableP2)
			m_P2Filter.SetMode(iMode);
		else
			m_P3Filter.SetMode(iMode);
		m_N3Filter.SetMode(iMode);
		m_El2Filter.SetMode(iMode);
		return true;

	default:
		return false;
	}
}

//////////////////////////////////////////////////////////////////////
/// Enable Lighting

void COGLMVFilter::EnableLighting(bool bLight)
{
	COGLMVFilterBase::EnableLighting(bLight); 

	m_E3Filter.EnableLighting();
	m_P2Filter.EnableLighting();
	m_P3Filter.EnableLighting();
	m_N3Filter.EnableLighting();
	m_El2Filter.EnableLighting();
}


//////////////////////////////////////////////////////////////////////
// Set Sensitivity

void COGLMVFilter::SetSensitivity(double dSens)
{
	COGLMVFilterBase::SetSensitivity(dSens);

	m_E3Filter.SetSensitivity(dSens);
	m_P2Filter.SetSensitivity(dSens);
	m_P3Filter.SetSensitivity(dSens);
	m_N3Filter.SetSensitivity(dSens);
	m_El2Filter.SetSensitivity(dSens);
}


//////////////////////////////////////////////////////////////////////
/// Set Color

bool COGLMVFilter::SetColor(COGLColor& rCol)
{
	COGLMVFilterBase::SetColor(rCol);

	m_E3Filter.SetColor(rCol);
	m_P2Filter.SetColor(rCol);
	m_P3Filter.SetColor(rCol);
	m_N3Filter.SetColor(rCol);
	m_El2Filter.SetColor(rCol);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Get MV Info

CMVInfo<float> COGLMVFilter::GetMVInfo()
{
	switch(m_iLastMode)
	{
	case ID_E3GA:
		return m_E3Filter.GetMVInfo();

	case ID_PGA:
		if (m_bEnableP2)
			return m_P2Filter.GetMVInfo();
		else
			return m_P3Filter.GetMVInfo();

	case ID_CONFGA:
		return m_N3Filter.GetMVInfo();

	case ID_EL2GA:
		return m_El2Filter.GetMVInfo();

	default:
		return m_MVInfo;
	}
}


//////////////////////////////////////////////////////////////////////
/// Get MV Info

void COGLMVFilter::GetMVInfo(CMVInfo<float>& rInfo)
{
	switch(m_iLastMode)
	{
	case ID_E3GA:
		m_E3Filter.GetMVInfo(rInfo);
		break;

	case ID_PGA:
		if (m_bEnableP2)
			m_P2Filter.GetMVInfo(rInfo);
		else
			m_P3Filter.GetMVInfo(rInfo);
		break;

	case ID_CONFGA:
		m_N3Filter.GetMVInfo(rInfo);
		break;

	case ID_EL2GA:
		m_El2Filter.GetMVInfo(rInfo);
		break;

	default:
		rInfo = m_MVInfo;
		break;
	}

}


//////////////////////////////////////////////////////////////////////
/// Get MV Info

void COGLMVFilter::GetMVInfo(CMVInfo<double>& rInfo)
{
	switch(m_iLastMode)
	{
	case ID_E3GA:
		m_E3Filter.GetMVInfo(rInfo);
		break;

	case ID_PGA:
		if (m_bEnableP2)
			m_P2Filter.GetMVInfo(rInfo);
		else
			m_P3Filter.GetMVInfo(rInfo);
		break;

	case ID_CONFGA:
		m_N3Filter.GetMVInfo(rInfo);
		break;

	case ID_EL2GA:
		m_El2Filter.GetMVInfo(rInfo);
		break;

	default:
		rInfo = m_dMVInfo;
		break;
	}

}

