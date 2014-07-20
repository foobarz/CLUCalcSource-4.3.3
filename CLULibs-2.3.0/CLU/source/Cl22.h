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


// Cl(2,2) Class Declaration and Definitions
// e1, e3 square to +1
// e2, e4 square to -1

#ifndef _CL22_H_
#define _CL22_H_

#include "cga.h"
#include "xutlib.h"


#define ID_CL22 0x0006

#define DeclareCl22VarsLocal(TYPE)\
	const Blade<TYPE> &bSC = Cl22<TYPE>::bSC;\
	const Blade<TYPE> &bE1 = Cl22<TYPE>::bE1;\
	const Blade<TYPE> &bE2 = Cl22<TYPE>::bE2;\
	const Blade<TYPE> &bE3 = Cl22<TYPE>::bE3;\
	const Blade<TYPE> &bE4 = Cl22<TYPE>::bE4;\
	const Blade<TYPE> &bL1 = Cl22<TYPE>::bL1;\
	const Blade<TYPE> &bL2 = Cl22<TYPE>::bL2;\
	const Blade<TYPE> &bL3 = Cl22<TYPE>::bL3;\
	const Blade<TYPE> &bL1I = Cl22<TYPE>::bL1I;\
	const Blade<TYPE> &bL2I = Cl22<TYPE>::bL2I;\
	const Blade<TYPE> &bL3I = Cl22<TYPE>::bL3I;\
	const Blade<TYPE> &bE1I = Cl22<TYPE>::bE1I;\
	const Blade<TYPE> &bE2I = Cl22<TYPE>::bE2I;\
	const Blade<TYPE> &bE3I = Cl22<TYPE>::bE3I;\
	const Blade<TYPE> &bE4I = Cl22<TYPE>::bE4I;\
	const Blade<TYPE> &bI = Cl22<TYPE>::bI;\
	const MultiV<TYPE> &vSC = Cl22<TYPE>::vSC;\
	const MultiV<TYPE> &vE1 = Cl22<TYPE>::vE1;\
	const MultiV<TYPE> &vE2 = Cl22<TYPE>::vE2;\
	const MultiV<TYPE> &vE3 = Cl22<TYPE>::vE3;\
	const MultiV<TYPE> &vE4 = Cl22<TYPE>::vE4;\
	const MultiV<TYPE> &vL1 = Cl22<TYPE>::vL1;\
	const MultiV<TYPE> &vL2 = Cl22<TYPE>::vL2;\
	const MultiV<TYPE> &vL3 = Cl22<TYPE>::vL3;\
	const MultiV<TYPE> &vL1I = Cl22<TYPE>::vL1I;\
	const MultiV<TYPE> &vL2I = Cl22<TYPE>::vL2I;\
	const MultiV<TYPE> &vL3I = Cl22<TYPE>::vL3I;\
	const MultiV<TYPE> &vE1I = Cl22<TYPE>::vE1I;\
	const MultiV<TYPE> &vE2I = Cl22<TYPE>::vE2I;\
	const MultiV<TYPE> &vE3I = Cl22<TYPE>::vE3I;\
	const MultiV<TYPE> &vE4I = Cl22<TYPE>::vE4I;\
	const MultiV<TYPE> &vI = Cl22<TYPE>::vI;\
	const MultiV<TYPE> &vZero = Cl22<TYPE>::vZero;\
	const uint iSC = Cl22<TYPE>::iSC;\
	const uint iE1 = Cl22<TYPE>::iE1;\
	const uint iE2 = Cl22<TYPE>::iE2;\
	const uint iE3 = Cl22<TYPE>::iE3;\
	const uint iE4 = Cl22<TYPE>::iE4;\
	const uint iL1 = Cl22<TYPE>::iL1;\
	const uint iL2 = Cl22<TYPE>::iL2;\
	const uint iL3 = Cl22<TYPE>::iL3;\
	const uint iL1I = Cl22<TYPE>::iL1I;\
	const uint iL2I = Cl22<TYPE>::iL2I;\
	const uint iL3I = Cl22<TYPE>::iL3I;\
	const uint iE1I = Cl22<TYPE>::iE1I;\
	const uint iE2I = Cl22<TYPE>::iE2I;\
	const uint iE3I = Cl22<TYPE>::iE3I;\
	const uint iE4I = Cl22<TYPE>::iE4I;\
	const uint iI = Cl22<TYPE>::iI;

// Cl22 Class Declaration -----------------------
template<class CType>
class Cl22 : public Base<CType>
{
public:
	Cl22();
	
	CType signature(uint i);

	static const MultiVStyle<CType>& GetMVStyle() { return m_MVStyle; }

	// Initialise variables. Called by constructor if m_bInit == true;
	void InitVars();

	// Additional Functions -----------------------------

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

