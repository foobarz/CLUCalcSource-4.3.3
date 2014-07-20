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

#include "CLUCodeBase.h"


//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
/// Set Horizontal and Vertical Align Function for Latex Text

bool CCLUCodeBase::SetLatexAlignFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	TCVScalar dHVal, dVVal;
	
	if (iVarCount == 2)
	{
		if (!mVars(0).CastToScalar(dHVal, m_fSensitivity))
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}								

		if (!mVars(1).CastToScalar(dVVal, m_fSensitivity))
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}								

		m_fHLatexAlign = float(dHVal);
		m_fVLatexAlign = float(dVVal);
	}
	else
	{
		m_ErrorList.WrongNoOfParams(2, iLine, iPos);
		return false;
	}
	
	return true;
} 




//////////////////////////////////////////////////////////////////////
/// The Latex String FUNCTION
/// Converts variables to latex strings

bool CCLUCodeBase::LatexStringFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	
	if (iVarCount == 1)
	{
		cStr csVal;

		CastToLatexString(mVars(0), csVal);
		rVar = csVal;
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}




//////////////////////////////////////////////////////////////////////
/// Set Latex Mag Step Function

bool CCLUCodeBase::SetLatexMagStepFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
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

		if (iVal <= -51 || iVal >= 51)
		{
			m_ErrorList.InvalidParVal(mVars(0), 1, iLine, iPos);
			return false;
		}

		m_iLatexMagStep = iVal;
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
} 

//////////////////////////////////////////////////////////////////////
/// Set Render Latex
void CCLUCodeBase::SetRenderLatex(bool bVal)
{
	if (m_bOverrideSetRenderLatex)
		return;

	if (bVal)
		m_oglLatex.EnableRenderLatex(true);
	else
		m_oglLatex.EnableRenderLatex(false);
}

//////////////////////////////////////////////////////////////////////
/// Set Render Latex Function

bool CCLUCodeBase::SetRenderLatexFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
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

		SetRenderLatex((iVal ? true : false));
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
} 


//////////////////////////////////////////////////////////////////////
/// Draw Latex Function

bool CCLUCodeBase::DrawLatexFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	int iVar;
	cStr csText, csBMPName;
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	float fMagStep;
	TCVScalar dVal[3];
	COGLVertex xA;
	
	if (iVarCount == 4 || iVarCount == 5)
	{
		for (iVar = 0; iVar < 3; iVar++)
		{
			if (!mVars(iVar).CastToScalar(dVal[iVar], m_fSensitivity))
			{
				m_ErrorList.InvalidParType(mVars(iVar), iVar+1, iLine, iPos);
				return false;
			}	
		}
		xA.Set(float(dVal[0]), float(dVal[1]), float(dVal[2]));

		CCodeVar &rVar = mVars(3);
		if (rVar.BaseType() == PDT_NOTYPE)
		{
			m_ErrorList.InvalidType(rVar, iLine, iPos);
			return false;
		}
		csText = rVar.ValStr();

		if (iVarCount == 5)
		{
			CCodeVar &rVar = mVars(4);
			if (rVar.BaseType() != PDT_STRING)
			{
				m_ErrorList.InvalidType(rVar, iLine, iPos);
				return false;
			}
			csBMPName = rVar.ValStr();
		}
	} 
	// If a single multivector is given as position vector, cast it to E3
	// and use that position.
	else if (iVarCount == 2 || iVarCount == 3)
	{
		if (mVars(0).BaseType() == PDT_MULTIV)
		{
			TMultiV &vA = *mVars(0).GetMultiVPtr();
			TMultiV vB;
			if (!CastMVtoE3(vA, vB))
			{
				m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
				return false;
			}								

			xA.Set((float) vB[E3GA<TCVScalar>::iE1], 
				(float) vB[E3GA<TCVScalar>::iE2], 
				(float) vB[E3GA<TCVScalar>::iE3]);
		}
		else if (mVars(0).BaseType() == PDT_VARLIST)
		{
			TVarList &rList = *mVars(0).GetVarListPtr();

			if (rList.Count() != 6)
			{
				m_ErrorList.InvalidParVal(mVars(0), 1, iLine, iPos);
				return false;
			}

			int i;
			TCVScalar pdVal[6];
			for (i = 0; i < 6; i++)
			{
				if (!(rList[i].CastToScalar(pdVal[i], m_fSensitivity)))
				{
					m_ErrorList.InvalidParVal(mVars(0), 1, iLine, iPos);
					return false;
				}
			}

			xA.Set( float(pdVal[0]), 
					float(pdVal[1] + 0.5*(pdVal[1]-pdVal[4])), 
				    float(pdVal[2]) );
		}
		else
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}								

		CCodeVar &rVar = mVars(1);

		if (rVar.BaseType() == PDT_NOTYPE)
		{
			m_ErrorList.InvalidType(rVar, iLine, iPos);
			return false;
		}
		csText << rVar.ValStr();

		if (iVarCount == 3)
		{
			CCodeVar &rVar = mVars(2);
			if (rVar.BaseType() != PDT_STRING)
			{
				m_ErrorList.InvalidType(rVar, iLine, iPos);
				return false;
			}
			csBMPName = rVar.ValStr();
		}
	}
	else
	{
		int piPar[] = {2, 4};

		m_ErrorList.WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	fMagStep = m_oglLatex.GetMagStepScale(m_iLatexMagStep);
	if (!m_oglLatex.Add(csText, xA, fMagStep, m_fVLatexAlign, m_fHLatexAlign,
						(csBMPName.Len() > 0 ? csBMPName.Str() : 0) ))
	{
		string sError;
		sError = "Error: Cannot render Latex text.\n\nLatex Errors:\n";
		sError += m_oglLatex.GetRenderErrorText();
		sError += "\n";
		m_ErrorList.AddMsg(sError.c_str(), iLine, iPos, CERR_INTERNAL, CEL_ERROR);
		return false;
	}

	m_oglLatex.Apply(0, csText.Str());
	
	float fL, fR, fB, fT, fZ1, fZ2;
	if (m_oglLatex.GetWorldCoordDims(csText.Str(), fL, fR, fB, fT, fZ1, fZ2))
	{
		TVarList mRet;
		mRet.Set(6);
		mRet[0] = fL;
		mRet[1] = fB;
		mRet[2] = fZ1;
		mRet[3] = fR;
		mRet[4] = fT;
		mRet[5] = fZ2;

		rVar = mRet;
	}

	return true;
} 


