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


// FLTKToolBox.h: Schnittstelle für die Klasse CFLTKToolBox.
//
// This creates a window to which tools can be added and removed.
// The tools are stacked first left to right and then top to bottom.
//////////////////////////////////////////////////////////////////////

#if !defined(_FLTKTOOLBOX_H__INCLUDED_)
#define _FLTKTOOLBOX_H__INCLUDED_

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
#include <FL/Fl_Counter.H>
#include <FL/Fl_Round_Button.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Repeat_Button.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Choice.H>

#include "time.h"
#include "sys/timeb.h"

#include "CLUDrawBase.h"
#include "IOGLWinBase.h"

#include <map>
#include <list>
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


class CFLTKToolBox
{
public:

	enum EToolType 
	{ 
		TOOL_NONE = 0,
		TOOL_SLIDER,
		TOOL_CHECKBOX,
		TOOL_INPUT,
		TOOL_BUTTON,
		TOOL_CHOICE,
		TOOL_TEXT,
		TOOL_COUNTER
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

		int iInitVal;
		double dInitVal;
		bool bInitVal;
		string sInitVal;

		// Values for Slider
		double dInitMin, dInitMax, dInitStep, dInitLStep;
	};

	class CInput : public Fl_Input
	{
	public:
		CInput(int iX, int iY, int iW, int iH, const char *pcText) 
			: Fl_Input(iX, iY, iW, iH, pcText)
		{ }

		int handle(int iEvent)
		{
			Fl_Widget *pParent = (Fl_Widget *) this->parent();
			Fl_Widget *pParent2 = 0, *pParent3 = 0;
			if (pParent)
				pParent2 = (Fl_Widget *) pParent->parent();

			if (pParent2)
				pParent3 = (Fl_Widget *) pParent2->parent();

			switch(iEvent)
			{
			case FL_KEYBOARD:
				if (this != Fl::belowmouse() &&
					pParent != Fl::belowmouse() &&
					pParent2 != Fl::belowmouse() &&
					pParent3 != Fl::belowmouse())
					return 0;
				break;
			}

			return Fl_Input::handle(iEvent);
		}
	};

	typedef map<string, STool> TToolMap;
	typedef list<Fl_Pack*> TPackList;

public:
	CFLTKToolBox();
	virtual ~CFLTKToolBox();

	// Create the Tool Box
	// pCallback is pointer to callback function which is called when 
	// any one of the tools has changed. 
	// pData is data passed on to callback
	virtual bool Create(int iPosX, int iPosY, int iSizeX, int iSizeY, 
						Fl_Callback *pCallback, void *pData,
						int iToolHeight = 0 /* uses default value from constructor */);

	/// Detach this window from the GLUT loop.
	virtual bool Destroy();


	// Resets falgs of tools which indicates that they are in use
	void ResetToolInUseFlags();
	// Prune, ie. delete all those tools that are not in use
	void PruneToolMap(bool bKeepFixedTools = true, bool bRedraw = true);
	// Remove tool with given name
	bool RemoveTool(const char *pcName, bool bRedraw = true);

	// Update Tool Box Packs
	// This first resets the tool box and then walks through map
	// m_mapTool to redistribute all tools
	bool Update();

	// Get Upper Left corner of scroll box enclosing tools
	bool GetPosition(int &iX, int &iY);
	
	// Get Size of scroll box enclosing tools
	bool GetSize(int &iWidth, int &iHeight);
	
	bool GetTool(const char *pcName, STool &rTool);
	bool GetToolValue(const char *pcName, double &rVal);
	bool GetToolValue(const char *pcName, string &sVal);
	int GetToolCount() { return m_mapTool.size(); }

	// Add a slider tool
	bool AddToolSlider(const char *pcName, 
						double dMin, double dMax, double dStep, double dVal,
						bool bFixed = false, const char *pcDesc = 0, bool bDoUpdate = true);

	// Add a counter tool
	bool AddToolCounter(const char *pcName, 
						double dMin, double dMax, double dStep, double dLStep, double dVal,
						bool bFixed = false, const char *pcDesc = 0, bool bDoUpdate = true);

	// Add a CheckBox tool
	bool AddToolCheckBox(const char *pcName, bool bSet, bool bFixed = false, 
						const char *pcDesc = 0, bool bDoUpdate = true);

	// Add a Button tool
	bool AddToolButton(const char *pcName, bool bFixed = false, 
						const char *pcDesc = 0, bool bDoUpdate = true);

	// Add an Input tool
	bool AddToolInput(const char *pcName, 
						double dMin, double dMax, double dVal,
						bool bFixed = false, const char *pcDesc = 0, bool bDoUpdate = true);

	// Add a Text Input tool
	bool AddToolInputText(const char *pcName, 
						  bool bImmediateCallback, 
						  const char* pcInitText,
						  bool bFixed = false, const char *pcDesc = 0, bool bDoUpdate = true);

	// Add a ComboBox tool
	bool AddToolChoice(const char *pcName, vector<string>& vecData, const char* pcCoice,
						bool bFixed = false, const char *pcDesc = 0, bool bDoUpdate = true);


protected:
	// Reset Tool Box
	// This removes all tools from the packs.
	bool ResetPacks();

protected:
	// Vertical Fl_Pack. This stacks horizontal Fl_Packs
	Fl_Pack *m_pVPackBox;

	// The Scroll box surrounding the m_pVPackBox
	Fl_Scroll *m_pScrollBox;

	// Vector of horizontal packs
	list<Fl_Pack*> m_listHPackBox;

	// Map of tool name to tool data
	map<string, STool> m_mapTool;

	// The height and width of a tool
	int m_iToolHeight, m_iToolWidth, m_iToolWBorder;

	// The tool callback function
	Fl_Callback *m_pToolCallback;

	// The callback data
	void *m_pToolCallbackData;

	// Store current width
	int m_iCurWidth;

	// Flag to indicate whether any tools were added or removed
	bool m_bToolsChanged;
};


#endif // !defined(AFX_OGLWINGLUT_H__6EB9C282_EE5B_11D5_BA34_00E07D8AFD6A__INCLUDED_)
