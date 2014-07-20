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


#ifndef __CGAINST_HH
#define __CGAINST_HH

// Use this macro to create a template library of a certain type
// given by TYPE

// Visual C++ doesn't need instantiation of abstract base class!
// GNU C++ seems to want this.

#ifndef WIN32
	#define Instantiate_Base(TYPE) template class Base<TYPE>
#else
	#define Instantiate_Base(TYPE) /**/
#endif


#define Instantiate_CGA(TYPE) \
\
Instantiate_Base(TYPE);\
\
template class Blade<TYPE>;\
\
template class BladeList<TYPE>;\
\
template class MultiVStyle<TYPE>;\
\
template class MultiV<TYPE>;\
\
template int operator==(const Blade<TYPE> &,const Blade<TYPE> &);\
template Blade<TYPE> operator+(const Blade<TYPE> &,const Blade<TYPE> &);\
template Blade<TYPE>& operator+=(Blade<TYPE> &,const Blade<TYPE> &);\
template Blade<TYPE> operator-(const Blade<TYPE> &, const Blade<TYPE> &);\
template Blade<TYPE>& operator-=(Blade<TYPE> &,const Blade<TYPE> &);\
template Blade<TYPE> operator^(const Blade<TYPE> &, const TYPE &);\
template Blade<TYPE> operator^(const TYPE &, const Blade<TYPE> &);\
template Blade<TYPE>& operator^=(Blade<TYPE> &,const TYPE &a);\
template Blade<TYPE> operator-(const Blade<TYPE> &);\
template Blade<TYPE> operator!(const Blade<TYPE> &);\
template Blade<TYPE> operator~(const Blade<TYPE> &);\
template Blade<TYPE> operator&(const Blade<TYPE> &,const Blade<TYPE> &);\
template Blade<TYPE> operator*(const Blade<TYPE> &,const Blade<TYPE> &);\
template Blade<TYPE> operator^(const Blade<TYPE> &,const Blade<TYPE> &);\
template Blade<TYPE> cp(const Blade<TYPE> &,const Blade<TYPE> &);\
template Blade<TYPE> acp(const Blade<TYPE> &,const Blade<TYPE> &);\
template Blade<TYPE> join(const Blade<TYPE> &, const Blade<TYPE> &);\
template Blade<TYPE> meet(const Blade<TYPE> &, const Blade<TYPE> &);\
\
\
template int operator==(const BladeList<TYPE> &,const Blade<TYPE> &);\
\
template TYPE Scalar(const BladeList<TYPE> &bl);\
\
template BladeList<TYPE> operator+(const BladeList<TYPE> &, const TYPE &);\
template BladeList<TYPE> operator+(const TYPE &, const BladeList<TYPE> &);\
\
template BladeList<TYPE> operator-(const BladeList<TYPE> &, const TYPE &);\
template BladeList<TYPE> operator-(const TYPE &, const BladeList<TYPE> &);\
\
template BladeList<TYPE>& operator+=(BladeList<TYPE> &,const TYPE &);\
template BladeList<TYPE>& operator-=(BladeList<TYPE> &,const TYPE &);\
template BladeList<TYPE>& operator<<(BladeList<TYPE> &,const TYPE &);\
\
template BladeList<TYPE> operator+(const BladeList<TYPE> &,\
				    const Blade<TYPE> &);\
template BladeList<TYPE> operator+(const Blade<TYPE> &,\
				    const BladeList<TYPE> &);\
\
template BladeList<TYPE> operator-(const BladeList<TYPE> &,\
				    const Blade<TYPE> &);\
template BladeList<TYPE> operator-(const Blade<TYPE> &,\
				    const BladeList<TYPE> &);\
\
template BladeList<TYPE>& operator+=(BladeList<TYPE> &,const Blade<TYPE> &);\
template BladeList<TYPE>& operator-=(BladeList<TYPE> &,const Blade<TYPE> &);\
\
template BladeList<TYPE>& operator<<(BladeList<TYPE> &,\
				      const Blade<TYPE> &); \
\
template BladeList<TYPE> operator+(const BladeList<TYPE> &,\
				    const BladeList<TYPE> &);\
template BladeList<TYPE> operator-(const BladeList<TYPE> &,\
				    const BladeList<TYPE> &);\
template BladeList<TYPE>& operator+=(BladeList<TYPE> &,\
				      const BladeList<TYPE> &);\
template BladeList<TYPE>& operator-=(BladeList<TYPE> &,\
				      const BladeList<TYPE> &);\
template BladeList<TYPE>& operator<<(BladeList<TYPE> &,\
				      const BladeList<TYPE> &);\
\
template BladeList<TYPE> operator&(const BladeList<TYPE> &,\
				    const Blade<TYPE> &);\
template BladeList<TYPE> operator&(const Blade<TYPE> &,\
				    const BladeList<TYPE> &);\
