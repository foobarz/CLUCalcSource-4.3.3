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

// Stack.h: Schnittstelle f�r die Klasse CStack.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STACK_H__29DD89BE_9839_4381_A23B_049B46BAD391__INCLUDED_)
#define AFX_STACK_H__29DD89BE_9839_4381_A23B_049B46BAD391__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



template <class CType>
class CStack : public MemObj<CType>
{
public:
	CStack();
	virtual ~CStack();

	bool Reset() { this->Set(0); m_mCurBotLevel.Set(0); return true;}

	// Get the number of entries in stack down to current bottom level.
	// This is in general not the number of elements stored in MemObj base.
	int GetActStackDepth();

	// If bOverrideLock == false, then current bottom level
	// is treated as bottom of stack. A call of Pop on bottom
	// level returns a false and no element!
	bool Pop(CType& rObj, bool bOverrideLock = false);
	bool Push(CType& rObj);

	// Make current level a new bottom level.
	// That is, lock stack at current level.
	// Calling lock twice on same level has no additional effect.
	// At each level a call of LockStack enters the current top level
	// into m_mCurBotLevel.
	// Returns current bottom level.
	int LockStack(); 

	// A call to unlock stack removes the top bottom level in m_mCurBotLevel.
	// Returns new bottom level.
	int UnlockStack();

protected:
	// Current bottom level. 
	// Can be used for sub-stacks.
	Mem<int> m_mCurBotLevel;

};

#endif // !defined(AFX_STACK_H__29DD89BE_9839_4381_A23B_049B46BAD391__INCLUDED_)
