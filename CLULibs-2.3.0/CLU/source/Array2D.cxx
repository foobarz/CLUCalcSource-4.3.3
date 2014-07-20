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


#include "Array2D.h"


////////////////////////////////////////////////////////////////////////////////////
/// Constructor

template<class CType>
CArray2D<CType>::CArray2D()
{
	m_iDimX = 0;
	m_iDimY = 0;
}

////////////////////////////////////////////////////////////////////////////////////
/// Copy Constructor

template<class CType>
CArray2D<CType>::CArray2D(const CArray2D<CType>& rA2D)
{
	*this = rA2D;
}

////////////////////////////////////////////////////////////////////////////////////
/// Init Constructor

template<class CType>
CArray2D<CType>::CArray2D(int iCX, int iCY)
{
	SetDim(iCX, iCY);
}

////////////////////////////////////////////////////////////////////////////////////
/// Init Constructor

template<class CType>
CArray2D<CType>::CArray2D(const CPointND<int, 2>& rP2D)
{
	SetDim(rP2D(0), rP2D(1));
}

////////////////////////////////////////////////////////////////////////////////////
/// Operator=

template<class CType>
CArray2D<CType>& CArray2D<CType>::operator= (const CArray2D<CType>& rA2D)
{
	if (!SetDim(rA2D.m_iDimX, rA2D.m_iDimY))
	{
		SetDim(0, 0);
		return *this;
	}

	m_mArray = rA2D.m_mArray;

	return *this;
}


////////////////////////////////////////////////////////////////////////////////////
/// Set Dimension

template<class CType>
bool CArray2D<CType>::SetDim(int iCX, int iCY)
{
	if (iCX < 0 || iCY < 0) return false;

	if (iCX == 0 && iCY == 0)
	{
		m_mArray.Set(0);
		return true;
	}

	if (iCX == m_iDimX && iCY == m_iDimY) return true;

	uint uNewCount = uint(iCX) * uint(iCY);

	if (uNewCount > m_mArray.Count())
	{
		if (!m_mArray.Set(uNewCount))
			return false;
	}

	// If X-Dimension changed need to reorder lines
	int iMinCY = (iCY < m_iDimY ? iCY : m_iDimY);
	int i, iOldLine, iNewLine;

	if (iCX < m_iDimX)
	{
		iOldLine = m_iDimX;
		iNewLine = iCX;
		for(i=1;i<iMinCY;i++, iOldLine+=m_iDimX, iNewLine+=iCX)
		{
			m_mArray.Move(iNewLine, iOldLine, iCX);
		}
	}
	else if (iCX > m_iDimX)
	{
		// Last of old lines
		iOldLine = m_iDimX * (iMinCY - 1);
		// Lst of new lines
		iNewLine = iCX * (iMinCY - 1);
		for(i=1;i<iMinCY;i++, iOldLine-=m_iDimX, iNewLine-=iCX)
		{
			m_mArray.Move(iNewLine, iOldLine, m_iDimX);
		}
	}
	
	if (uNewCount < m_mArray.Count())
	{
		if (!m_mArray.Set(uNewCount))
			return false;
	}

	m_iDimX = iCX;
	m_iDimY = iCY;

	return true;
}

