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
// OPERATORS
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "GL/glut.h"

#include "CLUCodeBase.h"
#include "TensorSingleLoop.h"
#include "TensorContractLoop.h"
#include "TensorOperators.h"

bool CCLUCodeBase::OpAssign(CCodeVar& rLVar, CCodeVar& rRVar, int iLine, int iPos)
{ 
	// Do not assign pointers, only their content.

	if (&rLVar == &rRVar)
		return true;

	if (&rLVar == &(rRVar.DereferenceVarPtr(true)))
		return true;

	TCVScalar dVal;
	CCodeVar &_rRVar = rRVar.DereferenceVarPtr();
	
	if (rLVar.BaseType() != PDT_TENSOR_IDX &&
		rRVar.BaseType() == PDT_TENSOR_IDX)
	{
		CCodeVar &_rLVar = rLVar.DereferenceVarPtr();
		if (_rLVar.IsPtr())
		{
			m_ErrorList.GeneralError("Tensor cannot be assigned to given L-value.", iLine, iPos);
			return false;
		}

		TTensorIdx& rTIdx = *_rRVar.GetTensorIdxPtr();
		_rLVar.New(PDT_TENSOR);
		TTensor &rT = *_rLVar.GetTensorPtr();

		MakeTensor(rT, rTIdx);
	}
	else if (rLVar.BaseType() == PDT_TENSOR_IDX)
	{
		if (_rRVar.CastToScalar(dVal, m_fSensitivity))
		{
			TTensorIdx &rTIdx = *rLVar.GetTensorIdxPtr();

			SetTensorComps(rTIdx, dVal);
		}
		else if (_rRVar.BaseType() == PDT_TENSOR_IDX)
		{
			TTensorIdx &rTTrg = *rLVar.GetTensorIdxPtr();
			TTensorIdx &rTSrc = *_rRVar.GetTensorIdxPtr();

			if (!AssignTensorComps(rTTrg, rTSrc))
			{
				m_ErrorList.GeneralError("Incompatible tensor indices for assignment.", iLine, iPos);
				return false;
			}
		}
		else
		{
			m_ErrorList.GeneralError("Invalid assignment.", iLine, iPos);
			return false;
		}
	}
	else if (rLVar.BaseType() == PDT_VARLIST && 
			 _rRVar.BaseType() == PDT_VARLIST &&
			 rLVar.IsProtected())
	{
		CCodeVar *pVar;
		CCodeElementList *pCodeList;
		SCodeData CodeData;
		TVarList &rLList = *rLVar.GetVarListPtr();
		TVarList &rRList = *_rRVar.GetVarListPtr();

		if (rLList.Count() != rRList.Count())
		{
			m_ErrorList.GeneralError("Variable lists on left and right are not of equal length.", iLine, iPos);
			return false;
		}

		if ( !(pCodeList = rLList.GetCodeList()) )
		{
			m_ErrorList.GeneralError("Cannot assign values to left hand side elements.", iLine, iPos);
			return false;
		}

		LockStack();

		CodeData.Set(iLine, iPos, pCodeList);
		// DO NOT use as count the number of elements in the code list,
		// since this also includes as last element a call to 'CreateVarList'
		// which creates the variable list. However, we only want to execute
		// the codes for the elements of the left variable list.
		int iCode, iCount = rLList.Count(); // pCodeList->Count();
		for (iCode = 0; iCode < iCount; iCode++)
		{
			if ( !((*pCodeList)[iCode]->Apply(this, &CodeData)) )
			{
				UnlockStack();
				return false;
			}

			if (!Pop(pVar))
			{
				m_ErrorList.GeneralError("Element in left list did not return an allowed left value.", iLine, iPos);
				UnlockStack();
				return false;
			}

			if( pVar->IsProtected() && 
				!pVar->IsPtr() && 
				pVar->BaseType() != PDT_TENSOR_IDX &&
				pVar->BaseType() != PDT_VARLIST)
			{
				//pCodeBase->m_ErrorList.InvalidLVal(*pvL, iLine, iPos);
				m_ErrorList.GeneralError("Cannot modify a constant L-value.", iLine, iPos);
				return false;
			}

			// Call assign for each pair in the lists
			if (!OpAssign(*pVar, rRList[iCode], iLine, iPos))
			{
				UnlockStack();
				return false;
			}

			rLList[iCode] = pVar->DereferenceVarPtr(true);
		}

		while( Pop(pVar) ); // Clear Stack
		UnlockStack();
	}
	else
	{
		if (rLVar.IsPtr())
		{
			// Set Pointer content
			if (!rLVar.SetPtrContent(_rRVar))
			{
				m_ErrorList.InvalidRVal(_rRVar, iLine, iPos);
				return false;
			}
		}
		else
		{
			rLVar = _rRVar; 
			if (rLVar.Type() != PDT_PTR_VAR)
				rLVar.DereferencePtr();
		}
	}

	return true; 
}


//////////////////////////////////////////////////////////////////////
/// Reference Operator

bool CCLUCodeBase::OpReference(CCodeVar& rLVar, CCodeVar& rRVar, int iLine, int iPos)
{ 
	// This is just like the assign operator if the RHS is NOT a pointer to a var,
	// i.e. of type PDT_PTR_VAR.

	// An element always references itself.
	if (&rLVar == &rRVar)
		return true;

	if (&rLVar == &(rRVar.DereferenceVarPtr(true)))
		return true;

	if (rRVar.Type() == PDT_PTR_VAR)
	{
		rLVar = rRVar;
	}
	else
	{
		// Check whether variable from stack is actually a non-temporary
		// variable. If this is the case, then only store reference
		// to variable in list.
		if (GetVar(rRVar.Name().Str()).Type() == PDT_NOTYPE)
		{
			rLVar = rRVar;
			rLVar.DereferencePtr();
		}
		else
		{
			rLVar = &rRVar;
		}
	}

	return true; 
}


//////////////////////////////////////////////////////////////////////
/// Left Shift
///
bool CCLUCodeBase::OpLShift(CCodeVar& _rLVar, CCodeVar& _rRVar, int iLine, int iPos)
{
	TMatrix xRes;
	TMultiV vRes;
	TCVScalar dVal;
	ECodeDataType eLType, eRType;

	CCodeVar &rLVar = _rLVar.DereferenceVarPtr(true);
	CCodeVar &rRVar = _rRVar.DereferenceVarPtr(true);

	eLType = rLVar.Type();
	eRType = rRVar.Type();

	if (eLType == PDT_VARLIST)
	{
		if (eRType == PDT_VARLIST)
		{
			TVarList &rLList = *rLVar.GetVarListPtr();
			TVarList &rRList = *rRVar.GetVarListPtr();

			int i, iLCount = rLList.Count(), iRCount = rRList.Count();

			if (iRCount > 0)
			{
				rLList.Add(iRCount);
				for( i = 0; i < iRCount; i++ )
				{
					rLList[iLCount + i] = rRList[i];
				}
			}
		}
		// Need to do this, since rRVar could be PDT_PTR_DOUBLE
		else if (rRVar.CastToScalar(dVal, m_fSensitivity))
		{
			TVarList &rLList = *rLVar.GetVarListPtr();

			rLList.Add(1);
			rLList.Last() = dVal;
		}
		else if (eRType != PDT_NOTYPE)
		{
			TVarList &rLList = *rLVar.GetVarListPtr();

			rLList.Add(1);
			rLList.Last() = rRVar;
		}
		else
		{
			m_ErrorList.InvalidRVal(rLVar, iLine, iPos);
			return false;
		}
	}
	else if (eLType == PDT_PTR_VAR)
	{
		CCodeVar& rVar = rLVar.DereferenceVarPtr(); //**((CCodeVar **) rLVar.Val());
		if (!OpLShift(rVar, rRVar, iLine, iPos))
			return false;
	}
	else if (eLType == PDT_STRING)
	{
		CCodeVar rResVar;

		if (!OpAdd(rLVar, rRVar, rResVar, iLine, iPos))
			return false;

		rLVar = rResVar;
	}
	else if (eLType != PDT_NOTYPE && !rLVar.IsPtr())
	{
		if (eRType == PDT_VARLIST)
		{
			TVarList &rRList = *rRVar.GetVarListPtr();

			rRList.Insert(0);
			rRList[0] = rLVar;
			rLVar = rRList;
		}
		// Need to do this, since rRVar could be PDT_PTR_DOUBLE
		else if (rRVar.CastToScalar(dVal, m_fSensitivity))
		{
			TVarList rList;

			rList.Set(2);
			rList[1] = dVal;

			// Need to do this, since rLVar could be PDT_PTR_DOUBLE
			if (rLVar.CastToScalar(dVal, m_fSensitivity))
				rList[0] = dVal;
			else
				rList[0] = rLVar;

			rLVar = rList;
		}
		else if (eRType != PDT_NOTYPE)
		{
			TVarList rList;

			rList.Set(2);
			rList[0] = rLVar;
			rList[1] = rRVar;
			rLVar = rList;
		}
		else
		{
			m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
			return false;
		}
	}
	else
	{
		m_ErrorList.InvalidLVal(rRVar, iLine, iPos);
		return false;
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Greater Than Operator >
///
bool CCLUCodeBase::OpGreater(CCodeVar& _rLVar, CCodeVar& _rRVar, CCodeVar& rResVar, int iLine, int iPos)
{
	TCVScalar fLVal, fRVal;

	CCodeVar &rLVar = _rLVar.DereferenceVarPtr(true);
	CCodeVar &rRVar = _rRVar.DereferenceVarPtr(true);

	if (rLVar.CastToScalar(fLVal, m_fSensitivity))
	{
		if (!rRVar.CastToScalar(fRVal, m_fSensitivity))
		{
			m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
			return false;
		}

		if (fLVal > fRVal)
			rResVar = int(1);
		else
			rResVar = int(0);
	}
	// Apply test to each element of matrix
	else if (rLVar.Type() == PDT_MATRIX)
	{
		if (!rRVar.CastToScalar(fRVal, m_fSensitivity))
		{
			m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
			return false;
		}

		int iPos, iCount, iRows, iCols;
		TMatrix &xT = *rLVar.GetMatrixPtr();
		
		rResVar.New(PDT_MATRIX, "Constant");
		TMatrix &xR = *rResVar.GetMatrixPtr();

		iRows = xT.Rows();
		iCols = xT.Cols();
		xR.Resize(iRows, iCols);

		TCVScalar *pT = xT.Data();
		TCVScalar *pR = xR.Data();

		iCount = iRows * iCols;
		for(iPos = 0; iPos < iCount; iPos++)
		{
			if (pT[iPos] > fRVal)
				pR[iPos] = TCVScalar(1);
		}
	}
	else
	{
		m_ErrorList.InvalidLVal(rLVar, iLine, iPos);
		return false;
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Less Than Operator <
///
bool CCLUCodeBase::OpLess(CCodeVar& _rLVar, CCodeVar& _rRVar, CCodeVar& rResVar, int iLine, int iPos)
{
	TCVScalar fLVal, fRVal;

	CCodeVar &rLVar = _rLVar.DereferenceVarPtr(true);
	CCodeVar &rRVar = _rRVar.DereferenceVarPtr(true);

	if (rLVar.CastToScalar(fLVal, m_fSensitivity))
	{
		if (!rRVar.CastToScalar(fRVal, m_fSensitivity))
		{
			m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
			return false;
		}

		if (fLVal < fRVal)
			rResVar = int(1);
		else
			rResVar = int(0);
	}
	// Apply test to each element of matrix
	else if (rLVar.BaseType() == PDT_MATRIX)
	{
		if (!rRVar.CastToScalar(fRVal, m_fSensitivity))
		{
			m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
			return false;
		}

		int iPos, iCount, iRows, iCols;
		TMatrix &xT = *rLVar.GetMatrixPtr();
		
		rResVar.New(PDT_MATRIX, "Constant");
		TMatrix &xR = *rResVar.GetMatrixPtr();

		iRows = xT.Rows();
		iCols = xT.Cols();
		xR.Resize(iRows, iCols);

		TCVScalar *pT = xT.Data();
		TCVScalar *pR = xR.Data();

		iCount = iRows * iCols;
		for(iPos = 0; iPos < iCount; iPos++)
		{
			if (pT[iPos] < fRVal)
				pR[iPos] = TCVScalar(1);
		}
	}
	else
	{
		m_ErrorList.InvalidLVal(rLVar, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Greater Equal Than Operator >=
///
bool CCLUCodeBase::OpGreaterEqual(CCodeVar& _rLVar, CCodeVar& _rRVar, CCodeVar& rResVar, int iLine, int iPos)
{
	TCVScalar fLVal, fRVal;

	CCodeVar &rLVar = _rLVar.DereferenceVarPtr(true);
	CCodeVar &rRVar = _rRVar.DereferenceVarPtr(true);

	if (rLVar.CastToScalar(fLVal, m_fSensitivity))
	{
		if (!rRVar.CastToScalar(fRVal, m_fSensitivity))
		{
			m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
			return false;
		}

		if (fLVal >= fRVal)
			rResVar = int(1);
		else
			rResVar = int(0);
	}
	// Apply test to each element of matrix
	else if (rLVar.BaseType() == PDT_MATRIX)
	{
		if (!rRVar.CastToScalar(fRVal, m_fSensitivity))
		{
			m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
			return false;
		}

		int iPos, iCount, iRows, iCols;
		TMatrix &xT = *rLVar.GetMatrixPtr();
		
		rResVar.New(PDT_MATRIX, "Constant");
		TMatrix &xR = *rResVar.GetMatrixPtr();

		iRows = xT.Rows();
		iCols = xT.Cols();
		xR.Resize(iRows, iCols);

		TCVScalar *pT = xT.Data();
		TCVScalar *pR = xR.Data();

		iCount = iRows * iCols;
		for(iPos = 0; iPos < iCount; iPos++)
		{
			if (pT[iPos] >= fRVal)
				pR[iPos] = TCVScalar(1);
		}
	}
	else
	{
		m_ErrorList.InvalidLVal(rLVar, iLine, iPos);
		return false;
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Less Equal Than Operator <=
///
bool CCLUCodeBase::OpLessEqual(CCodeVar& _rLVar, CCodeVar& _rRVar, CCodeVar& rResVar, int iLine, int iPos)
{
	TCVScalar fLVal, fRVal;

	CCodeVar &rLVar = _rLVar.DereferenceVarPtr(true);
	CCodeVar &rRVar = _rRVar.DereferenceVarPtr(true);

	if (rLVar.CastToScalar(fLVal, m_fSensitivity))
	{
		if (!rRVar.CastToScalar(fRVal, m_fSensitivity))
		{
			m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
			return false;
		}

		if (fLVal <= fRVal)
			rResVar = int(1);
		else
			rResVar = int(0);
	}
	// Apply test to each element of matrix
	else if (rLVar.BaseType() == PDT_MATRIX)
	{
		if (!rRVar.CastToScalar(fRVal, m_fSensitivity))
		{
			m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
			return false;
		}

		int iPos, iCount, iRows, iCols;
		TMatrix &xT = *rLVar.GetMatrixPtr();
		
		rResVar.New(PDT_MATRIX, "Constant");
		TMatrix &xR = *rResVar.GetMatrixPtr();

		iRows = xT.Rows();
		iCols = xT.Cols();
		xR.Resize(iRows, iCols);

		TCVScalar *pT = xT.Data();
		TCVScalar *pR = xR.Data();

		iCount = iRows * iCols;
		for(iPos = 0; iPos < iCount; iPos++)
		{
			if (pT[iPos] <= fRVal)
				pR[iPos] = TCVScalar(1);
		}
	}
	else
	{
		m_ErrorList.InvalidLVal(rLVar, iLine, iPos);
		return false;
	}
	
	return true;
}



//////////////////////////////////////////////////////////////////////
/// NOT Operator !!
///
bool CCLUCodeBase::OpNOT(CCodeVar& _rVar, CCodeVar& rResVar, int iLine, int iPos)						  
{
	TCVScalar fVal;

	CCodeVar &rVar = _rVar.DereferenceVarPtr(true);

	if (rVar.CastToScalar(fVal, m_fSensitivity))
	{
		rResVar.New(PDT_INT, "Constant");
		if (fVal)
			rResVar = int(0);
		else
			rResVar = int(1);
	}
	// Apply NOT to all elements of matrix
	else if (rVar.BaseType() == PDT_MATRIX)
	{
		int iPos, iCount, iRows, iCols;
		TMatrix &xT = *rVar.GetMatrixPtr();
		
		rResVar.New(PDT_MATRIX, "Constant");
		TMatrix &xR = *rResVar.GetMatrixPtr();

		iRows = xT.Rows();
		iCols = xT.Cols();
		xR.Resize(iRows, iCols);

		TCVScalar *pT = xT.Data();
		TCVScalar *pR = xR.Data();

		iCount = iRows * iCols;
		for(iPos = 0; iPos < iCount; iPos++)
		{
			if (pT[iPos] != TCVScalar(0))
				pR[iPos] = TCVScalar(0);
			else
				pR[iPos] = TCVScalar(1);
		}
	}
	else
	{
		cStr csOp = "NOT operator '!!'";
		m_ErrorList.InvalidTypeOp(rVar, csOp, iLine, iPos);
		return false;
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// AND Operator &&
///
bool CCLUCodeBase::OpAND(CCodeVar& _rLVar, CCodeVar& _rRVar, CCodeVar& rResVar, int iLine, int iPos)
{
	TCVScalar fLVal, fRVal;

	CCodeVar &rLVar = _rLVar.DereferenceVarPtr(true);
	CCodeVar &rRVar = _rRVar.DereferenceVarPtr(true);

	if (rLVar.CastToScalar(fLVal, m_fSensitivity))
	{
		if (!rRVar.CastToScalar(fRVal, m_fSensitivity))
		{
			m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
			return false;
		}

		if (fLVal && fRVal)
			rResVar = int(1);
		else
			rResVar = int(0);
	}
	// Apply test to each element of matrix
	else if (rLVar.BaseType() == PDT_MATRIX)
	{
		if (!rRVar.CastToScalar(fRVal, m_fSensitivity))
		{
			m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
			return false;
		}

		int iPos, iCount, iRows, iCols;
		TMatrix &xT = *rLVar.GetMatrixPtr();
		
		rResVar.New(PDT_MATRIX, "Constant");
		TMatrix &xR = *rResVar.GetMatrixPtr();

		iRows = xT.Rows();
		iCols = xT.Cols();
		xR.Resize(iRows, iCols);

		TCVScalar *pT = xT.Data();
		TCVScalar *pR = xR.Data();

		iCount = iRows * iCols;
		for(iPos = 0; iPos < iCount; iPos++)
		{
			if (pT[iPos] && fRVal)
				pR[iPos] = TCVScalar(1);
		}
	}
	else
	{
		m_ErrorList.InvalidLVal(rLVar, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Point AND Operator .&&
/// Element by element ANDing of lists and matrices

bool CCLUCodeBase::OpPointAND(CCodeVar& _rLVar, CCodeVar& _rRVar, CCodeVar& rResVar, int iLine, int iPos)
{
	CCodeVar &rLVar = _rLVar.DereferenceVarPtr(true);
	CCodeVar &rRVar = _rRVar.DereferenceVarPtr(true);

	// Apply test to each element of matrices (element by element)
	if (rLVar.BaseType() == PDT_MATRIX && rRVar.BaseType() == PDT_MATRIX)
	{
		int iIdx, iCount, iRows, iCols;
		TMatrix &xL = *rLVar.GetMatrixPtr();
		TMatrix &xR = *rRVar.GetMatrixPtr();
		
		rResVar.New(PDT_MATRIX, "Constant");
		TMatrix &xRes = *rResVar.GetMatrixPtr();

		iRows = xL.Rows();
		iCols = xL.Cols();
		if (iRows != xR.Rows() || iCols != xR.Cols())
		{
			m_ErrorList.GeneralError("Matrices are not of same dimension.", iLine, iPos);
			return false;
		}

		xRes.Resize(iRows, iCols);

		TCVScalar *pL = xL.Data();
		TCVScalar *pR = xR.Data();
		TCVScalar *pRes = xRes.Data();

		iCount = iRows * iCols;
		for(iIdx = 0; iIdx < iCount; iIdx++)
		{
			if (pL[iIdx] && pR[iIdx])
				pRes[iIdx] = TCVScalar(1);
		}
	}
	// Apply test to each element of lists (element by element)
	else if (rLVar.BaseType() == PDT_VARLIST && rRVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rL = *rLVar.GetVarListPtr();
		TVarList &rR = *rRVar.GetVarListPtr();

		if (rL.Count() != rR.Count())
		{
			m_ErrorList.GeneralError("Lists are not of same dimension.", iLine, iPos);
			return false;
		}

		int i, iCount = rL.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointAND(rL(i), rR(i), rRes(i), iLine, iPos))
				return false;
		}
	}
	else if (rLVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rL = *rLVar.GetVarListPtr();

		int i, iCount = rL.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointAND(rL(i), _rRVar, rRes(i), iLine, iPos))
				return false;
		}
	}
	else if (rRVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rR = *rRVar.GetVarListPtr();

		int i, iCount = rR.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointAND(_rLVar, rR(i), rRes(i), iLine, iPos))
				return false;
		}
	}
	else
	{
		return OpAND(_rLVar, _rRVar, rResVar, iLine, iPos);
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Point OR Operator .||
/// Element by element ORing of lists and matrices

bool CCLUCodeBase::OpPointOR(CCodeVar& _rLVar, CCodeVar& _rRVar, CCodeVar& rResVar, int iLine, int iPos)
{
	CCodeVar &rLVar = _rLVar.DereferenceVarPtr(true);
	CCodeVar &rRVar = _rRVar.DereferenceVarPtr(true);

	// Apply test to each element of matrices (element by element)
	if (rLVar.BaseType() == PDT_MATRIX && rRVar.BaseType() == PDT_MATRIX)
	{
		int iIdx, iCount, iRows, iCols;
		TMatrix &xL = *rLVar.GetMatrixPtr();
		TMatrix &xR = *rRVar.GetMatrixPtr();
		
		rResVar.New(PDT_MATRIX, "Constant");
		TMatrix &xRes = *rResVar.GetMatrixPtr();

		iRows = xL.Rows();
		iCols = xL.Cols();
		if (iRows != xR.Rows() || iCols != xR.Cols())
		{
			m_ErrorList.GeneralError("Matrices are not of same dimension.", iLine, iPos);
			return false;
		}

		xRes.Resize(iRows, iCols);

		TCVScalar *pL = xL.Data();
		TCVScalar *pR = xR.Data();
		TCVScalar *pRes = xRes.Data();

		iCount = iRows * iCols;
		for(iIdx = 0; iIdx < iCount; iIdx++)
		{
			if (pL[iIdx] || pR[iIdx])
				pRes[iIdx] = TCVScalar(1);
		}
	}
	// Apply test to each element of lists (element by element)
	else if (rLVar.BaseType() == PDT_VARLIST && rRVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rL = *rLVar.GetVarListPtr();
		TVarList &rR = *rRVar.GetVarListPtr();

		if (rL.Count() != rR.Count())
		{
			m_ErrorList.GeneralError("Lists are not of same dimension.", iLine, iPos);
			return false;
		}

		int i, iCount = rL.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointOR(rL(i), rR(i), rRes(i), iLine, iPos))
				return false;
		}
	}
	else if (rLVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rL = *rLVar.GetVarListPtr();

		int i, iCount = rL.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointOR(rL(i), _rRVar, rRes(i), iLine, iPos))
				return false;
		}
	}
	else if (rRVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rR = *rRVar.GetVarListPtr();

		int i, iCount = rR.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointOR(_rLVar, rR(i), rRes(i), iLine, iPos))
				return false;
		}
	}
	else
	{
		return OpOR(_rLVar, _rRVar, rResVar, iLine, iPos);
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Point equal Operator .==
/// Element by element Equality of lists and matrices

bool CCLUCodeBase::OpPointEqual(CCodeVar& _rLVar, CCodeVar& _rRVar, CCodeVar& rResVar, int iLine, int iPos)
{
	CCodeVar &rLVar = _rLVar.DereferenceVarPtr(true);
	CCodeVar &rRVar = _rRVar.DereferenceVarPtr(true);

	// Apply test to each element of matrices (element by element)
	if (rLVar.BaseType() == PDT_MATRIX && rRVar.BaseType() == PDT_MATRIX)
	{
		int iIdx, iCount, iRows, iCols;
		TMatrix &xL = *rLVar.GetMatrixPtr();
		TMatrix &xR = *rRVar.GetMatrixPtr();
		
		rResVar.New(PDT_MATRIX, "Constant");
		TMatrix &xRes = *rResVar.GetMatrixPtr();

		iRows = xL.Rows();
		iCols = xL.Cols();
		if (iRows != xR.Rows() || iCols != xR.Cols())
		{
			m_ErrorList.GeneralError("Matrices are not of same dimension.", iLine, iPos);
			return false;
		}

		xRes.Resize(iRows, iCols);

		TCVScalar *pL = xL.Data();
		TCVScalar *pR = xR.Data();
		TCVScalar *pRes = xRes.Data();

		iCount = iRows * iCols;
		for(iIdx = 0; iIdx < iCount; iIdx++)
		{
			if (pL[iIdx] == pR[iIdx])
				pRes[iIdx] = TCVScalar(1);
		}
	}
	// Apply test to each element of lists (element by element)
	else if (rLVar.BaseType() == PDT_VARLIST && rRVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rL = *rLVar.GetVarListPtr();
		TVarList &rR = *rRVar.GetVarListPtr();

		if (rL.Count() != rR.Count())
		{
			m_ErrorList.GeneralError("Lists are not of same dimension.", iLine, iPos);
			return false;
		}

		int i, iCount = rL.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointEqual(rL(i), rR(i), rRes(i), iLine, iPos))
				return false;
		}
	}
	else if (rLVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rL = *rLVar.GetVarListPtr();

		int i, iCount = rL.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointEqual(rL(i), _rRVar, rRes(i), iLine, iPos))
				return false;
		}
	}
	else if (rRVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rR = *rRVar.GetVarListPtr();

		int i, iCount = rR.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointEqual(_rLVar, rR(i), rRes(i), iLine, iPos))
				return false;
		}
	}
	else
	{
		return OpEqual(_rLVar, _rRVar, rResVar, iLine, iPos);
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Point Inequality Operator .!=
/// Element by element Inequality of lists and matrices

bool CCLUCodeBase::OpPointNotEqual(CCodeVar& _rLVar, CCodeVar& _rRVar, CCodeVar& rResVar, int iLine, int iPos)
{
	CCodeVar &rLVar = _rLVar.DereferenceVarPtr(true);
	CCodeVar &rRVar = _rRVar.DereferenceVarPtr(true);

	// Apply test to each element of matrices (element by element)
	if (rLVar.BaseType() == PDT_MATRIX && rRVar.BaseType() == PDT_MATRIX)
	{
		int iIdx, iCount, iRows, iCols;
		TMatrix &xL = *rLVar.GetMatrixPtr();
		TMatrix &xR = *rRVar.GetMatrixPtr();
		
		rResVar.New(PDT_MATRIX, "Constant");
		TMatrix &xRes = *rResVar.GetMatrixPtr();

		iRows = xL.Rows();
		iCols = xL.Cols();
		if (iRows != xR.Rows() || iCols != xR.Cols())
		{
			m_ErrorList.GeneralError("Matrices are not of same dimension.", iLine, iPos);
			return false;
		}

		xRes.Resize(iRows, iCols);

		TCVScalar *pL = xL.Data();
		TCVScalar *pR = xR.Data();
		TCVScalar *pRes = xRes.Data();

		iCount = iRows * iCols;
		for(iIdx = 0; iIdx < iCount; iIdx++)
		{
			if (pL[iIdx] != pR[iIdx])
				pRes[iIdx] = TCVScalar(1);
		}
	}
	// Apply test to each element of lists (element by element)
	else if (rLVar.BaseType() == PDT_VARLIST && rRVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rL = *rLVar.GetVarListPtr();
		TVarList &rR = *rRVar.GetVarListPtr();

		if (rL.Count() != rR.Count())
		{
			m_ErrorList.GeneralError("Lists are not of same dimension.", iLine, iPos);
			return false;
		}

		int i, iCount = rL.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointNotEqual(rL(i), rR(i), rRes(i), iLine, iPos))
				return false;
		}
	}
	else if (rLVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rL = *rLVar.GetVarListPtr();

		int i, iCount = rL.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointNotEqual(rL(i), _rRVar, rRes(i), iLine, iPos))
				return false;
		}
	}
	else if (rRVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rR = *rRVar.GetVarListPtr();

		int i, iCount = rR.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointNotEqual(_rLVar, rR(i), rRes(i), iLine, iPos))
				return false;
		}
	}
	else
	{
		return OpNotEqual(_rLVar, _rRVar, rResVar, iLine, iPos);
	}
	
	return true;
}



