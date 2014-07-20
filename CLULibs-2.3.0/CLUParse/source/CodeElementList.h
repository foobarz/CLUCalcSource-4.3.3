// CodeElementList.h: Schnittstelle für die Klasse CCodeElementList.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CODEELEMENTLIST_H__5746B287_573C_4800_9DE1_B801F31D0EB4__INCLUDED_)
#define AFX_CODEELEMENTLIST_H__5746B287_573C_4800_9DE1_B801F31D0EB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeElement.h"

class CCodeBase;

struct SCodeElementPtr
{
	// if bOwner == true, then list owns this element 
	// pointed to by pElement. In this case Delete
	// calls delete pElement; (!)
	bool bOwner;

	// Text Line in which code element appears
	int iTextLine;

	// Text Position at which code element starts.
	int iTextPos;

	CCodeElement *pElement;
};

class CCodeElementList : public CCodeElement  
{
public:
	CCodeElementList();
	virtual ~CCodeElementList();

	int Count() { return (int) m_mElementList.Count(); }
	bool Reset() { return Delete(0, m_mElementList.Count()); }

	CCodeElementList& operator << (CCodeElement& rElement) { Add(rElement); return *this; }
	CCodeElement* operator[] (int iPos) { return m_mElementList[iPos].pElement; }

	// if bOwner == true then list can call delete on Element.
	bool Add(CCodeElement& rElement, bool bOwner = false,
			int iTextLine = -1, int iTextPos = -1) 
	{ return Insert(rElement, -1, bOwner, iTextLine, iTextPos); }

	bool Insert(CCodeElement& rElement, int iPos = -1, bool bOwner = false,
				int iTextLine = -1, int iTextPos = -1);

	// DeleteInSubLists gives levels of sub lists in which element is to be deleted.
	// All instances of an element are deleted.
	bool Delete(CCodeElement& rElement, int DeleteInSubLists = 0);
	bool Delete(cStr& rName, int DeleteInSubLists = 0);

	// Delete iNo elements starting from iPos
	bool Delete(int iPos, int iNo = 1);

	virtual bool Apply(CCodeBase* pCodeBase, SCodeData *pData = 0);

protected:

	Mem<SCodeElementPtr> m_mElementList;
};

#endif // !defined(AFX_OGLBASEELEMENTLIST_H__5746B287_573C_4800_9DE1_B801F31D0EB4__INCLUDED_)
