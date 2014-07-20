// CodeBinaryOperator.h: Schnittstelle für die Klasse CCodeBinaryOperator.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CODEBINARYOPERATOR_H__8C57040D_ADF2_4A03_9158_EB04418BB758__INCLUDED_)
#define AFX_CODEBINARYOPERATOR_H__8C57040D_ADF2_4A03_9158_EB04418BB758__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeElement.h"
#include "CLUCodeBase.h"
#include "CodeVar.h"

class CCodeBinaryOperator : public CCodeElement  
{
public:
	CCodeBinaryOperator();
	virtual ~CCodeBinaryOperator();

	virtual bool Apply(CCodeBase* pCodeBase, SCodeData *pData = 0);

	virtual bool Evaluate(CCLUCodeBase* pBase, CCodeVar& rLVar, CCodeVar& rRVar, 
						  CCodeVar& rResVar, int iLine, int iPos);
};

#endif // !defined(AFX_CODEBINARYOPERATOR_H__8C57040D_ADF2_4A03_9158_EB04418BB758__INCLUDED_)
