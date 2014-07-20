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


// E3GA Function and Class Definitions

#include"e3ga.h"
#include"e3gainst.h"

//#include "xmalib.h"


////////////////////////////////////////////////////////////////////////////////////
/// Static Variables

template<class CType> Blade<CType> E3GA<CType>::bSC;
template<class CType> Blade<CType> E3GA<CType>::bE1;
template<class CType> Blade<CType> E3GA<CType>::bE2;
template<class CType> Blade<CType> E3GA<CType>::bE3;
template<class CType> Blade<CType> E3GA<CType>::bL1;
template<class CType> Blade<CType> E3GA<CType>::bL2;
template<class CType> Blade<CType> E3GA<CType>::bL3;
template<class CType> Blade<CType> E3GA<CType>::bI;

template<class CType> MultiV<CType> E3GA<CType>::vSC;
template<class CType> MultiV<CType> E3GA<CType>::vE1;
template<class CType> MultiV<CType> E3GA<CType>::vE2;
template<class CType> MultiV<CType> E3GA<CType>::vE3;
template<class CType> MultiV<CType> E3GA<CType>::vL1;
template<class CType> MultiV<CType> E3GA<CType>::vL2;
template<class CType> MultiV<CType> E3GA<CType>::vL3;
template<class CType> MultiV<CType> E3GA<CType>::vI;

template<class CType> MultiV<CType> E3GA<CType>::vZero;
template<class CType> RingBuf<uint> E3GA<CType>::BLADE;

template<class CType> MultiVStyle<CType> E3GA<CType>::m_MVStyle;

// E3GA needs initialisation
template<class CType> bool E3GA<CType>::m_bInit = true;

template<class CType> const uint E3GA<CType>::iSC = 0;
template<class CType> const uint E3GA<CType>::iE1 = 1;
template<class CType> const uint E3GA<CType>::iE2 = 2;
template<class CType> const uint E3GA<CType>::iE3 = 3;
template<class CType> const uint E3GA<CType>::iL1 = 4;
template<class CType> const uint E3GA<CType>::iL2 = 5;
template<class CType> const uint E3GA<CType>::iL3 = 6;
template<class CType> const uint E3GA<CType>::iI = 7;


#include "E3GATables.h"

////////////////////////////////////////////////////////////////////////////////////
/// Functions

template<class CType>
E3GA<CType>::E3GA() : Base<CType>(3,1,"e",ID_E3GA)
{
	if (m_bInit) 
	{
		InitVars();
		m_bInit = false;
	}
}


// E3GA Signature
template<class CType>
inline CType E3GA<CType>::signature(uint i) 
{ 
	return (CType(1)); 
}


////////////////////////////////////////////////////////////////////////////////////
/// Init Variables

template<class CType>
void E3GA<CType>::InitVars()
{
	BladeList<CType> E3GAStyle(*this);
	
	BLADE.Reset(1);
	bSC.Reset(*this, CType(1));
	bE1.Reset(*this,CType(1),1,(BLADE<<1),"e1");
	bE2.Reset(*this,CType(1),1,(BLADE<<2),"e2");
	bE3.Reset(*this,CType(1),1,(BLADE<<3),"e3");
	
	BLADE.Reset(2);
	bL1.Reset(*this,CType(1),2,(BLADE<<2<<3),"e23");
	bL2.Reset(*this,CType(1),2,(BLADE<<3<<1),"e31");
	bL3.Reset(*this,CType(1),2,(BLADE<<1<<2),"e12");
	
	BLADE.Reset(3);
	bI.Reset(*this,CType(1),3,(BLADE<<1<<2<<3),"I");
	
	E3GAStyle <<bSC<<bE1<<bE2<<bE3<<bL1<<bL2<<bL3<<bI;

	// Initialises multiplication tables
	m_MVStyle.Init(E3GAStyle, piGrades, piGPTable, piIPTable, 
					piOPTable, piVITable, piDualTable); 
	
	vZero.SetStyle(m_MVStyle);

	vSC.SetStyle(m_MVStyle); vSC[iSC] = 1;
	vE1.SetStyle(m_MVStyle); vE1[iE1] = 1;
	vE2.SetStyle(m_MVStyle); vE2[iE2] = 1;
	vE3.SetStyle(m_MVStyle); vE3[iE3] = 1;
	vL1.SetStyle(m_MVStyle); vL1[iL1] = 1;
	vL2.SetStyle(m_MVStyle); vL2[iL2] = 1;
	vL3.SetStyle(m_MVStyle); vL3[iL3] = 1;
	vI.SetStyle(m_MVStyle); vI[iI] = 1;

}

