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

#include "CLUCodeBase.h"


//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Enable Fog Function

bool CCLUCodeBase::EnableFogFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
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

	if (iVal)
		glEnable(GL_FOG);
	else
		glDisable(GL_FOG);

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Set Fog Color

bool CCLUCodeBase::SetFogColorFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	int iVar;
	TVarList& mVars = *rPars.GetVarListPtr();
	
	int iVarCount = mVars.Count();
	TCVScalar dVal[3];
	COGLColor Col;
	
	if (iVarCount == 3)
	{
		for (iVar = 0; iVar < 3; iVar++)
		{
			if (!mVars(iVar).CastToScalar(dVal[iVar], m_fSensitivity))
			{
				m_ErrorList.InvalidParType(mVars(iVar), iVar+1, iLine, iPos);
				return false;
			}								
		}

		Col.Set(float(dVal[0]), float(dVal[1]), float(dVal[2]));
	} 
	// If a single multivector is given as position vector, cast it to E3
	// and use that position.
	else if (iVarCount == 1)
	{
		if (mVars(0).BaseType() == PDT_COLOR)
		{
			Col = *mVars(0).GetOGLColorPtr();
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

	glFogfv(GL_FOG_COLOR, Col.Data());

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Fog Density

bool CCLUCodeBase::SetFogDensityFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
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
	} 
	else
	{
		int piPar[] = {1};

		m_ErrorList.WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (dVal < 0)
	{
		m_ErrorList.GeneralError("Invalid fog density value. Needs to be >= 0.", iLine, iPos);
		return false;
	}

	glFogf(GL_FOG_DENSITY, float(dVal));

	return true;
}



bool CCLUCodeBase::DrawMatrix(TMatrix &xA, int iLine, int iPos)
{
	int i, j, iGrade = 0;
	TCVScalar fMag, fPrec;
	CMVInfo<float> Info;
	TMatrix xU, xV, xD;//, xTD;
	
	if (!xA.SVD(xU, xD, xV))
	{
		m_ErrorList.Internal(iLine, iPos);
		return false;
	}
	
	fPrec = (TCVScalar) m_pFilter->GetDoubleSens();
	
	xU.Round(fPrec);
	xV.Round(fPrec);
	xD.Round(fPrec);
	//SVDOrder(xD, xU, xV, true);
	
	if (xD.Cols() == 3)
	{
		// xV is a 3x3 matrix
		
		//		xTD.Resize(3, 3);
		//		xTD = TCVScalar(0);
		fMag = TCVScalar(1);
		for(i=0;i<3;i++)
		{
			if (xD(1, i+1) != TCVScalar(0))
			{
				//				xTD(i+1, i+1) = xD(1, i+1);
				//				xTD(i+1, i+1) = TCVScalar(0);
				if (!Info.m_mVex.Add(1))
				{
					m_ErrorList.OutOfMemory(iLine, iPos);
					return false;
				}
				CPointND<float, 3>& rVex = Info.m_mVex.Last();
				for(j=0;j<3;j++)
					rVex[j] = (float) xV(j+1, i+1);
				
				fMag *= xD(1, i+1);
				iGrade++;
			}
		}
		
		//fMag = Mag(fMag);
		
		if (iGrade == 1)
		{
			Info.m_eType = GA_LINE;
			Info.m_mVex.Insert(0);
			Info.m_mVex[0][0] = 0;
			Info.m_mVex[0][1] = 0;
			Info.m_mVex[0][2] = 0;
			Info.m_mVal.Add(1);
			Info.m_mVal.Last() = 1;
			//Info.m_mVex[0] *= (float) fMag;
		}
		else if (iGrade == 2)
		{
			Info.m_eType = GA_PLANE;
			if (!Info.m_mVal.Set(1))
			{
				m_ErrorList.OutOfMemory(iLine, iPos);
				return false;
			}
			Info.m_mVal[0] = 1;
			
			Info.m_mVex.Insert(0);
			Info.m_mVex[0][0] = 0;
			Info.m_mVex[0][1] = 0;
			Info.m_mVex[0][2] = 0;
		}
		else if (iGrade == 3)
		{
			Info.m_eType = GA_SPACE;
			if (!Info.m_mVal.Set(1))
			{
				m_ErrorList.OutOfMemory(iLine, iPos);
				return false;
			}
			Info.m_mVal[0] = (float) 0.5;
		}
		
		m_pFilter->m_P3Filter.DrawMVInfo(Info);
	}
	else if (xD.Cols() == 4)
	{
		// xV is a 4x4 matrix
		TMultiV vA = m_P3GABase.vSC;
		TMultiV vX = m_P3GABase.vZero;
		cStr csTest;
		
		for(i=0;i<4;i++)
		{
			if (xD(1, i+1) != TCVScalar(0))
			{
				for(j=1;j<=4;j++)
					vX[j] = xV(j, i+1);
				
				vA = vA ^ vX ^ xD(1, i+1);
				csTest = vX.Str();
				csTest = vA.Str();
			}
		}
		
		m_pFilter->DrawMV(vA);
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
// Draw multivector with given offset and scale
// expects two or three parameters:
// 1: Multivector to be drawn
// 2: offset vector
// 3: scale (optional)

bool CCLUCodeBase::OffsetFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList &mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	CMVInfo<float> MVInfo;
	TMultiV vA, vB;
	TCVScalar dScale = 0;
	bool bBIsDirVec = false;
	
	if (iVarCount < 2 || iVarCount > 3)
	{
		int piIdx[] = { 2, 3 };
		m_ErrorList.WrongNoOfParams(piIdx, 2, iLine, iPos);
		return false;
	}
	
	
	if (mVars(0).BaseType() != PDT_MULTIV)
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}
	
	if (mVars(1).BaseType() != PDT_MULTIV)
	{
		m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}
	
	if (iVarCount == 3)
	{
		if (!mVars(2).CastToScalar(dScale, m_fSensitivity))
		{
			m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}
	}
	
	vA = *mVars(0).GetMultiVPtr();
	
	if (!CastMVtoE3(*mVars(1).GetMultiVPtr(), vB))
	{
		cStr csText = mVars(1).GetMultiVPtr()->Str();
		m_ErrorList.InvalidParVal(csText, 2, iLine, iPos);
		return false;
	}
	
	// Only Analyze Multivector
	m_pFilter->DrawMV(vA, true);
	MVInfo = m_pFilter->GetMVInfo();
	
	if (MVInfo.m_eType == GA_LINE)
	{
		int i;
		float fScale;
		COGLVertex xB, xD;
		
		// Offset vector
		for(i=0;i<3;i++)
			xB[i] = (float) vB[i+1];
		
		xD = MVInfo.m_mVex[1].Data(); // direction of line
		
		// Scale
		if (dScale != 0)
			fScale = 0.5f * (MVInfo.m_mVal[0] = (float) dScale);
		else
			fScale = 0.5f * MVInfo.m_mVal[0];
		
		xB = xB + fScale * xD;
		
		MVInfo.m_mVex[0] = xB;
	}
	else if (MVInfo.m_eType == GA_PLANE)
	{
		int i;
		COGLVertex xB, xD;
		
		// Offset vector
		for(i=0;i<3;i++)
			xB[i] = (float) vB[i+1];
		
		// Scale
		if (dScale != 0)
			MVInfo.m_mVal[0] = (float) dScale;
		
		MVInfo.m_mVex[0] = xB;
	}
	else
	{
		cStr csText = mVars(0).GetMultiVPtr()->Str();
		m_ErrorList.InvalidParVal(csText, 1, iLine, iPos);
		return false;
	}
	
	switch(m_pFilter->GetLastMVID())
	{
	case ID_E3GA:
		m_pFilter->m_E3Filter.DrawMVInfo(MVInfo);
		break;
		
	case ID_PGA:
		m_pFilter->m_P3Filter.DrawMVInfo(MVInfo);
		break;
		
	case ID_CONFGA:
		m_pFilter->m_N3Filter.DrawMVInfo(MVInfo);
		break;
		
	default:
		m_ErrorList.Internal(iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Draw Plane Function

bool CCLUCodeBase::DrawPlaneFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	int iVar;
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = mVars.Count();
	TCVScalar dVal[9];
	TCVScalar dRad = 0;
	
	if (iVarCount == 9 || iVarCount == 10)
	{
		for (iVar = 0; iVar < 9; iVar++)
		{
			if (!mVars(iVar).CastToScalar(dVal[iVar], m_fSensitivity))
			{
				m_ErrorList.InvalidParType(mVars(iVar), iVar+1, iLine, iPos);
				return false;
			}								
		}

		if (iVarCount == 10)
		{
			if (!mVars(9).CastToScalar(dRad))
			{
				m_ErrorList.InvalidParType(mVars(9), 10, iLine, iPos);
				return false;
			}								
		}

		COGLVertex xP, xA, xB;

		xP.Set((float) dVal[0], (float) dVal[1], (float) dVal[2]);
		xA.Set((float) dVal[3], (float) dVal[4], (float) dVal[5]);
		xB.Set((float) dVal[6], (float) dVal[7], (float) dVal[8]);
	
		if (dRad == 0)
			m_pFilter->DrawPlane(xP, xA, xB, false, true);
		else
			m_pFilter->DrawDisk(xP, xA, xB, Mag(dRad));

		//if (!rVar.New(PDT_VARLIST, "Constant"))
		//{
		//	m_ErrorList.Internal(iLine, iPos);
		//	return false;
		//}		
	} 
	else if (iVarCount == 3 || iVarCount == 4)
	{
		if (mVars(0).BaseType() != PDT_MULTIV)
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (mVars(1).BaseType() != PDT_MULTIV)
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		if (mVars(2).BaseType() != PDT_MULTIV)
		{
			m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}

		if (iVarCount == 4)
		{
			if (!mVars(3).CastToScalar(dRad))
			{
				m_ErrorList.InvalidParType(mVars(3), 4, iLine, iPos);
				return false;
			}								
		}

		TMultiV &vA = *mVars(0).GetMultiVPtr();
		TMultiV &vB = *mVars(1).GetMultiVPtr();
		TMultiV &vC = *mVars(2).GetMultiVPtr();
		TMultiV vX, vY, vZ;

		if (!CastMVtoE3(vA, vX))
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}								

		if (!CastMVtoE3(vB, vY))
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}								

		if (!CastMVtoE3(vC, vZ))
		{
			m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}								

		COGLVertex xA, xB, xC;
		xA.Set((float) vX[E3GA<TCVScalar>::iE1], 
			(float) vX[E3GA<TCVScalar>::iE2], 
			(float) vX[E3GA<TCVScalar>::iE3]);

		xB.Set((float) vY[E3GA<TCVScalar>::iE1], 
			(float) vY[E3GA<TCVScalar>::iE2], 
			(float) vY[E3GA<TCVScalar>::iE3]);

		xC.Set((float) vZ[E3GA<TCVScalar>::iE1], 
			(float) vZ[E3GA<TCVScalar>::iE2], 
			(float) vZ[E3GA<TCVScalar>::iE3]);

		if (dRad == 0)
			m_pFilter->DrawPlane(xA, xB, xC, false, true);
		else
			m_pFilter->DrawDisk(xA, xB, xC, Mag(dRad));
	}
	else
	{
		int piPar[] = {3, 9};

		m_ErrorList.WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}
	
	return true;
} 

//////////////////////////////////////////////////////////////////////
/// Draw Box Function

bool CCLUCodeBase::DrawBoxFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = mVars.Count();
	TCVScalar dVal[10];
/*	
	if (iVarCount == 10)
	{
		for (iVar = 0; iVar < 10; iVar++)
		{
			if (!mVars(iVar).CastToScalar(dVal[iVar], m_fSensitivity))
			{
				m_ErrorList.InvalidParType(mVars(iVar), 1, iLine, iPos);
				return false;
			}								
		}

		if (dVal[9] < 0)
		{
			m_ErrorList.InvalidParVal(mVars(9), 10, iLine, iPos);
			return false;
		}

		COGLVertex xP, xA, xB;

		xP.Set((float) dVal[0], (float) dVal[1], (float) dVal[2]);
		xA.Set((float) dVal[3], (float) dVal[4], (float) dVal[5]);
		xB.Set((float) dVal[6], (float) dVal[7], (float) dVal[8]);
	
		m_pFilter->DrawBox(xP, xA, xB, dVal[9], false, false);

		//if (!rVar.New(PDT_VARLIST, "Constant"))
		//{
		//	m_ErrorList.Internal(iLine, iPos);
		//	return false;
		//}		
	} 
	else */

	if (iVarCount == 4 || iVarCount == 5)
	{
		if (mVars(0).BaseType() != PDT_MULTIV)
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (mVars(1).BaseType() != PDT_MULTIV)
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		if (mVars(2).BaseType() != PDT_MULTIV)
		{
			m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}

		MemObj<COGLColor> mColList;

		if (iVarCount == 5)
		{
			if (mVars(4).BaseType() != PDT_VARLIST)
			{
				m_ErrorList.InvalidParType(mVars(4), 5, iLine, iPos);
				return false;
			}

			TVarList &rColList = *mVars(4).GetVarListPtr();
			if (rColList.Count() != 8)
			{
				m_ErrorList.GeneralError("Expect 8 colors in color list.", iLine, iPos);
				return false;
			}

			mColList.Set(8);

			int i;
			for( i = 0; i < 8; i++ )
			{
				if (rColList[i].BaseType() != PDT_COLOR)
				{
					char pcText[100];
					sprintf(pcText, "Element at position %d in color list is not a color.", i+1);
					m_ErrorList.GeneralError(pcText, iLine, iPos);
					return false;
				}

				mColList[i] = *rColList[i].GetOGLColorPtr();
			}
		}

		TMultiV &vP = *mVars(0).GetMultiVPtr();
		TMultiV &vA = *mVars(1).GetMultiVPtr();
		TMultiV &vB = *mVars(2).GetMultiVPtr();
		TMultiV vW, vX, vY;

		if (!CastMVtoE3(vP, vW))
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}								

		if (!CastMVtoE3(vA, vX))
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}								

		if (!CastMVtoE3(vB, vY))
		{
			m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}								

		COGLVertex xP, xA, xB, xC;
		xP.Set((float) vW[E3GA<TCVScalar>::iE1], 
			(float) vW[E3GA<TCVScalar>::iE2], 
			(float) vW[E3GA<TCVScalar>::iE3]);

		xA.Set((float) vX[E3GA<TCVScalar>::iE1], 
			(float) vX[E3GA<TCVScalar>::iE2], 
			(float) vX[E3GA<TCVScalar>::iE3]);

		xB.Set((float) vY[E3GA<TCVScalar>::iE1], 
			(float) vY[E3GA<TCVScalar>::iE2], 
			(float) vY[E3GA<TCVScalar>::iE3]);


		if (mVars(3).CastToScalar(dVal[0], m_fSensitivity))
		{
			if (dVal[0] <= 0)
			{
				m_ErrorList.GeneralError("Box depth has to be greater than zero.", iLine, iPos);
				return false;
			}

			xC = xA ^ xB; // vector cross product
			xC.Norm();
			xC *= float(dVal[0]);
		}
		else if (mVars(3).BaseType() == PDT_MULTIV)
		{
			TMultiV vZ;
			TMultiV &vC = *mVars(3).GetMultiVPtr();

			if (!CastMVtoE3(vC, vZ))
			{
				m_ErrorList.InvalidParType(mVars(3), 4, iLine, iPos);
				return false;
			}								

			xC.Set((float) vZ[E3GA<TCVScalar>::iE1], 
				(float) vZ[E3GA<TCVScalar>::iE2], 
				(float) vZ[E3GA<TCVScalar>::iE3]);
		}
		else
		{
			m_ErrorList.InvalidParType(mVars(3), 4, iLine, iPos);
			return false;
		}

		m_pFilter->DrawBox(xP, xA, xB, xC, mColList);
	}
	else
	{
		int piPar[] = {4, 10};

		m_ErrorList.WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}
	
	return true;
} 



