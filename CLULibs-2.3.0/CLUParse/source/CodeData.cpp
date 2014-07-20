// CodeData.cpp: Implementierung der Klasse CCodeData.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CodeData.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCodeData::CCodeData()
{
	m_VarData.New(PDT_INT, "Constant");
	m_VarData = (int) 0;
	SetName("Unnamed");

}

CCodeData::~CCodeData()
{
	// If the data stored in m_VarData is the pointer to code,
	// then delete the memory reserved for the code.
	if (m_VarData.BaseType() == PDT_CODEPTR)
	{
		TCodePtr *pCode = m_VarData.GetCodePtrPtr();
		if (*pCode)
			delete (*pCode);
	}
}


//////////////////////////////////////////////////////////////////////
// Apply

bool CCodeData::Apply(CCodeBase* pCodeBase, SCodeData *pData)
{
	if (!pCodeBase) 
		return false;

	int iLine, iPos;
	if (pData)
	{
		iLine = pData->iTextLine;
		iPos = pData->iTextPos;
	}

	if (!pCodeBase->Push(&m_VarData))
	{
		pCodeBase->m_ErrorList.Internal(iLine, iPos);
		return false;
	}

	return true;
}

