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






//// IMPORTANT IMPORTANT IMPORTANT
/*
	Never write this:

		m_VexList.AddVex(m_VexList[0]);

	since internally the memory of m_VexList is enlarged, such that m_VexList[0]
	may not be a valid reference anymore!!!
*/




// OGLFilterBase.cpp: Implementierung der Klasse COGLFilterBase.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OGLMVFilterBase.h"
#include "OGLVexListE3.h"
//#include "OGLObjWireSphere.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLMVFilterBase::COGLMVFilterBase()
{
	m_fBackFaceColFac = 0.5f;
	m_fArrowColFac = 0.8f;

	m_fLineWidth = 1.5f;
	m_fPointSize = 6.0f;
	m_fPointAsSphereFac = 0.01f;

	m_fPi = 2.0f * (float) asin(1.0);

	m_fArrowLength = 0.15f;
	m_fArrowAngle = 15.0f / 180.0f * m_fPi;

	m_fSensitivity = 1e-7f;
	
	m_fTexRepFac = 0;	// Textures are not repeated
	m_bUseAbsTexCoords = false;

	m_bUseLighting = false;
	m_bUseTranslucency = false;

	m_mMaterial.Set(MAT_COUNT);

	m_mMaterial[MAT_STD].Face(GL_FRONT);
	m_mMaterial[MAT_STD].Specular(0.8f, 0.8f, 0.8f, 0.0f);
	m_mMaterial[MAT_STD].Shininess(70.0f);

	for(int i=1;i<MAT_COUNT;i++)
	{
		m_mMaterial[i] = m_mMaterial[MAT_STD];
	}

	m_mMaterial[MAT_SPHERE].Specular(0.6f, 0.6f, 0.6f, 0.0f);
	m_mMaterial[MAT_SPHERE].Ambient(0.0f, 0.0f, 0.0f, 0.0f);
	m_mMaterial[MAT_SPHERE].Diffuse(1.0f, 1.0f, 1.0f, 1.0f);

	ResetModes();
}

COGLMVFilterBase::~COGLMVFilterBase()
{
	m_dlSolidSphere1.Destroy();
	m_dlSolidSphereI1.Destroy();
	m_dlWireSphere1.Destroy();
	m_dlSolidSphere2.Destroy();
	m_dlSolidSphereI2.Destroy();
	m_dlWireSphere2.Destroy();
}

//////////////////////////////////////////////////////////////////////
/// Init OpenGL with Filter Data

bool COGLMVFilterBase::Init()
{
	glLineWidth(m_fLineWidth);
	glPointSize(m_fPointSize);

	m_dlSolidSphere1.Create();
	m_dlSolidSphere2.Create();
	m_dlSolidSphereI1.Create();
	m_dlSolidSphereI2.Create();
	m_dlWireSphere1.Create();
	m_dlWireSphere2.Create();

	InitSphereDisplayLists();

	return true;
}

//////////////////////////////////////////////////////////////////////
// Reset Modes

bool COGLMVFilterBase::ResetModes()
{
	m_eDrawPointType = PntDot;
	m_bShowImaginaryObjects = true;
	
	return true;
}

//////////////////////////////////////////////////////////////////////
// Generate Sphere Vertex List
/*
bool COGLMVFIlterBase::GenerateSphereVL(double dAngleStepTheta, double dAngleStepPhi)
{
	double dTheta, dPhi;
	double dPi = acos(-1);
	double dX, dY, dZ;

	int iThetaSteps = 2.0*dPi / dAngleStepTheta;
	int iPhiSteps = dPi / dAngleStepPhi;

	
	

	return true;
}

*/
//////////////////////////////////////////////////////////////////////
// Make Random Polygon Stipple Patterns

