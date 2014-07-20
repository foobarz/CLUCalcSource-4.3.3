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

// OCOpOpenCurlyBracket.cpp: Implementierung der Klasse COCKeywordIf.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OCKeywordIf.h"

#include "ParseBase.h"
#include "CodeVar.h"
#include "CodeData.h"
#include "CodeIf.h"
#include "CodeElementList.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COCKeywordIf::COCKeywordIf()
{
	m_csIDSymbolSpace = OC_IDSYM_IF_STR;
}

COCKeywordIf::~COCKeywordIf()
{

}



//////////////////////////////////////////////////////////////////////
/// Parse Loop

bool COCKeywordIf::Parse(CParseBase& rParse, CCodeElementList& rCode)
{
	CCodeElementList *pMainList, *pElList, *pTrueBranch, *pFalseBranch;

	bool bElse = false;
	bool bElseIsLastLine = false;
	bool bFileEndBeforeElseBranch = false;
	bool bReparse = false;

	int iLine, iPos;
	CCodeData *pData;
	CCodeIf *pIfCode;
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

	if (!(pElList = new CCodeElementList))
	{
		rParse.SetError(CPB_OUTOFMEMORY);
		return false;
	}

	if (!pMainList->Add(*pElList, true, iLine, iPos))
	{
		rParse.SetError(CPB_OUTOFMEMORY);
		delete pElList;
		return false;
	}

	// Parse IF condition code
	if (!rParse.BasicParseLoop(pElList))
	{
		// If THEN was not encountered then 
		// stop with error.
		if (rParse.GetError() != CPB_THEN)
		{
			//rParse.SetError(CPB_MISSING_IFCOND);
			delete pMainList;
			return false;
		}
	}
	else
	{
		// Parse finished without encountering THEN
		rParse.SetError(CPB_MISSING_THEN);
		delete pMainList;
		return false;
	}

	rParse.ClearError();
	rParse.GetCurLinePos(iLine, iPos);

	// Parse TRUE branch (has to exist)
	if (!(pTrueBranch = new CCodeElementList))
	{
		rParse.SetError(CPB_OUTOFMEMORY);
		delete pMainList;
		return false;
	}

	// Reset Operator level
	rParse.PushOpLevel(-1);

	bReparse = false;
	if (!rParse.BasicParseLoop(pTrueBranch))
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

	// Create Label with pointer to CodeElementList pTrueBranch
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

	rTrueVar = (CCodeElement *) pTrueBranch;
	rTrueVar.Name() = "TrueBranch";
	rTrueVar.EnableProtect();

	if (!pMainList->Add(*pData, true, iLine, iPos))
	{
		rParse.SetError(CPB_OUTOFMEMORY);
		delete pMainList;
		return false;
	}

	// Go to next line
	rParse.ClearError();

	// If previous parse loop ended with CPB_REPARSE
	// then do not advance to next line
	if (!bReparse)
	{
		if (!rParse.NextLine())
			bFileEndBeforeElseBranch = true;
		else
			bFileEndBeforeElseBranch = false;
	}
	rParse.GetCurLinePos(iLine, iPos);

	// If file has ended before an else branch could be encountered
	// do not look for the else branch.
	if (!bFileEndBeforeElseBranch)
	{
		CObjectClass *pOC;

		do
		{
			// Peek at next object class
			pOC = rParse.GetNextObjectClass(-1, true);
			if (pOC && pOC->m_csIDSymbolSpace == " \t")
			{
				// Skip separator
				rParse.AddToCurPos(1);
			}
			else
			{
				break;
			}
		} while (true);

		// Check wether next object class is ELSE keyword
		if (pOC && pOC->m_csIDSymbolSpace == OC_IDSYM_ELSE_STR)
		{
			// Walk over ELSE Keyword ID
			rParse.AddToCurPos(2);

			// Parse FALSE branch (has to exist)
			if (!(pFalseBranch = new CCodeElementList))
			{
				rParse.SetError(CPB_OUTOFMEMORY);
				delete pMainList;
				return false;
			}

			// Reset Operator level
			rParse.PushOpLevel(-1);

			bReparse = false;
			if (!rParse.BasicParseLoop(pFalseBranch))
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

			// Create Label with pointer to CodeElementList pFalseBranch
			csElement << "__Code" << rParse.IncrCEL();

			if (!(pData = new CCodeData))
			{
				rParse.SetError(CPB_OUTOFMEMORY);
				delete pMainList;
				return false;
			}

			pData->SetName(csElement);

			// Set data to main list
			CCodeVar &rFalseVar = pData->RefData();

			rFalseVar = (CCodeElement *) pFalseBranch;
			rFalseVar.Name() = "FalseBranch";
			rFalseVar.EnableProtect();

			if (!pMainList->Add(*pData, true, iLine, iPos))
			{
				rParse.SetError(CPB_OUTOFMEMORY);
				delete pMainList;
				return false;
			}

			// Go to next line
			rParse.ClearError();
			if (!bReparse)
			{
				if (!rParse.NextLine())
					bElseIsLastLine = true;
				else
					bElseIsLastLine = false;
			}
			rParse.GetCurLinePos(iLine, iPos);

		} // if bElse
	} // if bFileEndBeforeElseBranch == false

	// Now add code to execute if statement
	if (!(pIfCode = new CCodeIf))
	{
		rParse.SetError(CPB_OUTOFMEMORY);
		delete pMainList;
		return false;
	}

	if (!pMainList->Add(*pIfCode, true, iLine, iPos))
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

	// Set Error to REPARSE
	// Since IF-THEN-ELSE construct is a single line.
	// External parse loop should not go to next line.
	// Current line is next line.
	if (bElseIsLastLine || bFileEndBeforeElseBranch)
		rParse.SetError(CPB_EOL);
	else
		rParse.SetError(CPB_REPARSE);

	return false;
}


