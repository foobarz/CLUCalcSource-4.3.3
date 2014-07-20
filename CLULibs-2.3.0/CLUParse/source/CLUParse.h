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

// CLUParse.h: Schnittstelle für die Klasse CCLUParse.
//
// This Class initializes the CParseBase with the appropriate syntax.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLUPARSE_H__7F82ECDB_7A39_4FE9_B21C_D4377F4DBCC2__INCLUDED_)
#define AFX_CLUPARSE_H__7F82ECDB_7A39_4FE9_B21C_D4377F4DBCC2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OCIDSymDef.h"

#include "Parse.h"
#include "CLUCodeBase.h"

#include "OCLabel.h"
#include "OCNumber.h"
#include "OCString.h"
#include "OCObjSepSym.h"

#include "OCOpOpenRoundBracket.h"
#include "OCOpCloseRoundBracket.h"
#include "OCOpComma.h"

#include "OCOpOpenSquareBracket.h"
#include "OCOpCloseSquareBracket.h"

#include "OCOpOpenCurlyBracket.h"
#include "OCOpCloseCurlyBracket.h"

#include "OCKeywordIf.h"
#include "OCKeywordThen.h"
#include "OCKeywordElse.h"
#include "OCKeywordLoop.h"
#include "OCKeywordBreak.h"

#include "OCOpStd.h"

#include "CodeFunction.h"

class CCLUParse : public CParse 
{
public:
	CCLUParse();
	virtual ~CCLUParse();

	virtual bool Init();

	cStr PrintVars();
	cStr PrintStack();

	cStr PrintParseErrors();
	cStr PrintCodeErrors();

	bool GetParseErrorPos(int& iLine, int& iPos, int &iInputPos, cStr &csFilename);
	bool GetCodeErrorPos(int& iLine, int& iPos, int &iInputPos, cStr &csFilename);
	string GetErrorFilename() { return m_sErrorFilename; }
	string GetErrorPath() { return m_sErrorPath; }

	void ResetErrors() { m_CodeBase.m_ErrorList.Reset(); }

	CCLUCodeBase& GetCodeBase() { return m_CodeBase; }
	void ResetCodeBase() { m_CodeBase.Reset(); }
	void CollectGarbage() { m_CodeBase.CollectGarbage(); }

	void ResetOutputObjectList() { m_CodeBase.ResetOutputObjectList(); }
	const CCLUCodeBase::TOutObjList& GetOutputObjectList() { return m_CodeBase.GetOutputObjectList(); }
	void InsertOutputObject(const CCLUCodeBase::SOutputObject &rObj) { m_CodeBase.InsertOutputObject(rObj); }

	void SetVersion(int iMajor, int iMinor, int iRevision) { m_CodeBase.SetVersion(iMajor, iMinor, iRevision); }
	void GetVersion(int& iMajor, int& iMinor, int& iRevision) { m_CodeBase.GetVersion(iMajor, iMinor, iRevision); }

	void SetRenderLatex(bool bVal = true) { m_CodeBase.SetRenderLatex(bVal); }
	void OverrideSetRenderLatex(bool bVal = true) { m_CodeBase.OverrideSetRenderLatex(bVal); }
	
	void InitBitmapFont() { m_CodeBase.InitBitmapFont(); }
	bool SetBitmapFont(int iFont) { return m_CodeBase.SetBitmapFont(iFont); }
	bool SetBitmapFontSize(int iSize) { return m_CodeBase.SetBitmapFontSize(iSize); }

	void SetMVFilter(COGLMVFilter *pFilter) { m_CodeBase.SetMVFilter(pFilter); }
	void SetTransform(Mem<STransform> *pTransform) { m_CodeBase.SetTransform(pTransform); }
	void SetFrameRotor(MultiV<float> *pFrameRotor) { m_CodeBase.SetFrameRotor(pFrameRotor); }
	void SetCLUDrawBase(CCLUDrawBase *pCLUDrawBase) { m_CodeBase.SetCLUDrawBase(pCLUDrawBase); }

