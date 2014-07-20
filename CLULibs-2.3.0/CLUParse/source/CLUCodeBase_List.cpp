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
#include <FL/fl_ask.H>
#include <FL/Fl_File_Chooser.H>
#include <time.h>
#include "XML.h"

#ifdef _GNUCPP3_
#   include <unistd.h>
#   define _getcwd getcwd
#   define _chdir chdir
#else
#   include <direct.h>
#endif

#ifdef WIN32
#	include <ShellApi.h>
#endif

#include "CLUCodeBase.h"
#include "TensorOperators.h"

#include <vector>
#include <algorithm>
#include <functional>      // For greater<int>( )

using namespace std;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////
/// Cast To List

bool CCLUCodeBase::ListFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList &mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	int iVal;

	if (iVarCount < 1 ||iVarCount > 2)
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	if (mVars(0).CastToCounter(iVal))
	{
		// iVal gives length of new list
		if (iVal < 0)
		{
			m_ErrorList.GeneralError("Lenght of new list has to be >= 0.", iLine, iPos);
			return false;
		}

		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rList = *rVar.GetVarListPtr();

		rList.Set(iVal);
		for( int i = 0; i < iVal; i++ )
		{
			rList[i] = TCVScalar(0);
		}
	}
	else if (mVars(0).BaseType() == PDT_MULTIV)
	{
		TMultiV &vA = *mVars(0).GetMultiVPtr();
		int i, iDim = vA.GetGADim();

		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rList = *rVar.GetVarListPtr();

		rList.Set(iDim);

		for( i = 0; i < iDim; i++ )
		{
			rList[i] = vA[i];
		}
	}
	else if (mVars(0).BaseType() == PDT_MATRIX)
	{
		TMatrix &rM = *mVars(0).GetMatrixPtr();
		int iR, iRows = rM.Rows();
		int iC, iCols = rM.Cols();
		int iMPos;
		TCVScalar *pData = rM.Data();

		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rList = *rVar.GetVarListPtr();

		rList.Set(iRows);
		
		for( iR = 0, iMPos = 0; iR < iRows; iR++ )
		{
			rList[iR].New(PDT_VARLIST, "Constant");
			TVarList &rRow = *rList[iR].GetVarListPtr();
			rRow.Set(iCols);

			for( iC = 0; iC < iCols; iC++, iMPos++ )
			{
				rRow[iC] = pData[iMPos];
			}
		}
	}
	else if (mVars(0).BaseType() == PDT_STRING)
	{
		if (iVarCount < 2)
		{
			m_ErrorList.GeneralError("Expect delimiter string as second parameter.", iLine, iPos);
			return false;
		}

		if (mVars(1).BaseType() != PDT_STRING)
		{
			m_ErrorList.GeneralError("Expect delimiter string as second parameter.", iLine, iPos);
			return false;
		}

		TString &rStr = *mVars(0).GetStringPtr();
		TString &rDel = *mVars(1).GetStringPtr();

		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rList = *rVar.GetVarListPtr();

		int iPos, iCount = rStr.Len();
		int iDelCount = rDel.Len();

		if (iDelCount == 0)
		{
			m_ErrorList.GeneralError("Delimiter string cannot be empty.", iLine, iPos);
			return false;
		}

		char *pcStr = rStr.Str();
		char *pcDel = rDel.Str();
		char *pcCur = pcStr;
		char *pcEnd;

		while( (pcEnd = strstr(pcCur, pcDel)) )
		{
			*pcEnd = 0;
			rList.Add(1);
			rList.Last() = pcCur;
			pcCur = &pcEnd[iDelCount];
		}

		// Copy rest of string which was not ended
		// by a delimiter.
		if (pcCur)
		{
			rList.Add(1);
			rList.Last() = pcCur;
		}

	}
	else
	{
		m_ErrorList.GeneralError("Casting of given element to list is not implemented.", iLine, iPos);
		return false;
	}

	return true;
}



