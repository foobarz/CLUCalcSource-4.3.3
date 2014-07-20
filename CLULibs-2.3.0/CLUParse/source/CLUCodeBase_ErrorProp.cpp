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



#include "StdAfx.h"
#include "GL/glut.h"

#include "CLUCodeBase.h"



//////////////////////////////////////////////////////////////////////
/// Error Propagating Embedding of Vector in 2d projective conic space
//
// Expects one vector and one covariance Matrix
// or one Mouse Mode ID and cov. mat.
// or three Euclidean coordinates and cov. mat.


bool CCLUCodeBase::ErrorPropVecC2Func(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList *pmVars = rPars.GetVarListPtr();

	int iVarCount = pmVars->Count();
	TCVScalar pfVec[3];
	int iGAID;

	if (iVarCount == 1 && (*pmVars)(0).BaseType() == PDT_VARLIST)
	{
		pmVars = (*pmVars)(0).GetVarListPtr();
		iVarCount = pmVars->Count();
	}

	TVarList& mVars = *pmVars;

	if (iVarCount == 4)
	{
		int i;
		
		for(i=0;i<3;i++)
		{
			if ( !(mVars(i).CastToScalar(pfVec[i], m_fSensitivity)) )
			{
				m_ErrorList.InvalidType(mVars(i), iLine, iPos);
				return false;
			}
		}

		iGAID = ID_E3GA;
	}
	else if (iVarCount == 2)
	{
		int i;
		TCVCounter iVal;
		
		if (mVars(0).CastToCounter(iVal))
		{
			if (iVal < 0 || iVal > 9)
			{
				cStr csText;
				csText = iVal;
				m_ErrorList.InvalidParVal(csText.Str(), 1, iLine, iPos);
				return false;
			}

			for(i=0;i<3;i++)
				pfVec[i] = (TCVScalar) (*m_pTransform)[iVal].pfTrans[i];
	
			iGAID = ID_E3GA;
		}
		else if (mVars(0).BaseType() == PDT_MULTIV)
		{
			TMultiV &vA = *mVars(0).GetMultiVPtr();
			iGAID = int(vA.GetBase().BaseID());

			if (iGAID == ID_E3GA)
			{
				pfVec[0] = vA[m_E3GABase.iE1];
				pfVec[1] = vA[m_E3GABase.iE2];
				pfVec[2] = vA[m_E3GABase.iE3];
			}
			else if (iGAID == ID_PGA)
			{
				m_ErrorList.GeneralError(
					"Embedding of projective vector in conic space "
					"with error propagation not implemented, yet.", iLine, iPos);
				return false;
			}
			else if (iGAID == ID_CONFGA)
			{
				m_ErrorList.GeneralError(
					"Embedding of conformal vector in conic space "
					"with error propagation not implemented, yet.", iLine, iPos);
				return false;
			}
			else
			{
				m_ErrorList.GeneralError(
					"Multivector is of unknown type.", iLine, iPos);
				return false;
			}
		}
		else
		{
			m_ErrorList.InvalidType(mVars(0), iLine, iPos);
			return false;
		}	
	}
	else
	{
		int piVal[] = { 2, 4 };
		m_ErrorList.WrongNoOfParams(piVal, 2, iLine, iPos);
		return false;
	}


	if (mVars(iVarCount-1).BaseType() != PDT_MATRIX)
	{
		m_ErrorList.InvalidType(mVars(iVarCount-1), iLine, iPos);
		return false;			
	}

	TMatrix &CovMat = *mVars(iVarCount-1).GetMatrixPtr();

	TMultiV vX;
	TMatrix xX, xM, xC;
	TCVScalar dSqrt2 = TCVScalar(sqrt(2.0));

	if (iGAID == ID_E3GA)
	{
		if (CovMat.Rows() != 8 || CovMat.Cols() != 8)
		{
			char pcText[300];
			sprintf(pcText, "Covariance matrix has wrong dimensions %dx%d. Expect 8x8 matrix.",
				CovMat.Rows(), CovMat.Cols());
			m_ErrorList.GeneralError(pcText, iLine, iPos);
			return false;
		}
		
		xX.Resize(8, 1);
		xC.Resize(64, 64);

		int iE[3];
		iE[0] = m_E3GABase.iE1 + 1;
		iE[1] = m_E3GABase.iE2 + 1;
		iE[2] = m_E3GABase.iE3 + 1;

		int iNE[6];
		iNE[0] = m_El2GABase.iE1 + 1;
		iNE[1] = m_El2GABase.iE2 + 1;
		iNE[2] = m_El2GABase.iE3 + 1;
		iNE[3] = m_El2GABase.iE4 + 1;
		iNE[4] = m_El2GABase.iE5 + 1;
		iNE[5] = m_El2GABase.iE6 + 1;

		// Embed Vector
		vX = m_El2GABase.Vec(pfVec[0], pfVec[1]);
		vX[m_El2GABase.iE4] += CovMat(iE[0], iE[0]) / dSqrt2;
		vX[m_El2GABase.iE5] += CovMat(iE[1], iE[1]) / dSqrt2;
		vX[m_El2GABase.iE6] += CovMat(iE[0], iE[1]);

		// Evaluate Conic Matrix in conformal space of vector.
		TCVScalar dS11, dS22, dS12, dX1, dX2;

		dS11 = CovMat(iE[0], iE[0]);
		dS22 = CovMat(iE[1], iE[1]);
		dS12 = CovMat(iE[0], iE[1]);
		dX1 = pfVec[0];
		dX2 = pfVec[1];

		xC = TCVScalar(0);
		xC(iNE[0], iNE[0]) = dS11;
		xC(iNE[0], iNE[1]) = dS12;
		xC(iNE[1], iNE[0]) = dS12;
		xC(iNE[1], iNE[1]) = dS22;
		
		xC(iNE[0], iNE[3]) = xC(iNE[3], iNE[0]) = dSqrt2 * dX1 * dS11;
		xC(iNE[0], iNE[4]) = xC(iNE[4], iNE[0]) = dSqrt2 * dX2 * dS12;
		xC(iNE[0], iNE[5]) = xC(iNE[5], iNE[0]) = dX2 * dS11 + dX1 * dS12;

		xC(iNE[1], iNE[3]) = xC(iNE[3], iNE[1]) = dSqrt2 * dX1 * dS12;
		xC(iNE[1], iNE[4]) = xC(iNE[4], iNE[1]) = dSqrt2 * dX2 * dS22;
		xC(iNE[1], iNE[5]) = xC(iNE[5], iNE[1]) = dX2 * dS12 + dX1 * dS22;

		xC(iNE[3], iNE[3]) = 2.0 * dX1 * dX1 * dS11;
		xC(iNE[3], iNE[4]) = xC(iNE[4], iNE[3]) = 2.0 * dX1 * dX2 * dS12;
		xC(iNE[3], iNE[5]) = xC(iNE[5], iNE[3]) = dSqrt2 * (dX1 * dX2 * dS11 + dX1 * dX1 * dS12);

		xC(iNE[4], iNE[4]) = 2.0 * dX2 * dX2 * dS22;
		xC(iNE[4], iNE[5]) = xC(iNE[5], iNE[4]) = dSqrt2 * (dX2 * dX2 * dS12 + dX1 * dX2 * dS22);

		xC(iNE[5], iNE[5]) = dX1 * dX1 * dS22 + dX2 * dX2 * dS11 + 2.0 * dX1 * dX2 * dS12;
	}
	else
	{
		m_ErrorList.GeneralError("Invalid multivector space.", iLine, iPos);
		return false;
	}

	TVarList ResList;

	ResList.Set(2);
	ResList[0] = vX;
	ResList[1] = xC;

	rVar = ResList;

	return true;
}




