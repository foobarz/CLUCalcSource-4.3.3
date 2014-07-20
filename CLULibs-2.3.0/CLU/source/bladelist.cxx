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


// BladeList Class Definitions ------------------------------

#include "bladelist.h"

#include "makestr.h"
#include "mathelp.h"



////////////////////////////////////////////////////////////////////////////////////
// Default Constructor

template<class CType>
BladeList<CType>::BladeList() : DynList<Blade<CType> >()
{
	this->eret = 0;
	base = 0;
	str = "";
}


////////////////////////////////////////////////////////////////////////////////////
// Constructor

template<class CType>
BladeList<CType>::BladeList(Base<CType> &nbase) : DynList<Blade<CType> >()
{
	this->eret = new Blade<CType>(nbase);
	base = &nbase;
	str = "";
}


////////////////////////////////////////////////////////////////////////////////////
// Copy Constructor

template<class CType>
BladeList<CType>::BladeList(const BladeList<CType> &a) : DynList<Blade<CType> >(a)
{
	base = a.base;
	str = "";
}


////////////////////////////////////////////////////////////////////////////////////
// Reset Blade List

template<class CType>
void BladeList<CType>::Reset(Base<CType> &nbase)
{
	if (!this->eret)
		this->eret = new Blade<CType>(nbase);
	
	base = &nbase;
	str = "";
	this->Empty();
}


////////////////////////////////////////////////////////////////////////////////////
// Destructor

template<class CType>
BladeList<CType>::~BladeList()
{
}




////////////////////////////////////////////////////////////////////////////////////
// Comparison operator
// Gives: absolut of result is position in DynList + 1
//        < 0 : if odd permutation
//        > 0 : if even permutation
//       == 0 : if not found

template<class CType>
int operator==(const BladeList<CType> &a, const Blade<CType> &b)
{
	int r = 0;
	uint cnt = a.list.Count();
	Blade<CType> **data = a.list.Data();
	
	for(uint i=0;i<cnt;i++)
	{ 
		if ((r = (data[i][0] == b)))
		{ 
			r *= i+1;
			break;
		}
	}
	
	return r;
}



////////////////////////////////////////////////////////////////////////////////////
// Extract Multivectors of certain grade

template<class CType>
BladeList<CType> BladeList<CType>::operator()(uint grade) const
{
	uint cnt = this->list.Count();
	Blade<CType> **data = this->list.Data();
	BladeList<CType> bl(base[0]);
	
	for(uint i=0;i<cnt;i++)
	{ 
		if (data[i]->Grade() == grade)
		{ 
			bl.Add(data[i][0]);
		}
	}
	
	return bl;
}


////////////////////////////////////////////////////////////////////////////////////
// Extract Scalar Part

template<class CType>
CType BladeList<CType>::operator()() const
{
	uint cnt = this->list.Count();
	Blade<CType> **data = this->list.Data();
	
	for(uint i=0;i<cnt;i++)
	{ 
		if (data[i]->Grade() == 0)
			return data[i]->Factor();
	}
	
	return CType(0);
}


////////////////////////////////////////////////////////////////////////////////////
// Extract Scalar Part

template<class CType>
CType Scalar(const BladeList<CType> &bl)
{
	uint cnt = bl.list.Count();
	Blade<CType> **data = bl.list.Data();
	
	for(uint i=0;i<cnt;i++)
	{ 
		if (data[i]->Grade() == 0)
			return data[i]->Factor();
	}
	
	return CType(0);
}



////////////////////////////////////////////////////////////////////////////////////
// Assignment of BladeList

template<class CType>
BladeList<CType>& BladeList<CType>::operator=(const BladeList<CType> &a)
{
	base = a.base;
	Set(a);
	check();
	return *this;
}


////////////////////////////////////////////////////////////////////////////////////
// Assignment of Blade  

