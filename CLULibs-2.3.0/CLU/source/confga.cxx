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


// ConfGA Function and Class Definitions

#include "confga.h"
#include "confgainst.h"
#include "pga.h"

//#include "xmalib.h"


////////////////////////////////////////////////////////////////////////////////////
/// Static Variables

template<class CType> Blade<CType> ConfGA<CType>::bSC;
template<class CType> Blade<CType> ConfGA<CType>::bE1;
template<class CType> Blade<CType> ConfGA<CType>::bE2;
template<class CType> Blade<CType> ConfGA<CType>::bE3;
template<class CType> Blade<CType> ConfGA<CType>::bEp;
template<class CType> Blade<CType> ConfGA<CType>::bEm;
template<class CType> Blade<CType> ConfGA<CType>::bB1;
template<class CType> Blade<CType> ConfGA<CType>::bB2;
template<class CType> Blade<CType> ConfGA<CType>::bB3;
template<class CType> Blade<CType> ConfGA<CType>::bBp1;
template<class CType> Blade<CType> ConfGA<CType>::bBp2;
template<class CType> Blade<CType> ConfGA<CType>::bBp3;
template<class CType> Blade<CType> ConfGA<CType>::bBm1;
template<class CType> Blade<CType> ConfGA<CType>::bBm2;
template<class CType> Blade<CType> ConfGA<CType>::bBm3;
template<class CType> Blade<CType> ConfGA<CType>::bBpm;
template<class CType> Blade<CType> ConfGA<CType>::bTp1;
template<class CType> Blade<CType> ConfGA<CType>::bTp2;
template<class CType> Blade<CType> ConfGA<CType>::bTp3;
template<class CType> Blade<CType> ConfGA<CType>::bTm1;
template<class CType> Blade<CType> ConfGA<CType>::bTm2;
template<class CType> Blade<CType> ConfGA<CType>::bTm3;
template<class CType> Blade<CType> ConfGA<CType>::bTpm1;
template<class CType> Blade<CType> ConfGA<CType>::bTpm2;
template<class CType> Blade<CType> ConfGA<CType>::bTpm3;
template<class CType> Blade<CType> ConfGA<CType>::bT123;
template<class CType> Blade<CType> ConfGA<CType>::bQp;
template<class CType> Blade<CType> ConfGA<CType>::bQm;
template<class CType> Blade<CType> ConfGA<CType>::bQpm1;
template<class CType> Blade<CType> ConfGA<CType>::bQpm2;
template<class CType> Blade<CType> ConfGA<CType>::bQpm3;
template<class CType> Blade<CType> ConfGA<CType>::bI;

template<class CType> MultiV<CType> ConfGA<CType>::vSC;
template<class CType> MultiV<CType> ConfGA<CType>::vE1;
template<class CType> MultiV<CType> ConfGA<CType>::vE2;
template<class CType> MultiV<CType> ConfGA<CType>::vE3;
template<class CType> MultiV<CType> ConfGA<CType>::vEp;
template<class CType> MultiV<CType> ConfGA<CType>::vEm;
template<class CType> MultiV<CType> ConfGA<CType>::vE;
template<class CType> MultiV<CType> ConfGA<CType>::vE0;
template<class CType> MultiV<CType> ConfGA<CType>::vN;
template<class CType> MultiV<CType> ConfGA<CType>::vNb;
template<class CType> MultiV<CType> ConfGA<CType>::vB1;
template<class CType> MultiV<CType> ConfGA<CType>::vB2;
template<class CType> MultiV<CType> ConfGA<CType>::vB3;
template<class CType> MultiV<CType> ConfGA<CType>::vBp1;
template<class CType> MultiV<CType> ConfGA<CType>::vBp2;
template<class CType> MultiV<CType> ConfGA<CType>::vBp3;
template<class CType> MultiV<CType> ConfGA<CType>::vBm1;
template<class CType> MultiV<CType> ConfGA<CType>::vBm2;
template<class CType> MultiV<CType> ConfGA<CType>::vBm3;
template<class CType> MultiV<CType> ConfGA<CType>::vBpm;
template<class CType> MultiV<CType> ConfGA<CType>::vTp1;
template<class CType> MultiV<CType> ConfGA<CType>::vTp2;
template<class CType> MultiV<CType> ConfGA<CType>::vTp3;
template<class CType> MultiV<CType> ConfGA<CType>::vTm1;
template<class CType> MultiV<CType> ConfGA<CType>::vTm2;
template<class CType> MultiV<CType> ConfGA<CType>::vTm3;
template<class CType> MultiV<CType> ConfGA<CType>::vTpm1;
template<class CType> MultiV<CType> ConfGA<CType>::vTpm2;
template<class CType> MultiV<CType> ConfGA<CType>::vTpm3;
template<class CType> MultiV<CType> ConfGA<CType>::vT123;
template<class CType> MultiV<CType> ConfGA<CType>::vQp;
template<class CType> MultiV<CType> ConfGA<CType>::vQm;
template<class CType> MultiV<CType> ConfGA<CType>::vQpm1;
template<class CType> MultiV<CType> ConfGA<CType>::vQpm2;
template<class CType> MultiV<CType> ConfGA<CType>::vQpm3;
template<class CType> MultiV<CType> ConfGA<CType>::vI;

template<class CType> MultiV<CType> ConfGA<CType>::vZero;
template<class CType> RingBuf<uint> ConfGA<CType>::BLADE;

template<class CType> MultiVStyle<CType> ConfGA<CType>::m_MVStyle;

// ConfGA needs initialisation
template<class CType> bool ConfGA<CType>::m_bInit = true;

