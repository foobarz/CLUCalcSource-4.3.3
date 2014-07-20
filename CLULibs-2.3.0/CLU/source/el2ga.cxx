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


// El2GA Function and Class Definitions
#ifndef _EL2GA_TEMPLATE_
#define _EL2GA_TEMPLATE_

#include "el2ga.h"
#include "el2gainst.h"



////////////////////////////////////////////////////////////////////////////////////
/// Static Variables

//template<class CType> RingBuf<uint> El2GA<CType>::BLADE;
template<class CType> MultiV<CType> El2GA<CType>::vZero;
template<class CType> MultiV<CType> El2GA<CType>::vSC;
template<class CType> MultiV<CType> El2GA<CType>::vE1;
template<class CType> MultiV<CType> El2GA<CType>::vE2;
template<class CType> MultiV<CType> El2GA<CType>::vE3;
template<class CType> MultiV<CType> El2GA<CType>::vE4;
template<class CType> MultiV<CType> El2GA<CType>::vE5;
template<class CType> MultiV<CType> El2GA<CType>::vE6;
template<class CType> MultiV<CType> El2GA<CType>::vI;

template<class CType> const uint El2GA<CType>::iSC = 0;
template<class CType> const uint El2GA<CType>::iE1 = 1;
template<class CType> const uint El2GA<CType>::iE2 = 2;
template<class CType> const uint El2GA<CType>::iE3 = 3;
template<class CType> const uint El2GA<CType>::iE4 = 4;
template<class CType> const uint El2GA<CType>::iE5 = 5;
template<class CType> const uint El2GA<CType>::iE6 = 6;
template<class CType> const uint El2GA<CType>::iI = 63;

template<class CType> const CType El2GA<CType>::m_dSqrt2 = CType(sqrt(2.0));

template<class CType> MultiVStyle<CType> El2GA<CType>::m_MVStyle;
template<class CType> MemObj<Mem<uint> > El2GA<CType>::m_mBladeIdx;

// El2GA needs initialisation
template<class CType> bool El2GA<CType>::m_bInit = true;


#include "El2GATables.h"


////////////////////////////////////////////////////////////////////////////////////
/// Functions

template<class CType>
El2GA<CType>::El2GA() : Base<CType>(6,1,"e",ID_EL2GA)
{
	if (m_bInit) 
	{
		InitVars();
		m_bInit = false;
	}
}


// El2GA Signature
template<class CType>
inline CType El2GA<CType>::signature(uint i) 
{ 
	return (CType(1)); 
}

