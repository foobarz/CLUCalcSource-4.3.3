/// LICENSE START
////////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the CLU Library.
//
// 
// Copyright (C) 1997-2004  Christian B.U. Perwass
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


// Library for use of tensors

#include<math.h>
#include<stdio.h>
#include<string.h>

#include "TensorIdx.h"

////////////////////////////////////////////////////////////////////////////////////
/// Default Constructor

template <class CType>
CTensorIdx<CType>::CTensorIdx()
{
	Reset();
}


////////////////////////////////////////////////////////////////////////////////////
// Constructor

template <class CType>
CTensorIdx<CType>::CTensorIdx(CTensorData<CType>& rT, const Mem<int>& mIdx) 
	throw (CCLUException)
{
	Reset();

	Set(rT, mIdx);
}

////////////////////////////////////////////////////////////////////////////////////
/// Copy Constructor

template <class CType>
CTensorIdx<CType>::CTensorIdx(const CTensorIdx<CType>& rTIdx)
{
	*this = rTIdx;
}


////////////////////////////////////////////////////////////////////////////////////
/// Destructor

template <class CType>
CTensorIdx<CType>::~CTensorIdx()
{
	if (m_pTensor)
		m_pTensor->UnregisterTensorIdx(*this);
}


////////////////////////////////////////////////////////////////////////////////////
/// Assign Operator

template <class CType>
CTensorIdx<CType>& CTensorIdx<CType>::operator= (const CTensorIdx<CType>& rTIdx)
{
	if (!rTIdx.IsValid())
	{
		if (m_pTensor)
			m_pTensor->UnregisterTensorIdx(*this);
	}
	else
	{
		Set(*rTIdx.m_pTensor, rTIdx.m_mIdx);
		m_mapFreeIdx = rTIdx.m_mapFreeIdx;
		UpdateStepPos();
	}

	return *this;
}


////////////////////////////////////////////////////////////////////////////////////
/// Reset

template <class CType>
void CTensorIdx<CType>::Reset()
{
	Invalidate();

	m_iFixedPos = 0;
	m_iStepPos = 0;

	m_mapFreeIdx.clear();
	m_mIdx.Set(0);
}

////////////////////////////////////////////////////////////////////////////////////
/// Invalidate Tensor Index, since TensorData has been deleted.

template <class CType>
void CTensorIdx<CType>::Invalidate()
{
	m_bIsValid = false;
	m_pTensor = 0;
	m_mIdx.Set(0);
}


////////////////////////////////////////////////////////////////////////////////////
// Check whether index is constant

template <class CType>
bool CTensorIdx<CType>::IsConstant() const
	throw (CCLUException)
{
	if (!m_bIsValid)
		throw InvalidTensorRef();

	if (m_mapFreeIdx.size() == 0)
		return true;

	return false;
}


////////////////////////////////////////////////////////////////////////////////////
/// Set TensorData and Index

template <class CType>
void CTensorIdx<CType>::Set(CTensorData<CType>& rT, const Mem<int>& mIdx) 
	throw (CCLUException)
{
	int i, iCount = rT.Valence();

	if (m_pTensor)
		m_pTensor->UnregisterTensorIdx(*this);

	rT.RegisterTensorIdx(*this);
	m_pTensor = &rT;

	try
	{
		// Clear list of free indices
		m_mapFreeIdx.clear();

		// Set size of tensor index list
		m_mIdx.Set(iCount);

		// Reset element position due to fixed indices
		m_iFixedPos = 0;

		// Loop through indices
		for (i = 0; i < iCount; i++)
		{
			m_mIdx[i] = mIdx[i];
			if (mIdx[i] >= rT.DimSize(i))	// Out of range
			{
				throw InvalidIdx();
			}
			else if (mIdx[i] < 0)	// Free index
			{
				SetFreeIdx(mIdx[i], i);		
			}
			else	// Fixed index
			{
				m_iFixedPos += mIdx[i] * rT.GetIdxSteps()[i];
			}
		}
	}
	catch (CCLUException &rEx)
	{
		m_pTensor->UnregisterTensorIdx(*this);
		throw rEx;
	}

	m_bIsValid = true;
}


