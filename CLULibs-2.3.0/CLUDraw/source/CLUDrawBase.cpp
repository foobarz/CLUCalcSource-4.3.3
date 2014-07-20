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

#include "StdAfx.h"
#include "CLUDrawBase.h"
#include "stdlib.h"
#include "math.h"

#ifdef _GNUCPP3_
#   include <unistd.h>
#   define _getcwd getcwd
#   define _chdir chdir
#else
#   include <direct.h>
#   include <io.h>
#endif

#include "IL/il.h"
#include "IL/ilu.h"
#include "IL/ilut.h"

extern "C"
{
	int ConvertJPEGtoPS(const char* pcSrcFilename, const char* pcTrgFilename);
}

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCLUDrawBase::CCLUDrawBase()
{	
	m_bMouseLDown = false;;
	m_bMouseRDown = false;
	m_bShiftDown = false;
	m_bCtrlDown = false;
	m_bAltDown = false;
	m_b2dView = false;

	m_bUseLighting = false;

	m_bUseLocalRot = false;

	m_bIsAnimated = false;
	m_iAnimRotType = 0;

	m_iPosX = 0;
	m_iPosY = 0;

	m_vRMain = m_E3Base.vSC;
	m_vU = m_E3Base.vSC;
	m_vAxis = m_E3Base.vSC;

	m_vU = 0;
	m_vAxis = 0;

	m_sPer.Set(70.0f, 1.0f, 0.1f, 100.0f);
	m_sOrtho.Set(-5, 5, -5, 5, -1, 10);
	m_fOrthoFac = 0.01f;
	m_fOrthoZoomFac = 1e-4f;

	m_iAnimPause = 20;

	m_iRTMode = 0;
	m_fFrameAngle = 0;
	m_mTransform.Set(10);

	int i=0;
	for(i=0;i<10;i++)
	{
		m_mTransform[i].Reset();
	}
	m_mTransform[0].pfTrans[2] = -5.0;

	Red.Set(1.0f, 0.0f, 0.0f);
	Green.Set(0.0f, 1.0f, 0.0f);
	Blue.Set(0.0f, 0.0f, 1.0f);

	Cyan.Set(0.0f, 1.0f, 1.0f);
	Yellow.Set(1.0f, 1.0f, 0.0f);
	Magenta.Set(1.0f, 0.0f, 1.0f);

	Orange.Set(1.0f, 0.7f, 0.0f);
	White.Set(1.0f, 1.0f, 1.0f);
	Black.Set(0.0f, 0.0f, 0.0f);

	DRed = 0.3f * Red;
	MRed = 0.6f * Red;

	DBlue = 0.3f * Blue;
	MBlue = 0.6f * Blue;

	DGreen = 0.3f * Green;
	MGreen = 0.6f * Green;

	DCyan = 0.3f * Cyan;
	MCyan = 0.6f * Cyan;

	DYellow = 0.3f * Yellow;
	MYellow = 0.6f * Yellow;

	DMagenta = 0.3f * Magenta;
	MMagenta = 0.6f * Magenta;

	DOrange = 0.3f * Orange;
	MOrange = 0.6f * Orange;

	DGray = 0.3f * White;
	MGray = 0.6f * White;

	m_BGCol.Set(0.0f, 0.0f, 0.0f, 1.0f);
	m_BoxCol.Set(0.5f, 0.5f, 0.5f, 1.0f);
	m_BoxXCol.Set(0.6f, 0.0f, 0.0f, 1.0f);
	m_BoxYCol.Set(0.0f, 0.6f, 0.0f, 1.0f);
	m_BoxZCol.Set(0.0f, 0.0f, 0.6f, 1.0f);

	m_fPi = float(asin(1.0) * 2.0);
	m_fRadPerDeg = m_fPi / 180.0f;

	m_fRotFac = 2.0f;
	m_fTransFac = 0.05f;

	m_iSaveScreenNo = 0;
	m_fTimeStep = 0;
	m_fTotalTime = 0;
	m_uAnimateTimeStep = 0;

	for(i=0;i<MRHLEN;i++)
	{
		m_pvMouseRotHist[i] = m_E3Base.vZero;
	}
	m_iMRHPos = 0;
	m_bMRHChanged = false;

	// Initialize Image Library
	ilInit();
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
}

CCLUDrawBase::~CCLUDrawBase()
{

}


//////////////////////////////////////////////////////////////////////
/// Adjust Rotation Angle

inline void CCLUDrawBase::AdjustRotAngle(float& fDeg)
{
	if (fDeg >= 360.0f) fDeg -= 360.0f;
	else if (fDeg < 0) fDeg = 360.0f + fDeg;
}

//////////////////////////////////////////////////////////////////////
/// Set Animation Time Step

