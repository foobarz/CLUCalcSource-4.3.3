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


// CLUException.cpp: Implementierung der Klasse CCLUException.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <iostream>
#include "CLUException.h"

using std::cerr;

// Initialize static variable
int CCLUException::sm_iDebugLevel = 1;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion

CCLUException::CCLUException(int iLevel)
{
	m_iLevel = iLevel;
}

CCLUException::~CCLUException()
{

}


//////////////////////////////////////////////////////////////////////
/// Print errors in some user readable way.

string CCLUException::PrintError()
{
	stringstream zText;

	if (m_iLevel <= sm_iDebugLevel)
	{
		switch(m_iLevel)
		{
		case 0:
			zText << string("Fatal Error:");
			break;

		case 1:
			zText << string("Error:");
			break;

		case 2:
			zText << string("Warning:");
			break;

		default:
			zText << string("Debug ") << (m_iLevel-2) << string(":");
			break;
		}

		zText << m_sNumberText << string(" ") << m_sErrorText << string("\n");
	}

	return zText.str();
}

//////////////////////////////////////////////////////////////////////
/// Add given number to m_sNumberText

void CCLUException::AddNumberToText(uchar ucNumber)
{
	char pcNumber[10];

	sprintf(pcNumber, "%02x:", ucNumber);

	m_sNumberText += pcNumber;
}

