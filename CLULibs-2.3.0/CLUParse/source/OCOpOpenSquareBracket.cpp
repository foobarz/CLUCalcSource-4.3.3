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

// OCOpOpenSquareBracket.cpp: Implementierung der Klasse COCOpOpenSquareBracket.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OCOpOpenSquareBracket.h"
#include "ParseBase.h"
#include "CodeElementList.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COCOpOpenSquareBracket::COCOpOpenSquareBracket()
{
	m_csIDSymbolSpace = "[";
}

COCOpOpenSquareBracket::~COCOpOpenSquareBracket()
{

}


//////////////////////////////////////////////////////////////////////
/// Parse Loop

bool COCOpOpenSquareBracket::Parse(CParseBase& rParse, CCodeElementList& rCode)
{
	rParse.IncrSBC();

	CCodeElementList *pMainList, *pElList;

	bool bCloseBracket = false;
	int iLine, iPos;
	rParse.GetCurLinePos(iLine, iPos);

	if (!(pMainList = new CCodeElementList))
	{
		rParse.SetError(CPB_OUTOFMEMORY);
		return false;
	}

	if (!rCode.Add(*pMainList, true, iLine, iPos))
	{
		rParse.SetError(CPB_OUTOFMEMORY);
		delete pMainList;
		return false;
	}

	rParse.AddToCurPos(1);
	iPos++;

	// Reset Operator level
	rParse.PushOpLevel(-1);

	while (true)
	{
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
			// end list of variables
			if (rParse.GetError() == CPB_CLOSEDSB)
			{
				rParse.ClearError();
				rParse.GetCurLinePos(iLine, iPos);
				bCloseBracket = true;
				break;
			}
			// If a comma was encountered then add next 
			// code element list.
			// Otherwise an error occured. Return false!
			else if (rParse.GetError() != CPB_COMMA)
			{
				rParse.PopOpLevel();
				if (rParse.GetError() == CPB_EOL)
					rParse.SetError(CPB_MISSINGCSB);

				return false;
			}
		}
		else break;

		rParse.ClearError();
		rParse.GetCurLinePos(iLine, iPos);
	}

	// Recall previous operator level
	rParse.PopOpLevel();

	if (!bCloseBracket)
	{
		rParse.SetError(CPB_MISSINGCSB);
		return false;
	}

	if (!(m_pCreateVarList = new CCodeCreateVarList))
	{
		rParse.SetError(CPB_OUTOFMEMORY);
		return false;
	}

	// Save pointer to list of code lines that
	// create code list. Maybe used by functions.
	m_pCreateVarList->SetCodeList(pMainList);

	// Bracket was closed
	if (!pMainList->Add(*m_pCreateVarList, true, iLine, iPos))
	{
		rParse.SetError(CPB_OUTOFMEMORY);
		delete m_pCreateVarList;
		return false;
	}

	return true;
}


