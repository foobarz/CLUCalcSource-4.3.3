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


//  Clifford Geometric Algebra Class Definitions


#ifndef _MULTIV_H_
#define _MULTIV_H_


#include "base.h"
#include "blade.h"
#include "bladelist.h"
#include "mvs.h"

#include "mem.h"
#include "memobj.h"
#include "dynlist.h"

#include "xmalib.h"

#ifndef _MAXSTRSIZE_
#define _MAXSTRSIZE_ 1024   // Maximum String Size for String output
#endif

#ifdef _GNUCPP3_
	#ifndef _TMPL_
	#define _TMPL_ <>
	#endif
#	ifndef _FRIEND_
#		define _FRIEND_  friend 
#	endif
#else
	#ifndef _TMPL_
	#define _TMPL_ <>
	#endif
#	ifndef _FRIEND_
#		define _FRIEND_  friend 
#	endif
#endif


typedef unsigned uint;
typedef unsigned long ulong;

enum tMVPos { Left = 0, Right };
enum EMVOpType { MVOP_GEO = 0, MVOP_INNER, MVOP_OUTER, MVOP_ADD, MVOP_SUB, MVOP_DUAL, MVOP_REVERSE, MVOP_INVOLUTE };

template<class CType> class MultiV;



/// Friend function defs for GNUCPP
//#ifdef _GNUCPP3_

template<class CType> bool FactorBlade(const MultiV<CType> &mvA, 
									   MemObj<MultiV<CType> > &mvList, 
									   bool bNorm, MultiV<CType> *pvB,  
									   CType *pFac, CType dPrec);

template<class CType> bool FactorBlade(const MultiV<CType> &mvA, 
									   const MultiV<CType> &vNegDim,
									   MemObj<MultiV<CType> > &mvList, 
									   bool bNorm, MultiV<CType> *pvB,  
									   CType *pFac, CType dPrec);

template<class CType> bool FactorBlade(const MultiV<CType> &vA, 
									   MemObj<MultiV<CType> > &mvList, 
									   MemObj<MultiV<CType> > &mvBasis, 
									   bool bNorm, int *piOrder, 
									   CType *pFac, CType dPrec);


template<class CType> MultiV<CType> Exp(const MultiV<CType>& vA, int iOrder = 9);


//////////////////////////////////////////////
// Multivector Product Matrices

template<class CType> bool CalcMultiVGPMat (const MultiV<CType> &mvA, Matrix<CType> &M, tMVPos bPos = Right,
								bool bEven = false);

template<class CType>  bool CalcMultiVGPMat (const MultiV<CType> &mvA, Matrix<CType> &M, tMVPos bPos,
								const Mem<int>& mGrades);

template<class CType>  bool CalcMultiVGPMatMask (const MultiV<CType> &mvA, Matrix<CType> &M, tMVPos bPos,
								const Mem<int>& mMask, EMVOpType eProdType = MVOP_GEO);

template<class CType>  bool CalcMVListGPMatMask (
								const MemObj<MultiV<CType> > &mvAList, Matrix<CType> &M, tMVPos bPos,
								const Mem<int>& mMask, EMVOpType eProdType = MVOP_GEO,
								bool bPruneNullRows = false);


//////////////////////////////////////////////
/// Multivector Error Propagation Functions

// Evaluate Error Propagating Mean value of Product
// vC: Resultant Multivector
// eProdType: which product
// vA, vB: (Mean) Multivectors of which product is to be evaluated
// Cab: Cross-Covariance Matrix of A and B

template<class CType>  bool EvalEPMeanProd (MultiV<CType> &vC, EMVOpType eProdType, 
											const MultiV<CType> &vA, const MultiV<CType> &vB,
											const Matrix<CType> &Cab);

// Evaluate Error Propagating Covariance Matrix of result of product
// Cuu: Resultant Covariance Matrix
// eProdType: which product
// vX, vY: (Mean) Multivectors of which product is to be evaluated
// Cxx, Cyy: Covariance Matrices of X and Y
// Cxy: Cross-Covariance Matrix of X and Y

template<class CType>  bool EvalEPCovMatProd (Matrix<CType> &Cuu, EMVOpType eProdType, 
											const MultiV<CType> &vX, const MultiV<CType> &vY,
											const Matrix<CType> &Cxx, const Matrix<CType> &Cyy, 
											const Matrix<CType> &Cxy);