//////////////////////////////////////////////////////////////////////
/// Point Division Operator ./
/// Element by element division of lists and matrices

bool CCLUCodeBase::OpPointDiv(CCodeVar& _rLVar, CCodeVar& _rRVar, CCodeVar& rResVar, int iLine, int iPos)
{

	CCodeVar &rLVar = _rLVar.DereferenceVarPtr(true);
	CCodeVar &rRVar = _rRVar.DereferenceVarPtr(true);

	// Apply test to each element of matrices (element by element)
	if (rLVar.BaseType() == PDT_MATRIX && rRVar.BaseType() == PDT_MATRIX)
	{
		int iIdx, iCount, iRows, iCols;
		TMatrix &xL = *rLVar.GetMatrixPtr();
		TMatrix &xR = *rRVar.GetMatrixPtr();
		
		rResVar.New(PDT_MATRIX, "Constant");
		TMatrix &xRes = *rResVar.GetMatrixPtr();

		iRows = xL.Rows();
		iCols = xL.Cols();
		if (iRows != xR.Rows() || (xR.Cols() != 1 && iCols != xR.Cols()) )
		{
			m_ErrorList.GeneralError("Matrices are not of compatible dimension.", iLine, iPos);
			return false;
		}

		xRes.Resize(iRows, iCols);

		TCVScalar *pL = xL.Data();
		TCVScalar *pR = xR.Data();
		TCVScalar *pRes = xRes.Data();

		if (iCols > 1 && xR.Cols() == 1)
		{
			TCVScalar fVal;
			int iR, iC;
			iIdx = 0;
			for( iR = 0; iR < iRows; iR++ )
			{
				fVal = pR[iR];
				if (fVal == TCVScalar(0))
				{
					m_ErrorList.DivByZero(iLine, iPos);
					return false;
				}

				for( iC = 0; iC < iCols; iC++, iIdx++ )
				{
					pRes[iIdx] = pL[iIdx] / fVal;
				}
			}
		}
		else
		{
			iCount = iRows * iCols;
			for(iIdx = 0; iIdx < iCount; iIdx++)
			{
				if (pR[iIdx] == TCVScalar(0))
				{
					m_ErrorList.DivByZero(iLine, iPos);
					return false;
				}
				else
					pRes[iIdx] = pL[iIdx] / pR[iIdx];
			}
		}
	}
	else if (rLVar.BaseType() == PDT_TENSOR_IDX && rRVar.BaseType() == PDT_TENSOR_IDX)
	{
		TTensorIdx &rLTIdx = *rLVar.GetTensorIdxPtr();
		TTensorIdx &rRTIdx = *rRVar.GetTensorIdxPtr();
			
		// Create a temporary variable that contains tensor.
		// Do this, so that we can return a tensor index.
		m_mTempVarList.Add(1);
		CCodeVar &rTemp = m_mTempVarList.Last();
		rTemp.New(PDT_TENSOR, "Constant");
		TTensor& rT = *rTemp.GetTensorPtr();

		rResVar.New(PDT_TENSOR_IDX, "Constant");
		TTensorIdx& rTIdx = *rResVar.GetTensorIdxPtr();

		if (!TensorDivPoint(rT, rTIdx, rLTIdx, rRTIdx))
		{
			m_ErrorList.GeneralError("Error dividing tensors componentwise.", iLine, iPos);
			return false;
		}
	}
	// Apply test to each element of lists (element by element)
	else if (rLVar.BaseType() == PDT_VARLIST && rRVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rL = *rLVar.GetVarListPtr();
		TVarList &rR = *rRVar.GetVarListPtr();

		if (rL.Count() != rR.Count())
		{
			m_ErrorList.GeneralError("Lists are not of same dimension.", iLine, iPos);
			return false;
		}

		int i, iCount = rL.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointDiv(rL(i), rR(i), rRes(i), iLine, iPos))
				return false;
		}
	}
	else if (rLVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rL = *rLVar.GetVarListPtr();

		int i, iCount = rL.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointDiv(rL(i), _rRVar, rRes(i), iLine, iPos))
				return false;
		}
	}
	else if (rRVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rR = *rRVar.GetVarListPtr();

		int i, iCount = rR.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointDiv(_rLVar, rR(i), rRes(i), iLine, iPos))
				return false;
		}
	}
	else
	{
		// Standard division
		return OpDiv(_rLVar, _rRVar, rResVar, iLine, iPos);
	}
	
	return true;
}



//////////////////////////////////////////////////////////////////////
/// Point Product Operator .*
/// Element by element division of lists and matrices

bool CCLUCodeBase::OpPointProd(CCodeVar& _rLVar, CCodeVar& _rRVar, CCodeVar& rResVar, int iLine, int iPos)
{

	CCodeVar &rLVar = _rLVar.DereferenceVarPtr(true);
	CCodeVar &rRVar = _rRVar.DereferenceVarPtr(true);

	// Apply test to each element of matrices (element by element)
	if (rLVar.BaseType() == PDT_MATRIX && rRVar.BaseType() == PDT_MATRIX)
	{
		int iIdx, iCount, iRows, iCols;
		TMatrix &xL = *rLVar.GetMatrixPtr();
		TMatrix &xR = *rRVar.GetMatrixPtr();
		
		rResVar.New(PDT_MATRIX, "Constant");
		TMatrix &xRes = *rResVar.GetMatrixPtr();

		iRows = xL.Rows();
		iCols = xL.Cols();
		if (iRows != xR.Rows() || (xR.Cols() != 1 && iCols != xR.Cols()) )
		{
			m_ErrorList.GeneralError("Matrices are not of compatible dimensions.", iLine, iPos);
			return false;
		}

		xRes.Resize(iRows, iCols);

		TCVScalar *pL = xL.Data();
		TCVScalar *pR = xR.Data();
		TCVScalar *pRes = xRes.Data();

		if (iCols > 1 && xR.Cols() == 1)
		{
			TCVScalar fVal;
			int iR, iC;
			iIdx = 0;
			for( iR = 0; iR < iRows; iR++ )
			{
				fVal = pR[iR];
				for( iC = 0; iC < iCols; iC++, iIdx++ )
				{
					pRes[iIdx] = pL[iIdx] * fVal;
				}
			}
		}
		else
		{
			iCount = iRows * iCols;
			for(iIdx = 0; iIdx < iCount; iIdx++)
			{
				pRes[iIdx] = pL[iIdx] * pR[iIdx];
			}
		}
	}
	else if (rLVar.BaseType() == PDT_TENSOR_IDX && rRVar.BaseType() == PDT_TENSOR_IDX)
	{
		TTensorIdx &rLTIdx = *rLVar.GetTensorIdxPtr();
		TTensorIdx &rRTIdx = *rRVar.GetTensorIdxPtr();

		// Create a temporary variable that contains tensor.
		// Do this, so that we can return a tensor index.
		m_mTempVarList.Add(1);
		CCodeVar &rTemp = m_mTempVarList.Last();
		rTemp.New(PDT_TENSOR, "Constant");
		TTensor& rT = *rTemp.GetTensorPtr();

		rResVar.New(PDT_TENSOR_IDX, "Constant");
		TTensorIdx& rTIdx = *rResVar.GetTensorIdxPtr();

		if (!TensorProductPoint(rT, rTIdx, rLTIdx, rRTIdx))
		{
			m_ErrorList.GeneralError("Error multiplying tensors.", iLine, iPos);
			return false;
		}
	}
	// Apply test to each element of lists (element by element)
	else if (rLVar.BaseType() == PDT_VARLIST && rRVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rL = *rLVar.GetVarListPtr();
		TVarList &rR = *rRVar.GetVarListPtr();

		if (rL.Count() != rR.Count())
		{
			m_ErrorList.GeneralError("Lists are not of same dimension.", iLine, iPos);
			return false;
		}

		int i, iCount = rL.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointProd(rL(i), rR(i), rRes(i), iLine, iPos))
				return false;
		}
	}
	else if (rLVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rL = *rLVar.GetVarListPtr();

		int i, iCount = rL.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointProd(rL(i), _rRVar, rRes(i), iLine, iPos))
				return false;
		}
	}
	else if (rRVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rR = *rRVar.GetVarListPtr();

		int i, iCount = rR.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointProd(_rLVar, rR(i), rRes(i), iLine, iPos))
				return false;
		}
	}
	else
	{
		// Standard division
		return OpProd(_rLVar, _rRVar, rResVar, iLine, iPos);
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Point Inner Product Operator ..
/// Element by element division of lists and matrices

bool CCLUCodeBase::OpPointInnerProd(CCodeVar& _rLVar, CCodeVar& _rRVar, CCodeVar& rResVar, int iLine, int iPos)
{

	CCodeVar &rLVar = _rLVar.DereferenceVarPtr(true);
	CCodeVar &rRVar = _rRVar.DereferenceVarPtr(true);

	// Apply test to each element of matrices (element by element)
	if (rLVar.BaseType() == PDT_MATRIX && rRVar.BaseType() == PDT_MATRIX)
	{
		int iIdx, iCount, iRows, iCols;
		TMatrix &xL = *rLVar.GetMatrixPtr();
		TMatrix &xR = *rRVar.GetMatrixPtr();
		
		rResVar.New(PDT_MATRIX, "Constant");
		TMatrix &xRes = *rResVar.GetMatrixPtr();

		iRows = xL.Rows();
		iCols = xL.Cols();
		if (iRows != xR.Rows() || iCols != xR.Cols())
		{
			m_ErrorList.GeneralError("Matrices are not of same dimension.", iLine, iPos);
			return false;
		}

		xRes.Resize(iRows, iCols);

		TCVScalar *pL = xL.Data();
		TCVScalar *pR = xR.Data();
		TCVScalar *pRes = xRes.Data();

		iCount = iRows * iCols;
		for(iIdx = 0; iIdx < iCount; iIdx++)
		{
			pRes[iIdx] = 0;
		}
	}
	// Apply test to each element of lists (element by element)
	else if (rLVar.BaseType() == PDT_VARLIST && rRVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rL = *rLVar.GetVarListPtr();
		TVarList &rR = *rRVar.GetVarListPtr();

		if (rL.Count() != rR.Count())
		{
			m_ErrorList.GeneralError("Lists are not of same dimension.", iLine, iPos);
			return false;
		}

		int i, iCount = rL.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointInnerProd(rL(i), rR(i), rRes(i), iLine, iPos))
				return false;
		}
	}
	else if (rLVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rL = *rLVar.GetVarListPtr();

		int i, iCount = rL.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointInnerProd(rL(i), _rRVar, rRes(i), iLine, iPos))
				return false;
		}
	}
	else if (rRVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rR = *rRVar.GetVarListPtr();

		int i, iCount = rR.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointInnerProd(_rLVar, rR(i), rRes(i), iLine, iPos))
				return false;
		}
	}
	else
	{
		// Standard inner product
		return OpInnerProd(_rLVar, _rRVar, rResVar, iLine, iPos);
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Point Wedge Operator .^
/// Element by element outer product of lists and matrices

bool CCLUCodeBase::OpPointWedge(CCodeVar& _rLVar, CCodeVar& _rRVar, CCodeVar& rResVar, int iLine, int iPos)
{

	CCodeVar &rLVar = _rLVar.DereferenceVarPtr(true);
	CCodeVar &rRVar = _rRVar.DereferenceVarPtr(true);

	// Apply test to each element of matrices (element by element)
	if (rLVar.BaseType() == PDT_MATRIX && rRVar.BaseType() == PDT_MATRIX)
	{
		int iIdx, iCount, iRows, iCols;
		TMatrix &xL = *rLVar.GetMatrixPtr();
		TMatrix &xR = *rRVar.GetMatrixPtr();
		
		rResVar.New(PDT_MATRIX, "Constant");
		TMatrix &xRes = *rResVar.GetMatrixPtr();

		iRows = xL.Rows();
		iCols = xL.Cols();
		if (iRows != xR.Rows() || iCols != xR.Cols())
		{
			m_ErrorList.GeneralError("Matrices are not of same dimension.", iLine, iPos);
			return false;
		}

		xRes.Resize(iRows, iCols);

		TCVScalar *pL = xL.Data();
		TCVScalar *pR = xR.Data();
		TCVScalar *pRes = xRes.Data();

		iCount = iRows * iCols;
		for(iIdx = 0; iIdx < iCount; iIdx++)
		{
			pRes[iIdx] = pL[iIdx] * pR[iIdx];
		}
	}
	// Apply test to each element of lists (element by element)
	else if (rLVar.BaseType() == PDT_VARLIST && rRVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rL = *rLVar.GetVarListPtr();
		TVarList &rR = *rRVar.GetVarListPtr();

		if (rL.Count() != rR.Count())
		{
			m_ErrorList.GeneralError("Lists are not of same dimension.", iLine, iPos);
			return false;
		}

		int i, iCount = rL.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointWedge(rL(i), rR(i), rRes(i), iLine, iPos))
				return false;
		}
	}
	else if (rLVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rL = *rLVar.GetVarListPtr();

		int i, iCount = rL.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointWedge(rL(i), _rRVar, rRes(i), iLine, iPos))
				return false;
		}
	}
	else if (rRVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rR = *rRVar.GetVarListPtr();

		int i, iCount = rR.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointWedge(_rLVar, rR(i), rRes(i), iLine, iPos))
				return false;
		}
	}
	else
	{
		// Standard inner product
		return OpWedge(_rLVar, _rRVar, rResVar, iLine, iPos);
	}
	
	return true;
}