bool COGLMVFilterBase::MakeRandPolyStipple(double dLevel)
{
	int i;
	unsigned char ucVal;

	for(i=0;i<1024;i++)
	{
		ucVal =  (m_Random.ran3() < dLevel ? 0x01 : 0);
		ucVal += (m_Random.ran3() < dLevel ? 0x02 : 0);
		ucVal += (m_Random.ran3() < dLevel ? 0x04 : 0);
		ucVal += (m_Random.ran3() < dLevel ? 0x08 : 0);

		ucVal += (m_Random.ran3() < dLevel ? 0x10 : 0);
		ucVal += (m_Random.ran3() < dLevel ? 0x20 : 0);
		ucVal += (m_Random.ran3() < dLevel ? 0x40 : 0);
		ucVal += (m_Random.ran3() < dLevel ? 0x80 : 0);

		m_pcPolyStipple[i] = ucVal;
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Set Color

bool COGLMVFilterBase::SetColor(COGLColor& rCol)
{
	m_ActiveColor = rCol; 
	rCol.Apply();

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Mode

bool COGLMVFilterBase::SetMode(int iMode)
{
	switch(iMode)
	{
	case DRAW_POINT_AS_DOT:
		m_eDrawPointType = PntDot;
		break;

	case DRAW_POINT_AS_SPHERE:
		m_eDrawPointType = PntSphere;
		break;

	default:
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Enable Line Stipple

void COGLMVFilterBase::EnableLineStipple(bool bSet)
{
	if (bSet)
	{
		glEnable(GL_LINE_STIPPLE);
		glLineWidth(0.5);
		glLineStipple(1, 0x0F0F);
	}
	else
	{
		glLineWidth(m_fLineWidth);
		glDisable(GL_LINE_STIPPLE);
	}
}

//////////////////////////////////////////////////////////////////////
/// Draw Vertex List

bool COGLMVFilterBase::DrawVertexList(COGLVertexList& VexList)
{
	GLenum eVexMode;
	eVexMode = VexList.GetMode();

	m_ActiveColor.Apply();

	if (eVexMode == GL_TRIANGLE_STRIP)
	{
		if (m_bUseLighting)
		{
			m_mMaterial[MAT_CUBE].Ambient(m_ActiveColor);
			m_mMaterial[MAT_CUBE].Diffuse(1.0, 1.0, 1.0, m_ActiveColor.Data()[3]);
			m_mMaterial[MAT_CUBE].Apply();
		}

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);
		glDisable( GL_CULL_FACE		);
	}
	else if (eVexMode == GL_LINE_STRIP || eVexMode == GL_LINES)
	{
		if (m_bUseLighting)
		{
			m_mMaterial[MAT_LINE].Ambient(m_ActiveColor);
			m_mMaterial[MAT_LINE].Diffuse(1.0, 1.0, 1.0, m_ActiveColor.Data()[3]);
			m_mMaterial[MAT_LINE].Apply();
		}

		glDisable( GL_CULL_FACE		);
	}
	else
	{
		if (m_bUseLighting)
		{
			m_mMaterial[MAT_CUBE].Ambient(m_ActiveColor);
			m_mMaterial[MAT_CUBE].Diffuse(1.0, 1.0, 1.0, m_ActiveColor.Data()[3]);
			m_mMaterial[MAT_CUBE].Apply();
		}
	}

	VexList.Apply();

	if (eVexMode == GL_TRIANGLE_STRIP)
	{
		glDisable(GL_POLYGON_OFFSET_FILL);
		glEnable( GL_CULL_FACE		);
	}
	else if (eVexMode == GL_LINE_STRIP || eVexMode == GL_LINES)
	{
		glEnable( GL_CULL_FACE		);
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Draw Point

bool COGLMVFilterBase::DrawPoint(const COGLVertex& xP)
{

	if (m_eDrawPointType == PntSphere)
	{
		float fSize;
		glGetFloatv(GL_POINT_SIZE, &fSize);
		DrawSphere(xP, fSize * m_fPointAsSphereFac, true);
	}
	else
	{
		glDisable( GL_CULL_FACE		);

		if (m_bUseLighting)
		{
			COGLColor Em;

			Em = 0.3f * m_ActiveColor;

			m_mMaterial[MAT_POINT].Emission(Em);
			m_mMaterial[MAT_POINT].Ambient(m_ActiveColor);
			m_mMaterial[MAT_POINT].Apply();

			glBegin(GL_POINTS);
			glNormal3f(0.0, 0.0, 0.0);
			glVertex3fv(xP);
			glEnd();
		}
		else
		{
			glBegin(GL_POINTS);
			glVertex3fv(xP);
			glEnd();
		}

		glEnable( GL_CULL_FACE		);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Cube

bool COGLMVFilterBase::DrawCube(float fSize, const float *pX, const float *pY, const float *pZ)
{
	COGLVertex X, Y, Z, A;
	float fHSize = 0.5f * fSize;
	int piIdxQuads[] = { 0, 1, 2, 3, 4, 5, 6, 7, 
		0, 7, 6, 1, 1, 6, 5, 2, 2, 5, 4, 3, 3, 4, 7, 0 };
	int i;

	m_VexList.Reset();
	m_VexList.SetMode(GL_QUADS);

	if (pX && pY && pZ)
	{
		float fSign = 1.0f;
		MultiV<float> vX, vY, vZ, vD;

		vX = m_E3Base.Vec(pX);
		vY = m_E3Base.Vec(pY);
		vZ = m_E3Base.Vec(pZ);

		vD = vX;
		vD ^= vY;
		vD *= vZ;
		if (-vD[0] < 0)
			fSign = -1.0f;

		X = pX;
		Y = pY;
		Z = pZ;

		X = X * fHSize;
		Y = Y * fHSize;
		Z = Z * fSign * fHSize;
	}
	else
	{
		X.Set(fHSize, 0, 0);
		Y.Set(0, fHSize, 0);
		Z.Set(0, 0, fHSize);
/*
		m_VexList.AddVex(-fHSize, - fHSize, - fHSize);
		m_VexList.AddVex( fHSize, - fHSize, - fHSize);
		m_VexList.AddVex( fHSize, - fHSize,   fHSize);
		m_VexList.AddVex(-fHSize, - fHSize,   fHSize);
		
		m_VexList.AddVex(-fHSize,  fHSize,   fHSize);
		m_VexList.AddVex( fHSize,  fHSize,   fHSize);
		m_VexList.AddVex( fHSize,  fHSize, - fHSize);
		m_VexList.AddVex(-fHSize,  fHSize, - fHSize);
*/
	}
	
	m_VexList.AddVex(-X-Y-Z);
	m_VexList.AddVex( X-Y-Z);
	m_VexList.AddVex( X-Y+Z);
	m_VexList.AddVex(-X-Y+Z);
	
	m_VexList.AddVex(-X+Y+Z);
	m_VexList.AddVex( X+Y+Z);
	m_VexList.AddVex( X+Y-Z);
	m_VexList.AddVex(-X+Y-Z);
	
	if (m_bUseLighting)
	{
		m_mMaterial[MAT_CUBE].Diffuse(1.0, 1.0, 1.0, m_ActiveColor.Data()[3]);
		m_mMaterial[MAT_CUBE].Ambient(m_ActiveColor);
		m_mMaterial[MAT_CUBE].Apply();

//		FILE *pFile;
//		if ((pFile = fopen("Cube.dat", "w")))
//		{
		
		for(i=0;i<4;i++) 
		{
			m_VexList.AddNormal(-Y);
//			fprintf(pFile, "%d (%f %f %f)\n", i, m_VexList[0][0], m_VexList[0][1], m_VexList[0][2]);
		}

		for(i=4;i<8;i++) 
		{
			m_VexList.AddNormal(Y);
//			fprintf(pFile, "%d (%f %f %f)\n", i, m_VexList[0][0], m_VexList[0][1], m_VexList[0][2]);
		}

		for(i=8;i<12;i++)
		{
			A = m_VexList[piIdxQuads[i]];
			m_VexList.AddVex(A);
			m_VexList.AddNormal(-Z);
//			fprintf(pFile, "%d (%f %f %f)\n", i, m_VexList[0][0], m_VexList[0][1], m_VexList[0][2]);
		}

		for(i=12;i<16;i++)
		{
			A = m_VexList[piIdxQuads[i]];
			m_VexList.AddVex(A);
			m_VexList.AddNormal(X);
//			fprintf(pFile, "%d (%f %f %f)\n", i, m_VexList[0][0], m_VexList[0][1], m_VexList[0][2]);
		}

		for(i=16;i<20;i++)
		{
			A = m_VexList[piIdxQuads[i]];
			m_VexList.AddVex(A);
			m_VexList.AddNormal(Z);
//			fprintf(pFile, "%d (%f %f %f)\n", i, m_VexList[0][0], m_VexList[0][1], m_VexList[0][2]);
		}

		for(i=20;i<24;i++)
		{
			A = (COGLVertex)m_VexList[piIdxQuads[i]];
			m_VexList.AddVex(A);
			m_VexList.AddNormal(-X);
//			fprintf(pFile, "%d (%f %f %f)\n", i, m_VexList[0][0], m_VexList[0][1], m_VexList[0][2]);
		}
//		fprintf(pFile, "Last (%f %f %f)\n", m_VexList[23][0], m_VexList[23][1], m_VexList[23][2]);

//		fclose(pFile);
//		}

	}
	else
	{
		m_VexList.SetIdxList(24, piIdxQuads);
	}

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0, 1.0);

	m_VexList.Apply();

	glDisable(GL_POLYGON_OFFSET_FILL);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Cube

bool COGLMVFilterBase::DrawRhombus(float fSize)
{
	COGLVertex pVex[3];
	float fHSize = 0.5f * fSize;
	int piIdx[] = { 0, 5, 4, 3, 2, 5, 1, 2, 3, 4, 5, 2 };

	m_VexList.Reset();
	m_VexList.SetMode(GL_TRIANGLE_FAN);

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0, 1.0);

	m_VexList.AddVex(0.0, fHSize, 0.0);
	m_VexList.AddVex(0.0, -fHSize, 0.0);
	
	m_VexList.AddVex(fHSize, 0.0, 0.0);
	m_VexList.AddVex(0.0, 0.0, fHSize);
	m_VexList.AddVex(-fHSize, 0.0, 0.0);
	m_VexList.AddVex(0.0, 0.0, -fHSize);

	if (m_bUseLighting)
	{
		m_mMaterial[MAT_RHOMBUS].Diffuse(1.0, 1.0, 1.0, m_ActiveColor.Data()[3]);
		m_mMaterial[MAT_RHOMBUS].Ambient(m_ActiveColor);
		m_mMaterial[MAT_RHOMBUS].Apply();

		m_VexList.AddNormal(0.0, 1.0, 0.0);
		m_VexList.AddNormal(0.0, -1.0, 0.0);
		m_VexList.AddNormal(1.0, 0.0, 0.0);
		m_VexList.AddNormal(0.0, 0.0, 1.0);
		m_VexList.AddNormal(-1.0, 0.0, 0.0);
		m_VexList.AddNormal(0.0, 0.0, -1.0);
	}

	m_VexList.SetIdxList(6, piIdx);
	m_VexList.Apply();
		
	m_VexList.SetIdxList(6, &piIdx[6]);
	m_VexList.Apply();

	glDisable(GL_POLYGON_OFFSET_FILL);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Initialize Sphere Display Lists

bool COGLMVFilterBase::InitSphereDisplayLists()
{
	GenVexIcosahedron(m_VexList, 1, 2);

	m_dlWireSphere1.BeginDraw();
		m_VexList.Apply();
	m_dlWireSphere1.EndDraw();

	m_dlSolidSphere1.BeginDraw();
		m_VexList.Apply();
	m_dlSolidSphere1.EndDraw();

	m_dlSolidSphereI1.BeginDraw();
		glCullFace(GL_FRONT);
		m_VexList.InvertNormals(0.5f);
		m_VexList.Apply();
		glCullFace(GL_BACK);
	m_dlSolidSphereI1.EndDraw();


	GenVexIcosahedron(m_VexList, 1, 3);

	m_dlWireSphere2.BeginDraw();
		m_VexList.Apply();
	m_dlWireSphere2.EndDraw();

	m_dlSolidSphere2.BeginDraw();
		m_VexList.Apply();
	m_dlSolidSphere2.EndDraw();

	m_dlSolidSphereI2.BeginDraw();
		glCullFace(GL_FRONT);
		m_VexList.InvertNormals(0.5f);
		m_VexList.Apply();
		glCullFace(GL_BACK);
	m_dlSolidSphereI2.EndDraw();

/*
	m_dlSolidSphere1.BeginDraw();
		GLUquadricObj *solidSphere = gluNewQuadric();
		gluQuadricTexture(solidSphere, GL_TRUE);
		gluQuadricNormals(solidSphere, (GLenum)GLU_SMOOTH);
		gluSphere(solidSphere, 1, 15, 15);	
		gluQuadricTexture(solidSphere, GL_FALSE);
	m_dlSolidSphere1.EndDraw();
	
	m_dlWireSphere1.BeginDraw();
		GLUquadricObj *wireSphere = gluNewQuadric();
		gluQuadricDrawStyle(wireSphere, (GLenum)GLU_LINE);
		gluQuadricNormals(wireSphere, (GLenum)GLU_SMOOTH);
		gluSphere(wireSphere, 1, 15, 15);	
	m_dlWireSphere1.EndDraw();


	m_dlSolidSphere2.BeginDraw();
		//GLUquadricObj *solidSphere = gluNewQuadric();
		gluQuadricTexture(solidSphere, GL_TRUE);
		gluQuadricNormals(solidSphere, (GLenum)GLU_SMOOTH);
		gluSphere(solidSphere, 1, 30, 30);	
		gluQuadricTexture(solidSphere, GL_FALSE);
	m_dlSolidSphere2.EndDraw();

	m_dlWireSphere2.BeginDraw();
		//GLUquadricObj *wireSphere = gluNewQuadric();
		gluQuadricDrawStyle(wireSphere, (GLenum)GLU_LINE);
		gluQuadricNormals(wireSphere, (GLenum)GLU_SMOOTH);
		gluSphere(wireSphere, 1, 30, 30);	
	m_dlWireSphere2.EndDraw();
*/

	return true;
}


//////////////////////////////////////////////////////////////////////
// Generates vertices of an Icosahedron with radius fRadius and 2^iPower subdivisions.
// This approximates a sphere for higher powers

bool COGLMVFilterBase::GenVexIcosahedron(COGLVertexList &rVexList, float fRadius, int iPower)
{
	int i, iStep, iTriCount, iTri, iVexCount, iIdxPos;
	float fTau = float((sqrt(5.0) - 1.0) / 2.0);
	double dTh, dPhi, dLen, dX, dY, dZ, dPi, dHPi, d2Pi;
	MultiV<double> pvX[12], vR, vRr, vD;

	dHPi = acos(0.0);
	dPi = 2.0 * dHPi;
	d2Pi = 2.0 * dPi;

	Mem<STriplet> mIdxList, mNewIdxList;
	TTriplet pInitTriplets[] =
	{
		{0, 1, 10},
		{0, 10, 7},
		{0, 7, 6},
		{0, 6, 2},
		{0, 2, 1},

		{11, 10, 9},
		{11, 9, 4},
		{11, 4, 5},
		{11, 5, 7},
		{11, 7, 10},

		{8, 1, 2},
		{8, 2, 3},
		{8, 3, 4},
		{8, 4, 9},
		{8, 9, 1},

		{2, 6, 3},
		{6, 7, 5},
		{5, 3, 6},
		{5, 4, 3},
		{1, 9, 10}
	};

	rVexList.Reset();
	rVexList.SetMode(GL_TRIANGLES);

	// Icosahedron constructed from golden squares
	pvX[0] = E3GA<double>::Vec(0, 1, fTau);
	pvX[1] = E3GA<double>::Vec(0, 1, -fTau);
	pvX[2] = E3GA<double>::Vec(1, fTau, 0);
	pvX[3] = E3GA<double>::Vec(1, -fTau, 0);
	pvX[4] = E3GA<double>::Vec(0, -1, -fTau);
	pvX[5] = E3GA<double>::Vec(0, -1, fTau);
	pvX[6] = E3GA<double>::Vec(fTau, 0, 1);
	pvX[7] = E3GA<double>::Vec(-fTau, 0, 1);
	pvX[8] = E3GA<double>::Vec(fTau, 0, -1);
	pvX[9] = E3GA<double>::Vec(-fTau, 0, -1);
	pvX[10] = E3GA<double>::Vec(-1, fTau, 0);
	pvX[11] = E3GA<double>::Vec(-1, -fTau, 0);

	vR = E3GA<double>::Rotor(1, 0, 0, acos(pvX[0][2] / sqrt(1 + fTau*fTau)));
	for(i = 0; i < 12; i++)
	{
		vD = vR;
		vD &= pvX[i];
		vD &= vR.Reverse();
		pvX[i] = vD;
		//pvX[i] = ((vR & pvX[i]) & (~vR));
		rVexList.AddVex(float(pvX[i][1]), float(pvX[i][2]), float(pvX[i][3]));
		rVexList[i].Norm();
	}

	mIdxList.Set(20);
	memcpy(mIdxList.Data(), pInitTriplets, 60*sizeof(int));
	
	// Subdivide Triangles
	for(iStep = 0; iStep < iPower; iStep++)
	{
		iTriCount = mIdxList.Count();	// Number of Triangles
		mNewIdxList.Set(0);

		for(iTri = 0; iTri < iTriCount; iTri++)
		{
			int *pTriIdx;
			TTriplet pIdx;
			TTriplet pNewIdx;
			
			memcpy(pIdx, mIdxList[iTri].Data(), 3*sizeof(int));

			rVexList.AddVex( rVexList[pIdx[0]] + rVexList[pIdx[1]] );
			rVexList.AddVex( rVexList[pIdx[1]] + rVexList[pIdx[2]] );
			rVexList.AddVex( rVexList[pIdx[2]] + rVexList[pIdx[0]] );

			iVexCount = rVexList.Count();
			for(i = 1; i <= 3; i++)
				rVexList[iVexCount - i].Norm();

			pNewIdx[0] = iVexCount - 2;
			pNewIdx[1] = iVexCount - 1;
			pNewIdx[2] = iVexCount - 3;

			iIdxPos = mNewIdxList.Count();
			mNewIdxList.Add(4);

			pTriIdx = mNewIdxList[iIdxPos].Data();
			pTriIdx[0] = pIdx[0];
			pTriIdx[1] = pNewIdx[2];
			pTriIdx[2] = pNewIdx[1];

			pTriIdx = mNewIdxList[iIdxPos+1].Data();
			pTriIdx[0] = pIdx[1];
			pTriIdx[1] = pNewIdx[0];
			pTriIdx[2] = pNewIdx[2];

			pTriIdx = mNewIdxList[iIdxPos+2].Data();
			pTriIdx[0] = pIdx[2];
			pTriIdx[1] = pNewIdx[1];
			pTriIdx[2] = pNewIdx[0];

			pTriIdx = mNewIdxList[iIdxPos+3].Data();
			pTriIdx[0] = pNewIdx[2];
			pTriIdx[1] = pNewIdx[0];
			pTriIdx[2] = pNewIdx[1];
		}

		mIdxList = mNewIdxList;
	}

	// Map all vertices to unit sphere
	iVexCount = rVexList.Count();
	for(i = 0; i < iVexCount; i++)
	{
		COGLVertex &rVex = rVexList[i];

		// The normals are vertices themselves
		rVexList.AddNormal(rVex);

		dX = rVex[0];
		dY = rVex[1];
		dZ = rVex[2];

		// Scale vertices to radius
		rVex *= fRadius;

		dLen = sqrt(dX*dX + dY*dY);
		if (!::IsZero(dLen, m_dSensitivity))
		{
			if (Mag(dY) < Mag(dX))
			{
				if (dX >= 0)
				{
					if (dY >= 0)
						dTh = atan(dY  / dX);
					else
						dTh = 2*dPi + atan(dY / dX);
				}
				else
					dTh = atan(dY / dX) + dPi;
			}
			else
			{
				if (dY >= 0)
					dTh = dHPi - atan(dX  / dY);
				else
					dTh = 3*dHPi - atan(dX / dY);
			}
		}

		if (dLen < Mag(dZ))
		{
			if (dZ >= 0)
				dPhi = atan(dLen / dZ);
			else
				dPhi = atan(dLen / dZ) + dPi;
		}
		else
		{
			dPhi = dHPi - atan(dZ / dLen);
		}
/*
		dTh = dTh - dHPi;
		if (dTh < 0)
			dTh += d2Pi;
*/
		rVexList.AddTex(float(Mag(dTh/dPi - 1)), float(dPhi/dPi), 0);
		//rVexList.AddCol(Mag(dTh/dPi - 1), 0, dPhi/dPi);
	}
	
	// Set the index list
	rVexList.SetIdxList(mIdxList.Count()*3, &mIdxList[0][0]);

	return true;
}


//////////////////////////////////////////////////////////////////////
// Draws an Icosahedron at pos. xP, with radius fRadius and 2^iPower subdivisions.
// This approximates a sphere for higher powers

bool COGLMVFilterBase::DrawIcosahedron(const COGLVertex& xP, float fRadius, int iPower, bool bSolid)
{
	bool bNegRadius = false;
	COGLColor Dark;

	if (fRadius == 0.0) 
		return false;

	if (fRadius < 0.0)
	{
		if (!m_bShowImaginaryObjects)
			return false;

		fRadius = -fRadius;
		bNegRadius = true;
	}

	Dark = m_fBackFaceColFac * m_ActiveColor;

	if (m_bUseLighting)
	{
		m_mMaterial[MAT_SPHERE].Ambient(m_ActiveColor);
		if (bNegRadius && bSolid)
			m_mMaterial[MAT_SPHERE].Diffuse(1.0, 1.0, 1.0, 0.5);
		else
			m_mMaterial[MAT_SPHERE].Diffuse(1.0, 1.0, 1.0, m_ActiveColor.Data()[3]);
		m_mMaterial[MAT_SPHERE].Apply();
	}

	GenVexIcosahedron(m_VexList, fRadius, iPower);

	glPushMatrix();
	glTranslatef(xP[0], xP[1], xP[2]);

	if (!bSolid)
	{		
		//glDisable( GL_CULL_FACE		);
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		if (bNegRadius)
		{
			glEnable(GL_LINE_STIPPLE);
			glLineStipple(1, 0x5555);
		}

		m_VexList.Apply();

		if (bNegRadius)
			glDisable(GL_LINE_STIPPLE);

		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		//glEnable( GL_CULL_FACE		);
	}
	else
	{
		if (m_bUseTranslucency)
		{
			glEnable(GL_POLYGON_STIPPLE);
			MakeRandPolyStipple(0.8);
			glPolygonStipple(m_pcPolyStipple);
		}
		
		
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);
		glEnable(GL_TEXTURE_2D);

		m_VexList.Apply();

		//Dark.Apply();

		glCullFace(GL_FRONT);
		m_VexList.InvertNormals();

		m_VexList.Apply();

		glCullFace(GL_BACK);

		glDisable(GL_TEXTURE_2D);
		glDisable(GL_POLYGON_OFFSET_FILL);

		if (m_bUseTranslucency)
			glDisable(GL_POLYGON_STIPPLE);
	}

	glPopMatrix();

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Draw Sphere

bool COGLMVFilterBase::DrawSphere(const COGLVertex& xC, float fRadius, bool bSolid)
{
	bool bNegRadius = false;

	if (fRadius == 0.0) 
		return false;

	if (fRadius < 0.0)
	{
		if (!m_bShowImaginaryObjects)
			return false;

		fRadius = -fRadius;
		bNegRadius = true;
	}

	COGLColor Dark;

	Dark = m_fBackFaceColFac * m_ActiveColor;

	if (m_bUseLighting)
	{
		m_mMaterial[MAT_SPHERE].Ambient(m_ActiveColor);
		if (bNegRadius && bSolid)
			m_mMaterial[MAT_SPHERE].Diffuse(1.0, 1.0, 1.0, 0.5);
		else
			m_mMaterial[MAT_SPHERE].Diffuse(1.0, 1.0, 1.0, m_ActiveColor.Data()[3]);
		m_mMaterial[MAT_SPHERE].Apply();
	}

	glPushMatrix();
	glTranslatef(xC[0], xC[1], xC[2]);
	glScalef(fRadius, fRadius, fRadius);
	glEnable(GL_NORMALIZE);

	if (!bSolid)
	{		
		//glDisable( GL_CULL_FACE		);
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		if (bNegRadius)
		{
			glEnable(GL_LINE_STIPPLE);
			glLineStipple(1, 0x5555);
		}

		if (fRadius < 0.25f)
			m_dlWireSphere1.Apply();
		else
			m_dlWireSphere2.Apply();

		if (bNegRadius)
			glDisable(GL_LINE_STIPPLE);

		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		//glEnable( GL_CULL_FACE		);
	}
	else
	{
		
		if (m_bUseTranslucency)
		{
			glEnable(GL_POLYGON_STIPPLE);
			MakeRandPolyStipple(0.8);
			glPolygonStipple(m_pcPolyStipple);
		}
		
		
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);

		glEnable(GL_TEXTURE_2D);

		if (fRadius < 0.25f)
		{
			Dark.Apply();
			m_dlSolidSphereI1.Apply();
			m_ActiveColor.Apply();
			m_dlSolidSphere1.Apply();
		}
		else
		{
			Dark.Apply();
			m_dlSolidSphereI2.Apply();
			m_ActiveColor.Apply();
			m_dlSolidSphere2.Apply();
		}

		glDisable(GL_POLYGON_OFFSET_FILL);
		glDisable(GL_TEXTURE_2D);

		if (m_bUseTranslucency)
			glDisable(GL_POLYGON_STIPPLE);
	}

	glDisable(GL_NORMALIZE);
	glPopMatrix();

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Draw Ellipsoid

bool COGLMVFilterBase::DrawEllipsoid(const COGLVertex& xC, 
									 const COGLVertex& xX, const COGLVertex& xY, 
									 const COGLVertex& xZ, bool bSolid)
{
	bool bNegRadius = false;
	int i;
	float fRX, fRY, fRZ, fRMag;
	GLfloat pfMat[16];
	//GLdouble pdWin[3], pdObj[3], pdModel[16], pdProj[16];

	fRX = xX.Mag();
	fRY = xY.Mag();
	fRZ = xZ.Mag();
	fRMag = sqrt(fRX*fRX + fRY*fRY + fRZ*fRZ);

	COGLColor Dark;

	Dark = m_fBackFaceColFac * m_ActiveColor;

	if (m_bUseLighting)
	{
		m_mMaterial[MAT_SPHERE].Ambient(m_ActiveColor);
		if (bNegRadius && bSolid)
			m_mMaterial[MAT_SPHERE].Diffuse(1.0, 1.0, 1.0, 0.5);
		else
			m_mMaterial[MAT_SPHERE].Diffuse(1.0, 1.0, 1.0, m_ActiveColor.Data()[3]);
		m_mMaterial[MAT_SPHERE].Apply();
	}

	for(i = 0; i < 3; i++)
		pfMat[i] = xX[i];
	pfMat[3] = 0;

	for(i = 0; i < 3; i++)
		pfMat[4+i] = xY[i];
	pfMat[7] = 0;

	for(i = 0; i < 3; i++)
		pfMat[8+i] = xZ[i];
	pfMat[11] = 0;

	for(i = 0; i < 3; i++)
		pfMat[12+i] = 0;
	pfMat[15] = 1.0f;


	glPushMatrix();
	glTranslatef(xC[0], xC[1], xC[2]);
	glMultMatrixf(pfMat);
//	glScalef(fRadius, fRadius, fRadius);
	glEnable(GL_NORMALIZE);
/*
	glGetDoublev(GL_MODELVIEW_MATRIX, pdModel);
	glGetDoublev(GL_PROJECTION_MATRIX, pdProj);
	glGetIntegerv(GL_VIEWPORT, piViewport);

	gluProject(0, 0, 0, pdModel, pdProj, piViewport, &pdWin[0], &pdWin[1], &pdWin[2]);
	gluUnProject(pdWin[0], pdWin[1], pdWin[2], pdModel, pdProj, piViewport, 
					&pdObj[0], &pdObj[1], &pdObj[2]);

	printf("Win: %g, %g, %g\n", pdWin[0], pdWin[1], pdWin[2]);
	printf("Obj: %g, %g, %g\n\n", pdObj[0], pdObj[1], pdObj[2]);
*/

	if (!bSolid)
	{		
		glDisable( GL_CULL_FACE		);
		if (bNegRadius)
		{
			glEnable(GL_LINE_STIPPLE);
			glLineStipple(1, 0x5555);
		}

		if (fRMag < 0.25f)
			m_dlWireSphere1.Apply();
		else
			m_dlWireSphere2.Apply();

		if (bNegRadius)
			glDisable(GL_LINE_STIPPLE);

		glEnable( GL_CULL_FACE		);
	}
	else
	{
		
		if (m_bUseTranslucency)
		{
			glEnable(GL_POLYGON_STIPPLE);
			MakeRandPolyStipple(0.8);
			glPolygonStipple(m_pcPolyStipple);
		}
		
		
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);

		if (fRMag < 0.25f)
		{
			Dark.Apply();
			m_dlSolidSphereI1.Apply();
			m_ActiveColor.Apply();
			m_dlSolidSphere1.Apply();
		}
		else
		{
			Dark.Apply();
			m_dlSolidSphereI2.Apply();
			m_ActiveColor.Apply();
			m_dlSolidSphere2.Apply();
		}

		glDisable(GL_POLYGON_OFFSET_FILL);

		if (!m_bUseLighting)
		{
			Dark.Apply();
			if (fRMag < 0.25f)
				m_dlWireSphere1.Apply();
			else
				m_dlWireSphere2.Apply();
			m_ActiveColor.Apply();
		}

		if (m_bUseTranslucency)
			glDisable(GL_POLYGON_STIPPLE);
	}

	glDisable(GL_NORMALIZE);
	glPopMatrix();

	return true;
}



/*
bool COGLMVFilterBase::DrawSphere(const COGLVertex& xC, float fRadius, bool bSolid)
{
	bool bNegRadius = false;

	if (fRadius == 0.0) 
		return false;

	if (fRadius < 0.0)
	{
		if (!m_bShowImaginaryObjects)
			return false;

		fRadius = -fRadius;
		bNegRadius = true;
	}

	int iSlices, iStacks, iVal;
	COGLColor Dark;

	Dark = m_fBackFaceColFac * m_ActiveColor;

	iVal = int((2.0f * m_fPi * fRadius) / 0.1f);
	if (iVal < 8)
		iVal = 8;
	else if (iVal > 30)
		iVal = 30;
	//printf("iVal: %d\n", iVal);

	iSlices = iVal;
	iStacks = iVal;

	if (m_bUseLighting)
	{
		m_mMaterial[MAT_SPHERE].Ambient(m_ActiveColor);
		if (bNegRadius && bSolid)
			m_mMaterial[MAT_SPHERE].Diffuse(1.0, 1.0, 1.0, 0.5);
		else
			m_mMaterial[MAT_SPHERE].Diffuse(1.0, 1.0, 1.0, m_ActiveColor.Data()[3]);
		m_mMaterial[MAT_SPHERE].Apply();
	}

	glPushMatrix();
	glTranslatef(xC[0], xC[1], xC[2]);

	if (!bSolid)
	{		
		glDisable( GL_CULL_FACE		);
		if (bNegRadius)
		{
			glEnable(GL_LINE_STIPPLE);
			glLineStipple(1, 0x5555);
		}

		GLUquadricObj *solidSphere = gluNewQuadric();
		gluQuadricDrawStyle(solidSphere, (GLenum)GLU_LINE);
		gluQuadricNormals(solidSphere, (GLenum)GLU_SMOOTH);
		gluSphere(solidSphere, fRadius, iSlices, iStacks);	

		if (bNegRadius)
			glDisable(GL_LINE_STIPPLE);

		glEnable( GL_CULL_FACE		);
	}
	else
	{
		
		if (m_bUseTranslucency)
		{
			glEnable(GL_POLYGON_STIPPLE);
			MakeRandPolyStipple(0.8);
			glPolygonStipple(m_pcPolyStipple);
		}
		
		
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);

		GLUquadricObj *solidSphere = gluNewQuadric();
		gluQuadricNormals(solidSphere, (GLenum)GLU_SMOOTH);
		gluSphere(solidSphere, fRadius, iSlices, iStacks);

		glDisable(GL_POLYGON_OFFSET_FILL);

		if (!m_bUseLighting)
		{
			Dark.Apply();
			GLUquadricObj *solidSphere = gluNewQuadric();
			gluQuadricDrawStyle(solidSphere,  (GLenum)GLU_LINE);
			gluQuadricNormals(solidSphere, (GLenum)GLU_SMOOTH);
			gluSphere(solidSphere, fRadius, iSlices, iStacks);	
			
			m_ActiveColor.Apply();
		}

		if (m_bUseTranslucency)
			glDisable(GL_POLYGON_STIPPLE);
	}

	glPopMatrix();

	return true;
}
*/

//////////////////////////////////////////////////////////////////////
/// Draw Plane

bool COGLMVFilterBase::DrawPlane(const COGLVertex& xP, const COGLVertex& xA, 
								 const COGLVertex& xB, bool bCornerVecs, bool bDirected)
{
	int piIdx[] = { 3, 2, 1, 0 };
	COGLColor Dark;
	COGLVertex xU, xV, xN;

	xN[0] = xA[1]*xB[2] - xA[2]*xB[1];
	xN[1] = xA[2]*xB[0] - xA[0]*xB[2];
	xN[2] = xA[0]*xB[1] - xA[1]*xB[0];
	xN.Norm();

	xU = 0.5f * xA;
	xV = 0.5f * xB;

	Dark = m_fBackFaceColFac * m_ActiveColor;

	m_VexList.Reset();

	m_VexList.SetMode(GL_QUADS);

	if (m_bUseLighting)
	{
		m_mMaterial[MAT_PLANE].Ambient(m_ActiveColor);
		m_mMaterial[MAT_PLANE].Diffuse(1.0, 1.0, 1.0, m_ActiveColor.Data()[3]);
		m_mMaterial[MAT_PLANE].Apply();

		m_VexList.AddNormal(xN);
		m_VexList.AddNormal(xN);
		m_VexList.AddNormal(xN);
		m_VexList.AddNormal(xN);
	}

	float fTexMaxX = 1.0f;
	float fTexMaxY = 1.0f;
	float fMaxX = 1.0f, fMaxY = 1.0f;

	if (m_bUseAbsTexCoords)
	{
		fMaxX = xA.Mag();
		fMaxY = xB.Mag();
	}

	if (m_fTexRepFac != 0.0f)
	{
		fTexMaxX = fMaxX / m_fTexRepFac;
		fTexMaxY = fMaxY / m_fTexRepFac;
	}

	m_VexList.AddTex(0,fTexMaxY,0);
	m_VexList.AddTex(fTexMaxX,fTexMaxY,0);
	m_VexList.AddTex(fTexMaxX,0,0);
	m_VexList.AddTex(0,0,0);


	if (bCornerVecs)
	{
		m_VexList.AddVex(xP - xU);
		m_VexList.AddVex(xP - xV);
		m_VexList.AddVex(xP + xU);
		m_VexList.AddVex(xP + xV);
	}
	else
	{
		m_VexList.AddVex(xP - xU - xV);
		m_VexList.AddVex(xP + xU - xV);
		m_VexList.AddVex(xP + xU + xV);
		m_VexList.AddVex(xP - xU + xV);
	}

	if (m_bUseTranslucency)
	{
		glEnable(GL_POLYGON_STIPPLE);
		MakeRandPolyStipple(0.3);
		glPolygonStipple(m_pcPolyStipple);
	}
	
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0, 1.0);
	 
	m_VexList.Apply();
	
	if (bDirected)
		Dark.Apply();
	
	m_VexList.SetIdxList(4, piIdx);
	m_VexList.InvertNormals(0.5f);
	
	m_VexList.Apply();
	
	glDisable(GL_POLYGON_OFFSET_FILL);
	

	if (bDirected)
		m_ActiveColor.Apply();
	
	if (m_bUseTranslucency)
		glDisable(GL_POLYGON_STIPPLE);
	
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Box

bool COGLMVFilterBase::DrawBox(const COGLVertex& xP, const COGLVertex& xA, 
								 const COGLVertex& xB, float fWidth, 
								 bool bCornerVecs, bool bDirected)
{
	int piIdx[] = { 3, 2, 1, 0 };
	COGLColor Dark;
	COGLVertex xU, xV, xN, xW, xNa, xNb, xC1, xC2, xC3, xC4;

	xN[0] = xA[1]*xB[2] - xA[2]*xB[1];
	xN[1] = xA[2]*xB[0] - xA[0]*xB[2];
	xN[2] = xA[0]*xB[1] - xA[1]*xB[0];
	xN.Norm();

	xNa = xA;
	xNa.Norm();

	xNb = xB;
	xNb.Norm();

	xU = 0.5f * xA;
	xV = 0.5f * xB;
	xW = 0.5f * fWidth * xN;

	Dark = m_fBackFaceColFac * m_ActiveColor;

	m_VexList.Reset();

	m_VexList.SetMode(GL_QUADS);

	float fTexMaxX = 1.0f;
	float fTexMaxY = 1.0f;
	float fMaxX = 1.0f, fMaxY = 1.0f;

	if (m_bUseAbsTexCoords)
	{
		fMaxX = xA.Mag();
		fMaxY = xB.Mag();
	}

	if (m_fTexRepFac != 0.0f)
	{
		fTexMaxX = fMaxX / m_fTexRepFac;
		fTexMaxY = fMaxY / m_fTexRepFac;
	}

	if (m_bUseLighting)
	{
		m_mMaterial[MAT_PLANE].Ambient(m_ActiveColor);
		m_mMaterial[MAT_PLANE].Diffuse(1.0, 1.0, 1.0, m_ActiveColor.Data()[3]);
		m_mMaterial[MAT_PLANE].Apply();
	}
	
	if (bCornerVecs)
	{
		m_VexList.AddVex(xP - xU);
		m_VexList.AddVex(xP - xV);
		m_VexList.AddVex(xP + xU);
		m_VexList.AddVex(xP + xV);
	}
	else
	{
		xC1 = xP - xU - xV;
		xC2 = xP + xU - xV;
		xC3 = xP + xU + xV;
		xC4 = xP - xU + xV;

		if (m_bUseLighting)
		{
			m_VexList.AddNormal(xN);
			m_VexList.AddNormal(xN);
			m_VexList.AddNormal(xN);
			m_VexList.AddNormal(xN);

			m_VexList.AddNormal(-xN);
			m_VexList.AddNormal(-xN);
			m_VexList.AddNormal(-xN);
			m_VexList.AddNormal(-xN);

			m_VexList.AddNormal(xNa);
			m_VexList.AddNormal(xNa);
			m_VexList.AddNormal(xNa);
			m_VexList.AddNormal(xNa);

			m_VexList.AddNormal(-xNa);
			m_VexList.AddNormal(-xNa);
			m_VexList.AddNormal(-xNa);
			m_VexList.AddNormal(-xNa);

			m_VexList.AddNormal(xNb);
			m_VexList.AddNormal(xNb);
			m_VexList.AddNormal(xNb);
			m_VexList.AddNormal(xNb);

			m_VexList.AddNormal(-xNb);
			m_VexList.AddNormal(-xNb);
			m_VexList.AddNormal(-xNb);
			m_VexList.AddNormal(-xNb);
		}

		// Top
		m_VexList.AddVex(xC1 + xW);
		m_VexList.AddVex(xC2 + xW);
		m_VexList.AddVex(xC3 + xW);
		m_VexList.AddVex(xC4 + xW);

		m_VexList.AddTex(0,fTexMaxY,0);
		m_VexList.AddTex(fTexMaxX,fTexMaxY,0);
		m_VexList.AddTex(fTexMaxX,0,0);
		m_VexList.AddTex(0,0,0);

		// Bottom
		m_VexList.AddVex(xC4 - xW);
		m_VexList.AddVex(xC3 - xW);
		m_VexList.AddVex(xC2 - xW);
		m_VexList.AddVex(xC1 - xW);

		m_VexList.AddTex(0,fTexMaxY,0);
		m_VexList.AddTex(fTexMaxX,fTexMaxY,0);
		m_VexList.AddTex(fTexMaxX,0,0);
		m_VexList.AddTex(0,0,0);

		// xA Normal side
		m_VexList.AddVex(xC2 - xW);
		m_VexList.AddVex(xC3 - xW);
		m_VexList.AddVex(xC3 + xW);
		m_VexList.AddVex(xC2 + xW);

		m_VexList.AddTex(0,fTexMaxY,0);
		m_VexList.AddTex(fTexMaxX,fTexMaxY,0);
		m_VexList.AddTex(fTexMaxX,0,0);
		m_VexList.AddTex(0,0,0);

		// -xA Normal side
		m_VexList.AddVex(xC1 + xW);
		m_VexList.AddVex(xC4 + xW);
		m_VexList.AddVex(xC4 - xW);
		m_VexList.AddVex(xC1 - xW);

		m_VexList.AddTex(0,fTexMaxY,0);
		m_VexList.AddTex(fTexMaxX,fTexMaxY,0);
		m_VexList.AddTex(fTexMaxX,0,0);
		m_VexList.AddTex(0,0,0);

		// xB Normal side
		m_VexList.AddVex(xC3 - xW);
		m_VexList.AddVex(xC4 - xW);
		m_VexList.AddVex(xC4 + xW);
		m_VexList.AddVex(xC3 + xW);

		m_VexList.AddTex(0,fTexMaxY,0);
		m_VexList.AddTex(fTexMaxX,fTexMaxY,0);
		m_VexList.AddTex(fTexMaxX,0,0);
		m_VexList.AddTex(0,0,0);

		// -xB Normal side
		m_VexList.AddVex(xC1 - xW);
		m_VexList.AddVex(xC2 - xW);
		m_VexList.AddVex(xC2 + xW);
		m_VexList.AddVex(xC1 + xW);

		m_VexList.AddTex(0,fTexMaxY,0);
		m_VexList.AddTex(fTexMaxX,fTexMaxY,0);
		m_VexList.AddTex(fTexMaxX,0,0);
		m_VexList.AddTex(0,0,0);
	}


	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0, 1.0);
	
	m_VexList.Apply();
	
	glDisable(GL_POLYGON_OFFSET_FILL);
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Draw Box

bool COGLMVFilterBase::DrawBox(const COGLVertex& xP, const COGLVertex& xA, 
								 const COGLVertex& xB, const COGLVertex& _xC,
								 const MemObj<COGLColor>& rColList)
{
	int piIdx[] = { 3, 2, 1, 0 };
	bool bIsLH = false;
	COGLColor Dark;
	COGLVertex xC, xU, xV, xN, xW, xNa, xNb, xNc, xC1, xC2, xC3, xC4;
	float fTexMaxX = 1.0f;
	float fTexMaxY = 1.0f;
	float fMaxX = 1.0f, fMaxY = 1.0f;

	if (m_bUseAbsTexCoords)
	{
		fMaxX = xA.Mag();
		fMaxY = xB.Mag();
	}

	if (m_fTexRepFac != 0.0f)
	{
		fTexMaxX = fMaxX / m_fTexRepFac;
		fTexMaxY = fMaxY / m_fTexRepFac;
	}

	xNc = xA ^ xB;	// vector cross product
	xNc.Norm();
	if (xNc * _xC < 0.0f)
	{
		xNc = -xNc;
		xC = -_xC;
		bIsLH = true;	// left handed system
	}
	else
	{
		xC = _xC;
	}

	xNa = xB ^ xC;	// vector cross product
	xNa.Norm();

	xNb = xC ^ xA;	// vector cross product
	xNb.Norm();


	xU = 0.5f * xA;
	xV = 0.5f * xB;
	xW = 0.5f * xC;

	Dark = m_fBackFaceColFac * m_ActiveColor;

	m_VexList.Reset();

	m_VexList.SetMode(GL_QUADS);

	if (m_bUseLighting)
	{
		m_mMaterial[MAT_PLANE].Ambient(m_ActiveColor);
		m_mMaterial[MAT_PLANE].Diffuse(1.0, 1.0, 1.0, m_ActiveColor.Data()[3]);
		m_mMaterial[MAT_PLANE].Apply();
	}
	
	xC1 = xP - xU - xV;
	xC2 = xP + xU - xV;
	xC3 = xP + xU + xV;
	xC4 = xP - xU + xV;

	if (m_bUseLighting)
	{
		m_VexList.AddNormal(xNc);
		m_VexList.AddNormal(xNc);
		m_VexList.AddNormal(xNc);
		m_VexList.AddNormal(xNc);

		m_VexList.AddNormal(-xNc);
		m_VexList.AddNormal(-xNc);
		m_VexList.AddNormal(-xNc);
		m_VexList.AddNormal(-xNc);

		m_VexList.AddNormal(xNa);
		m_VexList.AddNormal(xNa);
		m_VexList.AddNormal(xNa);
		m_VexList.AddNormal(xNa);

		m_VexList.AddNormal(-xNa);
		m_VexList.AddNormal(-xNa);
		m_VexList.AddNormal(-xNa);
		m_VexList.AddNormal(-xNa);

		m_VexList.AddNormal(xNb);
		m_VexList.AddNormal(xNb);
		m_VexList.AddNormal(xNb);
		m_VexList.AddNormal(xNb);

		m_VexList.AddNormal(-xNb);
		m_VexList.AddNormal(-xNb);
		m_VexList.AddNormal(-xNb);
		m_VexList.AddNormal(-xNb);
	}

	if (rColList.Count() == 8)
	{
		if (bIsLH)
		{
			m_VexList.AddCol(rColList[4]);
			m_VexList.AddCol(rColList[5]);
			m_VexList.AddCol(rColList[6]);
			m_VexList.AddCol(rColList[7]);

			m_VexList.AddCol(rColList[3]);
			m_VexList.AddCol(rColList[2]);
			m_VexList.AddCol(rColList[1]);
			m_VexList.AddCol(rColList[0]);

			m_VexList.AddCol(rColList[1]);
			m_VexList.AddCol(rColList[2]);
			m_VexList.AddCol(rColList[6]);
			m_VexList.AddCol(rColList[5]);

			m_VexList.AddCol(rColList[4]);
			m_VexList.AddCol(rColList[7]);
			m_VexList.AddCol(rColList[3]);
			m_VexList.AddCol(rColList[0]);

			m_VexList.AddCol(rColList[2]);
			m_VexList.AddCol(rColList[3]);
			m_VexList.AddCol(rColList[7]);
			m_VexList.AddCol(rColList[6]);

			m_VexList.AddCol(rColList[0]);
			m_VexList.AddCol(rColList[1]);
			m_VexList.AddCol(rColList[5]);
			m_VexList.AddCol(rColList[4]);
		}
		else
		{
			m_VexList.AddCol(rColList[0]);
			m_VexList.AddCol(rColList[1]);
			m_VexList.AddCol(rColList[2]);
			m_VexList.AddCol(rColList[3]);

			m_VexList.AddCol(rColList[7]);
			m_VexList.AddCol(rColList[6]);
			m_VexList.AddCol(rColList[5]);
			m_VexList.AddCol(rColList[4]);

			m_VexList.AddCol(rColList[5]);
			m_VexList.AddCol(rColList[6]);
			m_VexList.AddCol(rColList[2]);
			m_VexList.AddCol(rColList[1]);

			m_VexList.AddCol(rColList[0]);
			m_VexList.AddCol(rColList[3]);
			m_VexList.AddCol(rColList[7]);
			m_VexList.AddCol(rColList[4]);

			m_VexList.AddCol(rColList[6]);
			m_VexList.AddCol(rColList[7]);
			m_VexList.AddCol(rColList[3]);
			m_VexList.AddCol(rColList[2]);

			m_VexList.AddCol(rColList[4]);
			m_VexList.AddCol(rColList[5]);
			m_VexList.AddCol(rColList[1]);
			m_VexList.AddCol(rColList[0]);
		}

	}

	// Top
	m_VexList.AddVex(xC1 + xW);	// col 0
	m_VexList.AddVex(xC2 + xW);	// col 1
	m_VexList.AddVex(xC3 + xW);	// col 2
	m_VexList.AddVex(xC4 + xW);	// col 3

	m_VexList.AddTex(0,fTexMaxY,0);
	m_VexList.AddTex(fTexMaxX,fTexMaxY,0);
	m_VexList.AddTex(fTexMaxX,0,0);
	m_VexList.AddTex(0,0,0);


	// Bottom
	m_VexList.AddVex(xC4 - xW);	// col 7
	m_VexList.AddVex(xC3 - xW);	// col 6
	m_VexList.AddVex(xC2 - xW);	// col 5
	m_VexList.AddVex(xC1 - xW); // col 4

	m_VexList.AddTex(0,fTexMaxY,0);
	m_VexList.AddTex(fTexMaxX,fTexMaxY,0);
	m_VexList.AddTex(fTexMaxX,0,0);
	m_VexList.AddTex(0,0,0);

	// xA Normal side
	m_VexList.AddVex(xC2 - xW); // col 5
	m_VexList.AddVex(xC3 - xW); // col 6
	m_VexList.AddVex(xC3 + xW);	// col 2
	m_VexList.AddVex(xC2 + xW);	// col 1

	m_VexList.AddTex(fTexMaxX,fTexMaxY,0);
	m_VexList.AddTex(fTexMaxX,0,0);
	m_VexList.AddTex(0,0,0);
	m_VexList.AddTex(0,fTexMaxY,0);

	// -xA Normal side
	m_VexList.AddVex(xC1 + xW);	// col 0
	m_VexList.AddVex(xC4 + xW);	// col 3
	m_VexList.AddVex(xC4 - xW);	// col 7
	m_VexList.AddVex(xC1 - xW);	// col 4

	m_VexList.AddTex(fTexMaxX,fTexMaxY,0);
	m_VexList.AddTex(fTexMaxX,0,0);
	m_VexList.AddTex(0,0,0);
	m_VexList.AddTex(0,fTexMaxY,0);

	// xB Normal side
	m_VexList.AddVex(xC3 - xW);	// col 6
	m_VexList.AddVex(xC4 - xW);	// col 7
	m_VexList.AddVex(xC4 + xW);	// col 3
	m_VexList.AddVex(xC3 + xW);	// col 2

	m_VexList.AddTex(fTexMaxX,0,0);
	m_VexList.AddTex(0,0,0);
	m_VexList.AddTex(0,fTexMaxY,0);
	m_VexList.AddTex(fTexMaxX,fTexMaxY,0);

	// -xB Normal side
	m_VexList.AddVex(xC1 - xW);	// col 4
	m_VexList.AddVex(xC2 - xW);	// col 5
	m_VexList.AddVex(xC2 + xW);	// col 1
	m_VexList.AddVex(xC1 + xW);	// col 0

	m_VexList.AddTex(0,fTexMaxY,0);
	m_VexList.AddTex(fTexMaxX,fTexMaxY,0);
	m_VexList.AddTex(fTexMaxX,0,0);
	m_VexList.AddTex(0,0,0);


	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0, 1.0);
	
	m_VexList.Apply();
	
	glDisable(GL_POLYGON_OFFSET_FILL);
	
	return true;
}



//////////////////////////////////////////////////////////////////////
/// Draw Triangle

bool COGLMVFilterBase::DrawTriangle(const COGLVertex& xA, const COGLVertex& xB, 
									const COGLVertex& xC, bool bDirected)
{
	int piIdx[] = { 2, 1, 0 };
	COGLColor Dark;
	COGLVertex xU, xV, xN, xD;

	xU = xB - xA;
	xV = xC - xA;

	Dark = m_fBackFaceColFac * m_ActiveColor;

	m_VexList.Reset();

	m_VexList.SetMode(GL_TRIANGLES);

	if (m_bUseLighting)
	{
		MultiV<float> vN;

		vN = m_E3Base.Vec(xU[0], xU[1], xU[2]);
		vN ^= m_E3Base.Vec(xV[0], xV[1], xV[2]);
		vN = vN.Dual();
		xN = &vN[1];

		m_mMaterial[MAT_PLANE].Ambient(m_ActiveColor);
		m_mMaterial[MAT_PLANE].Diffuse(1.0, 1.0, 1.0, m_ActiveColor.Data()[3]);
		m_mMaterial[MAT_PLANE].Apply();

		m_VexList.AddNormal(xN);
		m_VexList.AddNormal(xN);
		m_VexList.AddNormal(xN);
	}
	
	m_VexList.AddVex(xA);
	m_VexList.AddVex(xB);
	m_VexList.AddVex(xC);

	float fTexMaxX = 1.0f;
	float fTexMaxY = 1.0f;
	float fMaxX = 1.0f, fMaxY = 1.0f;

	if (m_bUseAbsTexCoords)
	{
		xD = xB - xA;
		fMaxX = xD.Mag();

		xD.Norm();
		xD = xC - (xD * (xC - xA)) * xD;
		fMaxY = xD.Mag();
	}

	if (m_fTexRepFac != 0.0f)
	{
		fTexMaxX = fMaxX / m_fTexRepFac;
		fTexMaxY = fMaxY / m_fTexRepFac;
	}

	m_VexList.AddTex(0,fTexMaxY,0);
	m_VexList.AddTex(fTexMaxX,fTexMaxY,0);
	m_VexList.AddTex(fTexMaxX/2,0,0);

	if (m_bUseTranslucency)
	{
		glEnable(GL_POLYGON_STIPPLE);
		MakeRandPolyStipple(0.3);
		glPolygonStipple(m_pcPolyStipple);
	}
	
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0, 1.0);
	
	m_VexList.Apply();
	
	if (bDirected)
		Dark.Apply();
	
	m_VexList.SetIdxList(3, piIdx);
	m_VexList.InvertNormals();
	
	m_VexList.Apply();
	
	glDisable(GL_POLYGON_OFFSET_FILL);
	
	if (bDirected)
		m_ActiveColor.Apply();
	
	if (m_bUseTranslucency)
		glDisable(GL_POLYGON_STIPPLE);
	
	return true;
}



//////////////////////////////////////////////////////////////////////
/// Draw Circle

bool COGLMVFilterBase::DrawCircle(const COGLVertex& xC, const COGLVertex& xN, 
								  float fR, float fAngleStep)
{
	MemObj<MultiV<float> > mvList;
	MultiV<float> vU, vX, vR;
	COGLVertex xX, xA, xB;

	double dTheta = 0;
	double dRadStep;
	double dPi = acos(-1.0);

	bool bNegRadius = false;

	if (fR == 0.0f || fR > 1000.0f || fAngleStep <= 0.0f || fAngleStep > 180.0f ||
		xN.Mag() == 0)
		return false;

	if (fR < 0.0)
	{
		if (!m_bShowImaginaryObjects)
			return false;

		fR = -fR;
		bNegRadius = true;
	}

	int i, iSteps = int(360.0f / fAngleStep);
	dRadStep = 2.0*dPi / double(iSteps);

	vU = m_E3Base.Vec(xN[0], xN[1], xN[2]) & m_E3Base.vI;
	//vR = m_E3Base.Rotor(vU, (fAngleStep * m_fPi)/180.0f, false);

	FactorBlade<float>(vU, mvList, bool(true), (MultiV<float> *)0, (float *)0, m_fSensitivity);

	vX = fR & mvList[0];
	xA.Set(vX[1], vX[2], vX[3]);

	vX = fR & mvList[1];
	xB.Set(vX[1], vX[2], vX[3]);

	m_VexList.Reset();
	m_VexList.SetMode(GL_LINE_STRIP);

	if (bNegRadius)
	{
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(1, 0x5555);
	}

	if (m_bUseLighting)
	{
		m_mMaterial[MAT_CIRCLE].Ambient(m_ActiveColor);
		m_mMaterial[MAT_CIRCLE].Apply();
		//m_VexList.AddNormal(xN);
	}

	//m_VexList.AddVex(vX[1]+xC[0], vX[2]+xC[1], vX[3]+xC[2]);
	//m_VexList.AddVex(xX + xC);

	for(i=0;i<iSteps;i++)
	{
		xX = (float(cos(dTheta)) * xA) + (float(sin(dTheta)) * xB);
		m_VexList.AddVex(xX + xC);

		//vX = (vR & vX) & (~vR);
		//m_VexList.AddVex(vX[1]+xC[0], vX[2]+xC[1], vX[3]+xC[2]);

		if (m_bUseLighting)
			m_VexList.AddNormal(xN);

		dTheta += dRadStep;
	}
	m_VexList.AddVex(xA + xC);
	if (m_bUseLighting)
		m_VexList.AddNormal(xN);

	glDisable( GL_CULL_FACE		);
	m_VexList.Apply();
	glEnable( GL_CULL_FACE		);

	if (bNegRadius)
	{
		glDisable(GL_LINE_STIPPLE);
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Draw Ellipse

bool COGLMVFilterBase::DrawEllipse(const COGLVertex& xC,
								   const COGLVertex& xA, const COGLVertex& xB, 
								  float fAngleStep)
{
	MemObj<MultiV<float> > mvList;
	MultiV<float> vA, vB, vX, vN;
	COGLVertex xN, xX;

	double dTheta = 0;
	double dRadStep;
	double dPi = acos(-1.0);


	if (fAngleStep <= 0.0f || fAngleStep > 180.0f ||
		xA.Mag() == 0 || xB.Mag() == 0 || xA == xB)
		return false;

	int i, iSteps = int(360.0f / fAngleStep);
	dRadStep = 2.0*dPi / double(iSteps);

	vA = m_E3Base.Vec(xA[0], xA[1], xA[2]);
	vB = m_E3Base.Vec(xB[0], xB[1], xB[2]);
	vN = *(vA^vB);
	xN.Set(vN[1], vN[2], vN[3]);

	m_VexList.Reset();
	m_VexList.SetMode(GL_LINE_STRIP);

	if (m_bUseLighting)
	{
		m_mMaterial[MAT_CIRCLE].Ambient(m_ActiveColor);
		m_mMaterial[MAT_CIRCLE].Apply();
		//m_VexList.AddNormal(xN);
	}

	for(i=0;i<iSteps;i++)
	{
		xX = (float(cos(dTheta)) * xA) + (float(sin(dTheta)) * xB);
		m_VexList.AddVex(xX + xC);
		//m_VexList.AddVex(vX[1]+xC[0], vX[2]+xC[1], vX[3]+xC[2]);

		if (m_bUseLighting)
			m_VexList.AddNormal(xN);

		dTheta += dRadStep;
	}

	m_VexList.AddVex(xA + xC);
	if (m_bUseLighting)
		m_VexList.AddNormal(xN);

	glDisable( GL_CULL_FACE		);
	m_VexList.Apply();
	glEnable( GL_CULL_FACE		);


	return true;
}


//////////////////////////////////////////////////////////////////////
/// Draw Hyperbola

bool COGLMVFilterBase::DrawHyperbola(const COGLVertex& xC,
									const COGLVertex& xA, const COGLVertex& xB, 
									float fAngleStep)
{
	MemObj<MultiV<float> > mvList;
	MultiV<float> vA, vB, vX, vN, vD;
	COGLVertex xN;

	double dTheta = 0, dAsymptoteLength = 100.0;
	double dRadStep;
	double dPi = acos(-1.0);


	if (fAngleStep <= 0.0f || fAngleStep > 90.0f ||
		xA.Mag() == 0 || xB.Mag() == 0 || xA == xB)
		return false;

	int i, iSteps = int(180.0f / fAngleStep);
	dRadStep = 0.98*dPi / double(iSteps);

	//dAMag = sqrt(xA[0]*xA[0] + xA[1]*xA[1] + xA[2]*xA[2]);
	//dBMag = sqrt(xB[0]*xB[0] + xB[1]*xB[1] + xB[2]*xB[2]);
	//dBAFac = dBMag / dAMag;

	vA = m_E3Base.Vec(xA[0], xA[1], xA[2]);
	vB = m_E3Base.Vec(xB[0], xB[1], xB[2]);
	vN = *(vA^vB);
	xN.Set(vN[1], vN[2], vN[3]);

	m_VexList.Reset();
	m_VexList.SetMode(GL_LINE_STRIP);

	if (m_bUseLighting)
	{
		m_mMaterial[MAT_CIRCLE].Ambient(m_ActiveColor);
		m_mMaterial[MAT_CIRCLE].Apply();
		m_VexList.AddNormal(xN);
	}

	// Draw first branch
	dTheta = -0.49*dPi;
	// Add Asymptotic vector
	vX = (float(1.0 / cos(dTheta)) & vA) + (float(tan(dTheta)) & vB);
	vD = (float(1.0 / cos(-0.495*dPi)) & vA) + (float(tan(-0.495*dPi)) & vB);
	vD -= vX;
	vD /= float(sqrt(vD.Mag2()));
	vX += float(dAsymptoteLength) ^ vD;

	m_VexList.AddVex(vX[1]+xC[0], vX[2]+xC[1], vX[3]+xC[2]);
	if (m_bUseLighting)
		m_VexList.AddNormal(xN);

	// Draw branch
	for(i=0;i<iSteps;i++)
	{
		vX = (float(1.0 / cos(dTheta)) & vA) + (float(tan(dTheta)) & vB);
		m_VexList.AddVex(vX[1]+xC[0], vX[2]+xC[1], vX[3]+xC[2]);

		if (m_bUseLighting)
			m_VexList.AddNormal(xN);

		dTheta += dRadStep;
	}

	// Add Asymptotic vector
	dTheta = 0.49*dPi;
	vX = (float(1.0 / cos(dTheta)) & vA) + (float(tan(dTheta)) & vB);
	vD = (float(1.0 / cos(0.495*dPi)) & vA) + (float(tan(0.495*dPi)) & vB);
	vD -= vX;
	vD /= float(sqrt(vD.Mag2()));
	vX += float(dAsymptoteLength) ^ vD;
	m_VexList.AddVex(vX[1]+xC[0], vX[2]+xC[1], vX[3]+xC[2]);
	if (m_bUseLighting)
		m_VexList.AddNormal(xN);

	glDisable( GL_CULL_FACE		);
	m_VexList.Apply();
	glEnable( GL_CULL_FACE		);


	/////////////////////////////////////////////////
	// Draw second branch
	m_VexList.Reset();
	m_VexList.SetMode(GL_LINE_STRIP);

	dTheta = -0.49*dPi;
	// Add Asymptotic vector
	vX = (float(1.0 / -cos(dTheta)) & vA) + (float(tan(dTheta)) & vB);
	vD = (float(1.0 / -cos(-0.495*dPi)) & vA) + (float(tan(-0.495*dPi)) & vB);
	vD -= vX;
	vD /= float(sqrt(vD.Mag2()));
	vX += float(dAsymptoteLength) ^ vD;

	m_VexList.AddVex(vX[1]+xC[0], vX[2]+xC[1], vX[3]+xC[2]);
	if (m_bUseLighting)
		m_VexList.AddNormal(xN);

	// Draw branch
	for(i=0;i<iSteps;i++)
	{
		vX = (float(1.0 / -cos(dTheta)) & vA) + (float(tan(dTheta)) & vB);
		m_VexList.AddVex(vX[1]+xC[0], vX[2]+xC[1], vX[3]+xC[2]);

		if (m_bUseLighting)
			m_VexList.AddNormal(xN);

		dTheta += dRadStep;
	}

	// Add Asymptotic vector
	dTheta = 0.49*dPi;
	vX = (float(1.0 / -cos(dTheta)) & vA) + (float(tan(dTheta)) & vB);
	vD = (float(1.0 / -cos(0.495*dPi)) & vA) + (float(tan(0.495*dPi)) & vB);
	vD -= vX;
	vD /= float(sqrt(vD.Mag2()));
	vX += float(dAsymptoteLength) ^ vD;
	m_VexList.AddVex(vX[1]+xC[0], vX[2]+xC[1], vX[3]+xC[2]);
	if (m_bUseLighting)
		m_VexList.AddNormal(xN);

	glDisable( GL_CULL_FACE		);
	m_VexList.Apply();
	glEnable( GL_CULL_FACE		);


	return true;
}




//////////////////////////////////////////////////////////////////////
/// Draw Cylinder
///
/// Draw cylinder as surface spanned by two circles
/// xC: Center of bottom circle
/// xN: Vector pointing towards other end of cylinder giving length of it
/// fR: Radius of Cylinder
/// cCol: Color of Cylinder
/// fAngleStep: angle step for drawing circles

bool COGLMVFilterBase::DrawCylinder(const COGLVertex& xC, 
										 const COGLVertex& xN, 
										 float fR, 
										 float fAngleStep,
										 COGLVertexList *_pVexList,
										 bool bDoDraw)
{
	MemObj<COGLVertex> mC, mN;
	MemObj<COGLColor> mCol;
	Mem<float> mR;

	mC.Set(2);
	mN.Set(2);
	mCol.Set(2);
	mR.Set(2);

	mC[0] = xC;
	mC[1] = xC + xN;

	mN[0] = xN;
	mN[0].Norm();
	mN[1] = mN[0];

	mR[0] = mR[1] = fR;
	mCol[0] = mCol[1] = m_ActiveColor;

	return DrawCircleSurface(mC, mN, mR, mCol, fAngleStep, _pVexList, bDoDraw);
}




//////////////////////////////////////////////////////////////////////
/// Draw Circle Surface
///
/// Draw surface spanned by a set of circles
/// mC: List of circle centers
/// mN: List of circle normals
/// mR: List of circle radii
/// mCol: List of colors for circles
/// fAngleStep: angle step for drawing circles

bool COGLMVFilterBase::DrawCircleSurface(const MemObj<COGLVertex>& mC, 
										 const MemObj<COGLVertex>& mN, 
										 const Mem<float>& mR, 
										 const MemObj<COGLColor>& mCol,
										 float fAngleStep,
										 COGLVertexList *_pVexList,
										 bool bDoDraw)
{
	Mem<int> mIdxList;
	MemObj<MultiV<float> > mvList;
	MultiV<float> vU, vX, vR, vN, vNPrev, vB, vR12, vXN;
	COGLVertex xC;
	COGLVertexList *pVexList = 0;
	float fR, fAngle, fMinAngle, fMaxAngle;
	bool bUseCol;

	bool bNegRadius = false;


	if (fAngleStep <= 0.0f || fAngleStep > 180.0f)
		return false;

	if (_pVexList != 0)
	{
		pVexList = _pVexList;
	}
	else
	{
		pVexList = &m_VexList;
	}

	COGLVertexList &VexList = *pVexList;

	//fAngleStep = 60.0f;
	float fTexX, fTexY, fTexStepX, fTexStepY;
	int i, iSteps = int(360.0f / fAngleStep) + 1;
	int iCircle, iCircleCount = mC.Count();
	int iIdx, iPos;

	if (iCircleCount < 2)
		return false;

	if (mCol.Count() == 0)
	{
		bUseCol = false;
	}
	else
	{
		bUseCol = true;
	}

	float fTexMaxX = 1.0f;
	float fTexMaxY = 1.0f;
	float fMaxX = 1.0f, fMaxY = 1.0f;

	if (m_bUseAbsTexCoords)
	{
		COGLVertex xD;
		fMaxX = 0.0f;
		fMaxY = 0.0f;

		for(iIdx = 0; iIdx < iCircleCount-1; iIdx++)
		{
			if (float(mR[iIdx]) > fMaxY)
				fMaxY = mR[iIdx];

			xD = mC[iIdx+1] - mC[iIdx];
			fMaxX += xD.Mag();
		}
		if (float(mR[iIdx]) > fMaxY)
			fMaxY = mR[iIdx];
	}

	if (m_fTexRepFac != 0.0f)
	{
		fTexMaxX = fMaxX / m_fTexRepFac;
		fTexMaxY = fMaxY / m_fTexRepFac;
	}

	fTexStepY = fTexMaxY / float(iSteps - 1);
	fTexStepX = fTexMaxX / float(iCircleCount - 1);

	mIdxList.Set((iCircleCount - 1) * 2 * iSteps);

	vN = m_E3Base.Vec(mN[0][0], mN[0][1], mN[0][2]);
	if (vN.IsZero(m_fSensitivity))
		return false;

	vNPrev = vN;
	vU = vN & m_E3Base.vI;
	fAngleStep = (fAngleStep * m_fPi)/180.0f;
	vR = m_E3Base.Rotor(vU, fAngleStep, false);

	FactorBlade<float>(vU, mvList, true, 0, 0, m_fSensitivity);

	vB = mvList[0];
	xC = mC[0];
	fR = mR[0];
	vX = fR & vB;
	
	VexList.Reset();
	VexList.SetMode(GL_TRIANGLE_STRIP);

	fTexX = 0.0f;
	for(i=0,fTexY = 1;i<iSteps;i++, fTexY -= fTexStepY)
	{
		if (m_bUseLighting)
		{
			vXN = vX / float(sqrt(Scalar(vX * vX)));
			VexList.AddNormal(vXN[1], vXN[2], vXN[3]);
		}
		if (bUseCol)
			VexList.AddCol(mCol[0]);
		
		VexList.AddVex(vX[1]+xC[0], vX[2]+xC[1], vX[3]+xC[2]);
		VexList.AddTex(fTexX, fTexY, 0);

		vX = (vR & vX) & (~vR);
	}
	
	iIdx = iPos = 0;
	fMinAngle = -m_fPi / 1800.0f;
	fMaxAngle =  m_fPi / 1800.0f;
	fTexX = fTexStepX;

	for ( iCircle = 1; iCircle < iCircleCount; iCircle++, fTexX += fTexStepX )
	{
		const COGLColor &rCol = mCol[iCircle];

		xC = mC[iCircle];
		vN = m_E3Base.Vec(mN[iCircle][0], mN[iCircle][1], mN[iCircle][2]);
		if (vN.IsZero(m_fSensitivity))
			return false;

		vU = vN & m_E3Base.vI;
		vR = m_E3Base.Rotor(vU, fAngleStep, false);

		fAngle = acos(Scalar(vNPrev * vN));
		if (fAngle <= fMinAngle || fAngle >= fMaxAngle)
		{
			vR12 = m_E3Base.Rotor(vNPrev ^ vN, fAngle, false);
			vB = vR12 & (vB & (~vR12));
		}

		vX = mR[iCircle] & vB;

		for(i=0, fTexY = 1;i<iSteps;i++, fTexY -= fTexStepY)
		{
			if (m_bUseLighting)
			{
				vXN = vX / float(sqrt(Scalar(vX * vX)));
				VexList.AddNormal(vXN[1], vXN[2], vXN[3]);
			}
			if (bUseCol)
				VexList.AddCol(rCol);

			VexList.AddVex(vX[1]+xC[0], vX[2]+xC[1], vX[3]+xC[2]);
			VexList.AddTex(fTexX, fTexY, 0);

			mIdxList[iIdx++] = iPos;
			mIdxList[iIdx++] = iPos + iSteps;
			iPos++;

			vX = (vR & vX) & (~vR);
		}
		
		vNPrev = vN;
	}

	VexList.SetIdxList(mIdxList);

	if (bDoDraw)
	{
		if (m_bUseLighting)
		{
			m_mMaterial[MAT_CUBE].Ambient(m_ActiveColor);
			m_mMaterial[MAT_CUBE].Diffuse(1.0, 1.0, 1.0, m_ActiveColor.Data()[3]);
			m_mMaterial[MAT_CUBE].Apply();
		}

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);
		glDisable( GL_CULL_FACE		);

		VexList.Apply();

		glEnable( GL_CULL_FACE		);
		glDisable(GL_POLYGON_OFFSET_FILL);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Line Surface
///
/// Draw surface spanned by a set of lines
/// mC: List of line centers
/// mD: List of line directions
/// mL: List of line lengths
/// mCol: List of colors for lines
/// iSteps: number of separations for drawing lines

bool COGLMVFilterBase::DrawLineSurface(const MemObj<COGLVertex>& mC, 
										 const MemObj<COGLVertex>& mD, 
										 const Mem<float>& mL, 
										 const MemObj<COGLColor>& mCol,
										 int iSteps,
										 COGLVertexList *_pVexList,
										 bool bDoDraw)
{
	Mem<int> mIdxList;
	MemObj<MultiV<float> > mvList;
	MultiV<float> vX, vD[2], vN, vNPrev, vC[3], vU;
	COGLVertex xC, xD, xX, xN;
	COGLVertexList *pVexList = 0;
	float fL, fPosStep, fMinPos;
	bool bUseCol, bClosedSurface;


	if (iSteps <= 1 || iSteps > 1000)
		return false;

	if (_pVexList != 0)
	{
		pVexList = _pVexList;
	}
	else
	{
		pVexList = &m_VexList;
	}

	COGLVertexList &VexList = *pVexList;

	int i;
	int iLine, iNextLine, iLineCount = mC.Count();
	int iIdx, iPos;
	float fTexX, fTexY, fTexStepX, fTexStepY;
	float fTexMaxX = 1.0f;
	float fTexMaxY = 1.0f;
	float fMaxX = 1.0f, fMaxY = 1.0f;

	if (m_bUseAbsTexCoords)
	{
		COGLVertex xD;
		fMaxX = 0.0f;
		fMaxY = 0.0f;

		for(iIdx = 0; iIdx < iLineCount-1; iIdx++)
		{
			if (float(mL[iIdx]) > fMaxX)
				fMaxX = mL[iIdx];

			xD = mC[iIdx+1] - mC[iIdx];
			fMaxY += xD.Mag();
		}
		if (float(mL[iIdx]) > fMaxX)
			fMaxX = mL[iIdx];
	}

	if (m_fTexRepFac != 0.0f)
	{
		fTexMaxX = fMaxX / m_fTexRepFac;
		fTexMaxY = fMaxY / m_fTexRepFac;
	}

	fTexStepX = fTexMaxX / float(iSteps - 1);
	fTexStepY = fTexMaxY / float(iLineCount - 1);


	if (iLineCount < 2)
		return false;

	if (mCol.Count() == 0)
	{
		bUseCol = false;
	}
	else
	{
		bUseCol = true;
	}

	if (mC[0] == mC[iLineCount-1] && mD[0] == mD[iLineCount-1])
		bClosedSurface = true;
	else
		bClosedSurface = false;


	mIdxList.Set((iLineCount - 1) * 2 * iSteps + 3*(iLineCount - 1));

	vC[0] = m_E3Base.Vec(mC[0][0], mC[0][1], mC[0][2]);
	vC[1] = m_E3Base.Vec(mC[1][0], mC[1][1], mC[1][2]);
	vD[0] = m_E3Base.Vec(mD[0][0], mD[0][1], mD[0][2]);

	vN = *(vD[0] ^ (vC[1]-vC[0]));

	vN = vN / float(sqrt(Scalar(vN * vN)));
	vNPrev = vN;

	xC = mC[0];
	fL = mL[0];
	fPosStep = fL / float(iSteps-1);
	fMinPos = -fL * 0.5f;

	xD = mD[0];
	xX = xC - 0.5f * fL * xD;
	xD = fPosStep * xD;

	VexList.Reset();
	VexList.SetMode(GL_TRIANGLE_STRIP);

	fTexX = 0;
	fTexY = 0;
	xN.Set(vN[1], vN[2], vN[3]);
	for(i=0;i<iSteps;i++, fTexX += fTexStepX)
	{
		if (m_bUseLighting)
		{
			VexList.AddNormal(xN);
		}
		if (bUseCol)
			VexList.AddCol(mCol[0]);
		
		VexList.AddVex(xX);
		VexList.AddTex(fTexX, fTexY, 0);

		xX = xX + xD;
	}
	
	iIdx = iPos = 0;

	for ( iLine = 1, fTexY = fTexStepY; iLine < iLineCount; iLine++, fTexY += fTexStepY )
	{
		const COGLColor &rCol = mCol[iLine];

		xC = mC[iLine];

		if (iLine < iLineCount-1 || bClosedSurface)
		{
			if (bClosedSurface)
				iNextLine = 0;
			else
				iNextLine = iLine + 1;

			vC[1] = m_E3Base.Vec(mC[iLine][0], mC[iLine][1], mC[iLine][2]);
			vC[2] = m_E3Base.Vec(mC[iNextLine][0], mC[iNextLine][1], mC[iNextLine][2]);

			vD[1] = m_E3Base.Vec(mD[iLine][0], mD[iLine][1], mD[iLine][2]);

			vU = *((vC[1]-vC[0]) ^ (vC[2]-vC[1]));
			if (Scalar(vD[0] * vU) > 0.0)
				vD[0] = -vD[0];
			if (Scalar(vD[1] * vU) > 0.0)
				vD[1] = -vD[1];

			vNPrev = *(vD[0]^(vC[1]-vC[0]));
			vN = *(vD[1]^(vC[2]-vC[1]));

			vN = vN / float(sqrt(Scalar(vN * vN)));
			vNPrev = vNPrev / float(sqrt(Scalar(vNPrev * vNPrev)));
			vX = vN + vNPrev;
			vN = vX / float(sqrt(Scalar(vX * vX)));
			vNPrev = vN;

			vC[0] = vC[1];
			vC[1] = vC[2];
			vD[0] = vD[1];

			if (iLine == 1)
				VexList.GetNormal(0).Set(vN[1], vN[2], vN[3]);

			if (bClosedSurface && iLine == iLineCount-1)
				VexList.GetNormal(0).Set(vN[1], vN[2], vN[3]);
		}
		else
		{
			vN = vNPrev;
		}

		xC = mC[iLine];
		fL = mL[iLine];
		fPosStep = fL / float(iSteps-1);
		fMinPos = -fL * 0.5f;

		xD = mD[iLine];

		xX = xC - 0.5f * fL * xD;
		xD = fPosStep * xD;
		xN.Set(vN[1], vN[2], vN[3]);

		for(i=0, fTexX = 0;i<iSteps;i++, fTexX += fTexStepX)
		{
			if (m_bUseLighting)
			{
				VexList.AddNormal(xN);
			}
			if (bUseCol)
				VexList.AddCol(rCol);

			VexList.AddVex(xX);
			VexList.AddTex(fTexX, fTexY, 0);

			mIdxList[iIdx++] = iPos;
			mIdxList[iIdx++] = iPos + iSteps;
			iPos++;

			xX = xX + xD;
		}
		mIdxList[iIdx++] = iPos - 1 + iSteps;
		mIdxList[iIdx++] = iPos;
		mIdxList[iIdx++] = iPos;
	}

	VexList.SetIdxList(mIdxList);

	if (bDoDraw)
	{
		if (m_bUseLighting)
		{
			m_mMaterial[MAT_CUBE].Ambient(m_ActiveColor);
			m_mMaterial[MAT_CUBE].Diffuse(1.0, 1.0, 1.0, m_ActiveColor.Data()[3]);
			m_mMaterial[MAT_CUBE].Apply();
		}

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);
		glDisable( GL_CULL_FACE		);

		VexList.Apply();

		glEnable( GL_CULL_FACE		);
		glDisable(GL_POLYGON_OFFSET_FILL);
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Draw Point Surface
///
/// Draw surface spanned by a set of Points
/// Points are ordered left to right, rowwise.
/// iRowCount: Number of rows (bottom to top)
/// iColCount: Number of cols (left to right)
/// mP: List of points
/// mN: List of normals
/// mT: List of texture coordinates
/// mCol: List of colors
/// fNormScale: Scale of normals drawn.

bool COGLMVFilterBase::DrawPointSurface(int iRowCount, int iColCount,
										 const MemObj<COGLVertex>& mP, 
										 const MemObj<COGLVertex>& mN, 
										 const MemObj<COGLVertex>& mT, 
										 const MemObj<COGLColor>& mCol,
										 bool bNegateNormals,
										 float fNormScale,
										 MemObj<COGLVertexList*> *_pmVexList,
										 bool bDoDraw)
{
	Mem<int> mIdxList;
	COGLVertexList *pVexList = 0;
	COGLVertexList *pNormVexList = 0;
	bool bUseCol, bUseNormals, bUseTex, bDrawNormLines;

	if (fNormScale != 0)
		bDrawNormLines = true;
	else
		bDrawNormLines = false;


	if (_pmVexList != 0)
	{
		if (_pmVexList->Count() == 1)
		{
			pVexList = (*_pmVexList)[0];
			pNormVexList = &m_NormVexList;
		}
		else if (_pmVexList->Count() == 2)
		{
			pVexList = (*_pmVexList)[0];
			pNormVexList = (*_pmVexList)[1];
		}
		else
		{
			pVexList = &m_VexList;
			pNormVexList = &m_NormVexList;
		}
	}
	else
	{
		pVexList = &m_VexList;
		pNormVexList = &m_NormVexList;
	}

	COGLVertexList &VexList = *pVexList;
	COGLVertexList &NormVexList = *pNormVexList;

	int iPointCount = (int) mP.Count();

	if (iRowCount * iColCount > iPointCount ||
		iRowCount < 2 || iColCount < 2)
		return false;

	if (mCol.Count() == 0)
	{
		bUseCol = false;
	}
	else
	{
		bUseCol = true;
	}

	if (mN.Count() == 0)
	{
		bUseNormals = false;
	}
	else
	{
		bUseNormals = true;
	}

	if (mT.Count() == 0)
	{
		bUseTex = false;
	}
	else
	{
		bUseTex = true;
	}

	int iRow, iCol;
	int iIdx, iPos;
	int iColPos, iRowPos;
	int iPosL, iPosR, iPosT, iPosB;
	COGLVertex dX, dY, xN;

	iColPos = 0;
	iRowPos = 0;

	if (bDrawNormLines)
	{
		NormVexList.Reset();
		NormVexList.SetMode(GL_LINES);
	}

	VexList.Reset();
	VexList.SetMode(GL_TRIANGLE_STRIP);
	for ( iPos = 0; iPos < iPointCount; iPos++ )
	{
		VexList.AddVex(mP[iPos]);

		if (bUseTex)
			VexList.AddTex(mT[iPos]);

		if (m_bUseLighting)
		{
			if (bDrawNormLines)
				NormVexList.AddVex(mP[iPos]);
	
			if (bUseNormals)
			{
				if (bNegateNormals)
				{
					VexList.AddNormal(-mN[iPos]);
					if (bDrawNormLines)
						NormVexList.AddVex(mP[iPos] - fNormScale * mN[iPos]);
				}
				else
				{
					VexList.AddNormal(mN[iPos]);
					if (bDrawNormLines)
						NormVexList.AddVex(mP[iPos] + fNormScale * mN[iPos]);
				}
			}
			else
			{
				if ((iPosL = iColPos - 1) < 0)
					iPosL = 0;
				if ((iPosR = iColPos + 1) >= iColCount)
					iPosR = iColCount - 1;

				if ((iPosT = iRowPos + 1) >= iRowCount)
					iPosT = iRowCount - 1;
				if ((iPosB = iRowPos - 1) < 0)
					iPosB = 0;

				iPosL += iRowPos * iColCount;
				iPosR += iRowPos * iColCount;
				iPosT = iColPos + iPosT * iColCount;
				iPosB = iColPos + iPosB * iColCount;

				dX = 0.5f * (mP[iPosR] - mP[iPosL]);
				dY = 0.5f * (mP[iPosT] - mP[iPosB]);
				
				xN[0] = dX[1]*dY[2] - dX[2]*dY[1];
				xN[1] = dX[2]*dY[0] - dX[0]*dY[2];
				xN[2] = dX[0]*dY[1] - dX[1]*dY[0];

				xN.Norm();
				if (bNegateNormals)
				{
					VexList.AddNormal(-xN);
					if (bDrawNormLines)
						NormVexList.AddVex(mP[iPos] - fNormScale * xN);
				}
				else
				{
					VexList.AddNormal(xN);
					if (bDrawNormLines)
						NormVexList.AddVex(mP[iPos] + fNormScale * xN);
				}

			}
		}

		if (bUseCol)
		{
			VexList.AddCol(mCol[iPos]);
		}

		if (++iColPos >= iColCount)
		{
			iColPos = 0;
			iRowPos++;
		}
	}

	mIdxList.Set((iRowCount - 1) * 2 * iColCount + (iRowCount - 2));

	// If no texture coordinates are given, then calculate some.
	if (!bUseTex)
	{
		float fTexRow, fTexCol, fTexRowStep, fTexColStep;
		float fTexMaxX = 1.0f;
		float fTexMaxY = 1.0f;
		float fMaxX = 1.0f, fMaxY = 1.0f;

		if (m_bUseAbsTexCoords)
		{
			COGLVertex xD;
			fMaxX = 0.0f;
			fMaxY = 0.0f;

			for(iCol = 0; iCol < iColCount-1; iCol++)
			{
				xD = mP[iCol+1] - mP[iCol];
				fMaxX += xD.Mag();
			}

			for(iRow = 0, iPos = 0; iRow < iRowCount-1; iRow++, iPos += iColCount)
			{
				xD = mP[iPos+iColCount] - mP[iPos];
				fMaxY += xD.Mag();
			}
		}

		if (m_fTexRepFac != 0.0f)
		{
			fTexMaxX = fMaxX / m_fTexRepFac;
			fTexMaxY = fMaxY / m_fTexRepFac;
		}

		fTexRowStep = fTexMaxX / float(iRowCount-1);
		fTexColStep = fTexMaxY / float(iColCount-1);

		for(iRow = 0, fTexRow = 1; iRow < iRowCount; iRow++, fTexRow -= fTexRowStep)
		{
			for(iCol = 0, fTexCol = 0; iCol < iColCount; iCol++, iPos++, fTexCol += fTexColStep)
			{
				VexList.AddTex(fTexCol, fTexRow, 0);
			}
		}
	}

	iIdx = iPos = 0;
	for(iRow = 0; iRow < iRowCount-1; iRow++)
	{
		for(iCol = 0; iCol < iColCount; iCol++, iPos++)
		{
			mIdxList[iIdx++] = iPos;
			mIdxList[iIdx++] = iPos + iColCount;
		}
		
		if (++iRow >= iRowCount-1)
			break;

		iPos += iColCount - 1;
		mIdxList[iIdx++] = iPos;

		for(iCol = iColCount; iCol > 0; iCol--, iPos--)
		{
			mIdxList[iIdx++] = iPos;
			mIdxList[iIdx++] = iPos + iColCount;
		}

		if (iRow+1 >= iRowCount-1)
			break;

		iPos += iColCount + 1;
		mIdxList[iIdx++] = iPos;
	}


	VexList.SetIdxList(mIdxList);

	if (bDoDraw)
	{
		m_ActiveColor.Apply();
		if (m_bUseLighting)
		{
			m_mMaterial[MAT_CUBE].Diffuse(1.0, 1.0, 1.0, m_ActiveColor.Data()[3]);
			m_mMaterial[MAT_CUBE].Ambient(m_ActiveColor);
			m_mMaterial[MAT_CUBE].Apply();
		}

		glDisable( GL_CULL_FACE		);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);

		VexList.Apply();

		glDisable(GL_POLYGON_OFFSET_FILL);

		if (bDrawNormLines)
		{
			m_ActiveColor.Apply();
			if (m_bUseLighting)
			{
				m_mMaterial[MAT_CUBE].Diffuse(1.0, 1.0, 1.0, m_ActiveColor.Data()[3]);
				m_mMaterial[MAT_CUBE].Ambient(m_ActiveColor);
				m_mMaterial[MAT_CUBE].Apply();
			}

			NormVexList.Apply();
		}

		glEnable( GL_CULL_FACE		);
	}


	return true;
}



//////////////////////////////////////////////////////////////////////
/// Draw Point Grid
///
/// Draw grid spanned by a set of Points
/// Points are ordered left to right, rowwise.
/// iRowCount: Number of rows (bottom to top)
/// iColCount: Number of cols (left to right)
/// mP: List of points
/// mN: List of normals
/// mCol: List of colors

bool COGLMVFilterBase::DrawPointGrid(int iRowCount, int iColCount,
										 const MemObj<COGLVertex>& mP, 
										 const MemObj<COGLVertex>& mN, 
										 const MemObj<COGLColor>& mCol,
										 bool bNegateNormals,
										 COGLVertexList *_pVexList,
										 bool bDoDraw)
{
	Mem<int> mIdxList;
	COGLVertexList *pVexList = 0;
	bool bUseCol, bUseNormals;

	if (_pVexList != 0)
	{
		pVexList = _pVexList;
	}
	else
	{
		pVexList = &m_VexList;
	}

	COGLVertexList &VexList = *pVexList;

	int iPointCount = (int) mP.Count();

	if (iRowCount * iColCount > iPointCount ||
		iRowCount < 2 || iColCount < 2)
		return false;

	if (mCol.Count() == 0)
	{
		bUseCol = false;
	}
	else
	{
		bUseCol = true;
	}

	if (mN.Count() == 0)
	{
		bUseNormals = false;
	}
	else
	{
		bUseNormals = true;
	}

	int iRow, iCol;
	int iIdx, iPos;
	int iColPos, iRowPos;
	int iPosL, iPosR, iPosT, iPosB;
	COGLVertex dX, dY, xN;

	iColPos = 0;
	iRowPos = 0;

	VexList.Reset();
	VexList.SetMode(GL_LINES);
	for ( iPos = 0; iPos < iPointCount; iPos++ )
	{
		VexList.AddVex(mP[iPos]);

		if (m_bUseLighting)
		{
			if (bUseNormals)
			{
				if (bNegateNormals)
					VexList.AddNormal(-mN[iPos]);
				else
					VexList.AddNormal(mN[iPos]);
			}
			else
			{
				if ((iPosL = iColPos - 1) < 0)
					iPosL = 0;
				if ((iPosR = iColPos + 1) >= iColCount)
					iPosR = iColCount - 1;

				if ((iPosT = iRowPos + 1) >= iRowCount)
					iPosT = iRowCount - 1;
				if ((iPosB = iRowPos - 1) < 0)
					iPosB = 0;

				iPosL += iRowPos * iColCount;
				iPosR += iRowPos * iColCount;
				iPosT = iColPos + iPosT * iColCount;
				iPosB = iColPos + iPosT * iColCount;

				dX = 0.5f * (mP[iPosR] - mP[iPosL]);
				dY = 0.5f * (mP[iPosT] - mP[iPosB]);
				
				xN[0] = dX[1]*dY[2] - dX[2]*dY[1];
				xN[1] = dX[2]*dY[0] - dX[0]*dY[2];
				xN[2] = dX[0]*dY[1] - dX[1]*dY[0];

				xN.Norm();
				if (bNegateNormals)
					VexList.AddNormal(-xN);
				else
					VexList.AddNormal(xN);

			}
		}

		if (bUseCol)
			VexList.AddCol(mCol[iPos]);

		if (++iColPos >= iColCount)
		{
			iColPos = 0;
			iRowPos++;
		}
	}

	mIdxList.Set( (iColCount - 1) * ((iRowCount - 1) *  4 + 4) );

	iIdx = iPos = 0;
	for(iRow = 0; iRow < iRowCount; iRow++)
	{
		if (iRow < iRowCount - 1)
		{
			for(iCol = 0; iCol < iColCount - 1; iCol++, iPos++)
			{
				mIdxList[iIdx++] = iPos;
				mIdxList[iIdx++] = iPos + iColCount;

				mIdxList[iIdx++] = iPos;
				mIdxList[iIdx++] = iPos + 1;
			}
		}
		else
		{
			for(iCol = 0; iCol < iColCount - 1; iCol++, iPos++)
			{
				mIdxList[iIdx++] = iPos;
				mIdxList[iIdx++] = iPos + 1;
			}
			break;
		}

		++iRow;
		mIdxList[iIdx++] = iPos;
		iPos += iColCount;
		mIdxList[iIdx++] = iPos;

		if (iRow < iRowCount - 1)
		{
			for(iCol = iColCount; iCol > 1; iCol--, iPos--)
			{
				mIdxList[iIdx++] = iPos;
				mIdxList[iIdx++] = iPos + iColCount;

				mIdxList[iIdx++] = iPos;
				mIdxList[iIdx++] = iPos - 1;
			}
		}
		else
		{
			for(iCol = iColCount; iCol > 1; iCol--, iPos--)
			{
				mIdxList[iIdx++] = iPos;
				mIdxList[iIdx++] = iPos - 1;
			}
			break;
		}

		mIdxList[iIdx++] = iPos;
		iPos += iColCount;
		mIdxList[iIdx++] = iPos;
	}

	VexList.SetIdxList(mIdxList);

	if (bDoDraw)
	{
		if (m_bUseLighting)
		{
			m_mMaterial[MAT_LINE].Ambient(m_ActiveColor);
			m_mMaterial[MAT_LINE].Apply();
		}

		glDisable( GL_CULL_FACE		);

		VexList.Apply();

		glEnable( GL_CULL_FACE		);
	}

	return true;
}



//////////////////////////////////////////////////////////////////////
/// Draw Point List (as line)
///
/// Draw line spanned by a set of Points
/// Points are ordered left to right
/// mP: List of points
/// mN: List of normals
/// mCol: List of colors

bool COGLMVFilterBase::DrawPointList(    const MemObj<COGLVertex>& mP, 
										 const MemObj<COGLVertex>& mN, 
										 const MemObj<COGLColor>& mCol,
										 bool bNegateNormals,
										 COGLVertexList *_pVexList,
										 bool bDoDraw)
{
	Mem<int> mIdxList;
	COGLVertexList *pVexList = 0;
	bool bUseCol, bUseNormals;

	if (_pVexList != 0)
	{
		pVexList = _pVexList;
	}
	else
	{
		pVexList = &m_VexList;
	}

	COGLVertexList &VexList = *pVexList;

	int iPointCount = (int) mP.Count();

	if (mCol.Count() == 0)
	{
		bUseCol = false;
	}
	else
	{
		bUseCol = true;
	}

	if (mN.Count() == 0)
	{
		bUseNormals = false;
	}
	else
	{
		bUseNormals = true;
	}

	int iPos;
	COGLVertex dA, dB, xN;

	VexList.Reset();
	VexList.SetMode(GL_LINE_STRIP);
	for ( iPos = 0; iPos < iPointCount; iPos++ )
	{
		VexList.AddVex(mP[iPos]);

		if (m_bUseLighting)
		{
			if (bUseNormals)
			{
				if (bNegateNormals)
					VexList.AddNormal(-mN[iPos]);
				else
					VexList.AddNormal(mN[iPos]);
			}
			else
			{
				if (iPos > 0 && iPos < iPointCount-1)
				{
					dA = mP[iPos] - mP[iPos-1];
					dB = mP[iPos+1] - mP[iPos];

					xN[0] = dA[1]*dB[2] - dA[2]*dB[1];
					xN[1] = dA[2]*dB[0] - dA[0]*dB[2];
					xN[2] = dA[0]*dB[1] - dA[1]*dB[0];

					xN.Norm();

					if (iPos == 1 || iPos == iPointCount-1)
					{
						if (bNegateNormals)
							VexList.AddNormal(-xN);
						else
							VexList.AddNormal(xN);
					}
					if (bNegateNormals)
						VexList.AddNormal(-xN);
					else
						VexList.AddNormal(xN);
				}
			}
		}

		if (bUseCol)
			VexList.AddCol(mCol[iPos]);
	}


	if (bDoDraw)
	{
		if (m_bUseLighting)
		{
			m_mMaterial[MAT_LINE].Ambient(m_ActiveColor);
			m_mMaterial[MAT_LINE].Apply();
		}

		glDisable( GL_CULL_FACE		);

		VexList.Apply();

		glEnable( GL_CULL_FACE		);
	}

	return true;
}




//////////////////////////////////////////////////////////////////////
/// Draw Disk

bool COGLMVFilterBase::DrawDisk(const COGLVertex& xC, 
								const COGLVertex& xA, const COGLVertex& xB,
								float fR, float fAngleStep)
{
	COGLColor Dark;
	COGLVertex xN;
	MemObj<MultiV<float> > mvList;
	MultiV<float> vU, vX, vR, vN;
	Mem<int> mIdx;
	float fAngle = 0.0f;

	if (fR <= 0.0f || fR > 1000.0f || fAngleStep <= 0.0f || fAngleStep > 180.0f)
		return false;

	int i, iSteps = int(360.0f / fAngleStep);

	mIdx.Set(iSteps+2);
//	vU = m_E3Base.Vec(xN[0], xN[1], xN[2]) & m_E3Base.vI;
	vU = m_E3Base.Vec(xA[0], xA[1], xA[2]) ^ m_E3Base.Vec(xB[0], xB[1], xB[2]);
	
	fAngleStep = (fAngleStep * m_fPi)/180.0f;
	vR = m_E3Base.Rotor(vU, fAngleStep, false);

	FactorBlade<float>(vU, mvList, true, 0, 0, m_fSensitivity);

	vX = fR & mvList[0];

	m_VexList.Reset();
	m_VexList.SetMode(GL_TRIANGLE_FAN);

	if (m_bUseLighting)
	{
		vN = *vU;
		vN /= float(sqrt(Scalar(vN * vN)));
		xN = &vN[m_E3Base.iE1];

		m_mMaterial[MAT_PLANE].Ambient(m_ActiveColor);
		m_mMaterial[MAT_PLANE].Diffuse(1.0, 1.0, 1.0, m_ActiveColor.Data()[3]);
		m_mMaterial[MAT_PLANE].Apply();

		m_VexList.AddNormal(xN);
		m_VexList.AddNormal(xN);
	}


	mIdx[0] = 0;
	mIdx[1] = iSteps + 1;
	m_VexList.AddVex(xC[0], xC[1], xC[2]);
	m_VexList.AddVex(vX[1]+xC[0], vX[2]+xC[1], vX[3]+xC[2]);

	float fTexMaxR = 0.5f;
	float fMaxR = 0.5f;

	if (m_bUseAbsTexCoords)
	{
		fMaxR = fR;
	}

	if (m_fTexRepFac != 0.0f)
	{
		fTexMaxR = fMaxR / m_fTexRepFac;
	}


	m_VexList.AddTex(0.5f,0.5f,0);
	m_VexList.AddTex(0.5f + fTexMaxR, 0.5f,0);

	for(i=0;i<iSteps;i++)
	{
		fAngle += fAngleStep;
		vX = (vR & vX) & (~vR);
		m_VexList.AddVex(vX[1]+xC[0], vX[2]+xC[1], vX[3]+xC[2]);
		m_VexList.AddTex(0.5f + fTexMaxR*cos(fAngle), 0.5f - fTexMaxR*sin(fAngle), 0);
	
		if (m_bUseLighting)
			m_VexList.AddNormal(xN);

		mIdx[i+2] = iSteps - i;
	}
/*
	m_VexList.AddVex(m_VexList[0]);
	m_VexList.AddTex(m_VexList.GetTex(0));

	if (m_bUseLighting)
		m_VexList.AddNormal(xN);
*/
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0, 1.0);

	m_VexList.Apply();

	Dark = m_fBackFaceColFac * m_ActiveColor;

	Dark.Apply();
	m_VexList.InvertNormals(0.5f);
	m_VexList.SetIdxList(mIdx);
	m_VexList.Apply();
	m_ActiveColor.Apply();

	glDisable(GL_POLYGON_OFFSET_FILL);

	return true;
}




//////////////////////////////////////////////////////////////////////
/// Draw Line

bool COGLMVFilterBase::DrawLine(const COGLVertex& xP, const COGLVertex& xD, 
								bool bEndPoints, int iStipple)
{
	m_VexList.Reset();
	m_VexList.SetMode(GL_LINES);

	if (bEndPoints)
	{
		if (m_bUseLighting)
		{
			m_mMaterial[MAT_LINE].Ambient(m_ActiveColor);
			m_mMaterial[MAT_LINE].Apply();
			
			m_VexList.AddVex(xP);
			m_VexList.AddVex(xD);
			
			m_VexList.AddNormal(xP - xD);
			m_VexList.AddNormal(xD - xP);
		}
		else
		{
			m_VexList.AddVex(xP);
			m_VexList.AddVex(xD);
		}
	}
	else
	{
		if (m_bUseLighting)
		{
			m_mMaterial[MAT_LINE].Ambient(m_ActiveColor);
			m_mMaterial[MAT_LINE].Apply();
			
			m_VexList.AddVex(xP - 0.5f * xD);
			m_VexList.AddVex(xP + 0.5f * xD);
			
			m_VexList.AddNormal(-xD);
			m_VexList.AddNormal(xD);
		}
		else
		{
			m_VexList.AddVex(xP - 0.5f * xD);
			m_VexList.AddVex(xP + 0.5f * xD);
		}
	}
	
	glDisable( GL_CULL_FACE		);
	if (iStipple != 0)
	{
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(1, iStipple);
	}
	
	m_VexList.Apply();

	if (iStipple != 0)
	{
		glDisable(GL_LINE_STIPPLE);
	}
	glEnable( GL_CULL_FACE		);

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Draw Vector
///
/// fTipSize: length of tip
/// fLtoW: Ratio of length to width

bool COGLMVFilterBase::DrawVector(const COGLVertex& xA, const COGLVertex& xB, 
								  int iStipple)
{
	DeclareE3GAVarsLocal(float);
	MultiV<float> vA, vB, vR, vDir, vX, vY, vP;

	float fAngleStep;
	int i, iSteps = 16;
	Mem<int> mIdx;
	COGLColor Dark;
	COGLVertex xDir, xX;

	Dark = m_fBackFaceColFac * m_ActiveColor;

	mIdx.Set(iSteps+2);
	fAngleStep = 2.0f * m_fPi / float(iSteps);

	COGLVexListE3 VexList;

//	VexList.AddVex(pfVec1);
	VexList.AddVex(xB);

	vA = E3GA<float>::Vec(xA[0], xA[1], xA[2]);
	vB = E3GA<float>::Vec(xB[0], xB[1], xB[2]);

	vDir = vB - vA;
	if (vDir == vZero)
		return false;

	xDir = &vDir[1];

	// Normal for tip of vector
	if (m_bUseLighting)
	{
		VexList.AddNormal(xDir);
		m_mMaterial[MAT_VECTOR].Diffuse(1.0, 1.0, 1.0, m_ActiveColor.Data()[3]);
		m_mMaterial[MAT_VECTOR].Ambient(m_ActiveColor);
		m_mMaterial[MAT_VECTOR].Apply();
	}

	vR = E3GA<float>::Rotor(vDir, fAngleStep);

	// Find vector perpendicular to xDir
	vY = vDir * vL1;
	vY.TinyToZero();
	if (vY == vZero)
	{
		vY = vDir * vL2;
		vY.TinyToZero();
		if (vY == vZero)
			return false;
	}
	
	vY &= m_fArrowLength * float( tan(double(m_fArrowAngle)) / sqrt(vY.Mag2()) );
	vX = vA + vDir - (vDir & m_fArrowLength) / (float) sqrt(vDir.Mag2());

	VexList << (vX + vY);
	if (m_bUseLighting)
		VexList.AddNormal(&vY[1]);
	mIdx[1] = iSteps + 1;

	mIdx[0] = 0;
	for(i=0;i<iSteps;i++)
	{
		vY = (vR & vY) & (~vR);
		VexList << (vX + vY);
		if (m_bUseLighting)
			VexList.AddNormal(&vY[1]);
		mIdx[i+2] = iSteps - i;
	}

	VexList.SetMode(GL_TRIANGLE_FAN);

	VexList.Apply();

	Dark.Apply();
	VexList[0] = &vX[1];
	VexList.SetIdxList(mIdx);

	if (m_bUseLighting)
	{
		xDir = -xDir.Norm();

		for(i=0;i<iSteps+2;i++)
		{
			VexList.GetNormal(i) = xDir;
		}
	}

	VexList.Apply();
	m_ActiveColor.Apply();

	xX = &vX[1];
	DrawLine(xA, xX, true, iStipple);

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Draw Cone
///
/// xP: Origin
/// xD: Direction and length of cone
/// fR: Radius of cone at end of direction vector

bool COGLMVFilterBase::DrawCone(const COGLVertex& xP, const COGLVertex& xD, float fR)
{
	DeclareE3GAVarsLocal(float);
	MultiV<float> vA, vB, vR, vDir, vX, vY, vP;

	float fAngleStep;
	int i, iSteps;
	Mem<int> mIdx;
	COGLColor Dark;
	COGLVertex xB, xDir;

	xDir = xD;
	
	iSteps = int((2.0f * m_fPi * fR) / 0.1f);
	if (iSteps < 8)
		iSteps = 8;
	else if (iSteps > 30)
		iSteps = 30;


	Dark = m_fBackFaceColFac * m_ActiveColor;

	mIdx.Set(iSteps+2);
	fAngleStep = 2.0f * m_fPi / float(iSteps);

	COGLVexListE3 VexList;
	float fTexMaxR = 0.5f;
	float fMaxR = 0.5f;
	float fTexAngle;

	if (m_bUseAbsTexCoords)
	{
		fMaxR = sqrt(pow(xD.Mag(),2) + pow(fR,2));
	}

	if (m_fTexRepFac != 0.0f)
	{
		fTexMaxR = fMaxR / m_fTexRepFac;
	}

	VexList.AddTex(0.5f,0.5f,0);
	VexList.AddTex(0.5f + fTexMaxR, 0.5f,0);

	VexList.AddVex(xP);
	xB = xP + xD;

	vA = E3GA<float>::Vec(xP[0], xP[1], xP[2]);
	vB = E3GA<float>::Vec(xB[0], xB[1], xB[2]);
	vDir = E3GA<float>::Vec(xD[0], xD[1], xD[2]);

	if (vDir == vZero)
		return false;

	// Normal for tip of vector
	if (m_bUseLighting)
	{
		VexList.AddNormal(-xD);
		m_mMaterial[MAT_VECTOR].Diffuse(1.0, 1.0, 1.0, m_ActiveColor.Data()[3]);
		m_mMaterial[MAT_VECTOR].Ambient(m_ActiveColor);
		m_mMaterial[MAT_VECTOR].Apply();
	}

	vR = E3GA<float>::Rotor(-vDir, fAngleStep);

	// Find vector perpendicular to xDir
	vY = vDir * vL1;
	vY.TinyToZero();
	if (vY == vZero)
	{
		vY = vDir * vL2;
		vY.TinyToZero();
		if (vY == vZero)
			return false;
	}
	
	vY &= fR / (float) sqrt(vY.Mag2());
	vX = vB;

	VexList << (vX + vY);
	if (m_bUseLighting)
		VexList.AddNormal(&vY[1]);
	mIdx[1] = iSteps + 1;

	mIdx[0] = 0;
	for(i=0, fTexAngle = fAngleStep;i<iSteps;i++, fTexAngle += fAngleStep)
	{
		vY = (vR & vY) & (~vR);
		VexList << (vX + vY);

		VexList.AddTex(0.5f + fTexMaxR*cos(fTexAngle), 0.5f - fTexMaxR*sin(fTexAngle), 0);

		if (m_bUseLighting)
			VexList.AddNormal(&vY[1]);

		mIdx[i+2] = iSteps - i;
	}


	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0, 1.0);

	VexList.SetMode(GL_TRIANGLE_FAN);
	VexList.Apply();

	Dark.Apply();
	VexList.InvertNormals(0.8f);
	VexList.SetIdxList(mIdx);
	VexList.Apply();
	m_ActiveColor.Apply();

	glDisable(GL_POLYGON_OFFSET_FILL);

	return true;
}




//////////////////////////////////////////////////////////////////////
/// Draw Rotor

bool COGLMVFilterBase::DrawRotor(const COGLVertex& xP, const COGLVertex& xR, 
							 	 float fAngle, float fDegAngleStep)
{
	COGLVertex xA;
	MultiV<float> vR, vA, vU;
	MemObj<MultiV<float> > mvList;
	Mem<int> mIdx;
	float fAngleStep;
	int i, iAngleSteps;
	COGLColor Dark;

	fAngleStep = fDegAngleStep * m_fPi / 180.0f;
	Dark = m_fBackFaceColFac * m_ActiveColor;

	if (!(iAngleSteps = int(fAngle / fAngleStep + 0.5)))
		iAngleSteps = 1;

	fAngleStep = fAngle / float(iAngleSteps);

	vA = m_E3Base.Vec(xR[0], xR[1], xR[2]);
	vR = m_E3Base.Rotor(vA, fAngleStep);

	vU = vA & m_E3Base.vI;
	FactorBlade<float>(vU, mvList, true, 0, 0, m_fSensitivity);
	vA = mvList[0];
	
	if (m_bUseLighting)
	{
		m_mMaterial[MAT_ROTOR].Diffuse(1.0, 1.0, 1.0, m_ActiveColor.Alpha());
		m_mMaterial[MAT_ROTOR].Ambient(m_ActiveColor);
		m_mMaterial[MAT_ROTOR].Apply();
	}

	Dark.Apply();
	DrawLine(xP, xP+xR, true);
	m_ActiveColor.Apply();

	m_VexList.Reset();
	m_VexList.SetMode(GL_TRIANGLE_FAN);

	if (m_bUseLighting)
	{
		m_mMaterial[MAT_ROTOR].Diffuse(1.0f, 1.0f, 1.0f, 0.8f);
		m_mMaterial[MAT_ROTOR].Apply();

		m_VexList.AddNormal(xR);
		m_VexList.AddNormal(xR);
	}

	xA = &vA[1];

	float fTexMaxR = 0.5f;
	float fMaxR = 0.5f;
	float fTexAngle;

	if (m_bUseAbsTexCoords)
	{
		fMaxR = xA.Mag();
	}

	if (m_fTexRepFac != 0.0f)
	{
		fTexMaxR = fMaxR / m_fTexRepFac;
	}

	m_VexList.AddTex(0.5f,0.5f,0);
	m_VexList.AddTex(0.5f + fTexMaxR, 0.5f,0);

	m_VexList.AddVex(xP);
	m_VexList.AddVex(xP + xA);

	mIdx.Set(iAngleSteps + 2);
	mIdx[0] = 0;
	mIdx[1] = iAngleSteps + 1;

	for(i=0, fTexAngle = 0;i<iAngleSteps;i++, fTexAngle += fAngleStep)
	{
		vA = (vR & vA) & (~vR);
		xA = &vA[1];
		m_VexList.AddVex(xP + xA);
		m_VexList.AddTex(0.5f + fTexMaxR*cos(fTexAngle), 0.5f - fTexMaxR*sin(fTexAngle), 0);
		
		if (m_bUseLighting)
			m_VexList.AddNormal(xR);

		mIdx[iAngleSteps - i + 1] = i + 1;
	}

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0, 1.0);

	m_VexList.Apply();
	Dark.Apply();
	
	m_VexList.SetIdxList(mIdx);
	m_VexList.InvertNormals(0.5f);
	m_VexList.Apply();

	glDisable(GL_POLYGON_OFFSET_FILL);
	m_ActiveColor.Apply();

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Draw Twist
/// xP : offset of rot. axis
/// xT : translation along rot. axis
/// xR : rot. axis

bool COGLMVFilterBase::DrawTwist(const COGLVertex& xP, const COGLVertex& xT, const COGLVertex& xR, 
							 	 float fAngle, float fDegAngleStep)
{
	COGLVertex xA;
	MultiV<float> vR, vA, vU;
	MemObj<MultiV<float> > mvList;
	Mem<int> mIdx;
	float fAngleStep;
	int i, iAngleSteps;
	COGLColor Dark;

	fAngleStep = fDegAngleStep * m_fPi / 180.0f;
	Dark = m_fBackFaceColFac * m_ActiveColor;

	if (!(iAngleSteps = int(fAngle / fAngleStep + 0.5)))
		iAngleSteps = 1;

	fAngleStep = fAngle / float(iAngleSteps);

	vA = m_E3Base.Vec(xR[0], xR[1], xR[2]);
	vR = m_E3Base.Rotor(vA, fAngleStep);

	vU = vA & m_E3Base.vI;
	FactorBlade<float>(vU, mvList, true, 0, 0, m_fSensitivity);
	vA = mvList[0];
	
	if (m_bUseLighting)
	{
		m_mMaterial[MAT_ROTOR].Diffuse(1.0, 1.0, 1.0, m_ActiveColor.Alpha());
		m_mMaterial[MAT_ROTOR].Ambient(m_ActiveColor);
		m_mMaterial[MAT_ROTOR].Apply();
	}

	Dark.Apply();
	DrawLine(xP, xP+xR, true);
	DrawVector(xP, xP+xT);
	m_ActiveColor.Apply();

	m_VexList.Reset();
	m_VexList.SetMode(GL_TRIANGLE_FAN);

	if (m_bUseLighting)
	{
		m_mMaterial[MAT_ROTOR].Diffuse(1.0f, 1.0f, 1.0f, 0.8f);
		m_mMaterial[MAT_ROTOR].Apply();

		m_VexList.AddNormal(xR);
		m_VexList.AddNormal(xR);
	}

	xA = &vA[1];

	float fTexMaxR = 0.5f;
	float fMaxR = 0.5f;
	float fTexAngle;

	if (m_bUseAbsTexCoords)
	{
		fMaxR = xA.Mag();
	}

	if (m_fTexRepFac != 0.0f)
	{
		fTexMaxR = fMaxR / m_fTexRepFac;
	}

	m_VexList.AddTex(0.5f,0.5f,0);
	m_VexList.AddTex(0.5f + fTexMaxR, 0.5f,0);

	m_VexList.AddVex(xP);
	m_VexList.AddVex(xP + xA);

	mIdx.Set(iAngleSteps + 2);
	mIdx[0] = 0;
	mIdx[1] = iAngleSteps + 1;

	for(i=0, fTexAngle = 0;i<iAngleSteps;i++, fTexAngle += fAngleStep)
	{
		vA = (vR & vA) & (~vR);
		xA = &vA[1];
		m_VexList.AddVex(xP + xA);
		m_VexList.AddTex(0.5f + fTexMaxR*cos(fTexAngle), 0.5f - fTexMaxR*sin(fTexAngle), 0);
		
		if (m_bUseLighting)
			m_VexList.AddNormal(xR);

		mIdx[iAngleSteps - i + 1] = i + 1;
	}

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0, 1.0);

	m_VexList.Apply();
	Dark.Apply();
	
	m_VexList.SetIdxList(mIdx);
	m_VexList.InvertNormals(0.5f);
	m_VexList.Apply();

	glDisable(GL_POLYGON_OFFSET_FILL);
	m_ActiveColor.Apply();

	return true;
}



//////////////////////////////////////////////////////////////////////
// Draws an arc.
// xP is offset vector. Arc is drawn from (xA-xP) to (xB-xP) with radius 'fRadius'.
// if bFilled = true then a pie-slice is drawn.

bool COGLMVFilterBase::DrawArc(const COGLVertex& xP, const COGLVertex& xV1, 
							   const COGLVertex& xV2, float fRadius, 
							   bool bShort, bool bFilled, float fDegAngleStep)
{
	COGLVertex xA, xB, xN;
	MultiV<float> vR, vrR, vA, vB, vU, vP;
	MemObj<MultiV<float> > mvList;
	Mem<int> mIdx;
	float fAngleStep, fAngle;
	int i, iAngleSteps;
	COGLColor Dark;

	xA = xV1 - xP;
	xB = xV2 - xP;
	xA.Norm();
	xB.Norm();
	vP = m_E3Base.Vec(xP[0], xP[1], xP[2]);
	vA = m_E3Base.Vec(xA[0], xA[1], xA[2]);
	vB = m_E3Base.Vec(xB[0], xB[1], xB[2]);

	if (vA == m_E3Base.vZero ||
		vB == m_E3Base.vZero ||
		vA == vB)
	{
		// Do not draw anything
		return true;
	}

	fAngle = float(acos(Scalar(vA * vB)));
	if (!bShort)
		fAngle = 2.0f*m_fPi - fAngle;

	fAngleStep = fDegAngleStep * m_fPi / 180.0f;
	Dark = m_fBackFaceColFac * m_ActiveColor;

	if (!(iAngleSteps = int(fAngle / fAngleStep + 0.5)))
		iAngleSteps = 1;

	fAngleStep = fAngle / float(iAngleSteps);

	if (bShort)
		vU = vA ^ vB;
	else
		vU = vB ^ vA;

	vR = m_E3Base.Rotor(vU, fAngleStep, false);
	vrR = ~vR;

	// Get Normal to rotation plane
	vU = *vU;
	xN = &vU[1];
	xN.Norm();

	if (m_bUseLighting)
	{
		m_mMaterial[MAT_ROTOR].Ambient(m_ActiveColor);
		m_mMaterial[MAT_ROTOR].Diffuse(1.0, 1.0, 1.0, m_ActiveColor.Alpha());
		m_mMaterial[MAT_ROTOR].Apply();
	}

	m_VexList.Reset();

	float fTexMaxR = 0.5f;
	float fMaxR = 0.5f;
	float fTexAngle;

	if (bFilled)
	{
		m_VexList.SetMode(GL_TRIANGLE_FAN);
		m_VexList.AddVex(xP);

		if (m_bUseAbsTexCoords)
		{
			fMaxR = fRadius;
		}

		if (m_fTexRepFac != 0.0f)
		{
			fTexMaxR = fMaxR / m_fTexRepFac;
		}

		m_VexList.AddTex(0.5f,0.5f,0);
		m_VexList.AddTex(0.5f + fTexMaxR, 0.5f,0);
	}
	else
	{
		m_VexList.SetMode(GL_LINE_STRIP);
	}

	if (m_bUseLighting)
	{
		//m_mMaterial[MAT_ROTOR].Diffuse(1.0, 1.0, 1.0, 0.8);
		//m_mMaterial[MAT_ROTOR].Apply();

		m_VexList.AddNormal(xN);
		
		if (bFilled)
			m_VexList.AddNormal(xN);
	}

	vA &= fRadius;
	xA = &vA[1];
	m_VexList.AddVex(xP + xA);

	mIdx.Set(iAngleSteps + 2);
	mIdx[0] = 0;
	mIdx[1] = iAngleSteps + 1;

	for(i=0,fTexAngle=0;i<iAngleSteps;i++,fTexAngle += fAngleStep)
	{
		vA = (vR & vA) & vrR;

		xA = &vA[1];
		m_VexList.AddVex(xP + xA);

		if (bFilled)
			m_VexList.AddTex(0.5f + fTexMaxR*cos(fTexAngle), 0.5f - fTexMaxR*sin(fTexAngle), 0);

		if (m_bUseLighting)
			m_VexList.AddNormal(xN);

		mIdx[iAngleSteps - i + 1] = i + 1;
	}

	if (bFilled)
	{
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);
	}

	m_VexList.Apply();

	if (bFilled)
	{
		Dark.Apply();
		m_VexList.SetIdxList(mIdx);
		m_VexList.InvertNormals();
		m_VexList.Apply();

		glDisable(GL_POLYGON_OFFSET_FILL);
	}

	m_ActiveColor.Apply();

	return true;
}



//////////////////////////////////////////////////////////////////////
/// Draw List of Blades

bool COGLMVFilterBase::DrawListOfMV(MemObj<MultiV<float> >& mvA)
{
	int i, n = mvA.Count();

	for(i=0;i<n;i++)
	{
		DrawMV(mvA[i]);
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Draw List of Blades

bool COGLMVFilterBase::DrawListOfMV(MemObj<MultiV<double> >& mvA)
{
	int i, n = mvA.Count();

	for(i=0;i<n;i++)
	{
		DrawMV(mvA[i]);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Cast MV Info from double to float

bool COGLMVFilterBase::CastMVInfoToFloat(CMVInfo<float>& fMVInfo, CMVInfo<double>& dMVInfo)
{
	fMVInfo.m_eType = dMVInfo.m_eType;

	Mem<CPointND<float, 3> >& mfVex = fMVInfo.m_mVex;
	Mem<CPointND<double, 3> >& mdVex = dMVInfo.m_mVex;

	int i, n = mdVex.Count();
	
	if (!mfVex.Set(n))
		return false;

	for(i=0;i<n;i++)
	{
		mfVex[i][0] = (float) mdVex[i][0];
		mfVex[i][1] = (float) mdVex[i][1];
		mfVex[i][2] = (float) mdVex[i][2];
	}

	Mem<float>& mfVal = fMVInfo.m_mVal;
	Mem<double>& mdVal = dMVInfo.m_mVal;

	n = mdVal.Count();

	if (!mfVal.Set(n))
		return false;

	for(i=0;i<n;i++)
	{
		mfVal[i] = (float) mdVal[i];
	}

	return true;
}

