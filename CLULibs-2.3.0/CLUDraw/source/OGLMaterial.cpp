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


// OGLMaterial.cpp: Implementierung der Klasse COGLMaterial.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OGLMaterial.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLMaterial::COGLMaterial()
{
	Reset();
}

COGLMaterial::COGLMaterial(const COGLMaterial& rMat)
{
	*this = rMat;
}

COGLMaterial& COGLMaterial::operator= (const COGLMaterial& rMat)
{
	m_eFace = rMat.m_eFace;

	m_Ambient = rMat.m_Ambient;
	m_Diffuse = rMat.m_Diffuse;
	m_Specular = rMat.m_Specular;
	m_Emission = rMat.m_Emission;

	m_fShininess = rMat.m_fShininess;

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Reset

void COGLMaterial::Reset()
{
	m_eFace = GL_FRONT_AND_BACK;

	m_Ambient.Set(0.2f, 0.2f, 0.2f, 1.0f);
	m_Diffuse.Set(0.8f, 0.8f, 0.8f, 1.0f);
	m_Specular.Set(0.0f, 0.0f, 0.0f, 1.0f);
	m_Emission.Set(0.0f, 0.0f, 0.0f, 1.0f);

	m_fShininess = 0.0;
}

//////////////////////////////////////////////////////////////////////
/// Apply

bool COGLMaterial::Apply(int iMode , void *pvData)
{
	glMaterialfv(m_eFace, GL_AMBIENT, m_Ambient.Data());
	glMaterialfv(m_eFace, GL_DIFFUSE, m_Diffuse.Data());
	glMaterialfv(m_eFace, GL_SPECULAR, m_Specular.Data());
	glMaterialfv(m_eFace, GL_EMISSION, m_Emission.Data());

	glMaterialf(m_eFace, GL_SHININESS, m_fShininess);

	return true;
}
