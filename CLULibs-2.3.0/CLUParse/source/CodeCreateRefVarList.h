// CodeCreateVarList.h: Schnittstelle für die Klasse CCodeCreateVarList.
//
//	This class takes all vars from the stack and places them
//	in a VarList.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CODECREATEREFVARLIST_H__36B61796_A062_43BD_BFF1_C4903D56619D__INCLUDED_)
#define AFX_CODECREATEREFVARLIST_H__36B61796_A062_43BD_BFF1_C4903D56619D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeElement.h"
#include "CodeVar.h"

class CCodeCreateRefVarList : public CCodeElement  
{
public:
	CCodeCreateRefVarList();
	virtual ~CCodeCreateRefVarList();

	virtual bool Apply(CCodeBase* pCodeBase, SCodeData *pData = 0);

	void SetCodeList(CCodeElementList *pCodeList) { m_pCodeList = pCodeList; }
	CCodeElementList* GetCodeList() { return m_pCodeList; }

protected:
	CCodeElementList *m_pCodeList;
};

#endif // !defined(AFX_CODECREATEREFVARLIST_H__36B61796_A062_43BD_BFF1_C4903D56619D__INCLUDED_)
