// CodeCreateRefVarList.cpp: Implementierung der Klasse CCodeCreateRefVarList.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CodeCreateRefVarList.h"
#include "CodeBase.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCodeCreateRefVarList::CCodeCreateRefVarList()
{
	m_pCodeList = 0;
}

CCodeCreateRefVarList::~CCodeCreateRefVarList()
{

}


//////////////////////////////////////////////////////////////////////
/// Apply
///
/// Takes all vars from stack an places them into a var list onto the stack.

bool CCodeCreateRefVarList::Apply(CCodeBase* pCodeBase, SCodeData *pData)
{
	if (!pCodeBase)
		return false;

	int iLine, iPos;
	if (pData)
	{
		iLine = pData->iTextLine;
		iPos = pData->iTextPos;
	}


	CCodeVar *pVar;

	// create a temp. variable for the List
	if (!pCodeBase->m_mTempVarList.Add(1))
	{
		pCodeBase->m_ErrorList.OutOfMemory(iLine, iPos);
		return false;
	}	
	CCodeVar &rResVar = pCodeBase->m_mTempVarList.Last();

	rResVar.New(PDT_VARLIST, "Constant");
	TVarList &rVarList = *((TVarList *) rResVar.Val());

	rVarList.SetCodeList(m_pCodeList);

	while (pCodeBase->Pop(pVar))
	{
		if (! rVarList.Insert(0) )
		{
			pCodeBase->m_ErrorList.OutOfMemory(iLine, iPos);
			return false;
		}

		// Check whether variable from stack is actually a non-temporary
		// variable. If this is the case, then only store reference
		// to variable in list.
		if (pVar->Type() == PDT_PTR_VAR)
		{
			CCodeVar &rV = pVar->DereferenceVarPtr();
			if (rV.Type() == PDT_PTR_VAR)
			{
				rVarList[0].CopyInstance(rV);
			}
			else
			{
				rVarList[0].CopyInstance(*pVar);
			}
		}
		else if (pCodeBase->GetVar(pVar->Name().Str()).Type() == PDT_NOTYPE)
		{
			rVarList[0].CopyInstance(*pVar);
		}
		else
		{
			rVarList[0] = pVar;
			rVarList[0].Name() = "Unnamed";
		}
	}

	// Put var list on stack.
	if (!pCodeBase->Push(&rResVar))
	{
		pCodeBase->m_ErrorList.OutOfMemory(iLine, iPos);
		return false;
	}

	return true;
}

