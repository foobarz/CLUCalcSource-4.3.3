// CodeOperator.cpp: Implementierung der Klasse CCodeOperator.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CodeOperator.h"
#include "CodeCreateVarList.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCodeOperator::CCodeOperator()
{
	m_bAssign = false;
	m_bBinary = false;
	m_bUnary = false;

	m_bRecurseLists = true;

	m_UnaryFunc = 0; 
	m_BinaryFunc = 0; 
	m_AUnaryFunc = 0; 
	m_ABinaryFunc = 0;

}

CCodeOperator::~CCodeOperator()
{
	
}


//////////////////////////////////////////////////////////////////////
// Set function pointers

bool CCodeOperator::Init(TOpUnaryPtr UFunc, TOpBinaryPtr BFunc,
						 TOpAUnaryPtr AUFunc, TOpABinaryPtr ABFunc,
						 bool bRecurseLists)
{
	m_UnaryFunc = UFunc; 
	m_BinaryFunc = BFunc; 
	m_AUnaryFunc = AUFunc; 
	m_ABinaryFunc = ABFunc;

	m_bRecurseLists = bRecurseLists;
	
	if (m_UnaryFunc || m_AUnaryFunc)
		m_bUnary = true;

	if (m_BinaryFunc || m_ABinaryFunc)
		m_bBinary = true;

	if (m_AUnaryFunc || m_ABinaryFunc)
		m_bAssign = true;

	return true; 
}


//////////////////////////////////////////////////////////////////////
/// Apply
///
/// General implementation of Operator.
/// Calls virtual functions Evaluate for actual calculation.

bool CCodeOperator::Apply(CCodeBase* pCodeBase, SCodeData *pData)
{
	if (!pCodeBase)
		return false;
	
	bool bIsBinary = false;
	int iLine, iPos;
	CCodeVar *pvL = NULL, *pvR = NULL;
	
	if (pData)
	{
		iLine = pData->iTextLine;
		iPos = pData->iTextPos;
	}
	
	if (!pCodeBase->Pop(pvR))
	{
		pCodeBase->m_ErrorList.NoRVal(iLine, iPos);
		return false;
	}
	
	if (m_bBinary)
	{
		if (!pCodeBase->Pop(pvL))
		{
			if (!m_bUnary)
			{
				pCodeBase->m_ErrorList.NoLVal(iLine, iPos);
				return false;
			}
			else bIsBinary = false;
		}
		else 
		{
			bIsBinary = true;
			
			if (m_bAssign && 
				pvL->IsProtected() && 
				!pvL->IsPtr() && 
				pvL->BaseType() != PDT_TENSOR_IDX &&
				pvL->BaseType() != PDT_VARLIST)
			{
				//pCodeBase->m_ErrorList.InvalidLVal(*pvL, iLine, iPos);
				pCodeBase->m_ErrorList.GeneralError("Cannot modify a constant L-value.", iLine, iPos);
				return false;
			}
		}
	}
	else
	{
		bIsBinary = false;
		
		if (m_bAssign)
		{
			if (pvR->IsProtected())
			{
				pCodeBase->m_ErrorList.InvalidRVal(*pvR, iLine, iPos);
				return false;
			}
		}
	}

	return Eval(pCodeBase, pData, *pvL, *pvR, bIsBinary);
}

//////////////////////////////////////////////////////////////////////
/// Eval
///
/// General implementation of Evaluate
/// Calls virtual functions Evaluate for actual calculation.
/// and recursivley handles lists

