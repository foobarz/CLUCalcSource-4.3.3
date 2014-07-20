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
 

//------------------ Multi Vector Library -----------------------

#include "multiv.h"

#include "makestr.h"
#include "mathelp.h"
#include "matrix.cxx"


////////////////////////////////////////////////////////////////////////////////////
// Constructor

template<class CType>
MultiV<CType>::MultiV(const MultiVStyle<CType> &nstyle, CType ncomps[])
{
	m_pStyle = const_cast<MultiVStyle<CType>*>(&nstyle);
	m_uGADim = m_pStyle->GADim();
	m_csStr = "";
	
	if (m_mData.Set(m_uGADim))
	{
		if (!ncomps) 
			memset(m_mData.Data(), 0, m_uGADim*sizeof(CType));
		else
			memcpy(m_mData.Data(), ncomps, m_uGADim*sizeof(CType));
	}
}


////////////////////////////////////////////////////////////////////////////////////
// Default Constructor

template<class CType>
MultiV<CType>::MultiV()
{
	m_pStyle = 0;
	m_uGADim = 0;
	m_csStr = "";
	m_mData.Set(0);
}


////////////////////////////////////////////////////////////////////////////////////
// Copy Constructor

template<class CType>
MultiV<CType>::MultiV(const MultiV<CType> &a)
{
	m_pStyle = a.m_pStyle;
	m_uGADim = a.m_uGADim;
	m_mData = a.m_mData;
	m_csStr = "";
}


////////////////////////////////////////////////////////////////////////////////////
// Destructor

template<class CType>
MultiV<CType>::~MultiV()
{
}




////////////////////////////////////////////////////////////////////////////////////
// Set Style of Multivector

template<class CType>
void MultiV<CType>::SetStyle(const MultiVStyle<CType> &nstyle)
{
	m_pStyle = const_cast<MultiVStyle<CType>*>(&nstyle);
	m_uGADim = m_pStyle->GADim();
	m_csStr = "";
	
	if (m_mData.Set(m_uGADim))
	{
		memset(m_mData.Data(), 0, m_uGADim*sizeof(CType));
	}
}


////////////////////////////////////////////////////////////////////////////////////
/// check whether this is a zero multivector

