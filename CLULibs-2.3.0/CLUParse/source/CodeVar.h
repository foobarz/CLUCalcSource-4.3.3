// CodeVar.h: Schnittstelle für die Klasse CCodeVar.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARSEVAR_H__BFE0E78B_3030_4E0E_ABFA_B1571FB301F2__INCLUDED_)
#define AFX_PARSEVAR_H__BFE0E78B_3030_4E0E_ABFA_B1571FB301F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include "CodeElement.h"
#include "VarList.h"
#include "ImageRepository.h"

#include "TensorData.h"
#include "TensorIdx.h"

using std::list;

//class CCodeVar;

enum ECodeDataType
{
	PDT_NOTYPE,
	PDT_STRING,
	PDT_INT,
	PDT_UINT,
	PDT_LONG,
	PDT_FLOAT,
	PDT_DOUBLE,
	PDT_MULTIV,
	PDT_MATRIX,
	PDT_TENSOR,
	PDT_TENSOR_IDX,
	PDT_COLOR,
	PDT_CODEPTR,
	PDT_VARLIST,
	PDT_VEXLIST,
	PDT_IMAGE,
	PDT_PTR_STRING,
	PDT_PTR_INT,
	PDT_PTR_UINT,
	PDT_PTR_LONG,
	PDT_PTR_FLOAT,
	PDT_PTR_DOUBLE,
	PDT_PTR_MULTIV,
	PDT_PTR_MATRIX,
	PDT_PTR_TENSOR,
	PDT_PTR_TENSOR_IDX,
	PDT_PTR_COLOR,
	PDT_PTR_CODEPTR,
	PDT_PTR_VARLIST,
	PDT_PTR_VEXLIST,
	PDT_PTR_IMAGE,
	PDT_PTR_VAR,
	PDT_MAX_TYPE,
	PDT_SCALAR = PDT_DOUBLE,
	PDT_COUNTER = PDT_INT,
	PDT_PTR_SCALAR = PDT_PTR_DOUBLE,
	PDT_PTR_COUNTER = PDT_PTR_INT
};

typedef int TCVCounter;
typedef double TCVScalar;
typedef Matrix<TCVScalar> TMatrix;
typedef CTensorData<TCVScalar> TTensor;
typedef CTensorIdx<TCVScalar> TTensorIdx;
typedef MultiV<TCVScalar> TMultiV;
typedef CCodeElement* TCodePtr;
typedef CVarList* TVarListPtr;
typedef CVarList TVarList;
typedef COGLVertexList TVexList;
typedef COGLColor TOGLColor;
typedef cStr TString;
typedef CImageReference TImage;

class CCodeVar  
{
public:
	typedef TString*	TStringPtr;
	typedef int*		TIntPtr;
	typedef uint*		TUIntPtr;
	typedef long*		TLongPtr;
	typedef float*		TFloatPtr;
	typedef double*		TDoublePtr;
	typedef TMatrix*	TMatrixPtr;
	typedef TTensor*	TTensorPtr;
	typedef TTensorIdx*	TTensorIdxPtr;
	typedef TMultiV*	TMultiVPtr;
	typedef TOGLColor*	TOGLColorPtr;
	typedef TCodePtr*	TCodePtrPtr;
	typedef TVarList*	TVarListPtr;
	typedef TVexList*	TVexListPtr;
	typedef TImage*		TImagePtr;
	typedef CCodeVar*	TVarPtr;

public:
	CCodeVar();
	CCodeVar(const CCodeVar & rVar);
	virtual ~CCodeVar();

	bool New(ECodeDataType _nType, const char *pcName = 0);
	bool Delete(bool bForce = false);

	void EnableProtect(bool bVal = true) { m_bProtected = bVal; }
	bool IsProtected() { return m_bProtected; }

	// Also copies Name of variable!
	CCodeVar& CopyInstance(const CCodeVar& rVar);

	// Set Pointer Content
	// Sets content is this variable is pointer.
	// rVar has to be of same type as pointer content.
	// If bAllowCast = true then it is attempted to cast rVar appropriately.
	bool SetPtrContent(const CCodeVar& rVar, bool bAllowCast = true);

	// Only copies value of variable
	CCodeVar& operator= (const CCodeVar& rVar);
	CCodeVar& operator= (const TVarPtr& rVar);