template<class CType> const uint ConfGA<CType>::iSC = 0;
template<class CType> const uint ConfGA<CType>::iE1 = 1;
template<class CType> const uint ConfGA<CType>::iE2 = 2;
template<class CType> const uint ConfGA<CType>::iE3 = 3;
template<class CType> const uint ConfGA<CType>::iEp = 4;
template<class CType> const uint ConfGA<CType>::iEm = 5;
template<class CType> const uint ConfGA<CType>::iB1 = 6;
template<class CType> const uint ConfGA<CType>::iB2 = 7;
template<class CType> const uint ConfGA<CType>::iB3 = 8;
template<class CType> const uint ConfGA<CType>::iBp1 = 9;
template<class CType> const uint ConfGA<CType>::iBp2 = 10;
template<class CType> const uint ConfGA<CType>::iBp3 = 11;
template<class CType> const uint ConfGA<CType>::iBm1 = 12;
template<class CType> const uint ConfGA<CType>::iBm2 = 13;
template<class CType> const uint ConfGA<CType>::iBm3 = 14;
template<class CType> const uint ConfGA<CType>::iBpm = 15;
template<class CType> const uint ConfGA<CType>::iTp1 = 16;
template<class CType> const uint ConfGA<CType>::iTp2 = 17;
template<class CType> const uint ConfGA<CType>::iTp3 = 18;
template<class CType> const uint ConfGA<CType>::iTm1 = 19;
template<class CType> const uint ConfGA<CType>::iTm2 = 20;
template<class CType> const uint ConfGA<CType>::iTm3 = 21;
template<class CType> const uint ConfGA<CType>::iTpm1 = 22;
template<class CType> const uint ConfGA<CType>::iTpm2 = 23;
template<class CType> const uint ConfGA<CType>::iTpm3 = 24;
template<class CType> const uint ConfGA<CType>::iT123 = 25;
template<class CType> const uint ConfGA<CType>::iQp = 26;
template<class CType> const uint ConfGA<CType>::iQm = 27;
template<class CType> const uint ConfGA<CType>::iQpm1 = 28;
template<class CType> const uint ConfGA<CType>::iQpm2 = 29;
template<class CType> const uint ConfGA<CType>::iQpm3 = 30;
template<class CType> const uint ConfGA<CType>::iI = 31;


#include "ConfGATables.h"


////////////////////////////////////////////////////////////////////////////////////
/// Functions

template<class CType>
ConfGA<CType>::ConfGA() : Base<CType>(5,1,"e",ID_CONFGA)
{
	if (m_bInit) 
	{
		InitVars();
		m_bInit = false;
	}
}


// ConfGA Signature
template<class CType>
inline CType ConfGA<CType>::signature(uint i) 
{ 
	return (i < 5 ? CType(1) : CType(-1)); 
}


////////////////////////////////////////////////////////////////////////////////////
/// Init Variables

