// CodeErrorList.h: Schnittstelle für die Klasse CCodeErrorList.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CODEERRORLIST_H__3BD5D0E1_5D4E_4674_B624_27E72E1BBEFD__INCLUDED_)
#define AFX_CODEERRORLIST_H__3BD5D0E1_5D4E_4674_B624_27E72E1BBEFD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ParseMessageList.h"
#include "CodeVar.h"

#define CERR_NO_ERR				0x0000	
#define CERR_NO_LVAL			0x0001
#define CERR_NO_RVAL			0x0002
#define CERR_INVALID_LVAL		0x0003
#define CERR_NO_FUNCPARAMS		0x0004
#define CERR_INVALID_TYPE		0x0005
#define CERR_WRONG_NP			0x0006	// Wrong Number of Parameters
#define CERR_INVALID_RVAL		0x0007
#define CERR_INVALID_PARTYPE	0x0008
#define CERR_NO_CODE			0x0009
#define CERR_INVALID_TYPE_DUAL	0x000A
#define CERR_INVALID_PARVAL		0x000B
#define CERR_INVALID_MV_TYPE	0x000C
#define CERR_MAX_LOOP_COUNT		0x000D

#define CERR_INTERNAL		0x1000
#define CERR_OUTOFMEMORY	0x1001
#define CERR_GENERAL		0x1002
#define CERR_MATH			0x1003

#define CERR_BREAK			0xF000

#define CEL_FATAL		1
#define CEL_ERROR		2
#define CEL_WARNING		3
#define CEL_INTERNAL	4

class CCodeErrorList : public CMessageList  
{
public:
	CCodeErrorList();
	virtual ~CCodeErrorList();

///////////////
/// Warnings

	
///////////////
/// Errors
	bool NoCode()
	{ return AddMsg("No Code Available", 0, 0, CERR_NO_CODE, CEL_ERROR); }

	bool NoLVal(int iLine = -1, int iPos = -1) 
	{ return AddMsg("No L-Value", iLine, iPos, CERR_NO_LVAL, CEL_ERROR); }

	bool NoRVal(int iLine = -1, int iPos = -1) 
	{ return AddMsg("No R-Value", iLine, iPos, CERR_NO_RVAL, CEL_ERROR); }

	bool NoFuncParams(int iLine = -1, int iPos = -1) 
	{ return AddMsg("No Function Parameters", iLine, iPos, CERR_NO_FUNCPARAMS, CEL_ERROR); }

	bool InvalidLVal(CCodeVar &rVar, int iLine = -1, int iPos = -1) 
	{ cStr csText = rVar.Name(); return AddMsg("'" + csText + "' of type '" + rVar.TypeStr() + "' is an invalid L-Value", iLine, iPos, CERR_INVALID_RVAL, CEL_ERROR); }

	bool InvalidRVal(CCodeVar &rVar, int iLine = -1, int iPos = -1) 
	{ cStr csText = rVar.Name(); return AddMsg("'" + csText + "' of type '" + rVar.TypeStr() + "' is an invalid R-Value", iLine, iPos, CERR_INVALID_RVAL, CEL_ERROR); }

	bool InvalidParType(CCodeVar& rVar, int iPar, int iLine = -1, int iPos = -1) 
	{ cStr csText = "Parameter "; return AddMsg(csText << iPar << ", '" + rVar.Name() + "' is of invalid type '" + rVar.TypeStr() + "'.", iLine, iPos, CERR_INVALID_PARTYPE, CEL_ERROR); }

	bool InvalidParVal(CCodeVar& rVar, int iPar, int iLine = -1, int iPos = -1) 
	{ cStr csText = "Parameter "; return AddMsg(csText << iPar << ", '" << rVar.Name() << "' has invalid value '" << rVar.ValStr() << "'.", iLine, iPos, CERR_INVALID_PARVAL, CEL_ERROR); }

	bool InvalidParVal(const char *pcVal, int iPar, int iLine = -1, int iPos = -1) 
	{ cStr csText = "Parameter "; return AddMsg(csText << iPar << ", has invalid value '" << pcVal << "'.", iLine, iPos, CERR_INVALID_PARVAL, CEL_ERROR); }