template<class CType>
BladeList<CType>& BladeList<CType>::operator=(const Blade<CType> &a)
{
	Blade<CType> b(base[0]);
	b = a;
	
	this->Empty();
	Add(b);
	check();
	return *this;
}


////////////////////////////////////////////////////////////////////////////////////
// Assignment of MultiV

template<class CType>
BladeList<CType>& BladeList<CType>::operator=(const MultiV<CType> &a)
{
	operator=(MakeBladeList(a));
	
	return *this;
}


////////////////////////////////////////////////////////////////////////////////////
// String output of BladeList

template<class CType>
char* BladeList<CType>::Str()
{
	uint cnt = this->list.Count();
	Blade<CType> **data = this->list.Data();
	
	str = "[ ";
	
	for(uint i=0;i<cnt;i++)
	{ 
		str << data[i]->Str();
		if (i+1 < cnt) str << " + ";
	}
	
	str << " ]";
	
	return str;
}


////////////////////////////////////////////////////////////////////////////////////
// Checking BladeList

template<class CType>
uint BladeList<CType>::check()
{
	uint r = 0;
	Blade<CType> **data = this->list.Data();
	
	for(uint i=0;i<this->list.Count();i++)
	{ 
		if (data[i]->Factor() == CType(0))
		{ 
			this->Sub(i);
			data = this->list.Data(); // Pointer may have changed !!!
			i--;
			r = 1;
		}
	}
	
	return r;
}



////////////////////////////////////////////////////////////////////////////////////
// Adding of Scalar to BladeList

template<class CType>
BladeList<CType> /*BladeList<CType>::*/operator+(const BladeList<CType> &al,
												 const CType &b)
{
	int r;
	BladeList<CType> bl(al.base[0]);
	Blade<CType> c(al.base[0],b);
	bl = al;
	
	if ((r = (bl == c)))
	{ 
		if (r < 0) r *= -1;
		r--;
		bl.list.Data()[r][0] += c;
	}
	else bl.Add(c);
	
	bl.check();
	return bl;
}


////////////////////////////////////////////////////////////////////////////////////
// Adding of BladeList to Scalar

template<class CType>
BladeList<CType> /*BladeList<CType>::*/operator+(const CType &b, 
												 const BladeList<CType> &al)
{
	int r;
	BladeList<CType> bl(al.base[0]);
	Blade<CType> c(al.base[0],b);
	bl = al;
	
	if ((r = (bl == c)))
	{ 
		if (r < 0) r *= -1;
		r--;
		bl.list.Data()[r][0] += c;
	}
	else bl.Add(c);
	
	bl.check();
	return bl;
}



////////////////////////////////////////////////////////////////////////////////////
// Adding of Scalar to BladeList to this

template<class CType>
BladeList<CType>& /*BladeList<CType>::*/operator+=(BladeList<CType> &al,
												   const CType &b)
{
	int r;
	Blade<CType> c(al.base[0],b);
	
	if ((r = (al == c)))
	{ 
		if (r < 0) r *= -1;
		r--;
		al.list.Data()[r][0] += c;
	}
	else al.Add(c);
	
	al.check();
	return al;
}



////////////////////////////////////////////////////////////////////////////////////
// Adding of Scalar to BladeList to this using <<

template<class CType>
BladeList<CType>& /*BladeList<CType>::*/operator<<(BladeList<CType> &al,
												   const CType &b)
{
	int r;
	Blade<CType> c(al.base[0],b);
	
	if ((r = (al == c)))
	{ 
		if (r < 0) r *= -1;
		r--;
		al.list.Data()[r][0] += c;
	}
	else al.Add(c);
	
	al.check();
	return al;
}



////////////////////////////////////////////////////////////////////////////////////
// Subtraction of Scalar from BladeList

