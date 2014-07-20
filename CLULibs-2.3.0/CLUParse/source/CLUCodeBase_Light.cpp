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
// Functions that set lights.
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "GL/glut.h"

#include "CLUCodeBase.h"


//////////////////////////////////////////////////////////////////////
/// Reset Lights Function
///

bool CCLUCodeBase::ResetLightFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = mVars.Count();
	
	if (iVarCount != 0)
	{
		int piPar[] = {0};

		m_ErrorList.WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (!m_pCLUDrawBase)
	{
		m_ErrorList.Internal(iLine, iPos);
		return false;
	}

	m_pCLUDrawBase->ResetLights();

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Ambient Light Function
///
/// Parameters:
/// 1. Color

bool CCLUCodeBase::SetAmbientLightFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	
	if (iVarCount != 1)
	{
		int piPar[] = {1};

		m_ErrorList.WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_COLOR)
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}								

	COGLColor& rCol = *mVars(0).GetOGLColorPtr();
	
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, rCol.Data());

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Enable Light Function
///
/// Parameters:
/// 1. Light No.
/// 2. 1 / 0 representing enable/disable

bool CCLUCodeBase::EnableLightFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	TCVCounter iLight, iStatus;
	
	if (!m_pCLUDrawBase)
	{
		return false;
	}

	if (iVarCount != 2)
	{
		int piPar[] = {2};

		m_ErrorList.WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToCounter(iLight))
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}								

	if (!mVars(1).CastToCounter(iStatus))
	{
		m_ErrorList.InvalidParType(mVars(1), 1, iLine, iPos);
		return false;
	}								

	if (iLight < 0 || iLight > CLUDRAWBASE_MAXLIGHTCOUNT)
	{
		m_ErrorList.GeneralError("Light number out of range.", iLine, iPos);
		return false;
	}

	if (iLight == 0)
	{
		m_pCLUDrawBase->GetStdLight().Enable((iStatus ? true : false));
	}
	else
	{
		m_pCLUDrawBase->GetSurLight(iLight).Enable((iStatus ? true : false));
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Enable Light Fixed Function
///
/// Parameters:
/// 1. Light No.
/// 2. 1 / 0 representing fixed/not fixed

bool CCLUCodeBase::EnableLightFixedFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	TCVCounter iLight, iStatus;
	
	if (!m_pCLUDrawBase)
	{
		return false;
	}

	if (iVarCount != 2)
	{
		int piPar[] = {2};

		m_ErrorList.WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToCounter(iLight))
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}								

	if (!mVars(1).CastToCounter(iStatus))
	{
		m_ErrorList.InvalidParType(mVars(1), 1, iLine, iPos);
		return false;
	}								

	if (iLight < 1 || iLight > CLUDRAWBASE_MAXLIGHTCOUNT)
	{
		m_ErrorList.GeneralError("Light number out of range.", iLine, iPos);
		return false;
	}

	m_pCLUDrawBase->EnableSurLightPosFixed(iLight, (iStatus ? true : false));

	return true;
}



//////////////////////////////////////////////////////////////////////
/// Set Light Position Function
///
/// Parameters:
/// 1. Light No.
/// 2. Vector (is mapped to projective space)
/// OR 2. - 5. Projective coordinates of point