////////////////////////////////////////////////////////////////////////////////////
/// Set TensorData with given Tensor Index

template <class CType>
void CTensorIdx<CType>::Set(CTensorData<CType>& rT, const CTensorIdx<CType>& rTIdx) 
	throw (CCLUException)
{
	int i, iCount = rT.Valence();

	if (m_pTensor)
		m_pTensor->UnregisterTensorIdx(*this);

	rT.RegisterTensorIdx(*this);
	m_pTensor = &rT;

	const Mem<int>& mIdx = rTIdx.GetIdxList();

	try
	{
		// Clear list of free indices
		m_mapFreeIdx.clear();

		// Set size of tensor index list
		m_mIdx.Set(iCount);

		// Reset element position due to fixed indices
		m_iFixedPos = 0;

		// Loop through indices
		for (i = 0; i < iCount; i++)
		{
			m_mIdx[i] = mIdx[i];
			if (mIdx[i] >= rT.DimSize(i))	// Out of range
			{
				throw InvalidIdx();
			}
			else if (mIdx[i] < 0)	// Free index
			{
				SetFreeIdx(mIdx[i], i);		
			}
			else	// Fixed index
			{
				m_iFixedPos += mIdx[i] * rT.GetIdxSteps()[i];
			}
		}

		m_bIsValid = true;

		const TFreeIdxMap &rmapFreeIdx = rTIdx.GetFreeIdxMap();
		typename TFreeIdxMap::const_iterator it_El, it_End;

		it_End = rmapFreeIdx.end();
		for (it_El = rmapFreeIdx.begin();
			it_El != it_End;
			++it_El)
		{
			const SFreeIdxData &rIdx = it_El->second;
			SetIdxRange(it_El->first, rIdx.iMin, rIdx.iMax);
			SetIdxList(it_El->first, rIdx.mIdx);
		}
	}
	catch (CCLUException &rEx)
	{
		m_bIsValid = false;

		m_pTensor->UnregisterTensorIdx(*this);
		throw rEx;
	}
}


////////////////////////////////////////////////////////////////////////////////////
/// Set Free Index

template <class CType>
void CTensorIdx<CType>::SetFreeIdx(int iIdx, int iDim)
	throw (CCLUException)
{
	if (!m_pTensor)
		throw InvalidTensorRef();

	if (iIdx >= 0)
		throw InvalidIdx();

	// Check whether index already exists in map
	if ( m_mapFreeIdx.find(iIdx) == m_mapFreeIdx.end() )
	{
		// Index does not yet exist

		// Create index
		SFreeIdxData &rIdx = m_mapFreeIdx[iIdx];

		// Initialize Free Index Range
		rIdx.iMin = 0;
		rIdx.iMax = m_pTensor->DimSize(iDim) - 1;
		rIdx.iCount = rIdx.iMax + 1;
		rIdx.bUseIdxList = false;

		// Initialize Index Step
		rIdx.iStep = m_pTensor->GetIdxSteps()[iDim];

		// Initialize current index position
		rIdx.iIdx = 0;
	}
	else
	{
		// Free index already exists, i.e. a number of dimensions have same free index.

		// Get index
		SFreeIdxData &rIdx = m_mapFreeIdx[iIdx];

		// Increase step size in tensor
		rIdx.iStep += m_pTensor->GetIdxSteps()[iDim];

		// Adjust index range to smallest range
		if (rIdx.iMax >= m_pTensor->DimSize(iDim))
			rIdx.iMax = m_pTensor->DimSize(iDim) - 1;

		rIdx.iCount = rIdx.iMax + 1;
	}
}


////////////////////////////////////////////////////////////////////////////////////
/// Set index range of free index

