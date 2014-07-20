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
#include <time.h>
#include <algorithm>

#include "CLUCodeBase.h"
#include "TensorOperators.h"

using std::sort;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////
/// Reduce row space of matrix to set of linearly independent rows
//
// Parameters:
//	1. Matrix to be reduced
//	2. Precision to be used. That is, magnitude of rejection below which 
//		two vectors are parallel.
//
// Returns:
//	List of
//		1. reduced matrix
//		2. index list of indicating movement of rows.

typedef pair<int, TCVScalar> TIdxValPair;

bool PairGreater(TIdxValPair v1, TIdxValPair v2)
{
	if (v1.second > v2.second)
		return true;

	return false;
}

bool CCLUCodeBase::ReduceMatrixToRowSpaceFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	
	TCVScalar dPrecision = 1e-12;
	int iVarCount = mVars.Count();

	if (iVarCount < 1 || iVarCount > 2)
	{
		int piParNo[] = { 1, 2 };
		m_ErrorList.WrongNoOfParams(piParNo, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_MATRIX)
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (iVarCount > 1)
	{
		if (!mVars(1).CastToScalar(dPrecision, m_fSensitivity))
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}
		//printf("Prec.: %g\n", dPrecision);
		dPrecision = Mag(dPrecision);
		//printf("Prec.: %g\n", dPrecision);
	}

	TMatrix& mA = *mVars(0).GetMatrixPtr();

	int i, j, iRows, iCols;
	TCVScalar dNorm;

	iRows = mA.Rows();
	iCols = mA.Cols();

	vector<TIdxValPair> vecRowNorm;
	
	// Evaluate L2 norm of rows
	for (i = 0; i < iRows; i++)
	{
		dNorm = 0;
		for (j = 0; j < iCols; j++)
		{
			dNorm += pow(mA(i+1,j+1), 2.0);
		}
		dNorm = sqrt(dNorm);
		//printf("%d: Norm = %g\n", i, dNorm);
		vecRowNorm.push_back(TIdxValPair(i, dNorm));
	}

	// Sort row indices in descending order
	sort(vecRowNorm.begin(), vecRowNorm.end(), PairGreater);

	// Create matrix with normalized rows.
	int iRowIdx;
	TMatrix mB(1, iCols);
	vector<TMatrix> vecRows;

	for (i = 0; i < iRows; i++)
	{
		iRowIdx = vecRowNorm[i].first + 1;
		dNorm = vecRowNorm[i].second;
		if (dNorm > TCVScalar(0))
		{
			for (j = 0; j < iCols; j++)
			{
				mB(1,j+1) = mA(iRowIdx,j+1) / dNorm;
			}
			//printMat(mB, "B",0);
			vecRows.push_back(mB);
		}
  	}

	// Create reduced matrix
	int iBasisDim;
	TMatrix mX, mVal;
	vector<TMatrix> vecBasis;
	vector<int> vecBasisIdx;

	vecBasis.push_back(vecRows[0]);
	vecBasisIdx.push_back(vecRowNorm[0].first);
	for ( i = 1; i < iRows; i++ )
	{
		// Evaluate rejection of a row on basis found so far
		iBasisDim = vecBasis.size();
		mX = vecRows[i];
		for ( j = 0; j < iBasisDim; j++)
		{
			mVal = vecBasis[j] * ~mX;
			mX -= mVal(1,1) * vecBasis[j];
		}

		// Eval. norm of mX
		dNorm = 0;
		for (int iC = 1; iC <= iCols; iC++)
		{
			dNorm += pow(mX(1,iC), 2.0);
		}
		dNorm = sqrt(dNorm);
		
		//printMat(mX, "Rejection", 0);
		//printf("Norm: %g,  Prec.: %g\n", dNorm, dPrecision);

		if (dNorm >= dPrecision)
		{
			//mX = (1.0 / dNorm) * mX;
			dNorm = 1.0 / dNorm;
			mX = dNorm * mX;
			vecBasis.push_back(mX);
			vecBasisIdx.push_back(vecRowNorm[i].first);
		}
	}

	// Construct final matrix
	int iRowCount = vecBasisIdx.size();

	//printMat(mA, "A", 0);
	mB.Resize(iRowCount, iCols);
	for(i = 0; i < iRowCount; i++)
	{
		iRowIdx = vecBasisIdx[i]+1;
		//printf("Row: %d,  RowIdx: %d\n", i+1, iRowIdx);
		for (j = 0; j < iCols; j++)
		{
			mB(i+1, j+1) = mA(iRowIdx, j+1);
		}
	}
	//printMat(mB, "B", 0);

	// Generate output
	rVar.New(PDT_VARLIST, "Constant");
	TVarList &rList = *rVar.GetVarListPtr();
		
	if (!rList.Add(2))
	{
		m_ErrorList.OutOfMemory(iLine, iPos);
		return false;
	}
	
	rList[0] = mB;

	rList[1].New(PDT_VARLIST, "Constant");
	TVarList &rIdxList = *rList[1].GetVarListPtr();

	if (!rIdxList.Add(iRowCount))
	{
		m_ErrorList.OutOfMemory(iLine, iPos);
		return false;
	}
	
	for (i = 0; i < iRowCount; i++)
	{
		rIdxList[i] = vecBasisIdx[i] + 1;
	}

	return true;
}

