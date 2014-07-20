// CodeBinaryOperator.cpp: Implementierung der Klasse CCodeBinaryOperator.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CodeBinaryOperator.h"
#include "CodeVar.h"
#include "CodeBase.h"
#include "CLUCodeBase.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCodeBinaryOperator::CCodeBinaryOperator()
{

}

CCodeBinaryOperator::~CCodeBinaryOperator()
{

}


//////////////////////////////////////////////////////////////////////
/// Apply
///
/// General implementation of Binary Operator.
/// Calls virtual function Evaluate for actual calculation.

bool CCodeBinaryOperator::Apply(CCodeBase* pCodeBase, SCodeData *pData)
{
	if (!pCodeBase)
		return false;

	int iLine, iPos;
	CCodeVar *pvL, *pvR;

	if (pData)
	{
		iLine = pData->iTextLine;
		iPos = pData->iTextPos;
	}

	CCLUCodeBase *pBase = dynamic_cast<CCLUCodeBase*>(pCodeBase);

	if (!pBase)
	{
		pCodeBase->m_ErrorList.Internal(iLine, iPos);
		return false;
	}

	if (!pCodeBase->Pop(pvR))
	{
		pCodeBase->m_ErrorList.NoRVal(iLine, iPos);
		return false;
	}
		
	if (!pCodeBase->Pop(pvL))
	{
		pCodeBase->m_ErrorList.NoLVal(iLine, iPos);
		return false;
	}

	if (pvL->IsProtected())
	{
		pCodeBase->m_ErrorList.InvalidLVal(*pvL, iLine, iPos);
		return false;
	}

	if (!pCodeBase->m_mTempVarList.Add(1))
	{
		pCodeBase->m_ErrorList.OutOfMemory(iLine, iPos);
		return false;
	}

	CCodeVar &rResVar = pCodeBase->m_mTempVarList.Last();

	if (!Evaluate(pBase, *pvL, *pvR, rResVar, iLine, iPos))
		return false; // Operator sets error

	if (!pCodeBase->Push(&rResVar))
	{
		pCodeBase->m_ErrorList.OutOfMemory(iLine, iPos);
		return false;
	}

	return true;
}

