/// LICENSE START
////////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the CLUDraw Library.
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


// OGLWinFLTK.h: Schnittstelle für die Klasse COGLWinFLTK.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_OGLWINFLTK_H__INCLUDED_)
#define _OGLWINFLTK_H__INCLUDED_

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000

#include <FL/glut.H>
#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Single_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Tile.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Round_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Help_View.H>

#include "time.h"
#include "sys/timeb.h"

#include "CLUDrawBase.h"
#include "IOGLWinBase.h"
#include "FLTKToolBox.h"

#include <map>
#include <string>
#include <vector>

using namespace std;

#ifdef WIN32
	// Macro that is used by Windows and only confuses us here.
	#ifdef CreateWindow
		#undef CreateWindow
	#endif
	typedef struct _timeb TTimeStruct;
	#define GETTIME(S) _ftime(S)
#else
	typedef struct timeb TTimeStruct;
	#define GETTIME(S) ftime(S)
#endif

/*
struct SGLUTWin
{
	char *pcName;
	int iPosX, iPosY;
	int iSizeX, iSizeY;
	unsigned int uMode;

	SGLUTWin() 
	{ 
		pcName = "CLUDraw v1.1 by C.Perwass";
		iPosX = 100; iPosY = 100;
		iSizeX = 500; iSizeY = 500;
		uMode = GLUT_DOUBLE | GLUT_RGB;
	}
};
*/

class COGLWinFLTK : public CCLUDrawBase 
{
public:

	enum EToolType 
	{ 
		TOOL_NONE = 0,
		TOOL_SLIDER,
		TOOL_CHECKBOX
	};

	struct STool
	{
		EToolType eType;
		Fl_Widget *pWidget;
		Fl_Group *pBox;
		string sName;
		string sDesc;

		bool bInUse;
		bool bFixed;
	};


	typedef map<string, STool> TToolMap;

	class CKeyWindow : public Fl_Window
	{
	public:
		CKeyWindow(COGLWinFLTK *pWin, int iX, int iY, int iW, int iH, const char* pcTitle = 0)
			: Fl_Window(iX, iY, iW, iH, pcTitle)
		{ 
			m_pOGLWin = pWin;
		}

		virtual int handle(int iEvent)
		{
			//int iX, iY, iW, iH;
			Fl_Window *pWin = m_pOGLWin->GetGLUTChildWindow();
/*
			iX = x() + pWin->x();
			iY = y() + pWin->y();
			iW = pWin->w();
			iH = pWin->h();
*/
			if (m_pOGLWin &&
				!Fl::event_alt() &&
				!Fl::event_ctrl() && 
				Fl::event_inside(pWin))
			{
				if (iEvent == FL_KEYUP)
				{
					//Fl::event_inside(iX, iY, iW, iH);
					int iKey = Fl::event_key();
					if (iKey >= 32 && iKey <= 127)
					{
						m_pOGLWin->KeyUp((unsigned char) Fl::event_text()[0]);
						return 1;
					}
				}
				else if (iEvent == FL_KEYDOWN)
				{
					int iKey = Fl::event_key();
					if (iKey >= 32 && iKey <= 127)
					{
						m_pOGLWin->KeyDown((unsigned char) Fl::event_text()[0]);
						return 1;
					}
				}
			}

			return Fl_Window::handle(iEvent);
		}

	protected:
		COGLWinFLTK *m_pOGLWin;
	};

	friend class CKeyWindow;

public:
	COGLWinFLTK();
	virtual ~COGLWinFLTK();
		
	// Enter Main Loop
	void Run();
	void GetModifiers();
	
	bool IsCreated() { if (m_iHWin != 0) return true; return false; }

	void NotifyDestroy() { m_bNotifyDestroy  = true; }
	bool ShouldDestroy() { return m_bNotifyDestroy; }

	void ShowWindow(bool bVal = true);
	void HideMenu(bool bVal = true);

	Fl_Glut_Window* GetGLUTChildWindow() { return m_pGLUTWindow; }

	// Tool Functions
	// Overloaded functions from CLUDrawBase
	virtual bool GetToolValue(const char *pcName, double &rVal);
	virtual bool GetToolValue(const char *pcName, string &rVal);
	virtual bool AddToolSlider(const char *pcName, double dMin, double dMax, double dStep,
								double dVal, bool bFixed = false, const char *pcDesc = 0);
	virtual bool AddToolCounter(const char *pcName, double dMin, double dMax, double dStep,
								double dLStep, double dVal, bool bFixed = false, const char *pcDesc = 0);
	virtual bool AddToolInput(const char *pcName, double dMin, double dMax,
								double dVal, bool bFixed = false, const char *pcDesc = 0);
	virtual bool AddToolInputText(const char *pcName, bool bImmediateCallback, const char* pcInitText,
									bool bFixed = false, const char *pcDesc = 0, bool bDoUpdate = true);
	virtual bool AddToolCheckBox(const char *pcName, bool bSet, bool bFixed = false, 
								const char *pcDesc = 0);
	virtual bool AddToolButton(const char *pcName, bool bFixed = false, 
								const char *pcDesc = 0);
	virtual bool AddToolChoice(const char *pcName, vector<string>& vecData, const char* pcCoice,
						bool bFixed = false, const char *pcDesc = 0, bool bDoUpdate = true);

	virtual bool SetMouseMode(int iMode);
	virtual bool SetOGLSize(double dWidth, double dHeight);

