// CodeVar.cpp: Implementierung der Klasse CCodeVar.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CodeVar.h"
#include <algorithm>

using namespace std;

CImageRepository CCodeVar::m_ImgRep;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCodeVar::CCodeVar()
{
	m_nType = PDT_NOTYPE; 
	m_pData = 0; 

	m_bProtected = false;
	m_bIsPtr = false;
}

CCodeVar::CCodeVar(const CCodeVar& rVar)
{
	m_nType = PDT_NOTYPE; 
	m_pData = 0; 

	m_bProtected = false;
	m_bIsPtr = false;

	CopyInstance(rVar);
}

CCodeVar::~CCodeVar()
{
	// Reset all variables referencing this one
	list<CCodeVar*>::iterator it_El, it_End;
	it_End = m_listRefBy.end();
	it_El = m_listRefBy.begin();
	while (it_El != it_End)
	{
		// This already removes reference from list
		(*it_El)->InvalidateRefVar();
		it_El = m_listRefBy.begin();
		it_End = m_listRefBy.end();
	}
	//m_listRefBy.erase(m_listRefBy.begin(), m_listRefBy.end());

	Delete(true);
}

int* CCodeVar::GetIntPtr()
{
	if (m_nType == PDT_INT) 
		return ((int *) m_pData);
	else if (m_nType == PDT_PTR_INT) 
		return *((int **) m_pData);
	else 
		return 0;
}

uint* CCodeVar::GetUIntPtr()
{
	if (m_nType == PDT_UINT) 
		return ((uint *) m_pData);
	else if (m_nType == PDT_PTR_UINT) 
		return *((uint **) m_pData);
	else 
		return 0;
}

long* CCodeVar::GetLongPtr()
{
	if (m_nType == PDT_LONG) 
		return ((long *) m_pData);
	else if (m_nType == PDT_PTR_LONG) 
		return *((long **) m_pData);
	else 
		return 0;
}

float* CCodeVar::GetFloatPtr()
{
	if (m_nType == PDT_FLOAT) 
		return ((float *) m_pData);
	else if (m_nType == PDT_PTR_FLOAT) 
		return *((float **) m_pData);
	else 
		return 0;
}

double* CCodeVar::GetDoublePtr()
{
	if (m_nType == PDT_DOUBLE) 
		return ((double *) m_pData);
	else if (m_nType == PDT_PTR_DOUBLE) 
		return *((double **) m_pData);
	else 
		return 0;
}

TCVScalar* CCodeVar::GetScalarPtr()
{
	if (m_nType == PDT_SCALAR) 
		return ((TCVScalar *) m_pData);
	else if (m_nType == PDT_PTR_SCALAR) 
		return *((TCVScalar **) m_pData);
	else 
		return 0;
}

TCVCounter* CCodeVar::GetCounterPtr()
{
	if (m_nType == PDT_COUNTER) 
		return ((TCVCounter *) m_pData);
	else if (m_nType == PDT_PTR_COUNTER) 
		return *((TCVCounter **) m_pData);
	else 
		return 0;
}


TString* CCodeVar::GetStringPtr()
{
	if (m_nType == PDT_STRING) 
		return ((TString *) m_pData);
	else if (m_nType == PDT_PTR_STRING) 
		return *((TStringPtr *) m_pData);
	else 
		return 0;
}

TMultiV* CCodeVar::GetMultiVPtr()
{
	if (m_nType == PDT_MULTIV) 
		return ((TMultiV *) m_pData);
	else if (m_nType == PDT_PTR_MULTIV) 
		return *((TMultiVPtr *) m_pData);
	else 
		return 0;
}

TMatrix* CCodeVar::GetMatrixPtr()
{
	if (m_nType == PDT_MATRIX) 
		return ((TMatrix *) m_pData);
	else if (m_nType == PDT_PTR_MATRIX) 
		return *((TMatrixPtr *) m_pData);
	else 
		return 0;
}

TTensor* CCodeVar::GetTensorPtr()
{
	if (m_nType == PDT_TENSOR) 
		return ((TTensor *) m_pData);
	else if (m_nType == PDT_PTR_TENSOR) 
		return *((TTensorPtr *) m_pData);
	else 
		return 0;
}

TTensorIdx* CCodeVar::GetTensorIdxPtr()
{
	if (m_nType == PDT_TENSOR_IDX) 
		return ((TTensorIdx *) m_pData);
	else if (m_nType == PDT_PTR_TENSOR_IDX) 
		return *((TTensorIdxPtr *) m_pData);
	else 
		return 0;
}

TOGLColor* CCodeVar::GetOGLColorPtr()
{
	if (m_nType == PDT_COLOR) 
		return ((TOGLColor *) m_pData);
	else if (m_nType == PDT_PTR_COLOR) 
		return *((TOGLColorPtr *) m_pData);
	else 
		return 0;
}

TCodePtr* CCodeVar::GetCodePtrPtr()
{
	if (m_nType == PDT_CODEPTR) 
		return ((TCodePtr *) m_pData);
	else if (m_nType == PDT_PTR_CODEPTR) 
		return *((TCodePtrPtr *) m_pData);
	else 
		return 0;
}

TVarList* CCodeVar::GetVarListPtr()
{
	if (m_nType == PDT_VARLIST) 
		return ((TVarList *) m_pData);
	else if (m_nType == PDT_PTR_VARLIST) 
		return *((TVarListPtr *) m_pData);
	else 
		return 0;
}

TVexList* CCodeVar::GetVexListPtr()
{
	if (m_nType == PDT_VEXLIST) 
		return ((TVexList *) m_pData);
	else if (m_nType == PDT_PTR_VEXLIST) 
		return *((TVexListPtr *) m_pData);
	else 
		return 0;
}

TImage* CCodeVar::GetImagePtr()
{
	if (m_nType == PDT_IMAGE) 
		return ((TImage *) m_pData);
	else if (m_nType == PDT_PTR_IMAGE) 
		return *((TImagePtr *) m_pData);
	else 
		return 0;
}



//////////////////////////////////////////////////////////////////////
/// Cast To SCALAR

