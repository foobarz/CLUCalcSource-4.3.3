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


// Geometric Algebra GA_{1,2}
////////////////////////////////////////////////////////////////////////////////////


#ifndef _GA1P2M_H_
#define _GA1P2M_H_


#include "cga.h"
#include "xutlib.h"


#define ID_GA1P2M 0x0004


#define DeclareGA1p2mVarsLocal(TYPE)\
	const Blade<TYPE> &bSC = GA1p2m<TYPE>::bSC;\
	const Blade<TYPE> &bE0 = GA1p2m<TYPE>::bE0;\
	const Blade<TYPE> &bE1 = GA1p2m<TYPE>::bE1;\
	const Blade<TYPE> &bE2 = GA1p2m<TYPE>::bE2;\
	const Blade<TYPE> &bE01 = GA1p2m<TYPE>::bE01;\
	const Blade<TYPE> &bE02 = GA1p2m<TYPE>::bE02;\
	const Blade<TYPE> &bE12 = GA1p2m<TYPE>::bE12;\
	const Blade<TYPE> &bI = GA1p2m<TYPE>::bI;\
	const MultiV<TYPE> &vSC = GA1p2m<TYPE>::vSC;\
	const MultiV<TYPE> &vE0 = GA1p2m<TYPE>::vE0;\
	const MultiV<TYPE> &vE1 = GA1p2m<TYPE>::vE1;\
	const MultiV<TYPE> &vE2 = GA1p2m<TYPE>::vE2;\
	const MultiV<TYPE> &vE01 = GA1p2m<TYPE>::vE01;\
	const MultiV<TYPE> &vE02 = GA1p2m<TYPE>::vE02;\
	const MultiV<TYPE> &vE12 = GA1p2m<TYPE>::vE12;\
	const MultiV<TYPE> &vI = GA1p2m<TYPE>::vI;\
	const MultiV<TYPE> &vZero = GA1p2m<TYPE>::vZero;\
	const uint iSC = GA1p2m<TYPE>::iSC;\
	const uint iE0 = GA1p2m<TYPE>::iE0;\
	const uint iE1 = GA1p2m<TYPE>::iE1;\
	const uint iE2 = GA1p2m<TYPE>::iE2;\
	const uint iE01 = GA1p2m<TYPE>::iE01;\
	const uint iE02 = GA1p2m<TYPE>::iE02;\
	const uint iE12 = GA1p2m<TYPE>::iE12;\
	const uint iI = GA1p2m<TYPE>::iI;



template<class CType>
class GA1p2m : public Base<CType>  
{
public:
	GA1p2m();
//	virtual ~GA1p2m();

	CType signature(uint i);

	static const MultiVStyle<CType>& GetMVStyle() { return m_MVStyle; }

	// Initialise variables. Called by constructor if m_bInit == true;
	void InitVars();

	// Additional Functions -----------------------------

	// Make Vector
	static MultiV<CType> Vec(const CType &a0, const CType &a1, const CType &a2);

	// Make Bivector
	static MultiV<CType> Bivec(const CType &a0, const CType &a1, const CType &a2);
	
public:
	static Blade<CType> bSC, bE0, bE1, bE2, bE01, bE02, bE12, bI;
	static MultiV<CType> vSC, vE0, vE1, vE2, vE01, vE02, vE12, vI;
	static MultiV<CType> vZero;
	static RingBuf<uint> BLADE;

	static const uint iSC;
	static const uint iE0;
	static const uint iE1;
	static const uint iE2;
	static const uint iE01;
	static const uint iE02;
	static const uint iE12;
	static const uint iI;
		
protected:
	static MultiVStyle<CType> m_MVStyle;

private:
	static bool m_bInit;

};

#endif 