// Evaluate Error Propagating Cross-Covariance Matrix of result of product
// Cuz: Resultant Cross-Covariance Matrix
// eProdType: which product
// vX, vY: (Mean) Multivectors of which product is to be evaluated
// Cxz, Cyz: Cross-Covariance Matrices of X and Y with Z

template<class CType>  bool EvalEPCrossCovMatProd (Matrix<CType> &Cuz, EMVOpType eProdType, 
											const MultiV<CType> &vX, const MultiV<CType> &vY,
											const Matrix<CType> &Cxz, const Matrix<CType> &Cyz);



template<class CType>  bool MultiVSVD (const MultiV<CType> &mv, MemObj<MultiV<CType> > &mvList, 
						  Mem<CType> &evList, tMVPos bPos = Right);
	
template<class CType>  uint MultiVJacobi (const MultiV<CType> &mv, MemObj<MultiV<CType> > &mvEVecList, 
						  Mem<CType> &mEValList, tMVPos bPos = Right);

template<class CType>  bool MultiVSolve (const MultiV<CType> &mvA, const MultiV<CType> &mvB, 
							MultiV<CType> &mvX, tMVPos bPos = Left,
							Mem<CType> *pmDiag = 0);

template<class CType>  bool MultiVGEVSolve (const MultiV<CType> &mvA, const MultiV<CType> &mvB, 
							MemObj<MultiV<CType> > &mvX, 
							tMVPos bPos1 = Left, tMVPos bPos2 = Left, bool bEven = false);

template<class CType> MultiV<CType> Join(const MultiV<CType>& vA,const MultiV<CType>& vB, 
										 bool bNorm = true, CType dPrec = 0);

template<class CType> MultiV<CType> Join(const MultiV<CType>& vA, const MultiV<CType>& vB, 
										 const MultiV<CType>& vNegDim, bool bNorm = true, CType dPrec = 0);


	template<class CType> int operator==  (const MultiV<CType> &, const MultiV<CType> &);
	template<class CType> int operator!=  (const MultiV<CType> &, const MultiV<CType> &);
	
	template<class CType> CType Scalar (const MultiV<CType> &);


	template<class CType> bool GradeList (const MultiV<CType> &mvA, Mem<uint> &mList, CType dPrec = CType(0));

	template<class CType> MultiV<CType> Project (const MultiV<CType> &mvA, const MultiV<CType> &mvX);
	template<class CType> MultiV<CType> Reject (const MultiV<CType> &mvA, const MultiV<CType> &mvX);

	template<class CType> MultiV<CType> Involute (const MultiV<CType> &vA);

	template<class CType> MultiV<CType> operator-  (const MultiV<CType> &);  // Negation
	template<class CType> MultiV<CType> operator~  (const MultiV<CType> &);  // Reversion
	template<class CType> MultiV<CType> operator*  (const MultiV<CType> &);  // Dual
	template<class CType> MultiV<CType> operator!  (const MultiV<CType> &);  // Multivector Inverse
    
	template<class CType> MultiV<CType> operator+  (const MultiV<CType>&,const CType&);
	template<class CType> MultiV<CType> operator+  (const CType&,const MultiV<CType>&);
	template<class CType> MultiV<CType>& operator+=  (MultiV<CType>&,const CType&);
	template<class CType> MultiV<CType>& operator<<  (MultiV<CType>&,const CType&);
	
	template<class CType> MultiV<CType> operator-  (const MultiV<CType>&,const CType&);
	template<class CType> MultiV<CType> operator-  (const CType&,const MultiV<CType>&);
	template<class CType> MultiV<CType>& operator-=  (MultiV<CType>&,const CType&);
	
	template<class CType> MultiV<CType> operator+  (const MultiV<CType>&,const Blade<CType>&);
	template<class CType> MultiV<CType> operator+  (const Blade<CType>&,const MultiV<CType>&);
	template<class CType> MultiV<CType>& operator+=  (MultiV<CType>&,const Blade<CType>&);
	template<class CType> MultiV<CType>& operator<<  (MultiV<CType>&,const Blade<CType>&);
	
	template<class CType> MultiV<CType> operator-  (const MultiV<CType>&,const Blade<CType>&);
	template<class CType> MultiV<CType> operator-  (const Blade<CType>&,const MultiV<CType>&);
	template<class CType> MultiV<CType>& operator-=  (MultiV<CType>&,const Blade<CType>&);
	
	template<class CType> MultiV<CType> operator+  (const MultiV<CType>&,const MultiV<CType>&);
	template<class CType> MultiV<CType>& operator+=  (MultiV<CType>&,const MultiV<CType>&);
	template<class CType> MultiV<CType> operator-  (const MultiV<CType>&,const MultiV<CType>&);
	template<class CType> MultiV<CType>& operator-=  (MultiV<CType>&,const MultiV<CType>&);
	template<class CType> MultiV<CType>& operator<<  (MultiV<CType>&,const MultiV<CType>&);
	
	template<class CType> MultiV<CType> operator&  (const MultiV<CType>&,const MultiV<CType>&);
	template<class CType> MultiV<CType>& operator&=  (MultiV<CType>&,const MultiV<CType>&);
	template<class CType> MultiV<CType> operator*  (const MultiV<CType>&,const MultiV<CType>&);
	template<class CType> MultiV<CType>& operator*=  (MultiV<CType>&,const MultiV<CType>&);
	template<class CType> MultiV<CType> operator^  (const MultiV<CType>&,const MultiV<CType>&);
	template<class CType> MultiV<CType>& operator^=  (MultiV<CType>&,const MultiV<CType>&);
	
	template<class CType> MultiV<CType> operator^  (const MultiV<CType>&,const CType &);
	template<class CType> MultiV<CType> operator^  (const CType &,const MultiV<CType>&);
	template<class CType> MultiV<CType>& operator^=  (MultiV<CType>&,const CType &);
	
	template<class CType> MultiV<CType> operator&  (const MultiV<CType>&,const CType &);
	template<class CType> MultiV<CType> operator&  (const CType&,const MultiV<CType>&);
	template<class CType> MultiV<CType>& operator&=  (MultiV<CType>&, const CType &);
