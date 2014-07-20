/// LICENSE START
////////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the CLUParse Library.
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

// CLUCodeBase.cpp: Implementierung der Klasse CCLUCodeBase.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "GL/glut.h"
#include <FL/fl_ask.H>
#include <FL/Fl_File_Chooser.H>
#include <time.h>
#include <sys/timeb.h>

#ifdef _GNUCPP3_
#   include <unistd.h>
#   define _getcwd getcwd
#   define _chdir chdir
#else
#   include <direct.h>
#endif

#ifdef WIN32
#	include <ShellApi.h>
#endif

#include "CLUCodeBase.h"

#include <vector>
#include <algorithm>
#include <functional>      // For greater<int>( )

using namespace std;


CCLUCodeBase *CCLUCodeBase::sm_pCurCodeBase = 0;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCLUCodeBase::CCLUCodeBase()
{
	m_pFilter = 0;
	m_pFrameRotor = 0;
	m_pCLUDrawBase = 0;
	
	m_fPi = TCVScalar(2.0 * asin(1.0));
	m_fRadPerDeg = m_fPi / 180.0f;

	m_iCurScript = -1;

	m_iFrameStackDepth = 0;
	m_bOverrideSetRenderLatex = false;
	
	m_fLineWidth = 1;
	m_fPointSize = 1;
	m_bSmoothPoint = false;
	m_bSmoothLine = false;
	m_bSmoothPolygon = false;

	m_dCurrentInfoWidth = 0;

	m_eCurSpaceVars = SPACEVARS_NONE;

	m_fLineWidth = 2.0f;

	time_t TimeVal;
	time(&TimeVal);
	m_Random.seed3(TimeVal);

	m_vecVersion.resize(3);
	m_vecVersion[0] = 0;
	m_vecVersion[1] = 0;
	m_vecVersion[2] = 0;

	SFontData Font;

	Font.sName = "Times";
	Font.iFont = FL_TIMES;
	Font.vecSize.push_back(12);
	Font.vecSize.push_back(18);
	Font.vecSize.push_back(24);
	Font.vecSize.push_back(30);
	Font.vecSize.push_back(36);
	Font.vecSize.push_back(48);
	Font.vecSize.push_back(96);
	m_vecFont.push_back(Font);

	m_iActTempImageList = 0;
	m_mTempImageList.Set(2);

	sm_pCurCodeBase = this;
	m_pSortVarList = 0;

	Reset();
}

CCLUCodeBase::~CCLUCodeBase()
{
	
}


void CCLUCodeBase::Reset()
{
	m_mapBitmap.clear();
	CCodeVar::m_ImgRep.Reset();

	m_iPlotMode = GL_LINE_STRIP;

	// Delete all latex bitmaps
	m_oglLatex.ResetInUseFlags();
	m_oglLatex.PruneMap();

	// Reset Gauss Parameters
	m_Random.GaussMean(0);
	m_Random.GaussDev(1);

	// Delete Variables set for a space
	ResetSpaceVars();

	// Reset Variables for Info Window
	ResetInfoVars();
	
	// Reset environment variables
	ResetEnvVars();

	// Reset Serial IO Ports
	ResetSerialIO();

	// Reset Animation Time Step
	if (m_pCLUDrawBase)
		m_pCLUDrawBase->SetAnimationTimeStep(20);

	GETTIME(&m_tmStart);
}

void CCLUCodeBase::ResetSerialIO()
{
#ifdef WIN32
	m_mapSerialIO.clear();
#endif
}


void CCLUCodeBase::ResetEnvVars()
{
	m_iCurBitmapFont = 0; //FL_TIMES; //OGL_BMPFONT_Times_Std_12;
	m_iCurBitmapFontSize = 0; //24;

	// Current Bitmap Scale
	m_fCurBitmapScale = 1.0f;

	// Reset Latex Mag Step
	m_iLatexMagStep = 6;

	// Align Latex Bitmaps at bottom
	m_fVLatexAlign = 0.0f;
	m_fHLatexAlign = 0.0f;

	// Align Bitmaps at bottom
	m_fVBMPAlign = 0.0f;
	m_fHBMPAlign = 0.0f;

	// Position of Bitmaps
	m_xBMPPos.Set(0.0f, 0.0f, 0.0f);

	// Switch off transparency in Bitmap
	m_bUseBMPTrans = false;

	m_bOverlayModeActive = false;

	// Text precission
	m_iTextPrec = 3;

	// Evaluation precision
	m_fSensitivity = 1e-12;

	// Visualization precision
	if (m_pFilter)
		m_pFilter->SetSensitivity(1e-12);

	// Switch off Latex rendering.
	// The user can easily override this with shift+ctrl+p.
	SetRenderLatex(false);

	// Set Line Width
	SetLineWidth(2.0f);

	// Disable smooth lines
	SetSmoothLine(false);

	// Set Point Size
	SetPointSize(5.0f);

	// Disable smooth points
	SetSmoothPoint(false);

	// Disable smooth polygons
	SetSmoothPolygon(false);

	// Reset Texture
	glBindTexture(GL_TEXTURE_2D, 0);
	if (m_pFilter)
	{
		m_pFilter->EnableAbsTexCoords(false);
		m_pFilter->SetTextureRepeatFactor(0.0f);
	}

	// Reset Arrow Shape
	if (m_pFilter)
		m_pFilter->SetArrowShape(0.15f, 15.0f / 180.0f * float(m_fPi) );


	// Reset Fog Settings
	float pfFogCol[] = {1.0f, 1.0f, 1.0f, 0.0f };

	glDisable(GL_FOG);
	glFogi(GL_FOG_MODE, GL_EXP2);
	glFogfv(GL_FOG_COLOR, pfFogCol);
	glFogf(GL_FOG_DENSITY, 1.0f);

}



void CCLUCodeBase::ResetInfoVars()
{
	m_dCurrentInfoWidth = 0;
}


void CCLUCodeBase::ResetOutputObjectList()
{
	m_vecOutputObject.resize(0);
}


void CCLUCodeBase::CollectGarbage()
{
	// Delete all latex bitmaps that were not used the last time round.
	m_oglLatex.PruneMap();

	// Mark all latex bitmaps as not being used currently.
	m_oglLatex.ResetInUseFlags();
}

void CCLUCodeBase::CleanUp()
{
	if (m_bOverlayModeActive)
	{
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}
	
	int iViewStackDepth = m_vecViewStack.size();
	if (iViewStackDepth > 0)
	{
		SViewData NewView = m_vecViewStack[0];
		
		m_pCLUDrawBase->SetViewport(NewView.iX, NewView.iY, NewView.iW, NewView.iH);
		m_vecViewStack.resize(0);

		for( int i = 0; i < iViewStackDepth; i++ )
		{
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
		}
	}

	while (m_iFrameStackDepth > 0)
	{
		glPopMatrix();
		m_iFrameStackDepth--;
	}
}


//////////////////////////////////////////////////////////////////////
/// Set CLUCalc Version

void CCLUCodeBase::SetVersion(int iMajor, int iMinor, int iRevision)
{
	m_vecVersion[0] = iMajor;
	m_vecVersion[1] = iMinor;
	m_vecVersion[2] = iRevision;
}


//////////////////////////////////////////////////////////////////////
/// Get CLUCalc Version

void CCLUCodeBase::GetVersion(int& iMajor, int& iMinor, int& iRevision)
{
	iMajor = m_vecVersion[0];
	iMinor = m_vecVersion[1];
	iRevision = m_vecVersion[2];
}


//////////////////////////////////////////////////////////////////////
// Check that scalar is in particular range, taking sensitivity into account.

bool CCLUCodeBase::IsInRange(TCVScalar& dVal, TCVScalar dMin, TCVScalar dMax, int iLine, int iPos)
{
	if (dVal < dMin && dVal >= dMin - m_fSensitivity)
	{
		dVal = dMin;
	}
	else if (dVal < dMin)
	{
		m_ErrorList.OutOfRange(dVal, dMin, dMax, iLine, iPos);
		return false;
	}

	if (dVal > dMax && dVal <= dMax + m_fSensitivity)
	{
		dVal = dMax;
	}
	else if (dVal > dMax)
	{
		m_ErrorList.OutOfRange(dVal, dMin, dMax, iLine, iPos);
		return false;
	}

	return true;
}






void CCLUCodeBase::SetLineWidth(float fWidth)
{
	if (fWidth >= 1.0f)
	{
		glLineWidth(fWidth);
		m_fLineWidth = fWidth;
	}
}


void CCLUCodeBase::SetPointSize(float fWidth)
{
	if (fWidth >= 1)
	{
		glPointSize(fWidth);
		m_fPointSize = fWidth;
	}
}


void CCLUCodeBase::SetSmoothPoint(bool bVal)
{
	if (bVal)
	{
		glEnable(GL_POINT_SMOOTH);
		m_bSmoothPoint = true;
	}
	else
	{
		glDisable(GL_POINT_SMOOTH);
		m_bSmoothPoint = false;
	}
}

void CCLUCodeBase::SetSmoothLine(bool bVal)
{
	if (bVal)
	{
		glEnable(GL_LINE_SMOOTH);
		m_bSmoothLine = true;
	}
	else
	{
		glDisable(GL_LINE_SMOOTH);
		m_bSmoothLine = false;
	}
}


void CCLUCodeBase::SetSmoothPolygon(bool bVal)
{
	if (bVal)
	{
		glEnable(GL_POLYGON_SMOOTH);
		m_bSmoothPolygon = true;
	}
	else
	{
		glDisable(GL_POLYGON_SMOOTH);
		m_bSmoothPolygon = false;
	}
}

//////////////////////////////////////////////////////////////////////
// Set Animation Time Step FUNCTION
//

bool CCLUCodeBase::SetAnimationTimeStepFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList &mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	TCVCounter iTimeStep;

	if (iVarCount != 1)
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToCounter(iTimeStep))
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (iTimeStep <= 0)
	{
		m_ErrorList.GeneralError("Timestep value has to be greater than zero.", iLine, iPos);
		return false;
	}

	m_pCLUDrawBase->SetAnimationTimeStep(iTimeStep);
	
	return true;
}

//////////////////////////////////////////////////////////////////////
// Enable Animation FUNCTION
//