template<class CType>
void ConfGA<CType>::InitVars()
{
	BladeList<CType> ConfGAStyle(*this);
	
	BLADE.Reset(1);
	bSC.Reset(*this,CType(1));
	bE1.Reset(*this,CType(1),1,(BLADE<<1),"e1");
	bE2.Reset(*this,CType(1),1,(BLADE<<2),"e2");
	bE3.Reset(*this,CType(1),1,(BLADE<<3),"e3");
	bEp.Reset(*this,CType(1),1,(BLADE<<4),"e+");
	bEm.Reset(*this,CType(1),1,(BLADE<<5),"e-");
	
	BLADE.Reset(2);
	bB1.Reset(*this,CType(1),2,(BLADE<<2<<3),"e23");
	bB2.Reset(*this,CType(1),2,(BLADE<<3<<1),"e31");
	bB3.Reset(*this,CType(1),2,(BLADE<<1<<2),"e12");

	bBp1.Reset(*this,CType(1),2,(BLADE<<1<<4),"e1+");
	bBp2.Reset(*this,CType(1),2,(BLADE<<2<<4),"e2+");
	bBp3.Reset(*this,CType(1),2,(BLADE<<3<<4),"e3+");

	bBm1.Reset(*this,CType(1),2,(BLADE<<1<<5),"e1-");
	bBm2.Reset(*this,CType(1),2,(BLADE<<2<<5),"e2-");
	bBm3.Reset(*this,CType(1),2,(BLADE<<3<<5),"e3-");

	bBpm.Reset(*this,CType(1),2,(BLADE<<4<<5),"E");
	
	BLADE.Reset(3);
	bTp1.Reset(*this,CType(1),3,(BLADE<<2<<3<<4),"e23+");
	bTp2.Reset(*this,CType(1),3,(BLADE<<3<<1<<4),"e31+");
	bTp3.Reset(*this,CType(1),3,(BLADE<<1<<2<<4),"e12+");

	bTm1.Reset(*this,CType(1),3,(BLADE<<2<<3<<5),"e23-");
	bTm2.Reset(*this,CType(1),3,(BLADE<<3<<1<<5),"e31-");
	bTm3.Reset(*this,CType(1),3,(BLADE<<1<<2<<5),"e12-");

	bTpm1.Reset(*this,CType(1),3,(BLADE<<1<<4<<5),"e1E");
	bTpm2.Reset(*this,CType(1),3,(BLADE<<2<<4<<5),"e2E");
	bTpm3.Reset(*this,CType(1),3,(BLADE<<3<<4<<5),"e3E");

	bT123.Reset(*this,CType(1),3,(BLADE<<1<<2<<3),"e123");

	BLADE.Reset(4);
	bQp.Reset(*this,CType(1),4,(BLADE<<1<<2<<3<<4),"e123+");
	bQm.Reset(*this,CType(1),4,(BLADE<<1<<2<<3<<5),"e123-");
	bQpm1.Reset(*this,CType(1),4,(BLADE<<2<<3<<4<<5),"e23E");
	bQpm2.Reset(*this,CType(1),4,(BLADE<<3<<1<<4<<5),"e31E");
	bQpm3.Reset(*this,CType(1),4,(BLADE<<1<<2<<4<<5),"e12E");

	BLADE.Reset(5);
	bI.Reset(*this,CType(1),5,(BLADE<<1<<2<<3<<4<<5),"I");
	
	ConfGAStyle <<bSC<<bE1<<bE2<<bE3<<bEp<<bEm;
	ConfGAStyle <<bB1<<bB2<<bB3<<bBp1<<bBp2<<bBp3<<bBm1<<bBm2<<bBm3<<bBpm;
	ConfGAStyle <<bTp1<<bTp2<<bTp3<<bTm1<<bTm2<<bTm3<<bTpm1<<bTpm2<<bTpm3<<bT123;
	ConfGAStyle <<bQp<<bQm<<bQpm1<<bQpm2<<bQpm3<<bI;
	
	// Initialises multiplication tables
	m_MVStyle.Init(ConfGAStyle, piGrades, piGPTable, piIPTable, 
					piOPTable, piVITable, piDualTable); 
	
	vZero.SetStyle(m_MVStyle);

	vSC.SetStyle(m_MVStyle); vSC[iSC] = 1;

	vE1.SetStyle(m_MVStyle); vE1[iE1] = 1;
	vE2.SetStyle(m_MVStyle); vE2[iE2] = 1;
	vE3.SetStyle(m_MVStyle); vE3[iE3] = 1;
	vEp.SetStyle(m_MVStyle); vEp[iEp] = 1;
	vEm.SetStyle(m_MVStyle); vEm[iEm] = 1;

	vE.SetStyle(m_MVStyle); vE[iEp] = CType(1); vE[iEm] = CType(1); // e = e+ + e-
	vE0.SetStyle(m_MVStyle); vE0[iEm] = CType(0.5); vE0[iEp] = CType(-0.5); // e0 = 0.5 (e- - e+)

	vN.SetStyle(m_MVStyle); vN = vE; // n = e = e+ + e-
	vNb.SetStyle(m_MVStyle); vNb = vE0; // nb = e0 = 0.5 (e- - e+)
	
	vB1.SetStyle(m_MVStyle); vB1[iB1] = 1;
	vB2.SetStyle(m_MVStyle); vB2[iB2] = 1;
	vB3.SetStyle(m_MVStyle); vB3[iB3] = 1;
	vBp1.SetStyle(m_MVStyle); vBp1[iBp1] = 1;
	vBp2.SetStyle(m_MVStyle); vBp2[iBp2] = 1;
	vBp3.SetStyle(m_MVStyle); vBp3[iBp3] = 1;
	vBm1.SetStyle(m_MVStyle); vBm1[iBm1] = 1;
	vBm2.SetStyle(m_MVStyle); vBm2[iBm2] = 1;
	vBm3.SetStyle(m_MVStyle); vBm3[iBm3] = 1;
	vBpm.SetStyle(m_MVStyle); vBpm[iBpm] = 1;

	vTp1.SetStyle(m_MVStyle); vTp1[iTp1] = 1;
	vTp2.SetStyle(m_MVStyle); vTp2[iTp2] = 1;
	vTp3.SetStyle(m_MVStyle); vTp3[iTp3] = 1;
	vTm1.SetStyle(m_MVStyle); vTm1[iTm1] = 1;
	vTm2.SetStyle(m_MVStyle); vTm2[iTm2] = 1;
	vTm3.SetStyle(m_MVStyle); vTm3[iTm3] = 1;
	vTpm1.SetStyle(m_MVStyle); vTpm1[iTpm1] = 1;
	vTpm2.SetStyle(m_MVStyle); vTpm2[iTpm2] = 1;
	vTpm3.SetStyle(m_MVStyle); vTpm3[iTpm3] = 1;
	vT123.SetStyle(m_MVStyle); vT123[iT123] = 1;

	vQp.SetStyle(m_MVStyle); vQp[iQp] = 1;
	vQm.SetStyle(m_MVStyle); vQm[iQm] = 1;
	vQpm1.SetStyle(m_MVStyle); vQpm1[iQpm1] = 1;
	vQpm2.SetStyle(m_MVStyle); vQpm2[iQpm2] = 1;
	vQpm3.SetStyle(m_MVStyle); vQpm3[iQpm3] = 1;
	
	vI.SetStyle(m_MVStyle); vI[iI] = 1;

}

// Additional Functions -----------------------------


////////////////////////////////////////////////////////////////////////////////////
// Make Euclidean Vector from Null Vector
//
// This is done by taking the first three components and dividing
// them through by -(vA * vE).

template<class CType>
MultiV<CType> ConfGA<CType>::MakeEuclidVec(const MultiV<CType> &vA, CType fPrec)
{
	MultiV<CType> vB(m_MVStyle);
	CType dFac;

	if (fPrec <= 0)
		Tiny(fPrec);

	vB[iE1] = vA[iE1];
	vB[iE2] = vA[iE2];
	vB[iE3] = vA[iE3];

	dFac = -Scalar(vA * vE);

	if (!::IsZero(dFac, fPrec))
	{
		vB /= dFac;
	}

	return vB;
}

////////////////////////////////////////////////////////////////////////////////////
// Make Null Vector

template<class CType>
MultiV<CType> ConfGA<CType>::NullVec(const CType &a1, const CType &a2, const CType &a3)
{
	CType a[5];
	
	a[0] = a1;
	a[1] = a2;
	a[2] = a3;

	// e part is 0.5 a^2
	a[3] = a[4] = CType(0.5) * (a1*a1 + a2*a2 + a3*a3);

	// e0 part is 1
	a[3] -= CType(0.5);
	a[4] += CType(0.5);
	
	return MakeVector(m_MVStyle,1,a);
}


////////////////////////////////////////////////////////////////////////////////////
// Make Null Vector

template<class CType>
MultiV<CType> ConfGA<CType>::NullVec(const CType *pA)
{
	CType a[5];
	
	a[0] = pA[0];
	a[1] = pA[1];
	a[2] = pA[2];

	// e part is 0.5 a^2
	a[3] = a[4] = CType(0.5) * (a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);

	// e0 part is 1
	a[3] -= CType(0.5);
	a[4] += CType(0.5);
	
	return MakeVector(m_MVStyle,1,a);
}



////////////////////////////////////////////////////////////////////////////////////
// Make Null Vector from spherical coordinates

