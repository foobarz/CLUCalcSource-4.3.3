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
#include "FL/glut.H"

#include "CLUCodeBase.h"



//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
/// Initialize Bitmap Font 

void CCLUCodeBase::InitBitmapFont()
{
	int i, iCnt = m_vecFont.size();
	int j, iSCnt;

	for (i = 0; i < iCnt; i++)
	{
		SFontData &rFont = m_vecFont[i];
		iSCnt = rFont.vecSize.size();
		for (j = 0; j < iSCnt; j++)
		{
			gl_font(rFont.iFont, rFont.vecSize[j]);
		}
	}
}

//////////////////////////////////////////////////////////////////////
/// Set Bitmap Font

bool CCLUCodeBase::SetBitmapFont(int iFont)
{
	if (iFont < 0 || iFont >= int(m_vecFont.size()) )
		return false;

	m_iCurBitmapFont = iFont;
	m_iCurBitmapFontSize = 0;

	return true;

/*
	if (iFont >= 0 && iFont <= COGLBitmapText::GetFontCount())
		m_iCurBitmapFont = iFont;
*/
}


//////////////////////////////////////////////////////////////////////
/// Set Bitmap Font Scale

bool CCLUCodeBase::SetBitmapFontSize(int iSize)
{
	if (m_iCurBitmapFont < 0 || m_iCurBitmapFont >= int(m_vecFont.size()) )
		return false;

	SFontData &rFont = m_vecFont[m_iCurBitmapFont];

	if (iSize < 0 || iSize >= int(rFont.vecSize.size()))
		return false;

	m_iCurBitmapFontSize = iSize;

	return true;
}



//////////////////////////////////////////////////////////////////////
/// Set Bitmap Font Function
/*
bool CCLUCodeBase::SetBitmapFontFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
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

		if (iVal < 0 || iVal > COGLBitmapText::GetFontCount())
		{
			m_ErrorList.InvalidParVal(mVars(0), 1, iLine, iPos);
			return false;
		}

		m_iCurBitmapFont = iVal;
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
} 
*/


//////////////////////////////////////////////////////////////////////
/// Set Bitmap Font Size Function

bool CCLUCodeBase::SetBitmapFontSizeFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
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

		if (!SetBitmapFontSize(iVal-1))
		{
			m_ErrorList.GeneralError("Invalid font size.", iLine, iPos);
			return false;
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
/// Draw Text Function

bool CCLUCodeBase::DrawTextFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	int iVar;
	cStr m_csOutput;
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = mVars.Count();
	TCVScalar dVal[3];
	
	if (iVarCount == 4)
	{
		for (iVar = 0; iVar < 3; iVar++)
		{
			if (!mVars(iVar).CastToScalar(dVal[iVar], m_fSensitivity))
			{
				m_ErrorList.InvalidParType(mVars(iVar), 1, iLine, iPos);
				return false;
			}								
		}
	}
	else if (iVarCount == 2)
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

			dVal[0] = vB[E3GA<TCVScalar>::iE1];
			dVal[1] = vB[E3GA<TCVScalar>::iE2]; 
			dVal[2] = vB[E3GA<TCVScalar>::iE3];
		}
		else
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}								
	}
	else
	{
		int piPar[] = {2, 4};

		m_ErrorList.WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}


	CCodeVar &rDrawVar = mVars(iVarCount - 1);

	if (rDrawVar.BaseType() == PDT_NOTYPE)
	{
		m_ErrorList.InvalidType(rDrawVar, iLine, iPos);
		return false;
	}
/*
	else if (rVar.BaseType() == PDT_MATRIX)
	{

	}
*/
	else
	{
		if (!CastToString(rDrawVar, m_csOutput))
		{
			m_ErrorList.GeneralError("Cannot print passed value.", iLine, iPos);
			return false;
		}

		glRasterPos3d(GLdouble(dVal[0]), GLdouble(dVal[1]), GLdouble(dVal[2]));
		gl_font(m_vecFont[m_iCurBitmapFont].iFont, 
			m_vecFont[m_iCurBitmapFont].vecSize[m_iCurBitmapFontSize]);
		gl_draw(m_csOutput.Str(), m_csOutput.Len());
		glListBase(0);
	}

	return true;
} 


/*
bool CCLUCodeBase::DrawTextFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	int iVar;
	cStr m_csOutput;
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	TCVScalar dVal[3];
	
	// Do not write anything
	if (m_iCurBitmapFont == 0)
		return true;

	if (iVarCount == 4)
	{
		for (iVar = 0; iVar < 3; iVar++)
		{
			if (!mVars(iVar).CastToScalar(dVal[iVar], m_fSensitivity))
			{
				m_ErrorList.InvalidParType(mVars(iVar), 1, iLine, iPos);
				return false;
			}								
		}

		CCodeVar &rVar = mVars(3);

		if (rVar.BaseType() == PDT_NOTYPE)
		{
			m_ErrorList.InvalidType(rVar, iLine, iPos);
			return false;
		}
	
		m_csOutput = rVar.ValStr();

		// Create BitmapText Class
		COGLBitmapText oglText(float(dVal[0]), float(dVal[1]), float(dVal[2]), m_csOutput.Str());
	
		oglText.SetFont(m_iCurBitmapFont-1);
		oglText.SetScale(m_fCurBitmapFontScale);
		oglText.Apply();
	} 
	// If a single multivector is given as position vector, cast it to E3
	// and use that position.
	else if (iVarCount == 2)
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

			COGLVertex xA;
			xA.Set((float) vB[E3GA<TCVScalar>::iE1], 
				(float) vB[E3GA<TCVScalar>::iE2], 
				(float) vB[E3GA<TCVScalar>::iE3]);

			CCodeVar &rVar = mVars(1);

			if (rVar.BaseType() == PDT_NOTYPE)
			{
				m_ErrorList.InvalidType(rVar, iLine, iPos);
				return false;
			}

			m_csOutput << rVar.ValStr();

			// Create BitmapText Class
			COGLBitmapText oglText(xA, m_csOutput.Str());

			oglText.SetFont(m_iCurBitmapFont-1);
			oglText.SetScale(m_fCurBitmapFontScale);
			oglText.Apply();
		}
		else
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}								

	}
	else
	{
		int piPar[] = {2, 4};

		m_ErrorList.WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}
	
	return true;
} 
*/