//////////////////////////////////////////////////////////////////////
/// Transpose Elements in List
/// This can transform index lists into lists of numbers and vice versa.
/// Parameters:
///		1. The list
///		2. The sublevel at which transpose is to be executed.
///
// TODO Documentation for this function. Called 'TransList' in CLUScript.

bool CCLUCodeBase::TransposeListElementsFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	TCVCounter iRecurseDepth;
	
	if (iVarCount != 1 && iVarCount != 2)
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	if (iVarCount == 2)
	{
		if (!mVars(1).CastToCounter(iRecurseDepth))
		{
			m_ErrorList.GeneralError("Second parameter has to be counter if given.", iLine, iPos);
			return false;
		}
	}
	else
	{
		iRecurseDepth = 0;
	}

	if (mVars(0).BaseType() != PDT_VARLIST)
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	rVar.New(PDT_VARLIST, "Constant");
	TVarList &rTransList = *rVar.GetVarListPtr();
	TVarList &rList = *mVars(0).GetVarListPtr();
	
	return TransposeListElements(rTransList, rList, iRecurseDepth, iLine, iPos);
}

bool CCLUCodeBase::TransposeListElements(TVarList& rTransList, TVarList& rList, int iRecurseDepth, int iLine, int iPos)
{
	Mem<TVarList*> mSubList;
	int i, j, iListCount = rList.Count();
	int iCnt, iMaxCnt = 0;

	if (iListCount == 0)
		return true;

	if (iRecurseDepth > 0)
	{
		rTransList.Set(iListCount);
		for (i = 0; i < iListCount; i++)
		{
			rTransList(i).New(PDT_VARLIST, "Constant");
			TVarList &rSubTransList = *rTransList(i).GetVarListPtr();
			
			if (rList(i).BaseType() != PDT_VARLIST)
			{
				m_ErrorList.GeneralError("Transpose on given level not possible.", iLine, iPos);
				return false;
			}
			TVarList &rSubList = *rList(i).GetVarListPtr();

			if (!TransposeListElements(rSubTransList, rSubList, iRecurseDepth-1, iLine, iPos))
				return false;
		}

		return true;
	}

	// Find Maximum row length
	for (i = 0; i < iListCount; i++)
	{
		if (rList(i).BaseType() == PDT_VARLIST)
		{
			 iCnt = rList(i).GetVarListPtr()->Count();
			 if (iCnt > iMaxCnt)
				iMaxCnt = iCnt;
		}
		else if (1 > iMaxCnt)
			iMaxCnt = 1;
	}

	rTransList.Set(iMaxCnt);
	mSubList.Set(iMaxCnt);
	for (i = 0; i < iMaxCnt; i++)
	{
		rTransList(i).New(PDT_VARLIST, "Constant");
		mSubList[i] = rTransList(i).GetVarListPtr();
	}


	for (i = 0; i < iListCount; i++)
	{
		if (rList(i).BaseType() == PDT_VARLIST)
		{
			TVarList &rSubList = *rList(i).GetVarListPtr();

			iCnt = rSubList.Count();
			for (j = 0; j < iCnt; j++)
			{
				mSubList[j]->Add(1);
				mSubList[j]->Last() = rSubList(j);
			}
		}
		else
		{
			mSubList[0]->Add(1);
			mSubList[0]->Last() = rList(i);
		}
	}

	return true;
}




//////////////////////////////////////////////////////////////////////
/// Extract Elements from List
// TODO Documentation for this function. Called 'SubList' in CLUScript.