//////////////////////////////////////////////////////////////////////
/// Draw Line Function

bool CCLUCodeBase::DrawLineFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	int iVar;
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	TCVScalar dVal[6];
	
	if (iVarCount == 6)
	{
		for (iVar = 0; iVar < 6; iVar++)
		{
			if (!mVars(iVar).CastToScalar(dVal[iVar], m_fSensitivity))
			{
				m_ErrorList.InvalidParType(mVars(iVar), 1, iLine, iPos);
				return false;
			}								
		}

		COGLVertex xA, xB;

		xA.Set((float) dVal[0], (float) dVal[1], (float) dVal[2]);
		xB.Set((float) dVal[3], (float) dVal[4], (float) dVal[5]);
	
		m_pFilter->DrawLine(xA, xB, true);

		//if (!rVar.New(PDT_VARLIST, "Constant"))
		//{
		//	m_ErrorList.Internal(iLine, iPos);
		//	return false;
		//}		
	} 
	else if (iVarCount == 2)
	{
		if (mVars(0).BaseType() != PDT_MULTIV)
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (mVars(1).BaseType() != PDT_MULTIV)
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		TMultiV &vA = *mVars(0).GetMultiVPtr();
		TMultiV &vB = *mVars(1).GetMultiVPtr();
		TMultiV vX, vY;

		if (!CastMVtoE3(vA, vX))
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}								

		if (!CastMVtoE3(vB, vY))
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}								

		COGLVertex xA, xB;
		xA.Set((float) vX[E3GA<TCVScalar>::iE1], 
			(float) vX[E3GA<TCVScalar>::iE2], 
			(float) vX[E3GA<TCVScalar>::iE3]);

		xB.Set((float) vY[E3GA<TCVScalar>::iE1], 
			(float) vY[E3GA<TCVScalar>::iE2], 
			(float) vY[E3GA<TCVScalar>::iE3]);

		m_pFilter->DrawLine(xA, xB, true);
	}
	else
	{
		int piPar[] = {2, 6};

		m_ErrorList.WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}
	
	return true;
} 

//////////////////////////////////////////////////////////////////////
/// Draw Point Function

bool CCLUCodeBase::DrawPointFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	int iVar;
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

		COGLVertex xA;

		xA.Set((float) dVal[0], (float) dVal[1], (float) dVal[2]);
	
		m_pFilter->DrawPoint(xA);

		//if (!rVar.New(PDT_VARLIST, "Constant"))
		//{
		//	m_ErrorList.Internal(iLine, iPos);
		//	return false;
		//}		
	} 
	// If a single multivector is given, cast it to E3
	// and use that position.
	else if (iVarCount == 1)
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


			COGLVertex xA;
			xA.Set((float) vB[E3GA<TCVScalar>::iE1], 
				   (float) vB[E3GA<TCVScalar>::iE2], 
				   (float) vB[E3GA<TCVScalar>::iE3]);
	
			m_pFilter->DrawPoint(xA);
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
/// Set Arrow Shape Function

