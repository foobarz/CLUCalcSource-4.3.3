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

#include "StdAfx.h"
#include "GL/glut.h"
#include "OGLBitmapText.h"


#include "Font_FixedSys_Std_9.h"
#include "Font_FixedSys_Bold_9.h"
#include "Font_Symbol_Std_12.h"
#include "Font_Symbol_Bold_12.h"
#include "Font_Symbol_Std_24.h"
#include "Font_TimesNewRoman_Std_12.h"
#include "Font_Sylfaen_Bold_24.h"
#include "Font_T2_Std_12.h"
#include "Font_T2_Std_24.h"

const int COGLBitmapText::sm_iFontCount = 9;

const COGLBitmapText::SBitmapFont COGLBitmapText::sm_FontsTable[] = 
{
	{ Font_FixedSys_Std_9_Table, 16, 16, 128 },
	{ Font_FixedSys_Bold_9_Table, 16, 16, 128 },
	{ Font_TimesNewRoman_Std_12_Table, 16, 16, 128 },
	{ Font_Sylfaen_Bold_24_Table, 32, 32, 128 },
	{ Font_Symbol_Std_12_Table, 16, 16, 128 },
	{ Font_Symbol_Bold_12_Table, 16, 16, 128 },
	{ Font_Symbol_Std_24_Table, 32, 32, 128 },
	{ Font_T2_Std_12_Table, 16, 16, 128 },
	{ Font_T2_Std_24_Table, 32, 32, 128 }
};

MemObj<COGLBitmapText::SFontInfo> COGLBitmapText::m_mFontInfo;

int COGLBitmapText::GetFontCount()
{
	return sm_iFontCount;
}

void COGLBitmapText::InitFonts()
{
	int iFont, iLetterCount, iLetter;
	int iBytePos, iByteCol, iMaxByteCol, iByteRow, iMaxByteRow, iMaxCol, iMaxBit, iMinRow;
	unsigned char cMaxByte;

	m_mFontInfo.Add(sm_iFontCount);

	// Loop over all fonts
	for( iFont = 0; iFont < sm_iFontCount; iFont++ )
	{
		SFontInfo &rFontInfo = m_mFontInfo[iFont];

		rFontInfo.BitmapFont = sm_FontsTable[iFont];
		rFontInfo.iYOffset = 0;
		rFontInfo.mLetterInfo.Add(sm_FontsTable[iFont].uSize);

		iMinRow = rFontInfo.BitmapFont.uHeight;
		iLetterCount = (int) sm_FontsTable[iFont].uSize;
		for( iLetter = 0; iLetter < iLetterCount; iLetter++ )
		{
			SLetterInfo &rLetterInfo = rFontInfo.mLetterInfo[iLetter];
			const unsigned char *pcData = sm_FontsTable[iFont].pTable[iLetter];

			cMaxByte = 0;
			iMaxCol = -1;
			iMaxBit = 1;
			iMaxByteCol = rFontInfo.BitmapFont.uWidth / 8;
			iMaxByteRow = rFontInfo.BitmapFont.uHeight;

			for(iBytePos = 0, iByteRow = 0; iByteRow < iMaxByteRow; iByteRow++)
			{
				for(iByteCol = 0; iByteCol < iMaxByteCol; iByteCol++, iBytePos++)
				{
					if (pcData[iBytePos] != 0)
					{
						if (iByteCol > iMaxCol)
							iMaxCol = iByteCol;

						if (iByteRow < iMinRow)
							iMinRow = iByteRow;
					}
				}
			}

			if (iMaxCol == -1)
			{
				// Empty character is spacing of half the total bitmap width.
				rLetterInfo.iWidth = rFontInfo.BitmapFont.uWidth >> 1;
			}
			else
			{
				for (iBytePos = iMaxCol, iByteRow = 0; iByteRow < iMaxByteRow; 
					iByteRow++, iBytePos += iMaxByteCol)
				{
					cMaxByte |= pcData[iBytePos];
				}

				while (cMaxByte <<= 1)
					iMaxBit++;

				rLetterInfo.iWidth = iMaxCol * 8 + iMaxBit + 1;
			}
		}

		rFontInfo.iYOffset = iMinRow - 1;
	}

}



COGLBitmapText::COGLBitmapText(void)
{
	m_iFont = 0;
	m_fScale = 1.0f;
}

COGLBitmapText::COGLBitmapText(float fX, float fY, float fZ, const char* pcText)
{
	m_iFont = 0;
	m_fScale = 1.0f;
	
	m_fX = fX;
	m_fY = fY;
	m_fZ = fZ;
	m_csText = pcText;
}

COGLBitmapText::COGLBitmapText(COGLVertex &xVex, const char* pcText)
{
	m_iFont = 0;
	m_fScale = 1.0f;
	
	m_fX = xVex[0];
	m_fY = xVex[1];
	m_fZ = xVex[2];
	m_csText = pcText;
}


COGLBitmapText::~COGLBitmapText(void)
{
}



/////////////////////////////////////////////////////////////////////
/// Set Text

void COGLBitmapText::SetText(const char* pcText)
{
	if (pcText)
		m_csText = pcText;
}

/////////////////////////////////////////////////////////////////////
/// Set Position

void COGLBitmapText::SetPosition(float fX, float fY, float fZ)
{
	m_fX = fX;
	m_fY = fY;
	m_fZ = fZ;
}

