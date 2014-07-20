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


#ifndef __STATICARRAY_H__
#define __STATICARRAY_H__

typedef unsigned int uint;

// Static Array for non-class types

template<class CType, int CNo>
class CStaticArray
{
public:
	CStaticArray() 
	{ memset(pData, 0, CNo*sizeof(CType)); }

	CStaticArray(const CStaticArray<CType,CNo>& np) { *this = np; }

	CStaticArray<CType,CNo>& operator= (const CType *na)
		{ memcpy(pData, na, CNo*sizeof(CType)); return *this; }

	CStaticArray<CType,CNo>& operator= (const CStaticArray<CType,CNo>& np)
		{ memcpy(pData, np.pData, CNo*sizeof(CType)); return *this; }

	CType operator() (uint pos) const 
		{ return pData[(pos >= CNo ? 0 : pos)]; }

	CType& operator[] (uint pos)
		{ return pData[(pos >= CNo ? 0 : pos)]; }

	const CType* Data() const { return pData; }
	
	int Dim() const { return CNo; }

	
protected:
	CType pData[CNo]; // Elements

};



// Static array for class types that need constructor to be called
// operator= must be defined!

template<class CType, int CNo>
class CStaticArrayObj
{
public:
	CStaticArrayObj() 
	{ for(int i=0;i<CNo;i++) pData[i] = CType(0); }

	CStaticArrayObj(const CStaticArrayObj<CType,CNo>& np) { *this = np; }

	CStaticArrayObj<CType,CNo>& operator= (CType *na)
		{ for(int i=0;i<CNo;i++) pData[i] = na[i]; return *this; }

	CStaticArrayObj<CType,CNo>& operator= (const CStaticArrayObj<CType,CNo>& np)
		{ for(int i=0;i<CNo;i++) pData[i] = np.pData[i]; return *this; }

	CType& operator[] (uint pos)
		{ return pData[(pos >= CNo ? 0 : pos)]; }

	CType* Data() { return pData; }
	
	int Dim() const { return CNo; }

	
protected:
	CType pData[CNo]; // Elements

};



#endif

