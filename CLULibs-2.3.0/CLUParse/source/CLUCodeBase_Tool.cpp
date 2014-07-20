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
/// Functions relating to tools


//////////////////////////////////////////////////////////////////////
/// Add a slider tool

bool CCLUCodeBase::ToolSliderFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();

	int i;
	string sName, sToolTip;
	const char *pcToolTip = 0;
	TCVScalar dVal[4];

	if (iVarCount != 5 && iVarCount != 6 && iVarCount != 1)
	{
		int piParNo[] = { 1, 5, 6 };
		m_ErrorList.WrongNoOfParams(piParNo, 3, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	sName = mVars(0).ValStr();
		
	if (iVarCount >= 5)
	{
		for(i = 0; i < 4; i++)
		{
			if ( !(mVars(i+1).CastToScalar(dVal[i], m_fSensitivity)) )
			{
				m_ErrorList.InvalidParType(mVars(i+1), i+2, iLine, iPos);
				return false;
			}
		}
	
		if (iVarCount >= 6)
		{
			if (mVars(5).BaseType() != PDT_STRING)
			{
				m_ErrorList.InvalidParType(mVars(5), 6, iLine, iPos);
				return false;
			}
			
			sToolTip = mVars(5).ValStr();
			pcToolTip = sToolTip.c_str();
		}

		if (!m_pCLUDrawBase->AddToolSlider(sName.c_str(), dVal[0], dVal[1], dVal[2], dVal[3], false, pcToolTip))
		{
			m_ErrorList.GeneralError("Cannot create slider", iLine, iPos);
			return false;
		}
	}

	double dValue;
	m_pCLUDrawBase->GetToolValue(sName.c_str(), dValue);

	rVar = TCVScalar(dValue);

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Add a counter tool

bool CCLUCodeBase::ToolCounterFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	
	int iVarCount = mVars.Count();

	int i;
	string sName, sToolTip;
	const char *pcToolTip = 0;
	TCVScalar dVal[5];

	if (iVarCount != 6 && iVarCount != 7 && iVarCount != 1)
	{
		int piParNo[] = { 1, 6, 7 };
		m_ErrorList.WrongNoOfParams(piParNo, 3, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	sName = mVars(0).ValStr();
		
	if (iVarCount >= 6)
	{
		for(i = 0; i < 5; i++)
		{
			if ( !(mVars(i+1).CastToScalar(dVal[i], m_fSensitivity)) )
			{
				m_ErrorList.InvalidParType(mVars(i+1), i+2, iLine, iPos);
				return false;
			}
		}
	
		if (iVarCount >= 7)
		{
			if (mVars(6).BaseType() != PDT_STRING)
			{
				m_ErrorList.InvalidParType(mVars(6), 7, iLine, iPos);
				return false;
			}

			sToolTip = mVars(6).ValStr();
			pcToolTip = sToolTip.c_str();
		}

		if (!m_pCLUDrawBase->AddToolCounter(sName.c_str(), dVal[0], dVal[1], dVal[2], dVal[3], dVal[4], false, pcToolTip))
		{
			m_ErrorList.GeneralError("Cannot create stepper.", iLine, iPos);
			return false;
		}
	}

	double dValue;
	m_pCLUDrawBase->GetToolValue(sName.c_str(), dValue);

	rVar = TCVScalar(dValue);

	return true;
}




//////////////////////////////////////////////////////////////////////
/// Add a input tool

bool CCLUCodeBase::ToolInputFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();

	int i;
	string sName, sToolTip;
	const char *pcToolTip = 0;
	TCVScalar dVal[3];

	if (iVarCount != 4 && iVarCount != 5 && iVarCount != 1)
	{
		int piParNo[] = { 1, 4, 5 };
		m_ErrorList.WrongNoOfParams(piParNo, 3, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	sName = mVars(0).ValStr();
		
	if (iVarCount >= 4)
	{
		for(i = 0; i < 3; i++)
		{
			if ( !(mVars(i+1).CastToScalar(dVal[i], m_fSensitivity)) )
			{
				m_ErrorList.InvalidParType(mVars(i+1), i+2, iLine, iPos);
				return false;
			}
		}
	
		if (iVarCount >= 5)
		{
			if (mVars(4).BaseType() != PDT_STRING)
			{
				m_ErrorList.InvalidParType(mVars(4), 5, iLine, iPos);
				return false;
			}
			
			sToolTip = mVars(4).ValStr();
			pcToolTip = sToolTip.c_str();
		}

		if (!m_pCLUDrawBase->AddToolInput(sName.c_str(), dVal[0], dVal[1], dVal[2], false, pcToolTip))
		{
			m_ErrorList.GeneralError("Cannot create input", iLine, iPos);
			return false;
		}
	}

	double dValue;
	m_pCLUDrawBase->GetToolValue(sName.c_str(), dValue);

	rVar = TCVScalar(dValue);

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Add a input text tool

bool CCLUCodeBase::ToolInputTextFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();

	int i;
	string sName, sToolTip;
	const char *pcToolTip = 0;

	string sVal;
	TCVScalar dVal;

	if (iVarCount != 3 && iVarCount != 4 && iVarCount != 1)
	{
		int piParNo[] = { 1, 3, 4 };
		m_ErrorList.WrongNoOfParams(piParNo, 3, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	sName = mVars(0).ValStr();
		
	if (iVarCount >= 3)
	{
		if (mVars(2).BaseType() != PDT_STRING)
		{
			m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}

		sVal = mVars(2).ValStr();

		if ( !(mVars(1).CastToScalar(dVal, m_fSensitivity)) )
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}
	
		if (iVarCount >= 4)
		{
			if (mVars(3).BaseType() != PDT_STRING)
			{
				m_ErrorList.InvalidParType(mVars(3), 4, iLine, iPos);
				return false;
			}
			
			sToolTip = mVars(3).ValStr();
			pcToolTip = sToolTip.c_str();
		}

		if (!m_pCLUDrawBase->AddToolInputText(sName.c_str(), (dVal ? true : false), sVal.c_str(), false, pcToolTip))
		{
			m_ErrorList.GeneralError("Cannot create text input.", iLine, iPos);
			return false;
		}
	}

	m_pCLUDrawBase->GetToolValue(sName.c_str(), sVal);

	rVar = sVal.c_str();

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add a check box tool

bool CCLUCodeBase::ToolCheckBoxFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();

	string sName, sToolTip;
	const char *pcToolTip = 0;
	TCVScalar dVal;

	if (iVarCount != 2 && iVarCount != 3 && iVarCount != 1)
	{
		int piParNo[] = { 1, 2, 3 };
		m_ErrorList.WrongNoOfParams(piParNo, 3, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	sName = mVars(0).ValStr();
		
	if (iVarCount >= 2)
	{
		if ( !(mVars(1).CastToScalar(dVal, m_fSensitivity)) )
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}
	
		if (iVarCount >= 3)
		{
			if (mVars(2).BaseType() != PDT_STRING)
			{
				m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
				return false;
			}
			
			sToolTip = mVars(2).ValStr();
			pcToolTip = sToolTip.c_str();
		}

		if (!m_pCLUDrawBase->AddToolCheckBox(sName.c_str(), (dVal != TCVScalar(0) ? true : false), false, pcToolTip))
		{
			m_ErrorList.GeneralError("Cannot create check box.", iLine, iPos);
			return false;
		}
	}

	double dValue;
	m_pCLUDrawBase->GetToolValue(sName.c_str(), dValue);

	rVar = TCVScalar(dValue);

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Add a button tool

bool CCLUCodeBase::ToolButtonFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();

	string sName, sToolTip;
	const char *pcToolTip = 0;

	if (iVarCount != 1 && iVarCount != 2)
	{
		int piParNo[] = { 1, 2 };
		m_ErrorList.WrongNoOfParams(piParNo, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	sName = mVars(0).ValStr();
		
	if (iVarCount >= 2)
	{
		if (mVars(1).BaseType() != PDT_STRING)
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		sToolTip = mVars(1).ValStr();
		pcToolTip = sToolTip.c_str();
	}

	if (!m_pCLUDrawBase->AddToolButton(sName.c_str(), false, pcToolTip))
	{
		m_ErrorList.GeneralError("Cannot create button.", iLine, iPos);
		return false;
	}

	double dValue;
	m_pCLUDrawBase->GetToolValue(sName.c_str(), dValue);

	rVar = TCVScalar(dValue);

	return true;
}



//////////////////////////////////////////////////////////////////////
/// Add a Choice tool

bool CCLUCodeBase::ToolChoiceFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	
	int iVarCount = mVars.Count();

	string sName, sToolTip;
	const char *pcToolTip = 0;
	TCVCounter iVal;

	if (iVarCount != 3 && iVarCount != 4 && iVarCount != 1)
	{
		int piParNo[] = { 1, 3, 4 };
		m_ErrorList.WrongNoOfParams(piParNo, 3, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	sName = mVars(0).ValStr();
		
	if (iVarCount >= 3)
	{
		if (mVars(1).BaseType() != PDT_VARLIST)
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		TVarList &rData = *mVars(1).GetVarListPtr();
		TString csChoice;

		if ( mVars(2).CastToCounter(iVal) )
		{
			if (iVal > 0 && iVal <= int(rData.Count()) )
				csChoice = rData[iVal-1].ValStr();
			else
			{
				m_ErrorList.GeneralError("Index value is out of range.", iLine, iPos);
				return false;
			}
		}
		else if (mVars(2).BaseType() == PDT_STRING)
		{
			csChoice = *mVars(2).GetStringPtr();
		}
		else
		{
			m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}

		vector<string> vecData;
		int i, iCount = rData.Count();
		vecData.resize(iCount);

		for ( i = 0; i < iCount; i++ )
		{
			vecData[i] = rData[i].ValStr().Str();
		}

		if (iVarCount >= 4)
		{
			if (mVars(3).BaseType() != PDT_STRING)
			{
				m_ErrorList.InvalidParType(mVars(3), 4, iLine, iPos);
				return false;
			}
			
			sToolTip = mVars(3).ValStr();
			pcToolTip = sToolTip.c_str();
		}

		if (!m_pCLUDrawBase->AddToolChoice(sName.c_str(), vecData, csChoice.Str(), false, pcToolTip))
		{
			m_ErrorList.GeneralError("Cannot create choice tool.", iLine, iPos);
			return false;
		}
	}

	double dValue;
	m_pCLUDrawBase->GetToolValue(sName.c_str(), dValue);

	rVar = TCVScalar(dValue);

	return true;
}