bool CCLUCodeBase::SetArrowShapeFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	int iVar;
	TVarList& mVars = *rPars.GetVarListPtr();
	
	int iVarCount = mVars.Count();
	TCVScalar dVal[2];
	
	if (iVarCount == 2)
	{
		for (iVar = 0; iVar < 2; iVar++)
		{
			if (!mVars(iVar).CastToScalar(dVal[iVar], m_fSensitivity))
			{
				m_ErrorList.InvalidParType(mVars(iVar), iVar+1, iLine, iPos);
				return false;
			}								
		}

		if (dVal[0] <= 0)
		{
			m_ErrorList.GeneralError("Arrow length has to be larger than zero.", iLine, iPos);
			return false;
		}

		if (dVal[1] < 1 || dVal[1] > 89)
		{
			m_ErrorList.GeneralError("Arrow opening angle has to be in range [1,89] degress.", iLine, iPos);
			return false;
		}

		if (m_pFilter)
		{
			m_pFilter->SetArrowShape(float(dVal[0]), float(dVal[1]) * float(m_fRadPerDeg));
		}
	}
	else
	{
		int piPar[] = {2};

		m_ErrorList.WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Draw Arrow Function

bool CCLUCodeBase::DrawArrowFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	int iVar;
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	TCVScalar dVal[6];
	
	if (iVarCount == 6)
	{
		for (iVar = 0; iVar < 6; iVar++)
		{
			if (!mVars(iVar).CastToScalar(dVal[iVar], m_fSensitivity))
			{
				m_ErrorList.InvalidParType(mVars(iVar), iVar+1, iLine, iPos);
				return false;
			}								
		}

		COGLVertex xA, xB;

		xA.Set((float) dVal[0], (float) dVal[1], (float) dVal[2]);
		xB.Set((float) dVal[3], (float) dVal[4], (float) dVal[5]);
	
		m_pFilter->DrawVector(xA, xB);

		//if (!rVar.New(PDT_VARLIST, "Constant"))
		//{
		//	m_ErrorList.Internal(iLine, iPos);
		//	return false;
		//}		
	} 
	else if (iVarCount == 2)
	{
		if (mVars(0).BaseType() != PDT_MULTIV)
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (mVars(1).BaseType() != PDT_MULTIV)
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		TMultiV &vA = *mVars(0).GetMultiVPtr();
		TMultiV &vB = *mVars(1).GetMultiVPtr();
		TMultiV vX, vY;

		if (!CastMVtoE3(vA, vX))
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}								

		if (!CastMVtoE3(vB, vY))
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}								

		COGLVertex xA, xB;
		xA.Set((float) vX[E3GA<TCVScalar>::iE1], 
			(float) vX[E3GA<TCVScalar>::iE2], 
			(float) vX[E3GA<TCVScalar>::iE3]);

		xB.Set((float) vY[E3GA<TCVScalar>::iE1], 
			(float) vY[E3GA<TCVScalar>::iE2], 
			(float) vY[E3GA<TCVScalar>::iE3]);

		m_pFilter->DrawVector(xA, xB);
	}
	else
	{
		int piPar[] = {2, 6};

		m_ErrorList.WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}
	
	return true;
} 


//////////////////////////////////////////////////////////////////////
/// Draw Arc Function

bool CCLUCodeBase::DrawArcFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	int iVar;
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	TCVScalar dVal;
	bool bShort;
	
	if (iVarCount == 4 || iVarCount == 5)
	{
		for (iVar = 0; iVar < 3; iVar++)
		{
			if (mVars(iVar).BaseType() != PDT_MULTIV)
			{
				m_ErrorList.InvalidParType(mVars(iVar), iVar+1, iLine, iPos);
				return false;
			}
		}

		if (!mVars(3).CastToScalar(dVal, m_fSensitivity))
		{
			m_ErrorList.InvalidParType(mVars(3), 4, iLine, iPos);
			return false;
		}		

		if (iVarCount == 5)
		{
			int iVal;

			if (!mVars(4).CastToCounter(iVal))
			{
				m_ErrorList.InvalidParType(mVars(4), 5, iLine, iPos);
				return false;
			}		

			if (iVal)
				bShort = true;
			else
				bShort = false;
		}
		else
		{
			bShort = true;
		}

		TMultiV &vP = *mVars(0).GetMultiVPtr();
		TMultiV &vA = *mVars(1).GetMultiVPtr();
		TMultiV &vB = *mVars(2).GetMultiVPtr();
		TMultiV vX, vY, vZ;

		if (!CastMVtoE3(vP, vX))
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}								

		if (!CastMVtoE3(vA, vY))
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}								

		if (!CastMVtoE3(vB, vZ))
		{
			m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}								

		COGLVertex xA, xB, xP;
		xP.Set((float) vX[E3GA<TCVScalar>::iE1], 
			(float) vX[E3GA<TCVScalar>::iE2], 
			(float) vX[E3GA<TCVScalar>::iE3]);

		xA.Set((float) vY[E3GA<TCVScalar>::iE1], 
			(float) vY[E3GA<TCVScalar>::iE2], 
			(float) vY[E3GA<TCVScalar>::iE3]);

		xB.Set((float) vZ[E3GA<TCVScalar>::iE1], 
			(float) vZ[E3GA<TCVScalar>::iE2], 
			(float) vZ[E3GA<TCVScalar>::iE3]);

		m_pFilter->DrawArc(xP, xA, xB, float(dVal), bShort);
	}
	else
	{
		int piPar[] = {4};

		m_ErrorList.WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}
	
	return true;
} 


//////////////////////////////////////////////////////////////////////
/// Draw Disk Function

bool CCLUCodeBase::DrawDiskFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	int iVar;
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	TCVScalar dVal;
	bool bShort;
	
	if (iVarCount == 4 || iVarCount == 5)
	{
		for (iVar = 0; iVar < 3; iVar++)
		{
			if (mVars(iVar).BaseType() != PDT_MULTIV)
			{
				m_ErrorList.InvalidParType(mVars(iVar), iVar+1, iLine, iPos);
				return false;
			}
		}

		if (!mVars(3).CastToScalar(dVal, m_fSensitivity))
		{
			m_ErrorList.InvalidParType(mVars(3), 4, iLine, iPos);
			return false;
		}		

		if (iVarCount == 5)
		{
			int iVal;

			if (!mVars(4).CastToCounter(iVal))
			{
				m_ErrorList.InvalidParType(mVars(4), 5, iLine, iPos);
				return false;
			}		

			if (iVal)
				bShort = true;
			else
				bShort = false;
		}
		else
		{
			bShort = true;
		}

		TMultiV &vP = *mVars(0).GetMultiVPtr();
		TMultiV &vA = *mVars(1).GetMultiVPtr();
		TMultiV &vB = *mVars(2).GetMultiVPtr();
		TMultiV vX, vY, vZ;

		if (!CastMVtoE3(vP, vX))
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}								

		if (!CastMVtoE3(vA, vY))
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}								

		if (!CastMVtoE3(vB, vZ))
		{
			m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}								

		COGLVertex xA, xB, xP;
		xP.Set((float) vX[E3GA<TCVScalar>::iE1], 
			(float) vX[E3GA<TCVScalar>::iE2], 
			(float) vX[E3GA<TCVScalar>::iE3]);

		xA.Set((float) vY[E3GA<TCVScalar>::iE1], 
			(float) vY[E3GA<TCVScalar>::iE2], 
			(float) vY[E3GA<TCVScalar>::iE3]);

		xB.Set((float) vZ[E3GA<TCVScalar>::iE1], 
			(float) vZ[E3GA<TCVScalar>::iE2], 
			(float) vZ[E3GA<TCVScalar>::iE3]);

		m_pFilter->DrawArc(xP, xA, xB, float(dVal), bShort, true);
	}
	else
	{
		int piPar[] = {4};

		m_ErrorList.WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}
	
	return true;
} 


//////////////////////////////////////////////////////////////////////
/// Draw Ellipse Function

bool CCLUCodeBase::DrawEllipseFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	int iVar;
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	
	if (iVarCount == 3)
	{
		for (iVar = 0; iVar < 3; iVar++)
		{
			if (mVars(iVar).BaseType() != PDT_MULTIV)
			{
				m_ErrorList.InvalidParType(mVars(iVar), iVar+1, iLine, iPos);
				return false;
			}
		}

		TMultiV &vP = *mVars(0).GetMultiVPtr();
		TMultiV &vA = *mVars(1).GetMultiVPtr();
		TMultiV &vB = *mVars(2).GetMultiVPtr();
		TMultiV vC, vX, vY;

		if (!CastMVtoE3(vP, vC))
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}								

		if (!CastMVtoE3(vA, vX))
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}								

		if (!CastMVtoE3(vB, vY))
		{
			m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}								

		COGLVertex xP, xA, xB;
		xP.Set((float) vC[E3GA<TCVScalar>::iE1], 
			(float) vC[E3GA<TCVScalar>::iE2], 
			(float) vC[E3GA<TCVScalar>::iE3]);

		xA.Set((float) vX[E3GA<TCVScalar>::iE1], 
			(float) vX[E3GA<TCVScalar>::iE2], 
			(float) vX[E3GA<TCVScalar>::iE3]);

		xB.Set((float) vY[E3GA<TCVScalar>::iE1], 
			(float) vY[E3GA<TCVScalar>::iE2], 
			(float) vY[E3GA<TCVScalar>::iE3]);

		m_pFilter->DrawEllipse(xP, xA, xB);
	}
	else
	{
		int piPar[] = {3};

		m_ErrorList.WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}
	
	return true;
} 