//////////////////////////////////////////////////////////////////////
/// Error Propagating Embedding of Vector in Conformal space
//
// Expects one vector and one covariance Matrix
// or one Mouse Mode ID and cov. mat.
// or three Euclidean coordinates and cov. mat.


bool CCLUCodeBase::ErrorPropVecN3Func(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList *pmVars = rPars.GetVarListPtr();

	int iVarCount = pmVars->Count();
	TCVScalar pfVec[3];
	int iGAID;

	if (iVarCount == 1 && (*pmVars)(0).BaseType() == PDT_VARLIST)
	{
		pmVars = (*pmVars)(0).GetVarListPtr();
		iVarCount = pmVars->Count();
	}

	TVarList& mVars = *pmVars;

	if (iVarCount == 4)
	{
		int i;
		
		for(i=0;i<3;i++)
		{
			if ( !(mVars(i).CastToScalar(pfVec[i], m_fSensitivity)) )
			{
				m_ErrorList.InvalidType(mVars(i), iLine, iPos);
				return false;
			}
		}

		iGAID = ID_E3GA;
	}
	else if (iVarCount == 2)
	{
		int i;
		TCVCounter iVal;
		
		if (mVars(0).CastToCounter(iVal))
		{
			if (iVal < 0 || iVal > 9)
			{
				cStr csText;
				csText = iVal;
				m_ErrorList.InvalidParVal(csText.Str(), 1, iLine, iPos);
				return false;
			}

			for(i=0;i<3;i++)
				pfVec[i] = (TCVScalar) (*m_pTransform)[iVal].pfTrans[i];
	
			iGAID = ID_E3GA;
		}
		else if (mVars(0).BaseType() == PDT_MULTIV)
		{
			TMultiV &vA = *mVars(0).GetMultiVPtr();
			iGAID = int(vA.GetBase().BaseID());

			if (iGAID == ID_E3GA)
			{
				pfVec[0] = vA[m_E3GABase.iE1];
				pfVec[1] = vA[m_E3GABase.iE2];
				pfVec[2] = vA[m_E3GABase.iE3];
			}
			else if (iGAID == ID_PGA)
			{
				m_ErrorList.GeneralError(
					"Embedding of projective vector in conformal space "
					"with error propagation not implemented, yet.", iLine, iPos);
				return false;
			}
			else if (iGAID == ID_CONFGA)
			{
				m_ErrorList.GeneralError(
					"Embedding of conformal vector in conformal space "
					"with error propagation not implemented, yet.", iLine, iPos);
				return false;
			}
			else
			{
				m_ErrorList.GeneralError(
					"Multivector is of unknown type.", iLine, iPos);
				return false;
			}
		}
		else
		{
			m_ErrorList.InvalidType(mVars(0), iLine, iPos);
			return false;
		}	
	}
	else
	{
		int piVal[] = { 2, 4 };
		m_ErrorList.WrongNoOfParams(piVal, 2, iLine, iPos);
		return false;
	}


	if (mVars(iVarCount-1).BaseType() != PDT_MATRIX)
	{
		m_ErrorList.InvalidType(mVars(iVarCount-1), iLine, iPos);
		return false;			
	}

	TMatrix &CovMat = *mVars(iVarCount-1).GetMatrixPtr();

	int i, iRow, iCol;
	TMultiV vX;
	TMatrix xX, xM, xC;

	if (iGAID == ID_E3GA)
	{
		if (CovMat.Rows() != 8 || CovMat.Cols() != 8)
		{
			char pcText[300];
			sprintf(pcText, "Covariance matrix has wrong dimensions %dx%d. Expect 8x8 matrix.",
				CovMat.Rows(), CovMat.Cols());
			m_ErrorList.GeneralError(pcText, iLine, iPos);
			return false;
		}
		
		xX.Resize(8, 1);
		xC.Resize(32, 32);

		int iE[3];
		iE[0] = m_E3GABase.iE1 + 1;
		iE[1] = m_E3GABase.iE2 + 1;
		iE[2] = m_E3GABase.iE3 + 1;

		int iNE[5];
		iNE[0] = m_N3GABase.iE1 + 1;
		iNE[1] = m_N3GABase.iE2 + 1;
		iNE[2] = m_N3GABase.iE3 + 1;
		iNE[3] = m_N3GABase.iEp + 1;
		iNE[4] = m_N3GABase.iEm + 1;

		// Embed Vector
		vX = m_N3GABase.NullVec(pfVec[0], pfVec[1], pfVec[2]);
		vX += ( TCVScalar(0.5) * 
			  (CovMat(iE[0],iE[0]) + CovMat(iE[1],iE[1]) + CovMat(iE[2],iE[2])) ) & 
			  m_N3GABase.vE;

		// Evaluate Covariance Matrix in conformal space of vector.
		xX = TCVScalar(0);

		for(i = 0; i < 3; i++)
			xX(iE[i], 1) = pfVec[i];

		xC = TCVScalar(0);
		for( iRow = 0; iRow < 3; iRow++ )
		{
			for( iCol = 0; iCol < 3; iCol++ )
			{
				xC(iNE[iRow],iNE[iCol]) = CovMat(iE[iRow], iE[iCol]);
			}
		}

		xM = CovMat * xX;
		for( iRow = 0; iRow < 3; iRow++ )
		{
			// Write column vector into column for e+ and e- since
			// it should be written in column for e.
			xC(iNE[iRow], iNE[3]) = xC(iNE[iRow], iNE[4]) = 
			xC(iNE[3], iNE[iRow]) = xC(iNE[4], iNE[iRow]) = xM(iE[iRow], 1);
		}

		xM = (~xX) * xM;
		xC(iNE[3], iNE[4]) = xC(iNE[4], iNE[3]) = 
		xC(iNE[3], iNE[3]) = xC(iNE[4], iNE[4]) = xM(1,1);

	}
	else
	{
		m_ErrorList.GeneralError("Invalid multivector space.", iLine, iPos);
		return false;
	}

	TVarList ResList;

	ResList.Set(2);
	ResList[0] = vX;
	ResList[1] = xC;

	rVar = ResList;

	return true;
}



