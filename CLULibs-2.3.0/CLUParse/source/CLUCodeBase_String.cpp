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
#include "TensorSingleLoop.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
/// Text Round

double CCLUCodeBase::TextRound(double dVal)
{
	double dFact = pow(10.0, double(m_iTextPrec));
	double dR;

	if (dVal < 0)
		dR = ceil(dVal * dFact - 0.5);
	else
		dR = floor(dVal * dFact + 0.5);

	return dR / dFact;
}

//////////////////////////////////////////////////////////////////////
/// The VecN3 String FUNCTION
/// Returns string of N3 vectors in null basis

bool CCLUCodeBase::VecN3String(TMultiV &_vA, cStr &csVal, EStringType eStrType)
{
	TMultiV vA = _vA;

	// Check that vector is from conformal space
	if (vA.GetBase().BaseID() != ID_CONFGA)
	{
		return false;
	}

	// Check that it is a vector
	Mem<uint> mGList;

	vA.TinyToZero();
	if (!GradeList(vA, mGList, m_fSensitivity))
	{
		csVal = "0";
		return true;
	}

	DeclareConfGAVarsLocal(TCVScalar);
	double dSc, dE[5], dB[10], dI;
	int i, j, iGrade, iGradeCount;
	bool bFirst = true;
	cStr sText, csFormat;
	char pcVal[100];
	TMultiV vX;

	// Create format string
	sprintf(pcVal, "%%.%dlg", m_iTextPrec);
	csFormat = pcVal;

	iGradeCount = mGList.Count();

	for ( j = iGradeCount - 1; j >= 0; j-- )
	{
		iGrade = mGList[j];

		if (iGrade == 0)
		{
			if (!bFirst)
				sText += " + ";
			else
				bFirst = false;

			dSc = vA[iSC];
			sprintf(pcVal, csFormat, dSc);
			sText += pcVal;
		} // if iGrade == 0
		else if (iGrade == 1)
		{
			dE[0] = (double(Scalar(vA * vE1)));
			dE[1] = (double(Scalar(vA * vE2)));
			dE[2] = (double(Scalar(vA * vE3)));
			dE[4] = -(double(Scalar(vA * vE)));
			dE[3] = -(double(Scalar(vA * vE0)));


			for ( i = 0; i < 5; i++ )
			{
				if (dE[i] != 0)
				{
					if (eStrType == STRTYPE_NORMAL)
					{
						if (i == 3)
							sprintf(pcVal, csFormat + "^e", dE[3]);
						else if (i == 4)
							sprintf(pcVal, csFormat + "^e0", dE[4]);
						else
							sprintf(pcVal, csFormat + "^e%d", dE[i], i+1);
					}
					else if (eStrType == STRTYPE_LATEX)
					{
						if (i == 3)
							sprintf(pcVal, csFormat + "\\mathbf{e}_\\infty", dE[3]);
						else if (i == 4)
							sprintf(pcVal, csFormat + "\\mathbf{e}_0", dE[4]);
						else
							sprintf(pcVal, csFormat + "\\mathbf{e}_%d", dE[i], i+1);
					}
					else if (eStrType == STRTYPE_HTML)
					{
						if (i == 3)
							sprintf(pcVal, csFormat + " <b>e</b>", dE[3]);
						else if (i == 4)
							sprintf(pcVal, csFormat + " <b>e0</b>", dE[4]);
						else
							sprintf(pcVal, csFormat + " <b>e%d</b>", dE[i], i+1);
					}

					if (!bFirst)
						sText += " + ";
					else
						bFirst = false;

					sText += pcVal;
				}
			}
		} // if iGrade == 1
		else if (iGrade == 2) // Bivector Components
		{
			dB[0] = -(double(Scalar(vA * vB1))); // e2^e3
			dB[1] = -(double(Scalar(vA * vB2))); // e3^e1
			dB[2] = -(double(Scalar(vA * vB3))); // e1^e2

			vX = -vA * vE0;
			dB[3] = (double(Scalar(vX * vE1))); // e1^e
			dB[4] = (double(Scalar(vX * vE2))); // e2^e
			dB[5] = (double(Scalar(vX * vE3))); // e3^e

			vX = -vA * vE;
			dB[6] = (double(Scalar(vX * vE1))); // e1^e0
			dB[7] = (double(Scalar(vX * vE2))); // e2^e0
			dB[8] = (double(Scalar(vX * vE3))); // e3^e0

			dB[9] = (double(Scalar(vA * vBpm))); // E

			for ( i = 0; i < 10; i++ )
			{
				if (dB[i] != 0)
				{
					if (eStrType == STRTYPE_NORMAL)
					{
						if (i == 0)
							sprintf(pcVal, csFormat + "^e23", dB[0]);
						else if (i == 1)
							sprintf(pcVal, csFormat + "^e31", dB[1]);
						else if (i == 2)
							sprintf(pcVal, csFormat + "^e12", dB[2]);
						else if (i >= 3 && i <= 5)
							sprintf(pcVal, csFormat + "^(e%d^e)", dB[i], i-2);
						else if (i >= 6 && i <= 8)
							sprintf(pcVal, csFormat + "^(e%d^e0)", dB[i], i-5);
						else if (i == 9)
							sprintf(pcVal, csFormat + "^E", dB[9]);
					}
					else if (eStrType == STRTYPE_LATEX)
					{
						if (i == 0)
							sprintf(pcVal, csFormat + "\\mathbf{e}_{23}", dB[0]);
						else if (i == 1)
							sprintf(pcVal, csFormat + "\\mathbf{e}_{31}", dB[1]);
						else if (i == 2)
							sprintf(pcVal, csFormat + "\\mathbf{e}_{12}", dB[2]);
						else if (i >= 3 && i <= 5)
							sprintf(pcVal, csFormat + "\\mathbf{e}_%d\\wedge\\mathbf{e}_\\infty", dB[i], i-2);
						else if (i >= 6 && i <= 8)
							sprintf(pcVal, csFormat + "\\mathbf{e}_%d\\wedge\\mathbf{e}_0", dB[i], i-5);
						else if (i == 9)
							sprintf(pcVal, csFormat + "\\mathbf{E}", dB[9]);
					}
					else if (eStrType == STRTYPE_HTML)
					{
						if (i == 0)
							sprintf(pcVal, csFormat + " <b>e23</b>", dB[0]);
						else if (i == 1)
							sprintf(pcVal, csFormat + " <b>e31</b>", dB[1]);
						else if (i == 2)
							sprintf(pcVal, csFormat + " <b>e12</b>", dB[2]);
						else if (i >= 3 && i <= 5)
							sprintf(pcVal, csFormat + " <b>e%d</b>^<b>e</b>", dB[i], i-2);
						else if (i >= 6 && i <= 8)
							sprintf(pcVal, csFormat + " <b>e%d</b>^<b>e0</b>", dB[i], i-5);
						else if (i == 9)
							sprintf(pcVal, csFormat + " <b>E</b>", dB[9]);
					}

					if (!bFirst)
						sText += " + ";
					else
						bFirst = false;

					sText += pcVal;
				}
			}
		} // if iGrade == 2
		else if (iGrade == 3)
		{
			vX = vA * vE0;
			dB[0] = (double(Scalar(vX * vB1))); // e23^e
			dB[1] = (double(Scalar(vX * vB2))); // e31^e
			dB[2] = (double(Scalar(vX * vB3))); // e12^e

			vX = vA * vE;
			dB[3] = (double(Scalar(vX * vB1))); // e23^e0
			dB[4] = (double(Scalar(vX * vB2))); // e31^e0
			dB[5] = (double(Scalar(vX * vB3))); // e12^e0

			vX = vA * vBpm;
			dB[6] = (double(Scalar(vX * vE1))); // e1^E
			dB[7] = (double(Scalar(vX * vE2))); // e2^E
			dB[8] = (double(Scalar(vX * vE3))); // e3^E

			dB[9] = -(double(Scalar(vA * vT123))); // e123

			for ( i = 0; i < 10; i++ )
			{
				if (dB[i] != 0)
				{
					if (eStrType == STRTYPE_NORMAL)
					{
						if (i == 0)
							sprintf(pcVal, csFormat + "^(e23^e)", dB[0]);
						else if (i == 1)
							sprintf(pcVal, csFormat + "^(e31^e)", dB[1]);
						else if (i == 2)
							sprintf(pcVal, csFormat + "^(e12^e)", dB[2]);
						else if (i == 3)
							sprintf(pcVal, csFormat + "^(e23^e0)", dB[3]);
						else if (i == 4)
							sprintf(pcVal, csFormat + "^(e31^e0)", dB[4]);
						else if (i == 5)
							sprintf(pcVal, csFormat + "^(e12^e0)", dB[5]);
						else if (i >= 6 && i <= 8)
							sprintf(pcVal, csFormat + "^(e%d^E)", dB[i], i-5);
						else if (i == 9)
							sprintf(pcVal, csFormat + "^e123", dB[9]);
					}
					else if (eStrType == STRTYPE_LATEX)
					{
						if (i == 0)
							sprintf(pcVal, csFormat + "\\mathbf{e}_{23}\\wedge\\mathbf{e}_\\infty", dB[0]);
						else if (i == 1)
							sprintf(pcVal, csFormat + "\\mathbf{e}_{31}\\wedge\\mathbf{e}_\\infty", dB[1]);
						else if (i == 2)
							sprintf(pcVal, csFormat + "\\mathbf{e}_{12}\\wedge\\mathbf{e}_\\infty", dB[2]);
						else if (i == 3)
							sprintf(pcVal, csFormat + "\\mathbf{e}_{23}\\wedge\\mathbf{e}_0", dB[3]);
						else if (i == 4)
							sprintf(pcVal, csFormat + "\\mathbf{e}_{31}\\wedge\\mathbf{e}_0", dB[4]);
						else if (i == 5)
							sprintf(pcVal, csFormat + "\\mathbf{e}_{12}\\wedge\\mathbf{e}_0", dB[5]);
						else if (i >= 6 && i <= 8)
							sprintf(pcVal, csFormat + "\\mathbf{e}_{%d}\\wedge\\mathbf{E}", dB[i], i-5);
						else if (i == 9)
							sprintf(pcVal, csFormat + "\\mathbf{e}_{123}", dB[9]);
					}
					else if (eStrType == STRTYPE_HTML)
					{
						if (i == 0)
							sprintf(pcVal, csFormat + " <b>e23</b>^<b>e</b>", dB[0]);
						else if (i == 1)
							sprintf(pcVal, csFormat + " <b>e31</b>^<b>e</b>", dB[1]);
						else if (i == 2)
							sprintf(pcVal, csFormat + " <b>e12</b>^<b>e</b>", dB[2]);
						else if (i == 3)
							sprintf(pcVal, csFormat + " <b>e23</b>^<b>e0</b>", dB[3]);
						else if (i == 4)
							sprintf(pcVal, csFormat + " <b>e31</b>^<b>e0</b>", dB[4]);
						else if (i == 5)
							sprintf(pcVal, csFormat + " <b>e12</b>^<b>e0</b>", dB[5]);
						else if (i >= 6 && i <= 8)
							sprintf(pcVal, csFormat + " <b>e%d</b>^<b>E</b>", dB[i], i-5);
						else if (i == 9)
							sprintf(pcVal, csFormat + " <b>e123</b>", dB[9]);
					}

					if (!bFirst)
						sText += " + ";
					else
						bFirst = false;

					sText += pcVal;
				}
			}
		} // if iGrade == 3
		else if (iGrade == 4)
		{
			dE[0] = (double(Scalar((vA * vE0) * vT123))); // e123^e
			dE[1] = (double(Scalar((vA * vE) * vT123))); // e123^e0

			dE[2] = -(double(Scalar(vA * vQpm1)));
			dE[3] = -(double(Scalar(vA * vQpm2)));
			dE[4] = -(double(Scalar(vA * vQpm3)));


			for ( i = 0; i < 5; i++ )
			{
				if (dE[i] != 0)
				{
					if (eStrType == STRTYPE_NORMAL)
					{
						if (i == 0)
							sprintf(pcVal, csFormat + "^(e123^e)", dE[0]);
						else if (i == 1)
							sprintf(pcVal, csFormat + "^(e123^e0)", dE[1]);
						else if (i == 2)
							sprintf(pcVal, csFormat + "^(e23^E)", dE[2]);
						else if (i == 3)
							sprintf(pcVal, csFormat + "^(e31^E)", dE[3]);
						else if (i == 4)
							sprintf(pcVal, csFormat + "^(e12^E)", dE[4]);
					}
					else if (eStrType == STRTYPE_LATEX)
					{
						if (i == 0)
							sprintf(pcVal, csFormat + "\\mathbf{e}_{123}\\wedge\\mathbf{e}_\\infty", dE[0]);
						else if (i == 1)
							sprintf(pcVal, csFormat + "\\mathbf{e}_{123}\\wedge\\mathbf{e}_0", dE[1]);
						else if (i == 2)
							sprintf(pcVal, csFormat + "\\mathbf{e}_{23}\\wedge\\mathbf{E}", dE[2]);
						else if (i == 3)
							sprintf(pcVal, csFormat + "\\mathbf{e}_{31}\\wedge\\mathbf{E}", dE[3]);
						else if (i == 4)
							sprintf(pcVal, csFormat + "\\mathbf{e}_{12}\\wedge\\mathbf{E}", dE[4]);
					}
					else if (eStrType == STRTYPE_HTML)
					{
						if (i == 0)
							sprintf(pcVal, csFormat + " <b>e123</b>^<b>e</b>", dE[0]);
						else if (i == 1)
							sprintf(pcVal, csFormat + " <b>e123</b>^<b>e0</b>", dE[1]);
						else if (i == 2)
							sprintf(pcVal, csFormat + " <b>e23</b>^<b>E</b>", dE[2]);
						else if (i == 3)
							sprintf(pcVal, csFormat + " <b>e31</b>^<b>E</b>", dE[3]);
						else if (i == 4)
							sprintf(pcVal, csFormat + " <b>e12</b>^<b>E</b>", dE[4]);
					}

					if (!bFirst)
						sText += " + ";
					else
						bFirst = false;

					sText += pcVal;
				}
			}
		} // if iGrade == 4
		else if (iGrade == 5)
		{
			if (!bFirst)
				sText += " + ";
			else
				bFirst = false;

			dI = (vA[iI]);
			if (eStrType == STRTYPE_NORMAL)
				sprintf(pcVal, csFormat + "^I", dI);
			else if (eStrType == STRTYPE_NORMAL)
				sprintf(pcVal, csFormat + "\\mathbf{I}", dI);
			else if (eStrType == STRTYPE_HTML)
				sprintf(pcVal, csFormat + " <b>I</b>", dI);

			sText += pcVal;
		} // if iGrade == 5

	} // for grades

	if (iGradeCount == 0)
	{
		sText = "0";
	}

	csVal = sText;
	return true;
}


