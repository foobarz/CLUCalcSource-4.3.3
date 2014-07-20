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


// OGLWin.cpp: Implementierung der Klasse COGLWin.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OGLWin.h"
#include <stdlib.h>
#include "CLUOutput.h"

#ifdef _GNUCPP3_
#   include <unistd.h>
#   define _getcwd getcwd
#   define _chdir chdir
#   define _stat stat
//#   define _remove remove
#else
#   include <direct.h>
#   include <io.h>
#endif

#ifdef WIN32
#	include <ShellApi.h>
#endif

#define BUFSIZE 1024


COGLWin *COGLWin::m_pThisWin = 0;


//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLWin::COGLWin()
{	
	m_bHasError = false;
	m_bHasOutput = false;
	m_bHasMessage = false;
	m_bInDisplay = false;

	m_bExecCode = false;
	m_bCodeChanged = true;	
	m_bOutputChanged = false;
	m_bScriptLoaded = false;
	m_bCheckForAnimate = false;
	m_bBGColChanged = false;
	m_bFrameTransChanged = false;
	m_bCodeChangedForAnim = false;
	m_bKeepBGColor = false;
	m_bPresentationAction = false;
	m_bParseFromOGLWin = false;
	m_bWaitWithIdleForNextDisplay = false;
	m_bCheckToolBoxSize = false;
	m_bScriptChangedOGLSize = false;
	m_bAnimPauseChanged = false;

	m_bShowWindowMenu = true;
	m_bVizOnlyMode = false;

	m_iAllowKeyUp = 0;

	m_dFrameBoxSize = 3.0;

	m_vRot = m_E3Base.vZero;

	m_pOutput = 0;
	m_pBrowserWin = 0;
	m_pBrowser = 0;
	m_pThisWin = this;

	m_iExecMode = 0;
	m_iCurToolCount = 0;
	m_dNewInfoWidth = -1.0;

	m_dNewOGLWidth = 0;
	m_dNewOGLHeight = 0;
}

COGLWin::~COGLWin()
{
	if (m_pBrowser)
		delete m_pBrowser;

	if (m_pBrowserWin)
		delete m_pBrowserWin;
	
	m_pBrowserWin = 0;
	m_pBrowser = 0;

	m_dlSpace.Destroy();
}


void COGLWin::CB_Quit(Fl_Widget* pWidget, void *pData)
{
	exit(0);
}


void COGLWin::InitMenu()
{
	Fl_Menu_Item pMenuItems[] = 
	{
		{ "&Code",              0, 0, 0, FL_SUBMENU },
		{ "&Parse",        FL_CTRL + 'p', (Fl_Callback *) CB_Parse },
		{ "Parse (Force Render Late&x)",  FL_CTRL + FL_SHIFT + 'p', (Fl_Callback *) CB_ParseForce },
		{ 0 },

		{ "&Presentation", 0, 0, 0, FL_SUBMENU },
		{ "Ne&xt",        FL_Page_Down, (Fl_Callback *) CB_NextScript },
		{ "Pre&vious",    FL_Page_Up, (Fl_Callback *) CB_PrevScript, 0, FL_MENU_DIVIDER },
//		{ "&Reload Current",    FL_Enter, (Fl_Callback *) CB_ReloadScript, 0, FL_MENU_DIVIDER },
		{ "&Show List...",    FL_CTRL + FL_Enter, (Fl_Callback *) CB_ShowScriptList },
		{ "&Clear List",    0, (Fl_Callback *) CB_ClearScriptList },
		{ 0 },

		{ "&Window",              0, 0, 0, FL_SUBMENU },
		{ "&Editor", FL_CTRL + FL_SHIFT + 'e', (Fl_Callback *) CB_ShowEditorWin },
		{ "&Output", FL_CTRL + FL_SHIFT + 'o', (Fl_Callback *) CB_ShowOutputWin },
		{ "&Visualization", FL_CTRL + FL_SHIFT + 'v', (Fl_Callback *) CB_ShowVisWin },
		{ 0 },

		{ 0 }
	};

	COGLWinFLTK::InitMenu();

	int iCnt;
	int iSize = m_vecMenuItem.size();

	if (m_bShowWindowMenu && !m_bVizOnlyMode)
	{
		iCnt = 15;
	}
	else
	{
		iCnt = 10;
		pMenuItems[10].text = 0;
	}

	m_vecMenuItem.resize(iSize + iCnt);

	for (int i = iSize-1; i < iSize+iCnt; i++)
	{
		m_vecMenuItem[i] = pMenuItems[i - iSize + 1];
	}
}

//////////////////////////////////////////////////////////////////////
/// Code Menu Callbacks

void COGLWin::MenuParse()
{
	if (m_pEditor)
	{
		m_bParseFromOGLWin = true;
		m_pEditor->Parse(false, false, m_sScriptPath.c_str(), m_sScriptName.c_str());
	}
}

void COGLWin::MenuParseForce()
{
	if (m_pEditor)
	{
		m_bParseFromOGLWin = true;
		m_pEditor->Parse(false, true, m_sScriptPath.c_str(), m_sScriptName.c_str());
	}
}

void COGLWin::CB_Parse(Fl_Widget *pWidget, void *pvData)
{
	if (pvData)
		((COGLWin *) pvData)->MenuParse();
}

void COGLWin::CB_ParseForce(Fl_Widget *pWidget, void *pvData)
{
	if (pvData)
		((COGLWin *) pvData)->MenuParseForce();
}


//////////////////////////////////////////////////////////////////////
/// Window Menu Callbacks

void COGLWin::MenuShowEditorWin()
{
	if (m_pEditor)
		m_pEditor->show();
}

void COGLWin::MenuShowOutputWin()
{
	if (m_pOutput)
		m_pOutput->show();
}

void COGLWin::MenuShowVisWin()
{
	ShowWindow();
}



void COGLWin::CB_ShowEditorWin(Fl_Widget *pWidget, void *pvData)
{
	if (pvData)
		((COGLWin *) pvData)->MenuShowEditorWin();
}

void COGLWin::CB_ShowOutputWin(Fl_Widget *pWidget, void *pvData)
{
	if (pvData)
		((COGLWin *) pvData)->MenuShowOutputWin();
}

void COGLWin::CB_ShowVisWin(Fl_Widget *pWidget, void *pvData)
{
	if (pvData)
		((COGLWin *) pvData)->MenuShowVisWin();
}



//////////////////////////////////////////////////////////////////////
/// Script List Menu Functions

void COGLWin::CB_NextScript(Fl_Widget* pWidget, void *pData)
{
	if (pData)
	{
		((COGLWin *)pData)->MenuNextScript(pWidget);
	}
}

