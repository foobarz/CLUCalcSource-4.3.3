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

// MemObj Class ---------------------------------------------
//
// This class can be used for all objects. It uses new/delete and copies with operator=
// If this is not necessary use Mem class, to increase speed.

#ifndef _MEMOBJ_HH_
#define _MEMOBJ_HH_

#include "mem.h"

typedef unsigned int uint;

template<class CType>
class MemObj
{
public:
	typedef CType *PCType;

	MemObj(uint nno=0);
	MemObj(const MemObj<CType> &a);
	virtual ~MemObj();  
	
	bool Add(uint ano) { return Set(m_mData.Count() + ano); }
	bool Sub(uint ano) { uint no = m_mData.Count(); return Set((ano >= no ? 0 : no-ano)); }
	bool Set(uint ano);

	bool operator+=(uint ano) { return Add(ano); }
	bool operator-=(uint ano) { return Sub(ano); }
	bool operator++(int) { return Add(1); }
	bool operator--(int) { return Sub(1); }
	bool operator++() { return Add(1); }
	bool operator--() { return Sub(1); }
	
	bool operator=(uint ano) { return Set(ano); }
	MemObj<CType>& operator=(const MemObj<CType> &a);
	
	CType& operator[] (uint pos) const { return *m_mData[pos]; }
	
	CType& Last() const
	{ return (*this)[(m_mData.Count() > 0 ? m_mData.Count()-1 : 0)]; }

	uint Count() const { return m_mData.Count(); }

	// Moves Objects in list WITHOUT overwriting
	bool Move(uint dest, uint start, uint mno);
	bool Insert(uint pos, uint mno=1);
	bool Del(uint pos, uint mno=1);
	// Swap two elements in list
	bool Swap(uint uPos1, uint uPos2);

protected:
	Mem<PCType> m_mData; // Data: Pointers to CType 	
};


#endif
