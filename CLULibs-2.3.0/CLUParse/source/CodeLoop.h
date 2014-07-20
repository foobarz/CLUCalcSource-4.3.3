// CodeLabel.h: Schnittstelle für die Klasse CCodeLoop.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CODELOOP_H__C8137DD9_E080_487E_A460_ABB85EFD0EEB__INCLUDED_)
#define AFX_CODELOOP_H__C8137DD9_E080_487E_A460_ABB85EFD0EEB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeElement.h"


class CCodeLoop : public CCodeElement  
{
public:
	CCodeLoop();
	virtual ~CCodeLoop();

	virtual bool Apply(CCodeBase* pCodeBase, SCodeData *pData = 0);

};

#endif // !defined(AFX_CODEIF_H__C8137DD9_E080_487E_A460_ABB85EFD0EEB__INCLUDED_)
