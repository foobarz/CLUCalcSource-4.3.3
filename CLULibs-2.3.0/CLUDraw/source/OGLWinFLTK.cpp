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


// OGLWinFLTK.cpp: Implementierung der Klasse COGLWinFLTK.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#ifdef CLUDRAW_FLTK
#include "OGLWinFLTK.h"

// Explicit instantiation of map.
//template class map<int, COGLWinFLTK*>;

map<int, COGLWinFLTK*> COGLWinFLTK::m_mapThis;
int COGLWinFLTK::m_iHMainWin = 0;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLWinFLTK::COGLWinFLTK()
{
	//m_pThis = this;	
	m_iHWin = 0;
	m_bNotifyDestroy = false;
	m_bInFullScreenMode = false;
	m_iMinToolHeight = 1;
	m_iToolHeight = 44;
	m_iMenuHeight = 46;
	m_iMenuBarHeight = 20;
	m_iMenuBarSep = 3;
	m_iMMChoiceWidth = 150; // Mouse Mode Choice Width

	m_iToolBoxInitX = m_iToolBoxInitY = 0;
	m_iInfoWinInitX = m_iInfoWinInitY = 0;
	m_iMinInfoWidth = 1;
	m_iInfoLeftMargin = 5;

	m_pMouseModeChoice = 0;
}

COGLWinFLTK::~COGLWinFLTK()
{
	m_ToolBox.ResetToolInUseFlags();
	m_ToolBox.PruneToolMap(false);
}

//COGLWinFLTK* COGLWinFLTK::m_pThis = 0;

