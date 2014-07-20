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


// PGA Function and Class Definitions

#include"pga.h"
#include"pgainst.h"

#include "xmalib.h"


////////////////////////////////////////////////////////////////////////////////////
/// Static Variables

template<class CType> Blade<CType> PGA<CType>::bSC;
template<class CType> Blade<CType> PGA<CType>::bE1;
template<class CType> Blade<CType> PGA<CType>::bE2;
template<class CType> Blade<CType> PGA<CType>::bE3;
template<class CType> Blade<CType> PGA<CType>::bE4;
template<class CType> Blade<CType> PGA<CType>::bL1;
template<class CType> Blade<CType> PGA<CType>::bL2;
template<class CType> Blade<CType> PGA<CType>::bL3;
template<class CType> Blade<CType> PGA<CType>::bL1I;
template<class CType> Blade<CType> PGA<CType>::bL2I;
template<class CType> Blade<CType> PGA<CType>::bL3I;
template<class CType> Blade<CType> PGA<CType>::bE1I;
template<class CType> Blade<CType> PGA<CType>::bE2I;
template<class CType> Blade<CType> PGA<CType>::bE3I;
template<class CType> Blade<CType> PGA<CType>::bE4I;
template<class CType> Blade<CType> PGA<CType>::bI;

template<class CType> MultiV<CType> PGA<CType>::vSC;
template<class CType> MultiV<CType> PGA<CType>::vE1;
template<class CType> MultiV<CType> PGA<CType>::vE2;
template<class CType> MultiV<CType> PGA<CType>::vE3;
template<class CType> MultiV<CType> PGA<CType>::vE4;
template<class CType> MultiV<CType> PGA<CType>::vL1;
template<class CType> MultiV<CType> PGA<CType>::vL2;
template<class CType> MultiV<CType> PGA<CType>::vL3;
template<class CType> MultiV<CType> PGA<CType>::vL1I;
template<class CType> MultiV<CType> PGA<CType>::vL2I;
template<class CType> MultiV<CType> PGA<CType>::vL3I;
template<class CType> MultiV<CType> PGA<CType>::vE1I;
template<class CType> MultiV<CType> PGA<CType>::vE2I;
template<class CType> MultiV<CType> PGA<CType>::vE3I;
template<class CType> MultiV<CType> PGA<CType>::vE4I;
template<class CType> MultiV<CType> PGA<CType>::vI;

template<class CType> MultiV<CType> PGA<CType>::vZero;
template<class CType> RingBuf<uint> PGA<CType>::BLADE;

template<class CType> MultiVStyle<CType> PGA<CType>::m_MVStyle;

// PGA needs initialisation
template<class CType> bool PGA<CType>::m_bInit = true;

template<class CType> const uint PGA<CType>::iSC = 0;
template<class CType> const uint PGA<CType>::iE1 = 1;
template<class CType> const uint PGA<CType>::iE2 = 2;
template<class CType> const uint PGA<CType>::iE3 = 3;
template<class CType> const uint PGA<CType>::iE4 = 4;
template<class CType> const uint PGA<CType>::iL1 = 5;
template<class CType> const uint PGA<CType>::iL2 = 6;
template<class CType> const uint PGA<CType>::iL3 = 7;
template<class CType> const uint PGA<CType>::iL1I = 8;
template<class CType> const uint PGA<CType>::iL2I = 9;
template<class CType> const uint PGA<CType>::iL3I = 10;
template<class CType> const uint PGA<CType>::iE1I = 11;
template<class CType> const uint PGA<CType>::iE2I = 12;
template<class CType> const uint PGA<CType>::iE3I = 13;
template<class CType> const uint PGA<CType>::iE4I = 14;
template<class CType> const uint PGA<CType>::iI = 15;

#include "PGATables.h"

////////////////////////////////////////////////////////////////////////////////////
/// Functions

