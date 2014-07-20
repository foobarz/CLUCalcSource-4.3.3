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


// Cl22 Function and Class Definitions
// e1, e3 square to +1
// e2, e4 square to -1

#include"Cl22.h"
#include"Cl22inst.h"

#include "xmalib.h"


////////////////////////////////////////////////////////////////////////////////////
/// Static Variables

template<class CType> Blade<CType> Cl22<CType>::bSC;
template<class CType> Blade<CType> Cl22<CType>::bE1;
template<class CType> Blade<CType> Cl22<CType>::bE2;
template<class CType> Blade<CType> Cl22<CType>::bE3;
template<class CType> Blade<CType> Cl22<CType>::bE4;
template<class CType> Blade<CType> Cl22<CType>::bL1;
template<class CType> Blade<CType> Cl22<CType>::bL2;
template<class CType> Blade<CType> Cl22<CType>::bL3;
template<class CType> Blade<CType> Cl22<CType>::bL1I;
template<class CType> Blade<CType> Cl22<CType>::bL2I;
template<class CType> Blade<CType> Cl22<CType>::bL3I;
template<class CType> Blade<CType> Cl22<CType>::bE1I;
template<class CType> Blade<CType> Cl22<CType>::bE2I;
template<class CType> Blade<CType> Cl22<CType>::bE3I;
template<class CType> Blade<CType> Cl22<CType>::bE4I;
template<class CType> Blade<CType> Cl22<CType>::bI;

template<class CType> MultiV<CType> Cl22<CType>::vSC;
template<class CType> MultiV<CType> Cl22<CType>::vE1;
template<class CType> MultiV<CType> Cl22<CType>::vE2;
template<class CType> MultiV<CType> Cl22<CType>::vE3;
template<class CType> MultiV<CType> Cl22<CType>::vE4;
template<class CType> MultiV<CType> Cl22<CType>::vL1;
template<class CType> MultiV<CType> Cl22<CType>::vL2;
template<class CType> MultiV<CType> Cl22<CType>::vL3;
template<class CType> MultiV<CType> Cl22<CType>::vL1I;
template<class CType> MultiV<CType> Cl22<CType>::vL2I;
template<class CType> MultiV<CType> Cl22<CType>::vL3I;
template<class CType> MultiV<CType> Cl22<CType>::vE1I;
template<class CType> MultiV<CType> Cl22<CType>::vE2I;
template<class CType> MultiV<CType> Cl22<CType>::vE3I;
template<class CType> MultiV<CType> Cl22<CType>::vE4I;
template<class CType> MultiV<CType> Cl22<CType>::vI;

template<class CType> MultiV<CType> Cl22<CType>::vZero;
template<class CType> RingBuf<uint> Cl22<CType>::BLADE;

template<class CType> MultiVStyle<CType> Cl22<CType>::m_MVStyle;

// Cl22 needs initialisation
template<class CType> bool Cl22<CType>::m_bInit = true;

template<class CType> const uint Cl22<CType>::iSC = 0;
template<class CType> const uint Cl22<CType>::iE1 = 1;
template<class CType> const uint Cl22<CType>::iE2 = 2;
template<class CType> const uint Cl22<CType>::iE3 = 3;
template<class CType> const uint Cl22<CType>::iE4 = 4;
template<class CType> const uint Cl22<CType>::iL1 = 5;
template<class CType> const uint Cl22<CType>::iL2 = 6;
template<class CType> const uint Cl22<CType>::iL3 = 7;
template<class CType> const uint Cl22<CType>::iL1I = 8;
template<class CType> const uint Cl22<CType>::iL2I = 9;
template<class CType> const uint Cl22<CType>::iL3I = 10;
template<class CType> const uint Cl22<CType>::iE1I = 11;
template<class CType> const uint Cl22<CType>::iE2I = 12;
template<class CType> const uint Cl22<CType>::iE3I = 13;
template<class CType> const uint Cl22<CType>::iE4I = 14;
template<class CType> const uint Cl22<CType>::iI = 15;

#include "Cl22Tables.h"

////////////////////////////////////////////////////////////////////////////////////
/// Functions

template<class CType>
Cl22<CType>::Cl22() : Base<CType>(4,1,"e",ID_CL22)
{
	if (m_bInit) 
	{
		InitVars();
		m_bInit = false;
	}
}


// Cl22 Signature
template<class CType>
inline CType Cl22<CType>::signature(uint i) 
{ 
	if (i == 1 || i == 3) return CType(1);
	else return CType(-1);
}


////////////////////////////////////////////////////////////////////////////////////
/// Init Variables

template<class CType>
void Cl22<CType>::InitVars()
{
	BladeList<CType> Cl22Style(*this);
	
	BLADE.Reset(1);
	bSC.Reset(*this, CType(1));
	bE1.Reset(*this,CType(1),1,(BLADE<<1),"e1");
	bE2.Reset(*this,CType(1),1,(BLADE<<2),"e2");
	bE3.Reset(*this,CType(1),1,(BLADE<<3),"e3");
	bE4.Reset(*this,CType(1),1,(BLADE<<4),"e4");
	
	BLADE.Reset(2);
	bL1.Reset(*this,CType(1),2,(BLADE<<2<<3),"l1");
	bL2.Reset(*this,CType(1),2,(BLADE<<3<<1),"l2");
	bL3.Reset(*this,CType(1),2,(BLADE<<1<<2),"l3");
	bL1I.Reset(*this,CType(1),2,(BLADE<<4<<1),"l1I");
	bL2I.Reset(*this,CType(1),2,(BLADE<<4<<2),"l2I");
	bL3I.Reset(*this,CType(1),2,(BLADE<<4<<3),"l3I");
	
	BLADE.Reset(3);
	bE1I.Reset(*this,CType(1),3,(BLADE<<2<<3<<4),"e1I");
	bE2I.Reset(*this,CType(1),3,(BLADE<<3<<1<<4),"e2I");
	bE3I.Reset(*this,CType(1),3,(BLADE<<1<<2<<4),"e3I");
	bE4I.Reset(*this,CType(1),3,(BLADE<<3<<2<<1),"e4I");
	
	BLADE.Reset(4);
	bI.Reset(*this,CType(1),4,(BLADE<<1<<2<<3<<4),"I");
	
	Cl22Style <<bSC<<bE1<<bE2<<bE3<<bE4<<bL1<<bL2<<bL3;
	Cl22Style <<bL1I<<bL2I<<bL3I<<bE1I<<bE2I<<bE3I<<bE4I<<bI;

	// Initialises multiplication tables
	m_MVStyle.Init(Cl22Style);//, piGrades, piGPTable, piIPTable, piOPTable, 
					//piVITable, piDualTable); 
	
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