void COGLWin::CB_PrevScript(Fl_Widget* pWidget, void *pData)
{
	if (pData)
	{
		((COGLWin *)pData)->MenuPrevScript(pWidget);
	}
}

void COGLWin::CB_ReloadScript(Fl_Widget* pWidget, void *pData)
{
	if (pData)
	{
		((COGLWin *)pData)->MenuReloadScript(pWidget);
	}
}

void COGLWin::CB_ClearScriptList(Fl_Widget* pWidget, void *pData)
{
	if (pData)
	{
		((COGLWin *)pData)->MenuClearScriptList(pWidget);
	}
}

void COGLWin::CB_ShowScriptList(Fl_Widget* pWidget, void *pData)
{
	if (pData)
	{
		((COGLWin *)pData)->MenuShowScriptList(pWidget);
	}
}


void COGLWin::CB_ScriptListWindow(Fl_Widget* pWidget, void *pData)
{
	if (pData)
	{
		((COGLWin *)pData)->ScriptListWindowEvent(pWidget);
	}
}

void COGLWin::MenuNextScript(Fl_Widget *pWidget)
{
	if (!m_pEditor)
		return;

	m_pEditor->SetCurEditor(m_Parse.GetCodeBase().GetCurScriptFilename().c_str());

	if (m_Parse.GetCodeBase().NextScriptFile() != -1 
		&& !m_bWaitWithIdleForNextDisplay)
	{
		// In presentations do not reset BGColor
		m_bKeepBGColor = true;
		m_bPresentationAction = true;

		m_pEditor->LoadFile(m_Parse.GetCodeBase().GetCurScriptFilename().c_str());
		m_pEditor->Parse(true);
		m_pEditor->UpdateFileList();
	}
}


void COGLWin::MenuPrevScript(Fl_Widget *pWidget)
{
	if (!m_pEditor)
		return;

	m_pEditor->SetCurEditor(m_Parse.GetCodeBase().GetCurScriptFilename().c_str());

	if (m_Parse.GetCodeBase().PrevScriptFile() != -1
		&& !m_bWaitWithIdleForNextDisplay)
	{
		// In presentations do not reset BGColor
		m_bKeepBGColor = true;
		m_bPresentationAction = true;
		m_pEditor->LoadFile(m_Parse.GetCodeBase().GetCurScriptFilename().c_str());
		m_pEditor->Parse(true);
		m_pEditor->UpdateFileList();
		return;
	}
}

void COGLWin::MenuReloadScript(Fl_Widget *pWidget)
{
	if (m_pEditor && !m_bWaitWithIdleForNextDisplay)
	{
		string sFilename;
		sFilename = m_Parse.GetCodeBase().GetCurScriptFilename();
		if (sFilename.size() > 0)
		{
			// In presentations do not reset BGColor
			m_bKeepBGColor = true;
			m_bPresentationAction = true;
		
			m_pEditor->LoadFile(sFilename.c_str());
			m_pEditor->Parse(true);
			m_pEditor->UpdateFileList();
		}
	}
}

void COGLWin::MenuClearScriptList(Fl_Widget *pWidget)
{
	if (m_pEditor)
	{
		m_Parse.GetCodeBase().ClearScriptList();
		SetTitle();
		return;
	}
}


void COGLWin::MenuShowScriptList(Fl_Widget *pWidget)
{
	if (!m_pBrowserWin || !m_pBrowser)
	{
		fl_alert("Script list windows not available.");
		return;
	}

	if (m_pEditor)
	{
		vector<CCLUCodeBase::SScriptFile> &vecScriptList = m_Parse.GetCodeBase().GetScriptList();
		int iCurScript = m_Parse.GetCodeBase().GetCurScriptIdx();
		int iScript, iScriptCount = vecScriptList.size();

		if (iScriptCount == 0)
		{
			fl_alert("No scripts in list.");
			return;
		}

		m_pBrowser->clear();
		for( iScript = 0; iScript < iScriptCount; iScript++ )
		{
			m_pBrowser->add(vecScriptList[iScript].sTitle.c_str(), (void *) iScript);
		}
		m_pBrowser->select(iCurScript+1);

		// In presentations do not reset BGColor
		m_bKeepBGColor = true;
		m_bPresentationAction = true;
		m_pBrowserWin->set_modal();
		m_pBrowserWin->show();

		return;
	}
}

void COGLWin::InitScriptListWindow()
{
	int iHeight = Fl::h() - 80;
	if (!(m_pBrowserWin = new Fl_Window(Fl::w() - 220, 40, 200, iHeight, 
		"Presentation Scripts")))
	{
		fl_alert("Could not create script list window.");
		return;
	}

	//m_pBrowserWin->set_modal();
	m_pBrowserWin->begin();

	if (!(m_pBrowser = new CScriptBrowser(0, 0, 200, iHeight)))
	{
		delete m_pBrowserWin;
		fl_alert("Could not create script browser window.");
		return;
	}

	m_pBrowserWin->end();
	m_pBrowser->callback(CB_ScriptListWindow, (void *) this);

	//m_pBrowserWin->set_modal();
	m_pBrowserWin->hide();
}


void COGLWin::ScriptListWindowEvent(Fl_Widget *pWidget)
{
	if (!m_pBrowserWin || !m_pBrowser || !m_pEditor)
		return;

	int iSel = m_pBrowser->value();
	if (iSel <= 0)
		return;

	m_pEditor->SetCurEditor(m_Parse.GetCodeBase().GetCurScriptFilename().c_str());

	m_Parse.GetCodeBase().SetCurScriptIdx(iSel - 1);

	if (Fl::event() != FL_KEYBOARD ||
		(Fl::event() == FL_KEYBOARD && 
		 Fl::event_key() == FL_Enter && 
		 !Fl::event_state(FL_CTRL)))
	{
		m_pBrowserWin->set_non_modal();
		m_pBrowserWin->hide();
		MenuReloadScript(0);
	}
	else
	{
		MenuReloadScript(0);
		m_pBrowserWin->show();
	}
}

//////////////////////////////////////////////////////////////////////
/// Init

