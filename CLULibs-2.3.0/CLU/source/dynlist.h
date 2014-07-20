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


// DynList Class (Dynamic List) -------------------------------------
#ifndef _DYNLIST_HH_
#define _DYNLIST_HH_

#include"mem.h"




template<class CType>
class DynList
{
public:
	
	typedef CType* PCType;

	DynList();
	DynList(const CType &neret);
	DynList(const DynList<CType> &a);
	virtual ~DynList();
	
	CType& operator[](uint pos) const     // Returns Object at pos in List
	{ return ( (pos < list.Count()) ? *(list[pos]) : *eret ); }
	
	void Empty() { Sub(0, list.Count()); }
	uint Count() const { return list.Count(); }
	
//protected:
	Mem<PCType> list;
	CType *eret;                  // Instance returned if error
	
	uint Set(const DynList<CType> &a);
	
	// The add functions return 0 if unsuccessful and the position
	// of the newly added element +1 if successful.
	uint Add(const CType &a);
	uint Add(const CType &a, uint pos);
	uint AddNew(); // Adds element without initializing it
	
	uint Sub(uint pos, uint mno=1);
};

#endif
