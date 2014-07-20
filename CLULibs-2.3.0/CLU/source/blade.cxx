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



//------------------- Blade Class Library -----------------------

#include "blade.h"

#include "makestr.h"
#include "mathelp.h"

////////////////////////////////////////////////////////////////////////////////////
// Default Constructor

template<class CType>
Blade<CType>::Blade()
{
	base = 0;
	grade = 0;
	fact = 0;
	str = "";
	comps = 0;
	name = "";
}


////////////////////////////////////////////////////////////////////////////////////
// Constructor

template<class CType>
Blade<CType>::Blade(Base<CType> &nbase, CType nfact, 
					uint ngrade, uint ncomps[], char* nname)
{
	base = &nbase;
	uint dim = base->VSDim();
	
	if (dim < ngrade) ngrade = dim;
	
	grade = ngrade;
	fact = nfact;
	str = "";
	
	if ((comps = dim) && ncomps)
	{
		memcpy(comps.Data(), ncomps, grade*sizeof(uint));
	}

	if (nname) name = nname;
	else name = "";
		
	check();
}


////////////////////////////////////////////////////////////////////////////////////
// Copy Constructor

template<class CType>
Blade<CType>::Blade(const Blade<CType> &a)
{
	base = a.base;
	uint dim = base->VSDim();
	
	grade = a.grade;
	if (dim < grade) grade = dim;
	
	fact = a.fact;
	str = "";
	comps = a.comps;
	name = a.name;
	
	check();
}


////////////////////////////////////////////////////////////////////////////////////
// Destructor

template<class CType>
Blade<CType>::~Blade()
{
}


////////////////////////////////////////////////////////////////////////////////////
// Reset Blade

template<class CType>
void Blade<CType>::Reset(Base<CType> &nbase, CType nfact, 
						 uint ngrade, uint ncomps[], char* nname)
{
	base = &nbase;
	uint dim = base->VSDim();
	
	if (dim < ngrade) ngrade = dim;
	
	grade = ngrade;
	fact = nfact;
	str = "";
	
	if ((comps = dim) && ncomps)
	{
		memcpy(comps.Data(), ncomps, grade*sizeof(uint));
	}
		
	if (nname) name = nname;
	else name = "";
		
	check();
}



////////////////////////////////////////////////////////////////////////////////////
// Take Out element

template<class CType>
void Blade<CType>::takeOut(uint pos)
{
	if (pos >= 0 && pos < grade)
    { grade--;
	for(uint i=pos;i<grade;i++)
	{ comps[i] = comps[i+1];
	}
    }
}


////////////////////////////////////////////////////////////////////////////////////
// Put In element

template<class CType>
void Blade<CType>::putIn(uint e, uint pos)
{ 
	uint dim = base->VSDim();
	
	if (pos >= 0 && pos < dim && grade < dim)
    { if (pos >= grade) pos = grade;
	for(uint i=grade;i>pos;i--)
	{ comps[i] = comps[i-1];
	}
	comps[pos] = e;
	grade++;
    }
	
	check();
}

////////////////////////////////////////////////////////////////////////////////////
// Make Blade to Pseudo Scalar

template<class CType>
void Blade<CType>::makePS()
{
	uint dim = base->VSDim();
	uint j = base->FirstIdx();
	
	for(uint i=0;i<dim;i++,j++)
		comps[i] = j;
	
	grade = dim;
	fact = CType(1);
}


////////////////////////////////////////////////////////////////////////////////////
// Assignment of Scalar

template<class CType>
Blade<CType>& Blade<CType>::operator=(const CType f)
{
	fact = f;
	grade = 0;
	check();

	return *this;
}

////////////////////////////////////////////////////////////////////////////////////
// Assignment of a Blade part; first element in ncomps[] is grade

template<class CType>
Blade<CType>& Blade<CType>::operator=(const uint ncomps[])
{
	if (ncomps[0] > base->VSDim()) return *this;
	
	for(uint i=0;i<ncomps[0];i++)
	{ comps[i] = ncomps[i+1];
	}
	
	check();
	return *this;
}


////////////////////////////////////////////////////////////////////////////////////
// Assignment of Blade