bool COGLWin::Init()
{
	// OpenGL rendering context exists at this point.
	m_pWindow->callback((Fl_Callback *) COGLWin::CB_Quit, this);

	glutIdleFunc(CallIdleFunc);
/*
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, m_iOutSizeX, m_iOutSizeY, 0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
*/
	glutSetWindow(m_iHWin);
	////////////////////////////////////////////////////

	//COGLBitmapText::InitFonts();

	// Lighting has to be enabled BEFORE
	// you call CGLUTBase::Init() !!!
	EnableLighting();

	// Initialize OpenGL settings.
	CCLUDrawBase::Init();

	m_dlSpace.Create();

	// Can change sensitivity of mouse
	// for rotation...
	SetRotFac(0.5f); // default = 1.0

	// and for translation
	SetTransFac(0.02f);  // default = 0.05

	// Enable lighting for filter
	m_Filter.EnableLighting();

	// Reduce sensitivity
	m_Filter.SetSensitivity(1e-5);//1e-4);

	// Initialize Settings for E3Filter
	m_Filter.Init();

	// Initialize Parser
	if (!m_Parse.Init())
		return false;

	m_Parse.SetMVFilter(&m_Filter);
	m_Parse.SetTransform(&m_mTransform);
	m_Parse.SetFrameRotor(&m_vRMain);
	m_Parse.SetCLUDrawBase((CCLUDrawBase *) this);
	m_Parse.InitBitmapFont();

	InitScriptListWindow();

	//AddToolSlider("Mouse Mode", 0, 9, 1, 0, true, "Set the mouse mode here.");
	//AddToolSlider("Test2", -10, 2, 0.02, "World");

	SetTitle();

	return true;
}


//////////////////////////////////////////////////////////////////////
// Callback function of info window

const char* COGLWin::InfoLink(Fl_Widget* pWidget, const char *pcURL)
{
	if (strncmp(pcURL, "clu://", 6) == 0)
	{
		if (strncmp(&pcURL[6], "data/", 5) == 0)
		{
			string sData;

			sData = &pcURL[11];

			AddExecModeVar(EXEC_MODE_LINK);
			SetLinkDataVar(sData.c_str());

			if (!m_bIsAnimated)
				glutPostRedisplay();
		}
	}
	else if (strncmp(pcURL, "http://", 7) == 0 ||
			 strncmp(pcURL, "ftp://", 6) == 0)
	{
# ifdef WIN32
		HINSTANCE hInstance = ShellExecute(NULL,
			"open",
			pcURL,
			NULL,
			NULL,
			SW_SHOWNORMAL);//SHOWMINIMIZED);
		/*
		if ((uint) hInstance <= 32)
		fl_alert("Cannot start web-browser to view help manual.");
		*/
# else // Linux or other system
		char pcCmd[500];
		char pcLine[300];
		struct stat FileStat;
		ifstream zLog;
		char *pBrowser[] = { "firefox", "konqueror", "mozilla", "netscape" };
		int iPos, iCount = 3;

		// Check whether Mozilla is installed
		::remove("/tmp/clucalc_which.txt");

		for( iPos = 0; iPos < iCount; iPos++)
		{
			sprintf(pcCmd, "which %s > /tmp/clucalc_which.txt", pBrowser[iPos]);
			system(pcCmd);

			// Check whether text file exists
			if (stat("/tmp/clucalc_which.txt", &FileStat) == 0 && FileStat.st_size != 0)
			{
				// We found something
				break;
			}
		}

		::remove("/tmp/clucalc_which.txt");

		if (iPos == iCount)
		{
			// Could not find anything
			fl_alert("Could not find any HTML browser to display the URL.");
		}
		else
		{
			sprintf(pcCmd, "%s \"%s\" &", pBrowser[iPos], pcURL);
			system(pcCmd);
		}

# endif
	}

	return 0; // Do nothing when link is pressed
}



//////////////////////////////////////////////////////////////////////
/// Callback when tool has changed


void COGLWin::ToolChanged(string sName)
{
	//printf("%s\n", sName.c_str());
/*
	if (sName == "Mouse Mode")
	{
		CFLTKToolBox::STool theTool;

		if (m_ToolBox.GetTool(sName.c_str(), theTool))
		{
			Fl_Valuator *pValuator = (Fl_Valuator *) theTool.pWidget;

			CCLUDrawBase::Key('0' + char(pValuator->value()), 0, 0);
		}
	}
	else
	{
*/
	AddExecModeVar(EXEC_MODE_TOOL);
	SetToolNameVar(sName.c_str());

	if (!m_bIsAnimated)
		glutPostRedisplay();

//	printf("Tool\n\n");
			//Display();
//	}

	//if (!m_bIsAnimated)
	//	Display();
}



//////////////////////////////////////////////////////////////////////
/// Load Script and Display it.
/// Returns number of lines parsed. 
/// < 0 if error occured.
/// -1: general error
/// -2: error opening file
/// -3: Parse Error

int COGLWin::LoadScript()
{
	char pcText[BUFSIZE+1];
	int iRead, iLines;
	cStr csText;
	FILE *pFile;


  	if (!(pFile = fopen(m_csScriptFilename, "r")))
	{
		return -2;
	}
	

	while(!feof(pFile))
	{
		iRead = (int) fread(pcText, 1, BUFSIZE, pFile);
		pcText[iRead] = 0;
		csText << pcText;
	}

	fclose(pFile);

	ResetTransformVars();

	CCodeVar& rVarCol = m_Parse.GetVar("_BGColor");
	if (rVarCol.Type() != PDT_NOTYPE)
	{
		rVarCol = Black;
		SetBGColor(Black);
	}

	CCodeVar& rVarFBS = m_Parse.GetVar("_FrameBoxSize");
	if (rVarFBS.Type() != PDT_NOTYPE)
	{
		rVarFBS = TCVScalar(3);
	}

	m_Parse.Reset();
	iLines = m_Parse.InsertText(csText);

	if (iLines < 0)
	{
		return -3;
	}
	
	return iLines;
}

//////////////////////////////////////////////////////////////////////
/// Set Tool Name Variabel

void COGLWin::SetToolNameVar(const char *pcName)
{
	CCodeVar &rVar = m_Parse.GetVar("ToolName");

	if (rVar.Type() == PDT_STRING)
	{
		*((TString *) rVar.Val()) = (pcName == 0 ? "None" : pcName);
	}

}

//////////////////////////////////////////////////////////////////////
/// Set Link Data Variabel

void COGLWin::SetLinkDataVar(const char *pcData)
{
	CCodeVar &rVar = m_Parse.GetVar("LinkData");

	if (rVar.Type() == PDT_STRING)
	{
		*((TString *) rVar.Val()) = (pcData == 0 ? "None" : pcData);
	}

}

//////////////////////////////////////////////////////////////////////
/// Set Execution Mode

void COGLWin::SetExecModeVar(int iMode)
{
	CCodeVar &rVar = m_Parse.GetVar("ExecMode");

	if (rVar.Type() == PDT_INT)
	{
		*((int *) rVar.Val()) = iMode;
	}

	m_iExecMode = iMode;
}

