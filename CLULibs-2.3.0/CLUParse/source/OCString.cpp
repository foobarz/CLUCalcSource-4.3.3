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

// OCString.cpp: Implementierung der Klasse COCOString.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OCString.h"

#include "ParseBase.h"
#include "CodeVar.h"
#include "CodeData.h"
#include "CodeElementList.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COCString::COCString()
{
	m_csIDSymbolSpace = "\"";	
}

COCString::~COCString()
{

}
//////////////////////////////////////////////////////////////////////
/// Parse
bool COCString::Parse(CParseBase& rParse, CCodeElementList& rCodeList)
{
	int iPos, iLen, iLine;
	bool bError = true;
	cStr csLine, cString;
	char cSym, cPrevSym;
	CCodeData *pCode;

	rParse.GetCurLine(csLine, iLine, iPos);
	iLen = csLine.Len();

	iPos++; // skip the "
	cPrevSym = 0;
	for(;iPos<iLen;iPos++)
	{		
		cSym = csLine[iPos];
		if (cSym == '\"'  && 
			(cPrevSym != '\\' ||
			 (iPos > 1 && cPrevSym == '\\' && csLine[iPos-2] == '\\') )) 
		{
			bError= false; 
			break; // End Search
		}

		if (cSym != '\\') 
		{
			cString += cSym;
		}
		else 
		{
			cSym = csLine[iPos+1];

			if (cSym == '\"')
			{
				cString += cSym;
				iPos++;
			}
			else if (cSym == '\\')
			{
				cString += cSym;
				iPos++;
			}
			else if (cSym == 'n')
			{
				cString += "\n";
				iPos++;
			}
			else if (cSym == 't')
			{
				cString += "\t";
				iPos++;
			}
			else
			{
				cString += '\\';
				cString += cSym;
				iPos++;
			}

			cSym = 0;
		}

		cPrevSym = cSym;
	}
	
	rParse.SetCurPos(++iPos);

	if (bError)
	{
		rParse.SetError(CPB_INVALIDSYNTAX);
		return false;
	}
	if (!(pCode = new CCodeData))
	{
		rParse.SetError(CPB_OUTOFMEMORY);
		return false;
	}

    CCodeVar &rVar = pCode->RefData();

	pCode->SetName(cString);
	rVar.Name() = "Constant";
	rVar = cString;

	
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
