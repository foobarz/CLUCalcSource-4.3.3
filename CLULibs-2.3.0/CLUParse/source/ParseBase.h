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

// ParseBase.h: Schnittstelle für die Klasse CParseBase.
//
//	This is the fundamental parse class with helper
//	functions for parsing.
//	It contains the text that is to be parsed.
//
//	Lines are separated by m_cSepSym symbols.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARSEBASE_H__C464E1CC_D3E8_4D11_B373_5C94DD3C18A1__INCLUDED_)
#define AFX_PARSEBASE_H__C464E1CC_D3E8_4D11_B373_5C94DD3C18A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "ObjectClass.h"
#include "CodeElement.h"
#include "CodeBase.h"
#include "CodeFunction.h"

using std::vector;

// Operator Levels
// 0 binds strongest
#define OPLEVEL_FUNCTION	2		// '[label]('

#define OPLEVEL_GRADE		3		// '°'
#define OPLEVEL_DUAL		5		// '*'
#define OPLEVEL_INVERSE		5		// '!'
#define OPLEVEL_REVERSE		5		// '~'
#define OPLEVEL_INVOLUTE	5		// '''
#define OPLEVEL_NEGATE		5		// '-'
#define OPLEVEL_MODULUS		6		// '%'
#define OPLEVEL_POWER		7		// '^^'

#define OPLEVEL_WEDGE		10		// '^' e.g. outer product
#define OPLEVEL_POP			10		// '.^' e.g. point-wise outer product
#define OPLEVEL_POINT		13		// '.' e.g. inner product
#define OPLEVEL_PIP			13		// '..' e.g. point-wise inner product
#define OPLEVEL_TIMES		15		// '*' e.g. geometric product
#define OPLEVEL_PMUL		15		// '.*' e.g. point-wise geometric product
#define OPLEVEL_DIV			15		// '/' e.g. division
#define OPLEVEL_PDIV		15		// './' e.g. point-wise division
#define OPLEVEL_JOIN		17		// '|'
#define OPLEVEL_MEET		17		// '&'
#define OPLEVEL_MINUS		19		// '-'
#define OPLEVEL_PMINUS		19		// '.-'
#define OPLEVEL_PLUS		20		// '+'
#define OPLEVEL_PPLUS		20		// '.+'

#define OPLEVEL_STEPLIST	30		// '~'

#define OPLEVEL_EQUAL		50		// '=='
#define OPLEVEL_NOT			51		// '!!'
#define OPLEVEL_AND			52		// '&&'
#define OPLEVEL_PAND		52		// '.&&'
#define OPLEVEL_OR			53		// '||'
#define OPLEVEL_POR			53		// '.||'

#define OPLEVEL_LSHIFT		70		// '<<'

#define OPLEVEL_ASSIGN		100		// '='
#define OPLEVEL_OGLDRAW		200		// ':'
#define OPLEVEL_PRINT		200		// '?'


// Error IDs
#define CPB_NOERROR			0
#define CPB_UNKNOWN			1
#define CPB_OUTOFMEMORY		2
#define CPB_INVALIDSYNTAX	3
#define CPB_MISSINGORB		4	// Missing Open Round Bracket
#define CPB_MISSINGCRB		5	// Missing Close Round Bracket
#define CPB_UNKNOWNSYM		6	// Unkown symbol
#define CPB_UNEXPECTEDSYM	7	// Unexpected Symbol
#define CPB_UNEXPSYMCOMMA	8	// Unexpected Symbol Comma
#define CPB_NONUNARYOP		9	// Non-Unary Operator
#define CPB_NONBINARYOP		10	// Non-Binary Operator
#define CPB_MISSINGOCB		11	// Missing Open Curly Bracket
#define CPB_MISSINGCCB		12	// Missing Close Curly Bracket
#define CPB_MISSING_IFCOND	13	// Missing if condition
#define CPB_MISSING_THEN	14	// Missing THEN
#define CPB_MISSING_IFTRUE	15	// Missing IF TRUE branch
#define CPB_MISSING_IFFALSE	16	// Missing IF FALSE branch
#define CPB_NO_INCLUDEFILE  17  // Include file could not be found
#define CPB_MISSINGOSB		18	// Missing Open Square Bracket
#define CPB_MISSINGCSB		19	// Missing Close Square Bracket
#define CPB_UNEXPORB		20  // Unexpected Open Round Bracket
#define CPB_RECURSIVE_INCLUDEFILE		21  // Recursive includefile