//////////////////////////////////////////////////////////////////////
/// Error Propagating Embedding of Cross-Covariance of two Vectors in Conformal space
//
// Expects two vectors and one cross-covariance Matrix
// or two Mouse Mode IDs and cross.-cov. mat.
// or six Euclidean coordinates and cross.-cov. mat.


bool CCLUCodeBase::ErrorPropCrossCovVecN3Func(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	TCVScalar pfVec[6];
	int iGAID;

	if (iVarCount == 7)
	{
		int i;
		
		for(i=0;i<6;i++)
		{
			if ( !(mVars(i).CastToScalar(pfVec[i], m_fSensitivity)) )
			{
				m_ErrorList.InvalidType(mVars(i), iLine, iPos);
				return false;
			}
		}

		iGAID = ID_E3GA;
	}
	else if (iVarCount == 3)
	{
		int i, iVar;
		TCVCounter iVal;
		
		for( iVar = 0; iVar < 2; iVar++ )
		{
			if (mVars(iVar).CastToCounter(iVal))
			{
				if (iVal < 0 || iVal > 9)
				{
					cStr csText;
					csText = iVal;
					m_ErrorList.InvalidParVal(csText.Str(), iVar+1, iLine, iPos);
					return false;
				}

				for(i=0;i<3;i++)
					pfVec[3*iVar + i] = (TCVScalar) (*m_pTransform)[iVal].pfTrans[i];

				iGAID = ID_E3GA;
			}
			else if (mVars(iVar).BaseType() == PDT_MULTIV)
			{
				TMultiV &vA = *mVars(iVar).GetMultiVPtr();
				iGAID = int(vA.GetBase().BaseID());

				if (iGAID == ID_E3GA)
				{
					pfVec[3*iVar + 0] = vA[m_E3GABase.iE1];
					pfVec[3*iVar + 1] = vA[m_E3GABase.iE2];
					pfVec[3*iVar + 2] = vA[m_E3GABase.iE3];
				}
				else if (iGAID == ID_PGA)
				{
					m_ErrorList.GeneralError(
						"Embedding of projective vector in conformal space "
						"with error propagation not implemented, yet.", iLine, iPos);
					return false;
				}
				else if (iGAID == ID_CONFGA)
				{
					m_ErrorList.GeneralError(
						"Embedding of conformal vector in conformal space "
						"with error propagation not implemented, yet.", iLine, iPos);
					return false;
				}
				else
				{
					m_ErrorList.GeneralError(
						"Multivector is of unknown type.", iLine, iPos);
					return false;
				}
			}
			else
			{
				m_ErrorList.InvalidType(mVars(iVar), iLine, iPos);
				return false;
			}	
		}
	}
	else
	{
		int piVal[] = { 3, 7 };
		m_ErrorList.WrongNoOfParams(piVal, 2, iLine, iPos);
		return false;
	}


	if (mVars(iVarCount-1).BaseType() != PDT_MATRIX)
	{
		m_ErrorList.InvalidType(mVars(iVarCount-1), iLine, iPos);
		return false;			
	}

	TMatrix &CovMat = *mVars(iVarCount-1).GetMatrixPtr();

	int i, iRow, iCol;
	TMatrix xX, xY, xM, xC;

	if (iGAID == ID_E3GA)
	{
		if (CovMat.Rows() != 8 || CovMat.Cols() != 8)
		{
			char pcText[300];
			sprintf(pcText, "Covariance matrix has wrong dimensions %dx%d. Expect 8x8 matrix.",
				CovMat.Rows(), CovMat.Cols());
			m_ErrorList.GeneralError(pcText, iLine, iPos);
			return false;
		}
		
		xX.Resize(8, 1);
		xY.Resize(8, 1);
		xC.Resize(32, 32);

		int iE[3];
		iE[0] = m_E3GABase.iE1 + 1;
		iE[1] = m_E3GABase.iE2 + 1;
		iE[2] = m_E3GABase.iE3 + 1;

		int iNE[5];
		iNE[0] = m_N3GABase.iE1 + 1;
		iNE[1] = m_N3GABase.iE2 + 1;
		iNE[2] = m_N3GABase.iE3 + 1;
		iNE[3] = m_N3GABase.iEp + 1;
		iNE[4] = m_N3GABase.iEm + 1;

		// Evaluate Covariance Matrix in conformal space of vector.
		xX = TCVScalar(0);
		xY = TCVScalar(0);

		for(i = 0; i < 3; i++)
		{
			xX(iE[i], 1) = pfVec[i];
			xY(iE[i], 1) = pfVec[3 + i];
		}

		xC = TCVScalar(0);
		for( iRow = 0; iRow < 3; iRow++ )
		{
			for( iCol = 0; iCol < 3; iCol++ )
			{
				xC(iNE[iRow],iNE[iCol]) = CovMat(iE[iRow], iE[iCol]);
			}
		}

		xM = CovMat * xY;
		for( iRow = 0; iRow < 3; iRow++ )
		{
			// Write column vector into column for e+ and e- since
			// it should be written in column for e.
			xC(iNE[iRow], iNE[3]) = xC(iNE[iRow], iNE[4]) = xM(iE[iRow], 1);
		}

		xM = (~xX) * CovMat;
		for( iCol = 0; iCol < 3; iCol++ )
		{
			// Write row vector into row for e+ and e- since
			// it should be written in column for e.
			xC(iNE[3], iNE[iCol]) = xC(iNE[4], iNE[iCol]) = xM(1, iE[iCol]);
		}

		xM = xM * xY;
		xC(iNE[3], iNE[4]) = xC(iNE[4], iNE[3]) = 
		xC(iNE[3], iNE[3]) = xC(iNE[4], iNE[4]) = xM(1,1);

	}
	else
	{
		m_ErrorList.GeneralError("Invalid multivector space.", iLine, iPos);
		return false;
	}

	rVar = xC;

	return true;
}