template <class CType>
void CTensorIdx<CType>::SetIdxRange(int iIdx, int iMin, int iMax)
	throw (CCLUException)
{
	// This function only sets the index range for free indices.
	// It also resets the current index position to iMin.
	int iDim, iValence, iMinDimSize;

	if (!m_bIsValid)
		throw InvalidTensorRef();

	// Only allow free indices
	if (iIdx >= 0)
		throw InvalidIdx();

	// Check whether index exists in map
	if ( m_mapFreeIdx.find(iIdx) == m_mapFreeIdx.end() )
		throw InvalidIdx();

	// Get index data
	SFreeIdxData &rIdx = m_mapFreeIdx[iIdx];

	// If an index list has already been given, then ignore range.
	if (rIdx.mIdx.Count())
		return;

	// Get minimal max value of index range
	iMinDimSize = 0;
	iValence = m_pTensor->Valence();
	for (iDim = 0; iDim < iValence; iDim++)
	{
		if (m_mIdx[iDim] == iIdx)
		{
			if (iMinDimSize == 0 || iMinDimSize > m_pTensor->DimSize(iDim))
				iMinDimSize = m_pTensor->DimSize(iDim);
		}
	}

	// Check Index Range
	if (iMin < 0 || iMin >= iMinDimSize ||
		iMax < iMin || iMax >= iMinDimSize)
		throw InvalidIdxRange();

	// Set Index range
	rIdx.iMin = iMin;
	rIdx.iMax = iMax;

	// Set loop count
	rIdx.iCount = iMax - iMin + 1;

	// Reset Position
	rIdx.iIdx = iMin;
}


////////////////////////////////////////////////////////////////////////////////////
/// Set index list of free index

template <class CType>
void CTensorIdx<CType>::SetIdxList(int iIdx, const Mem<int>& mIdx)
	throw (CCLUException)
{
	// This function only sets the index range for free indices.
	// It also resets the current index position to iMin.
	int iDim, iValence, iMax, iCount, i;

	if (!m_bIsValid)
		throw InvalidTensorRef();

	// Only allow free indices
	if (iIdx >= 0)
		throw InvalidIdx();

	// Check whether index exists in map
	if ( m_mapFreeIdx.find(iIdx) == m_mapFreeIdx.end() )
		throw InvalidIdx();

	// Get index data
	SFreeIdxData &rIdx = m_mapFreeIdx[iIdx];

	iCount = mIdx.Count();
	if (!iCount)
	{
		// Reset index list
		rIdx.mIdx.Set(0);
		rIdx.bUseIdxList = false;
		return;
	}

	// Find largest index in list
	iMax = -1;
	for(i = 0; i < iCount; i++)
	{
		if (mIdx[i] > iMax)
			iMax = mIdx[i];

		if (mIdx[i] < 0)
			throw InvalidIdxList();
	}

	// Check whether maximum value in index list is larger than
	// smalles dimension.
	iValence = m_pTensor->Valence();
	for (iDim = 0; iDim < iValence; iDim++)
	{
		if (m_mIdx[iDim] == iIdx)
		{
			if (iMax >= m_pTensor->DimSize(iDim))
				throw InvalidIdxList();
		}
	}

	// Set Index list
	rIdx.mIdx = mIdx;

	// Set loop count
	rIdx.iCount = mIdx.Count();
	
	// Enable index list
	rIdx.bUseIdxList = true;

	// Reset Position
	// This is now index in index list
	rIdx.iIdx = 0;
}


////////////////////////////////////////////////////////////////////////////////////
/// Get reference to current data value

template <class CType>
CType& CTensorIdx<CType>::GetRef()
	throw (CCLUException)
{
	if (!m_bIsValid)
		throw InvalidTensorRef();

	int iPos = m_iFixedPos;
	typename TFreeIdxMap::iterator it_El, it_End;

	it_End = m_mapFreeIdx.end();
	for (it_El = m_mapFreeIdx.begin();
		 it_El != it_End;
		 ++it_El)
	{
		SFreeIdxData &rIdx = it_El->second;

		if (rIdx.bUseIdxList)
			iPos += rIdx.mIdx[rIdx.iIdx] * rIdx.iStep;
		else
			iPos += rIdx.iIdx * rIdx.iStep;
	}

	if (iPos >= m_pTensor->Size())
		throw Internal("GetRef");

	return m_pTensor->Data()[iPos];
}