template<class CType>
PGA<CType>::PGA() : Base<CType>(4,1,"e",ID_PGA)
{
	if (m_bInit) 
	{
		InitVars();
		m_bInit = false;
	}
}


// PGA Signature
template<class CType>
inline CType PGA<CType>::signature(uint i) 
{ 
	return (CType(1)); 
}


////////////////////////////////////////////////////////////////////////////////////
/// Init Variables

template<class CType>
void PGA<CType>::InitVars()
{
	BladeList<CType> PGAStyle(*this);
	
	BLADE.Reset(1);
	bSC.Reset(*this, CType(1));
	bE1.Reset(*this,CType(1),1,(BLADE<<1),"e1");
	bE2.Reset(*this,CType(1),1,(BLADE<<2),"e2");
	bE3.Reset(*this,CType(1),1,(BLADE<<3),"e3");
	bE4.Reset(*this,CType(1),1,(BLADE<<4),"e4");
	
	BLADE.Reset(2);
	bL1.Reset(*this,CType(1),2,(BLADE<<2<<3),"e23");
	bL2.Reset(*this,CType(1),2,(BLADE<<3<<1),"e31");
	bL3.Reset(*this,CType(1),2,(BLADE<<1<<2),"e12");
	bL1I.Reset(*this,CType(1),2,(BLADE<<4<<1),"e41");
	bL2I.Reset(*this,CType(1),2,(BLADE<<4<<2),"e42");
	bL3I.Reset(*this,CType(1),2,(BLADE<<4<<3),"e43");
	
	BLADE.Reset(3);
	bE1I.Reset(*this,CType(1),3,(BLADE<<2<<3<<4),"e234");
	bE2I.Reset(*this,CType(1),3,(BLADE<<3<<1<<4),"e314");
	bE3I.Reset(*this,CType(1),3,(BLADE<<1<<2<<4),"e124");
	bE4I.Reset(*this,CType(1),3,(BLADE<<3<<2<<1),"e321");
	
	BLADE.Reset(4);
	bI.Reset(*this,CType(1),4,(BLADE<<1<<2<<3<<4),"I");
	
	PGAStyle <<bSC<<bE1<<bE2<<bE3<<bE4<<bL1<<bL2<<bL3;
	PGAStyle <<bL1I<<bL2I<<bL3I<<bE1I<<bE2I<<bE3I<<bE4I<<bI;

	// Initialises multiplication tables
	m_MVStyle.Init(PGAStyle, piGrades, piGPTable, piIPTable, piOPTable, 
					piVITable, piDualTable); 
	
	vZero.SetStyle(m_MVStyle);

	vSC.SetStyle(m_MVStyle); vSC[iSC] = 1;
	vE1.SetStyle(m_MVStyle); vE1[iE1] = 1;
	vE2.SetStyle(m_MVStyle); vE2[iE2] = 1;
	vE3.SetStyle(m_MVStyle); vE3[iE3] = 1;
	vE4.SetStyle(m_MVStyle); vE4[iE4] = 1;
	vL1.SetStyle(m_MVStyle); vL1[iL1] = 1;
	vL2.SetStyle(m_MVStyle); vL2[iL2] = 1;
	vL3.SetStyle(m_MVStyle); vL3[iL3] = 1;
	vL1I.SetStyle(m_MVStyle); vL1I[iL1I] = 1;
	vL2I.SetStyle(m_MVStyle); vL2I[iL2I] = 1;
	vL3I.SetStyle(m_MVStyle); vL3I[iL3I] = 1;
	vE1I.SetStyle(m_MVStyle); vE1I[iE1I] = 1;
	vE2I.SetStyle(m_MVStyle); vE2I[iE2I] = 1;
	vE3I.SetStyle(m_MVStyle); vE3I[iE3I] = 1;
	vE4I.SetStyle(m_MVStyle); vE4I[iE4I] = 1;
	vI.SetStyle(m_MVStyle); vI[iI] = 1;

}

// Additional Functions -----------------------------