bool CCodeVar::CastToScalar(TCVScalar& Val, TCVScalar fPrec) const
{
	Mem<uint> mGList;
	TMultiV* pvA;
	TMatrix *pmA;
	TTensor *ptA;
	TTensorIdx *ptiA;

	if (fPrec == 0)
		Tiny(fPrec);

	switch(m_nType)
	{
	case PDT_INT:
		Val = ((TCVScalar) *((int *) m_pData));
		break;
		
	case PDT_PTR_INT:
		Val = ((TCVScalar) **((int **) m_pData));
		break;
		
	case PDT_UINT:
		Val = ((TCVScalar) *((uint *) m_pData));
		break;
		
	case PDT_PTR_UINT:
		Val = ((TCVScalar) **((uint **) m_pData));
		break;
		
	case PDT_LONG:
		Val = ((TCVScalar) *((long *) m_pData));
		break;
		
	case PDT_PTR_LONG:
		Val = ((TCVScalar) **((long **) m_pData));
		break;
		
	case PDT_FLOAT:
		Val = ((TCVScalar) *((float *) m_pData));
		break;
		
	case PDT_PTR_FLOAT:
		Val = ((TCVScalar) **((float **) m_pData));
		break;
		
	case PDT_DOUBLE:
		Val = ((TCVScalar) *((double *) m_pData));
		break;

	case PDT_PTR_DOUBLE:
		Val = ((TCVScalar) **((double **) m_pData));
		break;

	case PDT_MULTIV:
	case PDT_PTR_MULTIV:
		if (m_nType == PDT_MULTIV)
			pvA = ((TMultiV *) m_pData);
		else
			pvA = *((TMultiV **) m_pData);

		if (GradeList(*pvA, mGList, fPrec) && 
			mGList.Count() == 1 &&
			mGList[0] == 0) 
		{
			Val = Scalar(*pvA);
		}
		else if (mGList.Count() == 0)
		{
			Val = TCVScalar(0);
		}
		else return false;
		break;

	case PDT_MATRIX:
	case PDT_PTR_MATRIX:
		if (m_nType == PDT_MATRIX)
			pmA = ((TMatrix *) m_pData);
		else
			pmA = *((TMatrix **) m_pData);

		if (pmA->Rows() != 1 || pmA->Cols() != 1)
			return false;

		Val = (*pmA)(1,1);
		break;

	case PDT_TENSOR:
	case PDT_PTR_TENSOR:
		if (m_nType == PDT_TENSOR)
			ptA = ((TTensor *) m_pData);
		else
			ptA = *((TTensor **) m_pData);

		if (ptA->Size() != 1)
			return false;

		Val = *(ptA->Data());
		break;

	case PDT_TENSOR_IDX:
	case PDT_PTR_TENSOR_IDX:
		if (m_nType == PDT_TENSOR_IDX)
			ptiA = ((TTensorIdx *) m_pData);
		else
			ptiA = *((TTensorIdx **) m_pData);

		if (!ptiA->IsConstant())
			return false;

		Val = ptiA->GetRef();
		break;

	case PDT_PTR_VAR:
		return (*((TVarPtr *) m_pData))->CastToScalar(Val, fPrec);

	default:
		return false;
	}
	
	if (::IsZero(Val, fPrec))
		Val = 0;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Cast To COUNTER

bool CCodeVar::CastToCounter(TCVCounter& Val, bool bExact) const
{
	float fVal;
	double dVal;
	TVarList *pList;

	switch(m_nType)
	{
	case PDT_INT:
		Val = ((TCVCounter) *((int *) m_pData));
		break;
		
	case PDT_PTR_INT:
		Val = ((TCVCounter) **((int **) m_pData));
		break;
		
	case PDT_UINT:
		Val = ((TCVCounter) *((uint *) m_pData));
		break;
		
	case PDT_PTR_UINT:
		Val = ((TCVCounter) **((uint **) m_pData));
		break;
		
	case PDT_LONG:
		Val = ((TCVCounter) *((long *) m_pData));
		break;
		
	case PDT_PTR_LONG:
		Val = ((TCVCounter) **((long **) m_pData));
		break;
		
	case PDT_FLOAT:
		fVal = *((float *) m_pData);
		Val = (int) fVal;
		if (bExact && fVal != float(Val)) return false;
		break;
		
	case PDT_PTR_FLOAT:
		fVal = **((float **) m_pData);
		Val = (int) fVal;
		if (bExact && fVal != float(Val)) return false;
		break;
		
	case PDT_DOUBLE:
		dVal = *((double *) m_pData);
		Val = (int) dVal;
		if (bExact && dVal != double(Val)) return false;
		break;
		
	case PDT_PTR_DOUBLE:
		dVal = **((double **) m_pData);
		Val = (int) dVal;
		if (bExact && dVal != double(Val)) return false;
		break;

	case PDT_VARLIST:
		pList = ((TVarList *) m_pData);
		if (pList->Count() != 1 || !(*pList)(0).CastToCounter(Val, bExact))
			return false;
		break;
		
	default:
		return false;
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////
// Get the basis type of variable. That is, if var is a pointer,
// then return type it points to.

ECodeDataType CCodeVar::BaseType()
{
	if (!IsPtr())
		return m_nType;

	switch(m_nType)
	{
	case PDT_PTR_STRING:
		return PDT_STRING;

	case PDT_PTR_INT:
		return PDT_INT;

	case PDT_PTR_UINT:
		return PDT_UINT;

	case PDT_PTR_LONG:
		return PDT_LONG;

	case PDT_PTR_FLOAT:
		return PDT_FLOAT;

	case PDT_PTR_DOUBLE:
		return PDT_DOUBLE;

	case PDT_PTR_MULTIV:
		return PDT_MULTIV;

	case PDT_PTR_MATRIX:
		return PDT_MATRIX;

	case PDT_PTR_TENSOR:
		return PDT_TENSOR;

	case PDT_PTR_TENSOR_IDX:
		return PDT_TENSOR_IDX;

	case PDT_PTR_COLOR:
		return PDT_COLOR;

	case PDT_PTR_CODEPTR:
		return PDT_CODEPTR;

	case PDT_PTR_VARLIST:
		return PDT_VARLIST;

	case PDT_PTR_VEXLIST:
		return PDT_VEXLIST;

	case PDT_PTR_IMAGE:
		return PDT_IMAGE;

	default:
		return m_nType;
	}
}


//////////////////////////////////////////////////////////////////////
// Convert Single Element List to Single Element

CCodeVar& CCodeVar::ConvertSELtoSE()
{
	if (m_nType == PDT_VARLIST)
	{
		CCodeVar rNewVar;
		
		TVarList& rList = *((TVarList *) m_pData);
		if (rList.Count() == 1)
		{
			rList[0].ConvertSELtoSE();

			// Don't write *this = rList[0] since then rList is
			// destroyed before its element can be passed to *this,
			// since *this initially contains the list.
			rNewVar = rList[0];
			*this = rNewVar;
		}
	}

	return *this;
}

//////////////////////////////////////////////////////////////////////
// Convert Single Element to Single Element List

CCodeVar& CCodeVar::ConvertSEtoSEL()
{
	if (BaseType() != PDT_VARLIST)
	{
		TVarList List;
		
		List.Set(1);
		List[0] = this->DereferenceVarPtr(true);
		*this = List;
	}

	return *this;
}



//////////////////////////////////////////////////////////////////////
// Set Pointer Content
// Sets content is this variable is pointer.
// rVar has to be of same type as pointer content.

bool CCodeVar::SetPtrContent(const CCodeVar& rVar, bool bAllowCast)
{
	if (!IsPtr())
		return false;

	TCVCounter iVal;
	TCVScalar fVal;
	CCodeVar rNewVar;

	switch(m_nType)
	{
		case PDT_PTR_STRING:
			if (rVar.m_nType == PDT_STRING)
				**((TStringPtr *) m_pData) = *((TString *) rVar.m_pData);
			else if (rVar.m_nType == PDT_PTR_STRING)
				**((TStringPtr *) m_pData) = **((TStringPtr *) rVar.m_pData);
			else
				return false;
			break;

		case PDT_PTR_INT:
			if (rVar.m_nType == PDT_INT)
				**((TIntPtr *) m_pData) = *((int *) rVar.m_pData);
			else if (rVar.m_nType == PDT_PTR_INT)
				**((TIntPtr *) m_pData) = **((TIntPtr *) rVar.m_pData);
			else if (bAllowCast)
			{
				if (!rVar.CastToCounter(iVal))
					return false;
				**((TIntPtr *) m_pData) = iVal;
			}
			else
				return false;
			break;

		case PDT_PTR_UINT:
			if (rVar.m_nType == PDT_UINT)
				**((TUIntPtr *) m_pData) = *((uint *) rVar.m_pData);
			else if (rVar.m_nType == PDT_PTR_UINT)
				**((TUIntPtr *) m_pData) = **((TUIntPtr *) rVar.m_pData);
			else
				return false;
			break;

		case PDT_PTR_LONG:
			if (rVar.m_nType == PDT_LONG)
				**((TLongPtr *) m_pData) = *((long *) rVar.m_pData);
			else if (rVar.m_nType == PDT_PTR_LONG)
				**((TLongPtr *) m_pData) = **((TLongPtr *) rVar.m_pData);
			else
				return false;
			break;

		case PDT_PTR_FLOAT:
			if (rVar.m_nType == PDT_FLOAT)
				**((TFloatPtr *) m_pData) = *((float *) rVar.m_pData);
			else if (rVar.m_nType == PDT_PTR_FLOAT)
				**((TFloatPtr *) m_pData) = **((TFloatPtr *) rVar.m_pData);
			else
				return false;
			break;

		case PDT_PTR_DOUBLE:
			if (rVar.m_nType == PDT_DOUBLE)
				**((TDoublePtr *) m_pData) = *((double *) rVar.m_pData);
			else if (rVar.m_nType == PDT_PTR_DOUBLE)
				**((TDoublePtr *) m_pData) = **((TDoublePtr *) rVar.m_pData);
			else if (bAllowCast)
			{
				if (!rVar.CastToScalar(fVal))
					return false;
				**((TDoublePtr *) m_pData) = fVal;
			}
			else
				return false;
			break;

		case PDT_PTR_MULTIV:
			if (rVar.m_nType == PDT_MULTIV)
				**((TMultiVPtr *) m_pData) = *((TMultiV *) rVar.m_pData);
			else if (rVar.m_nType == PDT_PTR_MULTIV)
				**((TMultiVPtr *) m_pData) = **((TMultiVPtr *) rVar.m_pData);
			else
				return false;
			break;

		case PDT_PTR_MATRIX:
			if (rVar.m_nType == PDT_MATRIX)
				**((TMatrixPtr *) m_pData) = *((TMatrix *) rVar.m_pData);
			else if (rVar.m_nType == PDT_PTR_MATRIX)
				**((TMatrixPtr *) m_pData) = **((TMatrixPtr *) rVar.m_pData);
			else
				return false;
			break;

		case PDT_PTR_TENSOR:
			if (rVar.m_nType == PDT_TENSOR)
				**((TTensorPtr *) m_pData) = *((TTensor *) rVar.m_pData);
			else if (rVar.m_nType == PDT_PTR_TENSOR)
				**((TTensorPtr *) m_pData) = **((TTensorPtr *) rVar.m_pData);
			else
				return false;
			break;

		case PDT_PTR_TENSOR_IDX:
			if (rVar.m_nType == PDT_TENSOR_IDX)
				**((TTensorIdxPtr *) m_pData) = *((TTensorIdx *) rVar.m_pData);
			else if (rVar.m_nType == PDT_PTR_TENSOR_IDX)
				**((TTensorIdxPtr *) m_pData) = **((TTensorIdxPtr *) rVar.m_pData);
			else
				return false;
			break;

		case PDT_PTR_COLOR:
			if (rVar.m_nType == PDT_COLOR)
				**((TOGLColorPtr *) m_pData) = *((TOGLColor *) rVar.m_pData);
			else if (rVar.m_nType == PDT_PTR_COLOR)
				**((TOGLColorPtr *) m_pData) = **((TOGLColorPtr *) rVar.m_pData);
			else
				return false;
			break;

		case PDT_PTR_CODEPTR:
			if (rVar.m_nType == PDT_CODEPTR)
				**((TCodePtrPtr *) m_pData) = *((TCodePtr *) rVar.m_pData);
			else if (rVar.m_nType == PDT_PTR_CODEPTR)
				**((TCodePtrPtr *) m_pData) = **((TCodePtrPtr *) rVar.m_pData);
			else
				return false;
			break;

		case PDT_PTR_VARLIST:
			if (rVar.m_nType == PDT_VARLIST)
				**((TVarListPtr *) m_pData) = *((TVarList *) rVar.m_pData);
			else if (rVar.m_nType == PDT_PTR_VARLIST)
				**((TVarListPtr *) m_pData) = **((TVarListPtr *) rVar.m_pData);
			else
				return false;
			break;

		case PDT_PTR_VEXLIST:
			if (rVar.m_nType == PDT_VEXLIST)
				**((TVexListPtr *) m_pData) = *((TVexList *) rVar.m_pData);
			else if (rVar.m_nType == PDT_PTR_VEXLIST)
				**((TVexListPtr *) m_pData) = **((TVexListPtr *) rVar.m_pData);
			else
				return false;
			break;

		case PDT_PTR_IMAGE:
			if (rVar.m_nType == PDT_IMAGE)
				**((TImagePtr *) m_pData) = *((TImage *) rVar.m_pData);
			else if (rVar.m_nType == PDT_PTR_IMAGE)
				**((TImagePtr *) m_pData) = **((TImagePtr *) rVar.m_pData);
			else
				return false;
			break;

		case PDT_PTR_VAR:
			**((TVarPtr *) m_pData) = rVar;
			break;

		default:
			return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Dereference Variable Pointer
// Returns the reference to the variable a variable pointer points to.
// If variable is no variable pointer, then reference to itself is returned.

CCodeVar& CCodeVar::DereferenceVarPtr(bool bComplete)
{
	if (bComplete)
	{
		CCodeVar *pVar = this;
	
		while (pVar->Type() == PDT_PTR_VAR)
			pVar = *((CCodeVar **) pVar->Val());

		return *pVar;
	}
	else
	{
		if (Type() == PDT_PTR_VAR)
			return **((CCodeVar **) m_pData);
		else
			return *this;
	}

}

//////////////////////////////////////////////////////////////////////
// Dereference Pointer
// If variable is a pointer it is replaced by its content

CCodeVar& CCodeVar::DereferencePtr()
{
	if (!m_bIsPtr)
		return *this;

	bool bIsProtected = IsProtected();

	EnableProtect(false);

	switch(m_nType)
	{
		case PDT_PTR_STRING:
			*this = **((TStringPtr *) m_pData);
			break;

		case PDT_PTR_INT:
			*this = **((TIntPtr *) m_pData);
			break;

		case PDT_PTR_UINT:
			*this = **((TUIntPtr *) m_pData);
			break;

		case PDT_PTR_LONG:
			*this = **((TLongPtr *) m_pData);
			break;

		case PDT_PTR_FLOAT:
			*this = **((TFloatPtr *) m_pData);
			break;

		case PDT_PTR_DOUBLE:
			*this = **((TDoublePtr *) m_pData);
			break;

		case PDT_PTR_MULTIV:
			*this = **((TMultiVPtr *) m_pData);
			break;

		case PDT_PTR_MATRIX:
			*this = **((TMatrixPtr *) m_pData);
			break;

		case PDT_PTR_TENSOR:
			*this = **((TTensorPtr *) m_pData);
			break;

		case PDT_PTR_TENSOR_IDX:
			*this = **((TTensorIdxPtr *) m_pData);
			break;

		case PDT_PTR_COLOR:
			*this = **((TOGLColorPtr *) m_pData);
			break;

		case PDT_PTR_CODEPTR:
			*this = **((TCodePtrPtr *) m_pData);
			break;

		case PDT_PTR_VARLIST:
			*this = **((TVarListPtr *) m_pData);
			break;

		case PDT_PTR_VEXLIST:
			*this = **((TVexListPtr *) m_pData);
			break;

		case PDT_PTR_IMAGE:
			*this = **((TImagePtr *) m_pData);
			break;

		case PDT_PTR_VAR:
			*this = **((TVarPtr *) m_pData);
			break;
	}

	EnableProtect(bIsProtected);

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// String
/// Return Value as String

TString CCodeVar::ValStr()
{
	int i, n, iWidth, iHeight;
	int iRows;
	TString csVal, csDummy;
	char pcVal[100];
	float *pfCol;
	TVarList *pVarList;
	TVexList *pVexList;
	TMatrix *pMat;
	TImage *pImg;
	CCodeVar *pVar;

	switch(m_nType)
	{
		case PDT_NOTYPE:
			csVal = "NoType";
			break;

		case PDT_STRING:
			csVal = *((TString *) m_pData);
			break;

		case PDT_INT:
			csVal = *((int *) m_pData);
			break;

		case PDT_UINT:
			csVal = *((unsigned int *) m_pData);
			break;

		case PDT_LONG:
			csVal = *((long *) m_pData);
			break;

		case PDT_FLOAT:
			csVal = *((float *) m_pData);
			break;

		case PDT_DOUBLE:
			csVal = *((double *) m_pData);
			break;

		case PDT_MULTIV:
			csVal = ((TMultiV *) m_pData)->Str();
			break;

		case PDT_MATRIX:
			pMat = (TMatrix *) m_pData;
			iRows = (int) pMat->Rows();
			csVal = "(";
			for(i=0;i<iRows;i++)
			{
				csVal << pMat->Str(i+1);
				if (i < iRows-1)
					csVal << ", ";
			}
			csVal << ")";
			break;

		case PDT_TENSOR:
			csVal = "Tensor";
			break;

		case PDT_TENSOR_IDX:
			csVal = "Tensor Value";
			break;

		case PDT_COLOR:
			pfCol = ((TOGLColor *) m_pData)->Data();
			csVal << "(" << pfCol[0] << ", " << pfCol[1] << ", " << pfCol[2] << ")";
			break;

		case PDT_CODEPTR:
			sprintf(pcVal, "%p", *((TCodePtr *) m_pData));
			csVal = pcVal;
			break;

		case PDT_VARLIST:
		case PDT_PTR_VARLIST:
			csVal = "[";
			if (m_nType == PDT_VARLIST)
				pVarList = (TVarList *) m_pData;
			else
				pVarList = *((TVarList **) m_pData);

			n = pVarList->Count();
			
			for(i=0;i<n;i++)
			{
				csVal << (*pVarList)[i].ValStr();
				if (i < n-1)
					csVal << ", ";
			}
			csVal << "]";
			break;

		case PDT_VEXLIST:
			csVal = "(";
			pVexList = (TVexList *) m_pData;
			n = pVexList->Count();
			
			for(i=0;i<n;i++)
			{
				COGLVertex &rVex = (*pVexList)[i];

				csVal << "(" << rVex[0] << ", ";
				csVal << rVex[1] << ", ";
				csVal << rVex[2] << ")";
				if (i < n-1)
					csVal << ", ";
			}
			csVal << ")";
			break;

		case PDT_IMAGE:
			csVal = "Image(";
			pImg = (TImage *) m_pData;
			if (!pImg->IsValid())
			{
				csVal << "[Invalid])";
			}
			else
			{
				(*pImg)->GetFilename(csDummy);
				(*pImg)->GetSize(iWidth, iHeight);

                csVal << "'" << csDummy << "', " << iWidth << "x" << iHeight << ")";
			}
			break;

		case PDT_PTR_STRING:
		case PDT_PTR_INT:
		case PDT_PTR_UINT:
		case PDT_PTR_LONG:
		case PDT_PTR_FLOAT:
		case PDT_PTR_DOUBLE:
		case PDT_PTR_MULTIV:
		case PDT_PTR_MATRIX:
		case PDT_PTR_TENSOR:
		case PDT_PTR_COLOR:
		case PDT_PTR_CODEPTR:
		//case PDT_PTR_VARLIST:
		case PDT_PTR_VEXLIST:
			sprintf(pcVal, "%p", *((void **) m_pData));
			csVal = pcVal;
			break;

		case PDT_PTR_VAR:
			csVal = "";
			if (DereferenceVarPtr(true).Type() == PDT_STRING)
			{
				csVal = DereferenceVarPtr(true).ValStr();
			}
			else
			{
				pVar = this;
				do
				{
					pVar = &(pVar->DereferenceVarPtr());
					if (pVar->Type() == PDT_PTR_VAR)
						csVal << pVar->Name() << " -> ";
				} while (pVar->Type() == PDT_PTR_VAR);
				csVal << pVar->ValStr();
			}
			break;

		default:
			csVal = "Unknown";
			break;
	}

	return csVal;
}

//////////////////////////////////////////////////////////////////////
/// Type String

TString CCodeVar::TypeStr(ECodeDataType nType)
{
	TString csType;

	switch(nType)
	{
		case PDT_NOTYPE:
			csType = "unknown";
			break;

		case PDT_STRING:
			csType = "string";
			break;

		case PDT_INT:
			csType = "int";
			break;

		case PDT_UINT:
			csType = "uint";
			break;

		case PDT_LONG:
			csType = "long";
			break;

		case PDT_FLOAT:
			csType = "float";
			break;

		case PDT_DOUBLE:
			csType = "double";
			break;

		case PDT_MULTIV:
			csType = "multivector";
			break;

		case PDT_MATRIX:
			csType = "matrix";
			break;

		case PDT_TENSOR:
			csType = "tensor";
			break;

		case PDT_TENSOR_IDX:
			csType = "tensor";
			break;

		case PDT_COLOR:
			csType = "color";
			break;

		case PDT_CODEPTR:
			csType = "code";
			break;

		case PDT_VARLIST:
			csType = "list";
			break;

		case PDT_VEXLIST:
			csType = "vertex_list";
			break;

		case PDT_IMAGE:
			csType = "image";
			break;

		case PDT_PTR_STRING:
			csType = "string pointer";
			break;

		case PDT_PTR_INT:
			csType = "int pointer";
			break;

		case PDT_PTR_UINT:
			csType = "uint pointer";
			break;

		case PDT_PTR_LONG:
			csType = "long pointer";
			break;

		case PDT_PTR_FLOAT:
			csType = "float pointer";
			break;

		case PDT_PTR_DOUBLE:
			csType = "double pointer";
			break;

		case PDT_PTR_MULTIV:
			csType = "multivector pointer";
			break;

		case PDT_PTR_MATRIX:
			csType = "matrix pointer";
			break;

		case PDT_PTR_TENSOR:
			csType = "tensor pointer";
			break;

		case PDT_PTR_TENSOR_IDX:
			csType = "tensor index pointer";
			break;

		case PDT_PTR_COLOR:
			csType = "color pointer";
			break;

		case PDT_PTR_CODEPTR:
			csType = "code pointer";
			break;

		case PDT_PTR_VARLIST:
			csType = "list pointer";
			break;

		case PDT_PTR_VEXLIST:
			csType = "vertex_list pointer";
			break;

		case PDT_PTR_IMAGE:
			csType = "image pointer";
			break;

		case PDT_PTR_VAR:
			csType = "variable pointer";
			break;

		default:
			csType = "unknown";
			break;
	}

	return csType;
}

//////////////////////////////////////////////////////////////////////
/// Copy Instance of CCodeVar

CCodeVar& CCodeVar::CopyInstance(const CCodeVar& rVar)
{
	SetVar(rVar.m_nType, rVar.m_pData, rVar.m_csName.Str());

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Operator= CCodeVar
/// Only copies value of variable

CCodeVar& CCodeVar::operator= (const CCodeVar& rVar)
{
	if (this != &rVar) 
		SetVar(rVar.m_nType, rVar.m_pData);

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Operator= TVarPtr

CCodeVar& CCodeVar::operator= (const TVarPtr& rVar)
{
	if (this != rVar) 
		SetVar(PDT_PTR_VAR, (void *) &rVar);

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Operator= TString

CCodeVar& CCodeVar::operator= (const TString& rVar)
{
	SetVar(PDT_STRING, (void *) &rVar);

	return *this;
}


//////////////////////////////////////////////////////////////////////
/// Operator= int

CCodeVar& CCodeVar::operator= (const int& rVar)
{
	SetVar(PDT_INT, (void *) &rVar);

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Operator= uint

CCodeVar& CCodeVar::operator= (const uint& rVar)
{
	SetVar(PDT_UINT, (void *) &rVar);

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Operator= long

CCodeVar& CCodeVar::operator= (const long& rVar)
{
	SetVar(PDT_LONG, (void *) &rVar);

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Operator= float

CCodeVar& CCodeVar::operator= (const float& rVar)
{
	SetVar(PDT_FLOAT, (void *) &rVar);

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Operator= double

CCodeVar& CCodeVar::operator= (const double& rVar)
{
	SetVar(PDT_DOUBLE, (void *) &rVar);

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Operator= TMultiV

CCodeVar& CCodeVar::operator= (const TMultiV& rVar)
{
	SetVar(PDT_MULTIV, (void *) &rVar);

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Operator= TMatrix

CCodeVar& CCodeVar::operator= (const TMatrix& rVar)
{
	SetVar(PDT_MATRIX, (void *) &rVar);

	return *this;
}


//////////////////////////////////////////////////////////////////////
/// Operator= TTensor

CCodeVar& CCodeVar::operator= (const TTensor& rVar)
{
	SetVar(PDT_TENSOR, (void *) &rVar);

	return *this;
}


//////////////////////////////////////////////////////////////////////
/// Operator= TTensorIdx

CCodeVar& CCodeVar::operator= (const TTensorIdx& rVar)
{
	SetVar(PDT_TENSOR_IDX, (void *) &rVar);

	return *this;
}


//////////////////////////////////////////////////////////////////////
/// Operator= TOGLColor

CCodeVar& CCodeVar::operator= (const TOGLColor& rVar)
{
	SetVar(PDT_COLOR, (void *) &rVar);

	return *this;
}


//////////////////////////////////////////////////////////////////////
/// Operator= tObjectBasePtr

CCodeVar& CCodeVar::operator= (const TCodePtr& pVar)
{
	SetVar(PDT_CODEPTR, (void *) &pVar);

	return *this;
}


//////////////////////////////////////////////////////////////////////
/// Operator= CCodeVarList

CCodeVar& CCodeVar::operator= (const TVarList &codeVar)
{
	SetVar(PDT_VARLIST, (void *) &codeVar);

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Operator= VexList

CCodeVar& CCodeVar::operator= (const TVexList &codeVar)
{
	SetVar(PDT_VEXLIST, (void *) &codeVar);

	return *this;
}


//////////////////////////////////////////////////////////////////////
/// Operator= Image

CCodeVar& CCodeVar::operator= (const TImage &codeVar)
{
	SetVar(PDT_IMAGE, (void *) &codeVar);

	return *this;
}



//////////////////////////////////////////////////////////////////////
/// Operator= TString*

CCodeVar& CCodeVar::operator= (const TStringPtr& rVar)
{
	SetVar(PDT_PTR_STRING, (void *) &rVar);

	return *this;
}


//////////////////////////////////////////////////////////////////////
/// Operator= int*

CCodeVar& CCodeVar::operator= (const TIntPtr& rVar)
{
	SetVar(PDT_PTR_INT, (void *) &rVar);

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Operator= uint*

CCodeVar& CCodeVar::operator= (const TUIntPtr& rVar)
{
	SetVar(PDT_PTR_UINT, (void *) &rVar);

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Operator= long*

CCodeVar& CCodeVar::operator= (const TLongPtr& rVar)
{
	SetVar(PDT_PTR_LONG, (void *) &rVar);

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Operator= float*

CCodeVar& CCodeVar::operator= (const TFloatPtr& rVar)
{
	SetVar(PDT_PTR_FLOAT, (void *) &rVar);

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Operator= double*

CCodeVar& CCodeVar::operator= (const TDoublePtr& rVar)
{
	SetVar(PDT_PTR_DOUBLE, (void *) &rVar);

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Operator= TMultiV*

CCodeVar& CCodeVar::operator= (const TMultiVPtr& rVar)
{
	SetVar(PDT_PTR_MULTIV, (void *) &rVar);

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Operator= TMatrix*

CCodeVar& CCodeVar::operator= (const TMatrixPtr& rVar)
{
	SetVar(PDT_PTR_MATRIX, (void *) &rVar);

	return *this;
}


//////////////////////////////////////////////////////////////////////
/// Operator= TTensor*

CCodeVar& CCodeVar::operator= (const TTensorPtr& rVar)
{
	SetVar(PDT_PTR_TENSOR, (void *) &rVar);

	return *this;
}


//////////////////////////////////////////////////////////////////////
/// Operator= TTensorIdx*

CCodeVar& CCodeVar::operator= (const TTensorIdxPtr& rVar)
{
	SetVar(PDT_PTR_TENSOR_IDX, (void *) &rVar);

	return *this;
}


//////////////////////////////////////////////////////////////////////
/// Operator= TOGLColor*

CCodeVar& CCodeVar::operator= (const TOGLColorPtr& rVar)
{
	SetVar(PDT_PTR_COLOR, (void *) &rVar);

	return *this;
}


//////////////////////////////////////////////////////////////////////
/// Operator= TCodePtr*

CCodeVar& CCodeVar::operator= (const TCodePtrPtr& pVar)
{
	SetVar(PDT_PTR_CODEPTR, (void *) &pVar);

	return *this;
}


//////////////////////////////////////////////////////////////////////
/// Operator= CCodeVarList*

CCodeVar& CCodeVar::operator= (const TVarListPtr &codeVar)
{
	SetVar(PDT_PTR_VARLIST, (void *) &codeVar);

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Operator= VexList*

CCodeVar& CCodeVar::operator= (const TVexListPtr &codeVar)
{
	SetVar(PDT_PTR_VEXLIST, (void *) &codeVar);

	return *this;
}


//////////////////////////////////////////////////////////////////////
/// Operator= Image*

CCodeVar& CCodeVar::operator= (const TImagePtr &codeVar)
{
	SetVar(PDT_PTR_IMAGE, (void *) &codeVar);

	return *this;
}







//////////////////////////////////////////////////////////////////////
/// Set Variable

bool CCodeVar::SetVar(ECodeDataType _nType, void *pData, char *pcName)
{
	if (!New(_nType, pcName)) return false;

	TVarPtr pVar;

	switch(_nType)
	{
		case PDT_NOTYPE:
			return false;

		case PDT_STRING:
			*((TString *) m_pData) = *((TString *) pData);
			break;

		case PDT_INT:
			*((int *) m_pData) = *((int *) pData);
			break;

		case PDT_UINT:
			*((uint *) m_pData) = *((uint *) pData);
			break;

		case PDT_LONG:
			*((long *) m_pData) = *((long *) pData);
			break;

		case PDT_FLOAT:
			*((float *) m_pData) = *((float *) pData);
			break;

		case PDT_DOUBLE:
			*((double *) m_pData) = *((double *) pData);
			break;

		case PDT_MULTIV:
			*((TMultiV *) m_pData) = *((TMultiV *) pData);
			break;

		case PDT_MATRIX:
			*((TMatrix *) m_pData) = *((TMatrix *) pData);
			break;

		case PDT_TENSOR:
			*((TTensor *) m_pData) = *((TTensor *) pData);
			break;

		case PDT_TENSOR_IDX:
			*((TTensorIdx *) m_pData) = *((TTensorIdx *) pData);
			break;

		case PDT_COLOR:
			*((TOGLColor *) m_pData) = *((TOGLColor *) pData);

		case PDT_CODEPTR:
			*((TCodePtr *) m_pData) = *((TCodePtr *) pData);
			break;

		case PDT_VARLIST:
			*((TVarList *) m_pData) = *((TVarList *) pData);
			break;

		case PDT_VEXLIST:
			*((TVexList *) m_pData) = *((TVexList *) pData);
			break;

		case PDT_IMAGE:
			*((TImage *) m_pData) = *((TImage *) pData);
			break;

		case PDT_PTR_STRING:
			*((TStringPtr *) m_pData) = *((TStringPtr *) pData);
			break;

		case PDT_PTR_INT:
			*((TIntPtr *) m_pData) = *((TIntPtr *) pData);
			break;

		case PDT_PTR_UINT:
			*((TUIntPtr *) m_pData) = *((TUIntPtr *) pData);
			break;

		case PDT_PTR_LONG:
			*((TLongPtr *) m_pData) = *((TLongPtr *) pData);
			break;

		case PDT_PTR_FLOAT:
			*((TFloatPtr *) m_pData) = *((TFloatPtr *) pData);
			break;

		case PDT_PTR_DOUBLE:
			*((TDoublePtr *) m_pData) = *((TDoublePtr *) pData);
			break;

		case PDT_PTR_MULTIV:
			*((TMultiVPtr *) m_pData) = *((TMultiVPtr *) pData);
			break;

		case PDT_PTR_MATRIX:
			*((TMatrixPtr *) m_pData) = *((TMatrixPtr *) pData);
			break;

		case PDT_PTR_TENSOR:
			*((TTensorPtr *) m_pData) = *((TTensorPtr *) pData);
			break;

		case PDT_PTR_TENSOR_IDX:
			*((TTensorIdxPtr *) m_pData) = *((TTensorIdxPtr *) pData);
			break;

		case PDT_PTR_COLOR:
			*((TOGLColorPtr *) m_pData) = *((TOGLColorPtr *) pData);

		case PDT_PTR_CODEPTR:
			*((TCodePtrPtr *) m_pData) = *((TCodePtrPtr *) pData);
			break;

		case PDT_PTR_VARLIST:
			*((TVarListPtr *) m_pData) = *((TVarListPtr *) pData);
			break;

		case PDT_PTR_VEXLIST:
			*((TVexListPtr *) m_pData) = *((TVexListPtr *) pData);
			break;

		case PDT_PTR_IMAGE:
			*((TImagePtr *) m_pData) = *((TImagePtr *) pData);
			break;

		case PDT_PTR_VAR:
			pVar = *((TVarPtr *) pData);
			*((TVarPtr *) m_pData) = pVar;
			pVar->RegisterRefVar(this);
			break;

		default:
			return false;
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
// Register a variable that references this one

void CCodeVar::RegisterRefVar(CCodeVar *pVar)
{
	m_listRefBy.push_back(pVar);
}


//////////////////////////////////////////////////////////////////////
// DeRegister a variable that references this one

void CCodeVar::DeRegisterRefVar(CCodeVar *pVar)
{
	list<CCodeVar*>::iterator it_El;

	if ((it_El = find(m_listRefBy.begin(), m_listRefBy.end(), pVar)) != m_listRefBy.end())
	{
		m_listRefBy.erase(it_El);
	}
}

//////////////////////////////////////////////////////////////////////
// Invalidate a reference variable

void CCodeVar::InvalidateRefVar()
{
	if (m_nType != PDT_PTR_VAR)
		return;

	EnableProtect(false);
	(*this) = TCVCounter(0);
}

//////////////////////////////////////////////////////////////////////
/// New

bool CCodeVar::New(ECodeDataType _nType, const char *pcName)
{
	if (!Delete()) return false;

	switch(_nType)
	{
		case PDT_NOTYPE:
			m_bIsPtr = false;
			m_pData = 0;
			break;

		case PDT_STRING:
			m_bIsPtr = false;
			m_pData = (void *) new TString;
			break;

		case PDT_INT:
			m_bIsPtr = false;
			m_pData = (void *) new int;
			break;

		case PDT_UINT:
			m_bIsPtr = false;
			m_pData = (void *) new uint;
			break;

		case PDT_LONG:
			m_bIsPtr = false;
			m_pData = (void *) new long;
			break;

		case PDT_FLOAT:
			m_bIsPtr = false;
			m_pData = (void *) new float;
			break;

		case PDT_DOUBLE:
			m_bIsPtr = false;
			m_pData = (void *) new double;
			break;

		case PDT_MULTIV:
			m_bIsPtr = false;
			m_pData = (void *) new TMultiV;
			break;

		case PDT_MATRIX:
			m_bIsPtr = false;
			m_pData = (void *) new TMatrix;
			break;

		case PDT_TENSOR:
			m_bIsPtr = false;
			m_pData = (void *) new TTensor;
			break;

		case PDT_TENSOR_IDX:
			m_bIsPtr = false;
			m_pData = (void *) new TTensorIdx;
			break;

		case PDT_COLOR:
			m_bIsPtr = false;
			m_pData = (void *) new TOGLColor;
			break;

		case PDT_CODEPTR:
			m_bIsPtr = false;
			m_pData = (void *) new TCodePtr;
			break;

		case PDT_VARLIST:
			m_bIsPtr = false;
			m_pData = (void *) new TVarList;
			break;
				
		case PDT_VEXLIST:
			m_bIsPtr = false;
			m_pData = (void *) new TVexList;
			break;
				
		case PDT_IMAGE:
			m_bIsPtr = false;
			m_pData = (void *) m_ImgRep.New();
			break;
				
		case PDT_PTR_STRING:
			m_bIsPtr = true;
			m_pData = (void *) new TStringPtr;
			break;

		case PDT_PTR_INT:
			m_bIsPtr = true;
			m_pData = (void *) new TIntPtr;
			break;

		case PDT_PTR_UINT:
			m_pData = (void *) new TUIntPtr;
			m_bIsPtr = true;
			break;

		case PDT_PTR_LONG:
			m_bIsPtr = true;
			m_pData = (void *) new TLongPtr;
			break;

		case PDT_PTR_FLOAT:
			m_bIsPtr = true;
			m_pData = (void *) new TFloatPtr;
			break;

		case PDT_PTR_DOUBLE:
			m_bIsPtr = true;
			m_pData = (void *) new TDoublePtr;
			break;

		case PDT_PTR_MULTIV:
			m_bIsPtr = true;
			m_pData = (void *) new TMultiVPtr;
			break;

		case PDT_PTR_MATRIX:
			m_bIsPtr = true;
			m_pData = (void *) new TMatrixPtr;
			break;

		case PDT_PTR_TENSOR:
			m_bIsPtr = true;
			m_pData = (void *) new TTensorPtr;
			break;

		case PDT_PTR_TENSOR_IDX:
			m_bIsPtr = true;
			m_pData = (void *) new TTensorIdxPtr;
			break;

		case PDT_PTR_COLOR:
			m_bIsPtr = true;
			m_pData = (void *) new TOGLColorPtr;
			break;

		case PDT_PTR_CODEPTR:
			m_bIsPtr = true;
			m_pData = (void *) new TCodePtrPtr;
			break;

		case PDT_PTR_VARLIST:
			m_bIsPtr = true;
			m_pData = (void *) new TVarListPtr;
			break;
				
		case PDT_PTR_VEXLIST:
			m_bIsPtr = true;
			m_pData = (void *) new TVexListPtr;
			break;
				
		case PDT_PTR_IMAGE:
			m_bIsPtr = true;
			m_pData = (void *) new TImagePtr;
			break;
				
		case PDT_PTR_VAR:
			m_bIsPtr = true;
			m_pData = (void *) new TVarPtr;
			break;
				
		default:
			return false;
	}

	if (pcName) m_csName = pcName;

	if (!m_pData && _nType != PDT_NOTYPE)
	{
		m_nType = PDT_NOTYPE;
		return false;
	}

	m_nType = _nType;

	return true;
}	

//////////////////////////////////////////////////////////////////////
/// Delete

bool CCodeVar::Delete(bool bForce)
{
	if (m_bProtected && !bForce)
		return false;

	if (!m_pData) return true;

	TVarPtr pVar;

	switch(m_nType)
	{
		case PDT_NOTYPE:
			return false;

		case PDT_STRING:
			delete ((TString *) m_pData);
			break;

		case PDT_INT:
			delete ((int *) m_pData);
			break;

		case PDT_UINT:
			delete ((uint *) m_pData);
			break;

		case PDT_LONG:
			delete ((long *) m_pData);
			break;

		case PDT_FLOAT:
			delete ((float *) m_pData);
			break;

		case PDT_DOUBLE:
			delete ((double *) m_pData);
			break;

		case PDT_MULTIV:
			delete ((TMultiV *) m_pData);
			break;

		case PDT_MATRIX:
			delete ((TMatrix *) m_pData);
			break;

		case PDT_TENSOR:
			delete ((TTensor *) m_pData);
			break;

		case PDT_TENSOR_IDX:
			delete ((TTensorIdx *) m_pData);
			break;

		case PDT_COLOR:
			delete ((TOGLColor *) m_pData);
			break;

		case PDT_CODEPTR:
			delete ((TCodePtr *) m_pData);
			break;

		case PDT_VARLIST:
			delete ((TVarList *) m_pData);
			break;

		case PDT_VEXLIST:
			delete ((TVexList *) m_pData);
			break;

		case PDT_IMAGE:
			delete ((TImage *) m_pData);
			break;

		case PDT_PTR_STRING:
			delete ((TStringPtr *) m_pData);
			break;

		case PDT_PTR_INT:
			delete ((TIntPtr *) m_pData);
			break;

		case PDT_PTR_UINT:
			delete ((TUIntPtr *) m_pData);
			break;

		case PDT_PTR_LONG:
			delete ((TLongPtr *) m_pData);
			break;

		case PDT_PTR_FLOAT:
			delete ((TFloatPtr *) m_pData);
			break;

		case PDT_PTR_DOUBLE:
			delete ((TDoublePtr *) m_pData);
			break;

		case PDT_PTR_MULTIV:
			delete ((TMultiVPtr *) m_pData);
			break;

		case PDT_PTR_MATRIX:
			delete ((TMatrixPtr *) m_pData);
			break;

		case PDT_PTR_TENSOR:
			delete ((TTensorPtr *) m_pData);
			break;

		case PDT_PTR_TENSOR_IDX:
			delete ((TTensorIdxPtr *) m_pData);
			break;

		case PDT_PTR_COLOR:
			delete ((TOGLColorPtr *) m_pData);
			break;

		case PDT_PTR_CODEPTR:
			delete ((TCodePtrPtr *) m_pData);
			break;

		case PDT_PTR_VARLIST:
			delete ((TVarListPtr *) m_pData);
			break;

		case PDT_PTR_VEXLIST:
			delete ((TVexListPtr *) m_pData);
			break;

		case PDT_PTR_IMAGE:
			delete ((TImagePtr *) m_pData);
			break;

		case PDT_PTR_VAR:
			pVar = *((TVarPtr *) m_pData);
			pVar->DeRegisterRefVar(this);
			delete ((TVarPtr *) m_pData);
			break;

		default:
			return false;
	}

	m_nType = PDT_NOTYPE;
	m_pData = 0;
	m_bIsPtr = false;

//	m_csName = "";

	return true;
}
