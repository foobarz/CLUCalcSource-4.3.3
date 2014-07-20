/// LICENSE START
////////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the CLUParse Library.
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

// CLUCodeBase.cpp: Implementierung der Klasse CCLUCodeBase.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "GL/glut.h"

#ifdef _GNUCPP3_
#   include <unistd.h>
#   define _getcwd getcwd
#   define _chdir chdir
#else
#   include <direct.h>
#endif

#include "CLUCodeBase.h"


typedef unsigned char uchar;


//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
/// Convert Image to Matrix

bool CCLUCodeBase::BMP2MatrixFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	int iChannel = 0;
	
	if (iVarCount < 1 || iVarCount > 2)
	{
		int piPar[] = {1, 2};

		m_ErrorList.WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (iVarCount == 2)
	{
		if (!mVars(1).CastToCounter(iChannel))
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		if (iChannel < 1 || iChannel > 4)
		{
			m_ErrorList.InvalidParVal(mVars(1), 2, iLine, iPos);
			return false;
		}
	}

	if (mVars(0).BaseType() != PDT_IMAGE)
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}								

	TImage &rImg = *mVars(0).GetImagePtr();
	if (!rImg.IsValid())
	{
		m_ErrorList.GeneralError("Image is not valid.", iLine, iPos);
		return false;
	}

	COGLBitmap &oglBitmap = *((COGLBitmap *) rImg);

	int iWidth, iHeight, iImgPos, iMatPos, iSize, iLinePos;
	uchar *pImgData = oglBitmap.Data();
	oglBitmap.GetSize(iWidth, iHeight);

	rVar.New(PDT_MATRIX, "Constant");

	TMatrix& xM = *rVar.GetMatrixPtr();
	xM.Resize(iHeight, iWidth);
	TCVScalar *pMatData = xM.Data();

	iSize = iWidth * iHeight;

	if (iChannel > 0)
	{
		for(iMatPos = 0, iImgPos = 4*(iSize - iWidth) + iChannel - 1, iLinePos = 0;
			iMatPos < iSize;
			++iMatPos, iImgPos += 4, ++iLinePos)
		{
			if (iLinePos >= iWidth)
			{
				iLinePos = 0;
				iImgPos -= 8*iWidth;
			}

			pMatData[iMatPos] = TCVScalar(pImgData[iImgPos]) / TCVScalar(255.0);
		}
	}
	else
	{
		// if no channel given, then average color
		for(iMatPos = 0, iImgPos = 4*(iSize - iWidth), iLinePos = 0;
			iMatPos < iSize;
			++iMatPos, iImgPos += 4, ++iLinePos)
		{
			if (iLinePos >= iWidth)
			{
				iLinePos = 0;
				iImgPos -= 8*iWidth;
			}

			pMatData[iMatPos] = ( TCVScalar(pImgData[iImgPos]) 
								+ TCVScalar(pImgData[iImgPos+1])
								+ TCVScalar(pImgData[iImgPos+2])) / TCVScalar(3.0 * 255.0);
		}

	}

	return true;
} 


//////////////////////////////////////////////////////////////////////
/// Matrix to BMP