	bool InvalidType(CCodeVar& rVar, int iLine = -1, int iPos = -1) 
	{ cStr csText = rVar.Name(); return AddMsg("'" + csText + "' is of invalid type '" + rVar.TypeStr() + "'.", iLine, iPos, CERR_INVALID_TYPE, CEL_ERROR); }

	bool InvalidMVType(CCodeVar& rAVar, CCodeVar& rBVar, int iLine = -1, int iPos = -1) 
	{ cStr csText = rAVar.Name(); return AddMsg("Multivectors '" + csText + "' and '" + rBVar.Name() + "' are from different spaces.", iLine, iPos, CERR_INVALID_MV_TYPE, CEL_ERROR); }

	bool WrongNoOfParams(int iExpNP, int iLine = -1, int iPos = -1) 
	{ cStr csNP; csNP = iExpNP; return AddMsg("Expected " + csNP + " parameters.", iLine, iPos, CERR_INVALID_LVAL, CEL_ERROR); }

	bool WrongNoOfParams(int *piExpNP, int iNo, int iLine = -1, int iPos = -1) 
	{ cStr csNP; csNP = piExpNP[0]; 
	  for(int i=1;i<iNo;i++) csNP << " or " << piExpNP[i];
	  return AddMsg("Expected " + csNP + " parameters.", iLine, iPos, CERR_INVALID_LVAL, CEL_ERROR); }

	bool InvalidTypeOp(CCodeVar& rVar, cStr& csOp, int iLine = -1, int iPos = -1) 
	{ return AddMsg("'" + rVar.Name() + "' (" + rVar.TypeStr() + ") is of invalid type for " + csOp + ".", iLine, iPos, CERR_INVALID_TYPE_DUAL, CEL_ERROR); }

	bool InconsistentRowLength(int iRow, int iLine = -1, int iPos = -1) 
	{ cStr csText = "Inconsistent row length at row "; return AddMsg(csText << iRow << ".", iLine, iPos, CERR_INVALID_TYPE_DUAL, CEL_ERROR); }

	bool GeneralError(const char *pcText, int iLine = -1, int iPos = -1) 
	{ return AddMsg(pcText, iLine, iPos, CERR_GENERAL, CEL_ERROR); }


	bool LoopMaxCountReached(int iLine = -1, int iPos = -1) 
	{ cStr csText = "Maximum allowed loop count reached at row "; return AddMsg(csText, iLine, iPos, CERR_MAX_LOOP_COUNT, CEL_ERROR); }


	bool DivByZero(int iLine = -1, int iPos = -1)
	{ return AddMsg("Division by zero.", iLine, iPos, CERR_MATH, CEL_ERROR); }

	bool OutOfRange(double dVal, double dMin, double dMax, int iLine = -1, int iPos = -1)
	{ 
		cStr csText = "Parameter has to be in range [";
		csText << dMin << ", " << dMax << "]. ";
		csText << "Current value is " << dVal << ".";

		return AddMsg(csText, iLine, iPos, CERR_MATH, CEL_ERROR); 
	}

	bool IsNAN(double dVal, int iLine = -1, int iPos = -1)
	{
		cStr csText = "Resultant value (";
		csText << dVal << ") is not an expressible real number.";

		return AddMsg(csText, iLine, iPos, CERR_MATH, CEL_ERROR); 		
	}

	bool MatrixIsNAN(int iLine = -1, int iPos = -1)
	{
		return AddMsg("Resultant matrix contains values that are not expressible as real numbers.", 
						iLine, iPos, CERR_MATH, CEL_ERROR); 		
	}


////////////////
/// Fatal Errors

	bool Internal(int iLine = -1, int iPos = -1) 
	{ return AddMsg("Fatal Error: Unexpected internal error.", iLine, iPos, CERR_INTERNAL, CEL_FATAL); }

	bool OutOfMemory(int iLine = -1, int iPos = -1) 
	{ return AddMsg("Fatal Error: Out of memory.", iLine, iPos, CERR_OUTOFMEMORY, CEL_FATAL); }

//////////////////
/// Internal Controls
	bool Break()
	{ return AddMsg("Internal break.", -1, -1, CERR_BREAK, CEL_INTERNAL); }
};

#endif // !defined(AFX_CODEERRORLIST_H__3BD5D0E1_5D4E_4674_B624_27E72E1BBEFD__INCLUDED_)