// Messages returned for internal parsing operations
#define CPB_WEAKEROP		-1
#define CPB_COMMA			-2
#define CPB_CLOSEDRB		-3	// Closed Round Bracket
#define CPB_EOL				-4	// End of Line
#define CPB_CLOSEDCB		-5	// Closed Curly Bracket
#define CPB_THEN			-6  // Start of TRUE branch
#define CPB_ELSE			-7  // Start of FALSE branch
#define CPB_REPARSE			-8  // Parse current line again
#define CPB_CLOSEDSB		-9	// Closed Square Bracket


/*
struct SFuncCodeElement
{
	cStr csName;
	CCodeFunction CodeFunc;
};
*/

struct STextLine
{
	cStr csText;			// Preparsed text
	cStr csInputText;		// original input text
	cStr csFilename;		// Name of File from which line originates
	int iMainFilePos;		// Position in main file where include statement is found
	int iLine;				// Line in Text
	int iInputTextStartPos; // Position in original input text where csInputText starts.
	vector<int> vecPos; 
};	


class CParseBase  
{
public:
	CParseBase();
	virtual ~CParseBase();


// Initialisation
	// Set the line separation symbol
	void SetLineSepSym(char cSepSym) { m_cLineSepSym = cSepSym; }

	// Set current parse line. This function sets m_iCurPos = 0;
	bool SetCurLine(int iLine) 
	{ if (iLine >= 0 && iLine < (int) m_msText.Count()) { m_iCurLine = iLine; m_iCurPos = 0; return true; } else return false; }

	// Set m_iCurLine to next line
	bool NextLine() { if (m_iCurLine+1 < (int)m_msText.Count()) { m_iCurLine++; m_iCurPos = 0; return true; } return false; }

// Operations
	// Reset Text
	bool Reset() 
	{ 	
		m_iCurPos = 0;	m_iCurLine = 0;	m_iRoundBracketCount = 0; m_iCodeElementCount = 0;
		m_iSquareBracketCount = 0; m_mCurOpLevel.Set(0); m_pPrevOC = 0; m_pCurOC = 0;
		return m_msText.Set(0); 
	}

	// Automatically separates Text into lines.
	// Returns number of lines in text.
	// Returns -1 if error occured.
	int SetText(const char* pcText) { Reset(); return InsertText(pcText); }

	// Insert lines of text before iPos.
	// If iPos == -1 then add lines to end
	// Automatically separates Text into lines.
	// Returns number of lines contained in text.
	// Returns -1 if error occured.
	int InsertText(const char* pcText, int iPos = -1);

	// Delete lines of Text
	// iFirst: first line to be deleted (starts at 0)
	// iNum: number of lines to be deleted
	bool DeleteLine(int iFirst, int iNum = 1) { return m_msText.Del(iFirst, iNum); }

	// Add Object Class
	// An object class is entered into m_ppOCIDList depending on its
	// ID chars. Checks whether IDs are unique. Returns flase if not!
	bool AddObjectClass(CObjectClass* pObjectClass);

	// Read Symbol Subspace Element
	// reads element starting at m_iCurPos in line m_iCurLine which consists
	// of symbols given in rsSSS. Element is returned in rsElement. 
	// Reading of element stops at first symbol not contained in rsSSS,
	// or at end of line.
	bool ReadSymSubSpaceElement(cStr& rsElement, cStr& rsSSS);

	// Get object class of next element
	// Reads symbols until a symbol is an ID for an object class
	// in m_ppOCIDList. Returns null if error occured or no object
	// could be found.
	// iMaxLine gives last line to read. if iMaxLine == -1 then 
	// read till end of text.
	// If bPeek = true then do not change current position.
	CObjectClass* GetNextObjectClass(int iMaxLine = -1, bool bPeek = false);

	CObjectClass* GetPrevObjectClass() { return m_pPrevOC; }
	CObjectClass* GetCurObjectClass() { return m_pCurOC; }

	// Basic Parse Loop
	// Finds next Object Class and calls parse on it within current line.
	bool BasicParseLoop(CCodeElementList *pCodeList);

