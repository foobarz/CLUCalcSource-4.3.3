// CodeNumber.h: Schnittstelle für die Klasse CCodeNumber.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CODENUMBER_H__D52ED04E_4051_4BF7_9828_44C4BA89E2AD__INCLUDED_)
#define AFX_CODENUMBER_H__D52ED04E_4051_4BF7_9828_44C4BA89E2AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeElement.h"
#include "CodeVar.h"
#include "CodeBase.h"

class CCodeNumber : public CCodeElement  
{
public:
	CCodeNumber();
	virtual ~CCodeNumber();

	CCodeVar& RefNumber() { return m_VarNumber; }

	virtual bool Apply(CCodeBase* pCodeBase, SCodeData *pData = 0);

protected:
	CCodeVar m_VarNumber;

};

#endif // !defined(AFX_CODENUMBER_H__D52ED04E_4051_4BF7_9828_44C4BA89E2AD__INCLUDED_)
