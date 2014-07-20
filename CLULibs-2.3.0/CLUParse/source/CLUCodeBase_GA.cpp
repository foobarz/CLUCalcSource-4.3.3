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

// CLUCodeBase.cpp: Implementierung der Klasse CCLUCodeBase.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "GL/glut.h"

#include "CLUCodeBase.h"


#define VAR_LIST_PAIR(rCodeVar, Val1, Val2) \
		rCodeVar.New(PDT_VARLIST, "Constant"); \
		{ \
			TVarList &rVex = *rCodeVar.GetVarListPtr(); \
			rVex.Set(2); \
			rVex[0] = Val1; \
			rVex[1] = Val2; \
		}


//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
/// Analyze a Multivector and return information about it.
///
/// params: 1. Multivector to be analyzed
///			2. (optional) boolean entry: if 1 draw mv, otherwise only analyze (default)
/// return: a list containing information about the mv.

bool CCLUCodeBase::AnalyzeMVFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	TCVCounter iVal = 0;

	if (iVarCount < 1 || iVarCount > 2)
	{
		int piVal[] = { 1, 2 };
		m_ErrorList.WrongNoOfParams(piVal, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_MULTIV)
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;		
	}

	if (iVarCount == 2 && !mVars(1).CastToCounter(iVal))
	{
		m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
		return false;		
	}

	bool bAnalyzeOnly = (iVal ? false : true);
	TMultiV &rA = *mVars(0).GetMultiVPtr();
	
	rVar.New(PDT_VARLIST, "Constant");
	TVarList &rList = *rVar.GetVarListPtr();

	// Analyze Multivector
	if (!m_pFilter->DrawMV(rA, bAnalyzeOnly))
	{
		// Could not be analyzed
		rList.Set(1);
		rList[0] = "unknown";
		return true;
	}

	CMVInfo<TCVScalar> Info;
	m_pFilter->GetMVInfo(Info);
	EMVType mvType = Info.m_eType;