//////////////////////////////////////////////////////////////////////
/// Add to Execution Mode

void COGLWin::AddExecModeVar(int iMode)
{
	CCodeVar &rVar = m_Parse.GetVar("ExecMode");

	if (rVar.Type() == PDT_INT)
	{
		*((int *) rVar.Val()) |= iMode;
	}

	m_iExecMode |= iMode;
}


//////////////////////////////////////////////////////////////////////
/// Set Script

int COGLWin::SetScript(const char *pcScript, bool bResetEnv, bool bReparseLatex)
{
	CCodeVar Var;

	//printf("Enter SetScript.\n");

	// Only execute checks on change of global variables
	// in idle loop after the next display operation.
	m_bWaitWithIdleForNextDisplay = true;

	SetTitle();

	if (bReparseLatex)
	{
		m_Parse.OverrideSetRenderLatex(false);
		m_Parse.SetRenderLatex(true);
		m_Parse.OverrideSetRenderLatex(true);
	}

	if (bResetEnv)
	{
		// Switch off animation
		SetAnimCode(false);
		COGLWinFLTK::StopAnimation();

		SetExecModeVar(EXEC_MODE_NEW);
		AddExecModeVar(EXEC_MODE_CHANGE);

		ResetTransformVars();
		ResetLights();

		m_ToolBox.ResetToolInUseFlags();
		m_ToolBox.PruneToolMap();

		if (!m_bKeepBGColor)
		{
			CCodeVar& rVarCol = m_Parse.GetVar("_BGColor");
			if (rVarCol.Type() != PDT_NOTYPE)
			{
				rVarCol = Black;
				SetBGColor(Black);
			}
		}
		else
		{
			m_bKeepBGColor = false;
		}

		CCodeVar& rVarFBS = m_Parse.GetVar("_FrameBoxSize");
		if (rVarFBS.Type() != PDT_NOTYPE)
		{
			rVarFBS = TCVScalar(3);
		}

		CCodeVar& rVar2dV = m_Parse.GetVar("_2dView");
		if (rVar2dV.Type() != PDT_NOTYPE)
		{
			rVar2dV = 0;		
		}
		Enable2dView(false);

		/// Reset visualization mode to OPNS for all spaces.
		m_Filter.SetMode(MODE_OPNS);

		// Reset Code Base
		m_Parse.ResetCodeBase();
	}
	else
	{
		SetExecModeVar(EXEC_MODE_CHANGE);
	}
	
	m_Parse.CollectGarbage();
	m_Parse.Reset();
	m_Parse.GetCodeBase().ResetInfoVars();

	SetInfoText("Use <code>SetInfoText([string])</code> to put some text here. "
				"The text may include some 'simple' HTML tags.");
	SetInfoWidth(0);

	bool bShowOutput = true;

	if (m_bInFullScreenMode || m_bVizOnlyMode)
	{
		bShowOutput = false;
	}

	if (m_pOutput && bShowOutput)
	{
		m_pOutput->SetText("<body size=\"16\"><b>Parsing code...</b></body>");
		m_pOutput->show();
	}

	int iLines = m_Parse.InsertText(pcScript);

	if (m_pOutput && bShowOutput)
	{
		m_pOutput->SetText("<body size=\"14\">Parsing code...<b>finished</b>.<br>Running code after change. <b>Please wait...</b></body>");
		m_pOutput->show();
	}

	if (!m_bInFullScreenMode)
	{
		if (m_bPresentationAction || m_bParseFromOGLWin || m_bVizOnlyMode)
		{
			ShowWindow();
		}
		else
			m_pEditor->show();
	}
	m_bPresentationAction = false;
	m_bParseFromOGLWin = false;

	if (iLines >= 0)
	{
		//printf("Parsed %d lines.\n", iLines);
		m_bCheckForAnimate = true;
		m_bCodeChanged = true;
		m_bCodeChangedForAnim = true;
		//glutSetWindow(m_iHWin);

		//m_pWindow->show();

		m_bCheckToolBoxSize = true;

		// When visualization window is iconized, the script is not executed
		// because of a glutPostRedisplay(). So we have to call Display explicitly.
		if (!m_pWindow->visible())
		{
			Display();
		}

//		if (!m_bIsAnimated)
//			glutPostRedisplay();
	}
	else
	{
		ClearError();
		m_sError = "<b>Parse Errors:</b><p>";
		m_sError += m_Parse.PrintParseErrors().Str();
		m_sError += "</p>";

		m_bHasError = true;

		if (m_pOutput && !m_bVizOnlyMode)
		{
			m_pOutput->SetText(m_sError.c_str());
			m_pOutput->show();
		}
		
		if (m_pEditor && !m_bVizOnlyMode)
		{
			cStr csFilename, csPath;
			int iLine, iPos, iInputPos;

			if (iLines == -1)
				m_Parse.GetParseErrorPos(iLine, iPos, iInputPos, csFilename);
			else
			{
				csFilename = m_Parse.GetErrorFilename().c_str();
				csPath = m_Parse.GetErrorPath().c_str();

				if (csFilename.Len() == 0)
				{
					csFilename = m_sScriptPath.c_str();
					csFilename << m_sScriptName.c_str() << ".clu";
				}

				char *pcName = csFilename.Str();
				if (!strchr(pcName, ':') &&
					!strchr(pcName, '/') &&
					!strchr(pcName, '\\'))
				{
					csFilename = csPath + csFilename;
					csFilename |= ('.' < csFilename) - csFilename.Len() - 1;
				}
				iInputPos = -iLines - 2;
			}

			m_pEditor->HighlightLinePos(csFilename.Str(), iInputPos);
			m_pEditor->show();
		}

		//ClearError();

		m_Parse.ClearError();

		SetAnimCode(false);
/*
		m_bCodeChanged = true;
		glutSetWindow(m_iHWin);

		if (!m_bIsAnimated)
			glutPostRedisplay();
*/
	}


	if (!m_bIsAnimated)
		glutPostRedisplay();

//	CheckToolBoxSize();

//	if (iLines >= 0 && !m_bIsAnimated)
//		glutPostRedisplay();

	return iLines;
}

//////////////////////////////////////////////////////////////////////
/// Reload Script

int COGLWin::CheckScriptChanged()
{
  // Not implemented yet

  return -1;
}

//////////////////////////////////////////////////////////////////////
/// Check Key inputs