//////////////////////////////////////////////////////////////////////
/// Point Subtract Operator .-
/// Element by element division of lists and matrices

bool CCLUCodeBase::OpPointSubtract(CCodeVar& _rLVar, CCodeVar& _rRVar, CCodeVar& rResVar, int iLine, int iPos)
{

	CCodeVar &rLVar = _rLVar.DereferenceVarPtr(true);
	CCodeVar &rRVar = _rRVar.DereferenceVarPtr(true);

	// Apply test to each element of matrices (element by element)
	if (rLVar.BaseType() == PDT_MATRIX && rRVar.BaseType() == PDT_MATRIX)
	{
		int iIdx, iCount, iRows, iCols;
		TMatrix &xL = *rLVar.GetMatrixPtr();
		TMatrix &xR = *rRVar.GetMatrixPtr();
		
		rResVar.New(PDT_MATRIX, "Constant");
		TMatrix &xRes = *rResVar.GetMatrixPtr();

		iRows = xL.Rows();
		iCols = xL.Cols();
		if (iRows != xR.Rows() || iCols != xR.Cols())
		{
			m_ErrorList.GeneralError("Matrices are not of same dimension.", iLine, iPos);
			return false;
		}

		xRes.Resize(iRows, iCols);

		TCVScalar *pL = xL.Data();
		TCVScalar *pR = xR.Data();
		TCVScalar *pRes = xRes.Data();

		iCount = iRows * iCols;
		for(iIdx = 0; iIdx < iCount; iIdx++)
		{
			pRes[iIdx] = pL[iIdx] - pR[iIdx];
		}
	}
	// Apply test to each element of lists (element by element)
	else if (rLVar.BaseType() == PDT_VARLIST && rRVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rL = *rLVar.GetVarListPtr();
		TVarList &rR = *rRVar.GetVarListPtr();

		if (rL.Count() != rR.Count())
		{
			m_ErrorList.GeneralError("Lists are not of same dimension.", iLine, iPos);
			return false;
		}

		int i, iCount = rL.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointSubtract(rL(i), rR(i), rRes(i), iLine, iPos))
				return false;
		}
	}
	else if (rLVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rL = *rLVar.GetVarListPtr();

		int i, iCount = rL.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointSubtract(rL(i), _rRVar, rRes(i), iLine, iPos))
				return false;
		}
	}
	else if (rRVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rR = *rRVar.GetVarListPtr();

		int i, iCount = rR.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointSubtract(_rLVar, rR(i), rRes(i), iLine, iPos))
				return false;
		}
	}
	else
	{
		// Standard inner product
		return OpSubtract(_rLVar, _rRVar, rResVar, iLine, iPos);
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Point Add Operator .+
/// Element by element division of lists and matrices

bool CCLUCodeBase::OpPointAdd(CCodeVar& _rLVar, CCodeVar& _rRVar, CCodeVar& rResVar, int iLine, int iPos)
{

	CCodeVar &rLVar = _rLVar.DereferenceVarPtr(true);
	CCodeVar &rRVar = _rRVar.DereferenceVarPtr(true);

	// Apply test to each element of matrices (element by element)
	if (rLVar.BaseType() == PDT_MATRIX && rRVar.BaseType() == PDT_MATRIX)
	{
		int iIdx, iCount, iRows, iCols;
		TMatrix &xL = *rLVar.GetMatrixPtr();
		TMatrix &xR = *rRVar.GetMatrixPtr();
		
		rResVar.New(PDT_MATRIX, "Constant");
		TMatrix &xRes = *rResVar.GetMatrixPtr();

		iRows = xL.Rows();
		iCols = xL.Cols();
		if (iRows != xR.Rows() || iCols != xR.Cols())
		{
			m_ErrorList.GeneralError("Matrices are not of same dimension.", iLine, iPos);
			return false;
		}

		xRes.Resize(iRows, iCols);

		TCVScalar *pL = xL.Data();
		TCVScalar *pR = xR.Data();
		TCVScalar *pRes = xRes.Data();

		iCount = iRows * iCols;
		for(iIdx = 0; iIdx < iCount; iIdx++)
		{
			pRes[iIdx] = pL[iIdx] + pR[iIdx];
		}
	}
	// Apply test to each element of lists (element by element)
	else if (rLVar.BaseType() == PDT_VARLIST && rRVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rL = *rLVar.GetVarListPtr();
		TVarList &rR = *rRVar.GetVarListPtr();

		if (rL.Count() != rR.Count())
		{
			m_ErrorList.GeneralError("Lists are not of same dimension.", iLine, iPos);
			return false;
		}

		int i, iCount = rL.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointAdd(rL(i), rR(i), rRes(i), iLine, iPos))
				return false;
		}
	}
	else if (rLVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rL = *rLVar.GetVarListPtr();

		int i, iCount = rL.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointAdd(rL(i), _rRVar, rRes(i), iLine, iPos))
				return false;
		}
	}
	else if (rRVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rR = *rRVar.GetVarListPtr();

		int i, iCount = rR.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointAdd(_rLVar, rR(i), rRes(i), iLine, iPos))
				return false;
		}
	}
	else
	{
		// Standard inner product
		return OpAdd(_rLVar, _rRVar, rResVar, iLine, iPos);
	}
	
	return true;
}



//////////////////////////////////////////////////////////////////////
/// Point Greater Operator .>
/// Element by element division of lists and matrices

bool CCLUCodeBase::OpPointGreater(CCodeVar& _rLVar, CCodeVar& _rRVar, CCodeVar& rResVar, int iLine, int iPos)
{

	CCodeVar &rLVar = _rLVar.DereferenceVarPtr(true);
	CCodeVar &rRVar = _rRVar.DereferenceVarPtr(true);

	// Apply test to each element of matrices (element by element)
	if (rLVar.BaseType() == PDT_MATRIX && rRVar.BaseType() == PDT_MATRIX)
	{
		int iIdx, iCount, iRows, iCols;
		TMatrix &xL = *rLVar.GetMatrixPtr();
		TMatrix &xR = *rRVar.GetMatrixPtr();
		
		rResVar.New(PDT_MATRIX, "Constant");
		TMatrix &xRes = *rResVar.GetMatrixPtr();

		iRows = xL.Rows();
		iCols = xL.Cols();
		if (iRows != xR.Rows() || iCols != xR.Cols())
		{
			m_ErrorList.GeneralError("Matrices are not of same dimension.", iLine, iPos);
			return false;
		}

		xRes.Resize(iRows, iCols);

		TCVScalar *pL = xL.Data();
		TCVScalar *pR = xR.Data();
		TCVScalar *pRes = xRes.Data();

		iCount = iRows * iCols;
		for(iIdx = 0; iIdx < iCount; iIdx++)
		{
			if (pL[iIdx] > pR[iIdx])
				pRes[iIdx] = TCVScalar(1);
			else
				pRes[iIdx] = TCVScalar(0);
		}
	}
	// Apply test to each element of lists (element by element)
	else if (rLVar.BaseType() == PDT_VARLIST && rRVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rL = *rLVar.GetVarListPtr();
		TVarList &rR = *rRVar.GetVarListPtr();

		if (rL.Count() != rR.Count())
		{
			m_ErrorList.GeneralError("Lists are not of same dimension.", iLine, iPos);
			return false;
		}

		int i, iCount = rL.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointGreater(rL(i), rR(i), rRes(i), iLine, iPos))
				return false;
		}
	}
	else if (rLVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rL = *rLVar.GetVarListPtr();

		int i, iCount = rL.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointGreater(rL(i), _rRVar, rRes(i), iLine, iPos))
				return false;
		}
	}
	else if (rRVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rR = *rRVar.GetVarListPtr();

		int i, iCount = rR.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointGreater(_rLVar, rR(i), rRes(i), iLine, iPos))
				return false;
		}
	}
	else
	{
		// Standard operator
		return OpGreater(_rLVar, _rRVar, rResVar, iLine, iPos);
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Point Less Operator .<
/// Element by element Less test of lists and matrices

bool CCLUCodeBase::OpPointLess(CCodeVar& _rLVar, CCodeVar& _rRVar, CCodeVar& rResVar, int iLine, int iPos)
{

	CCodeVar &rLVar = _rLVar.DereferenceVarPtr(true);
	CCodeVar &rRVar = _rRVar.DereferenceVarPtr(true);

	// Apply test to each element of matrices (element by element)
	if (rLVar.BaseType() == PDT_MATRIX && rRVar.BaseType() == PDT_MATRIX)
	{
		int iIdx, iCount, iRows, iCols;
		TMatrix &xL = *rLVar.GetMatrixPtr();
		TMatrix &xR = *rRVar.GetMatrixPtr();
		
		rResVar.New(PDT_MATRIX, "Constant");
		TMatrix &xRes = *rResVar.GetMatrixPtr();

		iRows = xL.Rows();
		iCols = xL.Cols();
		if (iRows != xR.Rows() || iCols != xR.Cols())
		{
			m_ErrorList.GeneralError("Matrices are not of same dimension.", iLine, iPos);
			return false;
		}

		xRes.Resize(iRows, iCols);

		TCVScalar *pL = xL.Data();
		TCVScalar *pR = xR.Data();
		TCVScalar *pRes = xRes.Data();

		iCount = iRows * iCols;
		for(iIdx = 0; iIdx < iCount; iIdx++)
		{
			if (pL[iIdx] < pR[iIdx])
				pRes[iIdx] = TCVScalar(1);
			else
				pRes[iIdx] = TCVScalar(0);
		}
	}
	// Apply test to each element of lists (element by element)
	else if (rLVar.BaseType() == PDT_VARLIST && rRVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rL = *rLVar.GetVarListPtr();
		TVarList &rR = *rRVar.GetVarListPtr();

		if (rL.Count() != rR.Count())
		{
			m_ErrorList.GeneralError("Lists are not of same dimension.", iLine, iPos);
			return false;
		}

		int i, iCount = rL.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointLess(rL(i), rR(i), rRes(i), iLine, iPos))
				return false;
		}
	}
	else if (rLVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rL = *rLVar.GetVarListPtr();

		int i, iCount = rL.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointLess(rL(i), _rRVar, rRes(i), iLine, iPos))
				return false;
		}
	}
	else if (rRVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rR = *rRVar.GetVarListPtr();

		int i, iCount = rR.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointLess(_rLVar, rR(i), rRes(i), iLine, iPos))
				return false;
		}
	}
	else
	{
		// Standard operator
		return OpLess(_rLVar, _rRVar, rResVar, iLine, iPos);
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Point Greater Equal Operator .>=
/// Element by element >= test of lists and matrices

bool CCLUCodeBase::OpPointGreaterEqual(CCodeVar& _rLVar, CCodeVar& _rRVar, CCodeVar& rResVar, int iLine, int iPos)
{

	CCodeVar &rLVar = _rLVar.DereferenceVarPtr(true);
	CCodeVar &rRVar = _rRVar.DereferenceVarPtr(true);

	// Apply test to each element of matrices (element by element)
	if (rLVar.BaseType() == PDT_MATRIX && rRVar.BaseType() == PDT_MATRIX)
	{
		int iIdx, iCount, iRows, iCols;
		TMatrix &xL = *rLVar.GetMatrixPtr();
		TMatrix &xR = *rRVar.GetMatrixPtr();
		
		rResVar.New(PDT_MATRIX, "Constant");
		TMatrix &xRes = *rResVar.GetMatrixPtr();

		iRows = xL.Rows();
		iCols = xL.Cols();
		if (iRows != xR.Rows() || iCols != xR.Cols())
		{
			m_ErrorList.GeneralError("Matrices are not of same dimension.", iLine, iPos);
			return false;
		}

		xRes.Resize(iRows, iCols);

		TCVScalar *pL = xL.Data();
		TCVScalar *pR = xR.Data();
		TCVScalar *pRes = xRes.Data();

		iCount = iRows * iCols;
		for(iIdx = 0; iIdx < iCount; iIdx++)
		{
			if (pL[iIdx] >= pR[iIdx])
				pRes[iIdx] = TCVScalar(1);
			else
				pRes[iIdx] = TCVScalar(0);
		}
	}
	// Apply test to each element of lists (element by element)
	else if (rLVar.BaseType() == PDT_VARLIST && rRVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rL = *rLVar.GetVarListPtr();
		TVarList &rR = *rRVar.GetVarListPtr();

		if (rL.Count() != rR.Count())
		{
			m_ErrorList.GeneralError("Lists are not of same dimension.", iLine, iPos);
			return false;
		}

		int i, iCount = rL.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointGreaterEqual(rL(i), rR(i), rRes(i), iLine, iPos))
				return false;
		}
	}
	else if (rLVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rL = *rLVar.GetVarListPtr();

		int i, iCount = rL.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointGreaterEqual(rL(i), _rRVar, rRes(i), iLine, iPos))
				return false;
		}
	}
	else if (rRVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rR = *rRVar.GetVarListPtr();

		int i, iCount = rR.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointGreaterEqual(_rLVar, rR(i), rRes(i), iLine, iPos))
				return false;
		}
	}
	else
	{
		// Standard operator
		return OpGreaterEqual(_rLVar, _rRVar, rResVar, iLine, iPos);
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Point Less Equal Operator .<=
/// Element by element division of lists and matrices

bool CCLUCodeBase::OpPointLessEqual(CCodeVar& _rLVar, CCodeVar& _rRVar, CCodeVar& rResVar, int iLine, int iPos)
{

	CCodeVar &rLVar = _rLVar.DereferenceVarPtr(true);
	CCodeVar &rRVar = _rRVar.DereferenceVarPtr(true);

	// Apply test to each element of matrices (element by element)
	if (rLVar.BaseType() == PDT_MATRIX && rRVar.BaseType() == PDT_MATRIX)
	{
		int iIdx, iCount, iRows, iCols;
		TMatrix &xL = *rLVar.GetMatrixPtr();
		TMatrix &xR = *rRVar.GetMatrixPtr();
		
		rResVar.New(PDT_MATRIX, "Constant");
		TMatrix &xRes = *rResVar.GetMatrixPtr();

		iRows = xL.Rows();
		iCols = xL.Cols();
		if (iRows != xR.Rows() || iCols != xR.Cols())
		{
			m_ErrorList.GeneralError("Matrices are not of same dimension.", iLine, iPos);
			return false;
		}

		xRes.Resize(iRows, iCols);

		TCVScalar *pL = xL.Data();
		TCVScalar *pR = xR.Data();
		TCVScalar *pRes = xRes.Data();

		iCount = iRows * iCols;
		for(iIdx = 0; iIdx < iCount; iIdx++)
		{
			if (pL[iIdx] <= pR[iIdx])
				pRes[iIdx] = TCVScalar(1);
			else
				pRes[iIdx] = TCVScalar(0);
		}
	}
	// Apply test to each element of lists (element by element)
	else if (rLVar.BaseType() == PDT_VARLIST && rRVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rL = *rLVar.GetVarListPtr();
		TVarList &rR = *rRVar.GetVarListPtr();

		if (rL.Count() != rR.Count())
		{
			m_ErrorList.GeneralError("Lists are not of same dimension.", iLine, iPos);
			return false;
		}

		int i, iCount = rL.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointLessEqual(rL(i), rR(i), rRes(i), iLine, iPos))
				return false;
		}
	}
	else if (rLVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rL = *rLVar.GetVarListPtr();

		int i, iCount = rL.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointLessEqual(rL(i), _rRVar, rRes(i), iLine, iPos))
				return false;
		}
	}
	else if (rRVar.BaseType() == PDT_VARLIST)
	{
		rResVar.New(PDT_VARLIST, "Constant");
		TVarList &rRes = *rResVar.GetVarListPtr();

		TVarList &rR = *rRVar.GetVarListPtr();

		int i, iCount = rR.Count();
		rRes.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!OpPointLessEqual(_rLVar, rR(i), rRes(i), iLine, iPos))
				return false;
		}
	}
	else
	{
		// Standard operator
		return OpLessEqual(_rLVar, _rRVar, rResVar, iLine, iPos);
	}
	
	return true;
}




//////////////////////////////////////////////////////////////////////
/// Point left shift Operator .<<
/// Element by element list combination of lists