bool CCLUDrawBase::SetAnimationTimeStep(int iTimeStep)
{
	if (iTimeStep <= 0)
		return false;

	m_iAnimPause = iTimeStep;
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Init Function
///
/// Initialises Camera and other settings

bool CCLUDrawBase::Init()
{
	float *pfCol = m_BGCol.Data();

	glClearColor(pfCol[0], pfCol[1], pfCol[2], pfCol[3]);
	glEnable( GL_DEPTH_TEST		);
	glEnable( GL_CULL_FACE		);
	
	//glEnable( GL_POINT_SMOOTH	);
	//glEnable( GL_LINE_SMOOTH	);
	//glEnable( GL_POLYGON_SMOOTH	);
/*
#ifdef CLUDRAW_USE_VEX_ARRAY
	glEnableClientState( GL_VERTEX_ARRAY );
#endif
*/
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	m_sPer.fAspect = float(m_iSizeX) / float(m_iSizeY);

	m_sOrtho.fLeft = -float(m_iSizeX) * 0.5f * m_fOrthoFac;
	m_sOrtho.fRight = float(m_iSizeX) * 0.5f * m_fOrthoFac;
	m_sOrtho.fBottom = -float(m_iSizeY) * 0.5f * m_fOrthoFac;
	m_sOrtho.fTop = float(m_iSizeY) * 0.5f * m_fOrthoFac;
	m_sOrtho.fNear = -100.0f;
	m_sOrtho.fFar = 100.0f;

	gluPerspective(m_sPer.fAngle, m_sPer.fAspect, m_sPer.fNear, m_sPer.fFar);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glShadeModel(GL_SMOOTH);
	
	if (m_bUseLighting)
	{
		glEnable(GL_LIGHTING);
		
		m_StdMaterial.Face(GL_FRONT);
		m_StdMaterial.Ambient(0.5f, 0.5f, 0.5f, 1.0f);
		m_StdMaterial.Diffuse(1.0f, 1.0f, 1.0f, 1.0f);
		m_StdMaterial.Specular(0.8f, 0.8f, 0.8f, 1.0f);
		m_StdMaterial.Shininess(50.0f);
		m_StdMaterial.Apply();

		ResetLights();

		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT, GL_AMBIENT);		
	}

	//glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);

	ilutInit();
	glDisable(GL_TEXTURE_2D);

	return true;
}


bool CCLUDrawBase::ResetLights(bool bFixation)
{
	int i;

	m_StdLight.LightNo(0);
	m_StdLight.Ambient(0.5f, 0.5f, 0.5f, 0.0f);
	m_StdLight.Diffuse(0.8f, 0.8f, 0.8f, 0.0f);
	m_StdLight.Specular(0.2f, 0.2f, 0.2f, 0.0f);
	m_StdLight.Position(5.0f, 5.0f, 0.0f, 1.0f);
	m_StdLight.ConstAtt(0.0f);
	m_StdLight.LinAtt(0.2f);
	m_StdLight.QuadAtt(0.0f);
	m_StdLight.Enable();
	m_StdLight.Apply();

	for(i=0;i<CLUDRAWBASE_MAXLIGHTCOUNT;i++)
	{
		m_SurLight[i] = m_StdLight;
		m_SurLight[i].LightNo(i+1);
		m_SurLight[i].Diffuse(0.4f, 0.4f, 0.4f, 0.0f);
		m_SurLight[i].Specular(0.7f, 0.7f, 0.7f, 0.0f);
		m_SurLight[i].Enable(false);
		if (bFixation)
			m_bSurLightPosFixed[i] = false;
	}
	/*
	m_SurLight[0].Position(-10.0f, 0.0f, -1.0f, 1.0f);
	m_SurLight[1].Position(10.0f, 0.0f, -1.0f, 1.0f);
	m_SurLight[2].Position(0.0f, 15.0f, -1.0f, 1.0f);
	m_SurLight[3].Position(0.0f, -15.0f, -1.0f, 1.0f);
	*/

	for(i=0;i<CLUDRAWBASE_MAXLIGHTCOUNT;i++)
	{
		m_SurLight[i].Apply();
	}

	float pfAmbient[] = { 0.5f, 0.5f, 0.5f, 0.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, pfAmbient);

	return true;
}

COGLColor CCLUDrawBase::GetBGColor()
{
	return m_BGCol;
}

void CCLUDrawBase::SetBGColor(COGLColor &rBGCol)
{
	m_BGCol = rBGCol;
	float *pfCol = m_BGCol.Data();

	glClearColor(pfCol[0], pfCol[1], pfCol[2], 0.0f);
//	glClearColor(pfCol[0], pfCol[1], pfCol[2], pfCol[3]);

}

void CCLUDrawBase::SetBoxColor(COGLColor &rCol, COGLColor &rXCol, 
							   COGLColor &rYCol, COGLColor &rZCol)
{
	m_BoxCol = rCol;
	m_BoxXCol = rXCol;
	m_BoxYCol = rYCol;
	m_BoxZCol = rZCol;
}

bool CCLUDrawBase::SetMouseMode(int iMode)
{
	return SetRTMode(iMode);
}

bool CCLUDrawBase::GetMouseMode(int &iMode)
{
	iMode = GetActRTMode();
	return true;
}

bool CCLUDrawBase::SetOGLSize(double dWidth, double dHeight)
{
	//CCLUDrawBase::Reshape(iWidth, iHeight);
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Virtual Callback functions

//////////////////////////////////////////////////////////////////////
/// Begin Draw

bool CCLUDrawBase::BeginDraw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	if (m_bUseLighting)
	{
		ResetLights(false);

		int i;
		m_StdLight.Apply();
		for(i=0;i<CLUDRAWBASE_MAXLIGHTCOUNT;i++)
		{
			if (m_bSurLightPosFixed[i])
				m_SurLight[i].Apply();
		}
	}


	STransform &rTrans = m_mTransform[0];

	glTranslatef(rTrans.pfTrans[0], rTrans.pfTrans[1], rTrans.pfTrans[2]);
	//printf("Trans: %g, %g, %g\n", rTrans.pfTrans[0], rTrans.pfTrans[1], rTrans.pfTrans[2]);

	if (m_bUseLocalRot)// && !(m_bIsAnimated && m_iAnimRotType))
	{
		glRotatef(rTrans.pfRot[2], 1, 0, 0);
		glRotatef(rTrans.pfRot[0], 0, 1, 0);
		glRotatef(rTrans.pfRot[1], 0, 0, 1);
	}
	else
	{
		m_fFrameAngle = float(2.0 * acos(Scalar(m_vRMain))) / m_fRadPerDeg;
		m_vAxis = (m_vRMain(2)) & m_E3Base.vI;
		//printf("t, A : %f, %s\n", fAngle, m_vAxis.Str());

		rTrans.pfRot[0] = m_vAxis[1];
		rTrans.pfRot[1] = m_vAxis[2];
		rTrans.pfRot[2] = m_vAxis[3];

		glRotatef(m_fFrameAngle, m_vAxis[1], m_vAxis[2], m_vAxis[3]);
	}

	glListBase(0);
/*
// Old Mouse Control Mechanism

	glRotatef(rTrans.pfRot[1], 0.0, 1.0, 0.0);
	glRotatef(rTrans.pfRot[0], 1.0, 0.0, 0.0);
	glRotatef(rTrans.pfRot[2], 0.0, 0.0, 1.0);
*/
	return true;
}

