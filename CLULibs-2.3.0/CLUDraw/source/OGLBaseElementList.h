/// LICENSE START
////////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the CLUDraw Library.
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


// Class written by Vladimir Banarer, 3.12.2001.


// OGLBaseElementList.h: Schnittstelle für die Klasse COGLBaseElementList.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLBASEELEMENTLIST_H__5746B287_573C_4800_9DE1_B801F31D0EB4__INCLUDED_)
#define AFX_OGLBASEELEMENTLIST_H__5746B287_573C_4800_9DE1_B801F31D0EB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OGLBaseElement.h"

class COGLBaseElementList : public COGLBaseElement  
{
public:
	COGLBaseElementList();
	virtual ~COGLBaseElementList();

	COGLBaseElementList& operator << (COGLBaseElement& rElement) {Add(rElement); return *this;}
	COGLBaseElement* operator[] (int iPos) { return m_mElementList[iPos]; }

	bool Add(COGLBaseElement& rElement);
	// DeleteInSubLists gives levels of sub lists in which element is to be deleted.
	// All instances of an element are deleted.
	bool Delete(COGLBaseElement& rElement, int DeleteInSubLists = 0);
	bool Delete(cStr& rName, int DeleteInSubLists = 0);

	// Parameter have analogous meaning to Delete.
	bool Replace(COGLBaseElement& rOldElement, COGLBaseElement& rNewElement, int ReplaceInSubLists = 0);
	bool Replace(cStr& rName, COGLBaseElement& rNewElement, int ReplaceInSubLists = 0);

	virtual bool Apply(int iMode = 0, void *pvData = 0);

protected:

	Mem<COGLBaseElement *> m_mElementList;
};

#endif // !defined(AFX_OGLBASEELEMENTLIST_H__5746B287_573C_4800_9DE1_B801F31D0EB4__INCLUDED_)