bool CCLUCodeBase::ReduceMatrixToMajorRowsFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iNumb;
	
	int iVarCount = mVars.Count();

	if ( iVarCount != 2)
	{
		m_ErrorList.WrongNoOfParams( 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_MATRIX)
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToCounter( iNumb ))
	{
		m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}
	iNumb = Mag( iNumb );

	TMatrix& mA = *mVars(0).GetMatrixPtr();

	int i, j, iRows, iCols;
	TCVScalar dNorm;

	iRows = mA.Rows();
	iCols = mA.Cols();

	if ( iNumb > iRows )
	{
		m_ErrorList.InconsistentRowLength( iRows, iLine, iPos );
		return false;
	}

	vector<TIdxValPair> vecRowNorm, idxAndNorm;
	
	// Evaluate L2 norm of rows
	for (i = 0; i < iRows; i++)
	{
		dNorm = 0;
		for (j = 0; j < iCols; j++)
		{
			dNorm += pow(mA(i+1,j+1), 2.0);
		}
		dNorm = sqrt(dNorm);
		// printf("%d: Norm = %g\n", i, dNorm);
		vecRowNorm.push_back(TIdxValPair(i, dNorm));
	}

	// Sort row indices in descending order
	sort(vecRowNorm.begin(), vecRowNorm.end(), PairGreater);

	// Create matrix with normalized rows.
	int iRowIdx;
	TMatrix mB(1, iCols);
	vector<TMatrix> vecRows;

	for (i = 0; i < iRows; i++)
	{
		iRowIdx = vecRowNorm[i].first + 1;
		dNorm = vecRowNorm[i].second;
		if (dNorm > TCVScalar(0))
		{
			for (j = 0; j < iCols; j++)
			{
				mB(1,j+1) = mA(iRowIdx,j+1) / dNorm;
			}
			//printMat(mB, "B",0);
			vecRows.push_back(mB);
		}
  	}

	// Create reduced matrix
	int iBasisDim;
	TMatrix mX, mVal;
	vector<TMatrix> vecBasis;
	vector<int> vecBasisIdx;

	vecBasis.push_back(vecRows[0]);
	vecBasisIdx.push_back(vecRowNorm[0].first);
	idxAndNorm.push_back( TIdxValPair( vecRowNorm[0].first, 1.0  ) );
	for ( i = 1; i < iRows; i++ )
	{
		// Evaluate rejection of a row on basis found so far
		iBasisDim = vecBasis.size();
		mX = vecRows[i];
		for ( j = 0; j < iBasisDim; j++)
		{
			mVal = vecBasis[j] * ~mX;
			mX -= mVal(1,1) * vecBasis[j];
		}

		// Eval. norm of mX
		dNorm = 0;
		for (int iC = 1; iC <= iCols; iC++)
		{
			dNorm += pow(mX(1,iC), 2.0);
		}
		dNorm = sqrt(dNorm);
		
		if ( dNorm > 0 )
		{
			dNorm = 1.0 / dNorm;
			mX = dNorm * mX;
			vecBasis.push_back(mX);
			idxAndNorm.push_back( TIdxValPair( vecRowNorm[i].first, 1.0 / dNorm ) );
		}
	}
	// Sort row indices in descending order
	sort( idxAndNorm.begin(), idxAndNorm.end(), PairGreater);

	vecBasisIdx.clear();
	for (i = 0; i < iNumb; i++)
		vecBasisIdx.push_back( idxAndNorm[i].first + 1 );

	sort( vecBasisIdx.begin(), vecBasisIdx.end() );

	// Generate output
	rVar.New(PDT_VARLIST, "Constant");
	TVarList &rIdxList = *rVar.GetVarListPtr();

	if (!rIdxList.Add( iNumb))
	{
		m_ErrorList.OutOfMemory(iLine, iPos);
		return false;
	}
	
	for (i = 0; i < iNumb; i++)
	{
		rIdxList[i] = vecBasisIdx[i];
	}

	return true;
}

//////////////////////////////////////////////////////////////////
/// Extract Rows given by an index list from matrix
//
// Parameters:
//	1. matrix from which rows are to be extracted
//	2. list of row indices
//
// Returns:
//	Matrix constructed from extracted rows.
//