//////////////////////////////////////////////////////////////////////
/// Draw Ellipsoid Function

bool CCLUCodeBase::DrawEllipsoidFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	int iVar;
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	
	if (iVarCount == 4)
	{
		for (iVar = 0; iVar < 4; iVar++)
		{
			if (mVars(iVar).BaseType() != PDT_MULTIV)
			{
				m_ErrorList.InvalidParType(mVars(iVar), iVar+1, iLine, iPos);
				return false;
			}
		}

		TMultiV &vP = *mVars(0).GetMultiVPtr();
		TMultiV &vA = *mVars(1).GetMultiVPtr();
		TMultiV &vB = *mVars(2).GetMultiVPtr();
		TMultiV &vC = *mVars(3).GetMultiVPtr();
		TMultiV vQ, vX, vY, vZ;

		if (!CastMVtoE3(vP, vQ))
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}								

		if (!CastMVtoE3(vA, vX))
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}								

		if (!CastMVtoE3(vB, vY))
		{
			m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}								

		if (!CastMVtoE3(vC, vZ))
		{
			m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}								

		COGLVertex xP, xA, xB, xC;
		xP.Set((float) vQ[E3GA<TCVScalar>::iE1], 
			(float) vQ[E3GA<TCVScalar>::iE2], 
			(float) vQ[E3GA<TCVScalar>::iE3]);

		xA.Set((float) vX[E3GA<TCVScalar>::iE1], 
			(float) vX[E3GA<TCVScalar>::iE2], 
			(float) vX[E3GA<TCVScalar>::iE3]);

		xB.Set((float) vY[E3GA<TCVScalar>::iE1], 
			(float) vY[E3GA<TCVScalar>::iE2], 
			(float) vY[E3GA<TCVScalar>::iE3]);

		xC.Set((float) vZ[E3GA<TCVScalar>::iE1], 
			(float) vZ[E3GA<TCVScalar>::iE2], 
			(float) vZ[E3GA<TCVScalar>::iE3]);

		m_pFilter->DrawEllipsoid(xP, xA, xB, xC, true);
	}
	else
	{
		int piPar[] = {4};

		m_ErrorList.WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}
	
	return true;
} 


//////////////////////////////////////////////////////////////////////
/// Draw Circle Function

bool CCLUCodeBase::DrawCircleFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	int iVar;
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	TCVScalar dVal;
	
	if (iVarCount == 3)
	{
		for (iVar = 0; iVar < 2; iVar++)
		{
			if (mVars(iVar).BaseType() != PDT_MULTIV)
			{
				m_ErrorList.InvalidParType(mVars(iVar), iVar+1, iLine, iPos);
				return false;
			}
		}

		if (!mVars(2).CastToScalar(dVal, m_fSensitivity))
		{
			m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}		

		TMultiV &vP = *mVars(0).GetMultiVPtr();
		TMultiV &vN = *mVars(1).GetMultiVPtr();
		TMultiV vX, vY;

		if (!CastMVtoE3(vP, vX))
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}								

		if (!CastMVtoE3(vN, vY))
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}								

		COGLVertex xN, xP;
		xP.Set((float) vX[E3GA<TCVScalar>::iE1], 
			(float) vX[E3GA<TCVScalar>::iE2], 
			(float) vX[E3GA<TCVScalar>::iE3]);

		xN.Set((float) vY[E3GA<TCVScalar>::iE1], 
			(float) vY[E3GA<TCVScalar>::iE2], 
			(float) vY[E3GA<TCVScalar>::iE3]);

		m_pFilter->DrawCircle(xP, xN, float(dVal));
	}
	else
	{
		int piPar[] = {3};

		m_ErrorList.WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}
	
	return true;
} 


//////////////////////////////////////////////////////////////////////
/// Draw Circle Surface Function
///
/// Expects a list of multivectors representing circles
///
/// Parameters:
///		1. List of circles as multivectors in N3
///		2. List of colors (optional)
///		3. Flag indicating whether to draw surface immediately (optional)
///
///	Returns:
///		Vertex List representing object.

bool CCLUCodeBase::DrawCircleSurfaceFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!m_pFilter)
	{
		m_ErrorList.GeneralError("Internal multivector analysis filter not set.", iLine, iPos);
		return false;
	}

	TVarList *pColList = 0;
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	bool bUseCol = false;
	bool bDoDraw = true;
	
	if (iVarCount < 1 || iVarCount > 3)
	{
		int piPar[] = {1, 2};

		m_ErrorList.WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_VARLIST)
	{
		m_ErrorList.InvalidParType(mVars(0), 0, iLine, iPos);
		return false;
	}

	if (iVarCount >= 2)
	{
		if (mVars(1).BaseType() == PDT_VARLIST)
		{
			bUseCol = true;
			pColList = mVars(1).GetVarListPtr();
		}
/*
		else
		{
			m_ErrorList.InvalidParType(mVars(1), 1, iLine, iPos);
			return false;
		}
*/
	}

	if (iVarCount >= 3)
	{
		int iVal;
		if (!mVars(2).CastToCounter(iVal))
		{
			m_ErrorList.InvalidParType(mVars(2), 2, iLine, iPos);
			return false;
		}

		if (iVal)
			bDoDraw = true;
		else
			bDoDraw = false;
	}

	CMVInfo<float> MVInfo;
	MemObj<COGLVertex> mCenter, mNormal;
	Mem<float> mRadius;
	MemObj<COGLColor> mColor;

	TVarList &CList = *mVars(0).GetVarListPtr();
	int iP, iCount = CList.Count();

	mCenter.Set(iCount);
	mNormal.Set(iCount);
	mRadius.Set(iCount);

	if (bUseCol)
		mColor.Set(iCount);

	for (iP = 0; iP < iCount; iP++)
	{
		if (CList[iP].BaseType() != PDT_MULTIV)
		{
			m_ErrorList.GeneralError("Element in list is not a multivector.", iLine, iPos);
			return false;
		}

		if (bUseCol)
		{
			if ((*pColList)[iP].BaseType() != PDT_COLOR)
			{
				m_ErrorList.GeneralError("Element in color list is not a color.", iLine, iPos);
				return false;
			}
			
			mColor[iP] = *((COGLColor *) (*pColList)[iP].Val());
		}

		TMultiV &vC = *((TMultiV *) CList[iP].Val());

		// Only analyze multivector
		m_pFilter->DrawMV(vC, true);

		// Get MVInfo
		MVInfo = m_pFilter->GetMVInfo();

		if (MVInfo.m_eType != GA_CIRCLE)
		{
			char pcText[200];
			
			sprintf(pcText, "The multivectors at position %d in list does not represent a circle.", iP+1);
			m_ErrorList.GeneralError(pcText, iLine, iPos);
			return false;
		}
		
		mCenter[iP] = MVInfo.m_mVex[0].Data();
		mNormal[iP] = MVInfo.m_mVex[1].Data();
		mRadius[iP] = MVInfo.m_mVal[0];
	}

	rVar.New(PDT_VEXLIST, "CircleSurface");
	TVexList *pVexList = rVar.GetVexListPtr();

	m_pFilter->DrawCircleSurface(mCenter, mNormal, mRadius, mColor, 15.0f, pVexList, bDoDraw);

	return true;
} 



//////////////////////////////////////////////////////////////////////
/// Draw Cylinder Function
///
/// Expects:
///		1. MV giving center of bottom
///		2. MV giving direction and length of cylinder
///		3. Scalar giving radius of cylinder
///		4. (optional) Boolean value: if true draw cylinder directly

bool CCLUCodeBase::DrawCylinderFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!m_pFilter)
	{
		m_ErrorList.GeneralError("Internal multivector analysis filter not set.", iLine, iPos);
		return false;
	}

	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	bool bDoDraw = true;
	int iAngleSteps = 72;
	
	if (iVarCount < 3 || iVarCount > 5)
	{
		int piPar[] = {3, 4, 5};

		m_ErrorList.WrongNoOfParams(piPar, 3, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_MULTIV)
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_MULTIV)
	{
		m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}

	TCVScalar dR;
	if (!mVars(2).CastToScalar(dR, m_fSensitivity))
	{
		m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
		return false;
	}

	if (iVarCount >= 4)
	{
		int iVal;
		if (!mVars(3).CastToCounter(iVal))
		{
			m_ErrorList.InvalidParType(mVars(3), 4, iLine, iPos);
			return false;
		}

		if (iVal)
			bDoDraw = true;
		else
			bDoDraw = false;
	}

	if (iVarCount >= 5)
	{
		int iVal;
		if (!mVars(4).CastToCounter(iVal))
		{
			m_ErrorList.InvalidParType(mVars(4), 5, iLine, iPos);
			return false;
		}

		if (iVal < 3 || iVal > 360)
		{
			m_ErrorList.GeneralError("Number of slices needs to be between 3 and 360.", iLine, iPos);
			return false;
		}

		iAngleSteps = iVal;
	}

	float fAngle = 360.0f / float(iAngleSteps);

	TMultiV vC, vX;
	COGLVertex xCenter, xAxis;

	TMultiV &vA = *mVars(0).GetMultiVPtr();
	TMultiV &vB = *mVars(1).GetMultiVPtr();

	CastMVtoE3(vA, vC);
	CastMVtoE3(vB, vX);

	xCenter.Set(float(vC[1]), float(vC[2]), float(vC[3]));
	xAxis.Set(float(vX[1]), float(vX[2]), float(vX[3]));

	rVar.New(PDT_VEXLIST, "CircleSurface");
	TVexList *pVexList = rVar.GetVexListPtr();

	m_pFilter->DrawCylinder(xCenter, xAxis, float(dR), fAngle, pVexList, bDoDraw);

	return true;
} 