template<class CType>
MultiV<CType> ConfGA<CType>::NullSVec(const CType &r, const CType &theta, const CType &phi)
{
	CType a[5];
	
	a[0] = r * CType(sin(theta)) * CType(cos(phi));
	a[1] = r * CType(sin(theta)) * CType(sin(phi));
	a[2] = r * CType(cos(theta));

	// e part is 0.5 a^2
	a[3] = a[4] = CType(0.5) * (a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);

	// e0 part is 1
	a[3] -= CType(0.5);
	a[4] += CType(0.5);
	
	return MakeVector(m_MVStyle,1,a);
}


////////////////////////////////////////////////////////////////////////////////////
// Make Direction Vector

template<class CType>
MultiV<CType> ConfGA<CType>::DirVec(const CType &a1, const CType &a2, const CType &a3)
{
	CType a[5];
	
	a[0] = a1;
	a[1] = a2;
	a[2] = a3;
	a[3] = a[4] = 0.0; // e and e0 part are 0.

	return MakeVector(m_MVStyle,1,a);
}


////////////////////////////////////////////////////////////////////////////////////
// Make Direction Vector from spherical coordinates

template<class CType>
MultiV<CType> ConfGA<CType>::DirSVec(const CType &r, const CType &theta, const CType &phi)
{
	CType a[5];
	
	a[0] = r * CType(sin(theta)) * CType(cos(phi));
	a[1] = r * CType(sin(theta)) * CType(sin(phi));
	a[2] = r * CType(cos(theta));
	a[3] = a[4] = 0.0; // e and e0 part are 0.

	return MakeVector(m_MVStyle,1,a);
}


////////////////////////////////////////////////////////////////////////////////////
// Renormalize Null Vector

template<class CType>
MultiV<CType> ConfGA<CType>::RenormNullVec(const MultiV<CType> &vA, CType fPrec)
{
	MultiV<CType> vX(vA);

	if (fPrec <= 0)
		Tiny(fPrec);

	CType dNb = -Scalar(vX * vE);

	if (!::IsZero(dNb, fPrec))
		vX /= dNb;

	CType a1 = vX[iE1];
	CType a2 = vX[iE2];
	CType a3 = vX[iE3];
	CType ap, am;

	// e part is 0.5 a^2
	ap = am = CType(0.5) * (a1*a1 + a2*a2 + a3*a3);

	// e0 part is 1
	ap -= CType(0.5);
	am += CType(0.5);

	vX[iEp] = ap;
	vX[iEm] = am;

	return vX;
}


////////////////////////////////////////////////////////////////////////////////////
// Make Sphere

template<class CType>
MultiV<CType> ConfGA<CType>::Sphere(const CType &a1, const CType &a2, const CType &a3, const CType &rad)
{
	CType a[5];
	
	a[0] = a1;
	a[1] = a2;
	a[2] = a3;

	// e part is 0.5 a^2 - 0.5 rad^2
	a[3] = a[4] = CType(0.5) * ((a1*a1 + a2*a2 + a3*a3) - rad*rad);

	// e0 part is 1
	a[3] -= CType(0.5);
	a[4] += CType(0.5);
	
	return MakeVector(m_MVStyle,1,a);
}


////////////////////////////////////////////////////////////////////////////////////
// Make Sphere

template<class CType>
MultiV<CType> ConfGA<CType>::Sphere(const CType *pA, const CType &rad)
{
	CType a[5];
	
	a[0] = pA[0];
	a[1] = pA[1];
	a[2] = pA[2];

	// e part is 0.5 a^2 - 0.5 rad^2
	a[3] = a[4] = CType(0.5) * ((a[0]*a[0] + a[1]*a[1] + a[2]*a[2]) - rad*rad);

	// e0 part is 1
	a[3] -= CType(0.5);
	a[4] += CType(0.5);
	
	return MakeVector(m_MVStyle,1,a);
}


////////////////////////////////////////////////////////////////////////////////////
// Make Translator (Translation Operator)
// T = 1 - 0.5 a & e, translates by +a

template<class CType>
MultiV<CType> ConfGA<CType>::Translator(const CType &a1, const CType &a2, const CType &a3)
{
	MultiV<CType> vA(m_MVStyle);

	vA[iSC] = 1;
	vA[iBp1] = vA[iBm1] = -CType(0.5)*a1; 
	vA[iBp2] = vA[iBm2] = -CType(0.5)*a2; 
	vA[iBp3] = vA[iBm3] = -CType(0.5)*a3;
	
	return vA;
}

template<class CType>
MultiV<CType> ConfGA<CType>::Translator(const MultiV<CType> &vX)
{
	MultiV<CType> vA(m_MVStyle);

	vA[iSC] = 1;
	vA[iBp1] = vA[iBm1] = -CType(0.5)*vX[iE1]; 
	vA[iBp2] = vA[iBm2] = -CType(0.5)*vX[iE2]; 
	vA[iBp3] = vA[iBm3] = -CType(0.5)*vX[iE3];
	
	return vA;
}


////////////////////////////////////////////////////////////////////////////////////
// Make Rotor
// a is rotation axis, rad is rotation angle

template<class CType>
MultiV<CType> ConfGA<CType>::Rotor(const CType &a1, const CType &a2, const CType &a3, 
								   const CType &rad, CType fPrec)
{
	MultiV<CType> vA(m_MVStyle);

	if (fPrec <= 0)
		Tiny(fPrec);

	vA[iE1] = a1;
	vA[iE2] = a2;
	vA[iE3] = a3;

	vA /= CType(sqrt(a1*a1 + a2*a2 + a3*a3));
	vA &= ~vT123;

	vA = CType(cos(rad/2.0)) + (CType(sin(rad/2.0)) & vA);

	vA.TinyToZero(fPrec);

	return vA;
}

template<class CType>
MultiV<CType> ConfGA<CType>::Rotor(const MultiV<CType> &vX, const CType &rad, 
								bool bAxis, CType fPrec)
{
	MultiV<CType> vA(m_MVStyle);

	if (fPrec <= 0)
		Tiny(fPrec);

	if (bAxis)
	{
		// Assume vX is rotation axis
		vA[iE1] = vX[iE1];
		vA[iE2] = vX[iE2];
		vA[iE3] = vX[iE3];
	
		vA /= CType(sqrt(Scalar(vA * vA)));
		vA &= ~vT123; // Rotation plane is U = x I3, and x = U I3^(-1)
	}
	else
	{
		// Assume vX is rotation plane
		vA[iB1] = vX[iB1];
		vA[iB2] = vX[iB2];
		vA[iB3] = vX[iB3];
	
		vA /= CType(sqrt(Scalar(vA * (~vA))));
	}

	vA = CType(cos(rad/2.0)) - (CType(sin(rad/2.0)) & vA);

	vA.TinyToZero(fPrec);

	return vA;
}

