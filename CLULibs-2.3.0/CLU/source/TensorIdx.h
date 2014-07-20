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

// Tensor Index Library declaration

#ifndef _TENSOR_INDEX_HH_
#define _TENSOR_INDEX_HH_

#include <map>

#include "mem.h"
#include "makestr.h"
#include "cstr.h"
#include "mathelp.h"
#include "CLUException.h"
#include "TensorData.h"

using std::map;


template<class CType>
class CTensorIdx
{
public:
	class InvalidTensorRef : public CCLUError
	{
	public:
		InvalidTensorRef()
		{
			m_sErrorText = "Invalid tensor reference.";
		}
	};

	class InvalidIdx : public CCLUError
	{
	public:
		InvalidIdx()
		{
			m_sErrorText = "Invalid index.";
		}
	};

	class InvalidIdxRange : public CCLUError
	{
	public:
		InvalidIdxRange()
		{
			m_sErrorText = "Invalid index range.";
		}
	};

	class InvalidIdxList : public CCLUError
	{
	public:
		InvalidIdxList()
		{
			m_sErrorText = "Invalid index list.";
		}
	};

	class InvalidDim : public CCLUError
	{
	public:
		InvalidDim()
		{
			m_sErrorText = "Invalid tensor dimension.";
		}
	};

	class Internal : public CCLUError
	{
	public:
		Internal(const char *pcWhere)
		{
			m_sErrorText = pcWhere;
			m_sErrorText += ": Internal Error.";
		}
	};


public:
	struct SFreeIdxData
	{
		int iIdx;			// Current index value
		int iMin, iMax;		// Index Range
		int iStep;			// Number of elements in tensor to step over
							// if index changes by one.
		int iCount;			// Number of steps in free index.
		Mem<int> mIdx;		// Optionally set list of indices.
		bool bUseIdxList;	// If true, then index list is used.
	};

	typedef map<int, SFreeIdxData> TFreeIdxMap;


public:
	friend class CTensorData<CType>;

public:
	CTensorIdx();
	CTensorIdx(CTensorData<CType>& rT, const Mem<int>& mIdx) throw (CCLUException);
	CTensorIdx(const CTensorIdx<CType>& rTIdx);
	~CTensorIdx();

	CTensorIdx& operator= (const CTensorIdx<CType>& rTIdx);

	// The tensor index is valied, if a tensor and indices are set.
	bool IsValid() const { return m_bIsValid; }

	// Check whether index is constant
	bool IsConstant() const throw (CCLUException);

	// Set tensor to which this index relates, and the index.
	// A negative index gives a free index, and a positive or zero index gives a fixed index.
	void Set(CTensorData<CType>& rT, const Mem<int>& mIdx) throw (CCLUException);

	// Set index to a new tensor, with the same ranges or index lists.
	void Set(CTensorData<CType>& rT, const CTensorIdx<CType>& rTIdx) throw (CCLUException);

	// Set index range of free index
	void SetIdxRange(int iIdx, int iMin, int iMax) throw (CCLUException);

	// Set index list of free index
	void SetIdxList(int iIdx, const Mem<int>& mIdx) throw (CCLUException);

	// Get reference to current data value
	CType& GetRef() throw (CCLUException);

	// Initialize all loops
	void InitAllLoops() throw (CCLUException);

	// Initialize particular loop
	void InitLoop(int iIdx)  throw (CCLUException);

	// Step Loop. No check of validity of index and the instance.
	bool StepLoop(int iIdx);

	// Get reference to tensor element at current loop position.
	// No checking! Only use after InitAllLoops() or InitLoop().
	CType& GetStepRef() { return m_pTensor->Data()[m_iStepPos]; }

	// Get number of steps in loop of given free index.
	int GetStepCount(int iIdx);

	/// Get Free Index List
	const TFreeIdxMap& GetFreeIdxMap() const { return m_mapFreeIdx; }

	// Get Index List
	const Mem<int>& GetIdxList() const { return m_mIdx; }

	// Get Reference to Tensor Data
	CTensorData<CType>& GetTensorData() { if (!m_bIsValid) throw InvalidTensorRef(); return *m_pTensor; }

protected:
	// Reset class
	void Reset();

	// Invalidate Tensor Index, since TensorData has been deleted.
	void Invalidate();

	// Set a free index in the index map
	void SetFreeIdx(int iIdx, int iDim) throw (CCLUException);

	// Update Step Position
	void UpdateStepPos();

protected:
	// Is true if indices are valid and a pointer to a tensor is given.
	bool m_bIsValid;

	// Indices
	Mem<int> m_mIdx;

	// Element position in tensor due to fixed indices
	int m_iFixedPos;

	// Element position of tensor at current step position.
	int m_iStepPos;

	// Map of free indices
	TFreeIdxMap m_mapFreeIdx;

	// Reference to Tensor Data 
	CTensorData<CType> *m_pTensor;
};



#endif
