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


// OGLLight.h: Schnittstelle für die Klasse COGLLight.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLLIGHT_H__0AA89723_687A_4A9B_9FC0_50A77C5D2A73__INCLUDED_)
#define AFX_OGLLIGHT_H__0AA89723_687A_4A9B_9FC0_50A77C5D2A73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OGLColor.h"
#include "OGLBaseElement.h"

typedef CPointND<float, 4> TPoint4D;
typedef CPointND<float, 3> TPoint3D;

class COGLLight : public COGLBaseElement 
{
public:
	COGLLight();
	COGLLight(const COGLLight& rLight);

	COGLLight& operator= (const COGLLight& rLight);

	void Reset();

	void Ambient(float fR, float fG, float fB, float fA)
	{ m_Ambient.Set(fR, fG, fB, fA); }
	void Ambient(const COGLColor& rCol)
	{ m_Ambient = rCol; }
	void Ambient(float *pfVal)
	{ m_Ambient = pfVal; }

	void Diffuse(float fR, float fG, float fB, float fA)
	{ m_Diffuse.Set(fR, fG, fB, fA); }
	void Diffuse(const COGLColor& rCol)
	{ m_Diffuse = rCol; }
	void Diffuse(float *pfVal)
	{ m_Diffuse = pfVal; }

	void Specular(float fR, float fG, float fB, float fA)
	{ m_Specular.Set(fR, fG, fB, fA); }
	void Specular(const COGLColor& rCol)
	{ m_Specular = rCol; }
	void Specular(float *pfVal)
	{ m_Specular = pfVal; }

	void Position(float fX, float fY, float fZ, float fW)
	{ 
		m_Position[0] = fX; m_Position[1] = fY;
		m_Position[2] = fZ; m_Position[3] = fW; 
	}
	void Position(float *pfVal)
	{ m_Position = pfVal; }

	void SpotDir(float fX, float fY, float fZ)
	{ 
		m_SpotDir[0] = fX; m_SpotDir[1] = fY;
		m_SpotDir[2] = fZ; 
	}
	void SpotDir(float *pfVal)
	{ m_SpotDir = pfVal; }

	void SpotExp(float fExp) { m_fSpotExp = fExp; }
	void SpotCutOff(float fCO) { m_fSpotCutOff = fCO; }
	void ConstAtt(float fV) { m_fConstAtt = fV; }
	void LinAtt(float fV) { m_fLinAtt = fV; }
	void QuadAtt(float fV) { m_fQuadAtt = fV; }

	void LightNo(int iNo)
	{ if (iNo >= 0) m_eLight = GLenum(GL_LIGHT0+iNo); }

	void Enable(bool bAct = true)
	{ if ((m_bIsEnabled = bAct)) glEnable(m_eLight); else glDisable(m_eLight); }

	bool Apply(int iMode = 0, void *pvData = 0);
	bool Apply(GLenum eWhat);

protected:
	GLenum m_eLight;

	COGLColor m_Ambient, m_Diffuse, m_Specular;
	TPoint4D m_Position;
	TPoint3D m_SpotDir;

	float m_fSpotExp;		// Spot Exponent
	float m_fSpotCutOff;	// Spot Cut Off
	float m_fConstAtt;		// Constant Attenuation
	float m_fLinAtt;		// Linear Attenuation
	float m_fQuadAtt;		// Quadratic Attenuation

	bool m_bIsEnabled;
};

#endif // !defined(AFX_OGLLIGHT_H__0AA89723_687A_4A9B_9FC0_50A77C5D2A73__INCLUDED_)