bool CCLUCodeBase::Matrix2BMPFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	int iChannel = 0;
	int iWidth, iHeight, iImgPos, iMatPos, iSize, iLinePos;
	int iMatCount = 0, piChannel[4];
	TMatrix *pM[4];
	TCVScalar *pMatData[4];
	TCVScalar dVal;
	uchar ucVal;
	
	if (iVarCount < 1 || iVarCount > 2)
	{
		int piPar[] = {1, 2};

		m_ErrorList.WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (iVarCount == 1)
	{
		if (mVars(0).BaseType() != PDT_MATRIX)
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}
		
		pM[0] = mVars(0).GetMatrixPtr();
		pMatData[0] = pM[0]->Data();
		iMatCount = 1;
	
		iHeight = pM[0]->Rows();
		iWidth = pM[0]->Cols();
	}
	if (iVarCount == 2)
	{
		if (mVars(0).BaseType() == PDT_VARLIST && mVars(1).BaseType() == PDT_VARLIST)
		{
			int iMat;
			TVarList& rMatList = *mVars(0).GetVarListPtr();
			TVarList& rChList = *mVars(1).GetVarListPtr();
			iMatCount = rMatList.Count();

			if (iMatCount != rChList.Count())
			{
				m_ErrorList.GeneralError("Both lists need to be of same length.", iLine, iPos);
				return false;
			}

			for (iMat = 0; iMat < iMatCount; iMat++)
			{
				if (rMatList(iMat).BaseType() != PDT_MATRIX)
				{
					cStr csVal;

					csVal << "Element at position " << (iMat+1) << "in first list is not a matrix.";
					m_ErrorList.GeneralError(csVal.Str(), iLine, iPos);
					return false;
				}

				if (!rChList[iMat].CastToCounter(piChannel[iMat]))
				{
					cStr csVal;

					csVal << "Element at position " << (iMat+1) << "in second list is not a counter.";
					m_ErrorList.GeneralError(csVal.Str(), iLine, iPos);
					return false;
				}

				if (piChannel[iMat] < 1 || piChannel[iMat] > 4)
				{
					cStr csVal;

					csVal << "Element at position " << (iMat+1) 
						  << "in second list is not in the range of 1 to 4.";
					m_ErrorList.GeneralError(csVal.Str(), iLine, iPos);
					return false;
				}
	
				pM[iMat] = rMatList(iMat).GetMatrixPtr();
				if (iMat == 0)
				{
					iHeight = pM[0]->Rows();
					iWidth = pM[0]->Cols();
				}
				else
				{
					if (iHeight != pM[iMat]->Rows() ||
						iWidth != pM[iMat]->Cols())
					{
						cStr csVal;

						csVal << "Matrix at position " << (iMat+1) 
							  << "in first list is not of the same dimensions as first matrix in list.";
						m_ErrorList.GeneralError(csVal.Str(), iLine, iPos);
						return false;
					}
				}
				pMatData[iMat] = pM[iMat]->Data();
			}
		}
		else
		{
			if (!mVars(1).CastToCounter(iChannel))
			{
				m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
				return false;
			}

			if (iChannel < 1 || iChannel > 4)
			{
				m_ErrorList.InvalidParVal(mVars(1), 2, iLine, iPos);
				return false;
			}

			if (mVars(0).BaseType() != PDT_MATRIX)
			{
				m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
				return false;
			}

			pM[0] = mVars(0).GetMatrixPtr();
			pMatData[0] = pM[0]->Data();
			iMatCount = 1;

			iHeight = pM[0]->Rows();
			iWidth = pM[0]->Cols();
		}
	
	}


	rVar.New(PDT_IMAGE, "Constant");
	TImage &rImg = *rVar.GetImagePtr();
	if (!rImg.IsValid())
	{
		m_ErrorList.GeneralError("Cannot create image.", iLine, iPos);
		return false;
	}
	COGLBitmap &oglBitmap = *((COGLBitmap *) rImg);

	if (!oglBitmap.SetSize(iWidth, iHeight))
	{
		m_ErrorList.GeneralError("Cannot create image.", iLine, iPos);
		return false;
	}

	uchar *pImgData = oglBitmap.Data();

	iSize = iWidth * iHeight;
	// Set Image Data to zero everywhere
	memset(pImgData, 0, iSize*4);

	if (iChannel > 0)
	{
		for(iMatPos = 0, iImgPos = 4*(iSize - iWidth) + iChannel - 1, iLinePos = 0;
			iMatPos < iSize;
			++iMatPos, iImgPos += 4, ++iLinePos)
		{
			if (iLinePos >= iWidth)
			{
				iLinePos = 0;
				iImgPos -= 8*iWidth;
			}

			dVal = pMatData[0][iMatPos] * 255.0;
			if (dVal < 0.0)
				pImgData[iImgPos] = uchar(0);
			else if (dVal > 255.0)
				pImgData[iImgPos] = uchar(255);
			else
				pImgData[iImgPos] = uchar(floor(dVal + 0.5));

			if (iChannel < 4)
				pImgData[iImgPos-iChannel+4] = uchar(255);
		}
	}
	else if (iMatCount > 1)
	{
		for(iMatPos = 0, iImgPos = 4*(iSize - iWidth), iLinePos = 0;
			iMatPos < iSize;
			++iMatPos, iImgPos += 4, ++iLinePos)
		{
			if (iLinePos >= iWidth)
			{
				iLinePos = 0;
				iImgPos -= 8*iWidth;
			}

			// Set alpha channel just in case
			pImgData[iImgPos+3] = uchar(255);

			for (int iMat = 0; iMat < iMatCount; iMat++)
			{
				dVal = pMatData[iMat][iMatPos] * 255.0;
				if (dVal < 0.0)
					pImgData[iImgPos + piChannel[iMat] - 1] = uchar(0);
				else if (dVal > 255.0)
					pImgData[iImgPos + piChannel[iMat] - 1] = uchar(255);
				else
					pImgData[iImgPos + piChannel[iMat] - 1] = uchar(floor(dVal + 0.5));
			}
		}
	}
	else
	{
		// if no channel given, then average color
		TCVScalar dVal;

		for(iMatPos = 0, iImgPos = 4*(iSize - iWidth), iLinePos = 0;
			iMatPos < iSize;
			++iMatPos, iImgPos += 4, ++iLinePos)
		{
			if (iLinePos >= iWidth)
			{
				iLinePos = 0;
				iImgPos -= 8*iWidth;
			}

			dVal = pMatData[0][iMatPos] * 255.0;
			if (dVal < 0.0)
				ucVal = uchar(0);
			else if (dVal >= 255.0)
				ucVal = uchar(255);
			else
				ucVal = uchar(floor(dVal + 0.5));

			pImgData[iImgPos] = pImgData[iImgPos+1] = pImgData[iImgPos+2] = ucVal;
			pImgData[iImgPos+3] = uchar(255); // Opaque Image
		}
	}
	
	return true;
} 



