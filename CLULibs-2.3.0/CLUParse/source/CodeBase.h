// CodeBase.h: Schnittstelle für die Klasse CCodeBase.
//
//	This class forms the basis for running code.
//	It contains the variable/function List and the stack.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CODEBASE_H__85899394_3862_4967_B06C_A84E787CB1DE__INCLUDED_)
#define AFX_CODEBASE_H__85899394_3862_4967_B06C_A84E787CB1DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeVarList.h"
#include "Stack.h"
#include "CodeErrorList.h"

#define NS_CURRENT "current"
#define NS_LOCAL "local"
#define NS_GLOBAL "global"

typedef CCodeVar* TCodeVarPtr;
typedef CCodeVarList* TCodeVarListPtr;

class CCodeBase
{
public:
	CCodeBase();
	virtual ~CCodeBase();

	bool Reset() { ResetConstVarList(); ResetVarList(); ResetStack(); ResetTextOutput(); return true; }
	bool ResetConstVarList() { return m_mConstVarList.Reset(); }
	bool ResetVarList() { return m_mVarList.Reset(); }
	bool ResetStack() { return m_mVarStack.Reset(); }
	bool ResetTextOutput() { m_csOutput = ""; return true; }

	bool Push(CCodeVar* pVar) { return m_mVarStack.Push(pVar); }
	bool Pop(TCodeVarPtr& pVar) { return m_mVarStack.Pop(pVar); }

	int LockStack() { return m_mVarStack.LockStack(); }
	int UnlockStack() { return m_mVarStack.UnlockStack(); }

	bool PushLocal(TCodeVarListPtr pVarList) { return m_mLocalVarStack.Push(pVarList); }
	bool PopLocal(TCodeVarListPtr& pVarList) { return m_mLocalVarStack.Pop(pVarList); }
	TCodeVarListPtr GetStackLocalVarList(int i)
	{ int iC = m_mLocalVarStack.Count() - i - 1; if (iC < 0) return 0; else return m_mLocalVarStack[iC]; }

	// Returns Variable of type PDT_NOTYPE if error occured.
	// Creates variable in namespace. If pcNamespace == 0, the creates variable
	// in local if local exists. If not creates in global.
	CCodeVar& NewVar(const char *pcName, ECodeDataType _nType = PDT_INT, 
		const char* pcNamespace = 0);
	// Deletes variable using same principle as in NewVar.
	bool DeleteVar(const char *pcName, const char* pcNamespace = 0);

	// Returns Variable of type PDT_NOTYPE if error occured.
	CCodeVar& NewConstVar(const char *pcName, ECodeDataType _nType = PDT_INT)
	{ return m_mConstVarList[m_mConstVarList.New(pcName, _nType)]; } 

	bool DeleteConstVar(const char *pcName) { return m_mConstVarList.Delete(pcName); }

	// Returns Variable of type PDT_NOTYPE if error occured.
	// Searches in namespace but always first in ConstVarList.
	// If pcNamespace == 0, then searches in all namespaces
	CCodeVar& GetVar(const char* pcName, const char* pcNamespace = 0);
	
	CCodeVar& GetVar(int i) { return m_mVarList[i]; }
	CCodeVar& GetConstVar(int i) { return m_mConstVarList[i]; }

	// Returns i'th element on stack where 0 is element on top of stack.
	// Returns 0 if error occured.
	TCodeVarPtr GetStackVar(int i) 
	{ int iC = m_mVarStack.Count() - i - 1; if (iC < 0) return 0; else return m_mVarStack[iC]; }

	void SetCurrentNamespace(const char* pcNamespace);

	int ConstVarCount() { return m_mConstVarList.Count(); }
	int VarCount() { return m_mVarList.Count(); }
	int StackCount() { return m_mVarStack.Count(); }
	int ActStackCount() { return m_mVarStack.GetActStackDepth(); }

	void SetLoopCountLimit(int iLimit) { m_iLoopCountLimit = iLimit; }
	int GetLoopCountLimit() { return m_iLoopCountLimit; }

	cStr& GetTextOutput() { return m_csOutput; }

	CCodeErrorList m_ErrorList;
	MemObj<CCodeVar> m_mTempVarList;	

protected:
	CCodeVarList m_mConstVarList;	// Variable List for pre-defined constants
	CCodeVarList m_mVarList;		// Variable List for user variables.
	CStack<TCodeVarPtr> m_mVarStack;
	CStack<TCodeVarListPtr> m_mLocalVarStack; // Stack of pointer to lists of local variables

	cStr m_csOutput;
	cStr m_csCurNamespace; // Current namespace

	int m_iLoopCountLimit; // Maximum evaluations of a loop before error.

};

#endif // !defined(AFX_CODEBASE_H__85899394_3862_4967_B06C_A84E787CB1DE__INCLUDED_)
