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


///////////////////////////////////////////////////////////////////
/// RingBuf Class Definitions
///////////////////////////////////////////////////////////////////

#include "ringbuf.h"
#include "ringbinst.h"

///////////////////////////////////////////////////////////////////
/// Standard Constructor

template<class CType>
RingBuf<CType>::RingBuf()
{
	data = 1;
	full = 0;
	no = 0;
	fpos = 0;
	lpos = 0;
	type = 0;
}

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
/// Constructor

template<class CType>
RingBuf<CType>::RingBuf(uint nno, uint ntype)
{
	if (!(data = nno))
	{ full = 0;
		no = 0;
		fpos = 0;
		lpos = 0;
		type = 0;
		return;
	}

	no = nno;
	type = ntype;
	Empty();
}

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
/// COPY Constructor

template<class CType>
RingBuf<CType>::RingBuf(const RingBuf<CType>& R)
{
	this[0] = R;
}


///////////////////////////////////////////////////////////////////
/// Reset Ring

template<class CType>
uint RingBuf<CType>::Reset(uint nno, uint ntype)
{
	if (!Resize(nno)) return 0;

	type = ntype;

	Empty();
	return 1;
}

///////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////
/// Resize Ring

template<class CType>
uint RingBuf<CType>::Resize(uint nno)
{
	if (!(data = nno)) return 0;

	no = nno;

	return 1;
}
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
/// EQUAL OPERATOR

template<class CType>
RingBuf<CType>& RingBuf<CType>::operator= (const RingBuf<CType>& R)
{
	data = R.data;
	full = R.full;
	no = R.no;
	fpos = R.fpos;
	lpos = R.lpos;
	type = R.type;

	return this[0];
}

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
/// Empty Ring Buffer

template<class CType>
void RingBuf<CType>::Empty()
{
	uint i;
	CType *d = data.Data();

	full = 0;
	fpos = 0;
	lpos = 0;

	for(i=0;i<no;i++)
		d[i] = CType(0);
}

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
/// Empty Ring Buffer OPERATOR

template<class CType>
RingBuf<CType>& RingBuf<CType>::operator! ()
{
	Empty();

	return *this;
}

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
/// Add element to Ring

template<class CType>
RingBuf<CType>& RingBuf<CType>::operator<< (const CType& d)
{
	add(d);

	return *this;
}
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
/// Take element from Ring

template<class CType>
RingBuf<CType>& RingBuf<CType>::operator>> (CType& d)
{
	d = take();

	return *this;
}

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
/// Give reference to some element in ring

template<class CType>
CType RingBuf<CType>::operator[] (uint pos)
{
	return data[(pos+fpos)%no];
}

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
/// Add element

template<class CType>
uint RingBuf<CType>::add(const CType& d)
{
	if (!no || (full && type == RING_NOOVERWRITE)) return 0;

	data[lpos] = d;

	lpos = (++lpos) % no;

	if (full) fpos = (++fpos) % no;
	else if (lpos == fpos) full = 1;

	return 1;
}
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
/// Take element

template<class CType>
CType RingBuf<CType>::take()
{
	CType r = data[fpos];

	if (!full && fpos == lpos) return r;

	if (full) full = 0;

	data[fpos] = CType(0);
	fpos = (++fpos) % no;

	return r;
}
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
/// String Output

template<class CType>
cStr RingBuf<CType>::Str(uint how)
{
	uint i,j;

	if (!no) return (str = "[]");

	if (how == RING_ABSOLUTE_STR)
	{ str = "[";
		for(i=0;i<no;i++)
		{ if (fpos == i) 
			{ if (full) str << "<>";
			  else if (fpos == lpos) str << "><";
				else str << ">";
			}
			else if (lpos == i) str << "<";
			str << MakeStr(data[i]) << ", ";
		}
		(str |= -2) << "]";
	}
	else if (how == RING_RELATIVE_STR)
	{ if (!full && fpos == lpos) return (str = "[]");

		str = "[";
		for(i=0,j=fpos;i<no;i++)
		{ str << MakeStr(data[j]) << ", ";
			j = (++j) % no;
			if (j == lpos) break;
		}
		(str |= -2) << "]";
	}

	return str;
}
