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



// Notify.h: Schnittstelle für die Klasse CNotify.
//
// Class to collect notifications from program routines
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NOTIFY_H__87B4B452_A5ED_4CB5_803E_34CB9056F45C__INCLUDED_)
#define AFX_NOTIFY_H__87B4B452_A5ED_4CB5_803E_34CB9056F45C__INCLUDED_

#include "StrMessageList.h"


enum eNotifyType
{
	NT_NOTIFY, NT_ERROR, NT_WARN0, NT_WARN1, NT_WARN2, NT_WARN3, 
	NT_TRACE0, NT_TRACE1, NT_TRACE2, NT_TRACE3, NT_TRACE4, NT_TRACE5
};


class CNotify : public StrMessageList  
{
public:
	CNotify();
	virtual ~CNotify();

	CNotify& operator= (const CNotify& sml)
	{ StrMessageList::operator = (StrMessageList(sml)); return *this; }

	CNotify& operator<< (const CNotify& sml)
	{ StrMessageList::operator << (StrMessageList(sml)); return *this; }


	void Notify(const char *psMsg) { SetMessage(NT_NOTIFY, psMsg); }
	void Error(const char *psMsg) { SetMessage(NT_ERROR, psMsg); }
	void Warn(const char *psMsg) { SetMessage(NT_WARN0, psMsg); }
	void Warn(int i, const char *psMsg) 
	{ if (i < 0) i=0; else if (i > 3) i=3; SetMessage(NT_WARN0 + i, psMsg); }

	void Trace(const char *psMsg) { SetMessage(NT_TRACE0, psMsg); }
	void Trace(int iLevel, const char *psMsg) 
	{ if (iLevel < 0) iLevel=0; else if (iLevel > 5) iLevel=5; 
	  SetMessage(NT_TRACE0 + iLevel, psMsg); }

	cStr GetNotify() { cStr csMsg; GetNotify(csMsg); return csMsg; }
	void GetNotify(cStr &csMsg) { GetNotifyRange(NT_NOTIFY, NT_NOTIFY, csMsg); }

	cStr GetError() { cStr csMsg; GetError(csMsg); return csMsg; }
	void GetError(cStr &csMsg) { GetNotifyRange(NT_ERROR, NT_ERROR, csMsg); }

	cStr GetWarn(int iLevel) { cStr csMsg; GetWarn(iLevel, csMsg); return csMsg; }
	void GetWarn(int iLevel, cStr &csMsg) 
	{	if (iLevel < 0) iLevel=0; else if (iLevel>3) iLevel = 3;
		GetNotifyRange(NT_WARN0, eNotifyType(NT_WARN0+iLevel), csMsg); }

	cStr GetTrace(int iLevel) { cStr csMsg; GetWarn(iLevel, csMsg); return csMsg; }
	void GetTrace(int iLevel, cStr &csMsg) 
	{	if (iLevel < 0) iLevel=0; else if (iLevel>5) iLevel = 5;
		GetNotifyRange(NT_TRACE0, eNotifyType(NT_TRACE0+iLevel), csMsg); }

	void GetNotifyRange(eNotifyType ntLow, eNotifyType ntHigh, cStr &csMsg);
};

#endif // !defined(AFX_NOTIFY_H__87B4B452_A5ED_4CB5_803E_34CB9056F45C__INCLUDED_)
