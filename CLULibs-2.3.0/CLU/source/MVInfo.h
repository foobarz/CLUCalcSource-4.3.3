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


// MVInfo.h: Schnittstelle f�r die Klasse CMVInfo.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MVINFO_H__0C6104EF_B0A7_4D75_BC7F_B779A7EC936D__INCLUDED_)
#define AFX_MVINFO_H__0C6104EF_B0A7_4D75_BC7F_B779A7EC936D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Point.h"
#include "mem.h"

#define Instantiate_MVInfo(TYPE) \
	template class CPointND<TYPE, 3>;\
	template class Mem<CPointND<TYPE, 3> >;\
	template class Mem<TYPE>;\
	template class CMVInfo<TYPE>;


enum EMVType
{
	GA_NONE, GA_SCALAR, 
	GA_POINT, GA_LINE, GA_PLANE, GA_SPACE,
	GA_POINTPAIR, GA_CIRCLE, GA_SPHERE, 
	GA_DIR1D, GA_DIR2D, GA_DIR3D,
	GA_ROTOR, GA_MOTOR, GA_TWIST, GA_TRANSLATOR,
	GA_ELLIPSE, GA_HYPERBOLA, GA_PARLINEPAIR, 
	GA_LINEPAIR, GA_POINTSET,
	// The following are direction elements in conformal space,
	// i.e. elements with no real null space but a nullspace
	// w.r.t. x + e elements.
	GA_INF_POINTPAIR, GA_INF_LINE
};

template<class CType>
class CMVInfo  
{
public:
	CMVInfo();
	CMVInfo(const CMVInfo<CType>& rMVI);

	void Reset();

	// Multivector Type
	EMVType m_eType;

	// Analysis Data of Multivector
	Mem<CPointND<CType, 3> > m_mVex;
	Mem<CType> m_mVal;

};

#endif // !defined(AFX_MVINFO_H__0C6104EF_B0A7_4D75_BC7F_B779A7EC936D__INCLUDED_)
