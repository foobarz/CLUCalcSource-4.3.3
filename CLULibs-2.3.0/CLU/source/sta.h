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


// STA Class Declaration and Definitions

#ifndef __STA_HH
#define __STA_HH


#include "cga.h"
#include "xutlib.h"


#define ID_STA 0x0001

#define DeclareSTAVarsLocal(TYPE)\
	const Blade<TYPE> &bSC = STA<TYPE>::bSC;\
	const Blade<TYPE> &bG0 = STA<TYPE>::bG0;\
	const Blade<TYPE> &bG1 = STA<TYPE>::bG1;\
	const Blade<TYPE> &bG2 = STA<TYPE>::bG2;\
	const Blade<TYPE> &bG3 = STA<TYPE>::bG3;\
	const Blade<TYPE> &bS1 = STA<TYPE>::bS1;\
	const Blade<TYPE> &bS2 = STA<TYPE>::bS2;\
	const Blade<TYPE> &bS3 = STA<TYPE>::bS3;\
	const Blade<TYPE> &bIS1 = STA<TYPE>::bIS1;\
	const Blade<TYPE> &bIS2 = STA<TYPE>::bIS2;\
	const Blade<TYPE> &bIS3 = STA<TYPE>::bIS3;\
	const Blade<TYPE> &bIG0 = STA<TYPE>::bIG0;\
	const Blade<TYPE> &bIG1 = STA<TYPE>::bIG1;\
	const Blade<TYPE> &bIG2 = STA<TYPE>::bIG2;\
	const Blade<TYPE> &bIG3 = STA<TYPE>::bIG3;\
	const Blade<TYPE> &bI = STA<TYPE>::bI;\
	const MultiV<TYPE> &vSC = STA<TYPE>::vSC;\
	const MultiV<TYPE> &vG0 = STA<TYPE>::vG0;\
	const MultiV<TYPE> &vG1 = STA<TYPE>::vG1;\
	const MultiV<TYPE> &vG2 = STA<TYPE>::vG2;\
	const MultiV<TYPE> &vG3 = STA<TYPE>::vG3;\
	const MultiV<TYPE> &vS1 = STA<TYPE>::vS1;\
	const MultiV<TYPE> &vS2 = STA<TYPE>::vS2;\
	const MultiV<TYPE> &vS3 = STA<TYPE>::vS3;\
	const MultiV<TYPE> &vIS1 = STA<TYPE>::vIS1;\
	const MultiV<TYPE> &vIS2 = STA<TYPE>::vIS2;\
	const MultiV<TYPE> &vIS3 = STA<TYPE>::vIS3;\
	const MultiV<TYPE> &vIG0 = STA<TYPE>::vIG0;\
	const MultiV<TYPE> &vIG1 = STA<TYPE>::vIG1;\
	const MultiV<TYPE> &vIG2 = STA<TYPE>::vIG2;\
	const MultiV<TYPE> &vIG3 = STA<TYPE>::vIG3;\
	const MultiV<TYPE> &vI = STA<TYPE>::vI;\
	const MultiV<TYPE> &vZero = STA<TYPE>::vZero;\
	const uint iSC = STA<TYPE>::iSC;\
	const uint iG0 = STA<TYPE>::iG0;\
	const uint iG1 = STA<TYPE>::iG1;\
	const uint iG2 = STA<TYPE>::iG2;\
	const uint iG3 = STA<TYPE>::iG3;\
	const uint iS1 = STA<TYPE>::iS1;\
	const uint iS2 = STA<TYPE>::iS2;\
	const uint iS3 = STA<TYPE>::iS3;\
	const uint iIS1 = STA<TYPE>::iIS1;\
	const uint iIS2 = STA<TYPE>::iIS2;\
	const uint iIS3 = STA<TYPE>::iIS3;\
	const uint iIG0 = STA<TYPE>::iIG0;\
	const uint iIG1 = STA<TYPE>::iIG1;\
	const uint iIG2 = STA<TYPE>::iIG2;\
	const uint iIG3 = STA<TYPE>::iIG3;\
	const uint iI = STA<TYPE>::iI;


// STA Class Declaration -----------------------
template<class CType>
class STA : public Base<CType>
{
public:
	STA();
	
	CType signature(uint i);

	static const MultiVStyle<CType>& GetMVStyle() { return m_MVStyle; }

	// Initialise variables. Called by constructor if m_bInit == true;
	void InitVars();

	// Additional Functions -----------------------------
	
	// Create 4D Vector
	static MultiV<CType> Vec(const CType &a1, const CType &a2, 
					  const CType &a3, const CType &a4);

	// Create Bivector with components (S1, S2, S3)
	static MultiV<CType> SpaceBiVec(const CType &a1, const CType &a2, const CType &a3);
	
	// Make rotation operator
	static MultiV<CType> Rotor(const CType &a1, const CType &a2, const CType &a3, 
							   const CType &rad);

	// Make boost operator
	static MultiV<CType> Boost(const CType &a1, const CType &a2, const CType &a3, 
							   const CType &v);


public:
	static Blade<CType> bSC, bG0, bG1, bG2, bG3, bS1, bS2, bS3;
	static Blade<CType> bIS1, bIS2, bIS3, bIG0, bIG1, bIG2, bIG3, bI;
	static MultiV<CType> vSC, vG0, vG1, vG2, vG3, vS1, vS2, vS3;
	static MultiV<CType> vIS1, vIS2, vIS3, vIG0, vIG1, vIG2, vIG3, vI;
	static MultiV<CType> vZero;
	static RingBuf<uint> BLADE;

	static const uint iSC;
	static const uint iG0;
	static const uint iG1;
	static const uint iG2;
	static const uint iG3;
	static const uint iS1;
	static const uint iS2;
	static const uint iS3;
	static const uint iIS1;
	static const uint iIS2;
	static const uint iIS3;
	static const uint iIG0;
	static const uint iIG1;
	static const uint iIG2;
	static const uint iIG3;
	static const uint iI;
		
protected:
	static MultiVStyle<CType> m_MVStyle;

private:
	static bool m_bInit;

	static int piGrades[16], piVITable[16], piDualTable[16];
	static int piGPTable[256], piIPTable[256], piOPTable[256];
	
};

#endif

