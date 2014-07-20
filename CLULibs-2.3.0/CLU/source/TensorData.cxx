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


// Library for TensorData

#include<math.h>
#include<stdio.h>
#include<string.h>

#include "TensorData.h"


////////////////////////////////////////////////////////////////////////////////////
/// Default Constructor 

template<class CType>
CTensorData<CType>::CTensorData()
{
	Mem<int> mDim;

	mDim.Add(1);
	mDim.Last() = 1;

	Reset(mDim);
}



////////////////////////////////////////////////////////////////////////////////////
/// Constructor

template<class CType>
CTensorData<CType>::CTensorData(const Mem<int>& mDim)
	throw (CCLUException)
{
	Reset(mDim);
}


////////////////////////////////////////////////////////////////////////////////////
/// Copy Constructor

template<class CType>
CTensorData<CType>::CTensorData(const CTensorData<CType>& rT)
{
	*this = rT;
}


////////////////////////////////////////////////////////////////////////////////////
/// Destructor

template<class CType>
CTensorData<CType>::~CTensorData()
{
	int i, iCount = m_mTensorIdx.Count();

	// Invalidate all Tensor Index instances that are registered
	for (i = 0; i < iCount; i++)
	{
		m_mTensorIdx[i]->Invalidate();
	}
}


////////////////////////////////////////////////////////////////////////////////////
/// Assign Operator

template<class CType>
CTensorData<CType>& CTensorData<CType>::operator= (const CTensorData<CType>& rT)
{
	m_iValence = rT.m_iValence;
	m_mDim = rT.m_mDim;
	m_mIdxStep = rT.m_mIdxStep;
	m_iSize = rT.m_iSize;
	m_mData = rT.m_mData;

	// Unregister all tensor index instances
	int i, iCount = m_mTensorIdx.Count();

	// Invalidate all Tensor Index instances that are registered
	for (i = 0; i < iCount; i++)
	{
		m_mTensorIdx[i]->Invalidate();
	}

	return *this;
}


////////////////////////////////////////////////////////////////////////////////////
/// Reset Tensor

template<class CType>
void CTensorData<CType>::Reset(const Mem<int>& mDim)
	throw (CCLUException)
{
	int iValence = mDim.Count();

	if (iValence <= 0)
		throw InvalidValence();

	if (mDim.Count() != iValence)
		throw InvalidDim();

	int iDim;
	m_iSize = 1;
	m_iValence = iValence;
	m_mDim.Set(iValence);
	m_mIdxStep.Set(iValence);

	for(iDim = m_iValence-1; iDim >= 0; iDim--)
	{	
		if (mDim[iDim] <= 0)
			throw InvalidDim();

		m_mDim[iDim] = mDim[iDim];
		m_mIdxStep[iDim] = m_iSize;
		m_iSize *= mDim[iDim];
	}

	if (!m_mData.Set(m_iSize))
		throw CCLUOutOfMemory();

	memset(m_mData.Data(), 0, m_iSize*sizeof(CType));
}




////////////////////////////////////////////////////////////////////////////////////
/// Changes size of a dimension
/*
template<class CType>
void CTensorData<CType>::ResizeDim(int iDim, int iSize)
{
	if (iDim < 0 || iDim >= m_mDim.Count())
		throw InvalidDim();

	if (iSize <= 0)
		throw CCLUError("Invalid size.");

	int i;

	Mem<uint> ndims(dims);
	ndims[d] = no;         // Set size of new dimension

	Tensor<CType> T(rank, ndims.Data(), fidx);  // Create Resized Tensor
	if (!T.isOK()) 
	{ delete[] cidx;
	return 0;
	}

	if (no > dims[d])        // Restrict range if new dim bigger than old
		T.IdxRange(d+1, fidx, dims[d] + fidx - 1);

	this->ResetIdxRange();   // Free index range of old tensor

	T.actidx = 0;
	this->actidx = 0;

	for(i=0;i<rank;i++)     // Set counting indices
	{ T,&cidx[i];
	this[0],&cidx[i];
	}

	T = this[0];
	T.ResetIdxRange();
	this[0] << T;

	delete[] cidx;
	return 1;
}
*/


////////////////////////////////////////////////////////////////////////////////////
/// Register a TensorIdx instance

template<class CType>
void CTensorData<CType>::RegisterTensorIdx(CTensorIdx<CType>& rTIdx)
{
	m_mTensorIdx.Add(1);
	m_mTensorIdx.Last() = &rTIdx;
}


////////////////////////////////////////////////////////////////////////////////////
/// Unregister a tensor index

template<class CType>
void CTensorData<CType>::UnregisterTensorIdx(CTensorIdx<CType>& rTIdx)
{
	int i, iCount = m_mTensorIdx.Count();

	for (i = 0; i < iCount; i++)
	{
		if (m_mTensorIdx[i] == &rTIdx)
		{
			m_mTensorIdx.Del(i);
			rTIdx.Invalidate();
			break;
		}
	}
}


