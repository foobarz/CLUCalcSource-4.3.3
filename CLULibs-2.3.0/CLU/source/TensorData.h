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

// TensorData Library declaration

#ifndef _TENSOR_DATA_HH_
#define _TENSOR_DATA_HH_

#include "mem.h"
#include "makestr.h"
#include "cstr.h"
#include "mathelp.h"
#include "CLUException.h"

template<class CType> class CTensorIdx;

template<class CType>
class CTensorData
{
public:
	class InvalidValence : public CCLUError
	{
	public:
		InvalidValence()
		{
			m_sErrorText = "Invalid valence value.";
		}
	};

	class InvalidDim : public CCLUError
	{
	public:
		InvalidDim()
		{
			m_sErrorText = "Invalid dimension.";
		}
	};

public:
	CTensorData();
	CTensorData(const CTensorData<CType>& rT);
	CTensorData(const Mem<int>& mDim) throw (CCLUException);
	~CTensorData();
	
	CTensorData<CType>& operator= (const CTensorData<CType>& rT);
	void Reset(const Mem<int>& mDim) throw (CCLUException); 
	//bool ResizeDim(int iDim, int iSize) throw (CCLUException);

	int Size() const { return m_iSize; }
	int DimSize(int iDim) const { return m_mDim[iDim]; }
	int Valence() const { return m_iValence; }

	Mem<int> GetIdxSteps() { return m_mIdxStep; }
	CType* Data() const { return m_mData.Data(); }

	void RegisterTensorIdx(CTensorIdx<CType>& rTIdx);
	void UnregisterTensorIdx(CTensorIdx<CType>& rTIdx);

protected:
	// Valence of Tensor
	int m_iValence;

	// Dimensions of Tensor
	Mem<int> m_mDim;
	
	// total size of component array
	int m_iSize;              

	// Gives step sizes for every index:
	// Last Index will be 1, 
	// one before last: dimension of last
	// one before that: dim of last * dim of penultimate
	// etc.
	Mem<int> m_mIdxStep;      
	
	// Components of tensor. 
	// Ordered from right index to left index
	Mem<CType> m_mData;       

	// List of pointers to TensorIdx instances
	Mem<CTensorIdx<CType> *> m_mTensorIdx;
	
};



#endif