//////////////////////////////////////////////////////////////////////
/// Draw Line Surface Function
///
/// Expects a list of multivectors representing lines

bool CCLUCodeBase::DrawLineSurfaceFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!m_pFilter)
	{
		m_ErrorList.GeneralError("Internal multivector analysis filter not set.", iLine, iPos);
		return false;
	}

	TVarList *pColList = 0;
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	bool bUseCol = false;
	bool bDoDraw = true;
	
	if (iVarCount < 1 || iVarCount > 3)
	{
		int piPar[] = {1, 2};

		m_ErrorList.WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_VARLIST)
	{
		m_ErrorList.InvalidParType(mVars(0), 0, iLine, iPos);
		return false;
	}

	if (iVarCount >= 2)
	{
		if (mVars(1).BaseType() == PDT_VARLIST)
		{
			bUseCol = true;
			pColList = mVars(1).GetVarListPtr();
		}
/*
		else
		{
			m_ErrorList.InvalidParType(mVars(1), 1, iLine, iPos);
			return false;
		}
*/
	}

	if (iVarCount >= 3)
	{
		int iVal;
		if (!mVars(2).CastToCounter(iVal))
		{
			m_ErrorList.InvalidParType(mVars(2), 2, iLine, iPos);
			return false;
		}

		if (iVal)
			bDoDraw = true;
		else
			bDoDraw = false;
	}

	CMVInfo<float> MVInfo;
	MemObj<COGLVertex> mCenter, mDir;
	Mem<float> mLen;
	MemObj<COGLColor> mColor;

	TVarList &CList = *mVars(0).GetVarListPtr();
	int iP, iCount = CList.Count();

	mCenter.Set(iCount);
	mDir.Set(iCount);
	mLen.Set(iCount);

	if (bUseCol)
		mColor.Set(iCount);

	for (iP = 0; iP < iCount; iP++)
	{
		if (CList[iP].BaseType() != PDT_MULTIV)
		{
			m_ErrorList.GeneralError("Element in list is not a multivector.", iLine, iPos);
			return false;
		}

		if (bUseCol)
		{
			if ((*pColList)[iP].BaseType() != PDT_COLOR)
			{
				m_ErrorList.GeneralError("Element in color list is not a color.", iLine, iPos);
				return false;
			}
			
			mColor[iP] = *((COGLColor *) (*pColList)[iP].Val());
		}

		TMultiV &vC = *((TMultiV *) CList[iP].Val());

		// Only analyze multivector
		m_pFilter->DrawMV(vC, true);

		// Get MVInfo
		MVInfo = m_pFilter->GetMVInfo();

		if (MVInfo.m_eType != GA_LINE)
		{
			char pcText[200];
			
			sprintf(pcText, "The multivectors at position %d in list does not represent a line.", iP+1);
			m_ErrorList.GeneralError(pcText, iLine, iPos);
			return false;
		}
		
		mCenter[iP] = MVInfo.m_mVex[0].Data();
		mDir[iP] = MVInfo.m_mVex[1].Data();
		mLen[iP] = MVInfo.m_mVal[0];
	}

	rVar.New(PDT_VEXLIST, "CircleSurface");
	TVexList *pVexList = rVar.GetVexListPtr();

	m_pFilter->DrawLineSurface(mCenter, mDir, mLen, mColor, 20, pVexList, bDoDraw);

	return true;
} 




//////////////////////////////////////////////////////////////////////
/// Draw Point Surface Function
///
/// Expects:
/// 1. Number of columns, ie width in x-direction
/// 2. Number of rows, ie width in y-direction
/// 3. List of multivectors representing points in E3
/// 4. List of colors for each point (optional)
/// 5. Bool flag indicating whether surface is to be drawn directly (optional)
/// 6. Bool flag indicating whether surface normals are to be negated (optional)
/// 7. List of multivectors representing corresponding normals (optional)
/// 8. Scale of normal lines to be drawn (optional). By default this is 0, i.e. nothing
///		is drawn.
/// 9. List of multivectors representing 2D-texture coordinates. (optional)
///
/// Returns:
/// List of list of vertices that may be drawn later on with :-operator

bool CCLUCodeBase::DrawPointSurfaceFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!m_pFilter)
	{
		m_ErrorList.GeneralError("Internal multivector analysis filter not set.", iLine, iPos);
		return false;
	}

	TVarList *pColList = 0;
	TVarList *pNormalList = 0;
	TVarList *pTexList = 0;
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	int iRowCount, iColCount;
	bool bUseCol = false;
	bool bUseNormals = false;
	bool bUseTex = false;
	bool bNegateNormals = false;
	bool bDoDraw = true;
	TCVScalar fNormScale = 0;
	
	if (iVarCount < 1 || iVarCount > 9)
	{
		int piPar[] = {3, 4, 5, 6, 7, 8, 9};

		m_ErrorList.WrongNoOfParams(piPar, 7, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToCounter(iColCount))
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToCounter(iRowCount))
	{
		m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}

	if (mVars(2).BaseType() != PDT_VARLIST)
	{
		m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
		return false;
	}

	if (iVarCount >= 4)
	{
		if (mVars(3).BaseType() == PDT_VARLIST)
		{
			bUseCol = true;
			pColList = mVars(3).GetVarListPtr();
		}
/*
		else
		{
			m_ErrorList.InvalidParType(mVars(3), 4, iLine, iPos);
			return false;
		}
*/
	}

	if (iVarCount >= 5)
	{
		int iVal;
		if (!mVars(4).CastToCounter(iVal))
		{
			m_ErrorList.InvalidParType(mVars(4), 5, iLine, iPos);
			return false;
		}

		if (iVal)
			bDoDraw = true;
		else
			bDoDraw = false;
	}

	if (iVarCount >= 6)
	{
		int iVal;
		if (!mVars(5).CastToCounter(iVal))
		{
			m_ErrorList.InvalidParType(mVars(5), 6, iLine, iPos);
			return false;
		}

		if (iVal)
			bNegateNormals = true;
		else
			bNegateNormals = false;
	}

	if (iVarCount >= 7)
	{
		if (mVars(6).BaseType() == PDT_VARLIST)
		{
			bUseNormals = true;
			pNormalList = mVars(6).GetVarListPtr();
		}
/*
		else
		{
			m_ErrorList.InvalidParType(mVars(6), 7, iLine, iPos);
			return false;
		}
*/
	}

	if (iVarCount >= 8)
	{
		if (!mVars(7).CastToScalar(fNormScale, m_fSensitivity))
		{
			m_ErrorList.InvalidParType(mVars(7), 8, iLine, iPos);
			return false;
		}
	}

	if (iVarCount >= 9)
	{
		if (mVars(8).BaseType() == PDT_VARLIST)
		{
			bUseTex = true;
			pTexList = mVars(8).GetVarListPtr();
		}
	}

	CMVInfo<float> MVInfo;
	MemObj<COGLVertex> mPoint, mNormal, mTex;
	MemObj<COGLColor> mColor;
	TMultiV vZero;
	TCVScalar dVal;
	bool bIsOrigin;

	TVarList &PList = *mVars(2).GetVarListPtr();
	int iP, iCount = PList.Count();

	if (iRowCount * iColCount != iCount)
	{
		m_ErrorList.GeneralError("Number of points given is incompatible to row and column dimensions.", 
			iLine, iPos);
		return false;
	}

	if (pNormalList && iCount != pNormalList->Count())
	{
		m_ErrorList.GeneralError("List of points and list of normals are of different length.", iLine, iPos);
		return false;
	}

	mPoint.Set(iCount);

	if (bUseNormals)
		mNormal.Set(iCount);

	if (bUseTex)
		mTex.Set(iCount);

	if (bUseCol)
		mColor.Set(iCount);

	for (iP = 0; iP < iCount; iP++)
	{
		bIsOrigin = false;
		if (PList(iP).BaseType() != PDT_MULTIV)
		{
			if (PList(iP).CastToScalar(dVal))
			{
				bIsOrigin = true;
			}
			else
			{
				char pcText[200];
				sprintf(pcText, "Element at position %d with value %s is not a multivector.",
					iP+1, PList[iP].ValStr().Str());
				m_ErrorList.GeneralError(pcText/*"Element in list is not a multivector."*/, iLine, iPos);
				return false;
			}
		}

		if (bUseNormals)
		{
			if ((*pNormalList)(iP).BaseType() != PDT_MULTIV)
			{
				m_ErrorList.GeneralError("Element in normals list is not a multivector.", iLine, iPos);
				return false;
			}

			/////////////////////////////////////////////////
			// Get Normal
			TMultiV &vN = *((TMultiV *) (*pNormalList)[iP].Val());
			// Only analyze multivector
			m_pFilter->DrawMV(vN, true);
			// Get MVInfo
			MVInfo = m_pFilter->GetMVInfo();

			if (MVInfo.m_eType != GA_POINT && MVInfo.m_eType != GA_DIR1D)
			{
				m_ErrorList.GeneralError("One of the multivectors passed does not represent a point or a direction.", iLine, iPos);
				return false;
			}
			mNormal[iP] = MVInfo.m_mVex[0].Data();
			mNormal[iP].Norm();
		}

		if (bUseTex)
		{
			if ((*pTexList)[iP].BaseType() != PDT_MULTIV)
			{
				m_ErrorList.GeneralError("Element in texture coordinate list is not a multivector.", iLine, iPos);
				return false;
			}

			/////////////////////////////////////////////////
			// Get Texture Coordinate
			TMultiV &vN = *((TMultiV *) (*pTexList)[iP].Val());
			// Only analyze multivector
			m_pFilter->DrawMV(vN, true);
			// Get MVInfo
			MVInfo = m_pFilter->GetMVInfo();

			if (MVInfo.m_eType == GA_POINT || 
				MVInfo.m_eType == GA_DIR1D)
			{
				mTex[iP] = MVInfo.m_mVex[0].Data();
				mTex[iP][1] = float(1) - mTex[iP][1];
			}
			else if (MVInfo.m_eType == GA_SCALAR || vN == vZero)
			{
				mTex[iP].Set(0, 1, 0);
			}
			else
			{
				m_ErrorList.GeneralError("One of the multivectors passed as texture coordinate does not represent a point or a direction.", iLine, iPos);
				return false;
			}
		}

		if (bUseCol)
		{
			if ((*pColList)[iP].BaseType() != PDT_COLOR)
			{
				m_ErrorList.GeneralError("Element in color list is not a color.", iLine, iPos);
				return false;
			}
			
			mColor[iP] = *((COGLColor *) (*pColList)[iP].Val());
		}

		/////////////////////////////////////////////////
		// Get Point
		if (bIsOrigin)
		{
			mPoint[iP].Set(0,0,0);
		}
		else
		{
			TMultiV &vP = *(PList[iP].GetMultiVPtr());
			vZero.SetStyle(vP.GetStyle());
			vZero = 0;

			// Only analyze multivector
			m_pFilter->DrawMV(vP, true);
			// Get MVInfo
			MVInfo = m_pFilter->GetMVInfo();

			if (MVInfo.m_eType == GA_POINT)
			{
				mPoint[iP] = MVInfo.m_mVex[0].Data();		
			}
			else if (MVInfo.m_eType == GA_SCALAR || vP == vZero)
			{
				mPoint[iP].Set(0, 0, 0);
			}
			else
			{
				char pcText[200];
				sprintf(pcText, "The multivector at position %d in list does not represent a point.", iP+1);
				m_ErrorList.GeneralError(pcText, iLine, iPos);
				return false;
			}
		}
	}

	MemObj<COGLVertexList*> mVexListPtr;
	rVar.New(PDT_VARLIST, "List of VertexLists");
	TVarList &rVarList = *rVar.GetVarListPtr();

	if (fNormScale != 0)
	{
		rVarList.Set(2);
		mVexListPtr.Set(2);

		rVarList[0].New(PDT_VEXLIST, "PointSurface");
		rVarList[1].New(PDT_VEXLIST, "NormalLines");

		mVexListPtr[0] = rVarList[0].GetVexListPtr();
		mVexListPtr[1] = rVarList[1].GetVexListPtr();
	}
	else
	{
		rVarList.Set(1);
		mVexListPtr.Set(1);

		rVarList[0].New(PDT_VEXLIST, "PointSurface");
		mVexListPtr[0] = rVarList[0].GetVexListPtr();
	}

	m_pFilter->DrawPointSurface(iRowCount, iColCount, mPoint, mNormal, mTex, mColor, 
								bNegateNormals, float(fNormScale), &mVexListPtr, bDoDraw);

	return true;
} 