//////////////////////////////////////////////////////////////////////
/// Set Horizontal and Vertical Align Function for Images

bool CCLUCodeBase::SetBMPAlignFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	TCVScalar dHVal, dVVal;
	
	if (iVarCount == 2)
	{
		if (!mVars(0).CastToScalar(dHVal, m_fSensitivity))
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}								

		if (!mVars(1).CastToScalar(dVVal, m_fSensitivity))
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}								

		m_fHBMPAlign = float(dHVal);
		m_fVBMPAlign = float(dVVal);
	}
	else
	{
		m_ErrorList.WrongNoOfParams(2, iLine, iPos);
		return false;
	}
	
	return true;
} 


//////////////////////////////////////////////////////////////////////
/// Set Origin Position for Images

bool CCLUCodeBase::SetBMPPosFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int i, iVarCount = mVars.Count();
	TCVScalar pdVal[3];
	
	if (iVarCount == 3)
	{
		for (i = 0; i < 3; i++)
		{
			if (!mVars(i).CastToScalar(pdVal[i], m_fSensitivity))
			{
				m_ErrorList.InvalidParType(mVars(i), i+1, iLine, iPos);
				return false;
			}								
		}

		m_xBMPPos.Set(float(pdVal[0]), float(pdVal[1]), float(pdVal[2]));
	}
	else if (iVarCount == 1)
	{
		if (mVars(0).BaseType() != PDT_MULTIV)
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}								

		TMultiV &vA = *mVars(0).GetMultiVPtr();
		TMultiV vB;

		CastMVtoE3(vA, vB);
		m_xBMPPos.Set(float(vB[m_E3GABase.iE1]), 
					  float(vB[m_E3GABase.iE2]),
					  float(vB[m_E3GABase.iE3]));
	}
	else
	{
		int piPar[] = {1, 3};

		m_ErrorList.WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}
	
	return true;
} 


//////////////////////////////////////////////////////////////////////
/// Resize Image Function

bool CCLUCodeBase::ResizeBMPFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int i, iVarCount = mVars.Count();
	TCVScalar pdVal[2];
	
	if (iVarCount == 3)
	{
		if (mVars(0).BaseType() != PDT_IMAGE)
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		for (i = 1; i < 3; i++)
		{
			if (!mVars(i).CastToScalar(pdVal[i-1], m_fSensitivity))
			{
				m_ErrorList.InvalidParType(mVars(i), i+1, iLine, iPos);
				return false;
			}

			if (pdVal[i-1] <= 0)
			{
				char pcText[100];
				sprintf(pcText, "New %s of image has to be greater than zero.", 
					((i-1) == 0 ? "width" : "height") );
				m_ErrorList.GeneralError(pcText, iLine, iPos);
				return false;
			}
		}
	}
	else
	{
		int piPar[] = {3};

		m_ErrorList.WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}
	
	TImage &rImg = *(mVars(0).GetImagePtr());
	if (!rImg->ResizeImage(int(pdVal[0]), int(pdVal[1])) )
	{
		m_ErrorList.GeneralError("Error resizing image.", iLine, iPos);
		return false;
	}

	return true;
} 



//////////////////////////////////////////////////////////////////////
/// Resize Canvas Function

bool CCLUCodeBase::ReshapeBMPFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int i, iVarCount = mVars.Count();
	TCVScalar pdVal[2];
	COGLColor colClear;

	colClear.Set(0, 0, 0);
	
	if (iVarCount == 3 || iVarCount == 4)
	{
		if (mVars(0).BaseType() != PDT_IMAGE)
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		for (i = 1; i < 3; i++)
		{
			if (!mVars(i).CastToScalar(pdVal[i-1], m_fSensitivity))
			{
				m_ErrorList.InvalidParType(mVars(i), i+1, iLine, iPos);
				return false;
			}

			if (pdVal[i-1] <= 0)
			{
				char pcText[100];
				sprintf(pcText, "New %s of image has to be greater than zero.", 
					((i-1) == 0 ? "width" : "height") );
				m_ErrorList.GeneralError(pcText, iLine, iPos);
				return false;
			}
		}

		if (iVarCount > 3)
		{
			if (mVars(3).BaseType() != PDT_COLOR)
			{
				m_ErrorList.InvalidParType(mVars(3), 4, iLine, iPos);
				return false;
			}

			colClear = *mVars(3).GetOGLColorPtr();
		}
	}
	else
	{
		int piPar[] = {3, 4};

		m_ErrorList.WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}
	
	TImage &rImg = *(mVars(0).GetImagePtr());
	if (!rImg->ResizeCanvas(int(pdVal[0]), int(pdVal[1]), &colClear))
	{
		m_ErrorList.GeneralError("Error resizing image.", iLine, iPos);
		return false;
	}

	return true;
} 

