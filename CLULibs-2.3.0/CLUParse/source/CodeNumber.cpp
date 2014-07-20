// CodeNumber.cpp: Implementierung der Klasse CCodeNumber.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CodeNumber.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCodeNumber::CCodeNumber()
{
	m_VarNumber.New(PDT_INT, "Constant");
	m_VarNumber = (int) 0;
	SetName("0");
}

CCodeNumber::~CCodeNumber()
{

}


//////////////////////////////////////////////////////////////////////
// Apply

bool CCodeNumber::Apply(CCodeBase* pCodeBase, SCodeData *pData)
{
	if (!pCodeBase) 
		return false;

	int iLine, iPos;
	if (pData)
	{
		iLine = pData->iTextLine;
		iPos = pData->iTextPos;
	}

	if (!pCodeBase->Push(&m_VarNumber))
	{
		pCodeBase->m_ErrorList.Internal(iLine, iPos);
		return false;
	}

	return true;
}