bool CCLUCodeBase::ExtractListElementsFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	TCVCounter iFirst, iCount;

	if (iVarCount == 2)
	{
		if (mVars(0).BaseType() != PDT_VARLIST)
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (mVars(1).BaseType() != PDT_VARLIST)
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rSubList = *rVar.GetVarListPtr();

		TVarList &rList = *mVars(0).GetVarListPtr();
		TVarList &rIdxList = *mVars(1).GetVarListPtr();

		TCVCounter iIdx;
		int i, iCount = rIdxList.Count();
		int iListCount = rList.Count();
		rSubList.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (rIdxList(i).BaseType() == PDT_VARLIST)
			{
				if (!GetVarListElement(rSubList[i], rList, *rIdxList(i).GetVarListPtr(), 0, iLine, iPos))
					return false;

				// GetVarListElement returns pointer to element. Need to copy this
				// to a new element, since pointer will not be valid after initial
				// list does not exist anymore.
				rSubList[i].DereferencePtr();
			}
			else if (rIdxList(i).CastToCounter(iIdx))
			{
				if (iIdx <= 0 || iIdx > iListCount)
				{
					cStr csText;
					csText << "Index out of range at position " << (i+1) << " in index list.";
					m_ErrorList.GeneralError(csText.Str(), iLine, iPos);
					return false;
				}

				rSubList[i] = rList(iIdx-1);
			}
			else
			{
				cStr csText;
				csText << "Invalid index at position " << (i+1) << " in index list.";
				m_ErrorList.GeneralError(csText.Str(), iLine, iPos);
				return false;
			}
		}
	}
	else if (iVarCount == 3)
	{
		if (mVars(0).BaseType() != PDT_VARLIST)
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (!mVars(1).CastToCounter(iFirst))
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		if (!mVars(2).CastToCounter(iCount))
		{
			m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}


		// List containing free variable, plot range and step.
		rVar = *mVars(0).GetVarListPtr();
		TVarList &List = *rVar.GetVarListPtr();

		int iLCnt = (int) List.Count();

		if (iFirst < 1 || iFirst > iLCnt)
		{
			m_ErrorList.InvalidParVal(mVars(0), 1, iLine, iPos);
			return false;
		}
		iFirst--;

		if (iCount == 0)
		{
			rVar.New(PDT_VARLIST, "Constant");
			return true;
		}

		if (iCount < 0)
		{
			iCount = iLCnt - iFirst;
		}
		else if (iFirst + iCount > iLCnt)
		{
			m_ErrorList.InvalidParVal(mVars(1), 2, iLine, iPos);
			return false;
		}

		if (iFirst > 0)
		{
			List.Del((unsigned) 0, (unsigned) iFirst);
		}

		iFirst += iCount;
		if (iFirst < iLCnt)
		{
			List.Del((unsigned) iFirst, (unsigned) (iLCnt - iFirst));
		}
	}
	else
	{
		m_ErrorList.WrongNoOfParams(3, iLine, iPos);
		return false;
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Remove Elements from List
// TODO Documentation for this function. Called 'RemList' in CLUScript.

bool CCLUCodeBase::RemoveListElementsFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	TCVCounter iFirst, iCount;

	if (iVarCount == 2)
	{
		TCVCounter iIdx;

		if (mVars(0).BaseType() != PDT_VARLIST)
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (mVars(1).CastToCounter(iIdx))
		{
			TVarList &rList = *mVars(0).GetVarListPtr();
			int iListCount = rList.Count();

			if (iIdx <= 0 || iIdx > iListCount)
			{
				m_ErrorList.GeneralError("Index out of range.", iLine, iPos);
				return false;
			}

			rList.Del(iIdx-1);
		}
		else if (mVars(1).BaseType() == PDT_VARLIST)
		{
			TVarList &rList = *mVars(0).GetVarListPtr();
			TVarList &rIdxList = *mVars(1).GetVarListPtr();

			TCVCounter iIdx;
			int i, iCount = rIdxList.Count();
			int iListCount = rList.Count();
			vector<int> vecIdx;
			vecIdx.resize(iCount);

			for (i = 0; i < iCount; i++)
			{
				if (!rIdxList(i).CastToCounter(iIdx))
				{
					cStr csText;
					csText << "Invalid index at position " << (i+1) << " in index list.";
					m_ErrorList.GeneralError(csText.Str(), iLine, iPos);
					return false;
				}

				vecIdx[i] = iIdx;
			}

			// Sort indices in descending order
			sort( vecIdx.begin( ), vecIdx.end( ), greater<int>( ) );

			for (i = 0; i < iCount; i++)
			{
				iListCount = rList.Count();
				iIdx = vecIdx[i];

				if (iIdx <= 0 || iIdx > iListCount)
				{
					cStr csText;
					csText << "Index out of range at position " << (i+1) << " in index list.";
					m_ErrorList.GeneralError(csText.Str(), iLine, iPos);
					return false;
				}

				rList.Del(iIdx-1);

				while(i+1 < iCount && iIdx == vecIdx[i+1])
					i++;

				/*
				if (rIdxList(i).BaseType() == PDT_VARLIST)
				{
					if (!RemoveVarListElement(rList, *rIdxList(i).GetVarListPtr(), 0, iLine, iPos))
						return false;
				}
				else 
				if (rIdxList(i).CastToCounter(iIdx))
				{
					if (iIdx <= 0 || iIdx > iListCount)
					{
						cStr csText;
						csText << "Index out of range at position " << (i+1) << " in index list.";
						m_ErrorList.GeneralError(csText.Str(), iLine, iPos);
						return false;
					}

					rList.Del(iIdx-1);
				}
				else
				{
					cStr csText;
					csText << "Invalid index at position " << (i+1) << " in index list.";
					m_ErrorList.GeneralError(csText.Str(), iLine, iPos);
					return false;
				}
				*/
			}
		}
		else
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}
	}
	else if (iVarCount == 3)
	{
		if (mVars(0).BaseType() != PDT_VARLIST)
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (!mVars(1).CastToCounter(iFirst))
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		if (!mVars(2).CastToCounter(iCount))
		{
			m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}

		TVarList& List = *mVars(0).GetVarListPtr();

		int iLCnt = (int) List.Count();

		if (iFirst < 1 || iFirst > iLCnt)
		{
			m_ErrorList.InvalidParVal(mVars(0), 1, iLine, iPos);
			return false;
		}
		iFirst--;

		if (iCount == 0)
		{
			rVar.New(PDT_VARLIST, "Constant");
			return true;
		}

		if (iCount < 0)
		{
			iCount = iLCnt - iFirst;
		}
		else if (iFirst + iCount > iLCnt)
		{
			m_ErrorList.InvalidParVal(mVars(1), 2, iLine, iPos);
			return false;
		}

		List.Del((unsigned) iFirst, (unsigned) iCount);

		//rVar = List;
	}
	else
	{
		m_ErrorList.WrongNoOfParams(3, iLine, iPos);
		return false;
	}

	return true;
}



