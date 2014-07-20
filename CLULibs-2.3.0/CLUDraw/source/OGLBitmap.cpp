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
#include "OGLBitmap.h"

extern "C"
{
	int ConvertJPEGtoPS(const char* pcSrcFilename, const char* pcTrgFilename);
}


COGLBitmap::COGLBitmap(void)
{
	ResetVars();
}

COGLBitmap::COGLBitmap(const COGLBitmap& oglBitmap)
{
	*this = oglBitmap;
}

COGLBitmap::COGLBitmap(float fX, float fY, float fZ, const char* pcFilename)
{
	ResetVars();
	
	m_fX = fX;
	m_fY = fY;
	m_fZ = fZ;
	m_csFilename = pcFilename;
}

COGLBitmap::COGLBitmap(COGLVertex &xVex, const char* pcFilename)
{
	ResetVars();
	
	m_fX = xVex[0];
	m_fY = xVex[1];
	m_fZ = xVex[2];
	m_csFilename = pcFilename;
}


COGLBitmap::~COGLBitmap(void)
{
	ilDeleteImages(1, &m_uImgID);

	if (m_bIsTexture)
	{
		glDeleteTextures(1, &m_uTexID);
	}
}


void COGLBitmap::ResetVars()
{
	m_fVAlign = 0.0f;
	m_fHAlign = 0.0f;
	m_fScale = 1.0f;
	m_iWidth = 0;
	m_iHeight = 0;
	m_bInvert = false;
	m_bUseTransparency = false;
	m_bScaleWithCurColor = false;
	m_iTransMode = Exact;

	m_uTexID = 0;
	m_bIsTexture = false;

	m_uImgID = 0;
	ilGenImages(1, &m_uImgID);
	ilBindImage(m_uImgID);
	ilClearColor(0,0,0,1);

	// Convert the image to RGBA, uchar
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);


	m_fX = 0.0f;
	m_fY = 0.0f;
	m_fZ = 0.0f;
	m_csFilename = "";
}

COGLBitmap& COGLBitmap::operator= (const COGLBitmap& oglBitmap)
{
	m_fVAlign = oglBitmap.m_fVAlign;
	m_fHAlign = oglBitmap.m_fHAlign;
	m_fScale = oglBitmap.m_fScale;
	m_iWidth = oglBitmap.m_iWidth;
	m_iHeight = oglBitmap.m_iHeight;
	m_bInvert = oglBitmap.m_bInvert;
	m_bUseTransparency = oglBitmap.m_bUseTransparency;
	m_bScaleWithCurColor = oglBitmap.m_bScaleWithCurColor;
	m_iTransMode = oglBitmap.m_iTransMode;
	m_rgbaTransPix = oglBitmap.m_rgbaTransPix;

	m_fX = oglBitmap.m_fX;
	m_fY = oglBitmap.m_fY;
	m_fZ = oglBitmap.m_fZ;
	m_csFilename = oglBitmap.m_csFilename;

	ilBindImage(oglBitmap.m_uImgID);
	m_uImgID = ilCloneCurImage();

	m_uTexID = 0;
	m_bIsTexture = false;

	return *this;
}

/////////////////////////////////////////////////////////////////////
// Get Pixel value at given position

bool COGLBitmap::GetPixel(int iX, int iY, uchar &ucR, uchar &ucG, uchar &ucB, uchar &ucA)
{
	if (iX < 0 || iX >= m_iWidth ||
		iY < 0 || iY >= m_iHeight)
		return false;

	int iPos = iY * m_iWidth + iX;

	ilBindImage(m_uImgID);
	TRGBAPixel &xP = ((TRGBAPixel *) ilGetData())[iPos];

	ucR = xP.c[0];
	ucG = xP.c[1];
	ucB = xP.c[2];
	ucA = xP.c[3];

	return true;
}


/////////////////////////////////////////////////////////////////////
// Reduce Image to given size

