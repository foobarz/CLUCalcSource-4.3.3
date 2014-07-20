/// LICENSE START
////////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the CLUParse Library.
//
// 
// Copyright (C) 2001-2004  Christian B.U. Perwass
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

// Stack.h: Schnittstelle für die Klasse CStack.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGEREFERENCE_H__29DD89BE_9839_4381_A23B_049B46BAD391__INCLUDED_)
#define AFX_IMAGEREFERENCE_H__29DD89BE_9839_4381_A23B_049B46BAD391__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ImageRepository.h"

class CImageReference
{
public:
	friend class CImageRepository;

public:
	typedef CImageRepository::TImage TImage;
	typedef CImageRepository::TImagePtr TImagePtr;

public:
	CImageReference();
	CImageReference(const CImageReference& rRef);
	virtual ~CImageReference();

	CImageReference& operator= (const CImageReference& Ref);

	operator TImagePtr() const { return m_pImage; }
	TImagePtr operator-> () { return m_pImage; }

	bool IsValid() { return (m_pImage ? true : false); }
	

protected:
	// Set pointers
	bool Set(TImagePtr pImg, CImageRepository *pImgRep);

	// Set pointers to zero
	void Invalidate();

protected:
	// Pointer to the actual image
	TImagePtr m_pImage;

	// Pointer to the Image Repository where this reference is registered
	CImageRepository *m_pImgRep;

};

#endif // !defined(AFX_IMAGEREFERENCE_H__29DD89BE_9839_4381_A23B_049B46BAD391__INCLUDED_)
