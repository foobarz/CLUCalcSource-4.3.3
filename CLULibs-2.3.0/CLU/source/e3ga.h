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


// 3D Euclidean Geometric Algebra (E3GA) Class Declaration and Definitions

#ifndef __E3GA_HH
#define __E3GA_HH

#include "cga.h"
#include "xutlib.h"
#include "MVInfo.h"

#define ID_E3GA 0x0003

#define DeclareE3GAVarsLocal(TYPE)\
	const Blade<TYPE> &bSC = E3GA<TYPE>::bSC;\
	const Blade<TYPE> &bE1 = E3GA<TYPE>::bE1;\
	const Blade<TYPE> &bE2 = E3GA<TYPE>::bE2;\
	const Blade<TYPE> &bE3 = E3GA<TYPE>::bE3;\
	const Blade<TYPE> &bL1 = E3GA<TYPE>::bL1;\
	const Blade<TYPE> &bL2 = E3GA<TYPE>::bL2;\
	const Blade<TYPE> &bL3 = E3GA<TYPE>::bL3;\
	const Blade<TYPE> &bI = E3GA<TYPE>::bI;\
	const MultiV<TYPE> &vSC = E3GA<TYPE>::vSC;\
	const MultiV<TYPE> &vE1 = E3GA<TYPE>::vE1;\
	const MultiV<TYPE> &vE2 = E3GA<TYPE>::vE2;\
	const MultiV<TYPE> &vE3 = E3GA<TYPE>::vE3;\
	const MultiV<TYPE> &vL1 = E3GA<TYPE>::vL1;\
	const MultiV<TYPE> &vL2 = E3GA<TYPE>::vL2;\
	const MultiV<TYPE> &vL3 = E3GA<TYPE>::vL3;\
	const MultiV<TYPE> &vI = E3GA<TYPE>::vI;\
	const MultiV<TYPE> &vZero = E3GA<TYPE>::vZero;\
	const uint iSC = E3GA<TYPE>::iSC;\
	const uint iE1 = E3GA<TYPE>::iE1;\
	const uint iE2 = E3GA<TYPE>::iE2;\
	const uint iE3 = E3GA<TYPE>::iE3;\
	const uint iL1 = E3GA<TYPE>::iL1;\
	const uint iL2 = E3GA<TYPE>::iL2;\
	const uint iL3 = E3GA<TYPE>::iL3;\
	const uint iI = E3GA<TYPE>::iI;

// E3GA Class Declaration -----------------------
template<class CType>
class E3GA : public Base<CType>
{
public:
	E3GA();
	
	CType signature(uint i);

	static const MultiVStyle<CType>& GetMVStyle() { return m_MVStyle; }

	// Initialise variables. Called by constructor if m_bInit == true;
	void InitVars();

	// Additional Functions -----------------------------

	// Analyse Multivector. Result is returned in rMVI.
	static bool AnalyzeMV(const MultiV<CType>& vA, CMVInfo<CType>& rMVI, bool bOPNS, CType fPrec);
	static bool AnalyzeBlade(const MultiV<CType>& vA, CMVInfo<CType>& rMVI, bool bOPNS, CType fPrec);
	static bool AnalyzeRotor(const MultiV<CType>& vA, CMVInfo<CType>& rMVI, CType fPrec);
	
	// Make 3-D Vector
	static MultiV<CType> Vec(const CType &a1, const CType &a2, const CType &a3);
	static MultiV<CType> Vec(const CType *pA);

	// Make 3-D Vector from spherical coords
	static MultiV<CType> SVec(const CType &r, const CType &theta, const CType &phi);
	static MultiV<CType> SVec(const CType *pA);

	// Make Bivector
	static MultiV<CType> Bivec(const CType &a1, const CType &a2, const CType &a3);
	static MultiV<CType> Bivec(const CType *pA);
	
	// Make rotation operator
	static MultiV<CType> Rotor(const CType &a1, const CType &a2, const CType &a3, const CType &rad);
	static MultiV<CType> Rotor(const MultiV<CType> &vA, const CType &rad, bool bAxis = true);

public:
	static Blade<CType> bSC, bE1, bE2, bE3, bL1, bL2, bL3, bI;
	static MultiV<CType> vSC, vE1, vE2, vE3, vL1, vL2, vL3, vI;
	static MultiV<CType> vZero;
	static RingBuf<uint> BLADE;

	static const uint iSC;
	static const uint iE1;
	static const uint iE2;
	static const uint iE3;
	static const uint iL1;
	static const uint iL2;
	static const uint iL3;
	static const uint iI;
		
protected:
	static MultiVStyle<CType> m_MVStyle;

private:
	static bool m_bInit;

	static int piGrades[8], piVITable[8], piDualTable[8];
	static int piGPTable[64], piIPTable[64], piOPTable[64];
};
#endif

