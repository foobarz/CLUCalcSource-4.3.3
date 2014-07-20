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


// StrMessageList.h: Schnittstelle für die Klasse StrMessageList.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STRMESSAGELIST_H__2321E060_528F_11D3_9065_444553540001__INCLUDED_)
#define AFX_STRMESSAGELIST_H__2321E060_528F_11D3_9065_444553540001__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cstr.h"
#include "matrix.h"
#include "MessageList.h"


class StrMessageList : public MessageList<cStr>  
{
public:
	StrMessageList();
	virtual ~StrMessageList();

	StrMessageList& operator= (const StrMessageList& sml)
	{ MessageList<cStr>::operator = (MessageList<cStr>(sml)); return *this; }

	StrMessageList& operator<< (const StrMessageList& sml)
	{ MessageList<cStr>::operator << (MessageList<cStr>(sml)); return *this; }

	// Add message no and a text to msgData
	void AddMessage(uint mno, const char *mdata);

	// Set message with number and optionally some other data
	// than the one given in msgData
	void SetMessage(uint mno, const char *mdata = 0);

	// Set Standard Message Number
	void SetStdMsgNo(uint mno) { if (mno > 0) uStdMsgNo = mno; }

	// Set Messages with standard message number
	void SetMessage(const char *mdata);
	void SetMessage(cStr& mStr);
	void SetMessage(Matrix<double>& mMat);

	// Set Standard Message Number
	StrMessageList& operator << (uint mno) { SetStdMsgNo(mno); return *this; }

	// Set Messages with standard message number
	StrMessageList& operator << (const char *mdata) { SetMessage(mdata); return *this; }
	StrMessageList& operator << (cStr& mStr) { SetMessage(mStr); return *this; }
	StrMessageList& operator << (Matrix<double>& mMat) { SetMessage(mMat); return *this; }

protected:
	uint uStdMsgNo; // Std Message Number. Is used if no Msg. No. is given.
};

#endif // !defined(AFX_STRMESSAGELIST_H__2321E060_528F_11D3_9065_444553540001__INCLUDED_)
