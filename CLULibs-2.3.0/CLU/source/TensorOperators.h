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

/// Tensor Operator Functions

#ifndef _TENSOR_OPERATORS_HH_
#define _TENSOR_OPERATORS_HH_

//namespace CLUTensor
//{

// Create a tensor from a tensor index
template <class CType>
bool MakeTensor(CTensorData<CType>& rT, CTensorIdx<CType>& rTIdx);

// Set components of tensor to scalar value
template <class CType>
bool SetTensorComps(CTensorIdx<CType>& rTIdx, CType dVal);

// Assign components of one tensor to components of another one
template <class CType>
bool AssignTensorComps(CTensorIdx<CType>& rTTrg, CTensorIdx<CType>& rTSrc);


// Negate Tensor
template <class CType>
bool TensorIdxNegate(CTensorData<CType>& rTResult, CTensorIdx<CType>& rTIdxResult, CTensorIdx<CType>& rTLeft);

// Negate Tensor
template <class CType>
bool TensorNegate(CTensorData<CType>& rTResult, CTensorData<CType>& rT);


// Invert tensor components
template <class CType>
bool TensorIdxInvertComps(CTensorData<CType>& rTResult, CTensorIdx<CType>& rTIdxResult, 
						  CTensorIdx<CType>& rTLeft, const CType& dPrec = 0);

// Invert tensor components
template <class CType>
bool TensorInvertComps(CTensorData<CType>& rTResult, CTensorData<CType>& rT, const CType& dPrec = 0);



// Product with contraction
template <class CType>
bool TensorProductContract(CTensorData<CType>& rTResult, CTensorIdx<CType>& rTIdxResult, CTensorIdx<CType>& rTLeft, CTensorIdx<CType>& rTRight);

// Product without contraction, i.e. element by element
template <class CType>
bool TensorProductPoint(CTensorData<CType>& rTResult, CTensorIdx<CType>& rTIdxResult, CTensorIdx<CType>& rTLeft, CTensorIdx<CType>& rTRight);

// Product with scalar
template <class CType>
bool TensorIdxProductScalar(CTensorData<CType>& rTResult, CTensorIdx<CType>& rTIdxResult, CTensorIdx<CType>& rTLeft, const CType& dVal);

// Product with scalar
template <class CType>
bool TensorProductScalar(CTensorData<CType>& rTResult, CTensorData<CType>& rT, const CType& dVal);



// Div with contraction
template <class CType>
bool TensorDivContract(CTensorData<CType>& rTResult, CTensorIdx<CType>& rTIdxResult, CTensorIdx<CType>& rTLeft, CTensorIdx<CType>& rTRight, const CType& dPrec = 0);

// Div without contraction, i.e. element by element
template <class CType>
bool TensorDivPoint(CTensorData<CType>& rTResult, CTensorIdx<CType>& rTIdxResult, CTensorIdx<CType>& rTLeft, CTensorIdx<CType>& rTRight, const CType& dPrec = 0);

// Div with scalar
template <class CType>
bool TensorIdxDivRightScalar(CTensorData<CType>& rTResult, CTensorIdx<CType>& rTIdxResult, CTensorIdx<CType>& rTLeft, const CType& dVal, const CType& dPrec = 0);

// Div with scalar
template <class CType>
bool TensorDivRightScalar(CTensorData<CType>& rTResult, CTensorData<CType>& rT, const CType& dVal, const CType& dPrec = 0);

// Div with scalar
template <class CType>
bool TensorIdxDivLeftScalar(CTensorData<CType>& rTResult, CTensorIdx<CType>& rTIdxResult, CTensorIdx<CType>& rTLeft, const CType& dVal, const CType& dPrec = 0);

// Div with scalar
template <class CType>
bool TensorDivLeftScalar(CTensorData<CType>& rTResult, CTensorData<CType>& rT, const CType& dVal, const CType& dPrec = 0);



// Addition without contraction, i.e. element by element
template <class CType>
bool TensorAddPoint(CTensorData<CType>& rTResult, CTensorIdx<CType>& rTIdxResult, CTensorIdx<CType>& rTLeft, CTensorIdx<CType>& rTRight);

// Product with scalar
template <class CType>
bool TensorIdxAddScalar(CTensorData<CType>& rTResult, CTensorIdx<CType>& rTIdxResult, CTensorIdx<CType>& rTLeft, const CType& dVal);

// Product with scalar
template <class CType>
bool TensorAddScalar(CTensorData<CType>& rTResult, CTensorData<CType>& rT, const CType& dVal);


// Subtraction without contraction, i.e. element by element
template <class CType>
bool TensorSubPoint(CTensorData<CType>& rTResult, CTensorIdx<CType>& rTIdxResult, CTensorIdx<CType>& rTLeft, CTensorIdx<CType>& rTRight);

// Subtract scalar from right
template <class CType>
bool TensorIdxSubRightScalar(CTensorData<CType>& rTResult, CTensorIdx<CType>& rTIdxResult, CTensorIdx<CType>& rTLeft, const CType& dVal);

