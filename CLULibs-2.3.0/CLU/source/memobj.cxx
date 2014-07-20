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


// MemObj Class Definitions ------------------------------------

#include<stdlib.h>
#include<memory.h>
#include<string.h>

#include"memobj.h"


//////////////////////////////////////////////////////////////////////////////////
// Constructor

template<class CType>
MemObj<CType>::MemObj(uint nno)
{
	Set(nno);
}


////////////////////////////////////////////////////////////////////////////
// Copy Constructor

template<class CType>
MemObj<CType>::MemObj(const MemObj<CType> &_rMemObj)
{
	*this = _rMemObj;
}


////////////////////////////////////////////////////////////////////////////
// Destructor

template<class CType>
MemObj<CType>::~MemObj()
{
	Set(0);	// deletes all objects in m_mData
}


////////////////////////////////////////////////////////////////////////////
// Set Memory Data

template<class CType>
MemObj<CType>& MemObj<CType>::operator=(const MemObj<CType> &_rMemObj)
{
	uint no;

	if (Set(no = _rMemObj.Count()))
	{ 
		for(uint i=0;i<no;i++)
		{
			*m_mData[i] = *_rMemObj.m_mData[i];
		}
	}

	return *this;
}



////////////////////////////////////////////////////////////////////////////
// Move parts of Memory
// Does NOT overwrite

template<class CType>
bool MemObj<CType>::Move(uint dest, uint start, uint mno)
{
	PCType pDummy;
	uint no = m_mData.Count();

	if (dest+mno > no || start+mno > no) return false;
	if (!mno) return true;

	int i,j,k;
	
	if (dest > start)
	{ 
		for(i=int(start+mno-1),j=int(dest+mno-1);i>=int(start);i--,j--)
		{
			pDummy = m_mData[j];
			m_mData[j] = m_mData[i];
			m_mData[i] = pDummy;
		}
	}
	else if (start > dest)
	{ 
		for(i=0,j=int(start),k=int(dest);i<int(mno);i++,j++,k++)
		{
			pDummy = m_mData[k];
			m_mData[k] = m_mData[j];
			m_mData[j] = pDummy;
		}
	}
	
	return true;
}


////////////////////////////////////////////////////////////////////////////
// Insert Memory

template<class CType>
bool MemObj<CType>::Insert(uint pos, uint mno)
{
	uint ono = m_mData.Count();
	
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

template<class CType>
bool MemObj<CType>::Del(uint pos, uint mno)
{
	uint no = m_mData.Count();

	if (pos+1 < no)
	{ 
		if (!Move(pos, pos+mno, no-pos-mno))
			return false;
	}
	
	Sub(mno);

	return true;
}



////////////////////////////////////////////////////////////////////////////
// Set Memory Size

template<class CType>
bool MemObj<CType>::Set(uint nno)
{
	uint i, no = m_mData.Count();

	if (nno == no) return true;

	if (nno > no)
	{
		if (m_mData = nno)
		{
			for(i=no;i<nno;i++)
			{
				m_mData[i] = new CType;
			}
		}
	}
	else
	{
		// Delete Objects
		for(i=no;i>nno;i--)
		{
			if (m_mData[i-1]) delete m_mData[i-1];
		}

		m_mData = nno;
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////
// Swap two elements in list

template<class CType>
bool MemObj<CType>::Swap(uint uPos1, uint uPos2)
{
	PCType pDummy;
	uint uCount = m_mData.Count();

	if (uPos1 >= uCount || uPos2 >= uCount) 
		return false;

	if (uPos1 == uPos2) 
		return true;

	pDummy = m_mData[uPos1];
	m_mData[uPos1] = m_mData[uPos2];
	m_mData[uPos2] = pDummy;

	return true;
}