//////////////////////////////////////////////////////////////////////
/// Draw Point Grid Function
///
/// Expects:
/// 1. Number of columns, ie width in x-direction
/// 2. Number of rowas, ie width in y-direction
/// 3. List of multivectors representing points in E3
/// 4. List of colors for each point (optional)
/// 5. Bool flag indicating whether surface is to be drawn directly (optional)
/// 6. Bool flag indicating whether surface normals are to be negated (optional)
/// 7. List of multivectors representing corresponding normals (optional)
///
/// Returns:
/// List of vertices that may be drawn later on with :-operator

bool CCLUCodeBase::DrawPointGridFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!m_pFilter)
	{
		m_ErrorList.GeneralError("Internal multivector analysis filter not set.", iLine, iPos);
		return false;
	}

	TVarList *pColList = 0;
	TVarList *pNormalList = 0;
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	int iRowCount, iColCount;
	bool bUseCol = false;
	bool bUseNormals = false;
	bool bNegateNormals = false;
	bool bDoDraw = true;
	
	if (iVarCount < 1 || iVarCount > 7)
	{
		int piPar[] = {3, 4, 5, 6, 7};

		m_ErrorList.WrongNoOfParams(piPar, 5, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToCounter(iColCount))
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToCounter(iRowCount))
	{
		m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}

	if (mVars(2).BaseType() != PDT_VARLIST)
	{
		m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
		return false;
	}

	if (iVarCount >= 4)
	{
		if (mVars(3).BaseType() != PDT_VARLIST)
		{
			m_ErrorList.InvalidParType(mVars(3), 4, iLine, iPos);
			return false;
		}

		bUseCol = true;
		pColList = mVars(3).GetVarListPtr();
	}

	if (iVarCount >= 5)
	{
		int iVal;
		if (!mVars(4).CastToCounter(iVal))
		{
			m_ErrorList.InvalidParType(mVars(4), 5, iLine, iPos);
			return false;
		}

		if (iVal)
			bDoDraw = true;
		else
			bDoDraw = false;
	}

	if (iVarCount >= 6)
	{
		int iVal;
		if (!mVars(5).CastToCounter(iVal))
		{
			m_ErrorList.InvalidParType(mVars(5), 6, iLine, iPos);
			return false;
		}

		if (iVal)
			bNegateNormals = true;
		else
			bNegateNormals = false;
	}

	if (iVarCount >= 7)
	{
		if (mVars(6).BaseType() != PDT_VARLIST)
		{
			m_ErrorList.InvalidParType(mVars(6), 7, iLine, iPos);
			return false;
		}

		bUseNormals = true;
		pNormalList = mVars(6).GetVarListPtr();
	}

	CMVInfo<float> MVInfo;
	MemObj<COGLVertex> mPoint, mNormal;
	MemObj<COGLColor> mColor;
	TMultiV vZero;
	TCVScalar dVal;
	bool bIsOrigin;

	TVarList &PList = *mVars(2).GetVarListPtr();
	int iP, iCount = PList.Count();

	if (iRowCount * iColCount != iCount)
	{
		m_ErrorList.GeneralError("Number of points given is incompatible to row and column dimensions.", 
			iLine, iPos);
		return false;
	}

	if (pNormalList && iCount != pNormalList->Count())
	{
		m_ErrorList.GeneralError("List of points and list of normals are of different length.", iLine, iPos);
		return false;
	}

	mPoint.Set(iCount);

	if (bUseNormals)
		mNormal.Set(iCount);

	if (bUseCol)
		mColor.Set(iCount);

	for (iP = 0; iP < iCount; iP++)
	{
		bIsOrigin = false;
		if (PList(iP).BaseType() != PDT_MULTIV)
		{
			if (PList(iP).CastToScalar(dVal))
			{
				bIsOrigin = true;
			}
			else
			{
				m_ErrorList.GeneralError("Element in list is not a multivector.", iLine, iPos);
				return false;
			}
		}

		if (bUseNormals)
		{
			if ((*pNormalList)[iP].BaseType() != PDT_MULTIV)
			{
				m_ErrorList.GeneralError("Element in normals list is not a multivector.", iLine, iPos);
				return false;
			}

			/////////////////////////////////////////////////
			// Get Normal
			TMultiV &vN = *((TMultiV *) (*pNormalList)[iP].Val());
			// Only analyze multivector
			m_pFilter->DrawMV(vN, true);
			// Get MVInfo
			MVInfo = m_pFilter->GetMVInfo();

			if (MVInfo.m_eType != GA_POINT && MVInfo.m_eType != GA_DIR1D)
			{
				m_ErrorList.GeneralError("One of the multivectors passed does not represent a point or a direction.", iLine, iPos);
				return false;
			}
			mNormal[iP] = MVInfo.m_mVex[0].Data();
		}


		if (bUseCol)
		{
			if ((*pColList)[iP].BaseType() != PDT_COLOR)
			{
				m_ErrorList.GeneralError("Element in color list is not a color.", iLine, iPos);
				return false;
			}
			
			mColor[iP] = *((COGLColor *) (*pColList)[iP].Val());
		}

		/////////////////////////////////////////////////
		// Get Point
		if (bIsOrigin)
		{
			mPoint[iP].Set(0,0,0);
		}
		else
		{
			TMultiV &vP = *((TMultiV *) PList[iP].Val());
			vZero.SetStyle(vP.GetStyle());
			vZero = 0;

			// Only analyze multivector
			m_pFilter->DrawMV(vP, true);
			// Get MVInfo
			MVInfo = m_pFilter->GetMVInfo();

			if (MVInfo.m_eType == GA_POINT)
			{
				mPoint[iP] = MVInfo.m_mVex[0].Data();		
			}
			else if (MVInfo.m_eType == GA_SCALAR || vP == vZero)
			{
				mPoint[iP].Set(0, 0, 0);
			}
			else
			{
				char pcText[200];
				sprintf(pcText, "The multivector at position %d in list does not represent a point.", iP+1);
				m_ErrorList.GeneralError(pcText, iLine, iPos);
				return false;
			}
		}
	}

	rVar.New(PDT_VEXLIST, "PointSurface");
	TVexList *pVexList = rVar.GetVexListPtr();

	if (!m_pFilter->DrawPointGrid(iRowCount, iColCount, mPoint, mNormal, mColor, bNegateNormals, pVexList, bDoDraw))
	{
		m_ErrorList.GeneralError("Error drawing point list.", iLine, iPos);
		return false;
	}

	return true;
} 



