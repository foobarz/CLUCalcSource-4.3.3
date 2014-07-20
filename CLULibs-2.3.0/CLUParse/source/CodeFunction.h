// CodeFunction.h: Schnittstelle für die Klasse CCodeFunction.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CODEFUNCTION_H__9C9569AC_1CA2_4F36_9812_43D67E5B164F__INCLUDED_)
#define AFX_CODEFUNCTION_H__9C9569AC_1CA2_4F36_9812_43D67E5B164F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeElement.h"
#include "CodeBase.h"
#include "CodeVar.h"

#ifdef _GNUCPP3_
#define STDFUNC(funcType, funcName) (static_cast<funcType>(&CCLUCodeBase::funcName)) 
#else
#define STDFUNC(funcType, funcName) ((funcType)(CCLUCodeBase::funcName)) 
#endif
typedef bool (CCodeBase::*TFuncPtr)(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

class CCodeFunction : public CCodeElement  
{
public:
	CCodeFunction();
	virtual ~CCodeFunction();

	void SetFunc(const char* pcName, TFuncPtr pFunc)
	{ SetName(pcName); m_pFunc = pFunc; }

	virtual bool Apply(CCodeBase* pCodeBase, SCodeData *pData = 0);

protected:
	TFuncPtr m_pFunc;
};

#endif // !defined(AFX_CODEFUNCTION_H__9C9569AC_1CA2_4F36_9812_43D67E5B164F__INCLUDED_)