/*
	template<class CType> MultiV<CType> operator&  (const MultiV<CType>&,const Blade<CType>&);
	template<class CType> MultiV<CType> operator&  (const Blade<CType>&,const MultiV<CType>&);
	template<class CType> MultiV<CType>& operator&=  (MultiV<CType>&,const Blade<CType>&);
*/	
	template<class CType> MultiV<CType> operator*  (const MultiV<CType>&,const Blade<CType>&);
	template<class CType> MultiV<CType> operator*  (const Blade<CType>&,const MultiV<CType>&);
	template<class CType> MultiV<CType>& operator*=  (MultiV<CType>&,const Blade<CType>&);
	
	template<class CType> MultiV<CType> operator^  (const MultiV<CType>&,const Blade<CType>&);
	template<class CType> MultiV<CType> operator^  (const Blade<CType>&,const MultiV<CType>&);
	template<class CType> MultiV<CType>& operator^=  (MultiV<CType>&,const Blade<CType>&);
	
	template<class CType> MultiV<CType> operator/  (const MultiV<CType>&,const CType &);
	template<class CType> MultiV<CType>& operator/=  (MultiV<CType>&,const CType &);

	template<class CType> BladeList<CType> MakeBladeList (const MultiV<CType>&);
	
	template<class CType> MultiV<CType> MakeVector (MultiVStyle<CType> &m_pStyle, uint grade, const CType *comps);
	template<class CType> MultiV<CType> MakeVector (const MultiV<CType> &mv, uint grade, const CType *comps);
	template<class CType> void GetVector (const MultiV<CType> &mv, uint grade, CType *comps);
	
	template<class CType> MultiV<CType> MakePS (MultiVStyle<CType> &m_pStyle);
	

	template<class CType> bool MultiVGEVSolveMask (const MultiV<CType> &vA, const MultiV<CType> &vB, 
							MemObj<MultiV<CType> > &mvXList, Mem<CType>& mDiag, 
							tMVPos bPos1, tMVPos bPos2, const Mem<int>& mMask);

	template<class CType> bool MultiVGEVSolveMask (const MemObj<MultiV<CType> > &vA, const MemObj<MultiV<CType> >&vB, 
							MemObj<MultiV<CType> > &mvXList, Mem<CType>& mDiag, 
							tMVPos bPos1, tMVPos bPos2, const Mem<int>& mMask);

	template<class CType> bool EvalVersor (const MemObj<MultiV<CType> > &vA, const MemObj<MultiV<CType> >&vB, 
							MemObj<MultiV<CType> > &mvXList, Mem<CType>& mDiag, const Mem<int>& mMask);

	template<class CType> bool MultiVGEVSolve (const MultiV<CType> &vA, const MultiV<CType> &vB, const MultiV<CType> &vC, 
					MultiV<CType> &vX, tMVPos bPos1, tMVPos bPos2, const Mem<int>& mGrades);
	template<class CType> bool MultiVGEVSolveMask (const MultiV<CType> &vA, const MultiV<CType> &vB, const MultiV<CType> &vC, 
					MultiV<CType> &vX, tMVPos bPos1, tMVPos bPos2, const Mem<int> &mMask);

	// Calculate Join/Meet for Blades.
	template<class CType> MultiV<CType> Meet (const MultiV<CType>& mvA,const MultiV<CType>& mvB, CType dPrec = 0);
	template<class CType> MultiV<CType> Meet (const MultiV<CType>& mvA,const MultiV<CType>& mvB,
												const MultiV<CType>& vNegDim, CType dPrec = 0);

	// Only use Imeet to calculate intersections
	template<class CType> MultiV<CType> IMeet (const MultiV<CType>&,const MultiV<CType>&);