template<class CType>
BladeList<CType> /*BladeList<CType>::*/operator-(const BladeList<CType> &al,
												 const CType &b)
{
	int r;
	BladeList<CType> bl(al.base[0]);
	Blade<CType> c(al.base[0],-b);
	bl = al;
	
	if ((r = (bl == c)))
	{ 
		if (r < 0) r *= -1;
		r--;
		bl.list.Data()[r][0] += c;
	}
	else bl.Add(c);
	
	bl.check();
	return bl;
}



////////////////////////////////////////////////////////////////////////////////////
// Subtraction of BladeList from Scalar

template<class CType>
BladeList<CType> /*BladeList<CType>::*/operator-(const CType &b, 
												 const BladeList<CType> &al)
{
	int r;
	BladeList<CType> bl(al.base[0]);
	Blade<CType> c(al.base[0],b);
	bl = -al;
	
	if ((r = (bl == c)))
	{ 
		if (r < 0) r *= -1;
		r--;
		bl.list.Data()[r][0] += c;
	}
	else bl.Add(c);
	
	bl.check();
	return bl;
}



////////////////////////////////////////////////////////////////////////////////////
// Subtraction of Scalar from BladeList to this

template<class CType>
BladeList<CType>& /*BladeList<CType>::*/operator-=(BladeList<CType> &al,
												   const CType &b)
{
	int r;
	Blade<CType> c(al.base[0],-b);
	
	if ((r = (al == c)))
	{ 
		if (r < 0) r *= -1;
		r--;
		al.list.Data()[r][0] += c;
	}
	else al.Add(c);
	
	al.check();
	return al;
}



////////////////////////////////////////////////////////////////////////////////////
// Adding of Blade to BladeList

template<class CType>
BladeList<CType> /*BladeList<CType>::*/operator+(const BladeList<CType> &al,
												 const Blade<CType> &b)
{
	int r;
	BladeList<CType> bl(al.base[0]);
	bl = al;
	
	if ((r = (bl == b)))
	{ 
		if (r < 0) r *= -1;
		r--;
		bl.list.Data()[r][0] += b;
	}
	else bl.Add(b);
	
	bl.check();
	return bl;
}



////////////////////////////////////////////////////////////////////////////////////
// Adding of Blade to BladeList

template<class CType>
BladeList<CType> /*BladeList<CType>::*/operator+(const Blade<CType> &b,
												 const BladeList<CType> &al)
{
	int r;
	BladeList<CType> bl(al.base[0]);
	bl = al;
	
	if ((r = (bl == b)))
	{ 
		if (r < 0) r *= -1;
		r--;
		bl.list.Data()[r][0] += b;
	}
	else bl.Add(b);
	
	bl.check();
	return bl;
}



////////////////////////////////////////////////////////////////////////////////////
// Adding of Blade to this BladeList

template<class CType>
BladeList<CType>& /*BladeList<CType>::*/operator+=(BladeList<CType> &al,
												   const Blade<CType> &b)
{
	int r;
	
	if ((r = (al == b)))
	{ 
		if (r < 0) r = -r;
		r--;
		al.list.Data()[r][0] += b;
	}
	else al.Add(b);
	
	al.check();
	return al;
}


////////////////////////////////////////////////////////////////////////////////////
// Adding of Blade to this BladeList. Same as +=, however formulas are
// calculated left to right with << and right to left with +=.
// That is, list += blade += blade will first add the blades and the 
// result of this to the list. On the other hand, list << blade << blade
// will first evaluate list << blade and the second << operation will
// be acted on the result of this.

template<class CType>
BladeList<CType>& /*BladeList<CType>::*/operator<<(BladeList<CType> &al,
												   const Blade<CType> &b)
{
	int r;
	
	if ((r = (al == b)))
	{ 
		if (r < 0) r *= -1;
		r--;
		al.list.Data()[r][0] += b;
	}
	else al.Add(b);
	
	
	al.check();
	return al;
}



////////////////////////////////////////////////////////////////////////////////////
// Subtracting of Blade to BladeList

