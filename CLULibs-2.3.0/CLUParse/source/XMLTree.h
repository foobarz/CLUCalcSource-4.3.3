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

#if !defined (_CLU_XML_TREE_H_INCLUDED_)
#define _CLU_XML_TREE_H_INCLUDED_

extern "C" {
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
}

#include <iostream>
#include <sstream>
#include <vector>
#include <stack>
#include <map>

#include "CLUException.h"

using namespace std;


class CXMLElement;

typedef vector<CXMLElement> TXMLElementList;

class CXMLTree : public map<string, TXMLElementList>
{
public:
	class ElementNotAvailable : public CCLUError
	{
	public:
		ElementNotAvailable(const char *pcName)
		{
			m_sErrorText = "Element '";
			m_sErrorText += pcName;
			m_sErrorText += "' is not available.";
		}
	};

public:

public:
	CXMLTree(void);
	~CXMLTree(void);

	// \brief Print tree.
	string Str();

	void AddElement(const CXMLElement& rTEl);
	TXMLElementList& GetElementList(const char* pcName) throw (ElementNotAvailable);
};


#endif
