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


#include "StdAfx.h"
#include "CLUOutput.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

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


CCLUOutput::CCLUOutput() : CFLTKOutput()
{
	m_poglWin = 0;
	m_pEditor = 0;
	m_iFontSize = 14;
	m_bShowType = false;
	m_bSimpleText = false;
}


CCLUOutput::CCLUOutput(int iPosX, int iPosY, int iWidth, int iHeight, const char* pcTitle,
					   void *pvIcon)
{
	m_poglWin = 0;
	m_pEditor = 0;
	m_iFontSize = 14;
	m_bShowType = false;
	m_bSimpleText = false;

	Create(iPosX, iPosY, iWidth, iHeight, pcTitle, pvIcon);
}



CCLUOutput::~CCLUOutput(void)
{
}



/////////////////////////////////////////////////////////////////////
/// Init Menu

void CCLUOutput::InitMenu()
{
	CFLTKOutput::InitMenu();

	Fl_Menu_Item pMenuItems[] = 
	{
		{ "&Edit",              0, 0, 0, FL_SUBMENU },
		{ "&Copy All", FL_CTRL + 'c', (Fl_Callback *) CB_Copy },
		{ 0 },

		{ "&View",              0, 0, 0, FL_SUBMENU },
		{ "&Show Var. Type", 0, (Fl_Callback *) CB_ShowVarType, 0, FL_MENU_TOGGLE },
		{ "Simp&le Text", 0, (Fl_Callback *) CB_SimpleText, 0, FL_MENU_TOGGLE },
		{ 0 },

		{ "&Window",              0, 0, 0, FL_SUBMENU },
		{ "&Editor", FL_CTRL + FL_SHIFT + 'e', (Fl_Callback *) CB_ShowEditorWin },
		{ "&Output", FL_CTRL + FL_SHIFT + 'o', (Fl_Callback *) CB_ShowOutputWin },
		{ "&Visualization", FL_CTRL + FL_SHIFT + 'v', (Fl_Callback *) CB_ShowVisWin },
		{ 0 },

		{ 0 }
	};


	int iCount = m_vecMenuItem.size();
	m_vecMenuItem.resize(iCount + 12);

	for (int i = 0; i < 13; i++)
	{
		m_vecMenuItem[iCount + i - 1] = pMenuItems[i];
	}
}



//////////////////////////////////////////////////////////////////////
/// Window Menu Callbacks

void CCLUOutput::MenuShowEditorWin()
{
	if (m_pEditor)
		m_pEditor->show();
}

void CCLUOutput::MenuShowOutputWin()
{
	Show();
}

void CCLUOutput::MenuShowVisWin()
{
	if (m_poglWin)
		m_poglWin->ShowWindow();
}



void CCLUOutput::CB_ShowEditorWin(Fl_Widget *pWidget, void *pvData)
{
	if (pvData)
		((CCLUOutput *) pvData)->MenuShowEditorWin();
}

void CCLUOutput::CB_ShowOutputWin(Fl_Widget *pWidget, void *pvData)
{
	if (pvData)
		((CCLUOutput *) pvData)->MenuShowOutputWin();
}

void CCLUOutput::CB_ShowVisWin(Fl_Widget *pWidget, void *pvData)
{
	if (pvData)
		((CCLUOutput *) pvData)->MenuShowVisWin();
}


/////////////////////////////////////////////////////////////////////////////
/// Show Variable type

void CCLUOutput::CB_ShowVarType(Fl_Widget *pWidget, void *pvData)
{
	if (pvData)
		((CCLUOutput *) pvData)->ShowVarType();
}

void CCLUOutput::ShowVarType()
{
	if (m_bShowType)
		m_bShowType = false;
	else
		m_bShowType = true;

	Update();
}


/////////////////////////////////////////////////////////////////////////////
/// Simple Text

void CCLUOutput::CB_SimpleText(Fl_Widget *pWidget, void *pvData)
{
	if (pvData)
		((CCLUOutput *) pvData)->SimpleText();
}

