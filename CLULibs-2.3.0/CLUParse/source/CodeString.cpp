// CodeString.cpp: Implementierung der Klasse CCodeString.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CodeString.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCodeString::CCodeString()
{
	m_VarString.New(PDT_STRING, "Constant");
	m_VarString = "";
	SetName("<Empty>");
}

CCodeString::~CCodeString()
{

}
//////////////////////////////////////////////////////////////////////
// Apply

bool CCodeString::Apply(CCodeBase* pCodeBase, SCodeData *pData)
{
	if (!pCodeBase) 
		return false;

	int iLine, iPos;
	if (pData)
	{
		iLine = pData->iTextLine;
		iPos = pData->iTextPos;
	}

	if (!pCodeBase->Push(&m_VarString))
	{
		pCodeBase->m_ErrorList.Internal(iLine, iPos);
		return false;
	}

	return true;
}
