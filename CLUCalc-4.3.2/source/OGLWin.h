/// LICENSE START
////////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the CLUCalc program.
// CLUCalc is an interactive Clifford calculator with OpenGL visualiations.
//
// 
// Copyright (C) 2002-2004  Christian B.U. Perwass
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

//////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2001  Christian B.U. Perwass
//
// This file is part of CLUDraw.
//
// CLUDraw is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// CLUDraw is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CLUDraw; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// The GNU General Public License can be found in the file
// license.txt distributed with this library.
//
// Please send all queries and comments to
//
// email: public@perwass.com
// mail: Institut fuer Informatik, Preusserstr. 1-9, 24105 Kiel, Germany.
//
//////////////////////////////////////////////////////////////////////


// OGLWin.h: Schnittstelle für die Klasse COGLWin.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLWIN_H__948387CF_09B2_4CBB_B6D2_2A1473F061B2__INCLUDED_)
#define AFX_OGLWIN_H__948387CF_09B2_4CBB_B6D2_2A1473F061B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define ANIMROT_NONE	0
#define ANIMROT_X		1
#define ANIMROT_Y		2
#define ANIMROT_Z		3
#define ANIMROT_MOUSE	4

#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_ask.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Tile.H>
#include <FL/fl_ask.h>
#include <FL/Fl_Browser.h>
#include <FL/Fl_Hold_Browser.h>

#include <clu/OGLWinFLTK.h>

#include <string>

using namespace std;

class CCLUScriptEditor;
class CCLUOutput;

class COGLWin : public COGLWinFLTK 
{
protected:
	class CScriptBrowser : public Fl_Hold_Browser
	{
	public:
		CScriptBrowser(int iX, int iY, int iWidth, int iHeight, const char *pcText = 0) 
			: Fl_Hold_Browser(iX, iY, iWidth, iHeight, pcText)
		{
		}

		virtual int handle(int iEvent)
		{
			if (iEvent == FL_KEYUP && 
				Fl::event_key() == FL_Enter &&
				!Fl::event_state(FL_CTRL))
			{
				do_callback();
				return 1;
			}
			return Fl_Hold_Browser::handle(iEvent);
		}
	};

public:
	COGLWin();
	virtual ~COGLWin();

	bool Init();
	void InitMenu();
	bool SetTitle();

	static void CB_NextScript(Fl_Widget* pWidget, void *pData);
	static void CB_PrevScript(Fl_Widget* pWidget, void *pData);
	static void CB_ReloadScript(Fl_Widget* pWidget, void *pData);
	static void CB_ClearScriptList(Fl_Widget* pWidget, void *pData);
	static void CB_ShowScriptList(Fl_Widget* pWidget, void *pData);
	static void CB_ScriptListWindow(Fl_Widget* pWidget, void *pData);

	void MenuNextScript(Fl_Widget *pWidget);
	void MenuPrevScript(Fl_Widget *pWidget);
	void MenuReloadScript(Fl_Widget *pWidget);
	void MenuClearScriptList(Fl_Widget *pWidget);
	void MenuShowScriptList(Fl_Widget *pWidget);
	void ScriptListWindowEvent(Fl_Widget *pWidget);
	void InitScriptListWindow();

	static void CB_ShowEditorWin(Fl_Widget *pWidget, void *pvData);
	static void CB_ShowOutputWin(Fl_Widget *pWidget, void *pvData);
	static void CB_ShowVisWin(Fl_Widget *pWidget, void *pvData);

	void MenuShowEditorWin();
	void MenuShowOutputWin();
	void MenuShowVisWin();

	static void CB_Parse(Fl_Widget *pWidget, void *pvData);
	static void CB_ParseForce(Fl_Widget *pWidget, void *pvData);

	void MenuParse();
	void MenuParseForce();

	virtual bool SetOGLSize(double dWidth, double dHeight);

	void SetEditor(CCLUScriptEditor *pEditor)
	{ m_pEditor = pEditor; }

	void SetOutputWin(CCLUOutput *pOutput)
	{ m_pOutput = pOutput; }

	void SetScriptFilename(const char* pcFileName) { m_csScriptFilename = pcFileName; }
	void SetScriptPath(const char* pcScriptPath) 
	{ 
		m_sScriptPath = pcScriptPath; 
		m_Parse.SetScriptPath(pcScriptPath);
	}

	void SetScriptName(const char* pcScriptName) 
	{ 
		m_sScriptName = pcScriptName; 
		m_Parse.SetScriptName(pcScriptName);
	}

	string GetScriptName() { return m_sScriptName; }
	string GetScriptPath() { return m_sScriptPath; }

	int SetScript(const char *pcScript, bool bResetEnv = false, bool bReparseLatex = false);
	int LoadScript();
	int CheckScriptChanged();

	void SetAnimRotType(int iRotType) { m_iAnimRotType = iRotType; }
	void NotifyCodeChanged() { m_bCodeChanged = true; }

