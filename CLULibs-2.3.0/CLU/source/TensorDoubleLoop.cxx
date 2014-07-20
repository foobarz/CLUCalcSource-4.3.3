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

#include "TensorDoubleLoop.h"

////////////////////////////////////////////////////////////////////////////////////
/// Single Loop Class Member Functions
////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////
/// Constructor

template <class CType>
CTensorDoubleLoop<CType>::CTensorDoubleLoop()
{
	m_pTLeft = 0;
	m_pTRight = 0;
}


////////////////////////////////////////////////////////////////////////////////////
/// Set Tensor Idx Instance

template <class CType>
void CTensorDoubleLoop<CType>::Set(CTensorIdx<CType>& rTLeft, CTensorIdx<CType>& rTRight) 
	throw (CCLUException)
{
	if (!rTLeft.IsValid() || !rTRight.IsValid())
	{
		m_pTLeft = 0;
		m_pTRight = 0;
		m_mFreeIdx.Set(0);
		throw InvalidTensorIdxRef();
	}

	try
	{
		m_pTLeft = &rTLeft;
		m_pTRight = &rTRight;
		m_mFreeIdx.Set(0);

		const typename CTensorIdx<CType>::TFreeIdxMap& mapFILeft = rTLeft.GetFreeIdxMap();
		const typename CTensorIdx<CType>::TFreeIdxMap& mapFIRight = rTRight.GetFreeIdxMap();
		typename CTensorIdx<CType>::TFreeIdxMap::const_iterator it_LEl, it_LEnd;
		typename CTensorIdx<CType>::TFreeIdxMap::const_iterator it_REl, it_REnd;

		it_LEnd = mapFILeft.end();
		it_REnd = mapFIRight.end();

		// Store free indices in the order they appear in the two maps
		// Order of indices is joined, such that m_mFreeIdx contains indices
		// in ascending order.
		// If there are equal indices they are combined.


		// Add all left free indices to free index list
		for (it_LEl = mapFILeft.begin();
			it_LEl != it_LEnd;
			++it_LEl)
		{
			m_mFreeIdx.Add(1);
			SIdxData &rIdx = m_mFreeIdx.Last();

			rIdx.bBoth = false;
			rIdx.iFreeIdx = it_LEl->first;
			rIdx.iCount = it_LEl->second.iCount;
			rIdx.pTFirst = m_pTLeft;
			rIdx.pTSecond = 0;
		}

		int i = 0, iFICount;

		// Now add the right indices at the appropriate places
		for (it_REl = mapFIRight.begin();
			it_REl != it_REnd;
			++it_REl)
		{
			iFICount = m_mFreeIdx.Count();
			for (; i < iFICount; i++)
			{
				if (it_REl->first < m_mFreeIdx[i].iFreeIdx)
				{
					// Insert free index
					m_mFreeIdx.Insert(i);
					SIdxData &rIdx = m_mFreeIdx[i];

					rIdx.bBoth = false;
					rIdx.iFreeIdx = it_REl->first;
					rIdx.iCount = it_REl->second.iCount;
					rIdx.pTFirst = m_pTRight;
					rIdx.pTSecond = 0;
					break;
				}
				else if (it_REl->first == m_mFreeIdx[i].iFreeIdx)
				{
					// Combine free indices
					SIdxData &rIdx = m_mFreeIdx[i];
					int iCount = it_REl->second.iCount;

					if (rIdx.iCount != iCount)
						throw IncompatibleIndices();

					rIdx.bBoth = true;
					rIdx.pTSecond = m_pTRight;
					break;
				}
				else if (i == iFICount-1)
				{
					// Add Index to end
					m_mFreeIdx.Add(1);
					SIdxData &rIdx = m_mFreeIdx.Last();

					rIdx.bBoth = false;
					rIdx.iFreeIdx = it_REl->first;
					rIdx.iCount = it_REl->second.iCount;
					rIdx.pTFirst = m_pTRight;
					rIdx.pTSecond = 0;
					break;
				}
			} // for i
		} // for it_REl
	} // try
	catch(CCLUException &rEx)
	{
		m_pTLeft = 0;
		m_pTRight = 0;
		m_mFreeIdx.Set(0);

		throw rEx;
	}
}


////////////////////////////////////////////////////////////////////////////////////
// Get number of steps at particular loop level.

template <class CType>
int CTensorDoubleLoop<CType>::StepCount(int iLevel)
	throw (CCLUException)
{
	if (iLevel < 0 || iLevel >= int(m_mFreeIdx.Count()))
		throw InvalidLoopLevel();

	if (!m_pTLeft || !m_pTRight)
		throw InvalidTensorIdxRef();

	return m_mFreeIdx[iLevel].iCount;
}


////////////////////////////////////////////////////////////////////////////////////
// Returns true if both loops are stepped at given loop level

template <class CType>
bool CTensorDoubleLoop<CType>::IsDoubleLoop(int iLevel) 
	throw (CCLUException)
{
	if (iLevel < 0 || iLevel >= int(m_mFreeIdx.Count()))
		throw InvalidLoopLevel();

	if (!m_pTLeft || !m_pTRight)
		throw InvalidTensorIdxRef();

	return m_mFreeIdx[iLevel].bBoth;
}


////////////////////////////////////////////////////////////////////////////////////
// Returns free index for given loop level

template <class CType>
int CTensorDoubleLoop<CType>::FreeIdx(int iLevel) 
	throw (CCLUException)
{
	if (iLevel < 0 || iLevel >= int(m_mFreeIdx.Count()))
		throw InvalidLoopLevel();

	if (!m_pTLeft || !m_pTRight)
		throw InvalidTensorIdxRef();

	return m_mFreeIdx[iLevel].iFreeIdx;
}




////////////////////////////////////////////////////////////////////////////////////
// Initialize Loops. Returns number of cascaded loops.

template <class CType>
int CTensorDoubleLoop<CType>::Init() 
	throw (CCLUException)
{
	if (!m_pTLeft || !m_pTRight)
		throw InvalidTensorIdxRef();
	
	m_pTLeft->InitAllLoops();
	m_pTRight->InitAllLoops();
	
	return m_mFreeIdx.Count();
}



////////////////////////////////////////////////////////////////////////////////////
// Step loop. Returns loop level, in which step was made.
// Loop level starts at zero for most inner loop.
// If loop level is -1, all loops are finished.
// No exceptions generated by this function!

template <class CType>
int CTensorDoubleLoop<CType>::Step() 
{
	int iLevel = 0;

	if (m_mFreeIdx.Count() == 0)
		return -1;

	while (true)
	{
		SIdxData &rIdx = m_mFreeIdx[iLevel];
		if (rIdx.bBoth)
		{
			rIdx.pTFirst->StepLoop(rIdx.iFreeIdx);
			if (rIdx.pTSecond->StepLoop(rIdx.iFreeIdx))
				break;

			rIdx.pTFirst->InitLoop(rIdx.iFreeIdx);
			rIdx.pTSecond->InitLoop(rIdx.iFreeIdx);
		}
		else
		{
			if (rIdx.pTFirst->StepLoop(rIdx.iFreeIdx))
				break;

			rIdx.pTFirst->InitLoop(rIdx.iFreeIdx);
		}

		if (iLevel == m_mFreeIdx.Count() - 1)
			return -1;	// All loops finished

		++iLevel;
	}

	return iLevel;
}

