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

#if !defined(AFX_IMAGEREPOSITORY_H__29DD89BE_9839_4381_A23B_049B46BAD391__INCLUDED_)
#define AFX_IMAGEREPOSITORY_H__29DD89BE_9839_4381_A23B_049B46BAD391__INCLUDED_

#include <list>
#include <map>

using std::list;
using std::map;

class CImageReference;

class CImageRepository
{
public:
	friend class CImageReference;

public:
	typedef COGLBitmap TImage;
	typedef TImage* TImagePtr;
	typedef list<CImageReference*> TImgRefList;
	typedef map<TImagePtr, TImgRefList> TImgToRefMap;

public:
	CImageRepository();
	virtual ~CImageRepository();

	// Create a new image and return the reference
	CImageReference* New();

	// Create a new image which is a copy of the one given
	CImageReference* Copy(CImageReference& rRef);

	// Delete all images
	void Reset();

protected:
	bool Register(CImageReference& rRef);
	bool DeRegister(CImageReference& rRef);

protected:
	TImgToRefMap m_mapImgToRef;
};

#include "ImageReference.h"

#endif // !defined(AFX_IMAGEREPOSITORY_H__29DD89BE_9839_4381_A23B_049B46BAD391__INCLUDED_)