/////////////////////////////////////////////////////////////////////
/// Set Position

void COGLBitmapText::SetPosition(COGLVertex &rVex)
{
	m_fX = rVex[0];
	m_fY = rVex[1];
	m_fZ = rVex[2];
}


/////////////////////////////////////////////////////////////////////
/// Apply Text
// iMode alows different types of Apply calls.
// Arbitrary data may be passed by pvData.

bool COGLBitmapText::Apply(int iMode, void *pvData)
{
	int i;
	int iLen = (int) m_csText.Len();
	char *pcText = m_csText.Str();
	unsigned char cSym;
	GLboolean bValid;
	float fOffset;
	float pfCol[4];
	float pfRMap[2], pfGMap[2], pfBMap[2], pfAMap[2];

	if (m_iFont < 0 || m_iFont >= sm_iFontCount)
		return true;

	SFontInfo &rFontInfo = m_mFontInfo[m_iFont];
	unsigned uWidth = rFontInfo.BitmapFont.uWidth;
	unsigned uHeight = rFontInfo.BitmapFont.uHeight;

	int iFontID = (uWidth == 32 ? (OGL_BMPFONT_Symbol_Std_24 - 1) 
								: (OGL_BMPFONT_Symbol_Bold_12 - 1));
	SFontInfo &rSymbolFontInfo = m_mFontInfo[iFontID];
	unsigned uSymbolWidth = rSymbolFontInfo.BitmapFont.uWidth;
	unsigned uSymbolHeight = rSymbolFontInfo.BitmapFont.uHeight;

	int iOffset, iSymbolOffset;

	if (!iLen || !pcText)
		return true;

	glRasterPos3f(m_fX, m_fY, m_fZ);
	glGetBooleanv(GL_CURRENT_RASTER_POSITION_VALID, &bValid);

	// Check wether raster position can be seen
	if (!bValid)
		// If not, don't draw text.
		return true;

	glPixelZoom(m_fScale, -m_fScale);
	glPixelStorei(GL_UNPACK_ALIGNMENT, (uWidth / 8));

	glGetFloatv(GL_CURRENT_COLOR, pfCol);

	pfRMap[0] = 0;
	pfRMap[1] = pfCol[0];
	glPixelMapfv(GL_PIXEL_MAP_I_TO_R, 2, pfRMap);

	pfGMap[0] = 0;
	pfGMap[1] = pfCol[1];
	glPixelMapfv(GL_PIXEL_MAP_I_TO_G, 2, pfGMap);

	pfBMap[0] = 0;
	pfBMap[1] = pfCol[2];
	glPixelMapfv(GL_PIXEL_MAP_I_TO_B, 2, pfBMap);

	pfAMap[0] = 0;
	pfAMap[1] = pfCol[3];
	glPixelMapfv(GL_PIXEL_MAP_I_TO_A, 2, pfAMap);

	glPixelTransferi(GL_MAP_COLOR, 1);

	// Use glBitmap only to adapt raster position.
	glBitmap(0, 0, 0, 0, 0, float(uHeight)*m_fScale, NULL);
	iOffset = rFontInfo.iYOffset * (uWidth / 8);
	uHeight -= rFontInfo.iYOffset;

	iSymbolOffset = rSymbolFontInfo.iYOffset * (uSymbolWidth / 8);
	uSymbolHeight -= rSymbolFontInfo.iYOffset;

	const unsigned char **pTable = rFontInfo.BitmapFont.pTable;
	const unsigned char **pSymbolTable = rSymbolFontInfo.BitmapFont.pTable;

	for(i=0;i<iLen;i++)
	{
		cSym = pcText[i];

		if (cSym >= 32 && cSym < rFontInfo.BitmapFont.uSize)
		{
			if (cSym != '\\')
			{
				int iWidth = rFontInfo.mLetterInfo[cSym].iWidth;

				//glBitmap(Font.uWidth, Font.uHeight, 0, 0, Font.uWidth, 0, Font.pTable[cSym]);
				glDrawPixels(iWidth, uHeight, GL_COLOR_INDEX, GL_BITMAP, pTable[cSym]+iOffset);

				// Use glBitmap only to advance raster position.
				glBitmap(0, 0, 0, 0, float(iWidth)*m_fScale, 0, NULL);
			}
			else
			{
				if ((cSym = pcText[++i]) == 0)
					break;

				int iWidth = rSymbolFontInfo.mLetterInfo[cSym].iWidth;

				glPixelStorei(GL_UNPACK_ALIGNMENT, (uSymbolWidth / 8));
			
				//glBitmap(Font.uWidth, Font.uHeight, 0, 0, Font.uWidth, 0, Font.pTable[cSym]);
				glDrawPixels(iWidth, uSymbolHeight, GL_COLOR_INDEX, GL_BITMAP, 
								pSymbolTable[cSym]+iSymbolOffset);

				// Use glBitmap only to advance raster position.
				glBitmap(0, 0, 0, 0, float(iWidth)*m_fScale, 0, NULL);

				glPixelStorei(GL_UNPACK_ALIGNMENT, (uWidth / 8));
			}
		}
	}

	glPixelTransferi(GL_MAP_COLOR, 0);
	glPixelZoom(1, 1);

	return true;
}

