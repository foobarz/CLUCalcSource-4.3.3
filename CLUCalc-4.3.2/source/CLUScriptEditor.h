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

#if !defined( _CLUSCRIPTEDITOR_H__INCLUDED_ )
#define _CLUSCRIPTEDITOR_H__INCLUDED_

#include "StdAfx.h"
#include "OGLWin.h"
#include "FLTKEditor.h"
#include "FLTKOutput.h"

#include <FL/Fl_Help_Dialog.H>

#include <map>

// Insert Code Defines
#define CSE_IC_COLOR	"  User Defined  "
#define CSE_IC_RED		"  Red  "
#define CSE_IC_GREEN	"  Green  "
#define CSE_IC_BLUE		"  Blue  "
#define CSE_IC_MAGENTA	"  Magenta  "
#define CSE_IC_CYAN		"  Cyan  "
#define CSE_IC_YELLOW	"  Yellow  "
#define CSE_IC_ORANGE	"  Orange  "
#define CSE_IC_WHITE	"  White  "
#define CSE_IC_BLACK	"  Black  "

#define CSE_IC_E3		"  Euclidean 3D  "
#define CSE_IC_P3		"  Projective 3D  "
#define CSE_IC_N3		"  Conformal 3D  "
#define CSE_IC_C2		"  Conic 2D  "

#define CSE_IC_VE3		"  Euclidean 3D   "
#define CSE_IC_VP3		"  Projective 3D   "
#define CSE_IC_VN3		"  Conformal 3D   "
#define CSE_IC_VC2		"  Conic 2D   "

#define CSE_IC_EM_NEW		"  Initial Load  "
#define CSE_IC_EM_CHANGE	"  Script Changed  "
#define CSE_IC_EM_ANIMATE	"  Animation Step  "
#define CSE_IC_EM_USER		"  Action by User  "
#define CSE_IC_EM_TOOL		"  Tool Changed  "
#define CSE_IC_EM_KEYUP		"  Key Released  "
#define CSE_IC_EM_KEYDOWN	"  Key Pressed  "
#define CSE_IC_EM_RESHAPE	"  Window Size Changed  "
#define CSE_IC_EM_LINK		"  Info Text Link Selected  "

#define CSE_IC_MVOP_GEO			"  Geometric Product  "
#define CSE_IC_MVOP_INNER		"  Inner Product  "
#define CSE_IC_MVOP_OUTER		"  Outer Product  "
#define CSE_IC_MVOP_ADD			"  Addition  "
#define CSE_IC_MVOP_SUB			"  Subtraction  "
#define CSE_IC_MVOP_DUAL		"  Dual  "
#define CSE_IC_MVOP_REVERSE		"  Reverse  "
#define CSE_IC_MVOP_INVOLUTE	"  Main Involution  "

#define CSE_IC_GA_E3		"  3D Euclidean Space  "
#define CSE_IC_GA_P3		"  3D Projective Space  "
#define CSE_IC_GA_N3		"  3D Conformal Space  "
#define CSE_IC_GA_C2		"  2D Conic Space  "

#define CSE_IC_OPNS			"  Outer Product Null Space  "
#define CSE_IC_IPNS			"  Inner Product Null Space  "

#define CSE_IC_VS_POINT_DOT		"  Draw Points as Dots  "
#define CSE_IC_VS_POINT_SPHERE  "  Draw Points as Spheres  "

#define CSE_IC_VS_E3_VEC_LINE	"  Draw Vector as Line  "
#define CSE_IC_VS_E3_VEC_ARROW	"  Draw Vector as Arrow  "
#define CSE_IC_VS_E3_VEC_POINT	"  Draw Vector as Point  "

#define CSE_IC_VS_E3_PLANE_RECT	"  Draw Plane as Rectangle  "
#define CSE_IC_VS_E3_PLANE_DISK	"  Draw Plane as Disk  "

#define CSE_IC_VS_N3_SOLID		"  Draw Solid Spheres  "
#define CSE_IC_VS_N3_WIRE		"  Draw Wire-Frame Spheres  "

#define CSE_IC_VS_N3_IMAG		"  Draw Imaginary Objects  "
#define CSE_IC_VS_N3_NO_IMAG	"  Draw NO Imaginary Objects  "

#define CSE_IC_LOOP1			"  Incremental Loop  "
#define CSE_IC_LOOP2			"  Nested Incremental Loop  "

using namespace std;