template<class CType>
Blade<CType>& Blade<CType>::operator=(const Blade<CType> &a)
{
	base = a.base;
	comps = a.comps;
	
	grade = a.grade;
	fact = a.fact;
	
	name = a.name;
	
	check();
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////
// Comparing Blades but ignoring prefactor
// Gives: 0 if blades are not equal
//        1 if blades are even permutation of each other
//       -1 if blades are odd permutation of each other  

template<class CType>
int /*Blade<CType>::*/operator==(const Blade<CType> &c,const Blade<CType> &d)
{
	int f = 1;
	int j;
	Blade<CType> a(c.base[0]), b(d.base[0]);
	a = c; b = d;
	
	if (a.grade != b.grade) return 0;
	
	j = 0;
	do
	{ if (a.comps[0] == b.comps[j])
    { f *= 1 - 2*(j%2);
	a.takeOut(0);
	b.takeOut(j);
	j = -1;
    }
    j++;
	} while (a.grade && b.grade && uint(j) < b.grade);
	
	if (a.grade || b.grade) return 0;
	
	return f;
}

////////////////////////////////////////////////////////////////////////////////////
// Check Blade 
// Gives: 1 if blade was changed
//        0 if blade was not changed

template<class CType>
int Blade<CType>::check()
{
	
	if (fact == CType(0) && grade != 0)
    { grade = 0;
	return 1;
    }
	
	uint i,j;
	uint f = base->FirstIdx();
	uint l = f + base->VSDim() - 1;
	
	for(i=0;i<grade;i++)
		if (comps[i] < f || comps[i] > l)
		{ grade = 0;
		fact = 0;
		return 1;
		}
		
		for(i=0;i<grade;i++)
			for(j=i+1;j<grade;j++)
				if (comps[i] == comps[j])
				{ grade = 0;
				fact = 0;
				return 1;
				}
				
				return 0;
}


////////////////////////////////////////////////////////////////////////////////////
// Addition of two Blades +. Gives own Blade if Blades cannot be added

template<class CType>
Blade<CType> /*Blade<CType>::*/operator+(const Blade<CType> &a,
										 const Blade<CType> &b)
{
	int r;
	Blade<CType> c(a.base[0]);
	c = a;
	
	if ((r = (a == b)))
		c.fact += CType(r) * b.fact;
	
	c.check();
	return c;
}

////////////////////////////////////////////////////////////////////////////////////
// Addition of two Blades +=. Gives own Blade if Blades cannot be added

template<class CType>
Blade<CType>& /*Blade<CType>::*/operator+=(Blade<CType> &a,
										   const Blade<CType> &b)
{
	Blade<CType> c(b);
	int r;
	
	if ((r = (a == b)))
		a.fact += CType(r) * b.fact;
    
	a.check();
	
	return a;
}


////////////////////////////////////////////////////////////////////////////////////
// Subtraction of two Blades -. Gives own Blade if Blades cannot be added

template<class CType>
Blade<CType> /*Blade<CType>::*/operator-(const Blade<CType> &a,
										 const Blade<CType> &b)
{
	int r;
	Blade<CType> c(a.base[0]);
	c = a;
	
	if ((r = (a == b)))
		c.fact -= CType(r) * b.fact;
    
	c.check();
	return c;
}


////////////////////////////////////////////////////////////////////////////////////
// Subtraction of two Blades -=. Gives own Blade if Blades cannot be added

template<class CType>
Blade<CType>& /*Blade<CType>::*/operator-=(Blade<CType> &a,
										   const Blade<CType> &b)
{
	int r;
	
	if ((r = (a == b)))
		a.fact -= CType(r) * b.fact;
    
	a.check();
	return a;
}


////////////////////////////////////////////////////////////////////////////////////
// Multiplication with Prefactor

template<class CType>
Blade<CType> /*Blade<CType>::*/operator^(const Blade<CType> &a,
										 const CType &b)
{
	Blade<CType> c(a.base[0]);
	c = a;
	
	c.fact *= b;
	
	return c;
}


////////////////////////////////////////////////////////////////////////////////////
// Multiplication of Prefactor with Blade

template<class CType>
Blade<CType> /*Blade<CType>::*/operator^(const CType &b,
										 const Blade<CType> &a)
{
	Blade<CType> c(a.base[0]);
	c = a;
	
	c.fact *= b;
	
	return c;
}


////////////////////////////////////////////////////////////////////////////////////
// Multiplication of this Blade with Prefactor

template<class CType>
Blade<CType>& /*Blade<CType>::*/operator^=(Blade<CType> &a,
										   const CType &b)
{
	a.fact *= b;
	
	return a;
}



////////////////////////////////////////////////////////////////////////////////////
// Negation of Blade

template<class CType>
Blade<CType> /*Blade<CType>::*/operator-(const Blade<CType> &a)
{
	Blade<CType> b(a.base[0]);
	b = a;
	
	b.fact *= CType(-1);
	
	return b;
}


////////////////////////////////////////////////////////////////////////////////////
// Reverse of Blade

template<class CType>
Blade<CType> /*Blade<CType>::*/operator~(const Blade<CType> &a)
{
	Blade<CType> b(a.base[0]);
	b = a;
	
	for(uint i=0;i<b.grade;i++)
		b.comps[i] = a.comps[b.grade-i-1];
	
	
	b.name = "";
	
	return b;
}


////////////////////////////////////////////////////////////////////////////////////
// Inverse of Blade

template<class CType>
Blade<CType> /*Blade<CType>::*/operator!(const Blade<CType> &a)
{
	CType f = CType(1);
	Blade<CType> b(a.base[0]);
	b = a;
	
	for(uint i=0;i<b.grade;i++)
	{ 
		b.comps[i] = a.comps[b.grade-i-1];
		f *= a.base->signature(b.comps[i]);
	}
	
	b.fact = f / a.fact;
	
	b.name = "";
	
	return b;
}


////////////////////////////////////////////////////////////////////////////////////
// Geometric Product  

template<class CType>
Blade<CType> /*Blade<CType>::*/operator&(const Blade<CType> &c,
										 const Blade<CType> &d)
{
	//  printf("\n\nGeometric Prod. of two Blades -----------------\n\n");
	
	int dim = c.base->VSDim();
	int i,j,r;

	Blade<CType> e(c.base[0]);
	Blade<CType> a(c.base[0]), b(d.base[0]);

	a = c; b = d;
	
	e.fact = a.fact * b.fact;
	
	//  printf("FIRST>> a: %s, b: %s, e: %s\n\n", a.Str(), b.Str(), e.Str());
	
	for(i=0;i<int(a.grade);i++)
    { 
		for(j=0;j<int(b.grade);j++)
		{ 
			if (a.comps[i] == b.comps[j])
			{ 
				r = 1 - 2*((a.grade-i-1 + j) % 2);
				e.fact *= CType(r) * (a.base->signature(a.comps[i]));
				a.takeOut(i);
				b.takeOut(j);
				/*  
				printf("a: %s, b: %s, e: %s, r: %d\n", 
				a.Str(BS_EXPLICIT), b.Str(BS_EXPLICIT), 
				e.Str(BS_EXPLICIT), r);
				*/ 
				i = -1;
				break;
			}
		}
    }
	
	for(i=0;i<int(a.grade);i++)
		e.comps[i] = a.comps[i];
	
	for(i=0,j=a.grade;i<int(b.grade) && j<dim;i++,j++)
		e.comps[j] = b.comps[i];
	
	e.grade = a.grade + b.grade;
	
	//  printf("\nLAST>> a: %s, b: %s, e: %s\n", a.Str(), b.Str(), e.Str());
	
	return e;
}


////////////////////////////////////////////////////////////////////////////////////
// Inner Product

template<class CType>
Blade<CType> /*Blade<CType>::*/operator*(const Blade<CType> &c,
										 const Blade<CType> &d)
{
	int p = 1;
	int dim = c.base->VSDim();
	int i,j,r;
	Blade<CType> e(c.base[0]);
	Blade<CType> a(c.base[0]), b(d.base[0]);
	a = c; b = d;
	
	if (!a.grade && !b.grade)
	{ 
		e.fact = CType(0); 
		return e;
	}
	else if (!a.grade || !b.grade) return e;
	
	for(i=0;i<int(a.grade);i++)
    { 
		for(j=0;j<int(b.grade);j++)
		{ 
			if (a.comps[i] == b.comps[j])
			{
				r = (1 - 2*((a.grade-i-1 + j) % 2));
				p *= r * int(a.base->signature(a.comps[i]));
				a.takeOut(i);
				b.takeOut(j);
				i = -1;
				break;
			}
		}
    }
	
	if (!a.grade || !b.grade)
	{ 
		e.fact = a.fact * b.fact * p;
	
		if (a.grade)
		{ 
			for(i=0;i<int(a.grade);i++)
				e.comps[i] = a.comps[i];
			e.grade = a.grade;
		}
		else
		{ 
			for(i=0;i<int(b.grade);i++)
				e.comps[i] = b.comps[i];
			e.grade = b.grade;
		}
	}
	
	return e;
}


////////////////////////////////////////////////////////////////////////////////////
// Outer Product

template<class CType>
Blade<CType> /*Blade<CType>::*/operator^(const Blade<CType> &a,
										 const Blade<CType> &b)
{
	int dim = a.base->VSDim();
	int i,j;
	Blade<CType> c(a.base[0]);
	
	c.fact = a.fact * b.fact;
	
	for(i=0;i<int(a.grade);i++)
    { 
		for(j=0;j<int(b.grade);j++)
		{ 
			if (a.comps[i] == b.comps[j])
			{ 
				c.fact = 0;
				i = a.grade;
				break;
			}
		}
    }
	
	if (c.fact)
	{ 
		for(i=0;i<int(a.grade);i++)
			c.comps[i] = a.comps[i];
	
		for(i=0,j=a.grade;i<int(b.grade) && j<dim;i++,j++)
			c.comps[j] = b.comps[i];
	
		c.grade = a.grade + b.grade;
	}
	
	return c;
}



////////////////////////////////////////////////////////////////////////////////////
// The JOIN of two Blades

template<class CType>
Blade<CType> /*Blade<CType>::*/join(const Blade<CType> &a, 
									const Blade<CType> &b)
{
	uint i,j;
	uint dim = a.base->VSDim();
	Blade<CType> c(a.base[0]),d(a.base[0]);
	c = b;
	
	for(i=0;i<a.grade && i<dim;i++)
	{ 
		for(j=0;j<c.grade;j++)
		{ 
			if (a.comps[i] == c.comps[j])
			{ 
				c.takeOut(j);
				break;
			}
		}
		d.comps[i] = a.comps[i];
	}
	
	for(j=0;j<c.grade && i<dim;j++,i++)
		d.comps[i] = c.comps[j];
	
	d.grade = i;
	if (i) d.fact = CType(1); // "a.fact * b.fact;" is WRONG for
	// projective geometry calculations, when the join is needed
	// to create the dual. However, the above, gives the right results.
	
	d.check();
	return d;
}



////////////////////////////////////////////////////////////////////////////////////
// The MEET of two Blades

template<class CType>
Blade<CType> /*Blade<CType>::*/meet(const Blade<CType> &a, 
									const Blade<CType> &b)
{
	Blade<CType> c(a.base[0]),d(a.base[0]),e(a.base[0]);
	Blade<CType> I(a.base[0]),Ii(a.base[0]);
	
	I = join(a,b); // Join
	Ii = !I; // Inverse of Join
	
	c = ((a & Ii) ^ (b & Ii)) & I;  // `deMorgan' Rule
	
	if (c.grade == 0) c.fact = CType(0);
	
	return c;
}



////////////////////////////////////////////////////////////////////////////////////
// Commutator Product

template<class CType>
Blade<CType> cp(const Blade<CType> &a,const Blade<CType> &b)
{ 
	return ((a & b) - (b & a)); 
}


////////////////////////////////////////////////////////////////////////////////////
// Anti-Commutator Product

template<class CType>
Blade<CType> acp(const Blade<CType> &a,const Blade<CType> &b)
{ 
	return ((a & b) + (b & a)); 
}


////////////////////////////////////////////////////////////////////////////////////
// String output of blade

template<class CType>
char* Blade<CType>::Str(uint strtype)
{
	int len;
	cStr vname = base->VName();
	
	str = MakeStr(CType(fact));
	
	if (!name.Len() || strtype == BS_EXPLICIT)
	{ 
		str << "(";
	
		for(uint i=0;i<grade;i++)
			str << vname << comps[i] << "^";
	
		len = str.Len();
		if (str[len-1] == '^') str[len-1] = ')';
		else str << ")";
	}
	else str << "^" << name;
	
	return str;
}