bool CCLUCodeBase::OpPointLShift(CCodeVar& _rLVar, CCodeVar& _rRVar, int iLine, int iPos)
{

	CCodeVar &rLVar = _rLVar.DereferenceVarPtr(true);
	CCodeVar &rRVar = _rRVar.DereferenceVarPtr(true);

	// Apply test to each element of lists (element by element)
	if (rLVar.BaseType() == PDT_VARLIST && rRVar.BaseType() == PDT_VARLIST)
	{
		TVarList &rL = *rLVar.GetVarListPtr();
		TVarList &rR = *rRVar.GetVarListPtr();

		if (rL(0).BaseType() != PDT_VARLIST || rR(0).BaseType() != PDT_VARLIST)
		{
			return OpLShift(_rLVar, _rRVar, iLine, iPos);
		}

		if (rL.Count() != rR.Count())
		{
			m_ErrorList.GeneralError("Lists are not of same dimension.", iLine, iPos);
			return false;
		}

		int i, iCount = rL.Count();
		for (i = 0; i < iCount; i++)
		{
			if (!OpPointLShift(rL(i), rR(i), iLine, iPos))
				return false;
		}
	}
	else if (rLVar.BaseType() == PDT_VARLIST)
	{
		TVarList &rL = *rLVar.GetVarListPtr();

		if (rL(0).BaseType() != PDT_VARLIST)
		{
			return OpLShift(_rLVar, _rRVar, iLine, iPos);
		}

		int i, iCount = rL.Count();
		for (i = 0; i < iCount; i++)
		{
			if (!OpPointLShift(rL(i), _rRVar, iLine, iPos))
				return false;
		}
	}
	else if (rRVar.BaseType() == PDT_VARLIST)
	{
		TVarList &rR = *rRVar.GetVarListPtr();

		if (rR(0).BaseType() != PDT_VARLIST)
		{
			return OpLShift(_rLVar, _rRVar, iLine, iPos);
		}

		int i, iCount = rR.Count();
		for (i = 0; i < iCount; i++)
		{
			if (!OpPointLShift(_rLVar, rR(i), iLine, iPos))
				return false;
		}
	}
	else
	{
		// Standard inner product
		return OpLShift(_rLVar, _rRVar, iLine, iPos);
	}
	
	return true;
}





//////////////////////////////////////////////////////////////////////
/// OR Operator ||
///
bool CCLUCodeBase::OpOR(CCodeVar& _rLVar, CCodeVar& _rRVar, CCodeVar& rResVar, int iLine, int iPos)
{
	TCVScalar fLVal, fRVal;

	CCodeVar &rLVar = _rLVar.DereferenceVarPtr(true);
	CCodeVar &rRVar = _rRVar.DereferenceVarPtr(true);

	if (rLVar.CastToScalar(fLVal, m_fSensitivity))
	{
		if (!rRVar.CastToScalar(fRVal, m_fSensitivity))
		{
			m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
			return false;
		}

		if (fLVal || fRVal)
			rResVar = int(1);
		else
			rResVar = int(0);
	}
	// Apply test to each element of matrix
	else if (rLVar.BaseType() == PDT_MATRIX)
	{
		if (!rRVar.CastToScalar(fRVal, m_fSensitivity))
		{
			m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
			return false;
		}

		int iPos, iCount, iRows, iCols;
		TMatrix &xT = *rLVar.GetMatrixPtr();
		
		rResVar.New(PDT_MATRIX, "Constant");
		TMatrix &xR = *rResVar.GetMatrixPtr();

		iRows = xT.Rows();
		iCols = xT.Cols();
		xR.Resize(iRows, iCols);

		TCVScalar *pT = xT.Data();
		TCVScalar *pR = xR.Data();

		iCount = iRows * iCols;
		for(iPos = 0; iPos < iCount; iPos++)
		{
			if (pT[iPos] || fRVal)
				pR[iPos] = TCVScalar(1);
		}
	}
	else
	{
		m_ErrorList.InvalidLVal(rLVar, iLine, iPos);
		return false;
	}
	
	return true;
}