template BladeList<TYPE>& operator&=(BladeList<TYPE> &,\
				      const Blade<TYPE> &);\
\
template BladeList<TYPE> operator*(const BladeList<TYPE> &,\
				    const Blade<TYPE> &);\
template BladeList<TYPE> operator*(const Blade<TYPE> &,\
				    const BladeList<TYPE> &);\
template BladeList<TYPE>& operator*=(BladeList<TYPE> &,\
				      const Blade<TYPE> &);\
\
template BladeList<TYPE> operator^(const BladeList<TYPE> &,\
				    const TYPE &);\
template BladeList<TYPE> operator^(const TYPE &,\
				    const BladeList<TYPE> &);\
template BladeList<TYPE>& operator^=(BladeList<TYPE> &,\
				      const TYPE &);\
\
template BladeList<TYPE> operator^(const BladeList<TYPE> &,\
				    const Blade<TYPE> &);\
template BladeList<TYPE> operator^(const Blade<TYPE> &,\
				    const BladeList<TYPE> &);\
template BladeList<TYPE>& operator^=(BladeList<TYPE> &,\
				      const Blade<TYPE> &);\
\
template BladeList<TYPE> operator&(const BladeList<TYPE> &,\
				    const BladeList<TYPE> &);\
template BladeList<TYPE>& operator&=(BladeList<TYPE> &,\
				      const BladeList<TYPE> &);\
template BladeList<TYPE> operator*(const BladeList<TYPE> &,\
				    const BladeList<TYPE> &);\
template BladeList<TYPE>& operator*=(BladeList<TYPE> &,\
				      const BladeList<TYPE> &);\
template BladeList<TYPE> operator^(const BladeList<TYPE> &,\
				    const BladeList<TYPE> &);\
template BladeList<TYPE>& operator^=(BladeList<TYPE> &,\
				      const BladeList<TYPE> &);\
\
template BladeList<TYPE> operator~(const BladeList<TYPE> &);\
template BladeList<TYPE> operator-(const BladeList<TYPE> &);\
template BladeList<TYPE> operator!(const BladeList<TYPE> &);\
\
template BladeList<TYPE> cp(const BladeList<TYPE> &,\
			     const Blade<TYPE> &);\
template BladeList<TYPE> acp(const BladeList<TYPE> &,\
			      const Blade<TYPE> &);\
template BladeList<TYPE> cp(const BladeList<TYPE> &,\
			     const BladeList<TYPE> &);\
template BladeList<TYPE> acp(const BladeList<TYPE> &,\
			      const BladeList<TYPE> &);\
