// VarList.h: Schnittstelle für die Klasse CVarList.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VARLIST_H__049E04F1_8531_48E2_89AA_1D6DCF05B309__INCLUDED_)
#define AFX_VARLIST_H__049E04F1_8531_48E2_89AA_1D6DCF05B309__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeElementList.h"
//#include "CodeVar.h"

class CCodeVar;

class CVarList : public MemObj<CCodeVar>  
{
public:
	CVarList();
	CVarList(const CVarList& rVarList);
	virtual ~CVarList();

	CVarList& operator= (const CVarList& rVarList);
	// Return reference to dereferences var ptr variable
	CCodeVar& operator() (int iPos);
//	{ return (*this)[iPos].DereferenceVarPtr(true); }

	void SetCodeList(CCodeElementList *pCodeList) { m_pCodeList = pCodeList; }
	CCodeElementList* GetCodeList() { return m_pCodeList; }

	// Dereference Pointer variables in list
	CVarList& DereferencePtr();

	bool Order(vector<int> &rIdxList);

protected:
	// m_pCodeList gives pointer to CodeElementList which
	// contains the code lines that created elements of
	// Variable List. This parameter does not need to be set.
	// It is zero if CodeElementList is not available.
	CCodeElementList *m_pCodeList;
};

#endif // !defined(AFX_VARLIST_H__049E04F1_8531_48E2_89AA_1D6DCF05B309__INCLUDED_)