bool CCLUCodeBase::ExtractMatrixRowsFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	
	int iVarCount = mVars.Count();

	if (iVarCount != 2)
	{
		int piParNo[] = { 2 };
		m_ErrorList.WrongNoOfParams(piParNo, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_MATRIX)
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_VARLIST)
	{
		m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}

	TMatrix& mA = *mVars(0).GetMatrixPtr();
	TVarList& rIdxList = *mVars(1).GetVarListPtr();

	int iRows, iCols, iIdx;
	int i,j, iIdxCount = rIdxList.Count();
	vector<int> vecIdxList;

	iRows = mA.Rows();
	iCols = mA.Cols();
	vecIdxList.resize(iIdxCount);

	// Copy list of row indices
	for(i = 0; i < iIdxCount; i++)
	{
		if (!rIdxList[i].CastToCounter(iIdx))
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		if (iIdx > iRows || iIdx <= 0)
		{
			m_ErrorList.GeneralError("Index out of range.", iLine, iPos);
			return false;
		}

		vecIdxList[i] = iIdx;
	}

	// Copy rows
	rVar.New(PDT_MATRIX, "Constant");
	TMatrix &mB = *rVar.GetMatrixPtr();

	mB.Resize(iIdxCount, iCols);

	for (i = 0; i < iIdxCount; i++)
	{
		iIdx = vecIdxList[i];
		for (j = 0; j < iCols; j++)
		{
			mB(i+1, j+1) = mA(iIdx, j+1);
		}
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Add Rows to matrix

bool CCLUCodeBase::AddMatrixRowsFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();

	if (iVarCount != 2)
	{
		int piParNo[] = { 2 };
		m_ErrorList.WrongNoOfParams(piParNo, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_MATRIX)
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_MATRIX)
	{
		m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}

	TMatrix& mA = *mVars(0).GetMatrixPtr();
	TMatrix& mB = *mVars(1).GetMatrixPtr();
	
	rVar = mA;
	TMatrix& mR = *rVar.GetMatrixPtr();

	if (mA.Cols() != mB.Cols())
	{
		m_ErrorList.GeneralError("The matrices do not have the same number of columns.", iLine, iPos);
		return false;
	}

	if (!mR.AddRows(mR.Rows()+1, 1, mB, mB.Rows()))
	{
		m_ErrorList.GeneralError("Error adding rows.", iLine, iPos);
		return false;
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Add Cols to matrix

bool CCLUCodeBase::AddMatrixColsFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();

	if (iVarCount != 2)
	{
		int piParNo[] = { 2 };
		m_ErrorList.WrongNoOfParams(piParNo, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_MATRIX)
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_MATRIX)
	{
		m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}

	TMatrix& mA = *mVars(0).GetMatrixPtr();
	TMatrix& mB = *mVars(1).GetMatrixPtr();

	rVar = mA;
	TMatrix& mR = *rVar.GetMatrixPtr();

	if (mA.Rows() != mB.Rows())
	{
		m_ErrorList.GeneralError("The matrices do not have the same number of rows.", iLine, iPos);
		return false;
	}

	if (!mR.AddCols(mR.Cols()+1, 1, mB, mB.Cols()))
	{
		m_ErrorList.GeneralError("Error adding columns.", iLine, iPos);
		return false;
	}

	return true;
}



//////////////////////////////////////////////////////////////////////
/// Create Identity Matrix of given Dimensions

bool CCLUCodeBase::IdentityMatrixFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();

	if (iVarCount == 2)
	{
		// Dimension of Identity-Matrix given
		int iRows = 0, iCols = 0;
		
		if (!mVars(0).CastToCounter(iRows))
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}
		
		if (iRows <= 0)
		{
			cStr csText;
			csText = iRows;
			m_ErrorList.InvalidParVal(csText, 1, iLine, iPos);
			return false;
		}
		
		if (!mVars(1).CastToCounter(iCols))
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}
		
		if (iCols <= 0)
		{
			cStr csText;
			csText = iCols;
			m_ErrorList.InvalidParVal(csText, 2, iLine, iPos);
			return false;
		}
		
		TMatrix xA(iRows, iCols);
		xA.Identity();

		rVar = xA;
	}
	else
	{
		int piParNo[] = { 2 };
		m_ErrorList.WrongNoOfParams(piParNo, 1, iLine, iPos);
		return false;
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Resize Matrix and keep data

bool CCLUCodeBase::ResizeMatrixFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();

	if (iVarCount == 3)
	{
		// Dimension of Identity-Matrix given
		int iRows = 0, iCols = 0;
		
		if (mVars(0).BaseType() != PDT_MATRIX)
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (!mVars(1).CastToCounter(iRows))
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}
		
		if (iRows <= 0)
		{
			cStr csText;
			csText = iRows;
			m_ErrorList.InvalidParVal(csText, 2, iLine, iPos);
			return false;
		}
		
		if (!mVars(2).CastToCounter(iCols))
		{
			m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}
		
		if (iCols <= 0)
		{
			cStr csText;
			csText = iCols;
			m_ErrorList.InvalidParVal(csText, 3, iLine, iPos);
			return false;
		}
		
		// The Matrix
		TMatrix Mat = *mVars(0).GetMatrixPtr();;

		if (Mat.Resize(iRows, iCols) == 0)
		{
			m_ErrorList.GeneralError("Error resizing matrix.", iLine, iPos);
			return false;
		}
		rVar = Mat;
	}
	else
	{
		int piParNo[] = { 3 };
		m_ErrorList.WrongNoOfParams(piParNo, 1, iLine, iPos);
		return false;
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Reshape Matrix and keep data

bool CCLUCodeBase::ReshapeMatrixFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();

	if (iVarCount == 3)
	{
		// Dimension of Identity-Matrix given
		int iRows = 0, iCols = 0;
		
		if (mVars(0).BaseType() != PDT_MATRIX)
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (!mVars(1).CastToCounter(iRows))
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}
		
		if (iRows <= 0)
		{
			cStr csText;
			csText = iRows;
			m_ErrorList.InvalidParVal(csText, 2, iLine, iPos);
			return false;
		}
		
		if (!mVars(2).CastToCounter(iCols))
		{
			m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}
		
		if (iCols <= 0)
		{
			cStr csText;
			csText = iCols;
			m_ErrorList.InvalidParVal(csText, 3, iLine, iPos);
			return false;
		}
		
		// The Matrix
		TMatrix Mat = *mVars(0).GetMatrixPtr();;

		if (Mat.Reshape(iRows, iCols) == 0)
		{
			m_ErrorList.GeneralError("Error reshaping matrix.", iLine, iPos);
			return false;
		}
		rVar = Mat;
	}
	else
	{
		int piParNo[] = { 3 };
		m_ErrorList.WrongNoOfParams(piParNo, 1, iLine, iPos);
		return false;
	}
	
	return true;
}



