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


// OGLLight.cpp: Implementierung der Klasse COGLLight.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OGLLight.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLLight::COGLLight()
{
	Reset();

	m_eLight = GL_LIGHT0;
	m_bIsEnabled = false;
}

COGLLight::COGLLight(const COGLLight& rLight)
{
	*this = rLight;
}


COGLLight& COGLLight::operator= (const COGLLight& rLight)
{
	m_eLight = rLight.m_eLight;
	m_bIsEnabled = rLight.m_bIsEnabled;

	m_Ambient = rLight.m_Ambient;
	m_Diffuse = rLight.m_Diffuse;
	m_Specular = rLight.m_Specular;
	m_Position = rLight.m_Position;
	m_SpotDir = rLight.m_SpotDir;

	m_fSpotExp = rLight.m_fSpotExp;
	m_fSpotCutOff = rLight.m_fSpotCutOff;
	
	m_fConstAtt = rLight.m_fConstAtt;
	m_fLinAtt = rLight.m_fLinAtt;
	m_fQuadAtt = rLight.m_fQuadAtt;

	return *this;
}


//////////////////////////////////////////////////////////////////////
/// Reset

void COGLLight::Reset()
{
	m_Ambient.Set(0.0, 0.0, 0.0, 1.0);
	m_Diffuse.Set(1.0, 1.0, 1.0, 1.0);
	m_Specular.Set(1.0, 1.0, 1.0, 1.0);

	m_Position[0] = 0.0;
	m_Position[1] = 0.0;
	m_Position[2] = 1.0;
	m_Position[3] = 0.0;

	m_SpotDir[0] = 0.0;
	m_SpotDir[1] = 0.0;
	m_SpotDir[2] = -1.0;

	m_fSpotExp = 0.0;
	m_fSpotCutOff = 180.0;

	m_fConstAtt = 1.0;
	m_fLinAtt = 0.0;
	m_fQuadAtt = 0.0;
}

//////////////////////////////////////////////////////////////////////
// Apply

bool COGLLight::Apply(int iMode , void *pvData )
{
	glLightfv(m_eLight, GL_AMBIENT, m_Ambient.Data());
	glLightfv(m_eLight, GL_DIFFUSE, m_Diffuse.Data());
	glLightfv(m_eLight, GL_SPECULAR, m_Specular.Data());

	glLightfv(m_eLight, GL_POSITION, m_Position.Data());
	glLightfv(m_eLight, GL_SPOT_DIRECTION, m_SpotDir.Data());

	glLightf(m_eLight, GL_SPOT_EXPONENT, m_fSpotExp);
	glLightf(m_eLight, GL_SPOT_CUTOFF, m_fSpotCutOff);

	glLightf(m_eLight, GL_CONSTANT_ATTENUATION, m_fConstAtt);
	glLightf(m_eLight, GL_LINEAR_ATTENUATION, m_fLinAtt);
	glLightf(m_eLight, GL_QUADRATIC_ATTENUATION, m_fQuadAtt);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Apply

bool COGLLight::Apply(GLenum eWhat)
{
	if (eWhat == GL_AMBIENT) glLightfv(m_eLight, GL_AMBIENT, m_Ambient.Data());
	else if (eWhat == GL_DIFFUSE) glLightfv(m_eLight, GL_DIFFUSE, m_Diffuse.Data());
	else if (eWhat == GL_SPECULAR) glLightfv(m_eLight, GL_SPECULAR, m_Specular.Data());
	else if (eWhat == GL_POSITION) glLightfv(m_eLight, GL_POSITION, m_Position.Data());
	else if (eWhat == GL_SPOT_DIRECTION) glLightfv(m_eLight, GL_SPOT_DIRECTION, m_SpotDir.Data());
	else if (eWhat == GL_SPOT_EXPONENT) glLightf(m_eLight, GL_SPOT_EXPONENT, m_fSpotExp);
	else if (eWhat == GL_SPOT_CUTOFF) glLightf(m_eLight, GL_SPOT_CUTOFF, m_fSpotCutOff);
	else if (eWhat == GL_CONSTANT_ATTENUATION) glLightf(m_eLight, GL_CONSTANT_ATTENUATION, m_fConstAtt);
	else if (eWhat == GL_LINEAR_ATTENUATION) glLightf(m_eLight, GL_LINEAR_ATTENUATION, m_fLinAtt);
	else if (eWhat == GL_QUADRATIC_ATTENUATION) glLightf(m_eLight, GL_QUADRATIC_ATTENUATION, m_fQuadAtt);

	return true;
}
