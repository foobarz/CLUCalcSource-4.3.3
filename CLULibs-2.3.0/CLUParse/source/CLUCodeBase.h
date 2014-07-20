/// LICENSE START
////////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the CLUParse Library.
//
// 
// Copyright (C) 2001-2004  Christian B.U. Perwass
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// The GNU Library General Public License can be found in the file
// license.txt distributed with this library.
//
// Please send all queries and comments to
//
// email: help@clucalc.info
// mail: Institut fuer Informatik, Olshausenstr. 40, 24098 Kiel, Germany.
//
////////////////////////////////////////////////////////////////////////////////////
/// LICENSE END

// CLUCodeBase.h: Schnittstelle für die Klasse CCLUCodeBase.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLUCODEBASE_H__154A039C_A470_4AE1_99F2_F7D9072E1496__INCLUDED_)
#define AFX_CLUCODEBASE_H__154A039C_A470_4AE1_99F2_F7D9072E1496__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <string>
#include <vector>
#include <time.h>
#include <sys/timeb.h>

#include "XML.h"
#include "CodeBase.h"

#ifdef WIN32
#    include "SyncSerialComm.h"
#endif

#ifdef WIN32
	typedef struct _timeb TTimeStruct;
#	define GETTIME(S) _ftime(S)
#else
	typedef struct timeb TTimeStruct;
	#define GETTIME(S) ftime(S)
#endif

using namespace std;

//////////////////////////////////////////
/// Plot Modes

#define PLOT_POINTS			0x0001
#define PLOT_CONNECTED		0x0002

//////////////////////////////////////////
/// Execution Modes

#define EXEC_MODE_NONE		0x0000  // Code executed for no reason ;-)
#define EXEC_MODE_NEW		0x0001	// Code executed for the first time.
#define EXEC_MODE_CHANGE	0x0002	// Code has been changed.
#define EXEC_MODE_ANIM		0x0004	// Code is run due to animation
#define EXEC_MODE_USER		0x0008	// Code is run because of user interaction
#define EXEC_MODE_TOOL		0x0010	// Code is run because tool changed value
#define EXEC_MODE_AGAIN		0x0020	// Code is run because some command in script
									// makes it necessary to rerun code.
#define EXEC_MODE_KEYDOWN	0x0040	// Code is run because key was pressed down
#define EXEC_MODE_KEYUP		0x0080	// Code is run because key was released
#define EXEC_MODE_RESHAPE	0x0100	// Code is run because vis. window was reshaped
#define EXEC_MODE_LINK		0x0200	// Code is run because link has been activated


//////////////////////////////////////////


class CCLUCodeBase : public CCodeBase
{
public:
	class SortError : public CCLUError
	{
	public:
		SortError()
		{
			m_sErrorText = "Error in sorting operation.";
		}
	};

public:
	struct SScriptFile
	{
		string sTitle;
		string sFilename;
	};


	struct SViewData
	{
		int iX, iY, iW, iH;
	};

	enum ECurSpaceVars 
	{ 
		SPACEVARS_NONE, 
		SPACEVARS_E3, 
		SPACEVARS_P3, 
		SPACEVARS_N3, 
		SPACEVARS_C2 
	};

	enum ETensorSettings
	{
		TENSOR_MAX_VALENCE = 10
	};

	struct SFontData
	{
		string sName;
		int iFont;
		vector<int> vecSize;
	};

	enum EStringType
	{
		STRTYPE_NORMAL,
		STRTYPE_LATEX,
		STRTYPE_HTML
	};

	struct SOutputObject
	{
		cStr sName;
		cStr sValue;
		cStr sType;		// The type string
		cStr sRelSym;	// A symbol like '=' or '->'
		cStr sInfo;		// Additional info, like the size of a matrix or list
		
		int iSize;		// give relative size of font to be used
						// if this is 0 then use standard, +1 is one step higher,
						// -1 is one step lower, etc.

		bool bShowInfo;
		bool bShowValue;
		bool bBRBeforeValue;
		bool bBRAfterValue;
		bool bCanHideValue;
		bool bCanChangeSize;
	};

