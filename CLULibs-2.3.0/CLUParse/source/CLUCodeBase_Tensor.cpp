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


//////////////////////////////////////////////////////////////////////
/// Create Tensor

bool CCLUCodeBase::TensorFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();

	if (iVarCount == 2)
	{
		int iValence, iDim;
		Mem<int> mDim;

		if (!mVars(0).CastToCounter(iValence))
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (iValence <= 0)
		{
			m_ErrorList.GeneralError("Tensor valence has to be greater than zero.", iLine, iPos);
			return false;
		}
		else if (iValence > TENSOR_MAX_VALENCE)
		{
			m_ErrorList.GeneralError("Tensor valence too large.", iLine, iPos);
			return false;
		}


		if (mVars(1).BaseType() != PDT_VARLIST)
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		TVarList &rList = *mVars(1).GetVarListPtr();

		if (rList.Count() != iValence)
		{
			m_ErrorList.GeneralError("Incompatible valence and dimension list.", iLine, iPos);
			return false;
		}

		mDim.Set(iValence);
		for (iDim = 0; iDim < iValence; iDim++)
		{
			if (!rList[iDim].CastToCounter(mDim[iDim]))
			{
				m_ErrorList.GeneralError("Invalid dimension value given.", iLine, iPos);
				return false;
			}
		}

		rVar.New(PDT_TENSOR, "Constant");
		TTensor &rT = *rVar.GetTensorPtr();

		rT.Reset(mDim);
	}
	else if (iVarCount == 1)
	{
		if (mVars(0).BaseType() == PDT_MATRIX)
		{
			TMatrix &rM = *mVars(0).GetMatrixPtr();

			rVar.New(PDT_TENSOR, "Constant");
			TTensor &rT = *rVar.GetTensorPtr();

			Matrix2Tensor(rT, rM);
		}
		else if (mVars(0).BaseType() == PDT_VARLIST)
		{
			int iValence = 0;
			Mem<int> mDim;
			TVarList *pList;
			CCodeVar *pVar;
			pVar = &mVars(0);

			while(pVar->BaseType() == PDT_VARLIST)
			{
				iValence++;
				pList = pVar->GetVarListPtr();

				if (pList->Count() == 0)
				{
					m_ErrorList.GeneralError("Empty tensor is not allowed.", iLine, iPos);
					return false;
				}

				mDim.Add(1);
				mDim.Last() = pList->Count();

				pVar = &( (*pList)(0) );
			}
			
			rVar.New(PDT_TENSOR, "Constant");
			TTensor &rT = *rVar.GetTensorPtr();

			rT.Reset(mDim);

			if (!VarList2Tensor(rT, mVars(0), 0, 0, false, iLine, iPos))
				return false;
		}
		else
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}
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
/// Transform Matrix into Tensor