	void ResetTempImageList() { m_CodeBase.ResetTempImageList(); }
	void ToggleTempImageList() { m_CodeBase.ToggleTempImageList(); }

	void SetScriptPath(const char* pcScriptPath) 
	{ m_sScriptPath = pcScriptPath; m_CodeBase.SetScriptPath(pcScriptPath); }
	void SetScriptName(const char* pcScriptName) 
	{ m_sScriptName = pcScriptName; m_CodeBase.SetScriptName(pcScriptName); }

	bool DefineColor(const char *pcName, float fR, float fG, float fB, float fA = 1.0f, bool Protect = true);
	bool DefineFloat(const char *pcName, float fVal, bool Protect = true);
	bool DefineDouble(const char *pcName, double dVal, bool Protect = true);
	bool DefineInt(const char *pcName, int iVal, bool Protect = true);
	bool DefineString(const char *pcName, const char *pcVal, bool Protect = true);
	bool DefineVersion();

	bool GetVar(const char *pcName, CCodeVar& rVar)
	{ rVar = m_CodeBase.GetVar(pcName); if (rVar.Type() == PDT_NOTYPE) return false; else return true; }

	CCodeVar& GetVar(const char *pcName) { return m_CodeBase.GetVar(pcName); }

	CCodeVar& NewVar(const char *pcName, ECodeDataType _nType = PDT_INT)
	{ return m_CodeBase.NewVar(pcName, _nType); }

	// Run Code starting at line iStartLine for iLineCount lines.
	// if iStartLine == 0 then start at beginning.
	// if iLineCount == -1 then run all lines after iStartLine.
	bool RunCode(int iStartLine = 0, int iLineCount = -1);

	// Insert Text pcText at position iPos and parse it if bParse is true.
	// If iPos == -1 then add text to end.
	// Returns number of lines read. Returns -1 if error occured.
	virtual int InsertText(const char *pcText, int iPos = -1, bool bParse = true);

protected:
	// Insert lines of text before iPos.
	// If iPos == -1 then add lines to end
	// Automatically separates Text into lines.
	// Returns number of lines contained in text.
	// Returns -1 if error occured.
	// Overloaded from ParseBase.
	int PreParse(const char* pcText, int iPos = -1, 
				const char *pcFilename = 0, int iMainFilePos = -1);

protected:
	COCObjSepSym m_ocObjSepSym;

	COCLabel m_ocLabel;
	COCNumber m_ocNumber;
	COCString m_ocString;
	
	COCOpOpenRoundBracket m_ocOpOpenRoundBracket;
	COCOpCloseRoundBracket m_ocOpCloseRoundBracket;
	COCOpComma m_ocOpComma;

	COCOpOpenSquareBracket m_ocOpOpenSquareBracket;
	COCOpCloseSquareBracket m_ocOpCloseSquareBracket;

	COCOpOpenCurlyBracket m_ocOpOpenCurlyBracket;
	COCOpCloseCurlyBracket m_ocOpCloseCurlyBracket;

	COCKeywordIf m_ocKeywordIF;
	COCKeywordThen m_ocKeywordTHEN;
	COCKeywordElse m_ocKeywordELSE;
	COCKeywordLoop m_ocKeywordLOOP;
	COCKeywordBreak m_ocKeywordBREAK;

	// Standard Operators
	MemObj<COCOpStd> m_mOps;

	string m_sScriptPath;
	string m_sScriptName;
	string m_sIncludeFilename;
	string m_sErrorFilename;
	string m_sErrorPath;

	vector<string> m_vecIncludeFilename;

	CCLUCodeBase m_CodeBase;
};

#endif // !defined(AFX_CLUPARSE_H__7F82ECDB_7A39_4FE9_B21C_D4377F4DBCC2__INCLUDED_)
