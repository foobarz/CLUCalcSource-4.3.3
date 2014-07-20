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


#if !defined (_CLU_XML_DOM_H_INCLUDED_)
#define _CLU_XML_DOM_H_INCLUDED_

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

#include "XMLTree.h"
#include "XMLElement.h"

using namespace std;


class CXMLdom
{
public:
	class ParseError : public CCLUError
	{
	public:
		ParseError(const char *pcFilename)
		{
			m_sErrorText = "Error parsing XML-file '";
			m_sErrorText += pcFilename;
			m_sErrorText += "'.";
		}
	};

	class EmptyDocument : public CCLUError
	{
	public:
		EmptyDocument(const char *pcFilename)
		{
			m_sErrorText = "XML-file '";
			m_sErrorText += pcFilename;
			m_sErrorText += "' is empty.";
		}
	};

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
	class CElement
	{
	public:
		CElement(void) { }
		CElement(const CElement& El)
		{ *this = El; }

		CElement& operator= (const CElement& El)
		{
			sName = El.sName;
			sContent = El.sContent;
			mapProp = El.mapProp;
			return *this;
		}


		void SetName(const char* pcName)
		{ sName = pcName; }

		void AddProp(const char* pcProp)
		{ mapProp[pcProp] = ""; }

		void AddProp(const char* pcProp, const char* pcVal)
		{ mapProp[pcProp] = pcVal; }

		void AddProp(const char* pcProp, int iVal)
		{ char pcVal[30]; sprintf(pcVal, "%d", iVal); AddProp(pcProp, pcVal); }

		void AddProp(const char* pcProp, float fVal)
		{ char pcVal[30]; sprintf(pcVal, "%g", fVal); AddProp(pcProp, pcVal); }

		void AddProp(const char* pcProp, double dVal)
		{ char pcVal[30]; sprintf(pcVal, "%lg", dVal); AddProp(pcProp, pcVal); }

		void SetProp(const char* pcProp, int iVal)
		{ char pcVal[30]; sprintf(pcVal, "%d", iVal); (*this)[pcProp] = pcVal; }

		void SetProp(const char* pcProp, float fVal)
		{ char pcVal[30]; sprintf(pcVal, "%g", fVal); (*this)[pcProp] = pcVal; }

		void SetProp(const char* pcProp, double dVal)
		{ char pcVal[30]; sprintf(pcVal, "%lg", dVal); (*this)[pcProp] = pcVal; }

		string& operator[] (const char* pcProp)
		{ return mapProp[pcProp]; }

		string& GetName() 
		{ return sName; }

		const string& GetName() const
		{ return sName; }

		string& GetContent()
		{ return sContent; }

		const string& GetContent() const
		{ return sContent; }

		void SetContent(const string& sVal)
		{ sContent = sVal; }

		// \brief Set element data.
		void Set(const char* pcName, const vector<string>& vecProp)
		{
			sName = pcName;
			mapProp.clear();
			sContent = "";
			size_t uIdx, uSize = vecProp.size();
			for(uIdx = 0; uIdx < uSize; uIdx++)
			{
				mapProp[vecProp[uIdx]] = "";
			}
		}

		// \brief Print element.
		string Print()
		{
			stringstream szText;
			szText << sName;
			map<string,string>::iterator it_Prop, it_PropEnd;

			it_Prop = mapProp.begin();
			it_PropEnd = mapProp.end();

			for(;it_Prop != it_PropEnd; ++it_Prop)
			{
				szText << "  " << it_Prop->first << "=\"" << it_Prop->second << "\"";
			}
			//szText << ">";
			return szText.str();
		}


		/// \brief Name of Element
		string sName;

		/// \brief Names and values of properties.
		map<string, string> mapProp;

		/// \brief Content between element tags.
		string sContent;

	};


public:
	CXMLdom(void);
	CXMLdom(const CXMLdom& rXMLdom);
	~CXMLdom(void);
	void Close();
	CXMLdom& operator= (const CXMLdom& rXMLdom);

	/** \brief Simple sets its pointer to the doc to NULL.
		This is useful if another instance of this class
		already cares for the document.
	*/
	void GiveUpDoc();

	/** \brief New XML Doc. */
	void NewDoc(const char *pcRootName) throw (CCLUException);

	/** \brief Parse XML File. */
	void ParseFile(const char* pcFilename) throw (CCLUException);

	/** \brief Parse XML in memory. */
	void ParseMemory(const char* pcMemory, int iSize) throw (CCLUException);

	/** \brief Write XML File. */
	void WriteFile(const char* pcFilename, int iCompression = 0) throw (CCLUException);

	/** \brief Goto Root Node. */
	bool GotoRootElement();

	// Basic operations
	bool Enter();
	bool Next();
	bool Return();
	bool Push();
	bool Pop();

	bool HasChildElements();

	bool GetName(string& sName);
	bool GetContent(string& sName);
	bool GetProperty(const char* pcPropName, string& sPropValue);
	bool HasProperty(const char* pcPropName);
	bool GetPropertyList(map<string, string>& mapProp);

	bool SetName(const string& sName);
	bool SetContent(const string& sName);

	bool NewNode(const char* pcName);
	bool NewChildNode(const char* pcName);

	bool NewElement(const CElement& rEl);
	bool NewChildElement(const CElement& rEl);

	bool NewElement(const CXMLElement& rEl);
	bool NewChildElement(const CXMLElement& rEl);

	/** \brief Sets property. If the property name does not exist
		it is added. If the property value passed is empty the
		property is removed.
	**/
	bool SetProperty(const char* pcPropName, const string& sPropValue);

	/** \brief Go to next tag with given name. */
	bool GotoTag(const char* pcName);
	bool GotoTag(const CElement& rEl);

	// Advanced operations

	/** \brief Searches element with given name and reads all
				given properties and the text enclosed by
				the element.
				The global node pointer is NOT moved to the next element
				by this	operation. In this way you can still enter
				a node if the element is of interest.
				You need to fill the SElement structure before calling
				this function.
	*/
	bool GetElement(CElement& rEl);

	/** \brief Searches all elements with given name and reads all
				given properties and the text enclosed by
				the elements.
				The global node pointer is NOT moved to the next element
				by this	operation. 
				You need to fill the elSearch structure before calling
				this function.
	*/
	bool GetElementList(const CElement& elSearch, vector<CElement>& vecEl);

	/** \brief Set the name, properties and text of element at current node.
	**/
	bool SetElement(const CElement& rEl, bool bSetContent = false);

	/** \brief Set the name, properties and text of element at current node.
	**/
	bool SetElement(const CXMLElement& rEl, bool bSetContent = false);

	/** \brief Get the whole element tree.
	**/
	bool GetElementTree(CXMLTree &rTree, bool bFromTop = true);

	/** \brief Set a whole element tree.
		If you call this function with bFromTop == true it calls NewDoc to create
		a new document. Otherwise it sets the given XMLTree starting from the
		current node.
	**/
	bool SetElementTree(CXMLTree &rTree, bool bFromTop = true) throw (CCLUError);


protected:
	xmlDocPtr m_pDoc;
	xmlNodePtr m_pRootNode;
	stack<xmlNodePtr> m_stackNodePtr;
};

#endif
