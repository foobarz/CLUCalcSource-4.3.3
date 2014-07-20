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

// OCOpOpenCurlyBracket.cpp: Implementierung der Klasse COCOpOpenCurlyBracket.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OCOpOpenCurlyBracket.h"

#include "ParseBase.h"
#include "CodeVar.h"
#include "CodeData.h"
#include "CodeElementList.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COCOpOpenCurlyBracket::COCOpOpenCurlyBracket()
{
	m_csIDSymbolSpace = "{";
}

COCOpOpenCurlyBracket::~COCOpOpenCurlyBracket()
{

}



//////////////////////////////////////////////////////////////////////
/// Parse Loop

bool COCOpOpenCurlyBracket::Parse(CParseBase& rParse, CCodeElementList& rCode)
{
	CCodeElementList *pMainList, *pElList;

	bool bReparse = false;
	bool bCloseBracket = false;
	int iLine, iPos;
	rParse.GetCurLinePos(iLine, iPos);

	if (!(pMainList = new CCodeElementList))
	{
		rParse.SetError(CPB_OUTOFMEMORY);
		return false;
	}

	rParse.AddToCurPos(1);
	iPos++;

	// Reset Operator level
	rParse.PushOpLevel(-1);

	while (true)
	{
		bReparse = false;

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
		
		if (!rParse.BasicParseLoop(pElList))
		{
			// If a closed bracket was encountered then 
			// end code list.
			if (rParse.GetError() == CPB_CLOSEDCB)
			{
				rParse.ClearError();
				rParse.GetCurLinePos(iLine, iPos);
				bCloseBracket = true;
				break;
			}
			else if (rParse.GetError() == CPB_REPARSE)
			{
				bReparse = true;
			}
			// If end of line was encountered then add next 
			// code element list.
			// Otherwise an error occured. Return false!
			else if (rParse.GetError() != CPB_EOL)
			{
				return false;
			}
		}
		else break;

		rParse.ClearError();
		if (!bReparse && !rParse.NextLine())
			break;
		rParse.GetCurLinePos(iLine, iPos);
	}

	// Recall previous operator level
	rParse.PopOpLevel();

	if (!bCloseBracket)
	{
		rParse.SetError(CPB_MISSINGCCB);
		return false;
	}


	// Create Label with pointer to CodeElementList pMainList
	CCodeData *pData;
	cStr csElement;

	csElement << "__Code" << rParse.IncrCEL();

	if (!(pData = new CCodeData))
	{
		rParse.SetError(CPB_OUTOFMEMORY);
		return false;
	}

	pData->SetName(csElement);

	// Set data to main list
	CCodeVar &rVar = pData->RefData();

	rVar = (CCodeElement *) pMainList;
	rVar.Name() = "Constant";
	rVar.EnableProtect();

	if (!rCode.Add(*pData, true, iLine, iPos))
	{
		rParse.SetError(CPB_OUTOFMEMORY);
		delete pMainList;
		return false;
	}

	return true;
}