//////////////////////////////////////////////////////////////////////
/// End Draw

bool CCLUDrawBase::EndDraw()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Enable 2D-View

void CCLUDrawBase::Enable2dView(bool bVal)
{
	m_b2dView = bVal;

	if (m_b2dView)
	{
		// Reset original Ortho factor and update Ortho vars.
		m_fOrthoFac = 0.01f;
		m_sOrtho.fLeft = -float(m_iSizeX) * 0.5f * m_fOrthoFac;
		m_sOrtho.fRight = float(m_iSizeX) * 0.5f * m_fOrthoFac;
		m_sOrtho.fBottom = -float(m_iSizeY) * 0.5f * m_fOrthoFac;
		m_sOrtho.fTop = float(m_iSizeY) * 0.5f * m_fOrthoFac;
		m_sOrtho.fNear = -100.0f;
		m_sOrtho.fFar = 100.0f;
	}

	UpdateMatrices();

	m_mTransform[0].Reset();
	ResetTransformVars();
	if (!m_b2dView)
		m_mTransform[0].pfTrans[2] = -5.0;

	m_vRMain = m_E3Base.vSC;
	m_vU = m_E3Base.vSC;
	m_vAxis = m_E3Base.vSC;

	m_vU = 0;
	m_vAxis = 0;
	
	//if (!m_bIsAnimated)
	//	Display();
}


//////////////////////////////////////////////////////////////////////
/// Update Matrices

