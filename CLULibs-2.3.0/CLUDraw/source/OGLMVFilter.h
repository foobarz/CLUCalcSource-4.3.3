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


// OGLMVFilter.h: Schnittstelle für die Klasse COGLMVFilter.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLMVFILTER_H__BBFD3218_CC4C_4518_8232_DCEE325D9EB2__INCLUDED_)
#define AFX_OGLMVFILTER_H__BBFD3218_CC4C_4518_8232_DCEE325D9EB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OGLMVFilterBase.h"
#include "OGLFilterE3.h"
#include "OGLFilterP2.h"
#include "OGLFilterP3.h"
#include "OGLFilterN3.h"
#include "OGLFilterEl2.h"


class COGLMVFilter : public COGLMVFilterBase  
{
public:
	COGLMVFilter();
	virtual ~COGLMVFilter();

	virtual bool Init();
	virtual bool DrawMV(const MultiV<float>& vA, bool bAnalyzeOnly = false);
	virtual bool DrawMV(const MultiV<double>& vA, bool bAnalyzeOnly = false);
	virtual bool ResetModes();
	virtual bool SetMode(int iMode);
	virtual bool SetColor(COGLColor& rCol);
		
	void EnableLighting(bool bLight = true);
	void EnableP2(bool bVal = true) { m_bEnableP2 = bVal; }

	void SetSensitivity(double dSens);
	void SetTextureRepeatFactor(float fVal);
	void EnableAbsTexCoords(bool bVal = true);
	void SetArrowShape(float fSize, float fAngle);


	CMVInfo<float> GetMVInfo();
	void GetMVInfo(CMVInfo<float> &rInfo);
	void GetMVInfo(CMVInfo<double> &rInfo);

	int GetLastMVID() { return m_iLastMode; }

public:
	COGLFilterE3 m_E3Filter;
	COGLFilterP2 m_P2Filter;
	COGLFilterP3 m_P3Filter;
	COGLFilterN3 m_N3Filter;
	COGLFilterEl2 m_El2Filter;

protected:
	bool m_bEnableP2;
	int m_iLastMode;
};

#endif // !defined(AFX_OGLMVFILTER_H__BBFD3218_CC4C_4518_8232_DCEE325D9EB2__INCLUDED_)
