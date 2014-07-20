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


// OGLMaterial.h: Schnittstelle für die Klasse COGLMaterial.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLMATERIAL_H__2296DD19_C205_454A_82DB_D1881903B593__INCLUDED_)
#define AFX_OGLMATERIAL_H__2296DD19_C205_454A_82DB_D1881903B593__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "OGLBaseElement.h"
#include "OGLColor.h"


class COGLMaterial  : public COGLBaseElement
{
public:
	COGLMaterial();
	COGLMaterial(const COGLMaterial& rMat);

	COGLMaterial& operator= (const COGLMaterial& rMat);

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

	void Emission(float fR, float fG, float fB, float fA)
	{ m_Emission.Set(fR, fG, fB, fA); }
	void Emission(const COGLColor& rCol)
	{ m_Emission = rCol; }
	void Emission(float *pfVal)
	{ m_Emission = pfVal; }

	void Shininess(float fV) { m_fShininess = fV; }
	void Face(GLenum eFace) { m_eFace = eFace; }

	bool Apply(int iMode = 0, void *pvData = 0);

protected:
	COGLColor m_Ambient, m_Diffuse, m_Specular, m_Emission;
	float m_fShininess;

	GLenum m_eFace;
};

#endif // !defined(AFX_OGLMATERIAL_H__2296DD19_C205_454A_82DB_D1881903B593__INCLUDED_)