	typedef vector<SOutputObject> TOutObjList;
#ifdef WIN32
	typedef map<string, CSyncSerialComm> TSerialIOMap;
#endif

public:
	CCLUCodeBase();
	virtual ~CCLUCodeBase();

	void Reset();
	// Reset variables like font type, latex mag step, etc.
	void ResetEnvVars();
	void ResetSpaceVars();
	void ResetInfoVars();
	void ResetSerialIO();
	void ResetOutputObjectList();
	void CollectGarbage();

	// Clean up stuff the user might have forgotten.
	void CleanUp();

	// Return reference to Output Object List
	const TOutObjList& GetOutputObjectList() { return m_vecOutputObject; }
	void InsertOutputObject(const SOutputObject &rObj) 
	{ m_vecOutputObject.insert(m_vecOutputObject.begin(), rObj); }

	// Check that scalar is in particular range, taking sensitivity into account.
	bool IsInRange(TCVScalar& dVal, TCVScalar dMin, TCVScalar dMax, int iLine, int iPos);

	// Script List functions
	void DrawScriptFileIndex();
	void ClearScriptList();
	bool AddScriptToList(string sTitle, string sFilename);
	int NextScriptFile();
	int PrevScriptFile();
	string GetCurScriptFilename();
	string GetCurScriptTitle();
	vector<SScriptFile>& GetScriptList() { return m_vecScriptList; }
	int GetCurScriptIdx() { return m_iCurScript; }
	bool SetCurScriptIdx(int iVal);

	void SetScriptPath(const char* pcScriptPath)
	{ 
		m_sScriptPath = pcScriptPath; 
		m_oglLatex.SetScriptPath(pcScriptPath);
	}

	void SetScriptName(const char* pcScriptName)
	{ 
		m_sScriptName = pcScriptName; 
		m_oglLatex.SetScriptName(pcScriptName);
	}

	void InitBitmapFont();
	bool SetBitmapFont(int iFont);
	bool SetBitmapFontSize(int iSize);

	void SetMVFilter(COGLMVFilter* pFilter) { m_pFilter = pFilter; }
	void SetTransform(Mem<STransform> *pTransform) { m_pTransform = pTransform; }
	void SetFrameRotor(MultiV<float> *pFrameRot) { m_pFrameRotor = pFrameRot; }
	void SetCLUDrawBase(CCLUDrawBase *pCLUDrawBase) { m_pCLUDrawBase = pCLUDrawBase; }

	void SetPointSize(float fWidth);
	float GetPointSize() { return m_fPointSize; }

	void SetLineWidth(float fWidth);
	float GetLineWidth() { return m_fLineWidth; }

	void SetSmoothLine(bool bVal);
	bool GetSmoothLine() { return m_bSmoothLine; }

	void SetSmoothPoint(bool bVal);
	bool GetSmoothPoint() { return m_bSmoothPoint; }

	void SetSmoothPolygon(bool bVal);
	bool GetSmoothPolygon() { return m_bSmoothPolygon; }

	void SetVersion(int iMajor, int iMinor, int iRevision);
	void GetVersion(int& iMajor, int& iMinor, int& iRevision);

	double TextRound(double dVal);

	// String of vector in null basis
	bool VecN3String(TMultiV &vA, cStr &csVal, EStringType eStrType = STRTYPE_NORMAL);

	// String of projective vector
	bool VecP3String(TMultiV &vA, cStr &csVal, EStringType eStrType = STRTYPE_NORMAL);

	// String of Euclidean vector
	bool VecE3String(TMultiV &vA, cStr &csVal, EStringType eStrType = STRTYPE_NORMAL);

	// String of multivector in El2
	bool VecEl2String(TMultiV &vA, cStr &csVal, EStringType eStrType = STRTYPE_NORMAL);

	// Transform matrix into string
	bool MatrixString(TMatrix &M, cStr &csVal, EStringType eStrType = STRTYPE_NORMAL);

