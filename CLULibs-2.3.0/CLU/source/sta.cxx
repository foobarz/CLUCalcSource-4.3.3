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


// STA Class Library Definitions

#include"sta.h"
#include"stainst.h"

////////////////////////////////////////////////////////////////////////////////////
/// Static Variables

template<class CType> Blade<CType> STA<CType>::bSC;
template<class CType> Blade<CType> STA<CType>::bG0;
template<class CType> Blade<CType> STA<CType>::bG1;
template<class CType> Blade<CType> STA<CType>::bG2;
template<class CType> Blade<CType> STA<CType>::bG3;
template<class CType> Blade<CType> STA<CType>::bS1;
template<class CType> Blade<CType> STA<CType>::bS2;
template<class CType> Blade<CType> STA<CType>::bS3;
template<class CType> Blade<CType> STA<CType>::bIS1;
template<class CType> Blade<CType> STA<CType>::bIS2;
template<class CType> Blade<CType> STA<CType>::bIS3;
template<class CType> Blade<CType> STA<CType>::bIG0;
template<class CType> Blade<CType> STA<CType>::bIG1;
template<class CType> Blade<CType> STA<CType>::bIG2;
template<class CType> Blade<CType> STA<CType>::bIG3;
template<class CType> Blade<CType> STA<CType>::bI;

template<class CType> MultiV<CType> STA<CType>::vSC;
template<class CType> MultiV<CType> STA<CType>::vG0;
template<class CType> MultiV<CType> STA<CType>::vG1;
template<class CType> MultiV<CType> STA<CType>::vG2;
template<class CType> MultiV<CType> STA<CType>::vG3;
template<class CType> MultiV<CType> STA<CType>::vS1;
template<class CType> MultiV<CType> STA<CType>::vS2;
template<class CType> MultiV<CType> STA<CType>::vS3;
template<class CType> MultiV<CType> STA<CType>::vIS1;
template<class CType> MultiV<CType> STA<CType>::vIS2;
template<class CType> MultiV<CType> STA<CType>::vIS3;
template<class CType> MultiV<CType> STA<CType>::vIG0;
template<class CType> MultiV<CType> STA<CType>::vIG1;
template<class CType> MultiV<CType> STA<CType>::vIG2;
template<class CType> MultiV<CType> STA<CType>::vIG3;
template<class CType> MultiV<CType> STA<CType>::vI;

template<class CType> MultiV<CType> STA<CType>::vZero;
template<class CType> RingBuf<uint> STA<CType>::BLADE;

template<class CType> MultiVStyle<CType> STA<CType>::m_MVStyle;

// PGA needs initialisation
template<class CType> bool STA<CType>::m_bInit = true;

template<class CType> const uint STA<CType>::iSC = 0;
template<class CType> const uint STA<CType>::iG0 = 1;
template<class CType> const uint STA<CType>::iG1 = 2;
template<class CType> const uint STA<CType>::iG2 = 3;
template<class CType> const uint STA<CType>::iG3 = 4;
template<class CType> const uint STA<CType>::iS1 = 5;
template<class CType> const uint STA<CType>::iS2 = 6;
template<class CType> const uint STA<CType>::iS3 = 7;
template<class CType> const uint STA<CType>::iIS1 = 8;
template<class CType> const uint STA<CType>::iIS2 = 9;
template<class CType> const uint STA<CType>::iIS3 = 10;
template<class CType> const uint STA<CType>::iIG0 = 11;
template<class CType> const uint STA<CType>::iIG1 = 12;
template<class CType> const uint STA<CType>::iIG2 = 13;
template<class CType> const uint STA<CType>::iIG3 = 14;
template<class CType> const uint STA<CType>::iI = 15;


#include "STATables.h"


////////////////////////////////////////////////////////////////////////////////////
// Constructor

template<class CType>
STA<CType>::STA() :  Base<CType>(4,0,"g",ID_STA)
{ 
	if (m_bInit) 
	{
		InitVars();
		m_bInit = false;
	}
}

////////////////////////////////////////////////////////////////////////////////////
// STA Signature

template<class CType>
inline CType STA<CType>::signature(uint i) 
{ 
  return CType( (i==0) ? (1) : (-1) ); 
}


////////////////////////////////////////////////////////////////////////////////////
/// Init Variables

