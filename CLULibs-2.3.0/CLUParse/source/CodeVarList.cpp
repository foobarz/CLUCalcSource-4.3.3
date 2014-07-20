// CodeVarList.cpp: Implementierung der Klasse CCodeVarList.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CodeVarList.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCodeVarList::CCodeVarList()
{
	m_VarInvalid.New(PDT_NOTYPE, "_INVALID_");
	m_VarInvalid.EnableProtect();
}

CCodeVarList::~CCodeVarList()
{

}



//////////////////////////////////////////////////////////////////////
// New creates new variable with given name and type
// and returns position in List. return value -1 indicates error.

int CCodeVarList::New(const char *pcName, ECodeDataType _nType)
{
	if (!pcName || *pcName == 0)
		return -1;

	if (!m_mVarList.Add(1))
		return -1;

	if (!m_mVarList.Last().New(_nType, pcName))
		return -1;

	return m_mVarList.Count() - 1;

}

//////////////////////////////////////////////////////////////////////
// Delete Variable from List

bool CCodeVarList::Delete(const char *pcName)
{
	int iPos = GetPos(pcName);

	if (iPos < 0) return false;

	return m_mVarList.Del(iPos); // Calls Delete on Variable in destructor!
}


//////////////////////////////////////////////////////////////////////
// Get Position of Var in List. Returns -1 if variable does not exist.
// Very simple implementation currently. Should be improved to increase
// speed of operation.

int CCodeVarList::GetPos(const char* pcName)
{
	if (!pcName || *pcName == 0)
		return -1;

	int i, iLen = m_mVarList.Count();
	
	for(i=0;i<iLen;i++)
	{
		if (m_mVarList[i].Name() == pcName)
			return i;
	}

	return -1;
}