//////////////////////////////////////////////////////////////////////
/// Insert Elements into List
///
/// Parameters:
///		1. List where to insert elements
///		2. Position before which to insert
///		3. Either:
///			a) Number of zero elements to insert.
///			b) List of elements to insert.
///
///	Return:
///		Void
//////////////////////////////////////////////////////////////////////

bool CCLUCodeBase::InsertListElementsFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	TCVCounter iInsPos, iInsCount;

	if (iVarCount != 3)
	{
		m_ErrorList.WrongNoOfParams(3, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_VARLIST)
	{
		m_ErrorList.GeneralError("First parameter has to be a variable list.", iLine, iPos);
		return false;
	}

	TVarList &rList = *mVars(0).GetVarListPtr();
	TCVCounter iListCount = rList.Count();

	if (!mVars(1).CastToCounter(iInsPos))
	{
		m_ErrorList.GeneralError("Second parameter has to be the position before which to insert.", iLine, iPos);
		return false;
	}

	if (iInsPos < 1 || iInsPos > iListCount+1)
	{
		m_ErrorList.GeneralError("Invalid insert position.", iLine, iPos);
		return false;
	}

	if (mVars(2).CastToCounter(iInsCount))
	{
		// Insert zero elements
		
		if (iInsCount < 0)
		{
			m_ErrorList.GeneralError("Invalid number of elements to insert.", iLine, iPos);
			return false;
		}
		
		if (iInsCount == 0)
			return true;

		--iInsPos;
		rList.Insert((uint) iInsPos, (uint) iInsCount);

		for (int iPos = 0; iPos < iInsCount; iPos++)
		{
			rList[iInsPos + iPos] = TCVCounter(0);
		}
	}
	else if (mVars(2).BaseType() == PDT_VARLIST)
	{
		// Insert list elements

		TVarList &rInsList = *mVars(2).GetVarListPtr();

		if (&rList == &rInsList)
		{
			m_ErrorList.GeneralError("Cannot insert list into itself.", iLine, iPos);
			return false;
		}

		iInsCount = rInsList.Count();
		if (iInsCount == 0)
			return true;

		--iInsPos;
		rList.Insert((uint) iInsPos, (uint) iInsCount);

		for (int iPos = 0; iPos < iInsCount; iPos++)
		{
			rList[iInsPos + iPos] = rInsList[iPos];
		}
	}
	else
	{
		m_ErrorList.GeneralError("Third parameter has to be either a number or a list.", iLine, iPos);
		return false;
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Index list of combinations
// Expects number of elements to choose from and number of elements to combine
// Returns list of indices giving combinations.

bool CCLUCodeBase::CombinationIndexListFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	TCVCounter iChoose, iCount;

	if (iVarCount == 2)
	{
		if (!mVars(0).CastToCounter(iCount))
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (!mVars(1).CastToCounter(iChoose))
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		if (iCount <= 0 || iCount > 50)
		{
			m_ErrorList.GeneralError("Number of elements out of range.", iLine, iPos);
			return false;
		}

		if (iChoose <= 0 || iChoose > iCount)
		{
			m_ErrorList.GeneralError("Number of elements to choose out of range.", iLine, iPos);
			return false;
		}

		int *piIdxList = new int[iChoose];
		if (!piIdxList)
		{
			m_ErrorList.OutOfMemory(iLine, iPos);
			return false;
		}

		MemObj<Mem<int> > mCombIdx;
		if (!GenerateCombinationIndices(mCombIdx, iCount, iChoose, 1, piIdxList, 0))
		{
			m_ErrorList.GeneralError("Error generating combination indices.", iLine, iPos);
			delete[] piIdxList;
			return false;
		}

		delete[] piIdxList;

		int i, j, iCombCount = mCombIdx.Count();
		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rCList = *rVar.GetVarListPtr();
		rCList.Set(iCombCount);

		for (i = 0; i < iCombCount; i++)
		{
			rCList[i].New(PDT_VARLIST, "Constant");
			TVarList &rIdxList = *(rCList[i].GetVarListPtr());
			rIdxList.Set(iChoose);
			
			Mem<int>& rmIdx = mCombIdx[i];

			for (j = 0; j < iChoose; j++)
			{
				rIdxList[j].New(PDT_VARLIST, "Constant");
				TVarList &rTheIdx = *rIdxList[j].GetVarListPtr();
				rTheIdx.Set(1);

				rTheIdx[0] = TCVCounter(rmIdx[j]);
			}
		}
	}
	else
	{
		m_ErrorList.WrongNoOfParams(2, iLine, iPos);
		return false;
	}

	return true;
}



//////////////////////////////////////////////////////////////////////
/// Index list of permutations
// Expects number of elements to choose from and number of elements to permute
// Returns list of indices giving combinations.

bool CCLUCodeBase::PermutationIndexListFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	TCVCounter iChoose, iCount;

	if (iVarCount == 2)
	{
		if (!mVars(0).CastToCounter(iCount))
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (!mVars(1).CastToCounter(iChoose))
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		if (iCount <= 0 || iCount > 50)
		{
			m_ErrorList.GeneralError("Number of elements out of range.", iLine, iPos);
			return false;
		}

		if (iChoose <= 0 || iChoose > iCount)
		{
			m_ErrorList.GeneralError("Number of elements to choose out of range.", iLine, iPos);
			return false;
		}

		int *piIdxList = new int[iChoose];
		if (!piIdxList)
		{
			m_ErrorList.OutOfMemory(iLine, iPos);
			return false;
		}

		// First generate combinations. Then permute those combinations.
		MemObj<Mem<int> > mCombIdx;
		if (!GenerateCombinationIndices(mCombIdx, iCount, iChoose, 1, piIdxList, 0))
		{
			m_ErrorList.GeneralError("Error generating combination indices.", iLine, iPos);
			delete[] piIdxList;
			return false;
		}

		delete[] piIdxList;

		int i, j, iCombCount = mCombIdx.Count();
		MemObj<Mem<int> > mPermIdx, mPermIdxList;
		Mem<int> mCurPermIdx;

		// Now permute all combinations
		for (i = 0; i < iCombCount; i++)
		{
			mPermIdxList.Set(0);

			if (!GeneratePermutationIndices(mPermIdxList, mCurPermIdx, mCombIdx[i], iChoose))
			{
				m_ErrorList.GeneralError("Error generating permutations.", iLine, iPos);
				return false;
			}

			int iPermCount = mPermIdxList.Count();
			int iPermOffset = mPermIdx.Count();
			mPermIdx.Add(iPermCount);
			for (j = 0; j < iPermCount; j++)
			{
				mPermIdx[iPermOffset+j] = mPermIdxList[j];
			}
		}

		int iPermCount = mPermIdx.Count();

		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rCList = *rVar.GetVarListPtr();
		rCList.Set(iPermCount);

		for (i = 0; i < iPermCount; i++)
		{
			rCList[i].New(PDT_VARLIST, "Constant");
			TVarList &rIdxList = *(rCList[i].GetVarListPtr());
			rIdxList.Set(iChoose);
			
			Mem<int>& rmIdx = mPermIdx[i];

			for (j = 0; j < iChoose; j++)
			{
				rIdxList[j].New(PDT_VARLIST, "Constant");
				TVarList &rTheIdx = *rIdxList[j].GetVarListPtr();
				rTheIdx.Set(1);

				rTheIdx[0] = TCVCounter(rmIdx[j]);
			}
		}
	}
	else
	{
		m_ErrorList.WrongNoOfParams(2, iLine, iPos);
		return false;
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////////
// Generates all combinations of indices using recursion.

bool CCLUCodeBase::GenerateCombinationIndices(MemObj<Mem<int> >& mCombIdx, 
						int iCount, int iChoose, int iFirstIdx, 
						int *piIdxList, int iIdxPos)
{
	int iIdx;

	// Assume that indices start counting with 1
	if (iChoose < 0 || iChoose > iCount || iFirstIdx < 1)
		return false;

	// Scalar blade
	if (iChoose == 0)
	{
		// Save Index List
		mCombIdx.Add(1);
		Mem<int>& mIdxList = mCombIdx.Last();
		if (iIdxPos > 0)
		{
			mIdxList.Set(iIdxPos);
			for(int i = 0; i < iIdxPos; i++)
				mIdxList[i] = piIdxList[i];
		}
		else
		{
			mIdxList.Set(0);
		}

		return true;
	}


	// Blade of higher grade
	for (iIdx = iFirstIdx; iIdx <= iCount; iIdx++)
	{
		piIdxList[iIdxPos] = iIdx;
		if (!GenerateCombinationIndices(mCombIdx, iCount, 
						iChoose-1, iIdx+1, piIdxList, iIdxPos+1))
			return false;
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////////
// Generates all permutations of indices using recursion.

bool CCLUCodeBase::GeneratePermutationIndices(MemObj<Mem<int> >& mPermIdx, 
						Mem<int>& mCurPermIdx, Mem<int>& mIdxList, int iRest)
{

	if (iRest < 0)
		return false;

	if (iRest == 0)
	{
		mPermIdx.Add(1);
		mPermIdx.Last() = mCurPermIdx;
		return true;
	}

	int iIdx, iIdxCount = mIdxList.Count();
	Mem<int> mSubPermIdx;
	Mem<int> mSubIdxList;

	for (iIdx = 0; iIdx < iIdxCount; iIdx++)
	{
		mSubPermIdx = mCurPermIdx;
		mSubIdxList = mIdxList;
		mSubPermIdx.Add(1);
		mSubPermIdx.Last() = mIdxList[iIdx];
		mSubIdxList.Del(iIdx);

		if (!GeneratePermutationIndices(mPermIdx, mSubPermIdx, mSubIdxList, iRest-1))
			return false;
	}

	return true;
}