/*
	GA_NONE, GA_SCALAR, 
	GA_POINT, GA_LINE, GA_PLANE, GA_SPACE,
	GA_POINTPAIR, GA_CIRCLE, GA_SPHERE, 
	GA_DIR1D, GA_DIR2D, GA_DIR3D,
	GA_ROTOR, GA_MOTOR, GA_TWIST, GA_TRANSLATOR,
	GA_ELLIPSE, GA_HYPERBOLA, GA_PARLINEPAIR, 
	GA_LINEPAIR, GA_POINTSET
*/

	if (mvType == GA_NONE)
	{
		rList.Set(1);
		rList[0] = "unknown";
	}
	else if (mvType == GA_SCALAR)
	{
		rList.Set(2);
		rList[0] = "scalar";

		VAR_LIST_PAIR(rList[1], "value", Info.m_mVal[0]);
	}
	else if (mvType == GA_POINT)
	{
		rList.Set(2);
		rList[0] = "point";

		VAR_LIST_PAIR(rList[1], "pos", m_E3GABase.Vec(Info.m_mVex[0].Data()));
	}
	else if (mvType == GA_POINTSET)
	{
		int i, iCount = Info.m_mVex.Count();

		rList.Set(2);
		rList[0] = "pointset";

		rList[1].New(PDT_VARLIST, "Constant");
		TVarList &rPointSet = *rList[1].GetVarListPtr(); 
		rPointSet.Set(2); 
		rPointSet[0] = "pos";

		rPointSet[1].New(PDT_VARLIST, "Constant");
		TVarList &rVex = *rPointSet[1].GetVarListPtr();
		rVex.Set(iCount);
		for (i = 0; i < iCount; i++)
		{
			rVex[i] = m_E3GABase.Vec(Info.m_mVex[i].Data());
		}
	}
	else if (mvType == GA_LINE)
	{
		rList.Set(4);
		rList[0] = "line";

		VAR_LIST_PAIR(rList[1], "mag", Info.m_mVal[0]);
		VAR_LIST_PAIR(rList[2], "pos", m_E3GABase.Vec(Info.m_mVex[0].Data()));
		VAR_LIST_PAIR(rList[3], "dir", m_E3GABase.Vec(Info.m_mVex[1].Data()));
	}
	else if (mvType == GA_INF_POINTPAIR)
	{
		rList.Set(4);
		rList[0] = "inf_point_pair";

		VAR_LIST_PAIR(rList[1], "mag", Info.m_mVal[0]);
		VAR_LIST_PAIR(rList[2], "pos", m_E3GABase.Vec(Info.m_mVex[0].Data()));
		VAR_LIST_PAIR(rList[3], "dir", m_E3GABase.Vec(Info.m_mVex[1].Data()));
	}
	else if (mvType == GA_PLANE)
	{
		rList.Set(5);
		rList[0] = "plane";

		VAR_LIST_PAIR(rList[1], "mag", Info.m_mVal[0]);
		VAR_LIST_PAIR(rList[2], "pos", m_E3GABase.Vec(Info.m_mVex[0].Data()));
		VAR_LIST_PAIR(rList[3], "dir1", m_E3GABase.Vec(Info.m_mVex[1].Data()));
		VAR_LIST_PAIR(rList[4], "dir2", m_E3GABase.Vec(Info.m_mVex[2].Data()));
	}
	else if (mvType == GA_INF_LINE)
	{
		rList.Set(5);
		rList[0] = "inf_line";

		VAR_LIST_PAIR(rList[1], "mag", Info.m_mVal[0]);
		VAR_LIST_PAIR(rList[2], "pos", m_E3GABase.Vec(Info.m_mVex[0].Data()));
		VAR_LIST_PAIR(rList[3], "dir1", m_E3GABase.Vec(Info.m_mVex[1].Data()));
		VAR_LIST_PAIR(rList[4], "dir2", m_E3GABase.Vec(Info.m_mVex[2].Data()));
	}
	else if (mvType == GA_SPACE)
	{
		rList.Set(2);
		rList[0] = "space";

		VAR_LIST_PAIR(rList[1], "value", Info.m_mVal[0]);
	}
	else if (mvType == GA_POINTPAIR)
	{
		TCVScalar fMag;
		TMultiV vA, vD;
	
		vA = m_E3GABase.Vec(Info.m_mVex[0].Data());
		vD = m_E3GABase.Vec(Info.m_mVex[1].Data());
		fMag = Info.m_mVal[0];
	
		rList.Set(4);
		rList[0] = "point_pair";

		VAR_LIST_PAIR(rList[1], "mag", fMag);
		VAR_LIST_PAIR(rList[2], "pos1", vA + (Mag(fMag)^vD));
		VAR_LIST_PAIR(rList[3], "pos2", vA - (Mag(fMag)^vD));
	}
	else if (mvType == GA_DIR1D)
	{
		rList.Set(3);
		rList[0] = "dir_1d";

		VAR_LIST_PAIR(rList[1], "mag", Info.m_mVal[0]);
		VAR_LIST_PAIR(rList[2], "dir", m_E3GABase.Vec(Info.m_mVex[0].Data()));
	}
	else if (mvType == GA_DIR2D)
	{
		rList.Set(4);
		rList[0] = "dir_2d";

		VAR_LIST_PAIR(rList[1], "mag", Info.m_mVal[0]);
		VAR_LIST_PAIR(rList[2], "dir1", m_E3GABase.Vec(Info.m_mVex[0].Data()));
		VAR_LIST_PAIR(rList[3], "dir2", m_E3GABase.Vec(Info.m_mVex[1].Data()));
	}
	else if (mvType == GA_DIR3D)
	{
		rList.Set(5);
		rList[0] = "dir_3d";

		VAR_LIST_PAIR(rList[1], "mag", Info.m_mVal[0]);
		VAR_LIST_PAIR(rList[2], "dir1", m_E3GABase.Vec(Info.m_mVex[0].Data()));
		VAR_LIST_PAIR(rList[3], "dir2", m_E3GABase.Vec(Info.m_mVex[1].Data()));
		VAR_LIST_PAIR(rList[4], "dir3", m_E3GABase.Vec(Info.m_mVex[2].Data()));
	}
	else if (mvType == GA_CIRCLE)
	{
		TCVScalar fMag;
		TMultiV vC, vN, vU;
		MemObj<TMultiV> mvList;
	
		vC = m_E3GABase.Vec(Info.m_mVex[0].Data());
		vN = m_E3GABase.Vec(Info.m_mVex[1].Data());

		fMag = Info.m_mVal[0];
		
		vU = vN & m_E3GABase.vI;
		if (!FactorBlade<TCVScalar>(vU, mvList, true, 0, 0, m_fSensitivity))
		{
			m_ErrorList.GeneralError(
				"\nUnfortunately there still seems\n"
				"to be a bug in the analysis of\n"
				"multivectors in conformal space.\n"
				"Apparently a multivector was \n"
				"incorrectly analyzed as a circle.\n\n"
				"Please send me an email with the\n"
				"script where the error occured.\n"
				"I will try to fix this problem\n"
				"as soon as possible.\n", iLine, iPos);
			return false;
		}

		rList.Set(5);
		rList[0] = "circle";

		VAR_LIST_PAIR(rList[1], "radius", Info.m_mVal[0]);
		VAR_LIST_PAIR(rList[2], "pos", vC);
		VAR_LIST_PAIR(rList[3], "dir1", mvList[0]);
		VAR_LIST_PAIR(rList[4], "dir2", mvList[1]);
	}
	else if (mvType == GA_SPHERE)
	{
		rList.Set(3);
		rList[0] = "sphere";

		VAR_LIST_PAIR(rList[1], "radius", Info.m_mVal[0]);
		VAR_LIST_PAIR(rList[2], "pos", m_E3GABase.Vec(Info.m_mVex[0].Data()));
	}
	else if (mvType == GA_ELLIPSE)
	{
		rList.Set(4);
		rList[0] = "ellipse";

		VAR_LIST_PAIR(rList[1], "pos", m_E3GABase.Vec(Info.m_mVex[2].Data()));
		VAR_LIST_PAIR(rList[2], "dir1", m_E3GABase.Vec(Info.m_mVex[0].Data()));
		VAR_LIST_PAIR(rList[3], "dir2", m_E3GABase.Vec(Info.m_mVex[1].Data()));
	}
	else if (mvType == GA_HYPERBOLA)
	{
		rList.Set(4);
		rList[0] = "hyperbola";

		VAR_LIST_PAIR(rList[1], "pos", m_E3GABase.Vec(Info.m_mVex[2].Data()));
		VAR_LIST_PAIR(rList[2], "dir1", m_E3GABase.Vec(Info.m_mVex[0].Data()));
		VAR_LIST_PAIR(rList[3], "dir2", m_E3GABase.Vec(Info.m_mVex[1].Data()));
	}
	else if (mvType == GA_LINEPAIR)
	{
		rList.Set(4);
		rList[0] = "line_pair";

		//VAR_LIST_PAIR(rList[1], "mag", Info.m_mVal[0]);
		VAR_LIST_PAIR(rList[1], "pos", m_E3GABase.Vec(Info.m_mVex[0].Data()));
		VAR_LIST_PAIR(rList[2], "dir1", m_E3GABase.Vec(Info.m_mVex[1].Data()));
		VAR_LIST_PAIR(rList[3], "dir2", m_E3GABase.Vec(Info.m_mVex[2].Data()));
	}
	else if (mvType == GA_PARLINEPAIR)
	{
		TCVScalar fMag;
		TMultiV vA, vD;
	
		vA = m_E3GABase.Vec(Info.m_mVex[2].Data());
		vD = m_E3GABase.Vec(Info.m_mVex[0].Data());
		fMag = Info.m_mVal[0];

		rList.Set(5);
		rList[0] = "line_pair_parallel";

		VAR_LIST_PAIR(rList[1], "mag", fMag);
		VAR_LIST_PAIR(rList[2], "pos1", vA + (Mag(fMag) ^ vD) );
		VAR_LIST_PAIR(rList[3], "pos2", vA - (Mag(fMag) ^ vD) );
		VAR_LIST_PAIR(rList[4], "dir", m_E3GABase.Vec(Info.m_mVex[1].Data()));
	}
	else if (mvType == GA_TRANSLATOR)
	{
		TMultiV vA;
	
		vA = m_E3GABase.Vec(Info.m_mVex[0].Data());

		rList.Set(2);
		rList[0] = "translator";

		VAR_LIST_PAIR(rList[1], "dir", vA);
	}
	else if (mvType == GA_MOTOR)
	{
		TCVScalar fMag;
		TMultiV vA, vD;
	
		vA = m_E3GABase.Vec(Info.m_mVex[0].Data());
		vD = m_E3GABase.Vec(Info.m_mVex[1].Data());
		fMag = Info.m_mVal[0];

		rList.Set(4);
		rList[0] = "motor";

		VAR_LIST_PAIR(rList[1], "angle", fMag);
		VAR_LIST_PAIR(rList[2], "axis", vD);
		VAR_LIST_PAIR(rList[3], "offset", vA);
	}
	else if (mvType == GA_TWIST)
	{
		TCVScalar fMag;
		TMultiV vTp, vTr, vR;
	
		vTp = m_E3GABase.Vec(Info.m_mVex[0].Data());
		vTr = m_E3GABase.Vec(Info.m_mVex[1].Data());
		vR = m_E3GABase.Vec(Info.m_mVex[2].Data());
		fMag = Info.m_mVal[0];

		rList.Set(5);
		rList[0] = "twist";

		VAR_LIST_PAIR(rList[1], "angle", fMag);
		VAR_LIST_PAIR(rList[2], "axis", vR);
		VAR_LIST_PAIR(rList[3], "offset", vTp);
		VAR_LIST_PAIR(rList[4], "trans", vTr);
	}
	else if (mvType == GA_ROTOR)
	{
		TCVScalar fMag;
		TMultiV vA, vD;
	
		vA = m_E3GABase.Vec(Info.m_mVex[0].Data());
		fMag = Info.m_mVal[0];

		rList.Set(3);
		rList[0] = "rotor";

		VAR_LIST_PAIR(rList[1], "angle", fMag);
		VAR_LIST_PAIR(rList[2], "axis", vA);
	}
	else
	{
		rList.Set(1);
		rList[0] = "unknown";
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Generate a Mask as needed for GetMVProductMatrix from a multivector.
///
/// params: Multivector whose non-zero components give elements of mask.
/// return: a list containing the mask

bool CCLUCodeBase::GenerateMaskFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();

	if (iVarCount != 1)
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;		
	}
	
	if (mVars(0).BaseType() != PDT_MULTIV)
	{
		m_ErrorList.InvalidType(mVars(0), iLine, iPos);
		return false;		
	}

	TMultiV &vA = *mVars(0).GetMultiVPtr();
	
	rVar.New(PDT_VARLIST, "Mask");
	TVarList &IdxList = *rVar.GetVarListPtr();
	
	vA.TinyToZero();
	int iIdx, iMaskIdx, iGADim = vA.GetGADim();
	IdxList.Set(iGADim);

	for (iIdx = 0, iMaskIdx = 0; iIdx < iGADim; iIdx++)
	{
		if (vA[iIdx] != 0)
		{
			IdxList[iIdx] = ++iMaskIdx;
		}
		else
		{
			IdxList[iIdx] = TCVCounter(0);
		}
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Get indices of those components of a multivector that are
/// non-zero.
///
/// params: multivector
/// return: list of indices

bool CCLUCodeBase::GetMVIndicesFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();

	if (iVarCount != 1)
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;		
	}
	
	if (mVars(0).BaseType() != PDT_MULTIV)
	{
		m_ErrorList.InvalidType(mVars(0), iLine, iPos);
		return false;		
	}
	
	TMultiV &vA = *mVars(0).GetMultiVPtr();
	
	rVar.New(PDT_VARLIST, "IndexList");
	TVarList &IdxList = *rVar.GetVarListPtr();
	
	vA.TinyToZero();
	int iIdx, iGADim = vA.GetGADim();
	
	for (iIdx = 0; iIdx < iGADim; iIdx++)
	{
		if (vA[iIdx] != 0)
		{
			IdxList.Add(1);
			IdxList.Last() = iIdx+1;
		}
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Normalize homogeneous vectors
	
bool CCLUCodeBase::AffineVecP3Func(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!m_pFilter)
	{
		m_ErrorList.Internal(iLine, iPos);
		return false;
	}
	
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	
	// Recurse on list
	if (iVarCount == 1 && mVars(0).BaseType() == PDT_VARLIST)
	{
		return AffineVecP3Func(rVar, mVars(0), iLine, iPos);
	}
	
	if (!rVar.New(PDT_VARLIST, "Constant"))
	{
		m_ErrorList.OutOfMemory(iLine, iPos);
		return false;
	}
	
	TVarList &VarList = *rVar.GetVarListPtr();
	
	if (!VarList.Set(iVarCount))
	{
		m_ErrorList.OutOfMemory(iLine, iPos);
		return false;
	}
	
	TMultiV vX;
	TCVScalar dH;
	int i;
	
	for(i=0;i<iVarCount;i++)
	{
		if (mVars(i).BaseType() != PDT_MULTIV)
		{
			m_ErrorList.InvalidType(mVars(i), iLine, iPos);
			return false;
		}
		
		vX = *mVars(i).GetMultiVPtr();
		dH = vX[4];
		
		if (dH != 0.0)
			vX /= dH;
		
		VarList[i] = vX;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Delete Variables specific to the vector spaces

void CCLUCodeBase::ResetSpaceVars()
{
	switch (m_eCurSpaceVars)
	{
	case SPACEVARS_E3:
		DeleteConstVar("id");
		DeleteConstVar("e1");
		DeleteConstVar("e2");
		DeleteConstVar("e3");
		DeleteConstVar("I");
		break;

	case SPACEVARS_P3:
		DeleteConstVar("id");
		DeleteConstVar("e1");
		DeleteConstVar("e2");
		DeleteConstVar("e3");
		DeleteConstVar("e4");
		DeleteConstVar("I");
		break;

	case SPACEVARS_N3:
		DeleteConstVar("id");
		DeleteConstVar("e1");
		DeleteConstVar("e2");
		DeleteConstVar("e3");
		DeleteConstVar("ep");
		DeleteConstVar("em");
		DeleteConstVar("e");
		DeleteConstVar("einf");
		DeleteConstVar("e0");
		DeleteConstVar("ni");
		DeleteConstVar("no");
		DeleteConstVar("n");
		DeleteConstVar("nb");
		DeleteConstVar("I");
		break;

	case SPACEVARS_C2:
		DeleteConstVar("id");
		DeleteConstVar("e1");
		DeleteConstVar("e2");
		DeleteConstVar("e3");
		DeleteConstVar("e4");
		DeleteConstVar("e5");
		DeleteConstVar("e6");
		DeleteConstVar("I");
		break;
	}
	
	m_eCurSpaceVars = SPACEVARS_NONE;
}

//////////////////////////////////////////////////////////////////////
/// Define Variables specific to E3

bool CCLUCodeBase::DefVarsE3(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	
	if (iVarCount != 0)
	{
		m_ErrorList.WrongNoOfParams(0, iLine, iPos);
		return false;
	}

	if (m_eCurSpaceVars == SPACEVARS_E3)
		return true;

	ResetSpaceVars();

	(NewConstVar("id", PDT_MULTIV) = m_E3GABase.vSC).EnableProtect();
	(NewConstVar("e1", PDT_MULTIV) = m_E3GABase.vE1).EnableProtect();
	(NewConstVar("e2", PDT_MULTIV) = m_E3GABase.vE2).EnableProtect();
	(NewConstVar("e3", PDT_MULTIV) = m_E3GABase.vE3).EnableProtect();
	(NewConstVar("I", PDT_MULTIV) = m_E3GABase.vI).EnableProtect();
	
	m_eCurSpaceVars = SPACEVARS_E3;

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Define Variables specific to P3

bool CCLUCodeBase::DefVarsP3(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList *pmVars = rPars.GetVarListPtr();
	

	int iVarCount = pmVars->Count();
	
	if (iVarCount != 0)
	{
		m_ErrorList.WrongNoOfParams(0, iLine, iPos);
		return false;
	}
	
	if (m_eCurSpaceVars == SPACEVARS_P3)
		return true;

	ResetSpaceVars();

	(NewConstVar("id", PDT_MULTIV) = m_P3GABase.vSC).EnableProtect();
	(NewConstVar("e1", PDT_MULTIV) = m_P3GABase.vE1).EnableProtect();
	(NewConstVar("e2", PDT_MULTIV) = m_P3GABase.vE2).EnableProtect();
	(NewConstVar("e3", PDT_MULTIV) = m_P3GABase.vE3).EnableProtect();
	(NewConstVar("e4", PDT_MULTIV) = m_P3GABase.vE4).EnableProtect();
	(NewConstVar("I", PDT_MULTIV) = m_P3GABase.vI).EnableProtect();
	
	m_eCurSpaceVars = SPACEVARS_P3;
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Define Variables specific to N3

bool CCLUCodeBase::DefVarsN3(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList *pmVars = rPars.GetVarListPtr();
	
	int iVarCount = pmVars->Count();
	
	if (iVarCount != 0)
	{
		m_ErrorList.WrongNoOfParams(0, iLine, iPos);
		return false;
	}
	
	if (m_eCurSpaceVars == SPACEVARS_N3)
		return true;

	ResetSpaceVars();	

	(NewConstVar("id", PDT_MULTIV) = m_N3GABase.vSC).EnableProtect();
	(NewConstVar("e1", PDT_MULTIV) = m_N3GABase.vE1).EnableProtect();
	(NewConstVar("e2", PDT_MULTIV) = m_N3GABase.vE2).EnableProtect();
	(NewConstVar("e3", PDT_MULTIV) = m_N3GABase.vE3).EnableProtect();
	(NewConstVar("ep", PDT_MULTIV) = m_N3GABase.vEp).EnableProtect();
	(NewConstVar("em", PDT_MULTIV) = m_N3GABase.vEm).EnableProtect();
	(NewConstVar("e", PDT_MULTIV) = m_N3GABase.vE).EnableProtect();
	(NewConstVar("einf", PDT_MULTIV) = m_N3GABase.vE).EnableProtect();
	(NewConstVar("e0", PDT_MULTIV) = m_N3GABase.vE0).EnableProtect();
	(NewConstVar("ni", PDT_MULTIV) = m_N3GABase.vN).EnableProtect();
	(NewConstVar("no", PDT_MULTIV) = m_N3GABase.vNb).EnableProtect();
	(NewConstVar("n", PDT_MULTIV) = m_N3GABase.vN).EnableProtect();
	(NewConstVar("nb", PDT_MULTIV) = m_N3GABase.vNb).EnableProtect();
	(NewConstVar("I", PDT_MULTIV) = m_N3GABase.vI).EnableProtect();
	
	m_eCurSpaceVars = SPACEVARS_N3;

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Define Variables specific to Ellipsoidal 2d space

bool CCLUCodeBase::DefVarsEl2(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	
	TVarList& mVars = *rPars.GetVarListPtr();
//	

	int iVarCount = mVars.Count();
	
	if (iVarCount != 0)
	{
		m_ErrorList.WrongNoOfParams(0, iLine, iPos);
		return false;
	}

	if (m_eCurSpaceVars == SPACEVARS_C2)
		return true;

	ResetSpaceVars();

	(NewConstVar("id", PDT_MULTIV) = m_El2GABase.vSC).EnableProtect();
	(NewConstVar("e1", PDT_MULTIV) = m_El2GABase.vE1).EnableProtect();
	(NewConstVar("e2", PDT_MULTIV) = m_El2GABase.vE2).EnableProtect();
	(NewConstVar("e3", PDT_MULTIV) = m_El2GABase.vE3).EnableProtect();
	(NewConstVar("e4", PDT_MULTIV) = m_El2GABase.vE4).EnableProtect();
	(NewConstVar("e5", PDT_MULTIV) = m_El2GABase.vE5).EnableProtect();
	(NewConstVar("e6", PDT_MULTIV) = m_El2GABase.vE6).EnableProtect();
	(NewConstVar("I", PDT_MULTIV) = m_El2GABase.vI).EnableProtect();

	m_eCurSpaceVars = SPACEVARS_C2;

	return true;
}



//////////////////////////////////////////////////////////////////////
// Create E3 Vector

bool CCLUCodeBase::VecE3Func(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList *pmVars = rPars.GetVarListPtr();
	
	int iVarCount = pmVars->Count();
	
	if (iVarCount == 2 || iVarCount == 3)
	{
		TCVScalar pfVec[3];
		int i;
		
		for(i=0;i<iVarCount;i++)
		{
			if ( !((*pmVars)(i).CastToScalar(pfVec[i], m_fSensitivity)) )
			{
				m_ErrorList.InvalidType((*pmVars)(i), iLine, iPos);
				return false;
			}
		}

		if (iVarCount == 2)
			pfVec[2] = TCVScalar(0);
		
		if (!VecE3(rVar, pfVec, iLine, iPos))
			return false; // VecE3 sets error
	}
	else if (iVarCount == 1)
	{
		TCVCounter iVal;
		
		// If Parameter is a list of values
		// call VecE3 with this list of values.
		if ((*pmVars)(0).BaseType() == PDT_VARLIST)
		{
			int i, iCount;
			TVarList& rParList = *((*pmVars)(0).GetVarListPtr());

			iCount = rParList.Count();
			if (rParList(0).BaseType() == PDT_SCALAR ||
				rParList(0).BaseType() == PDT_COUNTER)
			{
				return VecE3Func(rVar, (*pmVars)(0), iLine, iPos);
			}
			else
			{
				rVar.New(PDT_VARLIST, "Constant");
				TVarList &rList = *rVar.GetVarListPtr();
				rList.Set(iCount);

				for (i = 0; i < iCount; i++)
				{
					if (rParList(i).BaseType() == PDT_VARLIST)
					{
						if (!VecE3Func(rList[i], rParList(i), iLine, iPos))
							return false;
					}
					else
					{
						if (rParList(i).BaseType() == PDT_MULTIV)
						{
							if (!VecE3(rList[i], *((TMultiV *) rParList(i).GetMultiVPtr()), iLine, iPos))
								return false;
						}
						else
						{
							m_ErrorList.InvalidType((*pmVars)(0), iLine, iPos);
							return false;
						}	
					}
				}
			}

			return true;
		}
		
		if ((*pmVars)(0).CastToCounter(iVal))
		{
			if (!VecE3(rVar, iVal, iLine, iPos))
				return false; // VecE3 sets error		
		}
		else if ((*pmVars)(0).BaseType() == PDT_MULTIV)
		{
			if (!VecE3(rVar, *((TMultiV *) (*pmVars)(0).GetMultiVPtr()), iLine, iPos))
				return false;
		}
		else
		{
			m_ErrorList.InvalidType((*pmVars)(0), iLine, iPos);
			return false;
		}	
	}
	else
	{
		int piVal[] = { 1, 2, 3 };
		m_ErrorList.WrongNoOfParams(piVal, 2, iLine, iPos);
		return false;
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////
// Create E3 Vector

bool CCLUCodeBase::VecE3(CCodeVar& rLVar, TCVScalar *pfVec, int iLine, int iPos)
{
	if (!pfVec)
		return false;
	
	if (!rLVar.New(PDT_MULTIV, "Constant"))
	{
		m_ErrorList.Internal(iLine, iPos);
		return false;
	}
	
	rLVar = m_E3GABase.Vec(pfVec);
	
	return true;
}

//////////////////////////////////////////////////////////////////////
// Create E3 Vector from transform variable

bool CCLUCodeBase::VecE3(CCodeVar& rLVar, int iVal, int iLine, int iPos)
{
	if (!m_pTransform)
		return false;
	
	if (!rLVar.New(PDT_MULTIV, "Constant"))
	{
		m_ErrorList.Internal(iLine, iPos);
		return false;
	}
	
	if (iVal < 0 || iVal > 9)
	{
		cStr csText;
		csText = iVal;
		m_ErrorList.InvalidParVal(csText.Str(), 1, iLine, iPos);
		return false;
	}
	
	TCVScalar pVec[3];
	if (m_pCLUDrawBase->Is2dView())
	{
		pVec[0] = (TCVScalar) (*m_pTransform)[iVal].pfTrans[0];
		pVec[1] = -(TCVScalar) (*m_pTransform)[iVal].pfTrans[2];
		pVec[2] = (TCVScalar) 0;
	}
	else
	{
		for(int i=0;i<3;i++)
			pVec[i] = (TCVScalar) (*m_pTransform)[iVal].pfTrans[i];
	}

	rLVar = m_E3GABase.Vec(pVec);
	
	return true;
}

//////////////////////////////////////////////////////////////////////
// Cast Multivector to E3

bool CCLUCodeBase::CastMVtoE3(const TMultiV& vA, TMultiV& vR)
{
	TMultiV vB;
	vR.SetStyle(m_E3GABase.GetMVStyle());
	int iID = (int) vA.GetBase().BaseID();
	
	if (iID == ID_E3GA)
	{
		vR = vA;
	}
	else if (iID == ID_PGA)
	{
		vB = PGA<TCVScalar>::Vec(vA);
		vR[1] = vB[1];
		vR[2] = vB[2];
		vR[3] = vB[3];
	}
	else if (iID == ID_CONFGA)
	{
		if ( !::IsZero(Scalar(vA * m_N3GABase.vE), m_pFilter->GetDoubleSens()) )
		{
			vB = ConfGA<TCVScalar>::MakeEuclidVec(vA);
			vR[1] = vB[1];
			vR[2] = vB[2];
			vR[3] = vB[3];
		}
		else vR = 0;
	}
	else if (iID == ID_EL2GA)
	{
		CMVInfo<float> Info;

		// Analyze Multivector
		m_pFilter->DrawMV(vA, true);
		Info = m_pFilter->GetMVInfo();
		if (Info.m_eType == GA_POINT)
		{
			vR = m_E3GABase.Vec(TCVScalar(Info.m_mVex[0][0]), 
								TCVScalar(Info.m_mVex[0][1]), 
								TCVScalar(Info.m_mVex[0][2]));
		}
		else vR = 0;
	}
	else return false;
	
	return true;
}


//////////////////////////////////////////////////////////////////////
// Cast Multivector to P3

bool CCLUCodeBase::CastMVtoP3(const TMultiV& vA, TMultiV& vR)
{
	TMultiV vB;
	int iID = (int) vA.GetBase().BaseID();
	
	if (iID == ID_E3GA)
	{
		vR = PGA<TCVScalar>::hVec(vA[1], vA[2], vA[3], TCVScalar(1));
	}
	else if (iID == ID_PGA)
	{
		vR = vA;
	}
	else if (iID == ID_CONFGA)
	{
		if ( !::IsZero(Scalar(vA * m_N3GABase.vE), m_pFilter->GetDoubleSens()) )
		{
			vB = ConfGA<TCVScalar>::MakeEuclidVec(vA);
			vR = PGA<TCVScalar>::hVec(vB[1], vB[2], vB[3], TCVScalar(1));
		}
		else
		{
			vB = ConfGA<TCVScalar>::MakeEuclidVec(vA);
			vR = PGA<TCVScalar>::hVec(vB[1], vB[2], vB[3], TCVScalar(0));			
		}
	}
	else if (iID == ID_EL2GA)
	{
		CMVInfo<float> Info;

		// Analyze Multivector
		m_pFilter->DrawMV(vA, true);
		Info = m_pFilter->GetMVInfo();
		if (Info.m_eType == GA_POINT)
		{
			vR = m_P3GABase.hVec(TCVScalar(Info.m_mVex[0][0]), 
								TCVScalar(Info.m_mVex[0][1]), 
								TCVScalar(Info.m_mVex[0][2]), 
								TCVScalar(1));
		}
		else 
		{
			vR = m_P3GABase.vZero;
		}
	}
	else
	{
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
// Convert N3,P3 vector into E3 Vector

bool CCLUCodeBase::VecE3(CCodeVar& rLVar, TMultiV& vA, int iLine, int iPos)
{
	TMultiV vR;
	
	if (!CastMVtoE3(vA, vR))
	{
		m_ErrorList.Internal(iLine, iPos);
		return false;
	}
	
	rLVar = vR.TinyToZero(m_fSensitivity);
	
	return true;
}


//////////////////////////////////////////////////////////////////////
// Create P3 Vector

bool CCLUCodeBase::VecP3Func(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList *pmVars = rPars.GetVarListPtr();
	
	int iVarCount = pmVars->Count();
	
	if (iVarCount >= 2 && iVarCount <= 4)
	{
		TCVScalar pfVec[4];
		int i;
		
		for(i=0;i<iVarCount;i++)
		{
			if ( !((*pmVars)(i).CastToScalar(pfVec[i], m_fSensitivity)) )
			{
				m_ErrorList.InvalidType((*pmVars)(i), iLine, iPos);
				return false;
			}
		}
		if (iVarCount < 3)
			pfVec[2] = TCVScalar(0);

		if (iVarCount < 4)
			pfVec[3] = TCVScalar(1);

		if (!VecP3(rVar, pfVec, iLine, iPos))
			return false; // VecE3 sets error
	}
	else if (iVarCount == 1)
	{
		TCVCounter iVal;
		
		// If Parameter is a list of values
		// call VecE3 with this list of values.
		if ((*pmVars)(0).BaseType() == PDT_VARLIST)
		{
			int i, iCount;
			TVarList& rParList = *((*pmVars)(0).GetVarListPtr());

			iCount = rParList.Count();
			if (rParList(0).BaseType() == PDT_SCALAR ||
				rParList(0).BaseType() == PDT_COUNTER)
			{
				return VecP3Func(rVar, (*pmVars)(0), iLine, iPos);
			}
			else
			{
				rVar.New(PDT_VARLIST, "Constant");
				TVarList &rList = *rVar.GetVarListPtr();
				rList.Set(iCount);

				for (i = 0; i < iCount; i++)
				{
					if (rParList(i).BaseType() == PDT_VARLIST)
					{
						if (!VecP3Func(rList[i], rParList(i), iLine, iPos))
							return false;
					}
					else
					{
						if (rParList(i).BaseType() == PDT_MULTIV)
						{
							if (!VecP3(rList[i], *((TMultiV *) rParList(i).GetMultiVPtr()), iLine, iPos))
								return false;
						}
						else
						{
							m_ErrorList.InvalidType((*pmVars)(0), iLine, iPos);
							return false;
						}	
					}
				}
			}

			return true;
		}
		
		if ((*pmVars)(0).CastToCounter(iVal))
		{
			if (!VecP3(rVar, iVal, iLine, iPos))
				return false; // VecE3 sets error		
		}
		else if ((*pmVars)(0).BaseType() == PDT_MULTIV)
		{
			if (!VecP3(rVar, *((TMultiV *) (*pmVars)(0).GetMultiVPtr()), iLine, iPos))
				return false;
		}
		else
		{
			m_ErrorList.InvalidType((*pmVars)(0), iLine, iPos);
			return false;
		}	
	}
	else
	{
		int piVal[] = { 1, 2, 3, 4 };
		m_ErrorList.WrongNoOfParams(piVal, 2, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
// Create P3 Vector

bool CCLUCodeBase::VecP3(CCodeVar& rLVar, TCVScalar *pfVec, int iLine, int iPos)
{
	if (!pfVec)
		return false;
	
	if (!rLVar.New(PDT_MULTIV, "Constant"))
	{
		m_ErrorList.Internal(iLine, iPos);
		return false;
	}
	
	rLVar = m_P3GABase.hVec(pfVec);
	
	return true;
}

//////////////////////////////////////////////////////////////////////
// Create P3 Vector from transform variable

bool CCLUCodeBase::VecP3(CCodeVar& rLVar, int iVal, int iLine, int iPos)
{
	if (!m_pTransform)
		return false;
	
	if (!rLVar.New(PDT_MULTIV, "Constant"))
	{
		m_ErrorList.Internal(iLine, iPos);
		return false;
	}
	
	if (iVal < 0 || iVal > 9)
	{
		cStr csText;
		csText = iVal;
		m_ErrorList.InvalidParVal(csText.Str(), 1, iLine, iPos);
		return false;
	}
	
	float *pfTrans = (*m_pTransform)[iVal].pfTrans;
	TCVScalar pfVec[4];
	
	if (m_pCLUDrawBase->Is2dView())
	{
		pfVec[0] = (TCVScalar) pfTrans[0];
		pfVec[1] = -(TCVScalar) pfTrans[2];
		pfVec[2] = (TCVScalar) 0.0;
		pfVec[3] = (TCVScalar) 1.0;
	}
	else
	{
		pfVec[0] = (TCVScalar) pfTrans[0];
		pfVec[1] = (TCVScalar) pfTrans[1];
		pfVec[2] = (TCVScalar) pfTrans[2];
		pfVec[3] = (TCVScalar) 1.0;
	}
	
	rLVar = m_P3GABase.hVec(pfVec);
	
	return true;
}

//////////////////////////////////////////////////////////////////////
// Convert N3,E3 vector into P3 Vector

bool CCLUCodeBase::VecP3(CCodeVar& rLVar, TMultiV& vA, int iLine, int iPos)
{
	TMultiV vR;
	
	if (!CastMVtoP3(vA, vR))
	{
		m_ErrorList.Internal(iLine, iPos);
		return false;
	}
	
	rLVar = vR.TinyToZero(m_fSensitivity);

/*
	TMultiV vR(m_P3GABase.GetMVStyle());
	int iID = (int) vA.GetBase().BaseID();
	
	if (iID == ID_E3GA)
	{
		vR = PGA<TCVScalar>::hVec(vA[1], vA[2], vA[3], TCVScalar(1));
	}
	else if (iID == ID_PGA)
	{
		vR = vA;
	}
	else if (iID == ID_CONFGA)
	{
		if (::Round(Scalar(vA * m_N3GABase.vE), m_pFilter->GetDoubleSens()) != 0)
		{
			vA = ConfGA<TCVScalar>::MakeEuclidVec(vA);
			vR = PGA<TCVScalar>::hVec(vA[1], vA[2], vA[3], TCVScalar(1));
		}
		else
		{
			vA = ConfGA<TCVScalar>::MakeEuclidVec(vA);
			vR = PGA<TCVScalar>::hVec(vA[1], vA[2], vA[3], TCVScalar(0));			
		}
	}
	else
	{
		m_ErrorList.Internal(iLine, iPos);
		return false;
	}
	
	rLVar = vR.TinyToZero(m_fSensitivity);
*/	
	return true;
}


//////////////////////////////////////////////////////////////////////
// Create N3 Vector

bool CCLUCodeBase::VecN3Func(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList *pmVars = rPars.GetVarListPtr();
	int iVarCount = pmVars->Count();
	TCVCounter iVal;
	TCVScalar dVal;

	if (iVarCount == 2 || iVarCount == 3)
	{
		if (!(*pmVars)(0).CastToScalar(dVal))
		{
			m_ErrorList.GeneralError("Expect scalar values if more than one parameter is specified.", iLine, iPos);
			return false;
		}

		if (!VecN3(rVar, *pmVars, iLine, iPos))
			return false;
	}
	else if (iVarCount == 1)
	{
		// If Parameter is a list of values
		// call VecN3 with this list of values.
		if ((*pmVars)(0).BaseType() == PDT_VARLIST)
		{
			if (!VecN3(rVar, *((*pmVars)(0).GetVarListPtr()), iLine, iPos))
				return false;
		}
		else if ((*pmVars)(0).CastToCounter(iVal))
		{
			if (!VecN3(rVar, iVal, iLine, iPos))
				return false; // VecE3 sets error		
		}
		else if ((*pmVars)(0).BaseType() == PDT_MULTIV)
		{
			if (!VecN3(rVar, *((TMultiV *) (*pmVars)(0).GetMultiVPtr()), iLine, iPos))
				return false;
		}
		else
		{
			m_ErrorList.InvalidType((*pmVars)(0), iLine, iPos);
			return false;
		}	
	}
	else
	{
		int piVal[] = { 1, 2, 3 };
		m_ErrorList.WrongNoOfParams(piVal, 3, iLine, iPos);
		return false;
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////
// Create N3 Vector

bool CCLUCodeBase::VecN3(CCodeVar& rVar, TVarList& rVarList, int iLine, int iPos)
{
	int i, iCount;

	iCount = rVarList.Count();

	if (rVarList(0).BaseType() == PDT_SCALAR ||
		rVarList(0).BaseType() == PDT_COUNTER)
	{
		TCVScalar pfVec[3];
		int i;
		
		for(i=0;i<iCount;i++)
		{
			if ( !(rVarList(i).CastToScalar(pfVec[i], m_fSensitivity)) )
			{
				m_ErrorList.InvalidType(rVarList(i), iLine, iPos);
				return false;
			}
		}

		if (iCount == 2)
			pfVec[2] = TCVScalar(0);
		
		if (!VecN3(rVar, pfVec, iLine, iPos))
			return false; // VecE3 sets error
	}
	else
	{
		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rList = *rVar.GetVarListPtr();
		rList.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (rVarList(i).BaseType() == PDT_VARLIST)
			{
				if (!VecN3(rList[i], *rVarList(i).GetVarListPtr(), iLine, iPos))
					return false;
			}
			else
			{
				if (rVarList(i).BaseType() == PDT_MULTIV)
				{
					if (!VecN3(rList[i], *((TMultiV *) rVarList(i).GetMultiVPtr()), iLine, iPos))
						return false;
				}
				else
				{
					m_ErrorList.InvalidType(rVarList(i), iLine, iPos);
					return false;
				}	
			}
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Create N3 Vector

bool CCLUCodeBase::VecN3(CCodeVar& rLVar, TCVScalar *pfVec, int iLine, int iPos)
{
	if (!pfVec)
		return false;
	
	if (!rLVar.New(PDT_MULTIV, "Constant"))
	{
		m_ErrorList.Internal(iLine, iPos);
		return false;
	}
	
	rLVar = m_N3GABase.NullVec(pfVec);
	
	return true;
}

//////////////////////////////////////////////////////////////////////
// Create N3 Vector from transform variable

bool CCLUCodeBase::VecN3(CCodeVar& rLVar, int iVal, int iLine, int iPos)
{
	if (!m_pTransform)
		return false;
	
	if (!rLVar.New(PDT_MULTIV, "Constant"))
	{
		m_ErrorList.Internal(iLine, iPos);
		return false;
	}
	
	if (iVal < 0 || iVal > 9)
	{
		cStr csText;
		csText = iVal;
		m_ErrorList.InvalidParVal(csText.Str(), 1, iLine, iPos);
		return false;
	}
	
	TCVScalar pVec[3];
	
	if (m_pCLUDrawBase->Is2dView())
	{
		pVec[0] = (TCVScalar) (*m_pTransform)[iVal].pfTrans[0];
		pVec[1] = -(TCVScalar) (*m_pTransform)[iVal].pfTrans[2];
		pVec[2] = (TCVScalar) 0;
	}
	else
	{
		for(int i=0;i<3;i++)
			pVec[i] = (TCVScalar) (*m_pTransform)[iVal].pfTrans[i];
	}

	rLVar = m_N3GABase.NullVec(pVec);
	
	return true;
}


//////////////////////////////////////////////////////////////////////
// Convert P3,E3 vector into N3 Vector

bool CCLUCodeBase::VecN3(CCodeVar& rLVar, TMultiV& vA, int iLine, int iPos)
{
	TMultiV vR(m_N3GABase.GetMVStyle());
	int iID = (int) vA.GetBase().BaseID();
	
	if (iID == ID_E3GA)
	{
		vR = ConfGA<TCVScalar>::NullVec(vA[1], vA[2], vA[3]);
	}
	else if (iID == ID_PGA)
	{
		if (vA[4] != 0)
		{
			vA = PGA<TCVScalar>::Vec(vA);
			vR = ConfGA<TCVScalar>::NullVec(vA[1], vA[2], vA[3]);
		}
		else
		{
			vR = ConfGA<TCVScalar>::DirVec(vA[1], vA[2], vA[3]);
		}
	}
	else if (iID == ID_CONFGA)
	{
		vR = vA;
	}
	else
	{
		m_ErrorList.Internal(iLine, iPos);
		return false;
	}
	
	rLVar = vR.TinyToZero(m_fSensitivity);
	
	return true;
}






//////////////////////////////////////////////////////////////////////
// Create El2 Vector

bool CCLUCodeBase::VecEl2Func(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList *pmVars = rPars.GetVarListPtr();
	
	int iVarCount = pmVars->Count();
	
	if (iVarCount == 2)
	{
		TCVScalar pfVec[2];
		int i;
		
		for(i=0;i<2;i++)
		{
			if ( !((*pmVars)(i).CastToScalar(pfVec[i], m_fSensitivity)) )
			{
				m_ErrorList.InvalidType((*pmVars)(i), iLine, iPos);
				return false;
			}
		}
		
		if (!VecEl2(rVar, pfVec, iLine, iPos))
			return false; // VecE3 sets error
	}
	else if (iVarCount == 1)
	{
		TCVCounter iVal;
		
		// If Parameter is a list of values
		// call VecEl2 with this list of values.
		if ((*pmVars)(0).BaseType() == PDT_VARLIST)
		{
			int i, iCount;
			TVarList& rParList = *((*pmVars)(0).GetVarListPtr());

			iCount = rParList.Count();
			if (rParList(0).BaseType() == PDT_SCALAR ||
				rParList(0).BaseType() == PDT_COUNTER)
			{
				return VecEl2Func(rVar, (*pmVars)(0), iLine, iPos);
			}
			else
			{
				rVar.New(PDT_VARLIST, "Constant");
				TVarList &rList = *rVar.GetVarListPtr();
				rList.Set(iCount);

				for (i = 0; i < iCount; i++)
				{
					if (rParList(i).BaseType() == PDT_VARLIST)
					{
						if (!VecEl2Func(rList[i], rParList(i), iLine, iPos))
							return false;
					}
					else
					{
						if (rParList(i).BaseType() == PDT_MULTIV)
						{
							if (!VecEl2(rList[i], *((TMultiV *) rParList(i).GetMultiVPtr()), iLine, iPos))
								return false;
						}
						else if (rParList(i).BaseType() == PDT_MATRIX)
						{
							if (!VecEl2(rList[i], *((TMatrix *) rParList(i).GetMatrixPtr()), iLine, iPos))
								return false;
						}
						else
						{
							m_ErrorList.InvalidType((*pmVars)(0), iLine, iPos);
							return false;
						}	
					}
				}
			}

			return true;
		}
		
		if ((*pmVars)(0).CastToCounter(iVal))
		{
			if (!VecEl2(rVar, iVal, iLine, iPos))
				return false; // VecE3 sets error		
		}
		else if ((*pmVars)(0).BaseType() == PDT_MULTIV)
		{
			if (!VecEl2(rVar, *((TMultiV *) (*pmVars)(0).GetMultiVPtr()), iLine, iPos))
				return false;
		}
		else if ((*pmVars)(0).BaseType() == PDT_MATRIX)
		{
			if (!VecEl2(rVar, *((TMatrix *) (*pmVars)(0).GetMatrixPtr()), iLine, iPos))
				return false;
		}
		else
		{
			m_ErrorList.InvalidType((*pmVars)(0), iLine, iPos);
			return false;
		}	
	}
	else
	{
		int piVal[] = { 1, 2 };
		m_ErrorList.WrongNoOfParams(piVal, 2, iLine, iPos);
		return false;
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////
// Create El2 Vector

bool CCLUCodeBase::VecEl2(CCodeVar& rLVar, TCVScalar *pfVec, int iLine, int iPos)
{
	if (!pfVec)
		return false;
	
	if (!rLVar.New(PDT_MULTIV, "Constant"))
	{
		m_ErrorList.Internal(iLine, iPos);
		return false;
	}
	
	rLVar = m_El2GABase.Vec(pfVec);
	
	return true;
}

//////////////////////////////////////////////////////////////////////
// Create El2 Vector from transform variable

bool CCLUCodeBase::VecEl2(CCodeVar& rLVar, int iVal, int iLine, int iPos)
{
	if (!m_pTransform)
		return false;
	
	if (!rLVar.New(PDT_MULTIV, "Constant"))
	{
		m_ErrorList.Internal(iLine, iPos);
		return false;
	}
	
	if (iVal < 0 || iVal > 9)
	{
		cStr csText;
		csText = iVal;
		m_ErrorList.InvalidParVal(csText.Str(), 1, iLine, iPos);
		return false;
	}
	
	TCVScalar pVec[2];
	
	pVec[0] = (TCVScalar) (*m_pTransform)[iVal].pfTrans[0];
	pVec[1] = -(TCVScalar) (*m_pTransform)[iVal].pfTrans[2];
	
	rLVar = m_El2GABase.Vec(pVec);
	
	return true;
}


//////////////////////////////////////////////////////////////////////
// Convert N3,P3, E3 vector into El2 Vector

bool CCLUCodeBase::VecEl2(CCodeVar& rLVar, TMultiV& vA, int iLine, int iPos)
{
	TMultiV vR;
	
	if (!CastMVtoEl2(vA, vR))
	{
		m_ErrorList.Internal(iLine, iPos);
		return false;
	}
	
	rLVar = vR.TinyToZero(m_fSensitivity);
	
	return true;
}


//////////////////////////////////////////////////////////////////////
// Convert 3x3 matrix to vector

bool CCLUCodeBase::VecEl2(CCodeVar& rLVar, TMatrix& xM, int iLine, int iPos)
{
	TMultiV vR;
	
	if (xM.Rows() != 3 || xM.Cols() != 3)	
	{
		m_ErrorList.GeneralError("Matrix is not of dimension 3x3.", iLine, iPos);
		return false;
	}

	vR = m_El2GABase.Vec(xM);
	rLVar = vR.TinyToZero(m_fSensitivity);
	
	return true;
}




//////////////////////////////////////////////////////////////////////
// Cast Multivector to El2

bool CCLUCodeBase::CastMVtoEl2(TMultiV& vA, TMultiV& vR)
{
	vR.SetStyle(m_El2GABase.GetMVStyle());
	int iID = (int) vA.GetBase().BaseID();
	
	if (iID == ID_E3GA)
	{
		vR = m_El2GABase.Vec(vA[1], vA[2]);
	}
	else if (iID == ID_PGA)
	{
		vA = PGA<TCVScalar>::Vec(vA);
		vR = m_El2GABase.Vec(vA[1], vA[2]);
	}
	else if (iID == ID_CONFGA)
	{
		if (!::IsZero(Scalar(vA * m_N3GABase.vE), m_pFilter->GetDoubleSens()))
		{
			vA = ConfGA<TCVScalar>::MakeEuclidVec(vA);
			vR = m_El2GABase.Vec(vA[1], vA[2]);
		}
		else vR = 0;
	}
	else if (iID == ID_EL2GA)
	{
		vR = vA;
	}
	else return false;
	
	return true;
}


//////////////////////////////////////////////////////////////////////
// Project Conic to nearest degenerate conic
//
// Parameters:
//	1. Conic
//
// Returns
//	List of
//		1. degenerate conic
//		2. distance from initial to degenerate conic

bool CCLUCodeBase::MakeConicDegenerateFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList &mVars = *rPars.GetVarListPtr();
	
	int iVarCount = mVars.Count();
	
	if (iVarCount != 1)
	{
		int piVal[] = { 1 };
		m_ErrorList.WrongNoOfParams(piVal, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_MULTIV)
	{
		m_ErrorList.InvalidType(mVars(0), iLine, iPos);
		return false;
	}	

	TCVScalar fDist;
	TMultiV &vA = *mVars(0).GetMultiVPtr();

	rVar.New(PDT_VARLIST, "Constant");
	TVarList &rList = *rVar.GetVarListPtr();

	rList.Add(2);

	rList[0].New(PDT_MULTIV, "Constant");
	TMultiV &vB = *rList[0].GetMultiVPtr();

	m_El2GABase.MakeConicDegenerate(vA, vB, fDist, m_pFilter->m_El2Filter.IsOPNSEnabled());

	rList[1] = fDist;

	return true;
}


//////////////////////////////////////////////////////////////////////
// Find all degenerate conics that can be generated through 
// linear combinations of two given conics.
//
// Parameters:
//	1. 1st Conic
//	2. 2nd Conic
//
// Returns
//		List of degenerate conics (1 or 3)

bool CCLUCodeBase::FindAllDegConicsFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList &mVars = *rPars.GetVarListPtr();
	
	int iVarCount = mVars.Count();
	
	if (iVarCount != 2)
	{
		int piVal[] = { 2 };
		m_ErrorList.WrongNoOfParams(piVal, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_MULTIV)
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}	

	if (mVars(1).BaseType() != PDT_MULTIV)
	{
		m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}	

	TMultiV &vA = *mVars(0).GetMultiVPtr();
	TMultiV &vB = *mVars(1).GetMultiVPtr();

	if (vA.GetBase().BaseID() != ID_EL2GA)
	{
		m_ErrorList.GeneralError("First multivector is not from conic space.", iLine, iPos);
		return false;
	}

	if (vB.GetBase().BaseID() != ID_EL2GA)
	{
		m_ErrorList.GeneralError("Second multivector is not from conic space.", iLine, iPos);
		return false;
	}

	bool bOPNS;
	Mem<uint> mList;
	MemObj<TMultiV> mConicList;

	if (!GradeList(vA, mList, m_fSensitivity) || mList.Count() != 1)
	{
		m_ErrorList.GeneralError("The first multivector does not represent a conic.", iLine, iPos);
		return false;
	}

	if (mList[0] == 5)
	{
		bOPNS = true;
	}
	else if (mList[0] == 1)
	{
		bOPNS = false;
	}
	else
	{
		m_ErrorList.GeneralError("The first multivector does not represent a conic.", iLine, iPos);
		return false;
	}

	if (!GradeList(vB, mList, m_fSensitivity) || mList.Count() != 1)
	{
		m_ErrorList.GeneralError("The second multivector does not represent a conic.", iLine, iPos);
		return false;
	}

	if ((bOPNS && mList[0] != 5) ||
		(!bOPNS && mList[0] != 1))
	{
		m_ErrorList.GeneralError("The second multivector does not represent a conic.", iLine, iPos);
		return false;
	}

	if (bOPNS)
	{
		if (!m_El2GABase.FindAllDegConics(*vA, *vB, mConicList, m_fSensitivity))
		{
			m_ErrorList.GeneralError("Error evaluating degenerate conics.", iLine, iPos);
			return false;
		}
	}
	else
	{
		if (!m_El2GABase.FindAllDegConics(vA, vB, mConicList, m_fSensitivity))
		{
			m_ErrorList.GeneralError("Error evaluating degenerate conics.", iLine, iPos);
			return false;
		}
	}

	int i, iCnt = mConicList.Count();
	rVar.New(PDT_VARLIST, "Constant");
	TVarList &rList = *rVar.GetVarListPtr();

	rList.Add(iCnt);
	for(i = 0; i < iCnt; i++)
	{
		if (bOPNS)
			rList[i] = *mConicList[i];
		else
			rList[i] = mConicList[i];
	}

	return true;
}







//////////////////////////////////////////////////////////////////////
// Create E3 Rotor from Multi-Vector FUNCTION

bool CCLUCodeBase::RotorE3Func(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	
	TVarList *pmVars = rPars.GetVarListPtr();
	
	int iVarCount = pmVars->Count();
	
	if (iVarCount == 4)
	{
		TCVScalar pfVec[4];
		int i;
		
		for(i=0;i<4;i++)
		{
			if (!((*pmVars)(i).CastToScalar(pfVec[i], m_fSensitivity)))
			{
				m_ErrorList.InvalidType((*pmVars)(i), iLine, iPos);
				return false;
			}
		}
		
		if (!RotorE3(rVar, pfVec, iLine, iPos))
			return false; // VecE3 sets error
	}
	else if (iVarCount == 2)
	{
		TCVScalar fDeg;
		TMultiV vA;
		
		if ((*pmVars)(0).BaseType() != PDT_MULTIV)
		{
			m_ErrorList.InvalidType((*pmVars)(0), iLine, iPos);
			return false;
		}
		
		vA = *((TMultiV*) (*pmVars)(0).GetMultiVPtr());
		
		if (!((*pmVars)(1).CastToScalar(fDeg, m_fSensitivity)))
		{
			m_ErrorList.InvalidType((*pmVars)(1), iLine, iPos);
			return false;
		}
		
		if (!RotorE3(rVar, vA, fDeg, iLine, iPos))
			return false; // VecE3 sets error		
	}
	else
	{
		int piParNo[] = { 2, 4 };
		m_ErrorList.WrongNoOfParams(piParNo, 2, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
// Create E3 Rotor from Multi-Vector

bool CCLUCodeBase::RotorE3(CCodeVar& rLVar, TMultiV& rMV, TCVScalar fRad, int iLine, int iPos)
{
	if (!rLVar.New(PDT_MULTIV, "Constant"))
	{
		m_ErrorList.Internal(iLine, iPos);
		return false;
	}
	
	bool bAxis = true;
	//TCVScalar fRad = fDeg * m_fRadPerDeg;
	
	if (rMV(2) != m_E3GABase.vZero)
		bAxis = false;
	
	rLVar = m_E3GABase.Rotor(rMV, fRad, bAxis);
	
	return true;
}


//////////////////////////////////////////////////////////////////////
// Create E3 Rotor from axis

bool CCLUCodeBase::RotorE3(CCodeVar& rLVar, TCVScalar *pfVec, int iLine, int iPos)
{
	if (!pfVec)
		return false;
	
	if (!rLVar.New(PDT_MULTIV, "Constant"))
	{
		m_ErrorList.Internal(iLine, iPos);
		return false;
	}
	
	TCVScalar fRad = pfVec[3];// * m_fRadPerDeg;
	
	rLVar = m_E3GABase.Rotor(pfVec[0], pfVec[1], pfVec[2], fRad);
	
	return true;
}


//////////////////////////////////////////////////////////////////////
// Create P3 Rotor from Multi-Vector FUNCTION
bool CCLUCodeBase::RotorP3Func(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList *pmVars = rPars.GetVarListPtr();
	
	int iVarCount = pmVars->Count();
	
	if (iVarCount == 4)
	{
		TCVScalar pfVec[4];
		int i;		
		for(i=0;i<4;i++)
		{
			if (!((*pmVars)(i).CastToScalar(pfVec[i], m_fSensitivity)))
			{
				m_ErrorList.InvalidType((*pmVars)(i), iLine, iPos);
				return false;
			}
		}
		if (!RotorP3(rVar, pfVec, iLine, iPos))
			return false; // VecP3 sets error
	}
	else if (iVarCount == 2)
	{
		TCVScalar fDeg;
		TMultiV vA, vB;
		
		if ((*pmVars)(0).BaseType() != PDT_MULTIV)
		{
			m_ErrorList.InvalidType((*pmVars)(0), iLine, iPos);
			return false;
		}
		
		vA = *((TMultiV*) (*pmVars)(0).GetMultiVPtr());
		CastMVtoP3(vA, vB);

		if (!((*pmVars)(1).CastToScalar(fDeg, m_fSensitivity)))
		{
			m_ErrorList.InvalidType((*pmVars)(1), iLine, iPos);
			return false;
		}
		
		if (!RotorP3(rVar, vB, fDeg, iLine, iPos))
			return false; // VecP3 sets error		
	}
	else
	{
		int piParNo[] = { 2, 4 };
		m_ErrorList.WrongNoOfParams(piParNo, 2, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
// Create P3 Rotor from Multi-Vector
bool CCLUCodeBase::RotorP3(CCodeVar& rLVar, TMultiV& rMV, TCVScalar fRad, int iLine, int iPos)
{
	if (!rLVar.New(PDT_MULTIV, "Constant"))
	{
		m_ErrorList.Internal(iLine, iPos);
		return false;
	}
	
	bool bAxis = true;
	//TCVScalar fRad = fDeg * m_fRadPerDeg;
	
	if (rMV(2) != m_P3GABase.vZero)		
		bAxis = false;
	
	rLVar = m_P3GABase.Rotor(rMV, fRad, bAxis);
	
	return true;
}


//////////////////////////////////////////////////////////////////////
// Create P3 Rotor from axis
bool CCLUCodeBase::RotorP3(CCodeVar& rLVar, TCVScalar *pfVec, int iLine, int iPos)
{
	if (!pfVec)
		return false;
	
	if (!rLVar.New(PDT_MULTIV, "Constant"))
	{
		m_ErrorList.Internal(iLine, iPos);
		return false;
	}
	
	TCVScalar fRad = pfVec[3];// * m_fRadPerDeg;
	
	rLVar = m_P3GABase.Rotor(pfVec[0], pfVec[1], pfVec[2], fRad);
	
	return true;
}

//////////////////////////////////////////////////////////////////////
// Create N3 Rotor from Multi-Vector FUNCTION
bool CCLUCodeBase::RotorN3Func(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	
	TVarList *pmVars = rPars.GetVarListPtr();
	
	int iVarCount = pmVars->Count();
	
	if (iVarCount == 4)
	{
		TCVScalar pfVec[4];
		int i;
		
		for(i=0;i<4;i++)
		{
			if (!((*pmVars)(i).CastToScalar(pfVec[i], m_fSensitivity)))
			{
				m_ErrorList.InvalidType((*pmVars)(i), iLine, iPos);
				return false;
			}
		}
		
		if (!RotorN3(rVar, pfVec, iLine, iPos))
			return false; // VecE3 sets error
	}
	else if (iVarCount == 2)
	{
		TCVScalar fDeg;
		TMultiV vA;
		
		if ((*pmVars)(0).BaseType() != PDT_MULTIV)
		{
			m_ErrorList.InvalidType((*pmVars)(0), iLine, iPos);
			return false;
		}
		
		vA = *((TMultiV*) (*pmVars)(0).GetMultiVPtr());
		
		if (!((*pmVars)(1).CastToScalar(fDeg, m_fSensitivity)))
		{
			m_ErrorList.InvalidType((*pmVars)(1), iLine, iPos);
			return false;
		}
		
		if (!RotorN3(rVar, vA, fDeg, iLine, iPos))
			return false; // VecE3 sets error		
	}
	else
	{
		int piParNo[] = { 2, 4 };
		m_ErrorList.WrongNoOfParams(piParNo, 2, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
// Create N3 Rotor from Multi-Vector
bool CCLUCodeBase::RotorN3(CCodeVar& rLVar, TMultiV& rMV, TCVScalar fRad, int iLine, int iPos)
{
	if (!rLVar.New(PDT_MULTIV, "Constant"))
	{
		m_ErrorList.Internal(iLine, iPos);
		return false;
	}
	
	bool bAxis = true;
	//TCVScalar fRad = fDeg * m_fRadPerDeg;
	
	if (rMV(2) != m_E3GABase.vZero)
		bAxis = false;
	
	rLVar = m_N3GABase.Rotor(rMV, fRad, bAxis);
	
	return true;
}


//////////////////////////////////////////////////////////////////////
// Create N3 Rotor from axis
bool CCLUCodeBase::RotorN3(CCodeVar& rLVar, TCVScalar *pfVec, int iLine, int iPos)
{
	if (!pfVec)
		return false;
	
	if (!rLVar.New(PDT_MULTIV, "Constant"))
	{
		m_ErrorList.Internal(iLine, iPos);
		return false;
	}
	
	TCVScalar fRad = pfVec[3];// * m_fRadPerDeg;
	
	rLVar = m_N3GABase.Rotor(pfVec[0], pfVec[1], pfVec[2], fRad);
	
	return true;
}




//////////////////////////////////////////////////////////////////////
// Create C2 Rotor from Angle

bool CCLUCodeBase::RotorC2Func(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	
	TVarList &mVars = *rPars.GetVarListPtr();
	
	int iVarCount = mVars.Count();
	TCVScalar fVal = 0;
	
	if (iVarCount == 1)
	{
		if (!mVars(0).CastToScalar(fVal, m_fSensitivity))
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}
	}
	else
	{
		int piParNo[] = { 1 };
		m_ErrorList.WrongNoOfParams(piParNo, 1, iLine, iPos);
		return false;
	}
	
	TCVScalar fS, fC, fSh, fCh, fFac;
	TMultiV R1(m_El2GABase.GetMVStyle()), R2(m_El2GABase.GetMVStyle());

	fFac = TCVScalar(1.0 / sqrt(2.0));
	fS = TCVScalar(sin(double(fVal)));
	fC = TCVScalar(cos(double(fVal)));
	fSh = TCVScalar(sin(double(fVal)/2.0));
	fCh = TCVScalar(cos(double(fVal)/2.0));

	R1 = (fC ^ m_El2GABase.vSC) + ((fFac * fS) ^ (m_El2GABase.vE5 - m_El2GABase.vE4) ^ m_El2GABase.vE6);
	R2 = (fCh  ^ m_El2GABase.vSC) + (fSh ^ m_El2GABase.vE2 ^ m_El2GABase.vE1);

	rVar = (R2 & R1);

	return true;
}



//////////////////////////////////////////////////////////////////////
// Create C2 Reflector from Angle

bool CCLUCodeBase::ReflectorC2Func(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	
	TVarList &mVars = *rPars.GetVarListPtr();
	
	int iVarCount = mVars.Count();
	TCVScalar fVal = 0;
	
	if (iVarCount == 1)
	{
		if (!mVars(0).CastToScalar(fVal, m_fSensitivity))
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}
	}
	else
	{
		int piParNo[] = { 1 };
		m_ErrorList.WrongNoOfParams(piParNo, 1, iLine, iPos);
		return false;
	}
	
	TCVScalar fS, fC, fSh, fCh, fFac;
	TMultiV R1(m_El2GABase.GetMVStyle()), R2(m_El2GABase.GetMVStyle());

	fFac = TCVScalar(1.0 / sqrt(2.0));
	fS = TCVScalar(sin(double(fVal)*2.0));
	fC = TCVScalar(cos(double(fVal)*2.0));
	fSh = TCVScalar(sin(double(fVal)));
	fCh = TCVScalar(cos(double(fVal)));

	R1 = (-fC ^ m_El2GABase.vE6) + ((fFac * fS) ^ (m_El2GABase.vE4 - m_El2GABase.vE5));
	R2 = (fCh  ^ m_El2GABase.vE2) - (fSh ^ m_El2GABase.vE1);

	rVar = (R2 & R1);

	return true;
}













//////////////////////////////////////////////////////////////////////
/// Create N3 Sphere from center and radius in OPNS
bool CCLUCodeBase::SphereN3(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList *pmVars = rPars.GetVarListPtr();
	
	int iVarCount = pmVars->Count();
	
	TCVScalar pfVec[4];

	if (iVarCount == 2)
	{
		TMultiV vA;
		
		if ((*pmVars)(0).BaseType() != PDT_MULTIV)
		{
			m_ErrorList.InvalidType((*pmVars)(0), iLine, iPos);
			return false;
		}
		
		if (!CastMVtoE3(*((TMultiV*) (*pmVars)(0).GetMultiVPtr()), vA))
		{
			m_ErrorList.InvalidParVal((*pmVars)(0), iLine, iPos);
			return false;
		}
		
		if (!((*pmVars)(1).CastToScalar(pfVec[3], m_fSensitivity)))
		{
			m_ErrorList.InvalidType((*pmVars)(1), iLine, iPos);
			return false;
		}

		pfVec[0] = vA[m_E3GABase.iE1];
		pfVec[1] = vA[m_E3GABase.iE2];
		pfVec[2] = vA[m_E3GABase.iE3];
	}
	else if (iVarCount == 4)
	{
		int i;
		
		for(i=0;i<4;i++)
		{
			if (!((*pmVars)(i).CastToScalar(pfVec[i], m_fSensitivity)))
			{
				m_ErrorList.InvalidType((*pmVars)(i), iLine, iPos);
				return false;
			}
		}
	}
	else
	{
		int piParNo[] = { 2, 4 };
		m_ErrorList.WrongNoOfParams(piParNo, 2, iLine, iPos);
		return false;
	}

	if (!rVar.New(PDT_MULTIV, "Constant"))
	{
		m_ErrorList.Internal(iLine, iPos);
		return false;
	}		
	rVar = *m_N3GABase.Sphere(pfVec[0], pfVec[1], pfVec[2], pfVec[3]);

	return true;
} 

//////////////////////////////////////////////////////////////////////
/// Create N3 Translator from 3 scalars or take vector part of multivector
bool CCLUCodeBase::TranslatorN3(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList *pmVars = rPars.GetVarListPtr();
	
	int iVarCount = pmVars->Count();
	
	if (iVarCount == 3)
	{
		TCVScalar pfVec[3];
		int i;
		
		for(i=0;i<3;i++)
		{
			if (!((*pmVars)(i).CastToScalar(pfVec[i], m_fSensitivity)))
			{
				m_ErrorList.InvalidType((*pmVars)(i), iLine, iPos);
				return false;
			}
		}
		
		if (!rVar.New(PDT_MULTIV, "Constant"))
		{
			m_ErrorList.Internal(iLine, iPos);
			return false;
		}
		
		rVar = m_N3GABase.Translator(pfVec[0], pfVec[1], pfVec[2]);
	}
	else if (iVarCount == 1)
	{		
		TMultiV vA;
		
		if ((*pmVars)(0).BaseType() != PDT_MULTIV)
		{
			m_ErrorList.InvalidType((*pmVars)(0), iLine, iPos);
			return false;
		}
		
		vA = *((TMultiV*) (*pmVars)(0).GetMultiVPtr());
		
		if (!rVar.New(PDT_MULTIV, "Constant"))
		{
			m_ErrorList.Internal(iLine, iPos);
			return false;
		}
		
		rVar = m_N3GABase.Translator(vA);			
	}
	else
	{
		int piParNo[] = { 1,3 };
		m_ErrorList.WrongNoOfParams(piParNo, 2, iLine, iPos);
		return false;
	}
	return true;
} 

//////////////////////////////////////////////////////////////////////
/// Create N3 Dilator 
bool CCLUCodeBase::DilatorN3(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList *pmVars = rPars.GetVarListPtr();
	
	int iVarCount = pmVars->Count();
	
	if (iVarCount == 1)
	{
		TCVScalar pfVec;		
		
		if (!((*pmVars)(0).CastToScalar(pfVec, m_fSensitivity)))
		{
			m_ErrorList.InvalidType((*pmVars)(0), iLine, iPos);
			return false;
		}
				
		if (!rVar.New(PDT_MULTIV, "Constant"))
		{
			m_ErrorList.Internal(iLine, iPos);
			return false;
		}
		
		rVar = m_N3GABase.Dilator(pfVec);
	}
	else
	{
		int piParNo[] = { 1 };
		m_ErrorList.WrongNoOfParams(piParNo, 1, iLine, iPos);
		return false;
	}
	return true;
} 

//////////////////////////////////////////////////////////////////////
/// Create N3 Direction Vector
bool CCLUCodeBase::DirVecN3(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList *pmVars = rPars.GetVarListPtr();
	
	int iVarCount = pmVars->Count();
	
	if (iVarCount == 3)
	{
		TCVScalar pfVec[3];
		int i;
		
		for(i=0;i<3;i++)
		{
			if (!((*pmVars)(i).CastToScalar(pfVec[i], m_fSensitivity)))
			{
				m_ErrorList.InvalidType((*pmVars)(i), iLine, iPos);
				return false;
			}
		}		
		if (!rVar.New(PDT_MULTIV, "Constant"))
		{
			m_ErrorList.Internal(iLine, iPos);
			return false;
		}
		
		rVar = m_N3GABase.DirVec(pfVec[0], pfVec[1], pfVec[2]);
	}
	else
	{
		int piParNo[] = { 3 };
		m_ErrorList.WrongNoOfParams(piParNo, 1, iLine, iPos);
		return false;
	}
	return true;
} 
//////////////////////////////////////////////////////////////////////
/// Create P3 Direction Vector
bool CCLUCodeBase::DirVecP3Func(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList *pmVars = rPars.GetVarListPtr();
	
	int iVarCount = pmVars->Count();
	
	if (iVarCount == 3)
	{
		TCVScalar pfVec[3];
		int i;
		
		for(i=0;i<3;i++)
		{
			if (!((*pmVars)(i).CastToScalar(pfVec[i], m_fSensitivity)))
			{
				m_ErrorList.InvalidType((*pmVars)(i), iLine, iPos);
				return false;
			}
		}		
		if (!rVar.New(PDT_MULTIV, "Constant"))
		{
			m_ErrorList.Internal(iLine, iPos);
			return false;
		}		
		rVar = m_P3GABase.Vec(pfVec[0], pfVec[1], pfVec[2]);
	}
	else if (iVarCount == 1)
	{
		TCVCounter iVal;
		
		// If Parameter is a list of values
		// call VecE3 with this list of values.
		if ((*pmVars)(0).BaseType() == PDT_VARLIST)
		{
			int i, iCount;
			TVarList& rParList = *((*pmVars)(0).GetVarListPtr());

			iCount = rParList.Count();
			if (rParList(0).BaseType() == PDT_SCALAR ||
				rParList(0).BaseType() == PDT_COUNTER)
			{
				return VecP3Func(rVar, (*pmVars)(0), iLine, iPos);
			}
			else
			{
				rVar.New(PDT_VARLIST, "Constant");
				TVarList &rList = *rVar.GetVarListPtr();
				rList.Set(iCount);

				for (i = 0; i < iCount; i++)
				{
					if (rParList(i).BaseType() == PDT_VARLIST)
					{
						if (!DirVecP3Func(rList[i], rParList(i), iLine, iPos))
							return false;
					}
					else
					{
						if (rParList(i).BaseType() == PDT_MULTIV)
						{
							if (!DirVecP3(rList[i], *((TMultiV *) rParList(i).GetMultiVPtr()), iLine, iPos))
								return false;
						}
						else
						{
							m_ErrorList.InvalidType((*pmVars)(0), iLine, iPos);
							return false;
						}	
					}
				}
			}

			return true;
		}
		
		if ((*pmVars)(0).CastToCounter(iVal))
		{
			if (!DirVecP3(rVar, iVal, iLine, iPos))
				return false; // VecE3 sets error		
		}
		else if ((*pmVars)(0).BaseType() == PDT_MULTIV)
		{
			TMultiV &rvA = *((*pmVars)(0).GetMultiVPtr());

			if (!DirVecP3(rVar, rvA, iLine, iPos))
				return false;
		}
		else
		{
			m_ErrorList.InvalidType((*pmVars)(0), iLine, iPos);
			return false;
		}	
	}
	else
	{
		int piParNo[] = { 1, 3 };
		m_ErrorList.WrongNoOfParams(piParNo, 2, iLine, iPos);
		return false;
	}
	return true;
} 


//////////////////////////////////////////////////////////////////////
// Create P3 Dir.-Vector from transform variable

bool CCLUCodeBase::DirVecP3(CCodeVar& rLVar, int iVal, int iLine, int iPos)
{
	if (!m_pTransform)
		return false;
	
	if (!rLVar.New(PDT_MULTIV, "Constant"))
	{
		m_ErrorList.Internal(iLine, iPos);
		return false;
	}
	
	if (iVal < 0 || iVal > 9)
	{
		cStr csText;
		csText = iVal;
		m_ErrorList.InvalidParVal(csText.Str(), 1, iLine, iPos);
		return false;
	}
	
	float *pfTrans = (*m_pTransform)[iVal].pfTrans;
	TCVScalar pfVec[3];
	
	if (m_pCLUDrawBase->Is2dView())
	{
		pfVec[0] = (TCVScalar) pfTrans[0];
		pfVec[1] = -(TCVScalar) pfTrans[2];
		pfVec[2] = (TCVScalar) 0.0;
	}
	else
	{
		pfVec[0] = (TCVScalar) pfTrans[0];
		pfVec[1] = (TCVScalar) pfTrans[1];
		pfVec[2] = (TCVScalar) pfTrans[2];
	}
	
	rLVar = m_P3GABase.Vec(pfVec);
	
	return true;
}


//////////////////////////////////////////////////////////////////////
// Convert N3,E3 vector into P3 Vector

bool CCLUCodeBase::DirVecP3(CCodeVar& rLVar, TMultiV& vA, int iLine, int iPos)
{
	TMultiV vR;

	if (!CastMVtoE3(vA, vR))
	{
		m_ErrorList.Internal(iLine, iPos);
		return false;
	}

	vR.TinyToZero(m_fSensitivity);
	rLVar = m_P3GABase.Vec(vR[1], vR[2], vR[3]);

	return true;
}



//////////////////////////////////////////////////////////////////////
/// Factorize a multivector blade
// TODO Update docu for FactorizeBlade. Now also for null blades.

bool CCLUCodeBase::FactorBladeFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList *pmVars = rPars.GetVarListPtr();
	
	int iVarCount = pmVars->Count();
	
	if (iVarCount == 1)
	{
		MemObj<TMultiV > memResultList,memBasisList;
		TMultiV vA;
		
		if ((*pmVars)(0).BaseType() != PDT_MULTIV)
		{
			m_ErrorList.InvalidType((*pmVars)(0), iLine, iPos);
			return false;
		}
		
		vA = *((TMultiV*) (*pmVars)(0).GetMultiVPtr());
		
		// If multivector is element of conformal space
		// then use special FactorBlade function that can
		// also factorize null blades.
		if (vA.BasePtr()->BaseID() == ID_CONFGA)
		{
			FactorBlade<TCVScalar>(vA, m_N3GABase.vEm, memResultList, true, 0, 0, m_fSensitivity);
		}
		else
		{
			FactorBlade<TCVScalar>(vA, memResultList, true, 0, 0, m_fSensitivity);
		}

		//if (iVarCount == 1) 
		/*else 
		{			
			if ((*pmVars)(1).BaseType() == PDT_VARLIST)
			{ // get the MultiVectors from PDT_VARLIST
				TVarList *pmBasisVars = (TVarList *) ((*pmVars)(1).Val());
				int iBasisVarCount = pmBasisVars->Count();
				for (int j=0;j<iBasisVarCount;j++) 
				{
					if ((*pmBasisVars)[j].BaseType() != PDT_MULTIV)
					{
						m_ErrorList.InvalidType((*pmBasisVars)[j], iLine, iPos);
						return false;
					}
					memBasisList++;
					memBasisList.Last()= *((TMultiV*) (*pmBasisVars)[j].Val());
				}
				FactorBlade(vA,memResultList,memBasisList);
			}
			else 
			{
				if (iVarCount == 2)
				{					
					if ((*pmVars)(1).BaseType()!=PDT_INT)
					{
						m_ErrorList.InvalidType((*pmVars)(1), iLine, iPos);
						return false;
					}
					int iNorm=*((int *) (*pmVars)(1).Val());
					bool bNorm = ( iNorm>0 );
					FactorBlade(vA,memResultList,bNorm);
				}			
				if (iVarCount == 3)
				{
				}		
			}
		}*/
		
		if (!rVar.New(PDT_VARLIST, "Constant"))
		{
			m_ErrorList.Internal(iLine, iPos);
			return false;
		}
		
		
		
		/* 
		if (iVarCount == 1) // only a multivector?
		{
		
		  TCVScalar pfVec[4];
		  int i;
		  
			for(i=0;i<iVarCount;i++)
			{
			if (!((*pmVars)(i).CastToScalar(pfVec[i], m_fSensitivity)))
			{
			m_ErrorList.InvalidType((*pmVars)(i), iLine, iPos);
			return false;
			}
			}
			if (!rVar.New(PDT_MULTIV, "Constant"))
			{
			m_ErrorList.Internal(iLine, iPos);
			return false;
			}
			
			  rVar = m_N3GABase.Sphere(pfVec[0], pfVec[1], pfVec[2], pfVec[3]);
		*/		
		for (unsigned int l=0;l<memResultList.Count();l++)
		{
			if (! rVar.GetVarListPtr()->Add(1) )
			{
				m_ErrorList.OutOfMemory(iLine, iPos);
				return false;
			}		
			rVar.GetVarListPtr()->Last() = memResultList[l];
		}
	} 
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
} 



