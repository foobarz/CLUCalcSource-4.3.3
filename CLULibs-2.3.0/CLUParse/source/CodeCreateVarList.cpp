// CodeCreateVarList.cpp: Implementierung der Klasse CCodeCreateVarList.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CodeCreateVarList.h"
#include "CodeBase.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCodeCreateVarList::CCodeCreateVarList()
{
	m_pCodeList = 0;
}

CCodeCreateVarList::~CCodeCreateVarList()
{

}


//////////////////////////////////////////////////////////////////////
/// Apply
///
/// Takes all vars from stack an places them into a var list onto the stack.

bool CCodeCreateVarList::Apply(CCodeBase* pCodeBase, SCodeData *pData)
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

		rVarList[0].CopyInstance(pVar->DereferenceVarPtr(true));
		rVarList[0].DereferencePtr();
	}

	// Protect constant list of variables
	rResVar.EnableProtect();

	// Put var list on stack.
	if (!pCodeBase->Push(&rResVar))
	{
		pCodeBase->m_ErrorList.OutOfMemory(iLine, iPos);
		return false;
	}

	return true;
}