bool CCLUCodeBase::Matrix2Tensor(TTensor& rT, const TMatrix &rM)
{
	Mem<int> mDim;

	mDim.Set(2);
	mDim[0] = rM.Rows();
	mDim[1] = rM.Cols();

	rT.Reset(mDim);

	memcpy(rT.Data(), rM.Data(), rT.Size()*sizeof(TCVScalar));

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Transform nested list to tensor

bool CCLUCodeBase::VarList2Tensor(TTensor &rT, CCodeVar &rVar, int iDim, int iDataPos, 
								  bool bDataDim, int iLine, int iPos)
{
	TCVScalar dVal;

	if (rVar.BaseType() == PDT_VARLIST)
	{
		if (bDataDim)
		{
			m_ErrorList.GeneralError("Variable list does not represent a tensor.", iLine, iPos);
			return false;
		}

		TVarList& rList = *rVar.GetVarListPtr();
		int iIdx, iStep, iNewPos, iCount = rList.Count();
		bool _bDataDim;
		TCVScalar *pData = rT.Data();

		if (iCount != rT.DimSize(iDim))
		{
			m_ErrorList.GeneralError("Variable list does not represent a tensor.", iLine, iPos);
			return false;
		}
	
		if (rList(0).CastToScalar(dVal))
			_bDataDim = true;
		else
			_bDataDim = false;

		iStep = rT.GetIdxSteps()[iDim];
		for( iIdx = 0, iNewPos = iDataPos; iIdx < iCount; iIdx++, iNewPos += iStep )
		{
			if (!VarList2Tensor(rT, rList(iIdx), iDim+1, iNewPos, _bDataDim, iLine, iPos))
				return false;
		}
	}
	else if (rVar.CastToScalar(dVal))
	{
		if (iDim != rT.Valence())
		{
			m_ErrorList.GeneralError("Variable list does not represent a tensor.", iLine, iPos);
			return false;
		}

		TCVScalar *pData = rT.Data();
		pData[iDataPos] = dVal;		
	}
	else
	{
		m_ErrorList.GeneralError("Invalid variable list component.", iLine, iPos);
		return false;
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Get Multivector from Tensor (along innermost index)

bool CCLUCodeBase::GetTensorMVFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	
	int iVarCount = mVars.Count();
	
	if (iVarCount == 1 || iVarCount == 2)
	{
		TTensor *pT, TDummy;

		if (mVars(0).BaseType() == PDT_TENSOR)
		{
			pT = mVars(0).GetTensorPtr();
		}
		else if (mVars(0).BaseType() == PDT_TENSOR_IDX)
		{
			TTensorIdx& rTIdx = *mVars(0).GetTensorIdxPtr();

			if (!MakeTensor(TDummy, rTIdx))
			{
				m_ErrorList.GeneralError("Error generating tensor.", iLine, iPos);
				return false;
			}
			pT = &TDummy;
		}
		else
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		TTensor &rT = *pT;

		int iValence = rT.Valence();
		if (iValence > 2)
		{
			m_ErrorList.GeneralError("Valence of tensor has to be 1 or 2.", iLine, iPos);
			return false;
		}

		TVarList mRet;
		MemObj<TMultiV> mvA;
		Mem<int> mMask;

		int iGADim;
		int iMVCount;
		int iDim, iMV;

		if (iValence == 1)
			iMVCount = 1;
		else
			iMVCount = rT.DimSize(1);

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
			iGADim = rT.DimSize(0);
			int i;
			
			mMask.Set(iGADim);
			for ( i = 0; i < iGADim; i++ )
			{
				mMask[i] = i+1;
			}
		}

		if (iGADim != 8 && iGADim != 16 && iGADim != 32 && iGADim != 64)
		{
			m_ErrorList.GeneralError("Tensor has invalid dimension. Expect 8, 16, 32 or 64.", iLine, iPos);
			return false;
		}

		int iMVTensorStep, iDimTensorStep, iTensorPos, iMaskVal;
		Mem<int> mIdxSteps = rT.GetIdxSteps();
		TCVScalar *pData = rT.Data();

		if (iValence == 1)
		{
			iMVTensorStep = 0;
			iDimTensorStep = mIdxSteps[0];
		}
		else
		{
			iMVTensorStep = mIdxSteps[1];
			iDimTensorStep = mIdxSteps[0];
		}
		iTensorPos = 0;

		mvA.Set(iMVCount);
		mRet.Set(iMVCount);
		for( iMV = 0; iMV < iMVCount; iMV++, iTensorPos += iMVTensorStep )
		{
			TMultiV &vA = mvA[iMV];

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
				iMaskVal = mMask[iDim];
				if (iMaskVal == 0)
					vA[iDim] = 0;
				else if (iMaskVal > 0)
					vA[iDim] = pData[(iMaskVal-1)*iDimTensorStep + iTensorPos];
				else
					vA[iDim] = -pData[(-iMaskVal-1)*iDimTensorStep + iTensorPos];
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
/// Get Tensor of multivector in algebra

bool CCLUCodeBase::GetMVTensorFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	
	bool bMVList = false;
	int iVarCount = mVars.Count();
	
	if (iVarCount == 1 || iVarCount == 2)
	{

		TTensor T;
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

/*
				if (iVal < 0)
				{
					m_ErrorList.GeneralError("Invalid mask. Negative index in mask.",
											iLine, iPos);
					return false;
				}
*/
				mMask[i] = iVal;
				if (Mag(iVal) > iRowDim)
					iRowDim = Mag(iVal);
			}

			if (iRowDim == 0)
			{
				m_ErrorList.GeneralError("Invalid mask. There is no non-zero value.",
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
			Mem<int> mDim;
			/*
			if (iMVCount == 1)
			{
				mDim.Set(2);
				mDim[0] = iRowDim;
				mDim[1] = 1;
			}
			else
			{*/
				mDim.Set(2);
				mDim[0] = iRowDim;
				mDim[1] = iMVCount;
			//}

			T.Reset(mDim);

			int iMVTensorStep, iDimTensorStep, iTensorPos, iMaskVal;
			Mem<int> mIdxSteps = T.GetIdxSteps();
			TCVScalar *pData = T.Data();
/*
			if (iMVCount == 1)
			{
				iMVTensorStep = 0;
				iDimTensorStep = mIdxSteps[0];
			}
			else
			{*/
				iMVTensorStep = mIdxSteps[1];
				iDimTensorStep = mIdxSteps[0];
			//}
			iTensorPos = 0;

			for (j = 0; j < iMVCount; j++, iTensorPos += iMVTensorStep)
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
					iMaskVal = mMask[i];
					if (iMaskVal > 0)
						pData[(iMaskVal-1)*iDimTensorStep + iTensorPos] = (*pvA)[i];
					else if (iMaskVal < 0)
						pData[(-iMaskVal-1)*iDimTensorStep + iTensorPos] = (*pvA)[i]; // also positive
				}
			}
		}
		else
		{
			int iMaskVal;
			Mem<int> mDim;
			mDim.Set(2);
			mDim[0] = iRowDim;
			mDim[1] = 1;
			T.Reset(mDim);
			TCVScalar *pData = T.Data();

			for( i = 0; i < iGADim; i++ )
			{
				iMaskVal = mMask[i];
				if (mMask[i] > 0)
					pData[iMaskVal-1] = (*pvA)[i];
				else if (mMask[i] < 0)
					pData[-iMaskVal-1] = (*pvA)[i];	// also positive
			}
		}

		rVar = T;
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}



//////////////////////////////////////////////////////////////////////
/// Get Operator Tensor of GA
/// 
/// Parameters:
///		1. GA Space
///		2. GA Operator
///		3. Left Mask (optional)
///		4. Right Mask (optional)
///		5. Result Mask (optional)
///
/// Returns:
///		Product tensor.
///

bool CCLUCodeBase::GetGAOpTensorFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	
	int iVarCount = mVars.Count();

	if (iVarCount != 2 && iVarCount != 4 && iVarCount != 5)
	{
		int piParNo[] = { 2, 4, 5 };
		m_ErrorList.WrongNoOfParams(piParNo, 3, iLine, iPos);
		return false;
	}

	int i, iGADim, iLeftDim, iRightDim;
	int iLeft, iRight, iPlane;
	int iLeftStep, iPlaneStep;
	int *piTable;
	bool bBinaryOp = true;
	Mem<int> mDim, mIdxStep, mLMask, mRMask, mResultMask;
	TCVScalar *pData;
	TCVCounter iGASpc, iGAOp;
	MultiVStyle<TCVScalar> *pStyle;

	if (!mVars(0).CastToCounter(iGASpc))
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToCounter(iGAOp))
	{
		m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}

	if (iGASpc == ID_E3GA)
	{
		pStyle = (MultiVStyle<TCVScalar> *) (&E3GA<TCVScalar>::GetMVStyle());
	}
	else if (iGASpc == ID_PGA)
	{
		pStyle = (MultiVStyle<TCVScalar> *) (&PGA<TCVScalar>::GetMVStyle());
	}
	else if (iGASpc == ID_CONFGA)
	{
		pStyle = (MultiVStyle<TCVScalar> *) (&ConfGA<TCVScalar>::GetMVStyle());
	}
	else if (iGASpc == ID_EL2GA)
	{
		pStyle = (MultiVStyle<TCVScalar> *) (&El2GA<TCVScalar>::GetMVStyle());
	}
	else
	{
		m_ErrorList.GeneralError("Invalid GA space ID.", iLine, iPos);
		return false;
	}

	iGADim = (int) pStyle->BasePtr()->GADim();
	iLeftDim = iRightDim = iGADim;

	switch (iGAOp)
	{
	case MVOP_GEO:
		piTable = pStyle->GPTable();
		bBinaryOp = true;
		break;

	case MVOP_INNER:
		piTable = pStyle->IPTable();
		bBinaryOp = true;
		break;

	case MVOP_OUTER:
		piTable = pStyle->OPTable();
		bBinaryOp = true;
		break;

	case MVOP_DUAL:
		piTable = pStyle->DualTable();
		bBinaryOp = false;
		break;

	case MVOP_REVERSE:
		piTable = pStyle->ReverseTable();
		bBinaryOp = false;
		break;

	case MVOP_INVOLUTE:
		piTable = pStyle->InvoTable();
		bBinaryOp = false;
		break;

	default:
		m_ErrorList.GeneralError("Given multivector operation not supported.", iLine, iPos);
		return false;
	}

	// Check whether masks are given
	if (bBinaryOp)
	{
		if (iVarCount == 4)
		{
			m_ErrorList.GeneralError("If masks are given, they have to be given for left, right and result multivectors.", iLine, iPos);
			return false;
		}

		mLMask.Set(iGADim);
		mRMask.Set(iGADim);

		if (iVarCount == 5)
		{
			if (mVars(2).BaseType() != PDT_VARLIST)
			{
				m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
				return false;
			}

			if (mVars(3).BaseType() != PDT_VARLIST)
			{
				m_ErrorList.InvalidParType(mVars(3), 4, iLine, iPos);
				return false;
			}

			TVarList &rLList = *mVars(2).GetVarListPtr();
			TVarList &rRList = *mVars(3).GetVarListPtr();
			
			if (iGADim != rLList.Count())
			{
				m_ErrorList.GeneralError("Invalid dimension of left mask.", iLine, iPos);
				return false;
			}
			
			if (iGADim != rRList.Count())
			{
				m_ErrorList.GeneralError("Invalid dimension of right mask.", iLine, iPos);
				return false;
			}
			
			int i, iVal;
			iLeftDim = 0;
			iRightDim = 0;
			for ( i = 0; i < iGADim; i++ )
			{
				if (!rLList[i].CastToCounter(iVal)) 
				{
					m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
					return false;			
				}

				mLMask[i] = iVal = Mag(iVal);
				if (iVal > iLeftDim)
					iLeftDim = iVal;

				if (!rRList[i].CastToCounter(iVal)) 
				{
					m_ErrorList.InvalidParType(mVars(3), 4, iLine, iPos);
					return false;			
				}

				mRMask[i] = iVal = Mag(iVal);
				if (iVal > iRightDim)
					iRightDim = iVal;
			}

			if (iLeftDim == 0)
			{
				m_ErrorList.GeneralError("Invalid left mask. There is no non-zero value.",
										iLine, iPos);
				return false;
			}

			if (iRightDim == 0)
			{
				m_ErrorList.GeneralError("Invalid right mask. There is no non-zero value.",
										iLine, iPos);
				return false;
			}
		}
		else // iVarCount == 2
		{
			int i;

			for (i = 0; i < iGADim; i++)
			{
				mLMask[i] = i+1;
				mRMask[i] = i+1;
			}
		}
	} 
	else // if bBinary
	{
		if (iVarCount == 5)
		{
			m_ErrorList.GeneralError("Only a single mask can be given for this type of product.", iLine, iPos);
			return false;
		}

		mLMask.Set(iGADim);

		if (iVarCount == 4)
		{
			if (mVars(2).BaseType() != PDT_VARLIST)
			{
				m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
				return false;
			}

			TVarList &rLList = *mVars(2).GetVarListPtr();
			
			if (iGADim != rLList.Count())
			{
				m_ErrorList.GeneralError("Invalid dimension of mask.", iLine, iPos);
				return false;
			}
			
			int i, iVal;
			iLeftDim = 0;
			for ( i = 0; i < iGADim; i++ )
			{
				if (!rLList[i].CastToCounter(iVal)) 
				{
					m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
					return false;			
				}

				mLMask[i] = iVal = Mag(iVal);
				if (iVal > iLeftDim)
					iLeftDim = iVal;
			}

			if (iLeftDim == 0)
			{
				m_ErrorList.GeneralError("Invalid mask. There is no non-zero value.",
										iLine, iPos);
				return false;
			}
		}
		else // iVarCount == 2
		{
			int i;

			for (i = 0; i < iGADim; i++)
			{
				mLMask[i] = i+1;
			}
		}
	} // if bBinary

	rVar.New(PDT_TENSOR, "Constant");
	TTensor &rT = *rVar.GetTensorPtr();

	if (bBinaryOp)
	{
		mDim.Set(3);
		mDim[0] = iGADim;
		mDim[1] = iLeftDim;
		mDim[2] = iRightDim;
	}
	else
	{
		mDim.Set(2);
		mDim[0] = iGADim;
		mDim[1] = iLeftDim;
	}

	rT.Reset(mDim);
	mIdxStep = rT.GetIdxSteps();
	iPlaneStep = mIdxStep[0];
	iLeftStep = mIdxStep[1];
	pData = rT.Data();

	mResultMask.Set(iGADim);
	for (i = 0; i < iGADim; i++)
		mResultMask[i] = 0;

	if (bBinaryOp)
	{
		for(iLeft = 0; iLeft < iGADim; iLeft++)
		{
			if (mLMask[iLeft] == 0)
				continue;

			for(iRight = 0; iRight < iGADim; iRight++)
			{
				if (mRMask[iRight] == 0)
					continue;

				if ((iPlane = piTable[iLeft*iGADim + iRight]) < 0)
				{ 
					mResultMask[(-iPlane-1)] = 1;
					pData[(-iPlane-1)*iPlaneStep + (mLMask[iLeft]-1)*iLeftStep + (mRMask[iRight]-1)] = TCVScalar(-1);
				}
				else if (iPlane > 0)
				{
					mResultMask[iPlane-1] = 1;
					pData[(iPlane-1)*iPlaneStep + (mLMask[iLeft]-1)*iLeftStep + (mRMask[iRight]-1)] = TCVScalar(1);
				}
			}
		}
	}
	else
	{
		for(iLeft = 0; iLeft < iGADim; iLeft++)
		{
			if (mLMask[iLeft] == 0)
				continue;

			if ((iPlane = piTable[iLeft]) < 0)
			{ 
				mResultMask[(-iPlane-1)] = 1;
				pData[(-iPlane-1)*iPlaneStep + (mLMask[iLeft]-1)] = TCVScalar(-1);
			}
			else if (iPlane > 0)
			{
				mResultMask[iPlane-1] = 1;
				pData[(iPlane-1)*iPlaneStep + (mLMask[iLeft]-1)] = TCVScalar(1);
			}
		}
	}

	if (iVarCount == 4 || iVarCount == 5)
	{
		bool bNewMask = false;
		CCodeVar &rMaskVar = mVars(iVarCount-1);

		// Only create new mask, if none is given.
		if (rMaskVar.BaseType() != PDT_VARLIST ||
			rMaskVar.GetVarListPtr()->Count() != iGADim)
		{
			rMaskVar.New(PDT_VARLIST);
			bNewMask = true;
		}

		TVarList &rList = *rMaskVar.GetVarListPtr();

		if (bNewMask)
		{
			rList.Set(iGADim);

			for (iPlane = 0, i = 1; iPlane < iGADim; iPlane++)
			{
				if (mResultMask[iPlane])
				{
					rList[iPlane] = i;
					mResultMask[iPlane] = i;
					++i;
				}
				else
					rList[iPlane] = 0;
			}
			mDim[0] = i-1;
		}
		else
		{
			int iVal, iMaxVal;

			for (iPlane = 0, iMaxVal = 0; iPlane < iGADim; iPlane++)
			{
				if (!rList[iPlane].CastToCounter(iVal))
				{
					m_ErrorList.GeneralError("Invalid index type in result mask.", iLine, iPos);
					return false;
				}

				iVal = Mag(iVal);
				if (iVal > iMaxVal)
					iMaxVal = iVal;

				mResultMask[iPlane] = iVal;
			}

			if (iMaxVal == 0)
			{
				m_ErrorList.GeneralError("Result mask is empty.", iLine, iPos);
				return false;
			}

			mDim[0] = iMaxVal;
		}

		TTensor Td( rT );
		TCVScalar *pSrcData = Td.Data();

		rT.Reset(mDim);
		pData = rT.Data();

		// Copy only those planes that are not completely zero
		int iVal;
		for (iPlane = 0; iPlane < iGADim; iPlane++)
		{
			if ( (iVal = mResultMask[iPlane]) )
			{
				memcpy(&pData[(iVal - 1)*iPlaneStep], &pSrcData[iPlane*iPlaneStep], iPlaneStep*sizeof(TCVScalar));
				//i++;
			}
		}
	}

	return true;
}