	// Transform tensor into string
	bool TensorString(TTensor &T, cStr &csVal, EStringType eStrType = STRTYPE_NORMAL);

	// Transform tensor index into string
	bool TensorIdxString(TTensorIdx &TIdx, cStr &csVal, EStringType eStrType = STRTYPE_NORMAL);

	// Transform variable into latex string.
	// Used to implement special string transformations
	bool CastToLatexString(CCodeVar& rVar, cStr &csVal);

	// Transform variable into HTML string.
	// Used to implement special string transformations
	bool CastToHTMLString(CCodeVar& rVar, cStr &csVal);

	// Transform variable into string.
	// Used to implement special string transformations
	bool CastToString(CCodeVar& rVar, cStr &csVal);

	bool CastToFloat(CCodeVar& rVar, float &rfVal);
	bool CastToDouble(CCodeVar& rVar, double &rdVal);
	bool CastMVtoE3(const TMultiV& vA, TMultiV& vR);
	bool CastMVtoP3(const TMultiV& vA, TMultiV& vR);
	bool CastMVtoEl2(TMultiV& vA, TMultiV& vR);

	bool OpLShift(CCodeVar& rLVar, CCodeVar& rRVar, int iLine, int iPos);
	bool OpPointLShift(CCodeVar& rLVar, CCodeVar& rRVar, int iLine, int iPos);