//#endif

// Multi Vector Class Declaration ----------------------------

template<class CType>
class MultiV
{
public:
	MultiV(const MultiVStyle<CType> &nstyle, CType ncomps[]=0);
	MultiV(const MultiV<CType> &a);
	MultiV();
	virtual ~MultiV();
	
	void SetStyle(const MultiVStyle<CType> &nstyle);
	
	MultiV<CType>& operator= (const MultiV<CType> &);
	MultiV<CType>& operator= (const Blade<CType> &);
	MultiV<CType>& operator= (const BladeList<CType> &);
	MultiV<CType>& operator= (const CType &);
	
	//_FRIEND_ int operator==  _TMPL_(const MultiV<CType> &, const MultiV<CType> &);
	//_FRIEND_ int operator!=  _TMPL_(const MultiV<CType> &, const MultiV<CType> &);

	uint GetGADim() { return m_uGADim; }
	CType& operator[] (const uint &pos) const;
	MultiV<CType> operator() (uint grade) const;
	CType operator() () const;	
	
	// Returns Homogeneous Multivector part.
	// Selects that grade vector with the largest magnitude.
	// Return result in vHMV.
	bool GetMaxHMV(MultiV<CType> &vHMV, CType dPrec = CType(0)) const;

	//_FRIEND_ CType Scalar _TMPL_(const MultiV<CType> &);