//////////////////////////////////////////////////////////////////////
/// Error Propagating Operation between two Multivectors
///
/// Expects Prod.Type, (mean, covar), (mean, covar), [cross-covar], [(Cxz, Cyz, Cxw, Cyw, etc.)]

bool CCLUCodeBase::ErrorPropagationOperationFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	int i;
	
	if (iVarCount < 3 || iVarCount > 5)
	{
		int piPar[] = {3, 4, 5};
		m_ErrorList.WrongNoOfParams(piPar, 3, iLine, iPos);
		return false;
	}

	int iOpType = 0;
	Mem<int> mMask;

	if (!mVars(0).CastToCounter(iOpType))
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (iOpType < int(MVOP_GEO) || iOpType > int(MVOP_SUB))
	{
		m_ErrorList.InvalidParVal(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_VARLIST)
	{
		m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}

	if (mVars(2).BaseType() != PDT_VARLIST)
	{
		m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
		return false;
	}

	if (iVarCount >= 4 && mVars(3).BaseType() != PDT_MATRIX)
	{
		m_ErrorList.InvalidParType(mVars(3), 4, iLine, iPos);
		return false;
	}

	// Get Lists of Multivector and Covariance Matrix
	TVarList& mListA = *mVars(1).GetVarListPtr();
	TVarList& mListB = *mVars(2).GetVarListPtr();

	if (mListA.Count() != 2 ||
		mListA(0).BaseType() != PDT_MULTIV ||
		mListA(1).BaseType() != PDT_MATRIX)
	{
		m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}

	if (mListB.Count() != 2 ||
		mListB(0).BaseType() != PDT_MULTIV ||
		mListB(1).BaseType() != PDT_MATRIX)
	{
		m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
		return false;
	}

	// Get Multivectors
	TMultiV &vA = *mListA(0).GetMultiVPtr();
	TMultiV &vB = *mListB(0).GetMultiVPtr();

	if (vA.GetBase().BaseID() != vB.GetBase().BaseID())
	{
		m_ErrorList.GeneralError("Multivectors are from different spaces.", iLine, iPos);
		return false;
	}

	int iGADim = int(vA.GetGADim());

	TMatrix &Ca = *mListA(1).GetMatrixPtr();
	if (int(Ca.Rows()) != iGADim || int(Ca.Cols()) != iGADim)
	{
		char pcText[300];
		sprintf(pcText, "Dimensions of matrix in parameter two are wrong. Expect %dx%d matrices.",
						iGADim, iGADim);
		m_ErrorList.GeneralError(pcText, iLine, iPos);
		return false;
	}

	TMatrix &Cb = *mListB(1).GetMatrixPtr();
	if (int(Cb.Rows()) != iGADim || int(Cb.Cols()) != iGADim)
	{
		char pcText[300];
		sprintf(pcText, "Dimensions of matrix in parameter three are wrong. Expect %dx%d matrices.",
						iGADim, iGADim);
		m_ErrorList.GeneralError(pcText, iLine, iPos);
		return false;
	}

	TMatrix *pCab, CabNull;
	if (iVarCount >= 4)
	{
		pCab = mVars(3).GetMatrixPtr();
		if (int(pCab->Rows()) != iGADim || int(pCab->Cols()) != iGADim)
		{
			char pcText[300];
			sprintf(pcText, "Dimensions of cross-covariance matrix in parameter four are wrong. Expect %dx%d matrices.",
				iGADim, iGADim);
			m_ErrorList.GeneralError(pcText, iLine, iPos);
			return false;
		}
	}
	else
	{
		CabNull.Resize(iGADim, iGADim);
		CabNull = TCVScalar(0);
		pCab = &CabNull;
	}

	// Cross-Covariance Matrix of MVs A and B.
	TMatrix &Cab = *pCab;

	// Result Multivector
	TMultiV vC;

	// Result Covariance Matrix
	TMatrix Cuu;

	// Evaluate Mean of MV product
	if (!EvalEPMeanProd(vC, EMVOpType(iOpType), vA, vB, Cab))
	{
		m_ErrorList.GeneralError("Error evaluating mean of product of multivectors.", iLine, iPos);
		return false;
	}

	// Evaluate Covariance Matrix of Result
	if (!EvalEPCovMatProd(Cuu, EMVOpType(iOpType), vA, vB, Ca, Cb, Cab))
	{
		m_ErrorList.GeneralError("Error evaluating covariance matrix of product of multivectors.", iLine, iPos);
		return false;
	}

	// Create Code Variable List which is returned
	TVarList ResList, List;

	List.Set(2);
	List[0] = vC;
	List[1] = Cuu;

	ResList.Set(1);
	ResList[0] = List;

	// Cross-Covariance Matrix List
	TVarList CrossCovList;

	if (iVarCount == 5)
	{
		int iCCCount;
		TMatrix Cuz;

		// Get Cross-Covariance Matrix List
		TVarList &rCCovMatList = *mVars(4).GetVarListPtr();

		if ((iCCCount = rCCovMatList.Count()) % 2 != 0)
		{
			m_ErrorList.GeneralError("Not an even number of cross-covariance matrices given.", iLine, iPos);
			return false;
		}

		CrossCovList.Set(iCCCount / 2);

		for ( i = 0; i < iCCCount; i += 2 )
		{
			if (rCCovMatList(i).BaseType() != PDT_MATRIX)
			{
				m_ErrorList.GeneralError("Expect list of cross-covariance matrices as fifth parameter.", iLine, iPos);
				return false;
			}

			TMatrix &Cxz = *rCCovMatList(i).GetMatrixPtr();
			if (int(Cxz.Rows()) != iGADim || int(Cxz.Cols()) != iGADim)
			{
				char pcText[300];
				sprintf(pcText, "Dimensions of matrix %d in cross-covariance list are wrong. Expect %dx%d matrices.",
					i+1, iGADim, iGADim);
				m_ErrorList.GeneralError(pcText, iLine, iPos);
				return false;
			}

			TMatrix &Cyz = *rCCovMatList(i+1).GetMatrixPtr();
			if (int(Cyz.Rows()) != iGADim || int(Cyz.Cols()) != iGADim)
			{
				char pcText[300];
				sprintf(pcText, "Dimensions of matrix %d in cross-covariance list are wrong. Expect %dx%d matrices.",
					i+2, iGADim, iGADim);
				m_ErrorList.GeneralError(pcText, iLine, iPos);
				return false;
			}

			if (!EvalEPCrossCovMatProd(Cuz, EMVOpType(iOpType), vA, vB, Cxz, Cyz))
			{
				m_ErrorList.GeneralError("Error evaluating cross-covariance matrix of product of multivectors.", iLine, iPos);
				return false;
			}

			CrossCovList[i/2] = Cuz;
		}

		ResList.Add(1);
		ResList.Last() = CrossCovList;
	}

	rVar = ResList;

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Error Propagating Evaluation of square root of scalar
//
// Expects mean value of square of value and corresponding variance.
// Returns initial value squared and initial variance, however, not necessarily
// in this order. This function uses particular approximation which cannot
// distinguish between mean value part and variance.

bool CCLUCodeBase::ErrorPropSQRTFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList &mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	TCVScalar pfVec[2], fVal1, fVal2, fVal;

	if (iVarCount != 1 && iVarCount != 2)
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_VARLIST)
	{
		m_ErrorList.InvalidType(mVars(0), iLine, iPos);
		return false;		
	}

	TVarList& mList = *mVars(0).GetVarListPtr();
	int i;

	for(i=0;i<2;i++)
	{
		if ( !(mList(i).CastToScalar(pfVec[i], m_fSensitivity)) )
		{
			m_ErrorList.InvalidType(mVars(0), iLine, iPos);
			return false;
		}
	}

	if ((fVal = pfVec[0]*pfVec[0] - pfVec[1]) < TCVScalar(0))
	{
		m_ErrorList.GeneralError("The given parameters have invalid values.", iLine, iPos);
		return false;
	}

	fVal1 = TCVScalar(0.5) * (pfVec[0] + TCVScalar(sqrt(fVal)));
	fVal2 = TCVScalar(0.5) * (pfVec[0] - TCVScalar(sqrt(fVal)));

	TVarList ResList, List1, List2;

	List1.Set(2);
	List1[0] = sqrt(fVal1);
	List1[1] = fVal2;