//////////////////////////////////////////////////////////////////////
/// Rotate Image Function
///
/// Parameters:
///		1. Angle of rotation in radians
///

bool CCLUCodeBase::RotateBMPFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int i, iVarCount = mVars.Count();
	TCVScalar pdVal[2];
	
	if (iVarCount == 2)
	{
		if (mVars(0).BaseType() != PDT_IMAGE)
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (!mVars(1).CastToScalar(pdVal[0], m_fSensitivity))
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}
	}
	else
	{
		int piPar[] = {2};

		m_ErrorList.WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}
	
	TImage &rImg = *mVars(0).GetImagePtr();
	if (!rImg.IsValid())
	{
		m_ErrorList.GeneralError("Invalid image.", iLine, iPos);
		return false;
	}

	if ( !rImg->RotateImage(float(pdVal[0]) / m_fRadPerDeg) )
	{
		m_ErrorList.GeneralError("Error rotating image.", iLine, iPos);
		return false;
	}

	return true;
} 


//////////////////////////////////////////////////////////////////////
/// Copy Image Function
///
/// Parameters:
///		1. Image to be copied
///

bool CCLUCodeBase::CopyBMPFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int i, iVarCount = mVars.Count();
	TCVScalar pdVal[2];
	
	if (iVarCount == 1)
	{
		if (mVars(0).BaseType() != PDT_IMAGE)
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}
	}
	else
	{
		int piPar[] = {1};

		m_ErrorList.WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}
	
	TImage &rImg = *mVars(0).GetImagePtr();
	if (!rImg.IsValid())
	{
		m_ErrorList.GeneralError("Invalid image.", iLine, iPos);
		return false;
	}

	rVar.New(PDT_IMAGE, "Constant");
	TImage &rNewImg = *rVar.GetImagePtr();

	if (!rNewImg.IsValid())
	{
		m_ErrorList.GeneralError("Could not create image.", iLine, iPos);
		return false;
	}

	rNewImg->SetFilename("Memory");
	rNewImg->operator =(*((COGLBitmap *) rImg));

	return true;
} 



//////////////////////////////////////////////////////////////////////
/// Insert Image Function
///
/// Parameters:
///		1. Target Image
///		2. Source Image
///		3. X-Pos. in Target
///		4. Y-Pos. in Target

bool CCLUCodeBase::InsertBMPFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int i, iVarCount = mVars.Count();
	TCVCounter piVal[2], piCh[2] = {-1, -1};
	
	if (iVarCount == 4 || iVarCount == 6)
	{
		if (mVars(0).BaseType() != PDT_IMAGE)
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (mVars(1).BaseType() != PDT_IMAGE)
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		for (i = 2; i < 4; i++)
		{
			if (!mVars(i).CastToCounter(piVal[i-2]))
			{
				m_ErrorList.InvalidParType(mVars(i), i+1, iLine, iPos);
				return false;
			}

			if (piVal[i-2] < 0)
			{
				if (i == 2)
					m_ErrorList.GeneralError("X-position has to be >= 0.", iLine, iPos);
				else
					m_ErrorList.GeneralError("Y-position has to be >= 0.", iLine, iPos);

				return false;
			}
		}
	
		if (iVarCount == 6)
		{
			for (i = 4; i < 6; i++)
			{
				if (!mVars(i).CastToCounter(piCh[i-4]))
				{
					m_ErrorList.InvalidParType(mVars(i), i+1, iLine, iPos);
					return false;
				}

				if (piCh[i-4] < 0 ||piCh[i-4] > 4)
				{
					if (i == 4)
						m_ErrorList.GeneralError("Target channel has to be in range 0 to 4.", iLine, iPos);
					else
						m_ErrorList.GeneralError("Source channel has to be in range 0 to 4.", iLine, iPos);

					return false;
				}

				piCh[i-4]--;
			}
		}
	}
	else
	{
		int piPar[] = {4, 6};

		m_ErrorList.WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}
	
	TImage &rTrgImg = *(mVars(0).GetImagePtr());
	TImage &rSrcImg = *(mVars(1).GetImagePtr());

	if ( ((COGLBitmap *) rSrcImg) == ((COGLBitmap *) rTrgImg) )
	{
		m_ErrorList.GeneralError("Cannot insert an image into itself.", iLine, iPos);
		return false;
	}

	if (!rTrgImg->InsertImage(*((COGLBitmap *) rSrcImg), piVal[0], piVal[1], piCh[0], piCh[1]))
	{
		m_ErrorList.GeneralError("Error inserting image.", iLine, iPos);
		return false;
	}

	return true;
} 



//////////////////////////////////////////////////////////////////////
/// Draw Bitmap Function

