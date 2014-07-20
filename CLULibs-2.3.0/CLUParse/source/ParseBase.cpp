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

// ParseBase.cpp: Implementierung der Klasse CParseBase.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ParseBase.h"
#include "OCIDSymDef.h"

char * CParseBase::m_ppcErrStrList[] = 
{	
	"No Error",
	"Unknown Error",
	"Out of Memory",
	"Invalid Syntax",
	"Missing '('",
	"Missing ')'",
	"Unknown Symbol",
	"Unexpected Symbol",
	"Unexpected Symbol ','",
	"Non-Unary Operator",
	"Non-Binary Operator",
	"Missing '{'",
	"Missing '}'",
	"Missing 'if' condition",
	"Incomplete 'if' statement",
	"Missing 'if' true branch",
	"Missing 'if' false branch",
	"Include file could not be found",
	"Missing '['",
	"Missing ']'",
	"Unexpected Symbol '('",
	"File included recursively"
};


//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CParseBase::CParseBase()
{
	m_iCurPos = 0;
	m_iCurLine = 0;
	m_cLineSepSym = ';';

	m_iErrNo = CPB_NOERROR;

	m_iRoundBracketCount = 0;
	m_iCodeElementCount = 0;

	m_pPrevOC = 0;

	// Initialize List of ObjectClass IDs
	for(int i=0;i<256;i++)
	{
		m_ppOCIDList[i] = 0;
		m_ppOCIDListSpecial[i] = 0;
	}
}

CParseBase::~CParseBase()
{

}



//////////////////////////////////////////////////////////////////////
// Get object class of next element
// Reads symbols until a symbol is an ID for an object class
// in m_ppOCIDList. Returns null if error occured or no object
// could be found.

CObjectClass* CParseBase::GetNextObjectClass(int iMaxLine, bool bPeek)
{
	int iPos, iLine, iLen, iLineCount;

	if (m_iCurLine < 0 || m_iCurLine >= (iLineCount = m_msText.Count()) || 
		m_iCurPos < 0 || m_iCurPos >= (iLen = m_msText[m_iCurLine].csText.Len()))
		return 0;

	if (iMaxLine < 0)
		iMaxLine = iLineCount;
	else if (iMaxLine >= iLineCount)
		return 0;

	char *pcLine;
	
	for(iLine=m_iCurLine;iLine<iLineCount;iLine++)
	{
		if (iLine > iMaxLine)
			break;

		if (!bPeek)
			m_iCurLine = iLine;

		pcLine = m_msText[iLine].csText.Str();
		for(iPos=m_iCurPos;iPos<iLen;iPos++)
		{
			m_pPrevOC = m_pCurOC;

			// Test for special symbol, which starts with OC_IDSYM_SPECIAL_CHAR.
			// The symbol following this switch then gives the actual object class.
			if ((unsigned char) pcLine[iPos] == OC_IDSYM_SPECIAL_CHAR)
			{
				if (iPos + 1 >= iLen)
				{
					SetError(CPB_UNKNOWNSYM);
					return 0;
				}
				iPos++;

				if ((m_pCurOC = m_ppOCIDListSpecial[(unsigned char) pcLine[iPos]]) != 0) 
				{
					return m_pCurOC;
				}
				else
				{
					SetError(CPB_UNKNOWNSYM);
					return 0;
				}

			}
			else if ((m_pCurOC = m_ppOCIDList[(unsigned char) pcLine[iPos]]) != 0) 
			{
				return m_pCurOC;
			}
			else
			{
				SetError(CPB_UNKNOWNSYM);
				return 0;
			}

			if (!bPeek)
				m_iCurPos++;
		}
		if (!bPeek)
			m_iCurPos = 0;
	}

	return 0;
}


//////////////////////////////////////////////////////////////////////
// Read Symbol Subspace Element
// reads element starting at m_iCurPos in line m_iCurLine which consists
// of symbols given in rsSSS. Element is returned in rsElement. 
// Reading of element stops at first symbol not contained in rsSSS.