/*
	List2.Set(2);
	List2[0] = sqrt(fVal2);
	List2[1] = fVal1;
*/

	ResList.Set(1);
	ResList[0] = List1;

	// Cross covariance matrices given
	// matrices may be of any dimension
	if (iVarCount == 2)
	{
		fVal = TCVScalar(0.5) / TCVScalar(sqrt(pfVec[0]));

		int iCol, iColNo, iMat, iMatNo;	
		TCVScalar fCov;

		TVarList ResMList;
		TVarList &MList = *mVars(1).GetVarListPtr();
		iMatNo = MList.Count();

		ResMList.Set(iMatNo);

		for (iMat = 0; iMat < iMatNo; iMat++)
		{
			if (MList(iMat).BaseType() == PDT_MATRIX)
			{
				TMatrix &xA = *MList(iMat).GetMatrixPtr();
				TMatrix xB;

				xB.Resize(xA.Rows(), xA.Cols());
				xB = TCVScalar(0);

				iColNo = xA.Cols();
				for(iCol = 1; iCol <= iColNo; iCol++)
				{
					xB(1,iCol) = fVal * xA(1,iCol);
				}

				ResMList[iMat] = xB;
			}
			else if (MList[iMat].CastToScalar(fCov, m_fSensitivity))
			{
				ResMList[iMat] = fVal * fCov;
			}
			else
			{
				m_ErrorList.InvalidType(mVars(1), iLine, iPos);
				return false;		
			}
		}

		ResList.Add(1);
		ResList.Last() = ResMList;
	}


	rVar = ResList;

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Error Propagating Evaluation of inverse of scalar
//
// Expects mean value of value and corresponding variance.
// Returns inverse value and corresponding variance.