bool CCLUCodeBase::DrawBMPFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	int iVar;
	cStr csFilename;
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	TCVScalar dVal[3];
	COGLVertex xA;
	
	if (iVarCount == 4)
	{
		for (iVar = 0; iVar < 3; iVar++)
		{
			if (!mVars(iVar).CastToScalar(dVal[iVar], m_fSensitivity))
			{
				m_ErrorList.InvalidParType(mVars(iVar), 1, iLine, iPos);
				return false;
			}								
		}

		xA.Set(float(dVal[0]), float(dVal[1]), float(dVal[2]));

		CCodeVar &rVar = mVars(3);
		if (rVar.BaseType() == PDT_NOTYPE)
		{
			m_ErrorList.InvalidType(rVar, iLine, iPos);
			return false;
		}
	
		csFilename = rVar.ValStr();
		
	} 
	// If a single multivector is given as position vector, cast it to E3
	// and use that position.
	else if (iVarCount == 2)
	{
		if (mVars(0).BaseType() == PDT_MULTIV)
		{
			TMultiV &vA = *mVars(0).GetMultiVPtr();
			TMultiV vB;
			if (!CastMVtoE3(vA, vB))
			{
				m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
				return false;
			}								

			xA.Set((float) vB[E3GA<TCVScalar>::iE1], 
				(float) vB[E3GA<TCVScalar>::iE2], 
				(float) vB[E3GA<TCVScalar>::iE3]);

			CCodeVar &rVar = mVars(1);

			if (rVar.BaseType() == PDT_NOTYPE)
			{
				m_ErrorList.InvalidType(rVar, iLine, iPos);
				return false;
			}

			csFilename << rVar.ValStr();
		}
		else
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}								

	}
	else
	{
		int piPar[] = {2, 4};

		m_ErrorList.WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}


	string sFilename = csFilename.Str();
	map<string, COGLBitmap>::iterator it_BMP;

	// Insert Path to currently loaded script before filename.
	sFilename = m_sScriptPath + sFilename;
	if ((it_BMP = m_mapBitmap.find(sFilename)) == m_mapBitmap.end())
	{
		m_mapBitmap[sFilename];
		it_BMP = m_mapBitmap.find(sFilename);
	}

	COGLBitmap &oglBitmap = it_BMP->second;

	oglBitmap.SetPosition(xA);
	oglBitmap.SetVAlign(m_fVBMPAlign);
	oglBitmap.SetHAlign(m_fHBMPAlign);
	oglBitmap.SetScale(m_fCurBitmapScale);
	oglBitmap.EnableTransparency(m_bUseBMPTrans);
	oglBitmap.SetTransparentColor(m_oglBMPTransCol);
	oglBitmap.SetTransparencyMode(COGLBitmap::Exact);
	oglBitmap.SetFilename(sFilename.c_str());
	if (!oglBitmap.LoadBitmap())
	{
		m_ErrorList.AddMsg("Error: Cannot load bitmap.", iLine, iPos, CERR_INTERNAL, CEL_ERROR);
		return false;
	}

	oglBitmap.Apply();

	float fL, fR, fB, fT, fZ1, fZ2;
	if (oglBitmap.GetWorldCoordDims(fL, fR, fB, fT, fZ1, fZ2))
	{
		TVarList mRet;
		mRet.Set(6);
		mRet[0] = fL;
		mRet[1] = fB;
		mRet[2] = fZ1;
		mRet[3] = fR;
		mRet[4] = fT;
		mRet[5] = fZ2;

		rVar = mRet;
	}

	return true;
} 


//////////////////////////////////////////////////////////////////////
/// Set Bitmap as Texture Function

bool CCLUCodeBase::SetBMPTextureFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	cStr csFilename;
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	COGLVertex xA;
	TCVCounter iGenMipMaps = 1;
	
	if (iVarCount > 1)
	{
		int piPar[] = {0, 1};

		m_ErrorList.WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (iVarCount == 0)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		return true;
	}

	if (iVarCount == 1)
	{
		CCodeVar &rVar = mVars(0);
		if (rVar.BaseType() == PDT_IMAGE)
		{
			TImage& oglBitmap = *mVars(0).GetImagePtr();
			if (oglBitmap.IsValid())
			{
				MemObj<TImage>& mTempImageList = GetActTempImageList();
				mTempImageList.Add(1);
				mTempImageList.Last() = oglBitmap;
				//if (iGenMipMaps)
				mTempImageList.Last()->ApplyAsTexture(true);
				//else
				//oglBitmap->ApplyAsTexture(false);
			}
			else
			{
				m_ErrorList.GeneralError("Invalid Image.", iLine, iPos);
				return false;
			}
		}
		else
		{
			m_ErrorList.InvalidParType(rVar, 1, iLine, iPos);
			return false;
		}
	}
