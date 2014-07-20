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

#include"mem.h"


//////////////////////////////////////////////////////////////////////////////////
// Constructor

template<class CType>
Mem<CType>::Mem(uint nno) : MemBase(sizeof(CType), nno)
{

}


////////////////////////////////////////////////////////////////////////////
// Copy Constructor

template<class CType>
Mem<CType>::Mem(const Mem<CType> &a) : MemBase(a)
{
}


////////////////////////////////////////////////////////////////////////////
// Destructor

template<class CType>
Mem<CType>::~Mem()
{
}



////////////////////////////////////////////////////////////////////////////
// Set Memory Data

template<class CType>
Mem<CType>& Mem<CType>::operator=(const Mem<CType> &a)
{
	MemBase::operator=(a);

	return *this;
}


