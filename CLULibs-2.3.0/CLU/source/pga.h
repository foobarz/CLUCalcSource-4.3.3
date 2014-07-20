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


// Projective Geometry Algebra (PGA) Class Declaration and Definitions

#ifndef __PGA_HH
#define __PGA_HH

#include "cga.h"
#include "xutlib.h"


#define ID_PGA 0x0002

#define DeclarePGAVarsLocal(TYPE)\
	const Blade<TYPE> &bSC = PGA<TYPE>::bSC;\
	const Blade<TYPE> &bE1 = PGA<TYPE>::bE1;\
	const Blade<TYPE> &bE2 = PGA<TYPE>::bE2;\
	const Blade<TYPE> &bE3 = PGA<TYPE>::bE3;\
	const Blade<TYPE> &bE4 = PGA<TYPE>::bE4;\
	const Blade<TYPE> &bL1 = PGA<TYPE>::bL1;\
	const Blade<TYPE> &bL2 = PGA<TYPE>::bL2;\
	const Blade<TYPE> &bL3 = PGA<TYPE>::bL3;\
	const Blade<TYPE> &bL1I = PGA<TYPE>::bL1I;\
	const Blade<TYPE> &bL2I = PGA<TYPE>::bL2I;\
	const Blade<TYPE> &bL3I = PGA<TYPE>::bL3I;\
	const Blade<TYPE> &bE1I = PGA<TYPE>::bE1I;\
	const Blade<TYPE> &bE2I = PGA<TYPE>::bE2I;\
	const Blade<TYPE> &bE3I = PGA<TYPE>::bE3I;\
	const Blade<TYPE> &bE4I = PGA<TYPE>::bE4I;\
	const Blade<TYPE> &bI = PGA<TYPE>::bI;\
	const MultiV<TYPE> &vSC = PGA<TYPE>::vSC;\
	const MultiV<TYPE> &vE1 = PGA<TYPE>::vE1;\
	const MultiV<TYPE> &vE2 = PGA<TYPE>::vE2;\
	const MultiV<TYPE> &vE3 = PGA<TYPE>::vE3;\
	const MultiV<TYPE> &vE4 = PGA<TYPE>::vE4;\
	const MultiV<TYPE> &vL1 = PGA<TYPE>::vL1;\
	const MultiV<TYPE> &vL2 = PGA<TYPE>::vL2;\
	const MultiV<TYPE> &vL3 = PGA<TYPE>::vL3;\
	const MultiV<TYPE> &vL1I = PGA<TYPE>::vL1I;\
	const MultiV<TYPE> &vL2I = PGA<TYPE>::vL2I;\
	const MultiV<TYPE> &vL3I = PGA<TYPE>::vL3I;\
	const MultiV<TYPE> &vE1I = PGA<TYPE>::vE1I;\
	const MultiV<TYPE> &vE2I = PGA<TYPE>::vE2I;\
	const MultiV<TYPE> &vE3I = PGA<TYPE>::vE3I;\
	const MultiV<TYPE> &vE4I = PGA<TYPE>::vE4I;\
	const MultiV<TYPE> &vI = PGA<TYPE>::vI;\
	const MultiV<TYPE> &vZero = PGA<TYPE>::vZero;\
	const uint iSC = PGA<TYPE>::iSC;\
	const uint iE1 = PGA<TYPE>::iE1;\
	const uint iE2 = PGA<TYPE>::iE2;\
	const uint iE3 = PGA<TYPE>::iE3;\
	const uint iE4 = PGA<TYPE>::iE4;\
	const uint iL1 = PGA<TYPE>::iL1;\
	const uint iL2 = PGA<TYPE>::iL2;\
	const uint iL3 = PGA<TYPE>::iL3;\
	const uint iL1I = PGA<TYPE>::iL1I;\
	const uint iL2I = PGA<TYPE>::iL2I;\
	const uint iL3I = PGA<TYPE>::iL3I;\
	const uint iE1I = PGA<TYPE>::iE1I;\
	const uint iE2I = PGA<TYPE>::iE2I;\
	const uint iE3I = PGA<TYPE>::iE3I;\
	const uint iE4I = PGA<TYPE>::iE4I;\
	const uint iI = PGA<TYPE>::iI;