/*
	else if (iVarCount == 2)
	{
		if (mVars(0).BaseType() == PDT_STRING &&
			mVars(1).BaseType() == PDT_STRING)
		{
			// Render Latex and use generated Bitmap as texture
			TString csText, csFile;
			float fMagStep;
			COGLVertex xA;

			csText = mVars(0).ValStr();
			csFile = mVars(1).ValStr();
			xA.Set(0,0,0);

			if (csText.Len() == 0)
			{
				m_ErrorList.GeneralError("Empty string cannot be rendered.", iLine, iPos);
				return false;
			}

			if (csFile.Len() == 0)
			{
				m_ErrorList.GeneralError("Name for LaTeX text has to be given.", iLine, iPos);
				return false;
			}

			fMagStep = m_oglLatex.GetMagStepScale(m_iLatexMagStep);
			if (!m_oglLatex.Add(csText, xA, fMagStep, m_fVLatexAlign, m_fHLatexAlign,
								csFile.Str() ))
			{
				string sError;
				sError = "Error: Cannot render Latex text.\n\nLatex Errors:\n";
				sError += m_oglLatex.GetRenderErrorText();
				sError += "\n";
				m_ErrorList.AddMsg(sError.c_str(), iLine, iPos, CERR_INTERNAL, CEL_ERROR);
				return false;
			}

			COGLBitmap *pImage = m_oglLatex.GetBitmap(csText.Str());
			if (!pImage)
			{
				m_ErrorList.GeneralError("Internal Error: Latex bitmap not available.", iLine, iPos);
				return false;
			}

			pImage->ApplyAsTexture(true);
		}
	}
*/

	return true;
} 


//////////////////////////////////////////////////////////////////////
/// Set Bitmap Texture Repeat Function

bool CCLUCodeBase::SetBMPTextureRepeatFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	cStr csFilename;
	TVarList& mVars = *rPars.GetVarListPtr();
	
	int iVarCount = mVars.Count();
	TCVScalar dVal;

	if (iVarCount == 1)
	{
		CCodeVar &rVar = mVars(0);
		if (!rVar.CastToScalar(dVal))
		{
			m_ErrorList.InvalidParType(rVar, 1, iLine, iPos);
			return false;
		}
	}
	else
	{
		int piPar[] = {1};

		m_ErrorList.WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (m_pFilter)
	{
		m_pFilter->SetTextureRepeatFactor(float(dVal));
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Enable Absolute Texture Coordinates

bool CCLUCodeBase::EnableBMPAbsTexCoordsFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	cStr csFilename;
	TVarList& mVars = *rPars.GetVarListPtr();
	
	int iVarCount = mVars.Count();
	TCVCounter iVal;

	if (iVarCount == 1)
	{
		CCodeVar &rVar = mVars(0);
		if (!rVar.CastToCounter(iVal))
		{
			m_ErrorList.InvalidParType(rVar, 1, iLine, iPos);
			return false;
		}
	}
	else
	{
		int piPar[] = {1};

		m_ErrorList.WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (m_pFilter)
	{
		m_pFilter->EnableAbsTexCoords((iVal == 0 ? false : true));
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Load Bitmap Function

bool CCLUCodeBase::LoadBMPFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	char pcCurPath[500];
	cStr csFilename;
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	COGLVertex xA;
	
	if (iVarCount == 1)
	{
		CCodeVar &rVar = mVars(0);
		if (rVar.BaseType() != PDT_STRING)
		{
			m_ErrorList.InvalidParType(rVar, 1, iLine, iPos);
			return false;
		}
	
		csFilename = rVar.ValStr();
		
	} 
	else
	{
		int piPar[] = {1};

		m_ErrorList.WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}


	string sFilename = csFilename.Str();

/*
	if (csFilename[1] != ':')
	{
		// Insert Path to currently loaded script before filename.
		sFilename = m_sScriptPath + sFilename;
	}
*/

	rVar.New(PDT_IMAGE, "Constant");

	TImage& oglBitmap = *rVar.GetImagePtr();
	if (oglBitmap.IsValid())
	{
		oglBitmap->SetPosition(0, 0, 0);
		oglBitmap->SetVAlign(m_fVBMPAlign);
		oglBitmap->SetHAlign(m_fHBMPAlign);
		oglBitmap->SetScale(1.0f);
		oglBitmap->EnableTransparency(m_bUseBMPTrans);
		oglBitmap->SetTransparentColor(m_oglBMPTransCol);
		oglBitmap->SetTransparencyMode(COGLBitmap::Exact);
		oglBitmap->SetFilename(sFilename.c_str());

		_getcwd(pcCurPath, 499);
		_chdir(m_sScriptPath.c_str());

		if (!oglBitmap->LoadBitmap())
		{
			m_ErrorList.AddMsg("Error: Cannot load bitmap.", iLine, iPos, CERR_INTERNAL, CEL_ERROR);
			_chdir(pcCurPath);
			return false;
		}

		_chdir(pcCurPath);
	}
	else
	{
		m_ErrorList.AddMsg("Error: Cannot create image variable.", iLine, iPos, CERR_INTERNAL, CEL_FATAL);
		return false;
	}

	return true;
} 


//////////////////////////////////////////////////////////////////////
/// New Bitmap Function

bool CCLUCodeBase::NewBMPFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	cStr csFilename;
	TVarList& mVars = *rPars.GetVarListPtr();
	TCVCounter iCX, iCY;

	int iVarCount = mVars.Count();
	COGLVertex xA;
	COGLColor colClear;

	colClear.Set(0,0,0, 1);
	
	if (iVarCount < 2 || iVarCount > 3)
	{
		int piPar[] = {2, 3};

		m_ErrorList.WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToCounter(iCX))
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}
	
	if (!mVars(1).CastToCounter(iCY))
	{
		m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}
	
	if (iCX <= 0)
	{
		m_ErrorList.GeneralError("Width of image has to be greater than zero.", iLine, iPos);
		return false;
	}

	if (iCY <= 0)
	{
		m_ErrorList.GeneralError("Height of image has to be greater than zero.", iLine, iPos);
		return false;
	}

	if (iVarCount > 2)
	{
		if (mVars(2).BaseType() != PDT_COLOR)
		{
			m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
			return false;			
		}

		colClear = *mVars(2).GetOGLColorPtr();
	}

	rVar.New(PDT_IMAGE, "Constant");

	TImage& oglBitmap = *rVar.GetImagePtr();
	if (oglBitmap.IsValid())
	{
		oglBitmap->SetPosition(0, 0, 0);
		oglBitmap->SetVAlign(m_fVBMPAlign);
		oglBitmap->SetHAlign(m_fHBMPAlign);
		oglBitmap->SetScale(1.0f);
		oglBitmap->EnableTransparency(m_bUseBMPTrans);
		oglBitmap->SetTransparentColor(m_oglBMPTransCol);
		oglBitmap->SetTransparencyMode(COGLBitmap::Exact);
		oglBitmap->SetFilename("Untitled.png");
		oglBitmap->SetSize(iCX, iCY, &colClear);
	}
	else
	{
		m_ErrorList.GeneralError("Cannot create image variable.", iLine, iPos);
		return false;
	}

	return true;
} 




//////////////////////////////////////////////////////////////////////
/// Save Bitmap Function

bool CCLUCodeBase::SaveBMPFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	char pcCurPath[500];
	cStr csFilename;
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	COGLVertex xA;
	
	if (iVarCount != 2)
	{
		int piPar[] = {2};

		m_ErrorList.WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_IMAGE)
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}

	TImage& Img = *mVars(1).GetImagePtr();
	csFilename = mVars(0).ValStr();

	string sFilename = csFilename.Str();