	bool MTProd(const Mem<CType>& mA, const Mem<CType>& mB, int *piTable);
/*
	_FRIEND_ bool GradeList _TMPL_(const MultiV<CType> &mvA, Mem<uint> &mList, CType dPrec);

	_FRIEND_ bool FactorBlade _TMPL_(const MultiV<CType> &mvA, MemObj<MultiV<CType> > &mvList, 
							bool bNorm, MultiV<CType> *pvB, CType *pFac, CType dPrec);

	_FRIEND_ bool FactorBlade _TMPL_(const MultiV<CType> &vA, MemObj<MultiV<CType> > &mvList, 
							MemObj<MultiV<CType> > &mvBasis, 
							bool bNorm, int *piOrder, CType *pFac, CType dPrec);

	_FRIEND_ bool FactorBlade _TMPL_(const MultiV<CType> &mvA, 
									   const MultiV<CType> &vNegDim,
									   MemObj<MultiV<CType> > &mvList, 
									   bool bNorm, MultiV<CType> *pvB,  
									   CType *pFac, CType dPrec);


	_FRIEND_ MultiV<CType> Project _TMPL_(const MultiV<CType> &mvA, const MultiV<CType> &mvX);
	_FRIEND_ MultiV<CType> Reject _TMPL_(const MultiV<CType> &mvA, const MultiV<CType> &mvX);

	_FRIEND_ MultiV<CType> Exp _TMPL_(const MultiV<CType>& vA, int iOrder);

	_FRIEND_ MultiV<CType> Involute _TMPL_(const MultiV<CType> &vA);

	_FRIEND_ MultiV<CType> operator-  _TMPL_(const MultiV<CType> &);  // Negation
	_FRIEND_ MultiV<CType> operator~  _TMPL_(const MultiV<CType> &vR);  // Reversion
*/
	MultiV<CType> Reverse ();  // Reversion
//	_FRIEND_ MultiV<CType> operator*  _TMPL_(const MultiV<CType> &);  // Dual
	MultiV<CType> Dual(); // Dual
/*	_FRIEND_ MultiV<CType> operator!  _TMPL_(const MultiV<CType> &);  // Multivector Inverse
    
	_FRIEND_ MultiV<CType> operator+  _TMPL_(const MultiV<CType>&,const CType&);
	_FRIEND_ MultiV<CType> operator+  _TMPL_(const CType&,const MultiV<CType>&);
	_FRIEND_ MultiV<CType>& operator+=  _TMPL_(MultiV<CType>&,const CType&);
	_FRIEND_ MultiV<CType>& operator<<  _TMPL_(MultiV<CType>&,const CType&);
	
	_FRIEND_ MultiV<CType> operator-  _TMPL_(const MultiV<CType>&,const CType&);
	_FRIEND_ MultiV<CType> operator-  _TMPL_(const CType&,const MultiV<CType>&);
	_FRIEND_ MultiV<CType>& operator-=  _TMPL_(MultiV<CType>&,const CType&);
	
	_FRIEND_ MultiV<CType> operator+  _TMPL_(const MultiV<CType>&,const Blade<CType>&);
	_FRIEND_ MultiV<CType> operator+  _TMPL_(const Blade<CType>&,const MultiV<CType>&);
	_FRIEND_ MultiV<CType>& operator+=  _TMPL_(MultiV<CType>&,const Blade<CType>&);
	_FRIEND_ MultiV<CType>& operator<<  _TMPL_(MultiV<CType>&,const Blade<CType>&);
	
	_FRIEND_ MultiV<CType> operator-  _TMPL_(const MultiV<CType>&,const Blade<CType>&);
	_FRIEND_ MultiV<CType> operator-  _TMPL_(const Blade<CType>&,const MultiV<CType>&);
	_FRIEND_ MultiV<CType>& operator-=  _TMPL_(MultiV<CType>&,const Blade<CType>&);
	
	_FRIEND_ MultiV<CType> operator+  _TMPL_(const MultiV<CType>&,const MultiV<CType>&);
	_FRIEND_ MultiV<CType>& operator+=  _TMPL_(MultiV<CType>&,const MultiV<CType>&);
	_FRIEND_ MultiV<CType> operator-  _TMPL_(const MultiV<CType>&,const MultiV<CType>&);
	_FRIEND_ MultiV<CType>& operator-=  _TMPL_(MultiV<CType>&,const MultiV<CType>&);
	_FRIEND_ MultiV<CType>& operator<<  _TMPL_(MultiV<CType>&,const MultiV<CType>&);
	
	_FRIEND_ MultiV<CType> operator&  _TMPL_(const MultiV<CType>&,const MultiV<CType>&);
	_FRIEND_ MultiV<CType>& operator&=  _TMPL_(MultiV<CType>&,const MultiV<CType>&);
	_FRIEND_ MultiV<CType> operator*  _TMPL_(const MultiV<CType>&,const MultiV<CType>&);
	_FRIEND_ MultiV<CType>& operator*=  _TMPL_(MultiV<CType>&,const MultiV<CType>&);
	_FRIEND_ MultiV<CType> operator^  _TMPL_(const MultiV<CType>&,const MultiV<CType>&);
	_FRIEND_ MultiV<CType>& operator^=  _TMPL_(MultiV<CType>&,const MultiV<CType>&);
	
	_FRIEND_ MultiV<CType> operator^  _TMPL_(const MultiV<CType>&,const CType &);
	_FRIEND_ MultiV<CType> operator^  _TMPL_(const CType &,const MultiV<CType>&);
	_FRIEND_ MultiV<CType>& operator^=  _TMPL_(MultiV<CType>&,const CType &);
	
	_FRIEND_ MultiV<CType> operator&  _TMPL_(const MultiV<CType>&,const CType &);
	_FRIEND_ MultiV<CType> operator&  _TMPL_(const CType&,const MultiV<CType>&);
	_FRIEND_ MultiV<CType>& operator&=  _TMPL_(MultiV<CType>&, const CType &);

	_FRIEND_ MultiV<CType> operator&  _TMPL_(const MultiV<CType>&,const Blade<CType>&);
	_FRIEND_ MultiV<CType> operator&  _TMPL_(const Blade<CType>&,const MultiV<CType>&);
	_FRIEND_ MultiV<CType>& operator&=  _TMPL_(MultiV<CType>&,const Blade<CType>&);
	
	_FRIEND_ MultiV<CType> operator*  _TMPL_(const MultiV<CType>&,const Blade<CType>&);
	_FRIEND_ MultiV<CType> operator*  _TMPL_(const Blade<CType>&,const MultiV<CType>&);
	_FRIEND_ MultiV<CType>& operator*=  _TMPL_(MultiV<CType>&,const Blade<CType>&);
	
	_FRIEND_ MultiV<CType> operator^  _TMPL_(const MultiV<CType>&,const Blade<CType>&);
	_FRIEND_ MultiV<CType> operator^  _TMPL_(const Blade<CType>&,const MultiV<CType>&);
	_FRIEND_ MultiV<CType>& operator^=  _TMPL_(MultiV<CType>&,const Blade<CType>&);
	
	_FRIEND_ MultiV<CType> operator/  _TMPL_(const MultiV<CType>&,const CType &);
	_FRIEND_ MultiV<CType>& operator/=  _TMPL_(MultiV<CType>&,const CType &);

	_FRIEND_ BladeList<CType> MakeBladeList _TMPL_(const MultiV<CType>&);
	
	_FRIEND_ MultiV<CType> MakeVector _TMPL_(MultiVStyle<CType> &m_pStyle, uint grade, const CType *comps);
	_FRIEND_ MultiV<CType> MakeVector _TMPL_(const MultiV<CType> &mv, uint grade, const CType *comps);
	_FRIEND_ void GetVector _TMPL_(const MultiV<CType> &mv, uint grade, CType *comps);
	
	_FRIEND_ MultiV<CType> MakePS _TMPL_(MultiVStyle<CType> &m_pStyle);
	
	_FRIEND_ bool CalcMultiVGPMat _TMPL_(const MultiV<CType> &mvA, Matrix<CType> &M, tMVPos bPos,
								bool bEven);
	_FRIEND_ bool MultiVSVD _TMPL_(const MultiV<CType> &mv, MemObj<MultiV<CType> > &mvList, 
						  Mem<CType> &evList, tMVPos bPos);
	
	_FRIEND_ uint MultiVJacobi _TMPL_(const MultiV<CType> &mv, MemObj<MultiV<CType> > &mvEVecList, 
						  Mem<CType> &mEValList, tMVPos bPos);

	_FRIEND_ bool MultiVSolve _TMPL_(const MultiV<CType> &mvA, const MultiV<CType> &mvB, 
							MultiV<CType> &mvX, tMVPos bPos,
							Mem<CType> *pmDiag);

	_FRIEND_ bool MultiVGEVSolve _TMPL_(const MultiV<CType> &mvA, const MultiV<CType> &mvB, 
							MemObj<MultiV<CType> > &mvX, 
							tMVPos bPos1, tMVPos bPos2, bool bEven);

	_FRIEND_ bool CalcMultiVGPMatMask _TMPL_(const MultiV<CType> &mvA, Matrix<CType> &M, tMVPos bPos,
								const Mem<int>& mMask, EMVOpType iProductType);

	_FRIEND_ bool CalcMVListGPMatMask _TMPL_(const MemObj<MultiV<CType> > &mvAList, Matrix<CType> &M, 
								tMVPos bPos, const Mem<int>& mMask, EMVOpType eProdType,
								bool bPruneNullRows);



	//////////////////////////////////////////////
	/// Multivector Error Propagation Functions

	// Evaluate Error Propagating Mean value of Product
	// vC: Resultant Multivector
	// eProdType: which product
	// vA, vB: (Mean) Multivectors of which product is to be evaluated
	// Cab: Cross-Covariance Matrix of A and B
	// bUseCrossCov: if true Cab is used in calculation, otherwise not.

	_FRIEND_ bool EvalEPMeanProd _TMPL_(MultiV<CType> &vC, EMVOpType eProdType, 
		const MultiV<CType> &vA, const MultiV<CType> &vB,
		const Matrix<CType> &Cab);

	// Evaluate Error Propagating Covariance Matrix of result of product
	// Cuu: Resultant Covariance Matrix
	// eProdType: which product
	// vX, vY: (Mean) Multivectors of which product is to be evaluated
	// Cxx, Cyy: Covariance Matrices of X and Y
	// Cxy: Cross-Covariance Matrix of X and Y

	_FRIEND_ bool EvalEPCovMatProd _TMPL_(Matrix<CType> &Cuu, EMVOpType eProdType, 
		const MultiV<CType> &vX, const MultiV<CType> &vY,
		const Matrix<CType> &Cxx, const Matrix<CType> &Cyy, 
		const Matrix<CType> &Cxy);

	// Evaluate Error Propagating Cross-Covariance Matrix of result of product
	// Cuz: Resultant Cross-Covariance Matrix
	// eProdType: which product
	// vX, vY: (Mean) Multivectors of which product is to be evaluated
	// Cxz, Cyz: Cross-Covariance Matrices of X and Y with Z

	_FRIEND_ bool EvalEPCrossCovMatProd _TMPL_(Matrix<CType> &Cuz, EMVOpType eProdType, 
		const MultiV<CType> &vX, const MultiV<CType> &vY,
		const Matrix<CType> &Cxz, const Matrix<CType> &Cyz);


	_FRIEND_ bool CalcMultiVGPMat _TMPL_(const MultiV<CType> &mvA, Matrix<CType> &M, tMVPos bPos,
								const Mem<int>& mGrades);
	

	_FRIEND_ bool MultiVGEVSolveMask _TMPL_(const MultiV<CType> &vA, const MultiV<CType> &vB, 
							MemObj<MultiV<CType> > &mvXList, Mem<CType>& mDiag, 
							tMVPos bPos1, tMVPos bPos2, const Mem<int>& mMask);

	_FRIEND_ bool MultiVGEVSolveMask _TMPL_(const MemObj<MultiV<CType> > &vA, const MemObj<MultiV<CType> >&vB, 
							MemObj<MultiV<CType> > &mvXList, Mem<CType>& mDiag, 
							tMVPos bPos1, tMVPos bPos2, const Mem<int>& mMask);

	_FRIEND_ bool EvalVersor _TMPL_(const MemObj<MultiV<CType> > &vA, const MemObj<MultiV<CType> >&vB, 
							MemObj<MultiV<CType> > &mvXList, Mem<CType>& mDiag, const Mem<int>& mMask);

	_FRIEND_ bool MultiVGEVSolve _TMPL_(const MultiV<CType> &vA, const MultiV<CType> &vB, const MultiV<CType> &vC, 
					MultiV<CType> &vX, tMVPos bPos1, tMVPos bPos2, const Mem<int>& mGrades);
	_FRIEND_ bool MultiVGEVSolveMask _TMPL_(const MultiV<CType> &vA, const MultiV<CType> &vB, const MultiV<CType> &vC, 
					MultiV<CType> &vX, tMVPos bPos1, tMVPos bPos2, const Mem<int> &mMask);
*/
	MultiV<CType>& TinyToZero(const CType tiny = CType(0));
	MultiV<CType>& Round(const CType fPrec = CType(0));
/*
	// Calculate Join/Meet for Blades.
	_FRIEND_ MultiV<CType> Join _TMPL_(const MultiV<CType>& vA,const MultiV<CType>& vB, bool bNorm, CType dPrec);
	_FRIEND_ MultiV<CType> Join _TMPL_(const MultiV<CType>& vA,const MultiV<CType>& vB, 
										const MultiV<CType> &vNegDim, bool bNorm, CType dPrec);

	_FRIEND_ MultiV<CType> Meet _TMPL_(const MultiV<CType>& mvA,const MultiV<CType>& mvB, CType dPrec);
	_FRIEND_ MultiV<CType> Meet _TMPL_(const MultiV<CType>& mvA, const MultiV<CType>& mvB,
										const MultiV<CType> &vNegDim, CType dPrec);

	// Only use Imeet to calculate intersections
	_FRIEND_ MultiV<CType> IMeet _TMPL_(const MultiV<CType>&,const MultiV<CType>&);
*/	
	CType Mag2() const;  // Magnitude SQUARED
	void Floor();
	void Ceil();
	bool IsZero(CType dPrec) const;  // check whether this is a zero multivector
	char* Str(int strstyle = MVS_SUM);
	
	const MultiVStyle<CType>& GetStyle() const { return *m_pStyle; }
	Base<CType>& GetBase() const { return *(m_pStyle->BasePtr()); }
	bool HasStyle() const { return (m_pStyle ? true : false); }

	Base<CType>* BasePtr() const { return m_pStyle->BasePtr(); }

	
//protected:
	MultiVStyle<CType> *m_pStyle;
	Mem<CType> m_mData;
	uint m_uGADim;
	cStr m_csStr;
	
};


#endif // _MULTIV_H_
