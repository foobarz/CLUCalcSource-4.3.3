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


// MemBase Class ---------------------------------------------
//
// This class can be used for objects that do not need to have their constructor/destructor
// called. It uses memcpy, memmove for copying.
//
// This class forms a base to the template class Mem

#ifndef _MEMBASE_HH_
#define _MEMBASE_HH_


typedef unsigned int uint;

class MemBase
{
public:	
	MemBase(uint _uElSize, uint nno = 0);
	MemBase(const MemBase &a);
	virtual ~MemBase();  

public:
	MemBase& operator= (const MemBase& a);

	bool Add(uint ano) { return Set(m_uElNo + ano); }
	bool Sub(uint ano) { return Set((ano >= m_uElNo ? 0 : m_uElNo-ano)); }
	bool Set(uint ano);

	bool operator+=(uint ano) { return Add(ano); }
	bool operator-=(uint ano) { return Sub(ano); }
	bool operator++(int) { return Add(1); }
	bool operator--(int) { return Sub(1); }
	bool operator++() { return Add(1); }
	bool operator--() { return Sub(1); }
	
	bool Move(uint dest, uint start, uint mno);
	bool Insert(uint pos, uint mno=1);
	bool Del(uint pos, uint mno=1);

	uint GetSizeOfElement() const { return m_uElSize; }
	uint Count() const { return m_uElNo; }

	void* Data() const { return data; }
	void* GetElPtr(uint uEl) const { return &data[(uEl >= m_uElNo ? 0 : uEl) * m_uElSize]; }
	
protected:
	uint m_uElSize;	// No of bytes per element
	uint m_uElNo;   // No of elements
	
	char *data;		// Data Mem pointer	
};


#endif
