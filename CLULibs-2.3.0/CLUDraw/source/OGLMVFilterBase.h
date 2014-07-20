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

// OGLFilterBase.h: Schnittstelle für die Klasse COGLFilterBase.
//
// Base Class for OpenGL filters. Used to display e.g. Multivectors
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLMVFILTERBASE_H__3D0F9FE0_C4C7_4C2C_9881_815CE941C428__INCLUDED_)
#define AFX_OGLMVFILTERBASE_H__3D0F9FE0_C4C7_4C2C_9881_815CE941C428__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OGLColor.h"
#include "OGLMaterial.h"
#include "OGLVertex.h"
#include "OGLVertexList.h"
#include "OGLDisplayList.h"

#define MODE_OPNS	(ID_ALL << 16) + 0x0001
#define MODE_IPNS	(ID_ALL << 16) + 0x0002
#define DRAW_POINT_AS_DOT		(ID_ALL << 16) + 0x0100
#define DRAW_POINT_AS_SPHERE	(ID_ALL << 16) + 0x0101
#define DRAW_POINT_AS_STD		DRAW_POINT_AS_DOT


#define MAT_STD		0
#define MAT_PLANE	1
#define MAT_CUBE	2
#define MAT_SPHERE	3
#define MAT_ROTOR	4
#define MAT_CIRCLE	5
#define MAT_LINE	6
#define MAT_POINT	7
#define MAT_VECTOR	8
#define MAT_RHOMBUS	9
#define MAT_COUNT	10

class COGLMVFilterBase  
{
public:
	enum TDrawPointType
	{
		PntStd = 0, PntDot = 0, PntSphere
	};

	struct STriplet
	{
	public:
		int& operator[] (int iPos) { return m_pData[iPos]; }
		
		int* Data() { return m_pData; }

	protected:
		int m_pData[3];
	};

	typedef int TTriplet[3];

public:
	COGLMVFilterBase();
	virtual ~COGLMVFilterBase();

	COGLMVFilterBase& operator<< (int iMode)
	{ SetMode(iMode); return *this; }

	COGLMVFilterBase& operator<< (COGLColor& rCol)
	{ SetColor(rCol); return *this; }

	COGLMVFilterBase& operator<< (const MultiV<double>& vA)
	{ DrawMV(vA); return *this; }

	COGLMVFilterBase& operator<< (const MultiV<float>& vA)
	{ DrawMV(vA); return *this; }

	COGLMVFilterBase& operator<< (MemObj<MultiV<double> >& mvA)
	{ DrawListOfMV(mvA); return *this; }

	COGLMVFilterBase& operator<< (MemObj<MultiV<float> >& mvA)
	{ DrawListOfMV(mvA); return *this; }

	virtual bool DrawMV(const MultiV<double>& vA, bool bAnalyzeOnly = false) { return true; }
	virtual bool DrawMV(const MultiV<float>& vA, bool bAnalyzeOnly = false) { return true; }
	
	virtual bool DrawListOfMV(MemObj<MultiV<double> >& mvA);
	virtual bool DrawListOfMV(MemObj<MultiV<float> >& mvA);

	virtual bool SetMode(int iMode);
	virtual bool ResetModes();
	virtual bool SetColor(COGLColor& rCol);

	virtual bool Init();
	virtual void EnableLineStipple(bool bSet = true);

	virtual bool DrawVertexList(COGLVertexList& VexList);

	virtual bool DrawVector(const COGLVertex& xA, const COGLVertex& xB, 
							int iStipple = 0);

	virtual bool DrawLine(const COGLVertex& xP, const COGLVertex& xD, 
						  bool bEndPoints = false, int iStipple = 0);

	virtual bool DrawCube(float fSize, const float *pX = 0, const float *pY = 0, const float *pZ = 0);
	virtual bool DrawRhombus(float fSize);
	virtual bool DrawSphere(const COGLVertex& xC, float fRadius, bool bSolid = false);
	virtual bool DrawEllipsoid(const COGLVertex& xC, 
								 const COGLVertex& xX, const COGLVertex& xY, 
								 const COGLVertex& xZ, bool bSolid = false);