bool CCLUCodeBase::ErrorPropINVFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList &mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	TCVScalar pfVec[2], fVal1, fVal2, fVal;

	if (iVarCount < 1 || iVarCount > 2)
	{
		int piPars[] = { 1, 2 };
		m_ErrorList.WrongNoOfParams(piPars, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_VARLIST)
	{
		m_ErrorList.InvalidType(mVars(0), iLine, iPos);
		return false;		
	}

	TVarList& mList = *mVars(0).GetVarListPtr();
	int i;

	for(i=0;i<2;i++)
	{
		if ( !(mList(i).CastToScalar(pfVec[i], m_fSensitivity)) )
		{
			m_ErrorList.InvalidType(mVars(0), iLine, iPos);
			return false;
		}
	}

	if (pfVec[0] == TCVScalar(0))
	{
		m_ErrorList.GeneralError("The given parameters have invalid values.", iLine, iPos);
		return false;
	}

	fVal = TCVScalar(1) / pfVec[0];

	fVal1 = fVal + pow(fVal, 3) * pfVec[1];
	fVal2 = pow(fVal, 4) * pfVec[1];

	TVarList ResList, PList;
	
	PList.Set(2);
	PList[0] = fVal1;
	PList[1] = fVal2;

	ResList.Set(1);
	ResList[0] = PList;

	if (iVarCount == 2)
	{
		mVars(1).ConvertSEtoSEL();

		TVarList& mList = *mVars(1).GetVarListPtr();
		int i, iCol, iColNo, iCount = mList.Count();

		TVarList CList;
		CList.Set(iCount);
		fVal1 = -fVal*fVal;

		for(i=0;i<iCount;i++)
		{
			if (mList(i).CastToScalar(fVal2, m_fSensitivity))
			{
				CList[i] = fVal1 * fVal2;
			}
			else if (mList(i).BaseType() == PDT_MATRIX)
			{
				TMatrix &xA = *mList(i).GetMatrixPtr();
				TMatrix xB;

				xB.Resize(xA.Rows(), xA.Cols());
				xB = TCVScalar(0);

				iColNo = xA.Cols();
				for(iCol = 1; iCol <= iColNo; iCol++)
				{
					xB(1,iCol) = fVal * xA(1,iCol);
				}

				CList[i] = xB;
			}
			else
			{
				m_ErrorList.InvalidType(mVars(1), iLine, iPos);
				return false;
			}
		}

		ResList.Add(1);
		ResList.Last() = CList;
	}

	rVar = ResList;

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Error Propagating Evaluation of product of two scalars
//
// Expects mean value and corresponding variance for both values as two lists.
// Third parameter gives optionally the cross-covariance.
// Returns mean value of product and corresponding variance.

bool CCLUCodeBase::ErrorPropMULFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList &mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	TCVScalar pfVec[2][2], fCC, fVal1, fVal2;

	if (iVarCount < 2 || iVarCount > 4)
	{
		int piVars[] = { 2, 3, 4 };
		m_ErrorList.WrongNoOfParams(piVars, 3, iLine, iPos);
		return false;
	}

	int i, j;

	for(i=0;i<2;i++)
	{
		if (mVars(i).BaseType() != PDT_VARLIST)
		{
			m_ErrorList.InvalidType(mVars(i), iLine, iPos);
			return false;
		}

		TVarList& mList = *mVars(i).GetVarListPtr();
		if (mList.Count() != 2)
		{
			m_ErrorList.GeneralError("List is not of length two.", iLine, iPos);
			return false;
		}

		for (j = 0; j < 2; j++)
		{
			if ( !(mList(j).CastToScalar(pfVec[i][j], m_fSensitivity)) )
			{
				m_ErrorList.InvalidType(mVars(i), iLine, iPos);
				return false;
			}
		}
	}

	// Is cross-covariance given?
	if (iVarCount == 3)
	{
		if ( !(mVars(2).CastToScalar(fCC, m_fSensitivity)) )
		{
			m_ErrorList.InvalidType(mVars(2), iLine, iPos);
			return false;
		}
	}
	else
	{
		fCC = TCVScalar(0);
	}

	fVal1 = pfVec[0][0] * pfVec[1][0] + fCC;
	fVal2 = pfVec[1][0] * pfVec[1][0] * pfVec[0][1] + 
			pfVec[0][0] * pfVec[0][0] * pfVec[1][1] + 
			TCVScalar(2) * pfVec[0][0] * pfVec[1][0] * fCC;

	TVarList ResList, List;

	List.Set(2);
	List[0] = fVal1;
	List[1] = fVal2;

	ResList.Set(1);
	ResList[0] = List;

	// Check whether cross.cov.mat. pairs are given.
	if (iVarCount == 4)
	{
		if (mVars(3).BaseType() != PDT_VARLIST)
		{
			m_ErrorList.InvalidType(mVars(3), iLine, iPos);
			return false;		
		}		

		TVarList& mList = *mVars(3).GetVarListPtr();
		int i, iCount = mList.Count();

		if (iCount%2 != 0)
		{
			m_ErrorList.GeneralError("Expect even number of cross-covariance matrices.", iLine, iPos);
			return false;
		}

		TVarList CList;
		CList.Set(iCount/2);

		for(i = 0;i < iCount;i += 2)
		{
			if (mList(i).CastToScalar(fVal1, m_fSensitivity) &&
				mList(i+1).CastToScalar(fVal2, m_fSensitivity))
			{
				CList[i/2] = pfVec[1][0] * fVal1 + pfVec[0][0] * fVal2;
			}
			else if (mList(i).BaseType() == PDT_MATRIX &&
					 mList(i+1).BaseType() == PDT_MATRIX)
			{
				TMatrix &Cxz = *mList(i).GetMatrixPtr();
				TMatrix &Cyz = *mList(i+1).GetMatrixPtr();
				TMatrix Cuz;

				CList[i/2] = pfVec[1][0] * Cxz + pfVec[0][0] * Cyz;
			}
			else
			{
				m_ErrorList.InvalidType(mVars(3), iLine, iPos);
				return false;
			}
		}

		ResList.Add(1);
		ResList.Last() = CList;
	}

	rVar = ResList;

	return true;
}



