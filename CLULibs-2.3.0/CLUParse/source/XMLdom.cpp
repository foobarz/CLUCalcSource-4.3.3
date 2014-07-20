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
#include "CLUDebug.h"
#include "XMLdom.h"



////////////////////////////////////////////////////////////
 

CXMLdom::CXMLdom(void)
{
	m_pDoc = 0;
	m_pRootNode = 0;
}

CXMLdom::CXMLdom(const CXMLdom& rXMLdom)
{
	m_pDoc = 0;
	m_pRootNode = 0;
	*this = rXMLdom;
}


CXMLdom& CXMLdom::operator= (const CXMLdom& rXMLdom)
{
	if (m_pDoc)
		xmlFreeDoc(m_pDoc);

	m_pDoc = rXMLdom.m_pDoc;
	m_pRootNode = rXMLdom.m_pRootNode;
	m_stackNodePtr = rXMLdom.m_stackNodePtr;

	return *this;
}

void CXMLdom::Close()
{
	if (m_pDoc)
		xmlFreeDoc(m_pDoc);
	m_pDoc = 0;
	m_pRootNode = 0;
}

CXMLdom::~CXMLdom(void)
{
	Close();
}


void CXMLdom::GiveUpDoc()
{
	m_pDoc = 0;
	m_pRootNode = 0;
	while (m_stackNodePtr.size() > 0)
		m_stackNodePtr.pop();
}


////////////////////////////////////////////////////
/// New XML document

void CXMLdom::NewDoc(const char* pcRootNode)
	throw (CCLUException)
{
	if (m_pDoc)
		xmlFreeDoc(m_pDoc);

	if (!(m_pDoc = xmlNewDoc((xmlChar *) "1.0")))
	{
		throw ParseError("New XML document.");
	}

	xmlNodePtr pNode, pNewNode;
	if (!(pNewNode = xmlNewDocNode(m_pDoc, 0, (xmlChar *) pcRootNode, 0)))
	{
		throw CCLUError("Cannot create new node.");
	}


	if (!(pNode = xmlDocSetRootElement(m_pDoc, pNewNode)))
	{
		//throw EmptyDocument("New XML document.");
		//cout << "Problem\n";
	}

	if (!(pNode = xmlDocGetRootElement(m_pDoc)))
	{
		throw EmptyDocument("new");
	}

	m_pRootNode = pNode;

	while(!m_stackNodePtr.empty()) 
		m_stackNodePtr.pop();

	m_stackNodePtr.push(pNode);
}



////////////////////////////////////////////////////
/// Parse XML document

void CXMLdom::ParseFile(const char *pcFilename)
	throw (CCLUException)
{
	if (m_pDoc)
		xmlFreeDoc(m_pDoc);

	if (!(m_pDoc = xmlParseFile(pcFilename)))
	{
		throw ParseError(pcFilename);
	}

	xmlNodePtr pNode;
	if (!(pNode = xmlDocGetRootElement(m_pDoc)))
	{
		throw EmptyDocument(pcFilename);
	}

	m_pRootNode = pNode;

	while(!m_stackNodePtr.empty()) 
		m_stackNodePtr.pop();

	m_stackNodePtr.push(pNode);
}


////////////////////////////////////////////////////
/// Parse XML from memory

void CXMLdom::ParseMemory(const char *pcMemory, int iSize)
	throw (CCLUException)
{
	if (m_pDoc)
		xmlFreeDoc(m_pDoc);

	if (!(m_pDoc = xmlParseMemory(pcMemory, iSize)))
	{
		throw ParseError("Memory");
	}

	xmlNodePtr pNode;
	if (!(pNode = xmlDocGetRootElement(m_pDoc)))
	{
		throw EmptyDocument("Memory");
	}

	m_pRootNode = pNode;

	while(!m_stackNodePtr.empty()) 
		m_stackNodePtr.pop();

	m_stackNodePtr.push(pNode);
}


////////////////////////////////////////////////////
/// Write XML document