template<class CType>
BladeList<CType> /*BladeList<CType>::*/operator-(const BladeList<CType> &al,
												 const Blade<CType> &b)
{
	int r;
	BladeList<CType> bl(al.base[0]);
	
	bl = al;
	
	if ((r = (bl == b)))
	{ 
		if (r < 0) r *= -1;
		r--;
		bl.list.Data()[r][0] -= b;
	}
	else bl.Add(-b);
	
	bl.check();
	return bl;
}



////////////////////////////////////////////////////////////////////////////////////
// Subtracting of Blade to BladeList

template<class CType>
BladeList<CType> /*BladeList<CType>::*/operator-(const Blade<CType> &b,
												 const BladeList<CType> &al)
{
	int r;
	BladeList<CType> bl(al.base[0]);
	
	bl = -al;
	Blade<CType> **data = bl.list.Data();
	
	if ((r = (bl == b)))
	{ 
		if (r < 0) r *= -1;
		r--;
		data[r][0] += b;
	}
	else bl.Add(b);
	
	bl.check();
	return bl;
}



////////////////////////////////////////////////////////////////////////////////////
// Subtracting of Blade to this BladeList

template<class CType>
BladeList<CType>& /*BladeList<CType>::*/operator-=(BladeList<CType> &al,
												   const Blade<CType> &b)
{
	int r;
	
	if ((r = (al == b)))
	{ 
		if (r < 0) r *= -1;
		r--;
		al.list.Data()[r][0] -= b;
	}
	else al.Add(-b);
	
	al.check();
	return al;
}



////////////////////////////////////////////////////////////////////////////////////
// Outer Product of BladeList with CType

template<class CType>
BladeList<CType> /*BladeList<CType>::*/operator^(const BladeList<CType> &al,
												 const CType &b)
{
	BladeList<CType> bl(al.base[0]);
	uint cnt = al.list.Count();
	Blade<CType> **data = al.list.Data();
	
	for(uint i=0;i<cnt;i++)
		bl += data[i][0] ^ b;
	
	return bl;
}


////////////////////////////////////////////////////////////////////////////////////
// Outer Product of CType with BladeList

template<class CType>
BladeList<CType> /*BladeList<CType>::*/operator^(const CType &b,
												 const BladeList<CType> &al)
{
	BladeList<CType> bl(al.base[0]);
	uint cnt = al.list.Count();
	Blade<CType> **data = al.list.Data();
	
	for(uint i=0;i<cnt;i++)
		bl += data[i][0] ^ b;
	
	return bl;
}



////////////////////////////////////////////////////////////////////////////////////
// Outer Product BladeList with CType to this

template<class CType>
BladeList<CType>& operator^=(BladeList<CType> &al, const CType &b)
{
	uint cnt = al.list.Count();
	Blade<CType> **data = al.list.Data();
	
	for(uint i=0;i<cnt;i++)
		data[i][0] ^= b;
	
	return al;
}



////////////////////////////////////////////////////////////////////////////////////
// Geometric Product of BladeList with Blade

template<class CType>
BladeList<CType> /*BladeList<CType>::*/operator&(const BladeList<CType> &al,
												 const Blade<CType> &b)
{
	BladeList<CType> bl(al.base[0]);
	uint cnt = al.list.Count();
	Blade<CType> **data = al.list.Data();
	
	for(uint i=0;i<cnt;i++)
		bl += data[i][0] & b;
	
	return bl;
}



////////////////////////////////////////////////////////////////////////////////////
// Geometric Product of Blade with BladeList

template<class CType>
BladeList<CType> /*BladeList<CType>::*/operator&(const Blade<CType> &b,
												 const BladeList<CType> &al)
{
	BladeList<CType> bl(al.base[0]);
	uint cnt = al.list.Count();
	Blade<CType> **data = al.list.Data();
	
	for(uint i=0;i<cnt;i++)
		bl += b & data[i][0];
	
	return bl;
}



////////////////////////////////////////////////////////////////////////////////////
// Geometric Product of BladeList with Blade