bool COGLBitmap::ExtractSubImage(COGLBitmap& rSrcImg, int iX, int iY, int iCX, int iCY)
{
	int iWidth, iHeight;

	rSrcImg.GetSize(iWidth, iHeight);

	if (iCX <= 0 || iCY <=0 ||
		iX < 0 || iX+iCX-1 >= iWidth ||
		iY < 0 || iY+iCY-1 >= iHeight)
		return false;

	ilBindImage(m_uImgID);
	ilCopyImage(rSrcImg.m_uImgID);
	if (iluCrop(iX, iY, 0, iCX, iCY, 1) == IL_FALSE)
		return false;
	
	m_iWidth = iCX;
	m_iHeight = iCY;

	DeleteTexture();
	return true;
}


/////////////////////////////////////////////////////////////////////
/// Get Data Pointer

COGLBitmap::uchar* COGLBitmap::Data()
{ 
	ilBindImage(m_uImgID);
	return (uchar *) ilGetData(); 
}


/////////////////////////////////////////////////////////////////////
/// Get World Coordinate Dimensions

bool COGLBitmap::GetWorldCoordDims(float &fLeft, float &fRight, float &fBottom, float &fTop,
								   float &fZ1, float &fZ2)
{

	double pdPos[4], pdObj[3];
	double pdMV[16], pdPM[16];
	int piVP[4];

	glGetDoublev(GL_MODELVIEW_MATRIX, pdMV);
	glGetDoublev(GL_PROJECTION_MATRIX, pdPM);
	glGetIntegerv(GL_VIEWPORT, piVP);

	glRasterPos3f(m_fX, m_fY, m_fZ);

	if (m_fVAlign != 0.0f || m_fHAlign != 0.0f)
	{
		float fYMove = -float(m_iHeight) * m_fVAlign * m_fScale;
		float fXMove = -float(m_iWidth) * m_fHAlign * m_fScale;
		glBitmap(0, 0, 0, 0, fXMove, fYMove, 0);
		glGetDoublev(GL_CURRENT_RASTER_POSITION, pdPos);

		gluUnProject(pdPos[0], pdPos[1], pdPos[2], pdMV, pdPM, piVP, 
			&pdObj[0], &pdObj[1], &pdObj[2]);

		fLeft = float(pdObj[0]);
		fBottom = float(pdObj[1]);
		fZ1 = float(pdObj[2]);
	}
	else
	{
		fLeft = m_fX;
		fBottom = m_fY;
		fZ1 = m_fZ;
	}

	glBitmap(0, 0, 0, 0, float(m_iWidth)*m_fScale, float(m_iHeight)*m_fScale, 0);
	glGetDoublev(GL_CURRENT_RASTER_POSITION, pdPos);

	gluUnProject(pdPos[0], pdPos[1], pdPos[2], pdMV, pdPM, piVP, 
				 &pdObj[0], &pdObj[1], &pdObj[2]);

	fRight = float(pdObj[0]);
	fTop = float(pdObj[1]);
	fZ2 = float(pdObj[2]);

	return true;
}


/////////////////////////////////////////////////////////////////////
/// Set Text

void COGLBitmap::SetFilename(const char* pcFilename)
{
	if (pcFilename)
		m_csFilename = pcFilename;
}

/////////////////////////////////////////////////////////////////////
/// Set Position

void COGLBitmap::SetPosition(float fX, float fY, float fZ)
{
	m_fX = fX;
	m_fY = fY;
	m_fZ = fZ;
}

/////////////////////////////////////////////////////////////////////
/// Set Position

void COGLBitmap::SetPosition(COGLVertex &rVex)
{
	m_fX = rVex[0];
	m_fY = rVex[1];
	m_fZ = rVex[2];
}


/////////////////////////////////////////////////////////////////////
/// Flush RGB

void COGLBitmap::FlushRGB(COGLColor& rCol)
{
	ilBindImage(m_uImgID);

	int iPixel, iPixelCount = m_iWidth * m_iHeight;
	TRGBAPixel *pData = (TRGBAPixel *) ilGetData();

	unsigned char pucCol[3];
	float fCol;
	int i;

	for(i=0;i<3;i++)
	{
		fCol = rCol.Data()[i] * 255.0f;
		if (fCol > 255.0f)
			pucCol[i] = 255;
		else
			pucCol[i] = (unsigned char) fCol;
	}

	for (iPixel=0; iPixel<iPixelCount; iPixel++)
	{
		memcpy(pData[iPixel].c, pucCol, 3*sizeof(unsigned char));
	}

	DeleteTexture();
}