void CXMLdom::WriteFile(const char *pcFilename, int iCompression)
	throw (CCLUException)
{
	if (!m_pDoc || !pcFilename)
		return;

	if (iCompression >= 0 && iCompression < 10)
		xmlSetDocCompressMode(m_pDoc, iCompression);
	else
		xmlSetDocCompressMode(m_pDoc, 0);

	xmlSaveFormatFile(pcFilename, m_pDoc, 1);
}


////////////////////////////////////////////////////
/// Goto Root Element

bool CXMLdom::GotoRootElement()
{
	if (!m_pRootNode)
		return false;

	while(!m_stackNodePtr.empty()) 
		m_stackNodePtr.pop();

	m_stackNodePtr.push(m_pRootNode);

	return true;
}


////////////////////////////////////////////////////
/// Push Element Ptr

bool CXMLdom::Push()
{
	xmlNodePtr pNode = m_stackNodePtr.top();

	if (!pNode)
		return false;

	m_stackNodePtr.push(pNode);
	return true;
}

////////////////////////////////////////////////////
/// Pop Element Ptr

bool CXMLdom::Pop()
{
	if (m_stackNodePtr.size() == 0)
		return false;

	m_stackNodePtr.pop();
	return true;
}

////////////////////////////////////////////////////
/// Enter Element

bool CXMLdom::Enter()
{
	xmlNodePtr pNode = m_stackNodePtr.top();

	if (!pNode)
		return false;

	if (!(pNode = pNode->xmlChildrenNode))
		return false;

	m_stackNodePtr.push(pNode);
	return true;
}

////////////////////////////////////////////////////
/// Next Element

bool CXMLdom::Next()
{
	xmlNodePtr pNode = m_stackNodePtr.top();

	if (!pNode)
		return false;

	if (!(pNode = pNode->next))
		return false;

	m_stackNodePtr.top() = pNode;
	return true;
}


////////////////////////////////////////////////////
/// Exit Element

bool CXMLdom::Return()
{
	if (m_stackNodePtr.size() == 1)
		return false;

	m_stackNodePtr.pop();
	return true;
}

////////////////////////////////////////////////////
/// Has Child Elements

bool CXMLdom::HasChildElements()
{
	xmlNodePtr pNode = m_stackNodePtr.top();

	if (!pNode)
		return false;

	// If an element has only one child this is a "text" element
	// which contains text. Otherwise it has child elements.
	if (!pNode->children || !pNode->children->next)
		return false;

	return true;
}

////////////////////////////////////////////////////
/// Get Name

bool CXMLdom::GetName(string& sName)
{
	xmlNodePtr pNode = m_stackNodePtr.top();

	if (!pNode)
		return false;

	//xmlChar* pContent = xmlNodeListGetString(m_pDoc, pNode, 1);

	//if (!pContent)
	//	return false;

	sName = (char *) pNode->name; //pContent;

	//xmlFree(pContent);

	return true;
}

////////////////////////////////////////////////////
/// Get Name

bool CXMLdom::GetContent(string& sName)
{
	xmlNodePtr pNode = m_stackNodePtr.top();

	sName = "";

	if (!pNode)
		return false;

	xmlChar* pContent = xmlNodeListGetString(m_pDoc, pNode->xmlChildrenNode, 1);

	if (!pContent)
		return false;

	sName = (char *) pContent;

	xmlFree(pContent);

	return true;
}


////////////////////////////////////////////////////
/// Get Property

bool CXMLdom::GetProperty(const char* pcPropName, string& sPropValue)
{
	xmlNodePtr pNode;
	
	pNode = m_stackNodePtr.top();

	if (!pNode)
		return false;

	xmlChar* pText = xmlGetProp(pNode, (const xmlChar *) pcPropName);

	if (!pText)
		return false;

	sPropValue = (char *) pText;

	xmlFree(pText);
	return true;
}

////////////////////////////////////////////////////
/// Has Property

bool CXMLdom::HasProperty(const char* pcPropName)
{
	xmlNodePtr pNode = m_stackNodePtr.top();

	if (!pNode)
		return false;

	if (!xmlHasProp(pNode, (const xmlChar*) pcPropName))
		return false;

	return true;
}



////////////////////////////////////////////////////
/// Set Name

