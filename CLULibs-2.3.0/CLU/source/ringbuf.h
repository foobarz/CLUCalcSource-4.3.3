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


//////////////////////////////////////////////////////////////////
/// RingBuf Class Declaration
/// Defines ring buffer
//////////////////////////////////////////////////////////////////

#ifndef _RINGBUF_H_
#define _RINGBUF_H_

#include "mem.h"
#include "cstr.h"
#include "makestr.h"

#define RING_OVERWRITE 1
#define RING_NOOVERWRITE 2

#define RING_ABSOLUTE_STR 1
#define RING_RELATIVE_STR 2

#ifdef _GNUCPP3_
	#ifndef _TMPL_
	#define _TMPL_ <>
	#endif
#else
	#ifndef _TMPL_
	#define _TMPL_
	#endif
#endif

typedef unsigned int uint;

/*
#ifdef _GNUCPP3_
template<class CType> class RingBuf;

	// Add element to ring
	template<class CType> RingBuf<CType>& operator, (RingBuf<CType>& R, const CType& d);

	// Retrieve element from Ring
	template<class CType> RingBuf<CType>& operator, (CType& d, RingBuf<CType>& R);

	// Reset Ring Buffer 
	template<class CType> RingBuf<CType>& operator! (RingBuf<CType>& R);
#endif
*/

template<class CType>
class RingBuf
{
	public:

	RingBuf();
	RingBuf(uint nno, uint ntype = RING_OVERWRITE);
	RingBuf(const RingBuf<CType>& R);

	// Resize Ring
	uint Resize(uint nno);

	// Reset Ring
	uint Reset(uint nno, uint ntype = RING_OVERWRITE);

	// Equal operator
	RingBuf<CType>& operator= (const RingBuf<CType>& R);

	// Add element to ring
	RingBuf<CType>& operator<< (const CType& d);

	// Retrieve element from Ring
	RingBuf<CType>& operator>> (CType& d);

	// Reset Ring Buffer 
	RingBuf<CType>& operator! ();

	// Return element in ring, relative to fpos;
	CType operator[] (uint pos);

	// Return Pointer to Data array
	operator CType*() { return data.Data(); }

	// Create String Output
	cStr Str(uint how = RING_ABSOLUTE_STR);

	protected:
		void Empty();
		uint add(const CType& d);
		CType take();
		
		uint full; // if buffer is full set to 1
		uint no; // No. of elements
		uint lpos; // Pos. of next new element
		uint fpos; // Pos. of first element
		uint type; // Type of Ring

		cStr str; // String for output
		Mem<CType> data; // Contents of Ring
};

#endif

