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


// OGLBitmapText.h: Schnittstelle für die Klasse COGLVertex.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLBITMAPTEXT_H__B634C808_40D5_4BE1_8E64_8AC4B1B21B65__INCLUDED_)
#define AFX_OGLBITMAPTEXT_H__B634C808_40D5_4BE1_8E64_8AC4B1B21B65__INCLUDED_


#include "OGLBaseElement.h"
#include "OGLVertex.h"


#define OGL_BMPFONT_FixedSys_Std_9		1
#define OGL_BMPFONT_FixedSys_Bold_9		2
#define OGL_BMPFONT_Times_Std_12		3
#define OGL_BMPFONT_Sylfaen_Bold_24		4
#define OGL_BMPFONT_Symbol_Std_12		5
#define OGL_BMPFONT_Symbol_Bold_12		6
#define OGL_BMPFONT_Symbol_Std_24		7
#define OGL_BMPFONT_T2_Std_12			8
#define OGL_BMPFONT_T2_Std_24			9




class COGLBitmapText
{
public:
	struct SBitmapFont
	{
		// Pointer to table of pointers to letter bitmaps
		const unsigned char **pTable;

		// Width and height of each bitmap
		unsigned uWidth, uHeight;

		// Number of letters in font
		unsigned uSize;
	};

	struct SLetterInfo
	{
		int iWidth;
	};

	struct SFontInfo
	{
		int iYOffset;
		SBitmapFont BitmapFont;
		MemObj<SLetterInfo> mLetterInfo;
	};

public:
	COGLBitmapText(void);
	COGLBitmapText(float fX, float fY, float fZ, const char* pcText);
	COGLBitmapText(COGLVertex &xVex, const char* pcText);
	~COGLBitmapText(void);

	bool SetFont(int iFont)
	{ 
		if (iFont >= 0 && iFont < sm_iFontCount) 
			m_iFont = iFont;
		else return false;
		return true;
	}
		
	void SetScale(float fScale)
	{ if (fScale > 0) m_fScale = fScale; }

	void SetText(const char* pcText);
	void SetPosition(float fX, float fY, float fZ);
	void SetPosition(COGLVertex &rVex);

	// iMode alows different types of Apply calls.
	// Arbitrary data may be passed by pvData.
	bool Apply(int iMode = 0, void *pvData = 0);

	// Initialize Fonts
	// This evaluates the widths of the separate letters in a font
	static void InitFonts();

	static int GetFontCount();

protected:
	int m_iFont;
	float m_fScale;
	cStr m_csText;
	float m_fX, m_fY, m_fZ;

	static MemObj<SFontInfo> m_mFontInfo;

	const static int sm_iFontCount;
	const static SBitmapFont sm_FontsTable[];

};

#endif
