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
/// Functions relating to the Info Window



//////////////////////////////////////////////////////////////////////
/// Set Info Text

bool CCLUCodeBase::SetInfoTextFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();

	string sText;

	if (iVarCount != 1)
	{
		int piParNo[] = { 1 };
		m_ErrorList.WrongNoOfParams(piParNo, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	sText = mVars(0).ValStr();
		
	if ( !m_pCLUDrawBase->SetInfoText(sText.c_str()) )
	{
		m_ErrorList.GeneralError("Cannot set info text.", iLine, iPos);
		return false;
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Set Info Width

bool CCLUCodeBase::SetInfoWidthFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();

	TCVScalar dVal;
	string sText;

	if (iVarCount != 1)
	{
		int piParNo[] = { 1 };
		m_ErrorList.WrongNoOfParams(piParNo, 1, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToScalar(dVal, m_fSensitivity))
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (dVal < 0)
	{
		m_ErrorList.GeneralError("Width of information window has to be greater than zero.", iLine, iPos);
		return false;
	}

	if (dVal != m_dCurrentInfoWidth)
	{
		if ( !m_pCLUDrawBase->SetInfoWidth(dVal) )
		{
			m_ErrorList.GeneralError("Cannot set width of information window.", iLine, iPos);
			return false;
		}

		m_dCurrentInfoWidth = dVal;
	}

	return true;
}

