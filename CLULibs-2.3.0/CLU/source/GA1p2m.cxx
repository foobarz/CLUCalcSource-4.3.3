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

#include"GA1p2m.h"
#include"GA1p2mInst.h"


////////////////////////////////////////////////////////////////////////////////////
/// Static Variables

template<class CType> Blade<CType> GA1p2m<CType>::bSC;
template<class CType> Blade<CType> GA1p2m<CType>::bE0;
template<class CType> Blade<CType> GA1p2m<CType>::bE1;
template<class CType> Blade<CType> GA1p2m<CType>::bE2;
template<class CType> Blade<CType> GA1p2m<CType>::bE01;
template<class CType> Blade<CType> GA1p2m<CType>::bE02;
template<class CType> Blade<CType> GA1p2m<CType>::bE12;
template<class CType> Blade<CType> GA1p2m<CType>::bI;

template<class CType> MultiV<CType> GA1p2m<CType>::vSC;
template<class CType> MultiV<CType> GA1p2m<CType>::vE0;
template<class CType> MultiV<CType> GA1p2m<CType>::vE1;
template<class CType> MultiV<CType> GA1p2m<CType>::vE2;
template<class CType> MultiV<CType> GA1p2m<CType>::vE01;
template<class CType> MultiV<CType> GA1p2m<CType>::vE02;
template<class CType> MultiV<CType> GA1p2m<CType>::vE12;
template<class CType> MultiV<CType> GA1p2m<CType>::vI;

template<class CType> MultiV<CType> GA1p2m<CType>::vZero;
template<class CType> RingBuf<uint> GA1p2m<CType>::BLADE;

template<class CType> MultiVStyle<CType> GA1p2m<CType>::m_MVStyle;

// GA1p2m needs initialisation
template<class CType> bool GA1p2m<CType>::m_bInit = true;

template<class CType> const uint GA1p2m<CType>::iSC = 0;
template<class CType> const uint GA1p2m<CType>::iE0 = 1;
template<class CType> const uint GA1p2m<CType>::iE1 = 2;
template<class CType> const uint GA1p2m<CType>::iE2 = 3;
template<class CType> const uint GA1p2m<CType>::iE01 = 4;
template<class CType> const uint GA1p2m<CType>::iE02 = 5;
template<class CType> const uint GA1p2m<CType>::iE12 = 6;
template<class CType> const uint GA1p2m<CType>::iI = 7;



//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

template<class CType>
GA1p2m<CType>::GA1p2m() : Base<CType>(3,0,"e",ID_GA1P2M)
{
	if (m_bInit) 
	{
		InitVars();
		m_bInit = false;
	}
}


////////////////////////////////////////////////////////////////////////////////////
/// Functions

// GA1p2m Signature
template<class CType>
inline CType GA1p2m<CType>::signature(uint i) 
{ 
	return (i == 0 ? CType(1) : CType(-1)); 
}


////////////////////////////////////////////////////////////////////////////////////
/// Init Variables

template<class CType>
void GA1p2m<CType>::InitVars()
{
	BladeList<CType> GA1p2mStyle(*this);
	
	BLADE.Reset(1);
	bSC.Reset(*this, CType(1));
	bE0.Reset(*this,CType(1),1,(BLADE<<0),"e0");
	bE1.Reset(*this,CType(1),1,(BLADE<<1),"e1");
	bE2.Reset(*this,CType(1),1,(BLADE<<2),"e2");
	
	BLADE.Reset(2);
	bE01.Reset(*this,CType(1),2,(BLADE<<0<<1),"e01");
	bE02.Reset(*this,CType(1),2,(BLADE<<0<<2),"e02");
	bE12.Reset(*this,CType(1),2,(BLADE<<1<<2),"e12");
	
	BLADE.Reset(3);
	bI.Reset(*this,CType(1),3,(BLADE<<0<<1<<2),"I");
	
	GA1p2mStyle <<bSC<<bE0<<bE1<<bE2<<bE01<<bE02<<bE12<<bI;
	m_MVStyle.Init(GA1p2mStyle); // Initialises multiplication tables
	
	vZero.SetStyle(m_MVStyle);

	vSC.SetStyle(m_MVStyle); vSC[iSC] = 1;
	vE0.SetStyle(m_MVStyle); vE0[iE0] = 1;
	vE1.SetStyle(m_MVStyle); vE1[iE1] = 1;
	vE2.SetStyle(m_MVStyle); vE2[iE2] = 1;
	vE01.SetStyle(m_MVStyle); vE01[iE01] = 1;
	vE02.SetStyle(m_MVStyle); vE02[iE02] = 1;
	vE12.SetStyle(m_MVStyle); vE12[iE12] = 1;
	vI.SetStyle(m_MVStyle); vI[iI] = 1;

}


////////////////////////////////////////////////////////////////////////////////////
// Make Vector

template<class CType>
MultiV<CType> GA1p2m<CType>::Vec(const CType &a1, const CType &a2, const CType &a3)
{
	CType a[4];
	
	a[0] = a1;
	a[1] = a2;
	a[2] = a3;
	
	return MakeVector(m_MVStyle,1,a);
}

////////////////////////////////////////////////////////////////////////////////////
// Make Bivector

template<class CType>
MultiV<CType> GA1p2m<CType>::Bivec(const CType &a1, const CType &a2, const CType &a3)
{
	CType a[4];
	
	a[0] = a1;
	a[1] = a2;
	a[2] = a3;
	
	return MakeVector(m_MVStyle,2,a);
}