bool CCLUCodeBase::EnableAnimationFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList &mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	TCVCounter iVal;

	if (iVarCount != 1)
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToCounter(iVal))
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	CCodeVar &rAVar = GetVar("_DoAnimate");
	if (rAVar.BaseType() == PDT_NOTYPE)
	{
		CCodeVar &rNewVar = NewVar("_DoAnimate", PDT_INT);
		rNewVar = iVal;
	}
	else
	{
		rAVar = iVal;
	}
	
	return true;
}

bool CCLUCodeBase::GetTimeFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList &mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();

	if (iVarCount != 0)
	{
		m_ErrorList.WrongNoOfParams(0, iLine, iPos);
		return false;
	}

	TTimeStruct tmCur;
	GETTIME(&tmCur);
	TCVScalar dTime;

	dTime = TCVScalar(tmCur.time - m_tmStart.time) 
					+ TCVScalar(1e-3) * TCVScalar(tmCur.millitm - m_tmStart.millitm);
	rVar = dTime;

	return true;
}


//////////////////////////////////////////////////////////////////////
// Set Evaluation Precision FUNCTION
//

bool CCLUCodeBase::SetEvalPrecFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList &mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	TCVCounter iPow;

	if (iVarCount != 1)
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToCounter(iPow))
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (iPow < 0 || iPow > 12)
	{
		m_ErrorList.GeneralError("Value for precision has to be in the range {0,...,12}", iLine, iPos);
		return false;
	}

	m_fSensitivity = TCVScalar(pow(0.1, double(iPow)));
	
	return true;
}


// Set Visualization Precision FUNCTION
//

bool CCLUCodeBase::SetVisPrecFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList &mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	double dPrec;
	TCVCounter iPow;

	if (iVarCount != 1)
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToCounter(iPow))
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (iPow < 0 || iPow > 12)
	{
		m_ErrorList.GeneralError("Value for precision has to be in the range {0,...,12}", iLine, iPos);
		return false;
	}

	dPrec = pow(0.1, double(iPow));
	m_pFilter->SetSensitivity(dPrec);
	
	return true;
}



//////////////////////////////////////////////////////////////////////
/// Set Mouse Mode Function

bool CCLUCodeBase::SetMouseModeFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList &mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	TCVCounter iMode;

	if (iVarCount != 1)
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToCounter(iMode))
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (iMode < 0 || iMode > 9)
	{
		m_ErrorList.GeneralError("Value for mouse mode has to be in the range {0,...,9}", iLine, iPos);
		return false;
	}

	m_pCLUDrawBase->SetMouseMode(iMode);

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Get Mouse Mode Function

bool CCLUCodeBase::GetMouseModeFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList &mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	TCVCounter iMode;

	if (iVarCount != 0)
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	m_pCLUDrawBase->GetMouseMode(iMode);
	rVar = iMode;

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Set Mouse Sensitivity Function

bool CCLUCodeBase::SetMouseSensFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList &mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	TCVScalar dRot, dTrans;

	if (iVarCount != 2)
	{
		m_ErrorList.WrongNoOfParams(2, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToScalar(dRot))
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToScalar(dTrans))
	{
		m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}

	m_pCLUDrawBase->SetRotFac(float(dRot));
	m_pCLUDrawBase->SetTransFac(float(dTrans));

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Get Mouse Sensitivity Function

bool CCLUCodeBase::GetMouseSensFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList &mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	TCVScalar dRot, dTrans;

	if (iVarCount != 0)
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	dRot = (float) m_pCLUDrawBase->GetRotFac();
	dTrans = (float) m_pCLUDrawBase->GetTransFac();

	rVar.New(PDT_VARLIST, "Constant");
	TVarList &rList = *rVar.GetVarListPtr();

	rList.Set(2);
	rList[0] = dRot;
	rList[1] = dTrans;

	return true;
}




//////////////////////////////////////////////////////////////////////
/// Set OpenGL Window Size Function

bool CCLUCodeBase::SetOGLSizeFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList &mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	TCVScalar dWidth, dHeight;

	if (iVarCount != 2)
	{
		m_ErrorList.WrongNoOfParams(2, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToScalar(dWidth))
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToScalar(dHeight))
	{
		m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}

	m_pCLUDrawBase->SetOGLSize(dWidth, dHeight);

	return true;
}





//////////////////////////////////////////////////////////////////////
/// Cast To Float
bool CCLUCodeBase::CastToFloat(CCodeVar& rVar, float &rfVal)
{
	
	switch(rVar.BaseType())
	{
	case PDT_INT:
		rfVal = ((float) *rVar.GetIntPtr());
		break;
		
	case PDT_UINT:
		rfVal = ((float) *rVar.GetUIntPtr());
		break;
		
	case PDT_LONG:
		rfVal = (float) *rVar.GetLongPtr();
		break;
		
	case PDT_FLOAT:
		rfVal = (float) *rVar.GetFloatPtr();
		break;
		
	case PDT_DOUBLE:
		rfVal = (float) *rVar.GetDoublePtr();
		break;
		
	default:
		return false;
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Cast To Double

bool CCLUCodeBase::CastToDouble(CCodeVar& rVar, double &rdVal)
{
	
	switch(rVar.BaseType())
	{
	case PDT_INT:
		rdVal = ((double) *rVar.GetIntPtr());
		break;
		
	case PDT_UINT:
		rdVal = ((double) *rVar.GetUIntPtr());
		break;
		
	case PDT_LONG:
		rdVal = ((double) *rVar.GetLongPtr());
		break;
		
	case PDT_FLOAT:
		rdVal = ((double) *rVar.GetFloatPtr());
		break;
		
	case PDT_DOUBLE:
		rdVal = ((double) *rVar.GetDoublePtr());
		break;
		
	default:
		return false;
	}
	
	return true;
}




//////////////////////////////////////////////////////////////////////
/// Set Mouse Value
///
/// Parameters:
///		1. Mouse Mode
///		2. Mouse button, 1: left, 2:right
///		3. List or Vector of three entries.
///
/// Return:
///		Nothing.
///
bool CCLUCodeBase::SetMouseValueFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList &mVars = *rPars.GetVarListPtr();
	int i, iVarCount = mVars.Count();
	TCVCounter iMode, iBut;
	TCVScalar pdVal[3];

	if (iVarCount != 3)
	{
		m_ErrorList.WrongNoOfParams(3, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToCounter(iMode))
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToCounter(iBut))
	{
		m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}

	if (iMode < 1 || iMode > 9)
	{
		m_ErrorList.GeneralError("Mouse mode has to be in range {1,...,9}.", iLine, iPos);
		return false;
	}
	
	if (iBut < 1 || iBut > 2)
	{
		m_ErrorList.GeneralError("Button ID has to be either 1 (left) or 2 (right).", iLine, iPos);
		return false;
	}

	if (mVars(2).BaseType() == PDT_VARLIST)
	{
		TVarList &rList = *mVars(2).GetVarListPtr();
		
		if (rList.Count() != 3)
		{
			m_ErrorList.GeneralError("Variable list has to contain three values.", iLine, iPos);
			return false;
		}
		
		for(i = 0; i < 3; i++)
		{
			CCodeVar& rVar = rList(i);
			
			if (!rVar.CastToScalar(pdVal[i]))
			{
				m_ErrorList.GeneralError("Value in mouse value list has to be a scalar.", iLine, iPos);
				return false;
			}
		}
	}
	else if (mVars(2).BaseType() == PDT_MULTIV)
	{
		TMultiV vB;
		TMultiV &vA = *mVars(2).GetMultiVPtr();

		if (!CastMVtoE3(vA, vB))
		{
			m_ErrorList.GeneralError("Cannot extract values from multivector.", iLine, iPos);
			return false;
		}

		for(i = 0; i < 3; i++)
			pdVal[i] = vB[i+1];
	}
	else
	{
		m_ErrorList.GeneralError("Mouse value has to be given by a list or a multivector.", iLine, iPos);
		return false;
	}

	
	if (iBut == 1)
	{
		for( i = 0; i < 3; i++ )
			(*m_pTransform)[iMode].pfRot[i] = GLfloat(pdVal[i] / m_fRadPerDeg);
	}
	else
	{
		for( i = 0; i < 3; i++ )
			(*m_pTransform)[iMode].pfTrans[i] = GLfloat(pdVal[i]);
	}


	return true;
}


//////////////////////////////////////////////////////////////////////
/// Get Mouse Value

bool CCLUCodeBase::GetMouseValueFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList &mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();

	int i;
	TCVCounter iMode, iBut;

	if (iVarCount != 2)
	{
		m_ErrorList.WrongNoOfParams(2, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToCounter(iMode))
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToCounter(iBut))
	{
		m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}

	if (iMode < 1 || iMode > 9)
	{
		m_ErrorList.GeneralError("Mouse mode has to be in range {1,...,9}.", iLine, iPos);
		return false;
	}
	
	if (iBut < 1 || iBut > 2)
	{
		m_ErrorList.GeneralError("Button ID has to be either 1 (left) or 2 (right).", iLine, iPos);
		return false;
	}

	rVar.New(PDT_VARLIST, "Constant");
	TVarList &rList = *rVar.GetVarListPtr();

	rList.Set(3);

	if (iBut == 1)
	{
		for( i = 0; i < 3; i++ )
			rList[i] = TCVScalar( (*m_pTransform)[iMode].pfRot[i] * m_fRadPerDeg );
	}
	else
	{
		for( i = 0; i < 3; i++ )
			rList[i] = TCVScalar( (*m_pTransform)[iMode].pfTrans[i] );
	}

	return true;
}



//////////////////////////////////////////////////////////////////////
// Get Mouse Value FUNCTION
//

