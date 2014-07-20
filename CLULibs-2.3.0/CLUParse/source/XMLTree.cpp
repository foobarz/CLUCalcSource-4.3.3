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

#include "StdAfx.h"
#include "XMLTree.h"
#include "XMLElement.h"



CXMLTree::CXMLTree(void)
{
}

CXMLTree::~CXMLTree(void)
{
}


void CXMLTree::AddElement(const CXMLElement& rTEl)
{
	iterator it_TEl;

	if ((it_TEl = find(rTEl.GetName())) != end())
	{
		TXMLElementList &rvecTEl =  it_TEl->second;
		rvecTEl.push_back(rTEl);
	}
	else
	{
		TXMLElementList vecTEl;

		vecTEl.push_back(rTEl);
		(*this)[rTEl.GetName()] = vecTEl;
	}
}

 
TXMLElementList& CXMLTree::GetElementList(const char* pcName) throw (ElementNotAvailable) 
{
	iterator it_TEl;
	it_TEl = find(pcName);
	if (it_TEl != end())
	{
		return it_TEl->second;
	}

	throw ElementNotAvailable(pcName);
}

string CXMLTree::Str()
{
  stringstream szText;
  iterator it_TEl;
  for(it_TEl=begin();it_TEl!=end();++it_TEl)
  {
	TXMLElementList::iterator it_Element;
    for(it_Element=it_TEl->second.begin();it_Element!=it_TEl->second.end();++it_Element)
		{
				szText << it_Element->Str();
		}
	}
  return szText.str();
}

////////////////////////////////////////////////////////////
