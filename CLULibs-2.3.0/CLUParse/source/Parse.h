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

// Parse.h: Schnittstelle für die Klasse CParse.
//
//	This class connects Parsing of text and the root list
//	of code elements. It organizes the lines of code in the
//	text into lines of the root list.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARSE_H__6FEFB6B2_70AD_4D88_8E78_6ED75745E75A__INCLUDED_)
#define AFX_PARSE_H__6FEFB6B2_70AD_4D88_8E78_6ED75745E75A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeElementList.h"
#include "ParseBase.h"
#include "CodeBase.h"

class CParse : 
	public CParseBase, 
	protected CCodeElementList  
{
public:
	CParse();
	virtual ~CParse();

	bool Reset() { CParseBase::Reset(); CCodeElementList::Reset(); return true; }
	bool ResetCode() { return CCodeElementList::Reset(); }

	void SetCodeBase(CCodeBase *pVal) { m_pCodeBase = pVal; }

	bool IsCodeOK() { return m_bCodeTreeOK; }

	// Initialize ParseBase and CodeList
	virtual bool Init() { return true; } 

	// Insert Text pcText at position iPos and parse it if bParse is true.
	// If iPos == -1 then add text to end.
	// Returns number of lines read. Returns -1 if error occured.
	virtual int InsertText(const char *pcText, int iPos = -1, bool bParse = true);

	// Delete Code and Text starting at line iStartLine for iLineCount lines.
	// if bDelCodeAndText == false then only delete text.
	virtual bool DeleteText(int iStartLine, int iLineCount = 1, bool bDelCodeAndText = true);

	// Parse text starting at line iStartLine for iLineCount lines.
	// If iStartLine == 0 then start at beginning.
	// If iLineCount == -1 then parse everything starting from line iStartLine.
	virtual bool Parse(int iStartLine = 0, int iLineCount = -1);

	// Run Code starting at line iStartLine for iLineCount lines.
	// if iStartLine == 0 then start at beginning.
	// if iLineCount == -1 then run all lines after iStartLine.
	virtual bool RunCode(int iStartLine = 0, int iLineCount = -1);

	cStr& GetTextOutput() { if (!m_pCodeBase) return m_csDummy; return m_pCodeBase->GetTextOutput(); }

protected:
	cStr m_csDummy;
	CCodeBase *m_pCodeBase;

	bool m_bCodeTreeOK;	// If false, cannot run code
};

#endif // !defined(AFX_PARSE_H__6FEFB6B2_70AD_4D88_8E78_6ED75745E75A__INCLUDED_)