bool CCLUCodeBase::MouseFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList *pmVars = rPars.GetVarListPtr();
	int iVarCount = pmVars->Count();
	
	if (iVarCount == 3)
	{
		int piVal[3];
		int i;
		
		for(i=0;i<3;i++)
		{
			CCodeVar& rVar = (*pmVars)(i);
			
			if (!rVar.CastToCounter(piVal[i]))
			{
				m_ErrorList.InvalidParType(rVar, i+1, iLine, iPos);
				return false;
			}
		}
		
		if (!Mouse(rVar, piVal, iLine, iPos))
			return false;
	}
	else
	{
		m_ErrorList.WrongNoOfParams(3, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
// Get Mouse Value
//
// piVal[0] : Key No.
// piVal[1] : Mouse button 1:left, 2:right
// piVal[2] : Movement axis: 1, 2, 3.

bool CCLUCodeBase::Mouse(CCodeVar& rLVar, int *piVal, int iLine, int iPos)
{
	if (!piVal)
		return false;
	
	if (!rLVar.New(PDT_SCALAR, "Constant"))
	{
		m_ErrorList.Internal(iLine, iPos);
		return false;
	}
	
	cStr csStr;
	
	if (piVal[0] < 1 || piVal[0] > 9)
	{
		cStr csVal;
		csVal = piVal[0];
		m_ErrorList.InvalidParVal(csVal.Str(), 1, iLine, iPos);
		return false;
	}
	
	if (piVal[1] < 1 || piVal[1] > 2)
	{
		cStr csVal;
		csVal = piVal[1];
		m_ErrorList.InvalidParVal(csVal.Str(), 2, iLine, iPos);
		return false;
	}
	
	if (piVal[2] < 1 || piVal[2] > 3)
	{
		cStr csVal;
		csVal = piVal[2];
		m_ErrorList.InvalidParVal(csVal.Str(), 3, iLine, iPos);
		return false;
	}
	
	//TCVScalar fVal;
	
	if (piVal[1] == 1)
		rLVar = ((TCVScalar) (*m_pTransform)[piVal[0]].pfRot[piVal[2]-1]) * m_fRadPerDeg;
	else
		rLVar = ((TCVScalar) (*m_pTransform)[piVal[0]].pfTrans[piVal[2]-1]);
	
	return true;
}



//////////////////////////////////////////////////////////////////////
/// Set OGL Filter Mode FUNCTION

bool CCLUCodeBase::SetModeFunc(CCodeVar& rLVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!m_pFilter)
	{
		m_ErrorList.Internal(iLine, iPos);
		return false;
	}
	
	TVarList *pmVars = rPars.GetVarListPtr();
	int iVarCount = pmVars->Count();
	
	int iVal;
	int i;
	
	for(i=0;i<iVarCount;i++)
	{
		switch ((*pmVars)(i).BaseType())
		{
		case PDT_INT:
			iVal = (int) *( (int *) ((*pmVars)(i).GetIntPtr()) );
			break;
			
		case PDT_UINT:
			iVal = (int) *( (uint *) ((*pmVars)(i).GetUIntPtr()) );
			break;
			
		case PDT_LONG:
			iVal = (int) *( (long *) ((*pmVars)(i).GetLongPtr()) );
			break;
			
		default:
			m_ErrorList.InvalidType((*pmVars)(i), iLine, iPos);
			return false;
		}
		
		m_pFilter->SetMode(iVal);
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Plot Mode FUNCTION

bool CCLUCodeBase::SetPlotModeFunc(CCodeVar& rLVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList *pmVars = rPars.GetVarListPtr();
	int iVarCount = pmVars->Count();
	
	int iVal;
	int i;
	
	for(i=0;i<iVarCount;i++)
	{
		if (!(*pmVars)(i).CastToCounter(iVal))
		{
			m_ErrorList.InvalidType((*pmVars)(i), iLine, iPos);
			return false;
		}

		switch(iVal)
		{
		case PLOT_POINTS:
			m_iPlotMode = GL_POINTS;
			break;

		case PLOT_CONNECTED:
			m_iPlotMode = GL_LINE_STRIP;
			break;
		}
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// The Plot FUNCTION
/// first parameter is function, or list of two functions [ func, col_func ]
/// second parameter is list containing:
///		free variable, start, stop, steps.

bool CCLUCodeBase::PlotFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	TCVScalar fMin, fMax, fStep;
	TCVCounter iSteps, i;
	bool bError = false;
	bool bIsFuncList = false;
	bool bUseColList = false;
	bool bUseNormList = false;
	bool bUseTexList = false;
	int iListSize = 0;
	int iColListPos = -1;
	int iNormListPos = -1;
	int iTexListPos = -1;
	
	CCodeElementList *pCodeList = mVars.GetCodeList();

	if (!pCodeList)
	{
		m_ErrorList.Internal(iLine, iPos);
		return false;
	}

	if (iVarCount == 2)
	{
		if (mVars(0).BaseType() != PDT_MULTIV)
		{
			if (mVars(0).BaseType() != PDT_VARLIST)
			{
				m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
				return false;
			}
			iListSize = mVars(0).GetVarListPtr()->Count();
			if (iListSize > 1)
				bUseColList = true;
			bIsFuncList = true;
		}

		if (mVars(1).BaseType() != PDT_VARLIST)
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		// List containing free variable, plot range and step.
		TVarList& mFVList = *mVars(1).GetVarListPtr();

		if (mFVList.Count() != 4)
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		// Free variable to be used
		CCodeVar &FreeVar = GetVar(mFVList(0).Name());

		if (FreeVar.BaseType() == PDT_NOTYPE)
		{
			m_ErrorList.GeneralError("No free variable given.", iLine, iPos);	
			return false;
		}

		if (!mFVList(1).CastToScalar(fMin, m_fSensitivity))
		{
			m_ErrorList.GeneralError("Minimum value is not a scalar.", iLine, iPos);
			return false;
		}

		if (!mFVList(2).CastToScalar(fMax, m_fSensitivity))
		{
			m_ErrorList.GeneralError("Maximum value is not a scalar.", iLine, iPos);
			return false;
		}

		if (!mFVList(3).CastToCounter(iSteps))
		{
			m_ErrorList.GeneralError("The number of steps must be an integer value.", iLine, iPos);
			return false;
		}

		if (fMin >= fMax || iSteps < 1)
		{
			m_ErrorList.GeneralError("Minimum value has to be smaller than maximum value.", iLine, iPos);
			return false;
		}

		fStep = (fMax - fMin) / TCVScalar(iSteps);

		SCodeData sData;
		sData.Set(iLine, iPos, 0);
		
/*
		// Initialize result variable
		if (!rVar.New(PDT_VEXLIST, "Constant"))
		{
			m_ErrorList.OutOfMemory(iLine, iPos);
			return false;
		}
		
		TVexList& rVexList = *rVar.GetVexListPtr();
		rVexList.SetMode((GLenum)m_iPlotMode);
*/
		// Initialize Point List
		CCodeVar Par;
		Par.New(PDT_VARLIST, "Constant");

		TVarList &rParList = *Par.GetVarListPtr();

		rParList.Set(3);

		// Set DoDraw flag to false, i.e. do not draw directly.
		rParList[2] = TCVCounter(0);

		rParList[0].New(PDT_VARLIST, "Constant");
		TVarList &rPointList = *rParList[0].GetVarListPtr();
		rPointList.Set(iSteps+1);

		TVarList *pColList = 0;
		if (bUseColList)
		{
			rParList[1].New(PDT_VARLIST, "Constant");
			pColList = rParList[1].GetVarListPtr();
			pColList->Set(iSteps+1);
		}

		// Initialize free variable
		FreeVar = fMin;

		// Get pointer to free variable
		TCVScalar& rX = *FreeVar.GetScalarPtr();

		// Result variable
		CCodeVar *pR;
		TMultiV vR;
		char pcText[200];

		LockStack();
		for(i=0;i<=iSteps;i++, rX+=fStep)
		{
			if (!(*pCodeList)[0]->Apply(this, &sData))
			{
				bError = true;
				break;
			}

			if (!Pop(pR))
			{
				sprintf(pcText, "Function did not return a value at step %d.", i+1);
				m_ErrorList.GeneralError(pcText, iLine, iPos);
				bError = true;
				break;
			}

			if (!bIsFuncList)
			{
				if (pR->BaseType() != PDT_MULTIV)
				{
					sprintf(pcText, "Value of function at step %d is not a multivector.", i+1);
					m_ErrorList.GeneralError(pcText, iLine, iPos);
					bError = true;
					break;
				}

				rPointList[i] = *(pR->GetMultiVPtr());
			}
			else
			{
				if (pR->BaseType() != PDT_VARLIST)
				{
					sprintf(pcText, "No list returned at step %d.", i+1);
					m_ErrorList.GeneralError(pcText, iLine, iPos);
					bError = true;
					break;
				}

				TVarList &rResList = *( (*pR).GetVarListPtr() );
				int iSize = rResList.Count();
				if (iSize != iListSize)
				{
					sprintf(pcText, "No value returned at step %d.", i+1);
					m_ErrorList.GeneralError(pcText, iLine, iPos);
					bError = true;
					break;
				}

				if (rResList[0].BaseType() != PDT_MULTIV)
				{
					sprintf(pcText, "Value of function at step %d is not a multivector.", i+1);
					m_ErrorList.GeneralError(pcText, iLine, iPos);
					bError = true;
					break;
				}

				rPointList[i] = rResList[0];

				if (bUseColList)
				{
					if (rResList[1].BaseType() != PDT_COLOR)
					{
						sprintf(pcText, "Invalid color value at step %d.", i+1);
						m_ErrorList.GeneralError(pcText, iLine, iPos);
						bError = true;
						break;
					}

					(*pColList)[i] = rResList[1];
				}
			}
		}
		while(Pop(pR));	// Empty Stack
		UnlockStack();

		if (bError)
			return false;

		CMVInfo<float> Info;

		// Only analyze multivector
		m_pFilter->DrawMV(*rPointList[0].GetMultiVPtr(), true);

		// Get MVInfo
		Info = m_pFilter->GetMVInfo();

		if (Info.m_eType == GA_POINT ||
			Info.m_eType == GA_SCALAR)
		{
			return DrawPointListFunc(rVar, Par, iLine, iPos);
		}
		else if (Info.m_eType == GA_LINE)
		{
			return DrawLineSurfaceFunc(rVar, Par, iLine, iPos);
		}
		else if (Info.m_eType == GA_CIRCLE)
		{
			return DrawCircleSurfaceFunc(rVar, Par, iLine, iPos);			
		}
		else
		{
			m_ErrorList.GeneralError("Unable to plot function of multivectors of given type.", iLine, iPos);
			return false;
		}
	}
	///////////////////////////////////////////////////////////////////////
	//// Surface Plot
	else if (iVarCount >= 3)
	{
		TCVScalar fNormScale = 0;

		if (mVars(0).BaseType() != PDT_MULTIV)
		{
			if (mVars(0).BaseType() != PDT_VARLIST)
			{
				m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
				return false;
			}
			
			TVarList &rList = *mVars(0).GetVarListPtr();
			iListSize = rList.Count();

			if (iListSize == 0)
			{
				m_ErrorList.GeneralError("No function given.", iLine, iPos);
				return false;
			}
			
			if (iListSize >= 1)
			{
				if (rList[0].BaseType() != PDT_MULTIV)
				{
					m_ErrorList.GeneralError("First element in function list must be multivector valued.", iLine, iPos);
					return false;
				}
			}

			if (iListSize >= 2)
			{
				if (rList[1].BaseType() == PDT_MULTIV)
				{
					bUseNormList = true;
					iNormListPos = 1;
				}
				else if (rList[1].BaseType() == PDT_COLOR)
				{
					bUseColList = true;
					iColListPos = 1;
				}
				// Counter is a dummy for 'no function given'
				else if (rList[1].BaseType() != PDT_COUNTER)
				{
					m_ErrorList.GeneralError(
						"Expect second element in function list to be either color or multivector (normal).",
						iLine, iPos);
					return false;
				}
			}

			if (iListSize >= 3)
			{
				if (rList[2].BaseType() == PDT_MULTIV)
				{
					if (bUseNormList)
					{
						m_ErrorList.GeneralError(
							"Invalid third element in function list. Surface normals already specified as second element of function list.",
							iLine, iPos);
						return false;
					}
					bUseNormList = true;
					iNormListPos = 2;
				}
				else if (rList[2].BaseType() == PDT_COLOR)
				{
					if (bUseColList)
					{
						m_ErrorList.GeneralError(
							"Invalid third element in function list. Color already specified as second element of function list.",
							iLine, iPos);
						return false;
					}
					bUseColList = true;
					iColListPos = 2;
				}
				else if (rList[2].BaseType() != PDT_COUNTER)
				{
					m_ErrorList.GeneralError(
						"Expect third element in function list to be either color or multivector (normal).",
						iLine, iPos);
					return false;
				}
			}

			if (iListSize >= 4)
			{
				if (rList[3].BaseType() == PDT_MULTIV)
				{
					bUseTexList = true;
					iTexListPos = 3;
				}
				else
				{
					m_ErrorList.GeneralError(
						"Expect fourth element in function list to be a multivector (texture coordinate).",
						iLine, iPos);
					return false;
				}
			}

			bIsFuncList = true;
		}

		if (mVars(1).BaseType() != PDT_VARLIST)
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		if (mVars(2).BaseType() != PDT_VARLIST)
		{
			m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}


		if (iVarCount == 4)
		{
			if (!mVars(3).CastToScalar(fNormScale, m_fSensitivity))
			{
				m_ErrorList.InvalidParType(mVars(3), 4, iLine, iPos);
				return false;
			}
		}

		// List containing first free variable, plot range and step.
		TVarList& mFV1List = *mVars(1).GetVarListPtr();

		// List containing second free variable, plot range and step.
		TVarList& mFV2List = *mVars(2).GetVarListPtr();

		if (mFV1List.Count() != 4)
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		if (mFV2List.Count() != 4)
		{
			m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}

		TCVScalar fMin1, fMax1, fStep1;
		TCVScalar fMin2, fMax2, fStep2;
		TCVCounter iSteps1, iSteps2, i, j;

		// Free variable to be used
		CCodeVar &FreeVar1 = GetVar(mFV1List(0).Name());
		CCodeVar &FreeVar2 = GetVar(mFV2List(0).Name());

		if (FreeVar1.BaseType() == PDT_NOTYPE)
		{
			m_ErrorList.GeneralError("No free variable given for first dimension.", iLine, iPos);	
			return false;
		}

		if (!mFV1List(1).CastToScalar(fMin1, m_fSensitivity))
		{
			m_ErrorList.GeneralError("Minimum value is not a scalar.", iLine, iPos);
			return false;
		}

		if (!mFV1List(2).CastToScalar(fMax1, m_fSensitivity))
		{
			m_ErrorList.GeneralError("Maximum value is not a scalar.", iLine, iPos);
			return false;
		}

		if (!mFV1List(3).CastToCounter(iSteps1))
		{
			m_ErrorList.GeneralError("The number of steps must be an integer value.", iLine, iPos);
			return false;
		}

		if (fMin1 >= fMax1 || iSteps1 < 1)
		{
			m_ErrorList.GeneralError("Minimum value has to be smaller than maximum value.", iLine, iPos);
			return false;
		}

		fStep1 = (fMax1 - fMin1) / TCVScalar(iSteps1);

		if (FreeVar2.BaseType() == PDT_NOTYPE)
		{
			m_ErrorList.GeneralError("No free variable given for second dimension.", iLine, iPos);	
			return false;
		}

		if (!mFV2List(1).CastToScalar(fMin2, m_fSensitivity))
		{
			m_ErrorList.GeneralError("Minimum value is not a scalar.", iLine, iPos);
			return false;
		}

		if (!mFV2List(2).CastToScalar(fMax2, m_fSensitivity))
		{
			m_ErrorList.GeneralError("Maximum value is not a scalar.", iLine, iPos);
			return false;
		}

		if (!mFV2List(3).CastToCounter(iSteps2))
		{
			m_ErrorList.GeneralError("The number of steps must be an integer value.", iLine, iPos);
			return false;
		}

		if (fMin2 >= fMax2 || iSteps2 < 1)
		{
			m_ErrorList.GeneralError("Minimum value has to be smaller than maximum value.", iLine, iPos);
			return false;
		}

		fStep2 = (fMax2 - fMin2) / TCVScalar(iSteps2);

		SCodeData sData;
		sData.Set(iLine, iPos, 0);
		
/*
		// Initialize result variable
		if (!rVar.New(PDT_VEXLIST, "Constant"))
		{
			m_ErrorList.OutOfMemory(iLine, iPos);
			return false;
		}
		
		TVexList& rVexList = *rVar.GetVexListPtr();
		rVexList.SetMode((GLenum)m_iPlotMode);
*/
		// Initialize Point List
		CCodeVar Par;
		Par.New(PDT_VARLIST, "Constant");

		TVarList &rParList = *Par.GetVarListPtr();

		rParList.Set(9);

		// Set DoDraw flag to false
		rParList[4] = TCVCounter(0);

		// Set negate normals flag to false
		rParList[5] = TCVCounter(0);

		// Set Norm Scale
		rParList[7] = fNormScale;

		rParList[0] = TCVCounter(iSteps1 + 1);
		rParList[1] = TCVCounter(iSteps2 + 1);
		rParList[2].New(PDT_VARLIST, "Constant");
		TVarList &rPointList = *rParList[2].GetVarListPtr();
		rPointList.Set((iSteps1+1)*(iSteps2+1));

		TVarList *pColList = 0;
		TVarList *pNormList = 0;
		TVarList *pTexList = 0;

		if (bUseColList)
		{
			rParList[3].New(PDT_VARLIST, "Constant");
			pColList = rParList[3].GetVarListPtr();
			pColList->Set((iSteps1+1)*(iSteps2+1));
		}

		if (bUseNormList)
		{
			rParList[6].New(PDT_VARLIST, "Constant");
			pNormList = rParList[6].GetVarListPtr();
			pNormList->Set((iSteps1+1)*(iSteps2+1));
		}

		if (bUseTexList)
		{
			rParList[8].New(PDT_VARLIST, "Constant");
			pTexList = rParList[8].GetVarListPtr();
			pTexList->Set((iSteps1+1)*(iSteps2+1));
		}

		// Initialize free variable
		FreeVar1 = fMin1;
		FreeVar2 = fMin2;

		// Get pointer to free variable
		TCVScalar& rX1 = *FreeVar1.GetScalarPtr();
		TCVScalar& rX2 = *FreeVar2.GetScalarPtr();

		// Result variable
		CCodeVar *pR;
		TMultiV vR;
		int iPos = 0;
		char pcText[200];

		LockStack();
		for(j=0;j<=iSteps2;j++, rX2 += fStep2)
		{
			rX1 = fMin1;
			for(i=0;i<=iSteps1;i++, rX1 += fStep1, iPos++)
			{
				if (!(*pCodeList)[0]->Apply(this, &sData))
				{
					bError = true;
					break;
				}

				if (!Pop(pR))
				{
					sprintf(pcText, "Function did not return a value at step %d.", i+1);
					m_ErrorList.GeneralError(pcText, iLine, iPos);
					bError = true;
					break;
				}

				CCodeVar& rRes = pR->DereferenceVarPtr(true);

				if (!bIsFuncList)
				{
					if (rRes.BaseType() != PDT_MULTIV)
					{
						sprintf(pcText, "Value of function at step %d is not a multivector.", i+1);
						m_ErrorList.GeneralError(pcText, iLine, iPos);
						bError = true;
						break;
					}

					rPointList[iPos] = *(rRes.GetMultiVPtr());
				}
				else
				{
					if (rRes.BaseType() != PDT_VARLIST)
					{
						sprintf(pcText, "No list returned at step %d.", i+1);
						m_ErrorList.GeneralError(pcText, iLine, iPos);
						bError = true;
						break;
					}

					TVarList &rResList = *( rRes.GetVarListPtr() );
					int iSize = rResList.Count();
					if (iSize != iListSize)
					{
						sprintf(pcText, "No value returned at step %d.", i+1);
						m_ErrorList.GeneralError(pcText, iLine, iPos);
						bError = true;
						break;
					}

					CCodeVar& rResMV = rResList[0].DereferenceVarPtr(true);
					if (rResMV.BaseType() != PDT_MULTIV)
					{
						sprintf(pcText, "Value of function at step %d is not a multivector.", i+1);
						m_ErrorList.GeneralError(pcText, iLine, iPos);
						bError = true;
						break;
					}

					rPointList[iPos] = rResMV;

					if (bUseColList)
					{
						CCodeVar& rResCol = rResList[iColListPos].DereferenceVarPtr(true);
						if (rResCol.BaseType() != PDT_COLOR)
						{
							sprintf(pcText, "Invalid color value at step %d.", i+1);
							m_ErrorList.GeneralError(pcText, iLine, iPos);
							bError = true;
							break;
						}

						(*pColList)[iPos] = rResCol;
					}

					if (bUseNormList)
					{
						CCodeVar& rResNorm = rResList[iNormListPos].DereferenceVarPtr(true);
						if (rResNorm.BaseType() != PDT_MULTIV)
						{
							sprintf(pcText, "Invalid normal at step %d.", i+1);
							m_ErrorList.GeneralError(pcText, iLine, iPos);
							bError = true;
							break;
						}

						(*pNormList)[iPos] = rResNorm;
					}

					if (bUseTexList)
					{
						CCodeVar& rResNorm = rResList[iTexListPos].DereferenceVarPtr(true);
						if (rResNorm.BaseType() != PDT_MULTIV)
						{
							sprintf(pcText, "Invalid texture coordinate at step %d.", i+1);
							m_ErrorList.GeneralError(pcText, iLine, iPos);
							bError = true;
							break;
						}

						(*pTexList)[iPos] = rResNorm;
					}

				}
			}
		}
		while(Pop(pR));	// Empty Stack
		UnlockStack();

		if (bError)
			return false;

		return DrawPointSurfaceFunc(rVar, Par, iLine, iPos);
	}
	else
	{
		m_ErrorList.WrongNoOfParams(2, iLine, iPos);
		return false;
	}
	
	return true;
}



//////////////////////////////////////////////////////////////////////
/// The Color FUNCTION

bool CCLUCodeBase::ColorFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();

	if (iVarCount == 1)
	{
		if (mVars(0).BaseType() != PDT_MULTIV)
		{
			m_ErrorList.GeneralError("Expect vector as single parameter.", iLine, iPos);
			return false;
		}

		TMultiV &rA = *mVars(0).GetMultiVPtr();

		// Analyze Multivector
		if (!m_pFilter->DrawMV(rA, true))
		{
			m_ErrorList.GeneralError("Multivector does not represent point.", iLine, iPos);
			return false;
		}

		CMVInfo<TCVScalar> Info;
		m_pFilter->GetMVInfo(Info);
		if (Info.m_eType != GA_POINT)
		{
			m_ErrorList.GeneralError("Multivector does not represent point.", iLine, iPos);
			return false;
		}

		if (!rVar.New(PDT_COLOR, "Constant"))
		{
			m_ErrorList.OutOfMemory(iLine, iPos);
			return false;
		}

		COGLColor& rCol = *rVar.GetOGLColorPtr();

		rCol.Set((float)Info.m_mVex[0][0], (float)Info.m_mVex[0][1], 
				 (float)Info.m_mVex[0][2]);
	}
	else if (iVarCount == 3)
	{
		TCVScalar fR, fG, fB;

		if (!mVars(0).CastToScalar(fR, m_fSensitivity))
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (!mVars(1).CastToScalar(fG, m_fSensitivity))
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		if (!mVars(2).CastToScalar(fB, m_fSensitivity))
		{
			m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}
/*
		if (fR < 0) fR = 0.0;
		else if (fR > 1) fR = 1.0;

		if (fG < 0) fG = 0.0;
		else if (fG > 1) fG = 1.0;
		
		if (fB < 0) fB = 0.0;
		else if (fB > 1) fB = 1.0;
*/
		if (!rVar.New(PDT_COLOR, "Constant"))
		{
			m_ErrorList.OutOfMemory(iLine, iPos);
			return false;
		}

		COGLColor& rCol = *rVar.GetOGLColorPtr();

		rCol.Set((float)fR, (float)fG, (float)fB);	
	}
	else if (iVarCount == 4)
	{
		TCVScalar fR, fG, fB, fA;

		if (!mVars(0).CastToScalar(fR, m_fSensitivity))
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (!mVars(1).CastToScalar(fG, m_fSensitivity))
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		if (!mVars(2).CastToScalar(fB, m_fSensitivity))
		{
			m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}

		if (!mVars(3).CastToScalar(fA, m_fSensitivity))
		{
			m_ErrorList.InvalidParType(mVars(3), 4, iLine, iPos);
			return false;
		}
/*
		if (fR < 0) fR = 0.0;
		else if (fR > 1) fR = 1.0;

		if (fG < 0) fG = 0.0;
		else if (fG > 1) fG = 1.0;
		
		if (fB < 0) fB = 0.0;
		else if (fB > 1) fB = 1.0;

		if (fA < 0) fA = 0.0;
		else if (fA > 1) fA = 1.0;
*/
		if (!rVar.New(PDT_COLOR, "Constant"))
		{
			m_ErrorList.OutOfMemory(iLine, iPos);
			return false;
		}

		COGLColor& rCol = *rVar.GetOGLColorPtr();

		rCol.Set((float)fR, (float)fG, (float)fB, (float)fA);	
	}
	else
	{
		int piParCount[2] = {3, 4};

		m_ErrorList.WrongNoOfParams(piParCount, 2, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// The Grade List FUNCTION
/// Returns list of grades contained in multivector

bool CCLUCodeBase::GradeListFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	if (iVarCount == 1)
	{
		TCVScalar dVal;

		if (!rVar.New(PDT_VARLIST, "Constant"))
		{
			m_ErrorList.OutOfMemory(iLine, iPos);
			return false;
		}

		TVarList& rList = *rVar.GetVarListPtr();

		if (mVars(0).CastToScalar(dVal))
		{
			rList.Set(1);
			rList[0] = 0;
			return true;
		}
		else if (mVars(0).BaseType() != PDT_MULTIV)
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		TMultiV &vA = *mVars(0).GetMultiVPtr();
		Mem<uint> mGList;

		GradeList(vA, mGList, m_fSensitivity);
		uint uCount = mGList.Count();

		if (!rList.Set(uCount))
		{
			m_ErrorList.OutOfMemory(iLine, iPos);
			return false;
		}

		for(uint u=0;u<uCount;u++)
		{
			rList[u] = (TCVCounter) mGList[u];
		}
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////
/// The Len FUNCTION
/// Returns length of string or list

bool CCLUCodeBase::LenFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	if (iVarCount == 1)
	{
		ECodeDataType eType = mVars(0).BaseType();

		if (eType == PDT_STRING)
		{
			cStr &rStr = *mVars(0).GetStringPtr();

			rVar = (int) rStr.Len();
		}
		else if (eType == PDT_VARLIST)
		{
			TVarList &rList = *mVars(0).GetVarListPtr();

			rVar = (int) rList.Count();
		}
		else if (eType == PDT_VEXLIST)
		{
			TVexList &rList = *mVars(0).GetVexListPtr();

			rVar = (int) rList.Count();
		}
		else if (eType == PDT_INT)
		{
			int iVal = *mVars(0).GetIntPtr();

			if (iVal == 0)
				rVar = (int) 0;
			else
				rVar = (int) 1;
		}
		else
		{
			rVar = (int) 1;
			//m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			//return false;
		}
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////
/// The Size FUNCTION
/// Returns the size of various objects

bool CCLUCodeBase::SizeFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	if (iVarCount == 1)
	{
		ECodeDataType eType = mVars(0).BaseType();

		if (eType == PDT_STRING)
		{
			cStr &rStr = *mVars(0).GetStringPtr();

			rVar = (int) rStr.Len();
		}
		else if (eType == PDT_VARLIST)
		{
			TVarList &rList = *mVars(0).GetVarListPtr();

			rVar = (int) rList.Count();
		}
		else if (eType == PDT_VEXLIST)
		{
			TVexList &rList = *mVars(0).GetVexListPtr();

			rVar = (int) rList.Count();
		}
		else if (eType == PDT_INT)
		{
			int iVal = *mVars(0).GetIntPtr();

			if (iVal == 0)
				rVar = (int) 0;
			else
				rVar = (int) 1;
		}
		else if (eType == PDT_MULTIV)
		{
			TMultiV &vA = *mVars(0).GetMultiVPtr();

			rVar = (TCVCounter) vA.GetGADim();
		}
		else if (eType == PDT_MATRIX)
		{
			TMatrix &xM = *mVars(0).GetMatrixPtr();

			rVar.New(PDT_VARLIST, "Constant");
			TVarList &rList = *rVar.GetVarListPtr();
			rList.Set(2);
			rList[0] = xM.Rows();
			rList[1] = xM.Cols();
		}
		else if (eType == PDT_TENSOR)
		{
			TTensor &rT = *mVars(0).GetTensorPtr();

			rVar.New(PDT_VARLIST, "Constant");
			TVarList &rList = *rVar.GetVarListPtr();

			int iDim, iValence = rT.Valence();
			rList.Set(iValence);

			for( iDim = 0; iDim < iValence; iDim++)
			{
				rList[iDim] = rT.DimSize(iDim);
			}
		}
		else if (eType == PDT_TENSOR_IDX)
		{
			TTensorIdx &rTIdx = *mVars(0).GetTensorIdxPtr();

			rVar.New(PDT_VARLIST, "Constant");
			TVarList &rList = *rVar.GetVarListPtr();

			try
			{
				int iLoop, iLoopCount;
				const TTensorIdx::TFreeIdxMap& mapFreeIdx = rTIdx.GetFreeIdxMap();
				TTensorIdx::TFreeIdxMap::const_iterator it_El, it_End;

				rList.Set(iLoopCount = mapFreeIdx.size());
				iLoop = iLoopCount - 1;
				it_End = mapFreeIdx.end();
				for(it_El = mapFreeIdx.begin();
					it_El != it_End;
					++it_El, --iLoop)
				{
					rList[iLoop] = it_El->second.iMax - it_El->second.iMin + 1;
				}
			}
			catch(...)
			{
				m_ErrorList.GeneralError("Error evaluating size of tensor index.", iLine, iPos);
				return false;
			}
		}
		else if (eType == PDT_IMAGE)
		{
			TImage& Img = *mVars(0).GetImagePtr();
			int iWidth, iHeight;

			Img->GetSize(iWidth, iHeight);

			rVar.New(PDT_VARLIST, "Constant");
			TVarList &rList = *rVar.GetVarListPtr();
			rList.Set(2);
			rList[0] = iWidth;
			rList[1] = iHeight;
		}
		else
		{
			rVar = (int) 1;
			//m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			//return false;
		}
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}




//////////////////////////////////////////////////////////////////////
/// The Type FUNCTION
/// Returns type of variable as string

bool CCLUCodeBase::TypeFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	if (iVarCount == 1)
	{
		if (mVars(0).BaseType() == PDT_PTR_VAR)
		{
			CCodeVar &rPar = mVars(0).DereferenceVarPtr();
			rVar = rPar.TypeStr();
		}
		else
		{
			rVar = mVars(0).TypeStr();
		}
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}






//////////////////////////////////////////////////////////////////////
/// Start View

bool CCLUCodeBase::StartViewFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();

	if (iVarCount != 4)
	{
		m_ErrorList.WrongNoOfParams(4, iLine, iPos);
		return false;
	}

	TCVScalar pfVec[4];
	int i;
	for(i=0;i<4;i++)
	{
		if ( !(mVars(i).CastToScalar(pfVec[i], m_fSensitivity)) )
		{
			m_ErrorList.InvalidParType(mVars(i), i+1, iLine, iPos);
			return false;
		}
	}

	if (pfVec[0] < 0)
	{
		m_ErrorList.GeneralError("Position x of view out of range.", iLine, iPos);
		return false;
	}

	if (pfVec[1] < 0)
	{
		m_ErrorList.GeneralError("Position y of view out of range.", iLine, iPos);
		return false;
	}

	if (pfVec[2] <= 0)
	{
		m_ErrorList.GeneralError("Width of view out of range.", iLine, iPos);
		return false;
	}

	if (pfVec[3] <= 0)
	{
		m_ErrorList.GeneralError("Height of view out of range.", iLine, iPos);
		return false;
	}

	int iW, iH;
	SViewData CurView, NewView;

	m_pCLUDrawBase->GetViewport(CurView.iX, CurView.iY, CurView.iW, CurView.iH);
	m_vecViewStack.push_back(CurView);

	iW = CurView.iW;
	iH = CurView.iH;

	if (pfVec[0] >= 0 && pfVec[0] <= 1)
		NewView.iX = int( float(iW) * float(pfVec[0]) );
	else if (int(pfVec[0]) <= iW-2)
		NewView.iX = int( pfVec[0] );
	else
		NewView.iX = iW - 2;

	if (pfVec[1] >= 0 && pfVec[1] <= 1)
		NewView.iY = int( float(iH) * float(pfVec[1]) );
	else if (int(pfVec[1]) <= iH-2)
		NewView.iY = int( pfVec[1] );
	else
		NewView.iY = iH - 2;

	if (pfVec[2] >= 0 && pfVec[2] <= 1)
		NewView.iW = int( float(iW) * float(pfVec[2]) );
	else
		NewView.iW = int( pfVec[2] );

	if (pfVec[3] >= 0 && pfVec[3] <= 1)
		NewView.iH = int( float(iH) * float(pfVec[3]) );
	else
		NewView.iH = int( pfVec[3] );

	if (NewView.iX + NewView.iW - 1 >= iW)
		NewView.iW = iW - NewView.iX + 1;

	if (NewView.iY + NewView.iH - 1 >= iH)
		NewView.iH = iH - NewView.iY + 1;

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	m_pCLUDrawBase->SetViewport(NewView.iX, NewView.iY, NewView.iW, NewView.iH);

	// Translate Modelview Frame
	glTranslatef(0, 0, -5.0f);


	return true;
} 


//////////////////////////////////////////////////////////////////////
/// End View

bool CCLUCodeBase::EndViewFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();

	if (iVarCount != 0)
	{
		m_ErrorList.WrongNoOfParams(0, iLine, iPos);
		return false;
	}

	if (m_vecViewStack.size() > 0)
	{
		SViewData NewView = m_vecViewStack.back();
		
		m_pCLUDrawBase->SetViewport(NewView.iX, NewView.iY, NewView.iW, NewView.iH);
		m_vecViewStack.pop_back();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}

	return true;
} 


//////////////////////////////////////////////////////////////////////
/// Start Overlay

bool CCLUCodeBase::StartOverlayFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();

	if (iVarCount != 0)
	{
		m_ErrorList.WrongNoOfParams(0, iLine, iPos);
		return false;
	}
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 100, 100, 0, -100, 100);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	m_bOverlayModeActive = true;

	return true;
} 

//////////////////////////////////////////////////////////////////////
/// End Overlay

bool CCLUCodeBase::EndOverlayFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();

	if (iVarCount != 0)
	{
		m_ErrorList.WrongNoOfParams(0, iLine, iPos);
		return false;
	}
	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	m_bOverlayModeActive = false;

	return true;
} 


//////////////////////////////////////////////////////////////////////
/// Push Frame

bool CCLUCodeBase::PushFrameFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();

	if (iVarCount != 0)
	{
		m_ErrorList.WrongNoOfParams(0, iLine, iPos);
		return false;
	}
	
	glPushMatrix();
	m_iFrameStackDepth++;
	
	return true;
} 

//////////////////////////////////////////////////////////////////////
/// Pop Frame

bool CCLUCodeBase::PopFrameFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();

	if (iVarCount != 0)
	{
		m_ErrorList.WrongNoOfParams(0, iLine, iPos);
		return false;
	}

	if (m_iFrameStackDepth > 0)
	{
		glPopMatrix();
		m_iFrameStackDepth--;
	}

	return true;
} 



//////////////////////////////////////////////////////////////////////
/// Set Point Size Function

bool CCLUCodeBase::SetPointSizeFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	TCVScalar dVal;
	
	if (iVarCount == 1)
	{
		if (!mVars(0).CastToScalar(dVal, m_fSensitivity))
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}								
		
		SetPointSize(float(dVal));
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
} 


