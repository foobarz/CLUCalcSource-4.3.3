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

// MessageList.h: Schnittstelle für die Klasse CMessageList.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESSAGELIST_H__51963663_24D3_4AB2_B8B0_5A688881FD3C__INCLUDED_)
#define AFX_MESSAGELIST_H__51963663_24D3_4AB2_B8B0_5A688881FD3C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


struct SMsg
{
	int iNo;
	int iLevel;
	int iLine;
	int iPos;
	cStr csText;

	SMsg() { iNo = 0; iLevel = 0; }
};

class CMessageList : public MemObj<SMsg>  
{
public:
	CMessageList();
	virtual ~CMessageList();

	bool Reset() { return Set(0); }

	bool AddMsg(const char* pcText, int iLine = -1, int iPos = -1, int iNo = 0, int iLevel = 0)
	{ if (!Add(1)) return false; 
		Last().iLine = iLine; Last().iPos = iPos; 
		Last().iNo = iNo; Last().iLevel = iLevel; 
		Last().csText = pcText; return true; }

};

#endif // !defined(AFX_MESSAGELIST_H__51963663_24D3_4AB2_B8B0_5A688881FD3C__INCLUDED_)
