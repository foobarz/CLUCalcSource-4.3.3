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
// MessageList.cpp: Implementierung der Klasse MessageList.
//
//////////////////////////////////////////////////////////////////////

#include "MessageList.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Dmstruktion
//////////////////////////////////////////////////////////////////////

template<class CType>
MessageList<CType>::MessageList()
{

}

template<class CType>
MessageList<CType>::~MessageList()
{

}


//////////////////////////////////////////////////////////////////////
/// Operator =

template<class CType>
MessageList<CType>& MessageList<CType>::operator= (const MessageList<CType>& ml)
{
	msgList = ml.msgList;
	msgData = ml.msgData;

	return *this;
}


//////////////////////////////////////////////////////////////////////
/// Operator += : Joins two Message lists

template<class CType>
MessageList<CType>& MessageList<CType>::operator<< (const MessageList<CType>& ml)
{
	uint mno = ml.msgList.Count();
	uint no = msgList.Count();
	uint i,j;

	if (!(msgList += mno)) return *this;

	for(i=0,j=no;i<mno;i++,j++)
	{
		msgList[j] = ml.msgList[i];
	}

	return *this;
}



//////////////////////////////////////////////////////////////////////
// HasMessage
//////////////////////////////////////////////////////////////////////

template<class CType>
uint MessageList<CType>::HasMessage(CType *mdata)
{
	uint i;

	if ((i = msgList.Count()))
	{
		if (mdata) *mdata = msgList[i-1].data;

		return msgList[i-1].no;
	}

	return 0;
}


//////////////////////////////////////////////////////////////////////
// GetLastMessage
//////////////////////////////////////////////////////////////////////

template<class CType>
uint MessageList<CType>::GetLastMessage(CType *mdata)
{
	uint i, mno;

	if ((i = msgList.Count()))
	{
		if (mdata) *mdata = msgList[i-1].data;
		mno =  msgList[i-1].no;
		msgList--;

		return mno;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////
// GetFirstMessage
//////////////////////////////////////////////////////////////////////

template<class CType>
uint MessageList<CType>::GetFirstMessage(CType *mdata)
{
	uint mno;

	if (msgList.Count())
	{
		if (mdata) *mdata = msgList[0].data;
		mno =  msgList[0].no;
		msgList.Del(0);

		return mno;
	}

	return 0;
}



//////////////////////////////////////////////////////////////////////
// AddMessage
//////////////////////////////////////////////////////////////////////

template<class CType>
void MessageList<CType>::AddMessage(uint mno, CType *mdata)
{
	uint i = msgData.Count();

	if (msgData++)
	{
		msgData[i].no = mno;

		if (mdata) msgData[i].data = *mdata;
	}
}


//////////////////////////////////////////////////////////////////////
// SetMessage
//////////////////////////////////////////////////////////////////////

template<class CType>
void MessageList<CType>::SetMessage(uint mno, CType *mdata)
{
	uint i = msgList.Count();

	if (msgList++)
	{
		msgList[i].no = mno;
		if (!mdata) msgList[i].data = msgData[GetMessageIdx(mno)].data;
		else msgList[i].data = *mdata;
	}
}



//////////////////////////////////////////////////////////////////////
// GetMessageIdx
//////////////////////////////////////////////////////////////////////

template<class CType>
uint MessageList<CType>::GetMessageIdx(uint mno)
{
	uint n = msgData.Count();

	for(uint i=0;i<n;i++)
	{
		if (msgData[i].no == mno)
			return i;
	}

	return 0;
}