void COGLWin::Key(unsigned char cKey, int iX, int iY)
{
  /// Reload Script
/*
	if (cKey == 'u' || cKey == 'U')
	{
	  m_bScriptLoaded = false;
	  return;
	}
*/
	// Avoid ending program through 'q' keystroke.
/*
	if (cKey == 'q' || cKey == 'Q')
		return;


	COGLWinFLTK::Key(cKey, iX, iY);
*/
	char pcKey[2];

	pcKey[0] = cKey;
	pcKey[1] = 0;

	AddExecModeVar(EXEC_MODE_KEYUP);
	SetToolNameVar(pcKey);

	if (!m_bIsAnimated)
		glutPostRedisplay();
}

void COGLWin::KeyUp(unsigned char cKey)
{
	if (m_iAllowKeyUp > 0)
	{
		char pcKey[2];

		pcKey[0] = cKey;
		pcKey[1] = 0;

		--m_iAllowKeyUp;

		AddExecModeVar(EXEC_MODE_KEYUP);
		SetToolNameVar(pcKey);

		if (!m_bIsAnimated)
			glutPostRedisplay();
	}
}


void COGLWin::KeyDown(unsigned char cKey)
{
	char pcKey[2];

	pcKey[0] = cKey;
	pcKey[1] = 0;

	// Key Up should only occur after Key Down
	++m_iAllowKeyUp; 

	AddExecModeVar(EXEC_MODE_KEYDOWN);
	SetToolNameVar(pcKey);

	if (!m_bIsAnimated)
		glutPostRedisplay();
}



//////////////////////////////////////////////////////////////////////
/// Call Display Output

void COGLWin::CallDisplayOutput()
{
	if (m_pThisWin)
		m_pThisWin->DisplayOutput();
}

//////////////////////////////////////////////////////////////////////
/// Call Reshape Output

void COGLWin::CallReshapeOutput(int iWidth, int iHeight)
{
	if (m_pThisWin)
		m_pThisWin->ReshapeOutput(iWidth, iHeight);
}
//////////////////////////////////////////////////////////////////////
/// Call Idle Func

void COGLWin::CallIdleFunc()
{
	if (m_pThisWin)
		m_pThisWin->IdleFunc();
}


//////////////////////////////////////////////////////////////////////
/// Reshape

