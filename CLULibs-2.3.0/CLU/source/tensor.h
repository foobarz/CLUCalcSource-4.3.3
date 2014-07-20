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

// Tensor Library declaration

#ifndef _TENSOR_HH_
#define _TENSOR_HH_

#include "mem.h"
#include "makestr.h"
#include "cstr.h"
#include "mathelp.h"


#ifdef _GNUCPP3_
	#ifndef _TMPL_
	#define _TMPL_ <>
	#endif
#	ifndef _FRIEND_
#		define _FRIEND_ template <class CType> friend 
#	endif
#else
	#ifndef _TMPL_
	#define _TMPL_
	#endif
#	ifndef _FRIEND_
#		define _FRIEND_ template <class CType> friend 
#	endif
#endif


#define DIMS (uint[])
#define TCOMP (uint[])
#define RANGES (uint[])

#ifndef _MAXSTRSIZE_
#define _MAXSTRSIZE_ 1024
#endif

// DEBUG defines
//#define _SHOWLOOPS_
//#define _SHOWTIMES_
//#define _SHOWSUM_

typedef unsigned int uint;


/* 
***  IMPORTANT  ***
Internal counting of indices always starts at 0. All protected functions expect
Indices to be given in this way. However, the user can choose what the first
index should be, by setting fidx, with regard to public functions that access
the components of the tensor directly.
*/


template<class CType> class Tensor;

template<class CType> Tensor<CType> operator-  (const Tensor<CType>& t);

template<class CType> Tensor<CType>& operator,  (Tensor<CType>& t, uint* idx);
template<class CType> Tensor<CType>& operator,  (Tensor<CType>& t, uint pos);

template<class CType> Tensor<CType> operator*  (const Tensor<CType>& t1, const Tensor<CType>& t2);
template<class CType> Tensor<CType> operator* (const Tensor<CType>& t1, const CType a);
template<class CType> Tensor<CType> operator* (const CType a, const Tensor<CType>& t1);

template<class CType> Tensor<CType>& operator*= (Tensor<CType>& t1, const Tensor<CType>& t2);
template<class CType> Tensor<CType>& operator*= (Tensor<CType>& t1, const CType a);

// Multiply Tensors without contraction
template<class CType> Tensor<CType> operator&  (const Tensor<CType>& t1, const Tensor<CType>& t2);
template<class CType> Tensor<CType>& operator&= (Tensor<CType>& t1, const Tensor<CType>& t2);

template<class CType> Tensor<CType> operator/  (const Tensor<CType>& t1, const Tensor<CType>& t2);
template<class CType> Tensor<CType> operator/ (const Tensor<CType>& t1, const CType a);

template<class CType> Tensor<CType>& operator/= (Tensor<CType>& t1, const Tensor<CType>& t2);
template<class CType> Tensor<CType>& operator/= (Tensor<CType>& t1, const CType a);

// Divide Tensors without contraction
template<class CType> Tensor<CType> operator%  (const Tensor<CType>& t1, const Tensor<CType>& t2);
template<class CType> Tensor<CType>& operator%= (Tensor<CType>& t1, const Tensor<CType>& t2);

template<class CType> Tensor<CType> operator+  (const Tensor<CType>& t1, const Tensor<CType>& t2);
template<class CType> Tensor<CType>& operator+= (Tensor<CType>& t1, const Tensor<CType>& t2);
template<class CType> Tensor<CType> operator+ (const Tensor<CType>& t1, const CType& a);
template<class CType> Tensor<CType>& operator+= (Tensor<CType>& t1, const CType& a);

template<class CType> Tensor<CType> operator-  (const Tensor<CType>& t1, const Tensor<CType>& t2);
template<class CType> Tensor<CType>& operator-= (Tensor<CType>& t1, const Tensor<CType>& t2);
template<class CType> Tensor<CType> operator-  (const Tensor<CType>& t1, const CType& a);
template<class CType> Tensor<CType>& operator-= (Tensor<CType>& t1, const CType& a);




template<class CType>
class Tensor
{
public:
	enum idxType { Number = 0, Pointer };

	struct idxProp
	{ 
		idxType type;
		union dataType
		{ 
			uint pos;
			uint* idx;
		} data;
	};