template<class CType>
BladeList<CType>& /*BladeList<CType>::*/operator&=(BladeList<CType> &al,
												   const Blade<CType> &b)
{
	BladeList<CType> bl(al.base[0]);
	uint cnt = al.list.Count();
	Blade<CType> **data = al.list.Data();
	
	for(uint i=0;i<cnt;i++)
		bl += data[i][0] & b;
	
	al = bl;
	return al;
}



////////////////////////////////////////////////////////////////////////////////////
// Inner Product of BladeList with Blade

template<class CType>
BladeList<CType> /*BladeList<CType>::*/operator*(const BladeList<CType> &al,
												 const Blade<CType> &b)
{
	BladeList<CType> bl(al.base[0]);
	uint cnt = al.list.Count();
	Blade<CType> **data = al.list.Data();
	
	for(uint i=0;i<cnt;i++)
		bl += data[i][0] * b;
	
	return bl;
}



////////////////////////////////////////////////////////////////////////////////////
// Inner Product of Blade with BladeList

template<class CType>
BladeList<CType> /*BladeList<CType>::*/operator*(const Blade<CType> &b,
												 const BladeList<CType> &al)
{
	BladeList<CType> bl(al.base[0]);
	uint cnt = al.list.Count();
	Blade<CType> **data = al.list.Data();
	
	for(uint i=0;i<cnt;i++)
		bl += b * data[i][0];
	
	return bl;
}


////////////////////////////////////////////////////////////////////////////////////
// Inner Product of this BladeList with Blade

template<class CType>
BladeList<CType>& /*BladeList<CType>::*/operator*=(BladeList<CType> &al,
												   const Blade<CType> &b)
{
	BladeList<CType> bl(al.base[0]);
	uint cnt = al.list.Count();
	Blade<CType> **data = al.list.Data();
	
	for(uint i=0;i<cnt;i++)
		bl += data[i][0] * b;
	
	al = bl;
	return al;
}



////////////////////////////////////////////////////////////////////////////////////
// Outer Product of BladeList with Blade

template<class CType>
BladeList<CType> /*BladeList<CType>::*/operator^(const BladeList<CType> &al,
												 const Blade<CType> &b)
{
	BladeList<CType> bl(al.base[0]);
	uint cnt = al.list.Count();
	Blade<CType> **data = al.list.Data();
	
	for(uint i=0;i<cnt;i++)
		bl += data[i][0] ^ b;
	
	return bl;
}



////////////////////////////////////////////////////////////////////////////////////
// Outer Product of Blade with BladeList

template<class CType>
BladeList<CType> /*BladeList<CType>::*/operator^(const Blade<CType> &b,
												 const BladeList<CType> &al)
{
	BladeList<CType> bl(al.base[0]);
	uint cnt = al.list.Count();
	Blade<CType> **data = al.list.Data();
	
	for(uint i=0;i<cnt;i++)
		bl += b ^ data[i][0];
	
	return bl;
}



////////////////////////////////////////////////////////////////////////////////////
// Outer Product of this BladeList with Blade

template<class CType>
BladeList<CType>& /*BladeList<CType>::*/operator^=(BladeList<CType> &al,
												   const Blade<CType> &b)
{
	BladeList<CType> bl(al.base[0]);
	uint cnt = al.list.Count();
	Blade<CType> **data = al.list.Data();
	
	for(uint i=0;i<cnt;i++)
		bl += data[i][0] ^ b;
	
	al = bl;
	return al;
}



////////////////////////////////////////////////////////////////////////////////////
// Geometric Product of BladeList with BladeList

