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


#ifndef _BLADELIST_H_
#define _BLADELIST_H_


#include "base.h"
#include "blade.h"

#include "mem.h"
#include "dynlist.h"


#ifndef _MAXSTRSIZE_
#define _MAXSTRSIZE_ 1024   // Maximum String Size for String output
#endif

#ifdef _GNUCPP3_
	#ifndef _TMPL_
	#define _TMPL_ <>
	#endif
#	ifndef _FRIEND_
#		define _FRIEND_  friend 
#	endif
#else
	#ifndef _TMPL_
	#define _TMPL_
	#endif
#	ifndef _FRIEND_
#		define _FRIEND_  friend 
#	endif
#endif


typedef unsigned uint;
typedef unsigned long ulong;

template<class CType> class MultiV;
template<class CType> class BladeList;

template<class CType> int operator== (const BladeList<CType> &,const Blade<CType> &); 

template<class CType> CType Scalar (const BladeList<CType> &bl);

template<class CType> BladeList<CType> operator+ (const BladeList<CType> &,
				const CType &);
template<class CType> BladeList<CType> operator+ (const CType &,
				const BladeList<CType> &);

template<class CType> BladeList<CType> operator- (const BladeList<CType> &,
				const CType &);
template<class CType> BladeList<CType> operator- (const CType &,
				const BladeList<CType> &);

template<class CType> BladeList<CType>& operator+= (BladeList<CType> &,const CType &);
template<class CType> BladeList<CType>& operator-= (BladeList<CType> &,const CType &);
template<class CType> BladeList<CType>& operator<< (BladeList<CType> &,const CType &);

template<class CType> BladeList<CType> operator+ (const BladeList<CType> &,
				const Blade<CType> &);
template<class CType> BladeList<CType> operator+ (const Blade<CType> &,
				const BladeList<CType> &);

template<class CType> BladeList<CType> operator- (const BladeList<CType> &,
				const Blade<CType> &);
template<class CType> BladeList<CType> operator- (const Blade<CType> &,
				const BladeList<CType> &);

template<class CType> BladeList<CType>& operator+= (BladeList<CType> &,const Blade<CType> &);
template<class CType> BladeList<CType>& operator-= (BladeList<CType> &,const Blade<CType> &);

// see note in cga.cc for comparison of << and += operator
template<class CType> BladeList<CType>& operator<< (BladeList<CType> &,
	const Blade<CType> &); 

template<class CType> BladeList<CType> operator+ (const BladeList<CType> &,
				const BladeList<CType> &);
template<class CType> BladeList<CType> operator- (const BladeList<CType> &,
				const BladeList<CType> &);
template<class CType> BladeList<CType>& operator+= (BladeList<CType> &,
	const BladeList<CType> &);
template<class CType> BladeList<CType>& operator-= (BladeList<CType> &,
	const BladeList<CType> &);

// see note in cga.cc for comparison of << and += operator
template<class CType> BladeList<CType>& operator<< (BladeList<CType> &,
	const BladeList<CType> &);

template<class CType> BladeList<CType> operator^ (const BladeList<CType> &,
				const CType &);
template<class CType> BladeList<CType> operator^ (const CType &,
				const BladeList<CType> &);
template<class CType> BladeList<CType>& operator^= (BladeList<CType> &,
	const CType &);

template<class CType> BladeList<CType> operator& (const BladeList<CType> &,
				const Blade<CType> &);
template<class CType> BladeList<CType> operator& (const Blade<CType> &,
				const BladeList<CType> &);
template<class CType> BladeList<CType>& operator&= (BladeList<CType> &,
	const Blade<CType> &);

template<class CType> BladeList<CType> operator* (const BladeList<CType> &,
				const Blade<CType> &);
template<class CType> BladeList<CType> operator* (const Blade<CType> &,
				const BladeList<CType> &);
template<class CType> BladeList<CType>& operator*= (BladeList<CType> &,
	const Blade<CType> &);

template<class CType> BladeList<CType> operator^ (const BladeList<CType> &,
				const Blade<CType> &);
template<class CType> BladeList<CType> operator^ (const Blade<CType> &,
				const BladeList<CType> &);
template<class CType> BladeList<CType>& operator^= (BladeList<CType> &,
	const Blade<CType> &);

template<class CType> BladeList<CType> operator& (const BladeList<CType> &,
				const BladeList<CType> &);