////////////////////////////////////////////////////////////////////////////////////
// Update Step Position
template <class CType>
void CTensorIdx<CType>::UpdateStepPos()
{
	typename TFreeIdxMap::iterator it_El, it_End;

	it_End = m_mapFreeIdx.end();

	// Update step position
	m_iStepPos = m_iFixedPos;

	for (it_El = m_mapFreeIdx.begin();
		 it_El != it_End;
		 ++it_El)
	{
		SFreeIdxData &rIdx = it_El->second;

		if (rIdx.bUseIdxList)
			m_iStepPos += rIdx.mIdx[rIdx.iIdx] * rIdx.iStep;
		else
			m_iStepPos += rIdx.iIdx * rIdx.iStep;
	}
}


////////////////////////////////////////////////////////////////////////////////////
// Initialize all loops

template <class CType>
void CTensorIdx<CType>::InitAllLoops() 
	throw (CCLUException)
{
	if (!m_bIsValid)
		throw InvalidTensorRef();

	typename TFreeIdxMap::iterator it_El, it_End;

	it_End = m_mapFreeIdx.end();
	for (it_El = m_mapFreeIdx.begin();
		 it_El != it_End;
		 ++it_El)
	{
		SFreeIdxData &rIdx = it_El->second;

		if (rIdx.bUseIdxList)
			rIdx.iIdx = 0;
		else
			rIdx.iIdx = rIdx.iMin;
	}

	UpdateStepPos();
}


////////////////////////////////////////////////////////////////////////////////////
// Initialize particular loop

template <class CType>
void CTensorIdx<CType>::InitLoop(int iIdx)
	throw (CCLUException)
{
	if (iIdx >= 0)
		throw InvalidIdx();

	typename TFreeIdxMap::iterator it_El, it_End;
	it_End = m_mapFreeIdx.end();

	if ( (it_El = m_mapFreeIdx.find(iIdx)) == m_mapFreeIdx.end() )
	{
		throw InvalidIdx();
	}

	SFreeIdxData &rIdx = it_El->second;

	if (rIdx.bUseIdxList)
		rIdx.iIdx = 0;
	else
		rIdx.iIdx = rIdx.iMin;

	UpdateStepPos();
}


////////////////////////////////////////////////////////////////////////////////////
// Step Loop. No check of validity of index and the instance.
// Returns true if step was made and false if no step could be made,
// because loop already at last position.

template <class CType>
bool CTensorIdx<CType>::StepLoop(int iIdx)
{
	SFreeIdxData &rIdx = m_mapFreeIdx[iIdx];
		
	if (rIdx.bUseIdxList)
	{
		if (rIdx.iIdx+1 == rIdx.iCount)
			return false;	// Loop finished
	}
	else
	{
		if (rIdx.iIdx == rIdx.iMax)
			return false;	// Loop finished
	}

	rIdx.iIdx++;
	if (rIdx.bUseIdxList)
		m_iStepPos += (rIdx.mIdx[rIdx.iIdx] - rIdx.mIdx[rIdx.iIdx-1]) * rIdx.iStep;
	else
		m_iStepPos += rIdx.iStep;

	return true;
}


////////////////////////////////////////////////////////////////////////////////////
// Get number of steps in loop of given free index.

template <class CType>
int CTensorIdx<CType>::GetStepCount(int iIdx)
{
	if (!m_bIsValid)
		throw InvalidTensorRef();

	// Only allow free indices
	if (iIdx >= 0)
		throw InvalidIdx();

	// Check whether index exists in map
	if ( m_mapFreeIdx.find(iIdx) == m_mapFreeIdx.end() )
		throw InvalidIdx();

	// Get index data
	SFreeIdxData &rData = m_mapFreeIdx[iIdx];
	
	return rData.iCount;
}