bool CParseBase::ReadSymSubSpaceElement(cStr& rsElement, cStr& rsSSS)
{
	int i, iLen;

	if (m_iCurLine < 0 || m_iCurLine >= (int) m_msText.Count() || 
		m_iCurPos < 0 || m_iCurPos >= (iLen = m_msText[m_iCurLine].csText.Len()))
		return false;

	char *pcLine = m_msText[m_iCurLine].csText.Str();
	char *pcSSS = rsSSS.Str();

	rsElement = "";

	for(i=m_iCurPos;i<iLen;i++)
	{
		if (!strchr(pcSSS, pcLine[i])) // if cSym is not in Symbol Sub Space
		{
			break;
		}
	}

	// Get Element
	rsElement = m_msText[m_iCurLine].csText(m_iCurPos, i-1);
	
	m_iCurPos = i;

	// Goto next line if we searched to the end of this one.
/*
	if ((m_iCurPos = i) >= iLen)
	{
		m_iCurPos = 0;
		m_iCurLine++;
	}
*/
	return true;
}


//////////////////////////////////////////////////////////////////////
// Add Object Class
// An object class is entered into m_ppOCIDList depending on its
// ID chars. Checks whether IDs are unique. Returns flase if not!

bool CParseBase::AddObjectClass(CObjectClass* pObjectClass)
{
	if (!pObjectClass) return false;

	cStr &csIDSS = pObjectClass->m_csIDSymbolSpace;
	int i, iLen = csIDSS.Len();

	if ((unsigned char) csIDSS[0] == OC_IDSYM_SPECIAL_CHAR)
	{
		if (iLen != 2)
			return false;	// Invalid Special Symbol

		 CObjectClass **ppID = &m_ppOCIDListSpecial[(unsigned char) csIDSS[1]];
	
		 if (*ppID != 0) 
			 return false; // ID-Set not unique

		 *ppID = pObjectClass;

		 return true;
	}

	for(i=0;i<iLen;i++)
	{
		 CObjectClass **ppID = &m_ppOCIDList[(unsigned char) csIDSS[i]];
	
		 if (*ppID != 0) 
			 return false; // ID-Set not unique

		 *ppID = pObjectClass;
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Insert Text
/// Insert text separated into lines before iPos.
/// If iPos == -1 then lines are added to end.
/// Separate Text into lines using m_cSepSym as separation symbol.


int CParseBase::InsertText(const char* _pcText, int iPos)
{
	// Has to be implemented by derived class
	return PreParse(_pcText, iPos);
}

int CParseBase::PreParse(const char* _pcText, int iPos, 
						 const char* pcFilename, int iMainFilePos)
{
	return -1;
}


//////////////////////////////////////////////////////////////////////
/// Basic Parse Loop
/// Finds next Object Class and calls parse on it within current line.

bool CParseBase::BasicParseLoop(CCodeElementList *pCodeList)
{
	CObjectClass *pOC;
	
	while((pOC = GetNextObjectClass(m_iCurLine)) != 0)
	{
		if (!pOC->Parse(*this, *pCodeList))
			return false;
	}

	if (m_iCurPos >= (int) m_msText[m_iCurLine].csText.Len())
	{
		SetError(CPB_EOL);	// End of Line
		return false;
	}
	
	if (GetError() != CPB_NOERROR)
		return false;

	return true;
}



//////////////////////////////////////////////////////////////////////
// Add a new operator level.

bool CParseBase::PushOpLevel(int iLevel)
{
	if (!m_mCurOpLevel.Add(1))
	{
		return false;
	}

	m_mCurOpLevel.Last() = iLevel;

	return true;
}


//////////////////////////////////////////////////////////////////////
// Remove operator level from top of stack.

int CParseBase::PopOpLevel()
{
	int iC;

	if ((iC = m_mCurOpLevel.Count()) == 0)
		return -1;

	int iVal = m_mCurOpLevel.Last();

	m_mCurOpLevel.Del(iC-1);

	return iVal;
}


//////////////////////////////////////////////////////////////////////
// Add Function to Functionlist

bool CParseBase::AddFunc(const char *pcName, TFuncPtr pFunc)
{
	if (!m_mFuncList.Add(1))
	{
		return false;
	}

	m_mFuncList.Last().SetFunc(pcName, pFunc);

	return true;
}


//////////////////////////////////////////////////////////////////////
// Check whether given label is a function

CCodeElement* CParseBase::GetFunc(const char *pcName)
{
	int i, n = m_mFuncList.Count();

	for(i=0;i<n;i++)
	{
		if (m_mFuncList[i].GetName() == pcName)
		{
			return &m_mFuncList[i];
		}
	}

	return 0;
}