	bool CanAnimateScript() 
	{ 
		CCodeVar Var; 
		TCVScalar iVal; 
		
		if (m_Parse.GetVar("_DoAnimate", Var) && 
			Var.CastToScalar(iVal) && 
			iVal != 0)
			return true; 
		return false; 
	}

	bool ScriptIs2d() 
	{ 
		CCodeVar Var; 
		TCVScalar iVal;

		if (m_Parse.GetVar("_2dView", Var) && 
			Var.CastToScalar(iVal) && 
			iVal != 0)
			return true; 
		return false; 
	}

	void SetToolNameVar(const char *pcName);
	void SetLinkDataVar(const char *pcData);
	void SetExecModeVar(int iMode);
	void AddExecModeVar(int iMode);

	CCLUParse m_Parse;

	bool HasError() { return m_bHasError; }
	bool HasOutput() { return m_bHasOutput; }
	bool HasMessage() { return m_bHasMessage; }

	string GetError() { return m_sError; }
	string GetOutput() { return m_sOutput; }
	string GetMessage() { return m_sMessage; }

	void ClearError() { m_sError = ""; m_bHasError = false; }
	void ClearOutput() { m_Parse.ResetOutputObjectList(); m_bHasOutput = false; }
	void ClearMessage() { m_sMessage = ""; m_bHasMessage = false; }

	const char* InfoLink(Fl_Widget* pWidget, const char *pcURL);

	void EnableWindowMenu(bool bVal = true) { m_bShowWindowMenu = bVal; }
	void EnableVizOnlyMode(bool bVal = true) { m_bVizOnlyMode = bVal; }

	void SetVersion(int iMajor, int iMinor, int iRevision) 
	{ m_Parse.SetVersion(iMajor, iMinor, iRevision); m_Parse.DefineVersion(); }
	void GetVersion(int& iMajor, int& iMinor, int& iRevision) { m_Parse.GetVersion(iMajor, iMinor, iRevision); }

	bool SetAnimationTimeStep(int iTimeStep);

protected:
	static void CallDisplayOutput();
	static void CallIdleFunc();
	static void CallReshapeOutput(int iWidth, int iHeight);

	void KeyUp(unsigned char cKey);
	void KeyDown(unsigned char cKey);

	void Key(unsigned char cKey, int iX, int iY);
	void ReshapeOutput(int iWidth, int iHeight);
	void DisplayOutput();
	void IdleFunc();

	virtual void Reshape(int iWidth, int iHeight);

	virtual bool TimedFunc();
	virtual bool AtAnimStart();
	void Display();
	void Draw();
	void CheckToolBoxSize();

	void Sleep(unsigned int uMilliSec);

	void SetAnimCode(bool bDoAnim);

	void ToolChanged(string sName);
	virtual bool SetInfoWidth(double dWidth);


protected:
	static void CB_Quit(Fl_Widget* pWidget, void *pData);

protected:
	bool m_bCodeChanged;
	bool m_bAnimCode;
	bool m_bExecCode;
	bool m_bOutputChanged;
	bool m_bScriptLoaded;
	bool m_bCheckForAnimate;
	bool m_bBGColChanged;
	bool m_bFrameTransChanged;
	bool m_bCodeChangedForAnim;
	bool m_bKeepBGColor;
	bool m_bPresentationAction;
	bool m_bParseFromOGLWin;
	bool m_bWaitWithIdleForNextDisplay;
	bool m_bCheckToolBoxSize;
	bool m_bScriptChangedOGLSize;

	bool m_bShowWindowMenu;
	bool m_bVizOnlyMode;
	
	double m_dNewOGLWidth, m_dNewOGLHeight;

	TCVScalar m_dFrameBoxSize;

	int m_iExecMode;
	int m_iHWinOutput;

	int m_iOutSizeX;
	int m_iOutSizeY;
	int m_iOutLineHeight;
	int m_iCurToolCount;
	double m_dNewInfoWidth;

	void *m_pOutputFont;
	
	// Script handling
	cStr m_csScriptFilename;

	bool m_bHasError;
	bool m_bHasOutput;
	bool m_bHasMessage;
	bool m_bInDisplay;
	bool m_bAnimPauseChanged;
	
	int m_iAllowKeyUp;

	string m_sError;
	string m_sOutput;
	string m_sMessage;
	string m_sScriptPath;
	string m_sScriptName;
	string m_sTitle;

	MultiV<float> m_vRot;

	COGLDisplayList m_dlSpace;

	CCLUCodeBase::TOutObjList m_vecOutObj;

	E3GA<float> m_E3Base;
	PGA<float> m_P3Base;
	ConfGA<float> m_N3Base;
	COGLMVFilter m_Filter;

	Fl_Window *m_pBrowserWin;
	CScriptBrowser *m_pBrowser;

	CCLUScriptEditor *m_pEditor;
	CCLUOutput *m_pOutput;

	static COGLWin *m_pThisWin;
};

#include "CLUScriptEditor.h"

#endif // !defined(AFX_OGLWIN_H__948387CF_09B2_4CBB_B6D2_2A1473F061B2__INCLUDED_)