////////////////////////////////////////////////////////////////////////////////////
// Make Dilator (Dilation Operator)

template<class CType>
MultiV<CType> ConfGA<CType>::Dilator(const CType &rho)
{
	MultiV<CType> vA(m_MVStyle);
//	CType rootRho = CType(sqrt(CType(rho)));

	vA[iSC] = CType(0.5) + CType(0.5)*rho;
	vA[iBpm] = CType(0.5) - CType(0.5)*rho;
	
	return vA;
}


////////////////////////////////////////////////////////////////////////////////////
/// Print Vector

template<class CType>
cStr ConfGA<CType>::MakeStrVector(const MultiV<CType>& vA)
{
	cStr csVec;
	CType dE, dE0, dVal;

	dE = -Scalar(vA * vE0);
	dE0 = -Scalar(vA * vE);

	csVec = "[ ";
	if ((dVal = vA[iE1]) != CType(0))
		csVec << MakeStr(dVal) << "^e1 + ";

	if ((dVal = vA[iE2]) != CType(0))
		csVec << MakeStr(dVal) << "^e2 + ";
	
	if ((dVal = vA[iE3]) != CType(0))
		csVec << MakeStr(dVal) << "^e3 + ";

	if (dE != CType(0))
		csVec << MakeStr(dE) << "^e + ";

	if (dE0 != CType(0))
		csVec << MakeStr(dE0) << "^e0 ";

	csVec << "]";

	return csVec;
}

////////////////////////////////////////////////////////////////////////////////////
// Make Biector
/*
template<class CType>
MultiV<CType> ConfGA<CType>::Bivec(const CType &a1, const CType &a2, const CType &a3)
{
	CType a[4];
	
	a[0] = a1;
	a[1] = a2;
	a[2] = a3;
	
	return MakeVector(m_MVStyle,2,a);
}

*/

////////////////////////////////////////////////////////////////////////////////////
// Analyse Multivector. 
// Result is returned in rMVI.
// If function returns false, MV could not be analyzed.

template<class CType>
bool ConfGA<CType>::AnalyzeMV(const MultiV<CType>& vX, CMVInfo<CType>& rMVI, 
							  bool bOPNS, CType fPrec)
{
	MultiV<CType> vA(vX);
	Mem<uint> mGList;
	int iGrades;

	if (!GradeList(vA, mGList, fPrec)) return false;
	if ((iGrades = mGList.Count()) == 0 || iGrades > 3) return false;

	if (iGrades == 1)
	{
		return AnalyzeBlade(vA, rMVI, bOPNS, fPrec);
	}
	else
	{
		return AnalyzeVersor(vA, rMVI, fPrec);
	}

	return false;
}


////////////////////////////////////////////////////////////////////////////////////
// Analyse Blade.
// Result is returned in rMVI.
// If function returns false, MV could not be analyzed.
// bOPNS == true: Analyze oter Product Null Space of blade, 
// otherwise inner product null space.

