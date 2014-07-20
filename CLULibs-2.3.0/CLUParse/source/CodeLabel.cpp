// CodeLabel.cpp: Implementierung der Klasse CCodeLabel.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CodeLabel.h"
#include "CodeBase.h"
#include "CodeVar.h"
#include "OCIDSymDef.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCodeLabel::CCodeLabel()
{
	m_StdVar.New(PDT_INT, "Unnamed");
	m_StdVar = (int) 0;
}

CCodeLabel::~CCodeLabel()
{

}


//////////////////////////////////////////////////////////////////////
/// Apply
/// If label refers to variable of type PDT_CODEPTR then calls Apply on Code Ptr.
/// otherwise it places the variable on the stack.
/// If label is unknown then variable is created with type PDT_INT.

bool CCodeLabel::Apply(CCodeBase* pCodeBase, SCodeData *pData)
{
	if (!pCodeBase)
		return false;

	int iLine, iPos;
	if (pData)
	{
		iLine = pData->iTextLine;
		iPos = pData->iTextPos;
	}

	const char* pcNamespace;
	cStr csVarName;
	if (m_csName[0] == OC_IDSYM_GLOBAL_CHAR)
	{
		csVarName = &(m_csName.Str()[1]);
		pcNamespace = NS_GLOBAL;
	}
	else
	{
		csVarName = m_csName;
		pcNamespace = NS_CURRENT;
	}

	CCodeVar& rVar = pCodeBase->GetVar(csVarName, pcNamespace);

	if (rVar.Type() == PDT_NOTYPE) // variable does not exist
	{
		CCodeVar& rNewVar = pCodeBase->NewVar(csVarName, PDT_INT, pcNamespace);
		if (rNewVar.Type() == PDT_NOTYPE)
		{
			pCodeBase->m_ErrorList.Internal(iLine, iPos);
			return false;
		}
	
		// Set Standard Variable
		rNewVar = m_StdVar;
		pCodeBase->Push(&rNewVar);
	}
/*	else if (rVar.Type() == PDT_CODEPTR) // execute code element
	{
		CCodeElement *pCE = (CCodeElement *) rVar.Val();

		if (!pCE)
		{
			pCodeBase->m_ErrorList.Internal(iLine, iPos);
			return false;
		}

		return pCE->Apply(pCodeBase);
	}
*/	else // Place var on stack
	{
		if (!pCodeBase->Push(&rVar))
		{
			pCodeBase->m_ErrorList.Internal(iLine, iPos);
			return false;
		}
	}

	return true;
}