//////////////////////////////////////////////////////////////////////
/// The VecP3 String FUNCTION
/// Returns string of P3 vectors

bool CCLUCodeBase::VecP3String(TMultiV &_vA, cStr &csVal, EStringType eStrType)
{
	TMultiV vA = _vA;

	// Check that vector is from conformal space
	if (vA.GetBase().BaseID() != ID_PGA)
	{
		return false;
	}

	// Check that it is a vector
	Mem<uint> mGList;

	vA.TinyToZero();
	if (!GradeList(vA, mGList, m_fSensitivity))
	{
		csVal = "0";
		return true;
	}

	DeclarePGAVarsLocal(TCVScalar);
	double dSc, dE[4], dB[6], dI;
	int i, j, iGrade, iGradeCount;
	bool bFirst = true;
	cStr sText, csFormat;
	char pcVal[100];
	TMultiV vX;

	// Create format string
	sprintf(pcVal, "%%.%dlg", m_iTextPrec);
	csFormat = pcVal;

	iGradeCount = mGList.Count();

	for ( j = iGradeCount - 1; j >= 0; j-- )
	{
		iGrade = mGList[j];

		if (iGrade == 0)
		{
			if (!bFirst)
				sText += " + ";
			else
				bFirst = false;

			dSc = (vA[iSC]);
			sprintf(pcVal, csFormat, dSc);
			sText += pcVal;
		} // if iGrade == 0
		else if (iGrade == 1)
		{
			dE[0] = (double(vA[iE1]));
			dE[1] = (double(vA[iE2]));
			dE[2] = (double(vA[iE3]));
			dE[3] = (double(vA[iE4]));

			for ( i = 0; i < 4; i++ )
			{
				if (dE[i] != 0)
				{
					if (eStrType == STRTYPE_NORMAL)
					{
						sprintf(pcVal, csFormat + "^e%d", dE[i], i+1);
					}
					else if (eStrType == STRTYPE_LATEX)
					{
						sprintf(pcVal, csFormat + "\\mathbf{e}_%d", dE[i], i+1);
					}
					else if (eStrType == STRTYPE_HTML)
					{
						sprintf(pcVal, csFormat + " <b>e%d</b>", dE[i], i+1);
					}

					if (!bFirst)
						sText += " + ";
					else
						bFirst = false;

					sText += pcVal;
				}
			}
		} // if iGrade == 1
		else if (iGrade == 2) // Bivector Components
		{
			dB[0] = (double(vA[iL1])); // e2^e3
			dB[1] = (double(vA[iL2])); // e3^e1
			dB[2] = (double(vA[iL3])); // e1^e2

			dB[3] = (double(vA[iL1I])); // e4^e1
			dB[4] = (double(vA[iL2I])); // e4^e2
			dB[5] = (double(vA[iL3I])); // e4^e3

			for ( i = 0; i < 6; i++ )
			{
				if (dB[i] != 0)
				{
					if (eStrType == STRTYPE_NORMAL)
					{
						if (i == 0)
							sprintf(pcVal, csFormat + "^e23", dB[0]);
						else if (i == 1)
							sprintf(pcVal, csFormat + "^e31", dB[1]);
						else if (i == 2)
							sprintf(pcVal, csFormat + "^e12", dB[2]);
						else if (i >= 3 && i <= 5)
							sprintf(pcVal, csFormat + "^(e4^e%d)", dB[i], i-2);
					}
					else if (eStrType == STRTYPE_LATEX)
					{
						if (i == 0)
							sprintf(pcVal, csFormat + "\\mathbf{e}_{23}", dB[0]);
						else if (i == 1)
							sprintf(pcVal, csFormat + "\\mathbf{e}_{31}", dB[1]);
						else if (i == 2)
							sprintf(pcVal, csFormat + "\\mathbf{e}_{12}", dB[2]);
						else if (i >= 3 && i <= 5)
							sprintf(pcVal, csFormat + "\\mathbf{e}_4\\wedge\\mathbf{e}_%d", dB[i], i-2);
					}
					else if (eStrType == STRTYPE_HTML)
					{
						if (i == 0)
							sprintf(pcVal, csFormat + " <b>e23</b>", dB[0]);
						else if (i == 1)
							sprintf(pcVal, csFormat + " <b>e31</b>", dB[1]);
						else if (i == 2)
							sprintf(pcVal, csFormat + " <b>e12</b>", dB[2]);
						else if (i >= 3 && i <= 5)
							sprintf(pcVal, csFormat + " <b>e4%d</b>", dB[i], i-2);
					}

					if (!bFirst)
						sText += " + ";
					else
						bFirst = false;

					sText += pcVal;
				}
			}
		} // if iGrade == 2
		else if (iGrade == 3)
		{
			dE[0] = (double(vA[iE1I]));
			dE[1] = (double(vA[iE2I]));
			dE[2] = (double(vA[iE3I]));
			dE[3] = (double(vA[iE4I]));

			for ( i = 0; i < 4; i++ )
			{
				if (dE[i] != 0)
				{
					if (eStrType == STRTYPE_NORMAL)
					{
						if (i == 0)
							sprintf(pcVal, csFormat + "^e234", dE[i]);
						else if (i == 1)
							sprintf(pcVal, csFormat + "^e314", dE[i]);
						else if (i == 2)
							sprintf(pcVal, csFormat + "^e124", dE[i]);
						else if (i == 3)
							sprintf(pcVal, csFormat + "^e321", dE[i]);
					}
					else if (eStrType == STRTYPE_LATEX)
					{
						if (i == 0)
							sprintf(pcVal, csFormat + "\\mathbf{e}_{234}", dE[i]);
						else if (i == 1)
							sprintf(pcVal, csFormat + "\\mathbf{e}_{314}", dE[i]);
						else if (i == 2)
							sprintf(pcVal, csFormat + "\\mathbf{e}_{124}", dE[i]);
						else if (i == 3)
							sprintf(pcVal, csFormat + "\\mathbf{e}_{321}", dE[i]);
					}
					else if (eStrType == STRTYPE_HTML)
					{
						if (i == 0)
							sprintf(pcVal, csFormat + " <b>e234</b>", dE[i]);
						else if (i == 1)
							sprintf(pcVal, csFormat + " <b>e314</b>", dE[i]);
						else if (i == 2)
							sprintf(pcVal, csFormat + " <b>e124</b>", dE[i]);
						else if (i == 3)
							sprintf(pcVal, csFormat + " <b>e321</b>", dE[i]);
					}

					if (!bFirst)
						sText += " + ";
					else
						bFirst = false;

					sText += pcVal;
				}
			}
		} // if iGrade == 3
		else if (iGrade == 4)
		{
			if (!bFirst)
				sText += " + ";
			else
				bFirst = false;

			dI = (vA[iI]);
			if (eStrType == STRTYPE_NORMAL)
				sprintf(pcVal, csFormat + "^I", dI);
			else if (eStrType == STRTYPE_LATEX)
				sprintf(pcVal, csFormat + "\\mathbf{I}", dI);
			else if (eStrType == STRTYPE_HTML)
				sprintf(pcVal, csFormat + " <b>I</b>", dI);

			sText += pcVal;
		} // if iGrade == 4

	} // for grades

	if (iGradeCount == 0)
	{
		sText = "0";
	}

	csVal = sText;
	return true;
}