// Subtract scalar from right
template <class CType>
bool TensorSubRightScalar(CTensorData<CType>& rTResult, CTensorData<CType>& rT, const CType& dVal);


// Subtract from scalar on left
template <class CType>
bool TensorIdxSubLeftScalar(CTensorData<CType>& rTResult, CTensorIdx<CType>& rTIdxResult, CTensorIdx<CType>& rTLeft, const CType& dVal);

// Subtract from scalar on left
template <class CType>
bool TensorSubLeftScalar(CTensorData<CType>& rTResult, CTensorData<CType>& rT, const CType& dVal);


////////////////////////////////////////////////////////////////////////////////////
/// Instantiate Macro

#define InstantiateTensorOperators(CType) \
template bool MakeTensor(CTensorData<CType>& rT, CTensorIdx<CType>& rTIdx); \
template bool SetTensorComps(CTensorIdx<CType>& rTIdx, CType dVal); \
template bool AssignTensorComps(CTensorIdx<CType>& rTTrg, CTensorIdx<CType>& rTSrc); \
\
template bool TensorIdxNegate(CTensorData<CType>& rTResult, CTensorIdx<CType>& rTIdxResult, CTensorIdx<CType>& rTLeft); \
template bool TensorNegate(CTensorData<CType>& rTResult, CTensorData<CType>& rT); \
\
template bool TensorIdxInvertComps(CTensorData<CType>& rTResult, CTensorIdx<CType>& rTIdxResult, CTensorIdx<CType>& rTLeft, const CType& dPrec); \
template bool TensorInvertComps(CTensorData<CType>& rTResult, CTensorData<CType>& rT, const CType& dPrec); \
\
template bool TensorProductContract(CTensorData<CType>& rTResult, CTensorIdx<CType>& rTIdxResult, CTensorIdx<CType>& rTLeft, CTensorIdx<CType>& rTRight); \
template bool TensorProductPoint(CTensorData<CType>& rTResult, CTensorIdx<CType>& rTIdxResult, CTensorIdx<CType>& rTLeft, CTensorIdx<CType>& rTRight); \
template bool TensorIdxProductScalar(CTensorData<CType>& rTResult, CTensorIdx<CType>& rTIdxResult, CTensorIdx<CType>& rTLeft, const CType& dVal); \
template bool TensorProductScalar(CTensorData<CType>& rTResult, CTensorData<CType>& rT, const CType& dVal); \
\
template bool TensorAddPoint(CTensorData<CType>& rTResult, CTensorIdx<CType>& rTIdxResult, CTensorIdx<CType>& rTLeft, CTensorIdx<CType>& rTRight); \
template bool TensorIdxAddScalar(CTensorData<CType>& rTResult, CTensorIdx<CType>& rTIdxResult, CTensorIdx<CType>& rTLeft, const CType& dVal); \
template bool TensorAddScalar(CTensorData<CType>& rTResult, CTensorData<CType>& rT, const CType& dVal); \
\
template bool TensorSubPoint(CTensorData<CType>& rTResult, CTensorIdx<CType>& rTIdxResult, CTensorIdx<CType>& rTLeft, CTensorIdx<CType>& rTRight); \
template bool TensorIdxSubLeftScalar(CTensorData<CType>& rTResult, CTensorIdx<CType>& rTIdxResult, CTensorIdx<CType>& rTLeft, const CType& dVal); \
template bool TensorSubLeftScalar(CTensorData<CType>& rTResult, CTensorData<CType>& rT, const CType& dVal); \
template bool TensorIdxSubRightScalar(CTensorData<CType>& rTResult, CTensorIdx<CType>& rTIdxResult, CTensorIdx<CType>& rTLeft, const CType& dVal); \
template bool TensorSubRightScalar(CTensorData<CType>& rTResult, CTensorData<CType>& rT, const CType& dVal); \
\
template bool TensorDivContract(CTensorData<CType>& rTResult, CTensorIdx<CType>& rTIdxResult, CTensorIdx<CType>& rTLeft, CTensorIdx<CType>& rTRight, const CType& dPrec); \
template bool TensorDivPoint(CTensorData<CType>& rTResult, CTensorIdx<CType>& rTIdxResult, CTensorIdx<CType>& rTLeft, CTensorIdx<CType>& rTRight, const CType& dPrec); \
template bool TensorIdxDivRightScalar(CTensorData<CType>& rTResult, CTensorIdx<CType>& rTIdxResult, CTensorIdx<CType>& rTLeft, const CType& dVal, const CType& dPrec); \
template bool TensorDivRightScalar(CTensorData<CType>& rTResult, CTensorData<CType>& rT, const CType& dVal, const CType& dPrec); \
template bool TensorIdxDivLeftScalar(CTensorData<CType>& rTResult, CTensorIdx<CType>& rTIdxResult, CTensorIdx<CType>& rTLeft, const CType& dVal, const CType& dPrec); \
template bool TensorDivLeftScalar(CTensorData<CType>& rTResult, CTensorData<CType>& rT, const CType& dVal, const CType& dPrec); 

//} // end namespace
#endif