//////////////////////////////////////////////////////////////////////
/// Make Diagonal of Matrix into a Row Vector

bool CCLUCodeBase::DiagToVectorFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	
	if (iVarCount == 1)
	{
		if (mVars(0).BaseType() == PDT_MATRIX)
		{
			TMatrix M = *mVars(0).GetMatrixPtr();
			M.Trace();
			rVar = M;
		}
		else
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Make Vector into diagonal of Matrix

bool CCLUCodeBase::VectorToDiagFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	
	if (iVarCount == 1)
	{
		if (mVars(0).BaseType() == PDT_MATRIX)
		{
			TMatrix M = *mVars(0).GetMatrixPtr();
			M.Diagonal();
			rVar = M;
		}
		else
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}





//////////////////////////////////////////////////////////////////////
/// Get Multivector from Matrix (column vector) in algebra

bool CCLUCodeBase::GetMatrixMVFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	
	if (iVarCount == 1 || iVarCount == 2)
	{
		if (mVars(0).BaseType() != PDT_MATRIX)
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		TVarList mRet;
		TMatrix &M = *mVars(0).GetMatrixPtr();
		MemObj<TMultiV> mvA;
		Mem<int> mMask;

		int iGADim;
		int iMVCount = int(M.Cols());
		int iDim, iMV;

		if (iVarCount == 2)
		{
			if (mVars(1).BaseType() != PDT_VARLIST)
			{
				m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
				return false;
			}

			TVarList &rList = *mVars(1).GetVarListPtr();
			int i, iVal;
			
			iGADim = rList.Count();
			mMask.Set(iGADim);
			for ( i = 0; i < iGADim; i++ )
			{
				if (!rList[i].CastToCounter(iVal)) 
				{
					m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
					return false;			
				}

				mMask[i] = iVal;
			}
		}
		else
		{
			iGADim = int(M.Rows());
			int i;
			
			mMask.Set(iGADim);
			for ( i = 0; i < iGADim; i++ )
			{
				mMask[i] = i+1;
			}
		}

		if (iGADim != 8 && iGADim != 16 && iGADim != 32 && iGADim != 64)
		{
			m_ErrorList.GeneralError("Matrix has invalid row dimension. Expect 8, 16, 32 or 64.", iLine, iPos);
			return false;
		}

		mvA.Set(iMVCount);
		mRet.Set(iMVCount);
		for( iMV = 0; iMV < iMVCount; iMV++ )
		{
			TMultiV &vA = mvA[iMV];
			int iRow;

			if (iGADim == 8)
			{
				vA = E3GA<TCVScalar>::vSC;
			}
			else if (iGADim == 16)
			{
				vA = PGA<TCVScalar>::vSC;
			}
			else if (iGADim == 32)
			{
				vA = ConfGA<TCVScalar>::vSC;
			}
			else if (iGADim == 64)
			{
				vA = El2GA<TCVScalar>::vSC;
			}

			for( iDim = 0; iDim < iGADim; iDim++ )
			{
				iRow = mMask[iDim];
				if (iRow == 0)
					vA[iDim] = 0;
				else
				{
					if (iRow > 0)
						vA[iDim] = M(iRow, iMV + 1);
					else
						vA[iDim] = -M(-iRow, iMV + 1);
				}
			}

			mRet[iMV] = vA;
		}

		rVar = mRet;
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Get Matrix (column vector) of multivector in algebra

bool CCLUCodeBase::GetMVMatrixFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	bool bMVList = false;
	int iVarCount = mVars.Count();
	
	if (iVarCount == 1 || iVarCount == 2)
	{

		TMatrix M;
		Mem<int> mMask;
		TMultiV *pvA = 0;
		TVarList *pMVList = 0;
		int i, j, iGADim; // = int(vA.GetGADim());
		int iRowDim, iMVCount;
		uint uBaseID;

		if (mVars(0).BaseType() == PDT_MULTIV)
		{
			bMVList = false;
			iMVCount = 1;
			pvA = mVars(0).GetMultiVPtr();
			iGADim = pvA->GetGADim();
			uBaseID = pvA->GetBase().BaseID();
		}
		else if (mVars(0).BaseType() == PDT_VARLIST)
		{
			bMVList = true;
			pMVList = mVars(0).GetVarListPtr();
			if ((iMVCount = pMVList->Count()) < 1)
			{
				m_ErrorList.GeneralError("List of multivectors is empty.", iLine, iPos);
				return false;
			}

			if ( (*pMVList)[0].BaseType() != PDT_MULTIV )
			{
				m_ErrorList.GeneralError("First element in list is not a multivector.", iLine, iPos);
				return false;
			}

			iGADim = int( (*pMVList)[0].GetMultiVPtr()->GetGADim() );
			uBaseID = (*pMVList)[0].GetMultiVPtr()->GetBase().BaseID();
		}
		else
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		mMask.Set(iGADim);

		if (iVarCount == 2)
		{
			if (mVars(1).BaseType() != PDT_VARLIST)
			{
				m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
				return false;
			}

			TVarList &rList = *mVars(1).GetVarListPtr();
			int i, iVal;
			

			if (iGADim != rList.Count())
			{
				m_ErrorList.GeneralError("Invalid mask dimension.", iLine, iPos);
				return false;
			}
			
			iRowDim = 0;
			for ( i = 0; i < iGADim; i++ )
			{
				if (!rList[i].CastToCounter(iVal)) 
				{
					m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
					return false;			
				}

				if (iVal < 0)
				{
					m_ErrorList.GeneralError("Invalid mask. Negative index in mask.",
											iLine, iPos);
					return false;
				}

				mMask[i] = iVal;
				if (iVal > iRowDim)
					iRowDim = iVal;
			}

			if (iRowDim == 0)
			{
				m_ErrorList.GeneralError("Invalid mask. There is no value larger than zero.",
										iLine, iPos);
				return false;
			}
		}
		else
		{
			int i;

			for (i = 0; i < iGADim; i++)
			{
				mMask[i] = i+1;
			}
			iRowDim = iGADim;
		}

		if (bMVList)
		{
			M.Resize(iRowDim, iMVCount);
			M = (TCVScalar) 0;

			for (j = 0; j < iMVCount; j++)
			{
				if ( (*pMVList)[j].BaseType() != PDT_MULTIV )
				{
					char pcText[200];
					sprintf(pcText, "Element at position %d is not a multivector.", j+1);
					m_ErrorList.GeneralError(pcText, iLine, iPos);
					return false;
				}

				pvA = (*pMVList)[j].GetMultiVPtr();
				if (pvA->GetBase().BaseID() != uBaseID)
				{
					char pcText[200];
					sprintf(pcText, "Multivectors at positions 1 and %d are from different spaces.", j+1);
					m_ErrorList.GeneralError(pcText, iLine, iPos);
					return false;
				}

				for( i = 0; i < iGADim; i++ )
				{
					if (mMask[i] > 0)
						M(mMask[i], j+1) = (*pvA)[i];
				}
			}
		}
		else
		{
			M.Resize(iRowDim, 1);
			M = (TCVScalar) 0;
			for( i = 0; i < iGADim; i++ )
			{
				if (mMask[i] > 0)
					M(mMask[i], 1) = (*pvA)[i];
			}
		}

		rVar = M;
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}



//////////////////////////////////////////////////////////////////////
/// Get Multivector Geometric Product Matrix

bool CCLUCodeBase::GetMVProductMatrixFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	
	if (iVarCount >= 1 && iVarCount <= 4)
	{
		int iProductType = 0;
		bool bPruneNullRows = false;
		Mem<int> mMask;

		if (iVarCount >= 2)
		{
			if (!mVars(1).CastToCounter(iProductType))
			{
				m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
				return false;
			}

			if (iProductType < 0 || iProductType > 2)
			{
				m_ErrorList.InvalidParVal(mVars(1), 2, iLine, iPos);
				return false;
			}
		}
		
		CCodeVar &rMVVar = mVars(0);
		MemObj<TMultiV> mvA;
		int iMV, iMVCount;

		if (rMVVar.BaseType() == PDT_MULTIV)
		{
			mvA.Set(1);
			mvA[0] = *rMVVar.GetMultiVPtr();
		}
		else if (rMVVar.BaseType() == PDT_VARLIST)
		{
			TVarList &rList = *rMVVar.GetVarListPtr();

			if (!(iMVCount = rList.Count()))
			{
				m_ErrorList.GeneralError("Multivector List is empty.", iLine, iPos);
				return false;
			}

			mvA.Set(iMVCount);
			for ( iMV = 0; iMV < iMVCount; iMV++ )
			{
				if (rList(iMV).BaseType() != PDT_MULTIV)
				{
					m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
					return false;
				}
				mvA[iMV] = *rList(iMV).GetMultiVPtr();
			}
		}
		else
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		TMatrix M;
		int i, iGADim = int(mvA[0].GetGADim());
		int iMaskParPos;

		if (iVarCount >= 3 && mVars(2).BaseType() != PDT_VARLIST && mVars(2).BaseType() != PDT_INT)
		{
			m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}
		
		if (iVarCount >= 4 && mVars(3).BaseType() != PDT_VARLIST && mVars(3).BaseType() != PDT_INT)
		{
			m_ErrorList.InvalidParType(mVars(3), 4, iLine, iPos);
			return false;
		}

		if (iVarCount >= 4 && mVars(2).BaseType() == mVars(3).BaseType())
		{
			m_ErrorList.GeneralError("Third and fourth parameter cannot be of same type.", iLine, iPos);
			return false;
		}

		// Check whether Mask is 3rd or 4th parameter
		if (iVarCount >= 3 && mVars(2).BaseType() == PDT_VARLIST)
		{
			iMaskParPos = 2;
			bPruneNullRows = true;
		}
		else if (iVarCount >= 4 && mVars(3).BaseType() == PDT_VARLIST)
		{
			iMaskParPos = 3;
			bPruneNullRows = true;
		}
		else
		{
			iMaskParPos = 0;
		}

		// If Mask is given either at 3rd or 4th position then read it.
		if (iMaskParPos)
		{
			if (mVars(iMaskParPos).BaseType() != PDT_VARLIST)
			{
				m_ErrorList.InvalidParType(mVars(iMaskParPos), iMaskParPos+1, iLine, iPos);
				return false;
			}

			TVarList &rList = *mVars(iMaskParPos).GetVarListPtr();
			int i, iVal, iCount = rList.Count();
			
			if (iCount != iGADim)
			{
				m_ErrorList.GeneralError("Invalid mask dimension.", iLine, iPos);
				return false;
			}

			mMask.Set(iCount);
			for ( i = 0; i < iCount; i++ )
			{
				if (!rList(i).CastToCounter(iVal)) 
				{
					m_ErrorList.InvalidParType(mVars(iMaskParPos), iMaskParPos+1, iLine, iPos);
					return false;			
				}

				mMask[i] = iVal;
			}
		}

		// If explicit mask was not given, create a standard mask here.
		if (mMask.Count() == 0)
		{
			mMask.Set(iGADim);
			for( i = 0; i < iGADim; i++ )
			{
				mMask[i] = i+1;
			}
		}

		// Check whether left/right flag was given either in third or in fourth position
		int iLRFlagParPos;
		tMVPos eProdDir = Left;

		if (iVarCount >= 3 && mVars(2).BaseType() == PDT_INT)
		{
			iLRFlagParPos = 2;
		}
		else if (iVarCount >= 4 && mVars(3).BaseType() == PDT_INT)
		{
			iLRFlagParPos = 3;
		}
		else
		{
			iLRFlagParPos = 0;
		}

		if (iLRFlagParPos)
		{
			int iVal = 0;

			mVars(iLRFlagParPos).CastToCounter(iVal);
			if (iVal)
				eProdDir = Left;
			else
				eProdDir = Right;
		}



		if (!CalcMVListGPMatMask(mvA, M, eProdDir, mMask, EMVOpType(iProductType), bPruneNullRows))
		{
			m_ErrorList.GeneralError("Multivector Product Matrix could not be evaluated.", iLine, iPos);
			return false;
		}

		rVar = M;
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}



//////////////////////////////////////////////////////////////////////
/// Determinant of Matrix

bool CCLUCodeBase::DetFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList *pmVars = rPars.GetVarListPtr();
	int iVarCount = pmVars->Count();
	
	if (iVarCount == 1)
	{
		TMatrix *pMat, Mat;

		if ((*pmVars)(0).BaseType() == PDT_MATRIX)
		{
			pMat = (*pmVars)(0).GetMatrixPtr();
		}
		else if (CastToMatrix(Mat, (*pmVars)(0), iLine, iPos))
		{
			pMat = &Mat;
		}
		else return false;
		
		TMatrix &xA = *pMat;
		
		rVar = xA.Det();
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// SVD of Matrix

bool CCLUCodeBase::SVDFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList *pmVars = rPars.GetVarListPtr();
	int iVarCount = pmVars->Count();
	
	if (iVarCount == 1)
	{
		TMatrix *pMat, Mat;
		bool bTensor = false;

		if ((*pmVars)(0).BaseType() == PDT_MATRIX)
		{
			pMat = (*pmVars)(0).GetMatrixPtr();
		}
		else if (CastToMatrix(Mat, (*pmVars)(0), iLine, iPos))
		{
			pMat = &Mat;
			bTensor = true;
		}
		else return false;
		
		TMatrix &xA = *pMat;
		TMatrix xU, xD, xV;
		
		xA.SVD(xU, xD, xV);
		SVDOrder(xD, xU, xV, false);
		
		xU.TinyToZero(m_fSensitivity);
		xV.TinyToZero(m_fSensitivity);
		xD.TinyToZero(m_fSensitivity);
		xD.Diagonal();
		
		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rList = *rVar.GetVarListPtr();
		
		if (!rList.Add(3))
		{
			m_ErrorList.OutOfMemory(iLine, iPos);
			return false;
		}
		
		rList[0] = xU;
		rList[1] = xD;
		rList[2] = xV;
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Eigenvalues of Matrix

bool CCLUCodeBase::EigenValuesFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList *pmVars = rPars.GetVarListPtr();
	int iVarCount = pmVars->Count();
	
	if (iVarCount == 1)
	{
		TMatrix *pMat, Mat;

		if ((*pmVars)(0).BaseType() == PDT_MATRIX)
		{
			pMat = (*pmVars)(0).GetMatrixPtr();
		}
		else if (CastToMatrix(Mat, (*pmVars)(0), iLine, iPos))
		{
			pMat = &Mat;
		}
		else return false;
		
		TMatrix &xA = *pMat;
		TMatrix xREVal, xIEVal;
		
		if (!EigenValues(xA, xREVal, xIEVal))
		{
			m_ErrorList.GeneralError("Error evaluating Eigenvalues", iLine, iPos);
			return false;
		}
		
		xREVal.Round(m_fSensitivity);
		xIEVal.Round(m_fSensitivity);
		
		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rList = *rVar.GetVarListPtr();
		
		if (!rList.Add(2))
		{
			m_ErrorList.OutOfMemory(iLine, iPos);
			return false;
		}
		
		rList[0] = xREVal;
		rList[1] = xIEVal;
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Eigenvectors and -values of Matrix

bool CCLUCodeBase::EigenFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList *pmVars = rPars.GetVarListPtr();
	int iVarCount = pmVars->Count();
	
	if (iVarCount == 1)
	{
		TMatrix *pMat, Mat;

		if ((*pmVars)(0).BaseType() == PDT_MATRIX)
		{
			pMat = (*pmVars)(0).GetMatrixPtr();
		}
		else if (CastToMatrix(Mat, (*pmVars)(0), iLine, iPos))
		{
			pMat = &Mat;
		}
		else return false;
		
		TMatrix &xA = *pMat;
		TMatrix xREVal, xIEVal, xREVecs, xIEVecs;
		
		if (!Eigen(xA, xREVal, xIEVal, xREVecs, xIEVecs))
		{
			m_ErrorList.GeneralError("Error evaluating Eigenvalues and -vectors.", iLine, iPos);
			return false;
		}
		
		xREVal.Round(m_fSensitivity);
		xIEVal.Round(m_fSensitivity);
		xREVecs.Round(m_fSensitivity);
		xIEVecs.Round(m_fSensitivity);
		
		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rList = *rVar.GetVarListPtr();
		
		if (!rList.Add(4))
		{
			m_ErrorList.OutOfMemory(iLine, iPos);
			return false;
		}
		
		rList[0] = xREVal;
		rList[1] = xIEVal;
		rList[2] = xREVecs;
		rList[3] = xIEVecs;
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Create Matrix
bool CCLUCodeBase::MatrixFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList *pmVars = rPars.GetVarListPtr();
	int iVarCount = pmVars->Count();
	
	if (iVarCount == 1)
	{
		rVar.New(PDT_MATRIX, "Constant");
		TMatrix &rMat = *rVar.GetMatrixPtr();

		return CastToMatrix(rMat, (*pmVars)(0), iLine, iPos);

	} // iVarCount == 1
	else if (iVarCount == 2)
	{
		// Dimension of Null-Matrix given
		int iRows = 0, iCols = 0;
		
		if (!(*pmVars)(0).CastToCounter(iRows))
		{
			m_ErrorList.InvalidParType((*pmVars)(0), 1, iLine, iPos);
			return false;
		}
		
		if (iRows <= 0)
		{
			cStr csText;
			csText = iRows;
			m_ErrorList.InvalidParVal(csText, 1, iLine, iPos);
			return false;
		}
		
		if (!(*pmVars)(1).CastToCounter(iCols))
		{
			m_ErrorList.InvalidParType((*pmVars)(1), 2, iLine, iPos);
			return false;
		}
		
		if (iCols <= 0)
		{
			cStr csText;
			csText = iCols;
			m_ErrorList.InvalidParVal(csText, 2, iLine, iPos);
			return false;
		}
		
		TMatrix xA(iRows, iCols);
		
		rVar = xA;
	}
	else
	{
		int piParNo[] = { 1, 2 };
		m_ErrorList.WrongNoOfParams(piParNo, 2, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Cast to Matrix

bool CCLUCodeBase::CastToMatrix(TMatrix& rMat, CCodeVar& rVar, int iLine, int iPos)
{
	if (rVar.BaseType() == PDT_VARLIST)
	{
		int i, j;
		int iRows = 0, iCols = 0;
		TCVScalar fVal;
		TVarList &rRowList = *rVar.GetVarListPtr();

		if (rRowList[0].BaseType() == PDT_VARLIST)
		{
			// Nested List defines Matrix
			iRows = rRowList.Count();

			for(i=0;i<iRows;i++)
			{
				if (rRowList[i].BaseType() != PDT_VARLIST)
				{
					m_ErrorList.InvalidType(rRowList[i], iLine, iPos);
					return false;
				}

				TVarList &rRow = *((TVarList *) rRowList[i].GetVarListPtr());

				if (i > 0 && (int) rRow.Count() != iCols)
				{
					m_ErrorList.InconsistentRowLength(i+1, iLine, iPos);
					return false;
				}
				else if (i == 0)
				{
					// Initialize Matrix Size
					iCols = rRow.Count();
					rMat.Resize(iRows, iCols);
				}

				// Walk along row
				for(j=0;j<iCols;j++)
				{
					if (!rRow[j].CastToScalar(fVal, m_fSensitivity))
					{
						m_ErrorList.InvalidType(rRow[j], iLine, iPos);
						return false;
					}

					rMat(i+1, j+1) = fVal;
				} // for j
			} // for i
		}
		else if (rRowList[0].CastToScalar(fVal, m_fSensitivity))
		{
			// Single List gives column vector
			iRows = rRowList.Count();
			rMat.Resize(iRows, 1);

			rMat(1,1) = fVal;

			for(i=1;i<iRows;i++)
			{
				if (!rRowList[i].CastToScalar(fVal, m_fSensitivity))
				{
					m_ErrorList.InvalidType(rRowList[i], iLine, iPos);
					return false;
				}

				rMat(i+1, 1) = fVal;
			}
		}
		else
		{
			m_ErrorList.InvalidParType(rRowList[0], 1, iLine, iPos);
			return false;
		}
	} // type == PDT_VARLIST
	else if (rVar.BaseType() == PDT_MULTIV)
	{
		TMultiV &vA = *rVar.GetMultiVPtr();

		if (vA.BasePtr()->BaseID() != ID_EL2GA)
		{
			m_ErrorList.GeneralError("Multivector is not in conics space.", iLine, iPos);
			return false;
		}

		// Embed conic vector as symmetric 3x3 matrix.
		rMat = m_El2GABase.Mat(vA);

	} // type == PDT_MULTIV
	else if (rVar.BaseType() == PDT_TENSOR_IDX ||
			 rVar.BaseType() == PDT_TENSOR)
	{
		TTensor T, *pT;

		if (rVar.BaseType() == PDT_TENSOR_IDX)
		{
			TTensorIdx &rTIdx = *rVar.GetTensorIdxPtr();

			if (!MakeTensor(T, rTIdx))
			{
				m_ErrorList.GeneralError("Error creating tensor from index.", iLine, iPos);
				return false;
			}

			pT = &T;
		}
		else
		{
			pT = rVar.GetTensorPtr();
		}

		TTensor &rT = *pT;

		if (rT.Valence() == 1)
		{
			rMat.Resize(rT.DimSize(0), 1);

			memcpy(rMat.Data(), rT.Data(), rT.Size()*sizeof(TCVScalar));
		}
		else if (rT.Valence() == 2)
		{
			rMat.Resize(rT.DimSize(0), rT.DimSize(1));

			memcpy(rMat.Data(), rT.Data(), rT.Size()*sizeof(TCVScalar));
		}
		else
		{
			m_ErrorList.GeneralError("Tensor needs to be of valence 1 or 2.", iLine, iPos);
			return false;
		}
	}
	else
	{
		m_ErrorList.InvalidParType(rVar, 1, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Create a single column matrix from a symmetric matrix

bool CCLUCodeBase::Sym2ColMatrixFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	
	int iVarCount = mVars.Count();

	if (iVarCount != 1)
	{
		int piParNo[] = { 1 };
		m_ErrorList.WrongNoOfParams(piParNo, 1, iLine, iPos);
		return false;
	}

	TMatrix *pMat, Mat;

	if (mVars(0).BaseType() == PDT_MATRIX)
	{
		pMat = mVars(0).GetMatrixPtr();
	}
	else if (CastToMatrix(Mat, mVars(0), iLine, iPos))
	{
		pMat = &Mat;
	}
	else return false;

	TMatrix &rM = *pMat;

	if (rM.Rows() != rM.Cols())
	{
		m_ErrorList.GeneralError("Matrix has to be symmetric.", iLine, iPos);
		return false;
	}

	rVar.New(PDT_MATRIX, "Constant");
	TMatrix& MRes = *rVar.GetMatrixPtr();

	int iDim1, iDimCount, iDim2, iTrgPos, iSrcPos;
	iDimCount = rM.Rows();
	int iSize = iDimCount * iDimCount;

	MRes.Resize( ((iSize - iDimCount) >> 1) + iDimCount, 1 );

	TCVScalar dFac = sqrt(2.0);
	TCVScalar *pData = MRes.Data();
	TCVScalar *pSrcData = rM.Data();

	for ( iDim1 = 0, iTrgPos = 0, iSrcPos = 0; iDim1 < iDimCount; iDim1++)
	{
		iSrcPos += iDim1;
		for( iDim2 = iDim1; iDim2 < iDimCount; iDim2++, iTrgPos++, iSrcPos++)
		{
			if (iDim1 == iDim2)
				pData[iTrgPos] = pSrcData[iSrcPos];
			else
				pData[iTrgPos] = dFac * pSrcData[iSrcPos];
		}
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Create a symmetric matrix from a single column matrix

bool CCLUCodeBase::Col2SymMatrixFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	
	int iVarCount = mVars.Count();

	if (iVarCount != 1)
	{
		int piParNo[] = { 1 };
		m_ErrorList.WrongNoOfParams(piParNo, 1, iLine, iPos);
		return false;
	}

	TMatrix *pMat, Mat;

	if (mVars(0).BaseType() == PDT_MATRIX)
	{
		pMat = mVars(0).GetMatrixPtr();
	}
	else if (CastToMatrix(Mat, mVars(0), iLine, iPos))
	{
		pMat = &Mat;
	}
	else return false;

	TMatrix &rM = *pMat;
	int iRows, iCols;

	iRows = rM.Rows();
	iCols = rM.Cols();

	if (iCols != 1)
	{
		m_ErrorList.GeneralError("Matrix has to consist of single column.", iLine, iPos);
		return false;
	}

	double dVal  = -0.5 + sqrt(0.25 + 2.0*double(iRows));
	// dVal should be an integer.

	if (dVal < 0 || !::IsZero(fmod(dVal, 1), 1e-4))
	{
		m_ErrorList.GeneralError("Column vector cannot be transformed into symmetric matrix.", iLine, iPos);
		return false;
	}

	rVar.New(PDT_MATRIX, "Constant");
	TMatrix& MRes = *rVar.GetMatrixPtr();
	int iDim1, iDimCount, iDim2, iSrcPos;
	iDimCount = int(dVal);
	MRes.Resize(iDimCount, iDimCount);
	
	TCVScalar dFac = 1.0 / sqrt(2.0);
	TCVScalar *pData = MRes.Data();
	TCVScalar *pSrcData = rM.Data();

	for ( iDim1 = 0, iSrcPos = 0; iDim1 < iDimCount; iDim1++)
	{
		for( iDim2 = iDim1; iDim2 < iDimCount; iDim2++, iSrcPos++)
		{
			if (iDim1 == iDim2)
				pData[iDim1*iDimCount + iDim2] = pSrcData[iSrcPos];
			else
			{
				pData[iDim1*iDimCount + iDim2] = dFac * pSrcData[iSrcPos];
				pData[iDim2*iDimCount + iDim1] = dFac * pSrcData[iSrcPos];
			}
		}
	}

	return true;
}