template <class CType>
bool MultiV<CType>::IsZero(CType dPrec) const
{
	uint uPos;

	for( uPos = 0; uPos < m_uGADim; uPos++ )
	{
		if (!::IsZero(m_mData[uPos], dPrec))
			return false;
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////////
// Comparison of two Multivectors ==

template<class CType>
int operator== (const MultiV<CType> &a, const MultiV<CType> &b)
{
	for(uint i=0;i<a.m_uGADim;i++)
		if (!(a.m_mData[i] == b.m_mData[i]))
			return 0;
		
	return 1;
}


////////////////////////////////////////////////////////////////////////////////////
// Comparison of two Multivectors !=

template<class CType>
int operator!= (const MultiV<CType> &a, const MultiV<CType> &b)
{
	for(uint i=0;i<a.m_uGADim;i++)
		if (!(a.m_mData[i] == b.m_mData[i]))
			return 1;
		
	return 0;
}



////////////////////////////////////////////////////////////////////////////////////
// Magnitude SQUARED

template<class CType>
CType MultiV<CType>::Mag2() const
{
	CType r = 0;
	
	int *viTable = m_pStyle->VecInvTable();
	
	for(uint i=0;i<m_uGADim;i++)
		r += m_mData[i] * m_mData[i] * viTable[i];
	
	return r;
}

////////////////////////////////////////////////////////////////////////////////////
// Floor

template<class CType>
void MultiV<CType>::Floor()
{
	for(uint i=0;i<m_uGADim;i++)
		m_mData[i] = CType(floor(double(m_mData[i])));
}

////////////////////////////////////////////////////////////////////////////////////
// Ceiling

template<class CType>
void MultiV<CType>::Ceil()
{
	for(uint i=0;i<m_uGADim;i++)
		m_mData[i] = CType(ceil(double(m_mData[i])));
}


////////////////////////////////////////////////////////////////////////////////////
// Get component of multivector

template<class CType>
CType& MultiV<CType>::operator[] (const uint &pos) const
{
	if (pos >= m_uGADim) return m_mData[0];
	
	return m_mData[pos];
}



////////////////////////////////////////////////////////////////////////////////////
// Projection of certain grade

template<class CType>
MultiV<CType> MultiV<CType>::operator() (uint grade) const
{
	MultiV<CType> a(m_pStyle[0]);
	uint *grades = (uint *) m_pStyle->Grades();
	
	for(uint i=0;i<m_uGADim;i++)
		if (grades[i] == grade) 
			a.m_mData[i] = m_mData[i];
		
	return a;
}


////////////////////////////////////////////////////////////////////////////////////
// Projection of scalar

template<class CType>
CType MultiV<CType>::operator() () const
{
	int *grades = m_pStyle->Grades();
	
	for(uint i=0;i<m_uGADim;i++)
		if (grades[i] == 0) 
			return m_mData[i];
		
	return CType(0);
}


////////////////////////////////////////////////////////////////////////////////////
// Projection of scalar (friend function)

template<class CType>
CType Scalar(const MultiV<CType> &a)
{
	int *grades = a.m_pStyle->Grades();
	
	for(uint i=0;i<a.m_uGADim;i++)
		if (grades[i] == 0) 
			return a.m_mData[i];
		
	return CType(0);
}



////////////////////////////////////////////////////////////////////////////////////
// Assignment of Multi Vector to Multi Vector

template<class CType>
MultiV<CType>& MultiV<CType>::operator= (const MultiV<CType> &a)
{
	m_pStyle = a.m_pStyle;
	m_uGADim = a.m_uGADim;
	m_mData = a.m_mData;
	
	return *this;
}


////////////////////////////////////////////////////////////////////////////////////
// Assignment of Scalar to MultiVector

template<class CType>
MultiV<CType>& MultiV<CType>::operator= (const CType &b)
{
	int *grades = m_pStyle->Grades();
	
	for(uint i=0;i<m_uGADim;i++)
		if (grades[i] == 0) m_mData[i] = b;
		else m_mData[i] = 0;
		
	return *this;
}



////////////////////////////////////////////////////////////////////////////////////
// Assignment of Blade to MultiVector

template<class CType>
MultiV<CType>& MultiV<CType>::operator= (const Blade<CType> &b)
{
	Blade<CType> a(m_pStyle->BasePtr()[0]);
	int r,f;
	
	a = b;
	
	for(uint i=0;i<m_uGADim;i++)
		m_mData[i] = 0;
	
	if ((r = (m_pStyle[0] == a)))
	{ 
		if (r < 0)
		{ 
			r *= -1;
			f = -1;
		}
		else f = 1;
		r--;
	
		m_mData[r] = a.Factor() * CType(f);
	}
	
	return *this;
}


////////////////////////////////////////////////////////////////////////////////////
// Assignment of BladeList to MultiVector

template<class CType>
MultiV<CType>& MultiV<CType>::operator= (const BladeList<CType> &bl)
{
	BladeList<CType> al(m_pStyle->BasePtr()[0]);
	Blade<CType> a(m_pStyle->BasePtr()[0]);
	int r,f;
	uint i, cnt;
	
	al = bl;
	cnt = al.Count();
	
	for(i=0;i<m_uGADim;i++)
		m_mData[i] = 0;
	
	for(i=0;i<cnt;i++)
	{ 
		a = al[i];
		if ((r = (m_pStyle[0] == a)))
		{ 
			if (r < 0)
			{ 
				r *= -1;
				f = -1;
			}
			else f = 1;
			r--;
	
			m_mData[r] = a.Factor() * CType(f);
		}
	}
	
	return *this;
}



////////////////////////////////////////////////////////////////////////////////////
// Negation of MultiVector

template<class CType>
MultiV<CType> operator- (const MultiV<CType> &amv)
{
	MultiV<CType> cmv(amv.m_pStyle[0]);
	cmv = amv;
	
	for(uint i=0;i<cmv.m_uGADim;i++)
		cmv.m_mData[i] *= -1;
	
	return cmv;
}


template<class CType>
MultiV<CType> Involute (const MultiV<CType> &vA)
{
	MultiV<CType> vB(vA);
	
	int *invoTable = vB.m_pStyle->InvoTable();

	for(uint i = 0; i < vB.m_uGADim; i++)
	{
		if (invoTable[i])
			vB.m_mData[i] = -vB.m_mData[i];
	}

	return vB;
}


////////////////////////////////////////////////////////////////////////////////////
// Reversion of MultiVector

template<class CType>
MultiV<CType> operator~ (const MultiV<CType> &amv)
{
	MultiV<CType> cmv(amv.m_pStyle[0]);
	cmv = amv;
	
	int *revTable = cmv.m_pStyle->ReverseTable();
	
	for(uint i=0;i<cmv.m_uGADim;i++)
		if (revTable[i] < 0)
			cmv.m_mData[i] = -cmv.m_mData[i];
		
	return cmv;
}


template<class CType>
MultiV<CType> MultiV<CType>::Reverse ()
{
	MultiV<CType> cmv;//(amv.m_pStyle[0]);
	cmv = *this;
	
	int *revTable = cmv.m_pStyle->ReverseTable();
	
	for(uint i=0;i<cmv.m_uGADim;i++)
		if (revTable[i] < 0)
			cmv.m_mData[i] = -cmv.m_mData[i];
		
	return cmv;
}



////////////////////////////////////////////////////////////////////////////////////
// Dual of MultiVector

template<class CType>
MultiV<CType> operator* (const MultiV<CType> &amv)
{
	MultiV<CType> cmv(amv.m_pStyle[0]);
	
	int r;
	int *dualTable = cmv.m_pStyle->DualTable();
	
	for(uint i=0;i<cmv.m_uGADim;i++)
	{ 
		if ((r = dualTable[i]) < 0)
		{ 
			cmv.m_mData[-r-1] = -amv.m_mData[i];
		}
		else if (r > 0)
		{ 
			cmv.m_mData[r-1] = amv.m_mData[i];
		}
	}
	
	return cmv;
}


template<class CType>
MultiV<CType> MultiV<CType>::Dual ()
{
	MultiV<CType> cmv(this->m_pStyle[0]);
	
	int r;
	int *dualTable = cmv.m_pStyle->DualTable();
	
	for(uint i=0;i<cmv.m_uGADim;i++)
	{ 
		if ((r = dualTable[i]) < 0)
		{ 
			cmv.m_mData[-r-1] = -this->m_mData[i];
		}
		else if (r > 0)
		{ 
			cmv.m_mData[r-1] = this->m_mData[i];
		}
	}
	
	return cmv;
}



////////////////////////////////////////////////////////////////////////////////////
// Inverse of Multivector

template<class CType>
MultiV<CType> operator! (const MultiV<CType> &vA)
{
	MultiV<CType> vB(vA.GetStyle()), vX;

	vB = 1.0;

	MultiVSolve(vA, vB, vX, Left, (Mem<CType>*) 0);

	return vX;
}



////////////////////////////////////////////////////////////////////////////////////
// Addition of Multivector to Scalar

template<class CType>
MultiV<CType> operator+ (const MultiV<CType> &amv, const CType &b)
{
	uint i;
	int *grades = amv.m_pStyle->Grades();
	MultiV<CType> cmv(amv.m_pStyle[0]);
	cmv = amv;
	
	for(i=0;i<cmv.m_uGADim;i++)
	{
		if (grades[i] == 0)
		{ 
			cmv.m_mData[i] += b;
			break;
		}
	}
	
	return cmv;
}



////////////////////////////////////////////////////////////////////////////////////
// Addition of Scalar to Multivector

template<class CType>
MultiV<CType> operator+ (const CType &b, const MultiV<CType> &amv)
{
	uint i;
	int *grades = amv.m_pStyle->Grades();
	MultiV<CType> cmv(amv.m_pStyle[0]);
	cmv = amv;
	
	for(i=0;i<cmv.m_uGADim;i++)
	{
		if (grades[i] == 0)
		{ 
			cmv.m_mData[i] += b;
			break;
		}
	}

	return cmv;
}



////////////////////////////////////////////////////////////////////////////////////
// Addition of Multivector to Scalar to this

template<class CType>
MultiV<CType>& operator+= (MultiV<CType> &amv, const CType &b)
{
	uint i;
	int *grades = amv.m_pStyle->Grades();
	
	for(i=0;i<amv.m_uGADim;i++)
	{
		if (grades[i] == 0)
		{ 
			amv.m_mData[i] += b;
			break;
		}
	}

	return amv;
}


////////////////////////////////////////////////////////////////////////////////////
// Addition of Multivector to Scalar to this using <<

template<class CType>
MultiV<CType>& operator<< (MultiV<CType> &amv, const CType &b)
{
	uint i;
	int *grades = amv.m_pStyle->Grades();
	
	for(i=0;i<amv.m_uGADim;i++)
	{
		if (grades[i] == 0)
		{ 
			amv.m_mData[i] += b;
			break;
		}
	}

	return amv;
}


////////////////////////////////////////////////////////////////////////////////////
// Subtraction of Scalar from Multivector

template<class CType>
MultiV<CType> operator- (const MultiV<CType> &amv, const CType &b)
{
	uint i;
	int *grades = amv.m_pStyle->Grades();
	MultiV<CType> cmv(amv.m_pStyle[0]);
	cmv = amv;
	
	for(i=0;i<cmv.m_uGADim;i++)
	{
		if (grades[i] == 0)
		{ 
			cmv.m_mData[i] -= b;
			break;
		}
	}
	return cmv;
}



////////////////////////////////////////////////////////////////////////////////////
// Subtraction of Multivector from Scalar

template<class CType>
MultiV<CType> operator- (const CType &b, const MultiV<CType> &amv)
{
	uint i;
	int *grades = amv.m_pStyle->Grades();
	MultiV<CType> cmv(amv.m_pStyle[0]);
	cmv = -amv;
	
	for(i=0;i<cmv.m_uGADim;i++)
	{
		if (grades[i] == 0)
		{ 
			cmv.m_mData[i] += b;
			break;
		}
	}
	return cmv;
}



////////////////////////////////////////////////////////////////////////////////////
// Subtraction of Scalar from Multivector from this

template<class CType>
MultiV<CType>& operator-= (MultiV<CType> &amv, const CType &b)
{
	uint i;
	int *grades = amv.m_pStyle->Grades();
	
	for(i=0;i<amv.m_uGADim;i++)
	{
		if (grades[i] == 0)
		{ 
			amv.m_mData[i] -= b;
			break;
		}
	}
	return amv;
}



////////////////////////////////////////////////////////////////////////////////////
// Addition of Blade to MultiVector

template<class CType>
MultiV<CType> operator+ (const MultiV<CType> &amv, const Blade<CType> &b)
{
	int r,f;
	MultiV<CType> cmv(amv.m_pStyle[0]);
	cmv = amv;
	
	if ((r = (amv.m_pStyle[0] == b)))
	{ 
		if (r < 0)
		{ 
			r *= -1;
			f = -1;
		}
		else f = 1;
		r--;
	
		cmv.m_mData[r] += b.fact * CType(f);
	}
	
	return cmv;
}


////////////////////////////////////////////////////////////////////////////////////
// Addition of MultiVector to Blade

template<class CType>
MultiV<CType> operator+ (const Blade<CType> &b, const MultiV<CType> &amv)
{
	int r,f;
	MultiV<CType> cmv(amv.m_pStyle[0]);
	cmv = amv;
	
	if ((r = (amv.m_pStyle[0] == b)))
	{ 
		if (r < 0)
		{ 
			r *= -1;
			f = -1;
		}
		else f = 1;
		r--;
	
		cmv.m_mData[r] += b.fact * CType(f);
	}
	
	return cmv;
}



////////////////////////////////////////////////////////////////////////////////////
// Addition of Blade to this MultiVector 

template<class CType>
MultiV<CType>& operator+= (MultiV<CType> &amv, const Blade<CType> &b)
{
	int r,f;
	
	if ((r = (amv.m_pStyle[0] == b)))
	{ 
		if (r < 0)
		{ 
			r *= -1;
			f = -1;
		}
		else f = 1;
		r--;
	
		amv.m_mData[r] += b.fact * CType(f);
	}
	
	return amv;
}



////////////////////////////////////////////////////////////////////////////////////
// Addition of Blade to this MultiVector using <<

template<class CType>
MultiV<CType>& operator<< (MultiV<CType> &amv, const Blade<CType> &b)
{
	int r,f;
	
	if ((r = (amv.m_pStyle[0] == b)))
	{ 
		if (r < 0)
		{ 
			r *= -1;
			f = -1;
		}
		else f = 1;
		r--;
	
		amv.m_mData[r] += b.fact * CType(f);
	}
	
	return amv;
}



////////////////////////////////////////////////////////////////////////////////////
// Subtraction of Blade from MultiVector

template<class CType>
MultiV<CType> operator- (const MultiV<CType> &amv, const Blade<CType> &b)
{
	int r,f;
	MultiV<CType> cmv(amv.m_pStyle[0]);
	cmv = amv;
	
	if ((r = (amv.m_pStyle[0] == b)))
	{ 
		if (r < 0)
		{ 
			r *= -1;
			f = -1;
		}
		else f = 1;
		r--;
	
		cmv.m_mData[r] -= b.fact * CType(f);
	}
	
	return cmv;
}



////////////////////////////////////////////////////////////////////////////////////
// Subtraction of MultiVector from Blade

template<class CType>
MultiV<CType> operator- (const Blade<CType> &b, const MultiV<CType> &amv)
{
	int r,f;
	MultiV<CType> cmv(amv.m_pStyle[0]);
	cmv = -amv;
	
	if ((r = (amv.m_pStyle[0] == b)))
	{ 
		if (r < 0)
		{ 
			r *= -1;
		f = -1;
		}
		else f = 1;
		r--;
	
		cmv.m_mData[r] += b.fact * CType(f);
	}
	
	return cmv;
}



////////////////////////////////////////////////////////////////////////////////////
// Subtraction of Blade from this MultiVector 

template<class CType>
MultiV<CType>& operator-= (MultiV<CType> &amv, const Blade<CType> &b)
{
	int r,f;
	
	if ((r = (amv.m_pStyle[0] == b)))
	{ 
		if (r < 0)
		{ 
			r *= -1;
			f = -1;
		}
		else f = 1;
		r--;
	
		amv.m_mData[r] -= b.fact * CType(f);
	}
	
	return amv;
}



////////////////////////////////////////////////////////////////////////////////////
// Addition of two Multivectors

template<class CType>
MultiV<CType> operator+ (const MultiV<CType>&a, const MultiV<CType>&b)
{
	MultiV<CType> c(a.m_pStyle[0]);
	
	for(uint i=0;i<a.m_uGADim;i++)
		c.m_mData[i] = a.m_mData[i] + b.m_mData[i];
	
	return c;
}



////////////////////////////////////////////////////////////////////////////////////
// Addition of two Multivectors to this

template<class CType>
MultiV<CType>& operator+= (MultiV<CType>&a, const MultiV<CType>&b)
{
	for(uint i=0;i<a.m_uGADim;i++)
		a.m_mData[i] += b.m_mData[i];
	
	return a;
}



////////////////////////////////////////////////////////////////////////////////////
// Addition of two Multivectors to this

template<class CType>
MultiV<CType>& operator<< (MultiV<CType>&a, const MultiV<CType>&b)
{
	for(uint i=0;i<a.m_uGADim;i++)
		a.m_mData[i] += b.m_mData[i];
	
	return a;
}


////////////////////////////////////////////////////////////////////////////////////
// Subtraction of two Multivectors

template<class CType>
MultiV<CType> operator- (const MultiV<CType>&a, const MultiV<CType>&b)
{
	MultiV<CType> c(a.m_pStyle[0]);
	
	for(uint i=0;i<a.m_uGADim;i++)
		c.m_mData[i] = a.m_mData[i] - b.m_mData[i];
	
	return c;
}


////////////////////////////////////////////////////////////////////////////////////
// Subtraction of two Multivectors to this

template<class CType>
MultiV<CType>& operator-= (MultiV<CType>&a, const MultiV<CType>&b)
{
	for(uint i=0;i<a.m_uGADim;i++)
		a.m_mData[i] -= b.m_mData[i];
	
	return a;
}

////////////////////////////////////////////////////////////////////////////////////
/// Product with Multiplication table

template<class CType>
bool MultiV<CType>::MTProd(const Mem<CType>& mA, const Mem<CType>& mB, int *piTable)
{
	int r;
	uint i,j,k;
	uint uADim, uBDim;
	CType dA, dB;
	CType *pAData, *pBData, *pCData;
	pAData = mA.Data();
	pBData = mB.Data();
	pCData = m_mData.Data();
	
	uADim = mA.Count();
	uBDim = mB.Count();

	for(i=0,k=0;i<uADim;i++)
	{
		dA = pAData[i];
		if (dA != CType(0))
		{
			for(j=0;j<uBDim;j++,k++)
			{
				dB = pBData[j];
				if (dB != CType(0))
				{
					if ((r = piTable[k]) < 0)
					{ 
						pCData[-r-1] -= dA * dB;
					}
					else if (r > 0)
					{
						pCData[r-1] += dA * dB;
					}
				}
			}
		}
		else k += uBDim;
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////////
// Geometric Product of two Multi Vectors

template<class CType>
MultiV<CType> operator& (const MultiV<CType>&a, const MultiV<CType>&b)
{
	int *piTable = a.m_pStyle->GPTable();
	MultiV<CType> c(a.m_pStyle[0]);

	c.MTProd(a.m_mData, b.m_mData, piTable);
	
	return c;
}



////////////////////////////////////////////////////////////////////////////////////
// Geometric Product of two Multi Vectors to this

template<class CType>
MultiV<CType>& operator&= (MultiV<CType>&a, const MultiV<CType>&b)
{
	int *piTable = a.m_pStyle->GPTable();
	MultiV<CType> c(*a.m_pStyle);

	c.MTProd(a.m_mData, b.m_mData, piTable);

	a = c;
	return a;
}



////////////////////////////////////////////////////////////////////////////////////
// Inner Product of two Multi Vectors

template<class CType>
MultiV<CType> operator* (const MultiV<CType>&a, const MultiV<CType>&b)
{
	int *piTable = a.m_pStyle->IPTable();
	MultiV<CType> c(a.m_pStyle[0]);
	
	c.MTProd(a.m_mData, b.m_mData, piTable);

	return c;
}



////////////////////////////////////////////////////////////////////////////////////
// Inner Product of two Multi Vectors to this

template<class CType>
MultiV<CType>& operator*= (MultiV<CType>&a, const MultiV<CType>&b)
{
	int *piTable = a.m_pStyle->IPTable();
	MultiV<CType> c(a.m_pStyle[0]);
	
	c.MTProd(a.m_mData, b.m_mData, piTable);

	a = c;
	return a;
}



////////////////////////////////////////////////////////////////////////////////////
// Outer Product of two Multi Vectors

template<class CType>
MultiV<CType> operator^ (const MultiV<CType>&a, const MultiV<CType>&b)
{
	int *piTable = a.m_pStyle->OPTable();
	MultiV<CType> c(a.m_pStyle[0]);
	
	c.MTProd(a.m_mData, b.m_mData, piTable);

	return c;
}



////////////////////////////////////////////////////////////////////////////////////
// Outer Product of two Multi Vectors to this

template<class CType>
MultiV<CType>& operator^= (MultiV<CType>&a, const MultiV<CType>&b)
{
	int *piTable = a.m_pStyle->OPTable();
	MultiV<CType> c(a.m_pStyle[0]);
	
	c.MTProd(a.m_mData, b.m_mData, piTable);

	a = c;
	return a;
}



////////////////////////////////////////////////////////////////////////////////////
// Outer Prod of Multivector and Scalar

template<class CType>
MultiV<CType> operator^ (const MultiV<CType>&a, const CType &b)
{
	uint i;
	MultiV<CType> c(a);
	
	for(i=0;i<a.m_uGADim;i++)
		c.m_mData[i] *= b;
	
	return c;
}



////////////////////////////////////////////////////////////////////////////////////
// Outer Prod of Scalar and Multivector

template<class CType>
MultiV<CType> operator^ (const CType &b, const MultiV<CType>&a)
{
	uint i;
	MultiV<CType> c(a);
	
	for(i=0;i<a.m_uGADim;i++)
		c.m_mData[i] *= b;
	
	return c;
}


////////////////////////////////////////////////////////////////////////////////////
// Outer Prod of Multivector and Scalar to this

template<class CType>
MultiV<CType>& operator^= (MultiV<CType>&a, const CType &b)
{
	uint i;
	
	for(i=0;i<a.m_uGADim;i++)
		a.m_mData[i] *= b;
	
	return a;
}

////////////////////////////////////////////////////////////////////////////////////
// Outer Prod of Multivector and Scalar

template<class CType>
MultiV<CType> operator& (const MultiV<CType>&a, const CType &b)
{
	uint i;
	MultiV<CType> c(a);
	
	for(i=0;i<a.m_uGADim;i++)
		c.m_mData[i] *= b;
	
	return c;
}



////////////////////////////////////////////////////////////////////////////////////
// Outer Prod of Scalar and Multivector

template<class CType>
MultiV<CType> operator& (const CType &b, const MultiV<CType>&a)
{
	uint i;
	MultiV<CType> c(a);
	
	for(i=0;i<a.m_uGADim;i++)
		c.m_mData[i] *= b;
	
	return c;
}


////////////////////////////////////////////////////////////////////////////////////
// Outer Prod of Multivector and Scalar to this

template<class CType>
MultiV<CType>& operator&= (MultiV<CType>&a, const CType &b)
{
	uint i;
	
	for(i=0;i<a.m_uGADim;i++)
		a.m_mData[i] *= b;
	
	return a;
}

////////////////////////////////////////////////////////////////////////////////////
// GP of Blade and Multivector

template<class CType>
MultiV<CType> operator& (const Blade<CType> &b, const MultiV<CType> &a)
{
	MultiV<CType> c(a.m_pStyle[0]), d(a.m_pStyle[0]);
	
	d = b;
	
	c = d & a;
	
	return c;
}


////////////////////////////////////////////////////////////////////////////////////
// GP of Blade and Multivector

template<class CType>
MultiV<CType> operator& (const MultiV<CType> &a, const Blade<CType> &b)
{
	MultiV<CType> c(a.m_pStyle[0]), d(a.m_pStyle[0]);
	
	d = b;
	
	c = a & d;
	
	return c;
}


////////////////////////////////////////////////////////////////////////////////////
// GP of Blade and Multivector to this

template<class CType>
MultiV<CType>& operator&= (MultiV<CType> &a, const Blade<CType> &b)
{
	MultiV<CType> d(a.m_pStyle[0]);
	
	d = b;
	
	a &= d;
	
	return a;
}


////////////////////////////////////////////////////////////////////////////////////
// Inner Prod of Blade and Multivector

template<class CType>
MultiV<CType> operator* (const Blade<CType> &b, const MultiV<CType> &a)
{
	MultiV<CType> c(a.m_pStyle[0]), d(a.m_pStyle[0]);
	
	d = b;
	
	c = d * a;
	
	return c;
}


////////////////////////////////////////////////////////////////////////////////////
// Inner Prod of Blade and Multivector

template<class CType>
MultiV<CType> operator* (const MultiV<CType> &a, const Blade<CType> &b)
{
	MultiV<CType> c(a.m_pStyle[0]), d(a.m_pStyle[0]);
	
	d = b;
	
	c = a * d;
	
	return c;
}


////////////////////////////////////////////////////////////////////////////////////
// Inner Prod of Blade and Multivector to this

template<class CType>
MultiV<CType>& operator*= (MultiV<CType> &a, const Blade<CType> &b)
{
	MultiV<CType> d(a.m_pStyle[0]);
	
	d = b;
	
	a *= d;
	
	return a;
}



////////////////////////////////////////////////////////////////////////////////////
// Outer Prod of Blade and Multivector

template<class CType>
MultiV<CType> operator^ (const Blade<CType> &b, const MultiV<CType> &a)
{
	MultiV<CType> c(a.m_pStyle[0]), d(a.m_pStyle[0]);
	
	d = b;
	
	c = d ^ a;
	
	return c;
}


////////////////////////////////////////////////////////////////////////////////////
// Outer Prod of Blade and Multivector

template<class CType>
MultiV<CType> operator^ (const MultiV<CType> &a, const Blade<CType> &b)
{
	MultiV<CType> c(a.m_pStyle[0]), d(a.m_pStyle[0]);
	
	d = b;
	
	c = a ^ d;
	
	return c;
}


////////////////////////////////////////////////////////////////////////////////////
// Outer Prod of Blade and Multivector to this

template<class CType>
MultiV<CType>& operator^= (MultiV<CType> &a, const Blade<CType> &b)
{
	MultiV<CType> d(a.m_pStyle[0]);
	
	d = b;
	
	a ^= d;
	
	return a;
}





////////////////////////////////////////////////////////////////////////////////////
// Outer Prod of Multivector and Inverse of Scalar

template<class CType>
MultiV<CType> operator/ (const MultiV<CType>&a, const CType &b)
{
	uint i;
	MultiV<CType> c(a.m_pStyle[0]);
	c = a;
	
	for(i=0;i<a.m_uGADim;i++)
		c.m_mData[i] /= b;
	
	return c;
}


////////////////////////////////////////////////////////////////////////////////////
// Outer Prod of Multivector and Inverse of Scalar to this

template<class CType>
MultiV<CType>& operator/= (MultiV<CType>&a, const CType &b)
{
	uint i;
	
	for(i=0;i<a.m_uGADim;i++)
		a.m_mData[i] /= b;
	
	return a;
}



////////////////////////////////////////////////////////////////////////////////////
// String output of MultiVector

template<class CType>
char* MultiV<CType>::Str(int strstyle)
{
	uint i,j;
	int isfirst = 1;
	cStr hstr;
	
	m_csStr = "[";
	
	if (strstyle == MVS_LIST)
	{ 
		for(i=0;i<m_uGADim;i++)
		{ 
			m_csStr << MakeStr(CType(m_mData[i]));
			if (i+1 < m_uGADim) m_csStr << ",";
		}
	}
	else if (strstyle == MVS_SUM)
	{ 
		for(i=0;i<m_uGADim;i++)
		{ 
			if (!(m_mData[i] == CType(0)))
			{ 
				if (isfirst)
				{ 
					m_csStr << " ";
					isfirst = 0;
				}
				else m_csStr << "+ ";
				m_csStr << MakeStr(CType(m_mData[i]));
				if (i > 0)
				{ 
					m_csStr << "^";
					hstr = m_pStyle->BladeName(i);
					if ((j = ('^' < hstr))) 
						m_csStr << hstr.Last(hstr.Len() - j);
				}
				m_csStr << " ";
			}
		}
	}
	
	m_csStr << "]";
	
	return m_csStr;
}


////////////////////////////////////////////////////////////////////////////////////
// Return Vector of certain grade blades

template<class CType>
MultiV<CType> MakeVector(MultiVStyle<CType> &m_pStyle, uint grade, const CType *m_mData)
{
	uint i,j;
	uint *grades = (uint *) m_pStyle.Grades();
	MultiV<CType> a(m_pStyle);
	
	for(i=0,j=0;i<a.m_uGADim;i++)
		if (grades[i] == grade)
			a.m_mData[i] = m_mData[j++]; 
		
	return a;
}



////////////////////////////////////////////////////////////////////////////////////
// Return Vector of certain grade blades

template<class CType>
MultiV<CType> MakeVector(const MultiV<CType> &mv, uint grade, const CType *m_mData)
{
	uint i,j;
	uint *grades = (uint *) mv.m_pStyle->Grades();
	MultiV<CType> a(mv.m_pStyle[0]);
	
	for(i=0,j=0;i<a.m_uGADim;i++)
		if (grades[i] == grade)
			a.m_mData[i] = m_mData[j++]; 
		
	return a;
}



////////////////////////////////////////////////////////////////////////////////////
// Get components of Vector of certain grade

template<class CType>
void GetVector(const MultiV<CType> &mv, uint grade, CType *m_mData)
{
	uint i,j;
	uint *grades = (uint *) mv.m_pStyle->Grades();
	
	for(i=0,j=0;i<mv.m_uGADim;i++)
		if (grades[i] == grade)
			m_mData[j++] = mv.m_mData[i]; 
		
}


////////////////////////////////////////////////////////////////////////////////////
// Return Pseudo Scalar

template<class CType>
MultiV<CType> MakePS(MultiVStyle<CType> &m_pStyle)
{
	uint i,j;
	uint *grades = (uint *) m_pStyle.Grades();
	uint dim = m_pStyle.BasePtr()->VSDim();
	MultiV<CType> a(m_pStyle);
	
	for(i=0,j=0;i<a.m_uGADim;i++)
		if (grades[i] == dim)
			a.m_mData[i] = 1; 
		
	return a;
}


////////////////////////////////////////////////////////////////////////////////////
// Cast MultiV to BladeList

template<class CType>
BladeList<CType> MakeBladeList(const MultiV<CType> &mv)
{
	BladeList<CType> bl(mv.m_pStyle->BasePtr()[0]);
	BladeList<CType> *m_pStyle = mv.m_pStyle->Style();
	
	for(uint i=0;i<mv.m_uGADim;i++)
		if (!(mv.m_mData[i] == CType(0)))
			bl << (mv.m_mData[i] ^ m_pStyle[0][i]);
		
	return bl;
}


////////////////////////////////////////////////////////////////////////////////////
// MEET of two Multivectors, taking the JOIN as the whole Space

template<class CType>
MultiV<CType> IMeet(const MultiV<CType> &a, const MultiV<CType> &b)
{
	MultiV<CType> c(a.m_pStyle[0]);
	
	c = (*a) * b; // Inner Prod. of dual of A and B.
	c -= c(0);    // Scalar Parts will be discarded
	
	return c;
}


////////////////////////////////////////////////////////////////////////////////////
// Set tiny components to zero

template<class CType>
MultiV<CType>& MultiV<CType>::TinyToZero(const CType tiny)
{
	uint i;
	CType ctTiny = tiny;

	if (ctTiny == CType(0)) Tiny(ctTiny);

	for(i=0;i<m_uGADim;i++)
	{
		if ( m_mData[i] >= -ctTiny && m_mData[i] <= ctTiny ) 
			m_mData[i] = CType(0);
	}

	return *this;
}

////////////////////////////////////////////////////////////////////////////////////
// Set tiny components to zero

template<class CType>
MultiV<CType>& MultiV<CType>::Round(const CType fPrec)
{
	uint i;
	CType ctPrec = fPrec;

	if (ctPrec == CType(0)) Tiny(ctPrec);

	for(i=0;i<m_uGADim;i++)
	{
		m_mData[i] = ::Round(m_mData[i], ctPrec);
	}

	return *this;
}


////////////////////////////////////////////////////////////////////////////////////
// Project mvX on mvA space, i.e. component of mvX in mvA
// expects that mvX and mvA are pure blades.

template<class CType>
MultiV<CType> Project(const MultiV<CType> &vA, const MultiV<CType> &vX)
{
	MultiV<CType> vR(vX), vrA;

	vrA = ~vA;

	vR = ((vR * vA) & vrA);
	vR /= Scalar(vA * vrA);

	return vR;
}

////////////////////////////////////////////////////////////////////////////////////
// Reject mvX from mvA space, i.e. component of mvX not in mvA
// expects that mvX and mvA are pure blades.

template<class CType>
MultiV<CType> Reject(const MultiV<CType> &vA, const MultiV<CType> &vX)
{
	MultiV<CType> vR(vX), vrA;

	vrA = ~vA;

	vR -= ((vR * vA) & vrA) / Scalar(vA & vrA);

	return vR;
}


////////////////////////////////////////////////////////////////////////////////////
// Get List of Grades of elements in multivector
// Grades are given in descending order

template<class CType>
bool GradeList(const MultiV<CType> &vA, Mem<uint> &mList, CType dPrec)
{
	if (!vA.HasStyle()) return false;
	
	if (dPrec == CType(0))
		Tiny(dPrec);

	int *grades = vA.GetStyle().Grades();
	int g;
	uint m_uGADim = vA.GetStyle().GADim();
	uint vsdim = vA.BasePtr()->VSDim();
	uint i,j,n;

	mList = 0;

	for(i=m_uGADim,n=0;i>0;i--)
	{
		if (!::IsZero(vA.m_mData[i-1], dPrec)) 
		{
			g = grades[i-1];
			for(j=0;j<n;j++)
			{
				if (mList[j] == uint(g)) break;
			}
			if (j == n)
			{
				mList++;
				mList[n++] = uint(g);
			}
		}
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////////
// Returns Homogeneous Multivector part.
// Selects that grade vector with the largest magnitude.
// Return result in vHMV.

template<class CType>
bool MultiV<CType>::GetMaxHMV(MultiV<CType> &vHMV, CType dPrec) const
{
	int iGradeCount, iGrade, iSelGrade;
	CType dVal, dMax;
	Mem<uint> mGList;
	MultiV<CType> vG;
	const MultiV<CType> &vA = *this;

	if (dPrec == CType(0))
		Tiny(dPrec);

	if (!GradeList(*this, mGList, dPrec)) return false;

	// Check whether multivector consists of a single grade
	if ((iGradeCount = mGList.Count()) == 1) 
	{
		vHMV = vA;
	}
	else if (iGradeCount > 1)
	{
		// Multivector consists of  more than one grade.
		// Reduce it to vector of grade with largest magnitude.
		iSelGrade = 0;
		dVal = dMax = 0;
		for(iGrade = 0; iGrade < iGradeCount; iGrade++ )
		{
			// Get k-vector of mGList[iGrade]
			vG = vA(mGList[iGrade]);
			if ((dVal = vG.Mag2()) > dMax)
			{
				dMax = dVal;
				iSelGrade = iGrade;
			}
		}

		vHMV = vA(mGList[iSelGrade]); 
	}
	else if (iGradeCount == 0)
	{
		return false;
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////////
// Factor a Blade into vectors
// Works also for null-blades, if at most one basis vector squares to -1.
// This particular basis vector has to be passed to the function in vNegDim.
// if bNorm == true (default) then normalise factored vectors
// factored vectors are always orthogonal!

template<class CType> 
bool FactorBlade(const MultiV<CType> &vA, const MultiV<CType> &vNegDim,
				 MemObj<MultiV<CType> > &mvList, 
				 bool bNorm, MultiV<CType> *pvB, CType *pFac, CType dPrec)
{
	int iGrade, iListOffset;
	CType dVal, dMax;
	Mem<uint> mGList;
	MultiV<CType> vG, vB, vNBlade;

	// Get largest homogeneous multivector
	if (!vA.GetMaxHMV(vNBlade, dPrec))
		return false;

	if (!GradeList(vNBlade, mGList, dPrec)) return false;

	iGrade = mGList[0];
	if (pvB)
		*pvB = vNBlade;

	if (iGrade == 0)
	{
		mvList.Set(0);
		return true;
	}

	uint vsdim = vA.BasePtr()->VSDim(); // Dimension of Vector space

	int i,j, iMaxBase;
	Mem<CType> mVComps;
	Mem<int> miOrder;
	MultiV<CType> vX, vBlade(vA.GetStyle()), vZero(vA.GetStyle()), vDiag;
	MemObj<MultiV<CType> > mvBasis;

	miOrder.Set(vsdim);
	mVComps.Set(vsdim);
	mvBasis.Set(vsdim);
	for (i=0;i<int(vsdim);i++) 
	{
		mVComps[i] = CType(0);
	}

	mVComps[0] = 1;
	for (i=0,j=0;i<int(vsdim);i++) 
	{
		// Create Basis Vector
		mvBasis[i] = MakeVector(vA, 1, mVComps.Data());

		mVComps[j] = 0;
		mVComps[++j] = 1;
	}

	mvList.Set(iGrade); // List of factor vectors has iGrade entries	
	vBlade = CType(1);

	// Check whether blade is null blade
	vX = vNBlade * ~vNBlade;
	//vX.TinyToZero(dPrec);
	if (vX.IsZero(dPrec))
	{
		if (iGrade > 1)
		{
			// Remove Null vector from blade. vB may then be factorized with 
			// the standard method.
			vB = vNegDim * vNBlade;

			// Get null vector from blade vNBlade and make scalar factor unity.
			vX = (vB * (~vNBlade)) / Scalar(vB * ~vB);
			vX.TinyToZero(dPrec);

			// Store null vector in list of factorized vectors
			mvList[0] = vX;
		}
		else
		{
			// If blade consists only of null-vector then end here
			mvList[0] = vNBlade;
			if (pFac)
				*pFac = CType(1);
			return true;
		}
		iListOffset = 1;
	}
	else
	{
		// If vNBlade is not a null blade then proceed
		// with standard factorization
		vB = vNBlade;
		iListOffset = 0;
	}

	vB.TinyToZero(dPrec);

	// Factorize the blade
	for(i = iListOffset; i < iGrade-1; i++)
	{
		MultiV<CType> &vNew = mvList[i];

		// Find a stable vector
		dMax = 0;
		iMaxBase = 0;
		for (j = 0; j < int(vsdim); j++)
		{
			vX = mvBasis[j] * vB;
			if ((dVal = vX.Mag2()) > dMax)
			{
				iMaxBase = j;
				dMax = dVal;
			}
		}

		vNew = ((mvBasis[iMaxBase] * vB) * ~vB) / Scalar(vB * ~vB);

		if (bNorm)
		{
			dVal = CType(sqrt(Mag(vNew.Mag2())));
			if (!::IsZero(dVal, dPrec))
				vNew /= dVal;
		}
		vNew.TinyToZero(dPrec);
		vB = vNew * vB;
		vB.TinyToZero(dPrec);
		if (pFac)
			vBlade ^= vNew;
	}
	
	if (bNorm)
	{
		dVal = CType(sqrt(Mag(vB.Mag2())));
		if (!::IsZero(dVal, dPrec))
			vB /= dVal;
	}
	vB.TinyToZero(dPrec);
	mvList[i] = vB;
	if (pFac)
		vBlade ^= vB;

	// Calc factor between original and new blade
	if (pFac)
	{
		*pFac = Scalar((vA * (~vBlade)) / Scalar(vBlade * (~vBlade)));
	}
	
	return true;
}



////////////////////////////////////////////////////////////////////////////////////
// Factor a Blade into vectors
// Only works for real blades, i.e. a1^a2^...^an blades
// if bNorm == true (default) then normalise factored vectors
// factored vectors are always orthogonal!

template<class CType>
bool FactorBlade(const MultiV<CType> &vA, MemObj<MultiV<CType> > &mvList, 
				 bool bNorm, MultiV<CType> *pvB, CType *pFac, CType dPrec)
{
	int iGrade;
	CType dVal, dMax;
	Mem<uint> mGList;
	MultiV<CType> vG, vB;

	// Get largest homogeneous multivector
	if (!vA.GetMaxHMV(vB, dPrec))
		return false;

	if (!GradeList(vB, mGList, dPrec)) return false;

	iGrade = mGList[0];
	if (pvB)
		*pvB = vB;

	if (iGrade == 0)
	{
		mvList.Set(0);
		return true;
	}

	uint vsdim = vA.BasePtr()->VSDim(); // Dimension of Vector space

	int i,j, iMaxBase;
	Mem<CType> mVComps;
	Mem<int> miOrder;
	MultiV<CType> vX, vBlade(vA.GetStyle()), vZero(vA.GetStyle()), vDiag;
	MemObj<MultiV<CType> > mvBasis;

	miOrder.Set(vsdim);
	mVComps.Set(vsdim);
	mvBasis.Set(vsdim);
	for (i=0;i<int(vsdim);i++) 
	{
		mVComps[i] = CType(0);
	}

	mVComps[0] = 1;
	for (i=0,j=0;i<int(vsdim);i++) 
	{
		// Create Basis Vector
		mvBasis[i] = MakeVector(vA, 1, mVComps.Data());

		mVComps[j] = 0;
		mVComps[++j] = 1;
	}

	mvList.Set(iGrade); // List of factor vectors has iGrade entries	
	vBlade = CType(1);

	// Factorize the blade
	for(i=0;i<iGrade-1;i++)
	{
		MultiV<CType> &vNew = mvList[i];

		// Find a stable vector
		dMax = 0;
		iMaxBase = 0;
		for (j = 0; j < int(vsdim); j++)
		{
			vX = mvBasis[j] * vB;
			if ((dVal = vX.Mag2()) > dMax)
			{
				iMaxBase = j;
				dMax = dVal;
			}
		}

		vNew = ((mvBasis[iMaxBase] * vB) * ~vB) / Scalar(vB * ~vB);

		if (bNorm)
		{
			dVal = CType( sqrt(Mag(vNew.Mag2())) );
			if (!::IsZero(dVal, dPrec))
				vNew /= dVal;
		}
		vNew.TinyToZero(dPrec);
		vB = vNew * vB;
		vB.TinyToZero(dPrec);
		if (pFac)
			vBlade ^= vNew;
	}
	
	if (bNorm)
	{
		dVal = CType(sqrt(Mag(vB.Mag2())));
		if (!::IsZero(dVal, dPrec))
			vB /= dVal;
	}
	vB.TinyToZero(dPrec);
	mvList[i] = vB;
	if (pFac)
		vBlade ^= vB;

	// Calc factor between original and new blade
	if (pFac)
	{
		*pFac = Scalar((vA * (~vBlade)) / Scalar(vBlade * (~vBlade)));
	}
	
	return true;
}


////////////////////////////////////////////////////////////////////////////////////
// Factor a Blade into vectors with given basis
// Only works for real blades, i.e. a1^a2^...^an blades
// if bNorm == true (default) then normalise factored vectors
// factored vectors are always orthogonal!
// mvBasis gives basis vectors onto which to factorize blade.

template<class CType>
bool FactorBlade(const MultiV<CType> &vA, MemObj<MultiV<CType> > &mvList, 
				 MemObj<MultiV<CType> > &mvBasis, 
				 bool bNorm, int *piOrder, CType *pFac, CType dPrec)
{
	Mem<uint> mGList;
	uint uBaseVecCnt = mvBasis.Count();

	if (!GradeList(vA, mGList, dPrec)) return false;
	if (mGList.Count() != 1) return false;

	uint grade = mGList[0]; // Grade of Blade
	if (!grade || uBaseVecCnt < grade) return false;

	uint vsdim = vA.BasePtr()->VSDim(); // Dimension of Vector space

	uint i,j;
	Mem<int> miOrder;
	MultiV<CType> vB(vA), vX, vBlade(vA.GetStyle()), vZero(vA.GetStyle());

	miOrder.Set(vsdim);
	for (i=0;i<vsdim;i++) 
	{
		if (!piOrder) miOrder[i] = i;
		else miOrder[i] = piOrder[i];
	}

	mvList = grade; // List of factor vectors has grade entries	
	vBlade = CType(1);

	for(i=0,j=0;i<grade-1;i++)
	{
		MultiV<CType> &vNew = mvList[i];

		// Go through basis vectors as factor elements
		do
		{
			vNew = Project(vB, mvBasis[miOrder[j++]]); 
		} 
		while (vNew.IsZero(dPrec));

		if (bNorm)
		{
			vNew /= CType(sqrt(Mag(vNew.Mag2())));
		}
		vNew.TinyToZero(dPrec);
		vB = vNew * vB;
		vB.TinyToZero(dPrec);
		vBlade ^= vNew;
	}
	
	if (bNorm)
	{
		vB /= CType(sqrt(Mag(vB.Mag2())));
	}
	vB.TinyToZero(dPrec);
	mvList[i] = vB;
	vBlade ^= vB;

	// Calc factor between original and new blade
	if (pFac)
	{
		*pFac = Scalar((vA * (~vBlade)) / Scalar(vBlade * (~vBlade)));
	}
	
	return true;
}



////////////////////////////////////////////////////////////////////////////////////
// Join of two Blades
// Need to be pure blades to work

template<class CType>
MultiV<CType> Join(const MultiV<CType>& vA, const MultiV<CType>& vB, bool bNorm,
				   CType dPrec)
{
	MultiV<CType> vJ(vB), vX, vZero(vA.GetStyle());
	MemObj<MultiV<CType> > mvAList;
	Mem<uint> mGList;
	CType f;
	uint i,n;

	if (vA.IsZero(dPrec) || vB.IsZero(dPrec))
		return vZero;

	vJ.TinyToZero(dPrec);

	if (!FactorBlade(vA, mvAList, false, (MultiV<CType> *) 0, (CType *)0, dPrec))
	{
		// Check whether vA is Scalar
		if (GradeList(vA, mGList, dPrec) && 
			mGList.Count() == 1 &&
			mGList[0] == 0) 
		{
			if (bNorm)
			{
				f = Scalar(vJ & (~vJ));
				vJ /= CType(sqrt(Mag(f)));
			}

			return vJ;
		}

		return vZero;
	}
	
	// Check whether vB is Scalar
	if (GradeList(vB, mGList, dPrec) && 
		mGList.Count() == 1 &&
		mGList[0] == 0) 
	{
		vJ = vA;
		if (bNorm)
		{
			f = Scalar(vJ & (~vJ));
			vJ /= CType(sqrt(Mag(f)));
		}
		
		return vJ;
	}
	
	
	n = mvAList.Count();
	for(i=0;i<n;i++)
	{
		vX = Reject(vJ, mvAList[n-i-1]);
		vX.TinyToZero(dPrec);

		if (!vX.IsZero(dPrec)) vJ = vX ^ vJ;
	}

	if (bNorm)
	{
		f = Scalar(vJ & (~vJ));
		if (!::IsZero(f, dPrec)) 
			vJ /= (f > CType(0) ? CType(1) : CType(-1)) * CType(sqrt(Mag(f)));
	}

	return vJ;
}


////////////////////////////////////////////////////////////////////////////////////
// Join of two potential null Blades
// vNegDim contains basis vector that squares to minus one
// Need to be pure blades to work

template<class CType>
MultiV<CType> Join(const MultiV<CType>& vABlade, const MultiV<CType>& vBBlade, 
				   const MultiV<CType>& vNegDim, bool bNorm, CType dPrec)
{
	MultiV<CType> vJ, vX, vZero(vABlade.GetStyle()), vA, vB, vBNull;
	MemObj<MultiV<CType> > mvAList;
	Mem<uint> mGList;
	CType f;
	uint i;
	bool bAIsNullBlade;
	bool bBIsNullBlade = false;
	bool bFactOK = false;

	// Make sure we have homogeneous multivectors as input.
	// This does not ensure that they are blade.
	vABlade.GetMaxHMV(vA, dPrec);
	vBBlade.GetMaxHMV(vB, dPrec);

	if (vA.IsZero(dPrec) || vB.IsZero(dPrec))
		return vZero;


	// Check whether vB is Scalar
	if (GradeList(vB, mGList, dPrec) && 
		mGList.Count() == 1 &&
		mGList[0] != 0) 
	{
		// if not then...

		// Check whether vB is null blade.
		if ((vB * vB).IsZero(dPrec))
		{
			bBIsNullBlade = true;
			// Check whether vB is a vector
			if (mGList[0] == 1) 
			{
				vBNull = vB;
				vJ.SetStyle(vB.GetStyle());
				vJ = CType(1);
			}
			else
			{
				// Get part of vB that is not null blade
				vJ = vNegDim * vB;
				// Null vector part
				vBNull = (vJ * ~vB) / CType(sqrt(Scalar(vJ * ~vJ)));
			}
		}
		else
		{
			bBIsNullBlade = false;
			vJ = vB;
		}
	}
	else
	{
		bBIsNullBlade = false;
		vJ.SetStyle(vB.GetStyle());
		vJ = CType(1);		
	}

	// Check whether vA is Scalar
	if (GradeList(vA, mGList, dPrec) && 
		mGList.Count() == 1 &&
		mGList[0] != 0) 
	{
		// Check whether vA is null blade
		if ((vA * vA).IsZero(dPrec))
		{
			bAIsNullBlade = true;
			bFactOK = FactorBlade(vA, vNegDim, mvAList, false, (MultiV<CType> *) 0, (CType *)0, dPrec);
			//printf("mvAList[0]: %s\n", mvAList[0].Str());
		}
		else
		{
			bAIsNullBlade = false;
			bFactOK = FactorBlade(vA, mvAList, false, (MultiV<CType> *) 0, (CType *)0, dPrec);
		}

		if (!bFactOK) 
		{
			return vZero;
		}
	

		uint iAVecCount, iAVecUsedCount;
		iAVecCount = iAVecUsedCount = mvAList.Count();
		// if vA is null blade then first element of mvAList is the null vector part.
		// Therefore, do not reject this part from vJ.
		if (bAIsNullBlade)
			iAVecUsedCount--;

		// Find join of non-null parts
		for(i=0;i<iAVecUsedCount;i++)
		{
			//printf("vJ: %s\n", vJ.Str());
			//printf("mvAList[%d]: %s\n", iAVecCount-i-1, mvAList[iAVecCount-i-1].Str());

			vX = Reject(vJ, mvAList[iAVecCount-i-1]);
			vX.TinyToZero(dPrec);
			
			//printf("vX: %s\n\n", vX.Str());

			if (!vX.IsZero(dPrec)) vJ = vX ^ vJ;
		}
		//printf("Final vJ: %s\n\n", vJ.Str());
	}
	else
	{
		bAIsNullBlade = false;
	}

	// Norm. the join of non-null parts
	if (bNorm)
	{
		f = Scalar(vJ & (~vJ));
		if (!::IsZero(f, dPrec)) 
			vJ /= CType(sqrt(Mag(f)));
	}

	// if neither vA nor vB are null blades then we are finished.
	if (!bAIsNullBlade && !bBIsNullBlade)
	{
		return vJ;
	}
	// if vA and vB are null blades, then...
	else if (bAIsNullBlade && bBIsNullBlade)
	{
		// Evaluate outer product of null blades
		vX = mvAList[0] ^ vBNull;
		vX.TinyToZero(dPrec);
		// If this is zero, then they are equal
		if (vX.IsZero(dPrec))
		{
			// thus add null vector to join
			vJ ^= vBNull;
		}
		else
		{
			// otherwise add outer product of null vectors to join
			vJ ^= vX;
		}
	}
	// if only vA is null blade, then ...
	else if (bAIsNullBlade && !bBIsNullBlade)
	{
		// add null part of vA to join
		vX = vJ ^ mvAList[0];
		// if this is zero then null vector is already contained in vJ.
		// this is possible even if vJ is not a null blade!
		vX.TinyToZero(dPrec);
		if (!vX.IsZero(dPrec))
			vJ = vX;
	}
	// if only vB is null blade, then ...
	else
	{
		// add null part of vB to join
		vX = vJ ^ vBNull;
		// if this is zero then null vector is already contained in vJ.
		// this is possible even if vJ is not a null blade!
		vX.TinyToZero(dPrec);
		if (!vX.IsZero(dPrec))
			vJ = vX;
	}

	// Return join
	return vJ;
}




////////////////////////////////////////////////////////////////////////////////////
// Meet of two Blades
// Need to be pure blades to work. Calculates Meet from Join

template<class CType>
MultiV<CType> Meet(const MultiV<CType>& vA, const MultiV<CType>& vB, CType dPrec)
{
	MultiV<CType> vM(vA.GetStyle()), vJ, viJ;

	vJ = Join(vA, vB, true, dPrec);
	viJ = ~vJ;

	vM = ( (vA & (viJ)) ^ (vB & (viJ)) ) & vJ;
/*
	cStr csT;
	MultiV<CType> vvA(vA), vvB(vB);

	csT = vJ.Str();
	csT = viJ.Str();
	csT = vvA.Str();
	csT = vvB.Str();
	csT = vM.Str();
*/	
	return vM;
}


////////////////////////////////////////////////////////////////////////////////////
// Meet of two Blades
// This also allows for null blades
// Need to be pure blades to work. Calculates Meet from Join

template<class CType>
MultiV<CType> Meet(const MultiV<CType>& vA, const MultiV<CType>& vB, const MultiV<CType>& vNegDim, CType dPrec)
{
	MultiV<CType> vM(vA.GetStyle()), vJ, viJ, vABlade, vBBlade, vBNull, vANull, vX;
	MultiV<CType> vZero(vA.GetStyle());
	bool bAIsNullBlade, bBIsNullBlade;
	Mem<uint> mGList;

	// Separate vA and vB into null component and non-null component
	// vABlade, vBBlade will contain the non-null components
	// vANull, vBNull will contain the null vectors

	// Check whether vA is Scalar
	if (GradeList(vA, mGList, dPrec) && 
		mGList.Count() == 1 &&
		mGList[0] != 0) 
	{
		// if not then...

		// Check whether vA is null blade.
		if ((vA * vA).IsZero(dPrec))
		{
			bAIsNullBlade = true;
			// Check whether vA is a vector
			if (mGList[0] == 1) 
			{
				vANull = vA;
				vABlade.SetStyle(vA.GetStyle());
				vABlade = CType(1);
			}
			else
			{
				// Get part of vA that is not null blade
				vABlade = vNegDim * vA;
				// Null vector part
				vANull = (vABlade * ~vA) / CType(sqrt(Scalar(vABlade * ~vABlade)));
			}
		}
		else
		{
			bAIsNullBlade = false;
			vABlade = vA;
		}
	}
	else
	{
		bAIsNullBlade = false;
		vABlade.SetStyle(vA.GetStyle());
		vABlade = CType(1);		
	}

	// Check whether vB is Scalar
	if (GradeList(vB, mGList, dPrec) && 
		mGList.Count() == 1 &&
		mGList[0] != 0) 
	{
		// if not then...

		// Check whether vB is null blade.
		if ((vB * vB).IsZero(dPrec))
		{
			bBIsNullBlade = true;
			// Check whether vB is a vector
			if (mGList[0] == 1) 
			{
				vBNull = vB;
				vBBlade.SetStyle(vB.GetStyle());
				vBBlade = CType(1);
			}
			else
			{
				// Get part of vB that is not null blade
				vBBlade = vNegDim * vB;
				// Null vector part
				vBNull = (vBBlade * ~vB) / CType(sqrt(Scalar(vBBlade * ~vBBlade)));
			}
		}
		else
		{
			bBIsNullBlade = false;
			vBBlade = vB;
		}
	}
	else
	{
		bBIsNullBlade = false;
		vBBlade.SetStyle(vB.GetStyle());
		vBBlade = CType(1);		
	}


	// Now evaluate meet of non-null parts
	vJ = Join(vABlade, vBBlade, true, dPrec);
	viJ = ~vJ;

	vM = ( (vABlade & (viJ)) ^ (vBBlade & (viJ)) ) & vJ;

	// Check whether null vectors vANull and vBNull are linearly dependent
	// if yes, they contribute to meet
	if (bAIsNullBlade && bBIsNullBlade)
	{
		vX = vANull ^ vBNull;
		vX.TinyToZero();

		if (vX.IsZero(dPrec))
		{
			vX = vM ^ vANull;
			vX.TinyToZero();

			if (!vX.IsZero(dPrec))
				vM = vX;
		}
	}

	return vM;
}


////////////////////////////////////////////////////////////////////////////////////
// Calculate matrix of MV with GP Mult. Table
// if bEven == true, then use only even subspace of GA

template<class CType>
bool CalcMultiVGPMat(const MultiV<CType> &vA, Matrix<CType> &M, tMVPos bPos, bool bEven)
{
	uint m_uGADim = vA.GetStyle().GADim();
	int *gpTable = vA.GetStyle().GPTable();
	int *grades = vA.GetStyle().Grades();

	uint i,j,k;
	int row, col;
	CType f;

	M.Resize(m_uGADim, (bEven ? (m_uGADim >> 1) : m_uGADim));
	M = CType(0);

	if (bPos == Left) // Multivector multiplied from left
	{
		// Problem: a_i*b_j*g_ijk = c_k. find singular multivectors of a_i multivector.
		// g_ijk is mult. table of geometric product.
		
		// i counts through m_mData of left MV.
		for(i=0,k=0;i<m_uGADim;i++)
		{
			// j counts through m_mData of Right MV and gives column of M
			for(j=0,col=1;j<m_uGADim;j++,k++)
			{
				if ((bEven && grades[j]%2 == 0) || !bEven)
				{
					// The abs value of gpTable at i,j gives row of M.
					// The entry in M is given by the factor in gpTable times a_i.
					if ((row = gpTable[k]) < 0)
					{ 
						row = -row;
						f = CType(-1);
					}
					else f = CType(1);
					
					if (row > 0) M(row, col) = f * vA.m_mData[i];
					col++;
				}
			}
		}
	}
	else // Multivector multiplied from right
	{
		// Problem: a_i*b_j*g_ijk = c_k. find singular multivectors of b_j multivector.
		// g_ijk is mult. table of geometric product.

		// i counts through m_mData of Right MV.
		for(i=0;i<m_uGADim;i++)
		{
			k = i;
			// j counts through m_mData of left MV and gives column of M
			for(j=0,col=1;j<m_uGADim;j++,k+=m_uGADim)
			{
				if ((bEven && grades[j]%2 == 0) || !bEven)
				{
					// The abs value of gpTable at i,j gives row of M.
					// The entry in M is given by the factor in gpTable times b_i.
					if ((row = gpTable[k]) < 0)
					{ 
						row = -row;
						f = CType(-1);
					}
					else f = CType(1);
					
					if (row > 0) M(row, col) = f * vA.m_mData[i];
					col++;
				}
			}
		}
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////////
// Calculate matrix of MV with GP Mult. Table
// mGrades gives list of grades that are allowed

template<class CType>
bool CalcMultiVGPMat(const MultiV<CType> &vA, Matrix<CType> &M, tMVPos bPos, const Mem<int>& mGrades)
{
	uint m_uGADim = vA.GetStyle().GADim();
	uint vsdim = vA.BasePtr()->VSDim();
	int *gpTable = vA.GetStyle().GPTable();
	int *grades = vA.GetStyle().Grades();

	uint i,j,k,r,nGradeCount;
	int row, col,coldim = 0;
	CType f;

	if (!(nGradeCount = mGrades.Count())) return false;
	for(i=0;i<nGradeCount;i++)
	{
		coldim += Comb(vsdim, mGrades[i]);
	}

	M.Resize(m_uGADim, coldim);
	M = CType(0);

	if (bPos == Left) // Multivector multiplied from left
	{
		// Problem: a_i*b_j*g_ijk = c_k. find singular multivectors of a_i multivector.
		// g_ijk is mult. table of geometric product.
		
		// i counts through m_mData of left MV.
		for(i=0,k=0;i<m_uGADim;i++)
		{
			// j counts through m_mData of Right MV and gives column of M
			for(j=0,col=1;j<m_uGADim;j++,k++)
			{
				for(r=0;r<nGradeCount;r++)
				{
					if (grades[j] == mGrades[r])
					{
						// The abs value of gpTable at i,j gives row of M.
						// The entry in M is given by the factor in gpTable times a_i.
						if ((row = gpTable[k]) < 0)
						{ 
							row = -row;
							f = CType(-1);
						}
						else f = CType(1);
						
						if (row > 0) M(row, col) = f * vA.m_mData[i];
						col++;
						break;
					}
				}
			}
		}
	}
	else // Multivector multiplied from right
	{
		// Problem: a_i*b_j*g_ijk = c_k. find singular multivectors of b_j multivector.
		// g_ijk is mult. table of geometric product.

		// i counts through m_mData of Right MV.
		for(i=0;i<m_uGADim;i++)
		{
			k = i;
			// j counts through m_mData of left MV and gives column of M
			for(j=0,col=1;j<m_uGADim;j++,k+=m_uGADim)
			{
				for(r=0;r<nGradeCount;r++)
				{
					if (grades[j] == mGrades[r])
					{
						// The abs value of gpTable at i,j gives row of M.
						// The entry in M is given by the factor in gpTable times b_i.
						if ((row = gpTable[k]) < 0)
						{ 
							row = -row;
							f = CType(-1);
						}
						else f = CType(1);
						
						if (row > 0) M(row, col) = f * vA.m_mData[i];
						col++;
						break;
					}
				}
			}
		}
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////////
// Calculate matrix of MV with GP Mult. Table
// vMask sets allowed elements
// ProductType == 0: GP, == 1:IP, == 2:OP.

template<class CType>
bool CalcMultiVGPMatMask(const MultiV<CType> &vA, Matrix<CType> &M, tMVPos bPos, 
						 const Mem<int>& mMask, EMVOpType eOPType)
{
	uint m_uGADim = vA.GetStyle().GADim();
	uint vsdim = vA.BasePtr()->VSDim();
	int *gpTable;
	int *grades = vA.GetStyle().Grades();

	if (eOPType == MVOP_GEO)
		gpTable = vA.GetStyle().GPTable();
	else if (eOPType == MVOP_INNER)
		gpTable = vA.GetStyle().IPTable();
	else if (eOPType == MVOP_OUTER)
		gpTable = vA.GetStyle().OPTable();
	else
		return false;

	uint i,j,k;
	int row, col,coldim = 0;
	CType f, val;

	if (mMask.Count() != m_uGADim) return false;

	for(i=0;i<m_uGADim;i++)
	{
		if (Mag(mMask[i]) > coldim) coldim = Mag(mMask[i]);
	}

	M.Resize(m_uGADim, coldim);
	M = CType(0);

	if (bPos == Left) // Multivector is multiplied from left
	{
		// Problem: a_i*b_j*g_ijk = c_k. find singular multivectors of a_i multivector.
		// g_ijk is mult. table of geometric product.
		
		// i counts through m_mData of left MV.
		for(i=0,k=0;i<m_uGADim;i++)
		{
			val = vA.m_mData[i];
			if (val != CType(0))
			{
				// j counts through m_mData of Right MV and gives column of M
				for(j=0;j<m_uGADim;j++,k++)
				{
					if ((col = mMask[j]))
					{
						// The abs value of gpTable at i,j gives row of M.
						// The entry in M is given by the factor in gpTable times a_i.
						if ((row = gpTable[k]) < 0)
						{ 
							row = -row;
							f = CType(-1);
						}
						else f = CType(1);

						if (col < 0)
						{
							col = -col;
							f *= CType(-1);
						}
						
						if (row > 0) M(row, col) += f * val;
					}
				}
			}
			else k += m_uGADim;
		}
	}
	else // Multivector is multiplied from right
	{
		// Problem: a_i*b_j*g_ijk = c_k. find singular multivectors of b_j multivector.
		// g_ijk is mult. table of geometric product.

		// i counts through m_mData of Right MV.
		for(i=0;i<m_uGADim;i++)
		{
			val = vA.m_mData[i];
			if (val != CType(0))
			{
				k = i;
				// j counts through m_mData of left MV and gives column of M
				for(j=0;j<m_uGADim;j++,k+=m_uGADim)
				{
					if ((col = mMask[j]))
					{
						// The abs value of gpTable at i,j gives row of M.
						// The entry in M is given by the factor in gpTable times b_i.
						if ((row = gpTable[k]) < 0)
						{ 
							row = -row;
							f = CType(-1);
						}
						else f = CType(1);
						
						if (col < 0)
						{
							col = -col;
							f *= CType(-1);
						}

						if (row > 0) M(row, col) += f * val;
					}
				}
			}
		}
	}

	return true;
}



////////////////////////////////////////////////////////////////////////////////////
// Get Product Matrix for a list of Multivectors.

template<class CType>
bool CalcMVListGPMatMask(const MemObj<MultiV<CType> > &mvAList, Matrix<CType> &M, 
						 tMVPos bPos, const Mem<int>& mMask, EMVOpType eOPType,
						 bool bPruneNullRows)
{
	uint i,row,n,rowcnt, col,colcnt, ActRow;
	uint uGADim = mvAList[0].GetStyle().GADim();
	int *grades = mvAList[0].GetStyle().Grades();

	Mem<CType> mRow;
	Matrix<CType> M1;
	
	n = mvAList.Count();
	for (i=0;i<n;i++)
	{
		// Create Matrix of MV with GP Mult.Table
		if (!CalcMultiVGPMatMask(mvAList[i], M1, bPos, mMask, eOPType)) 
			return false;	
		
		// Accumulate equations
		// and eliminate null rows if bPruneNullRow = true
		if (i == 0)
		{
			colcnt = M1.Cols();
			rowcnt = uGADim;

			mRow = colcnt;
			M.Resize(n * uGADim, colcnt);
			M = CType(0);
			ActRow = 0;
		}

		for(row=1;row<=rowcnt;row++)
		{
			M1.GetRow(row, mRow.Data());
			for(col=0;col<colcnt;col++)
			{
				if (!bPruneNullRows || mRow[col] != CType(0))
				{
					M.SetRow(++ActRow, mRow.Data());
					break;
				}
			}
		}
	}

	M.Resize((ActRow == 0 ? 1 : ActRow), colcnt);
	return true;
}


////////////////////////////////////////////////////////////////////////////////////
// Find SVD of Multivector
//
// if bRight = true (default), finds Right singular multivectors
// else finds left singular multivectors.

template<class CType>
bool MultiVSVD(const MultiV<CType> &mvA, MemObj<MultiV<CType> > &mvList, Mem<CType> &evList,
			   tMVPos bPos)
{
	uint i,j;
	uint m_uGADim = mvA.m_pStyle->GADim();

	Matrix<CType> M,U,V,D;

	// Reset Lists
	mvList = 0;
	evList = 0;

	// Create Matrix of MV with GP Mult.Table
	if (!CalcMultiVGPMat(mvA, M, bPos, false)) return false;	

	// SVD of M gives singular multivectors of mv
	if (M.SVD(U,D,V) <= 0) return false;

	SVDOrder(D, U, V, false); // Order with ascending singularvalues

	mvList = m_uGADim;
	evList = m_uGADim;

	// Columns of U give left singular multivectors
	// Columns of V give Right singular multivectors
	for(i=0;i<m_uGADim;i++)
	{
		MultiV<CType> &mvNew = mvList[i];
		mvNew.SetStyle(*mvA.m_pStyle);

		evList[i] = D(1, i+1);
		for(j=0;j<m_uGADim;j++)
		{
			mvNew.m_mData[j] = V(j+1, i+1);
		}
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////////
// Find Eigenmultivectors and Eigenvalues of Multivector using Jacobi decomposition
//
// if bRight = true (default), finds Right eigenmultivectors and -values multivectors
// else finds left eigenmultivectors and -values.

template<class CType>
uint MultiVJacobi(const MultiV<CType> &vA, MemObj<MultiV<CType> > &mvEVecList, 
				  Mem<CType> &mEValList, tMVPos bPos)
{
	uint i,j,roots;
	uint m_uGADim = vA.m_pStyle->GADim();

	Matrix<CType> M,EVal,EVec;

	// Reset Lists
	mvEVecList = 0;
	mEValList = 0;

	// Create Matrix of MV with GP Mult.Table
	if (!CalcMultiVGPMat(vA, M, (bPos == Left ? Right : Left), false)) return false;	

//	M *= ~M;
	roots = M.Jacobi(EVal, EVec);

	mvEVecList = m_uGADim;
	mEValList = m_uGADim;

	for(i=0;i<m_uGADim;i++)
	{
		MultiV<CType> &vNew = mvEVecList[i];
		vNew.SetStyle(*vA.m_pStyle);

		mEValList[i] = EVal(1, i+1);
		for(j=0;j<m_uGADim;j++)
		{
			vNew.m_mData[j] = EVec(j+1, i+1);
		}
	}


	return roots;
}



////////////////////////////////////////////////////////////////////////////////////
// Calculate general inverse of multivector
//
// Solves equation vA & vX = vB if bPos = Left (default),
// or vX & vA = vB if bPos = Right, for vX.

template<class CType>
bool MultiVSolve(const MultiV<CType> &vA, const MultiV<CType> &vB, 
				 MultiV<CType> &vX, tMVPos bPos,
				 Mem<CType> *pmDiag)
{
	uint i;
	uint m_uGADim = vA.m_pStyle->GADim();

	Matrix<CType> M,U,V,D,D2, Inv, X(m_uGADim, 1);
	
	// Create Matrix of v with GP Mult.Table
	if (!CalcMultiVGPMat(vA, M, bPos, false)) return false;	

	// SVD of M gives singular multivectors of v
	if (M.SVD(U,D,V) <= 0) return false;

	D.TinyToZero();
	D2 = D;
	D.InvComps();
	D.Diagonal();

	Inv = V * D * (~U);
	
	if (pmDiag)
	{
		SVDOrder(D2, U, V, false);

		pmDiag->Set(m_uGADim);
		for(i=0;i<m_uGADim;i++)
			(*pmDiag)[i] = D2(1,i+1);
	}

	// Make vB a column vector
	X = vB.m_mData.Data();

	// Solve for X
	X = Inv * X;

	vX.SetStyle(*vA.m_pStyle);
	for(i=0;i<m_uGADim;i++)
	{
		vX.m_mData[i] = X(i+1,1);
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////////
// General Eigen-Multivector Problem Solve
//
// Solves Equations of type A & X = B & X for X.
// Right1 and Right2 give position of A and B on Right or left of
// X respectively.

template<class CType>
bool MultiVGEVSolve(const MultiV<CType> &vA, const MultiV<CType> &vB, 
					MemObj<MultiV<CType> > &mvXList, tMVPos bPos1, tMVPos bPos2, bool bEven)
{
	uint i,j,row,n;
	uint m_uGADim = vA.GetStyle().GADim();
	int *grades = vA.GetStyle().Grades();

	Matrix<CType> M1, M2 ,U,V,D, Inv, X(m_uGADim, 1);
	
	// Create Matrix of MV with GP Mult.Table
	if (!CalcMultiVGPMat(vA, M1, bPos1, bEven)) return false;	

	// Create Matrix of MV with GP Mult.Table
	if (!CalcMultiVGPMat(vB, M2, bPos2, bEven)) return false;	

	// Solve (M1 X - M2 X) = 0
	M1 -= M2;

	// SVD of M gives singular multivectors of mv
	if (M1.SVD(U,D,V) <= 0) return false;

	SVDOrder(D, U, V, false); // Order with ascending singularvalues

	D.TinyToZero();

	// Reset Solution List
	mvXList = 0;

	// Get all solution multivectors
	n = D.Cols();
	for(i=0;i<n;i++)
	{
		if (D(1,i+1) != CType(0)) break;

		mvXList++; // Add Element to Solution List
		MultiV<CType> &vNew = mvXList[i];

		vNew.SetStyle(*vA.m_pStyle);

		for(j=0,row=1;j<m_uGADim;j++)
		{
			if ((bEven && grades[j]%2 == 0) || !bEven)
			{
				vNew.m_mData[j] = V(row++, i+1);
			}
		}
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////////
// General Eigen-Multivector Problem Solve with Mask
//
// Solves Equations of type A & X = B & X for X.
// Right1 and Right2 give position of A and B on Right or left of
// X respectively.

template<class CType>
bool MultiVGEVSolveMask(const MultiV<CType> &vA, const MultiV<CType> &vB, 
					MemObj<MultiV<CType> > &mvXList, Mem<CType>& mDiag, 
					tMVPos bPos1, tMVPos bPos2, const Mem<int>& mMask)
{
	uint i,j,row,n;
	uint m_uGADim = vA.GetStyle().GADim();
	int *grades = vA.GetStyle().Grades();

	Matrix<CType> M1, M2 ,U,V,D, Inv, X(m_uGADim, 1);
	
	// Create Matrix of MV with GP Mult.Table
	if (!CalcMultiVGPMatMask(vA, M1, bPos1, mMask, MVOP_GEO)) return false;	

	// Create Matrix of MV with GP Mult.Table
	if (!CalcMultiVGPMatMask(vB, M2, bPos2, mMask, MVOP_GEO)) return false;	

	// Solve (M1 X - M2 X) = 0
	M1 -= M2;

	// SVD of M gives singular multivectors of mv
	if (M1.SVD(U,D,V) <= 0) return false;

	SVDOrder(D, U, V, false); // Order with ascending singularvalues

	D.TinyToZero();

	// Get all solution multivectors
	n = D.Cols();
	mDiag = n;

	// Reset Solution List
	mvXList = n;

	for(i=0;i<n;i++)
	{
		//if (D(1,i+1) != CType(0)) break;
		mDiag[i] = D(1, i+1);

		//mvXList++; // Add Element to Solution List
		MultiV<CType> &vNew = mvXList[i];

		vNew.SetStyle(*vA.m_pStyle);

		for(j=0,row=1;j<m_uGADim;j++)
		{
			if (mMask[j]) vNew.m_mData[j] = V(row++, i+1);
			else vNew.m_mData[j] = CType(0);
		}
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////////
// General Eigen-Multivector Problem Solve with Mask
//
// Solves Equations of type A & X = B & X for X.
// bPos1 and bPos2 give position of A and B on Right or left of
// X respectively.
// Mask defines which element in m_uGADim multivector is mapped to which element in solution mv.
// 0 means element is not used.

template<class CType>
bool MultiVGEVSolveMask(const MemObj<MultiV<CType> > &mvAList, const MemObj<MultiV<CType> >&mvBList, 
					MemObj<MultiV<CType> > &mvXList, Mem<CType>& mDiag, 
					tMVPos bPos1, tMVPos bPos2, const Mem<int>& mMask)
{
	uint i,j,row,n,iPos,rowcnt, col,colcnt, ActRow;
	uint m_uGADim = mvAList[0].GetStyle().GADim();
	int *grades = mvAList[0].GetStyle().Grades();

	Mem<CType> mRow;
	Matrix<CType> M, M1, M2 ,U,V,D, Inv, X(m_uGADim, 1);
	
	n = mvAList.Count();
	for (i=0;i<n;i++)
	{
		// Create Matrix of MV with GP Mult.Table
		if (!CalcMultiVGPMatMask(mvAList[i], M1, bPos1, mMask, MVOP_GEO)) return false;	
		
		// Create Matrix of MV with GP Mult.Table
		if (!CalcMultiVGPMatMask(mvBList[i], M2, bPos2, mMask, MVOP_GEO)) return false;	
		
		// Solve (M1 X - M2 X) = 0
		M1 -= M2;

		// Accumulate equations
		// and eliminate null rows
		if (i == 0)
		{
			colcnt = M1.Cols();
			rowcnt = m_uGADim;

			mRow = colcnt;
			M.Resize(n * m_uGADim, colcnt);
			M = CType(0);
			ActRow = 0;
		}

		for(row=1;row<=rowcnt;row++)
		{
			M1.GetRow(row, mRow.Data());
			for(col=0;col<colcnt;col++)
			{
				if (mRow[col] != CType(0))
				{
					M.SetRow(++ActRow, mRow.Data());
					break;
				}
			}
		}
	}

	M.Resize(ActRow, colcnt);

	// SVD of M gives singular multivectors of mv
	if (M.SVD(U,D,V) <= 0) return false;

	SVDOrder(D, U, V, false); // Order with ascending singularvalues

	D.TinyToZero();

	// Get all solution multivectors
	n = D.Cols();
	mDiag = n;

	// Reset Solution List
	mvXList = n;

	for(i=0;i<n;i++)
	{
		//if (D(1,i+1) != CType(0)) break;
		mDiag[i] = D(1, i+1);

		//mvXList++; // Add Element to Solution List
		MultiV<CType> &vNew = mvXList[i];

		vNew.SetStyle(*mvAList[0].m_pStyle);

		for(j=0,row=1;j<m_uGADim;j++)
		{
			if ((iPos = mMask[j])) vNew.m_mData[j] = V(iPos/*row++*/, i+1);
			else vNew.m_mData[j] = CType(0);
		}
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////////
// Evaluate Versor from two Multivector sets connected by it.
//
// Solves Versor Equations of type V & A - B & V = 0 for V.
// Implements constraint V & ~V = 1 via A & ~V - ~V & B = 0.
// Mask defines which element in m_uGADim multivector is mapped to which element in solution mv.
// 0 means element is not used.

template<class CType>
bool EvalVersor(const MemObj<MultiV<CType> > &mvAList, const MemObj<MultiV<CType> >&mvBList, 
				MemObj<MultiV<CType> > &mvXList, Mem<CType>& mDiag, const Mem<int>& mMask)
{
	uint i,j,row,n,iPos,rowcnt, col,colcnt, ActRow;
	uint m_uGADim = mvAList[0].GetStyle().GADim();
	int *grades = mvAList[0].GetStyle().Grades();

	Mem<CType> mRow;
	Matrix<CType> M, M1, M2 ,U,V,D, Inv, X(m_uGADim, 1);
	
	n = mvAList.Count();
	for (i=0;i<n;i++)
	{
		// Create Matrix of MV with GP Mult.Table
		if (!CalcMultiVGPMatMask(mvAList[i], M1, Right, mMask, MVOP_GEO)) return false;	
		
		// Create Matrix of MV with GP Mult.Table
		if (!CalcMultiVGPMatMask(mvBList[i], M2, Left, mMask, MVOP_GEO)) return false;	
		
		// Solve (M1 X - M2 X) = 0
		M1 -= M2;

		// Accumulate equations
		// and eliminate null rows
		if (i == 0)
		{
			colcnt = M1.Cols();
			rowcnt = m_uGADim;

			mRow = colcnt;
			M.Resize(n * m_uGADim, colcnt);
			M = CType(0);
			ActRow = 0;
		}

		for(row=1;row<=rowcnt;row++)
		{
			M1.GetRow(row, mRow.Data());
			for(col=0;col<colcnt;col++)
			{
				if (mRow[col] != CType(0))
				{
					M.SetRow(++ActRow, mRow.Data());
					break;
				}
			}
		}
	}

	M.Resize(ActRow, colcnt);

	// SVD of M gives singular multivectors of mv
	if (M.SVD(U,D,V) <= 0) return false;

	SVDOrder(D, U, V, false); // Order with ascending singularvalues

	D.TinyToZero();

	// Get all solution multivectors
	n = D.Cols();
	mDiag = n;

	// Reset Solution List
	mvXList = n;

	for(i=0;i<n;i++)
	{
		//if (D(1,i+1) != CType(0)) break;
		mDiag[i] = D(1, i+1);

		//mvXList++; // Add Element to Solution List
		MultiV<CType> &vNew = mvXList[i];

		vNew.SetStyle(*mvAList[0].m_pStyle);

		for(j=0,row=1;j<m_uGADim;j++)
		{
			if ((iPos = mMask[j])) vNew.m_mData[j] = V(iPos/*row++*/, i+1);
			else vNew.m_mData[j] = CType(0);
		}
	}

	return true;
}





////////////////////////////////////////////////////////////////////////////////////
// General Eigen-Multivector Problem Type 2 Solve
//
// Solves Equations of type A & X - B & X  = C for X.
// Right1 and Right2 give position of X on Right or left of
// A and B respectively.

template<class CType>
bool MultiVGEVSolve(const MultiV<CType> &vA, const MultiV<CType> &vB, const MultiV<CType> &vC, 
					MultiV<CType> &vX, tMVPos bPos1, tMVPos bPos2, const Mem<int>& mGrades)
{
	uint i,j,row,n;
	uint m_uGADim = vA.GetStyle().GADim();
	int *grades = vA.GetStyle().Grades();

	Matrix<CType> M1, M2 ,U,V,D, Inv, X(m_uGADim, 1);
	
	// Create Matrix of MV with GP Mult.Table
	if (!CalcMultiVGPMat(vA, M1, bPos1, mGrades)) return false;	

	// Create Matrix of MV with GP Mult.Table
	if (!CalcMultiVGPMat(vB, M2, bPos2, mGrades)) return false;	

	// Solve (M1 X - M2 X) = 0
	M1 -= M2;

	// SVD of M gives singular multivectors of mv
	if (M1.SVD(U,D,V) <= 0) return false;

	D.TinyToZero();
	D.InvComps();
	D.Diagonal();

	Inv = V * D * (~U);
	
	// Make vB a column vector
	X = vC.m_mData.Data();

	// Solve for X
	X = Inv * X;

	n = mGrades.Count();
	vX.SetStyle(*vA.m_pStyle);
	for(i=0,row=1;i<m_uGADim;i++)
	{
		for(j=0;j<n;j++)
		{
			if (grades[i] == mGrades[j])
			{
				vX.m_mData[i] = X(row,1);
				row++;
				break;
			}
		}
	}


	return true;
}


////////////////////////////////////////////////////////////////////////////////////
// General Eigen-Multivector Problem Type 2 Solve
//
// Solves Equations of type A & X - B & X  = C for X.
// Right1 and Right2 give position of X on Right or left of
// A and B respectively.
// mMask gives parts of Multivector that lie in solution space

template<class CType>
bool MultiVGEVSolveMask(const MultiV<CType> &vA, const MultiV<CType> &vB, const MultiV<CType> &vC, 
					MultiV<CType> &vX, tMVPos bPos1, tMVPos bPos2, const Mem<int>& mMask)
{
	uint i,row;
	uint m_uGADim = vA.GetStyle().GADim();
	int *grades = vA.GetStyle().Grades();

	Matrix<CType> M1, M2 ,U,V,D, Inv, X(m_uGADim, 1);
	
	// Create Matrix of MV with GP Mult.Table
	if (!CalcMultiVGPMatMask(vA, M1, bPos1, mMask, MVOP_GEO)) return false;	

	// Create Matrix of MV with GP Mult.Table
	if (!CalcMultiVGPMatMask(vB, M2, bPos2, mMask, MVOP_GEO)) return false;	

	// Solve (M1 X - M2 X) = 0
	M1 -= M2;

	// SVD of M gives singular multivectors of mv
	if (M1.SVD(U,D,V) <= 0) return false;

	D.TinyToZero();
	D.InvComps();
	D.Diagonal();

	Inv = V * D * (~U);
	
	// Make vB a column vector
	X = vC.m_mData.Data();

	// Solve for X
	X = Inv * X;

	vX.SetStyle(*vA.m_pStyle);
	for(i=0,row=1;i<m_uGADim;i++)
	{
		if (mMask[i])
		{
			vX.m_mData[i] = X(row,1);
			row++;
		}
	}


	return true;
}

////////////////////////////////////////////////////////////////////////////////////
/// Approximate exponential of multivector

template<class CType>
MultiV<CType> Exp(const MultiV<CType>& vA, int iOrder)
{
	MultiV<CType> vX(vA), vR(vA.GetStyle());
	int i;

	vR = CType(1);

	for(i=0;i<iOrder;i++)
	{
		vR += vX;
		vX &= vA / CType(i+2);
	}

	return vR;
}


//////////////////////////////////////////////
/// Multivector Error Propagation Functions

//////////////////////////////////////////////
// Evaluate Error Propagating Mean value of Product
// vC: Resultant multivector
// eOPType: which product
// vA, vB: (Mean) Multivectors of which product is to be evaluated
// Cab: Cross-Covariance Matrix of A and B
// bUseCrossCov: if true Cab is used in calculation, otherwise not.

template<class CType>  
bool EvalEPMeanProd (MultiV<CType>& vC, 
					 EMVOpType eOPType, 
					 const MultiV<CType> &vA, const MultiV<CType> &vB,
					 const Matrix<CType> &Cab)
{
	uint uGADim = vA.m_uGADim;

	// Check for right dimensions and types
	if (vA.GetBase().BaseID() != vB.GetBase().BaseID() || 
		uGADim != Cab.Rows() || uGADim != Cab.Cols())
		return false;

	// Set MV Style of resultant MV
	vC.SetStyle(vA.GetStyle());
	vC = CType(0);

	// Pointer to product table
	int *pProdTable = 0;

	if (eOPType == MVOP_GEO)
	{
		pProdTable = vA.GetStyle().GPTable();
	}
	else if (eOPType == MVOP_INNER)
	{
		pProdTable = vA.GetStyle().IPTable();
	}
	else if (eOPType == MVOP_OUTER)
	{
		pProdTable = vA.GetStyle().OPTable();
	}
	else if (eOPType == MVOP_ADD)
	{
		vC = vA + vB;
		return true;
	}
	else if (eOPType == MVOP_SUB)
	{
		vC = vA - vB;
		return true;
	}

	if (!pProdTable)
		return false;

	// Problem: Cab^ij * g_ij^k = c_k. 
	// g_ijk is mult. table of geometric product.
	// Evaluate contraction of covariance matrix with multiplication tensor.
	// Add the result to vC.
	
	int iPos;
	uint i,j,k;
	CType fValA, fValB, fCab;
	const Mem<CType> &mA = vA.m_mData;
	const Mem<CType> &mB = vB.m_mData;
	Mem<CType> &mC = vC.m_mData;

	// Pointer to covariance matrix
	const CType *pCab = Cab.Data();

	// i counts through m_mData of left MV.
	for ( i = 0, k = 0; i < uGADim; i++ )
	{
		fValA = mA[i];

		// j counts through m_mData of Right MV and gives column of M
		for ( j = 0; j < uGADim; j++, k++ )
		{
			// Check whether product has to result in zero
			if ((iPos = pProdTable[k]) == 0)
				continue;

			fCab = pCab[k];

			fValB = mB[j];
			if ((fValA != 0 && fValB != CType(0)) || fCab != CType(0))
			{
				// The abs value of pProdTable at i,j gives position in vC
				if (iPos < 0)
				{ 
					mC[-iPos - 1] -= fValA * fValB + fCab;
				}
				else
				{ 
					mC[iPos - 1] += fValA * fValB + fCab;
				}
			}
		}
	}

	return true;
}



// Evaluate Error Propagating Covariance Matrix of result of product
// Cuu: Resultant Covariance Matrix
// eOPType: which product
// vX, vY: (Mean) Multivectors of which product is to be evaluated
// Cxx, Cyy: Covariance Matrices of X and Y
// Cxy: Cross-Covariance Matrix of X and Y

template<class CType>  
bool EvalEPCovMatProd (Matrix<CType> &Cuu, EMVOpType eOPType, 
						const MultiV<CType> &vX, const MultiV<CType> &vY,
						const Matrix<CType> &Cxx, const Matrix<CType> &Cyy, 
						const Matrix<CType> &Cxy)
{
	uint uGADim = vX.m_uGADim;

	// Check for right dimensions and types
	if (vX.GetBase().BaseID() != vY.GetBase().BaseID() || 
		uGADim != Cxy.Rows() || uGADim != Cxy.Cols() ||
		uGADim != Cxx.Rows() || uGADim != Cxx.Cols() ||
		uGADim != Cyy.Rows() || uGADim != Cyy.Cols())
		return false;

	// Pointer to product table
	int *pProdTable = 0;

	if (eOPType == MVOP_GEO)
	{
		pProdTable = vX.GetStyle().GPTable();
	}
	else if (eOPType == MVOP_INNER)
	{
		pProdTable = vX.GetStyle().IPTable();
	}
	else if (eOPType == MVOP_OUTER)
	{
		pProdTable = vX.GetStyle().OPTable();
	}
	else if (eOPType == MVOP_ADD)
	{
		Cuu = Cxx + Cyy + Cxy + (~Cxy);
		return true;
	}
	else if (eOPType == MVOP_SUB)
	{
		Cuu = Cxx + Cyy - Cxy - (~Cxy);
		return true;
	}

	if (!pProdTable)
		return false;

	int i;
	Mem<int> mMask;

	mMask.Set(uGADim);
	for ( i = 0; i < int(uGADim); i++ )
	{
		mMask[i] = i + 1;
	}

	// Create G matrices.
	Matrix<CType> Gx, Gy, Gxt, Gyt, M;

	if (!CalcMultiVGPMatMask(vX, Gx, Left, mMask, eOPType))
		return false;

	if (!CalcMultiVGPMatMask(vY, Gy, Right, mMask, eOPType))
		return false;

	// Create transposed matrices
	Gxt = ~Gx;
	Gyt = ~Gy;

	// Initialize resultant covariance matrix
	Cuu.Resize(uGADim, uGADim);

	// Evaluate Covariance Matrix
	Cuu = Gy * Cxy * Gxt;
	Cuu += ~Cuu + Gy * Cxx * Gyt + Gx * Cyy * Gxt;

	return true;
}


// Evaluate Error Propagating Cross-Covariance Matrix of result of product
// Cuz: Resultant Cross-Covariance Matrix
// eOPType: which product
// vX, vY: (Mean) Multivectors of which product is to be evaluated
// Cxz, Cyz: Cross-Covariance Matrices of X and Y with Z

template<class CType>  
bool EvalEPCrossCovMatProd (Matrix<CType> &Cuz, EMVOpType eOPType, 
							const MultiV<CType> &vX, const MultiV<CType> &vY,
							const Matrix<CType> &Cxz, const Matrix<CType> &Cyz)
{
	uint uGADim = vX.m_uGADim;

	// Check for right dimensions and types
	if (vX.GetBase().BaseID() != vY.GetBase().BaseID() || 
		uGADim != Cxz.Rows() || uGADim != Cxz.Cols() ||
		uGADim != Cyz.Rows() || uGADim != Cyz.Cols())
		return false;

	// Pointer to product table
	int *pProdTable = 0;

	if (eOPType == MVOP_GEO)
	{
		pProdTable = vX.GetStyle().GPTable();
	}
	else if (eOPType == MVOP_INNER)
	{
		pProdTable = vX.GetStyle().IPTable();
	}
	else if (eOPType == MVOP_OUTER)
	{
		pProdTable = vX.GetStyle().OPTable();
	}
	else if (eOPType == MVOP_ADD)
	{
		Cuz = Cxz + Cyz;
		return true;
	}
	else if (eOPType == MVOP_SUB)
	{
		Cuz = Cxz - Cyz;
		return true;
	}

	if (!pProdTable)
		return false;

	int i;
	Mem<int> mMask;

	mMask.Set(uGADim);
	for ( i = 0; i < int(uGADim); i++ )
	{
		mMask[i] = i + 1;
	}

	// Create G matrices.
	Matrix<CType> Gx, Gy, Gxt, Gyt, M;

	if (!CalcMultiVGPMatMask(vX, Gx, Left, mMask, eOPType))
		return false;

	if (!CalcMultiVGPMatMask(vY, Gy, Right, mMask, eOPType))
		return false;

	// Initialize resultant covariance matrix
	Cuz.Resize(uGADim, uGADim);

	// Evaluate Cross-Covariance Matrix
	Cuz = Gy * Cxz + Gx * Cyz;

	return true;
}