/////////////////////////////////////////////////////////////////////
/// Set Size of bitmap


bool COGLBitmap::SetSize(int iWidth, int iHeight, COGLColor *pcolClear)
{
	if (iWidth <= 0 || iHeight <= 0)
		return false;

	if (pcolClear)
	{
		ilClearColor((*pcolClear)[0],(*pcolClear)[1],(*pcolClear)[2],(*pcolClear)[3]);
	}

	ilBindImage(m_uImgID);

	if (iluEnlargeCanvas(iWidth, iHeight, 1) == IL_FALSE)
	{
		if (pcolClear)
			ilClearColor(0,0,0,1);
		return false;
	}

	m_iWidth = iWidth;
	m_iHeight = iHeight;

	if (pcolClear)
		ilClearColor(0,0,0,1);

	DeleteTexture();

	return true;
}


/////////////////////////////////////////////////////////////////////
/// Resize the Image

bool COGLBitmap::ResizeImage(int iWidth, int iHeight)
{
	if (iWidth <= 0 || iHeight <= 0)
		return false;

	ilBindImage(m_uImgID);
	iluImageParameter(ILU_FILTER, ILU_SCALE_TRIANGLE);	
	if (iluScale(iWidth, iHeight, 1) == IL_FALSE)
		return false;

	m_iWidth = iWidth;
	m_iHeight = iHeight;

	DeleteTexture();

	return true;
}


/////////////////////////////////////////////////////////////////////
/// Rotate the Image

bool COGLBitmap::RotateImage(float fAngle)
{
/*
	ILimage *pCurImg, *pRotImg;

	ilBindImage(m_uImgID);
	pCurImg = ilGetCurImage();

	pRotImg = iluRotate_(pCurImg, fAngle);

	ilTexImage(pRotImg->Width, pRotImg->Height, pRotImg->Depth, pRotImg->Bpp, 
			   pRotImg->Format, pRotImg->Type, pRotImg->Data);
	ilCopyImageAttr(pCurImg, pRotImg);
	ilCloseImg(pRotImg);

	DeleteTexture();
*/
	return true;
}

/////////////////////////////////////////////////////////////////////
/// Resize the Canvas

bool COGLBitmap::ResizeCanvas(int iWidth, int iHeight, COGLColor *pcolClear)
{
	if (iWidth <= 0 || iHeight <= 0)
		return false;
	else if (iWidth == m_iWidth && iHeight == m_iHeight)
		return true;

	int iOffX, iOffY;
	ilBindImage(m_uImgID);
	iluImageParameter(ILU_PLACEMENT, ILU_CENTER);
	
	if (pcolClear)
	{
		ilClearColor((*pcolClear)[0],(*pcolClear)[1],(*pcolClear)[2],(*pcolClear)[3]);
	}

	if (iWidth >= m_iWidth && iHeight >= m_iHeight)
	{
		if (iluEnlargeCanvas(iWidth, iHeight, 1) == IL_FALSE)
		{
			if (pcolClear)
				ilClearColor(0,0,0,1);
			return false;
		}
	}
	else if (iWidth <= m_iWidth && iHeight <= m_iHeight)
	{
		iOffX = (m_iWidth - iWidth) >> 1;
		iOffY = (m_iHeight - iHeight) >> 1;

		if (iluCrop(iOffX, iOffY, 0, iWidth, iHeight, 1) == IL_FALSE)
		{
			if (pcolClear)
				ilClearColor(0,0,0,1);
			return false;
		}
	}
	else if (iWidth <= m_iWidth && iHeight >= m_iHeight)
	{
		if (iluEnlargeCanvas(m_iWidth, iHeight, 1) == IL_FALSE)
		{
			if (pcolClear)
				ilClearColor(0,0,0,1);
			return false;
		}

		iOffX = (m_iWidth - iWidth) >> 1;

		if (iluCrop(iOffX, 0, 0, iWidth, iHeight, 1) == IL_FALSE)
		{
			if (pcolClear)
				ilClearColor(0,0,0,1);
			return false;
		}
	}
	else if (iWidth >= m_iWidth && iHeight <= m_iHeight)
	{
		if (iluEnlargeCanvas(iWidth, m_iHeight, 1) == IL_FALSE)
		{
			if (pcolClear)
				ilClearColor(0,0,0,1);
			return false;
		}

		iOffY = (m_iHeight - iHeight) >> 1;

		if (iluCrop(0, iOffY, 0, iWidth, iHeight, 1) == IL_FALSE)
		{
			if (pcolClear)
				ilClearColor(0,0,0,1);
			return false;
		}
	}

	if (pcolClear)
		ilClearColor(0,0,0,1);

	m_iWidth = iWidth;
	m_iHeight = iHeight;

	DeleteTexture();

	return true;
}