\
\
\
\
template int operator==(const MultiVStyle<TYPE> &, const Blade<TYPE> &);\
\
\
template int operator==(const MultiV<TYPE> &, const MultiV<TYPE> &);\
template int operator!=(const MultiV<TYPE> &, const MultiV<TYPE> &);\
\
template TYPE Scalar(const MultiV<TYPE> &);\
\
template MultiV<TYPE> Exp(const MultiV<TYPE>& vA, int iOrder);\
\
template MultiV<TYPE> Involute(const MultiV<TYPE>& vA);\
\
template MultiV<TYPE> operator- (const MultiV<TYPE> &);\
template MultiV<TYPE> operator~ (const MultiV<TYPE> &);\
template MultiV<TYPE> operator* (const MultiV<TYPE> &);\
template MultiV<TYPE> operator! (const MultiV<TYPE> &);\
\
template MultiV<TYPE> operator+ (const MultiV<TYPE>&,const TYPE&);\
template MultiV<TYPE> operator+ (const TYPE&,const MultiV<TYPE>&);\
template MultiV<TYPE>& operator+= (MultiV<TYPE>&,const TYPE&);\
template MultiV<TYPE>& operator<< (MultiV<TYPE>&,const TYPE&);\
\
template MultiV<TYPE> operator- (const MultiV<TYPE>&,const TYPE&);\
template MultiV<TYPE> operator- (const TYPE&,const MultiV<TYPE>&);\
template MultiV<TYPE>& operator-= (MultiV<TYPE>&,const TYPE&);\
\
template MultiV<TYPE> operator+ (const MultiV<TYPE>&,const Blade<TYPE>&);\
template MultiV<TYPE> operator+ (const Blade<TYPE>&,const MultiV<TYPE>&);\
template MultiV<TYPE>& operator+= (MultiV<TYPE>&,const Blade<TYPE>&);\
template MultiV<TYPE>& operator<< (MultiV<TYPE>&,const Blade<TYPE>&);\
\
template MultiV<TYPE> operator- (const MultiV<TYPE>&,const Blade<TYPE>&);\
template MultiV<TYPE> operator- (const Blade<TYPE>&,const MultiV<TYPE>&);\
template MultiV<TYPE>& operator-= (MultiV<TYPE>&,const Blade<TYPE>&);\
\
template MultiV<TYPE> operator+ (const MultiV<TYPE>&,const MultiV<TYPE>&);\
template MultiV<TYPE>& operator+= (MultiV<TYPE>&,const MultiV<TYPE>&);\
template MultiV<TYPE> operator- (const MultiV<TYPE>&,const MultiV<TYPE>&);\
template MultiV<TYPE>& operator-= (MultiV<TYPE>&,const MultiV<TYPE>&);\
template MultiV<TYPE>& operator<< (MultiV<TYPE>&,const MultiV<TYPE>&);\
\
template MultiV<TYPE> operator& (const MultiV<TYPE>&,const MultiV<TYPE>&);\
template MultiV<TYPE>& operator&= (MultiV<TYPE>&,const MultiV<TYPE>&);\
template MultiV<TYPE> operator* (const MultiV<TYPE>&,const MultiV<TYPE>&);\
template MultiV<TYPE>& operator*= (MultiV<TYPE>&,const MultiV<TYPE>&);\
template MultiV<TYPE> operator^ (const MultiV<TYPE>&,const MultiV<TYPE>&);\
template MultiV<TYPE>& operator^= (MultiV<TYPE>&,const MultiV<TYPE>&);\
\
template MultiV<TYPE> operator^ (const MultiV<TYPE>&,const TYPE &);\
template MultiV<TYPE> operator^ (const TYPE &,const MultiV<TYPE>&);\
template MultiV<TYPE>& operator^= (MultiV<TYPE>&,const TYPE &);\
\
template MultiV<TYPE> operator& (const MultiV<TYPE>&,const Blade<TYPE>&);\
template MultiV<TYPE> operator& (const Blade<TYPE>&,const MultiV<TYPE>&);\
template MultiV<TYPE>& operator&= (MultiV<TYPE>&,const Blade<TYPE>&);\
\
template MultiV<TYPE> operator& (const MultiV<TYPE>&,const TYPE &);\
template MultiV<TYPE> operator& (const TYPE&,const MultiV<TYPE>&);\
template MultiV<TYPE>& operator&= (MultiV<TYPE>&, const TYPE &);\
\
template MultiV<TYPE> operator* (const MultiV<TYPE>&,const Blade<TYPE>&);\
template MultiV<TYPE> operator* (const Blade<TYPE>&,const MultiV<TYPE>&);\
template MultiV<TYPE>& operator*= (MultiV<TYPE>&,const Blade<TYPE>&);\
\
template MultiV<TYPE> operator^ (const MultiV<TYPE>&,const Blade<TYPE>&);\
template MultiV<TYPE> operator^ (const Blade<TYPE>&,const MultiV<TYPE>&);\
template MultiV<TYPE>& operator^= (MultiV<TYPE>&,const Blade<TYPE>&);\
\
template MultiV<TYPE> operator/ (const MultiV<TYPE>&,const TYPE &);\
template MultiV<TYPE>& operator/= (MultiV<TYPE>&,const TYPE &);\
\
template BladeList<TYPE> MakeBladeList(const MultiV<TYPE>&);\
\
template MultiV<TYPE> MakeVector(MultiVStyle<TYPE> &style,uint grade, const TYPE *comps);\
template MultiV<TYPE> MakeVector(const MultiV<TYPE> &mv,uint grade, const TYPE *comps);\
template void GetVector(const MultiV<TYPE> &mv, uint grade, TYPE *comps);\
\
template MultiV<TYPE> MakePS(MultiVStyle<TYPE> &style);\
\
template MultiV<TYPE> IMeet(const MultiV<TYPE>&,const MultiV<TYPE>&);\
\
template bool MultiVSVD(const MultiV<TYPE> &mv, MemObj<MultiV<TYPE> > &mvList, Mem<TYPE> &evList, tMVPos bPos);\
\
template uint MultiVJacobi(const MultiV<TYPE> &mv, MemObj<MultiV<TYPE> > &mvEVecList, Mem<TYPE> &mEValList, tMVPos bPos);\
\
template bool CalcMultiVGPMat(const MultiV<TYPE> &mvA, Matrix<TYPE> &M, tMVPos bPos, bool bEven);\
\
template bool CalcMultiVGPMat(const MultiV<TYPE> &mvA, Matrix<TYPE> &M, tMVPos bPos, const Mem<int>& mGrades);\
\
template bool CalcMultiVGPMatMask(const MultiV<TYPE> &mvA, Matrix<TYPE> &M, tMVPos bPos, const Mem<int>& mMask, EMVOpType eOPType);\
\
template bool CalcMVListGPMatMask(const MemObj<MultiV<TYPE> > &mvAList, Matrix<TYPE> &M, \
								tMVPos bPos, const Mem<int>& mMask, EMVOpType eOPType, \
								bool bPruneNullRows);\
