// CodeVarList.h: Schnittstelle für die Klasse CCodeVarList.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CODEVARLIST_H__FD74F99C_5109_452F_B7C3_E4765984BA88__INCLUDED_)
#define AFX_CODEVARLIST_H__FD74F99C_5109_452F_B7C3_E4765984BA88__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeVar.h"

class CCodeVarList
{
public:
	CCodeVarList();
	virtual ~CCodeVarList();

	// Empty variable list.
	bool Reset() { return m_mVarList.Set(0); }

	// New creates new variable with given name and type
	// and returns position in List. return value -1 indicates error.
	int New(const char *pcName, ECodeDataType _nType = PDT_INT);
	bool Delete(const char *pcName);

	// If variable of given name does not exist, returns variable m_VarInvalid.
	// This variable is by default called "_INVALID_" and is of type PDT_NOTYPE.
	CCodeVar& GetVar(const char* pcName) 
	{ int i = GetPos(pcName); if (i < 0) return m_VarInvalid; else return m_mVarList[i]; }

	// Get Position of Var in List. Returns -1 if variable does not exist.
	int GetPos(const char* pcName);

	CCodeVar& operator[] (int i) 
	{ if (i < 0 || i >= (int) m_mVarList.Count()) return m_VarInvalid; else return m_mVarList[i]; }

	int Count() { return (int) m_mVarList.Count(); }

protected:
	MemObj<CCodeVar> m_mVarList;

	CCodeVar m_VarInvalid;
};

#endif // !defined(AFX_CODEVARLIST_H__FD74F99C_5109_452F_B7C3_E4765984BA88__INCLUDED_)
