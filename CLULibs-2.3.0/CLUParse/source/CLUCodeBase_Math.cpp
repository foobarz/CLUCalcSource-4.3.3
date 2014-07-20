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
#include <time.h>
#include <float.h>

#include "CLUCodeBase.h"
#include "TensorOperators.h"


#ifndef WIN32

#include <math.h>
#define _isnan isnan

#endif


//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////
/// Random FUNCTION
///
/// returns random value in the range of 0 and 1.

bool CCLUCodeBase::RandomFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	
	if (iVarCount == 0)
	{		
		rVar = TCVScalar(m_Random.ran3());
	}
	else
	{
		m_ErrorList.WrongNoOfParams(0, iLine, iPos);
		return false;
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Gauss FUNCTION
///
/// returns gaussian distribution about mean and with given variance.

bool CCLUCodeBase::GaussFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	
	if (iVarCount == 0)
	{		
		rVar = TCVScalar(m_Random.Gauss());
	}
	else
	{
		m_ErrorList.WrongNoOfParams(0, iLine, iPos);
		return false;
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Gauss Parameter FUNCTION
///
/// sets gaussian parameters mean and variance.

bool CCLUCodeBase::GaussParametersFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();

	if (iVarCount == 2)
	{		
		TCVScalar fMean, fVar;

		if (!mVars(0).CastToScalar(fMean, m_fSensitivity))
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}								

		if (!mVars(1).CastToScalar(fVar, m_fSensitivity))
		{
			m_ErrorList.InvalidParType(mVars(1), 1, iLine, iPos);
			return false;
		}								

		m_Random.GaussMean(fMean);
		m_Random.GaussDev(fVar);

		// Call Gauss twice, so that new setting can take effect
		m_Random.Gauss();
		m_Random.Gauss();
	}
	else
	{
		m_ErrorList.WrongNoOfParams(0, iLine, iPos);
		return false;
	}
	
	return true;
}



//////////////////////////////////////////////////////////////////////
/// Random Seed FUNCTION
///
/// Sets seed for random number generator. For negative numbers uses time.

bool CCLUCodeBase::RandomSeedFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	int iVal;

	if (iVarCount == 1)
	{		
		if (!mVars(0).CastToCounter(iVal))
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}								

		if (iVal < 0)
		{
			time_t TimeVal;
			time(&TimeVal);
			m_Random.seed3(TimeVal);
			rVar = int(TimeVal);
		}
		else
		{
			m_Random.seed3(iVal);
			rVar = iVal;
		}
	}
	else
	{
		m_ErrorList.WrongNoOfParams(0, iLine, iPos);
		return false;
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Factorial FUNCTION
///
/// evaluates factorial up to 170!

bool CCLUCodeBase::FactFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	int iVal;
	TCVScalar dRes;

	if (iVarCount == 1)
	{
		if (!mVars(0).CastToCounter(iVal))
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}
		
		if (iVal < 0 || iVal > 170)
		{
			cStr csText;
			csText = iVal;
			m_ErrorList.InvalidParVal(csText, 1, iLine, iPos);
			return false;
		}
		
		dRes = 1;
		for(iVal; iVal > 1; iVal--)
		{
			dRes *= (TCVScalar) iVal;
		}
		
		rVar = dRes;
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Sign FUNCTION
///
/// evaluates the sign of a scalar

bool CCLUCodeBase::SignFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	TCVScalar dVal;
	TCVScalar dRes;

	if (iVarCount == 1)
	{
		if (!mVars(0).CastToScalar(dVal))
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}
		
		if (dVal >= 0)
			dRes = 1;
		else
			dRes = -1;

		rVar = dRes;
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}



//////////////////////////////////////////////////////////////////////
/// The Commutator Product FUNCTION