// Additional Functions -----------------------------


////////////////////////////////////////////////////////////////////////////////////
// Make 3-D Vector

template<class CType>
MultiV<CType> E3GA<CType>::Vec(const CType &a1, const CType &a2, const CType &a3)
{
	CType a[4];
	
	a[0] = a1;
	a[1] = a2;
	a[2] = a3;
	
	return MakeVector(m_MVStyle,1,a);
}

////////////////////////////////////////////////////////////////////////////////////
// Make 3-D Vector

template<class CType>
MultiV<CType> E3GA<CType>::Vec(const CType *pA)
{
	return MakeVector(m_MVStyle,1,pA);
}



////////////////////////////////////////////////////////////////////////////////////
// Make 3-D Vector from spherical coordinates
// 0<=r<inf, 0 <= theta <= pi, -pi <= phi <= pi

template<class CType>
MultiV<CType> E3GA<CType>::SVec(const CType &r, const CType &theta, const CType &phi)
{
	CType a[3];

	a[0] = r * CType(sin(theta)) * CType(cos(phi));
	a[1] = r * CType(sin(theta)) * CType(sin(phi));
	a[2] = r * CType(cos(theta));

	return MakeVector(m_MVStyle, 1, a);
}


////////////////////////////////////////////////////////////////////////////////////
// Make 3-D Vector from spherical coordinates
// 0<=r<inf, 0 <= theta <= pi, -pi <= phi <= pi

template<class CType>
MultiV<CType> E3GA<CType>::SVec(const CType *pA)
{
	CType a[3];

	a[0] = pA[0] * CType(sin(pA[1])) * CType(cos(pA[2]));
	a[1] = pA[0] * CType(sin(pA[1])) * CType(sin(pA[2]));
	a[2] = pA[0] * CType(cos(pA[1]));

	return MakeVector(m_MVStyle, 1, a);
}


////////////////////////////////////////////////////////////////////////////////////
// Make Biector

template<class CType>
MultiV<CType> E3GA<CType>::Bivec(const CType &a1, const CType &a2, const CType &a3)
{
	CType a[4];
	
	a[0] = a1;
	a[1] = a2;
	a[2] = a3;
	
	return MakeVector(m_MVStyle,2,a);
}

////////////////////////////////////////////////////////////////////////////////////
// Make Biector

template<class CType>
MultiV<CType> E3GA<CType>::Bivec(const CType *pA)
{
	return MakeVector(m_MVStyle,2,pA);
}

////////////////////////////////////////////////////////////////////////////////////
// Make Rotor
// a is rotation axis, rad is rotation angle

template<class CType>
MultiV<CType> E3GA<CType>::Rotor(const CType &a1, const CType &a2, const CType &a3, const CType &rad)
{
	MultiV<CType> vA(m_MVStyle);

	vA[iE1] = a1;
	vA[iE2] = a2;
	vA[iE3] = a3;

	vA /= CType(sqrt(a1*a1 + a2*a2 + a3*a3));
	vA &= vI; // Rotation plane is U = x I, and x = U I^(-1)

	vA = CType(cos(rad/2.0)) - (CType(sin(rad/2.0)) & vA);

	vA.TinyToZero();

	return vA;
}


////////////////////////////////////////////////////////////////////////////////////
// Make Rotor
// vA is rotation axis, rad is rotation angle

template<class CType>
MultiV<CType> E3GA<CType>::Rotor(const MultiV<CType> &vX, const CType &rad, bool bAxis)

