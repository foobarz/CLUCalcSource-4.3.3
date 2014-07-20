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

// StrMessageList.cpp: Implementierung der Klasse StrMessageList.
//
//////////////////////////////////////////////////////////////////////

#include "StrMessageList.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

StrMessageList::StrMessageList()
{
	uStdMsgNo = 1;
}

StrMessageList::~StrMessageList()
{

}



//////////////////////////////////////////////////////////////////////
// AddMessage
//////////////////////////////////////////////////////////////////////

void StrMessageList::AddMessage(uint mno, const char *mdata)
{
	uint i = msgData.Count();

	if (msgData++)
	{
		msgData[i].no = mno;

		if (mdata) msgData[i].data = mdata;
	}
}


//////////////////////////////////////////////////////////////////////
// SetMessage
//////////////////////////////////////////////////////////////////////

void StrMessageList::SetMessage(uint mno, const char *mdata)
{
	uint i = msgList.Count();

	if (msgList++)
	{
		msgList[i].no = mno;
		if (!mdata) msgList[i].data = msgData[GetMessageIdx(mno)].data;
		else msgList[i].data = mdata;
	}
}

//////////////////////////////////////////////////////////////////////
// SetMessage const char*

void StrMessageList::SetMessage(const char *mdata)
{
	if (!mdata) return;

	uint i = msgList.Count();

	if (msgList++)
	{
		msgList[i].no = uStdMsgNo;
		msgList[i].data = mdata;
	}
}

//////////////////////////////////////////////////////////////////////
// SetMessage cStr&

void StrMessageList::SetMessage(cStr& mStr)
{
	uint i = msgList.Count();

	if (msgList++)
	{
		msgList[i].no = uStdMsgNo;
		msgList[i].data = mStr;
	}
}


//////////////////////////////////////////////////////////////////////
// SetMessage Matrix<double>&

void StrMessageList::SetMessage(Matrix<double>& mMat)
{
	uint i, n = msgList.Count();

	if (!msgList++) return;

	uint rows = mMat.Rows();
	cStr str;

	for(i=1;i<=rows;i++)
	{
		str << mMat.Str(i) << "\n";
	}

	msgList[n].no = uStdMsgNo;
	msgList[n].data = str;
}