	virtual bool DrawPlane(const COGLVertex& xP, const COGLVertex& xA, 
						   const COGLVertex& xB, bool bCornerVecs = false, 
						   bool bDirected = true);

	virtual bool DrawBox(const COGLVertex& xP, const COGLVertex& xA, 
								 const COGLVertex& xB, float fWidth, 
								 bool bCornerVecs, bool bDirected);

	virtual bool DrawBox(const COGLVertex& xP, const COGLVertex& xA, 
								 const COGLVertex& xB, const COGLVertex& xC,
								 const MemObj<COGLColor>& rColList);

	virtual bool DrawTriangle(const COGLVertex& xA, const COGLVertex& xB, 
							  const COGLVertex& xC, bool bDirected = true);

	virtual bool DrawCone(const COGLVertex& xP, const COGLVertex& xD, float fR);

	virtual bool DrawCircle(const COGLVertex& xC, const COGLVertex& xN, 
		float fR, float fAngleStep = 5.0f);


	virtual bool DrawCylinder(const COGLVertex& xC, 
								const COGLVertex& xN, 
								float fR, 
								float fAngleStep = 5.0f,
								COGLVertexList *_pVexList = 0,
								bool bDoDraw = true);

	virtual bool DrawCircleSurface(const MemObj<COGLVertex>& mC, 
									 const MemObj<COGLVertex>& mN, 
									 const Mem<float>& mR, 
									 const MemObj<COGLColor>& mCol,
									 float fAngleStep = 5.0f,
									 COGLVertexList *_pVexList = 0,
									 bool bDoDraw = true);

	virtual bool DrawLineSurface(const MemObj<COGLVertex>& mC, 
									 const MemObj<COGLVertex>& mD, 
									 const Mem<float>& mL, 
									 const MemObj<COGLColor>& mCol,
									 int iSteps = 20,
									 COGLVertexList *_pVexList = 0,
									 bool bDoDraw = true);

	virtual bool DrawEllipse(const COGLVertex& xC,
								   const COGLVertex& xA, const COGLVertex& xB, 
								  float fAngleStep = 5.0f);

	virtual bool DrawHyperbola(const COGLVertex& xC,
								   const COGLVertex& xA, const COGLVertex& xB, 
								  float fAngleStep = 5.0f);

	virtual bool DrawPointSurface(int iRowCount, int iColCount,
								  const MemObj<COGLVertex>& mP, 
								  const MemObj<COGLVertex>& mN, 
								  const MemObj<COGLVertex>& mT, 
								  const MemObj<COGLColor>& mCol,
								  bool bNegateNormals = false,
								  float fNormScale = float(0),
								  MemObj<COGLVertexList*> *_pmVexList = 0,
								  bool bDoDraw = true);

	virtual bool DrawPointGrid(int iRowCount, int iColCount,
								  const MemObj<COGLVertex>& mP, 
								  const MemObj<COGLVertex>& mN, 
								  const MemObj<COGLColor>& mCol,
								  bool bNegateNormals = false,
								  COGLVertexList *_pVexList = 0,
								  bool bDoDraw = true);

	virtual bool DrawPointList(    const MemObj<COGLVertex>& mP, 
										 const MemObj<COGLVertex>& mN, 
										 const MemObj<COGLColor>& mCol,
										 bool bNegateNormals = false,
										 COGLVertexList *_pVexList = 0,
										 bool bDoDraw = true);

	virtual bool DrawDisk(const COGLVertex& xC, const COGLVertex& xA, const COGLVertex& xB, 
		float fR, float fAngleStep = 5.0f);

	virtual bool DrawRotor(const COGLVertex& xP, const COGLVertex& xR, 
		float fAngle, float fDegAngleStep = 5.0f);

	virtual bool DrawTwist(const COGLVertex& xP, const COGLVertex& xT, const COGLVertex& xR, 
							 	 float fAngle, float fDegAngleStep = 5.0f);

	// Draws an arc.
	// xP is offset vector. Arc is drawn from (xA-xP) to (xB-xP) with radius 'fRadius'.
	// if bFilled = true then a pie-slice is drawn.
	// if bShort = true, then the arc along the shortest path from xA to xB is drawn.
	// otherwise the arc along the longest path. 
	virtual bool DrawArc(const COGLVertex& xP, const COGLVertex& xA, const COGLVertex& xB, 
		float fRadius, bool bShort = true, bool bFilled = false, float fDegAngleStep = 5.0f);