//////////////////////////////////////////////////////////////////////
/// Get Point Size Function

bool CCLUCodeBase::GetPointSizeFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	if (iVarCount != 0)
	{
		m_ErrorList.WrongNoOfParams(0, iLine, iPos);
		return false;
	}
	
	rVar = TCVScalar(GetPointSize());
	
	return true;
} 



//////////////////////////////////////////////////////////////////////
/// Set Line Width Function

bool CCLUCodeBase::SetLineWidthFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	TCVScalar dVal;
	
	if (iVarCount == 1)
	{
		if (!mVars(0).CastToScalar(dVal, m_fSensitivity))
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}								
		
		SetLineWidth(float(dVal));
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
} 


//////////////////////////////////////////////////////////////////////
/// Get Line Width Function

bool CCLUCodeBase::GetLineWidthFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	if (iVarCount != 0)
	{
		m_ErrorList.WrongNoOfParams(0, iLine, iPos);
		return false;
	}
	
	rVar = TCVScalar(GetLineWidth());
	
	return true;
} 


//////////////////////////////////////////////////////////////////////
/// Set Smooth Point Function

bool CCLUCodeBase::SetSmoothPointFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	TCVCounter iVal;
	
	if (iVarCount == 1)
	{
		if (!mVars(0).CastToCounter(iVal))
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}								

		if (iVal)
			SetSmoothPoint(true);
		else
			SetSmoothPoint(false);
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
} 