/*
	if (csFilename[1] != ':')
	{
		// Insert Path to currently loaded script before filename.
		sFilename = m_sScriptPath + sFilename;
	}
*/

	if (!Img.IsValid())
	{
		m_ErrorList.GeneralError("Invalid Image.", iLine, iPos);
		return false;
	}

	_getcwd(pcCurPath, 499);
	_chdir(m_sScriptPath.c_str());

	if (!Img->SaveBitmap(sFilename.c_str()))
	{
		m_ErrorList.GeneralError("Error saving image.", iLine, iPos);
		_chdir(pcCurPath);
		return false;
	}

	_chdir(pcCurPath);

	return true;
} 







//////////////////////////////////////////////////////////////////////
/// Enable BMP Transparency Function

bool CCLUCodeBase::EnableBMPTransparencyFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	TCVCounter iVal;
	
	if (iVarCount == 1)
	{
		if (!mVars(0).CastToCounter(iVal))
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}								

		if (iVal)
			m_bUseBMPTrans = true;
		else
			m_bUseBMPTrans = false;
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
} 





//////////////////////////////////////////////////////////////////////
/// Set Bitmap Transparent Color Function

bool CCLUCodeBase::SetBMPTransparentColorFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	int iVar;
	cStr csFilename;
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	TCVScalar dVal[3];
	
	if (iVarCount == 3)
	{
		for (iVar = 0; iVar < 3; iVar++)
		{
			if (!mVars(iVar).CastToScalar(dVal[iVar], m_fSensitivity))
			{
				m_ErrorList.InvalidParType(mVars(iVar), 1, iLine, iPos);
				return false;
			}								
		}

		m_oglBMPTransCol.Set(float(dVal[0]), float(dVal[1]), float(dVal[2]));
	} 
	// If a single multivector is given as position vector, cast it to E3
	// and use that position.
	else if (iVarCount == 1)
	{
		if (mVars(0).BaseType() == PDT_COLOR)
		{
			m_oglBMPTransCol = *mVars(0).GetOGLColorPtr();
		}
		else
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}								

	}
	else
	{
		int piPar[] = {1, 3};

		m_ErrorList.WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}
	
	return true;
} 



//////////////////////////////////////////////////////////////////////
/// Set Bitmap Scale Function