template<class CType> BladeList<CType>& operator&= (BladeList<CType> &,
	const BladeList<CType> &);
template<class CType> BladeList<CType> operator* (const BladeList<CType> &,
				const BladeList<CType> &);
template<class CType> BladeList<CType>& operator*= (BladeList<CType> &,
	const BladeList<CType> &);
template<class CType> BladeList<CType> operator^ (const BladeList<CType> &,
				const BladeList<CType> &);
template<class CType> BladeList<CType>& operator^= (BladeList<CType> &,
	const BladeList<CType> &);
// Reversion of Blades
template<class CType> BladeList<CType> operator~ (const BladeList<CType> &);   
// Negation of factors of Blades
template<class CType> BladeList<CType> operator- (const BladeList<CType> &); 
// Inversion of Blades
template<class CType> BladeList<CType> operator! (const BladeList<CType> &);   

template<class CType> BladeList<CType> cp (const BladeList<CType> &,
			    const Blade<CType> &);
template<class CType> BladeList<CType> acp (const BladeList<CType> &,
			const Blade<CType> &);
template<class CType> BladeList<CType> cp (const BladeList<CType> &,
			    const BladeList<CType> &);
template<class CType> BladeList<CType> acp (const BladeList<CType> &,
	const BladeList<CType> &);


// Blade List Class ----------------------------------------

template<class CType>
class BladeList : public DynList<Blade<CType> >
{
public:
	//  typedef Blade<CType> * PBlade;
	
	BladeList();
	BladeList(Base<CType> &nbase);
	BladeList(const BladeList<CType> &a);
	virtual ~BladeList();
	
	void Reset(Base<CType> &nbase);
	
	// Gives Position, where Blade can be found
//	_FRIEND_ int operator== _TMPL_(const BladeList<CType> &,const Blade<CType> &); 
	
	BladeList<CType> operator()(uint grade) const; // Returns List of Blades of grade
	CType operator()() const;  // Returns Scalar Part as CType
//	_FRIEND_ CType Scalar _TMPL_(const BladeList<CType> &bl);
	