bool CXMLdom::SetName(const string& sName)
{
	xmlNodePtr pNode = m_stackNodePtr.top();

	if (!pNode)
		return false;

	xmlNodeSetName(pNode, (const xmlChar *) sName.c_str());

	return true;
}

////////////////////////////////////////////////////
/// Set Content

bool CXMLdom::SetContent(const string& sName)
{
	xmlNodePtr pNode = m_stackNodePtr.top();

	if (!pNode)
		return false;

	xmlNodeSetContent(pNode, (const xmlChar *) sName.c_str());

	return true;
}


////////////////////////////////////////////////////
/// Set Property

bool CXMLdom::SetProperty(const char* pcPropName, const string& sPropValue)
{
	xmlNodePtr pNode;
	
	pNode = m_stackNodePtr.top();

	if (!pNode)
		return false;

	if (!HasProperty(pcPropName) && sPropValue.size() > 0)
	{
		xmlNewProp(pNode, (const xmlChar *) pcPropName, (const xmlChar *) sPropValue.c_str());
	}
	else
	{
		if (sPropValue.size() > 0)
		{
			xmlSetProp(pNode, (const xmlChar *) pcPropName, (const xmlChar *) sPropValue.c_str());
		}
		else
		{
			xmlUnsetProp(pNode, (const xmlChar *) pcPropName);
		}
	}

	return true;
}


////////////////////////////////////////////////////
/// New Node

bool CXMLdom::NewNode(const char *pcName)
{
	xmlNodePtr pNode = m_stackNodePtr.top();

	if (!pNode)
		return false;

	xmlNodePtr pNewNode;
	if (!(pNewNode = xmlNewNode(0, (const xmlChar *) pcName)))
		return false;

	if (!(pNode = xmlAddSibling(pNode, pNewNode)))
		return false;

	m_stackNodePtr.top() = pNode;

	return true;
}

////////////////////////////////////////////////////
/// New Child Node

bool CXMLdom::NewChildNode(const char *pcName)
{
	xmlNodePtr pNode = m_stackNodePtr.top();

	if (!pNode)
		return false;

	if (!xmlNewTextChild(pNode, 0, (const xmlChar *) pcName, 0))
		return false;

	return true;
}


////////////////////////////////////////////////////
/// New Element

bool CXMLdom::NewChildElement(const CElement& rEl)
{
	xmlNodePtr pNode = m_stackNodePtr.top();
	xmlNodePtr pNewNode;

	if (!pNode)
		return false;

	if (rEl.GetName().size() == 0)
		return false;

	if (!(pNewNode = xmlNewNode(0, (const xmlChar *) rEl.GetName().c_str())))
		return false;

	xmlAddChild(pNode, pNewNode);

	Push();
	m_stackNodePtr.top() = pNewNode;

	SetElement(rEl, true);

	Pop();
	return true;
}

////////////////////////////////////////////////////
/// New Element

bool CXMLdom::NewElement(const CElement& rEl)
{
	if (rEl.GetName().size() == 0)
		return false;

	if (!NewNode(rEl.GetName().c_str()))
		return false;

	SetElement(rEl, true);

	return true;
}


////////////////////////////////////////////////////
/// New Element

bool CXMLdom::NewChildElement(const CXMLElement& rEl)
{
	xmlNodePtr pNode = m_stackNodePtr.top();
	xmlNodePtr pNewNode;

	if (!pNode)
		return false;

	if (rEl.GetName().size() == 0)
		return false;

	if (!(pNewNode = xmlNewNode(0, (const xmlChar *) rEl.GetName().c_str())))
		return false;

	xmlAddChild(pNode, pNewNode);

	Push();
	m_stackNodePtr.top() = pNewNode;

	SetElement(rEl, true);

	Pop();
	return true;
}

////////////////////////////////////////////////////
/// New Element

bool CXMLdom::NewElement(const CXMLElement& rEl)
{
	if (rEl.GetName().size() == 0)
		return false;

	if (!NewNode(rEl.GetName().c_str()))
		return false;

	SetElement(rEl, true);

	return true;
}



////////////////////////////////////////////////////
/// Goto Tag

bool CXMLdom::GotoTag(const char* pcName)
{
	string sName;

	while (GetName(sName) && sName != pcName)
	{
		if (!Next())
			return false;
	}

	return true;
}