//////////////////////////////////////////////////////////////////////
/// Error Propagating Evaluation of projection of Affine bivector
/// in N3 of the form A^e into E3.
//
// Expects mean bivector and corresponding covariance matrix.
// Third parameter may also be a list of ....
// Returns E3 vector with covariance matrix

bool CCLUCodeBase::ErrorPropPrjVecN3ToE3Func(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	int i;
	TVarList &mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();

	if (iVarCount < 1 || iVarCount > 2)
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_VARLIST)
	{
		m_ErrorList.InvalidType(mVars(0), iLine, iPos);
		return false;		
	}

	TVarList& mList = *mVars(0).GetVarListPtr();

	if (mList(0).BaseType() != PDT_MULTIV || mList(1).BaseType() != PDT_MATRIX)
	{
		m_ErrorList.InvalidType(mVars(0), iLine, iPos);
		return false;
	}

	TMultiV &vA = *mList(0).GetMultiVPtr();
	TMatrix &CAA = *mList(1).GetMatrixPtr();

	if (vA.GetBase().BaseID() != ID_CONFGA)
	{
		m_ErrorList.GeneralError("Expect multivector from conformal space.", iLine, iPos);
		return false;
	}

	int iGADim = int(vA.GetGADim());

	if (CAA.Rows() != iGADim || CAA.Cols() != iGADim)
	{
		char pcText[300];
		sprintf(pcText, "Covariance matrix is of wrong dimension. Expect %dx%d matrix.", 
						iGADim, iGADim);
		m_ErrorList.GeneralError(pcText, iLine, iPos);
		return false;
	}

	MemObj<TMatrix> mCCList;
	if (iVarCount == 2)
	{
		mVars(1).ConvertSEtoSEL();

		TVarList &MatList = *mVars(1).GetVarListPtr();
		int iMat, iMatCount = MatList.Count();
		
		mCCList.Set(iMatCount);
		for (iMat = 0; iMat < iMatCount; iMat++)
		{
			if (MatList(iMat).BaseType() != PDT_MATRIX)
			{
				m_ErrorList.GeneralError("Expect list of cross covariance matrices.", iLine, iPos);
				return false;
			}

			TMatrix &rMat = *MatList(iMat).GetMatrixPtr();

			if (rMat.Rows() != iGADim || rMat.Cols() != iGADim)
			{
				char pcText[300];
				sprintf(pcText, "Covariance matrix is of wrong dimension. Expect %dx%d matrix.", 
					iGADim, iGADim);
				m_ErrorList.GeneralError(pcText, iLine, iPos);
				return false;
			}

			mCCList[iMat] = rMat;
		}
	}


	TMultiV vE0 = -m_N3GABase.vE0;
	TMatrix CNull(iGADim, iGADim);
	CNull = TCVScalar(0);
	
	// Result Multivector andcovariance matrix
	TMultiV vB;
	TMatrix CBB;

	// Evaluate inner product of bivector with -e0
	if (!EvalEPMeanProd(vB, MVOP_INNER, vA, vE0, CNull))
	{
		m_ErrorList.GeneralError("Error evaluating error propagating product of multivector with e0.",
									iLine, iPos);
		return false;
	}

	// Evaluate cov. mat. of inner product of bivector with -e0
	if (!EvalEPCovMatProd(CBB, MVOP_INNER, vA, vE0, CAA, CNull, CNull))
	{
		m_ErrorList.GeneralError("Error evaluating covariance matrix of error propagating product of multivector with e0.",
									iLine, iPos);
		return false;
	}

	// Evaluate cross covariances
	TMatrix CDummy;
	int iMat, iMatCount = mCCList.Count();

	for (iMat = 0; iMat < iMatCount; iMat++)
	{
		if (!EvalEPCrossCovMatProd(CDummy, MVOP_INNER, vA, vE0, mCCList[iMat], CNull))
		{
			m_ErrorList.GeneralError("Error evaluating cross-covariance matrix of product of multivectors.", iLine, iPos);
			return false;
		}

		mCCList[iMat] = CDummy;
	}

	TCVScalar fH, fH2, fH3, fH4;

	// Get homogeneous component of vector
	fH = -Scalar(vB * m_N3GABase.vE);