bool CCLUCodeBase::SetBMPScaleFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	TCVScalar dVal;
	
	if (iVarCount == 1)
	{
		if (!mVars(0).CastToScalar(dVal, m_fSensitivity))
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}								

		if (dVal <= TCVScalar(0))
		{
			m_ErrorList.InvalidParVal(mVars(0), 1, iLine, iPos);
			return false;
		}

		m_fCurBitmapScale = float(dVal);
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
} 


//////////////////////////////////////////////////////////////////////
/// Get Bitmap Coordinates Function

bool CCLUCodeBase::GetBMPCoordsFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	cStr csFilename;
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	
	if (iVarCount != 1)
	{
		int piPar[] = {1};

		m_ErrorList.WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_IMAGE)
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}								

	TImage &rImg = *mVars(0).GetImagePtr();
	if (!rImg.IsValid())
	{
		m_ErrorList.GeneralError("Image is not valid.", iLine, iPos);
		return false;
	}

	COGLBitmap &oglBitmap = *((COGLBitmap *) rImg);

	oglBitmap.SetPosition(m_xBMPPos);
	oglBitmap.SetVAlign(m_fVBMPAlign);
	oglBitmap.SetHAlign(m_fHBMPAlign);
	oglBitmap.SetScale(m_fCurBitmapScale);

	float fL, fR, fB, fT, fZ1, fZ2;
	if (oglBitmap.GetWorldCoordDims(fL, fR, fB, fT, fZ1, fZ2))
	{
		TVarList mRet;
		mRet.Set(6);
		mRet[0] = fL;
		mRet[1] = fB;
		mRet[2] = fZ1;
		mRet[3] = fR;
		mRet[4] = fT;
		mRet[5] = fZ2;

		rVar = mRet;
	}

	return true;
} 




/* Old version

bool CCLUCodeBase::GetBMPCoordsFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	int iVar;
	cStr csFilename;
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	TCVScalar dVal[3];
	COGLVertex xA;
	
	if (iVarCount == 4)
	{
		for (iVar = 0; iVar < 3; iVar++)
		{
			if (!mVars(iVar).CastToScalar(dVal[iVar], m_fSensitivity))
			{
				m_ErrorList.InvalidParType(mVars(iVar), 1, iLine, iPos);
				return false;
			}								
		}

		xA.Set(dVal[0], dVal[1], dVal[2]);

		CCodeVar &rVar = mVars(3);
		if (rVar.BaseType() == PDT_NOTYPE)
		{
			m_ErrorList.InvalidType(rVar, iLine, iPos);
			return false;
		}
	
		csFilename = rVar.ValStr();
		
	} 
	// If a single multivector is given as position vector, cast it to E3
	// and use that position.
	else if (iVarCount == 2)
	{
		if (mVars(0).BaseType() == PDT_MULTIV)
		{
			TMultiV &vA = *mVars(0).GetMultiVPtr();
			TMultiV vB;
			if (!CastMVtoE3(vA, vB))
			{
				m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
				return false;
			}								

			xA.Set((float) vB[E3GA<TCVScalar>::iE1], 
				(float) vB[E3GA<TCVScalar>::iE2], 
				(float) vB[E3GA<TCVScalar>::iE3]);

			CCodeVar &rVar = mVars(1);

			if (rVar.BaseType() == PDT_NOTYPE)
			{
				m_ErrorList.InvalidType(rVar, iLine, iPos);
				return false;
			}

			csFilename << rVar.ValStr();
		}
		else
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}								

	}
	else
	{
		int piPar[] = {2, 4};

		m_ErrorList.WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}


	string sFilename = csFilename.Str();
	map<string, COGLBitmap>::iterator it_BMP;

	// Insert Path to currently loaded script before filename.
	sFilename = m_sScriptPath + sFilename;
	if ((it_BMP = m_mapBitmap.find(sFilename)) == m_mapBitmap.end())
	{
		m_mapBitmap[sFilename];
		it_BMP = m_mapBitmap.find(sFilename);
	}

	COGLBitmap &oglBitmap = it_BMP->second;

	oglBitmap.SetPosition(xA);
	oglBitmap.SetVAlign(m_fVBMPAlign);
	oglBitmap.SetHAlign(m_fHBMPAlign);
	oglBitmap.SetScale(m_fCurBitmapScale);
	oglBitmap.SetFilename(csFilename.Str());
	if (!oglBitmap.LoadBitmap())
	{
		m_ErrorList.AddMsg("Error: Cannot load bitmap.", iLine, iPos, CERR_INTERNAL, CEL_ERROR);
		return false;
	}

	float fL, fR, fB, fT, fZ1, fZ2;
	if (oglBitmap.GetWorldCoordDims(fL, fR, fB, fT, fZ1, fZ2))
	{
		TVarList mRet;
		mRet.Set(6);
		mRet[0] = fL;
		mRet[1] = fB;
		mRet[2] = fZ1;
		mRet[3] = fR;
		mRet[4] = fT;
		mRet[5] = fZ2;

		rVar = mRet;
	}

	return true;
} 
*/