bool CCLUCodeBase::SetLightPosFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	TCVCounter iLight;
	TCVScalar fVal[4];
	
	if (!m_pCLUDrawBase)
	{
		return false;
	}

	if (iVarCount < 2 || iVarCount > 5)
	{
		int piPar[] = {2, 5};

		m_ErrorList.WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToCounter(iLight))
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}								

	if (iLight < 0 || iLight > CLUDRAWBASE_MAXLIGHTCOUNT)
	{
		m_ErrorList.GeneralError("Light number out of range.", iLine, iPos);
		return false;
	}

	if (iVarCount == 2)
	{
		if (mVars(1).BaseType() != PDT_MULTIV)
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}
		
		TMultiV &vA = *mVars(1).GetMultiVPtr();
		TMultiV vX;

		if (!CastMVtoP3(vA, vX))
		{
			m_ErrorList.GeneralError("Multivector does not represent a vector.", iLine, iPos);
			return false;
		}

		int i;
		for(i = 0; i < 4; i++)
		{
			fVal[i] = vX[i+1];
		}
	}
	else if (iVarCount == 5)
	{
		int i;
		for (i = 1; i < 5; i++)
		{
			if (!mVars(i).CastToScalar(fVal[i-1], m_fSensitivity))
			{
				m_ErrorList.InvalidParType(mVars(i), i+1, iLine, iPos);
				return false;
			}
		}
	}

	if (iLight == 0)
	{
		m_pCLUDrawBase->GetStdLight().Position(
			float(fVal[0]), float(fVal[1]), float(fVal[2]), float(fVal[3]));

		m_pCLUDrawBase->GetStdLight().Apply((GLenum) GL_POSITION);
	}
	else
	{
		m_pCLUDrawBase->GetSurLight(iLight).Position(
			float(fVal[0]), float(fVal[1]), float(fVal[2]), float(fVal[3]));

		//if (!m_pCLUDrawBase->IsSurLightPosFixed(iLight))
			m_pCLUDrawBase->GetSurLight(iLight).Apply((GLenum) GL_POSITION);
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Set Light Ambient Color Function
///
/// Parameters:
/// 1. Light No.
/// 2. Ambient Color

bool CCLUCodeBase::SetLightAmbientColFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	TCVCounter iLight;
	
	if (!m_pCLUDrawBase)
	{
		return false;
	}

	if (iVarCount != 2)
	{
		int piPar[] = {2};

		m_ErrorList.WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToCounter(iLight))
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}								

	if (mVars(1).BaseType() != PDT_COLOR)
	{
		m_ErrorList.InvalidParType(mVars(1), 1, iLine, iPos);
		return false;
	}								

	if (iLight < 0 || iLight > CLUDRAWBASE_MAXLIGHTCOUNT)
	{
		m_ErrorList.GeneralError("Light number out of range.", iLine, iPos);
		return false;
	}

	COGLColor& rCol = *mVars(1).GetOGLColorPtr();

	if (iLight == 0)
	{
		m_pCLUDrawBase->GetStdLight().Ambient(rCol);

		m_pCLUDrawBase->GetStdLight().Apply((GLenum) GL_AMBIENT);
	}
	else
	{
		m_pCLUDrawBase->GetSurLight(iLight).Ambient(rCol);
	
		//if (!m_pCLUDrawBase->IsSurLightPosFixed(iLight))
			m_pCLUDrawBase->GetSurLight(iLight).Apply((GLenum) GL_AMBIENT);
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Set Light Diffuse Color Function
///
/// Parameters:
/// 1. Light No.
/// 2. Diffuse Color

bool CCLUCodeBase::SetLightDiffuseColFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	TCVCounter iLight;
	
	if (!m_pCLUDrawBase)
	{
		return false;
	}

	if (iVarCount != 2)
	{
		int piPar[] = {2};

		m_ErrorList.WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToCounter(iLight))
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}								

	if (mVars(1).BaseType() != PDT_COLOR)
	{
		m_ErrorList.InvalidParType(mVars(1), 1, iLine, iPos);
		return false;
	}								

	if (iLight < 0 || iLight > CLUDRAWBASE_MAXLIGHTCOUNT)
	{
		m_ErrorList.GeneralError("Light number out of range.", iLine, iPos);
		return false;
	}

	COGLColor& rCol = *mVars(1).GetOGLColorPtr();

	if (iLight == 0)
	{
		m_pCLUDrawBase->GetStdLight().Diffuse(rCol);
		m_pCLUDrawBase->GetStdLight().Apply((GLenum) GL_DIFFUSE);
	}
	else
	{
		m_pCLUDrawBase->GetSurLight(iLight).Diffuse(rCol);
	
		//if (!m_pCLUDrawBase->IsSurLightPosFixed(iLight))
			m_pCLUDrawBase->GetSurLight(iLight).Apply((GLenum) GL_DIFFUSE);
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Set Light Specular Color Function
///
/// Parameters:
/// 1. Light No.
/// 2. Specular Color

bool CCLUCodeBase::SetLightSpecularColFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	TCVCounter iLight;
	
	if (!m_pCLUDrawBase)
	{
		return false;
	}

	if (iVarCount != 2)
	{
		int piPar[] = {2};

		m_ErrorList.WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToCounter(iLight))
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}								

	if (mVars(1).BaseType() != PDT_COLOR)
	{
		m_ErrorList.InvalidParType(mVars(1), 1, iLine, iPos);
		return false;
	}								

	if (iLight < 0 || iLight > CLUDRAWBASE_MAXLIGHTCOUNT)
	{
		m_ErrorList.GeneralError("Light number out of range.", iLine, iPos);
		return false;
	}

	COGLColor& rCol = *mVars(1).GetOGLColorPtr();

	if (iLight == 0)
	{
		m_pCLUDrawBase->GetStdLight().Specular(rCol);
		m_pCLUDrawBase->GetStdLight().Apply((GLenum) GL_SPECULAR);
	}
	else
	{
		m_pCLUDrawBase->GetSurLight(iLight).Specular(rCol);
	
		//if (!m_pCLUDrawBase->IsSurLightPosFixed(iLight))
			m_pCLUDrawBase->GetSurLight(iLight).Apply((GLenum) GL_SPECULAR);
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Set Light Spot Direction Function
///
/// Parameters:
/// 1. Light No.
/// 2. Vector (is mapped to Euclidean space)
/// OR 2. - 4. coordinates of direction

bool CCLUCodeBase::SetLightSpotDirFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	TCVCounter iLight;
	TCVScalar fVal[4];
	
	if (!m_pCLUDrawBase)
	{
		return false;
	}

	if (iVarCount < 2 || iVarCount > 4)
	{
		int piPar[] = {2, 4};

		m_ErrorList.WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToCounter(iLight))
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}								

	if (iLight < 0 || iLight > CLUDRAWBASE_MAXLIGHTCOUNT)
	{
		m_ErrorList.GeneralError("Light number out of range.", iLine, iPos);
		return false;
	}

	if (iVarCount == 2)
	{
		if (mVars(1).BaseType() != PDT_MULTIV)
		{
			m_ErrorList.InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}
		
		TMultiV &vA = *mVars(1).GetMultiVPtr();
		TMultiV vX;

		if (!CastMVtoE3(vA, vX))
		{
			m_ErrorList.GeneralError("Multivector does not represent a vector.", iLine, iPos);
			return false;
		}

		int i;
		for(i = 0; i < 3; i++)
		{
			fVal[i] = vX[i+1];
		}
	}
	else if (iVarCount == 4)
	{
		int i;
		for (i = 1; i < 4; i++)
		{
			if (!mVars(i).CastToScalar(fVal[i-1], m_fSensitivity))
			{
				m_ErrorList.InvalidParType(mVars(i), i+1, iLine, iPos);
				return false;
			}
		}
	}

	if (iLight == 0)
	{
		m_pCLUDrawBase->GetStdLight().SpotDir(
			float(fVal[0]), float(fVal[1]), float(fVal[2]));

		m_pCLUDrawBase->GetStdLight().Apply((GLenum) GL_SPOT_DIRECTION);
	}
	else
	{
		m_pCLUDrawBase->GetSurLight(iLight).SpotDir(
			float(fVal[0]), float(fVal[1]), float(fVal[2]));

		//if (!m_pCLUDrawBase->IsSurLightPosFixed(iLight))
			m_pCLUDrawBase->GetSurLight(iLight).Apply((GLenum) GL_SPOT_DIRECTION);
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Set Light Spot Exponent Function
///
/// Parameters:
/// 1. Light No.
/// 2. Exponent value (scalar)

bool CCLUCodeBase::SetLightSpotExpFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	TCVCounter iLight;
	TCVScalar fVal;
	
	if (!m_pCLUDrawBase)
	{
		return false;
	}

	if (iVarCount != 2)
	{
		int piPar[] = {2};

		m_ErrorList.WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToCounter(iLight))
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}								

	if (!mVars(1).CastToScalar(fVal, m_fSensitivity))
	{
		m_ErrorList.InvalidParType(mVars(1), 1, iLine, iPos);
		return false;
	}								

	if (iLight < 0 || iLight > CLUDRAWBASE_MAXLIGHTCOUNT)
	{
		m_ErrorList.GeneralError("Light number out of range.", iLine, iPos);
		return false;
	}

	if (iLight == 0)
	{
		m_pCLUDrawBase->GetStdLight().SpotExp(float(fVal));
		m_pCLUDrawBase->GetStdLight().Apply((GLenum) GL_SPOT_EXPONENT);
	}
	else
	{
		m_pCLUDrawBase->GetSurLight(iLight).SpotExp(float(fVal));

		//if (!m_pCLUDrawBase->IsSurLightPosFixed(iLight))
			m_pCLUDrawBase->GetSurLight(iLight).Apply((GLenum) GL_SPOT_EXPONENT);

	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Light Spot Cut Off Function
///
/// Parameters:
/// 1. Light No.
/// 2. Cut off value (scalar)

bool CCLUCodeBase::SetLightSpotCutOffFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	TCVCounter iLight;
	TCVScalar fVal;
	
	if (!m_pCLUDrawBase)
	{
		return false;
	}

	if (iVarCount != 2)
	{
		int piPar[] = {2};

		m_ErrorList.WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToCounter(iLight))
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}								

	if (!mVars(1).CastToScalar(fVal, m_fSensitivity))
	{
		m_ErrorList.InvalidParType(mVars(1), 1, iLine, iPos);
		return false;
	}								

	if (iLight < 0 || iLight > CLUDRAWBASE_MAXLIGHTCOUNT)
	{
		m_ErrorList.GeneralError("Light number out of range.", iLine, iPos);
		return false;
	}

	if (iLight == 0)
	{
		m_pCLUDrawBase->GetStdLight().SpotCutOff(float(fVal));
		m_pCLUDrawBase->GetStdLight().Apply((GLenum) GL_SPOT_CUTOFF);
	}
	else
	{
		m_pCLUDrawBase->GetSurLight(iLight).SpotCutOff(float(fVal));

		//if (!m_pCLUDrawBase->IsSurLightPosFixed(iLight))
			m_pCLUDrawBase->GetSurLight(iLight).Apply((GLenum) GL_SPOT_CUTOFF);

	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Light Attenuation Function
///
/// Parameters:
/// 1. Light No.
/// 2. Constant attenuation value (scalar)
/// 3. Linear attenuation value (scalar)
/// 4. Quadratic attenuation value (scalar)

bool CCLUCodeBase::SetLightAttenuationFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	TCVCounter iLight;
	TCVScalar fVal[3];
	
	if (!m_pCLUDrawBase)
	{
		return false;
	}

	if (iVarCount != 4)
	{
		int piPar[] = {4};

		m_ErrorList.WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToCounter(iLight))
	{
		m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}								

	int i;

	for (i = 0; i < 3; i++)
	{
		if (!mVars(i+1).CastToScalar(fVal[i], m_fSensitivity))
		{
			m_ErrorList.InvalidParType(mVars(i+1), i+2, iLine, iPos);
			return false;
		}								
	}

	if (iLight < 0 || iLight > CLUDRAWBASE_MAXLIGHTCOUNT)
	{
		m_ErrorList.GeneralError("Light number out of range.", iLine, iPos);
		return false;
	}

	if (iLight == 0)
	{
		COGLLight &rLight = m_pCLUDrawBase->GetStdLight();

		rLight.ConstAtt(float(fVal[0]));
		rLight.LinAtt(float(fVal[1]));
		rLight.QuadAtt(float(fVal[2]));

		rLight.Apply((GLenum) GL_CONSTANT_ATTENUATION);
		rLight.Apply((GLenum) GL_LINEAR_ATTENUATION);
		rLight.Apply((GLenum) GL_QUADRATIC_ATTENUATION);
	}
	else
	{
		COGLLight &rLight = m_pCLUDrawBase->GetSurLight(iLight);
		
		rLight.ConstAtt(float(fVal[0]));
		rLight.LinAtt(float(fVal[1]));
		rLight.QuadAtt(float(fVal[2]));

		//if (!m_pCLUDrawBase->IsSurLightPosFixed(iLight))
		//{
			rLight.Apply((GLenum) GL_CONSTANT_ATTENUATION);
			rLight.Apply((GLenum) GL_LINEAR_ATTENUATION);
			rLight.Apply((GLenum) GL_QUADRATIC_ATTENUATION);
		//}
	}

	return true;
}