template<class CType>
bool ConfGA<CType>::AnalyzeBlade(const MultiV<CType>& vA, CMVInfo<CType>& rMVI, 
								 bool bOPNS, CType fPrec)
{
//	COGLVertex pVex[4], pActVex[4];

	PGA<CType> P3Base;
	MultiV<CType> vX(vA), vP, vL, vC, vS, vU, vD;
	MemObj<MultiV<CType> > mvList;
	int piOrder[] = { 3, 2, 1, 0 };
	CType fR, fFac, fMag;
	CType fE, fE0;
	CType fZeroMin, fZeroMax, fUnityMin, fUnityMax;

	if (fPrec <= CType(0))
		Tiny(fPrec);

	Mem<uint> mGList;
	if (!GradeList(vX, mGList, fPrec)) return false;
	if (mGList.Count() != 1) return false;
	int iGrade = mGList[0];

	if (iGrade < 0 || iGrade > 5)
		return false;

	if (bOPNS)
		iGrade = 5 - iGrade;

	fZeroMin = CType(0) - fPrec;
	fZeroMax = CType(0) + fPrec;

	fUnityMin = CType(1) - fPrec;
	fUnityMax = CType(1) + fPrec;

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

		vD = NullVec(vX[1], vX[2], vX[3]);
		vC = vX - vD;

		fE = -Scalar(vC * vE0);
		fE0 = -Scalar(vC * vE);

		//printf("fE0: %le, fE: %le\n", fE0, fE);
		//printf("UMin: %le, UMax: %le\n", fUnityMin, fUnityMax);

		// Single Point
		if ((fE >= fZeroMin && fE <= fZeroMax) && 
			(fE0 >= fZeroMin && fE0 <= fZeroMax))
		{
			// Type = GA_POINT
			// Vex[0] = point

			rMVI.m_eType = GA_POINT;
			rMVI.m_mVex.Set(1);

			rMVI.m_mVex[0] = &vD[iE1];
		}
		// Plane
		else if (fE0 <= -fUnityMin && fE0 >= -fUnityMax)//, printf("fE0 = %f\n", fE0))
		{
			// Type = GA_PLANE
			// Vex[0] = plumb vector
			// Vex[1..2] = dir. vectors
			// Val[0] = magnitude

			vP = P3Base.vZero;

			vP[P3Base.iE1I] = vX[iE1];
			vP[P3Base.iE2I] = vX[iE2];
			vP[P3Base.iE3I] = vX[iE3];

			vP[P3Base.iE4I] = Scalar(vX * vNb);

			//printf("fE0 = %le, vP = %s\n", fE0, vP.Str());
			P3Base.AnalyzeBlade(vP, rMVI, true);
		}
		// Sphere or nothing
		else
		{
			// Type = GA_SPHERE
			// Vex[0] = center
			// Val[0] = radius
			CType fVal = CType(1.0 + fE0);

			CType fN2 = vD[1]*vD[1] + vD[2]*vD[2] + vD[3]*vD[3];
			CType fR = CType(-(2.0 * fE / fVal + fE0 * fN2 / (fVal*fVal)));
			CType fV = CType(1.0 / fVal);

			if (fR < fZeroMin || fR > fZeroMax)
			{
				rMVI.m_eType = GA_SPHERE;
				rMVI.m_mVex.Set(1);
				rMVI.m_mVal.Set(1);

				rMVI.m_mVex[0] = &vD[iE1];
				rMVI.m_mVex[0] *= fV;

				if (fR > CType(0))
					rMVI.m_mVal[0] = CType(sqrt(fR));
				else
					rMVI.m_mVal[0] = CType(-sqrt(-fR));
			}
			else
			{
				rMVI.m_eType = GA_POINT;
				rMVI.m_mVex.Set(1);

				rMVI.m_mVex[0] = &vD[iE1];
				rMVI.m_mVex[0] *= fV;
			}
		}

	}
	else if (iGrade == 2)
	{
		if (bOPNS)
			vX = *vX;

		// Intersection Plane as IP null space
		vP = vX * vN;
		vP.TinyToZero(fPrec);

		// Test whether vX is a line
		if (vP.IsZero(fPrec))
		{
			vL = vX ^vN;
			vL.TinyToZero(fPrec);

			if (vL.IsZero(fPrec))
			{
				// Direction plane through origin
				// This object has no real Euclidean IPNS.
				// Can be interpreted as direction plane

				// Type = GA_INF_LINE
				// Vex[0] = plumb vector
				// Vex[1..2] = dir. vectors
				// Val[0] = magnitude

				vP = P3Base.vZero;

				vP[P3Base.iE1I] = vX[iBp1];
				vP[P3Base.iE2I] = vX[iBp2];
				vP[P3Base.iE3I] = vX[iBp3];
				vP[P3Base.iE4I] = 0;

				//printf("fE0 = %le, vP = %s\n", fE0, vP.Str());
				P3Base.AnalyzeBlade(vP, rMVI, true /* OPNS */, fPrec);
				rMVI.m_eType = GA_INF_LINE;
			}
			else
			{
				// Line

				// Type = GA_LINE
				// Vex[0] = plumb vector
				// Vex[1] = dir. vector
				// Val[0] = magnitude

				vL = P3Base.vZero;

				vL[P3Base.iL1] = vX[iB1];
				vL[P3Base.iL2] = vX[iB2];
				vL[P3Base.iL3] = vX[iB3];

				vL[P3Base.iL1I] = vX[iBp1];
				vL[P3Base.iL2I] = vX[iBp2];
				vL[P3Base.iL3I] = vX[iBp3];
				/*
				vL[P3Base.iL1] = vX[iBp1];
				vL[P3Base.iL2] = vX[iBp2];
				vL[P3Base.iL3] = vX[iBp3];

				vL[P3Base.iL1I] = vX[iB1];
				vL[P3Base.iL2I] = vX[iB2];
				vL[P3Base.iL3I] = vX[iB3];
				*/
				P3Base.AnalyzeBlade(vL, rMVI, false /* IPNS */, fPrec);
			}
		}
		else
		{
			// Line through centers of spheres as OP null space
			vL = vX ^ vN;
			vL.TinyToZero(fPrec);
			
			if (vL.IsZero(fPrec))
			{
				// This object has no real Euclidean IPNS.
				// Can be interpreted as direction plane

				// Type = GA_INF_LINE
				// Vex[0] = plumb vector
				// Vex[1..2] = dir. vectors
				// Val[0] = magnitude

				vP = P3Base.vZero;

				vP[P3Base.iE1I] = vX[iBp1];
				vP[P3Base.iE2I] = vX[iBp2];
				vP[P3Base.iE3I] = vX[iBp3];
				vP[P3Base.iE4I] = -vX[iBpm];

				//printf("fE0 = %le, vP = %s\n", fE0, vP.Str());
				P3Base.AnalyzeBlade(vP, rMVI, true /* OPNS */, fPrec);
				rMVI.m_eType = GA_INF_LINE;
			}
			else
			{
				// Circle

				// Type = GA_CIRCLE
				// Vex[0] = center
				// Vex[1] = normal to plane of circle
				// Val[2] = radius

				// Center of intersection circle in form X^n
				vC = (vP * vL) * vNb;
				vC.TinyToZero(fPrec);
				vC = RenormNullVec(vC);

				// Radius squared of Circle
				fR = -Scalar(vX * vX) / Scalar(vP * vP);

				if (fR < fZeroMin || fR > fZeroMax)
				{
					rMVI.m_eType = GA_CIRCLE;
					rMVI.m_mVex.Set(2);
					rMVI.m_mVal.Set(1);

					if ((fMag = vP[iE1]*vP[iE1] + vP[iE2]*vP[iE2] + vP[iE3]*vP[iE3]) == CType(0))
					{
						fMag = CType(1);
					}
					else
					{
						fMag = CType(sqrt(fMag));
					}

					rMVI.m_mVex[0] = &vC[iE1];
					rMVI.m_mVex[1][0] = vP[iE1] / fMag;
					rMVI.m_mVex[1][1] = vP[iE2] / fMag;
					rMVI.m_mVex[1][2] = vP[iE3] / fMag;

					if (fR > CType(0))
						rMVI.m_mVal[0] = CType(sqrt(fR));
					else
						rMVI.m_mVal[0] = CType(-sqrt(-fR));
					/*
					printf("\nCircle:\n");
					printf("    Radius: %f\n", fR);
					printf("    Center: (%f, %f, %f)\n", rMVI.m_mVex[0][0], rMVI.m_mVex[0][1], rMVI.m_mVex[0][2]);
					printf("    Normal: (%f, %f, %f)\n", rMVI.m_mVex[1][0], rMVI.m_mVex[1][1], rMVI.m_mVex[1][2]);
					*/
				}
				else
				{
					rMVI.m_eType = GA_POINT;
					rMVI.m_mVex.Set(1);

					rMVI.m_mVex[0] = &vC[iE1];
				}
			}
		}
	}
	else if (iGrade == 3)
	{
		if (!bOPNS)
			vX = *vX;

		vC = vX ^ vN;
		vC.TinyToZero(fPrec);

		if (vC.IsZero(fPrec))
		{
			// Single Point
			// Type = GA_POINT
			// Vex[0] = point

			vC = -vX * vNb;
			vC.TinyToZero(fPrec);

			fFac = -Scalar(vC * vN); 

			if (fFac < fZeroMin || fFac > fZeroMax)
			{
				vC /= fFac;

				rMVI.m_eType = GA_POINT;
				rMVI.m_mVex.Set(1);
				rMVI.m_mVex[0] = &vC[iE1];
			}
			else
			{
				// Direction Vector
				// Type = GA_DIR1D
				// Vex[0] = vector
				CType fMag;

				fMag = CType(sqrt(vC[iE1]*vC[iE1] + vC[iE2]*vC[iE2] + vC[iE3]*vC[iE3]));

				if (fMag < fZeroMin || fMag > fZeroMax)
					vC = vC / fMag;
				else
					fMag = 0;

				rMVI.m_eType = GA_DIR1D;
				rMVI.m_mVex.Set(1);
				rMVI.m_mVal.Set(1);
				rMVI.m_mVex[0] = &vC[iE1];
				rMVI.m_mVal[0] = fMag;
			}
		}
		else
		{
			// Point Pair or Point

			CType fLen;
			// Point Pair
			// Type = GA_POINTPAIR
			// Vex[0] = Central Point
			// Vex[1] = Normalized Direction Vector
			// Val[0] = Distance of Points from center

			CPointND<CType, 3> xC, xD;

			//fLen = 0.5f * sqrt(fLen);

			// Median Plane between points
			vP = vX * vN;
			vP.TinyToZero(fPrec);

			if (vP.IsZero(fPrec))
			{
				// Line
				// Type = GA_INF_POINTPAIR
				// Vex[0] = plumb vector
				// Vex[1] = dir. vector
				// Val[0] = magnitude

				vL = P3Base.vZero;

				vL[P3Base.iL1] = vX[iB1];
				vL[P3Base.iL2] = vX[iB2];
				vL[P3Base.iL3] = vX[iB3];

				vL[P3Base.iL1I] = vX[iBp1];
				vL[P3Base.iL2I] = vX[iBp2];
				vL[P3Base.iL3I] = vX[iBp3];

				P3Base.AnalyzeBlade(vL, rMVI, true /* OPNS */, fPrec);
				rMVI.m_eType = GA_INF_POINTPAIR;
			}
			else
			{
				// Line connecting intersection points
				vL = vX ^ vN;
				vL.TinyToZero(fPrec);

				// Intersection of vL and vP
				vC = (vP * vL) * vNb;
				vC.TinyToZero(fPrec);

				fFac = -Scalar(vC * vN);
				vC /= fFac;

				//vC = RenormNullVec(vC);
				xC = &vC[iE1];

				// Direction of Line
				vD = (vX * vN);
				vD.TinyToZero(fPrec);

				fFac = CType(sqrt(Scalar(vD * vD)));
				if (!::IsZero(fFac, fPrec))
					vD /= fFac;
				xD = &vD[iE1];
				//printf("X = %s\nL = %s\nD2 = %s\n", vX.Str(), vL.Str(), vD.Str());

				fLen = Scalar(vX * vX) / Scalar(vL * vL);

				// Circle through Origin that passes through intersection points
				//vS = *(vX ^ vNb);
				//vS.TinyToZero(fPrec);

				//vD = -vS * vN;
				//vD.TinyToZero(fPrec);

				// Sphere through Origin that passes through intersection points
				//fFac = Round(Scalar(vD * vD), fPrec);
				//if (fFac != CType(0))
				//vS = (vD * vS) / fFac;

				//vS.TinyToZero(fPrec);

				// Distance from vC to intersection points
				//fLen = Round(CType(sqrt(2.0 * Scalar(vC * vS))), fPrec);

				if (fLen < fZeroMin || fLen > fZeroMax)
				{
					rMVI.m_eType = GA_POINTPAIR;
					rMVI.m_mVex.Set(2);
					rMVI.m_mVal.Set(1);

					rMVI.m_mVex[0] = xC;
					rMVI.m_mVex[1] = xD;
					if (fLen < CType(0))
						rMVI.m_mVal[0] = -CType(sqrt(-fLen));
					else
						rMVI.m_mVal[0] = CType(sqrt(fLen));
				}
				else
				{
					rMVI.m_eType = GA_POINT;
					rMVI.m_mVex.Set(1);

					rMVI.m_mVex[0] = (xC);
				}
			}
		}
	}
	else if (iGrade == 4)
	{
		// Point
		// Type = GA_POINT
		// Vex[0] = point

		if (!bOPNS)
			vX = *vX;
		
		fFac = -Scalar(vX * vN);
		if (fFac < fZeroMin || fFac > fZeroMax)
		{
			vX /= fFac;
			
			vD = vX - NullVec(vX[1], vX[2], vX[3]);
			vD.TinyToZero(fPrec);
			if (vD.IsZero(fPrec))
			{
				rMVI.m_eType = GA_POINT;
				rMVI.m_mVex.Set(1);

				rMVI.m_mVex[0] = &vX[iE1];
			}
		}
		else
		{
			// Direction Vector
			// Type = GA_DIR1D
			// Vex[0] = vector
			CType fMag;

			fMag = CType(sqrt(vX[iE1]*vX[iE1] + vX[iE2]*vX[iE2] + vX[iE3]*vX[iE3]));

			if (fMag < fZeroMin || fMag > fZeroMax)
				vX = vX / fMag;
			else
				fMag = 0;

			rMVI.m_eType = GA_DIR1D;
			rMVI.m_mVex.Set(1);
			rMVI.m_mVal.Set(1);
			rMVI.m_mVex[0] = &vX[1];
			rMVI.m_mVal[0] = fMag;
		}
	}
	else if (iGrade == 5)
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
// Analyse Motor
//
// IMPORTANT: Can only analyze motors of type M = TR(~T),
// and Translators T.
//
// Result is returned in rMVI.
// If function returns false, MV could not be analyzed.
//
// Type = GA_MOTOR
// Vex[0] = Offset vector to rotation axis
// Vex[1] = Rotation axis
// Val[0] = Rotation angle

