// CodeFunction.cpp: Implementierung der Klasse CCodeFunction.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CodeFunction.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCodeFunction::CCodeFunction()
{

}

CCodeFunction::~CCodeFunction()
{

}


//////////////////////////////////////////////////////////////////////
/// Apply
///
/// Standard function apply

bool CCodeFunction::Apply(CCodeBase* pCodeBase, SCodeData *pData)
{
	if (!pCodeBase)
		return false;

	int iLine, iPos;
	if (pData)
	{
		iLine = pData->iTextLine;
		iPos = pData->iTextPos;
	}

	if (!pCodeBase->m_mTempVarList.Add(1))
	{
		pCodeBase->m_ErrorList.OutOfMemory(iLine, iPos);
		return false;
	}

	CCodeVar &rVar = pCodeBase->m_mTempVarList.Last();
	CCodeVar *pVar;

	rVar.Name() = "Constant";

	if (!pCodeBase->Pop(pVar) ||
		pVar->Type() == PDT_NOTYPE)
	{
		pCodeBase->m_ErrorList.NoFuncParams(iLine, iPos);
		return false;
	}

	if (pVar->Type() != PDT_VARLIST)
	{
		pCodeBase->m_ErrorList.NoFuncParams(iLine, iPos);
		return false;
	}

	// Call Function
	if (! (pCodeBase->*m_pFunc)(rVar, *pVar, iLine, iPos))
		return false;

	if (rVar.Type() != PDT_NOTYPE) 
	{
		rVar.EnableProtect();
		if (!pCodeBase->Push(&rVar))
		{
			pCodeBase->m_ErrorList.OutOfMemory(iLine, iPos);
			return false;
		}
	}

	return true;
}

