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


// OGLBitmap.h: Schnittstelle für die Klasse OGLBitmap
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLBITMAP_H__B634C808_40D5_4BE1_8E64_8AC4B1B21B65__INCLUDED_)
#define AFX_OGLBITMAP_H__B634C808_40D5_4BE1_8E64_8AC4B1B21B65__INCLUDED_


#include "OGLBaseElement.h"
#include "OGLVertex.h"
#include "OGLColor.h"

#include "IL/il.h"
#include "IL/ilu.h"
#include "IL/ilut.h"

#ifdef LoadBitmap
#	undef LoadBitmap
#endif



class COGLBitmap
{
public:
	typedef unsigned char uchar;
	struct TBGRAPixel;

	struct TRGBAPixel
	{
		unsigned char c[4];

		TRGBAPixel() { c[0] = 0; c[1] = 0; c[2] = 0; c[3] = 0; }
		TRGBAPixel& Set(uchar r, uchar g, uchar b, uchar a)
		{ c[0] = r; c[1] = g; c[2] = b; c[3] = a; return *this; }

		void operator= (TBGRAPixel& rIPt)
		{
			c[0] = rIPt.c[2];
			c[1] = rIPt.c[1];
			c[2] = rIPt.c[0];
			c[3] = rIPt.c[3];
		}

		bool operator== (const TRGBAPixel& rIPt)
		{
			if (c[0] == rIPt.c[0] &&
				c[1] == rIPt.c[1] &&
				c[2] == rIPt.c[2])
				return true;

			return false;
		}

		void InvertRGB()
		{
			c[0] = 255 - c[0];
			c[1] = 255 - c[1];
			c[2] = 255 - c[2];
		}

		uchar Luminance()
		{
			double dVal = 0.299*double(c[0]) + 0.587*double(c[1]) + 0.144*double(c[2]);
		
			if (dVal > 255.0)
				return 255;
			else
				return uchar(dVal);
		}
	};

	struct TBGRAPixel
	{
		unsigned char c[4];

		TBGRAPixel() { c[0] = 0; c[1] = 0; c[2] = 0; c[3] = 0; }
		TBGRAPixel& Set(uchar b, uchar g, uchar r, uchar a)
		{ c[0] = b; c[1] = g; c[2] = r; c[3] = a; return *this; }
	};

	enum TTransMode 
	{
		Exact = 0,
		Luminance,
		Font
	};

public:
	COGLBitmap(void);
	COGLBitmap(const COGLBitmap& oglBitmap);
	COGLBitmap(float fX, float fY, float fZ, const char* pcFilename);
	COGLBitmap(COGLVertex &xVex, const char* pcFilename);
	~COGLBitmap(void);

	COGLBitmap& operator= (const COGLBitmap& oglBitmap);

	// Reduce Image to given size and write result into rImg
	bool ExtractSubImage(COGLBitmap& rSrcImg, int iX, int iY, int iCX, int iCY);

	// Get width and height of bitmap in world coordinates.
	bool GetWorldCoordDims(float &fLeft, float &fRight, float &fBottom, float &fTop,
							float &fZ1, float &fZ2);

	void GetFilename(cStr& csFilename)
	{ csFilename = m_csFilename; }

	void GetSize(int &iWidth, int &iHeight)
	{ iWidth = m_iWidth; iHeight = m_iHeight; }

	bool GetPixel(int iX, int iY, uchar &ucR, uchar &ucG, uchar &ucB, uchar &ucA);

	bool SetSize(int iWidth, int iHeight, COGLColor *pcolClear = 0);
	bool ResizeImage(int iWidth, int iHeight);
	bool ResizeCanvas(int iWidth, int iHeight, COGLColor *pcolClear = 0);
	bool RotateImage(float fAngle);
	bool InsertImage(COGLBitmap& rSrcImg, int iX, int iY, int iTrgC = -1, int iSrcC = -1);

	bool MaskEqual(COGLBitmap& rMask, COGLColor &rCol);
	bool MaskNotEqual(COGLBitmap& rMask, COGLColor &rCol);

	void SetScale(float fScale)
	{ if (fScale > 0) m_fScale = fScale; }

	void SetFilename(const char* pcText);
	void SetPosition(float fX, float fY, float fZ);
	void SetPosition(COGLVertex &rVex);
	void SetTransparentColor(unsigned char r, unsigned char g, unsigned char b)
	{ m_rgbaTransPix.Set(r, g, b, 0); }
	void SetTransparentColor(COGLColor &rCol)
	{ 
		float fCol;
		m_rgbaTransPix.Set( 
			((fCol = rCol[0]*255.0f) >= 255.0f ? uchar(255) : uchar(fCol)),
			((fCol = rCol[1]*255.0f) >= 255.0f ? uchar(255) : uchar(fCol)),
			((fCol = rCol[2]*255.0f) >= 255.0f ? uchar(255) : uchar(fCol)),
			0); 
	}

	void SetTransparencyMode(TTransMode iMode)
	{ m_iTransMode = iMode; }

	void SetVAlign(float fAlign)
	{ 
		/*if (fAlign <= -1.0f) m_fAlign = -1.0f;
		else if (fAlign >= 2.0f) m_fAlign = 2.0f;
		else*/
		m_fVAlign = fAlign;
	}

	void SetHAlign(float fAlign)
	{ 
		/*if (fAlign <= -1.0f) m_fAlign = -1.0f;
		else if (fAlign >= 2.0f) m_fAlign = 2.0f;
		else*/
		m_fHAlign = fAlign;
	}

	void EnableInvert(bool bVal = true)
	{ m_bInvert = bVal; }

	void EnableTransparency(bool bVal = true)
	{ m_bUseTransparency = bVal; }

	void EnableScaleWithCurColor(bool bVal = true)
	{ m_bScaleWithCurColor = bVal; }

	// Set all rgb values to given color
	void FlushRGB(COGLColor& rCol);

	// If bForce == false, then bitmap is only loaded
	// if its filename is different to that of the 
	// currently loaded bitmap. Otherwise, it is always loaded.
	bool LoadBitmap(bool bForce = false);

	// If pcFilename == 0, then internal name is used.
	bool SaveBitmap(const char *pcFilename = 0);

	// Apply image as texture
	bool ApplyAsTexture(bool bGenMipMaps = true);

	// Delete Texture
	bool DeleteTexture();

	// iMode alows different types of Apply calls.
	// Arbitrary data may be passed by pvData.
	bool Apply(int iMode = 0, void *pvData = 0);

	// Return a pointer to the data
	uchar* Data();

protected:
	void ResetVars();

protected:
	float m_fScale;
	cStr m_csFilename;
	cStr m_csCurFilename;
	float m_fX, m_fY, m_fZ;
	bool m_bInvert;
	bool m_bUseTransparency;
	bool m_bScaleWithCurColor;
	bool m_bIsTexture;
	TTransMode m_iTransMode;
	TRGBAPixel m_rgbaTransPix;

	int m_iWidth, m_iHeight;

	// If m_fVAlign == 0 then bitmaps are aligned with their bottom 
	// to the point given. If Align == 1, then it is aligned with
	// the top.
	float m_fVAlign;

	// If m_fHAlign == 0 then bitmaps are aligned with their left 
	// to the point given. If Align == 1, then it is aligned with
	// the right.
	float m_fHAlign;

	ILuint m_uImgID;
	GLuint m_uTexID;
};

#endif