/////////////////////////////////////////////////////////////////////
/// Insert Image
/// Overwrite part of this image with given image starting at pos iX, iY.
/// If given image would extend over this image, then false is returned
/// and this image is not changed.

bool COGLBitmap::InsertImage(COGLBitmap& rSrcImg, int iX, int iY, int iTrgC, int iSrcC)
{
	int iSrcW, iSrcH, iIdx, iSrcPos, iTrgPos, iSrcY, iLineSize;
	TRGBAPixel *pTrgData, *pSrcData;

	rSrcImg.GetSize(iSrcW, iSrcH);

	if (iX + iSrcW > m_iWidth ||
		iY + iSrcH > m_iHeight)
		return false;

	pSrcData = (TRGBAPixel *) rSrcImg.Data();

	ilBindImage(m_uImgID);
	pTrgData = (TRGBAPixel *) ilGetData();

	iLineSize = iSrcW * sizeof(TRGBAPixel);
	if (iTrgC < 0 || iTrgC > 3 ||
		iSrcC < 0 || iSrcC > 3)
	{
		for (iSrcY = 0, iSrcPos = 0, iTrgPos = iY*m_iWidth + iX; 
			iSrcY < iSrcH;
			iSrcY++, iSrcPos += iSrcW, iTrgPos += m_iWidth)
		{
			memcpy(&pTrgData[iTrgPos], &pSrcData[iSrcPos], iLineSize);
		}
	}
	else
	{
		int iSrcX, iSrcPosX, iTrgPosX;

		for (iSrcY = 0, iSrcPos = 0, iTrgPos = iY*m_iWidth + iX; 
			iSrcY < iSrcH;
			iSrcY++, iSrcPos += iSrcW, iTrgPos += m_iWidth)
		{
			for (iSrcX = 0, iSrcPosX = iSrcPos, iTrgPosX = iTrgPos;
				 iSrcX < iSrcW;
				 iSrcX++, iSrcPosX++, iTrgPosX++)
			{
				pTrgData[iTrgPosX].c[iTrgC] = pSrcData[iSrcPosX].c[iSrcC];
			}
		}
	}

	DeleteTexture();

	return true;
}


/////////////////////////////////////////////////////////////////////
/// Returns mask of same dimension as image. The mask is set to 1
/// whereever the image is equal to the given color.

bool COGLBitmap::MaskEqual(COGLBitmap& rMask, COGLColor &rCol)
{
	TRGBAPixel *pTrgData, *pSrcData;
	TRGBAPixel PixCol;

	rMask.SetSize(m_iWidth, m_iHeight);
	pTrgData = (TRGBAPixel *) rMask.Data();

	ilBindImage(m_uImgID);
	pSrcData = (TRGBAPixel *) ilGetData(); 

	float fVal;
	for (int i = 0; i < 3; i++)
	{
		fVal = rCol[i] * 255.0f;
		if (fVal > 255.0f)
			PixCol.c[i] = 255;
		else if (fVal < 0.0f)
			PixCol.c[i] = 0;
		else
			PixCol.c[i] = (unsigned char) fVal;
	}

	int iPix, iCount = m_iWidth * m_iHeight;

	for (iPix = 0; iPix < iCount; iPix++)
	{
		if (pSrcData[iPix].c[0] == PixCol.c[0] &&
			pSrcData[iPix].c[1] == PixCol.c[1] &&
			pSrcData[iPix].c[2] == PixCol.c[2])
			pTrgData[iPix].Set(255, 255, 255, 255);
	}

	return true;
}


/////////////////////////////////////////////////////////////////////
/// Returns mask of same dimension as image. The mask is set to 1
/// whereever the image is NOT equal to the given color.