////////////////////////////////////////////////////
/// Goto Tag

bool CXMLdom::GotoTag(const CElement& rEl)
{
	string sName, sFind;

	sFind = rEl.GetName();
    while (GetName(sName) && sName != sFind)
	{
		if (!Next())
			return false;
	}

	return true;
}


////////////////////////////////////////////////////
/// Get Element

bool CXMLdom::GetElement(CElement& rEl)
{
	string sName;

	while (GetName(sName))
	{
		if (sName == rEl.sName)
		{
			// Found Element
			map<string,string>::iterator it_Prop, it_PropEnd;

			it_Prop = rEl.mapProp.begin();
			it_PropEnd = rEl.mapProp.end();

			for(;it_Prop != it_PropEnd; ++it_Prop)
			{
				GetProperty(it_Prop->first.c_str(), it_Prop->second);
			}

			GetContent(rEl.sContent);
			return true;
		}

		if (!Next())
			return false;
	}

	return false;
}


////////////////////////////////////////////////////
/// Get Element

bool CXMLdom::GetElementList(const CElement& elSearch, vector<CElement>& vecEl)
{
	CElement elTag;
	string sName;
	bool bFoundElement = false;

	// Save current node pointer
	Push();

	while (GetName(sName))
	{
		if (sName == elSearch.sName)
		{
			bFoundElement = true;

			// Found Element
			elTag = elSearch;
			map<string,string>::iterator it_Prop, it_PropEnd;

			it_Prop = elTag.mapProp.begin();
			it_PropEnd = elTag.mapProp.end();

			for(;it_Prop != it_PropEnd; ++it_Prop)
			{
				GetProperty(it_Prop->first.c_str(), it_Prop->second);
			}

			GetContent(elTag.sContent);

			vecEl.push_back(elTag);
		}

		if (!Next())
			break;
	}

	// Return to old node pointer
	Pop();

	return bFoundElement;
}


////////////////////////////////////////////////////
/// Set Element

bool CXMLdom::SetElement(const CElement& rEl, bool bSetContent)
{
	if (rEl.sName.size() == 0)
		return false;

	SetName(rEl.sName);

	map<string,string>::const_iterator it_Prop, it_PropEnd;

	it_Prop = rEl.mapProp.begin();
	it_PropEnd = rEl.mapProp.end();

	for(;it_Prop != it_PropEnd; ++it_Prop)
	{
		if (it_Prop->second.size() > 0)
		{
			SetProperty(it_Prop->first.c_str(), it_Prop->second);
		}
	}

	if (rEl.sContent.size() > 0 && bSetContent)
	{
		SetContent(rEl.sContent);
	}

	return true;
}


////////////////////////////////////////////////////
/// Set Element

bool CXMLdom::SetElement(const CXMLElement& rEl, bool bSetContent)
{
	if (rEl.GetName().size() == 0)
		return false;

	SetName(rEl.GetName());

	map<string,string>::const_iterator it_Prop, it_PropEnd;

	it_Prop = rEl.GetPropMap().begin();
	it_PropEnd = rEl.GetPropMap().end();

	for(;it_Prop != it_PropEnd; ++it_Prop)
	{
		if (it_Prop->second.size() > 0)
		{
			SetProperty(it_Prop->first.c_str(), it_Prop->second);
		}
	}

	if (rEl.GetContent().size() > 0 && bSetContent)
	{
		SetContent(rEl.GetContent());
	}

	return true;
}



////////////////////////////////////////////////////
/// Get Property List

bool CXMLdom::GetPropertyList(map<string, string>& mapProp)
{
	xmlNodePtr pNode;
	xmlAttrPtr pAttr;
	string sPropName;

	// Get current node pointer
	if (!(pNode = m_stackNodePtr.top()))
		return false;

	// Erase all elements from property map
	mapProp.clear();

	// Get pointer to properties
	if (!(pAttr = pNode->properties))
		// No properties for this element
		return true;

	// Fill property map
	do
	{
		sPropName = (const char *) pAttr->name;
		mapProp[sPropName] = (const char *) pAttr->children->content;
	} 
	while ((pAttr = pAttr->next) != 0);

	return true;
}