class CCLUScriptEditor :
	public CFLTKEditor
{
/*
public:
	typedef bool (CCLUScriptEditor::*TTextFuncPtr)();

	struct SCMenuData
	{
		SCMenuData()
		{ iFinalPosOffset = 0; pFunc = 0; }

		Set(const char* pcTitle, const char* pcText, int iFinalPosOff, TTextFuncPtr _pFunc)
		{ 
			sTitle = pcTitle; sText = pcText; iFinalPosOffset = iFinalPosOff;
			pFunc = _pFunc; 
		}

		string sTitle;	// Name of Menu item which is displayed
		string sText;	// Text to be inserted in editor
		int iFinalPosOffset;  // Number of characters to go back from end of 
							  // insertion. Can be used to position curser at
							  // sensible position.
		TTextFuncPtr pFunc; // If this is not null, this function is called
							// instead of inserting simply the text
	};
*/

public:
	CCLUScriptEditor(void);
	CCLUScriptEditor(int iPosX, int iPosY, int iWidth, int iHeight, 
					const char *pcFilename = 0, const char *pcExecPath = 0,
					void *pvIcon = 0);
	~CCLUScriptEditor(void);

	bool Create(int iPosX, int iPosY, int iWidth, int iHeight, 
				const char *pcFilename = 0, const char *pcExecPath = 0,
				void *pvIcon = 0);

	void SetExecPath(const char *pcExecPath)
	{ m_sExecPath = pcExecPath; }

	void SetOGLWin(COGLWin *poglWin)
	{ m_poglWin = poglWin; }

	void SetOutputWin(CFLTKOutput *pOutputWin)
	{ m_pOutputWin = pOutputWin; }

	void SetScript(const char* pcScript, bool bResetChangedFlag = false);

	void Parse(bool bResetEnv = false, bool bReparseLatex = false, 
				const char *pcPath = 0, const char *pcName = 0);

protected:
	void Help();
	void About();
	void InitStyle();

	void MenuInsertCode();
	void InsertColor();

	void MenuShowEditorWin();
	void MenuShowOutputWin();
	void MenuShowVisWin();
	void MenuParseMain(bool bResetEnv = false, bool bReparseLatex = false);

protected:
	// Overloaded virtual functions
	void InitMenu();
	void New();
	void Load();
	void Insert();
	void Close();
	void SaveAs();

	virtual void LoadNoExec();

	void InitStyleTable();
	void StyleParse(const char *pcText, char *pcStyle, int iLength);

	// Is called when text is pasted into editor with pasted text in sText.
	// If text need not be adapted returns false. If it returns true,
	// then the text in sNewText is inserted instead of original text.
	bool AdaptText(string &sNewText, string &sText);

protected:
	static void CB_LoadNoExec(Fl_Widget* pWidget, void *pData);

	static void CB_InsertCode(Fl_Widget *pWidget, void *pvData);

	static void CB_Parse(Fl_Widget *pWidget, void *pvData);
	static void CB_ParseForce(Fl_Widget *pWidget, void *pvData);
	static void CB_ParseMain(Fl_Widget *pWidget, void *pvData);
	static void CB_ParseMainForce(Fl_Widget *pWidget, void *pvData);
	static void CB_Help(Fl_Widget *pWidget, void *pvData);
	static void CB_About(Fl_Widget *pWidget, void *pvData);

	static void CB_NextScript(Fl_Widget* pWidget, void *pData);
	static void CB_PrevScript(Fl_Widget* pWidget, void *pData);
	static void CB_ReloadScript(Fl_Widget* pWidget, void *pData);
	static void CB_ClearScriptList(Fl_Widget* pWidget, void *pData);
	static void CB_ShowScriptList(Fl_Widget* pWidget, void *pData);
	static void CB_ScriptListWindow(Fl_Widget* pWidget, void *pData);

	static void CB_ShowEditorWin(Fl_Widget *pWidget, void *pvData);
	static void CB_ShowOutputWin(Fl_Widget *pWidget, void *pvData);
	static void CB_ShowVisWin(Fl_Widget *pWidget, void *pvData);

protected:
	COGLWin			*m_poglWin;
	CFLTKOutput		*m_pOutputWin;
	Fl_Button		*m_pFileParse;
	Fl_Button		*m_pFileParseForce;

	Fl_Help_Dialog *m_pHelpDialog;

	bool m_bParseAfterNew;

	string m_sAllowedChars;
	string m_sSymbolChars;
	string m_sStdSingleCharOps;
	string m_sSpcSingleCharOps;
	string m_sExecPath;
	map<string, char> m_mapHighlight;
	map<string, char> m_mapSymbol;
//	map<string, SCMenuData> m_mapCMenuItem;
};


#endif