template<class CType>
BladeList<CType> /*BladeList<CType>::*/operator&(const BladeList<CType> &al,
												 const BladeList<CType> &bl)
{
	uint i,j;
	uint acnt = al.list.Count();
	Blade<CType> **adata = al.list.Data();
	uint bcnt = bl.list.Count();
	Blade<CType> **bdata = bl.list.Data();
	BladeList<CType> cl(al.base[0]);
	
	//  Blade<CType> hb(al.base[0]);
	
	for(i=0;i<acnt;i++)
		for(j=0;j<bcnt;j++)
			cl += adata[i][0] & bdata[j][0];
			/*
			{ hb = adata[i][0] & bdata[j][0];
			cl += hb;
			printf("%s & %s -> %s\n", adata[i][0].Str(), bdata[j][0].Str(), 
			hb.Str());
			printf(">>> SUM: %s\n\n", cl.Str());
			}
		*/
	return cl;
}


////////////////////////////////////////////////////////////////////////////////////
// Geometric Product of BladeList with BladeList

template<class CType>
BladeList<CType>& /*BladeList<CType>::*/operator&=(BladeList<CType> &al,
												   const BladeList<CType> &bl)
{
	uint i,j;
	uint acnt = al.list.Count();
	Blade<CType> **adata = al.list.Data();
	uint bcnt = bl.list.Count();
	Blade<CType> **bdata = bl.list.Data();
	BladeList<CType> cl(al.base[0]);
	
	for(i=0;i<acnt;i++)
		for(j=0;j<bcnt;j++)
			cl += adata[i][0] & bdata[j][0];
		
	al = cl;
	return al;
}


////////////////////////////////////////////////////////////////////////////////////
// Inner Product of BladeList with BladeList

template<class CType>
BladeList<CType> /*BladeList<CType>::*/operator*(const BladeList<CType> &al,
												 const BladeList<CType> &bl)
{
	uint i,j;
	uint acnt = al.list.Count();
	Blade<CType> **adata = al.list.Data();
	uint bcnt = bl.list.Count();
	Blade<CType> **bdata = bl.list.Data();
	BladeList<CType> cl(al.base[0]);
	
	for(i=0;i<acnt;i++)
		for(j=0;j<bcnt;j++)
			cl += adata[i][0] * bdata[j][0];
		
		return cl;
}



////////////////////////////////////////////////////////////////////////////////////
// Inner Product of BladeList with BladeList

template<class CType>
BladeList<CType>& /*BladeList<CType>::*/operator*=(BladeList<CType> &al,
												   const BladeList<CType> &bl)
{
	uint i,j;
	uint acnt = al.list.Count();
	Blade<CType> **adata = al.list.Data();
	uint bcnt = bl.list.Count();
	Blade<CType> **bdata = bl.list.Data();
	BladeList<CType> cl(al.base[0]);
	
	for(i=0;i<acnt;i++)
		for(j=0;j<bcnt;j++)
			cl += adata[i][0] * bdata[j][0];
		
		al = cl;
		return al;
}



////////////////////////////////////////////////////////////////////////////////////
// Outer Product of BladeList with BladeList

template<class CType>
BladeList<CType> /*BladeList<CType>::*/operator^(const BladeList<CType> &al,
												 const BladeList<CType> &bl)
{
	uint i,j;
	uint acnt = al.list.Count();
	Blade<CType> **adata = al.list.Data();
	uint bcnt = bl.list.Count();
	Blade<CType> **bdata = bl.list.Data();
	BladeList<CType> cl(al.base[0]);
	
	for(i=0;i<acnt;i++)
		for(j=0;j<bcnt;j++)
			cl += adata[i][0] ^ bdata[j][0];
		
		return cl;
}



////////////////////////////////////////////////////////////////////////////////////
// Outer Product of BladeList with BladeList

template<class CType>
BladeList<CType>& /*BladeList<CType>::*/operator^=(BladeList<CType> &al,
												   const BladeList<CType> &bl)
{
	uint i,j;
	uint acnt = al.list.Count();
	Blade<CType> **adata = al.list.Data();
	uint bcnt = bl.list.Count();
	Blade<CType> **bdata = bl.list.Data();
	BladeList<CType> cl(al.base[0]);
	
	for(i=0;i<acnt;i++)
		for(j=0;j<bcnt;j++)
			cl += adata[i][0] ^ bdata[j][0];
		
		al = cl;
		return al;
}