	struct rangeType   // Ses counting range
	{ 
		uint first;      // First counting index
		uint last;       // Last counting index (inclusive)
	};

	struct loopVar
	{ 
		uint *var;    // Pointer to counting variable
		rangeType count;   // Counting Range
		uint step;    // Step size in component array
		uint unique;  // Used in contraction operations like *
		// 1: if index is unique in calculation
		uint dim;     // The dimension this loop refers to
	};


	struct cloopVar   // Combined Loop variable
	{ 
		uint *var;    // Pointer to counting variable
		rangeType count;   // Counting Range
		uint step[3]; // Step size in comp array of three different tensors
	};


public:
	Tensor();
	Tensor(const Tensor<CType>& t);
	Tensor(uint nrank, uint* ndims, uint nfidx = 1);
	//~Tensor();
	
	void Reset(uint nrank, uint* ndims, uint nfidx = 1); 
	void ResetActIdx() { actidx = 0; }
	
	uint Size() const;
	uint isOK();
	uint ErrNo() const { return Terrno; }
	uint Dim(uint d) const { return dims[d-1]; }
	uint Rank() const { return rank; }

	uint InitLoops(); // Initialization of loops. Returns no of loops
	uint StepLoops(); // Steps through loops. Return 0 when loops finished
	CType& ActPosElement() { return comps[actpos]; } // Use only within InitLoops, StepLoops
	
	Tensor<CType>& Norm(CType *_pmax = 0); // Divide Tensor by its largest element
	CType FrobNorm(); // Frobenius Norm: sqrt of sum of squares of elements
	CType Mean(bool useMag = false); // Calculate Mean of Tensor
	
	char* Str();
	
	void IdxRange(uint rng[]);
	void IdxRange(uint d, uint f, uint l);
	void ResetIdxRange();
	
	Tensor<CType>& Idx(uint dim, const uint pos);
	Tensor<CType>& Idx(uint dim, uint* idx);
	
	uint ResizeDim(uint d, uint no);
	
	CType& operator() () { return ActComp(); } 
	
	Tensor<CType>& operator= (const CType& a);
	Tensor<CType>& operator= (const CType* a);
	Tensor<CType>& operator= (const Tensor<CType>& t);
	
//	_FRIEND_ Tensor<CType> operator-  _TMPL_(const Tensor<CType>& t);
	
	// Creates Copy of Tensor
	//  _FRIEND_ Tensor<CType>& operator+ (const Tensor<CType>& t);
	
	Tensor<CType>& operator<< (const Tensor<CType>& t);
	
//	_FRIEND_ Tensor<CType>& operator,  _TMPL_(Tensor<CType>& t, uint* idx);
//	_FRIEND_ Tensor<CType>& operator,  _TMPL_(Tensor<CType>& t, uint pos);
/*	
	_FRIEND_ Tensor<CType> operator*  _TMPL_(const Tensor<CType>& t1, const Tensor<CType>& t2);
	_FRIEND_ Tensor<CType> operator* _TMPL_(const Tensor<CType>& t1, const CType a);
	_FRIEND_ Tensor<CType> operator* _TMPL_(const CType a, const Tensor<CType>& t1);

	_FRIEND_ Tensor<CType>& operator*= _TMPL_(Tensor<CType>& t1, const Tensor<CType>& t2);
	_FRIEND_ Tensor<CType>& operator*= _TMPL_(Tensor<CType>& t1, const CType a);
	
	// Multiply Tensors without contraction
	_FRIEND_ Tensor<CType> operator&  _TMPL_(const Tensor<CType>& t1, const Tensor<CType>& t2);
	_FRIEND_ Tensor<CType>& operator&= _TMPL_(Tensor<CType>& t1, const Tensor<CType>& t2);
	
	_FRIEND_ Tensor<CType> operator/  _TMPL_(const Tensor<CType>& t1, const Tensor<CType>& t2);
	_FRIEND_ Tensor<CType> operator/ _TMPL_(const Tensor<CType>& t1, const CType a);

	_FRIEND_ Tensor<CType>& operator/= _TMPL_(Tensor<CType>& t1, const Tensor<CType>& t2);
	_FRIEND_ Tensor<CType>& operator/= _TMPL_(Tensor<CType>& t1, const CType a);
	
	// Divide Tensors without contraction
	_FRIEND_ Tensor<CType> operator%  _TMPL_(const Tensor<CType>& t1, const Tensor<CType>& t2);
	_FRIEND_ Tensor<CType>& operator%= _TMPL_(Tensor<CType>& t1, const Tensor<CType>& t2);
	
	_FRIEND_ Tensor<CType> operator+  _TMPL_(const Tensor<CType>& t1, const Tensor<CType>& t2);
	_FRIEND_ Tensor<CType>& operator+= _TMPL_(Tensor<CType>& t1, const Tensor<CType>& t2);
	_FRIEND_ Tensor<CType> operator+ _TMPL_(const Tensor<CType>& t1, const CType& a);
	_FRIEND_ Tensor<CType>& operator+= _TMPL_(Tensor<CType>& t1, const CType& a);
	
	_FRIEND_ Tensor<CType> operator-  _TMPL_(const Tensor<CType>& t1, const Tensor<CType>& t2);
	_FRIEND_ Tensor<CType>& operator-= _TMPL_(Tensor<CType>& t1, const Tensor<CType>& t2);
	_FRIEND_ Tensor<CType> operator-  _TMPL_(const Tensor<CType>& t1, const CType& a);
	_FRIEND_ Tensor<CType>& operator-= _TMPL_(Tensor<CType>& t1, const CType& a);
*/	
	CType* Data() const { return comps.Data(); }
	
	
//protected:
	