template<class CType>
void STA<CType>::InitVars()
{
	BladeList<CType> STAStyle(*this);
	
	BLADE.Reset(1);
	bSC.Reset(*this, CType(1));
	bG0.Reset(*this,CType(1),1,(BLADE<<0),"g0");
	bG1.Reset(*this,CType(1),1,(BLADE<<1),"g1");
	bG2.Reset(*this,CType(1),1,(BLADE<<2),"g2");
	bG3.Reset(*this,CType(1),1,(BLADE<<3),"g3");
	
	BLADE.Reset(2);
	bS1.Reset(*this,CType(1),2,(BLADE<<1<<0),"s1");
	bS2.Reset(*this,CType(1),2,(BLADE<<2<<0),"s2");
	bS3.Reset(*this,CType(1),2,(BLADE<<3<<0),"s3");
	bIS1.Reset(*this,CType(1),2,(BLADE<<3<<2),"is1");
	bIS2.Reset(*this,CType(1),2,(BLADE<<1<<3),"is2");
	bIS3.Reset(*this,CType(1),2,(BLADE<<2<<1),"is3");
	
	BLADE.Reset(3);
	bIG0.Reset(*this,CType(1),3,(BLADE<<3<<2<<1),"ig0");
	bIG1.Reset(*this,CType(1),3,(BLADE<<2<<0<<3),"ig1");
	bIG2.Reset(*this,CType(1),3,(BLADE<<3<<0<<1),"ig2");
	bIG3.Reset(*this,CType(1),3,(BLADE<<1<<0<<2),"ig3");
	
	BLADE.Reset(4);
	bI.Reset(*this,CType(1),4,(BLADE<<0<<1<<2<<3),"i");
	
	STAStyle <<bSC<<bG0<<bG1<<bG2<<bG3<<bS1<<bS2<<bS3;
	STAStyle <<bIS1<<bIS2<<bIS3<<bIG0<<bIG1<<bIG2<<bIG3<<bI;

	// Initialises multiplication tables
	//m_MVStyle.Init(STAStyle);
	m_MVStyle.Init(STAStyle, piGrades, piGPTable, piIPTable, piOPTable, 
					piVITable, piDualTable); 
	
	vZero.SetStyle(m_MVStyle);

	vSC.SetStyle(m_MVStyle); vSC[iSC] = 1;
	vG0.SetStyle(m_MVStyle); vG0[iG0] = 1;
	vG1.SetStyle(m_MVStyle); vG1[iG1] = 1;
	vG2.SetStyle(m_MVStyle); vG2[iG2] = 1;
	vG3.SetStyle(m_MVStyle); vG3[iG3] = 1;
	vS1.SetStyle(m_MVStyle); vS1[iS1] = 1;
	vS2.SetStyle(m_MVStyle); vS2[iS2] = 1;
	vS3.SetStyle(m_MVStyle); vS3[iS3] = 1;
	vIS1.SetStyle(m_MVStyle); vIS1[iIS1] = 1;
	vIS2.SetStyle(m_MVStyle); vIS2[iIS2] = 1;
	vIS3.SetStyle(m_MVStyle); vIS3[iIS3] = 1;
	vIG0.SetStyle(m_MVStyle); vIG0[iIG0] = 1;
	vIG1.SetStyle(m_MVStyle); vIG1[iIG1] = 1;
	vIG2.SetStyle(m_MVStyle); vIG2[iIG2] = 1;
	vIG3.SetStyle(m_MVStyle); vIG3[iIG3] = 1;
	vI.SetStyle(m_MVStyle); vI[iI] = 1;

}

////////////////////////////////////////////////////////////////////////////////////
// Vector

template<class CType>
MultiV<CType> STA<CType>::Vec(const CType &a1, const CType &a2, 
							  const CType &a3, const CType &a4)
{
  CType a[4];

  a[0] = a1;
  a[1] = a2;
  a[2] = a3;
  a[3] = a4;

  return MakeVector(m_MVStyle,1,a);
}


////////////////////////////////////////////////////////////////////////////////////
// Create Bivector with components (S1, S2, S3)

template<class CType>
MultiV<CType> STA<CType>::SpaceBiVec(const CType &a1, const CType &a2, const CType &a3)
{
  CType a[6];

  a[0] = a1;
  a[1] = a2;
  a[2] = a3;
  a[3] = CType(0);
  a[4] = CType(0);
  a[5] = CType(0);

  return MakeVector(m_MVStyle,2,a);
}

////////////////////////////////////////////////////////////////////////////////////
// Make Rotor
// a is rotation axis, rad is rotation angle

template<class CType>
MultiV<CType> STA<CType>::Rotor(const CType &a1, const CType &a2, const CType &a3, 
								const CType &rad)
{
	MultiV<CType> vA(m_MVStyle);

	vA[iIS1] = a1;
	vA[iIS2] = a2;
	vA[iIS3] = a3;

	vA /= CType(sqrt(a1*a1 + a2*a2 + a3*a3));

	vA = CType(cos(rad/2.0)) + (CType(sin(rad/2.0)) & vA);

	vA.TinyToZero();

	return vA;
}


////////////////////////////////////////////////////////////////////////////////////
// Make Boost
// a is relative boost direction, v is relative speed, 0 <= v < 1

template<class CType>
MultiV<CType> STA<CType>::Boost(const CType &a1, const CType &a2, const CType &a3, 
								const CType &v)
{
	MultiV<CType> vA(m_MVStyle);

	if (double(v) < 0.0 || double(v) >= 1.0)
		return vA;

	vA[iS1] = a1;
	vA[iS2] = a2;
	vA[iS3] = a3;

	vA /= CType(sqrt(a1*a1 + a2*a2 + a3*a3));

	double dU;

	// Evaluate u = atanh(v)/2
	dU = 0.25 * log( (1.0 + double(v)) / (1.0 - double(v)) );

	vA = CType(cosh(dU)) + (CType(sinh(dU)) & vA);

	vA.TinyToZero();

	return vA;
}