////////////////////////////////////////////////////////////////////////////////////
// Reversion of Blades

template<class CType>
BladeList<CType> /*BladeList<CType>::*/operator~(const BladeList<CType> &al)
{
	uint cnt = al.list.Count();
	Blade<CType> **data = al.list.Data();
	BladeList<CType> bl(al.base[0]);
	
	for(uint i=0;i<cnt;i++)
		bl += ~data[i][0];
	
	return bl;
}


////////////////////////////////////////////////////////////////////////////////////
// Inversion of Blades

template<class CType>
BladeList<CType> /*BladeList<CType>::*/operator!(const BladeList<CType> &al)
{
	uint cnt = al.list.Count();
	Blade<CType> **data = al.list.Data();
	BladeList<CType> bl(al.base[0]);
	
	for(uint i=0;i<cnt;i++)
		bl += !data[i][0];
	
	return bl;
}



////////////////////////////////////////////////////////////////////////////////////
// Negation of Factors of Blades

template<class CType>
BladeList<CType> /*BladeList<CType>::*/operator-(const BladeList<CType> &al)
{
	uint cnt = al.list.Count();
	Blade<CType> **data = al.list.Data();
	BladeList<CType> bl(al.base[0]);
	
	for(uint i=0;i<cnt;i++)
		bl += -data[i][0];
	
	return bl;
}


////////////////////////////////////////////////////////////////////////////////////
// Magnitude of Blade List SQUARED(!)

template<class CType>
CType BladeList<CType>::mag2() const
{
	CType r = CType(0);
	uint cnt = this->list.Count();
	Blade<CType> **data = this->list.Data();
	Blade<CType> a(base[0]);
	
	for(uint i=0;i<cnt;i++)
	{ 
		a = data[i][0] * (~data[i][0]);
		r += a.Factor();
	}
	
	return r;
}



////////////////////////////////////////////////////////////////////////////////////
// Adding of BladeList to BladeList

template<class CType>
BladeList<CType> /*BladeList<CType>::*/operator+(const BladeList<CType> &al,
												 const BladeList<CType> &bl)
{
	uint cnt = bl.list.Count();
	Blade<CType> **data = bl.list.Data();
	BladeList<CType> cl(al.base[0]);
	cl = al;
	
	for(uint i=0;i<cnt;i++)
		cl += data[i][0];
	
	return cl;
}


////////////////////////////////////////////////////////////////////////////////////
// Adding of BladeList to this BladeList

template<class CType>
BladeList<CType>& /*BladeList<CType>::*/operator+=(BladeList<CType> &al,
												   const BladeList<CType> &bl)
{
	uint cnt = bl.list.Count();
	Blade<CType> **data = bl.list.Data();
	
	for(uint i=0;i<cnt;i++)
		al += data[i][0];
	
	return al;
}


////////////////////////////////////////////////////////////////////////////////////
// Adding of BladeList to this BladeList using <<

template<class CType>
BladeList<CType>& /*BladeList<CType>::*/operator<<(BladeList<CType> &al,
												   const BladeList<CType> &bl)
{
	uint cnt = bl.list.Count();
	Blade<CType> **data = bl.list.Data();
	
	for(uint i=0;i<cnt;i++)
		al += data[i][0];
	
	return al;
}



////////////////////////////////////////////////////////////////////////////////////
// Subtraction of BladeList from BladeList

template<class CType>
BladeList<CType> /*BladeList<CType>::*/operator-(const BladeList<CType> &al,
												 const BladeList<CType> &bl)
{
	uint cnt = bl.list.Count();
	Blade<CType> **data = bl.list.Data();
	BladeList<CType> cl(al.base[0]);
	cl = al;
	
	for(uint i=0;i<cnt;i++)
		cl -= data[i][0];
	
	return cl;
}