//////////////////////////////////////////////////////////////////////
/// Get Smooth Point Function

bool CCLUCodeBase::GetSmoothPointFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	if (iVarCount != 0)
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	rVar = TCVCounter( GetSmoothPoint() ? 1 : 0 );

	return true;
} 



//////////////////////////////////////////////////////////////////////
/// Set Smooth Line Function

bool CCLUCodeBase::SetSmoothLineFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	TCVCounter iVal;
	
	if (iVarCount == 1)
	{
		if (!mVars(0).CastToCounter(iVal))
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}								

		if (iVal)
			SetSmoothLine(true);
		else
			SetSmoothLine(false);
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
} 


//////////////////////////////////////////////////////////////////////
/// Get Smooth Line Function

bool CCLUCodeBase::GetSmoothLineFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	if (iVarCount != 0)
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	rVar = TCVCounter( GetSmoothLine() ? 1 : 0 );

	return true;
} 


//////////////////////////////////////////////////////////////////////
/// Set Smooth Polygon Function

bool CCLUCodeBase::SetSmoothPolygonFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	TCVCounter iVal;
	
	if (iVarCount == 1)
	{
		if (!mVars(0).CastToCounter(iVal))
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}								

		if (iVal)
			SetSmoothPolygon(true);
		else
			SetSmoothPolygon(false);
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
} 