////////////////////////////////////////////////////
/// Get Element Tree

bool CXMLdom::GetElementTree(CXMLTree &rTree, bool bFromTop)
{
	string sElName, sElContent;
	map<string, string> mapProp;
	CXMLElement XMLEl;

	DEBUG(90, "CXMLdom::GetElementTree: Start\n");

	if (bFromTop)
	{
		DEBUG(90, "CXMLdom::GetElementTree:     Goto Root Node\n");

		// Goto Beginning of XML document
		if (!GotoRootElement())
			return false;
	}

	// Reset XML Tree instance
	rTree.clear();

	// Loop over all elements
	do
	{
		DEBUG(90, "CXMLdom::GetElementTree:     Start read element\n");

		// Reset XML Element
		XMLEl.Reset();
		sElContent = "";

		// Get name of current element
		if (!GetName(sElName))
			return false;

		// Ignore pseudo-elements with name "text".
		// Still have to figure out what these are good for.
		if (sElName == "text")
			continue;

		// Get List of properties of current element
		if (!GetPropertyList(mapProp))
			return false;

		// Get content of element
		if (!HasChildElements())
			GetContent(sElContent);

		// Initialize CXMLElement instance
		XMLEl.Set(sElName.c_str(), sElContent.c_str(), mapProp);

		DEBUG(90, "CXMLdom::GetElementTree:     Element read:\n" << XMLEl.Str() << "\n");

		// Try to enter element
		if (Enter())
		{
			DEBUG(90, "CXMLdom::GetElementTree:     Entering Element...\n");

			GetElementTree(XMLEl.GetSubTree(), false);
			Return();

			DEBUG(90, "CXMLdom::GetElementTree:     Return from element...\n");
		}

		// Add Element to Tree
		rTree.AddElement(XMLEl);
	}
	while (Next());

	DEBUG(90, "CXMLdom::GetElementTree: End\n");
	return true;
}


////////////////////////////////////////////////////
/// Set Element Tree

bool CXMLdom::SetElementTree(CXMLTree &rTree, bool bFromTop)
	throw (CCLUError)
{
	bool bFirst = true;
	CXMLElement elDummy;
	CXMLTree::iterator it_ElList;

	elDummy.SetName("Dummy");

	DEBUG(90, "CXMLdom::SetElementTree: Start\n");

	if (bFromTop)
	{
		DEBUG(90, "CXMLdom::SetElementTree:     New document...\n");
		NewDoc("Dummy");
	}

	// Loop over all Elements in Tree
	for (it_ElList = rTree.begin(); it_ElList != rTree.end(); ++it_ElList)
	{
		// Element iterator
		TXMLElementList::iterator it_El;

		// Get vector of Elements of current label
		TXMLElementList &vecEl = it_ElList->second;

		// Loop over all Elements
		for (it_El = vecEl.begin(); it_El != vecEl.end(); ++it_El)
		{
			// Are we setting the first element?
			if (bFirst)
			{
				DEBUG(90, "CXMLdom::SetElementTree:     Set element:\n" << it_El->Str() << "\n");

				// Goto to the dummy element which we have to replace.
				GotoTag(elDummy.GetName().c_str());

				// Set Element with content
				SetElement(*it_El, true);
				bFirst = false;
			}
			else
			{
				DEBUG(90, "CXMLdom::SetElementTree:     Set new element:\n" << it_El->Str() << "\n");

				// Add new element
				NewElement(*it_El);
			}

			// Check whether element contains sub-elements
			if (it_El->GetSubTree().size())
			{
				DEBUG(90, "CXMLdom::SetElementTree:     Enter sub-tree...\n");

				// Create Dummy Child Element
				if (!NewChildElement(elDummy))
				{
					throw CCLUError("Cannot create child element.");
				}

				// Enter Sub-Tree
				Enter();

				// Set Sub-Tree
				SetElementTree(it_El->GetSubTree(), false);

				// Return from sub-tree
				Return();

				DEBUG(90, "CXMLdom::SetElementTree:     Return from sub-tree...\n");
			}
		}

		DEBUG(90, "CXMLdom::SetElementTree: End.\n");
	}

	return true;
}