void CCLUOutput::SimpleText()
{
	if (m_bSimpleText)
		m_bSimpleText = false;
	else
		m_bSimpleText = true;

	Update();
}


/////////////////////////////////////////////////////////////////////////////
/// Copy Output to Clipboard

void CCLUOutput::CB_Copy(Fl_Widget* pWidget, void *pData)
{
	if (pData)
		((CCLUOutput *) pData)->Copy();
}

void CCLUOutput::Copy()
{
	cStr sText;

	// generate html without internal links and without font commands
	GenHTML(sText, false, false);

	Fl::copy(sText.Str(), sText.Len(), 1);
}



/////////////////////////////////////////////////////////////////////////////
// Info Link Callback

const char* CCLUOutput::InfoLink(Fl_Widget* pWidget, const char *pcURL)
{

	if (strstr(pcURL, "clu://"))
	{
		if (m_poglWin)
		{
			m_poglWin->InfoLink(pWidget, pcURL);
		}
	}
	else if (strstr(pcURL, "clu_output://"))
	{
		char pcCmd[30], pcVal[30], *pcSlash;
		int iVal;
		
		strncpy(pcCmd, &(pcURL[13]), 29);

		pcSlash = strstr(pcCmd, "/");
		if (!pcSlash)
			return 0;

		*pcSlash = 0;

		strncpy(pcVal, &(pcSlash[1]), 29);

		pcSlash = strstr(pcVal, "/");
		if (!pcSlash)
			return 0;

		*pcSlash = 0;

		iVal = atoi(pcVal);

		if (strcmp(pcCmd, "hide_value") == 0)
		{
			int iObjCnt = m_vecOutObj.size();

			if (iVal < 0 || iVal >= iObjCnt)
				return 0;

			m_vecOutObj[iVal].bShowValue = false;
		}
		else if (strcmp(pcCmd, "show_value") == 0)
		{
			int iObjCnt = m_vecOutObj.size();

			if (iVal < 0 || iVal >= iObjCnt)
				return 0;

			m_vecOutObj[iVal].bShowValue = true;
		}
		else if (strcmp(pcCmd, "smaller_font") == 0)
		{
			int iObjCnt = m_vecOutObj.size();

			if (iVal < 0 || iVal >= iObjCnt)
				return 0;

			m_vecOutObj[iVal].iSize--;
		}
		else if (strcmp(pcCmd, "larger_font") == 0)
		{
			int iObjCnt = m_vecOutObj.size();

			if (iVal < 0 || iVal >= iObjCnt)
				return 0;

			m_vecOutObj[iVal].iSize++;
		}
		else
		{
			return 0;
		}

		Update();
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
		::remove("clucalc_which.txt");

		for( iPos = 0; iPos < iCount; iPos++)
		{
			sprintf(pcCmd, "which %s > clucalc_which.txt", pBrowser[iPos]);
			system(pcCmd);

			// Check whether text file exists
			if (stat("clucalc_which.txt", &FileStat) == 0 && FileStat.st_size != 0)
			{
				// We found something
				break;
			}
		}

		::remove("clucalc_which.txt");

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


/////////////////////////////////////////////////////////////////////////////
// Set Output Object List

void CCLUOutput::SetOutputObjectList(const CCLUCodeBase::TOutObjList& rvecOutObj)
{
	int iObj, iObjCnt = rvecOutObj.size();

	if (m_vecOutObj.size() == iObjCnt)
	{
		for (iObj = 0; iObj < iObjCnt; iObj++)
		{
			CCLUCodeBase::SOutputObject& rOldObj = m_vecOutObj[iObj];
			const CCLUCodeBase::SOutputObject& rNewObj = rvecOutObj[iObj];

			if (rOldObj.sName == rNewObj.sName &&
				rOldObj.sType == rNewObj.sType)
			{
				rOldObj.sValue = rNewObj.sValue;
				rOldObj.sInfo  = rNewObj.sInfo;
			}
			else
			{
				m_vecOutObj = rvecOutObj;
				break;
			}
		}
	}
	else
	{
		m_vecOutObj = rvecOutObj;
	}

	Update();
}


/////////////////////////////////////////////////////////////////////////////
// Update Object View

void CCLUOutput::Update()
{
	int iObjCnt = m_vecOutObj.size();

	if (!iObjCnt)
	{
		SetText(" ");
		return;
	}

	cStr sText;

	sText << "<body size=\"" << m_iFontSize << "\">";
	GenHTML(sText);
	sText << "</body>";

	SetText(sText.Str());
}


/////////////////////////////////////////////////////////////////////////////
/// Generate HTML Text from Object list

bool CCLUOutput::GenHTML(cStr &sText, bool bGenLinks, bool bGenFontCmd)
{
	int iCurSize;
	int iObj, iObjCnt = m_vecOutObj.size();

	for (iObj = 0; iObj < iObjCnt; iObj++)
	{
		CCLUCodeBase::SOutputObject& rObj = m_vecOutObj[iObj];

		sText << "<b>" << rObj.sName << "</b> ";

		if ( (m_bShowType || rObj.bShowInfo) && !m_bSimpleText )
		{
			if (bGenFontCmd)
				sText << "<font size=\"" << (m_iFontSize - 2) << "\">";

			sText << "<i>(";
			if (m_bShowType)
				sText << rObj.sType;

			if (m_bShowType && rObj.bShowInfo)
				sText << ", ";

			if (rObj.bShowInfo)
				sText << rObj.sInfo;

			sText << ")</i>";

			if (bGenFontCmd)
				sText << "</font> ";
		}

		if (rObj.bCanHideValue && bGenLinks && !m_bSimpleText)
		{
			if (rObj.bShowValue)
			{
				if (bGenFontCmd)
					sText << "<font size=\"" << (m_iFontSize - 2) 
					<< "\">";

				sText << "(<a href=\"clu_output://hide_value/" << iObj 
					<< "/\">hide</a>)";

				if (bGenFontCmd)
					sText << "</font> ";
			}	
			else
			{
				if (bGenFontCmd)
					sText << "<font size=\"" << m_iFontSize 
					<< "\">";

				sText << "(<a href=\"clu_output://show_value/" << iObj 
					<< "/\">show</a>)";

				if (bGenFontCmd)
					sText << "</font> ";
			}
		}

		if (rObj.bShowValue)
		{
			if (rObj.bCanChangeSize && bGenLinks && !m_bSimpleText)
			{
				if (bGenFontCmd)
					sText << " <font size=\"" << (m_iFontSize - 2) << "\">";

				sText << "<i>(size: ";

				int iCurSize = m_iFontSize + rObj.iSize * 2;

				if (iCurSize > 6)
				{
					sText << "<a href=\"clu_output://smaller_font/" << iObj 
						<< "/\">--</a>";
				}
				else
				{
					sText << "--";
				}

				sText << " [" << rObj.iSize << "] ";

				if (iCurSize < 24)
				{
					sText << "<a href=\"clu_output://larger_font/" << iObj 
						<< "/\">++</a>";
				}
				else
				{
					sText << "++";
				}

				sText << ")</i>";

				if (bGenFontCmd)
					sText << "</font> ";
			}

			sText << rObj.sRelSym;

			if (rObj.iSize != 0)
			{
				iCurSize = m_iFontSize + rObj.iSize * 2;
				if (iCurSize < 6)
					iCurSize = 6;
				else if (iCurSize > 48)
					iCurSize = 48;

				if (bGenFontCmd)
					sText << "<font size=\"" << iCurSize << "\">";
			}

			if (rObj.bBRBeforeValue && !m_bSimpleText)
				sText << "<br>";

			sText << rObj.sValue;

			if (rObj.iSize != 0 && bGenFontCmd)
				sText << "</font>";

			if (rObj.bBRAfterValue && !m_bSimpleText)
				sText << "<p>";

		}

		sText << "<br>";
	}

	return true;
}