//////////////////////////////////////////////////////////////////////
/// Not Equal Operator !=
///
bool CCLUCodeBase::OpNotEqual(CCodeVar& _rLVar, CCodeVar& _rRVar, CCodeVar& rResVar, int iLine, int iPos)
{
	TCVScalar fRVal;
	CCodeVar &rLVar = _rLVar.DereferenceVarPtr(true);
	CCodeVar &rRVar = _rRVar.DereferenceVarPtr(true);

	if (rLVar.BaseType() == PDT_MATRIX && rRVar.CastToScalar(fRVal, m_fSensitivity))
	{
		int iPos, iCount, iRows, iCols;
		TMatrix &xT = *rLVar.GetMatrixPtr();

		rResVar.New(PDT_MATRIX, "Constant");
		TMatrix &xR = *rResVar.GetMatrixPtr();

		iRows = xT.Rows();
		iCols = xT.Cols();
		xR.Resize(iRows, iCols);

		TCVScalar *pT = xT.Data();
		TCVScalar *pR = xR.Data();

		iCount = iRows * iCols;
		for(iPos = 0; iPos < iCount; iPos++)
		{
			if (pT[iPos] != fRVal)
				pR[iPos] = TCVScalar(1);
		}
	}
	else if (rLVar.BaseType() == PDT_IMAGE && rRVar.BaseType() == PDT_COLOR)
	{
		TImage &rLImg = *rLVar.GetImagePtr();
		COGLColor &rCol = *rRVar.GetOGLColorPtr();

		rResVar.New(PDT_IMAGE, "Constant");
		TImage& oglBitmap = *rResVar.GetImagePtr();
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
		}
		else
		{
			m_ErrorList.GeneralError("Cannot create mask image.", iLine, iPos);
			return false;
		}

		rLImg->MaskNotEqual(*((COGLBitmap *) oglBitmap), rCol);
	}
	else
	{
		if (!OpEqual(rLVar, rRVar, rResVar, iLine, iPos))
			return false;

		if (rResVar.BaseType() == PDT_INT)
		{
			int &iVal = *rResVar.GetIntPtr();

			if (iVal == 1)
				iVal = 0;
			else
				iVal = 1;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Equal Operator ==
///
bool CCLUCodeBase::OpEqual(CCodeVar& _rLVar, CCodeVar& _rRVar, CCodeVar& rResVar, int iLine, int iPos)
{
	TMatrix xRes;
	TMultiV vRes;
	TCVScalar fLVal, fRVal;
	ECodeDataType eLType, eRType;
	

	CCodeVar &rLVar = _rLVar.DereferenceVarPtr(true);
	CCodeVar &rRVar = _rRVar.DereferenceVarPtr(true);

	eLType = rLVar.BaseType();
	eRType = rRVar.BaseType();

	rResVar.New(PDT_INT, "Constant");

	if (eLType == PDT_STRING || eRType == PDT_STRING) 
	{		
		cStr csLeft, csRight;

		CastToString(rLVar, csLeft);
		CastToString(rRVar, csRight);

		if (csLeft == csRight)
			rResVar = 1;
		else
			rResVar = 0;
	}
	else if (eLType == PDT_MULTIV)
	{
		TMultiV vA = *rLVar.GetMultiVPtr();
		
		if (eRType == PDT_MULTIV)
		{
			TMultiV vB = *rRVar.GetMultiVPtr();
			if (vA.GetBase().BaseID() != vB.GetBase().BaseID())
			{
				m_ErrorList.InvalidMVType(rLVar, rRVar, iLine, iPos);
				return false;
			}

			//vA.Round(m_fSensitivity);
			//vB.Round(m_fSensitivity);

			if (vA == vB)
				rResVar = 1;
			else
				rResVar = 0;
		}
		else
		{
			if (!rRVar.CastToScalar(fRVal, m_fSensitivity))
			{
				m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
				return false;
			}
			
			//vA.Round(m_fSensitivity);
			//fRVal = ::Round(fRVal, m_fSensitivity);

			Mem<uint> mGList;
			GradeList(vA, mGList, m_fSensitivity);
			if (mGList.Count() == 0 && ::IsZero(fRVal, m_fSensitivity))
			{
				rResVar = 1;
			}
			else if (mGList.Count() == 1 && mGList[0] == 0)
			{
				if (Scalar(vA) == fRVal)
					rResVar = 1;
				else
					rResVar = 0;
			}
			else
			{
				rResVar = 0;
			}
		}
	}
	else if (eLType == PDT_MATRIX)
	{
		TMatrix &xA = *rLVar.GetMatrixPtr();
		
		if (eRType == PDT_MATRIX)
		{
			if (xA == *rRVar.GetMatrixPtr())
				rResVar = 1;
			else
				rResVar = 0;
		}
		else if (rRVar.CastToScalar(fRVal, m_fSensitivity))
		{
			int iPos, iCount, iRows, iCols;
			TMatrix &xT = *rLVar.GetMatrixPtr();

			rResVar.New(PDT_MATRIX, "Constant");
			TMatrix &xR = *rResVar.GetMatrixPtr();

			iRows = xT.Rows();
			iCols = xT.Cols();
			xR.Resize(iRows, iCols);

			TCVScalar *pT = xT.Data();
			TCVScalar *pR = xR.Data();

			iCount = iRows * iCols;
			for(iPos = 0; iPos < iCount; iPos++)
			{
				if (pT[iPos] == fRVal)
					pR[iPos] = TCVScalar(1);
			}
		}
		else
		{
			m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
			return false;
		}
	}
	else if (eLType == PDT_IMAGE)
	{
		TImage &rLImg = *rLVar.GetImagePtr();
		
		if (rRVar.BaseType() == PDT_COLOR)
		{
			COGLColor &rCol = *rRVar.GetOGLColorPtr();

			rResVar.New(PDT_IMAGE, "Constant");
			TImage& oglBitmap = *rResVar.GetImagePtr();
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
			}
			else
			{
				m_ErrorList.GeneralError("Cannot create mask image.", iLine, iPos);
				return false;
			}

			rLImg->MaskEqual(*((COGLBitmap *) oglBitmap), rCol);
		}
		else
		{
			m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
			return false;
		}
	}
	else if (rLVar.CastToScalar(fLVal, m_fSensitivity))
	{
		//fLVal = ::Round(fLVal, m_fSensitivity);
		if (eRType == PDT_MULTIV)
		{
			TMultiV vA = *rRVar.GetMultiVPtr();
			//vA.Round(m_fSensitivity);

			Mem<uint> mGList;
			GradeList(vA, mGList, m_fSensitivity);
			if (mGList.Count() != 1 || mGList[0] != 0)
				rResVar = 0;
			else
			{
				if (Scalar(vA) == fLVal)
					rResVar = 1;
				else
					rResVar = 0;
			}
		}
		else
		{
			if (!rRVar.CastToScalar(fRVal, m_fSensitivity))
			{
				m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
				return false;
			}

			//fRVal = ::Round(fRVal, m_fSensitivity);
			if (fLVal == fRVal)
				rResVar = TCVCounter(1);
			else
				rResVar = TCVCounter(0);
		}
	}
	else
	{
		m_ErrorList.InvalidLVal(rLVar, iLine, iPos);
		return false;
	}
	
	return true;
}



//////////////////////////////////////////////////////////////////////
/// Geometric Product Operator *
///
bool CCLUCodeBase::OpProd(CCodeVar& _rLVar, CCodeVar& _rRVar, CCodeVar& rResVar, int iLine, int iPos)
{
	TMatrix xRes;
	TMultiV vRes;
	TCVScalar fLVal, fRVal, fRes;
	ECodeDataType eLType, eRType, eResType;
	
	eResType = PDT_NOTYPE;
	
	CCodeVar &rLVar = _rLVar.DereferenceVarPtr(true);
	CCodeVar &rRVar = _rRVar.DereferenceVarPtr(true);

	eLType = rLVar.BaseType();
	eRType = rRVar.BaseType();

	if (eLType == PDT_MULTIV)
	{
		eResType = PDT_MULTIV;
		TMultiV &vA = *rLVar.GetMultiVPtr();
		
		if (eRType == PDT_MULTIV)
		{
			TMultiV &vB = *rRVar.GetMultiVPtr();

			if (vA.GetBase().BaseID() != vB.GetBase().BaseID())
			{
				m_ErrorList.InvalidMVType(rLVar, rRVar, iLine, iPos);
				return false;
			}
			vRes = vA & vB;
		}
		else
		{
			if (!rRVar.CastToScalar(fRVal, m_fSensitivity))
			{
				m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
				return false;
			}
			
			//TMultiV vB(vA.GetStyle());
			//vB = fRVal;
			
			vRes = vA & fRVal;
		}
	}
	else if (eLType == PDT_MATRIX)
	{
		eResType = PDT_MATRIX;
		TMatrix &xA = *rLVar.GetMatrixPtr();
		
		if (eRType == PDT_MATRIX)
		{
			TMatrix &xB = *rRVar.GetMatrixPtr();

			if (xA.Cols() != xB.Rows() && (xA.Cols() > 1 || xB.Rows() > 1))
			{
				m_ErrorList.GeneralError("Matrices in product have incompatible dimensions.", iLine, iPos);
				return false;
			}

			xRes = xA * xB;
		}
		else if (rRVar.CastToScalar(fRVal, m_fSensitivity))
		{
			xRes = xA * fRVal;
		}
		else
		{
			m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
			return false;
		}
	}
	else if (eLType == PDT_TENSOR_IDX)
	{
		TTensorIdx &rLTIdx = *rLVar.GetTensorIdxPtr();

		if (eRType == PDT_TENSOR_IDX)
		{
			TTensorIdx &rRTIdx = *rRVar.GetTensorIdxPtr();
			
			// Create a temporary variable that contains tensor.
			// Do this, so that we can return a tensor index.
			m_mTempVarList.Add(1);
			CCodeVar &rTemp = m_mTempVarList.Last();
			rTemp.New(PDT_TENSOR, "Constant");
			TTensor& rT = *rTemp.GetTensorPtr();

			rResVar.New(PDT_TENSOR_IDX, "Constant");
			TTensorIdx& rTIdx = *rResVar.GetTensorIdxPtr();

			if (!TensorProductContract(rT, rTIdx, rLTIdx, rRTIdx))
			{
				m_ErrorList.GeneralError("Error contracting tensors.", iLine, iPos);
				return false;
			}

			return true;
		}
		else if (rRVar.CastToScalar(fRVal, m_fSensitivity))
		{
			// Create a temporary variable that contains tensor.
			// Do this, so that we can return a tensor index.
			m_mTempVarList.Add(1);
			CCodeVar &rTemp = m_mTempVarList.Last();
			rTemp.New(PDT_TENSOR, "Constant");
			TTensor& rT = *rTemp.GetTensorPtr();

			rResVar.New(PDT_TENSOR_IDX, "Constant");
			TTensorIdx& rTIdx = *rResVar.GetTensorIdxPtr();

			if (!TensorIdxProductScalar(rT, rTIdx, rLTIdx, fRVal))
			{
				m_ErrorList.GeneralError("Error scaling tensors.", iLine, iPos);
				return false;
			}

			return true;			
		}
		else
		{
			m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
			return false;
		}
	}
	else if (eLType == PDT_TENSOR)
	{
		TTensor &rLT = *rLVar.GetTensorPtr();

		if (rRVar.CastToScalar(fRVal, m_fSensitivity))
		{
			rResVar.New(PDT_TENSOR, "Constant");
			TTensor& rT = *rResVar.GetTensorPtr();

			if (!TensorProductScalar(rT, rLT, fRVal))
			{
				m_ErrorList.GeneralError("Error scaling tensors.", iLine, iPos);
				return false;
			}

			return true;			
		}
		else
		{
			m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
			return false;
		}
	}
	else if (eLType == PDT_COLOR)
	{
		TOGLColor &rCol = *rLVar.GetOGLColorPtr();
		if (rRVar.CastToScalar(fRVal, m_fSensitivity))
		{
			rResVar.New(PDT_COLOR, "Constant");
			TOGLColor &rNewCol = *rResVar.GetOGLColorPtr();

			rNewCol = fRVal * rCol;
			//rNewCol.Clamp();
			return true;
		}
		else
		{
			m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
			return false;
		}
	}
	else if (rLVar.CastToScalar(fLVal, m_fSensitivity))
	{
		if (eRType == PDT_MULTIV)
		{
			eResType = PDT_MULTIV;
			
			TMultiV &vA = *rRVar.GetMultiVPtr();
			//TMultiV vB(vA.GetStyle());
			//vB = fLVal;
			
			vRes = fLVal & vA;
		}
		else if (eRType == PDT_MATRIX)
		{
			eResType = PDT_MATRIX;
			TMatrix &xA = *rRVar.GetMatrixPtr();
			
			xRes = fLVal * xA;
		}
		else if (eRType == PDT_TENSOR_IDX)
		{
			TTensorIdx &rRTIdx = *rRVar.GetTensorIdxPtr();

			// Create a temporary variable that contains tensor.
			// Do this, so that we can return a tensor index.
			m_mTempVarList.Add(1);
			CCodeVar &rTemp = m_mTempVarList.Last();
			rTemp.New(PDT_TENSOR, "Constant");
			TTensor& rT = *rTemp.GetTensorPtr();

			rResVar.New(PDT_TENSOR_IDX, "Constant");
			TTensorIdx& rTIdx = *rResVar.GetTensorIdxPtr();

			if (!TensorIdxProductScalar(rT, rTIdx, rRTIdx, fLVal))
			{
				m_ErrorList.GeneralError("Error scaling tensor.", iLine, iPos);
				return false;
			}

			return true;			
		}
		else if (eRType == PDT_TENSOR)
		{
			TTensor &rRT = *rRVar.GetTensorPtr();

			rResVar.New(PDT_TENSOR, "Constant");
			TTensor& rT = *rResVar.GetTensorPtr();

			if (!TensorProductScalar(rT, rRT, fLVal))
			{
				m_ErrorList.GeneralError("Error scaling tensor.", iLine, iPos);
				return false;
			}

			return true;			
		}
		else if (eRType == PDT_COLOR)
		{
			TOGLColor &rCol = *rRVar.GetOGLColorPtr();

			rResVar.New(PDT_COLOR, "Constant");
			TOGLColor &rNewCol = *rResVar.GetOGLColorPtr();

			rNewCol = fLVal * rCol;
			//rNewCol.Clamp();
			return true;
		}
		else
		{
			if (!rRVar.CastToScalar(fRVal, m_fSensitivity))
			{
				m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
				return false;
			}
			
			eResType = PDT_SCALAR;
			fRes = fLVal * fRVal;
		}
	}
	else
	{
		m_ErrorList.InvalidLVal(rLVar, iLine, iPos);
		return false;
	}
	
	rResVar.New(eResType, "Constant");
	if (eResType == PDT_MULTIV)
		rResVar = vRes.TinyToZero(m_fSensitivity);
	else if (eResType == PDT_MATRIX)
	{
		xRes.TinyToZero(m_fSensitivity);
		rResVar = xRes;
	}
	else
		rResVar = fRes;
	
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Inner Product Operator .
///
bool CCLUCodeBase::OpInnerProd(CCodeVar& _rLVar, CCodeVar& _rRVar, CCodeVar& rResVar, int iLine, int iPos)
{
	TMultiV vRes;
	TCVScalar fLVal, fRVal, fRes;
	ECodeDataType eLType, eRType, eResType;
	
	eResType = PDT_NOTYPE;
	
	CCodeVar &rLVar = _rLVar.DereferenceVarPtr(true);
	CCodeVar &rRVar = _rRVar.DereferenceVarPtr(true);

	eLType = rLVar.BaseType();
	eRType = rRVar.BaseType();	

	if (eLType == PDT_MULTIV && eRType == PDT_MULTIV)
	{
		eResType = PDT_MULTIV;
		TMultiV &vA = *rLVar.GetMultiVPtr();	
		TMultiV &vB = *rRVar.GetMultiVPtr();	

		if (vA.GetBase().BaseID() != vB.GetBase().BaseID())
		{
			m_ErrorList.InvalidMVType(rLVar, rRVar, iLine, iPos);
			return false;
		}
		vRes = vA * vB;
	}
	else		
	{
		if (!rRVar.CastToScalar(fRVal, m_fSensitivity))
		{
			m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
			return false;
		}	
		if (rLVar.CastToScalar(fLVal, m_fSensitivity))
		{
			m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
			return false;
		}
		eResType = PDT_INT;
		fRes=0;
	}	
	
	
	rResVar.New(eResType, "Constant");
	if (eResType == PDT_MULTIV)
		rResVar = vRes.TinyToZero(m_fSensitivity);
	else
		rResVar = fRes;	
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Division Operator /
///
bool CCLUCodeBase::OpDiv(CCodeVar& _rLVar, CCodeVar& _rRVar, CCodeVar& rResVar, int iLine, int iPos)
{
	TMatrix xRes;
	TMultiV vRes;
	TCVScalar fLVal, fRVal, fRes;
	ECodeDataType eLType, eRType, eResType;
	
	eResType = PDT_NOTYPE;
	
	CCodeVar &rLVar = _rLVar.DereferenceVarPtr(true);
	CCodeVar &rRVar = _rRVar.DereferenceVarPtr(true);

	eLType = rLVar.BaseType();
	eRType = rRVar.BaseType();	
	
	if (eLType == PDT_MULTIV)
	{
		eResType = PDT_MULTIV;
		TMultiV &vA = *rLVar.GetMultiVPtr();
		
		if (rRVar.CastToScalar(fRVal, m_fSensitivity))
		{
			//TMultiV vB(vA.GetStyle());
			//vB = fRVal;
			if (fRVal != TCVScalar(0.0))
				vRes = vA & (1/fRVal);
			else
			{
				m_ErrorList.DivByZero(iLine, iPos);
				return false;
				//TMultiV vB(vA.GetStyle());
				//vRes = vB;
			}
		}
		else if (eRType == PDT_MULTIV)
		{
			TMultiV &vB = *rRVar.GetMultiVPtr();

			if (vA.GetBase().BaseID() != vB.GetBase().BaseID())
			{
				m_ErrorList.InvalidMVType(rLVar, rRVar, iLine, iPos);
				return false;
			}

			TMultiV vZero(vB.GetStyle());

			if (vB == vZero)
			{
				m_ErrorList.DivByZero(iLine, iPos);
				return false;
			}
	
			vRes = vA & !vB;
		}
		else
		{
			m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
			return false;
		}
	}
	else if (eLType == PDT_MATRIX)
	{
		eResType = PDT_MATRIX;
		TMatrix &xA = *rLVar.GetMatrixPtr();
		
		if (eRType == PDT_MATRIX)
		{
			TMatrix xZero;
			TMatrix &xB = *rRVar.GetMatrixPtr();

			if (xZero == xB)
			{
				m_ErrorList.DivByZero(iLine, iPos);
				return false;
			}

			xRes = xA * (!xB);
		}
		else if (rRVar.CastToScalar(fRVal, m_fSensitivity))
		{
			if (fRVal == TCVScalar(0))
			{
				m_ErrorList.DivByZero(iLine, iPos);
				return false;
			}

			xRes = xA * (TCVScalar(1) / fRVal);
		}
		else
		{
			m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
			return false;
		}
	}
	else if (eLType == PDT_TENSOR_IDX)
	{
		TTensorIdx &rLTIdx = *rLVar.GetTensorIdxPtr();

		if (eRType == PDT_TENSOR_IDX)
		{
			TTensorIdx &rRTIdx = *rRVar.GetTensorIdxPtr();
			
			// Create a temporary variable that contains tensor.
			// Do this, so that we can return a tensor index.
			m_mTempVarList.Add(1);
			CCodeVar &rTemp = m_mTempVarList.Last();
			rTemp.New(PDT_TENSOR, "Constant");
			TTensor& rT = *rTemp.GetTensorPtr();

			rResVar.New(PDT_TENSOR_IDX, "Constant");
			TTensorIdx& rTIdx = *rResVar.GetTensorIdxPtr();

			if (!TensorDivContract(rT, rTIdx, rLTIdx, rRTIdx))
			{
				m_ErrorList.GeneralError("Error division contracting tensors. Possibly division by zero.", iLine, iPos);
				return false;
			}

			return true;
		}
		else if (rRVar.CastToScalar(fRVal, m_fSensitivity))
		{
			// Create a temporary variable that contains tensor.
			// Do this, so that we can return a tensor index.
			m_mTempVarList.Add(1);
			CCodeVar &rTemp = m_mTempVarList.Last();
			rTemp.New(PDT_TENSOR, "Constant");
			TTensor& rT = *rTemp.GetTensorPtr();

			rResVar.New(PDT_TENSOR_IDX, "Constant");
			TTensorIdx& rTIdx = *rResVar.GetTensorIdxPtr();

			if (!TensorIdxDivRightScalar(rT, rTIdx, rLTIdx, fRVal))
			{
				m_ErrorList.GeneralError("Error dividing tensor by scalar.", iLine, iPos);
				return false;
			}

			return true;			
		}
		else
		{
			m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
			return false;
		}
	}
	else if (eLType == PDT_TENSOR)
	{
		TTensor &rLT = *rLVar.GetTensorPtr();

		if (rRVar.CastToScalar(fRVal, m_fSensitivity))
		{
			rResVar.New(PDT_TENSOR, "Constant");
			TTensor& rT = *rResVar.GetTensorPtr();

			if (!TensorDivRightScalar(rT, rLT, fRVal))
			{
				m_ErrorList.GeneralError("Error dividing tensor by scalar.", iLine, iPos);
				return false;
			}

			return true;			
		}
		else
		{
			m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
			return false;
		}
	}
	else if (rLVar.CastToScalar(fLVal, m_fSensitivity))
	{
		if (eRType == PDT_MULTIV)
		{
			eResType = PDT_MULTIV;
			
			TMultiV &vA = *rRVar.GetMultiVPtr();
			TMultiV vB(vA.GetStyle());

			if (vA == vB)
			{
				m_ErrorList.DivByZero(iLine, iPos);
				return false;
			}

			vB = fLVal;
			vRes = vB & !vA;
		}
		else if (eRType == PDT_MATRIX)
		{
			eResType = PDT_MATRIX;
			TMatrix &xA = *rRVar.GetMatrixPtr();
			TMatrix xZero;

			if (xA == xZero)
			{
				m_ErrorList.DivByZero(iLine, iPos);
				return false;
			}
			
			xRes = fLVal * (!xA);
		}
		else if (eRType == PDT_TENSOR_IDX)
		{
			TTensorIdx &rRTIdx = *rRVar.GetTensorIdxPtr();

			// Create a temporary variable that contains tensor.
			// Do this, so that we can return a tensor index.
			m_mTempVarList.Add(1);
			CCodeVar &rTemp = m_mTempVarList.Last();
			rTemp.New(PDT_TENSOR, "Constant");
			TTensor& rT = *rTemp.GetTensorPtr();

			rResVar.New(PDT_TENSOR_IDX, "Constant");
			TTensorIdx& rTIdx = *rResVar.GetTensorIdxPtr();

			if (!TensorIdxDivLeftScalar(rT, rTIdx, rRTIdx, fLVal))
			{
				m_ErrorList.GeneralError("Error dividing scalar by tensor components.", iLine, iPos);
				return false;
			}

			return true;			
		}
		else if (eRType == PDT_TENSOR)
		{
			TTensor &rRT = *rRVar.GetTensorPtr();

			rResVar.New(PDT_TENSOR, "Constant");
			TTensor& rT = *rResVar.GetTensorPtr();

			if (!TensorDivLeftScalar(rT, rRT, fLVal))
			{
				m_ErrorList.GeneralError("Error dividing scalar by tensor components.", iLine, iPos);
				return false;
			}

			return true;			
		}
		else
		{
			if (!rRVar.CastToScalar(fRVal, m_fSensitivity))
			{
				m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
				return false;
			}
			
			if (fRVal == TCVScalar(0))
			{
				m_ErrorList.DivByZero(iLine, iPos);
				return false;
			}

			eResType = PDT_SCALAR;
			fRes = fLVal / fRVal;
		}
	}
	else
	{
		m_ErrorList.InvalidLVal(rLVar, iLine, iPos);
		return false;
	}
	
	rResVar.New(eResType, "Constant");
	if (eResType == PDT_MULTIV)
		rResVar = vRes.TinyToZero(m_fSensitivity);
	else if (eResType == PDT_MATRIX)
	{
		xRes.TinyToZero(m_fSensitivity);
		rResVar = xRes;
	}
	else
		rResVar = fRes;
	
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add Operator +
///
bool CCLUCodeBase::OpAdd(CCodeVar& _rLVar, CCodeVar& _rRVar, CCodeVar& rResVar, int iLine, int iPos)
{
	TMatrix xRes;
	TMultiV vRes;
	TCVScalar fLVal, fRVal, fRes;
	ECodeDataType eLType, eRType, eResType;
	
	eResType = PDT_NOTYPE;
	
	CCodeVar &rLVar = _rLVar.DereferenceVarPtr(true);
	CCodeVar &rRVar = _rRVar.DereferenceVarPtr(true);

	eLType = rLVar.BaseType();
	eRType = rRVar.BaseType();	

	if (eLType == PDT_STRING || eRType == PDT_STRING) 
	{		
		cStr csLeft, csRight;

		CastToString(rLVar, csLeft);
		CastToString(rRVar, csRight);

		rResVar.New(PDT_STRING, "Constant");
		rResVar = csLeft + csRight;
		return true;
	}
	else if (eLType == PDT_MULTIV)
	{
		eResType = PDT_MULTIV;
		TMultiV &vA = *rLVar.GetMultiVPtr();
		
		if (eRType == PDT_MULTIV)
		{
			TMultiV &vB = *rRVar.GetMultiVPtr();
			if (vA.GetBase().BaseID() != vB.GetBase().BaseID())
			{
				m_ErrorList.InvalidMVType(rLVar, rRVar, iLine, iPos);
				return false;
			}

			vRes = vA + vB;
		}
		else
		{
			if (!rRVar.CastToScalar(fRVal, m_fSensitivity))
			{
				m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
				return false;
			}
			
			//TMultiV vB(vA.GetStyle());
			//vB = fRVal;
			
			vRes = vA + fRVal;
		}
	}
	else if (eLType == PDT_MATRIX)
	{
		TCVScalar dVal;
		eResType = PDT_MATRIX;
		TMatrix &xA = *rLVar.GetMatrixPtr();
		
		if (rRVar.CastToScalar(dVal, m_fSensitivity))
		{
			xRes = xA + dVal;
		}
		else if (eRType == PDT_MATRIX)
		{
			xRes = xA + *rRVar.GetMatrixPtr();
		}
		else
		{
			m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
			return false;
		}
	}
	else if (eLType == PDT_TENSOR_IDX)
	{
		TTensorIdx &rLTIdx = *rLVar.GetTensorIdxPtr();

		if (eRType == PDT_TENSOR_IDX)
		{
			TTensorIdx &rRTIdx = *rRVar.GetTensorIdxPtr();
			
			// Create a temporary variable that contains tensor.
			// Do this, so that we can return a tensor index.
			m_mTempVarList.Add(1);
			CCodeVar &rTemp = m_mTempVarList.Last();
			rTemp.New(PDT_TENSOR, "Constant");
			TTensor& rT = *rTemp.GetTensorPtr();

			rResVar.New(PDT_TENSOR_IDX, "Constant");
			TTensorIdx& rTIdx = *rResVar.GetTensorIdxPtr();

			if (!TensorAddPoint(rT, rTIdx, rLTIdx, rRTIdx))
			{
				m_ErrorList.GeneralError("Error adding tensors.", iLine, iPos);
				return false;
			}

			return true;
		}
		else if (rRVar.CastToScalar(fRVal, m_fSensitivity))
		{
			// Create a temporary variable that contains tensor.
			// Do this, so that we can return a tensor index.
			m_mTempVarList.Add(1);
			CCodeVar &rTemp = m_mTempVarList.Last();
			rTemp.New(PDT_TENSOR, "Constant");
			TTensor& rT = *rTemp.GetTensorPtr();

			rResVar.New(PDT_TENSOR_IDX, "Constant");
			TTensorIdx& rTIdx = *rResVar.GetTensorIdxPtr();

			if (!TensorIdxAddScalar(rT, rTIdx, rLTIdx, fRVal))
			{
				m_ErrorList.GeneralError("Error adding to tensor.", iLine, iPos);
				return false;
			}

			return true;			
		}
		else
		{
			m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
			return false;
		}
	}
	else if (eLType == PDT_TENSOR)
	{
		TTensor &rLT = *rLVar.GetTensorPtr();

		if (rRVar.CastToScalar(fRVal, m_fSensitivity))
		{
			rResVar.New(PDT_TENSOR, "Constant");
			TTensor& rT = *rResVar.GetTensorPtr();

			if (!TensorAddScalar(rT, rLT, fRVal))
			{
				m_ErrorList.GeneralError("Error adding tensors.", iLine, iPos);
				return false;
			}

			return true;			
		}
		else
		{
			m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
			return false;
		}
	}
	else if (eLType == PDT_COLOR && eRType == PDT_COLOR)
	{
		TOGLColor &Col1 = *rLVar.GetOGLColorPtr();
		TOGLColor &Col2 = *rRVar.GetOGLColorPtr();
		TOGLColor ResCol;
		
		ResCol = Col1 + Col2;
		//ResCol.Clamp();
		
		rResVar = ResCol;
		return true;
	}
	else if (rLVar.CastToScalar(fLVal, m_fSensitivity))
	{
		if (eRType == PDT_MULTIV)
		{
			eResType = PDT_MULTIV;
			
			TMultiV &vA = *rRVar.GetMultiVPtr();
			TMultiV vB(vA.GetStyle());
			vB = fLVal;
			
			vRes = vB + vA;
		}
		else if (eRType == PDT_TENSOR_IDX)
		{
			TTensorIdx &rRTIdx = *rRVar.GetTensorIdxPtr();

			// Create a temporary variable that contains tensor.
			// Do this, so that we can return a tensor index.
			m_mTempVarList.Add(1);
			CCodeVar &rTemp = m_mTempVarList.Last();
			rTemp.New(PDT_TENSOR, "Constant");
			TTensor& rT = *rTemp.GetTensorPtr();

			rResVar.New(PDT_TENSOR_IDX, "Constant");
			TTensorIdx& rTIdx = *rResVar.GetTensorIdxPtr();

			if (!TensorIdxAddScalar(rT, rTIdx, rRTIdx, fLVal))
			{
				m_ErrorList.GeneralError("Error adding scalar to tensor.", iLine, iPos);
				return false;
			}

			return true;			
		}
		else if (eRType == PDT_TENSOR)
		{
			TTensor &rRT = *rRVar.GetTensorPtr();

			rResVar.New(PDT_TENSOR, "Constant");
			TTensor& rT = *rResVar.GetTensorPtr();

			if (!TensorAddScalar(rT, rRT, fLVal))
			{
				m_ErrorList.GeneralError("Error adding scalar tensor.", iLine, iPos);
				return false;
			}

			return true;			
		}
		else
		{
			if (!rRVar.CastToScalar(fRVal, m_fSensitivity))
			{
				m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
				return false;
			}
			
			eResType = PDT_SCALAR;
			fRes = fLVal + fRVal;
		}
	}
	else
	{
		m_ErrorList.InvalidLVal(rLVar, iLine, iPos);
		return false;
	}
	
	rResVar.New(eResType, "Constant");
	if (eResType == PDT_MULTIV)
		rResVar = vRes.TinyToZero(m_fSensitivity);
	else if (eResType == PDT_MATRIX)
	{
		xRes.TinyToZero(m_fSensitivity);
		rResVar = xRes;
	}
	else
		rResVar = fRes;
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Power Operator ^^
///
bool CCLUCodeBase::OpPow(CCodeVar& _rLVar, CCodeVar& _rRVar, CCodeVar& rResVar, int iLine, int iPos)
{
	CCodeVar &rLVar = _rLVar.DereferenceVarPtr(true);
	CCodeVar &rRVar = _rRVar.DereferenceVarPtr(true);

	if (!Pow(rResVar, rLVar, rRVar, iLine, iPos))
		return false;

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Join Operator |
///
// TODO Update docu for join. Now also for null blades.

bool CCLUCodeBase::OpJoin(CCodeVar& _rLVar, CCodeVar& _rRVar, CCodeVar& rResVar, int iLine, int iPos)
{
	TMultiV vRes;
	TCVScalar fLVal, fRVal, fRes;
	ECodeDataType eLType, eRType, eResType;
	
	eResType = PDT_NOTYPE;
	
	CCodeVar &rLVar = _rLVar.DereferenceVarPtr(true);
	CCodeVar &rRVar = _rRVar.DereferenceVarPtr(true);

	eLType = rLVar.BaseType();
	eRType = rRVar.BaseType();	
	
	if (eLType == PDT_INT && eRType == PDT_INT)
	{
		// This is standard bitwise OR
		TCVCounter iL, iR, iRes;

		rLVar.CastToCounter(iL);
		rRVar.CastToCounter(iR);

		iRes = iL | iR;

		rResVar.New(PDT_INT, "Constant");
		rResVar = iRes;
		return true;
	}
	else if (eLType == PDT_MULTIV)
	{
		eResType = PDT_MULTIV;
		TMultiV &vA = *rLVar.GetMultiVPtr();
		
		if (eRType == PDT_MULTIV)
		{
			TMultiV &vB = *rRVar.GetMultiVPtr();
			if (vA.GetBase().BaseID() != vB.GetBase().BaseID())
			{
				m_ErrorList.InvalidMVType(rLVar, rRVar, iLine, iPos);
				return false;
			}

			if (vA.GetBase().BaseID() == ID_CONFGA)
			{
				vRes = Join(vA , vB, m_N3GABase.vEm, true, m_fSensitivity);
			}
			else
			{
				vRes = Join(vA , vB, true, m_fSensitivity);
			}
		}
		else
		{
			if (!rRVar.CastToScalar(fRVal, m_fSensitivity))
			{
				m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
				return false;
			}
			
			TMultiV vB(vA.GetStyle());
			vB = fRVal;

			if (vA.GetBase().BaseID() == ID_CONFGA)
			{
				vRes = Join(vA , vB, m_N3GABase.vEm, true, m_fSensitivity);
			}
			else
			{
				vRes = Join(vA , vB, true, m_fSensitivity);
			}
		}
	}
	else if (rLVar.CastToScalar(fLVal, m_fSensitivity))
	{
		if (eRType == PDT_MULTIV)
		{
			eResType = PDT_MULTIV;
			
			TMultiV &vA = *rRVar.GetMultiVPtr();
			TMultiV vB(vA.GetStyle());
			vB = fLVal;
			
			if (vA.GetBase().BaseID() == ID_CONFGA)
			{
				vRes = Join(vB , vA, m_N3GABase.vEm, true, m_fSensitivity);
			}
			else
			{
				vRes = Join(vB , vA, true, m_fSensitivity);
			}
		}
		else
		{
			if (!rRVar.CastToScalar(fRVal, m_fSensitivity))
			{
				m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
				return false;
			}
			
			eResType = PDT_SCALAR;
			fRes = 1;
		}
	}
	else
	{
		m_ErrorList.InvalidLVal(rLVar, iLine, iPos);
		return false;
	}
	
	rResVar.New(eResType, "Constant");
	if (eResType == PDT_MULTIV)
		rResVar = vRes.TinyToZero(m_fSensitivity);
	else
		rResVar = fRes;
	
	return true;
}
//////////////////////////////////////////////////////////////////////
/// Meet Operator &
///
// TODO Update docu for meet. Now also for null blades.

bool CCLUCodeBase::OpMeet(CCodeVar& _rLVar, CCodeVar& _rRVar, CCodeVar& rResVar, int iLine, int iPos)
{
	TMultiV vRes;
	TCVScalar fLVal, fRVal, fRes;
	ECodeDataType eLType, eRType, eResType;
	
	eResType = PDT_NOTYPE;
	
	CCodeVar &rLVar = _rLVar.DereferenceVarPtr(true);
	CCodeVar &rRVar = _rRVar.DereferenceVarPtr(true);

	eLType = rLVar.BaseType();
	eRType = rRVar.BaseType();	

	if (eLType == PDT_INT && eRType == PDT_INT)
	{
		// This is standard bitwise AND
		TCVCounter iL, iR, iRes;

		rLVar.CastToCounter(iL);
		rRVar.CastToCounter(iR);

		iRes = iL & iR;

		rResVar.New(PDT_INT, "Constant");
		rResVar = iRes;
		return true;
	}
	else if (eLType == PDT_MULTIV)
	{
		eResType = PDT_MULTIV;
		TMultiV &vA = *rLVar.GetMultiVPtr();
		
		if (eRType == PDT_MULTIV)
		{
			TMultiV &vB = *rRVar.GetMultiVPtr();
			if (vA.GetBase().BaseID() != vB.GetBase().BaseID())
			{
				m_ErrorList.InvalidMVType(rLVar, rRVar, iLine, iPos);
				return false;
			}

			if (vA.GetBase().BaseID() == ID_CONFGA)
			{
				vRes = Meet(vA, vB, m_N3GABase.vEm, m_fSensitivity);
			}
			else
			{
				vRes = Meet(vA, vB, m_fSensitivity);
			}
		}
		else
		{
			if (!rRVar.CastToScalar(fRVal, m_fSensitivity))
			{
				m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
				return false;
			}		
			TMultiV vB(vA.GetStyle());
			vB = fRVal;

			if (vA.GetBase().BaseID() == ID_CONFGA)
			{
				vRes = Meet(vA, vB, m_N3GABase.vEm, m_fSensitivity);
			}
			else
			{
				vRes = Meet(vA, vB, m_fSensitivity);
			}
		}
	}
	else if (rLVar.CastToScalar(fLVal, m_fSensitivity))
	{
		if (eRType == PDT_MULTIV)
		{
			eResType = PDT_MULTIV;
			TMultiV &vA = *rRVar.GetMultiVPtr();
			TMultiV vB(vA.GetStyle());
			vB = fLVal;			

			if (vA.GetBase().BaseID() == ID_CONFGA)
			{
				vRes = Meet(vA, vB, m_N3GABase.vEm, m_fSensitivity);
			}
			else
			{
				vRes = Meet(vA, vB, m_fSensitivity);
			}
		}
		else
		{
			if (!rRVar.CastToScalar(fRVal, m_fSensitivity))
			{
				m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
				return false;
			}
			eResType = PDT_SCALAR;
			
			if (fRVal == TCVScalar(0) || fLVal == TCVScalar(0))
				fRes = 0;
			else
				fRes = 1;
		}
	}
	else
	{
		m_ErrorList.InvalidLVal(rLVar, iLine, iPos);
		return false;
	}
	
	rResVar.New(eResType, "Constant");
	if (eResType == PDT_MULTIV)
		rResVar = vRes.TinyToZero(m_fSensitivity);
	else
		rResVar = fRes;
	
	return true;
}
//////////////////////////////////////////////////////////////////////
/// Subtract Operator -
///
bool CCLUCodeBase::OpSubtract(CCodeVar& _rLVar, CCodeVar& _rRVar, CCodeVar& rResVar, int iLine, int iPos)
{
	TMatrix xRes;
	TMultiV vRes;
	TCVScalar fLVal, fRVal, fRes;
	ECodeDataType eLType, eRType, eResType;
	
	eResType = PDT_NOTYPE;
	
	CCodeVar &rLVar = _rLVar.DereferenceVarPtr(true);
	CCodeVar &rRVar = _rRVar.DereferenceVarPtr(true);

	eLType = rLVar.BaseType();
	eRType = rRVar.BaseType();	
	
	if (eLType == PDT_MULTIV)
	{
		eResType = PDT_MULTIV;
		TMultiV &vA = *rLVar.GetMultiVPtr();
		
		if (eRType == PDT_MULTIV)
		{
			TMultiV &vB = *rRVar.GetMultiVPtr();
			if (vA.GetBase().BaseID() != vB.GetBase().BaseID())
			{
				m_ErrorList.InvalidMVType(rLVar, rRVar, iLine, iPos);
				return false;
			}
			vRes = vA - vB;
		}
		else
		{
			if (!rRVar.CastToScalar(fRVal, m_fSensitivity))
			{
				m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
				return false;
			}
			
			TMultiV vB(vA.GetStyle());
			vB = fRVal;
			
			vRes = vA - vB;
		}
	}
	else if (eLType == PDT_MATRIX)
	{
		TCVScalar dVal;
		eResType = PDT_MATRIX;
		TMatrix &xA = *rLVar.GetMatrixPtr();
		
		if (rRVar.CastToScalar(dVal, m_fSensitivity))
		{
			xRes = xA - dVal;
		}
		else if (eRType == PDT_MATRIX)
		{
			xRes = xA - *rRVar.GetMatrixPtr();
		}
		else
		{
			m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
			return false;
		}
	}
	else if (eLType == PDT_TENSOR_IDX)
	{
		TTensorIdx &rLTIdx = *rLVar.GetTensorIdxPtr();

		if (eRType == PDT_TENSOR_IDX)
		{
			TTensorIdx &rRTIdx = *rRVar.GetTensorIdxPtr();
			
			// Create a temporary variable that contains tensor.
			// Do this, so that we can return a tensor index.
			m_mTempVarList.Add(1);
			CCodeVar &rTemp = m_mTempVarList.Last();
			rTemp.New(PDT_TENSOR, "Constant");
			TTensor& rT = *rTemp.GetTensorPtr();

			rResVar.New(PDT_TENSOR_IDX, "Constant");
			TTensorIdx& rTIdx = *rResVar.GetTensorIdxPtr();

			if (!TensorSubPoint(rT, rTIdx, rLTIdx, rRTIdx))
			{
				m_ErrorList.GeneralError("Error subtracting tensors.", iLine, iPos);
				return false;
			}

			return true;
		}
		else if (rRVar.CastToScalar(fRVal, m_fSensitivity))
		{
			// Create a temporary variable that contains tensor.
			// Do this, so that we can return a tensor index.
			m_mTempVarList.Add(1);
			CCodeVar &rTemp = m_mTempVarList.Last();
			rTemp.New(PDT_TENSOR, "Constant");
			TTensor& rT = *rTemp.GetTensorPtr();

			rResVar.New(PDT_TENSOR_IDX, "Constant");
			TTensorIdx& rTIdx = *rResVar.GetTensorIdxPtr();

			if (!TensorIdxSubRightScalar(rT, rTIdx, rLTIdx, fRVal))
			{
				m_ErrorList.GeneralError("Error subtracting from tensor.", iLine, iPos);
				return false;
			}

			return true;			
		}
		else
		{
			m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
			return false;
		}
	}
	else if (eLType == PDT_TENSOR)
	{
		TTensor &rLT = *rLVar.GetTensorPtr();

		if (rRVar.CastToScalar(fRVal, m_fSensitivity))
		{
			rResVar.New(PDT_TENSOR, "Constant");
			TTensor& rT = *rResVar.GetTensorPtr();

			if (!TensorSubRightScalar(rT, rLT, fRVal))
			{
				m_ErrorList.GeneralError("Error subtracting from tensor.", iLine, iPos);
				return false;
			}

			return true;			
		}
		else
		{
			m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
			return false;
		}
	}
	else if (eLType == PDT_COLOR && eRType == PDT_COLOR)
	{
		TOGLColor &Col1 = *rLVar.GetOGLColorPtr();
		TOGLColor &Col2 = *rRVar.GetOGLColorPtr();
		TOGLColor ResCol;
		
		ResCol = Col1 - Col2;
		//ResCol.Clamp();
		
		rResVar = ResCol;
		return true;
	}
	else if (rLVar.CastToScalar(fLVal, m_fSensitivity))
	{
		if (eRType == PDT_MULTIV)
		{
			eResType = PDT_MULTIV;
			
			TMultiV &vA = *rRVar.GetMultiVPtr();
			TMultiV vB(vA.GetStyle());
			vB = fLVal;
			
			vRes = vB - vA;
		}
		else if (eRType == PDT_TENSOR_IDX)
		{
			TTensorIdx &rRTIdx = *rRVar.GetTensorIdxPtr();

			// Create a temporary variable that contains tensor.
			// Do this, so that we can return a tensor index.
			m_mTempVarList.Add(1);
			CCodeVar &rTemp = m_mTempVarList.Last();
			rTemp.New(PDT_TENSOR, "Constant");
			TTensor& rT = *rTemp.GetTensorPtr();

			rResVar.New(PDT_TENSOR_IDX, "Constant");
			TTensorIdx& rTIdx = *rResVar.GetTensorIdxPtr();

			if (!TensorIdxSubLeftScalar(rT, rTIdx, rRTIdx, fLVal))
			{
				m_ErrorList.GeneralError("Error subtracting tensor from scalar.", iLine, iPos);
				return false;
			}

			return true;			
		}
		else if (eRType == PDT_TENSOR)
		{
			TTensor &rRT = *rRVar.GetTensorPtr();

			rResVar.New(PDT_TENSOR, "Constant");
			TTensor& rT = *rResVar.GetTensorPtr();

			if (!TensorSubLeftScalar(rT, rRT, fLVal))
			{
				m_ErrorList.GeneralError("Error subtracting scalar from tensor.", iLine, iPos);
				return false;
			}

			return true;			
		}
		else
		{
			if (!rRVar.CastToScalar(fRVal, m_fSensitivity))
			{
				m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
				return false;
			}
			
			eResType = PDT_SCALAR;
			fRes = fLVal - fRVal;
		}
	}
	else
	{
		m_ErrorList.InvalidLVal(rLVar, iLine, iPos);
		return false;
	}
	
	rResVar.New(eResType, "Constant");
	if (eResType == PDT_MULTIV)
		rResVar = vRes.TinyToZero(m_fSensitivity);
	else if (eResType == PDT_MATRIX)
	{
		xRes.TinyToZero(m_fSensitivity);
		rResVar = xRes;
	}
	else
		rResVar = fRes;
	
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Dual Operator *
///
bool CCLUCodeBase::OpDual(CCodeVar& _rVar, CCodeVar& rResVar, int iLine, int iPos)						  
{
	TMultiV vRes;
	ECodeDataType eRType;
	
	CCodeVar &rVar = _rVar.DereferenceVarPtr(true);

	eRType = rVar.BaseType();
	
	if (eRType == PDT_MULTIV)
	{				
		rResVar = (*(*rVar.GetMultiVPtr())).TinyToZero(m_fSensitivity);
	}
	else
	{			
		cStr csOp = "dual operator '*'";
		m_ErrorList.InvalidTypeOp(rVar, csOp, iLine, iPos);
		return false;
	}
	rResVar.Name() = "Constant";	
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Involution Operator '
///
bool CCLUCodeBase::OpInvolute(CCodeVar& _rVar, CCodeVar& rResVar, int iLine, int iPos)						  
{
	TMultiV vRes;
	ECodeDataType eRType;
	
	CCodeVar &rVar = _rVar.DereferenceVarPtr(true);

	eRType = rVar.BaseType();
	
	if (eRType == PDT_MULTIV)
	{				
		rResVar = Involute(*rVar.GetMultiVPtr()); //).Round(m_fSensitivity);
	}
	else
	{			
		cStr csOp = "involution operator >> ' <<";
		m_ErrorList.InvalidTypeOp(rVar, csOp, iLine, iPos);
		return false;
	}
	rResVar.Name() = "Constant";	
	return true;
}





//////////////////////////////////////////////////////////////////////
/// Inverse Operator !
///
bool CCLUCodeBase::OpInverse(CCodeVar& _rVar, CCodeVar& rResVar, int iLine, int iPos)						  
{
	TCVScalar fRes;
	TMultiV vRes;
	ECodeDataType eRType;
	
	CCodeVar &rVar = _rVar.DereferenceVarPtr(true);

	eRType = rVar.BaseType();
	
	if (rVar.CastToScalar(fRes, m_fSensitivity))
	{
		if (::IsZero(fRes, m_fSensitivity))
		{
			m_ErrorList.DivByZero(iLine, iPos);
			return false;
		}
		rResVar = TCVScalar(1) / fRes;
	}
	else if (eRType == PDT_MULTIV)
	{				
		TMultiV vA = *rVar.GetMultiVPtr();
		TMultiV vB(vA.GetStyle()), vX(vA.GetStyle());
		Mem<TCVScalar> mDiag;
		
		vB = TCVScalar(1);
		
		MultiVSolve(vA, vB, vX, Right, &mDiag);
		if (::IsZero(mDiag[0], m_fSensitivity))
			vX = TCVScalar(0);
		
		rResVar = vX.TinyToZero(m_fSensitivity);
	}
	else if (eRType == PDT_MATRIX)
	{
		TMatrix &xA = *rVar.GetMatrixPtr();
		TMatrix xB;
		
		xB = xA.InvSVD(m_fSensitivity);
		xB.TinyToZero(m_fSensitivity);
		rResVar = xB;
	}
	else
	{			
		cStr csOp = "inverse operator '!'";
		m_ErrorList.InvalidTypeOp(rVar, csOp, iLine, iPos);
		return false;
	}
	rResVar.Name() = "Constant";	
	
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Negate Operator -
bool CCLUCodeBase::OpNegate(CCodeVar& _rVar, CCodeVar& rResVar, int iLine, int iPos)
{
	ECodeDataType eType;
	
	CCodeVar &rVar = _rVar.DereferenceVarPtr(true);

	eType = rVar.BaseType();
	
	switch(eType)
	{
	case PDT_INT:
		rResVar = -( *rVar.GetIntPtr() );
		break;
		
	case PDT_UINT:
		rResVar = ( *rVar.GetUIntPtr() );
		break;
		
	case PDT_LONG:
		rResVar = -( *rVar.GetLongPtr() );
		break;
		
	case PDT_FLOAT:
		rResVar = -( *rVar.GetFloatPtr() );
		break;
		
	case PDT_DOUBLE:
		rResVar = -( *rVar.GetDoublePtr() );
		break;
		
	case PDT_MULTIV:
		rResVar = -( *rVar.GetMultiVPtr() );
		break;
		
	case PDT_MATRIX:
		rResVar = -( *rVar.GetMatrixPtr() );
		break;
	case PDT_TENSOR_IDX:
		{
			TTensorIdx &rTIdxData = *rVar.GetTensorIdxPtr();

			// Create a temporary variable that contains tensor.
			// Do this, so that we can return a tensor index.
			m_mTempVarList.Add(1);
			CCodeVar &rTemp = m_mTempVarList.Last();
			rTemp.New(PDT_TENSOR, "Constant");
			TTensor& rT = *rTemp.GetTensorPtr();

			rResVar.New(PDT_TENSOR_IDX, "Constant");
			TTensorIdx& rTIdx = *rResVar.GetTensorIdxPtr();

			if (!TensorIdxNegate(rT, rTIdx, rTIdxData))
			{
				m_ErrorList.GeneralError("Error negating tensor.", iLine, iPos);
				return false;
			}

			return true;			
		}

	case PDT_TENSOR:
		{
			TTensor &rTData = *rVar.GetTensorPtr();

			rResVar.New(PDT_TENSOR, "Constant");
			TTensor& rT = *rResVar.GetTensorPtr();

			if (!TensorNegate(rT, rTData))
			{
				m_ErrorList.GeneralError("Error negating tensor.", iLine, iPos);
				return false;
			}

			return true;			
		}

	default:
		cStr csOp = "negate operator '-'";
		m_ErrorList.InvalidTypeOp(rVar, csOp, iLine, iPos);
		return false;
	}
	
	rResVar.Name() = "Constant";	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Operator Reverse

bool CCLUCodeBase::OpReverse(CCodeVar& _rVar, CCodeVar& rResVar, int iLine, int iPos)
{
	ECodeDataType eType;
	
	CCodeVar &rVar = _rVar.DereferenceVarPtr(true);

	eType = rVar.BaseType();
	
	switch(eType)
	{
	case PDT_INT:
		rResVar = ( *rVar.GetIntPtr() );
		break;
		
	case PDT_UINT:
		rResVar = ( *rVar.GetUIntPtr() );
		break;
		
	case PDT_LONG:
		rResVar = ( *rVar.GetLongPtr() );
		break;
		
	case PDT_FLOAT:
		rResVar = ( *rVar.GetFloatPtr() );
		break;
		
	case PDT_DOUBLE:
		rResVar = ( *rVar.GetDoublePtr() );
		break;
		
	case PDT_MULTIV:
		rResVar = ~( *rVar.GetMultiVPtr() );
		break;
		
	case PDT_MATRIX:
		rResVar = ~( *rVar.GetMatrixPtr() );
		break;
		
	default:
		cStr csOp = "reverse operator '~'";
		m_ErrorList.InvalidTypeOp(rVar, csOp, iLine, iPos);
		return false;
	}
	
	rResVar.Name() = "Constant";
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Create list integer values between given start and end value
///
bool CCLUCodeBase::OpStepList(CCodeVar& _rLVar, CCodeVar& _rRVar, CCodeVar& rResVar, int iLine, int iPos)
{
	TMatrix xRes;
	TMultiV vRes;
	TCVCounter iMinVal, iMaxVal, iStepVal, iVal, iIdxPos, iCount;
	
	CCodeVar &rLVar = _rLVar.DereferenceVarPtr(true);
	CCodeVar &rRVar = _rRVar.DereferenceVarPtr(true);

	if (!rLVar.CastToCounter(iMinVal))
	{
		m_ErrorList.GeneralError("Invalid left value. Expected an integer.", iLine, iPos);
		return false;
	}

	if (!rRVar.CastToCounter(iMaxVal))
	{
		m_ErrorList.GeneralError("Invalid right value. Expected an integer.", iLine, iPos);
		return false;
	}

	if (iMinVal > iMaxVal)
	{
		iStepVal = -1;
		iCount = iMinVal - iMaxVal + 1;
	}
	else if (iMinVal < iMaxVal)
	{
		iStepVal = 1;
		iCount = iMaxVal - iMinVal + 1;
	}
	else
	{
		iStepVal = 0;
		iCount = 1;
	}

	rResVar.New(PDT_VARLIST, "Constant");
	TVarList &rIdxList = *rResVar.GetVarListPtr();
	rIdxList.Set(iCount);

	iVal = iMinVal;
	for( iIdxPos = 0; iIdxPos < iCount; iIdxPos++ )
	{
		rIdxList[iIdxPos].New(PDT_VARLIST, "Constant");
		TVarList &rSubList = *rIdxList[iIdxPos].GetVarListPtr();
		rSubList.Set(1);

		rSubList[0] = iVal;

		iVal += iStepVal;
	} 


	return true;
}




//////////////////////////////////////////////////////////////////////
/// Implementation of operator ^

bool CCLUCodeBase::OpWedge(CCodeVar& _rLVar, CCodeVar& _rRVar, CCodeVar& rResVar, int iLine, int iPos)
{
	TMultiV vRes;
	TCVScalar fLVal, fRVal, fRes;
	ECodeDataType eLType, eRType, eResType;
	
	eResType = PDT_NOTYPE;
	
	CCodeVar &rLVar = _rLVar.DereferenceVarPtr(true);
	CCodeVar &rRVar = _rRVar.DereferenceVarPtr(true);

	eLType = rLVar.BaseType();
	eRType = rRVar.BaseType();	
	
	if (eLType == PDT_MULTIV)
	{
		eResType = PDT_MULTIV;
		TMultiV &vA = *rLVar.GetMultiVPtr();
		
		if (eRType == PDT_MULTIV)
		{
			TMultiV &vB = *rRVar.GetMultiVPtr();
			if (vA.GetBase().BaseID() != vB.GetBase().BaseID())
			{
				m_ErrorList.InvalidMVType(rLVar, rRVar, iLine, iPos);
				return false;
			}
			vRes = vA ^ vB;
		}
		else
		{
			if (!rRVar.CastToScalar(fRVal, m_fSensitivity))
			{
				m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
				return false;
			}
			
			vRes = vA ^ fRVal;
		}
	}
	else if (rLVar.CastToScalar(fLVal, m_fSensitivity))
	{
		if (eRType == PDT_MULTIV)
		{
			eResType = PDT_MULTIV;
			vRes = fLVal ^ *rRVar.GetMultiVPtr();
		}
		else
		{
			if (!rRVar.CastToScalar(fRVal, m_fSensitivity))
			{
				m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
				return false;
			}
			
			eResType = PDT_SCALAR;
			fRes = fLVal * fRVal;
		}
	}
	else
	{
		m_ErrorList.InvalidLVal(rLVar, iLine, iPos);
		return false;
	}
	
	rResVar.New(eResType, "Constant");
	if (eResType == PDT_MULTIV)
		rResVar = vRes.TinyToZero(m_fSensitivity);
	else
		rResVar = fRes;
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Operator Draw OGL
//OpOGLDraw(rRVar, rResVar, iLine, iPos)

bool CCLUCodeBase::OpOGLDraw(CCodeVar& _rVar, CCodeVar& rResVar, int iLine, int iPos)
{
	if (!m_pFilter)
	{
		m_ErrorList.Internal(iLine, iPos);
		return false;
	}
	
	CCodeVar &rVar = _rVar.DereferenceVarPtr(true);

	TVexList *pVexList;
	TImage *pImg;
	COGLBitmap *pBMP;
	ECodeDataType eType = rVar.BaseType();
	
	switch(eType)
	{
	case PDT_MULTIV:
		m_pFilter->DrawMV(*rVar.GetMultiVPtr());
		break;
		
	case PDT_COLOR:
		m_pFilter->SetColor(*rVar.GetOGLColorPtr());
		break;
		
	case PDT_INT:
		m_pFilter->SetMode(*rVar.GetIntPtr());
		break;
		
	case PDT_MATRIX:
		DrawMatrix(*rVar.GetMatrixPtr(), iLine, iPos);
		break;
		
	case PDT_VEXLIST:
		pVexList = rVar.GetVexListPtr();
		// The vertex list may be temporary, so we have to store
		// it in a separate temporary vertex list list.
		//m_mTempVexList.Add(1);
		//m_pFilter->DrawVertexList(m_mTempVexList.Last() = *pVexList);
		m_pFilter->DrawVertexList(*pVexList);
		break;

	case PDT_IMAGE:
		pImg = rVar.GetImagePtr();
		if (pImg->IsValid())
		{
			pBMP = (COGLBitmap *) (*pImg);
			
			pBMP->SetPosition(m_xBMPPos);
			pBMP->SetVAlign(m_fVBMPAlign);
			pBMP->SetHAlign(m_fHBMPAlign);
			pBMP->SetScale(m_fCurBitmapScale);
			pBMP->EnableTransparency(m_bUseBMPTrans);
			pBMP->SetTransparentColor(m_oglBMPTransCol);
			pBMP->SetTransparencyMode(COGLBitmap::Exact);
			
			pBMP->Apply();
		}
		break;

	default:
		m_ErrorList.InvalidType(rVar, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Print value of variable as text output

bool CCLUCodeBase::OpPrint(CCodeVar& _rVar, CCodeVar& rResVar, int iLine, int iPos)
{
	// Get reference to content of variable
	CCodeVar &rVar = _rVar.DereferenceVarPtr(true);

	if (rVar.BaseType() == PDT_NOTYPE)
	{
		m_ErrorList.InvalidType(rVar, iLine, iPos);
		return false;
	}

	
	SOutputObject OutObj;

	OutObj.sName = _rVar.Name();
	OutObj.sType = rVar.TypeStr();
	
	if (_rVar.Type() == PDT_PTR_VAR)
		OutObj.sRelSym = "-> ";
	else
		OutObj.sRelSym = "= ";

	if (!CastToHTMLString(rVar, OutObj.sValue))
	{
		m_ErrorList.InvalidType(rVar, iLine, iPos);
		return false;
	}

	if (rVar.BaseType() == PDT_MATRIX)
	{
		TMatrix &rMat = *rVar.GetMatrixPtr();

		OutObj.sInfo << rMat.Rows() << "x" << rMat.Cols();
		OutObj.bShowInfo = true;

		OutObj.bBRBeforeValue = false;
		OutObj.bBRAfterValue = false;
	}
	else if (rVar.BaseType() == PDT_TENSOR)
	{
		TTensor &rT = *rVar.GetTensorPtr();

		int iDim, iValence = rT.Valence();
		for (iDim = 0; iDim < iValence; iDim++)
		{
			OutObj.sInfo << rT.DimSize(iDim);
			if (iDim < iValence-1)
				OutObj.sInfo << "x";
		}
		OutObj.bShowInfo = true;

		OutObj.bBRBeforeValue = false;
		OutObj.bBRAfterValue = false;
	}
	else if (rVar.BaseType() == PDT_TENSOR_IDX)
	{
		TTensorIdx &rTIdx = *rVar.GetTensorIdxPtr();
		CTensorSingleLoop<TCVScalar> Loop;

		Loop.Set(rTIdx);
		int iLoop, iLoopCount = Loop.Init();
	
		if (iLoopCount == 0)
			OutObj.sInfo = "1";
		else
		{
			for (iLoop = iLoopCount-1; iLoop >= 0; iLoop--)
			{
				OutObj.sInfo << Loop.StepCount(iLoop);
				if (iLoop > 0)
					OutObj.sInfo << "x";
			}
		}
		OutObj.bShowInfo = true;

		OutObj.bBRBeforeValue = false;
		OutObj.bBRAfterValue = false;
	}
	else if (rVar.BaseType() == PDT_VARLIST)
	{
		TVarList &rList = *rVar.GetVarListPtr();

		OutObj.sInfo << rList.Count();
		OutObj.bShowInfo = true;
		OutObj.bBRBeforeValue = true;
		OutObj.bBRAfterValue = true;
	}
	else if (rVar.BaseType() == PDT_VEXLIST)
	{
		TVexList &rList = *rVar.GetVexListPtr();

		OutObj.sInfo << rList.Count();
		OutObj.bShowInfo = true;
		OutObj.bBRBeforeValue = true;
		OutObj.bBRAfterValue = true;
	}
	else if (rVar.BaseType() == PDT_STRING)
	{
		TString &rStr = *rVar.GetStringPtr();

		OutObj.sInfo << rStr.Len();
		OutObj.bShowInfo = true;
		OutObj.bBRBeforeValue = true;
		OutObj.bBRAfterValue = true;
	}
	else
	{
		OutObj.sInfo = "";
		OutObj.bShowInfo = false;
		OutObj.bBRBeforeValue = false;
		OutObj.bBRAfterValue = false;
	}

	if (rVar.BaseType() == PDT_MATRIX ||
		rVar.BaseType() == PDT_TENSOR ||
		rVar.BaseType() == PDT_TENSOR_IDX ||
		rVar.BaseType() == PDT_VARLIST ||
		rVar.BaseType() == PDT_VEXLIST)
	{
		OutObj.iSize = -1;
		OutObj.bCanHideValue = true;
		OutObj.bCanChangeSize = true;
	}
	else
	{
		OutObj.iSize = 0;
		OutObj.bCanHideValue = false;
		OutObj.bCanChangeSize = false;
	}

	OutObj.bShowValue = true;

	m_vecOutputObject.push_back(OutObj);

/*
	if (rVar.DereferenceVarPtr(true).Type() == PDT_STRING)
		m_csOutput << csVal << "<br>\n";
	else 
		m_csOutput << csName << csVal << "<br>\n";
*/
/* Old version */
/*
	if (rVar.Type() == PDT_PTR_VAR)
		csName << rVar.Name() << " -> ";
	else
		csName << rVar.Name() << " = ";

	if (!CastToString(rVar, csVal))
	{
		m_ErrorList.InvalidType(rVar, iLine, iPos);
		return false;
	}

	if (rVar.DereferenceVarPtr(true).Type() == PDT_STRING)
		m_csOutput << csVal << "\n";
	else 
		m_csOutput << csName << csVal << "\n";
*/

	return true;
}


//////////////////////////////////////////////////////////////////////
// Get Mouse Value FUNCTION
//

bool CCLUCodeBase::OpMod(CCodeVar& _rLVar, CCodeVar& _rRVar, CCodeVar& rVar, int iLine, int iPos)
{
	CCodeVar &rLVar = _rLVar.DereferenceVarPtr(true);
	CCodeVar &rRVar = _rRVar.DereferenceVarPtr(true);

	ECodeDataType eLType = rLVar.BaseType();
	ECodeDataType eRType = rRVar.BaseType();
	
	if (eLType == PDT_INT)
	{
		if (eRType == PDT_INT)
		{
			int iVal = *rRVar.GetIntPtr();
			
			if (iVal > 0)
			{
				rVar = *rLVar.GetIntPtr() % iVal;
			}
			else
			{
				m_ErrorList.InvalidParVal(rRVar, 2, iLine, iPos);
				return false;
			}
		}
		else if (eRType == PDT_SCALAR)
		{
			TCVScalar fVal, fRVal;
			
			if (!rLVar.CastToScalar(fVal, m_fSensitivity))
			{
				m_ErrorList.InvalidParType(rLVar, 1, iLine, iPos);
				return false;
			}
			
			fRVal = *rRVar.GetScalarPtr();
			
			if (fRVal > 0)
			{
				rVar = (TCVScalar) fmod((double) fVal, (double) fRVal);
			}
			else
			{
				m_ErrorList.InvalidParVal(rRVar, 2, iLine, iPos);
				return false;
			}
		}
		else
		{
			m_ErrorList.InvalidParType(rRVar, 2, iLine, iPos);
			return false;
		}
	}
	else if (eLType == PDT_SCALAR)
	{
		TCVScalar fVal;
		
		if (!rRVar.CastToScalar(fVal, m_fSensitivity))
		{
			m_ErrorList.InvalidParType(rRVar, 2, iLine, iPos);
			return false;
		}
		
		if (fVal > 0)
		{
			rVar = (TCVScalar) fmod((double) *rLVar.GetScalarPtr(), (double) fVal);
		}
		else
		{
			m_ErrorList.InvalidParVal(rRVar, 2, iLine, iPos);
			return false;
		}
	}
	else
	{
		m_ErrorList.InvalidParType(rLVar, 1, iLine, iPos);
		return false;
	}
	
	return true;
}
//////////////////////////////////////////////////////////////////////
/// Grade Projection Operator 
///
bool CCLUCodeBase::OpGradeProj(CCodeVar& _rLVar, CCodeVar& _rRVar, CCodeVar& rResVar, int iLine, int iPos)
{
	TMultiV vRes;
	TCVCounter iNo = -1;
	TCVScalar fLVal, fRes=0;
	TVarList *pNoList = 0;
	ECodeDataType eLType, eRType, eResType;
	cStr csOp = "grade projection operator ''";
	
	eResType = PDT_NOTYPE;
	
	CCodeVar &rLVar = _rLVar.DereferenceVarPtr(true);
	CCodeVar &rRVar = _rRVar.DereferenceVarPtr(true);

	eLType = rLVar.BaseType();
	eRType = rRVar.BaseType();	
	
	if (!rRVar.CastToCounter(iNo)) 
	{
		m_ErrorList.InvalidTypeOp(rRVar, csOp, iLine, iPos);		
		return false;
	}
		
	if (eLType == PDT_MULTIV)
	{
		eResType = PDT_MULTIV;
		TMultiV &vA = *rLVar.GetMultiVPtr();	

		if (iNo < 0 || iNo > int(vA.GetBase().VSDim()))
		{
			m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
			return false;
		}

		vRes = vA(iNo);		
	}
	else 
	{
		if (!rLVar.CastToScalar(fLVal, m_fSensitivity))
		{
			m_ErrorList.InvalidLVal(rLVar, iLine, iPos);
			return false;
		}
		eResType = PDT_SCALAR;
		if (iNo == 0) fRes = fLVal;
		else fRes = 0;			
	}
	
	rResVar.New(eResType, "Constant");
	if (eResType == PDT_MULTIV)
		rResVar = vRes;
	else
		rResVar = fRes;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Function Call Operator
///
bool CCLUCodeBase::OpFuncCall(CCodeVar& _rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos)
{
	int iListCount;
	TCVCounter iNo = -1;
	TVarList *pNoList = 0, rSEList;
	ECodeDataType eLType, eRType, eResType;
	cStr csOp = "function call operator";
	
	eResType = PDT_NOTYPE;
	
	CCodeVar &rLVar = _rLVar.DereferenceVarPtr(true);

	eLType = rLVar.BaseType();
	eRType = rRVar.BaseType();	

	if (eRType != PDT_VARLIST)
	{
		m_ErrorList.InvalidRVal(rRVar, iLine, iPos);
		return false;
	}

	TVarList &rList = *rRVar.GetVarListPtr();
	iListCount = rList.Count();

	/// It's a function
	if (eLType == PDT_CODEPTR)
	{
		if (!ExecUserFunc(rResVar, *rLVar.GetCodePtrPtr(), rList, iLine, iPos))
			return false;
	}
	/////////////////////////////////////////////////////////////////////
	/// Something else but a function
	else 
	{
		if (iListCount == 1 && 
			rLVar.BaseType() != PDT_TENSOR &&  // Tensors interpret single list in a particular way
			rList(0).BaseType() == PDT_VARLIST)
		{
			// Check whether the list is actually an index list, 
			// or just a list of consecutive indices, referencing
			// a single element.
			bool bInList = false;
			TVarList &rSubList = *rList(0).GetVarListPtr();
			if (rSubList(0).BaseType() == PDT_VARLIST)
				bInList = true;

			if (!GetObjectElements(rResVar, rLVar, rSubList, bInList, iLine, iPos))
				return false;
		}
		else 
		{
			if (!GetObjectElements(rResVar, rLVar, rList, false, iLine, iPos))
				return false;
		}
	}

	return true;
}


/////////////////////////////////////////////////////////////////
/// Get Sub Element from Object

bool CCLUCodeBase::GetObjectElements(CCodeVar &rVar, CCodeVar &rObj, 
									 TVarList &rIdxList, bool bInList, int iLine, int iPos)
{
	int iIdxPos, iIdxCount = rIdxList.Count();
	ECodeDataType eObjType;

	eObjType = rObj.BaseType();

	if (bInList && rIdxList(0).BaseType() == PDT_VARLIST)
	{
		// There seems to be a list of indices.
		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rResList = *rVar.GetVarListPtr();
		rResList.Set(iIdxCount);

		for (iIdxPos = 0; iIdxPos < iIdxCount; iIdxPos++)
		{
			if (rIdxList(iIdxPos).BaseType() != PDT_VARLIST)
			{
				char pcText[50];

				sprintf(pcText, "Expect variable list at position %d in index list.", iIdxPos+1);
				m_ErrorList.GeneralError(pcText, iLine, iPos);

				return false;
			}

			if (!GetObjectElements(rResList[iIdxPos], rObj, 
						*rIdxList(iIdxPos).GetVarListPtr(), true, iLine, iPos))
				return false;
		}
	}
	else
	{
		// There seems to be a proper index here

		if (eObjType == PDT_MATRIX)
		{
			int iRows, iCols, iNo;
			int iR, iC, iRNo, iCNo;
			TMatrix *pxA;

			pxA = rObj.GetMatrixPtr();

			iRows = pxA->Rows();
			iCols = pxA->Cols();

			if (iIdxCount == 1)
			{
				// Extract row
				if (rIdxList(0).CastToCounter(iNo))
				{
					if (iNo <= 0 || iNo > iRows)
					{
						m_ErrorList.InvalidRVal(rIdxList(0), iLine, iPos);
						return false;
					}

					rVar.New(PDT_MATRIX, "Constant");
					TMatrix &rMat = *rVar.GetMatrixPtr();

					pxA->GetSubMatrix(rMat, iNo, 1, 1, iCols);
				}
				else
				{
					m_ErrorList.InvalidRVal(rIdxList(0), iLine, iPos);
					return false;
				}
			}
			else if (iIdxCount == 2)
			{
				if (!rIdxList(0).CastToCounter(iR) ||
					!rIdxList(1).CastToCounter(iC))
				{
					m_ErrorList.GeneralError("Invalid index.", iLine, iPos);
					return false;
				}


				if (iR <= 0 || iR > iRows ||
					iC <= 0 || iC > iCols)
				{
					m_ErrorList.GeneralError("Index out of range.", iLine, iPos);
					return false;
				}

				if (!bInList)
					rVar = &((*pxA)(iR, iC));
				else
					rVar = ((*pxA)(iR, iC));
			}
			else if (iIdxCount == 4)
			{
				if (!rIdxList(0).CastToCounter(iR) ||
					!rIdxList(1).CastToCounter(iC) ||
					!rIdxList(2).CastToCounter(iRNo) ||
					!rIdxList(3).CastToCounter(iCNo))
				{
					m_ErrorList.GeneralError("Invalid index.", iLine, iPos);
					return false;
				}

				TMatrix xB;

				if (!pxA->GetSubMatrix(xB, iR, iC, iRNo, iCNo))
				{
					m_ErrorList.GeneralError("Error extracting submatrix.", iLine, iPos);
					return false;
				}

				rVar = xB;
			}
			else
			{
				m_ErrorList.GeneralError("Invalid index.", iLine, iPos);
				return false;
			}

			return true;
		}
		else if (eObjType == PDT_TENSOR)
		{
			bool bConstantIdx = true;
			int iDim, iValence, iVal, iMin, iMax;
			TCVScalar dVal, dVal1, dVal2, dVal3;
			Mem<int> mDim, mMin, mMax;
			MemObj<Mem<int> > mIdxList;
			TTensorIdx TIdx;
			TTensor& rT = *rObj.GetTensorPtr();

			iValence = rT.Valence();
			if (iIdxCount != iValence)
			{
				m_ErrorList.GeneralError("Need index for each dimension.", iLine, iPos);
				return false;
			}

			mDim.Set(iValence);
			mMin.Set(iValence);
			mMax.Set(iValence);
			mIdxList.Set(iValence);
			for (iDim = 0; iDim < iValence; iDim++)
			{
				if (rIdxList(iDim).CastToScalar(dVal))
				{
					if (dVal > -1.0 && dVal < 0) // Point Index
					{
						dVal = Mag(dVal);
						dVal1 = floor(dVal * 1e1);
						dVal2 = floor(dVal * 1e2) - dVal1 * 1e1;
						dVal3 = floor(dVal * 1e3) - dVal2 * 1e1 - dVal1 * 1e2;

						iVal = -int( dVal1 + dVal2*1e1 + dVal3*1e2 );
						if (iVal < CTensorContractLoop<TCVScalar>::MIN_CONTRACT_IDX)
						{
							m_ErrorList.GeneralError("Free point index out of range.", iLine, iPos);
							return false;
						}

						iVal += CTensorContractLoop<TCVScalar>::MIN_CONTRACT_IDX;
					}
					else
					{
						iVal = int( dVal );

						if (iVal < CTensorContractLoop<TCVScalar>::MIN_CONTRACT_IDX)
						{
							m_ErrorList.GeneralError("Free index has to be between -1 and -255.", iLine, iPos);
							return false;
						}
					}

					if (iVal == 0 || iVal > rT.DimSize(iDim))
					{
						m_ErrorList.GeneralError("Index out of range.", iLine, iPos);
						return false;
					}

					if (iVal < 0)
						bConstantIdx = false;
					else
						--iVal;

					mDim[iDim] = iVal;
					mMin[iDim] = -1;
					mMax[iDim] = -1;
				}
				else if (rIdxList(iDim).BaseType() == PDT_VARLIST)
				{
					TVarList &rList = *rIdxList(iDim).GetVarListPtr();

					if (rList.Count() != 2 && rList.Count() != 3)
					{
						m_ErrorList.GeneralError("Invalid index range format.", iLine, iPos);
						return false;
					}

					if (!rList[0].CastToScalar(dVal))
					{
						m_ErrorList.GeneralError("Index is not a scalar.", iLine, iPos);
						return false;
					}

					if (dVal > -1.0 && dVal < 0) // Point Index
					{
						dVal = Mag(dVal);
						dVal1 = floor(dVal * 1e1);
						dVal2 = floor(dVal * 1e2) - dVal1 * 1e1;
						dVal3 = floor(dVal * 1e3) - dVal2 * 1e1 - dVal1 * 1e2;

						iVal = -int( dVal1 + dVal2*1e1 + dVal3*1e2 );
						if (iVal < CTensorContractLoop<TCVScalar>::MIN_CONTRACT_IDX)
						{
							m_ErrorList.GeneralError("Free point index out of range.", iLine, iPos);
							return false;
						}

						iVal += CTensorContractLoop<TCVScalar>::MIN_CONTRACT_IDX;
					}
					else
					{
						iVal = int( dVal );

						if (iVal < CTensorContractLoop<TCVScalar>::MIN_CONTRACT_IDX)
						{
							m_ErrorList.GeneralError("Free index has to be between -1 and -255.", iLine, iPos);
							return false;
						}
					}

					if (iVal >= 0)
					{
						m_ErrorList.GeneralError("Only free indices can be given ranges.", iLine, iPos);
						return false;
					}

					bConstantIdx = false;
					mDim[iDim] = iVal;
					mMin[iDim] = -1;
					mMax[iDim] = -1;

					if (rList[1].BaseType() == PDT_VARLIST)
					{
						// Index List given
						TVarList &rIdxList = *rList[1].GetVarListPtr();

						int iDimSize = rT.DimSize(iDim);
						int i, iIdx, iIdxCount = rIdxList.Count();
						mIdxList[iDim].Set(iIdxCount);
						for(i = 0; i < iIdxCount; i++)
						{
							if (!rIdxList[i].CastToCounter(iIdx))
							{
								// Index could be VarList with single element
								if (rIdxList[i].BaseType() == PDT_VARLIST)
								{
									TVarList &rSubList = *rIdxList[i].GetVarListPtr();

									if (rSubList.Count() != 1 || 
										!rSubList[0].CastToCounter(iIdx))
									{
										m_ErrorList.GeneralError("Invalid index in index list.", iLine, iPos);
										return false;
									}
								}
								else
								{
									m_ErrorList.GeneralError("Invalid index in index list.", iLine, iPos);
									return false;
								}
							}
							
							if (iIdx <= 0 || iIdx > iDimSize)
							{
								m_ErrorList.GeneralError("Index in index list out of range.", iLine, iPos);
								return false;
							}
							mIdxList[iDim][i] = iIdx - 1;
						}
					}
					else if (rList[1].CastToCounter(iMin))
					{
						// Index range given

						if (iMin <= 0 || iMin > rT.DimSize(iDim))
						{
							m_ErrorList.GeneralError("Minimum index out of range.", iLine, iPos);
							return false;
						}

						if (!rList[2].CastToCounter(iMax))
						{
							m_ErrorList.GeneralError("Maximum index is not an integer.", iLine, iPos);
							return false;
						}

						if (iMax <= 0 || iMax > rT.DimSize(iDim) || iMax < iMin)
						{
							m_ErrorList.GeneralError("Maximum index out of range.", iLine, iPos);
							return false;
						}

						mMin[iDim] = iMin - 1;
						mMax[iDim] = iMax - 1;
					}
					else
					{
						m_ErrorList.GeneralError("Invalid index range specification.", iLine, iPos);
						return false;
					}

				}
				else
				{
					m_ErrorList.GeneralError("Index is of invalid type.", iLine, iPos);
					return false;
				}

			}

			try
			{
				// Set indices
				TIdx.Set(rT, mDim);

				// Set Index Ranges
				for (iDim = 0; iDim < iValence; iDim++)
				{
					if (mDim[iDim] < 0 && mMin[iDim] >= 0 && mMax[iDim] >= 0)
						TIdx.SetIdxRange(mDim[iDim], mMin[iDim], mMax[iDim]);
					else if (mDim[iDim] < 0 && mIdxList[iDim].Count() > 0)
						TIdx.SetIdxList(mDim[iDim], mIdxList[iDim]);
				}
			}
			catch( ... )
			{
				m_ErrorList.GeneralError("Invalid index.", iLine, iPos);
				return false;
			}

			// If all indices were constant, return value.
			if (bConstantIdx)
			{
				if (!bInList)
					rVar = &(TIdx.GetRef());
				else
					rVar = TIdx.GetRef();
			}
			else
			{
				rVar = TIdx;
			}

			return true;
		}
		else if (eObjType == PDT_IMAGE)
		{
			int iX, iY, iXNo, iYNo;
			COGLBitmap::uchar pucVal[4];

			if (iIdxCount == 2)
			{
				if (!rIdxList(0).CastToCounter(iX) ||
					!rIdxList(1).CastToCounter(iY))
				{
					m_ErrorList.GeneralError("Invalid index.", iLine, iPos);
					return false;
				}

				int iWidth, iHeight;

				TImage &rA = *rObj.GetImagePtr();
				if (!rA.IsValid())
				{
					m_ErrorList.GeneralError("Image is invalid.", iLine, iPos);
					return false;
				}

				rA->GetSize(iWidth, iHeight);

				if (iX <= 0 || iX > iWidth ||
					iY <= 0 || iY > iHeight)
				{
					m_ErrorList.GeneralError("Index out of range.", iLine, iPos);
					return false;
				}

				rVar.New(PDT_VARLIST, "Constant");
				TVarList &rResList = *rVar.GetVarListPtr();

				((COGLBitmap *) rA)->GetPixel(iX-1, iHeight-iY, pucVal[0], pucVal[1], pucVal[2], pucVal[3]);
				rResList.Set(4);

				for (int i = 0; i < 4; i++)
					rResList[i] = TCVScalar(pucVal[i]) / TCVScalar(255);
			}
			else if (iIdxCount == 4)
			{
				if (!rIdxList(0).CastToCounter(iX) ||
					!rIdxList(1).CastToCounter(iY) ||
					!rIdxList(2).CastToCounter(iXNo) ||
					!rIdxList(3).CastToCounter(iYNo))
				{
					m_ErrorList.GeneralError("Invalid index.", iLine, iPos);
					return false;
				}

				TImage &rA = *rObj.GetImagePtr();
				if (!rA.IsValid())
				{
					m_ErrorList.GeneralError("Image is invalid.", iLine, iPos);
					return false;
				}

				int iWidth, iHeight;
				rA->GetSize(iWidth, iHeight);

				// Change iY coordinate since coordinates used by OGLBitmap
				// start bottom left, and in CLUScript top left.
				iY = iHeight - iY + 1 - iYNo + 1;

				if (iXNo <= 0 || iYNo <= 0 ||
					iX <= 0 || iX + iXNo - 1 > iWidth ||
					iY <= 0 || iY + iYNo - 1 > iHeight)
				{
					m_ErrorList.GeneralError("Coordinates of area to extract are out of range.", iLine, iPos);
					return false;
				}

				rVar.New(PDT_IMAGE, "Constant");
				TImage &rImg = *rVar.GetImagePtr();
				rImg->SetFilename("Memory");
				if (!rImg->ExtractSubImage(*((COGLBitmap *) rA), iX-1, iY-1, iXNo, iYNo))
				{
					m_ErrorList.GeneralError("Error extracting subimage.", iLine, iPos);
					return false;
				}
			}
			else
			{
				m_ErrorList.GeneralError("Invalid index.", iLine, iPos);
				return false;
			}
		}
		else if (eObjType ==  PDT_MULTIV)
		{
			if (iIdxCount == 1)
			{
				int iNo;
				TMultiV &vA = *rObj.GetMultiVPtr();

				if (!rIdxList(0).CastToCounter(iNo))
				{
					m_ErrorList.InvalidRVal(rIdxList(0), iLine, iPos);
					return false;
				}

				if (iNo < 1 || iNo > int(vA.GetGADim()))
				{
					m_ErrorList.InvalidRVal(rIdxList(0), iLine, iPos);
					return false;
				}

				rVar.New(PDT_SCALAR, "Constant");
				if (!bInList)
					rVar = &(vA[iNo-1]);
				else
					rVar = vA[iNo - 1];	
			}
			else
			{
				m_ErrorList.GeneralError("Invalid index.", iLine, iPos);
				return false;
			}
		}
		else if (eObjType == PDT_VARLIST)
		{			
			TVarList &rList = *rObj.GetVarListPtr();
			int iLListCount = rList.Count();

			if (iIdxCount == 1 && rIdxList(0).BaseType() == PDT_STRING)
			{
				// Return element in list just after string element
				char *pcVal = (rIdxList(0).GetStringPtr())->Str();

				int i;
				bool bFound = false;

				for (i = 0; i < iLListCount; i++)
				{
					CCodeVar &rElVar = rList(i);

					if (rElVar.BaseType() == PDT_VARLIST)
					{
						TVarList &rSubList = *rElVar.GetVarListPtr();

						if (rSubList.Count() == 2 &&
							rSubList[0].Type() == PDT_STRING &&
							*rSubList[0].GetStringPtr() == cStr(pcVal))
						{
							if (!bInList)
								rVar = &(rSubList[1]);
							else
								rVar = (rSubList[1]);

							bFound = true;
							break;
						}
					}
				}

				// If element was not found then create it
				if (!bFound && !bInList)
				{
					rList.Add(1);
					CCodeVar &rSubVar = rList.Last();

					rSubVar.New(PDT_VARLIST, "Constant");
					TVarList *pSubList = rSubVar.GetVarListPtr();

					pSubList->Add(2);
					(*pSubList)[0] = pcVal;
					(*pSubList)[0].Name() = "Name";

					(*pSubList)[1] = TCVCounter(0);
					(*pSubList)[1].Name() = "Unnamed";

					rVar = &(pSubList->Last());
				}
				else if (!bFound && bInList)
				{
					cStr csText;
					csText << "Element '" << pcVal << "' not found in list.";
					m_ErrorList.GeneralError(csText.Str(), iLine, iPos);
					return false;
				}
			}
			else 
			{
				if (!GetVarListElement(rVar, rList, rIdxList, 0, iLine, iPos))
					return false;
	
				if (bInList)
					rVar.DereferencePtr();
			}
		} 
		else if (eObjType == PDT_VEXLIST)
		{
			int iNo;

			if (iIdxCount == 1)
			{
				if (!rIdxList(0).CastToCounter(iNo))
				{
					m_ErrorList.InvalidRVal(rIdxList(0), iLine, iPos);
					return false;
				}

				if (!rVar.New(PDT_VARLIST, "Constant"))
				{
					m_ErrorList.OutOfMemory(iLine, iPos);
					return false;
				}

				TVexList& rVexList = *rObj.GetVexListPtr();
				TVarList& rVarList = *rVar.GetVarListPtr();

				if (iNo < 1 || iNo > rVexList.Count())
				{
					m_ErrorList.InvalidRVal(rIdxList(0), iLine, iPos);
					return false;
				}

				--iNo;

				rVarList.Set(3);
				rVarList(0) = rVexList[iNo][0];
				rVarList(1) = rVexList[iNo][1];
				rVarList(2) = rVexList[iNo][2];
			}
			else
			{
				m_ErrorList.GeneralError("Invalid index.", iLine, iPos);
				return false;
			}
		}
		else if (eObjType == PDT_STRING)
		{
			int iIdx;
			TString &rStr = *rObj.GetStringPtr();

			if (iIdxCount == 1)
			{
				// Expect single index of letter
				if (rIdxList(0).CastToCounter(iIdx))
				{
					if (iIdx <= 0 || iIdx > int(rStr.Len()))
					{
						m_ErrorList.GeneralError("Index out of range", iLine, iPos);
						return false;
					}

					rVar.New(PDT_STRING, "Constant");
					TString &csStr = *rVar.GetStringPtr();

					csStr = rStr[iIdx-1];
				}
				else if (rIdxList(0).BaseType() == PDT_STRING)
				{
					TString &csIdxStr = *rIdxList(0).GetStringPtr();

					rVar.New(PDT_VARLIST, "Constant");
					TVarList &rList = *rVar.GetVarListPtr();

					char *pcPos = rStr.Str();
					while ((pcPos = strstr(pcPos, csIdxStr.Str())))
					{
						rList.Add(1);
						rList.Last() = TCVCounter(pcPos) - TCVCounter(rStr.Str()) + 1;
						pcPos++;
					}
				}
				else
				{
					m_ErrorList.GeneralError("Invalid index", iLine, iPos);
					return false;
				}
			}
			else if (iIdxCount == 2)
			{
				int iIdx2;

				// Expect single index of letter
				if (!rIdxList(0).CastToCounter(iIdx) || 
					!rIdxList(1).CastToCounter(iIdx2))
				{
					m_ErrorList.GeneralError("Invalid index.", iLine, iPos);
					return false;
				}

				if (iIdx <= 0 || iIdx > int(rStr.Len()) ||
					iIdx2 <= 0 || iIdx2 > int(rStr.Len()))
				{
					m_ErrorList.GeneralError("Index out of range.", iLine, iPos);
					return false;
				}

				rVar.New(PDT_STRING, "Constant");
				TString &csStr = *rVar.GetStringPtr();
				csStr = rStr(iIdx-1, iIdx2-1);
			}
			else
			{
				m_ErrorList.GeneralError("Invalid index.", iLine, iPos);
				return false;
			}
		}
		else 
		{
			//m_ErrorList.InvalidLVal(rObj, iLine, iPos);
			m_ErrorList.GeneralError("Variable to left of '@' is neither a function, matrix, multivector or list.", iLine, iPos);
			return false;
		}
	}	
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////
/// Get element of var. list

bool CCLUCodeBase::GetVarListElement(CCodeVar& rElVar, TVarList& rList, TVarList &rIdxList, 
									 int iIdxPos, int iLine, int iPos)
{
	int iCount = rList.Count();
	TCVCounter iIdx;
	CCodeVar *pSubVar;


 	if (rIdxList(iIdxPos).CastToCounter(iIdx)) 
	{
		if (iIdx > iCount || iIdx <= 0)
		{
			cStr csText;
			csText << "Index out of range at position " << (iIdxPos+1) << " in index list.";
			m_ErrorList.GeneralError(csText.Str(), iLine, iPos);
			return false;
		}

		pSubVar = &rList(iIdx-1);
	}
	else if (rIdxList(iIdxPos).BaseType() == PDT_STRING)
	{
		// Return element index in list just after string element
		char *pcVal = (rIdxList(iIdxPos).GetStringPtr())->Str();

		int i;
		bool bFound = false;

		for (i = 0; i < iCount; i++)
		{
			CCodeVar &rElVar = rList(i);

			if (rElVar.BaseType() == PDT_VARLIST)
			{
				TVarList &rSubList = *rElVar.GetVarListPtr();

				if (rSubList.Count() == 2 &&
					rSubList[0].Type() == PDT_STRING &&
					*rSubList[0].GetStringPtr() == cStr(pcVal))
				{
					pSubVar = &(rSubList[1]);
					bFound = true;
					break;
				}
			}
		}

		if (!bFound)
		{
			cStr csText;
			csText << "Element with index '" << pcVal << "' does not exist in list.";
			m_ErrorList.GeneralError(csText.Str(), iLine, iPos);
			return false;
		}
	}
	else
	{
		cStr csText;
		csText << "Invalid element at position " << (iIdxPos+1) << " in index list.";
		m_ErrorList.GeneralError(csText.Str(), iLine, iPos);
		return false;
	}

	// Are we at the end of the index list?
	if (iIdxPos == rIdxList.Count()-1)
	{
		rElVar = pSubVar;
		return true;
	}

	// If not, then the element at iIdx in rList should be a var. list
	if (pSubVar->BaseType() == PDT_VARLIST)
	{
		return GetVarListElement(rElVar, *(pSubVar->GetVarListPtr()), rIdxList, iIdxPos+1,
								iLine, iPos);
	}
	else
	{
		m_ErrorList.GeneralError("Invalid index.", iLine, iPos);
		return false;
	}

	return true;
}



///////////////////////////////////////////////////////////////////////////////////////
/// Remove element of var. list

bool CCLUCodeBase::RemoveVarListElement(TVarList& rList, TVarList &rIdxList, 
										int iIdxPos, int iLine, int iPos)
{
	int iCount = rList.Count();
	TCVCounter iIdx;

	if (!rIdxList(iIdxPos).CastToCounter(iIdx)) 
	{
		cStr csText;
		csText << "Invalid element at position " << (iIdxPos+1) << " in index list.";
		m_ErrorList.GeneralError(csText.Str(), iLine, iPos);
		return false;
	}

	if (iIdx > iCount || iIdx <= 0)
	{
		cStr csText;
		csText << "Index out of range at position " << (iIdxPos+1) << " in index list.";
		m_ErrorList.GeneralError(csText.Str(), iLine, iPos);
		return false;
	}

	// Are we at the end of the index list?
	if (iIdxPos == rIdxList.Count()-1)
	{
		rList.Del(iIdx-1);
		return true;
	}

	// If not, then the element at iIdx in rList should be a var. list
	CCodeVar &rSubVar = rList(iIdx-1);
	if (rSubVar.BaseType() == PDT_VARLIST)
	{
		return RemoveVarListElement(*rSubVar.GetVarListPtr(), rIdxList, iIdxPos+1,
								iLine, iPos);
	}
	else
	{
		m_ErrorList.GeneralError("Invalid index.", iLine, iPos);
		return false;
	}

	return true;
}
