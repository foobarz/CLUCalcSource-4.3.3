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

// OCOpOpenCurlyBracket.cpp: Implementierung der Klasse COCKeywordLoop.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OCKeywordLoop.h"

#include "ParseBase.h"
#include "CodeVar.h"
#include "CodeData.h"
#include "CodeLoop.h"
#include "CodeElementList.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COCKeywordLoop::COCKeywordLoop()
{
	m_csIDSymbolSpace = OC_IDSYM_LOOP_STR;
}

COCKeywordLoop::~COCKeywordLoop()
{

}



//////////////////////////////////////////////////////////////////////
/// Parse Loop

bool COCKeywordLoop::Parse(CParseBase& rParse, CCodeElementList& rCode)
{
	CCodeElementList *pMainList, *pLoopBranch;
	bool bFileEnd = false;
	bool bReparse = false;

	int iLine, iPos;
	CCodeData *pData;
	CCodeLoop *pLoopCode;
	cStr csElement;

	rParse.GetCurLinePos(iLine, iPos);

	if (!(pMainList = new CCodeElementList))
	{
		rParse.SetError(CPB_OUTOFMEMORY);
		return false;
	}

	rParse.AddToCurPos(2);
	iPos += 2;

	// Reset Operator level
	rParse.PushOpLevel(-1);

	// Parse LOOP branch (has to exist)
	if (!(pLoopBranch = new CCodeElementList))
	{
		rParse.SetError(CPB_OUTOFMEMORY);
		delete pMainList;
		return false;
	}

	// Reset Operator level
	rParse.PushOpLevel(-1);

	bReparse = false;
	if (!rParse.BasicParseLoop(pLoopBranch))
	{
		if (rParse.GetError() == CPB_REPARSE)
		{
			bReparse = true;
		}
		else if (rParse.GetError() != CPB_EOL)
		{
			delete pMainList;
			return false;
		}
	}

	rParse.PopOpLevel();

	// Create Label with pointer to CodeElementList pLoopBranch
	csElement << "__Code" << rParse.IncrCEL();

	if (!(pData = new CCodeData))
	{
		rParse.SetError(CPB_OUTOFMEMORY);
		delete pMainList;
		return false;
	}

	pData->SetName(csElement);

	// Set data to main list
	CCodeVar &rTrueVar = pData->RefData();

	rTrueVar = (CCodeElement *) pLoopBranch;
	rTrueVar.Name() = "LoopBranch";
	rTrueVar.EnableProtect();

	if (!pMainList->Add(*pData, true, iLine, iPos))
	{
		rParse.SetError(CPB_OUTOFMEMORY);
		delete pMainList;
		return false;
	}

	// Clear Errors
	rParse.ClearError();

	// If previous parse loop ended with CPB_REPARSE
	// then do not advance to next line
	/*
	if (!bReparse)
	{
		rParse.NextLine();
	}
	*/
	rParse.GetCurLinePos(iLine, iPos);

	// Now add code to execute loop statement
	if (!(pLoopCode = new CCodeLoop))
	{
		rParse.SetError(CPB_OUTOFMEMORY);
		delete pMainList;
		return false;
	}

	if (!pMainList->Add(*pLoopCode, true, iLine, iPos))
	{
		rParse.SetError(CPB_OUTOFMEMORY);
		delete pMainList;
		return false;
	}

	if (!rCode.Add(*pMainList, true, iLine, iPos))
	{
		rParse.SetError(CPB_OUTOFMEMORY);
		delete pMainList;
		return false;
	}

	// Recall previous operator level
	rParse.PopOpLevel();
	
	if (!bReparse)
		rParse.SetError(CPB_EOL);
	else
		rParse.SetError(CPB_REPARSE);

	return false;
}


