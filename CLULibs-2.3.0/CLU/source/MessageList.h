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


// MessageList.h: Schnittstelle für die Klasse MessageList.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESSAGELIST_H__A1A03E01_5249_11D3_9065_00E098180C06__INCLUDED_)
#define AFX_MESSAGELIST_H__A1A03E01_5249_11D3_9065_00E098180C06__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "memobj.h"

template<class CType>
struct tMessageElement
{
	uint no;
	CType data;
};


template<class CType>
class MessageList  
{
public:
	MessageList();
	virtual ~MessageList();

	MessageList<CType>& operator= (const MessageList<CType>& ml);

	// Join two Message lists
	MessageList<CType>& operator<< (const MessageList<CType>& ml);

	// Returns last message in list
	uint HasMessage(CType *mdata = 0);

	// Return message on bottom of message list and removes it from the list.
	// Returns 0 if no message in message list.
	uint GetFirstMessage(CType *mdata = 0);

	// Return message on top of message list and removes it from the list.
	// Returns 0 if no message in message list.
	uint GetLastMessage(CType *mdata = 0);

	// Add message no and a text to msgData
	void AddMessage(uint mno, CType *mdata);

	// Set message with number and optionally some other data
	// than the one given in msgData
	void SetMessage(uint mno, CType *mdata = 0);

	// Get message from msgData
	CType& GetMessageData(uint mno)
	{ return msgData[GetMessageIdx(mno)].data; }

	// Reset Message List
	void ResetList() { msgList = 0; }

	// Reset Message Data
	void ResetData() { msgData = 0; }


protected:
	uint GetMessageIdx(uint mno);

protected:
	MemObj<tMessageElement<CType> > msgList, msgData;

};

#endif // !defined(AFX_MESSAGELIST_H__A1A03E01_5249_11D3_9065_00E098180C06__INCLUDED_)