{
	MultiV<CType> vA(m_MVStyle);
	CType fVal;
		
	if (bAxis)
	{
		// Assume vX is rotation axis
		vA = vX(1);

		fVal = CType(sqrt(Scalar(vA * vA)));
		if (fVal != CType(0))
		{
			vA /= fVal;
			vA &= vI; // Rotation plane is U = x I, and x = U I^(-1)
		}
	}
	else
	{
		// Assume vX is rotation plane
		vA = vX(2);
	
		fVal = CType(sqrt(Scalar(vA * (~vA))));

		if (fVal != CType(0))
			vA /= fVal;
	}

	if (fVal != CType(0))
	{
		vA = CType(cos(rad/2.0)) - (CType(sin(rad/2.0)) & vA);
		vA.TinyToZero();
	}
	else
	{
		vA = CType(1);
	}

	return vA;
}


////////////////////////////////////////////////////////////////////////////////////
// Analyse Multivector. 
// Result is returned in rMVI.
// If function returns false, MV could not be analyzed.

template<class CType>
bool E3GA<CType>::AnalyzeMV(const MultiV<CType>& vX, CMVInfo<CType>& rMVI, bool bOPNS, CType fPrec)
{
	MultiV<CType> vA(vX);
	Mem<uint> mGList;
	int iGrades;

	rMVI.Reset();

	if (vA == vZero)
	{
		// Scalar
		// Type = GA_SCALAR
		// Val[0] = scalar

		rMVI.m_eType = GA_SCALAR;
		rMVI.m_mVal.Set(1);
		rMVI.m_mVal[0] = CType(0);
		return true;
	}

	if (!GradeList(vA, mGList, fPrec)) return false;
	if ((iGrades = mGList.Count()) == 0) return false;

	if (iGrades == 2 && mGList[0] == 2 && mGList[1] == 0)
	{
		return AnalyzeRotor(vA, rMVI, fPrec);
	}
	else
	{
		return AnalyzeBlade(vA, rMVI, bOPNS, fPrec);
	}

	return false;
}


////////////////////////////////////////////////////////////////////////////////////
// Analyse Blade.
// Result is returned in rMVI.
// If function returns false, MV could not be analyzed.

template<class CType>
bool E3GA<CType>::AnalyzeBlade(const MultiV<CType>& vY, CMVInfo<CType>& rMVI, bool bOPNS,
							   CType fPrec)
{
	CType pfVec[3][3];
	MultiV<CType> vA, vX, vN;
	MemObj<MultiV<CType> > mvList;

	CType fMag;

	if (!bOPNS)
		vX = -*vY;
	else
		vX = vY;

	FactorBlade(vX, mvList, true, &vA, (CType *) 0, fPrec);
	fMag = CType(sqrt(Scalar(vA * ~vA)));

	int i, n = mvList.Count();

	if (n < 0 || n > 3)
		return false;

	if (n > 1)
	{
		for(i=0;i<n;i++)
		{
			GetVector(mvList[i], 1, pfVec[i]);
		}
	}
	else if (n == 1)
	{
		GetVector(vA, 1, pfVec[0]);
	}

	if (n == 0)
	{
		// Scalar
		// Type = GA_SCALAR
		// Val[0] = scalar

		rMVI.m_eType = GA_SCALAR;
		rMVI.m_mVal.Set(1);
		rMVI.m_mVal[0] = vX[0];
	}
	else if (n == 1)
	{
		// Point
		// Type = GA_POINT
		// 1. Vex = point

		rMVI.m_eType = GA_POINT;
		rMVI.m_mVex.Set(1);
		rMVI.m_mVex[0] = pfVec[0];
	}
	else if (n == 2)
	{
		// Plane through origin
		// Type = GA_PLANE
		// Vex[0] = 1st direction (normalized)
		// Vex[1] = 2nd direction (normalized)
		// Val[0] = Magnitude

		rMVI.m_eType = GA_PLANE;
		rMVI.m_mVex.Set(2);
		rMVI.m_mVal.Set(1);

		rMVI.m_mVal[0] = fMag;

		rMVI.m_mVex[0] = pfVec[0];
		rMVI.m_mVex[1] = pfVec[1];
	}
	else if (n == 3)
	{
		// Space
		// Type = GA_SPACE
		// Val[0] = Magnitude

		rMVI.m_eType = GA_SPACE;
		rMVI.m_mVal.Set(1);
		rMVI.m_mVal[0] = fMag;
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
bool E3GA<CType>::AnalyzeRotor(const MultiV<CType>& vX, CMVInfo<CType>& rMVI, CType fPrec)
{
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
	
	return true;
}