//////////////////////////////////////////////////////////////////////
/// Create OpenGL Window with GLUT
bool COGLWinFLTK::Create(int *piArg, char **pcArg, char *pcName,
					   int iPosX, int iPosY, int iSizeX, int iSizeY,
					   unsigned int uMode)
{
	m_iSizeX = iSizeX;
	m_iSizeY = iSizeY;

	m_pWindow = new CKeyWindow(this, iPosX, iPosY, iSizeX, iSizeY, pcName);
	m_pWindow->begin();

	InitMenu();
	m_pMenuBar = new Fl_Menu_Bar(0, 0, iSizeX, m_iMenuBarHeight);
	m_pMenuBar->copy(&m_vecMenuItem[0], this);

	m_pMouseModeChoice = new Fl_Choice(6, m_iMenuBarHeight+m_iMenuBarSep, m_iMMChoiceWidth, m_iMenuBarHeight);
	m_pMouseModeChoice->box(FL_FLAT_BOX);
	m_pMouseModeChoice->textsize(11);
	m_pMouseModeChoice->callback((Fl_Callback *) CB_MouseMode, this);


	m_pGLUTWindow = new Fl_Glut_Window(0, m_iMenuHeight, iSizeX, iSizeY - m_iMenuHeight, "");

	m_pGLUTWindow->mode(uMode);
	m_pWindow->size_range(100, 100);
	m_pWindow->resizable(*m_pGLUTWindow);
	m_pWindow->show(*piArg, pcArg);


	//glutInit(piArg, pcArg);
	//glutInitDisplayMode(uMode);
	//glutInitWindowSize(iSizeX, iSizeY);
	//glutInitWindowPosition(0, 0);
	m_iHMainWin = m_iHWin = m_pGLUTWindow->number; //glutCreateWindow(pcName);

	//printf("Main Window: %d\n", m_iHMainWin);

	// Enter this pointer into map.
	m_mapThis[m_iHWin] = this;

	glutDisplayFunc(GLUTDisplay);
	glutReshapeFunc(GLUTReshape);
	glutMouseFunc(GLUTMouse);
	//glutKeyboardFunc(GLUTKey);
	glutMotionFunc(GLUTActiveMouseMove);
	m_pWindow->end();

	//m_iMainMenu = glutCreateMenu(GLUTMenu);
	//glutAttachMenu(GLUT_MIDDLE_BUTTON);

	if (!Init()) return false;
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Create OpenGL Window with GLUT
bool COGLWinFLTK::CreateWindow(const char *pcName,
					   int iPosX, int iPosY, int iSizeX, int iSizeY,
					   void *vpIcon, unsigned int uMode)
{
	// Only Create this window once.
	if (m_iHWin != 0)
		return false;
	
	m_iPosX = iPosX;
	m_iPosY = iPosY;
	m_iSizeX = iSizeX;
	m_iSizeY = iSizeY;

//	m_pWindow = new Fl_Window(iPosX, iPosY, iSizeX, iSizeY, pcName);
	m_pWindow = new CKeyWindow(this, iPosX, iPosY, iSizeX, iSizeY, pcName);
	m_pWindow->color(FL_LIGHT2);
	m_pWindow->icon(vpIcon);
	m_pWindow->begin();

	InitMenu();
	m_pMenuBar = new Fl_Menu_Bar(0, 0, iSizeX, m_iMenuBarHeight);
	m_pMenuBar->copy(&m_vecMenuItem[0], this);
	m_pMenuBar->textsize(11);
	m_pMenuBar->box(FL_FLAT_BOX);
	m_pMenuBar->down_box(FL_FLAT_BOX);
	m_pMenuBar->color(FL_LIGHT2);

	m_pMouseModeChoice = new Fl_Choice(6, m_iMenuBarHeight+m_iMenuBarSep, m_iMMChoiceWidth, m_iMenuBarHeight);
	m_pMouseModeChoice->box(FL_FLAT_BOX);
	m_pMouseModeChoice->textsize(11);
	m_pMouseModeChoice->callback((Fl_Callback *) CB_MouseMode, this);

	m_pMouseModeChoice->add("Standard Mouse Mode", FL_CTRL + '0', 0);
	m_pMouseModeChoice->add("Mouse Mode 1", FL_CTRL + '1', 0);
	m_pMouseModeChoice->add("Mouse Mode 2", FL_CTRL + '2', 0);
	m_pMouseModeChoice->add("Mouse Mode 3", FL_CTRL + '3', 0);
	m_pMouseModeChoice->add("Mouse Mode 4", FL_CTRL + '4', 0);
	m_pMouseModeChoice->add("Mouse Mode 5", FL_CTRL + '5', 0);
	m_pMouseModeChoice->add("Mouse Mode 6", FL_CTRL + '6', 0);
	m_pMouseModeChoice->add("Mouse Mode 7", FL_CTRL + '7', 0);
	m_pMouseModeChoice->add("Mouse Mode 8", FL_CTRL + '8', 0);
	m_pMouseModeChoice->add("Mouse Mode 9", FL_CTRL + '9', 0);

	m_pMouseModeChoice->value(0);

	// Tile Window for tools
	m_pTile = new Fl_Tile(0, m_iMenuHeight, iSizeX, iSizeY - m_iMenuHeight);
	m_pTile->begin();

	// OpenGL part
	// VERY IMPORTANT:
	// If this window extends over the size of the actual window, a WM_PAINT
	// message can lock the rest of the program, since it is constantly regenerated.
	// This is probably because the OGL-Window and the window frame drawn by Windows
	// overlap and thus try to update the same region.
	m_pGLUTWindow = new Fl_Glut_Window(m_iMinInfoWidth, m_iMenuHeight, iSizeX - m_iMinInfoWidth, 
										iSizeY - m_iMenuHeight - m_iMinToolHeight, "");
	m_pGLUTWindow->mode(uMode);
	m_pGLUTWindow->align(FL_ALIGN_CLIP);
	m_pGLUTWindow->box(FL_DOWN_BOX);

	// Create Toolbox
	m_iToolBoxInitX = m_iMinInfoWidth;
	m_iToolBoxInitY = iSizeY-m_iMinToolHeight;
	m_ToolBox.Create(m_iToolBoxInitX, m_iToolBoxInitY, iSizeX, m_iMinToolHeight, 
						(Fl_Callback *) CB_ToolChanged, (void *) this, m_iToolHeight-4);

	// Create Info Window
	m_iInfoWinInitX = m_iMinInfoWidth;
	m_iInfoWinInitY = m_iMenuHeight;
	m_pInfoWin = new Fl_Help_View(0, m_iMenuHeight, m_iMinInfoWidth, 
									iSizeY - m_iMenuHeight, "");
	m_pInfoWin->selection_color(FL_BLUE);
	m_pInfoWin->link((Fl_Help_Func *) CB_InfoLink, (void *) this);
	m_pInfoWin->value(" ");
/*
		"Use <code>SetInfoText([string])</code> to put some text here. "
		"The text may include some 'simple' HTML tags.");
*/
	m_pDummyBox = new Fl_Box(m_iMinInfoWidth, m_iMenuHeight + m_iToolHeight, 
							iSizeX - 50, iSizeY - m_iMenuHeight - m_iToolHeight - m_iMinToolHeight);
	//Fl_Box DummyBox(0, 0, iSizeX, iSizeY);
	m_pDummyBox->box(FL_NO_BOX);
	// This is very important since otherwise FL_MOVE events are
	// not sent to the actual widgets in the tile window.
	m_pDummyBox->hide(); 

	m_pTile->resizable(*m_pDummyBox);
//	m_pTile->resizable(*m_pGLUTWindow);
	m_pTile->end();
	// End tile window

	m_pWindow->size_range(100, 100);
	m_pWindow->resizable(*m_pGLUTWindow);
	m_pWindow->show();

	m_iSaveX = m_pWindow->x();
	m_iSaveY = m_pWindow->y();
	m_iSaveW = m_pWindow->w();
	m_iSaveH = m_pWindow->h();

	//glutInit(piArg, pcArg);
	//glutInitDisplayMode(uMode);
	//glutInitWindowSize(iSizeX, iSizeY);
	//glutInitWindowPosition(0, 0);
	m_iHWin = m_pGLUTWindow->number; //glutCreateWindow(pcName);

	//printf("Main Window: %d\n", m_iHMainWin);

	// Enter this pointer into map.
	m_mapThis[m_iHWin] = this;

	glutDisplayFunc(GLUTDisplay);
	glutReshapeFunc(GLUTReshape);
	glutMouseFunc(GLUTMouse);
	//glutKeyboardFunc(GLUTKey);
	glutMotionFunc(GLUTActiveMouseMove);
	m_pWindow->end();


	//m_iMainMenu = glutCreateMenu(GLUTMenu);
	//glutAttachMenu(GLUT_MIDDLE_BUTTON);

	if (!Init()) return false;
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Detach window

bool COGLWinFLTK::DestroyWindow()
{
	if (m_iHWin == 0)
		return true;
		
	if (m_bIsAnimated)
		StopAnimation();
	
	glutDestroyWindow(m_iHWin);
	glutSetWindow(m_iHMainWin);
	m_mapThis.erase(m_iHWin);
	m_iHWin = 0;
	
	return true;
}


//////////////////////////////////////////////////////////////////////
// Callback function of info window

const char* COGLWinFLTK::InfoLink(Fl_Widget* pWidget, const char *pcURL)
{
	return 0; // Do nothing when link is pressed
}

//////////////////////////////////////////////////////////////////////
/// Callback when link is pressed in info window

const char* COGLWinFLTK::CB_InfoLink(Fl_Widget* pWidget, const char *pcURL, void *pvData)
{
	if (pvData)
	{
		return ((COGLWinFLTK *) pvData)->InfoLink(pWidget, pcURL);
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////
/// Callback when tool has changed
// Entered by functions AddTool... as callback function
// Hence this is called each time a tool changes.

void COGLWinFLTK::CB_ToolChanged(Fl_Widget* pWidget, void *pData)
{
	if (pData)
	{
		string sName = pWidget->label();
	
		((COGLWinFLTK *)pData)->ToolChanged(sName);
	}
}

//////////////////////////////////////////////////////////////////////
/// Callback when tool has changed

void COGLWinFLTK::ToolChanged(string sName)
{
	//printf("%s\n", sName.c_str());

	if (!m_bIsAnimated)
		Display();
}


//////////////////////////////////////////////////////////////////////
/// Get value of a tool

bool COGLWinFLTK::GetToolValue(const char *pcName, double &rVal)
{
	return m_ToolBox.GetToolValue(pcName, rVal);
}

//////////////////////////////////////////////////////////////////////
/// Get value of a tool

bool COGLWinFLTK::GetToolValue(const char *pcName, string &rVal)
{
	return m_ToolBox.GetToolValue(pcName, rVal);
}


//////////////////////////////////////////////////////////////////////
/// Add a slider tool

bool COGLWinFLTK::AddToolSlider(const char *pcName, double dMin, double dMax, double dStep,
								double dVal, bool bFixed, const char *pcDesc)
{
	// Do not allow tool to update packs
	// This is done explicitly later
	return m_ToolBox.AddToolSlider(pcName, dMin, dMax, dStep, dVal, bFixed, pcDesc, false);
}

//////////////////////////////////////////////////////////////////////
/// Add a counter tool

bool COGLWinFLTK::AddToolCounter(const char *pcName, double dMin, double dMax, double dStep,
								double dLStep, double dVal, bool bFixed, const char *pcDesc)
{
	// Do not allow tool to update packs
	// This is done explicitly later
	return m_ToolBox.AddToolCounter(pcName, dMin, dMax, dStep, dLStep, dVal, bFixed, pcDesc, false);
}

//////////////////////////////////////////////////////////////////////
/// Add a slider tool

bool COGLWinFLTK::AddToolInput(const char *pcName, double dMin, double dMax,
								double dVal, bool bFixed, const char *pcDesc)
{
	// Do not allow tool to update packs
	// This is done explicitly later
	return m_ToolBox.AddToolInput(pcName, dMin, dMax, dVal, bFixed, pcDesc, false);
}


//////////////////////////////////////////////////////////////////////
/// Add a text input tool

bool COGLWinFLTK::AddToolInputText(const char *pcName, 
								bool bImmediateCallback, 
								const char* pcInitText,
								bool bFixed, const char *pcDesc, bool bDoUpdate)
{
	return m_ToolBox.AddToolInputText(pcName, bImmediateCallback, pcInitText,
										bFixed, pcDesc, bDoUpdate);
}


//////////////////////////////////////////////////////////////////////
/// Add a check box tool

bool COGLWinFLTK::AddToolCheckBox(const char *pcName, bool bSet, bool bFixed, 
									const char *pcDesc)
{
	return m_ToolBox.AddToolCheckBox(pcName, bSet, bFixed, pcDesc, false);
}

//////////////////////////////////////////////////////////////////////
/// Add a button tool

bool COGLWinFLTK::AddToolButton(const char *pcName, bool bFixed, 
									const char *pcDesc)
{
	return m_ToolBox.AddToolButton(pcName, bFixed, pcDesc, false);
}


//////////////////////////////////////////////////////////////////////
/// Add a choice tool

bool COGLWinFLTK::AddToolChoice(const char *pcName, vector<string>& vecData, const char* pcChoice,
						bool bFixed, const char *pcDesc, bool bDoUpdate)
{
	return m_ToolBox.AddToolChoice(pcName, vecData, pcChoice, bFixed, pcDesc, bDoUpdate);
}


//////////////////////////////////////////////////////////////////////
// Info Window Functions

bool COGLWinFLTK::SetInfoText(const char *pcText)
{
	if (!m_pInfoWin || !pcText)
		return false;

	m_pInfoWin->value(pcText);

	return true;
}


//////////////////////////////////////////////////////////////////////
// Set Width of Info window in percent

bool COGLWinFLTK::SetInfoWidth(double dWidth)
{
	if (!m_pInfoWin)
		return false;

	int iPixWidth = m_pTile->w();
	int iDeltaWidth;
	int iCurWidth = m_pInfoWin->w();
	int iPosY = m_pInfoWin->y();

	if (dWidth <= 0)
	{
		dWidth = 0;
	}
	
	if (dWidth >= 0 && dWidth < 1) 
	{
		// interpret width as percentage of max width
		iPixWidth = int( double(iPixWidth) * dWidth );
	}
	else
	{
		// interpret width as pixel width
		if (int(dWidth) >= iPixWidth)
			iPixWidth--;
		else
			iPixWidth = int(dWidth);
	}

	if (iPixWidth < m_iMinInfoWidth)
		iPixWidth = m_iMinInfoWidth;

	iDeltaWidth = iPixWidth - iCurWidth;

	m_pTile->position(m_iInfoWinInitX, m_iInfoWinInitY, iPixWidth, iPosY);
	CCLUDrawBase::Reshape(m_pGLUTWindow->w() - iDeltaWidth, m_pGLUTWindow->h());

//	if (!m_bIsAnimated)
//		glutPostRedisplay();

	return true;
}


//////////////////////////////////////////////////////////////////////
// Set Size of OGL Window in pixel or percent

bool COGLWinFLTK::SetOGLSize(double dWidth, double dHeight)
{
	if (!m_pGLUTWindow || !m_pWindow)
		return false;

	int iScrWidth = Fl::w();
	int iScrHeight = Fl::h();
	int iCurWidth = m_pWindow->w();
	int iCurHeight = m_pWindow->h();
	int iInfoWidth = m_pInfoWin->w();
	int iInfoHeight = m_pInfoWin->h();
	int iToolWidth, iToolHeight;
	int iNewWidth, iNewHeight;

	m_ToolBox.GetSize(iToolWidth, iToolHeight);

	if (dWidth < 0)
		dWidth = 0;

	if (dHeight < 0)
		dHeight = 0;

	if (dWidth >= 0 && dWidth <= 1)
	{
		iNewWidth = int(double(iScrWidth) * dWidth);
	}
	else
	{
		iNewWidth = int(dWidth);
	}

	if (dHeight >= 0 && dHeight <= 1)
	{
		iNewHeight = int(double(iScrHeight) * dHeight);
	}
	else
	{
		iNewHeight = int(dHeight);
	}

	iNewWidth += iInfoWidth;
	iNewHeight += iToolHeight + m_iMenuHeight;

	// Minimal Width
	if (iNewWidth < 50)
		iNewWidth = 50;
	// Maximal Width
	else if (iNewWidth > iScrWidth-10)
		iNewWidth = iScrWidth-10;

	// Minimal Height
	if (iNewHeight < 50)
		iNewHeight = 50;
	// Maximal Height
	else if (iNewHeight > iScrHeight-10)
		iNewHeight = iScrHeight-10;

	if (iNewWidth == iCurWidth &&
		iNewHeight == iCurHeight)
		return true;

	m_pWindow->size(iNewWidth, iNewHeight);
//	m_pWindow->show();
//	CCLUDrawBase::Reshape(iNewWidth, iNewHeight);
//	CCLUDrawBase::Reshape(m_pGLUTWindow->w(), m_pGLUTWindow->h());

//	if (!m_bIsAnimated)
//		glutPostRedisplay();

/*
	int iPixWidth = m_pTile->w();
	int iPixHeight = m_pTile->h();
	int iDeltaWidth, iDeltaHeight;
	int iCurWidth = m_pGLUTWindow->w();
	int iCurHeight = m_pGLUTWindow->h();
	int iPosX = m_pGLUTWindow->x();
	int iPosY = m_pGLUTWindow->y();

	if (dWidth < 0)
		dWidth = 0;

	if (dHeight < 0)
		dHeight = 0;
	
	if (dWidth >= 0 && dWidth < 1) 
	{
		// interpret width as percentage of max width
		iPixWidth = int( double(iPixWidth) * dWidth );
	}
	else
	{
		// interpret width as pixel width
		if (int(dWidth) >= iPixWidth)
			iPixWidth--;
		else
			iPixWidth = int(dWidth);
	}

		iPixHeight = int( double(iPixHeight) * dHeight );

	if (iPixWidth < m_iMinInfoWidth)
		iPixWidth = m_iMinInfoWidth;

	iDeltaWidth = iPixWidth - iCurWidth;

	m_pTile->position(m_iInfoWinInitX, m_iInfoWinInitY, iPixWidth, iPosY);
	CCLUDrawBase::Reshape(m_pGLUTWindow->w() - iDeltaWidth, m_pGLUTWindow->h());

//	if (!m_bIsAnimated)
//		glutPostRedisplay();
*/
	return true;
}




//////////////////////////////////////////////////////////////////////
/// Show Window

void COGLWinFLTK::ShowWindow(bool bVal)
{
	if (m_pWindow)
	{
		if (bVal)
			m_pWindow->show();
		else
			m_pWindow->hide();
	}
}


//////////////////////////////////////////////////////////////////////
/// Hide Menu

void COGLWinFLTK::HideMenu(bool bVal) 
{ 
	if (m_pMenuBar) 
	{
		if (bVal) 
		{
			m_pMenuBar->resize(0, 0, m_iSizeX, 1); 
			m_pGLUTWindow->resize(0, 1, m_iSizeX, m_iSizeY + 29);
		}
		else
		{
			m_pGLUTWindow->resize(0, 30, m_iSizeX, m_iSizeY - 30);
			m_pMenuBar->resize(0, 0, m_iSizeX, 30); 
		}
	}
}

//////////////////////////////////////////////////////////////////////
/// Create OpenGL Window with GLUT
/*
bool COGLWinFLTK::Create(int *piArg, char **pcArg, MemObj<SGLUTWin>& mWin)
{


	return true;
}

*/

void COGLWinFLTK::Key(unsigned char cKey, int iX, int iY)
{
	// Do not react to keys.
	// Functionality is now available through menu,
	return;
}

void COGLWinFLTK::KeyUp(unsigned char cKey)
{
	return;
}

void COGLWinFLTK::KeyDown(unsigned char cKey)
{
	return;
}


void COGLWinFLTK::InitMenu()
{
	Fl_Menu_Item pMenuItems[] = 
	{
		{ "&Visualization", 0, 0, 0, FL_SUBMENU },
		{ "&Animate",        FL_CTRL + 'a', (Fl_Callback *) CB_Animate, 0, FL_MENU_TOGGLE },
		{ "&Mouse animation",    FL_CTRL + 'm', (Fl_Callback *) CB_MouseAnim, 0, FL_MENU_TOGGLE },
		{ "&Local Rotation",    FL_CTRL + 'l', (Fl_Callback *) CB_LocalRot, 0, FL_MENU_TOGGLE },
		{ "&Full Screen",        FL_CTRL + 'f', (Fl_Callback *) CB_FullScreen, 0, FL_MENU_TOGGLE },
		{ "Save &Image...", FL_CTRL + 'b', (Fl_Callback *) CB_SaveImage },
		{ 0 },
/*
		{ "&Mouse Mode", 0, 0, 0, FL_SUBMENU },
		{ "Standard", FL_CTRL + '0', (Fl_Callback *) CB_MouseMode, (void *) '0', FL_MENU_RADIO |FL_MENU_VALUE },
		{ "Mode &1", FL_CTRL + '1', (Fl_Callback *) CB_MouseMode, (void *) '1', FL_MENU_RADIO },
		{ "Mode &2", FL_CTRL + '2', (Fl_Callback *) CB_MouseMode, (void *) '2', FL_MENU_RADIO },
		{ "Mode &3", FL_CTRL + '3', (Fl_Callback *) CB_MouseMode, (void *) '3', FL_MENU_RADIO },
		{ "Mode &4", FL_CTRL + '4', (Fl_Callback *) CB_MouseMode, (void *) '4', FL_MENU_RADIO },
		{ "Mode &5", FL_CTRL + '5', (Fl_Callback *) CB_MouseMode, (void *) '5', FL_MENU_RADIO },
		{ "Mode &6", FL_CTRL + '6', (Fl_Callback *) CB_MouseMode, (void *) '6', FL_MENU_RADIO },
		{ "Mode &7", FL_CTRL + '7', (Fl_Callback *) CB_MouseMode, (void *) '7', FL_MENU_RADIO },
		{ "Mode &8", FL_CTRL + '8', (Fl_Callback *) CB_MouseMode, (void *) '8', FL_MENU_RADIO },
		{ "Mode &9", FL_CTRL + '9', (Fl_Callback *) CB_MouseMode, (void *) '9', FL_MENU_RADIO },
		{ 0 },
*/
		{ 0 }
	};

	m_vecMenuItem.clear();
	m_vecMenuItem.resize(8);

	for (int i = 0; i < 8; i++)
	{
		m_vecMenuItem[i] = pMenuItems[i];
	}
}

void COGLWinFLTK::MenuAnimate(Fl_Widget *pWidget)
{
	CCLUDrawBase::Key('a', 0, 0);
}

void COGLWinFLTK::MenuMouseAnim(Fl_Widget *pWidget)
{
	CCLUDrawBase::Key('m', 0, 0);
}


void COGLWinFLTK::MenuMouseMode(Fl_Widget *pWidget)
{
	if (pWidget)
	{
		int iVal = ((Fl_Choice *)pWidget)->value();
		SetRTMode(iVal);
	}
}

bool COGLWinFLTK::SetMouseMode(int iMode)
{
	if (m_pMouseModeChoice)
	{
		if (SetRTMode(iMode))
		{
			m_pMouseModeChoice->value(iMode);
			return true;
		}
	}

	return false;
}



void COGLWinFLTK::MenuSaveImage(Fl_Widget *pWidget)
{
	char *pcNewFile, *pcFilter = 0, *pcExt;
	char pcFilename[512];

	pcFilename[0] = 0;
	Fl_File_Chooser::sort = fl_casenumericsort;
	pcNewFile = fl_file_chooser("Save Screen Image", 
		"PNG Files (*.png)\t"
		"JPEG Files (*.jpg)\t"
		"Bitmap Files (*.bmp)\t"
		"Postscript Files (*.eps)\t"
		"Save as JPEG + EPS (*.jps)"
		, pcFilename, 0, &pcFilter);

	if (pcNewFile != NULL) 
	{
		if (*fl_filename_ext(pcNewFile) == 0)
		{
			pcExt = (char *) fl_filename_ext(pcFilter);

			if (pcExt == 0 || *pcExt == 0)
				strcat(pcNewFile, ".png");
			else
				strcat(pcNewFile, pcExt);
		}

		GLUTDisplay();
		CCLUDrawBase::SaveScreen2BMP(pcNewFile);
	}
}

void COGLWinFLTK::MenuLocalRot(Fl_Widget *pWidget)
{
	if (pWidget)
	{
		const Fl_Menu_Item *pItem = ((Fl_Menu_ *)pWidget)->mvalue();
		if (pItem->checked())
		{
			EnableLocalRot(true);
		}
		else
		{
			EnableLocalRot(false);
		}
	}
}


void COGLWinFLTK::MenuFullScreen(Fl_Widget *pWidget)
{
	if (pWidget)
	{
		const Fl_Menu_Item *pItem = ((Fl_Menu_ *)pWidget)->mvalue();
		if (pItem->checked())
		{
			m_iSaveX = m_pWindow->x();
			m_iSaveY = m_pWindow->y();
			m_iSaveW = m_pWindow->w();
			m_iSaveH = m_pWindow->h();
			m_iSaveMMCWidth = m_pMouseModeChoice->w();

			m_pWindow->fullscreen();

			m_pMenuBar->resize(0, 0, m_iSaveW, 0); 
			m_pMouseModeChoice->resize(0, 0, m_iSaveMMCWidth, 0); 
			m_pTile->resize(0, 0, m_pWindow->w(), m_pWindow->h());

			Fl_Widget* pDummyBox = m_pTile->resizable();
			pDummyBox->resize(1, m_iToolHeight, 
							m_pWindow->w() - 50, m_pWindow->h() - m_iToolHeight - m_iMinToolHeight);

			m_bInFullScreenMode = true;
		}
		else
		{
			m_pTile->resize(0, m_iMenuHeight, m_pWindow->w(), m_pWindow->h() - m_iMenuHeight);

			Fl_Widget* pDummyBox = m_pTile->resizable();
			pDummyBox->resize(1, m_iMenuHeight + m_iToolHeight, 
						m_pWindow->w() - 50, 
						m_pWindow->h() - m_iMenuHeight - m_iToolHeight - m_iMinToolHeight);

			m_pMenuBar->resize(0, 0, m_pWindow->w(), m_iMenuBarHeight);
			m_pMouseModeChoice->resize(6, m_iMenuBarHeight+m_iMenuBarSep, 
											m_iSaveMMCWidth, m_iMenuBarHeight); 
			//m_pGLUTWindow->show();
			//m_pMenuBar->show();

			m_pWindow->fullscreen_off(m_iSaveX, m_iSaveY, m_iSaveW, m_iSaveH);
			//m_pWindow->show();
			m_bInFullScreenMode = false;


			//HideMenu(false);
		}
	}
}


void COGLWinFLTK::CB_Animate(Fl_Widget* pWidget, void *pData)
{
	if (pData)
	{
		((COGLWinFLTK *)pData)->MenuAnimate(pWidget);
	}
}

void COGLWinFLTK::CB_LocalRot(Fl_Widget* pWidget, void *pData)
{
	if (pData)
	{
		((COGLWinFLTK *)pData)->MenuLocalRot(pWidget);
	}
}

void COGLWinFLTK::CB_MouseAnim(Fl_Widget* pWidget, void *pData)
{
	if (pData)
	{
		((COGLWinFLTK *)pData)->MenuMouseAnim(pWidget);
	}
}

void COGLWinFLTK::CB_MouseMode(Fl_Widget* pWidget, void *pData)
{
	if (pData)
	{
		((COGLWinFLTK *)pData)->MenuMouseMode(pWidget);
	}
}

void COGLWinFLTK::CB_SaveImage(Fl_Widget* pWidget, void *pData)
{
	if (pData)
	{
		((COGLWinFLTK *)pData)->MenuSaveImage(pWidget);
	}
}

void COGLWinFLTK::CB_FullScreen(Fl_Widget* pWidget, void *pData)
{
	if (pData)
	{
		((COGLWinFLTK *)pData)->MenuFullScreen(pWidget);
	}
}

//////////////////////////////////////////////////////////////////////
/// Reshape Windpw

void COGLWinFLTK::Reshape(int iWidth, int iHeight)
{
	CCLUDrawBase::Reshape(iWidth, iHeight);

	m_ToolBox.Update();
//	m_pInfoWin->leftline(m_iInfoLeftMargin);
//	m_pInfoWin->format();
}


//////////////////////////////////////////////////////////////////////
/// Post Main Window Redisplay

void COGLWinFLTK::PostMainWindowUpdate()
{
	//printf("PostMainWindowRedisplay (%d)\n", m_iHMainWin);
	COGLWinFLTK *pThis = m_mapThis[m_iHMainWin];
	pThis->PostWindowUpdate();
	//printf("PostMainWindowRedisplay Exit\n");
}

void COGLWinFLTK::PostWindowUpdate()
{
	return;
}


//////////////////////////////////////////////////////////////////////
/// Static functions to call virtual functions

void COGLWinFLTK::GLUTDisplay()
{
	// Get this pointer for current window
	COGLWinFLTK *pThis = m_mapThis[glutGetWindow()];

	//printf("Display: %d\n", glutGetWindow());

	if (pThis)
		pThis->Display();
}

void COGLWinFLTK::GLUTReshape(int iWidth, int iHeight)
{
	// Get this pointer for current window
	COGLWinFLTK *pThis = m_mapThis[glutGetWindow()];
	
	if (pThis)
		pThis->Reshape(iWidth, iHeight);
}

void COGLWinFLTK::GLUTMouse(int iButton, int iState, int iX, int iY)
{
	// Get this pointer for current window
	COGLWinFLTK *pThis = m_mapThis[glutGetWindow()];
	
	if (pThis)
		pThis->Mouse(iButton, iState, iX, iY);
}

void COGLWinFLTK::GLUTKey(unsigned char cKey, int iX, int iY)
{
	// Get this pointer for current window
	COGLWinFLTK *pThis = m_mapThis[glutGetWindow()];
	
	if (pThis)
		pThis->Key(cKey, iX, iY);
}

void COGLWinFLTK::GLUTActiveMouseMove(int iX, int iY)
{
	// Get this pointer for current window
	COGLWinFLTK *pThis = m_mapThis[glutGetWindow()];
	
	if (pThis)
		pThis->ActiveMouseMove(iX, iY);
}

void COGLWinFLTK::GLUTIdleFunc() 
{
	// Idle func is always called first with this pointer
	// of main window!
	// Get this pointer for current window
	COGLWinFLTK *pThis = m_mapThis[m_iHMainWin];
	
	if (pThis)
	{
		glutSetWindow(m_iHMainWin);
		pThis->IdleFunc();
	}
		
	// Now call IdleFunc for all other windows.
	std::map<int, COGLWinFLTK*>::iterator itCur, itEnd;
	
	itCur = m_mapThis.begin();
	itEnd = m_mapThis.end();
	for(;itCur != itEnd; ++itCur)
	{
		if (itCur->first != m_iHMainWin)
		{
			glutSetWindow(itCur->first);
			itCur->second->IdleFunc();

			if (itCur->second->ShouldDestroy())
			{
				itCur->second->DestroyWindow();
				break;
			}
		}
	}
}

void COGLWinFLTK::GLUTMenu(int iIdx)
{
	// Get this pointer for current window
	COGLWinFLTK *pThis = m_mapThis[glutGetWindow()];
	
	if (pThis)
		pThis->Menu(iIdx);
}

//////////////////////////////////////////////////////////////////////
/// Begin Draw

bool COGLWinFLTK::BeginDraw()
{
	glutSetWindow(m_iHWin);

	CCLUDrawBase::BeginDraw();

	return true;
}

//////////////////////////////////////////////////////////////////////
/// End Draw

bool COGLWinFLTK::EndDraw()
{
	CCLUDrawBase::EndDraw();

	glutSwapBuffers();
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Enter Main Loop

void COGLWinFLTK::Run()
{
	//glutMainLoop();
	Fl::run();
}

//////////////////////////////////////////////////////////////////////
/// Start Animation

bool COGLWinFLTK::StartAnimation(unsigned int uiTime) 
{
	if (m_bIsAnimated)
		return false;

	GETTIME(&m_tmStart);
	memcpy(&m_tmPrev, &m_tmStart, sizeof(TTimeStruct));

	m_fTotalTime = 0;	 
	m_fTimeStep = 0;
	m_uAnimateTimeStep = uiTime;

	if (!AtAnimStart())
		return false;

	m_bIsAnimated = true;
	glutTimerFunc((unsigned int) m_iAnimPause /*m_uAnimateTimeStep*/, GLUTTimedFunc, m_iHWin);	

	m_pMenuBar->value(1);
	((Fl_Menu_Item *) m_pMenuBar->mvalue())->check();

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Stop Animation

bool COGLWinFLTK::StopAnimation() 
{
	m_bIsAnimated = false;

	AtAnimEnd();

	m_pMenuBar->value(1);
	((Fl_Menu_Item *) m_pMenuBar->mvalue())->uncheck();

	return true;
}

//////////////////////////////////////////////////////////////////////
/// GLUT Timed Func

void COGLWinFLTK::GLUTTimedFunc(int value) 
{
	// Get this pointer for the calling window
	COGLWinFLTK *pThis;
	std::map<int, COGLWinFLTK*>::iterator itCur;
	
	// If Window handle is not found in map then return.
	if ((itCur = m_mapThis.find(value)) == m_mapThis.end())
		return;
	
	pThis = itCur->second;

	if (pThis) 
	{
		pThis->calcTimeElapsed();

		if (pThis->TimedFunc() && pThis->m_bIsAnimated)
			glutTimerFunc((unsigned int) pThis->m_iAnimPause /*m_uAnimateTimeStep*/, COGLWinFLTK::GLUTTimedFunc, value);
	}
}

//////////////////////////////////////////////////////////////////////
/// Get Modifiers
void COGLWinFLTK::GetModifiers()
{
	int iMod = glutGetModifiers();

	if (iMod & GLUT_ACTIVE_SHIFT) m_bShiftDown = true;
	else m_bShiftDown = false;

	if (iMod & GLUT_ACTIVE_CTRL) m_bCtrlDown = true;
	else m_bCtrlDown = false;

	if (iMod & GLUT_ACTIVE_ALT) m_bAltDown = true;
	else m_bAltDown = false;
}

//////////////////////////////////////////////////////////////////////
/// calculate elpased time since last idlefunc call

void COGLWinFLTK::calcTimeElapsed() 
{
	TTimeStruct tmCur;
	GETTIME(&tmCur);

	m_fTotalTime = float(tmCur.time - m_tmStart.time) 
					+ float(1e-3) * float(tmCur.millitm - m_tmStart.millitm);

	m_fTimeStep = float(tmCur.time - m_tmPrev.time) 
					+ float(1e-3) * float(tmCur.millitm - m_tmPrev.millitm);

	memcpy(&m_tmPrev, &tmCur, sizeof(TTimeStruct));
}

#endif // CLUDRAW_GLUT