bool COGLBitmap::MaskNotEqual(COGLBitmap& rMask, COGLColor &rCol)
{
	TRGBAPixel *pTrgData, *pSrcData;
	TRGBAPixel PixCol;

	rMask.SetSize(m_iWidth, m_iHeight);
	pTrgData = (TRGBAPixel *) rMask.Data();

	ilBindImage(m_uImgID);
	pSrcData = (TRGBAPixel *) ilGetData(); 

	float fVal;
	for (int i = 0; i < 3; i++)
	{
		fVal = rCol[i] * 255.0f;
		if (fVal > 255.0f)
			PixCol.c[i] = 255;
		else if (fVal < 0.0f)
			PixCol.c[i] = 0;
		else
			PixCol.c[i] = (unsigned char) fVal;
	}

	int iPix, iCount = m_iWidth * m_iHeight;

	for (iPix = 0; iPix < iCount; iPix++)
	{
		if (pSrcData[iPix].c[0] != PixCol.c[0] ||
			pSrcData[iPix].c[1] != PixCol.c[1] ||
			pSrcData[iPix].c[2] != PixCol.c[2])
			pTrgData[iPix].Set(255, 255, 255, 255);
	}

	return true;
}


/////////////////////////////////////////////////////////////////////
/// Load Bitmap

bool COGLBitmap::LoadBitmap(bool bForce)
{
	// Check whether Bitmap has already been loaded
	if (!bForce && m_csFilename == m_csCurFilename)
		return true;

	ilBindImage(m_uImgID);

	// Load the image
	if (ilLoadImage(m_csFilename.Str()) == IL_FALSE)
		return false;
	
	// Convert the image to RGBA, uchar
	if (ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE) == IL_FALSE)
		return false;

	m_iWidth = ilGetInteger(IL_IMAGE_WIDTH);
	m_iHeight = ilGetInteger(IL_IMAGE_HEIGHT);

	m_csCurFilename = m_csFilename;


	if (m_bUseTransparency)
	{
		TRGBAPixel* pRGBData = (TRGBAPixel *) ilGetData();
		int iP, iSize = m_iWidth * m_iHeight;
		unsigned char ucLumTrans = m_rgbaTransPix.Luminance();
		//TRGBAPixel rgbaInvTransPix = m_rgbaTransPix;
		//rgbaInvTransPix.InvertRGB();

		if (m_iTransMode == Exact)
		{
			for (iP = 0; iP < iSize; iP++)
			{
				if (pRGBData[iP] == m_rgbaTransPix)
					pRGBData[iP].c[3] = 0;
			}
		}
		else if (m_iTransMode == Luminance)
		{
			unsigned char ucLum;
			
			for (iP = 0; iP < iSize; iP++)
			{
				ucLum = pRGBData[iP].Luminance();

				if (ucLum > ucLumTrans)
					pRGBData[iP].c[3] = ucLum - ucLumTrans;
				else
					pRGBData[iP].c[3] = ucLumTrans - ucLum;
			}
		}
		else if (m_iTransMode == Font)
		{
			unsigned char ucLum;

			for (iP = 0; iP < iSize; iP++)
			{
				ucLum = pRGBData[iP].Luminance();
				pRGBData[iP].Set(1, 1, 1, 1);

				if (ucLum > ucLumTrans)
					pRGBData[iP].c[3] = ucLum - ucLumTrans;
				else
					pRGBData[iP].c[3] = ucLumTrans - ucLum;
			}
		}
	}

	if (m_bInvert)
	{
		iluNegative();
	}

	// New image, so delete a possibly exitant texture
	DeleteTexture();

	return true;
}


/////////////////////////////////////////////////////////////////////
/// Save Bitmap

