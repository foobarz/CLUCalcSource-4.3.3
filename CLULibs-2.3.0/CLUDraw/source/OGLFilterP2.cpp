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


// OGLFilterP2.cpp: Implementierung der Klasse COGLFilterP2.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OGLFilterP2.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLFilterP2::COGLFilterP2()
{

}

COGLFilterP2::~COGLFilterP2()
{

}

//////////////////////////////////////////////////////////////////////
/// Draw embedded E2 Plane

bool COGLFilterP2::DrawE2Plane(float fSize, const COGLColor& rCol)
{
	if (fSize <= 0.0f || fSize > OGL_VEX_COORD_MAX)
		return false;

	COGLVertex xP, xA, xB;
	COGLColor Col;

	Col = rCol;
	Col.Apply();

	m_ActiveColor = Col;

	xP.Set(0.0, 0.0, 1.0);
	xA.Set(fSize, 0.0, 0.0);
	xB.Set(0.0, fSize, 0.0);

	DrawPlane(xP, xA, xB);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Create P2 Rotor.

MultiV<float> COGLFilterP2::Rotor(const MultiV<float>& vX, float fRad, bool bAxis)
{
	MultiV<float> vA(vX.GetStyle());

	if (bAxis)
	{
		// Assume vX is rotation axis
		vA = vX(1);

		vA /= float(sqrt(Scalar(vA * vA)));
		vA &= PGA<float>::vE3I; // Rotation plane is U = x I3, and x = U I3^(-1)
	}
	else
	{
		// Assume vX is rotation plane
		vA = vX(2);
	
		vA /= float(sqrt(Scalar(vA * (~vA))));
	}

	vA = float(cos(fRad/2.0)) - (float(sin(fRad/2.0)) & vA);

	vA.TinyToZero();

	return vA;
}


//////////////////////////////////////////////////////////////////////
/// Draw Multivector

bool COGLFilterP2::DrawMV(const MultiV<float>& vX, bool bAnalyzeOnly)
{
	MultiV<float> vA(vX);
	Mem<uint> mGList;
	int iGrades;

	vA.TinyToZero(m_fSensitivity);

	if (!GradeList<float>(vA, mGList, 0)) return false;
	if ((iGrades = mGList.Count()) == 0 || iGrades > 2) return false;

	if (iGrades == 1)
	{
		return DrawBlade(vA);
	}
	else if (iGrades == 2 && mGList[0] == 2 && mGList[1] == 0)
	{
		return DrawRotor(vA);
	}

	return false;
}


//////////////////////////////////////////////////////////////////////
/// Draw Rotor

bool COGLFilterP2::DrawRotor(const MultiV<float>& vA)
{
	Mem<int> mIdx;
	MultiV<float> vU, vX, vR;
	MemObj<MultiV<float> > mvList;
	COGLVertex xA, xB, xNull;

	// Get Bivector Part
	vU = vA(2);

	float fAngle;

	// Angle
	fAngle = 2.0f * (float) acos(Scalar(vA));

	// Rotation axis
	vX = vU / (float) sqrt(Scalar(vU * (~vU)));
	vX &= PGA<float>::vE3I;

	xNull.Set(0.0, 0.0, 0.0);
	xA.Set(vX[1], vX[2], vX[4]);

	COGLMVFilterBase::DrawRotor(xNull, xA, fAngle);
	
	return true;
}



//////////////////////////////////////////////////////////////////////
/// Draw Blade

bool COGLFilterP2::DrawBlade(const MultiV<float>& vA)
{
	float pfVec[4][10];
//	float pfVal[3][3];

	COGLVertex pPVex[3];
	COGLVertex pEVex[3];
	COGLVertex NullVex;

	MultiV<float> vX(vA);
	MemObj<MultiV<float> > mvList;
	COGLColor Dark;
	int piOrder[] = { 3, 2, 1, 0 };
	int piIdxLine[] = { 1, 2, 3, 4, 0, 1, 0, 2, 1, 3, 2, 4 };
	float pfNull[] = { 0.0f, 0.0f, 0.0f };

	NullVex = pfNull;

	m_VexList.Reset();

	Dark = m_fBackFaceColFac * m_ActiveColor;

	float fMag;

	FactorBlade<float>(vA, mvList, false, 0, 0, m_fSensitivity);
	fMag = (float) sqrt(Scalar(vA * (~vA)));

	int i, n = mvList.Count();

	if (n < 1 || n > 3)
		return false;

	for(i=0;i<n;i++)
	{
		GetVector(mvList[i], 1, pfVec[i]);
		pPVex[i].Set(pfVec[i][0], pfVec[i][1], pfVec[i][3]);
		pEVex[i].Set(pfVec[i][0], pfVec[i][1], pfVec[i][3]);

		if (i > 0)
		{
			pPVex[i] /= pPVex[i].Mag();
			pEVex[i] /= pEVex[i].Mag();
		}
	}

	if (pfVec[0][3] != 0.0f)
	{
		for(i=0;i<n;i++)
			pEVex[i] /= pfVec[0][3];
	}

	if (n == 1)
	{
		if (pfVec[0][3] == 0.0f)
		{
			DrawVector(NullVex, pPVex[0]);
		}
		else
		{
			DrawPoint(pEVex[0]);
			DrawPoint(pPVex[0]);
			
			EnableLineStipple();
			DrawLine(NullVex, pEVex[0], true);
			DrawLine(NullVex, pPVex[0], true);
			EnableLineStipple(false);
		}
	}
	else if (n == 2)
	{
		fMag = 0.5f * fMag;

		pPVex[1] *= fMag;
		pEVex[1] *= fMag;// / (pfVec[0][3] != 0.0f ? pfVec[0][3] : 1.0f);

		m_VexList.SetMode(GL_LINES);

		m_VexList.AddVex(pfNull);
		m_VexList.AddCol(m_ActiveColor);

		m_VexList.AddVex(pPVex[0] - pPVex[1]);
		m_VexList.AddCol(Dark);

		m_VexList.AddVex(pPVex[0] + pPVex[1]);
		m_VexList.AddCol(Dark);

		m_VexList.AddVex(pEVex[0] - pEVex[1]);
		m_VexList.AddCol(m_ActiveColor);
		m_VexList.AddVex(pEVex[0] + pEVex[1]);
		m_VexList.AddCol(m_ActiveColor);

		if (m_bUseLighting)
		{
			m_mMaterial[MAT_LINE].Apply();

			m_VexList.AddNormal(pfNull);

			m_VexList.AddNormal(-pPVex[1]);
			m_VexList.AddNormal(pPVex[1]);

			m_VexList.AddNormal(-pEVex[1]);
			m_VexList.AddNormal(pEVex[1]);
		}
		m_VexList.SetIdxList(4, piIdxLine);

		m_VexList.Apply();

		EnableLineStipple();
		m_VexList.SetIdxList(8, &piIdxLine[4]);
		m_VexList.Apply();
		EnableLineStipple(false);
	}
	else if (n == 3)
	{
		fMag = (float) pow(double(fMag), 1.0/3.0);
		DrawCube(fMag);
	}

	return true;
}