////////////////////////////////////////////////////////////////////////////////////
// Make 3-D Vector

template<class CType>
MultiV<CType> PGA<CType>::Vec(const CType &a1, const CType &a2, const CType &a3)
{
	CType a[4];
	
	a[0] = a1;
	a[1] = a2;
	a[2] = a3;
	a[3] = CType(0);
	
	return MakeVector(m_MVStyle,1,a);
}

////////////////////////////////////////////////////////////////////////////////////
// Make 3-D Vector

template<class CType>
MultiV<CType> PGA<CType>::Vec(const CType *pA)
{
	CType a[4];
	
	a[0] = pA[0];
	a[1] = pA[1];
	a[2] = pA[2];
	a[3] = CType(0);
	
	return MakeVector(m_MVStyle,1,a);
}


////////////////////////////////////////////////////////////////////////////////////
// Make Vector with homogeneous coords

template<class CType>
MultiV<CType> PGA<CType>::hVec(const CType &a1, const CType &a2, const CType &a3, const CType &a4)
{
	CType a[4];
	
	a[0] = a1;
	a[1] = a2;
	a[2] = a3;
	a[3] = a4;
	
	return MakeVector(m_MVStyle,1,a);
}


////////////////////////////////////////////////////////////////////////////////////
// Make Vector with homogeneous coords

template<class CType>
MultiV<CType> PGA<CType>::hVec(const CType *pA)
{
	return MakeVector(m_MVStyle,1,pA);
}

////////////////////////////////////////////////////////////////////////////////////
// Make 3-D Vector from homogeneous Vector
// Using recip vectors here. NOT projective split of Hestenes.
// Ignores all but vector part. If 4th comp. zero, returns zero vector.

template<class CType>
MultiV<CType> PGA<CType>::Vec(const MultiV<CType> &a)
{
	CType c = a[iE4];
	MultiV<CType> b(m_MVStyle);

	if (c != CType(0))
	{
		b[iE1] = a[iE1] / c;
		b[iE2] = a[iE2] / c;
		b[iE3] = a[iE3] / c;
	}
	
	return b;
}


////////////////////////////////////////////////////////////////////////////////////
// Make Vector with homogeneous coords from space Vector

template<class CType>
MultiV<CType> PGA<CType>::hVec(const MultiV<CType> &a, const CType &a4)
{
	MultiV<CType> b(a);
	
	b[iE4] = a4;
	
	return b;
}



////////////////////////////////////////////////////////////////////////////////////
// Make Vector with homogeneous coords from space Vector

template<class CType>
MultiV<CType> PGA<CType>::hVec(const MultiV<CType> &a) // Assumes 4th coord unity
{
	MultiV<CType> b(a);

	b[iE4] = CType(1);

	return b;
}


////////////////////////////////////////////////////////////////////////////////////
// Get Vector Part of Multivector

template<class CType>
MultiV<CType> PGA<CType>::VecPart(const MultiV<CType> &a)
{
	MultiV<CType> b(m_MVStyle);

	b[iE1] = a[iE1];
	b[iE2] = a[iE2];
	b[iE3] = a[iE3];
	b[iE4] = a[iE4];

	return b;
}

////////////////////////////////////////////////////////////////////////////////////
// Get Dual Vector Part of Multivector

template<class CType>
MultiV<CType> PGA<CType>::iVecPart(const MultiV<CType> &a)
{
	MultiV<CType> b(m_MVStyle);

	b[iE1I] = a[iE1I];
	b[iE2I] = a[iE2I];
	b[iE3I] = a[iE3I];
	b[iE4I] = a[iE4I];

	return b;
}



////////////////////////////////////////////////////////////////////////////////////
// Get Line  Part of Multivector