bool COGLBitmap::SaveBitmap(const char *pcFilename)
{
	char *pcName;
	cStr csFilename, csBaseName, csExt;
	int iExtPos;
	bool bEPSFile = false;
	bool bJPS = false;

	if (!pcFilename)
		pcName = m_csFilename.Str();
	else
		pcName = (char *)pcFilename;

	ilBindImage(m_uImgID);
	
	csBaseName = csFilename = pcName;

	iExtPos = ('.' < csBaseName);
	csExt = csBaseName.Last(csFilename.Len() - iExtPos, 1);
	csBaseName |= -1;

	if (csExt == "eps")
	{
		csFilename = csBaseName + ".tmp";
		bEPSFile = true;
	}
	else if (csExt == "jps")
	{
		csFilename = csBaseName + ".jpg";
		bEPSFile = true;
		bJPS = true;
	}

	remove(csFilename.Str());

	if (bEPSFile)
	{
		if (ilSave(IL_JPG, (ILstring) csFilename.Str()) == IL_FALSE)
			return false;
	}
	else
	{
		if (ilSaveImage((ILstring) csFilename.Str()) == IL_FALSE)
			return false;
	}

	if (bEPSFile)
	{
		csBaseName += ".eps";

		if (!ConvertJPEGtoPS(csFilename.Str(), csBaseName.Str()))
			return false;

		if (!bJPS)
			remove(csFilename.Str());
	}
	
	return true;
}


/////////////////////////////////////////////////////////////////////
/// Apply Text
// iMode alows different types of Apply calls.
// Arbitrary data may be passed by pvData.

bool COGLBitmap::Apply(int iMode, void *pvData)
{
	float pfCol[4];
	GLboolean bValid;
	
	if (m_iWidth == 0 || m_iHeight == 0)
		return true;

	glRasterPos3f(m_fX, m_fY, m_fZ);
	glGetBooleanv(GL_CURRENT_RASTER_POSITION_VALID, &bValid);

	// Check wether raster position can be seen
	if (!bValid)
		// If not, don't draw bitmap.
		return true;

	glPixelZoom(m_fScale, m_fScale);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	if (m_bScaleWithCurColor)
	{
		// This is very slow!!!
		glGetFloatv(GL_CURRENT_COLOR, pfCol);
		glPixelTransferf(GL_RED_SCALE, pfCol[0]);
		glPixelTransferf(GL_GREEN_SCALE, pfCol[1]);
		glPixelTransferf(GL_BLUE_SCALE, pfCol[2]);
	}

	// Use glBitmap only to adapt raster position.

	if (m_fVAlign != 0.0f || m_fHAlign != 0.0f)
	{
		float fYMove = -float(m_iHeight) * m_fVAlign * m_fScale;
		float fXMove = -float(m_iWidth) * m_fHAlign * m_fScale;
		glBitmap(0, 0, 0, 0, fXMove, fYMove, 0);
	}

	ilBindImage(m_uImgID);
	glDrawPixels(m_iWidth, m_iHeight, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid *) ilGetData());

	if (m_bScaleWithCurColor)
	{
		glPixelTransferf(GL_RED_SCALE, 1.0f);
		glPixelTransferf(GL_GREEN_SCALE, 1.0f);
		glPixelTransferf(GL_BLUE_SCALE, 1.0f);
	}

	glPixelZoom(1, 1);

	return true;
}

/////////////////////////////////////////////////////////////////////
/// Apply as Texture

bool COGLBitmap::ApplyAsTexture(bool bGenMipMaps)
{
	if (m_bIsTexture)
	{
		glBindTexture(GL_TEXTURE_2D, m_uTexID);
	}
	else
	{
		float fPriority = 1.0f;

		ilBindImage(m_uImgID);
		m_uTexID = ilutGLBindMipmaps();
/*
		if (bGenMipMaps)
			m_uTexID = ilutGLBindMipmaps();
		else
		{
			// Does not work for some reason....????
			glGenTextures(1, &m_uTexID);
			glBindTexture(GL_TEXTURE_2D, m_uTexID);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			if (!ilutGLTexImage(0))
			{
				glDeleteTextures(1, &m_uTexID);
				m_bIsTexture = false;
				m_uTexID = 0;
				return false;
			}
		}
*/
		if (m_uTexID == 0)
		{
			m_bIsTexture = false;
			return false;
		}
		glPrioritizeTextures(1, &m_uTexID, &fPriority);
		m_bIsTexture = true;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////
/// If image is also texture, then delete the texture

bool COGLBitmap::DeleteTexture()
{
	if (m_bIsTexture)
	{
		glDeleteTextures(1, &m_uTexID);
		m_bIsTexture = false;
		m_uTexID = 0;
	}

	return true;
}