void CCLUDrawBase::UpdateMatrices()
{
	int i;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (m_b2dView)
	{
		glOrtho(m_sOrtho.fLeft, m_sOrtho.fRight, m_sOrtho.fBottom, m_sOrtho.fTop,
				m_sOrtho.fNear, m_sOrtho.fFar);
	}
	else
	{
		gluPerspective(m_sPer.fAngle, m_sPer.fAspect, m_sPer.fNear, m_sPer.fFar);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
		
	if (m_bUseLighting)
	{
		m_StdLight.Apply((GLenum) GL_POSITION);
		for(i=0;i<CLUDRAWBASE_MAXLIGHTCOUNT;i++)
		{
			if (m_bSurLightPosFixed[i])
				m_SurLight[i].Apply((GLenum) GL_POSITION);
		}
	}
}

//////////////////////////////////////////////////////////////////////
/// Set Perspective Angle

void CCLUDrawBase::SetPerspectiveAngle(GLfloat fAngle, bool bUpdateMatrices)
{ 
	m_sPer.Set(fAngle, float(m_iSizeX) / float(m_iSizeY), m_sPer.fNear, m_sPer.fFar); 

	if (bUpdateMatrices)
		UpdateMatrices();
}


//////////////////////////////////////////////////////////////////////
/// Set Perspective

void CCLUDrawBase::SetPerspective(GLfloat fAngle, GLfloat fNear, GLfloat fFar)
{ 
	m_sPer.Set(fAngle, float(m_iSizeX) / float(m_iSizeY), fNear, fFar); 
	UpdateMatrices();

	if (!m_bIsAnimated)
		Display();
}

//////////////////////////////////////////////////////////////////////
/// Set Orthographic Perspective

void CCLUDrawBase::SetOrtho(GLfloat fLeft, GLfloat fRight, GLfloat fBottom, GLfloat fTop, 
		      GLfloat fNear, GLfloat fFar)
{
	m_sOrtho.Set(fLeft, fRight, fBottom, fTop, fNear, fFar);
	UpdateMatrices();

	if (!m_bIsAnimated)
		Display();
}


//////////////////////////////////////////////////////////////////////
/// Draw Coordinate Frame Box
void CCLUDrawBase::DrawBox(float fSize)
{
	glDisable( GL_CULL_FACE		);

	//if (m_bUseLighting)
	//{
 //	    m_StdMaterial.Diffuse(1.0, 1.0, 1.0, 1.0);
	//    m_StdMaterial.Ambient(m_BoxCol);
	//	m_StdMaterial.Apply();
	//}
	float fS = fSize/2;

	//m_BoxCol.Apply();
	glBegin(GL_LINES);// front side

	if (m_bUseLighting)
	{
	    m_BoxXCol.Apply();
 	    m_StdMaterial.Diffuse(1.0, 1.0, 1.0, 1.0);
	    m_StdMaterial.Ambient(m_BoxXCol);
		m_StdMaterial.Apply();
		glNormal3f(-1, 0, 0);
		glVertex3f(-fS, -fS , -fS);
		glNormal3f(1, 0, 0);
		glVertex3f(fS, -fS, -fS);

		m_BoxCol.Apply();
 	    m_StdMaterial.Diffuse(1.0, 1.0, 1.0, 1.0);
	    m_StdMaterial.Ambient(m_BoxCol);
		m_StdMaterial.Apply();
		glNormal3f(0, -1, 0);
		glVertex3f(fS, -fS, -fS);
		glNormal3f(0, 1, 0);
		glVertex3f(fS, fS, -fS);

		glNormal3f(1, 0, 0);
		glVertex3f(fS, fS, -fS);
		glNormal3f(-1, 0, 0);
		glVertex3f(-fS, fS, -fS);

		m_BoxYCol.Apply();
 	    m_StdMaterial.Diffuse(1.0, 1.0, 1.0, 1.0);
	    m_StdMaterial.Ambient(m_BoxYCol);
		m_StdMaterial.Apply();
		glNormal3f(0, 1, 0);
		glVertex3f(-fS, fS, -fS);
		glNormal3f(0, -1, 0);
		glVertex3f(-fS, -fS, -fS);
	}
	else
	{
		m_BoxXCol.Apply();
		glVertex3f(-fS, -fS , -fS);
		glVertex3f(fS, -fS, -fS);

		m_BoxCol.Apply();
		glVertex3f(fS, -fS, -fS);
		glVertex3f(fS, fS, -fS);

		glVertex3f(fS, fS, -fS);
		glVertex3f(-fS, fS, -fS);

		m_BoxYCol.Apply();
		glVertex3f(-fS, fS, -fS);
		glVertex3f(-fS, -fS, -fS);
	}

	glEnd();
	
	
 	m_StdMaterial.Diffuse(1.0, 1.0, 1.0, 1.0);
	m_StdMaterial.Ambient(m_BoxCol);
	m_StdMaterial.Apply();
	m_BoxCol.Apply();
	//glColor3f(0.8f, 0.8f, 0.8f);

	glBegin(GL_LINES);// back side

	if (m_bUseLighting)
	{
		glNormal3f(-1, 0, 0);
		glVertex3f(-fS, -fS, fS);
		glNormal3f(1, 0, 0);
		glVertex3f(fS, -fS, fS);

		glNormal3f(0, -1, 0);
		glVertex3f(fS, -fS, fS);
		glNormal3f(0, 1, 0);
		glVertex3f(fS, fS, fS);

		glNormal3f(1, 0, 0);
		glVertex3f(fS, fS, fS);
		glNormal3f(-1, 0, 0);
		glVertex3f(-fS, fS, fS);

		glNormal3f(0, 1, 0);
		glVertex3f(-fS, fS, fS);
		glNormal3f(0, -1, 0);
		glVertex3f(-fS, -fS, fS);
	}
	else
	{
		glVertex3f(-fS, -fS, fS);
		glVertex3f(fS, -fS, fS);

		glVertex3f(fS, -fS, fS);
		glVertex3f(fS, fS, fS);

		glVertex3f(fS, fS, fS);
		glVertex3f(-fS, fS, fS);

		glVertex3f(-fS, fS, fS);
		glVertex3f(-fS, -fS, fS);
	}

	glEnd();
	
	glBegin(GL_LINES);// connecting lines

	if (m_bUseLighting)
	{
 	    m_StdMaterial.Diffuse(1.0, 1.0, 1.0, 1.0);
	    m_StdMaterial.Ambient(m_BoxZCol);
		m_StdMaterial.Apply();
		m_BoxZCol.Apply();

		glNormal3f(0, 0, -1);
		glVertex3f(-fS, -fS , -fS);
		glNormal3f(0, 0, 1);
		glVertex3f(-fS, -fS , fS);

 	    m_StdMaterial.Diffuse(1.0, 1.0, 1.0, 1.0);
	    m_StdMaterial.Ambient(m_BoxCol);
		m_StdMaterial.Apply();
		m_BoxCol.Apply();
		glNormal3f(0, 0, -1);
		glVertex3f(fS, -fS, -fS);
		glNormal3f(0, 0, 1);
		glVertex3f(fS, -fS, fS);

		glNormal3f(0, 0, -1);
		glVertex3f(fS, fS, -fS);
		glNormal3f(0, 0, 1);
		glVertex3f(fS, fS, fS);

		glNormal3f(0, 0, -1);
		glVertex3f(-fS, fS, -fS);
		glNormal3f(0, 0, 1);
		glVertex3f(-fS, fS, fS);
	}
	else
	{
		m_BoxZCol.Apply();
		glVertex3f(-fS, -fS , -fS);
		glVertex3f(-fS, -fS , fS);

		m_BoxCol.Apply();
		glVertex3f(fS, -fS, -fS);
		glVertex3f(fS, -fS, fS);

		glVertex3f(fS, fS, -fS);
		glVertex3f(fS, fS, fS);

		glVertex3f(-fS, fS, -fS);
		glVertex3f(-fS, fS, fS);
	}

	glEnd();

	glEnable( GL_CULL_FACE		);
}

//////////////////////////////////////////////////////////////////////
/// Draw Base Vectors
void CCLUDrawBase::DrawBase()
{
	glDisable( GL_CULL_FACE		);

	if (m_bUseLighting)
	{
		m_StdMaterial.Apply();

		glBegin(GL_LINES);

		glColor3f(1.0, 0.0, 0.0);

		glNormal3f(-1.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);

		glNormal3f(1.0, 0.0, 0.0);
		glVertex3f(1.0, 0.0, 0.0);
		
		glColor3f(0.0, 1.0, 0.0);
	
		glNormal3f(0.0, -1.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);

		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f(0.0, 1.0, 0.0);
		
		glColor3f(0.0, 0.0, 1.0);

		glNormal3f(0.0, 0.0, -1.0);
		glVertex3f(0.0, 0.0, 0.0);

		glNormal3f(0.0, 0.0, 1.0);
		glVertex3f(0.0, 0.0, 1.0);

		glEnd();
	}
	else
	{
		glBegin(GL_LINES);
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(1.0, 0.0, 0.0);
		
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 1.0, 0.0);
		
		glColor3f(0.0, 0.0, 1.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 1.0);
		glEnd();
	}

	glEnable( GL_CULL_FACE		);

}