//////////////////////////////////////////////////////////////////////
/// Get Smooth Polygon Function

bool CCLUCodeBase::GetSmoothPolygonFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	if (iVarCount != 0)
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	rVar = TCVCounter( GetSmoothPolygon() ? 1 : 0 );

	return true;
} 



//////////////////////////////////////////////////////////////////////
// Set Frame Origin

bool CCLUCodeBase::SetFrameOriginFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	if (iVarCount == 3)
	{
		TCVScalar pfVec[3];
		int i;
		
		for(i=0;i<3;i++)
		{
			if ( !(mVars(i).CastToScalar(pfVec[i], m_fSensitivity)) )
			{
				m_ErrorList.InvalidType(mVars(i), iLine, iPos);
				return false;
			}
		}

		// Set z-dim relative to a place that can be seen.
		pfVec[2] -= TCVScalar(5);

		STransform &rTrans = (*m_pTransform)[0];
		for(int i = 0; i < 3; i++)
		{
			if (rTrans.pfTrans[i] != pfVec[i])
			{
				rTrans.pfTrans[i] = GLfloat(pfVec[i]);
				rTrans.SetTransChanged(i);
			}
		}
	}
	else
	{
		int piVal[] = { 3 };
		m_ErrorList.WrongNoOfParams(piVal, 1, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
// Translate View Origin (Camera)

bool CCLUCodeBase::TranslateViewFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	TCVScalar pfVec[3];
	
	if (iVarCount == 3)
	{
		int i;
		
		for(i=0;i<3;i++)
		{
			if ( !(mVars(i).CastToScalar(pfVec[i], m_fSensitivity)) )
			{
				m_ErrorList.InvalidType(mVars(i), iLine, iPos);
				return false;
			}
		}

	}
	else if (iVarCount == 1)
	{
		if ( mVars(0).BaseType() != PDT_MULTIV )
		{
			m_ErrorList.InvalidType(mVars(0), iLine, iPos);
			return false;
		}

		TMultiV &vA = *mVars(0).GetMultiVPtr();
		TMultiV vR;

		CastMVtoE3(vA, vR);
		pfVec[0] = vR[1];
		pfVec[1] = vR[2];
		pfVec[2] = vR[3];
	}
	else
	{
		int piVal[] = { 1, 3 };
		m_ErrorList.WrongNoOfParams(piVal, 2, iLine, iPos);
		return false;
	}

	glMatrixMode(GL_PROJECTION);
	glTranslatef(-GLfloat(pfVec[0]), -GLfloat(pfVec[1]), -GLfloat(pfVec[2]));
	glMatrixMode(GL_MODELVIEW);

	return true;
}


//////////////////////////////////////////////////////////////////////
// Rotate View Function

bool CCLUCodeBase::RotateViewFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	if (iVarCount == 1)
	{
		if (mVars(0).BaseType() != PDT_MULTIV)
		{
			m_ErrorList.InvalidType(mVars(0), iLine, iPos);
			return false;
		}
		
		float fFrameAngle;
		TMultiV vAxis;
		TMultiV &vR = *mVars(0).GetMultiVPtr();
		
		fFrameAngle = float(2.0 * acos(Scalar(vR)) / m_fRadPerDeg);
	
		glMatrixMode(GL_PROJECTION);
		glRotatef(fFrameAngle, GLfloat(vR[m_E3GABase.iL1]), 
					GLfloat(vR[m_E3GABase.iL2]), GLfloat(vR[m_E3GABase.iL3]) );
		glMatrixMode(GL_MODELVIEW);
	}
	else
	{
		int piVal[] = { 1 };
		m_ErrorList.WrongNoOfParams(piVal, 1, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
// Set View Angle (Camera)

bool CCLUCodeBase::SetViewAngleFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	TCVScalar fVal;
	
	if (iVarCount == 1)
	{
		if ( !(mVars(0).CastToScalar(fVal, m_fSensitivity)) )
		{
			m_ErrorList.InvalidType(mVars(0), iLine, iPos);
			return false;
		}
	}
	else
	{
		int piVal[] = { 1 };
		m_ErrorList.WrongNoOfParams(piVal, 1, iLine, iPos);
		return false;
	}

	if (fVal < 1.0f || fVal > 179.0f)
	{
		m_ErrorList.GeneralError("View angle has to be in the range of 1 to 179.", iLine, iPos);
		return false;
	}

	glPushMatrix();
	m_pCLUDrawBase->SetPerspectiveAngle( GLfloat(fVal) );
	glPopMatrix();

	return true;
}



//////////////////////////////////////////////////////////////////////
// Translate Frame Origin

bool CCLUCodeBase::TranslateFrameFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	TCVScalar pfVec[3];
	
	if (iVarCount == 3)
	{
		int i;
		
		for(i=0;i<3;i++)
		{
			if ( !(mVars(i).CastToScalar(pfVec[i], m_fSensitivity)) )
			{
				m_ErrorList.InvalidType(mVars(i), iLine, iPos);
				return false;
			}
		}

	}
	else if (iVarCount == 1)
	{
		if ( mVars(0).BaseType() != PDT_MULTIV )
		{
			m_ErrorList.InvalidType(mVars(0), iLine, iPos);
			return false;
		}

		TMultiV &vA = *mVars(0).GetMultiVPtr();
		TMultiV vR;

		CastMVtoE3(vA, vR);
		pfVec[0] = vR[1];
		pfVec[1] = vR[2];
		pfVec[2] = vR[3];
	}
	else
	{
		int piVal[] = { 3 };
		m_ErrorList.WrongNoOfParams(piVal, 1, iLine, iPos);
		return false;
	}

	glTranslatef(GLfloat(pfVec[0]), GLfloat(pfVec[1]), GLfloat(pfVec[2]));

	return true;
}


//////////////////////////////////////////////////////////////////////
// Rotate Frame Function

bool CCLUCodeBase::RotateFrameFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	if (iVarCount == 1)
	{
		if (mVars(0).BaseType() != PDT_MULTIV)
		{
			m_ErrorList.InvalidType(mVars(0), iLine, iPos);
			return false;
		}
		
		float fFrameAngle;
		TMultiV vAxis;
		TMultiV &vR = *mVars(0).GetMultiVPtr();
		
		fFrameAngle = -float(2.0 * acos(Scalar(vR)) / m_fRadPerDeg);

		glRotatef(fFrameAngle, GLfloat(vR[m_E3GABase.iL1]), 
					GLfloat(vR[m_E3GABase.iL2]), GLfloat(vR[m_E3GABase.iL3]) );
	}
	else
	{
		int piVal[] = { 1 };
		m_ErrorList.WrongNoOfParams(piVal, 1, iLine, iPos);
		return false;
	}
	
	return true;
}



