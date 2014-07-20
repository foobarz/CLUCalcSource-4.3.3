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

// OCNumber.cpp: Implementierung der Klasse COCNumber.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OCNumber.h"

#include "ParseBase.h"
#include "CodeVar.h"
#include "CodeData.h"
#include "CodeElementList.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COCNumber::COCNumber()
{
	m_csIDSymbolSpace = "1234567890";
	m_csSymbolSpace = m_csIDSymbolSpace + ".-edf";
}

COCNumber::~COCNumber()
{

}


//////////////////////////////////////////////////////////////////////
/// Parse
bool COCNumber::Parse(CParseBase& rParse, CCodeElementList& rCodeList)
{
	int iPos, iLen, iLine, iNPos;
	bool bError = false;
	cStr csLine;
	char cSym, cPrevSym, pcNumber[100];
	ECodeDataType eType = PDT_INT;
	CCodeData *pCode;

	rParse.GetCurLine(csLine, iLine, iPos);
	iLen = csLine.Len();

	cPrevSym = 0;
	for(iNPos=0;iPos<iLen;iPos++,iNPos++)
	{
		cSym = csLine[iPos];
		if (strchr(m_csSymbolSpace, cSym) == 0)
			break; // End Search

		if (cSym == '-' && cPrevSym != 'e')
			break;

		if (cSym == 'e')
		{
			if (cPrevSym != '.' && strchr(m_csIDSymbolSpace, cPrevSym) == 0)
			{
				bError = true;
				break;
			}
			else
			{
				eType = PDT_DOUBLE;
			}
		}

		if (cSym == 'd')
		{
			if (cPrevSym != '.' && strchr(m_csIDSymbolSpace, cPrevSym) == 0)
			{
				bError = true;
				break;
			}
			else
			{
				eType = PDT_DOUBLE;
				iPos++;
				break;
			}
		}

		if (cSym == 'f')
		{
			if (cPrevSym != '.' && strchr(m_csIDSymbolSpace, cPrevSym) == 0)
			{
				bError = true;
				break;
			}
			else
			{
				eType = PDT_FLOAT;
				iPos++;
				break;
			}
		}

		if (cSym == '.')
		{
			if (strchr(m_csIDSymbolSpace, cPrevSym) == 0)
			{
				bError = true;
				break;
			}
			else
			{
				eType = PDT_DOUBLE;
			}
		}

		pcNumber[iNPos] = cSym;
		cPrevSym = cSym;
	}
	
	rParse.SetCurPos(iPos);

	if (bError)
	{
		rParse.SetError(CPB_INVALIDSYNTAX);
		return false;
	}

	pcNumber[iNPos] = 0;

	if (!(pCode = new CCodeData))
	{
		rParse.SetError(CPB_OUTOFMEMORY);
		return false;
	}

    CCodeVar &rVar = pCode->RefData();

	pCode->SetName(pcNumber);
	rVar.Name() = "Constant";

	if (eType == PDT_INT)
	{
		int iVal;

		if (sscanf(pcNumber, "%d", &iVal) != 1)
		{
			rParse.SetError(CPB_UNKNOWN);
			delete pCode;
			return false;
		}
		rVar = iVal;
	}
	else if (eType == PDT_FLOAT)
	{
		float fVal;

		if (sscanf(pcNumber, "%g", &fVal) != 1)
		{
			rParse.SetError(CPB_UNKNOWN);
			delete pCode;
			return false;
		}

		rVar = fVal;
	}
	else if (eType == PDT_DOUBLE)
	{
		double dVal;

		if (sscanf(pcNumber, "%lg", &dVal) != 1)
		{
			rParse.SetError(CPB_UNKNOWN);
			delete pCode;
			return false;
		}

		rVar = dVal;
	}

	// Variable is constant and cannot be an L-Value.
	rVar.EnableProtect();

	rParse.GetCurLinePos(iLine, iPos);

	// Add Code Element to end of Code Element List.
	if (!rCodeList.Add(*pCode, true, iLine, iPos))
	{
		delete pCode;
		rParse.SetError(CPB_OUTOFMEMORY);
		return false;
	}

	return true;
}

