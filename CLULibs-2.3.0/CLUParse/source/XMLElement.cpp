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

#include "XMLElement.h"


/////////////////////////////////////////////////////////////
/// CXMLElement functions

void CXMLElement::Set(const char* pcName, const char* pcContent, 
					  const map<string, string>& mapProp)
{
	m_sName = pcName;
	m_sContent = pcContent;
	m_mapProp = mapProp;
}


string& CXMLElement::GetProp(const char* pcName) throw (PropertyNotAvailable)
{
	map<string, string>::iterator it_Prop;

	if ((it_Prop = m_mapProp.find(pcName)) != m_mapProp.end())
	{
		return it_Prop->second;
	}

	throw PropertyNotAvailable(pcName);
}

const string& CXMLElement::GetProp(const char* pcName) const throw (PropertyNotAvailable)
{
	map<string, string>::const_iterator it_Prop;

	if ((it_Prop = m_mapProp.find(pcName)) != m_mapProp.end())
	{
		return it_Prop->second;
	}

	throw PropertyNotAvailable(pcName);
}



string CXMLElement::Str()
{
	stringstream szText;
	szText << "<" << m_sName;
	map<string,string>::iterator it_Prop, it_PropEnd;

	it_Prop = m_mapProp.begin();
	it_PropEnd = m_mapProp.end();

	for(;it_Prop != it_PropEnd; ++it_Prop)
	{
		szText << "  " << it_Prop->first << "=\"" << it_Prop->second << "\"";
	}
	if(m_mapSubTree.size()==0)
	  szText << "/>\n";
    else
	  szText << ">\n";
 	
	szText << m_mapSubTree.Str();

    if(m_mapSubTree.size()!=0)
	  szText << "</" << m_sName << ">\n";
	return szText.str();
}