	CCodeVar& operator= (const TString& rVar);
	CCodeVar& operator= (const int& rVar);
	CCodeVar& operator= (const uint& rVar);
	CCodeVar& operator= (const long& rVar);
	CCodeVar& operator= (const float& rVar);
	CCodeVar& operator= (const double& rVar);
	CCodeVar& operator= (const TMultiV& rVar);
	CCodeVar& operator= (const TMatrix& rVar);
	CCodeVar& operator= (const TTensor& rVar);
	CCodeVar& operator= (const TTensorIdx& rVar);
	CCodeVar& operator= (const TOGLColor& rVar);
	CCodeVar& operator= (const TCodePtr& pVar);
	CCodeVar& operator= (const TVarList& pVar);
	CCodeVar& operator= (const TVexList& pVar);
	CCodeVar& operator= (const TImage& pVar);

	CCodeVar& operator= (const TStringPtr& rVar);
	CCodeVar& operator= (const TIntPtr& rVar);
	CCodeVar& operator= (const TUIntPtr& rVar);
	CCodeVar& operator= (const TLongPtr& rVar);
	CCodeVar& operator= (const TFloatPtr& rVar);
	CCodeVar& operator= (const TDoublePtr& rVar);
	CCodeVar& operator= (const TMultiVPtr& rVar);
	CCodeVar& operator= (const TMatrixPtr& rVar);
	CCodeVar& operator= (const TTensorPtr& rVar);
	CCodeVar& operator= (const TTensorIdxPtr& rVar);
	CCodeVar& operator= (const TOGLColorPtr& rVar);
	CCodeVar& operator= (const TCodePtrPtr& rVar);
	CCodeVar& operator= (const TVarListPtr& rVar);
	CCodeVar& operator= (const TVexListPtr& rVar);
	CCodeVar& operator= (const TImagePtr& rVar);

	int* GetIntPtr();
	uint* GetUIntPtr();
	long* GetLongPtr();
	float* GetFloatPtr();
	double* GetDoublePtr();
	TCVScalar* GetScalarPtr();
	TCVCounter* GetCounterPtr();
	TString* GetStringPtr();
	TMultiV* GetMultiVPtr();
	TMatrix* GetMatrixPtr();
	TTensor* GetTensorPtr();
	TTensorIdx* GetTensorIdxPtr();
	TOGLColor* GetOGLColorPtr();
	TCodePtr* GetCodePtrPtr();
	TVarList* GetVarListPtr();
	TVexList* GetVexListPtr();
	TImage* GetImagePtr();

	bool CastToScalar(TCVScalar& Val, TCVScalar fPrec = 0) const;

	// If bExact == true then float and double are not
	// converted to counter. Instead false is returned.
	bool CastToCounter(TCVCounter& Val, bool bExact = true) const;

	// Convert Single Element List to Single Element
	CCodeVar& ConvertSELtoSE();

	// Convert Single Element to Single Element List
	CCodeVar& ConvertSEtoSEL();

	// Dereference Pointer
	CCodeVar& DereferencePtr();

	// Dereference Variable Pointer
	// If bComplete == true, then variable is dereferenced until it is
	// not a pointer to a variable anymore.
	CCodeVar& DereferenceVarPtr(bool bComplete = false);

	// Register a variable that references this one
	void RegisterRefVar(CCodeVar *pVar);

	// DeRegister a variable that references this one
	void DeRegisterRefVar(CCodeVar *pVar);

	// Tell this variable that its reference is no longer valid
	void InvalidateRefVar();

	// Check whether variable is pointer
	bool IsPtr() { return m_bIsPtr; }

	TString& Name() { return m_csName; }
	ECodeDataType Type() { return m_nType; }
	ECodeDataType BaseType();
	TString TypeStr() { return TypeStr(m_nType); }
	static TString TypeStr(ECodeDataType nType);
	bool IsTypeValid() 
	{ 
		if (m_nType < PDT_NOTYPE || m_nType >= PDT_MAX_TYPE) return false;
		else return true;
	}


	void * Val() { return m_pData; }

	// Returns value as string
	TString ValStr();

public:
	// The image repository
	// Image variables only contain references to images in this repository.
	static CImageRepository m_ImgRep;

protected:
	bool SetVar(ECodeDataType _nType, void *_pData, char* pcName = 0);

	ECodeDataType	m_nType;
	void *m_pData;	// Data of type nType

	TString m_csName;

	// If true variable cannot be overwritten, or deleted.
	bool m_bProtected;

	// True if stored variable is pointer
	bool m_bIsPtr;

	// List of CodeVars that reference this CodeVar
	list<CCodeVar*> m_listRefBy;
};




#endif // !defined(AFX_PARSEVAR_H__BFE0E78B_3030_4E0E_ABFA_B1571FB301F2__INCLUDED_)