bool CCodeOperator::Eval(CCodeBase* pBase, SCodeData *pData, 
						 CCodeVar &rvL, CCodeVar &rvR, bool isBinary) 
{
	int iLine, iPos;
	if (pData)
	{
		iLine = pData->iTextLine;
		iPos = pData->iTextPos;
	}	
	
	if (m_bRecurseLists)
	{
		CCodeCreateVarList cCVList;
		if (&rvL) 
		{
			CCodeVar &rLVar = rvL.DereferenceVarPtr(true);

			if (rLVar.Type()== PDT_VARLIST)
			{
				TVarList *pmVars =  (TVarList *) rLVar.Val();
				
				//call eval for each element
				CCodeVar *pNewL;
				int n = pmVars->Count();

				// If list contains only 1 element, do not create new list
/*
				if (n == 1)
				{
					if (!Eval(pBase, pData, (*pmVars)[0], rvR, isBinary))
						return false;
				}
				else
				{

				// Always Create List
*/
					if (pBase->LockStack() < 0)
						return false;
					
					for (int i=0;i<n;i++) 
					{
						pNewL = &((*pmVars)[i]);					
						if (!Eval(pBase, pData, *pNewL, rvR, isBinary))
							return false;
					}
					cCVList.Apply(pBase, pData);			
					pBase->UnlockStack();
//				}

				return true;
			} 
		}
		if (&rvR) 
		{
			CCodeVar &rRVar = rvR.DereferenceVarPtr(true);

			if (rRVar.Type()== PDT_VARLIST)
			{
				TVarList *pmVars = (TVarList *) rRVar.Val();
				//call eval for each element
				CCodeVar *pNewR;
				int n = (*pmVars).Count();		
				
				// If list contains only 1 element, do not create new list
/*
				if (n == 1)
				{
					if (!Eval(pBase, pData, rvL, (*pmVars)[0], isBinary))
						return false;
				}
				else
				{

				// Always create a list
*/
					if (pBase->LockStack() < 0)
						return false;
					
					for (int i=0;i<n;i++) 
					{
						pNewR = &((*pmVars)[i]);					
						if (!Eval(pBase, pData, rvL, *pNewR, isBinary))
							return false;
					}
					cCVList.Apply(pBase, pData);
					pBase->UnlockStack();
//				}
				return true;
			}	
		}		
	}

	// no list, calculate operator result, 	
	if (m_bAssign)
	{
		if (isBinary)
		{			 			 
			if (!(pBase->*m_ABinaryFunc)(rvL, rvR, iLine, iPos))
				return false; // Operator sets error
		
			//rvL.ConvertSELtoSE();
			if (!pBase->Push(&rvL))
			{
				pBase->m_ErrorList.OutOfMemory(iLine, iPos);
				return false;
			}
		}
		else
		{
			if (!(pBase->*m_AUnaryFunc)(rvR, iLine, iPos))
				return false; // Operator sets error
			
			//rvR.ConvertSELtoSE();
			if (!pBase->Push(&rvR))
			{
				pBase->m_ErrorList.OutOfMemory(iLine, iPos);
				return false;
			}
		}
	}
	else // Create temporary variable
	{
		if (!pBase->m_mTempVarList.Add(1))
		{
			pBase->m_ErrorList.OutOfMemory(iLine, iPos);
			return false;
		}	
		
		CCodeVar &rResVar = pBase->m_mTempVarList.Last();
		rResVar.Name() = "Constant";
		
		if (isBinary)
		{
			// Since this is no assignment operator dereference
			// left and right variables.
			//rvL.DereferencePtr();
			//rvR.DereferencePtr();

			if (!(pBase->*m_BinaryFunc)(rvL, rvR, 
										rResVar, iLine, iPos))
				return false; // Operator sets error
		}
		else
		{
			// Since this is no assignment operator dereference
			// right variable.
			//rvR.DereferencePtr();

			if (!(pBase->*m_UnaryFunc)(rvR, rResVar, iLine, iPos))
				return false; // Operator sets error
		}
		
		// Temporary variable should not be changed.
		if (rResVar.Type()!= PDT_NOTYPE) 
		{
			//rResVar.ConvertSELtoSE();
			rResVar.EnableProtect(); // only if not list

			if (!pBase->Push(&rResVar))
			{
				pBase->m_ErrorList.OutOfMemory(iLine, iPos);
				return false;
			}
		}			
	} // else Create temporay variable

	return true;
}