template<class CType>
MultiV<CType> PGA<CType>::LinePart(const MultiV<CType> &a)
{
	MultiV<CType> b(m_MVStyle);

	b[iL1] = a[iL1];
	b[iL2] = a[iL2];
	b[iL3] = a[iL3];
	b[iL1I] = a[iL1I];
	b[iL2I] = a[iL2I];
	b[iL3I] = a[iL3I];

	return b;
}

////////////////////////////////////////////////////////////////////////////////////
// Get Line Part L1, L2, L3 of Multivector

template<class CType>
MultiV<CType> PGA<CType>::LinePart1(const MultiV<CType> &a)
{
	MultiV<CType> b(m_MVStyle);

	b[iL1] = a[iL1];
	b[iL2] = a[iL2];
	b[iL3] = a[iL3];

	return b;
}

////////////////////////////////////////////////////////////////////////////////////
// Get Line Part L1I, L2I, L3I of Multivector

template<class CType>
MultiV<CType> PGA<CType>::LinePart2(const MultiV<CType> &a)
{
	MultiV<CType> b(m_MVStyle);

	b[iL1I] = a[iL1I];
	b[iL2I] = a[iL2I];
	b[iL3I] = a[iL3I];

	return b;
}


////////////////////////////////////////////////////////////////////////////////////
// Get Vector Part

template<class CType>
void PGA<CType>::GetVec(MultiV<CType> &a, CType *vec)
{
	uint i,j;
	
	for(i=iE1,j=0;i<=iE4;i++,j++)
		vec[j] = a[i];
}

////////////////////////////////////////////////////////////////////////////////////
// Get Component Vector Part of Multivector

template<class CType>
CType PGA<CType>::VecComp(MultiV<CType> &a, const uint &pos)
{
	if (pos < 1 || pos > 4) return CType(0);
	
	return a[iE1+pos-1];
}


////////////////////////////////////////////////////////////////////////////////////
// Get Component dual Vector Part of Multivector

template<class CType>
CType PGA<CType>::iVecComp(MultiV<CType> &a, const uint &pos)
{
	if (pos < 1 || pos > 4) return CType(0);
	
	return a[iE1I+pos-1];
}


////////////////////////////////////////////////////////////////////////////////////
// Get Component Line Part of Multivector

template<class CType>
CType PGA<CType>::LineComp(MultiV<CType> &a, const uint &pos)
{
	if (pos < 1 || pos > 6) return CType(0);
	
	return a[iL1+pos-1];
}


////////////////////////////////////////////////////////////////////////////////////
// Make Rotor
// a is rotation axis, rad is rotation angle

template<class CType>
MultiV<CType> PGA<CType>::Rotor(const CType &a1, const CType &a2, const CType &a3, const CType &rad)
{
	MultiV<CType> vA(m_MVStyle);

	vA[iE1] = a1;
	vA[iE2] = a2;
	vA[iE3] = a3;

	vA /= CType(sqrt(a1*a1 + a2*a2 + a3*a3));
	vA &= ~vE4I;

	vA = CType(cos(rad/2.0)) - (CType(sin(rad/2.0)) & vA);

	vA.TinyToZero();

	return vA;
}


////////////////////////////////////////////////////////////////////////////////////
// Make Rotor
// vA is rotation axis, rad is rotation angle

template<class CType>
MultiV<CType> PGA<CType>::Rotor(const MultiV<CType> &vX, const CType &rad, bool bAxis)

{
	MultiV<CType> vA(m_MVStyle);

	if (bAxis)
	{
		// Assume vX is rotation axis
		vA = vX(1);
		vA[iE4] = CType(0);

		vA /= CType(sqrt(Scalar(vA * vA)));
		vA &= -vE4I; // Rotation plane is U = x I3, and x = U I3^(-1)
	}
	else
	{
		// Assume vX is rotation plane
		vA = vX(2);
	
		vA /= CType(sqrt(Scalar(vA * (~vA))));
	}

	vA = CType(cos(rad/2.0)) - (CType(sin(rad/2.0)) & vA);

	vA.TinyToZero();

	return vA;
}