// PGA Class Declaration -----------------------
template<class CType>
class PGA : public Base<CType>
{
public:
	PGA();
	
	CType signature(uint i);

	static const MultiVStyle<CType>& GetMVStyle() { return m_MVStyle; }

	// Initialise variables. Called by constructor if m_bInit == true;
	void InitVars();

	// Additional Functions -----------------------------
	
	// Analyse Multivector. Result is returned in rMVI.
	static bool AnalyzeMV(const MultiV<CType>& vA, CMVInfo<CType>& rMVI, 
						bool bOPNS, CType fPrec = CType(0));
	static bool AnalyzeBlade(const MultiV<CType>& vA, CMVInfo<CType>& rMVI, 
						bool bOPNS, CType fPrec = CType(0));
	static bool AnalyzeRotor(const MultiV<CType>& vA, CMVInfo<CType>& rMVI,
						CType fPrec = CType(0));

	// Make 3-D Vector
	static MultiV<CType> Vec(const CType &a1, const CType &a2, const CType &a3);
	static MultiV<CType> Vec(const CType *pA);
	
	// Make Vector with homogeneous coords
	static MultiV<CType> hVec(const CType &a1, const CType &a2, const CType &a3, const CType &a4);
	static MultiV<CType> hVec(const CType *pA);
	
	// Make 3-D Vector from homogeneous Vector
	static MultiV<CType> Vec(const MultiV<CType> &a);
	
	// Make Vector with homogeneous coords from space Vector
	static MultiV<CType> hVec(const MultiV<CType> &a, const CType &a4);
	
	// Make Vector with homogeneous coords from space Vector
	// Assumes 4th coord unity
	static MultiV<CType> hVec(const MultiV<CType> &a); 
	
	
	// Get Space Vector Part
	static void GetVec(MultiV<CType> &a, CType *vec);
	
	
	// Get Space Part of Multivector
	static MultiV<CType> VecPart(const MultiV<CType> &a);
	
	// Get Dual Space Part of Multivector
	static MultiV<CType> iVecPart(const MultiV<CType> &a);
	
	// Get bivec part of Multivector
	static MultiV<CType> LinePart(const MultiV<CType> &a);
	
	// Get first three comps of bivec part
	static MultiV<CType> LinePart1(const MultiV<CType> &a);
	
	// Get last three comps of bivec part
	static MultiV<CType> LinePart2(const MultiV<CType> &a);
	
	// Get Component of Space Part of Multivector
	static CType VecComp(MultiV<CType> &a, const uint &pos);
	
	// Get Component of Dual Space Part of Multivector
	static CType iVecComp(MultiV<CType> &a, const uint &pos);
	
	// Get Component of homo Part of Multivector
	static CType LineComp(MultiV<CType> &a, const uint &pos);

	// Make rotation operator
	static MultiV<CType> Rotor(const CType &a1, const CType &a2, const CType &a3, const CType &rad);
	static MultiV<CType> Rotor(const MultiV<CType> &vA, const CType &rad, bool bAxis = true);

public:
	static Blade<CType> bSC, bE1, bE2, bE3, bE4, bL1, bL2, bL3;
	static Blade<CType> bL1I, bL2I, bL3I, bE1I, bE2I, bE3I, bE4I, bI;
	static MultiV<CType> vSC, vE1, vE2, vE3, vE4, vL1, vL2, vL3;
	static MultiV<CType> vL1I, vL2I, vL3I, vE1I, vE2I, vE3I, vE4I, vI;
	static MultiV<CType> vZero;
	static RingBuf<uint> BLADE;

	static const uint iSC;
	static const uint iE1;
	static const uint iE2;
	static const uint iE3;
	static const uint iE4;
	static const uint iL1;
	static const uint iL2;
	static const uint iL3;
	static const uint iL1I;
	static const uint iL2I;
	static const uint iL3I;
	static const uint iE1I;
	static const uint iE2I;
	static const uint iE3I;
	static const uint iE4I;
	static const uint iI;
		
protected:
	static MultiVStyle<CType> m_MVStyle;

private:
	static bool m_bInit;

	static int piGrades[16], piVITable[16], piDualTable[16];
	static int piGPTable[256], piIPTable[256], piOPTable[256];
	
};
#endif