//////////////////////////////////////////////////////////////////////
/// Draw Point List Function
///
/// Expects:
/// 1. List of multivectors representing points in E3
/// 2. List of colors for each point (optional)
/// 3. Bool flag indicating whether surface is to be drawn directly (optional)
/// 4. Bool flag indicating whether surface normals are to be negated (optional)
/// 5. List of multivectors representing corresponding normals (optional)
///
/// Returns:
/// List of vertices that may be drawn later on with :-operator

bool CCLUCodeBase::DrawPointListFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!m_pFilter)
	{
		m_ErrorList.GeneralError("Internal multivector analysis filter not set.", iLine, iPos);
		return false;
	}

	TVarList *pColList = 0;
	TVarList *pNormalList = 0;
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	bool bUseCol = false;
	bool bUseNormals = false;
	bool bNegateNormals = false;
	bool bDoDraw = true;
	
	if (iVarCount < 1 || iVarCount > 5)
	{
		int piPar[] = {1, 2, 3, 4, 5};

		m_ErrorList.WrongNoOfParams(piPar, 5, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_VARLIST)
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (iVarCount >= 2)
	{
		if (mVars(1).BaseType() == PDT_VARLIST)
		{
			bUseCol = true;
			pColList = mVars(1).GetVarListPtr();
		}
/*
		else
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}
*/
	}

	if (iVarCount >= 3)
	{
		int iVal;
		if (!mVars(2).CastToCounter(iVal))
		{
			m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}

		if (iVal)
			bDoDraw = true;
		else
			bDoDraw = false;
	}

	if (iVarCount >= 4)
	{
		int iVal;
		if (!mVars(3).CastToCounter(iVal))
		{
			m_ErrorList.InvalidParType(mVars(3), 4, iLine, iPos);
			return false;
		}

		if (iVal)
			bNegateNormals = true;
		else
			bNegateNormals = false;
	}

	if (iVarCount >= 5)
	{
		if (mVars(4).BaseType() != PDT_VARLIST)
		{
			m_ErrorList.InvalidParType(mVars(4), 5, iLine, iPos);
			return false;
		}

		bUseNormals = true;
		pNormalList = mVars(5).GetVarListPtr();
	}

	CMVInfo<float> MVInfo;
	COGLVertex xOrigin;
	TMultiV vZero;
	MemObj<COGLVertex> mPoint, mNormal;
	MemObj<COGLColor> mColor;
	TCVScalar dVal;
	bool bIsOrigin;

	xOrigin.Set(0, 0, 0);

	TVarList &PList = *mVars(0).GetVarListPtr();
	int iP, iCount = PList.Count();

	if (pNormalList && iCount != pNormalList->Count())
	{
		m_ErrorList.GeneralError("List of points and list of normals are of different length.", iLine, iPos);
		return false;
	}

	mPoint.Set(iCount);

	if (bUseNormals)
		mNormal.Set(iCount);

	if (bUseCol)
		mColor.Set(iCount);

	for (iP = 0; iP < iCount; iP++)
	{
		bIsOrigin = false;
		if (PList(iP).BaseType() != PDT_MULTIV)
		{
			if (PList(iP).CastToScalar(dVal))
			{
				bIsOrigin = true;
			}
			else
			{
				m_ErrorList.GeneralError("Element in list is not a multivector.", iLine, iPos);
				return false;
			}
		}

		if (bUseNormals)
		{
			if ((*pNormalList)[iP].BaseType() != PDT_MULTIV)
			{
				m_ErrorList.GeneralError("Element in normals list is not a multivector.", iLine, iPos);
				return false;
			}

			/////////////////////////////////////////////////
			// Get Normal
			TMultiV &vN = *((TMultiV *) (*pNormalList)[iP].Val());
			// Only analyze multivector
			m_pFilter->DrawMV(vN, true);
			// Get MVInfo
			MVInfo = m_pFilter->GetMVInfo();

			if (MVInfo.m_eType != GA_POINT && MVInfo.m_eType != GA_DIR1D)
			{
				m_ErrorList.GeneralError("One of the multivectors passed does not represent a point or a direction.", iLine, iPos);
				return false;
			}
			mNormal[iP] = MVInfo.m_mVex[0].Data();
		}


		if (bUseCol)
		{
			if ((*pColList)[iP].BaseType() != PDT_COLOR)
			{
				m_ErrorList.GeneralError("Element in color list is not a color.", iLine, iPos);
				return false;
			}
			
			mColor[iP] = *((COGLColor *) (*pColList)[iP].Val());
		}

		/////////////////////////////////////////////////
		// Get Point
		if (bIsOrigin)
		{
			mPoint[iP].Set(0,0,0);
		}
		else
		{
			TMultiV &vP = *((TMultiV *) PList[iP].Val());
			// Only analyze multivector
			m_pFilter->DrawMV(vP, true);
			// Get MVInfo
			MVInfo = m_pFilter->GetMVInfo();

			if (MVInfo.m_eType == GA_SCALAR)
			{
				mPoint[iP] = xOrigin;
			}
			else if (MVInfo.m_eType == GA_POINT)
			{
				CPointND<float, 3> rVex = MVInfo.m_mVex[0];
				mPoint[iP] = MVInfo.m_mVex[0].Data();
			}
			else
			{
				m_ErrorList.GeneralError("One of the multivectors passed does not represent a point.", iLine, iPos);
				return false;
			}
		}
	}

	rVar.New(PDT_VEXLIST, "PointList");
	TVexList *pVexList = rVar.GetVexListPtr();

	m_pFilter->DrawPointList(mPoint, mNormal, mColor, bNegateNormals, pVexList, bDoDraw);

	return true;
} 




//////////////////////////////////////////////////////////////////////
/// Draw Triangle Function

bool CCLUCodeBase::DrawTriangleFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	int iVar;
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	TCVScalar dVal[9];
	
	if (iVarCount == 9)
	{
		for (iVar = 0; iVar < 9; iVar++)
		{
			if (!mVars(iVar).CastToScalar(dVal[iVar], m_fSensitivity))
			{
				m_ErrorList.InvalidParType(mVars(iVar), 1, iLine, iPos);
				return false;
			}								
		}

		COGLVertex xA, xB, xC;

		xA.Set((float) dVal[0], (float) dVal[1], (float) dVal[2]);
		xB.Set((float) dVal[3], (float) dVal[4], (float) dVal[5]);
		xC.Set((float) dVal[6], (float) dVal[7], (float) dVal[8]);
	
		m_pFilter->DrawTriangle(xA, xB, xC);

		//if (!rVar.New(PDT_VARLIST, "Constant"))
		//{
		//	m_ErrorList.Internal(iLine, iPos);
		//	return false;
		//}		
	} 
	else if (iVarCount == 3)
	{
		if (mVars(0).BaseType() != PDT_MULTIV)
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (mVars(1).BaseType() != PDT_MULTIV)
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		if (mVars(2).BaseType() != PDT_MULTIV)
		{
			m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}

		TMultiV &vA = *mVars(0).GetMultiVPtr();
		TMultiV &vB = *mVars(1).GetMultiVPtr();
		TMultiV &vC = *mVars(2).GetMultiVPtr();
		TMultiV vX, vY, vZ;

		if (!CastMVtoE3(vA, vX))
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}								

		if (!CastMVtoE3(vB, vY))
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}								

		if (!CastMVtoE3(vC, vZ))
		{
			m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}								

		COGLVertex xA, xB, xC;
		xA.Set((float) vX[E3GA<TCVScalar>::iE1], 
			(float) vX[E3GA<TCVScalar>::iE2], 
			(float) vX[E3GA<TCVScalar>::iE3]);

		xB.Set((float) vY[E3GA<TCVScalar>::iE1], 
			(float) vY[E3GA<TCVScalar>::iE2], 
			(float) vY[E3GA<TCVScalar>::iE3]);

		xC.Set((float) vZ[E3GA<TCVScalar>::iE1], 
			(float) vZ[E3GA<TCVScalar>::iE2], 
			(float) vZ[E3GA<TCVScalar>::iE3]);

		m_pFilter->DrawTriangle(xA, xB, xC);
	}
	else
	{
		int piPar[] = {3, 9};

		m_ErrorList.WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}
	
	return true;
} 

//////////////////////////////////////////////////////////////////////
/// Draw Cone Function

bool CCLUCodeBase::DrawConeFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	TCVScalar dVal;
	
	if (iVarCount == 3)
	{
		if (mVars(0).BaseType() != PDT_MULTIV)
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (mVars(1).BaseType() != PDT_MULTIV)
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		if (!mVars(2).CastToScalar(dVal, m_fSensitivity))
		{
			m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}

		TMultiV &vA = *mVars(0).GetMultiVPtr();
		TMultiV &vB = *mVars(1).GetMultiVPtr();
		TMultiV vX, vY;

		if (!CastMVtoE3(vA, vX))
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}								

		if (!CastMVtoE3(vB, vY))
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}								

		COGLVertex xA, xB;
		xA.Set((float) vX[E3GA<TCVScalar>::iE1], 
			(float) vX[E3GA<TCVScalar>::iE2], 
			(float) vX[E3GA<TCVScalar>::iE3]);

		xB.Set((float) vY[E3GA<TCVScalar>::iE1], 
			(float) vY[E3GA<TCVScalar>::iE2], 
			(float) vY[E3GA<TCVScalar>::iE3]);

		m_pFilter->DrawCone(xA, xB, float(dVal));
	}
	else
	{
		m_ErrorList.WrongNoOfParams(3, iLine, iPos);
		return false;
	}
	
	return true;
} 


