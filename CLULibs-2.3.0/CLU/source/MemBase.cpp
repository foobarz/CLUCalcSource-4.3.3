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


// Mem Class Definitions ------------------------------------

#include<stdlib.h>
#include<memory.h>
#include<string.h>

#include"MemBase.h"


//////////////////////////////////////////////////////////////////////////////////
// Constructor

MemBase::MemBase(uint _uElSize, uint _uElNo)
{
	m_uElNo = _uElNo;
	m_uElSize = _uElSize;

	uint size = ((m_uElNo == 0) ? 1 : m_uElNo) * m_uElSize;

	data = (char *) malloc(size);
}


////////////////////////////////////////////////////////////////////////////
// Copy Constructor

MemBase::MemBase(const MemBase &a)
{
	data = 0;

	*this = a;
}


////////////////////////////////////////////////////////////////////////////
// Destructor

MemBase::~MemBase()
{
	if (data) free(data);
}


////////////////////////////////////////////////////////////////////////////
// Set Memory Data

MemBase& MemBase::operator=(const MemBase &a)
{
	m_uElSize = a.m_uElSize;
	m_uElNo = 0;

	if ( Set(a.m_uElNo) )
	{
		uint size = m_uElNo * m_uElSize;

		if (size)
			memcpy(data, a.data, size);
	}

	return *this;
}


////////////////////////////////////////////////////////////////////////////
// Set Memory Size

bool MemBase::Set(uint nno)
{
	if (nno == m_uElNo) return true;
	
	uint size = ((nno == 0) ? 1 : nno) * m_uElSize;
	
	if (data)
	{
		char *ndata;
	
		if ( !(ndata = (char *) realloc(data, size)) ) return false;
	
		data = ndata;
	}
	else
	{
		if ( !(data = (char *) malloc(size)) ) return false;
	}

	m_uElNo = nno;
	
	return true;
}



////////////////////////////////////////////////////////////////////////////
// Move parts of Memory

bool MemBase::Move(uint dest, uint start, uint mno)
{
	if (dest+mno > m_uElNo || start+mno > m_uElNo) return false;
	
	memmove(&data[dest*m_uElSize], &data[start*m_uElSize], mno * m_uElSize);

	return true;
}


////////////////////////////////////////////////////////////////////////////
// Insert Memory

bool MemBase::Insert(uint pos, uint mno)
{
	uint ono = m_uElNo;
	
	if (Add(mno))
	{ 
		if (Move(pos+mno, pos, ono-pos))
			return true;

		Sub(mno);
	}
	
	return false;
}



////////////////////////////////////////////////////////////////////////////
// Delete Memory

bool MemBase::Del(uint pos, uint mno)
{
	if (pos+1 < m_uElNo)
	{ 
		if (!Move(pos, pos+mno, m_uElNo-pos-mno))
			return false;
	}
	
	Sub(mno);

	return true;
}