//	fH = -Scalar(vB * m_N3GABase.vEm);

	if (fH == TCVScalar(0))
	{
		m_ErrorList.GeneralError("The homogeneous component of the given vector is zero.", 
									iLine, iPos);
		return false;
	}

	int iE[3];
	iE[0] = m_E3GABase.iE1 + 1;
	iE[1] = m_E3GABase.iE2 + 1;
	iE[2] = m_E3GABase.iE3 + 1;

	int iNE[5];
	iNE[0] = m_N3GABase.iE1 + 1;
	iNE[1] = m_N3GABase.iE2 + 1;
	iNE[2] = m_N3GABase.iE3 + 1;
	iNE[3] = m_N3GABase.iEp + 1;
	iNE[4] = m_N3GABase.iEm + 1;

	// Evaluate mean vector in E3
	TCVScalar fVal1, fVal2;
	TMultiV vX(m_E3GABase.GetMVStyle()); // Resultant vector in E3
	TMatrix CXX(8, 8); // Resultant cov. mat.

	// Reset vector and matrix
	vX = TCVScalar(0);
	CXX = TCVScalar(0);

	// Constant values that are needed in following calculation
	fH = TCVScalar(1) / fH;
	fH2 = fH * fH;
	fH3 = fH2 * fH;
	fH4 = fH3 * fH;

	fVal1 = (CBB(iNE[3], iNE[3]) + CBB(iNE[4], iNE[4]) - TCVScalar(2) * CBB(iNE[3], iNE[4]));
//	fVal1 = CBB(iNE[4], iNE[4]);
	fVal2 = fH + fH3 * fVal1;

	// Mean vector in E3
	for (i = 0; i < 3; i++)
	{
		vX[iE[i]-1] =		vB[iNE[i]-1] * fVal2 
						+	fH2 * (CBB(iNE[i], iNE[3]) - CBB(iNE[i], iNE[4]));
	}

	fVal2 = fH4 * fVal1;

	// Covariance Matrix of vX
	int iRow, iCol;
	for ( iRow = 0; iRow < 3; iRow++ )
	{
		for ( iCol = 0; iCol < 3; iCol++ )
		{
			CXX(iE[iRow], iE[iCol]) = 
					fH2 * CBB(iNE[iRow], iNE[iCol]) 
				-	vB[iNE[iRow]-1] * fH3 * ( CBB(iNE[4], iNE[iCol]) - CBB(iNE[3], iNE[iCol]) )
				-	vB[iNE[iCol]-1] * fH3 * ( CBB(iNE[iRow], iNE[4]) - CBB(iNE[iRow], iNE[3]) )
				+	vB[iNE[iRow]-1] * vB[iNE[iCol]-1] * fVal2;
		}
	}

	TVarList ResList, PList;
	
	PList.Set(2);
	PList[0] = vX;
	PList[1] = CXX;

	ResList.Set(1);
	ResList[0] = PList;

	if (iMatCount > 0)
	{
		int iRow, iCol;
		TVarList CList;
		TMatrix CrossCov(8, 32);
		CrossCov = TCVScalar(0);

		CList.Set(iMatCount);

		for (iMat = 0; iMat < iMatCount; iMat++)
		{
			TMatrix &rMat = mCCList[iMat];
			for ( iRow = 0; iRow < 3; iRow++ )
			{
				for ( iCol = 0; iCol < 32; iCol++ )
				{
					CrossCov(iE[iRow], iCol+1) = 
							fH * rMat(iNE[iRow], iCol+1)
						-	fH2 * vB[iNE[iRow]-1] * (rMat(iNE[4], iCol+1) - rMat(iNE[3], iCol+1));
				}
			}

			CList[iMat] = CrossCov;
		}

		ResList.Add(1);
		ResList.Last() = CList;
	}

	rVar = ResList;

	return true;
}
