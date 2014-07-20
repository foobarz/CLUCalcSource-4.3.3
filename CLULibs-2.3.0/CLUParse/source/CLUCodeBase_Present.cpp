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
/// Draw Script Files Text
void CCLUCodeBase::DrawScriptFileIndex()
{
	int iSize;
	if ((iSize = m_vecScriptList.size()) == 0 || m_iCurScript == -1)
		return;

	// Switch to overlay
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 100, 100, 0, -1, 10);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

/*
	string sText1, sText2;
	int i;

	for ( i = 0; i < m_iCurScript; i++ )
	{
		sText1 += m_vecScriptList[i].sTitle;
		if (i < iSize-1)
			sText1 += " - ";
	}

	for ( ; i < iSize; i++ )
	{
		sText2 += m_vecScriptList[i].sTitle;
		if (i < iSize-1)
			sText2 += " - ";
	}



*/	
	glColor3f(1, 0, 0);
	
	float fMagStep = float(pow(1.2, double(10) * 0.5));
	if (m_oglLatex.Add(m_vecScriptList[m_iCurScript].sTitle.c_str(), 50, 0, 0, 
						fMagStep, 1.0f, 0.5f))
	{
		m_oglLatex.Apply(0, (void *)m_vecScriptList[m_iCurScript].sTitle.c_str());
	}


	// return from overlay
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}


//////////////////////////////////////////////////////////////////////
/// Clear Script List
void CCLUCodeBase::ClearScriptList()
{
	m_vecScriptList.clear();
	m_iCurScript = -1;
}

//////////////////////////////////////////////////////////////////////
/// Add Script File
bool CCLUCodeBase::AddScriptToList(string sTitle, string sFilename)
{
	SScriptFile Script;

	if (!sTitle.size() || !sFilename.size())
		return false;

	Script.sTitle = sTitle;
	Script.sFilename = sFilename;

	m_vecScriptList.push_back(Script);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Next Script File
int CCLUCodeBase::NextScriptFile()
{
	int iSize;

	if ((iSize = m_vecScriptList.size()) == 0)
		return -1;

	if (m_iCurScript == -1)
		m_iCurScript = 0;
	else if (m_iCurScript+1 < iSize)
		m_iCurScript++;

	return m_iCurScript;
}

//////////////////////////////////////////////////////////////////////
/// Previous Script File
int CCLUCodeBase::PrevScriptFile()
{
	int iSize;

	if ((iSize = m_vecScriptList.size()) == 0)
		return -1;

	if (m_iCurScript == -1)
		m_iCurScript = 0;
	else if (m_iCurScript-1 >= 0)
		m_iCurScript--;

	return m_iCurScript;
}

//////////////////////////////////////////////////////////////////////
/// Set Current Script
bool CCLUCodeBase::SetCurScriptIdx(int iVal)
{
	int iSize;

	if ((iSize = m_vecScriptList.size()) == 0)
		return false;

	if (iVal < 0  || iVal >= iSize)
		return false;

	m_iCurScript = iVal;
	return true;
}



//////////////////////////////////////////////////////////////////////
/// Get Script Filename
string CCLUCodeBase::GetCurScriptFilename()
{
	string sFilename;

	if (m_iCurScript == -1)
		return sFilename;
	else
		return m_vecScriptList[m_iCurScript].sFilename;
}

//////////////////////////////////////////////////////////////////////
/// Get Script Title
string CCLUCodeBase::GetCurScriptTitle()
{
	string sTitle;

	if (m_iCurScript == -1)
		return sTitle;
	else
		return m_vecScriptList[m_iCurScript].sTitle;
}

//////////////////////////////////////////////////////////////////////
/// Clear Script List

bool CCLUCodeBase::ClearScriptListFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();

	if (iVarCount != 0)
	{
		m_ErrorList.WrongNoOfParams(0, iLine, iPos);
		return false;
	}

	ClearScriptList();

	return true;
} 



//////////////////////////////////////////////////////////////////////
/// Add Script To List

bool CCLUCodeBase::AddScriptToListFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	
	if (iVarCount == 2)
	{
		CCodeVar &rVar1 = mVars(0);
		if (rVar1.BaseType() != PDT_STRING)
		{
			m_ErrorList.InvalidParVal(rVar1, 1, iLine, iPos);
			return false;
		}
		CCodeVar &rVar2 = mVars(1);
		if (rVar2.BaseType() != PDT_STRING)
		{
			m_ErrorList.InvalidParVal(rVar2, 2, iLine, iPos);
			return false;
		}
	
		string sFilename, sTitle;
		sFilename = m_sScriptPath + rVar2.ValStr().Str();
		sTitle = rVar1.ValStr().Str();

		AddScriptToList(sTitle, sFilename);
	} 
	else
	{
		m_ErrorList.WrongNoOfParams(2, iLine, iPos);
		return false;
	}
	
	return true;
} 