//////////////////////////////////////////////////////////////////////
/// The VecE3 String FUNCTION
/// Returns string of E3 vectors

bool CCLUCodeBase::VecE3String(TMultiV &_vA, cStr &csVal, EStringType eStrType)
{
	TMultiV vA = _vA;

	// Check that vector is from conformal space
	if (vA.GetBase().BaseID() != ID_E3GA)
	{
		return false;
	}

	// Check that it is a vector
	Mem<uint> mGList;

	vA.TinyToZero();
	if (!GradeList(vA, mGList, m_fSensitivity))
	{
		csVal = "0";
		return true;
	}

	DeclareE3GAVarsLocal(TCVScalar);
	double dSc, dE[3], dB[3], dI;
	int i, j, iGrade, iGradeCount;
	bool bFirst = true;
	cStr sText, csFormat;
	char pcVal[100];
	TMultiV vX;

	// Create format string
	sprintf(pcVal, "%%.%dlg", m_iTextPrec);
	csFormat = pcVal;

	iGradeCount = mGList.Count();

	for ( j = iGradeCount - 1; j >= 0; j-- )
	{
		iGrade = mGList[j];

		if (iGrade == 0)
		{
			if (!bFirst)
				sText += " + ";
			else
				bFirst = false;

			dSc = (vA[iSC]);
			sprintf(pcVal, csFormat, dSc);
			sText += pcVal;
		} // if iGrade == 0
		else if (iGrade == 1)
		{
			dE[0] = (double(vA[iE1]));
			dE[1] = (double(vA[iE2]));
			dE[2] = (double(vA[iE3]));

			for ( i = 0; i < 3; i++ )
			{
				if (dE[i] != 0)
				{
					if (eStrType == STRTYPE_NORMAL)
					{
						sprintf(pcVal, csFormat + "^e%d", dE[i], i+1);
					}
					else if (eStrType == STRTYPE_LATEX)
					{
						sprintf(pcVal, csFormat + "\\mathbf{e}_%d", dE[i], i+1);
					}
					else if (eStrType == STRTYPE_HTML)
					{
						sprintf(pcVal, csFormat + " <b>e%d</b>", dE[i], i+1);
					}

					if (!bFirst)
						sText += " + ";
					else
						bFirst = false;

					sText += pcVal;
				}
			}
		} // if iGrade == 1
		else if (iGrade == 2) // Bivector Components
		{
			dB[0] = (double(vA[iL1])); // e2^e3
			dB[1] = (double(vA[iL2])); // e3^e1
			dB[2] = (double(vA[iL3])); // e1^e2

			for ( i = 0; i < 3; i++ )
			{
				if (dB[i] != 0)
				{
					if (eStrType == STRTYPE_NORMAL)
					{
						if (i == 0)
							sprintf(pcVal, csFormat + "^e23", dB[0]);
						else if (i == 1)
							sprintf(pcVal, csFormat + "^e31", dB[1]);
						else if (i == 2)
							sprintf(pcVal, csFormat + "^e12", dB[2]);
					}
					else if (eStrType == STRTYPE_LATEX)
					{
						if (i == 0)
							sprintf(pcVal, csFormat + "\\mathbf{e}_{23}", dB[0]);
						else if (i == 1)
							sprintf(pcVal, csFormat + "\\mathbf{e}_{31}", dB[1]);
						else if (i == 2)
							sprintf(pcVal, csFormat + "\\mathbf{e}_{12}", dB[2]);
					}
					else if (eStrType == STRTYPE_HTML)
					{
						if (i == 0)
							sprintf(pcVal, csFormat + " <b>e23</b>", dB[0]);
						else if (i == 1)
							sprintf(pcVal, csFormat + " <b>e31</b>", dB[1]);
						else if (i == 2)
							sprintf(pcVal, csFormat + " <b>e12</b>", dB[2]);
					}

					if (!bFirst)
						sText += " + ";
					else
						bFirst = false;

					sText += pcVal;
				}
			}
		} // if iGrade == 2
		else if (iGrade == 3)
		{
			if (!bFirst)
				sText += " + ";
			else
				bFirst = false;

			dI = (vA[iI]);
			if (eStrType == STRTYPE_NORMAL)
				sprintf(pcVal, csFormat + "^I", dI);
			else if (eStrType == STRTYPE_LATEX)
				sprintf(pcVal, csFormat + "\\mathbf{I}", dI);
			else if (eStrType == STRTYPE_HTML)
				sprintf(pcVal, csFormat + " <b>I</b>", dI);

			sText += pcVal;
		} // if iGrade == 3

	} // for grades

	if (iGradeCount == 0)
	{
		sText = "0";
	}

	csVal = sText;
	return true;
}



