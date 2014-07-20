// CodeLabel.h: Schnittstelle für die Klasse CCodeLabel.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CODELABEL_H__C8137DD9_E080_487E_A460_ABB85EFD0EEB__INCLUDED_)
#define AFX_CODELABEL_H__C8137DD9_E080_487E_A460_ABB85EFD0EEB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeElement.h"

#include "CodeVar.h"


class CCodeLabel : public CCodeElement  
{
public:
	CCodeLabel();
	virtual ~CCodeLabel();

	bool SetLabel(const char *pcLabel)
	{ if (!pcLabel || *pcLabel == 0) return false; else SetName(pcLabel); return true; }

	virtual bool Apply(CCodeBase* pCodeBase, SCodeData *pData = 0);

	CCodeVar& RefStdVar() { return m_StdVar; }

protected:
	CCodeVar m_StdVar;
};

#endif // !defined(AFX_CODELABEL_H__C8137DD9_E080_487E_A460_ABB85EFD0EEB__INCLUDED_)