void COGLWin::ReshapeOutput(int iWidth, int iHeight)
{
	glViewport(0, 0, m_iOutSizeX = iWidth, m_iOutSizeY = iHeight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, m_iOutSizeX, m_iOutSizeY, 0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//////////////////////////////////////////////////////////////////////

void COGLWin::Sleep(unsigned int uMilliSec)
{
#ifdef WIN32
	::Sleep(uMilliSec);
#else
	timespec waittime;
	
	waittime.tv_sec = uMilliSec/1000;
	waittime.tv_nsec = long(1000000) * long(uMilliSec%1000);
		
	nanosleep(&waittime, 0);
#endif
}

//////////////////////////////////////////////////////////////////////
/// Reshape Windpw

void COGLWin::Reshape(int iWidth, int iHeight)
{
	COGLWinFLTK::Reshape(iWidth, iHeight);

	AddExecModeVar(EXEC_MODE_RESHAPE);
}

//////////////////////////////////////////////////////////////////////
// Set Size of OGL Window in pixel or percent

bool COGLWin::SetOGLSize(double dWidth, double dHeight)
{
	m_bScriptChangedOGLSize = true;
	m_dNewOGLWidth = dWidth;
	m_dNewOGLHeight = dHeight;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Info Window Width
/// Called by script

bool COGLWin::SetInfoWidth(double dWidth)
{
	m_dNewInfoWidth = dWidth;
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Idle Func

void COGLWin::IdleFunc()
{

	bool bRedisplay = false;

	if (m_bHasOutput && m_pOutput)
	{
		m_pOutput->SetOutputObjectList(m_Parse.GetOutputObjectList());

		//m_sOutput.insert(0, "<body size=\"14\">");
		//m_sOutput += "</body>";

		//m_pOutput->SetText(m_sOutput.c_str());
		// Uncomment this block in order to bring output window to front 
		// when new output text is available.
		/*
		if (!m_bInFullScreenMode && (!m_pOutput->shown() ||
		(m_pOutput->shown() && !m_pOutput->visible())))
		m_pOutput->show();
		*/
		ClearOutput();
	}

	if (m_bInFullScreenMode)
	{
		if (m_pOutput && m_pOutput->shown())
			m_pOutput->iconize();
	}

	// Have to check global variables of script only after script
	// has been executed once.
	if (!m_bWaitWithIdleForNextDisplay)
	{
		/// If script can be animated then animate it.
		if (m_bCheckForAnimate)
		{
			m_bCheckForAnimate = false;

			if (!m_bIsAnimated && CanAnimateScript())
			{
				m_bAnimPauseChanged = false; // Set by SetAnimationTimeStep
				StartAnimation(m_iAnimPause);
				SetAnimCode(true);
				m_sMessage += "Switching to animation mode...\n";
				m_bHasMessage = true;
			}
			else
			{
				m_bCodeChangedForAnim = false;
			}

		}

		if (m_bCheckToolBoxSize)
		{
			m_ToolBox.Update();
			CheckToolBoxSize();
			m_bCheckToolBoxSize = false;
		}

		if (m_dNewInfoWidth >= 0)
		{
			COGLWinFLTK::SetInfoWidth(m_dNewInfoWidth);
			m_dNewInfoWidth = -1.0;
		}

		if (m_bScriptChangedOGLSize)
		{
			COGLWinFLTK::SetOGLSize(m_dNewOGLWidth, m_dNewOGLHeight);
			m_bScriptChangedOGLSize = false;
		}
	}
/*
	else
	{
		printf("Waiting to check global vars.\n");
	}
*/

	Sleep(1);

}

void COGLWin::SetAnimCode(bool bDoAnim)
{
	if (bDoAnim)
	{
		m_bAnimCode = true;
//		m_pMenuBar->value(1);
//		((Fl_Menu_Item *) m_pMenuBar->mvalue())->check();
	}
	else
	{
//		m_pMenuBar->value(1);
//		((Fl_Menu_Item *) m_pMenuBar->mvalue())->uncheck();
		m_bAnimCode = false;
	}
}

//////////////////////////////////////////////////////////////////////
/// Display Output

void COGLWin::DisplayOutput()
{
	/*
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	int i;
	int iLine = m_iOutLineHeight;
	int iLen = m_csOutput.Len();
	char *pcText = m_csOutput.Str();
	unsigned char cSym;

	if (!iLen || !pcText)
		return;

	glRasterPos2f(0, iLine);

	for(i=0;i<iLen;i++)
	{
		cSym = pcText[i];

		if (cSym == 10)
		{
			iLine += m_iOutLineHeight;
			glRasterPos2f(0, iLine);
		}
		else if (cSym >= 32 && cSym < 255)
		{
			glutBitmapCharacter(m_pOutputFont, cSym);
		}
	}

	glutSwapBuffers();
	*/
}


//////////////////////////////////////////////////////////////////////
/// Display

void COGLWin::Display()
{
	//printf("Display\n");
	if (m_bInDisplay)
		return;

	m_bInDisplay = true;

	bool bRedisplay = false;
	//printf("Enter Display\n");

	// Always call this function before drawing

	// Now we display, so we can allow idle loop to check script.
	m_bWaitWithIdleForNextDisplay = false;

	// Reset Perspective Angle to standard
	SetPerspectiveAngle(70);

	BeginDraw();

	// Save Frame Transform 
	float fFrameAngle = m_fFrameAngle;
	STransform Trans = m_mTransform[0];
	bool bRedraw = false;
	bool bEnable2dView = false;
	bool b2dViewChanged = false;
	int iActRTMode = GetActRTMode();

	if (iActRTMode && m_mTransform[iActRTMode].iFlags)
		AddExecModeVar(EXEC_MODE_USER);

	if (m_bCodeChanged)
	{
		bRedraw = true;
		m_bCodeChanged = false;
	}
	else
	{
		for(int i=1;i<=9;i++)
		{
			if (m_mTransform[i].iFlags)
			{
				bRedraw = true;
				break;
			}
		}
		
		if (m_iExecMode & EXEC_MODE_TOOL ||
			m_iExecMode & EXEC_MODE_LINK ||
			m_iExecMode & EXEC_MODE_KEYUP ||
			m_iExecMode & EXEC_MODE_KEYDOWN)
			//||
			//m_iExecMode & EXEC_MODE_RESHAPE)
		{
			bRedraw = true;
			m_bCheckForAnimate = true;
		}

//		if (!m_mTransform[0].iFlags)
//			bRedraw = true;	
	}

	if (HasError() && !bRedraw)
	{
		// Draw Frame Box (optional)
		if (m_dFrameBoxSize > 0.0 && !m_b2dView)
			DrawBox((float)m_dFrameBoxSize);

		EndDraw();
		//SetExecModeVar(EXEC_MODE_NONE);
		m_bInDisplay = false;
		return;
	}

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GEQUAL, 1.0f);

	// Check whether only space has been transformed
	if (bRedraw)
	{		
		CCodeVar Var;

		// Start drawing into display list
		m_dlSpace.BeginDraw(true);
		
		// Draw Frame Box (optional)
		// DrawBox(3.0);
		
		// Draw you own stuff
		Draw();
	
		// End drawing into display list
		m_dlSpace.EndDraw();

		if (!HasError())
		{
			// Check whether Frame Box is to be drawn and in what size
			if (m_Parse.GetVar("_FrameBoxSize", Var))
			{	
				TCVScalar dVal;
				if (Var.CastToScalar(dVal))
					m_dFrameBoxSize = dVal;
			}
			else m_dFrameBoxSize = 3.0;

			// Set Background Color
			if (m_Parse.GetVar("_BGColor", Var))
			{	
				if (Var.Type() == PDT_COLOR)
				{
					COGLColor colBG = *((COGLColor *) Var.Val());
					if (!(colBG == GetBGColor()))
						bRedisplay = true;
//						m_bBGColChanged = true;

					SetBGColor(colBG);
				}
			}
			//else SetBGColor(Black);

			if (!m_b2dView && ScriptIs2d())
			{
				//printf("Switch to 2d view.\n");

				bEnable2dView = true;
				b2dViewChanged = true;
				//m_bCodeChanged = true;
				//SetExecModeVar(EXEC_MODE_AGAIN);

				bRedisplay = true;
			}
			else if (m_b2dView && !ScriptIs2d())
			{	
				//printf("Switch to 3d view.\n");

				bEnable2dView = false;
				b2dViewChanged = true;
				//m_bCodeChanged = true;
				//SetExecModeVar(EXEC_MODE_AGAIN);

				bRedisplay = true;
			}

			if (m_bBGColChanged)
			{
				//printf("BGCol. changed.\n");

				//m_bBGColChanged = false;
				//SetExecModeVar(EXEC_MODE_AGAIN);

				bRedisplay = true;
			}

			if (m_bFrameTransChanged)
			{
				m_bFrameTransChanged = false;
				//SetExecModeVar(EXEC_MODE_AGAIN);

				bRedisplay = true;
			}

			if (bRedisplay)
			{
				//printf("Redisplay\n");
				// End Draw without swapping buffers
				CCLUDrawBase::EndDraw();

				if (b2dViewChanged)
					Enable2dView(bEnable2dView);

				// Now begin drawing again
				BeginDraw();
				fFrameAngle = m_fFrameAngle;
				Trans = m_mTransform[0];

				glEnable(GL_ALPHA_TEST);
				glAlphaFunc(GL_GEQUAL, 1.0f);
				m_dlSpace.Apply();
			}

		}

		// Draw Frame Box (optional)
		if (m_dFrameBoxSize > 0.0 && !m_b2dView)
			DrawBox((float)m_dFrameBoxSize);
	}
	else
	{
		// Apply display list
		m_dlSpace.Apply();

		// Draw Frame Box (optional)
		if (m_dFrameBoxSize > 0.0 && !m_b2dView)
			DrawBox((float)m_dFrameBoxSize);
	}


	if (!HasError())
	{
		// Get ModelView Identity Matrix back
		glPopMatrix(); 
		// Save it again
		glPushMatrix();
		// Set initial origin again
		glTranslatef(Trans.pfTrans[0], Trans.pfTrans[1], Trans.pfTrans[2]);

		if (m_bUseLocalRot)
		{
			glRotatef(Trans.pfRot[2], 1, 0, 0);
			glRotatef(Trans.pfRot[0], 0, 1, 0);
			glRotatef(Trans.pfRot[1], 0, 0, 1);
		}
		else
		{
			glRotatef(fFrameAngle, Trans.pfRot[0], Trans.pfRot[1], Trans.pfRot[2]);
		}

		glEnable(GL_BLEND);
		glAlphaFunc(GL_LESS, 1.0f);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(GL_FALSE);

		m_dlSpace.Apply();
		
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
	}
	glDisable(GL_ALPHA_TEST);
	//m_Filter << m_E3Base.vI;

	// Always call this function after drawing
	EndDraw();
	

	//CheckToolBoxSize();
	m_bCheckToolBoxSize = true;
	
	//SetExecModeVar(EXEC_MODE_NONE);
	m_bInDisplay = false;
}


//////////////////////////////////////////////////////////////////////
/// Check whether the size of the tool box should be enlarged or reduced.

void COGLWin::CheckToolBoxSize()
{
	int iX, iY, iWidth, iHeight, iDeltaHeight;
	int iToolCount = m_ToolBox.GetToolCount();

	iX = iY = 0;
	iWidth = iHeight = 0;
	iDeltaHeight = 0;

	if (m_iCurToolCount == 0 && iToolCount > 0)
	{
		m_ToolBox.GetPosition(iX, iY);
		m_ToolBox.GetSize(iWidth, iHeight);
		iDeltaHeight = iHeight - m_iMinToolHeight - m_iToolHeight;
/*
		printf("Add Toolbar\n");
		printf("iX, iY: %d, %d\n", iX, iY);
		printf("iWidth, iHeight: %d, %d\n", iWidth, iHeight);
		printf("m_iMinToolHeight, m_iToolHeight: %d, %d\n", m_iMinToolHeight, m_iToolHeight);
		printf("iDeltaHeight: %d\n", iDeltaHeight);
		printf("m_iSizeX, m_iSizeY: %d, %d\n\n", m_iSizeX, m_iSizeY);
*/
		m_pTile->position(m_iToolBoxInitX, m_iToolBoxInitY, iX, iY + iDeltaHeight);
		CCLUDrawBase::Reshape(m_iSizeX, m_iSizeY + iDeltaHeight);

//		printf("New m_iSizeX, m_iSizeY: %d, %d\n\n", m_iSizeX, m_iSizeY);
	}
	else if (m_iCurToolCount > 0 && iToolCount == 0)
	{
		m_ToolBox.GetPosition(iX, iY);
		m_ToolBox.GetSize(iWidth, iHeight);
		iDeltaHeight = iHeight - m_iMinToolHeight;
/*
		printf("Remove toolbar\n");
		printf("iX, iY: %d, %d\n", iX, iY);
		printf("iWidth, iHeight: %d, %d\n", iWidth, iHeight);
		printf("m_iMinToolHeight, m_iToolHeight: %d, %d\n", m_iMinToolHeight, m_iToolHeight);
		printf("iDeltaHeight: %d\n", iDeltaHeight);
		printf("m_iSizeX, m_iSizeY: %d, %d\n\n", m_iSizeX, m_iSizeY);
*/
		m_pTile->position(m_iToolBoxInitX, m_iToolBoxInitY, iX, iY + iDeltaHeight);		
		CCLUDrawBase::Reshape(m_iSizeX, m_iSizeY + iDeltaHeight);

//		printf("New m_iSizeX, m_iSizeY: %d, %d\n\n", m_iSizeX, m_iSizeY);
	}
	
	m_iCurToolCount = iToolCount;	
}


//////////////////////////////////////////////////////////////////////
/// Draw
void COGLWin::Draw()
{

	if (!m_Parse.IsCodeOK())
	{
		m_ToolBox.ResetToolInUseFlags();
		// Keep fixed tools but do not redraw here
		m_ToolBox.PruneToolMap(true, false);
		// Redraw tools here explicitly
//		m_ToolBox.Update();

		if (m_pOutput)
			m_pOutput->SetText(" ");
	
		// Standard reason for re-evaluation of script.
		SetExecModeVar(EXEC_MODE_NONE);

		return;
	}

	// Set standard color
	if (m_bUseLighting)
	{
		m_Filter.SetColor(MGray);
		m_StdMaterial.Apply();
	}
	else
	{
		m_Filter.SetColor(MGray);
	}

	m_ToolBox.ResetToolInUseFlags();
	m_Parse.ToggleTempImageList();
	m_Parse.ResetTempImageList();

	// Reset Filter Modes
	m_Filter.ResetModes();
	m_bFrameTransChanged = false;
	MultiV<float> vR(m_vRMain);

	m_mTransform[0].ResetTransChanged();

	ClearOutput();
	ClearError();
	//m_Parse.ResetTempVexList();

	if (!m_Parse.RunCode())
	{
		int iErrLine, iErrPos, iErrInPos;
		cStr csErrFile, csText;
		CCLUCodeBase::SOutputObject OutObj;
/*
		m_sError = "<b>Runtime Errors:</b><p>";
		m_sError += m_Parse.PrintCodeErrors().Str();
		m_sError += "</p> <p>";
		//csText << "Vars:\n" << m_Parse.PrintVars() << "\n";
		m_sError += "<b>Stack:</b><br> <br>";
		m_sError += m_Parse.PrintStack().Str();
		m_sError += "</p>";
*/

		OutObj.sName = "Output up to Error";
		OutObj.bCanChangeSize = false;
		OutObj.bCanHideValue = false;
		OutObj.bBRAfterValue = true;
		OutObj.bBRBeforeValue = true;
		OutObj.bShowInfo = false;
		OutObj.bShowValue = true;
		OutObj.sRelSym = ":";
		OutObj.sType = " ";
		OutObj.iSize = 0;
		OutObj.sValue = " ";
		m_Parse.InsertOutputObject(OutObj);

		OutObj.sName = "Stack";
		OutObj.bCanChangeSize = true;
		OutObj.bCanHideValue = true;
		OutObj.bBRAfterValue = true;
		OutObj.bBRBeforeValue = true;
		OutObj.bShowInfo = false;
		OutObj.bShowValue = false;
		OutObj.sRelSym = ":";
		OutObj.sType = "stack";
		OutObj.iSize = 0;
		OutObj.sValue = m_Parse.PrintStack().Str();
		m_Parse.InsertOutputObject(OutObj);

		OutObj.sName = "Runtime Error";
		OutObj.bCanChangeSize = false;
		OutObj.bCanHideValue = true;
		OutObj.bBRAfterValue = true;
		OutObj.bBRBeforeValue = true;
		OutObj.bShowInfo = true;
		OutObj.bShowValue = true;
		OutObj.sRelSym = " ";
		OutObj.sType = "error message";
		OutObj.iSize = 0;

		m_Parse.GetCodeErrorPos(iErrLine, iErrPos, iErrInPos, csErrFile);
		csText = "";
		csText << "Line " << iErrLine << ", Pos " << iErrPos;
		OutObj.sInfo = csText;

		csText = "<font size=\"10\">File: <i>";
		csText << csErrFile << "</i></font><p>";
		csText << m_Parse.PrintCodeErrors().Str();
		OutObj.sValue = csText;

		m_Parse.InsertOutputObject(OutObj);

		m_bHasError = true;
		m_bHasOutput = true;

/*
		cStr &rText = m_Parse.GetTextOutput();

		if (rText.Len())
		{
			m_sError += "<br><b>Script Output:</b><br> <br>";
			m_sError += rText.Str();
		}
*/
/*
		if (m_pOutput && !m_bVizOnlyMode)
		{
			m_pOutput->SetText(m_sError.c_str());
			m_pOutput->show();
		}
*/		//ClearError();

		if (m_pEditor && !m_bVizOnlyMode)
		{
			cStr csFilename;
			int iLine, iPos, iInputPos;

			if (m_Parse.GetCodeErrorPos(iLine, iPos, iInputPos, csFilename))
				m_pEditor->HighlightLinePos(csFilename.Str(), iInputPos);

			m_pEditor->show();
		}

		m_Parse.ResetErrors();

		m_bExecCode = false;
		SetAnimCode(false);
		StopAnimation();

		m_Parse.OverrideSetRenderLatex(false);
	}
	else 
	{
		//m_vecOutObj = m_Parse.GetOutputObjectList();
		//cStr &rText = m_Parse.GetTextOutput();

		m_bHasOutput = true;

		if (m_mTransform[0].HasTransChanged(0) ||
			m_mTransform[0].HasTransChanged(1) ||
			m_mTransform[0].HasTransChanged(2))
		{
			m_mTransform[0].iFlags = 0;
			m_bFrameTransChanged = true;
		}

		if (vR != m_vRMain)
		{
			m_bFrameTransChanged = true;
		}

		m_Parse.OverrideSetRenderLatex(false);
	}

	// Keep fixed tools but do not redraw here
	m_ToolBox.PruneToolMap(true, false);

	// Redraw tools here explicitly
//	m_ToolBox.Update();

	// Reset Tool Name Variable
	if (m_iExecMode & 
		(EXEC_MODE_TOOL | EXEC_MODE_LINK | EXEC_MODE_KEYUP | EXEC_MODE_KEYDOWN | EXEC_MODE_ANIM))
	{
		SetToolNameVar("None");
		SetLinkDataVar("None");
	}

	// Standard reason for re-evaluation of script.
	SetExecModeVar(EXEC_MODE_NONE);
}

//////////////////////////////////////////////////////////////////////
// At Animation Start
bool COGLWin::AtAnimStart()
{	
	CCodeVar Var;
	TCVScalar dVal;

	if (m_Parse.GetVar("_DoAnimate", Var))
	{
		if (Var.CastToScalar(dVal) && dVal != 0)
		{
			m_bExecCode = true;
			SetAnimCode(true);
		}
		else
		{
			m_bExecCode = false;
			SetAnimCode(false);
		}
	}
	else 
	{
		//m_Parse.NewVar("_DoAnimate") = 1;
		SetAnimCode(true);
		m_bExecCode = false;
	}
		
//	COGLWinFLTK::AtAnimStart();

	return true;
}

/////////////////////////////////////////////////////////////////////
/// Set Animation Time Step

bool COGLWin::SetAnimationTimeStep(int iTimeStep)
{
	if (!CCLUDrawBase::SetAnimationTimeStep(iTimeStep))
		return false;

	m_bAnimPauseChanged = true;
	return true;
}

/////////////////////////////////////////////////////////////////////
/// Timed Func
bool COGLWin::TimedFunc()
{	
	CCodeVar Var;
	bool bCodeChanged = m_bCodeChanged;

	if (m_bExecCode)
	{
		CCodeVar &rVarDTime = m_Parse.GetVar("dTime");
		if (rVarDTime.Type() == PDT_DOUBLE)
		{
			*((double *) rVarDTime.Val()) = (double) m_fTimeStep;
		}

		CCodeVar &rVarTime = m_Parse.GetVar("Time");
		if (rVarTime.Type() == PDT_DOUBLE)
		{
			*((double *) rVarTime.Val()) = (double) m_fTotalTime;
		}

		// Set this true to rerun code
		m_bCodeChanged = true;
	}

	if (m_iAnimRotType == ANIMROT_MOUSE)
	{	 
		if (HasMouseRotHistChanged())
		{
			m_vRot = GetMouseRotHistSum();
			//ResetMouseRotHist();
		}
	 
		float fVal = 2e-2f * float(sqrt(m_vRot.Mag2()));
		float fAngle = m_fTimeStep * fVal;

		if (fAngle != 0.0f && m_vRot != m_E3Base.vZero)
		{
			if (!m_bUseLocalRot)
				m_vRMain = m_E3Base.Rotor(m_vRot, fAngle, false) & m_vRMain;
			else
				m_vRMain = m_vRMain & m_E3Base.Rotor(m_vRot, fAngle, false);
		
			m_mTransform[0].SetRotChanged(0);
			m_mTransform[0].SetRotChanged(1);
			m_mTransform[0].SetRotChanged(2);
		}
	}

	if (m_bCodeChangedForAnim)
	{
		SetExecModeVar(EXEC_MODE_CHANGE);
		m_bCodeChangedForAnim = false;
	}
	else
		AddExecModeVar(EXEC_MODE_ANIM);

	Display();

	if (bCodeChanged || m_bExecCode)
	{
		TCVScalar dVal;

		if (m_Parse.GetVar("_DoAnimate", Var))
		{
			if (Var.CastToScalar(dVal) && dVal != 0)
			{
				if (m_bAnimPauseChanged)
				{
					m_bAnimPauseChanged = false; // Set by SetAnimationTimeStep
					/*
					SetAnimCode(false);
					StopAnimation();
					StartAnimation(m_iAnimPause);
					*/
					SetAnimCode(true);
					m_bExecCode = true;
				}
				else
				{
					m_bExecCode = true;
					SetAnimCode(true);
				}
			}
			else
			{
				m_bExecCode = false;
				SetAnimCode(false);
				StopAnimation();
			}
		}
		else 
		{
			m_bExecCode = false;
			SetAnimCode(false);
			StopAnimation();
		}
	}

	return true;
}


////////////////////////////////////////////////////////
/// Set Title

bool COGLWin::SetTitle()
{
	m_sTitle = "CLUCalc v" CLUCALC_VERSION " by C. Perwass - ";

	string sVal = m_Parse.GetCodeBase().GetCurScriptTitle();
	string sFile = m_Parse.GetCodeBase().GetCurScriptFilename();
	string sCurFile = m_sScriptName + ".clu";
	if (sVal.size() > 0 && sFile.size() > 0 && 
		sFile.find(sCurFile) != string::npos)
	{
		m_sTitle += sVal;
	}
	else
	{
		if (m_sScriptName.size() == 0) 
		{
			m_sTitle += "Untitled";
		}
		else 
		{
			m_sTitle += m_sScriptName;
		}
	}

	m_pWindow->label(m_sTitle.c_str());

	return true;
}



