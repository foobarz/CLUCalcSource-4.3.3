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


// ------- DynList Library functions

#include"dynlist.h"

////////////////////////////////////////////////////////////////////////////////
// Constructor

template<class CType>
DynList<CType>::DynList()
{
	eret = 0;
}

////////////////////////////////////////////////////////////////////////////////
// Constructor

template<class CType>
DynList<CType>::DynList(const CType &neret)
{
	eret = 0;
}


////////////////////////////////////////////////////////////////////////////////
// Copy Constructor

template<class CType>
DynList<CType>::DynList(const DynList<CType> &a)
{
	eret = 0;
	Set(a);
}


////////////////////////////////////////////////////////////////////////////////
// Destructor

template<class CType>
DynList<CType>::~DynList()
{
	uint i,cnt = list.Count();
	CType **data = list.Data();
	
	for(i=0;i<cnt;i++)
		if(data[i]) delete data[i];
		
		
	if (eret) 
	{ 
		delete eret;
		eret = 0;
	}
}



////////////////////////////////////////////////////////////////////////////////
// Add Object to List

template<class CType>
uint DynList<CType>::Add(const CType &a)
{
	CType *nelem = new CType(a);
	
	if (!nelem) return 0;
	
	if (list.Add(1))
	{ 
		uint pos = list.Count()-1;
		list[pos] = nelem;
		
		return pos+1;		
	}

	return 0;
}


////////////////////////////////////////////////////////////////////////////////
// Insert Object in List

template<class CType>
uint DynList<CType>::Add(const CType &a, uint pos)
{    
	uint cnt = list.Count();
	
	if (pos > cnt) pos = cnt;
	
	CType *nelem = new CType(a);
	
	if (!nelem) return 0;
	
	if (list.Insert(pos))
	{ 
		CType **data = list.Data();
		
		list[pos] = nelem;
		
		return pos+1;
	}
	return 0;
}


////////////////////////////////////////////////////////////////////////////////
// Add Element without initializing it

template<class CType>
uint DynList<CType>::AddNew()
{    
	CType *nelem = new CType;
	
	if (!nelem) return 0;
	
	if (list.Add(1))
	{ 
		uint pos = list.Count()-1;
		
		list[pos] = nelem;
		
		return pos+1;		
	}

	return 0;
}


////////////////////////////////////////////////////////////////////////////////
// Delete Blade(s) from List

template<class CType>
uint DynList<CType>::Sub(uint pos, uint mno)
{ 
	if (mno == 0) return 0;
	
	uint cnt = list.Count();
	CType **data = list.Data();
	
	if (pos+mno > cnt) 
		mno = cnt-pos;
	
	for(uint i=0;i<mno;i++)
		if (data[pos+i]) delete data[pos+i];
		
	list.Del(pos, mno);
		
	return mno;
}



////////////////////////////////////////////////////////////////////////////////
// Set DynList

template<class CType>
uint DynList<CType>::Set(const DynList<CType> &a)
{ 
	uint cnt = a.list.Count();
	uint i,j;
	
	Empty();
	
	if ((list = cnt))
	{ 
		CType **data = list.Data();
		CType **odata = a.list.Data();
		
		for(i=0;i<cnt;i++) 
		{ 
			if (!(data[i] = new CType(odata[i][0])))
			{ 
				for(j=i;j>0;j--)
					delete data[j-1];
				
				list = 0;
				return 0;
			}
		}
		return cnt;
	}

	return 0;
}