//////////////////////////////////////////////////////////////////////
/// Draw Sphere Function

bool CCLUCodeBase::DrawSphereFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = mVars.Count();
	TCVScalar dVal;
	
	if (iVarCount == 2)
	{
		if (mVars(0).BaseType() != PDT_MULTIV)
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (!mVars(1).CastToScalar(dVal, m_fSensitivity))
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}		

		TMultiV &vP = *mVars(0).GetMultiVPtr();
		TMultiV vX;

		if (!CastMVtoE3(vP, vX))
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}								

		COGLVertex xP;
		xP.Set((float) vX[E3GA<TCVScalar>::iE1], 
			(float) vX[E3GA<TCVScalar>::iE2], 
			(float) vX[E3GA<TCVScalar>::iE3]);

		// Draw solid sphere
		m_pFilter->DrawSphere(xP, float(dVal), true);
	}
	else
	{
		int piPar[] = {2};

		m_ErrorList.WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}
	
	return true;
} 


//////////////////////////////////////////////////////////////////////
/// Draw Icosahedron Function

bool CCLUCodeBase::DrawIcosahedronFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = mVars.Count();
	TCVScalar dRadius;
	TCVCounter iPower, iSolid = 1;
	
	if (iVarCount != 3 && iVarCount != 4)
	{
		int piPar[] = {3, 4};

		m_ErrorList.WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_MULTIV)
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToScalar(dRadius, m_fSensitivity))
	{
		m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}		

	if (dRadius < 1e-4)
	{
		m_ErrorList.GeneralError("Radius has to be larger than zero.", iLine, iPos);
		return false;
	}

	if (!mVars(2).CastToCounter(iPower))
	{
		m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
		return false;
	}

	if (iPower > 6)
	{	
		m_ErrorList.GeneralError("Evaluation of Icosahedron for a power > 6 will take too long :-(", iLine, iPos);
		return false;
	}

	if (iPower < 0)
	{
		m_ErrorList.GeneralError("A negative power makes no sense here.", iLine, iPos);
		return false;
	}

	if (iVarCount >= 4)
	{
		if (!mVars(3).CastToCounter(iSolid))
		{
			m_ErrorList.InvalidParType(mVars(3), 4, iLine, iPos);
			return false;
		}		
	}

	TMultiV &vP = *mVars(0).GetMultiVPtr();
	TMultiV vX;

	if (!CastMVtoE3(vP, vX))
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}								

	COGLVertex xP;
	xP.Set((float) vX[E3GA<TCVScalar>::iE1], 
		(float) vX[E3GA<TCVScalar>::iE2], 
		(float) vX[E3GA<TCVScalar>::iE3]);

	// Draw Icosahedron
	m_pFilter->DrawIcosahedron(xP, float(dRadius), iPower, (iSolid ? true : false));
	
	return true;
} 



//////////////////////////////////////////////////////////////////////
/// Draw Quantum Fractal Function
///
/// This is an implementation of 'Quantum Jumps, EEQT and the Five Platonic Fractals'
/// by A. Jadczyk and R. Öberg.
///
/// Parameters:
///		- List of detector positions
///		- The value of 'alpha'
///		- The number of points to generate
///		- The start point.
///

bool CCLUCodeBase::DrawQuantumFractalFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();	

	int iVarCount = mVars.Count();
	TCVScalar dVal;
	float dAlpha;
	TCVCounter iPntCnt, iPnt;
	CMVInfo<TCVScalar> Info;

	if (iVarCount != 4)
	{
		int piPar[] = {4};

		m_ErrorList.WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}


	if (mVars(0).BaseType() != PDT_VARLIST)
	{
		m_ErrorList.GeneralError("Expect list of detector positions as first parameter.", iLine, iPos);
		return false;
	}

	if ( !mVars(1).CastToScalar( dVal ) )
	{
		m_ErrorList.GeneralError("Expect value of 'alpha' as second parameter.", iLine, iPos);
		return false;
	}
	dAlpha = float( dVal );

	if ( !mVars(2).CastToCounter( iPntCnt ) )
	{
		m_ErrorList.GeneralError("Expect number of iterations as third parameters.", iLine, iPos);
		return false;
	}

	if (iPntCnt <= 0)
	{
		m_ErrorList.GeneralError("Point count has to be larger than zero.", iLine, iPos);
		return false;
	}

	if ( mVars(3).BaseType() != PDT_MULTIV )
	{
		m_ErrorList.GeneralError("Expect start point as fourth parameter.", iLine, iPos);
		return false;
	}

	rVar.New( PDT_VEXLIST, "Constant");
	TVexList &rVexList = *rVar.GetVexListPtr();
	Mem<COGLVertex> &rVex = rVexList.GetVexList();

	TVarList &rList = *mVars(0).GetVarListPtr();
	int iDetector, iDetectorCount = rList.Count();
	COGLVertex xR;
	Mem<COGLVertex> mxN;

	rVexList.SetMode( GL_POINTS );
	
	if ( !rVex.Set( iPntCnt ) )
	{
		m_ErrorList.GeneralError("Insufficient memory available.", iLine, iPos);
		return false;
	}

	// Analyze Multivector
	if (!m_pFilter->DrawMV( *mVars(3).GetMultiVPtr(), true) )
	{
		m_ErrorList.GeneralError("Start point given is not a point.", iLine, iPos);
		return false;
	}

	m_pFilter->GetMVInfo(Info);

	if (Info.m_eType != GA_POINT)
	{
		m_ErrorList.GeneralError("Start point given is not a point.", iLine, iPos);
		return false;		
	}
	
	CPointND<TCVScalar,3> &rPnt = Info.m_mVex[0];

	xR.Set(float(rPnt[0]), float(rPnt[1]), float(rPnt[2]));

	mxN.Set( iDetectorCount );

	for ( iDetector = 0; iDetector < iDetectorCount; iDetector++ )
	{
		if ( rList(iDetector).BaseType() != PDT_MULTIV )
		{
			m_ErrorList.GeneralError("Element in detector position list is not a multivector.", iLine, iPos);
			return false;
		}

		// Analyze Multivector
		if (!m_pFilter->DrawMV( *rList(iDetector).GetMultiVPtr(), true) )
		{
			m_ErrorList.GeneralError("Multivector in detector position list is not a point.", iLine, iPos);
			return false;
		}

		m_pFilter->GetMVInfo(Info);

		if (Info.m_eType != GA_POINT)
		{
			m_ErrorList.GeneralError("Multivector in detector position list is not a point.", iLine, iPos);
			return false;		
		}

		CPointND<TCVScalar,3> &rPnt = Info.m_mVex[0];

		mxN[ iDetector ].Set(float(rPnt[0]), float(rPnt[1]), float(rPnt[2]));
	}

	///////////////////////////////////////////////////////////////////////

//	COGLVertexList vlIco;
	float dZ;
	float dAlpha2, dOPAlpha2, dOMAlpha2, dProb, dProbNorm, dNdR, dFac;
/*
	m_pFilter->GenVexIcosahedron(vlIco, 1.0f, 5);
	Mem<COGLVertex> &rIcoVex = vlIco.GetVexList();
	Mem<TColor> &rIcoCol = vlIco.GetColList();
	Mem<int> mIcoHist;
	int iIco, iIcoCnt = rIcoVex.Count();
	rIcoCol.Set( iIcoCnt );
	mIcoHist.Set( iIcoCnt );
	for (iIco = 0; iIco < iIcoCnt; iIco++)
	{
		mIcoHist[ iIco ] = 0;
	}
*/

	dAlpha2 = dAlpha * dAlpha;
	dOPAlpha2 = 1 + dAlpha2;
	dOMAlpha2 = 1 - dAlpha2;
	dProbNorm = 1.0f / (float( iDetectorCount ) * dOPAlpha2);

	// Evaluate Points
	for (iPnt = 0; iPnt < iPntCnt; iPnt++)
	{
		rVex[ iPnt ] = xR;

		// Generate Random Number
		dZ = float(m_Random.ran3());

		// Find Detector to use
		dProb = 0.0;
		for( iDetector = 0; iDetector < iDetectorCount; iDetector++ )
		{
			dNdR = xR * mxN[ iDetector ]; // Inner product
			dFac = dOPAlpha2 + 2*dAlpha*dNdR;
			dProb += dFac * dProbNorm;
			if (dZ <= dProb)
				break;
		}

		// iDetector has now the value of the detector chosen
		xR = ( dOMAlpha2 * xR + 2.0f*dAlpha*(1.0f + dAlpha*dNdR) * mxN[ iDetector ] );// / dFac;
		xR.Norm();
/*
		for (iIco = 0; iIco < iIcoCnt; iIco++)
		{
			if ((rIcoVex[ iIco ] - xR).Mag() <= 0.1)
			{
				++mIcoHist[ iIco ];
				//break;
			}
		}
*/
	}
/*
	int iMax = 0;
	float fCol;
	for (iIco = 0; iIco < iIcoCnt; iIco++)
	{
		if (mIcoHist[ iIco ] > iMax)
			iMax = mIcoHist[ iIco ];
	}

	for (iIco = 0; iIco < iIcoCnt; iIco++)
	{
		fCol = float( log(1.0 + double(mIcoHist[ iIco])) / log(1 + double(iMax)) );
		rIcoCol[ iIco ][0] = fCol;
		rIcoCol[ iIco ][2] = 1.0f - fCol;
	}

	vlIco.Apply();
*/
	//rVexList.Apply();

	return true;
}