	virtual bool DrawPoint(const COGLVertex& xP);

	// Draws an Icosahedron at pos. xP, with radius fRadius and 2^iPower subdivisions.
	// This approximates a sphere for higher powers
	virtual bool DrawIcosahedron(const COGLVertex& xP, float fRadius, int iPower, bool bSolid = true);

	void SetSensitivity(double dSens) { m_fSensitivity = (float) dSens; m_dSensitivity = dSens;}
	float GetFloatSens() { return m_fSensitivity; }
	double GetDoubleSens() { return m_dSensitivity; }

	float Round(float fVal) 
	{ return ::Round(fVal, m_fSensitivity); }

	double Round(double dVal) 
	{ return ::Round(dVal, m_dSensitivity); }

	CMVInfo<float> GetMVInfo() { return m_MVInfo; }
	void GetMVInfo(CMVInfo<float> &rInfo) { rInfo = m_MVInfo; }
	void GetMVInfo(CMVInfo<double> &rInfo) { rInfo = m_dMVInfo; }

	void EnableLighting(bool bLight = true) { m_bUseLighting = bLight; }
	void EnableTranslucency(bool bVal = true) { m_bUseTranslucency = bVal; }
	//void EnableTextures(bool bVal = true) { m_bUseTextures = bVal; }

	void SetTranslucencySeed(int iSeed) { m_Random.seed3(iSeed); }

	void SetTextureRepeatFactor(float fVal) { m_fTexRepFac = fVal; }
	void EnableAbsTexCoords(bool bVal = true) { m_bUseAbsTexCoords = bVal; }

	void SetArrowShape(float fSize, float fAngle) { m_fArrowLength = fSize; m_fArrowAngle = fAngle; }

	bool GenVexIcosahedron(COGLVertexList &rVexList, float fRadius, int iPower);

protected:
	//bool GenerateSphereVL(double dAngleStepTheta, double dAngleStepPhi);
	bool MakeRandPolyStipple(double dLevel);

	bool CastMVInfoToFloat(CMVInfo<float>& fMVInfo, CMVInfo<double>& dMVInfo);
	bool InitSphereDisplayLists();

protected:
	E3GA<float> m_E3Base;
	PGA<float> m_P3Base;
	ConfGA<float> m_N3Base;
	El2GA<float> m_El2Base;

	E3GA<double> m_dE3Base;
	PGA<double> m_dP3Base;
	ConfGA<double> m_dN3Base;
	El2GA<double> m_dEl2Base;

	COGLColor m_ActiveColor;
	COGLVertexList m_VexList;
	COGLVertexList m_NormVexList;

	MemObj<COGLMaterial> m_mMaterial;

	CMVInfo<float> m_MVInfo;
	CMVInfo<double> m_dMVInfo;

	COGLDisplayList m_dlSolidSphere1, m_dlSolidSphere2;
	COGLDisplayList m_dlSolidSphereI1, m_dlSolidSphereI2;
	COGLDisplayList m_dlWireSphere1, m_dlWireSphere2;

	float m_fPi;
	float m_fBackFaceColFac;
	float m_fArrowColFac;
	float m_fLineWidth;
	float m_fPointSize;
	float m_fPointAsSphereFac;
	float m_fTexRepFac;	// Texture Repeat Factor
	// Arrow Shape: Length of arrow and opening angle
	float m_fArrowLength, m_fArrowAngle; // Arrow Shape

	float m_fSensitivity; // Used as TinyToZero parameter
	double m_dSensitivity;

	Rand m_Random;
	unsigned char m_pcPolyStipple[1024]; // Polygon Stipple pattern

	bool m_bUseLighting;
	bool m_bUseTranslucency;
	bool m_bShowImaginaryObjects;
	bool m_bUseAbsTexCoords;
	//bool m_bUseTextures;

	TDrawPointType m_eDrawPointType;

};

#endif // !defined(AFX_OGLMVFILTERBASE_H__3D0F9FE0_C4C7_4C2C_9881_815CE941C428__INCLUDED_)