	// *** Protected Vars
	cStr str;               // String Output
	
	uint Terrno;            // Error Number
	
	uint fidx;              // first index for user
	uint actidx;            // new index will be written to this position
	// in the index list "idxlist"
	uint actpos;            // active position in comp array.
	// used by implicit summation routines
	
	uint rank;
	Mem<uint> dims;
	Mem<rangeType> idxrange; // Range for counting indices
	
	uint size;              // total size of comp array
	Mem<uint> idxstep;      // Gives step sizes for every index:
	// Last Index will be 1, 
	// one before last: dimension of last
	// one before that: dim of last * dim of penultimate
	// etc.
	
	Mem<idxProp> idxlist;   // lists active indices
	
	Mem<loopVar> looplist;  // Data describing loops
	
	Mem<CType> comps;       // Components of tensor. 
	// Ordered from right index to left index
	
	// *** Protected functions
	
	void copyTensor(const Tensor<CType>& t);
	void setDims(uint nrank, uint* ndims);// Set dims and related data structures
	
	CType& Comp(uint posa[]);  // return component at position 
	// given by the indices in posa
	// Indices start at zero. NO range check.
	
	CType& ActComp(); // Same as Comp, only it uses position given in idxlist
	
	uint ActPos();  // Active Position
/*
	// Initialization of contraction loops
	_FRIEND_ int InitCLoops _TMPL_(Tensor<CType>& t1, Tensor<CType>& t2,
		      uint apos[], Mem<cloopVar>& loops, Tensor<CType>& nt0); 
	
	// Initialization of non-contracting multiple loops
	_FRIEND_ int InitMLoops _TMPL_(Tensor<CType>& t1, Tensor<CType>& t2,
		      uint apos[], Mem<cloopVar>& loops, Tensor<CType>& nt0); 
	
	// Step Contraction and Multiple Loops
	_FRIEND_ int StepCMLoops(int lno, uint apos[], Mem<cloopVar>& loops); 
*/
};

/*
// Initialization of contraction loops
template<class CType> int InitCLoops(Tensor<CType>& t1, Tensor<CType>& t2,
									 uint apos[], Mem<Tensor::cloopVar>& loops, 
									 Tensor<CType>& nt0); 

// Initialization of non-contracting multiple loops
template<class CType> int InitMLoops(Tensor<CType>& t1, Tensor<CType>& t2,
									 uint apos[], Mem<Tensor::cloopVar>& loops, 
									 Tensor<CType>& nt0); 

// Step Contraction and Multiple Loops
template<class CType> 
int StepCMLoops(int lno, uint apos[], Mem<Tensor::cloopVar>& loops); 
*/


#endif