//////////////////////////////////////////////////////////////////////
// Set Frame Rotor

bool CCLUCodeBase::SetFrameRotorFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	if (iVarCount == 1)
	{
		if (mVars(0).BaseType() != PDT_MULTIV)
		{
			m_ErrorList.InvalidType(mVars(0), iLine, iPos);
			return false;
		}
		
		TMultiV &vR = *mVars(0).GetMultiVPtr();
		MultiV<float> &vRot = *m_pFrameRotor;
		
		vRot = E3GA<float>::vSC;
		
		vRot[0] = float(vR[0]);
		vRot[E3GA<float>::iL1] = float(vR[E3GA<float>::iL1]);
		vRot[E3GA<float>::iL2] = float(vR[E3GA<float>::iL2]);
		vRot[E3GA<float>::iL3] = float(vR[E3GA<float>::iL3]);
	}
	else
	{
		int piVal[] = { 1 };
		m_ErrorList.WrongNoOfParams(piVal, 1, iLine, iPos);
		return false;
	}
	
	return true;
}



//////////////////////////////////////////////////////////////////////
// Add Frame Origin

bool CCLUCodeBase::AddFrameOriginFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	if (iVarCount == 3)
	{
		TCVScalar pfVec[3];
		int i;
		
		for(i=0;i<3;i++)
		{
			if ( !(mVars(i).CastToScalar(pfVec[i], m_fSensitivity)) )
			{
				m_ErrorList.InvalidType(mVars(i), iLine, iPos);
				return false;
			}
		}
		
		STransform &rTrans = (*m_pTransform)[0];
		for(int i = 0; i < 3; i++)
		{
			rTrans.pfTrans[i] += GLfloat(pfVec[i]);
			rTrans.SetTransChanged(i);
		}
	}
	else
	{
		int piVal[] = { 3 };
		m_ErrorList.WrongNoOfParams(piVal, 1, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
// Add Frame Rotor

bool CCLUCodeBase::AddFrameRotorFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	if (iVarCount == 1)
	{
		if (mVars(0).BaseType() != PDT_MULTIV)
		{
			m_ErrorList.InvalidType(mVars(0), iLine, iPos);
			return false;
		}
		
		TMultiV &vR = *mVars(0).GetMultiVPtr();
		MultiV<float> vRot;
		MultiV<float> &vFrameRot = *m_pFrameRotor;
		
		vRot = E3GA<float>::vSC;
		
		vRot[0] = float(vR[0]);
		vRot[E3GA<float>::iL1] = float(vR[E3GA<float>::iL1]);
		vRot[E3GA<float>::iL2] = float(vR[E3GA<float>::iL2]);
		vRot[E3GA<float>::iL3] = float(vR[E3GA<float>::iL3]);

		vFrameRot = vRot & vFrameRot;
	}
	else
	{
		int piVal[] = { 1 };
		m_ErrorList.WrongNoOfParams(piVal, 1, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// The Argument FUNCTION
/// Returns a list of arguments, for which the element passed with these arguments is true.

bool CCLUCodeBase::ArgTrueFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	TCVCounter iRetainStructure = 0;
	
	if (iVarCount == 0 || iVarCount > 2)
	{
		int piPar[] = {1, 2};

		m_ErrorList.WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (iVarCount == 2)
	{
		if (!mVars(1).CastToCounter(iRetainStructure))
		{
			m_ErrorList.GeneralError("Expect integer value 0 or 1 as second parameter.", iLine, iPos);
			return false;
		}
	}

	ECodeDataType eType = mVars(0).BaseType();

	if (eType == PDT_MATRIX)
	{
		int iR, iC, iRows, iCols;
		TMatrix &xM = *mVars(0).GetMatrixPtr();
		iRows = xM.Rows();
		iCols = xM.Cols();

		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rList = *rVar.GetVarListPtr();

		for(iR = 1; iR <= iRows; iR++)
		{
			/*
			rList.Add(1);
			rList.Last().New(PDT_VARLIST, "Constant");
			TVarList &rRowList = *rList.Last().GetVarListPtr();
			*/
			for (iC = 1; iC <= iCols; iC++)
			{
				if (xM(iR,iC) != TCVScalar(0))
				{
					rList.Add(1);
					rList.Last().New(PDT_VARLIST, "Constant");

					TVarList &rArg = *rList.Last().GetVarListPtr();
					rArg.Set(2);
					rArg[0] = iR;
					rArg[1] = iC;
				}
			}

			/*
			if (rRowList.Count() == 0)
			{
			rList.Sub(1);
			}
			*/
		}
	}
	else if (eType == PDT_VARLIST)
	{
		/*
		TVarList& rList = *mVars(0).GetVarListPtr();
		int i, iCount = rList.Count();
		*/
		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rRetList = *rVar.GetVarListPtr();

		TVarList IdxList;

		if (!ArgTrueList(rRetList, IdxList, mVars(0), (iRetainStructure ? true : false), 
						iLine, iPos))
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}
	}
	else
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////
/// ArgTrueList. Return arguments of list where content is not equal to zero

bool CCLUCodeBase::ArgTrueList(TVarList& rArgList, TVarList& rIdxList, CCodeVar& rData,
							   bool bRetainStructure, int iLine, int iPos)
{
	TCVScalar dVal;

	if (rData.BaseType() == PDT_VARLIST)
	{
		TVarList& rList = *rData.GetVarListPtr();
		int i, iCount = rList.Count();
		TVarList *pSubArgList;

		if (rIdxList.Count() == 0)
		{
			pSubArgList = &rArgList;
		}
		else
		{
			rArgList.Add(1);
			rArgList.Last().New(PDT_VARLIST, "Constant");
			pSubArgList = rArgList.Last().GetVarListPtr();
		}

		TVarList CurIdxList = rIdxList;
		CurIdxList.Add(1);

		for ( i = 0; i < iCount; i++ )
		{
			CurIdxList.Last() = i+1;

			if (!ArgTrueList(*pSubArgList, CurIdxList, rList(i), bRetainStructure, iLine, iPos))
				return false;
		}

		// If sublist of arguments is empty, then remove it again.
		if (!bRetainStructure && pSubArgList->Count() == 0 && rIdxList.Count() > 0)
		{
			rArgList.Sub(1);
		}
	}
	else if (rData.CastToScalar(dVal, m_fSensitivity))
	{
		if (dVal != TCVScalar(0))
		{
			rArgList.Add(1);
			rArgList.Last() = rIdxList;
		}
	}
	else
	{
		return false;
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Sorting

bool CCLUCodeBase::SortFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList &mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	int iAscend, iExecute;
	vector<int> vecSortIdx;
	bool bSortExecute = true;
	
	m_pSortCode = 0;
	m_bSortAscend = true;
	m_iSortLine = iLine;
	m_iSortPos = iPos;

	if (iVarCount < 1 || iVarCount > 3)
	{
		int piPar[] = {1, 2, 3};

		m_ErrorList.WrongNoOfParams(piPar, 3, iLine, iPos);
		return false;
	}

	if (iVarCount > 1)
	{
		if (!mVars(1).CastToCounter(iAscend))
		{
			if (mVars(1).BaseType() != PDT_CODEPTR)
			{
				m_ErrorList.GeneralError("Invalid sorting type.", iLine, iPos);
				return false;
			}

			m_pSortCode = *mVars(1).GetCodePtrPtr();
		}

		m_bSortAscend = (iAscend > 0 ? true : false);
	}

	if (iVarCount > 2)
	{
		if (!mVars(2).CastToCounter(iExecute))
		{
			m_ErrorList.GeneralError("Expect 'true' or 'false' as third parameter.", iLine, iPos);
			return false;
		}

		bSortExecute = (iExecute > 0 ? true : false);
	}


	if (mVars(0).BaseType() == PDT_VARLIST)
	{
		sm_pCurCodeBase = this;
		m_pSortVarList = mVars(0).GetVarListPtr();

		int iPos, iCount = m_pSortVarList->Count();
		vecSortIdx.resize(iCount);

		for (iPos = 0; iPos < iCount; iPos++)
		{
			vecSortIdx[iPos] = iPos;	
		}

		try
		{
			/// Compare Function uses sm_pCurCodeBase to sort m_pSortVarList.
			sort(vecSortIdx.begin(), vecSortIdx.end(), StaticListSortCompare);
		}
		catch (SortError &rEx)
		{
			return false;
		}
		catch (CCLUException &rEx)
		{
			m_ErrorList.GeneralError(rEx.PrintError().c_str(), iLine, iPos);
			return false;
		}
		

		// Return value of sort is index list of sorted elements
		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rIdxList = *rVar.GetVarListPtr();

		rIdxList.Set(iCount);
		for (iPos = 0; iPos < iCount; iPos++)
		{
			CCodeVar &rEl = rIdxList[iPos];
			rEl.New(PDT_VARLIST, "Constant");
			TVarList &rSubList = *rEl.GetVarListPtr();

			rSubList.Set(1);
			rSubList[0] = vecSortIdx[iPos] + 1;	
		}

		if (bSortExecute)
			m_pSortVarList->Order(vecSortIdx);
	}
	else
	{
		m_ErrorList.GeneralError("Cannot sort variable of given type.", iLine, iPos);
		return false;
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Static Version of List Sort Comparison

bool CCLUCodeBase::StaticListSortCompare(int iLIdx, int iRIdx)
{
	if (sm_pCurCodeBase)
		return sm_pCurCodeBase->ListSortCompare(iLIdx, iRIdx);

	return false;
}

//////////////////////////////////////////////////////////////////////
/// List Sort Comparison

bool CCLUCodeBase::ListSortCompare(int iLIdx, int iRIdx)
{
	if (!m_pSortVarList)
		return false;

	bool bRes = false;
	CCodeVar &rLVar = (*m_pSortVarList)[iLIdx];
	CCodeVar &rRVar = (*m_pSortVarList)[iRIdx];

	if (m_pSortCode == 0)
	{
		bool bLisS, bRisS, bLisStr, bRisStr;
		bool bLisEl = false, bRisEl = false;
		TCVScalar dLVal, dRVal;
		char *pcLVal, *pcRVal;

		bLisS = rLVar.CastToScalar(dLVal);
		bRisS = rRVar.CastToScalar(dRVal);

		bLisStr = (rLVar.BaseType() == PDT_STRING);
		bRisStr = (rRVar.BaseType() == PDT_STRING);

		if (rLVar.BaseType() == PDT_VARLIST)
		{
			TVarList &rSubList = *rLVar.GetVarListPtr();
			if (rSubList(0).BaseType() == PDT_STRING)
			{
				bLisEl = true;
				pcLVal = rSubList(0).GetStringPtr()->Str();
			}
		}

		if (rRVar.BaseType() == PDT_VARLIST)
		{
			TVarList &rSubList = *rRVar.GetVarListPtr();
			if (rSubList(0).BaseType() == PDT_STRING)
			{
				bRisEl = true;
				pcRVal = rSubList(0).GetStringPtr()->Str();
			}
		}

		if (bLisS && bRisS)
		{
			bRes = (dLVal < dRVal);
		}
		else if (bLisStr && bRisStr)
		{
			pcLVal = (*rLVar.GetStringPtr()).Str();
			pcRVal = (*rRVar.GetStringPtr()).Str();

			bRes = (strcmp(pcLVal, pcRVal) < 0);
		}
		else if (bLisEl && bRisEl)
		{
			bRes = (strcmp(pcLVal, pcRVal) < 0);		
		}
		else if (bLisS && bRisStr)
		{
			bRes = true;
		}
		else if (bRisS && bLisStr)
		{
			bRes = false;
		}
		else if ( (bLisS || bLisStr) && !(bRisS || bRisStr) )
		{
			bRes = true;
		}
		else if ( (bRisS || bRisStr) && !(bLisS || bLisStr) )
		{
			bRes = false;
		}

		if (!m_bSortAscend)
			bRes = !bRes;
	}
	else
	{
		TCVScalar dVal;
		CCodeVar Res;
		TVarList List;

		List.Set(2);
		List(0) = &rLVar;
		List(1) = &rRVar;

		if (!ExecUserFunc(Res, m_pSortCode, List, m_iSortLine, m_iSortPos))
		{
			throw SortError();
		}

		if (!Res.CastToScalar(dVal, m_fSensitivity))
		{
			m_ErrorList.GeneralError("Invalid return value from comparison function.", 
										m_iSortLine, m_iSortPos);

			throw SortError();
		}

		if (dVal > 0)
			bRes = true;
		else
			bRes = false;
	}
	
	return bRes;
}


////////////////////////////////////////////////////////////////////////
/// Execute User Defined Function

bool CCLUCodeBase::ExecUserFunc(CCodeVar& rResVar, TCodePtr pCodePtr, TVarList &rParList,
								int iCodeLine, int iCodePos)
{
	// function call

	// Create a local var list which is added to
	// the local var list stack.
	// GetVar searches first in this stack,
	// then in ConstVarList and then in VarList.
	CCodeVarList mLocalVarList;

	// Parameters to function can be accessed via
	// the variable _P.
	int iPos = mLocalVarList.New("_P", PDT_PTR_VARLIST);

	if (iPos < 0)
	{
		m_ErrorList.Internal(iCodeLine, iCodePos);
		return false;
	}

	mLocalVarList[iPos] = &rParList;

	PushLocal(&mLocalVarList);
	SetCurrentNamespace(NS_LOCAL);

	// Dummy Var for popping LocalVarList pointer later.
	CCodeVarList *pDummy;

	// Execute code
	CCodeElementList *pCode = dynamic_cast<CCodeElementList*>(((CCodeElement *) pCodePtr));
	if (pCode)
	{
		int iTempVarCount, iCurTempVarCount;
		int iCL, iCount = pCode->Count();
		CCodeVar *pDVar;

		LockStack();

		for(iCL=0;iCL<iCount;iCL++)
		{
			// Empty stack up to lock
			while(Pop(pDVar));

			iTempVarCount = m_mTempVarList.Count();
			if (!(*pCode)[iCL]->Apply(this))
			{
				if (m_ErrorList.Last().iLevel == CEL_INTERNAL &&
					m_ErrorList.Last().iNo == CERR_BREAK)
				{
					m_ErrorList.Reset();
					break;
				}

				// Pop Local Var List
				PopLocal(pDummy);

				if (m_mLocalVarStack.Count() == 0)
					SetCurrentNamespace(NS_GLOBAL);

				// Clear Stack up to lock
				// Otherwise stack may return references to variables
				// which only exist locally within the function.
				while(Pop(pDVar));
				return false;
			}

			// Do not delete temp. vars. created in last line of function,
			// since this may be return value.
			if (iCL+1 < iCount)
			{
				iCurTempVarCount = m_mTempVarList.Count();
				m_mTempVarList.Del(iTempVarCount, iCurTempVarCount-iTempVarCount);
			}
		}

		pDVar = 0;

		// Allow only single return value
		while(Pop(pDVar));

		// If there is a return value, then dereference it if its a pointer
		if (pDVar)
		{
			rResVar = pDVar->DereferenceVarPtr(true);
			rResVar.DereferencePtr();

			/*
			if (rResVar.Type() == PDT_VARLIST)
			{
			TVarList &rResList = *rResVar.GetVarListPtr();
			int i, iCount = rResList.Count();

			for (i = 0; i < iCount; i++)
			{

			((TVarList *) rResVar.Val())->DereferencePtr();
			}
			*/
		}

		UnlockStack();
	}

	// Pop Local Var List
	PopLocal(pDummy);
	if (m_mLocalVarStack.Count() == 0)
		SetCurrentNamespace(NS_GLOBAL);

	return true;
}