	bool OpEqual(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
	bool OpPointEqual(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
	bool OpNotEqual(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
	bool OpPointNotEqual(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
	bool OpAND(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
	bool OpPointAND(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
	bool OpOR(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
	bool OpPointOR(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
	bool OpNOT(CCodeVar& rVar, CCodeVar& rResVar, int iLine, int iPos);

	bool OpGreater(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
	bool OpPointGreater(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);

	bool OpLess(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
	bool OpPointLess(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);

	bool OpGreaterEqual(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
	bool OpPointGreaterEqual(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);

	bool OpLessEqual(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
	bool OpPointLessEqual(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);

	bool OpAssign(CCodeVar& rLVar, CCodeVar& rRVar, int iLine, int iPos);
	bool OpReference(CCodeVar& rLVar, CCodeVar& rRVar, int iLine, int iPos);

	bool OpReverse(CCodeVar& rVar, CCodeVar& rResVar, int iLine, int iPos);
	
	bool OpWedge(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
	bool OpPointWedge(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);

	bool OpSubtract(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
	bool OpPointSubtract(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);

	bool OpNegate(CCodeVar& rVar, CCodeVar& rResVar, int iLine, int iPos);
	bool OpDual(CCodeVar& rVar, CCodeVar& rResVar, int iLine, int iPos);
	bool OpInvolute(CCodeVar& rVar, CCodeVar& rResVar, int iLine, int iPos);
	bool OpInverse(CCodeVar& rVar, CCodeVar& rResVar, int iLine, int iPos);

	bool OpInnerProd(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
	bool OpPointInnerProd(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);

	bool OpProd(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
	bool OpPointProd(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
	bool OpAdd(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
	bool OpPointAdd(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);

	bool OpDiv(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
	bool OpPointDiv(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);

	bool OpPow(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
	bool OpGradeProj(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
//	bool OpElementSelect(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
	bool OpJoin(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
	bool OpMeet(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);

//	bool ModFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool OpMod(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);

	bool OpStepList(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);

	bool OpOGLDraw(CCodeVar& rVar, CCodeVar& rResVar, int iLine, int iPos);
	bool OpPrint(CCodeVar& rVar, CCodeVar& rResVar, int iLine, int iPos);

	bool OpFuncCall(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
	bool GetVarListElement(CCodeVar& rElVar, TVarList& rList, TVarList &rIdxList, int iIdxPos, int iLine, int iPos);
	bool RemoveVarListElement(TVarList& rList, TVarList &rIdxList, int iIdxPos, int iLine, int iPos);


	bool MatrixFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool CastToMatrix(TMatrix& rMat, CCodeVar& rVar, int iLine, int iPos);
	bool DetFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool SVDFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool EigenValuesFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool EigenFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool IdentityMatrixFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool Sym2ColMatrixFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool Col2SymMatrixFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool TensorFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool Matrix2Tensor(TTensor& rT, const TMatrix &rM);
	bool VarList2Tensor(TTensor& rT, CCodeVar &rVar, int iDim, int iDataPos, bool bDataDim, int iLine, int iPos);
	bool GetTensorMVFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool GetMVTensorFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool GetGAOpTensorFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool MouseFunc(CCodeVar& rLVar, CCodeVar& rPars, int iLine, int iPos);
	bool Mouse(CCodeVar& rVar, int *piVal, int iLine, int iPos);

	bool SetModeFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool SetPlotModeFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool FloorFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool Floor(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool CeilFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool Ceil(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool RoundFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool Round(CCodeVar& rVar, CCodeVar& rPars, TCVScalar dPrec, int iLine, int iPos);

	bool InvFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool AbsFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool Abs(CCodeVar& rVar, CCodeVar& rData, int iLine, int iPos);

	//bool ArgFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	//bool Arg(CCodeVar& rVar, CCodeVar& rData, int iLine, int iPos);

	bool SumFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool Sum(CCodeVar& rVar, TVarList& rList, int iLine, int iPos);

	bool SubFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool Sub(CCodeVar& rVar, TVarList& rList, int iLine, int iPos);

	bool ProdFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool Prod(CCodeVar& rVar, TVarList& rList, int iLine, int iPos);

	bool OuterProdFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool OuterProd(CCodeVar& rVar, TVarList& rList, int iLine, int iPos);

	bool InnerProdFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool InnerProd(CCodeVar& rVar, TVarList& rList, int iLine, int iPos);

	bool MinFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool Min(CCodeVar& rVar, TVarList& rList, int iLine, int iPos);

	bool MaxFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool Max(CCodeVar& rVar, TVarList& rList, int iLine, int iPos);

	bool ArgMinFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool ArgMin(TVarList& rIdxList, TVarList& rList, int iLine, int iPos);

	bool ArgMaxFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool ArgMax(TVarList& rIdxList, TVarList& rList, int iLine, int iPos);



	bool SqrtFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool Sqrt(CCodeVar& rVar, CCodeVar& rData, int iLine, int iPos);

	bool PowFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool Pow(CCodeVar& rVar, CCodeVar& rLVar, CCodeVar& rRVar, int iLine, int iPos);

	bool RandomSeedFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool RandomFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool GaussFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool GaussParametersFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool CPFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool ACPFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool FactFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	
	bool SinFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool Sin(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool CosFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool Cos(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool TanFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool Tan(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool AsinFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool Asin(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool AcosFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool Acos(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool AtanFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool Atan(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool Atan(CCodeVar& rVar, CCodeVar& rLPars, CCodeVar& rRPars, int iLine, int iPos);

	bool ExpFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool Exponential(CCodeVar& rVar, CCodeVar& rData, int iLine, int iPos);
	bool LogFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool Log(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool SignFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool ProjectFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool RejectFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool GradeListFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	// Len is obsolete from v4.0.0 onwards. Replaced by 'Size'.
	bool LenFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool SizeFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool StringFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool ScalarFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool ScalarEval(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool LatexStringFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool TypeFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool ArgTrueFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool ArgTrueList(TVarList& rArgList, TVarList& rIdxList, CCodeVar& rData, 
						bool bRetainStructure, int iLine, int iPos);
	bool SortFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool GetMVIndicesFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool GenerateMaskFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);


	////////////////////////////////////////////////////////////////////////////////////
	// List Functions
	bool ListFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool ExtractListElementsFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool RemoveListElementsFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool InsertListElementsFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool TransposeListElementsFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool TransposeListElements(TVarList& rTransList, TVarList& rList, int iRecurseDepth, int iLine, int iPos);

	///////////////////////////////////////////////////////////////////////////////////

	bool DefVarsE3(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool DefVarsP3(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool DefVarsN3(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool DefVarsEl2(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool VecE3Func(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool VecE3(CCodeVar& rLVar, TCVScalar *pfVec, int iLine, int iPos);
	bool VecE3(CCodeVar& rLVar, int iVal, int iLine, int iPos);
	bool VecE3(CCodeVar& rLVar, TMultiV& vA, int iLine, int iPos);

	bool VecP3Func(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool VecP3(CCodeVar& rLVar, TCVScalar *pfVec, int iLine, int iPos);
	bool VecP3(CCodeVar& rLVar, int iVal, int iLine, int iPos);
	bool VecP3(CCodeVar& rLVar, TMultiV& vA, int iLine, int iPos);

	bool VecN3Func(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool VecN3(CCodeVar& rVar, TVarList& rVarList, int iLine, int iPos);
	bool VecN3(CCodeVar& rLVar, TCVScalar *pfVec, int iLine, int iPos);
	bool VecN3(CCodeVar& rLVar, int iVal, int iLine, int iPos);
	bool VecN3(CCodeVar& rLVar, TMultiV& vA, int iLine, int iPos);

	bool VecEl2Func(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool VecEl2(CCodeVar& rLVar, TCVScalar *pfVec, int iLine, int iPos);
	bool VecEl2(CCodeVar& rLVar, int iVal, int iLine, int iPos);
	bool VecEl2(CCodeVar& rLVar, TMultiV& vA, int iLine, int iPos);
	bool VecEl2(CCodeVar& rLVar, TMatrix& xM, int iLine, int iPos);

	bool MakeConicDegenerateFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool FindAllDegConicsFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool RotorC2Func(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool ReflectorC2Func(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool RotorE3Func(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool RotorE3(CCodeVar& rLVar, TMultiV& rMV, TCVScalar fDeg, int iLine, int iPos);
	bool RotorE3(CCodeVar& rLVar, TCVScalar *pfVec, int iLine, int iPos);

	bool RotorP3Func(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool RotorP3(CCodeVar& rLVar, TMultiV& rMV, TCVScalar fDeg, int iLine, int iPos);
	bool RotorP3(CCodeVar& rLVar, TCVScalar *pfVec, int iLine, int iPos);

	bool RotorN3Func(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool RotorN3(CCodeVar& rLVar, TMultiV& rMV, TCVScalar fDeg, int iLine, int iPos);
	bool RotorN3(CCodeVar& rLVar, TCVScalar *pfVec, int iLine, int iPos);

	bool SphereN3(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool TranslatorN3(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool DilatorN3(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool DirVecN3(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);	
	
	bool DirVecP3Func(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);	
	bool DirVecP3(CCodeVar& rLVar, int iVal, int iLine, int iPos);
	bool DirVecP3(CCodeVar& rLVar, TMultiV& vA, int iLine, int iPos);

	bool AffineVecP3Func(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool OffsetFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool FactorBladeFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool AnalyzeMVFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool PlotFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool RepeatFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool ColorFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool SetArrowShapeFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool DrawPlaneFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool DrawBoxFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool DrawLineFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool DrawLineSurfaceFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool DrawPointFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool DrawPointSurfaceFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool DrawPointGridFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool DrawPointListFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool DrawArrowFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool DrawArcFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool DrawDiskFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool DrawEllipseFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool DrawEllipsoidFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool DrawCircleFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool DrawCircleSurfaceFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool DrawTriangleFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool DrawConeFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool DrawSphereFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool DrawCylinderFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool DrawIcosahedronFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool DrawQuantumFractalFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);


	bool EnableFogFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool SetFogColorFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool SetFogDensityFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	//bool SetBitmapFontFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool SetBitmapFontSizeFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool DrawTextFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool DrawLatexFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool GetLatexImageFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool SetLatexMagStepFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool SetRenderLatexFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool SetLatexAlignFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	void SetRenderLatex(bool bVal = true);
	void OverrideSetRenderLatex(bool bVal = true) { m_bOverrideSetRenderLatex = bVal; }

	bool SetTextPrecFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool SetEvalPrecFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool SetVisPrecFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool DrawBMPFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool NewBMPFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool LoadBMPFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool SaveBMPFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool EnableBMPTransparencyFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool SetBMPTransparentColorFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool SetBMPScaleFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool GetBMPCoordsFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool SetBMPAlignFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool SetBMPPosFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool BMP2MatrixFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool Matrix2BMPFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool CopyBMPFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool RotateBMPFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool ResizeBMPFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool ReshapeBMPFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool InsertBMPFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool SetBMPTextureFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool SetBMPTextureRepeatFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool EnableBMPAbsTexCoordsFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);


	bool StartOverlayFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool EndOverlayFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool StartViewFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool EndViewFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool PushFrameFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool PopFrameFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool DrawMatrix(TMatrix &xA, int iLine, int iPos);

	bool ClearScriptListFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool AddScriptToListFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool SetLineWidthFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool GetLineWidthFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool SetPointSizeFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool GetPointSizeFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool SetSmoothPointFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool GetSmoothPointFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool SetSmoothLineFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool GetSmoothLineFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool SetSmoothPolygonFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool GetSmoothPolygonFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool SetFrameOriginFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool SetFrameRotorFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool AddFrameOriginFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool AddFrameRotorFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	
	bool TranslateFrameFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool RotateFrameFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	
	bool TranslateViewFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool RotateViewFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool SetViewAngleFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool GetMVProductMatrixFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool GetMVMatrixFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool GetMatrixMVFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool DiagToVectorFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool VectorToDiagFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool ResizeMatrixFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool ReshapeMatrixFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool AddMatrixRowsFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool AddMatrixColsFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool ReduceMatrixToRowSpaceFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool ReduceMatrixToMajorRowsFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool ExtractMatrixRowsFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	
	bool ErrorPropagationOperationFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool ErrorPropVecN3Func(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool ErrorPropCrossCovVecN3Func(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool ErrorPropPrjVecN3ToE3Func(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool ErrorPropVecC2Func(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool ErrorPropSQRTFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool ErrorPropINVFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool ErrorPropMULFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	// File Functions 
	bool ReadDataFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool WriteDataFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool ReadMatrixFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	void WriteVariable(CXMLTree& rXMLTree, CCodeVar& rVar, int iPos) throw (CCLUException);
	bool WriteVariableFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	void ReadVariable(CCodeVar& rVar, CXMLElement &rEl) throw (CCLUException);
	bool ReadVariableFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool ReadXMLFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos) throw (CCLUException);
	void ReadXMLTree(TVarList& rList, CXMLTree& rTree, int iLine, int iPos) throw (CCLUException);

	bool ReadTextFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool WriteTextFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool ShowFileFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool FileChooserFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool SaveScreenFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	// Light Functions
	bool ResetLightFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool SetAmbientLightFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool EnableLightFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool EnableLightFixedFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool SetLightPosFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool SetLightAmbientColFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool SetLightDiffuseColFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool SetLightSpecularColFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool SetLightSpotDirFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool SetLightSpotExpFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool SetLightSpotCutOffFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool SetLightAttenuationFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool ToolSliderFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool ToolCounterFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool ToolInputFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool ToolInputTextFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool ToolCheckBoxFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool ToolButtonFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool ToolChoiceFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool SetInfoTextFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool SetInfoWidthFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	
	bool SetOGLSizeFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool SetMouseModeFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool GetMouseModeFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool SetMouseValueFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool GetMouseValueFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool SetMouseSensFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool GetMouseSensFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	bool GetObjectElements(CCodeVar &rVar, CCodeVar &rObj, TVarList &rIdxList, bool bInList, int iLine, int iPos);

	bool CombinationIndexListFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool GenerateCombinationIndices(MemObj<Mem<int> >& mCombIdx, 
						int iCount, int iChoose, int iFirstIdx, 
						int *piIdxList, int iIdxPos);

	bool PermutationIndexListFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool GeneratePermutationIndices(MemObj<Mem<int> >& mPermIdx, 
						Mem<int>& mCurPermIdx, Mem<int>& mIdxList, int iRest);

	// Serial Port Access
	bool OpenSerialPortFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool WriteToSerialPortFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool ReadFromSerialPortFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	// Misc Functions
	bool EnableAnimationFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool SetAnimationTimeStepFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
	bool GetTimeFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

	// Execute a user defined function
	bool ExecUserFunc(CCodeVar& rResVar, TCodePtr pCodePtr, TVarList &rParList, int iLine, int iPos);

	void ResetTempImageList() { m_mTempImageList[m_iActTempImageList].Set(0); }
	MemObj<TImage>& GetActTempImageList() { return m_mTempImageList[m_iActTempImageList]; }
	void ToggleTempImageList() 
	{ if (m_iActTempImageList) m_iActTempImageList = 0; else m_iActTempImageList = 1; }

protected:
	static bool StaticListSortCompare(int iLIdx, int iRIdx);
	bool ListSortCompare(int iLIdx, int iRIdx);

protected:
	static CCLUCodeBase *sm_pCurCodeBase;
	TVarList *m_pSortVarList;
	bool m_bSortAscend; 
	TCodePtr m_pSortCode;
	int m_iSortLine, m_iSortPos;

	TCVScalar m_fPi, m_fRadPerDeg;
	TCVScalar m_fSensitivity;

	int m_iPlotMode;

	E3GA<TCVScalar> m_E3GABase;
	PGA<TCVScalar> m_P3GABase;
	ConfGA<TCVScalar> m_N3GABase;
	El2GA<TCVScalar> m_El2GABase;

	COGLMVFilter *m_pFilter;
	Mem<STransform> *m_pTransform;
	MultiV<float> *m_pFrameRotor;
	CCLUDrawBase *m_pCLUDrawBase;

	MemObj<MemObj<TImage> > m_mTempImageList;
	int m_iActTempImageList;

	COGLLatexText m_oglLatex;
	int m_iLatexMagStep;

	float m_fVBMPAlign, m_fHBMPAlign;
	float m_fVLatexAlign, m_fHLatexAlign;
	COGLVertex m_xBMPPos;
	map<string, COGLBitmap> m_mapBitmap;

	vector<SScriptFile> m_vecScriptList;
	int m_iCurScript;

	vector<SViewData> m_vecViewStack;

	bool m_bOverrideSetRenderLatex;
	bool m_bOverlayModeActive;

	int m_iFrameStackDepth;

	bool m_bUseBMPTrans;
	COGLColor m_oglBMPTransCol;

	int m_iCurBitmapFont;
	int m_iCurBitmapFontSize;
	float m_fCurBitmapScale;
	vector<SFontData> m_vecFont;

	float m_fLineWidth, m_fPointSize;
	bool m_bSmoothPoint, m_bSmoothLine, m_bSmoothPolygon;

	TTimeStruct m_tmStart;

	TOutObjList m_vecOutputObject;

	ECurSpaceVars m_eCurSpaceVars;

	string m_sScriptPath;
	string m_sScriptName;

	Rand m_Random;

	vector<int> m_vecVersion;

	// Number of digit behind the decimal point
	int m_iTextPrec;

	TCVScalar m_dCurrentInfoWidth;

#ifdef WIN32
	TSerialIOMap m_mapSerialIO;
#endif
};

#endif // !defined(AFX_CLUCODEBASE_H__154A039C_A470_4AE1_99F2_F7D9072E1496__INCLUDED_)