//////////////////////////////////////////////////////////////////////
/// The VecEl2 String FUNCTION
/// Returns string of El2 vectors

bool CCLUCodeBase::VecEl2String(TMultiV &vA, cStr &csVal, EStringType eStrType)
{
	// Check that vector is from El2 space
	if (vA.GetBase().BaseID() != ID_EL2GA)
	{
		return false;
	}

	int iIdx, iIdxPos, iIdxCount;	
	double dVal;
	char pcVal[100];
	bool bFirst = true;
	cStr sText, sIdx, csFormat;
	vA.TinyToZero();

	// Create format string
	sprintf(pcVal, "%%.%dlg", m_iTextPrec);
	csFormat = pcVal;

	for (iIdx = 0; iIdx < 64; iIdx++)
	{
		if ((dVal = vA[iIdx]) != 0.0)
		{
			if (!bFirst)
				sText += " + ";
			else
				bFirst = false;

			const Mem<uint>& rBladeIdx = m_El2GABase.GetBladeIdx(iIdx);
			iIdxCount = rBladeIdx.Count();
			sIdx = "";
			for (iIdxPos = 0; iIdxPos < iIdxCount; iIdxPos++)
			{
				sprintf(pcVal, "%d", rBladeIdx[iIdxPos]);
				sIdx += pcVal;
			}

			if (iIdxCount > 0)
			{
				if (eStrType == STRTYPE_NORMAL)
				{
					sprintf(pcVal, csFormat + "^e%s", dVal, sIdx.Str());
				}
				else if (eStrType == STRTYPE_LATEX)
				{
					sprintf(pcVal, csFormat + "\\,\\mathbf{e}_{%s}", dVal, sIdx.Str());
				}
				else if (eStrType == STRTYPE_HTML)
				{
					sprintf(pcVal, csFormat + " <b>e%s</b>", dVal, sIdx.Str());
				}
			}
			else
			{
				if (eStrType == STRTYPE_NORMAL)
				{
					sprintf(pcVal, csFormat, dVal);
				}
				else if (eStrType == STRTYPE_LATEX)
				{
					sprintf(pcVal, csFormat, dVal);
				}
				else if (eStrType == STRTYPE_HTML)
				{
					sprintf(pcVal, csFormat, dVal);
				}
			}

			sText += pcVal;			
		}
	}

	if (bFirst)
	{
		sText = "0";
	}

	csVal = sText;
	return true;
}