template<class CType>
bool ConfGA<CType>::AnalyzeVersor(const MultiV<CType>& _vX, CMVInfo<CType>& rMVI, CType fPrec)
{
	MultiV<CType> vU, vA, vX;
	CType fU2, fFac;
	Mem<uint> mGList;
//	int iGrades;

	if (fPrec <= CType(0))
		Tiny(fPrec);


	fFac = sqrt(Scalar(_vX & (~_vX)));
	if (::IsZero(fFac, fPrec))
		return false;

	vX = _vX / fFac;

	if (!GradeList(vX, mGList, fPrec)) return false;
	
	if (mGList.Count() == 2 && mGList[0] == 2 && mGList[1] == 0)
	{
		// Get Rotation Plane Bivector Part
		vU = vZero;
		vU[iB1] = vX[iB1];
		vU[iB2] = vX[iB2];
		vU[iB3] = vX[iB3];

		vA = vX * vE;
		vA.TinyToZero(fPrec);

		fFac = vX[iSC];

		if (vU.IsZero(fPrec) && !::IsZero(fFac, fPrec) && vA.IsZero(fPrec))
		{
			// Translator
			// Type = GA_TRANSLATOR
			// Vex[0] = Translation vector

			vA = (CType(2)/fFac) & (vX * vE0);
			vA.TinyToZero(fPrec);

			if (vA.IsZero(fPrec))
			{	
				// Scalar
				rMVI.m_eType = GA_SCALAR;
				rMVI.m_mVal.Set(1);
				rMVI.m_mVal[0] = CType(1);
			}
			else
			{
				rMVI.m_eType = GA_TRANSLATOR;
				rMVI.m_mVex.Set(1);
				rMVI.m_mVex[0] = &vA[iE1];
			}
		}
		else
		{
			// Motor
			// Type = GA_MOTOR
			// Vex[0] = Rot. Axis Offset Vector
			// Vex[1] = Rot. Axis
			// Val[0] = Rot. Angle

			rMVI.m_eType = GA_MOTOR;
			rMVI.m_mVex.Set(2);
			rMVI.m_mVal.Set(1);

			fU2 = Scalar(vU * (~vU));

			// Rotation axis
			vA = (vU / CType(sqrt( fU2 )) ) & vT123;
			rMVI.m_mVex[1] = &vA[1];

			// Offset vector
			vA = (vX * vE0) * ((~vU) / fU2);
			rMVI.m_mVex[0] = &vA[1];

			rMVI.m_mVal[0] = CType(2.0 * acos(Scalar(vX)));
		}
	}
	else if ((mGList.Count() == 3 && mGList[0] == 4 && mGList[1] == 2 && mGList[2] == 0) ||
			 (mGList.Count() == 2 && mGList[0] == 4 && mGList[1] == 2))
	{
		// Twist
		// Type = GA_TWIST
		// Vex[0] = Rot. Axis Offset Vector
		// Vex[1] = Trans. along Rot. Axis
		// Vex[2] = Rot. Axis
		// Val[0] = Rot. Angle

		rMVI.m_eType = GA_TWIST;
		rMVI.m_mVex.Set(3);
		rMVI.m_mVal.Set(1);

		MultiV<CType> vA(vX), vB, vR, vU, vTp, vTr, vt, vtp, vtr;
		CType fVal;

		fVal = Scalar(vA & ~vA);
		if (::IsZero(fVal, fPrec))
			return false;

		vA /= fVal;
		vA.TinyToZero(fPrec);

		vR = vA + ((vA * vE0) ^ vE);
		vR.TinyToZero(fPrec);

		vU = vR(2);

/*
		if (!vU.IsZero(fPrec))
		{
			fVal = CType(sqrt(Mag(Scalar(vU * vU))));
			vU /= fVal;
		}

		vB = vA - vR;
		vB.TinyToZero(fPrec);

		vTp = CType(1) + ( CType(0.5) ^ ((vB & (~vR)) - ((~vR) & vB)) );
		vTp.TinyToZero(fPrec);
		char *pcValTp = vTp.Str();

		vB = ( (vTp & vR) & (~vTp) );
		vB.TinyToZero(fPrec);
		char *pcValB = vB.Str();

		vTr = ( vA & (~vB) );
		vTr.TinyToZero(fPrec);
		char *pcValTr = vTr.Str();
*/

		vt = ( (CType(2) ^ (vA * vE0)) & (~vR) );
		vt.TinyToZero(fPrec);

		if (vU.IsZero(fPrec))
		{
			vtp = vZero;
			vtr = vt;
		}
		else
		{
			fVal = CType(sqrt(Mag(Scalar(vU * vU))));
			vU /= fVal;
			vtp = ( (vt * vU) & ~vU );
			vtr = vt - (vtp);
			vTr = CType(1) - (CType(0.5) ^ (vtr ^ vE));

			vB = (~vTr) & vA;
			vtp = (vB * vE0) * ((~vU) / fVal);
			vtp.TinyToZero(fPrec);
		}

//		vtr = CType(2)^(vTr * vE0);
//		vtr.TinyToZero(fPrec);

		// Translation in rot. plane
		rMVI.m_mVex[0] = &vtp[1];

		// Translation perp. to rot. plane
		rMVI.m_mVex[1] = &vtr[1];

		// Rotation axis
		vU = (vU & vT123);
		rMVI.m_mVex[2] = &vU[1];

		// Rotation angle
		rMVI.m_mVal[0] = CType(2.0 * acos(Scalar(vR)));
	}
	else 
	{
		return false;
	}

	return true;
}


