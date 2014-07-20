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

#if !defined( _FLTKOUTPUT_H__INCLUDED_ )
#define _FLTKOUTPUT_H__INCLUDED_

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
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Help_View.H>

#include <string>
#include <vector>

using namespace std;



class CFLTKOutput : public Fl_Double_Window
{
public:
	CFLTKOutput();
	CFLTKOutput(int iPosX, int iPosY, int iWidth, int iHeight, const char* pcTitle = 0,
		void *pvIcon = 0);
	~CFLTKOutput(void);

	virtual bool Create(int iPosX, int iPosY, int iWidth, int iHeight, const char* pcTitle = 0,
						void *pvIcon = 0);
	void Show();

	virtual bool SetText(const char* pcText);

protected:
	// Virtual Functions that influence Editor behaviour

	// Override this function to create your own menus
	virtual void InitMenu();

protected:
//	virtual void Copy();
	virtual void Quit();
	virtual const char* InfoLink(Fl_Widget* pWidget, const char *pcURL);

protected:
	static void CB_Quit(Fl_Widget* pWidget, void *pData);
//	static void CB_Copy(Fl_Widget* pWidget, void *pData);
	static const char* CB_InfoLink(Fl_Widget* pWidget, const char *pcURL, void *pvData);

protected:
	bool m_bIsOK;
	bool m_bHasMenu;
	string m_sTitle;

	Fl_Help_View *m_pWinOutput;
//	Fl_Text_Editor *m_pWinOutput;
//	Fl_Text_Buffer *m_pTextBuf;

	Fl_Menu_Bar			*m_pMenuBar;
	vector<Fl_Menu_Item> m_vecMenuItem;
};


#endif  // #define _FLTKEDITOR_H__INCLUDED_

