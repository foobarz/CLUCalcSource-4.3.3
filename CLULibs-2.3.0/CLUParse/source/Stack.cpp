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

// Stack.cpp: Implementierung der Klasse CStack.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Stack.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

template <class CType>
CStack<CType>::CStack()
{

}


template <class CType>
CStack<CType>::~CStack()
{

}


//////////////////////////////////////////////////////////////////////
// Get the number of entries in stack down to current bottom level.
// This is in general not the number of elements stored in MemObj base.

template <class CType>
int CStack<CType>::GetActStackDepth()
{
	int iC, iBotLevel;

	if (!m_mCurBotLevel.Count())
		iBotLevel = 0;
	else
		iBotLevel = m_mCurBotLevel.Last();

	iC = this->Count() - iBotLevel;
	if (iC <= 0)
		iC = 0;

	return iC;
}

//////////////////////////////////////////////////////////////////////
/// Pop Element from stack

template <class CType>
bool CStack<CType>::Pop(CType& rObj, bool bOverrideLock)
{
	int iC, iBotLevel;

	if (!m_mCurBotLevel.Count())
		iBotLevel = 0;
	else
		iBotLevel = m_mCurBotLevel.Last();

	if ((iC = this->Count()) <= 0 || 
		(!bOverrideLock && iC <= iBotLevel))
		return false;

	rObj = this->Last();

	this->Del(iC-1);

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Push Element onto stack

template <class CType>
bool CStack<CType>::Push(CType& rObj)
{
	if (!this->Add(1))
		return false;

	this->Last() = rObj;

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Lock Stack

template <class CType>
int CStack<CType>::LockStack()
{
	int iC = this->Count();
	
	if (iC == 0)// || iC == m_mCurBotLevel.Last())
		return iC;

	if (!m_mCurBotLevel.Add(1))
		return -1;

	m_mCurBotLevel.Last() = iC;

	return iC;
}


//////////////////////////////////////////////////////////////////////
/// Unlock Stack

template <class CType>
int CStack<CType>::UnlockStack()
{
	int iC = m_mCurBotLevel.Count();

	if (iC == 0)
		return 0;

	m_mCurBotLevel.Del(iC-1);

	return m_mCurBotLevel.Last();
}