////////////////////////////////////////////////////////////////////////////////////
// Generates all blades of a particular grade, using recursion.
template<class CType>
bool El2GA<CType>::GenerateBlades(BladeList<CType>& blList, MemObj<Mem<uint> >& mBladeIdx, 
								  uint uGrade, uint uFirstIdx, uint *puIdxList, uint uIdxPos)
{
	uint uDim = this->VSDim();
	uint uIdx;

	// Assume that indices start counting with 1
	if (uGrade < 0 || uGrade > uDim || uFirstIdx < 1)
		return false;

	// Scalar blade
	if (uGrade == 0)
	{
		// Save Index List
		mBladeIdx.Add(1);
		Mem<uint>& mIdxList = mBladeIdx.Last();
		if (uIdxPos > 0)
		{
			mIdxList.Set(uIdxPos);
			for(uint i = 0; i < uIdxPos; i++)
				mIdxList[i] = puIdxList[i];
		}
		else
		{
			mIdxList.Set(0);
		}

		Blade<CType> bDummy;
		cStr csName;
		csName = "e";
		for(int i = 0; i < int(uIdxPos); i++)
			csName << puIdxList[i];

		bDummy.Reset(*this, CType(1), uIdxPos, puIdxList, csName.Str());
		blList << bDummy;
		return true;
	}


	// Blade of higher grade
	for (uIdx = uFirstIdx; uIdx <= uDim; uIdx++)
	{
		puIdxList[uIdxPos] = uIdx;
		if (!GenerateBlades(blList, mBladeIdx, uGrade-1, uIdx+1, puIdxList, uIdxPos+1))
			return false;
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////////
/// Init Variables

template<class CType>
void El2GA<CType>::InitVars()
{
	BladeList<CType> El2GAStyle(*this);

	uint uGrade;
	uint puIdxList[6];

	// Generate a list of all blades, where the basis vectors
	// are given in a lexicographical order.
	for (uGrade = 0; uGrade <= 6; uGrade++)
	{
		GenerateBlades(El2GAStyle, m_mBladeIdx, uGrade, 1, puIdxList);
	}

	
	// Initialises multiplication tables
	m_MVStyle.Init(El2GAStyle, piGrades, piGPTable, piIPTable, 
					piOPTable, piVITable, piDualTable); 
	
	vZero.SetStyle(m_MVStyle);
	vSC.SetStyle(m_MVStyle); vSC[0] = 1;
	vE1.SetStyle(m_MVStyle); vE1[1] = 1;
	vE2.SetStyle(m_MVStyle); vE2[2] = 1;
	vE3.SetStyle(m_MVStyle); vE3[3] = 1;
	vE4.SetStyle(m_MVStyle); vE4[4] = 1;
	vE5.SetStyle(m_MVStyle); vE5[5] = 1;
	vE6.SetStyle(m_MVStyle); vE6[6] = 1;
	vI.SetStyle(m_MVStyle); vI[63] = 1;

}

// Additional Functions -----------------------------


////////////////////////////////////////////////////////////////////////////////////
// Embed a 2d Euclidean vector in this algebra. 
// The vector will be homogenized by adding a 1 in a third dimension.
// Then the tensor product of the vector with itself is evaluated and
// the upper triangular part of the resultant symmetric matrix is encoded
// as a vector in this 6d space.

template<class CType>
MultiV<CType> El2GA<CType>::Vec(CType dX, CType dY)
{
	MultiV<CType> vA(m_MVStyle);
	CType dVal;

	dVal = CType(1.0 / sqrt(2.0));

	vA[1] = dX;
	vA[2] = dY;
	vA[3] = dVal;
	vA[4] = dVal*dX*dX;
	vA[5] = dVal*dY*dY;
	vA[6] = dX*dY;

	return vA;
}


////////////////////////////////////////////////////////////////////////////////////
// Embed a 3x3 symmetric matrix as vector in this algebra. 
// Uses upper triangular part of matrix for embedding.

template<class CType>
MultiV<CType> El2GA<CType>::Vec(Matrix<CType>& xM)
{
	MultiV<CType> vA(m_MVStyle);
	CType dVal;

	dVal = CType(1.0 / sqrt(2.0));

	vA[1] = xM(1,3);
	vA[2] = xM(2,3);
	vA[3] = dVal*xM(3,3);
	vA[4] = dVal*xM(1,1);
	vA[5] = dVal*xM(2,2);
	vA[6] = xM(1,2);

	return vA;
}


////////////////////////////////////////////////////////////////////////////////////
// Generate 3x3 symmetric matrix from given vector in this algebra. 

template<class CType>
Matrix<CType> El2GA<CType>::Mat(const MultiV<CType>& vA)
{
	Matrix<CType> xM(3,3);
	CType dVal;
	
	if (vA.BasePtr()->BaseID() != ID_EL2GA)
		return xM;

	dVal = CType(sqrt(2.0));

	xM(1,3) = xM(3,1) = vA[1];
	xM(2,3) = xM(3,2) = vA[2];
	xM(3,3) = vA[3]*dVal;
	xM(1,1) = vA[4]*dVal;
	xM(2,2) = vA[5]*dVal;
	xM(1,2) = xM(2,1) = vA[6];

	return xM;
}



////////////////////////////////////////////////////////////////////////////////////
/// Find All Degenerate Conics
// Expects vA and vB to represent conics as vectors, i.e. through their IPNS.
// Finds all degenerate conics that can be generated through the linear comb.
// of the given conics.

template<class CType>
bool El2GA<CType>::FindAllDegConics(const MultiV<CType>& vA, const MultiV<CType>& vB,
								MemObj<MultiV<CType> >& mConicList, CType fPrec)
{
	if (vA.BasePtr()->BaseID() != ID_EL2GA ||
		vB.BasePtr()->BaseID() != ID_EL2GA)
		return false;

	int i;
	CType dZeroMin, dZeroMax, dValA, dValB;
	Matrix<CType> xMA, xMB, xDegMat, xFitMat, xM, xREVals, xIEVals;
	MultiV<CType> vDeg, vFit, vR;

	if (fPrec <= CType(0))
		Tiny(fPrec);

	dZeroMin = -fPrec;
	dZeroMax = fPrec;

	xMA = Mat(vA);
	xMB = Mat(vB);

	// Check whether either of the two matrices is degenerate.
	dValA = xMA.Det();
	dValB = xMB.Det();

	if ((dValA >= dZeroMin && dValA <= dZeroMax) &&
		(dValB >= dZeroMin && dValB <= dZeroMax))
	{
		// Both matrices are degenerate
		// Need to generate one matrix that is not degenerate
		// in order to find all degenerate matrices.
		// One of these newly generated matrices has to be non-degenerate
		// since there are at most three degenerate matrices and
		// xMA and xMB are already degenerate.
		xDegMat = xMA + xMB;
		xFitMat = xMA - xMB;

		// Take the matrix with the larger determinant
		// as the new non-degenerate matrix
		if (Mag(xDegMat.Det()) > Mag(xFitMat.Det()))
		{
			xFitMat = xDegMat;
			xDegMat = xMA;
		}
	}
	else if (dValA >= dZeroMin && dValA <= dZeroMax)
	{
		// xMA is degenerate.
		xDegMat = xMA;
		xFitMat = xMB;
		vDeg = vA;
		vFit = vB;
	}
	else if (dValB >= dZeroMin && dValB <= dZeroMax)
	{
		// xMB is degenerate
		xDegMat = xMB;
		xFitMat = xMA;
		vDeg = vB;
		vFit = vA;
	}
	else
	{
		// Set xDegMat and xFitMat arbitrarily
		xDegMat = xMA;
		xFitMat = xMB;
		vDeg = vA;
		vFit = vB;
	}

	// Now evaluate all degenerate conics.
	// There has to be at least one.
	xM = (!xFitMat) * xDegMat;

	// Get Eigenvalues of xM
	if (!EigenValues(xM, xREVals, xIEVals))
		return false;

	mConicList.Set(0);
	for (i = 0; i < 3; i++)
	{
		if (xIEVals(1,i+1) >= dZeroMax || xIEVals(1,i+1) <= dZeroMin)
			continue; // imaginary eigenvalue, i.e. no deg. conic

		// Generate matrix representing degenerate conic
		xM = xDegMat - xREVals(1,i+1)*xFitMat;
		vR = Vec(xM);
		if (!vR.IsZero(fPrec))
		{
			mConicList.Add(1);
			mConicList.Last() = vR;
		}
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////////
/// Intersect Conics
// Expects vA and vB to represent conics as vectors, i.e. through their IPNS.

template<class CType>
bool El2GA<CType>::IntersectConics(const MultiV<CType>& vA, const MultiV<CType>& vB,
								MemObj<CPointND<CType,2> >& mPointList, CType fPrec)
{
	if (vA.BasePtr()->BaseID() != ID_EL2GA ||
		vB.BasePtr()->BaseID() != ID_EL2GA)
		return false;

	// xDegMat is set to the degenerate matrix
	// xFitMat is the matrix with which xDegMat is intersected
	bool bAllDeg = false, bOneDeg = false, bDoGenDeg = false;
	int i, j;
	CType dZeroMin, dZeroMax, dValA, dValB, dA, dB, dC;
	Matrix<CType> xMA, xMB, xDegMat, xFitMat, xM, xREVals, xIEVals;
	Matrix<CType> xP, xR[2];
	MultiV<CType> vDeg, vFit, vR;
	CMVInfo<CType> Info;

	if (fPrec <= CType(0))
		Tiny(fPrec);

	dZeroMin = -fPrec;
	dZeroMax = fPrec;

	xMA = Mat(vA);
	xMB = Mat(vB);

	// Check whether either of the two matrices is degenerate.
	dValA = xMA.Det();
	dValB = xMB.Det();

	if ((dValA >= dZeroMin && dValA <= dZeroMax) &&
		(dValB >= dZeroMin && dValB <= dZeroMax))
	{
		// Both matrices are degenerate
		bAllDeg = true;
	}
	else if (dValA >= dZeroMin && dValA <= dZeroMax)
	{
		// xMA is degenerate.
		xDegMat = xMA;
		xFitMat = xMB;
		vDeg = vA;
		vFit = vB;
		bOneDeg = true;
	}
	else if (dValB >= dZeroMin && dValB <= dZeroMax)
	{
		// xMB is degenerate
		xDegMat = xMB;
		xFitMat = xMA;
		vDeg = vB;
		vFit = vA;
		bOneDeg = true;
	}
	else
	{
		// Need to generate a degenerate matrix
		// Set xDegMat and xFitMat arbitrarily
		xDegMat = xMA;
		xFitMat = xMB;
		vDeg = vA;
		vFit = vB;
		bDoGenDeg = true;
	}

	if (!bAllDeg)
	{
		// Not both matrices are degenerate

		if (bOneDeg)
		{
			// There is one degenerate conic

			// Check whether we should generate a degenerate matrix.
			if (!AnalyzeMV(vDeg, Info, false, fPrec))
				return false;

			// We prefer a line pair since it should give more stable results
			// than a parallel line pair.
			if (Info.m_eType != GA_LINEPAIR)
			{
				bDoGenDeg = true; // Generate a new degenerate conic
			}
		}

		if (bDoGenDeg)
		{
			// If we do not have a line pair, we should try to generate one
			// that shares the same intersection points as the two given conics.
			xM = (!xFitMat) * xDegMat;

			// Get Eigenvalues of xM
			if (!EigenValues(xM, xREVals, xIEVals))
				return false;

			for (i = 0; i < 3; i++)
			{
				if (!::IsZero(xIEVals(1,i+1), fPrec))
					continue; // imaginary eigenvalue, i.e. no deg. conic

				// Generate matrix representing degenerate conic
				xM = xDegMat - xREVals(1,i+1)*xFitMat;
				vR = Vec(xM);
				//vR = vDeg - (xREVals(1,i+1)^vFit);
				//vR.Round(fPrec);

				if (!AnalyzeMV(vR, Info, false, fPrec))
					return false;

				if (Info.m_eType == GA_LINEPAIR)
					break; // We found a line pair
			}

			if (i == 3)
			{
				// If we get to this point then no degenerate conic which
				// represents a line pair was found.
				// Maybe could use parallel line pairs instead. For now we will
				// not process this any further.
				return false;
			}
		}

		// Now we should have a degenerate conic analysis in Info.
		if (Info.m_eType == GA_LINEPAIR)
		{
			CType dSr, dS1, dS2;

			// Intersect line pair with non-degenerate conic (xFitMat)
			xP.Resize(3,1);
			xR[0].Resize(3,1);
			xR[1].Resize(3,1);

			xP = Info.m_mVex[0].Data();			// Line offset
			xP(3,1) = CType(1);
			xR[0] = Info.m_mVex[1].Data();		// Dir. of 1st line
			xR[0](3,1) = CType(0);

			xR[1] = Info.m_mVex[2].Data();		// Dir. of 2nd line
			xR[1](3,1) = CType(0);

			for (j = 0; j < 2; j++ )
			{
				dC = (~xP * xFitMat * xP)(1,1);
				dB = (~xP * xFitMat * xR[j] + ~xR[j] * xFitMat * xP)(1,1);
				dA = (~xR[j] * xFitMat * xR[j])(1,1);
			
				dSr = dB*dB - 4*dA*dC;
				if (dSr > dZeroMax)
				{
					dS1 = (-dB + CType(sqrt(dSr)))/(2*dA);
					dS2 = (-dB - CType(sqrt(dSr)))/(2*dA);
		
					int iCount = mPointList.Count();
					mPointList.Add(2);

					xM = xP + dS1 * xR[j];
					mPointList[iCount] = xM.Data();

					xM = xP + dS2 * xR[j];
					mPointList[iCount + 1] = xM.Data();
				}
				else if (dSr >= dZeroMin && dSr <= dZeroMax)
				{
					dS1 = -dB/(2*dA);
		
					int iCount = mPointList.Count();
					mPointList.Add(1);

					xM = xP + dS1 * xR[j];
					mPointList[iCount] = xM.Data();
				}

			} // for j
		} // Info.m_eType == GA_LINEPAIR
	}
	else // both matrices are degenerate
	{
		return false; // Cannot handle this yet.
	}

	return true;
}




////////////////////////////////////////////////////////////////////////////////////
// Analyse Multivector. 
// Result is returned in rMVI.
// If function returns false, MV could not be analyzed.

template<class CType>
bool El2GA<CType>::AnalyzeMV(const MultiV<CType>& vX, CMVInfo<CType>& rMVI, 
							 bool bOPNS, CType fPrec)
{
	MultiV<CType> vA(vX);
	Mem<uint> mGList;
	int iGrades;

	rMVI.Reset();

	if (!GradeList(vA, mGList, fPrec)) return false;
	if ((iGrades = mGList.Count()) == 0) return false;

	if (iGrades == 2 && mGList[0] == 2 && mGList[1] == 0)
	{
		return AnalyzeVersor(vA, rMVI, fPrec);
	}
	else
	{
		return AnalyzeBlade(vA, rMVI, bOPNS, fPrec);
	}

	return false;
}



////////////////////////////////////////////////////////////////////////////////////
// Make Conic Degenerate
//
// Result is returned in vB and fDist. 
// fDist gives distance from given conic to degenerate conic.
// If function returns false, MV could not be made into degenerate conic

template<class CType>
bool El2GA<CType>::MakeConicDegenerate(const MultiV<CType>& vA, MultiV<CType>& vB, CType &fDist,
										bool bOPNS, CType fPrec)
{
	MultiV<CType> vX(vA);
	MemObj<MultiV<CType> > mvList;
	CType fZeroMin, fZeroMax, fUnityMin, fUnityMax;

	vB.SetStyle(vA.GetStyle());

	Mem<uint> mGList;
	if (!GradeList(vX, mGList, fPrec)) return false;
	if (mGList.Count() != 1) return false;
	int iGrade = mGList[0];

	if (fPrec <= CType(0))
		Tiny(fPrec);

	if (bOPNS)
		iGrade = 6 - iGrade;

	if (iGrade != 1)
		return false;

	fZeroMin = CType(0) - fPrec;
	fZeroMax = CType(0) + fPrec;

	fUnityMin = CType(1) - fPrec;
	fUnityMax = CType(1) + fPrec;

	if (bOPNS)
		vX = *vX;

	Matrix<CType> xA(2,2), xU, xV, xD, xS;
	CType dS1, dS2, dL1, dL2, dR, dFac;

	dFac = CType(sqrt(2.0));

	// Initialize matrix with components of vector
	xA(1,1) = dFac * vX[iE4];
	xA(1,2) = vX[iE6];
	xA(2,1) = vX[iE6];
	xA(2,2) = dFac * vX[iE5];
	//printMat(xA, "A", 0);

	// Now use SVD to obtain eigenvectors and eigenvalues squared.
	xA.SVD(xU, xD, xV);
	//printMat(xD, "D", 0);

	// xV now contains the eigenvectors as column vectors
	// Extend xV to make it a 3x3 matrix
	xV.Resize(3, 3);
	xV(3,3) = CType(1);

	//printMat(xV, "V", 0);

	// Extend xA to include all components of vector
	xA.Resize(3, 3);
	xA(1,3) = xA(3,1) = vX[iE1];
	xA(2,3) = xA(3,2) = vX[iE2];
	xA(3,3) = dFac * vX[iE3];

	//printMat(xA, "Ah", 0);

	// Now we will obtain the translation vector
	// and the eigenvalues squared.
	xS = ~xV * (xA * xV);
	//xS = ~xU * (xA * xV);
	//printMat(xS, "Sh", 0);
	//printf("\n\n");

	// Extract intermediate components
	dL1 = xS(1,1);
	dL2 = xS(2,2);
	dR = xS(3,3);

	// This is a little hack, since xV does not give the eigenvectors
	// of xA for a line crossing. I havn't really understodd the problem here.
	// Probably need to use a proper eigenvector evaluation instead of SVD.

	if ((dL1 >= fZeroMin && dL1 <= fZeroMax) &&
		(dL2 >= fZeroMin && dL2 <= fZeroMax) &&
		(dR >= fZeroMin && dR <= fZeroMax) &&
		(xS(1,2) <= fZeroMin || xS(1,2) >= fZeroMax))
	{
		vB = vA;
		fDist = CType(0);
	}
	else
	{
		if (dL1 >= fZeroMin && dL1 <= fZeroMax)
		{
			dS1 = CType(0);
			dL1 = CType(0);
		}
		else
		{
			dS1 = xS(1,3) / dL1;
			dR -= dS1*dS1 * dL1;
		}

		if (dL2 >= fZeroMin && dL2 <= fZeroMax)
		{
			dS2 = CType(0);
			dL2 = CType(0);
		}
		else
		{
			dS2 = xS(2,3) / dL2;
			dR -= dS2*dS2 * dL2;
		}

		fDist = Mag(dR);

		xS(3,3) = dS1*dS1*dL1 + dS2*dS2*dL2;
		xA = xV * (xS * ~xV);

		vX[iE1] = xA(1,3);
		vX[iE2] = xA(2,3);
		vX[iE3] = xA(3,3) / dFac;

		vX[iE4] = xA(1,1) / dFac;
		vX[iE5] = xA(2,2) / dFac;
		vX[iE6] = xA(1,2);

		vB = vX;
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////////
// Analyse Blade.
// Result is returned in rMVI.
// If function returns false, MV could not be analyzed.

template<class CType>
bool El2GA<CType>::AnalyzeBlade(const MultiV<CType>& vA, CMVInfo<CType>& rMVI, 
								bool bOPNS, CType fPrec)
{

	MultiV<CType> vX(vA);
	MemObj<MultiV<CType> > mvList;
	CType fZeroMin, fZeroMax, fUnityMin, fUnityMax;
	
	Mem<uint> mGList;
	if (!GradeList(vX, mGList, fPrec)) return false;
	if (mGList.Count() != 1) return false;
	int iGrade = mGList[0];

	if (iGrade < 0 || iGrade > 6)
		return false;
	
	if (fPrec <= CType(0))
		Tiny(fPrec);

	if (bOPNS)
		iGrade = 6 - iGrade;

	fZeroMin = CType(0) - fPrec;
	fZeroMax = CType(0) + fPrec;

	fUnityMin = CType(1) - fPrec;
	fUnityMax = CType(1) + fPrec;

	CType dMag = vX.Mag2();
	if (!::IsZero(dMag, fPrec))
		vX /= dMag;

	if (iGrade == 0)
	{
		// Space
		// Type = GA_SPACE
		// Val[0] = scale

		rMVI.m_eType = GA_SPACE;
		rMVI.m_mVal.Set(1);

		if (bOPNS)
			rMVI.m_mVal[0] = vX[iI];
		else
			rMVI.m_mVal[0] = vX[iSC];
	}
	else if (iGrade == 1)
	{
		if (bOPNS)
			vX = *vX;

		Matrix<CType> xA(2,2), xU, xV, xD, xS, xREVal, xIEVal, xREVec, xIEVec;
		CType dS1, dS2, dL1, dL2, dR, dRS, pdX[3], dVal, dFac;

		dFac = CType(sqrt(2.0));

		// Initialize matrix with components of vector
		xA(1,1) = dFac * vX[iE4];
		xA(1,2) = vX[iE6];
		xA(2,1) = vX[iE6];
		xA(2,2) = dFac * vX[iE5];
		//printMat(xA, "A", 0);

		// Now use SVD to obtain eigenvectors and eigenvalues squared.
		xA.SVD(xU, xD, xV);
		//Eigen(xA, xREVal, xIEVal, xREVec, xIEVec);
		//xV = xREVec;
		//xU = xREVec;

		//printMat(xD, "D", 0);

		// xV now contains the eigenvectors as column vectors
		// Extend xV to make it a 3x3 matrix
		xV.Resize(3, 3);
		xV(3,3) = CType(1);
		xU.Resize(3, 3);
		xU(3,3) = CType(1);

		//printMat(xV, "V", 0);

		// Extend xA to include all components of vector
		xA.Resize(3, 3);
		xA(1,3) = xA(3,1) = vX[iE1];
		xA(2,3) = xA(3,2) = vX[iE2];
		xA(3,3) = dFac * vX[iE3];

		//printMat(xA, "Ah", 0);

		// Now we will obtain the translation vector
		// and the eigenvalues squared.
		xS = ~xV * (xA * xV);
		//xS = ~xU * (xA * xV);
		//printMat(xS, "Sh", 0);
		//printf("\n\n");

		// Extract intermediate components
		dL1 = xS(1,1);
		dL2 = xS(2,2);
		dR = xS(3,3);

		// This is a little hack, since xV does not give the eigenvectors
		// of xA for a line crossing. I havn't really understood the problem here.
		// Probably need to use a proper eigenvector evaluation instead of SVD.
		
		if ((dL1 >= fZeroMin && dL1 <= fZeroMax) &&
			(dL2 >= fZeroMin && dL2 <= fZeroMax) &&
			(dR >= fZeroMin && dR <= fZeroMax) &&
			(xS(1,2) <= fZeroMin || xS(1,2) >= fZeroMax))
		{
			dL1 = xS(1,2);
			dL2 = -dL1;
			dS1 = xS(2,3) / dL1;
			dS2 = xS(1,3) / dL1;
			dR = CType(0);
		}
		else
		{
			if (dL1 >= fZeroMin && dL1 <= fZeroMax)
			{
				dS1 = CType(0);
				dL1 = CType(0);
			}
			else
			{
				dS1 = xS(1,3) / dL1;
				dR -= dS1*dS1 * dL1;
			}

			if (dL2 >= fZeroMin && dL2 <= fZeroMax)
			{
				dS2 = CType(0);
				dL2 = CType(0);
			}
			else
			{
				dS2 = xS(2,3) / dL2;
				dR -= dS2*dS2 * dL2;
			}

			if (dR >= fZeroMin && dR <= fZeroMax)
				dR = CType(0);
		}

//		if (dR >= -1e-3 && dR <= 1e-3)
//			dR = CType(0);

/*
		printf("L1 = %lg\n", dL1);
		printf("L2 = %lg\n", dL2);
		printf("S1 = %lg\n", dS1);
		printf("S2 = %lg\n", dS2);
		printf("R = %lg\n\n", dR);
*/
		pdX[2] = CType(0);

		if (dL1 > fZeroMax && dL2 > fZeroMax)
		{
			if (!::IsZero(dR, fPrec))
			{
				// Set Type
				rMVI.m_eType = GA_ELLIPSE;
				rMVI.m_mVal.Set(1);
				rMVI.m_mVex.Set(3);

				// If dR is less than zero, then this is a real ellipse.
				// Otherwise it is imaginary. This is indicated by +1 and -1;
				if (dR < fZeroMin)
					rMVI.m_mVal[0] = CType(1);
				else
					rMVI.m_mVal[0] = CType(-1);

				dRS = CType(sqrt(Mag(dR)));

				// First axis
				dVal = dRS / CType(sqrt(dL1));
				pdX[0] = xV(1,1) * dVal;
				pdX[1] = xV(2,1) * dVal;
				rMVI.m_mVex[0] = pdX;

				// Second axis
				dVal = dRS / CType(sqrt(dL2));
				pdX[0] = xV(1,2) * dVal;
				pdX[1] = xV(2,2) * dVal;
				rMVI.m_mVex[1] = pdX;

				// Translation from center
				pdX[0] = -dS1 * xV(1,1) - dS2 * xV(1,2);
				pdX[1] = -dS1 * xV(2,1) - dS2 * xV(2,2);
				rMVI.m_mVex[2] = pdX;
			}
			else
			{
				// Type = GA_POINT
				// Vex[0] = point
				rMVI.m_eType = GA_POINT;
				rMVI.m_mVex.Set(1);

				pdX[0] = -dS1 * xV(1,1) - dS2 * xV(1,2);
				pdX[1] = -dS1 * xV(2,1) - dS2 * xV(2,2);
				rMVI.m_mVex[0] = pdX;
			}
		}
		else if (dL1 < fZeroMin && dL2 < fZeroMin)
		{
			if (!::IsZero(dR, fPrec))
			{
				// Set Type
				rMVI.m_eType = GA_ELLIPSE;
				rMVI.m_mVal.Set(1);
				rMVI.m_mVex.Set(3);

				// If dR is GREATER than zero, then this is a real ellipse.
				// Otherwise it is imaginary. This is indicated by +1 and -1;
				if (dR > fZeroMax)
					rMVI.m_mVal[0] = CType(1);
				else
					rMVI.m_mVal[0] = CType(-1);

				dRS = CType(sqrt(Mag(dR)));

				// First axis
				dVal = dRS / CType(sqrt(-dL1));
				pdX[0] = xV(1,1) * dVal;
				pdX[1] = xV(2,1) * dVal;
				rMVI.m_mVex[0] = pdX;

				// Second axis
				dVal = dRS / CType(sqrt(-dL2));
				pdX[0] = xV(1,2) * dVal;
				pdX[1] = xV(2,2) * dVal;
				rMVI.m_mVex[1] = pdX;

				// Translation from center
				pdX[0] = -dS1 * xV(1,1) - dS2 * xV(1,2);
				pdX[1] = -dS1 * xV(2,1) - dS2 * xV(2,2);
				rMVI.m_mVex[2] = pdX;
			}
			else
			{
				// Type = GA_POINT
				// Vex[0] = point
				rMVI.m_eType = GA_POINT;
				rMVI.m_mVex.Set(1);

				pdX[0] = -dS1 * xV(1,1) - dS2 * xV(1,2);
				pdX[1] = -dS1 * xV(2,1) - dS2 * xV(2,2);
				rMVI.m_mVex[0] = pdX;
			}
		}
		else if (dL1 > fZeroMax && dL2 < fZeroMin)
		{
			if (!::IsZero(dR, fPrec))
			{
				bool bChangeBranches = false;

				// Set Type
				rMVI.m_eType = GA_HYPERBOLA;
				rMVI.m_mVex.Set(3);

				if (dR > fZeroMax)
				{
					bChangeBranches = true;
					//printf("Changed branches");
				}

				dRS = CType(sqrt(Mag(dR)));
				//printf("dRS: %lg\n", dRS);

				// First axis
				dVal = dRS / CType(sqrt(dL1));
				pdX[0] = xV(1,1) * dVal;
				pdX[1] = xV(2,1) * dVal;
				rMVI.m_mVex[(bChangeBranches ? 1 : 0)] = pdX;

				// Second axis
				dVal = dRS / CType(sqrt(-dL2));
				pdX[0] = xV(1,2) * dVal;
				pdX[1] = xV(2,2) * dVal;
				rMVI.m_mVex[(bChangeBranches ? 0 : 1)] = pdX;

				// Translation from center
				pdX[0] = -dS1 * xV(1,1) - dS2 * xV(1,2);
				pdX[1] = -dS1 * xV(2,1) - dS2 * xV(2,2);
				rMVI.m_mVex[2] = pdX;
			}
			else
			{
				// Set Type
				// vex[0]: Offset
				// vex[1]: Dir1
				// vex[2]: Dir2
				// val[0]: Mag
				rMVI.m_eType = GA_LINEPAIR;
				rMVI.m_mVex.Set(3);
				rMVI.m_mVal.Set(1);

				rMVI.m_mVal[0] = CType(100);

				// Translation from center
				pdX[0] = -dS1 * xV(1,1) - dS2 * xV(1,2);
				pdX[1] = -dS1 * xV(2,1) - dS2 * xV(2,2);
				rMVI.m_mVex[0] = pdX;

				dVal = CType(sqrt(dL1/-dL2));

				// Check whether both singular values are equal
				if (dVal-1 < fZeroMin || dVal-1 > fZeroMax)
				{
					// If not then...

					// First direction
					pdX[0] = xV(1,1) + xV(1,2) * dVal;
					pdX[1] = xV(2,1) + xV(2,2) * dVal;
					rMVI.m_mVex[1] = pdX;

					// Second direction
					pdX[0] = xV(1,1) - xV(1,2) * dVal;
					pdX[1] = xV(2,1) - xV(2,2) * dVal;
					rMVI.m_mVex[2] = pdX;
				}
				else
				{
					// If singluar values are equal, then initial
					// matrix gives axes of line crossing.
					CType dTheta, dCos, dSin, dScale;

					// Scale should be 1 for unscaled matrix
					dScale = CType(-sqrt(pow(xA(1,1),2) + pow(xA(1,2),2)));
					if ((xA(1,1) < fZeroMin && xA(1,2) > fZeroMax) ||
						(xA(1,1) > fZeroMax && xA(1,2) > fZeroMax))
						dScale = -dScale;
					//printf("(1) Scale = %g\n", dScale);
					
					dTheta = CType(acos(xA(1,1)/dScale)) / CType(2);
					//printf("(1) Theta = %g\n\n", dTheta);

					dCos = CType(cos(dTheta));
					dSin = CType(sin(dTheta));
/*
					if ((xA(1,1) > fZeroMin && xA(1,2) > fZeroMin) ||
						(xA(1,1) < fZeroMin && xA(1,2) < fZeroMin))
						dSin = sin(dTheta);
					else
						dSin = -sin(dTheta);
*/
					// First direction
					pdX[0] = dCos - dSin;
					pdX[1] = dSin + dCos;
					rMVI.m_mVex[1] = pdX;

					// Second direction
					pdX[0] = dCos + dSin;
					pdX[1] = dSin - dCos;
					rMVI.m_mVex[2] = pdX;
				}
			}
		}
		else if (dL2 > fZeroMax && dL1 < fZeroMin)
		{
			if (!::IsZero(dR, fPrec))
			{
				bool bChangeBranches = false;

				// Set Type
				rMVI.m_eType = GA_HYPERBOLA;
				rMVI.m_mVex.Set(3);

				if (dR > fZeroMax)
					bChangeBranches = true;

				dRS = CType(sqrt(Mag(dR)));

				// First axis
				dVal = dRS / CType(sqrt(-dL1));
				pdX[0] = xV(1,1) * dVal;
				pdX[1] = xV(2,1) * dVal;
				rMVI.m_mVex[(bChangeBranches ? 0 : 1)] = pdX;

				// Second axis
				dVal = dRS / CType(sqrt(dL2));
				pdX[0] = xV(1,2) * dVal;
				pdX[1] = xV(2,2) * dVal;
				rMVI.m_mVex[(bChangeBranches ? 1 : 0)] = pdX;

				// Translation from center
				pdX[0] = -dS1 * xV(1,1) - dS2 * xV(1,2);
				pdX[1] = -dS1 * xV(2,1) - dS2 * xV(2,2);
				rMVI.m_mVex[2] = pdX;
			}
			else
			{
				// Set Type
				// vex[0]: Offset
				// vex[1]: Dir1
				// vex[2]: Dir2
				// val[0]: Mag
				rMVI.m_eType = GA_LINEPAIR;
				rMVI.m_mVex.Set(3);
				rMVI.m_mVal.Set(1);

				rMVI.m_mVal[0] = CType(100);

				// Translation from center
				pdX[0] = -dS1 * xV(1,1) - dS2 * xV(1,2);
				pdX[1] = -dS1 * xV(2,1) - dS2 * xV(2,2);
				rMVI.m_mVex[0] = pdX;

				dVal = CType(sqrt(dL2/-dL1));

				// Check whether both singular values are equal
				if (dVal-1 < fZeroMin || dVal-1 > fZeroMax)
				{
					// If not then...

					// First direction
					pdX[0] = xV(1,2) + xV(1,1) * dVal;
					pdX[1] = xV(2,2) + xV(2,1) * dVal;
					rMVI.m_mVex[1] = pdX;

					// Second direction
					pdX[0] = xV(1,2) - xV(1,1) * dVal;
					pdX[1] = xV(2,2) - xV(2,1) * dVal;
					rMVI.m_mVex[2] = pdX;
				}
				else
				{
					// If singluar values are equal, then initial
					// matrix gives axes of line crossing.
					CType dTheta, dCos, dSin, dScale;

					// Scale should be 1 for unscaled matrix
					dScale = CType(-sqrt(pow(xA(1,1),2) + pow(xA(1,2),2)));
					if ((xA(1,1) < fZeroMin && xA(1,2) > fZeroMax) ||
						(xA(1,1) > fZeroMax && xA(1,2) > fZeroMax))
						dScale = -dScale;

					//printf("(2) Scale = %g\n", dScale);

					dTheta = CType(acos(xA(1,1)/dScale)) / CType(2);
					//printf("(2) Theta = %g\n\n", dTheta);

					dCos = CType(cos(dTheta));
					dSin = CType(sin(dTheta));
/*
					if ((xA(1,1) > fZeroMin && xA(1,2) > fZeroMin) ||
						(xA(1,1) < fZeroMin && xA(1,2) < fZeroMin))
						dSin = sin(dTheta);
					else
						dSin = -sin(dTheta);
*/
					// First direction
					pdX[0] = dCos - dSin;
					pdX[1] = dSin + dCos;
					rMVI.m_mVex[1] = pdX;

					// Second direction
					pdX[0] = dCos + dSin;
					pdX[1] = dSin - dCos;
					rMVI.m_mVex[2] = pdX;
				}
			}
		}
		else if ((dL1 >= fZeroMin && dL1 <= fZeroMax) && 
				 (dL2 < fZeroMin || dL2 > fZeroMax))
		{
			CType dFac;
			dFac = -dR / dL2;

			// Set Type
			// val[0]: separation magnitude. if < 0 then imaginary.
			// vex[0]: separation direction.
			// vex[1]: line direction.
			// vex[2]: offset from origin.
			rMVI.m_eType = GA_PARLINEPAIR;
			rMVI.m_mVal.Set(1);
			rMVI.m_mVex.Set(3);

			// If dFac is greater or equal to zero, then this is a real object.
			// Otherwise it is imaginary. This is indicated by +1 and -1;
			if (dFac > fZeroMin)
				rMVI.m_mVal[0] = CType(1);
			else
				rMVI.m_mVal[0] = CType(-1);

			dFac = CType(sqrt(Mag(dFac)));
			if (dFac >= fZeroMin && dFac <= fZeroMax)
				dFac = CType(0);

			rMVI.m_mVal[0] *= dFac;

			// Line direction vector
			pdX[0] = xV(1,1);
			pdX[1] = xV(2,1);
			rMVI.m_mVex[1] = pdX;

			// Line separation vector
			pdX[0] = xV(1,2);
			pdX[1] = xV(2,2);
			rMVI.m_mVex[0] = pdX;

			// Translation from center
			pdX[0] = -dS1 * xV(1,1) - dS2 * xV(1,2);
			pdX[1] = -dS1 * xV(2,1) - dS2 * xV(2,2);
			rMVI.m_mVex[2] = pdX;			
		}
		else if ((dL2 >= fZeroMin && dL2 <= fZeroMax) && 
				 (dL1 < fZeroMin || dL1 > fZeroMax))
		{
			CType dFac;
			dFac = -dR / dL1;

			// Set Type
			// val[0]: separation magnitude. if < 0 then imaginary.
			// vex[0]: separation direction.
			// vex[1]: line direction.
			// vex[2]: offset from origin.
			rMVI.m_eType = GA_PARLINEPAIR;
			rMVI.m_mVal.Set(1);
			rMVI.m_mVex.Set(3);

			// If dFac is greater or equal to zero, then this is a real object.
			// Otherwise it is imaginary. This is indicated by +1 and -1;
			if (dFac > fZeroMin)
				rMVI.m_mVal[0] = CType(1);
			else
				rMVI.m_mVal[0] = CType(-1);

			dFac = CType(sqrt(Mag(dFac)));
			if (dFac >= fZeroMin && dFac <= fZeroMax)
				dFac = CType(0);

			rMVI.m_mVal[0] *= dFac;

			// Line direction vector
			pdX[0] = xV(1,2);
			pdX[1] = xV(2,2);
			rMVI.m_mVex[1] = pdX;

			// Line separation vector
			pdX[0] = xV(1,1);
			pdX[1] = xV(2,1);
			rMVI.m_mVex[0] = pdX;

			// Translation from center
			pdX[0] = -dS1 * xV(1,1) - dS2 * xV(1,2);
			pdX[1] = -dS1 * xV(2,1) - dS2 * xV(2,2);
			rMVI.m_mVex[2] = pdX;			
		}
		else if ((dL1 >= fZeroMin && dL1 <= fZeroMax) && 
				 (dL2 >= fZeroMin && dL2 <= fZeroMax))
		{
			// Set Type
			rMVI.m_eType = GA_LINE;
			rMVI.m_mVal.Set(1);
			rMVI.m_mVex.Set(2);

			CType dFac, dMag2, dMag;
			dS1 = xS(1,3);
			dS2 = xS(2,3);

			dMag2 = dS1*dS1 + dS2*dS2;
			dMag = CType(sqrt(dMag2));
			dFac = -CType(0.5) * dR / dMag2;

			// Always scale line to size 100
			rMVI.m_mVal[0] = CType(100);

			// Plumb vector vector 
			pdX[0] = dFac * dS1;
			pdX[1] = dFac * dS2;
			rMVI.m_mVex[0] = pdX;

			// Line direction vector
			pdX[0] = -dS2 / dMag;
			pdX[1] = dS1 / dMag;
			rMVI.m_mVex[1] = pdX;
		}
	}
	//////////////////////////////////////////////////////
	/// Intersection of two conics
	else if (iGrade == 2)
	{
		if (bOPNS)
			vX = *vX;

		MultiV<CType> vA;
		MemObj<MultiV<CType> > mvList;
		MemObj<CPointND<CType,2> > mPointList;
		//CType fMag;

		FactorBlade(vX, mvList, true, &vA, (CType *) 0, fPrec);
		//fMag = CType(sqrt(Scalar(vA * ~vA)));

		// Intersect conics
		if (!IntersectConics(mvList[0], mvList[1], mPointList, CType(1e-7)/*fPrec*/))
			return false;

		int i, iCount = mPointList.Count();

		if (!iCount)
			return true;	// Object represents nothing real.

		rMVI.m_eType = GA_POINTSET;
		rMVI.m_mVex.Set(iCount);
		
		for (i = 0; i < iCount; i++)
		{
			rMVI.m_mVex[i][0] = mPointList[i][0];
			rMVI.m_mVex[i][1] = mPointList[i][1];
			rMVI.m_mVex[i][2] = CType(0);
		}
	}
	//////////////////////////////////////////////////////
	/// Intersection of five conics. Can at most be a point.
	else if (iGrade == 5)
	{
		if (!bOPNS)
			vX = *vX;

		bool bIsDir = false;
		CType dVal, pdX[3], dR, dR2;
		pdX[2] = CType(0);

		if ((dR = vX[iE3]) >= fZeroMin && dR <= fZeroMax)
		{
			bIsDir = true;
			dR = CType(1);
			dR2 = CType(1);
		}
		else
		{
			bIsDir = false;
			vX /= CType(m_dSqrt2 * dR);
		}

		// Check whether components can represent point
		dVal = m_dSqrt2*vX[iE4] - vX[iE1]*vX[iE1];
		if (dVal < fZeroMin || dVal > fZeroMax)
			return true;

		dVal = m_dSqrt2*vX[iE5] - vX[iE2]*vX[iE2];
		if (dVal < fZeroMin || dVal > fZeroMax)
			return true;

		dVal = vX[iE6] - vX[iE1]*vX[iE2];
		if (dVal < fZeroMin || dVal > fZeroMax)
			return true;

		if (bIsDir)
		{
			// Direction Vector
			// Type = GA_DIR1D
			// Vex[0] = vector

			rMVI.m_eType = GA_DIR1D;
			rMVI.m_mVex.Set(1);
			rMVI.m_mVal.Set(1);

			pdX[0] = vX[iE1];
			pdX[1] = vX[iE2];

			rMVI.m_mVex[0] = pdX;
			rMVI.m_mVal[0] = CType(sqrt(pdX[0]*pdX[0] + pdX[1]*pdX[1]));
		}
		else
		{
			// Type = GA_POINT
			// Vex[0] = point
			rMVI.m_eType = GA_POINT;
			rMVI.m_mVex.Set(1);

			pdX[0] = vX[iE1];
			pdX[1] = vX[iE2];
			rMVI.m_mVex[0] = pdX;
		}
	}
	else if (iGrade == 6)
	{
		// Scalar
		// Type = GA_SCALAR
		// Val[0] = scalar

		rMVI.m_eType = GA_SCALAR;
		rMVI.m_mVal.Set(1);

		if (bOPNS)
			rMVI.m_mVal[0] = vX[iSC];
		else
			rMVI.m_mVal[0] = vX[iI];
	}


	return true;
}


////////////////////////////////////////////////////////////////////////////////////
// Analyse Rotor
// Result is returned in rMVI.
// If function returns false, MV could not be analyzed.
//
// Type = GA_ROTOR
// Vex[0] = Rotation axis
// Val[0] = Rotation angle

template<class CType>
bool El2GA<CType>::AnalyzeVersor(const MultiV<CType>& vX, CMVInfo<CType>& rMVI, CType fPrec)
{
/*
	MultiV<CType> vU, vA;
	MemObj<MultiV<CType> > mvList;

	rMVI.m_eType = GA_ROTOR;
	rMVI.m_mVex.Set(1);
	rMVI.m_mVal.Set(1);

	// Get Bivector Part
	vU = vX(2);

	// Rotation axis
	vA = *( -vU / CType(sqrt(Scalar(vU * (~vU)))) );

	rMVI.m_mVex[0] = &vA[1];
	rMVI.m_mVal[0] = CType(2.0 * acos(Scalar(vX)));
*/	
	return true;
}

#endif