	// Info Window Functions
	virtual bool SetInfoText(const char *pcText);
	virtual bool SetInfoWidth(double dWidth);
	virtual const char* InfoLink(Fl_Widget* pWidget, const char *pcURL);


	virtual bool Create(int *piArg = 0, char **pcArg = 0, 
		char *pcName = "CLUDraw v1.1 by C. Perwass",
		int iPosX = 100, int iPosY = 100,
		int iSizeX = 500, int iSizeY = 500, 
		unsigned int umode = FL_DOUBLE | FL_RGB | FL_DEPTH );

	/// Create a GLUT window without calling glutInit.
	/// This can be used if GLUT has already been
	/// initialized by this process.
	virtual bool CreateWindow(
		const char *pcName = "CLUDraw v1.1 by C. Perwass",
		int iPosX = 100, int iPosY = 100,
		int iSizeX = 500, int iSizeY = 500, 
		void *vpIcon = 0,
		unsigned int umode = FL_DOUBLE | FL_RGB | FL_DEPTH);
	
	/// Detach this window from the GLUT loop.
	virtual bool DestroyWindow();
	
//	virtual bool Create(int *piArg, char **pcArg, MemObj<SGLUTWin>& mWin);

	virtual bool StartAnimation(unsigned int uiTime); // idlefunc is called each fTime milliseconds
	virtual bool StopAnimation();

	// Post Main Window Redisplay to run through all idle loops
	virtual void PostMainWindowUpdate();

	unsigned char CLU_LEFT_BUTTON() {return GLUT_LEFT_BUTTON;};
	unsigned char CLU_RIGHT_BUTTON() {return GLUT_RIGHT_BUTTON;};	
	unsigned char CLU_DOWN() {return GLUT_DOWN;};
	unsigned char CLU_UP() {return GLUT_UP;};

protected:
	virtual bool BeginDraw();
	virtual bool EndDraw();
	virtual void PostWindowUpdate();

	virtual void KeyUp(unsigned char cKey);
	virtual void KeyDown(unsigned char cKey);

	virtual void Key(unsigned char cKey, int iX, int iY);
	virtual void Reshape(int iWidth, int iHeight);

	virtual void InitMenu();
	virtual void MenuAnimate(Fl_Widget* pWidget);
	virtual void MenuMouseAnim(Fl_Widget* pWidget);
	virtual void MenuMouseMode(Fl_Widget* pWidget);
	virtual void MenuSaveImage(Fl_Widget* pWidget);
	virtual void MenuFullScreen(Fl_Widget* pWidget);
	virtual void MenuLocalRot(Fl_Widget* pWidget);

	virtual void ToolChanged(string sName);

	static void GLUTDisplay();
	static void GLUTReshape(int iWidth, int iHeight);
	static void GLUTMouse(int iButton, int iState, int iX, int iY);
	static void GLUTKey(unsigned char cKey, int iX, int iY);
	static void GLUTActiveMouseMove(int iX, int iY);
	static void GLUTIdleFunc();
	static void GLUTTimedFunc(int value);
	static void GLUTMenu(int iIdx);

	static void CB_Animate(Fl_Widget* pWidget, void *pData);
	static void CB_MouseAnim(Fl_Widget* pWidget, void *pData);
	static void CB_MouseMode(Fl_Widget* pWidget, void *pData);
	static void CB_SaveImage(Fl_Widget* pWidget, void *pData);
	static void CB_FullScreen(Fl_Widget* pWidget, void *pData);
	static void CB_LocalRot(Fl_Widget* pWidget, void *pData);

	static void CB_ToolChanged(Fl_Widget* pWidget, void *pData);

	static const char* CB_InfoLink(Fl_Widget* pWidget, const char *pcURL, void *pvData);


	void calcTimeElapsed();

	int m_iHWin;
	TTimeStruct m_tmStart, m_tmPrev;

	CKeyWindow *m_pWindow;
	Fl_Tile *m_pTile;
	Fl_Box *m_pDummyBox;

	Fl_Glut_Window *m_pGLUTWindow;
	Fl_Menu_Bar *m_pMenuBar;
	Fl_Choice *m_pMouseModeChoice;
	vector<Fl_Menu_Item> m_vecMenuItem;

	// Store the initial x,y pos. of the tool box.
	// This is needed by m_pTile to address the correct child.
	int m_iToolBoxInitX, m_iToolBoxInitY;
	CFLTKToolBox m_ToolBox;

	int m_iInfoWinInitX, m_iInfoWinInitY;
	int m_iMinInfoWidth;
	int m_iInfoLeftMargin;
	Fl_Box *m_pInfoBox;
	Fl_Help_View *m_pInfoWin;

	bool m_bNotifyDestroy;
	int m_iPosX, m_iPosY;

	bool m_bInFullScreenMode;
	int m_iSaveX, m_iSaveY, m_iSaveW, m_iSaveH, m_iSaveMMCWidth;
	int m_iMinToolHeight, m_iToolHeight;
	int m_iMenuHeight, m_iMenuBarHeight, m_iMenuBarSep, m_iMMChoiceWidth;

	// Main Window Handle
	static int m_iHMainWin;
	
	// map from window handle to this pointer.
	static map<int,COGLWinFLTK*> m_mapThis; 
};


#endif // !defined(AFX_OGLWINGLUT_H__6EB9C282_EE5B_11D5_BA34_00E07D8AFD6A__INCLUDED_)