	BladeList<CType>& operator=(const BladeList<CType> &a);
	BladeList<CType>& operator=(const Blade<CType> &a);
	BladeList<CType>& operator=(const MultiV<CType> &a);
/*	
	_FRIEND_ BladeList<CType> operator+ _TMPL_(const BladeList<CType> &,
				    const CType &);
	_FRIEND_ BladeList<CType> operator+ _TMPL_(const CType &,
				    const BladeList<CType> &);
	
	_FRIEND_ BladeList<CType> operator- _TMPL_(const BladeList<CType> &,
				    const CType &);
	_FRIEND_ BladeList<CType> operator- _TMPL_(const CType &,
				    const BladeList<CType> &);
	
	_FRIEND_ BladeList<CType>& operator+= _TMPL_(BladeList<CType> &,const CType &);
	_FRIEND_ BladeList<CType>& operator-= _TMPL_(BladeList<CType> &,const CType &);
	_FRIEND_ BladeList<CType>& operator<< _TMPL_(BladeList<CType> &,const CType &);
	
	_FRIEND_ BladeList<CType> operator+ _TMPL_(const BladeList<CType> &,
				    const Blade<CType> &);
	_FRIEND_ BladeList<CType> operator+ _TMPL_(const Blade<CType> &,
				    const BladeList<CType> &);
	
	_FRIEND_ BladeList<CType> operator- _TMPL_(const BladeList<CType> &,
				    const Blade<CType> &);
	_FRIEND_ BladeList<CType> operator- _TMPL_(const Blade<CType> &,
				    const BladeList<CType> &);
	
	_FRIEND_ BladeList<CType>& operator+= _TMPL_(BladeList<CType> &,const Blade<CType> &);
	_FRIEND_ BladeList<CType>& operator-= _TMPL_(BladeList<CType> &,const Blade<CType> &);
	
	// see note in cga.cc for comparison of << and += operator
	_FRIEND_ BladeList<CType>& operator<< _TMPL_(BladeList<CType> &,
		const Blade<CType> &); 
	
	_FRIEND_ BladeList<CType> operator+ _TMPL_(const BladeList<CType> &,
				    const BladeList<CType> &);
	_FRIEND_ BladeList<CType> operator- _TMPL_(const BladeList<CType> &,
				    const BladeList<CType> &);
	_FRIEND_ BladeList<CType>& operator+= _TMPL_(BladeList<CType> &,
		const BladeList<CType> &);
	_FRIEND_ BladeList<CType>& operator-= _TMPL_(BladeList<CType> &,
		const BladeList<CType> &);
	
	// see note in cga.cc for comparison of << and += operator
	_FRIEND_ BladeList<CType>& operator<< _TMPL_(BladeList<CType> &,
		const BladeList<CType> &);
    
	_FRIEND_ BladeList<CType> operator^ _TMPL_(const BladeList<CType> &,
				    const CType &);
	_FRIEND_ BladeList<CType> operator^ _TMPL_(const CType &,
				    const BladeList<CType> &);
	_FRIEND_ BladeList<CType>& operator^= _TMPL_(BladeList<CType> &,
		const CType &);
	
	_FRIEND_ BladeList<CType> operator& _TMPL_(const BladeList<CType> &,
				    const Blade<CType> &);
	_FRIEND_ BladeList<CType> operator& _TMPL_(const Blade<CType> &,
				    const BladeList<CType> &);
	_FRIEND_ BladeList<CType>& operator&= _TMPL_(BladeList<CType> &,
		const Blade<CType> &);
	
	_FRIEND_ BladeList<CType> operator* _TMPL_(const BladeList<CType> &,
				    const Blade<CType> &);
	_FRIEND_ BladeList<CType> operator* _TMPL_(const Blade<CType> &,
				    const BladeList<CType> &);
	_FRIEND_ BladeList<CType>& operator*= _TMPL_(BladeList<CType> &,
		const Blade<CType> &);
	
	_FRIEND_ BladeList<CType> operator^ _TMPL_(const BladeList<CType> &,
				    const Blade<CType> &);
	_FRIEND_ BladeList<CType> operator^ _TMPL_(const Blade<CType> &,
				    const BladeList<CType> &);
	_FRIEND_ BladeList<CType>& operator^= _TMPL_(BladeList<CType> &,
		const Blade<CType> &);
	
	_FRIEND_ BladeList<CType> operator& _TMPL_(const BladeList<CType> &,
				    const BladeList<CType> &);
	_FRIEND_ BladeList<CType>& operator&= _TMPL_(BladeList<CType> &,
		const BladeList<CType> &);
	_FRIEND_ BladeList<CType> operator* _TMPL_(const BladeList<CType> &,
				    const BladeList<CType> &);
	_FRIEND_ BladeList<CType>& operator*= _TMPL_(BladeList<CType> &,
		const BladeList<CType> &);
	_FRIEND_ BladeList<CType> operator^ _TMPL_(const BladeList<CType> &,
				    const BladeList<CType> &);
	_FRIEND_ BladeList<CType>& operator^= _TMPL_(BladeList<CType> &,
		const BladeList<CType> &);
	// Reversion of Blades
	_FRIEND_ BladeList<CType> operator~ _TMPL_(const BladeList<CType> &);   
	// Negation of factors of Blades
	_FRIEND_ BladeList<CType> operator- _TMPL_(const BladeList<CType> &); 
	// Inversion of Blades
	_FRIEND_ BladeList<CType> operator! _TMPL_(const BladeList<CType> &);   
	
	_FRIEND_ BladeList<CType> cp _TMPL_(const BladeList<CType> &,
			     const Blade<CType> &);
	_FRIEND_ BladeList<CType> acp _TMPL_(const BladeList<CType> &,
				const Blade<CType> &);
	_FRIEND_ BladeList<CType> cp _TMPL_(const BladeList<CType> &,
			     const BladeList<CType> &);
	_FRIEND_ BladeList<CType> acp _TMPL_(const BladeList<CType> &,
		const BladeList<CType> &);
*/
// The following are not implemented correctly
/*	
	_FRIEND_ BladeList<CType> join(const BladeList<CType> &, 
		const Blade<CType> &);
	_FRIEND_ BladeList<CType> meet(const BladeList<CType> &, 
		const Blade<CType> &);
	_FRIEND_ BladeList<CType> join(const BladeList<CType> &, 
		const BladeList<CType> &);
	_FRIEND_ BladeList<CType> meet(const BladeList<CType> &, 
		const BladeList<CType> &);
*/	
	
	
	CType mag2() const; // Magnitude of Blade List SQUARED(!)
	char* Str();
	Base<CType>* BasePtr() const { return base; }
	
//protected:
	Base<CType> *base;
	cStr str;
	
	uint check();
};


#endif // _BLADELIST_H_