bool CCLUCodeBase::CPFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	
	if (iVarCount == 2)
	{
		if (mVars(0).BaseType() == PDT_MULTIV && mVars(1).BaseType() == PDT_MULTIV)
		{
			TMultiV &vA = *mVars(0).GetMultiVPtr();
			TMultiV &vB = *mVars(1).GetMultiVPtr();

			rVar = ( 0.5 & ( (vA & vB) - (vB & vA) ) );
		}
		else
		{
			if (mVars(0).BaseType() != PDT_MULTIV)
				m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			else if (mVars(1).BaseType() != PDT_MULTIV)
				m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);

			return false;
		}
	}
	else
	{
		m_ErrorList.WrongNoOfParams(2, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// The Anti-Commutator Product FUNCTION

bool CCLUCodeBase::ACPFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	
	if (iVarCount == 2)
	{
		if (mVars(0).BaseType() == PDT_MULTIV && mVars(1).BaseType() == PDT_MULTIV)
		{
			TMultiV &vA = *mVars(0).GetMultiVPtr();
			TMultiV &vB = *mVars(1).GetMultiVPtr();

			rVar = ( 0.5 & ( (vA & vB) + (vB & vA) ) );
		}
		else
		{
			if (mVars(0).BaseType() != PDT_MULTIV)
				m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			else if (mVars(1).BaseType() != PDT_MULTIV)
				m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);

			return false;
		}
	}
	else
	{
		m_ErrorList.WrongNoOfParams(2, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// The floor FUNCTION

bool CCLUCodeBase::FloorFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	
	if (iVarCount == 1)
	{
		return Floor(rVar, mVars(0), iLine, iPos);
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}



//////////////////////////////////////////////////////////////////////
/// Evaluate floor

bool CCLUCodeBase::Floor(CCodeVar& rVar, CCodeVar& rDataVar, int iLine, int iPos)
{
	ECodeDataType eType = rDataVar.BaseType();
	TCVScalar fVal;

	if (eType == PDT_VARLIST)
	{
		TVarList &rList = *rDataVar.GetVarListPtr();
		int i, iCount = rList.Count();

		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rFloorList = *rVar.GetVarListPtr();
		rFloorList.Set(iCount);

		for( i = 0; i < iCount; i++ )
		{
			if (!Floor(rFloorList[i], rList(i), iLine, iPos))
				return false;
		}
	}
	else if (rDataVar.CastToScalar(fVal, m_fSensitivity))
	{
		rVar = (TCVScalar) floor((double) fVal);
	}
	else if (rDataVar.BaseType() == PDT_MULTIV)
	{
		rVar.New(PDT_MULTIV, "Constant");
		TMultiV &vA = *rVar.GetMultiVPtr();
		vA = *rDataVar.GetMultiVPtr();
		vA.Floor();
	}
	else
	{
		m_ErrorList.InvalidParType(rDataVar, 1, iLine, iPos);
		return false;
	}
	
	return true;
}




//////////////////////////////////////////////////////////////////////
/// The ceiling FUNCTION

bool CCLUCodeBase::CeilFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	
	if (iVarCount == 1)
	{
		return Ceil(rVar, mVars(0), iLine, iPos);
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}



//////////////////////////////////////////////////////////////////////
/// Evaluate ceiling

bool CCLUCodeBase::Ceil(CCodeVar& rVar, CCodeVar& rDataVar, int iLine, int iPos)
{
	ECodeDataType eType = rDataVar.BaseType();
	TCVScalar fVal;

	if (eType == PDT_VARLIST)
	{
		TVarList &rList = *rDataVar.GetVarListPtr();
		int i, iCount = rList.Count();

		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rFloorList = *rVar.GetVarListPtr();
		rFloorList.Set(iCount);

		for( i = 0; i < iCount; i++ )
		{
			if (!Ceil(rFloorList[i], rList(i), iLine, iPos))
				return false;
		}
	}
	else if (rDataVar.CastToScalar(fVal, m_fSensitivity))
	{
		rVar = (TCVScalar) ceil((double) fVal);
	}
	else if (rDataVar.BaseType() == PDT_MULTIV)
	{
		rVar.New(PDT_MULTIV, "Constant");
		TMultiV &vA = *rVar.GetMultiVPtr();
		vA = *rDataVar.GetMultiVPtr();
		vA.Ceil();
	}
	else
	{
		m_ErrorList.InvalidParType(rDataVar, 1, iLine, iPos);
		return false;
	}
	
	return true;
}



//////////////////////////////////////////////////////////////////////
/// The Round FUNCTION

bool CCLUCodeBase::RoundFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	
	TCVCounter iPrec;
	TCVScalar dPrec;
	int iVarCount = mVars.Count();
	
	if (iVarCount >= 1)
	{
		if (iVarCount == 2)
		{
			if (!mVars(1).CastToCounter(iPrec))
			{
				m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
				return false;
			}

			dPrec = pow(0.1, double(iPrec));
			if (dPrec == 0.0)
			{
				m_ErrorList.GeneralError("Precision value out of range.", iLine, iPos);
				return false;
			}
		}
		else
		{
			dPrec = m_fSensitivity;
		}

		return Round(rVar, mVars(0), dPrec, iLine, iPos);
	}
	else
	{
		int piParNo[] = { 1, 2 };
		m_ErrorList.WrongNoOfParams(piParNo, 2, iLine, iPos);
		return false;
	}
	
	return true;
}



//////////////////////////////////////////////////////////////////////
/// Evaluate floor

bool CCLUCodeBase::Round(CCodeVar& rVar, CCodeVar& rDataVar, TCVScalar dPrec, int iLine, int iPos)
{
	ECodeDataType eType = rDataVar.BaseType();
	TCVScalar fVal;

	if (eType == PDT_VARLIST)
	{
		TVarList &rList = *rDataVar.GetVarListPtr();
		int i, iCount = rList.Count();

		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rFloorList = *rVar.GetVarListPtr();
		rFloorList.Set(iCount);

		for( i = 0; i < iCount; i++ )
		{
			if (!Round(rFloorList[i], rList(i), dPrec, iLine, iPos))
				return false;
		}
	}
	else if (rDataVar.CastToScalar(fVal, m_fSensitivity))
	{
		rVar = (TCVScalar) ::Round(fVal, dPrec);
	}
	else if (rDataVar.BaseType() == PDT_MULTIV)
	{
		rVar.New(PDT_MULTIV, "Constant");
		TMultiV &vA = *rVar.GetMultiVPtr();
		vA = *rDataVar.GetMultiVPtr();
		vA.Round(dPrec);
	}
	else if (rDataVar.BaseType() == PDT_MATRIX)
	{
		rVar.New(PDT_MATRIX, "Constant");
		TMatrix &mA = *rVar.GetMatrixPtr();
		mA = *rDataVar.GetMatrixPtr();
		mA.Round(dPrec);
	}
	else
	{
		m_ErrorList.InvalidParType(rDataVar, 1, iLine, iPos);
		return false;
	}
	
	return true;
}





//////////////////////////////////////////////////////////////////////
/// The Exponential FUNCTION

bool CCLUCodeBase::ExpFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	
	if (iVarCount == 1)
	{
		if (!Exponential(rVar, mVars(0), iLine, iPos))
			return false;
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////
/// The Exponential

bool CCLUCodeBase::Exponential(CCodeVar& rVar, CCodeVar& rData, int iLine, int iPos)
{
	ECodeDataType eType = rData.BaseType();
	TCVScalar fVal;

	if (rData.CastToScalar(fVal, m_fSensitivity))
	{
		rVar = (TCVScalar) exp((double) fVal);
	}
	else if (eType == PDT_MULTIV)
	{
		TMultiV &vA = *rData.GetMultiVPtr();

		rVar = Exp(vA, 12);
	}
	else if (eType == PDT_VARLIST)
	{
		// Loop over all elements of list and call this function recursively.
		TVarList &List = *rData.GetVarListPtr();
		int i, iCount = List.Count();

		rVar.New(PDT_VARLIST, "Constant");
		TVarList &ResultList = *rVar.GetVarListPtr();

		ResultList.Set(iCount);
		for ( i = 0; i < iCount; i++)
		{
			if (!Exponential(ResultList[i], List[i], iLine, iPos))
				return false;
		}
	}
	else if (eType == PDT_MATRIX)
	{
		rVar = *rData.GetMatrixPtr();
		TMatrix& rMat = *rVar.GetMatrixPtr();

		// Take exp of matrix components separately
		rMat.ExpComps();
	}
	else
	{
		m_ErrorList.InvalidParType(rData, 1, iLine, iPos);
		return false;
	}

	return true;
}




//////////////////////////////////////////////////////////////////////
/// Log FUNCTION
bool CCLUCodeBase::LogFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	if (iVarCount == 1)
	{
		return Log(rVar, mVars(0), iLine, iPos);
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Cosine
bool CCLUCodeBase::Log(CCodeVar& rVar, CCodeVar& rData, int iLine, int iPos)
{
	TCVScalar dVal;

	if (rData.BaseType() == PDT_VARLIST)
	{
		TVarList &rList = *rData.GetVarListPtr();
		int i, iCount = rList.Count();

		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for ( i = 0; i < iCount; i++)
		{
			if (!Log(rRetList[i], rList(i), iLine, iPos))
				return false;
		}
	}
	else if (rData.BaseType() == PDT_MATRIX)
	{
		rVar = *rData.GetMatrixPtr();
		TMatrix& rMat = *rVar.GetMatrixPtr();

		// Take log of matrix components separately
		rMat.LogComps();
	}
	else
	{
		if (!rData.CastToScalar(dVal, m_fSensitivity))
		{
			m_ErrorList.InvalidParType(rData, 1, iLine, iPos);
			return false;
		}								

		if (dVal > 0.0)
			rVar = (TCVScalar) log((double) dVal);
		else
		{
			m_ErrorList.GeneralError("Parameter has to be greater than zero.", iLine, iPos);
			return false;
		}
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// The Projection FUNCTION
/// Project par 1 onto par 2

bool CCLUCodeBase::ProjectFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	
	if (iVarCount == 2)
	{
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
	
		rVar = Project(vB, vA);
	}
	else
	{
		m_ErrorList.WrongNoOfParams(2, iLine, iPos);
		return false;
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////
/// The Rejection FUNCTION
/// Reject par 1 from par 2

bool CCLUCodeBase::RejectFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	
	if (iVarCount == 2)
	{
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
	
		rVar = Reject(vB, vA);
	}
	else
	{
		m_ErrorList.WrongNoOfParams(2, iLine, iPos);
		return false;
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Absolute FUNCTION

bool CCLUCodeBase::AbsFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	if (iVarCount == 1)
	{
		return Abs(rVar, mVars(0), iLine, iPos);
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Absolute Value

bool CCLUCodeBase::Abs(CCodeVar& rVar, CCodeVar& rData, int iLine, int iPos)
{
	TCVScalar dVal;
	MultiV<TCVScalar> MV;
	TMatrix Mat;
	ECodeDataType eType = rData.BaseType();

	if (eType == PDT_VARLIST)
	{
		TVarList &rList = *rData.GetVarListPtr();
		int i, iCount = rList.Count();

		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for ( i = 0; i < iCount; i++)
		{
			if (!Abs(rRetList[i], rList(i), iLine, iPos))
				return false;
		}
	}
	else
	{
		switch(eType)
		{
		case PDT_INT:
			rVar = Mag( *((int *) rData.GetIntPtr()) );
			break;

		case PDT_UINT:
			rVar = *((uint *) rData.GetUIntPtr());
			break;

		case PDT_LONG:
			rVar = Mag( *((long *) rData.GetLongPtr()) );
			break;

		case PDT_FLOAT:
			rVar = Mag( *((float *) rData.GetFloatPtr()) );
			break;

		case PDT_DOUBLE:
			rVar = Mag( *((double *) rData.GetDoublePtr()) );
			break;

		case PDT_MULTIV:
			MV = *((MultiV<TCVScalar> *) rData.GetMultiVPtr());
			if (!rData.CastToScalar(dVal, m_fSensitivity))
			{
				rVar = TCVScalar(sqrt(double(Mag(Scalar(MV * (~MV))))));
			}
			else
			{
				rVar = Mag(dVal);
			}
			break;

		case PDT_MATRIX:
			rVar = *rData.GetMatrixPtr();
			rVar.GetMatrixPtr()->AbsComps();
			break;

		default:
			m_ErrorList.InvalidType(rData, iLine, iPos);
			return false;
		}
	}

	return true;
}


/*
//////////////////////////////////////////////////////////////////////
/// Argument FUNCTION

bool CCLUCodeBase::ArgFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	if (iVarCount == 1)
	{
		return Arg(rVar, mVars(0), iLine, iPos);
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Argument Value

bool CCLUCodeBase::Arg(CCodeVar& rVar, CCodeVar& rData, int iLine, int iPos)
{
	TCVScalar dVal;
	MultiV<TCVScalar> MV;
	TMatrix Mat;
	ECodeDataType eType = rData.BaseType();

	if (eType == PDT_VARLIST)
	{
		TVarList &rList = *rData.GetVarListPtr();
		int i, iCount = rList.Count();

		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for ( i = 0; i < iCount; i++)
		{
			if (!Arg(rRetList[i], rList(i), iLine, iPos))
				return false;
		}
	}
	else
	{
		switch(eType)
		{
		case PDT_INT:
			rVar = Mag( *((int *) rData.GetIntPtr()) );
			break;

		case PDT_UINT:
			rVar = *((uint *) rData.GetUIntPtr());
			break;

		case PDT_LONG:
			rVar = Mag( *((long *) rData.GetLongPtr()) );
			break;

		case PDT_FLOAT:
			rVar = Mag( *((float *) rData.GetFloatPtr()) );
			break;

		case PDT_DOUBLE:
			rVar = Mag( *((double *) rData.GetDoublePtr()) );
			break;

		case PDT_MULTIV:
			MV = *((MultiV<TCVScalar> *) rData.GetMultiVPtr());
			if (!rData.CastToScalar(dVal, m_fSensitivity))
			{
				rVar = TCVScalar(sqrt(double(Mag(Scalar(MV * (~MV))))));
			}
			else
			{
				rVar = Mag(dVal);
			}
			break;

		case PDT_MATRIX:
			Mat = *rData.GetMatrixPtr();
			rVar = Mat.AbsComps();
			break;

		default:
			m_ErrorList.InvalidType(rData, iLine, iPos);
			return false;
		}
	}

	return true;
}

*/




//////////////////////////////////////////////////////////////////////
/// Summation FUNCTION

bool CCLUCodeBase::SumFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	if (iVarCount == 1)
	{
		if (mVars(0).BaseType() == PDT_VARLIST)
		{
            return Sum(rVar, *mVars(0).GetVarListPtr(), iLine, iPos);
		}
		else if (mVars(0).BaseType() == PDT_MATRIX)
		{
			TMatrix &mA = *mVars(0).GetMatrixPtr();

			TCVScalar dSum;
			int iR, iC, iRows, iCols;
			iRows = mA.Rows();
			iCols = mA.Cols();

			if (!iRows || !iCols)
			{
				m_ErrorList.GeneralError("Matrix passed is empty.", iLine, iPos);
				return false;
			}

			// Only sum rows separately. 
			rVar.New(PDT_MATRIX, "Constant");
			TMatrix &rColMat = *rVar.GetMatrixPtr();
			rColMat.Resize(iRows, 1);

			for (iR = 1; iR <= iRows; iR++)
			{
				dSum = mA(iR, 1);
				for (iC = 2; iC <= iCols; iC++)
				{
					dSum += mA(iR, iC);
				}

				rColMat(iR,1) = dSum;
			}
		}
		else
		{
			m_ErrorList.InvalidType(mVars(0), iLine, iPos);			
			return false;
		}
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Sum of  List

bool CCLUCodeBase::Sum(CCodeVar& rVar, TVarList& rList, int iLine, int iPos)
{
	MultiV<TCVScalar> MV;
	TMatrix Mat;

	int i, iCount = rList.Count();
	if (!iCount)
		return true;

	if (rList(0).BaseType() == PDT_VARLIST)
	{
		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for ( i = 0; i < iCount; i++)
		{
			CCodeVar &rSubVar = rList(i);
			if (rSubVar.BaseType() == PDT_VARLIST)
			{
				TVarList &rSubList = *rSubVar.GetVarListPtr();

				if (!Sum(rRetList[i], rSubList, iLine, iPos))
					return false;
			}
			else
			{
				m_ErrorList.GeneralError("Expect nested lists.", iLine, iPos);
				return false;
			}
		}
	}
	else
	{
		CCodeVar ValVar;
		rVar = rList(0);

		for ( i = 1; i < iCount; i++ )
		{
			if (!OpAdd(rVar, rList(i), ValVar, iLine, iPos))
				return false;

			rVar = ValVar;
		}
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Subtraction FUNCTION

bool CCLUCodeBase::SubFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	if (iVarCount == 1)
	{
		if (mVars(0).BaseType() == PDT_VARLIST)
		{
            return Sub(rVar, *mVars(0).GetVarListPtr(), iLine, iPos);
		}
		else if (mVars(0).BaseType() == PDT_MATRIX)
		{
			TMatrix &mA = *mVars(0).GetMatrixPtr();

			TCVScalar dSum;
			int iR, iC, iRows, iCols;
			iRows = mA.Rows();
			iCols = mA.Cols();

			if (!iRows || !iCols)
			{
				m_ErrorList.GeneralError("Matrix passed is empty.", iLine, iPos);
				return false;
			}

			// Only sum rows separately. 
			rVar.New(PDT_MATRIX, "Constant");
			TMatrix &rColMat = *rVar.GetMatrixPtr();
			rColMat.Resize(iRows, 1);

			for (iR = 1; iR <= iRows; iR++)
			{
				dSum = mA(iR, 1);
				for (iC = 2; iC <= iCols; iC++)
				{
					dSum -= mA(iR, iC);
				}

				rColMat(iR,1) = dSum;
			}
		}
		else
		{
			m_ErrorList.InvalidType(mVars(0), iLine, iPos);			
			return false;
		}
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Subtraction of  List
/// From first element the rest is subtracted.

bool CCLUCodeBase::Sub(CCodeVar& rVar, TVarList& rList, int iLine, int iPos)
{
	MultiV<TCVScalar> MV;
	TMatrix Mat;

	int i, iCount = rList.Count();
	if (!iCount)
		return true;

	if (rList(0).BaseType() == PDT_VARLIST)
	{
		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for ( i = 0; i < iCount; i++)
		{
			CCodeVar &rSubVar = rList(i);
			if (rSubVar.BaseType() == PDT_VARLIST)
			{
				TVarList &rSubList = *rSubVar.GetVarListPtr();

				if (!Sub(rRetList[i], rSubList, iLine, iPos))
					return false;
			}
			else
			{
				m_ErrorList.GeneralError("Expect nested lists.", iLine, iPos);
				return false;
			}
		}
	}
	else
	{
		CCodeVar ValVar;
		rVar = rList(0);

		for ( i = 1; i < iCount; i++ )
		{
			if (!OpSubtract(rVar, rList(i), ValVar, iLine, iPos))
				return false;

			rVar = ValVar;
		}
	}

	return true;
}



//////////////////////////////////////////////////////////////////////
/// Product FUNCTION

bool CCLUCodeBase::ProdFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	if (iVarCount == 1)
	{
		if (mVars(0).BaseType() == PDT_VARLIST)
		{
            return Prod(rVar, *mVars(0).GetVarListPtr(), iLine, iPos);
		}
		else if (mVars(0).BaseType() == PDT_MATRIX)
		{
			TMatrix &mA = *mVars(0).GetMatrixPtr();

			TCVScalar dProd;
			int iR, iC, iRows, iCols;
			iRows = mA.Rows();
			iCols = mA.Cols();

			if (!iRows || !iCols)
			{
				m_ErrorList.GeneralError("Matrix passed is empty.", iLine, iPos);
				return false;
			}

			// Only take product of rows separately. 
			rVar.New(PDT_MATRIX, "Constant");
			TMatrix &rColMat = *rVar.GetMatrixPtr();
			rColMat.Resize(iRows, 1);

			for (iR = 1; iR <= iRows; iR++)
			{
				dProd = mA(iR, 1);
				for (iC = 2; iC <= iCols; iC++)
				{
					dProd *= mA(iR, iC);
				}

				rColMat(iR,1) = dProd;
			}
		}
		else
		{
			m_ErrorList.InvalidType(mVars(0), iLine, iPos);			
			return false;
		}
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Product of  List

bool CCLUCodeBase::Prod(CCodeVar& rVar, TVarList& rList, int iLine, int iPos)
{
	MultiV<TCVScalar> MV;
	TMatrix Mat;

	int i, iCount = rList.Count();
	if (!iCount)
		return true;

	if (rList(0).BaseType() == PDT_VARLIST)
	{
		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for ( i = 0; i < iCount; i++)
		{
			CCodeVar &rSubVar = rList(i);
			if (rSubVar.BaseType() == PDT_VARLIST)
			{
				TVarList &rSubList = *rSubVar.GetVarListPtr();

				if (!Prod(rRetList[i], rSubList, iLine, iPos))
					return false;
			}
			else
			{
				m_ErrorList.GeneralError("Expect nested lists.", iLine, iPos);
				return false;
			}
		}
	}
	else
	{
		CCodeVar ValVar;
		rVar = rList(0);

		for ( i = 1; i < iCount; i++ )
		{
			if (!OpProd(rVar, rList(i), ValVar, iLine, iPos))
				return false;

			rVar = ValVar;
		}
	}

	return true;
}




//////////////////////////////////////////////////////////////////////
/// Outer Product FUNCTION

bool CCLUCodeBase::OuterProdFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	if (iVarCount == 1)
	{
		if (mVars(0).BaseType() == PDT_VARLIST)
		{
            return OuterProd(rVar, *mVars(0).GetVarListPtr(), iLine, iPos);
		}
		else
		{
			m_ErrorList.InvalidType(mVars(0), iLine, iPos);			
			return false;
		}
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Product of  List

bool CCLUCodeBase::OuterProd(CCodeVar& rVar, TVarList& rList, int iLine, int iPos)
{
	MultiV<TCVScalar> MV;
	TMatrix Mat;

	int i, iCount = rList.Count();
	if (!iCount)
		return true;

	if (rList(0).BaseType() == PDT_VARLIST)
	{
		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for ( i = 0; i < iCount; i++)
		{
			CCodeVar &rSubVar = rList(i);
			if (rSubVar.BaseType() == PDT_VARLIST)
			{
				TVarList &rSubList = *rSubVar.GetVarListPtr();

				if (!OuterProd(rRetList[i], rSubList, iLine, iPos))
					return false;
			}
			else
			{
				m_ErrorList.GeneralError("Expect nested lists.", iLine, iPos);
				return false;
			}
		}
	}
	else
	{
		CCodeVar ValVar;
		rVar = rList(0);

		for ( i = 1; i < iCount; i++ )
		{
			if (!OpWedge(rVar, rList(i), ValVar, iLine, iPos))
				return false;

			rVar = ValVar;
		}
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Inner Product FUNCTION

bool CCLUCodeBase::InnerProdFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	if (iVarCount == 1)
	{
		if (mVars(0).BaseType() == PDT_VARLIST)
		{
            return InnerProd(rVar, *mVars(0).GetVarListPtr(), iLine, iPos);
		}
		else
		{
			m_ErrorList.InvalidType(mVars(0), iLine, iPos);			
			return false;
		}
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Inner Product of  List

bool CCLUCodeBase::InnerProd(CCodeVar& rVar, TVarList& rList, int iLine, int iPos)
{
	MultiV<TCVScalar> MV;
	TMatrix Mat;

	int i, iCount = rList.Count();
	if (!iCount)
		return true;

	if (rList(0).BaseType() == PDT_VARLIST)
	{
		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for ( i = 0; i < iCount; i++)
		{
			CCodeVar &rSubVar = rList(i);
			if (rSubVar.BaseType() == PDT_VARLIST)
			{
				TVarList &rSubList = *rSubVar.GetVarListPtr();

				if (!InnerProd(rRetList[i], rSubList, iLine, iPos))
					return false;
			}
			else
			{
				m_ErrorList.GeneralError("Expect nested lists.", iLine, iPos);
				return false;
			}
		}
	}
	else
	{
		CCodeVar ValVar;
		rVar = rList(0);

		for ( i = 1; i < iCount; i++ )
		{
			if (!OpInnerProd(rVar, rList(i), ValVar, iLine, iPos))
				return false;

			rVar = ValVar;
		}
	}

	return true;
}






//////////////////////////////////////////////////////////////////////
/// Minimum Value FUNCTION

bool CCLUCodeBase::MinFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	if (iVarCount == 1)
	{
		if (mVars(0).BaseType() == PDT_VARLIST)
		{
            return Min(rVar, *mVars(0).GetVarListPtr(), iLine, iPos);
		}
		else if (mVars(0).BaseType() == PDT_MATRIX)
		{
			TMatrix &mA = *mVars(0).GetMatrixPtr();

			TCVScalar dMinVal;
			int iR, iC, iRows, iCols;
			iRows = mA.Rows();
			iCols = mA.Cols();

			if (!iRows || !iCols)
			{
				m_ErrorList.GeneralError("Matrix passed is empty.", iLine, iPos);
				return false;
			}

			rVar.New(PDT_VARLIST, "Constant");
			TVarList &rRowList = *rVar.GetVarListPtr();
			rRowList.Set(iRows);

			for (iR = 1; iR <= iRows; iR++)
			{
				dMinVal = mA(iR, 1);
				for (iC = 2; iC <= iCols; iC++)
				{
					if (mA(iR, iC) < dMinVal)
						dMinVal = mA(iR, iC);
				}

				rRowList[iR-1] = dMinVal;
			}
		}
		else
		{
			m_ErrorList.InvalidType(mVars(0), iLine, iPos);			
			return false;
		}
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Minimum Value in List

bool CCLUCodeBase::Min(CCodeVar& rVar, TVarList& rList, int iLine, int iPos)
{
	MultiV<TCVScalar> MV;
	TMatrix Mat;

	int i, iCount = rList.Count();
	if (!iCount)
		return true;

	if (rList(0).BaseType() == PDT_VARLIST)
	{
		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for ( i = 0; i < iCount; i++)
		{
			CCodeVar &rSubVar = rList(i);
			if (rSubVar.BaseType() == PDT_VARLIST)
			{
				TVarList &rSubList = *rSubVar.GetVarListPtr();

				if (!Min(rRetList[i], rSubList, iLine, iPos))
					return false;
			}
			else
			{
				m_ErrorList.GeneralError("Expect nested lists.", iLine, iPos);
				return false;
			}
		}
	}
	else
	{
		TCVScalar dVal, dMinVal;
		if (!rList(0).CastToScalar(dMinVal, m_fSensitivity))
		{
			cStr csText;
			csText << "Cannot cast element at position " << 1 << " in (sub-)list to scalar.";
			m_ErrorList.GeneralError(csText.Str(), iLine, iPos);
			return false;
		}

		for ( i = 1; i < iCount; i++ )
		{
			if (!rList(i).CastToScalar(dVal, m_fSensitivity))
			{
				cStr csText;
				csText << "Cannot cast element at position " << (i+1) << " in (sub-)list to scalar.";
				m_ErrorList.GeneralError(csText.Str(), iLine, iPos);
				return false;
			}

			if (dVal < dMinVal)
				dMinVal = dVal;
		}

		rVar = dMinVal;
	}

	return true;
}




//////////////////////////////////////////////////////////////////////
/// Maximum Value FUNCTION

bool CCLUCodeBase::MaxFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	if (iVarCount == 1)
	{
		if (mVars(0).BaseType() == PDT_VARLIST)
		{
            return Max(rVar, *mVars(0).GetVarListPtr(), iLine, iPos);
		}
		else if (mVars(0).BaseType() == PDT_MATRIX)
		{
			TMatrix &mA = *mVars(0).GetMatrixPtr();

			TCVScalar dMaxVal;
			int iR, iC, iRows, iCols;
			iRows = mA.Rows();
			iCols = mA.Cols();

			if (!iRows || !iCols)
			{
				m_ErrorList.GeneralError("Matrix passed is empty.", iLine, iPos);
				return false;
			}

			rVar.New(PDT_VARLIST, "Constant");
			TVarList &rRowList = *rVar.GetVarListPtr();
			rRowList.Set(iRows);

			for (iR = 1; iR <= iRows; iR++)
			{
				dMaxVal = mA(iR, 1);
				for (iC = 2; iC <= iCols; iC++)
				{
					if (mA(iR, iC) > dMaxVal)
						dMaxVal = mA(iR, iC);
				}

				rRowList[iR-1] = dMaxVal;
			}
		}
		else
		{
			m_ErrorList.InvalidType(mVars(0), iLine, iPos);			
			return false;
		}
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Maximum Value in List

bool CCLUCodeBase::Max(CCodeVar& rVar, TVarList& rList, int iLine, int iPos)
{
	MultiV<TCVScalar> MV;
	TMatrix Mat;

	int i, iCount = rList.Count();
	if (!iCount)
		return true;

	if (rList(0).BaseType() == PDT_VARLIST)
	{
		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for ( i = 0; i < iCount; i++)
		{
			CCodeVar &rSubVar = rList(i);
			if (rSubVar.BaseType() == PDT_VARLIST)
			{
				TVarList &rSubList = *rSubVar.GetVarListPtr();

				if (!Max(rRetList[i], rSubList, iLine, iPos))
					return false;
			}
			else
			{
				m_ErrorList.GeneralError("Expect nested lists.", iLine, iPos);
				return false;
			}
		}
	}
	else
	{
		TCVScalar dVal, dMaxVal;
		if (!rList(0).CastToScalar(dMaxVal, m_fSensitivity))
		{
			cStr csText;
			csText << "Cannot cast element at position " << 1 << " in (sub-)list to scalar.";
			m_ErrorList.GeneralError(csText.Str(), iLine, iPos);
			return false;
		}

		for ( i = 1; i < iCount; i++ )
		{
			if (!rList(i).CastToScalar(dVal, m_fSensitivity))
			{
				cStr csText;
				csText << "Cannot cast element at position " << (i+1) << " in (sub-)list to scalar.";
				m_ErrorList.GeneralError(csText.Str(), iLine, iPos);
				return false;
			}

			if (dVal > dMaxVal)
				dMaxVal = dVal;
		}

		rVar = dMaxVal;
	}

	return true;
}




//////////////////////////////////////////////////////////////////////
/// Argument of Minimum Value FUNCTION

bool CCLUCodeBase::ArgMinFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	if (iVarCount == 1)
	{
		if (mVars(0).BaseType() == PDT_VARLIST)
		{
			rVar.New(PDT_VARLIST, "Constant");
			TVarList& rIdxList = *rVar.GetVarListPtr();

            return ArgMin(rIdxList, *mVars(0).GetVarListPtr(), iLine, iPos);
		}
		else if (mVars(0).BaseType() == PDT_MATRIX)
		{
			TMatrix &mA = *mVars(0).GetMatrixPtr();

			TCVScalar dMinVal;
			TCVCounter iMinIdx;
			int iR, iC, iRows, iCols;
			iRows = mA.Rows();
			iCols = mA.Cols();

			if (!iRows || !iCols)
			{
				m_ErrorList.GeneralError("Matrix passed is empty.", iLine, iPos);
				return false;
			}

			rVar.New(PDT_VARLIST, "Constant");
			TVarList &rRowList = *rVar.GetVarListPtr();
			rRowList.Set(iRows);

			for (iR = 1; iR <= iRows; iR++)
			{
				dMinVal = mA(iR, 1);
				iMinIdx = 1;
				for (iC = 2; iC <= iCols; iC++)
				{
					if (mA(iR, iC) < dMinVal)
					{
						dMinVal = mA(iR, iC);
						iMinIdx = TCVCounter(iC);
					}
				}

				rRowList[iR-1].New(PDT_VARLIST, "Constant");
				TVarList &rMatIdx = *rRowList[iR-1].GetVarListPtr();
				rMatIdx.Set(2);
				rMatIdx[0] = iR;
				rMatIdx[1] = iMinIdx;
			}
		}
		else
		{
			m_ErrorList.InvalidType(mVars(0), iLine, iPos);			
			return false;
		}
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Argument of Minimum Value in List

bool CCLUCodeBase::ArgMin(TVarList& rIdxList, TVarList& rList, int iLine, int iPos)
{
	MultiV<TCVScalar> MV;
	TMatrix Mat;

	rIdxList.Set(0);

	int i, iCount = rList.Count();
	if (!iCount)
		return true;

	if (rList(0).BaseType() == PDT_VARLIST)
	{
		TVarList SubIdxList;

		for ( i = 0; i < iCount; i++)
		{
			CCodeVar &rSubVar = rList(i);
			if (rSubVar.BaseType() == PDT_VARLIST)
			{
				TVarList &rSubList = *rSubVar.GetVarListPtr();

				if (!ArgMin(SubIdxList, rSubList, iLine, iPos))
					return false;

				int j, iSubCount = SubIdxList.Count();
				for ( j = 0; j < iSubCount; j++ )
				{
					rIdxList.Add(1);
					rIdxList.Last() = SubIdxList(j);
					TVarList &rIdx = *rIdxList.Last().GetVarListPtr();
					rIdx.Insert(0);
					rIdx(0) = i+1;
				}
			}
			else
			{
				m_ErrorList.GeneralError("Expect nested lists.", iLine, iPos);
				return false;
			}
		}
	}
	else
	{
		TCVScalar dVal, dMinVal;
		int iMinIdx = 0;

		if (!rList(0).CastToScalar(dMinVal, m_fSensitivity))
		{
			cStr csText;
			csText << "Cannot cast element at position " << 1 << " in (sub-)list to scalar.";
			m_ErrorList.GeneralError(csText.Str(), iLine, iPos);
			return false;
		}

		for ( i = 1; i < iCount; i++ )
		{
			if (!rList(i).CastToScalar(dVal, m_fSensitivity))
			{
				cStr csText;
				csText << "Cannot cast element at position " << (i+1) << " in (sub-)list to scalar.";
				m_ErrorList.GeneralError(csText.Str(), iLine, iPos);
				return false;
			}

			if (dVal < dMinVal)
			{
				dMinVal = dVal;
				iMinIdx = i;
			}
		}
		
		rIdxList.Add(1);
		rIdxList.Last().New(PDT_VARLIST, "Constant");
		TVarList &rIdx = *rIdxList.Last().GetVarListPtr();
		rIdx.Add(1);
		rIdx[0] = TCVCounter(iMinIdx+1);
	}

	return true;
}




//////////////////////////////////////////////////////////////////////
/// Argument of Maximum Value FUNCTION

bool CCLUCodeBase::ArgMaxFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	if (iVarCount == 1)
	{
		if (mVars(0).BaseType() == PDT_VARLIST)
		{
			rVar.New(PDT_VARLIST, "Constant");
			TVarList& rIdxList = *rVar.GetVarListPtr();

            return ArgMax(rIdxList, *mVars(0).GetVarListPtr(), iLine, iPos);
		}
		else if (mVars(0).BaseType() == PDT_MATRIX)
		{
			TMatrix &mA = *mVars(0).GetMatrixPtr();

			TCVScalar dMaxVal;
			TCVCounter iMaxIdx;
			int iR, iC, iRows, iCols;
			iRows = mA.Rows();
			iCols = mA.Cols();

			if (!iRows || !iCols)
			{
				m_ErrorList.GeneralError("Matrix passed is empty.", iLine, iPos);
				return false;
			}

			rVar.New(PDT_VARLIST, "Constant");
			TVarList &rRowList = *rVar.GetVarListPtr();
			rRowList.Set(iRows);

			for (iR = 1; iR <= iRows; iR++)
			{
				dMaxVal = mA(iR, 1);
				iMaxIdx = 1;
				for (iC = 2; iC <= iCols; iC++)
				{
					if (mA(iR, iC) > dMaxVal)
					{
						dMaxVal = mA(iR, iC);
						iMaxIdx = TCVCounter(iC);
					}
				}

				rRowList[iR-1].New(PDT_VARLIST, "Constant");
				TVarList &rMatIdx = *rRowList[iR-1].GetVarListPtr();
				rMatIdx.Set(2);
				rMatIdx[0] = iR;
				rMatIdx[1] = iMaxIdx;
			}
		}
		else
		{
			m_ErrorList.InvalidType(mVars(0), iLine, iPos);			
			return false;
		}
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Argument of Maximum Value in List

bool CCLUCodeBase::ArgMax(TVarList& rIdxList, TVarList& rList, int iLine, int iPos)
{
	MultiV<TCVScalar> MV;
	TMatrix Mat;

	rIdxList.Set(0);

	int i, iCount = rList.Count();
	if (!iCount)
		return true;

	if (rList(0).BaseType() == PDT_VARLIST)
	{
		TVarList SubIdxList;

		for ( i = 0; i < iCount; i++)
		{
			CCodeVar &rSubVar = rList(i);
			if (rSubVar.BaseType() == PDT_VARLIST)
			{
				TVarList &rSubList = *rSubVar.GetVarListPtr();

				if (!ArgMax(SubIdxList, rSubList, iLine, iPos))
					return false;

				int j, iSubCount = SubIdxList.Count();
				for ( j = 0; j < iSubCount; j++ )
				{
					rIdxList.Add(1);
					rIdxList.Last() = SubIdxList(j);
					TVarList &rIdx = *rIdxList.Last().GetVarListPtr();
					rIdx.Insert(0);
					rIdx(0) = i+1;
				}
			}
			else
			{
				m_ErrorList.GeneralError("Expect nested lists.", iLine, iPos);
				return false;
			}
		}
	}
	else
	{
		TCVScalar dVal, dMaxVal;
		int iMaxIdx = 0;

		if (!rList(0).CastToScalar(dMaxVal, m_fSensitivity))
		{
			cStr csText;
			csText << "Cannot cast element at position " << 1 << " in (sub-)list to scalar.";
			m_ErrorList.GeneralError(csText.Str(), iLine, iPos);
			return false;
		}

		for ( i = 1; i < iCount; i++ )
		{
			if (!rList(i).CastToScalar(dVal, m_fSensitivity))
			{
				cStr csText;
				csText << "Cannot cast element at position " << (i+1) << " in (sub-)list to scalar.";
				m_ErrorList.GeneralError(csText.Str(), iLine, iPos);
				return false;
			}

			if (dVal > dMaxVal)
			{
				dMaxVal = dVal;
				iMaxIdx = i;
			}
		}
		
		rIdxList.Add(1);
		rIdxList.Last().New(PDT_VARLIST, "Constant");
		TVarList &rIdx = *rIdxList.Last().GetVarListPtr();
		rIdx.Add(1);
		rIdx[0] = TCVCounter(iMaxIdx+1);
	}

	return true;
}






//////////////////////////////////////////////////////////////////////
/// Inverse FUNCTION

bool CCLUCodeBase::InvFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	
	if (iVarCount != 1)
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() == PDT_MATRIX)
	{
		rVar = *mVars(0).GetMatrixPtr();
		TMatrix& rMat = *rVar.GetMatrixPtr();

		rMat.InvComps(0, m_fSensitivity);
	}
	else if (mVars(0).BaseType() == PDT_TENSOR_IDX)
	{
		TTensorIdx &rTIdxData = *mVars(0).GetTensorIdxPtr();

		// Create a temporary variable that contains tensor.
		// Do this, so that we can return a tensor index.
		m_mTempVarList.Add(1);
		CCodeVar &rTemp = m_mTempVarList.Last();
		rTemp.New(PDT_TENSOR, "Constant");
		TTensor& rT = *rTemp.GetTensorPtr();

		rVar.New(PDT_TENSOR_IDX, "Constant");
		TTensorIdx& rTIdx = *rVar.GetTensorIdxPtr();

		if (!TensorIdxInvertComps(rT, rTIdx, rTIdxData, m_fSensitivity))
		{
			m_ErrorList.GeneralError("Error inverting tensor components.", iLine, iPos);
			return false;
		}
	}
	else if (mVars(0).BaseType() == PDT_TENSOR)
	{
		TTensor &rTData = *mVars(0).GetTensorPtr();

		rVar.New(PDT_TENSOR, "Constant");
		TTensor& rT = *rVar.GetTensorPtr();

		if (!TensorInvertComps(rT, rTData))
		{
			m_ErrorList.GeneralError("Error inverting tensor components.", iLine, iPos);
			return false;
		}
	}
	else
	{
		m_ErrorList.GeneralError("Inv() function can only be applied to matrices and tensors, where it inverts the components separately. "
								"Use the operator '!' for a general inverse.", iLine, iPos);
		return false;
	}

	return true;
}



//////////////////////////////////////////////////////////////////////
/// Square Root FUNCTION

bool CCLUCodeBase::SqrtFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	
	if (iVarCount == 1)
	{
		if (!Sqrt(rVar, mVars(0), iLine, iPos))
			return false;
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Square Root Value

bool CCLUCodeBase::Sqrt(CCodeVar& rVar, CCodeVar& rData, int iLine, int iPos)
{
	TCVScalar fVal;
	
	if (rData.CastToScalar(fVal, m_fSensitivity))
	{
		if (fVal < 0.0 && fVal >= -m_fSensitivity)
			rVar = TCVScalar(0);
		else if (fVal >= 0.0)
			rVar = (TCVScalar) sqrt((double) fVal);
		else
		{
			m_ErrorList.GeneralError("Parameter has to be greater or equal to zero.", iLine, iPos);
			return false;
		}
	}
	else if (rData.BaseType() == PDT_MATRIX)
	{
		rVar = *rData.GetMatrixPtr();
		TMatrix& rMat = *rVar.GetMatrixPtr();

		if (!rMat.SqrtComps(m_fSensitivity))
		{
			m_ErrorList.MatrixIsNAN(iLine, iPos);
			return false;
		}
	}
	else if (rData.BaseType() == PDT_VARLIST)
	{
		// Loop over all elements of list and call this function recursively.
		TVarList &List = *rData.GetVarListPtr();
		int i, iCount = List.Count();

		rVar.New(PDT_VARLIST, "Constant");
		TVarList &ResultList = *rVar.GetVarListPtr();

		ResultList.Set(iCount);
		for ( i = 0; i < iCount; i++)
		{
			if (!Sqrt(ResultList[i], List[i], iLine, iPos))
				return false;
		}
	}
	else
	{
		m_ErrorList.InvalidType(rData, iLine, iPos);
		return false;
	}
	
	
	return true;
}



//////////////////////////////////////////////////////////////////////
/// Power FUNCTION

bool CCLUCodeBase::PowFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	
	if (iVarCount == 2)
	{
		if (!Pow(rVar, mVars(0), mVars(1), iLine, iPos))
			return false;
	}
	else
	{
		m_ErrorList.WrongNoOfParams(2, iLine, iPos);
		return false;
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Power

bool CCLUCodeBase::Pow(CCodeVar& rVar, CCodeVar& rLVar, CCodeVar& rRVar, int iLine, int iPos)
{
	TCVScalar fLVal, fRVal;
	TCVCounter iVal;
	
	if (rRVar.BaseType() == PDT_VARLIST)
	{
		// Loop over all elements of list and call this function recursively.
		TVarList &List = *rRVar.GetVarListPtr();
		int i, iCount = List.Count();

		rVar.New(PDT_VARLIST, "Constant");
		TVarList &ResultList = *rVar.GetVarListPtr();

		ResultList.Set(iCount);
		for ( i = 0; i < iCount; i++)
		{
			if (!Pow(ResultList[i], rLVar, List[i], iLine, iPos))
				return false;
		}

		return true;
	}

	if (!rLVar.CastToScalar(fLVal, m_fSensitivity))
	{
		ECodeDataType eLType = rLVar.BaseType();
		
		if (eLType == PDT_MULTIV)
		{
			// Multivectors can only be taken to an integer power.
			if (!rRVar.CastToCounter(iVal))
			{
				m_ErrorList.InvalidType(rRVar, iLine, iPos);
				return false;
			}
			
			TMultiV vA(*rLVar.GetMultiVPtr());
			TMultiV vR;
			
			if (iVal < 0)
			{
				vA = (!vA).Round(m_fSensitivity);
				iVal = -iVal;
			}
			else if (iVal == 0)
			{
				vA = (TCVScalar) 1;
			}
			
			cStr csVal;
			
			vR = vA;
			
			for(int i=1;i<iVal;i++)
			{
				vR &= vA;
			}
			
			rVar = vR.Round(m_fSensitivity);
		}
		else if (eLType == PDT_VARLIST)
		{
			// Loop over all elements of list and call this function recursively.
			TVarList &List = *rLVar.GetVarListPtr();
			int i, iCount = List.Count();

			rVar.New(PDT_VARLIST, "Constant");
			TVarList &ResultList = *rVar.GetVarListPtr();

			ResultList.Set(iCount);
			for ( i = 0; i < iCount; i++)
			{
				if (!Pow(ResultList[i], List[i], rRVar, iLine, iPos))
					return false;
			}
		}
		else if (eLType == PDT_MATRIX)
		{
			if (!rRVar.CastToScalar(fRVal, m_fSensitivity))
			{
				m_ErrorList.InvalidType(rRVar, iLine, iPos);
				return false;
			}

			rVar = *rLVar.GetMatrixPtr();
			TMatrix& rMat = *rVar.GetMatrixPtr();

			// Take power of matrix components separately
			if (!rMat.PowComps(fRVal, m_fSensitivity))
			{
				m_ErrorList.MatrixIsNAN(iLine, iPos);
				return false;
			}
		}
		else
		{
			m_ErrorList.InvalidParType(rLVar, 1, iLine, iPos);
			return false;
		}
	}
	else if (rRVar.CastToScalar(fRVal, m_fSensitivity))
	{
		if (fLVal > -m_fSensitivity && fLVal < m_fSensitivity &&
			fRVal < 0.0)
		{
			m_ErrorList.DivByZero(iLine, iPos);
			return false;
		}

		TCVScalar fVal = (TCVScalar) pow((double) fLVal, (double) fRVal);

		if (_isnan(fVal))
		{
			m_ErrorList.IsNAN(fVal, iLine, iPos);
			return false;
		}

		rVar = fVal;
	}
	else
	{
		m_ErrorList.InvalidParType(rRVar, 2, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Sin FUNCTION
bool CCLUCodeBase::SinFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	if (iVarCount == 1)
	{
		return Sin(rVar, mVars(0), iLine, iPos);
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Sine
bool CCLUCodeBase::Sin(CCodeVar& rVar, CCodeVar& rData, int iLine, int iPos)
{
	TCVScalar dVal;

	if (rData.BaseType() == PDT_VARLIST)
	{
		TVarList &rList = *rData.GetVarListPtr();
		int i, iCount = rList.Count();

		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for ( i = 0; i < iCount; i++)
		{
			if (!Sin(rRetList[i], rList(i), iLine, iPos))
				return false;
		}
	}
	else if (rData.BaseType() == PDT_MATRIX)
	{
		rVar = *rData.GetMatrixPtr();
		TMatrix &rMat = *rVar.GetMatrixPtr();

		// Take sin of matrix components separately
		rMat.SinComps();
	}
	else
	{
		if (!rData.CastToScalar(dVal, m_fSensitivity))
		{
			m_ErrorList.InvalidParType(rData, 1, iLine, iPos);
			return false;
		}								

		rVar = (TCVScalar) sin(dVal);
	}

	return true;
}





//////////////////////////////////////////////////////////////////////
/// Cos FUNCTION
bool CCLUCodeBase::CosFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	if (iVarCount == 1)
	{
		return Cos(rVar, mVars(0), iLine, iPos);
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Cosine
bool CCLUCodeBase::Cos(CCodeVar& rVar, CCodeVar& rData, int iLine, int iPos)
{
	TCVScalar dVal;

	if (rData.BaseType() == PDT_VARLIST)
	{
		TVarList &rList = *rData.GetVarListPtr();
		int i, iCount = rList.Count();

		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for ( i = 0; i < iCount; i++)
		{
			if (!Cos(rRetList[i], rList(i), iLine, iPos))
				return false;
		}
	}
	else if (rData.BaseType() == PDT_MATRIX)
	{
		rVar = *rData.GetMatrixPtr();
		TMatrix &rMat = *rVar.GetMatrixPtr();

		// Take sin of matrix components separately
		rMat.CosComps();
	}
	else
	{
		if (!rData.CastToScalar(dVal, m_fSensitivity))
		{
			m_ErrorList.InvalidParType(rData, 1, iLine, iPos);
			return false;
		}								

		rVar = (TCVScalar) cos(dVal);
	}

	return true;
}



//////////////////////////////////////////////////////////////////////
/// Tan FUNCTION
bool CCLUCodeBase::TanFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	if (iVarCount == 1)
	{
		return Tan(rVar, mVars(0), iLine, iPos);
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Tan
bool CCLUCodeBase::Tan(CCodeVar& rVar, CCodeVar& rData, int iLine, int iPos)
{
	TCVScalar dVal;

	if (rData.BaseType() == PDT_VARLIST)
	{
		TVarList &rList = *rData.GetVarListPtr();
		int i, iCount = rList.Count();

		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for ( i = 0; i < iCount; i++)
		{
			if (!Tan(rRetList[i], rList(i), iLine, iPos))
				return false;
		}
	}
	else if (rData.BaseType() == PDT_MATRIX)
	{
		rVar = *rData.GetMatrixPtr();
		TMatrix &rMat = *rVar.GetMatrixPtr();

		// Take sin of matrix components separately
		rMat.TanComps();
	}
	else
	{
		if (!rData.CastToScalar(dVal, m_fSensitivity))
		{
			m_ErrorList.InvalidParType(rData, 1, iLine, iPos);
			return false;
		}								

		rVar = (TCVScalar) tan(dVal);
	}

	return true;
}



//////////////////////////////////////////////////////////////////////
/// ASin FUNCTION
bool CCLUCodeBase::AsinFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	if (iVarCount == 1)
	{
		return Asin(rVar, mVars(0), iLine, iPos);
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Asin
bool CCLUCodeBase::Asin(CCodeVar& rVar, CCodeVar& rData, int iLine, int iPos)
{
	TCVScalar dVal;

	if (rData.BaseType() == PDT_VARLIST)
	{
		TVarList &rList = *rData.GetVarListPtr();
		int i, iCount = rList.Count();

		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for ( i = 0; i < iCount; i++)
		{
			if (!Asin(rRetList[i], rList(i), iLine, iPos))
				return false;
		}
	}
	else if (rData.BaseType() == PDT_MATRIX)
	{
		rVar = *rData.GetMatrixPtr();
		TMatrix &rMat = *rVar.GetMatrixPtr();

		// Take sin of matrix components separately
		if (!rMat.ArcSinComps(m_fSensitivity))
		{
			m_ErrorList.MatrixIsNAN(iLine, iPos);
			return false;
		}
	}
	else
	{
		if (!rData.CastToScalar(dVal, m_fSensitivity))
		{
			m_ErrorList.InvalidParType(rData, 1, iLine, iPos);
			return false;
		}								

		if (!IsInRange(dVal, -1, 1, iLine, iPos))
			return false;

		rVar = (TCVScalar) asin(dVal);
	}

	return true;
}



//////////////////////////////////////////////////////////////////////
/// Acos FUNCTION
bool CCLUCodeBase::AcosFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	if (iVarCount == 1)
	{
		return Acos(rVar, mVars(0), iLine, iPos);
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Cosine
bool CCLUCodeBase::Acos(CCodeVar& rVar, CCodeVar& rData, int iLine, int iPos)
{
	TCVScalar dVal;

	if (rData.BaseType() == PDT_VARLIST)
	{
		TVarList &rList = *rData.GetVarListPtr();
		int i, iCount = rList.Count();

		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for ( i = 0; i < iCount; i++)
		{
			if (!Acos(rRetList[i], rList(i), iLine, iPos))
				return false;
		}
	}
	else if (rData.BaseType() == PDT_MATRIX)
	{
		rVar = *rData.GetMatrixPtr();
		TMatrix &rMat = *rVar.GetMatrixPtr();

		// Take sin of matrix components separately
		if (!rMat.ArcCosComps(m_fSensitivity))
		{
			m_ErrorList.MatrixIsNAN(iLine, iPos);
			return false;
		}
	}
	else
	{
		if (!rData.CastToScalar(dVal, m_fSensitivity))
		{
			m_ErrorList.InvalidParType(rData, 1, iLine, iPos);
			return false;
		}								

		if (!IsInRange(dVal, -1, 1, iLine, iPos))
			return false;

		rVar = (TCVScalar) acos(dVal);
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Atan FUNCTION
bool CCLUCodeBase::AtanFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount = mVars.Count();
	
	if (iVarCount == 1)
	{
		return Atan(rVar, mVars(0), iLine, iPos);
	}
	else if (iVarCount == 2)
	{
		return Atan(rVar, mVars(0), mVars(1), iLine, iPos);
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Arcus Tangent

bool CCLUCodeBase::Atan(CCodeVar& rVar, CCodeVar& rData, int iLine, int iPos)
{
	TCVScalar dVal;

	if (rData.BaseType() == PDT_VARLIST)
	{
		TVarList &rList = *rData.GetVarListPtr();
		int i, iCount = rList.Count();

		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for ( i = 0; i < iCount; i++)
		{
			if (!Atan(rRetList[i], rList(i), iLine, iPos))
				return false;
		}
	}
	else if (rData.BaseType() == PDT_MATRIX)
	{
		rVar = *rData.GetMatrixPtr();
		TMatrix &rMat = *rVar.GetMatrixPtr();

		// Take sin of matrix components separately
		rMat.ArcTanComps();
	}
	else
	{
		if (!rData.CastToScalar(dVal, m_fSensitivity))
		{
			m_ErrorList.InvalidParType(rData, 1, iLine, iPos);
			return false;
		}								

		rVar = (TCVScalar) atan(dVal);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Arcus Tangent Complete

bool CCLUCodeBase::Atan(CCodeVar& rVar, CCodeVar& rLData, CCodeVar& rRData, int iLine, int iPos)
{
	TCVScalar dVal, dLVal, dRVal;

	if (rLData.BaseType() == PDT_VARLIST && 
		rRData.BaseType() != PDT_VARLIST)
	{
		TVarList &rList = *rLData.GetVarListPtr();
		int i, iCount = rList.Count();

		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for ( i = 0; i < iCount; i++)
		{
			if (!Atan(rRetList[i], rList(i), rRData, iLine, iPos))
				return false;
		}
	}
	else if (rLData.BaseType() != PDT_VARLIST && 
			 rRData.BaseType() == PDT_VARLIST)
	{
		TVarList &rList = *rRData.GetVarListPtr();
		int i, iCount = rList.Count();

		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for ( i = 0; i < iCount; i++)
		{
			if (!Atan(rRetList[i], rLData, rList(i), iLine, iPos))
				return false;
		}
	}
	else if (rLData.BaseType() == PDT_VARLIST && 
			 rRData.BaseType() == PDT_VARLIST)
	{
		TVarList &rLList = *rLData.GetVarListPtr();
		TVarList &rRList = *rRData.GetVarListPtr();
		int i, iCount = rLList.Count();

		if (iCount != rRList.Count())
		{
			m_ErrorList.GeneralError("Lists are not of same length.", iLine, iPos);
			return false;
		}

		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for ( i = 0; i < iCount; i++)
		{
			if (!Atan(rRetList[i], rLList(i), rRList(i), iLine, iPos))
				return false;
		}
	}
/*
	else if (rData.BaseType() == PDT_MATRIX)
	{
		rVar = *rData.GetMatrixPtr();
		TMatrix &rMat = *rVar.GetMatrixPtr();

		// Take sin of matrix components separately
		rMat.ArcTanComps();
	}
*/
	else
	{
		if (!rLData.CastToScalar(dLVal, m_fSensitivity))
		{
			m_ErrorList.GeneralError("Exepcted scalar value as first parameter.", iLine, iPos);
			return false;
		}								

		if (!rRData.CastToScalar(dRVal, m_fSensitivity))
		{
			m_ErrorList.GeneralError("Exepcted scalar value as second parameter.", iLine, iPos);
			return false;
		}

		if (dRVal > 0)
		{
			if (dLVal >= 0)
				dVal = TCVScalar(atan(dLVal/dRVal));
			else
				dVal = 2*m_fPi + TCVScalar(atan(dLVal/dRVal));
		}
		else if (dRVal < 0)
		{
			dVal = m_fPi + TCVScalar(atan(dLVal/dRVal));
		}
		else if (::IsZero(dRVal, m_fSensitivity) &&
				 !::IsZero(dLVal, m_fSensitivity))
		{
			if (dLVal > 0)
				dVal = m_fPi / 2.0;
			else
				dVal = 1.5 * m_fPi;
		}
		else
		{
			m_ErrorList.GeneralError("Undefined result.", iLine, iPos);
			return false;
		}

		rVar = dVal;
	}

	return true;
}


