// CodeData.h: Schnittstelle für die Klasse CCodeData.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CODEDATA_H__EB938ED0_DA28_4FE4_B2BD_7DDC3EB2914C__INCLUDED_)
#define AFX_CODEDATA_H__EB938ED0_DA28_4FE4_B2BD_7DDC3EB2914C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeElement.h"
#include "CodeVar.h"
#include "CodeBase.h"


class CCodeData : public CCodeElement  
{
public:
	CCodeData();
	virtual ~CCodeData();

	CCodeVar& RefData() { return m_VarData; }

	virtual bool Apply(CCodeBase* pCodeBase, SCodeData *pData = 0);

protected:
	CCodeVar m_VarData;


};

#endif // !defined(AFX_CODEDATA_H__EB938ED0_DA28_4FE4_B2BD_7DDC3EB2914C__INCLUDED_)
