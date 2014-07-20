// CodeBase.cpp: Implementierung der Klasse CCodeBase.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CodeBase.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCodeBase::CCodeBase()
{
	m_iLoopCountLimit = 100000;
	SetCurrentNamespace(NS_GLOBAL);
}

CCodeBase::~CCodeBase()
{

}


//////////////////////////////////////////////////////////////////////
// Returns Variable of type PDT_NOTYPE if error occured.
// Tries first to create variable in local variable list.
// if there is no such list, then variable is created in global list.

CCodeVar& CCodeBase::NewVar(const char *pcName, ECodeDataType _nType, const char* pcNamespace)
{ 
	if (pcNamespace && !strcmp(pcNamespace, NS_CURRENT))
	{
		if (m_csCurNamespace.Len() == 0)
			pcNamespace = 0;
		else
			pcNamespace = m_csCurNamespace.Str();
	}

	// Check whether local variable list exists
	TCodeVarListPtr pLocalList = GetStackLocalVarList(0);

	if (!pcNamespace || !strcmp(pcNamespace, NS_LOCAL))
	{
		if (pLocalList)
		{
			return (*pLocalList)[pLocalList->New(pcName, _nType)];
		}
		else
		{
			return m_mVarList[m_mVarList.New(pcName, _nType)]; 
		}
	}
	else // if (!strcmp(pcNamespace, NS_GLOBAL))
	{
		return m_mVarList[m_mVarList.New(pcName, _nType)]; 
	}
} 

//////////////////////////////////////////////////////////////////////

bool CCodeBase::DeleteVar(const char *pcName, const char* pcNamespace) 
{ 
	if (pcNamespace && !strcmp(pcNamespace, NS_CURRENT))
	{
		if (m_csCurNamespace.Len() == 0)
			pcNamespace = 0;
		else
			pcNamespace = m_csCurNamespace.Str();
	}

	// Check whether local variable list exists
	TCodeVarListPtr pLocalList = GetStackLocalVarList(0);

	if (!pcNamespace || !strcmp(pcNamespace, NS_LOCAL))
	{
		if (pLocalList)
		{
			return pLocalList->Delete(pcName);
		}
		else
		{
			return m_mVarList.Delete(pcName); 
		}
	}
	else
	{
		return m_mVarList.Delete(pcName); 
	}
}


//////////////////////////////////////////////////////////////////////
// Returns Variable of type PDT_NOTYPE if error occured.
// Searches first in LocalVarList then in ConstVarList and then in VarList

CCodeVar& CCodeBase::GetVar(const char* pcName, const char* pcNamespace)
{
	TCodeVarListPtr pLocalVar;
	//int i, iCount = m_mLocalVarStack.Count();

	if (pcNamespace && !strcmp(pcNamespace, NS_CURRENT))
	{
		if (m_csCurNamespace.Len() == 0)
			pcNamespace = 0;
		else
			pcNamespace = m_csCurNamespace.Str();
	}

	CCodeVar& rVar = m_mConstVarList.GetVar(pcName); 
	if (rVar.Type() != PDT_NOTYPE)
		return rVar;

	if (!pcNamespace || !strcmp(pcNamespace, NS_LOCAL))
	{
		if ((pLocalVar = GetStackLocalVarList(0)) != 0)
		{
			CCodeVar &rLVar = pLocalVar->GetVar(pcName);

			if (rLVar.Type() != PDT_NOTYPE)
			{
				return rLVar;
			}
		}
		else
		{
			return m_mVarList.GetVar(pcName);
		}
	}
	
	if (!pcNamespace || !strcmp(pcNamespace, NS_GLOBAL))
	{
		return m_mVarList.GetVar(pcName);
	}

	return rVar; 
}

///////////////////////////////////////////////////////////////
/// Set Currentnamespace

void CCodeBase::SetCurrentNamespace(const char* pcNamespace)
{
	if (!pcNamespace)
		m_csCurNamespace = "";
	else
		m_csCurNamespace = pcNamespace;
}