////////////////////////////////////////////////////////////////////////////////////
// Subtraction of BladeList from this BladeList

template<class CType>
BladeList<CType>& /*BladeList<CType>::*/operator-=(BladeList<CType> &al,
												   const BladeList<CType> &bl)
{
	uint cnt = bl.list.Count();
	Blade<CType> **data = bl.list.Data();
	
	for(uint i=0;i<cnt;i++)
		al -= data[i][0];
	
	return al;
}


////////////////////////////////////////////////////////////////////////////////////
// Commutator Product BladeList with Blade

template<class CType>
BladeList<CType> /*BladeList<CType>::*/cp(const BladeList<CType> &al,
										  const Blade<CType> &b)
{
	return ((al & b) - (b & al));
}


////////////////////////////////////////////////////////////////////////////////////
// Anti-Commutator Product BladeList with Blade

template<class CType>
BladeList<CType> /*BladeList<CType>::*/acp(const BladeList<CType> &al,
										   const Blade<CType> &b)
{
	return ((al & b) + (b & al));
}


////////////////////////////////////////////////////////////////////////////////////
// Commutator Product

template<class CType>
BladeList<CType> /*BladeList<CType>::*/cp(const BladeList<CType> &al,
										  const BladeList<CType> &bl)
{
	return ((al & bl) - (bl & al));
}


////////////////////////////////////////////////////////////////////////////////////
// Anti-Commutator Product

template<class CType>
BladeList<CType> /*BladeList<CType>::*/acp(const BladeList<CType> &al,
										   const BladeList<CType> &bl)
{
	return ((al & bl) + (bl & al));
}


/* The following functions are not implemented correctly

////////////////////////////////////////////////////////////////////////////////////
// JOIN of BladeList with Blade

template<class CType>
BladeList<CType> join(const BladeList<CType> &al, const Blade<CType> &b)
{
	uint cnt = al.list.Count();
	Blade<CType> **data = al.list.Data();
	BladeList<CType> bl(al.base[0]);
	
	for(uint i=0;i<cnt;i++)
		bl += join(data[i][0],b);
	
	return bl;
}


////////////////////////////////////////////////////////////////////////////////////
// JOIN of BladeList with BladeList

template<class CType>
BladeList<CType> join(const BladeList<CType> &al, const BladeList<CType> &bl)
{
	uint acnt = al.list.Count();
	Blade<CType> **adata = al.list.Data();
	uint bcnt = bl.list.Count();
	Blade<CType> **bdata = bl.list.Data();
	BladeList<CType> cl(al.base[0]);
	
	for(uint i=0;i<acnt;i++)
		for(uint j=0;j<bcnt;j++)
			cl += join(adata[i][0],bdata[j][0]);
		
		return cl;
}



////////////////////////////////////////////////////////////////////////////////////
// MEET of BladeList with Blade

template<class CType>
BladeList<CType> meet(const BladeList<CType> &al, const Blade<CType> &b)
{
	uint cnt = al.list.Count();
	Blade<CType> **data = al.list.Data();
	BladeList<CType> bl(al.base[0]);
	
	for(uint i=0;i<cnt;i++)
		bl += meet(data[i][0],b);
	
	return bl;
}


////////////////////////////////////////////////////////////////////////////////////
// MEET of BladeList with BladeList

template<class CType>
BladeList<CType> meet(const BladeList<CType> &al, const BladeList<CType> &bl)
{
	uint acnt = al.list.Count();
	Blade<CType> **adata = al.list.Data();
	uint bcnt = bl.list.Count();
	Blade<CType> **bdata = bl.list.Data();
	BladeList<CType> cl(al.base[0]);
	
	for(uint i=0;i<acnt;i++)
		for(uint j=0;j<bcnt;j++)
			cl += meet(adata[i][0],bdata[j][0]);
		
		return cl;
}

*/