////////////////////////////////////////////////////////////////////////////////////
// Analyse Multivector. 
// Result is returned in rMVI.
// If function returns false, MV could not be analyzed.

template<class CType>
bool PGA<CType>::AnalyzeMV(const MultiV<CType>& vX, CMVInfo<CType>& rMVI, 
						   bool bOPNS, CType fPrec)
{
	MultiV<CType> vA(vX);
	Mem<uint> mGList;
	int iGrades;

	if (!GradeList(vA, mGList, fPrec)) return false;
	if ((iGrades = mGList.Count()) == 0 || iGrades > 2) return false;

	if (iGrades == 1)
	{
		return AnalyzeBlade(vA, rMVI, bOPNS, fPrec);
	}
	else if (iGrades == 2 && mGList[0] == 2 && mGList[1] == 0)
	{
		return AnalyzeRotor(vA, rMVI, fPrec);
	}

	return false;
}


////////////////////////////////////////////////////////////////////////////////////
// Analyse Blade.
// Result is returned in rMVI.
// If function returns false, MV could not be analyzed.

template<class CType>
bool PGA<CType>::AnalyzeBlade(const MultiV<CType>& vY, CMVInfo<CType>& rMVI, 
							  bool bOPNS, CType fPrec)
{
	CType pfVec[4][4];
	Mem<uint> mGList;
	MultiV<CType> vA, vX, vN, vE4Proj;
	MemObj<MultiV<CType> > mvList;
	int piOrder[] = { 3, 2, 1, 0 };
	CPointND<CType, 3> pVex[4];
	CType fMag;
	CType fZeroMin, fZeroMax, fUnityMin, fUnityMax;
	bool bNoE4 = true;

	if (fPrec <= CType(0))
		Tiny(fPrec);

	fZeroMin = CType(0) - fPrec;
	fZeroMax = CType(0) + fPrec;

	fUnityMin = CType(1) - fPrec;
	fUnityMax = CType(1) + fPrec;

	rMVI.Reset();

	if (!bOPNS)
		vX = *vY;
	else
		vX = vY;

	if (!vX.GetMaxHMV(vA, fPrec))
		return false;

	fMag = CType(sqrt(Scalar(vA * ~vA)));

	if (!GradeList(vA, mGList, fPrec)) 
		return false;

	// If vA is of higher grade than 1
	if (mGList[0] > 1)
	{
		// Project e4 onto vA.
		vE4Proj = (vE4 * vA) * (~vA) / Scalar(vA * ~vA);
		vE4Proj.TinyToZero();

		if (vE4Proj.IsZero(fPrec))
		{
			FactorBlade(vA, mvList, true, (MultiV<CType> *) 0, (CType *) 0, fPrec);
			bNoE4 = true;
		}
		else
		{
			// Remove that part from vA
			vA = vE4Proj * vA;
			FactorBlade(vA, mvList, true, (MultiV<CType> *) 0, (CType *) 0, fPrec);

			// Insert E4 projection at first position
			mvList.Insert(0);
			mvList[0] = vE4Proj;
			bNoE4 = false;
		}
	}
	else
	{
		mvList.Set(1);
		if (fMag < fZeroMin || fMag > fZeroMax)
			mvList[0] = vA / fMag;
		else
			mvList[0] = vA;

		if (vA[iE4] >= fZeroMin && vA[iE4] <= fZeroMax)
		{
			bNoE4 = true;
		}
		else
		{
			bNoE4 = false;
		}
	}

	int i, n = mvList.Count();

	if (n < 0 || n > 4)
		return false;

	if (n == 0)
	{
		// Scalar
		// Type = GA_SCALAR
		// Val[0] = scalar

		rMVI.m_eType = GA_SCALAR;
		rMVI.m_mVal.Set(1);
		rMVI.m_mVal[0] = vX[0];

		return true;
	}

	for(i=0;i<n;i++)
	{
		GetVector(mvList[i], 1, pfVec[i]);
		pVex[i] = pfVec[i];
	}

	if (pfVec[0][3] != 0.0f)
	{
		pVex[0] /= pfVec[0][3];
	}

	if (n == 1)
	{
		if (bNoE4)
		{
			// Direction Vector
			// Type = GA_DIR1D
			// Vex[0] = vector

			rMVI.m_eType = GA_DIR1D;
			rMVI.m_mVex.Set(1);
			rMVI.m_mVal.Set(1);
			rMVI.m_mVex[0] = pVex[0];
			rMVI.m_mVal[0] = fMag;
		}
		else
		{
			// Point
			// Type = GA_POINT
			// Vex[0] = point

			rMVI.m_eType = GA_POINT;
			rMVI.m_mVex.Set(1);
			rMVI.m_mVex[0] = pVex[0];
		}
	}
	else if (n == 2)
	{
		if (bNoE4)
		{
			// Line at Infinity
			// Type = GA_DIR2D
			// Vex[0] = vector 1
			// Vex[1] = vector 2
			// Val[0] = magnitude

			rMVI.m_eType = GA_DIR2D;
			rMVI.m_mVex.Set(2);
			rMVI.m_mVal.Set(1);

			rMVI.m_mVex[0] = pVex[0];
			rMVI.m_mVex[1] = pVex[1];

			rMVI.m_mVal[0] = fMag;
		}
		else
		{
			// Line
			// Type = GA_LINE
			// Vex[0] = plumb vector
			// Vex[1] = dir. vector
			// Val[0] = magnitude

			rMVI.m_eType = GA_LINE;
			rMVI.m_mVex.Set(2);
			rMVI.m_mVal.Set(1);

			rMVI.m_mVex[0] = pVex[0];
			rMVI.m_mVex[1] = pVex[1];

			rMVI.m_mVal[0] = fMag;
		}
	}
	else if (n == 3)
	{
		if (bNoE4)
		{
			// Plane at infinity
			// Type = GA_DIR3D
			// Vex[0..2] = dir. vectors
			// Val[0] = magnitude

			rMVI.m_eType = GA_DIR3D;
			rMVI.m_mVex.Set(3);
			rMVI.m_mVal.Set(1);

			rMVI.m_mVex[0] = pVex[0];
			rMVI.m_mVex[1] = pVex[1];
			rMVI.m_mVex[2] = pVex[2];

			rMVI.m_mVal[0] = fMag;
		}
		else
		{
			// Plane
			// Type = GA_PLANE
			// Vex[0] = plumb vector
			// Vex[1..2] = dir. vectors
			// Val[0] = magnitude

			rMVI.m_eType = GA_PLANE;
			rMVI.m_mVex.Set(3);
			rMVI.m_mVal.Set(1);

			rMVI.m_mVex[0] = pVex[0];
			rMVI.m_mVex[1] = pVex[1];
			rMVI.m_mVex[2] = pVex[2];

			rMVI.m_mVal[0] = fMag;
		}
	}
	else if (n == 4)
	{
		// Space
		// Type = GA_SPACE
		// Val[0] = magnitude

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
bool PGA<CType>::AnalyzeRotor(const MultiV<CType>& vX, CMVInfo<CType>& rMVI,
							  CType fPrec)
{
	MultiV<CType> vU, vA;
	MemObj<MultiV<CType> > mvList;

	if (fPrec <= CType(0))
		Tiny(fPrec);

	rMVI.m_eType = GA_ROTOR;
	rMVI.m_mVex.Set(1);
	rMVI.m_mVal.Set(1);

	// Get Bivector Part
	vU = vX(2);

	// Rotation axis
	vA = ( -vU / CType(sqrt(Scalar(vU * (~vU)))) ) & vE4I;

	rMVI.m_mVex[0] = &vA[1];
	rMVI.m_mVal[0] = CType(2.0 * acos(Scalar(vX)));
	
	return true;
}
