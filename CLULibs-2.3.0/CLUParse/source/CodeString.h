// CodeString.h: Schnittstelle für die Klasse CCodeString.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CODESTRING_H__9AEFFBC1_0536_11D6_BA34_00E07D8AFD6A__INCLUDED_)
#define AFX_CODESTRING_H__9AEFFBC1_0536_11D6_BA34_00E07D8AFD6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeElement.h"
#include "CodeVar.h"
#include "CodeBase.h"

class CCodeString : public CCodeElement  
{
public:
	CCodeString();
	virtual ~CCodeString();
	CCodeVar& Ref() { return m_VarString; }
	virtual bool Apply(CCodeBase* pCodeBase, SCodeData *pData = 0);
protected:
	CCodeVar m_VarString;
};

#endif // !defined(AFX_CODESTRING_H__9AEFFBC1_0536_11D6_BA34_00E07D8AFD6A__INCLUDED_)