template bool EvalEPMeanProd (MultiV<TYPE> &vC, EMVOpType eOPType, \
											const MultiV<TYPE> &vA, const MultiV<TYPE> &vB, \
											const Matrix<TYPE> &Cab);\
\
template bool EvalEPCovMatProd (Matrix<TYPE> &Cuu, EMVOpType eOPType, \
						const MultiV<TYPE> &vX, const MultiV<TYPE> &vY, \
						const Matrix<TYPE> &Cxx, const Matrix<TYPE> &Cyy, \
						const Matrix<TYPE> &Cxy);\
\
template bool EvalEPCrossCovMatProd (Matrix<TYPE> &Cuz, EMVOpType eOPType, \
		const MultiV<TYPE> &vX, const MultiV<TYPE> &vY, \
		const Matrix<TYPE> &Cxz, const Matrix<TYPE> &Cyz);\
\
template bool MultiVSolve(const MultiV<TYPE> &mvA, const MultiV<TYPE> &mvB, MultiV<TYPE> &mvX, tMVPos bPos, Mem<TYPE> *pmDiag);\
\
template bool MultiVGEVSolve(const MultiV<TYPE> &mvA, const MultiV<TYPE> &mvB,\
					 MemObj<MultiV<TYPE> > &mvX, tMVPos bPos1, tMVPos bPos2, bool bEven);\
\
template bool MultiVGEVSolveMask(const MultiV<TYPE> &vA, const MultiV<TYPE> &vB, \
							MemObj<MultiV<TYPE> > &mvXList, Mem<TYPE>& mDiag, \
							tMVPos bPos1, tMVPos bPos2, const Mem<int>& mMask);\
\
template bool MultiVGEVSolveMask(const MemObj<MultiV<TYPE> >&vA, const MemObj<MultiV<TYPE> >&vB, \
							MemObj<MultiV<TYPE> > &mvXList, Mem<TYPE>& mDiag, \
							tMVPos bPos1, tMVPos bPos2, const Mem<int>& mMask);\
\
template bool EvalVersor(const MemObj<MultiV<TYPE> >&vA, const MemObj<MultiV<TYPE> >&vB, \
							MemObj<MultiV<TYPE> > &mvXList, Mem<TYPE>& mDiag, \
							const Mem<int>& mMask);\
\
template bool MultiVGEVSolve(const MultiV<TYPE> &mvA, const MultiV<TYPE> &mvB, const MultiV<TYPE> &mvC,\
					 MultiV<TYPE> &vX, tMVPos bPos1, tMVPos bPos2, const Mem<int>& mGrades);\
\
template bool MultiVGEVSolveMask(const MultiV<TYPE> &mvA, const MultiV<TYPE> &mvB, const MultiV<TYPE> &mvC,\
					 MultiV<TYPE> &vX, tMVPos bPos1, tMVPos bPos2, const Mem<int>& mMask);\
\
template MultiV<TYPE> Project(const MultiV<TYPE> &mvA, const MultiV<TYPE> &mvX);\
template MultiV<TYPE> Reject(const MultiV<TYPE> &mvA, const MultiV<TYPE> &mvX);\
\
template bool GradeList(const MultiV<TYPE> &mvA, Mem<uint> &mList, TYPE dPrec);\
template bool FactorBlade(const MultiV<TYPE> &vA, MemObj<MultiV<TYPE> > &mvList, \
						  bool bNorm, MultiV<TYPE> *pvB, TYPE *pFac, TYPE dPrec);\
template bool FactorBlade(const MultiV<TYPE> &vA, const MultiV<TYPE>& vNegDim, \
						  MemObj<MultiV<TYPE> > &mvList, bool bNorm, \
						  MultiV<TYPE> *pvB, TYPE *pFac, TYPE dPrec);\
template bool FactorBlade(const MultiV<TYPE> &vA, MemObj<MultiV<TYPE> > &mvList, \
						  MemObj<MultiV<TYPE> > &mvBasis, bool bNorm, \
						  int *piOrder, TYPE *pFac, TYPE dPrec);\
\
template MultiV<TYPE> Join(const MultiV<TYPE>& vA,const MultiV<TYPE>& vB, \
							bool bNorm, TYPE dPrec);\
template MultiV<TYPE> Join(const MultiV<TYPE>& vA,const MultiV<TYPE>& vB, \
							const MultiV<TYPE>& vNegDim, bool bNorm, TYPE dPrec);\
template MultiV<TYPE> Meet(const MultiV<TYPE>& mvA,const MultiV<TYPE>& mvB, TYPE dPrec);\
template MultiV<TYPE> Meet(const MultiV<TYPE>& mvA,const MultiV<TYPE>& mvB, \
							const MultiV<TYPE>& vNegDim, TYPE dPrec);\
\

#endif
