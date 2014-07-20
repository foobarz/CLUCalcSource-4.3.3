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


// OGLLatexText.h: Schnittstelle für die Klasse COGLLatexText
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLLATEXTEXT_H__B634C808_40D5_4BE1_8E64_8AC4B1B21B65__INCLUDED_)
#define AFX_OGLLATEXTEXT_H__B634C808_40D5_4BE1_8E64_8AC4B1B21B65__INCLUDED_

#include <map>
#include <string>
#include <fstream>

using namespace std;

#include "OGLBaseElement.h"
#include "OGLVertex.h"
#include "OGLBitmap.h"

#include <sys/types.h>
#include <sys/stat.h>

#ifdef _GNUCPP3_
#   define _stat stat
#endif


class COGLLatexText
{
public:
	struct SLatexText
	{
		string sText;
		string sBMPName;
		string sBMPFilename;
		string sScriptName;
		string sScriptPath;
		float fScale;
		COGLBitmap oglBitmap;
		COGLColor oglColor;
		bool bInUse;
		bool bIsRendered;
		// Variables used if latex file is given.
		bool bIsFile;
		string sLatexFilename;
		struct _stat FileInfo; // Used in case latex filename is given.
	};

	typedef map<string, SLatexText> TLatexMap;

	class CRenderError
	{
	public:
		CRenderError()
		{ m_sErrorText = ""; }

		CRenderError(const char* pcText)
		{ m_sErrorText = pcText; }

		string m_sErrorText;
	};

public:
	COGLLatexText(void);
	~COGLLatexText(void);

	// Reset All In Use flags
	void ResetInUseFlags();

	// Remove all elements from map that are not in use
	void PruneMap();

	// Set Name of current script without extension
	void SetScriptName(const char* pcScriptName)
	{ m_sScriptName = pcScriptName; }

	// Set Path to current script
	void SetScriptPath(const char* pcScriptPath)
	{ m_sScriptPath = pcScriptPath; }

	void EnableRenderLatex(bool bVal = true)
	{ m_bRenderLatex = bVal; }

	string GetRenderErrorText()
	{ return m_sRenderError; }

	float GetMagStepScale(int iMagStep);

	bool Add(const char* pcText, float fX, float fY, float fZ, 
			float fScale = 1.0f, float fVAlign = 0.0f, float fHAlign = 0.0f, 
			const char *pcBMPName = 0);
	bool Add(const char* pcText, COGLVertex &rVex, 
			float fScale = 1.0f, float fVAlign = 0.0f, float fHAlign = 0.0f, 
			const char *pcBMPName = 0)
	{ return Add(pcText, rVex[0], rVex[1], rVex[2], fScale, fVAlign, fHAlign, pcBMPName); }

	// iMode alows different types of Apply calls.
	// Arbitrary data may be passed by pvData.
	// If pvData == 0 then all bitmaps are displayed.
	// If pvData != 0 it has to point to a string which
	// identifies the element to be displayed.
	bool Apply(int iMode = 0, void *pvData = 0);

	// Get extension of Latex Bitmap in world coordinates
	// for text given in pcText.
	bool GetWorldCoordDims(const char *pcText, float &fLeft, float &fRight, 
							float &fBottom, float &fTop,
							float &fZ1, float &fZ2);

	// Get pointer to OGLBitmap for text, if text has been rendered.
	COGLBitmap* GetBitmap(const char *pcText);

protected:
	bool RenderLatex(SLatexText &rLatex);
	bool GetGhostscriptPath();

protected:
	TLatexMap m_mapLatex;
	string m_sScriptPath;
	string m_sScriptName;
	string m_sRenderError;
	
	string m_sGSPath;
	string m_sGSError; // String contains error message if ghostscript could not be located

	bool m_bRenderLatex;

};

#endif
