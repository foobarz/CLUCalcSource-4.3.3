// CodeLabel.cpp: Implementierung der Klasse CCodeIf.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CodeIf.h"
#include "CodeBase.h"
#include "CodeVar.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCodeIf::CCodeIf()
{
}

CCodeIf::~CCodeIf()
{

}


//////////////////////////////////////////////////////////////////////
/// Apply

bool CCodeIf::Apply(CCodeBase* pCodeBase, SCodeData *pData)
{
	if (!pCodeBase)
		return false;

	int iTempVarCount, iCurTempVarCount;
	int iLine, iPos;
	if (pData)
	{
		iLine = pData->iTextLine;
		iPos = pData->iTextPos;
	}

	// Get Depth of stack up to current lock
	int iStackCount = pCodeBase->ActStackCount();

	TCVScalar dVal;
	TCodeVarPtr pVarTrueBranch, pVarFalseBranch, pVarCond;

/*
	for (int i = 0; i < iStackCount; i++)
	{
		TCodeVarPtr pVar = pCodeBase->GetStackVar(i);
		printf("%02d. %s (%s): %s\n", i, pVar->Name().Str(), pVar->TypeStr().Str(), 
					pVar->ValStr().Str());
	}
*/
	// Condition, True branch, False branch (optional), on stack
	if (iStackCount == 2 || iStackCount == 3)
	{
		// Get elements from stack
		if (iStackCount == 3)
		{
			if (!pCodeBase->Pop(pVarFalseBranch))
				return false;
		}
		else
		{
			pVarFalseBranch = 0;
		}

		if (!pCodeBase->Pop(pVarTrueBranch))
			return false;

		if (!pCodeBase->Pop(pVarCond))
			return false;

		if (pVarCond)
		{
			if (!pVarCond->CastToScalar(dVal))
			{
				pCodeBase->m_ErrorList.InvalidParType(*pVarCond, 1, iLine, iPos);
				return false;
			}
		}

		if (dVal)
		{
			// Is variable of type CodePtr then execute code
			if (pVarTrueBranch && pVarTrueBranch->Type() == PDT_CODEPTR)
			{
				// Execute code
				CCodeElementList *pCode = dynamic_cast<CCodeElementList*>
											(*((CCodeElement **) pVarTrueBranch->Val()));
				if (pCode)
				{
					int iCL, iCount = pCode->Count();
					CCodeVar *pDVar;

					// Lock Stack
					pCodeBase->LockStack();
					
					for(iCL=0;iCL<iCount;iCL++)
					{
						// Empty Stack up to lock
						// while(pCodeBase->Pop(pDVar));

						//iTempVarCount = pCodeBase->m_mTempVarList.Count();
						if (!(*pCode)[iCL]->Apply(pCodeBase, pData))
						{
							pCodeBase->UnlockStack();
							return false;
						}
						//iCurTempVarCount = pCodeBase->m_mTempVarList.Count();
						//pCodeBase->m_mTempVarList.Del(iTempVarCount, iCurTempVarCount-iTempVarCount);
					}

					// Empty stack up to lock
					while(pCodeBase->Pop(pDVar))
					{
						// If CODEPTR variable is left on stack
						// then execute it since it is a code block
						// in curly brackets.
						if (pDVar && pDVar->Type() == PDT_CODEPTR)
						{
							// Execute code
							CCodeElementList *pCode = dynamic_cast<CCodeElementList*>
								(*((CCodeElement **) pDVar->Val()));
							if (pCode)
							{
								int iCL, iCount = pCode->Count();
								CCodeVar *pDVar;

								// Lock Stack
								pCodeBase->LockStack();

								for(iCL=0;iCL<iCount;iCL++)
								{
									// Empty Stack up to lock
									while(pCodeBase->Pop(pDVar));

									iTempVarCount = pCodeBase->m_mTempVarList.Count();
									if (!(*pCode)[iCL]->Apply(pCodeBase, pData))
									{
										pCodeBase->UnlockStack();
										pCodeBase->UnlockStack();
										return false;
									}
									iCurTempVarCount = pCodeBase->m_mTempVarList.Count();
									pCodeBase->m_mTempVarList.Del(iTempVarCount, iCurTempVarCount-iTempVarCount);
								}
								// Empty stack up to lock
								while(pCodeBase->Pop(pDVar));

								// Unlock Stack
								pCodeBase->UnlockStack();
							}
						}
					} // while

					// Unlock Stack
					pCodeBase->UnlockStack();
				}
			}
		}
		else if (pVarFalseBranch)
		{
			// Is variable of type CodePtr then execute code
			if (pVarFalseBranch->Type() == PDT_CODEPTR)
			{
				// Execute code
				CCodeElementList *pCode = dynamic_cast<CCodeElementList*>
											(*((CCodeElement **) pVarFalseBranch->Val()));
				if (pCode)
				{
					int iCL, iCount = pCode->Count();
					CCodeVar *pDVar;

					// Lock Stack
					pCodeBase->LockStack();

					for(iCL=0;iCL<iCount;iCL++)
					{
						// Empty Stack up to lock
						// while(pCodeBase->Pop(pDVar));

						//iTempVarCount = pCodeBase->m_mTempVarList.Count();
						if (!(*pCode)[iCL]->Apply(pCodeBase, pData))
						{
							pCodeBase->UnlockStack();
							return false;
						}
						//iCurTempVarCount = pCodeBase->m_mTempVarList.Count();
						//pCodeBase->m_mTempVarList.Del(iTempVarCount, iCurTempVarCount-iTempVarCount);
					}
					// Empty stack up to lock
					while(pCodeBase->Pop(pDVar))
					{
						// If CODEPTR variable is left on stack
						// then execute it since it is a code block
						// in curly brackets.
						if (pDVar && pDVar->Type() == PDT_CODEPTR)
						{
							// Execute code
							CCodeElementList *pCode = dynamic_cast<CCodeElementList*>
								(*((CCodeElement **) pDVar->Val()));
							if (pCode)
							{
								int iCL, iCount = pCode->Count();
								CCodeVar *pDVar;

								// Lock Stack
								pCodeBase->LockStack();

								for(iCL=0;iCL<iCount;iCL++)
								{
									// Empty Stack up to lock
									while(pCodeBase->Pop(pDVar));

									iTempVarCount = pCodeBase->m_mTempVarList.Count();
									if (!(*pCode)[iCL]->Apply(pCodeBase, pData))
									{
										pCodeBase->UnlockStack();
										pCodeBase->UnlockStack();
										return false;
									}
									iCurTempVarCount = pCodeBase->m_mTempVarList.Count();
									pCodeBase->m_mTempVarList.Del(iTempVarCount, iCurTempVarCount-iTempVarCount);
								}
								// Empty stack up to lock
								while(pCodeBase->Pop(pDVar));

								// Unlock Stack
								pCodeBase->UnlockStack();
							}
						}
					} // while

					// Unlock Stack
					pCodeBase->UnlockStack();
				}
			}
		} // if dVal
	}
	else
	{
		// Error
		return false;
	}

	return true;
}