//////////////////////////////////////////////////////////////////////
/// Get Latex Image Function

bool CCLUCodeBase::GetLatexImageFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	int iVar;
	cStr csText, csBMPName;
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = mVars.Count();

	if (iVarCount == 2)
	{
		if (mVars(0).BaseType() == PDT_STRING &&
			mVars(1).BaseType() == PDT_STRING)
		{
			// Render Latex and use generated Bitmap as texture
			TString csText, csFile;
			float fMagStep;
			COGLVertex xA;

			csText = mVars(0).ValStr();
			csFile = mVars(1).ValStr();
			xA.Set(0,0,0);

			if (csText.Len() == 0)
			{
				m_ErrorList.GeneralError("Empty string cannot be rendered.", iLine, iPos);
				return false;
			}

			if (csFile.Len() == 0)
			{
				m_ErrorList.GeneralError("Name for LaTeX text has to be given.", iLine, iPos);
				return false;
			}

			fMagStep = m_oglLatex.GetMagStepScale(m_iLatexMagStep);
			if (!m_oglLatex.Add(csText, xA, fMagStep, m_fVLatexAlign, m_fHLatexAlign,
								csFile.Str() ))
			{
				string sError;
				sError = "Error: Cannot render Latex text.\n\nLatex Errors:\n";
				sError += m_oglLatex.GetRenderErrorText();
				sError += "\n";
				m_ErrorList.AddMsg(sError.c_str(), iLine, iPos, CERR_INTERNAL, CEL_ERROR);
				return false;
			}

			COGLBitmap *pImage = m_oglLatex.GetBitmap(csText.Str());
			if (!pImage)
			{
				m_ErrorList.GeneralError("Internal Error: Latex bitmap not available.", iLine, iPos);
				return false;
			}

			rVar.New(PDT_IMAGE, "Constant");
			TImage &rImg = *rVar.GetImagePtr();

			COGLColor rCurCol;
			glGetFloatv(GL_CURRENT_COLOR, rCurCol.Data());
			//pImage->FlushRGB(rCurCol);

			// Copy Image
			*((COGLBitmap *) rImg) = *pImage;
			rImg->FlushRGB(rCurCol);
		}
	}
	else
	{
		int piPar[] = {2 };

		m_ErrorList.WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	return true;
}

