// CodeElement.h: Schnittstelle für die Klasse CCodeElement.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CODEELEMENT_H__E6C5FE42_FABC_480E_AA2A_AEF202ED542C__INCLUDED_)
#define AFX_CODEELEMENT_H__E6C5FE42_FABC_480E_AA2A_AEF202ED542C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CCodeBase;
class CCodeElementList;


struct SCodeData
{
	int iTextLine;
	int iTextPos;
	CCodeElementList *pParentList;

	void Set(int iLine, int iPos, CCodeElementList *pParent)
	{ iTextLine = iLine; iTextPos = iPos; pParentList = pParent; }
};


class CCodeElement  
{
public:
	CCodeElement();
	virtual ~CCodeElement();

	// Executes Code. Needs to implement functionality of code element.
	virtual bool Apply(CCodeBase* pCodeBase, SCodeData *pData = 0) { return true; }

	cStr GetName() { return m_csName; }
	void SetName(const char* pcName) { m_csName = pcName; }

protected:
	cStr m_csName;
};

#endif // !defined(AFX_CODEELEMENT_H__E6C5FE42_FABC_480E_AA2A_AEF202ED542C__INCLUDED_)