//////////////////////////////////////////////////////////////////////
/// Display

void CCLUDrawBase::Display()
{
	// Need to overwrite this function

	BeginDraw();

	DrawBase();
		
	EndDraw();
}

//////////////////////////////////////////////////////////////////////
/// Reshape

void CCLUDrawBase::Reshape(int iWidth, int iHeight)
{
	SetViewport(0, 0, iWidth, iHeight);
}

void CCLUDrawBase::GetViewport(int &iX, int &iY, int &iW, int &iH)
{
	iX = m_iPosX;
	iY = m_iPosY;
	iW = m_iSizeX;
	iH = m_iSizeY;
}

bool CCLUDrawBase::SetViewport(int iX, int iY, int iW, int iH)
{
	if (iW <= 0 || iH <= 0)
		return false;

	glViewport(m_iPosX = iX, m_iPosY = iY, m_iSizeX = iW, m_iSizeY = iH);

	m_sPer.fAspect = float(m_iSizeX) / float(m_iSizeY);

	m_sOrtho.fLeft = -float(m_iSizeX) * 0.5f * m_fOrthoFac;
	m_sOrtho.fRight = float(m_iSizeX) * 0.5f * m_fOrthoFac;
	m_sOrtho.fBottom = -float(m_iSizeY) * 0.5f * m_fOrthoFac;
	m_sOrtho.fTop = float(m_iSizeY) * 0.5f * m_fOrthoFac;

	UpdateMatrices();

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Key

void CCLUDrawBase::Key(unsigned char cKey, int iX, int iY)
{
	/// Need to implement this function
	if (cKey == 'q' || cKey == 'Q')
		exit(0);

	if (cKey == 's' || cKey == 'S')
	{
		char pcName[30];
		sprintf(pcName, "OGLScreen%03d.bmp", m_iSaveScreenNo++);
		SaveScreen2BMP(pcName);
	}

	if (cKey == 'a' || cKey == 'A')
	{
		if (!m_bIsAnimated)
			StartAnimation(m_iAnimPause);
		else StopAnimation();
	}
	
	if (cKey == 'm' || cKey == 'M')
	{
		if (m_iAnimRotType !=4)
			 m_iAnimRotType = 4;  // ANIMROT_MOUSE
		else m_iAnimRotType =0;		
	}


	// Select Rotation mode
	if (cKey >= '0' && cKey <= '9')
	{
		SetRTMode(cKey - '0');
	}
}


//////////////////////////////////////////////////////////////////////
/// Key

bool CCLUDrawBase::SetRTMode(int iMode)
{
	if (iMode < 0 || iMode > 9)
		return false;

	m_mTransform[m_iRTMode].iFlags = 0;
	m_iRTMode = iMode;
	m_mTransform[m_iRTMode].iFlags = 0;

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Mouse

void CCLUDrawBase::Mouse(int iButton, int iState, int iX, int iY)
{
	GetModifiers();

	if (iButton == CLU_LEFT_BUTTON())
	{
		if (iState == CLU_DOWN())
		{
			m_bMouseLDown = true;
			m_sMousePos.iX = iX;
			m_sMousePos.iY = iY;

			if (!(m_bCtrlDown ? 0 : m_iRTMode))
				ResetMouseRotHist();
		}
		else if (iState == CLU_UP())
		{
			m_bMouseLDown = false;
			m_mTransform[m_iRTMode].iFlags = 0;
		}
	}
	else if (iButton == CLU_RIGHT_BUTTON())
	{
		if (iState == CLU_DOWN())
		{
			m_bMouseRDown = true;
			m_sMousePos.iX = iX;
			m_sMousePos.iY = iY;
		}
		else if (iState == CLU_UP())
		{
			m_bMouseRDown = false;
			m_mTransform[m_iRTMode].iFlags = 0;
		}
	}
}


//////////////////////////////////////////////////////////////////////
///	Active Mouse Move

void CCLUDrawBase::ActiveMouseMove(int iX, int iY)
{
	int iStepX = iX - m_sMousePos.iX;
	int iStepY = iY - m_sMousePos.iY;

	m_mTransform[0].iFlags = 0;

	if (!iStepX && !iStepY)
		return;

	int iActMode;

	// If ALT is pressed always rotate whole space
	iActMode = (m_bCtrlDown ? 0 : m_iRTMode);
	STransform &rTrans = m_mTransform[iActMode];
	rTrans.iFlags = 0;

	if (m_bMouseLDown)
	{
		if (iActMode == 0)
		{
			float fX, fY, fAngle;
			
			if (!m_b2dView)
			{
				if (!m_bShiftDown)
				{
					m_vU[5] = (fX = float(iStepX)); // iL2
					m_vU[4] = (fY = float(iStepY));  //iL1
					m_vU[6] = 0.0;
				}
				else
				{
					m_vU[4] = 0.0;
					m_vU[5] = (fX = float(iStepX)); // iL2
					m_vU[6] = (fY = float(iStepY));  //iL3
				}
			}
			else
			{
				m_vU[4] = 0.0;
				m_vU[5] = (fY = 0.0f); // iL2
				m_vU[6] = (fX = -float(iStepX));  //iL3
			}
			
			if (fX != 0.0f || fY != 0.0f)
			{
				if (m_bUseLocalRot)
				{
//					MultiV<float> vR, vdR;

					rTrans.pfRot[0] += fX * m_fRotFac;

					if (m_bShiftDown)
						rTrans.pfRot[1] += fY * m_fRotFac;
					else
						rTrans.pfRot[2] += fY * m_fRotFac;
/*
					vR = m_E3Base.Rotor(0,1,0, rTrans.pfRot[0]*m_fRadPerDeg) & 
									m_E3Base.Rotor(1,0,0, rTrans.pfRot[2]*m_fRadPerDeg);
					m_vU = vR & (~m_vRMain);
					m_vU = m_vU(2);
					m_vRMain = vR;

					m_pvMouseRotHist[m_iMRHPos] = (float(100) & m_vU);
					m_iMRHPos++;

					if (m_iMRHPos >= MRHLEN)
						m_iMRHPos = 0;

					m_bMRHChanged = true;
*/
				}
				else
				{
					m_pvMouseRotHist[m_iMRHPos] = m_vU;
					m_iMRHPos++;

					if (m_iMRHPos >= MRHLEN)
						m_iMRHPos = 0;

					m_bMRHChanged = true;

					fAngle = float(sqrt(fX*fX + fY*fY)) * m_fRotFac * m_fRadPerDeg;


					//				if (!m_bUseLocalRot)
					m_vRMain = m_E3Base.Rotor(m_vU, fAngle, false) & m_vRMain;
					//				else
					//m_vRMain = m_vRMain & m_E3Base.Rotor(m_vU, fAngle, false);
				}

				rTrans.SetRotChanged(0);
				rTrans.SetRotChanged(1);
				rTrans.SetRotChanged(2);
			}
		}
		else
		{
			// Old Mouse Control Mechanism
			if (iStepX)
			{
				rTrans.pfRot[0] += GLfloat(iStepX) * m_fRotFac;
				rTrans.SetRotChanged(0);
/*
				if (!m_b2dView)
				{
					rTrans.pfRot[1] += GLfloat(iStepX) * m_fRotFac;
					rTrans.SetRotChanged(1);
				}
				else
				{
					rTrans.pfRot[2] += GLfloat(iStepX) * m_fRotFac;
					rTrans.SetRotChanged(2);
				}
*/
			}
			
			if (iStepY)
			{
				if (m_bShiftDown)
				{
					rTrans.pfRot[1] += GLfloat(iStepY) * m_fRotFac;
					rTrans.SetRotChanged(1);
				}
				else
				{
					rTrans.pfRot[2] += GLfloat(iStepY) * m_fRotFac;
					rTrans.SetRotChanged(2);
				}
			}
		}

		AdjustRotAngle(rTrans.pfRot[0]);
		AdjustRotAngle(rTrans.pfRot[1]);
		AdjustRotAngle(rTrans.pfRot[2]);

		m_sMousePos.iX = iX;
		m_sMousePos.iY = iY;

		if (!m_bIsAnimated)
			Display();
	}
	else if (m_bMouseRDown)
	{
		if (iStepX)
		{
			rTrans.pfTrans[0] += GLfloat(iStepX) * m_fTransFac;
			rTrans.SetTransChanged(0);
		}
		
		if (iStepY)
		{
			if (!m_b2dView)
			{
				if (m_bShiftDown)
				{
					rTrans.pfTrans[1] -= GLfloat(iStepY) * m_fTransFac;
					rTrans.SetTransChanged(1);
				}
				else
				{
					rTrans.pfTrans[2] += GLfloat(iStepY) * m_fTransFac;
					rTrans.SetTransChanged(2);
				}
			}
			else
			{
				if (iActMode == 0)
				{
					// Zoom Ortho
					if (m_bShiftDown)
					{
						m_fOrthoFac -= GLfloat(iStepY) * m_fOrthoZoomFac;
						if (m_fOrthoFac < 0.001f)
							m_fOrthoFac = 0.001f;

						m_sOrtho.fLeft = -float(m_iSizeX) * 0.5f * m_fOrthoFac;
						m_sOrtho.fRight = float(m_iSizeX) * 0.5f * m_fOrthoFac;
						m_sOrtho.fBottom = -float(m_iSizeY) * 0.5f * m_fOrthoFac;
						m_sOrtho.fTop = float(m_iSizeY) * 0.5f * m_fOrthoFac;
						UpdateMatrices();

						rTrans.SetTransChanged(2);
					}
					else
					{
						rTrans.pfTrans[1] -= GLfloat(iStepY) * m_fTransFac;
						rTrans.SetTransChanged(1);
					}
				}
				else
				{
					//rTrans.pfTrans[1] -= GLfloat(iStepY) * m_fTransFac;
					//rTrans.SetTransChanged(1);
					if (m_bShiftDown)
					{
						rTrans.pfTrans[1] -= GLfloat(iStepY) * m_fTransFac;
						rTrans.SetTransChanged(1);
					}
					else
					{
						rTrans.pfTrans[2] += GLfloat(iStepY) * m_fTransFac;
						rTrans.SetTransChanged(2);
					}
				}
			}
		}

		m_sMousePos.iX = iX;
		m_sMousePos.iY = iY;

		if (!m_bIsAnimated)
			Display();
	}
}


//////////////////////////////////////////////////////////////////////
///	Menu

void CCLUDrawBase::Menu(int iIdx)
{
	return;
}


//////////////////////////////////////////////////////////////////////
/// Timed Function

bool CCLUDrawBase::TimedFunc() 
{
	float fAngle = m_fTimeStep * 20.0f * m_fRadPerDeg; 

	m_vU[4] = 0.0f;
	m_vU[5] = 1.0f;
	m_vU[6] = 0.0f;

	m_vRMain = m_E3Base.Rotor(m_vU, fAngle, false) & m_vRMain;

	m_mTransform[0].SetRotChanged(0);
	m_mTransform[0].SetRotChanged(1);
	m_mTransform[0].SetRotChanged(2);

	Display();

	return true;
}


/////////////////////////////////////////////////////////////////////
// Save Image as PGM-file
//
/// Function written by Bodo Rosenhahn. 3.12.2001

bool CCLUDrawBase::SaveScreen2PGM(const char *pcFilename)
{
	int iWidth, iHeight;
    FILE *pFile; 
	unsigned char *pImage;
	unsigned char *pImages;
	int i,j;

	iWidth = m_iSizeX; 
	iHeight =  m_iSizeY;
	// open Image / Get Memory
	if (!(pFile = fopen(pcFilename, "w"))) 
	{ 
		return false;
	} 
	if ((pImage = (unsigned char *)malloc(3*iWidth*iHeight*sizeof(unsigned char))) == NULL) 
	{
		fclose(pFile);
		return false;
	}
	if ((pImages = (unsigned char *)malloc(3*iWidth*iHeight*sizeof(unsigned char))) == NULL) 
	{
		fclose(pFile);
		free(pImage);
		return false;
	}
	// Get Pixels
	glPixelStorei(GL_PACK_ALIGNMENT,1);
	glReadBuffer(GL_BACK_LEFT);
	glReadPixels( 0,0,iWidth,iHeight,GL_RGB,GL_UNSIGNED_BYTE, pImage );
	// Header for PGM-File
	fwrite ("P6 \n", 4, 1, pFile ); 
	//fwrite ("500 500 \n", 9, 1, pFile ); 
	fprintf (pFile,"%d %d \n",iWidth, iHeight);
	fwrite ("255 \n",5, 1, pFile ); 
	// Reorder Bytes
	for(i=0;i<iHeight;i++)
		for(j=0;j<iWidth*3;j=j+3)
		{
			pImages[iWidth*3*(iHeight-(i+1))+(j+1)]=pImage[iWidth*3*i+j];
			pImages[iWidth*3*(iHeight-(i+1))+(j+2)]=pImage[iWidth*3*i+(j+1)];
			pImages[iWidth*3*(iHeight-(i+1))+(j+0)]=pImage[iWidth*3*i+(j+2)];
		}
	fwrite  ( pImages , iWidth * iHeight * 3, 1 , pFile );  
	fclose ( pFile); 
//	printf("Image saved under '%s'.\n", cFilename);

	return true;
}


/////////////////////////////////////////////////////////////////////
// Save Image in file (any format supported by DevIL)
//

bool CCLUDrawBase::SaveScreen2BMP(const char *pcFilename)
{
	ILuint uImgID;
	int iWidth, iHeight;
	unsigned char *pImage;
	cStr csFilename, csBaseName, csExt;
	int iExtPos;
	bool bEPSFile = false;
	bool bJPS = false;

	csBaseName = csFilename = pcFilename;

	iExtPos = ('.' < csBaseName);
	csExt = csBaseName.Last(csFilename.Len() - iExtPos, 1);
	csBaseName |= -1;

	if (csExt == "eps")
	{
		csFilename = csBaseName + ".tmp";
		bEPSFile = true;
	}
	else if (csExt == "jps")
	{
		csFilename = csBaseName + ".jpg";
		bEPSFile = true;
		bJPS = true;
	}

	iWidth = m_iSizeX; 
	iHeight =  m_iSizeY;

	ilGenImages(1, &uImgID);
	ilBindImage(uImgID);
	if (iluEnlargeCanvas(iWidth, iHeight, 1) == IL_FALSE)
	{
		ilDeleteImages(1, &uImgID);
		return false;
	}

	ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
	pImage = (unsigned char *) ilGetData();

	// Get Pixels
	glPixelStorei(GL_PACK_ALIGNMENT,1);
	glReadBuffer(GL_FRONT_LEFT);
	glReadPixels( 0, 0, iWidth, iHeight, GL_RGB, GL_UNSIGNED_BYTE, pImage );

	remove(csFilename.Str());

	if (bEPSFile)
	{
		if (ilSave(IL_JPG, (ILstring) csFilename.Str()) == IL_FALSE)
		{
			ilDeleteImages(1, &uImgID);
			return false;
		}
	}
	else
	{
		if (ilSaveImage((ILstring) csFilename.Str()) == IL_FALSE)
		{
			ilDeleteImages(1, &uImgID);
			return false;
		}
	}

	ilDeleteImages(1, &uImgID);

	if (bEPSFile)
	{
		csBaseName += ".eps";

		if (!ConvertJPEGtoPS(csFilename.Str(), csBaseName.Str()))
			return false;

		if (!bJPS)
			remove(csFilename.Str());
	}


	return true;
}

/*
bool CCLUDrawBase::SaveScreen2BMP(const char *pcFilename)
{
	int iWidth, iHeight;
    FILE *pFile; 
	BITMAPFILEHEADER FileHead;
	BITMAPINFOHEADER InfoHead;
	unsigned char *pImage;

	iWidth = m_iSizeX; 
	iHeight =  m_iSizeY;

	// open Image / Get Memory
	if (!(pFile = fopen(pcFilename, "wb"))) 
	{ 
		return false;
	} 

	if ((pImage = (unsigned char *)malloc(3*iWidth*iHeight*sizeof(unsigned char))) == NULL) 
	{
		fclose(pFile);
		return false;
	}

	// Get Pixels
	glPixelStorei(GL_PACK_ALIGNMENT,1);
	glReadBuffer(GL_FRONT_LEFT);
	glReadPixels( 0, 0, iWidth, iHeight, GL_RGB, GL_UNSIGNED_BYTE, pImage );

	//////////////////////////////////////////////////////////////////////////
	// Write BMP File Header
	FileHead.bfType = BITMAP_TYPE;
	FileHead.bfSize = 3 * iWidth * iHeight;	// 24-bit format
	FileHead.bfReserved1 = 0;
	FileHead.bfReserved2 = 0;
	FileHead.bfOffBits = 54;//sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	FileHead.bfSize += FileHead.bfOffBits;

	if (fwrite(&FileHead.bfType, sizeof(WORD), 1, pFile) != 1) return false;
	if (fwrite(&FileHead.bfSize, sizeof(DWORD), 1, pFile) != 1) return false;
	if (fwrite(&FileHead.bfReserved1, sizeof(WORD), 1, pFile) != 1) return false;
	if (fwrite(&FileHead.bfReserved2, sizeof(WORD), 1, pFile) != 1) return false;
	if (fwrite(&FileHead.bfOffBits, sizeof(DWORD), 1, pFile) != 1) return false;

	//////////////////////////////////////////////////////////////////////////
	// Write BMP Info Header
	InfoHead.biSize = sizeof(BITMAPINFOHEADER);
	InfoHead.biWidth = iWidth;
	InfoHead.biHeight = iHeight;
	InfoHead.biPlanes = 1;
	InfoHead.biBitCount = 24;
	InfoHead.biCompression = BI_RGB;
	InfoHead.biSizeImage = 3 * iWidth * iHeight;
	InfoHead.biXPelsPerMeter = 2880;
	InfoHead.biYPelsPerMeter = 2880;
	InfoHead.biClrUsed = 0;
	InfoHead.biClrImportant = 0;

	if (fwrite(&InfoHead.biSize, sizeof(DWORD), 1, pFile) != 1) return false;
	if (fwrite(&InfoHead.biWidth, sizeof(LONG), 2, pFile) != 2) return false;
	if (fwrite(&InfoHead.biPlanes, sizeof(WORD), 2, pFile) != 2) return false;
	if (fwrite(&InfoHead.biCompression, sizeof(DWORD), 2, pFile) != 2) return false;
	if (fwrite(&InfoHead.biXPelsPerMeter, sizeof(LONG), 2, pFile) != 2) return false;
	if (fwrite(&InfoHead.biClrUsed, sizeof(DWORD), 2, pFile) != 2) return false;
	
	//////////////////////////////////////////////////////////////////////////
	// Write BMP Data
	unsigned char *pBMPData;

	int x = 0, y = 0, fx = 0, fy = 0, nPixels;
	int uLineLength, uOffset;
	int uAbsOffset;
	int uPixType = InfoHead.biBitCount;
	
	nPixels = iWidth * iHeight;
	uLineLength = uPixType * iWidth;
	if ((uOffset = uLineLength % 32) != 0) 
		uLineLength += 32 - uOffset;
	uLineLength /= 16;

	if (!(pBMPData = new unsigned char[uLineLength<<1])) 
	{
		fclose(pFile);
		return false;
	}

	for(fy=0,y=0;fy<iHeight;fy++,y++)
	{
		uOffset = y * iWidth * 3;

		for(x=0,fx=0,uAbsOffset=uOffset;x<iWidth;x++,fx+=3)
		{
			pBMPData[fx+2] = pImage[uAbsOffset++];
			pBMPData[fx+1] = pImage[uAbsOffset++];
			pBMPData[fx]   = pImage[uAbsOffset++];
		}

		// Write Line
		if (int(fwrite(pBMPData, 2, uLineLength, pFile)) != uLineLength) break;

	} // for
	delete pBMPData;
	free(pImage);

	// Close File
	fclose ( pFile ); 

	return true;
}
*/


//////////////////////////////////////////////////////////////////////////
// Tool Functions


bool CCLUDrawBase::GetToolValue(const char *pcName, double &rVal)
{
	return false;
}

bool CCLUDrawBase::GetToolValue(const char *pcName, string &rVal)
{
	return false;
}

bool CCLUDrawBase::AddToolSlider(const char *pcName, double dMin, double dMax, double dStep,
								 double dVal, bool bFixed, const char *pcDesc)
{
	return false;
}

bool CCLUDrawBase::AddToolCounter(const char *pcName, double dMin, double dMax, double dStep,
								 double dLStep, double dVal, bool bFixed, const char *pcDesc)
{
	return false;
}

bool CCLUDrawBase::AddToolInput(const char *pcName, double dMin, double dMax,
								 double dVal, bool bFixed, const char *pcDesc)
{
	return false;
}

bool CCLUDrawBase::AddToolInputText(const char *pcName, bool bImmediateCallback, const char* pcInitText,
						  bool bFixed, const char *pcDesc, bool bDoUpdate)
{
	return false;
}


bool CCLUDrawBase::AddToolCheckBox(const char *pcName, bool bSet, 
								bool bFixed, const char *pcDesc)
{
	return false;
}

bool CCLUDrawBase::AddToolButton(const char *pcName,  
								bool bFixed, const char *pcDesc)
{
	return false;
}

bool CCLUDrawBase::AddToolChoice(const char *pcName, vector<string>& vecData, const char* pcCoice,
						bool bFixed, const char *pcDesc, bool bDoUpdate)
{
	return false;
}

// Info Window Functions
bool CCLUDrawBase::SetInfoText(const char *pcText)
{
	return false;
}

// Set Width of Info window in percent
bool CCLUDrawBase::SetInfoWidth(double dWidth)
{
	return false;
}
