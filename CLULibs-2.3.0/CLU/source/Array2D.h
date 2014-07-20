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


#ifndef __ARRAY2D_H__
#define __ARRAY2D_H__

#include "Point.h"
#include "memobj.h"
// 2D Array of Class Objects

template<class CType>
class CArray2D
{
public:
	CArray2D();
	CArray2D(int iCX, int iCY);
	CArray2D(const CPointND<int, 2>& rP2D);
	CArray2D(const CArray2D<CType>& rA2D);

	CArray2D<CType>& operator= (const CArray2D<CType>& rA2D);

	void GetDim(int &iCX, int &iCY) { iCX = m_iDimX; iCY = m_iDimY; }
	bool SetDim(int iCX, int iCY);
	bool SetDim(const CPointND<int, 2>& rP2D) { return SetDim(rP2D(0), rP2D(1)); }

	CType& operator() (int iX, int iY) 
	{ return m_mArray[iY * m_iDimX + iX]; }

	CType& operator() (const CPointND<int, 2>& rP2D) 
	{ return m_mArray[rP2D(1) * m_iDimX + rP2D(0)]; }

	bool ExistElement(int iX, int iY)
	{ return (iX >= 0 && iX < m_iDimX && iY >= 0 && iY < m_iDimY); }

	bool ExistElement(const CPointND<int, 2>& rP2D)
	{ return (rP2D(0) >= 0 && rP2D(0) < m_iDimX && rP2D(1) >= 0 && rP2D(1) < m_iDimY); }

	
protected:
	int m_iDimX, m_iDimY;

	Mem<CType> m_mArray;
};


#endif