	// Get current line and position in line
	bool GetCurLine(cStr& rsLine, int& iLine, int& iPos) 
	{ rsLine = m_msText[m_iCurLine].csText; iLine = m_iCurLine; iPos = m_iCurPos; return true; }

	bool GetCurLinePos(int& iLine, int& iPos) { iLine = m_iCurLine; iPos = m_iCurPos; return true; }

	int GetLineCount() { return (int) m_msText.Count(); }
	cStr GetLine(int iLine) { return m_msText[iLine].csText; }
	int GetTextLineNo(int iLine) { return m_msText[iLine].iLine; }
	int GetInputPos(int iLine, int iPos) { return m_msText[iLine].vecPos[iPos]; }

	// Set current position in current line
	void SetCurPos(int iPos) 
	{ if (iPos >= 0 && iPos <= (int) m_msText[m_iCurLine].csText.Len()) m_iCurPos = iPos; }

	// returns false if changed to new line
	bool AddToCurPos(int iAdd)
	{ int iLen; if (m_iCurPos+iAdd >= (iLen = (int) m_msText[m_iCurLine].csText.Len())) 
		{ m_iCurPos=iLen; return false; } 
		else { m_iCurPos += iAdd; return true; } 
	}

	void SetError(int iErrNo) { m_iErrNo = iErrNo; }
	int GetError() { return m_iErrNo; }
	void ClearError() { m_iErrNo = CPB_NOERROR; }

	const char* GetErrorStr(int iErrNo) const 
	{ if (iErrNo < 0) iErrNo = CPB_UNEXPECTEDSYM; return m_ppcErrStrList[iErrNo]; }

	// Add a new operator level.
	bool PushOpLevel(int iLevel);

	// Remove top element on operator level stack.
	int PopOpLevel();

	// Read top element on operator level stack.
	int GetCurOpLevel() { if (m_mCurOpLevel.Count() == 0) return -1; return m_mCurOpLevel.Last(); }

	// Add Function to Functionlist
	bool AddFunc(const char *pcName, TFuncPtr pFunc);

	// Check whether given label is a function
	// Returns pointer to function if function exists,
	// otherwise returns zero.
	CCodeElement* GetFunc(const char *pcName);

	// Increment Round Bracket Count (RBC)
	int IncrRBC() { return ++m_iRoundBracketCount; }

	// Decrement Round Bracket Count (RBC)
	// Returns -1 if RBC is already at 0
	int DecrRBC() { if (!m_iRoundBracketCount) return -1; return --m_iRoundBracketCount; }

	// Increment Square Bracket Count (SBC)
	int IncrSBC() { return ++m_iSquareBracketCount; }

	// Decrement Square Bracket Count (SBC)
	// Returns -1 if SBC is already at 0
	int DecrSBC() { if (!m_iSquareBracketCount) return -1; return --m_iSquareBracketCount; }


	// Increment Code Element Count
	int IncrCEL() { return ++m_iCodeElementCount; }

protected:
	virtual int PreParse(const char* pcText, int iPos = -1, 
							const char* pcFilename = 0, int iMainFilePos = -1);

protected:
	CObjectClass* m_ppOCIDList[256]; // Object Class ID List.
	CObjectClass* m_ppOCIDListSpecial[256]; // Object Class ID List Special.
	CObjectClass *m_pCurOC; // Current Object class
	CObjectClass *m_pPrevOC; // Previous Object class

	MemObj<STextLine> m_msText;	// Lines of Parse Text

	char m_cLineSepSym;	// Line separation symbol

	int m_iCurLine;	// Current Line.
	int m_iCurPos;	// Current Position in Text

	int m_iErrNo;	// Error Number

	int m_iRoundBracketCount;	// Count of currently open round brackets
	int m_iSquareBracketCount;	// Count of currently open round brackets

	int m_iCodeElementCount;	// Count of code elements enclosed in curly brackets

	Mem<int> m_mCurOpLevel;	// Current operator level
	MemObj<CCodeFunction> m_mFuncList; // List of Functions

	static char *m_ppcErrStrList[];
};

#endif // !defined(AFX_PARSEBASE_H__C464E1CC_D3E8_4D11_B373_5C94DD3C18A1__INCLUDED_)