//////////////////////////////////////////////////////////////////////
// Transform matrix into string

bool CCLUCodeBase::MatrixString(TMatrix &Mat, cStr &csVal, EStringType eStrType)
{
	unsigned uRows = Mat.Rows();
	unsigned uCols = Mat.Cols();

	cStr csText, csFormat;
	char pcVal[100];
	unsigned uIdx, uC, uR;

	// Create format string
	sprintf(pcVal, "%%.%dlg", m_iTextPrec);
	csFormat = pcVal;

	if (eStrType == STRTYPE_NORMAL)
	{
		uRows = Mat.Rows();
		csVal = "(";
		for(uR = 0; uR < uRows; uR++)
		{
			csVal << Mat.Str(uR+1);
			if (uR < uRows-1)
				csVal << ", ";
		}
		csVal << ")";
	}
	else if (eStrType == STRTYPE_LATEX)
	{
		pcVal[0] = 0;
		for (uIdx = 0; uIdx < uCols; uIdx++)
		{
			strcat(pcVal, "c");
		}

		csText = "\\left(\\begin{array}{";
		csText += pcVal;
		csText += "}\n";

		for(uR = 1; uR <= uRows; uR++)
		{
			for(uC = 1; uC <= uCols; uC++)
			{
				sprintf(pcVal, csFormat, (double(Mat(uR, uC))));
				csText += pcVal;
				if (uC < uCols)
					csText += " & ";
			}
			if (uR < uRows)
				csText += "\\\\\n";
			else
				csText += "\n";
		}
		csText += "\\end{array}\\right)\n";

		csVal = csText;
	}
	else if (eStrType == STRTYPE_HTML)
	{
		pcVal[0] = 0;

		csText = "<table border=\"0\"><tr><td>\n";

		for(uR = 1; uR <= uRows; uR++)
		{
			for(uC = 1; uC <= uCols; uC++)
			{
				sprintf(pcVal, csFormat, double(Mat(uR, uC)));
				csText += pcVal;
				if (uC < uCols)
					csText += "</td><td>";
			}
			if (uR < uRows)
				csText += "</td></tr><tr><td>\n";
			else
				csText += "\n";
		}
		csText += "</td></tr></table>\n";

		csVal = csText;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
// Transform tensor into string

bool CCLUCodeBase::TensorString(TTensor &T, cStr &csVal, EStringType eStrType)
{
	int iDim, iValence;
	TTensorIdx TIdx;
	Mem<int> mIdx;

	iValence = T.Valence();
	mIdx.Set(iValence);

	for (iDim = 0; iDim < iValence; iDim++)
	{
		mIdx[iDim] = -iDim - 1;
	}

	TIdx.Set(T, mIdx);

	return TensorIdxString(TIdx, csVal, eStrType);
}



//////////////////////////////////////////////////////////////////////
// Transform tensor index into string

bool CCLUCodeBase::TensorIdxString(TTensorIdx &TIdx, cStr &csVal, EStringType eStrType)
{
	cStr csText, csFormat;
	char pcVal[100];
	int iLevel;
	CTensorSingleLoop<TCVScalar> Loop;

	// Create format string
	sprintf(pcVal, "%%.%dlg", m_iTextPrec);
	csFormat = pcVal;

	Loop.Set(TIdx);
	Loop.Init();
	iLevel = 0;

	if (eStrType == STRTYPE_NORMAL)
	{
		if (TIdx.GetTensorData().Size() > 4100)
		{
			csVal = "";
			csVal << "Tensor contains " << TIdx.GetTensorData().Size() << " elements.";
		}
		else
		{
			csVal = "| ";

			do
			{
				if (iLevel == 1)
					csVal << "|\n| "; // New line
				else if (iLevel > 1)
					csVal << "|\n\n| "; // New Block

				// Element
				sprintf(pcVal, csFormat, Loop.GetRef());
				csVal << pcVal << " ";
			}
			while ( (iLevel = Loop.Step()) >= 0 );
			csVal << "|";
		}
	}
	else if (eStrType == STRTYPE_LATEX)
	{
/*
		pcVal[0] = 0;
		for (uIdx = 0; uIdx < uCols; uIdx++)
		{
			strcat(pcVal, "c");
		}

		csText = "\\left(\\begin{array}{";
		csText += pcVal;
		csText += "}\n";

		for(uR = 1; uR <= uRows; uR++)
		{
			for(uC = 1; uC <= uCols; uC++)
			{
				sprintf(pcVal, csFormat, (double(Mat(uR, uC))));
				csText += pcVal;
				if (uC < uCols)
					csText += " & ";
			}
			if (uR < uRows)
				csText += "\\\\\n";
			else
				csText += "\n";
		}
		csText += "\\end{array}\\right)\n";

		csVal = csText;
*/
		csVal = "Not implemented.";
	}
	else if (eStrType == STRTYPE_HTML)
	{
		if (TIdx.GetTensorData().Size() > 4100)
		{
			csVal = "";
			csVal << "<b>Tensor contains " << TIdx.GetTensorData().Size() << " elements.</b>";
		}
		else
		{
			pcVal[0] = 0;

			csVal = "<table border=\"0\"><tr>\n";
			do
			{
				if (iLevel == 1)
					csVal << "</tr><tr>\n"; // New line
				else if (iLevel > 1)
				{
					// New Block
					csVal << "</tr></table>\n\n"; 
					csVal << "<table border=\"0\"><tr>\n";
				}

				// Element
				sprintf(pcVal, csFormat, Loop.GetRef());
				csVal << "<td>" << pcVal << "</td>";
			}
			while ( (iLevel = Loop.Step()) >= 0 );
			csVal << "</tr></table>\n";
		}
	}
	
	return true;
}



//////////////////////////////////////////////////////////////////////
/// Cast To String

bool CCLUCodeBase::CastToString(CCodeVar& rVar, cStr &csVal)
{
	TCVScalar dVal;
	
	if (rVar.BaseType() == PDT_MULTIV)
	{
		// Check that vector is from conformal space
		TMultiV &vA = *rVar.GetMultiVPtr();
		if (vA.GetBase().BaseID() == ID_CONFGA)
		{
			VecN3String(vA, csVal);
		}
		else if (vA.GetBase().BaseID() == ID_PGA)
		{
			VecP3String(vA, csVal);
		}
		else if (vA.GetBase().BaseID() == ID_E3GA)
		{
			VecE3String(vA, csVal);
		}
		else if (vA.GetBase().BaseID() == ID_EL2GA)
		{
			VecEl2String(vA, csVal);
		}
	}
	else if (rVar.CastToScalar(dVal, m_fSensitivity))
	{
		char pcVal[100], pcVal2[10];
		TCVScalar dValRound;

		// Create format string
		sprintf(pcVal2, "%%.%dlg", m_iTextPrec);

		//dValRound = (dVal);
		sprintf(pcVal, pcVal2, dVal);

		csVal = pcVal;
	}
	else if (rVar.BaseType() == PDT_VARLIST)
	{
		TVarList &rList = *rVar.GetVarListPtr();
		int iPos, iCnt = rList.Count();
		cStr csSubVal;

		csVal = "[ ";
		for (iPos = 0; iPos < iCnt; iPos++)
		{
			CastToString(rList[iPos], csSubVal);
			csVal << csSubVal;
			
			if (iPos < iCnt-1)
				csVal << " , ";
		}
		csVal << " ]";
	}
	else
	{
		csVal = rVar.ValStr();
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Cast To Latex String

bool CCLUCodeBase::CastToLatexString(CCodeVar& _rVar, cStr &csVal)
{
	TCVScalar dVal;

	// Get the actual value a variable points to
	CCodeVar& rVar = _rVar.DereferenceVarPtr(true);

	if (rVar.BaseType() == PDT_MULTIV)
	{
		// Check that vector is from conformal space
		TMultiV &vA = *rVar.GetMultiVPtr();
		if (vA.GetBase().BaseID() == ID_CONFGA)
		{
			VecN3String(vA, csVal, STRTYPE_LATEX);
			return true;
		}
		else if (vA.GetBase().BaseID() == ID_PGA)
		{
			VecP3String(vA, csVal, STRTYPE_LATEX);
			return true;
		}
		else if (vA.GetBase().BaseID() == ID_E3GA)
		{
			VecE3String(vA, csVal, STRTYPE_LATEX);
			return true;
		}
		else if (vA.GetBase().BaseID() == ID_EL2GA)
		{
			VecEl2String(vA, csVal, STRTYPE_LATEX);
			return true;
		}
	}
	else if (rVar.BaseType() == PDT_MATRIX)
	{
		TMatrix &Mat = *rVar.GetMatrixPtr();

		return MatrixString(Mat, csVal, STRTYPE_LATEX);
	}
	else if (rVar.BaseType() == PDT_TENSOR)
	{
		TTensor &T = *rVar.GetTensorPtr();

		return TensorString(T, csVal, STRTYPE_LATEX);
	}
	else if (rVar.BaseType() == PDT_TENSOR_IDX)
	{
		TTensorIdx &T = *rVar.GetTensorIdxPtr();

		return TensorIdxString(T, csVal, STRTYPE_LATEX);
	}
	else if (rVar.CastToScalar(dVal, m_fSensitivity))
	{
		char pcVal[100], pcVal2[10];
		TCVScalar dValRound;

		// Create format string
		sprintf(pcVal2, "%%.%dlg", m_iTextPrec);

		dValRound = dVal;
		sprintf(pcVal, pcVal2, dValRound);

		csVal = pcVal;
	}
	else if (rVar.BaseType() == PDT_VARLIST)
	{
		TVarList &rList = *rVar.GetVarListPtr();
		int iPos, iCnt = rList.Count();
		cStr csSubVal;

		csVal = "\\bigg[\\; ";
		for (iPos = 0; iPos < iCnt; iPos++)
		{
			CastToHTMLString(rList[iPos], csSubVal);
			csVal << csSubVal;
			
			if (iPos < iCnt-1)
				csVal << " \\;{\\large ,}\\; ";
		}
		csVal << " \\;\\bigg]";
	}
	else if (rVar.BaseType() == PDT_COLOR)
	{
		TOGLColor &rCol = *rVar.GetOGLColorPtr();
		char pcVal[200], pcVal2[100];

		// Create format string
		sprintf(pcVal2, "(%%.%dlg,\\, %%.%dlg,\\, %%.%dlg,\\, %%.%dlg)", 
						m_iTextPrec, m_iTextPrec, m_iTextPrec, m_iTextPrec);

		sprintf(pcVal, pcVal2, rCol[0], rCol[1], rCol[2], rCol[3]);

		csVal = pcVal;
	}
	else
	{
		csVal = rVar.ValStr();
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Cast To Info Text String (HTML)

bool CCLUCodeBase::CastToHTMLString(CCodeVar& _rVar, cStr &csVal)
{
	TCVScalar dVal;

	// Get the actual value a variable points to
	CCodeVar& rVar = _rVar.DereferenceVarPtr(true);

	if (rVar.BaseType() == PDT_MULTIV)
	{
		// Check that vector is from conformal space
		TMultiV &vA = *rVar.GetMultiVPtr();
		if (vA.GetBase().BaseID() == ID_CONFGA)
		{
			VecN3String(vA, csVal, STRTYPE_HTML);
			return true;
		}
		else if (vA.GetBase().BaseID() == ID_PGA)
		{
			VecP3String(vA, csVal, STRTYPE_HTML);
			return true;
		}
		else if (vA.GetBase().BaseID() == ID_E3GA)
		{
			VecE3String(vA, csVal, STRTYPE_HTML);
			return true;
		}
		else if (vA.GetBase().BaseID() == ID_EL2GA)
		{
			VecEl2String(vA, csVal, STRTYPE_HTML);
			return true;
		}
	}
	else if (rVar.BaseType() == PDT_MATRIX)
	{
		TMatrix &Mat = *rVar.GetMatrixPtr();

		return MatrixString(Mat, csVal, STRTYPE_HTML);
	}
	else if (rVar.BaseType() == PDT_TENSOR)
	{
		TTensor &T = *rVar.GetTensorPtr();

		return TensorString(T, csVal, STRTYPE_HTML);
	}
	else if (rVar.BaseType() == PDT_TENSOR_IDX)
	{
		TTensorIdx &T = *rVar.GetTensorIdxPtr();

		return TensorIdxString(T, csVal, STRTYPE_HTML);
	}
	else if (rVar.CastToScalar(dVal, m_fSensitivity))
	{
		char pcVal[100], pcVal2[10];
		TCVScalar dValRound;

		// Create format string
		sprintf(pcVal2, "%%.%dlg", m_iTextPrec);

		//dValRound = (dVal);
		sprintf(pcVal, pcVal2, dVal);

		csVal = pcVal;
	}
	else if (rVar.BaseType() == PDT_VARLIST)
	{
		TVarList &rList = *rVar.GetVarListPtr();
		int iPos, iCnt = rList.Count();
		cStr csSubVal;

		csVal = "<b>[</b> ";
		for (iPos = 0; iPos < iCnt; iPos++)
		{
			CastToHTMLString(rList[iPos], csSubVal);
			csVal << csSubVal;
			
			if (iPos < iCnt-1)
				csVal << " <b>,</b> ";
		}
		csVal << " <b>]</b>";
	}
	else if (rVar.BaseType() == PDT_COLOR)
	{
		TOGLColor &rCol = *rVar.GetOGLColorPtr();
		char pcVal[200], pcVal2[100];

		// Create format string
		sprintf(pcVal2, "<b>(</b>%%.%dlg<b>,</b> %%.%dlg<b>,</b> %%.%dlg<b>,</b> %%.%dlg<b>)</b>", 
						m_iTextPrec, m_iTextPrec, m_iTextPrec, m_iTextPrec);

		sprintf(pcVal, pcVal2, rCol[0], rCol[1], rCol[2], rCol[3]);

		csVal = pcVal;
	}
	else if (rVar.BaseType() == PDT_STRING)
	{
		char *pcVal, cEl;
		int iPos, iStrPos, iMaxPos = 1000, iStrLen;
		char pcBuf[ 1010 ];

		csVal = "";
		pcVal = rVar.GetStringPtr()->Str();
		iStrLen = strlen(pcVal);

		iPos = 0;
		iStrPos = 0;
		pcBuf[0] = 0;
		while (iStrPos < iStrLen)
		{
			cEl = pcVal[ iStrPos++ ];
			if (cEl == '<')
			{
				strcat(pcBuf, "&lt;");
				iPos += 4;
			}
			else if (cEl == '>')
			{
				strcat(pcBuf, "&gt;");
				iPos += 4;
			}
			else if (cEl == '\n')
			{
				strcat(pcBuf, "<br>");
				iPos += 4;
			}
			else
			{
				pcBuf[ iPos++ ] = cEl;
				pcBuf[ iPos ] = 0;
			}

			if (iPos >= iMaxPos)
			{
				csVal << pcBuf;
				iPos = 0;
				pcBuf[ 0 ] = 0;
			}
		}

		if (iPos > 0)
		{
			csVal << pcBuf;
		}
		
	}
	else
	{
		csVal = rVar.ValStr();
	}

	return true;
}



//////////////////////////////////////////////////////////////////////
/// The String FUNCTION
/// Converts variables to strings

bool CCLUCodeBase::StringFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	
	if (iVarCount == 1)
	{
		rVar.New(PDT_STRING, "Constant");
		TString &csVal = *rVar.GetStringPtr();

		CastToString(mVars(0), csVal);
	}
	else if (iVarCount == 2)
	{
		if (mVars(0).BaseType() != PDT_VARLIST ||
			mVars(1).BaseType() != PDT_STRING)
		{
			m_ErrorList.GeneralError(
				"If two parameters are given, expect the first "
				"one to be a list and the second a delimiter string.", iLine, iPos);
			return false;
		}

		TVarList &rList = *mVars(0).GetVarListPtr();
		TString &rStr = *mVars(1).GetStringPtr();

		cStr csEl;
		rVar.New(PDT_STRING, "Constant");
		TString &csVal = *rVar.GetStringPtr();

		int i, iCount = rList.Count();
		for( i = 0; i < iCount; i++ )
		{
			CastToString( rList(i), csEl );
			csVal << csEl;

			if (i+1 < iCount)
				csVal << rStr;
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
/// The Scalar FUNCTION
/// Converts strings and other variables to scalars

bool CCLUCodeBase::ScalarFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	
	TCVScalar dVal;
	int iVarCount = mVars.Count();
	
	if (iVarCount == 1)
	{
		return ScalarEval(rVar, mVars(0), iLine, iPos);
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////
/// Casting of elements to a scalar

bool CCLUCodeBase::ScalarEval(CCodeVar& rVar, CCodeVar& rPar, int iLine, int iPos)
{
	TCVScalar dVal;
	
	if (rPar.BaseType() == PDT_STRING)
	{
		TString &rStr = *rPar.GetStringPtr();

		if (sscanf(rStr.Str(), "%lg", &dVal) == 0)
		{
			m_ErrorList.GeneralError("String could not be converted to scalar.", iLine, iPos);
			return false;
		}

		rVar = dVal;
	}
	else if (rPar.CastToScalar(dVal, m_fSensitivity))
	{
		rVar = dVal;
	}
	else if (rPar.BaseType() == PDT_VARLIST)
	{
		TVarList &rList = *rPar.GetVarListPtr();
		int i, iCount = rList.Count();

		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rVarList = *rVar.GetVarListPtr();
		rVarList.Set( iCount );

		for (i = 0; i < iCount; i++)
		{
			if (!ScalarEval(rVarList(i), rList(i), iLine, iPos))
				return false;
		}
	}
	else
	{
		m_ErrorList.InvalidParType(rPar, 1, iLine, iPos);
		return false;
	}
	
	return true;
}



//////////////////////////////////////////////////////////////////////
/// Set Text Precission Function

bool CCLUCodeBase::SetTextPrecFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = mVars.Count();
	TCVCounter iVal;
	
	if (iVarCount == 1)
	{
		if (!mVars(0).CastToCounter(iVal))
		{
			m_ErrorList.InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}								

		if (iVal < 0 || iVal > 17)
		{
			m_ErrorList.InvalidParVal(mVars(0), 1, iLine, iPos);
			return false;
		}

		m_iTextPrec = iVal;
	}
	else
	{
		m_ErrorList.WrongNoOfParams(1, iLine, iPos);
		return false;
	}
	
	return true;
} 





