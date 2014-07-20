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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "FLTKOutput.h"


CFLTKOutput::CFLTKOutput() : Fl_Double_Window(100, 100)
{
	m_bIsOK = false;
	m_bHasMenu = false;

	m_pWinOutput = 0;
	//m_pTextBuf = 0;
}


CFLTKOutput::CFLTKOutput(int iPosX, int iPosY, int iWidth, int iHeight, const char* pcTitle,
						 void *pvIcon)
		: Fl_Double_Window(iWidth, iHeight)
{
	m_bIsOK = false;
	m_bHasMenu = false;

	m_pWinOutput = 0;
	//m_pTextBuf = 0;

	Create(iPosX, iPosY, iWidth, iHeight, pcTitle, pvIcon);
	//Show();
}



CFLTKOutput::~CFLTKOutput(void)
{
}


bool CFLTKOutput::Create(int iPosX, int iPosY, int iWidth, int iHeight, const char* pcTitle,
						 void *pvIcon)
{
	if (m_bIsOK)
		return true;

	int iMenuHeight = 0;

	icon(pvIcon);
	size(iWidth, iHeight);
	
	if (iPosX >= 0 && iPosY >= 0)
		position(iPosX, iPosY);

	begin();

	InitMenu();
	if (m_vecMenuItem.size() > 1)
	{
		iMenuHeight = 20;
		m_pMenuBar = new Fl_Menu_Bar(0, 0, iWidth, iMenuHeight);
		m_pMenuBar->copy(&m_vecMenuItem[0], this);
		m_pMenuBar->textsize(11);
		m_pMenuBar->box(FL_FLAT_BOX);
		m_pMenuBar->down_box(FL_FLAT_BOX);
		m_pMenuBar->color(FL_LIGHT2);
		m_bHasMenu = true;
	}

	m_pWinOutput = new Fl_Help_View(0, iMenuHeight, iWidth, 
									iHeight - iMenuHeight, "");
	m_pWinOutput->selection_color(FL_BLUE);
	m_pWinOutput->link((Fl_Help_Func *) CB_InfoLink, (void *) this);
	m_pWinOutput->value(" ");

/*
	m_pTextBuf = new Fl_Text_Buffer;

	m_pWinOutput = new Fl_Text_Editor(0, iMenuHeight, iWidth, iHeight - iMenuHeight);
	m_pWinOutput->buffer(m_pTextBuf);
	m_pWinOutput->box(FL_NO_BOX);
	m_pWinOutput->resizable(m_pWinOutput);
	m_pWinOutput->textfont(FL_COURIER);
	m_pWinOutput->end();
*/

	resizable(m_pWinOutput);
	callback((Fl_Callback *) CFLTKOutput::CB_Quit, this);

	end();


	if (pcTitle)
	{
		m_sTitle = pcTitle;
		label(m_sTitle.c_str());
	}

	//Show();

	m_bIsOK = true;
	return true;
}


void CFLTKOutput::Show()
{
	show();
}


bool CFLTKOutput::SetText(const char* pcText)
{
	if (pcText)
	{
		//m_pTextBuf->text(pcText);
		m_pWinOutput->value(pcText);
		return true;
	}

	return false;
}


/////////////////////////////////////////////////////////////////////
/// Init Menu

void CFLTKOutput::InitMenu()
{
	Fl_Menu_Item pMenuItems[] = 
	{
//		{ "&Edit", 0, 0, 0, FL_SUBMENU },
//		{ "&Copy",       FL_CTRL + 'c', (Fl_Callback *) CB_Copy },
//		{ 0 },

		{ 0 }
	};

	m_vecMenuItem.clear();
	m_vecMenuItem.resize(1);

	for (int i = 0; i < 1; i++)
	{
		m_vecMenuItem[i] = pMenuItems[i];
	}

}

/////////////////////////////////////////////////////////////////////
/// Standard Implementations of virtual functions
/*
void CFLTKOutput::Copy()
{
	Fl_Text_Editor::kf_copy(0, m_pWinOutput);
}
*/
void CFLTKOutput::Quit()
{
	exit(0);
}

/////////////////////////////////////////////////////////////////////
/// Callbacks
/*
void CFLTKOutput::CB_Copy(Fl_Widget* pWidget, void *pData)
{
	if (pData)
		((CFLTKOutput *) pData)->Copy();
}
*/

void CFLTKOutput::CB_Quit(Fl_Widget* pWidget, void *pData)
{
	if (pData)
		((CFLTKOutput *) pData)->Quit();
}


//////////////////////////////////////////////////////////////////////
// Callback function of info window

const char* CFLTKOutput::InfoLink(Fl_Widget* pWidget, const char *pcURL)
{
	return 0; // Do nothing when link is pressed
}

//////////////////////////////////////////////////////////////////////
/// Callback when link is pressed in info window

const char* CFLTKOutput::CB_InfoLink(Fl_Widget* pWidget, const char *pcURL, void *pvData)
{
	if (pvData)
	{
		return ((CFLTKOutput *) pvData)->InfoLink(pWidget, pcURL);
	}

	return 0;
}

