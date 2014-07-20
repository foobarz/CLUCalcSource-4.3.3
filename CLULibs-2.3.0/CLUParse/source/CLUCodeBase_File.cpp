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
#include <FL/fl_ask.H>
#include <FL/Fl_File_Chooser.H>
#include <time.h>
#include "XML.h"

#ifdef _GNUCPP3_
#   include <unistd.h>
#   define _getcwd getcwd
#   define _chdir chdir
#else
#   include <direct.h>
#endif

#ifdef WIN32
#	include <ShellApi.h>
#endif

#include "CLUCodeBase.h"
#include "TensorOperators.h"

#include <vector>
#include <algorithm>
#include <functional>      // For greater<int>( )

using namespace std;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////
/// Write File Function
///
/// 1. Filename
/// 2. Variable (any type)
/// 3. XML Compression (0-9) (optional, default 0)
///
/// Returns string "OK" if data was written, otherwise an error message.

bool CCLUCodeBase::WriteVariableFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	bool bError = false;
	cStr csFilename;
	TVarList& mVars = *rPars.GetVarListPtr();
	TCVCounter iCompression = 0;

	int iVarCount = mVars.Count();
	
	if (iVarCount < 2 ||iVarCount > 3)
	{
		int piPar[] = {2};

		m_ErrorList.WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		m_ErrorList.GeneralError("First parameter has to be filename.", iLine, iPos);
		return false;
	}

	if (iVarCount >= 3)
	{
		if (!mVars(2).CastToCounter(iCompression))
		{
			m_ErrorList.InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}

		if (iCompression < 0 || iCompression > 9)
		{
			m_ErrorList.GeneralError(
				"Compression value has to be in range 0 (no compression), "
				"to 9 (max. compression).", iLine, iPos);
			return false;
		}
	}

	csFilename = mVars(0).ValStr();

	char pcCurPath[500];
	CXMLdom DOM;
	CXMLTree xmlTree;
	CXMLElement xmlEl;

	try
	{
		xmlEl.SetName("clu_data");
		xmlEl.AddProp("version", "1.0");

		WriteVariable(xmlEl.GetSubTree(), mVars(1), 0); 

		xmlTree.AddElement(xmlEl);
		DOM.SetElementTree(xmlTree);

		_getcwd(pcCurPath, 499);
		_chdir(m_sScriptPath.c_str());

		DOM.WriteFile(csFilename.Str(), iCompression);

		_chdir(pcCurPath);

		// Return with OK message to script
		rVar = "OK";
	}
	catch(CCLUException &rEx)
	{
		rVar = rEx.PrintError().c_str();
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Write a single variable
///
/// iPIL (position in list) gives position in varlist. 

void CCLUCodeBase::WriteVariable(CXMLTree &rTree, CCodeVar& rVar, int iPIL)
	throw (CCLUException)
{
	bool bIsOK = true;
	char pcText[100];
	string sError;
	CXMLElement rEl;

	rEl.SetName( "var" );
	if (rVar.BaseType() == PDT_TENSOR_IDX)
		rEl.AddProp( "type", rVar.TypeStr(PDT_TENSOR).Str() );
	else
		rEl.AddProp( "type", rVar.TypeStr(rVar.Type()).Str() );
	//rEl.AddProp( "pil", iPIL + 1);

	if (rVar.BaseType() == PDT_SCALAR)
	{
		TCVScalar dVal;
		dVal = *rVar.GetScalarPtr();

		sprintf(pcText, "%g", dVal);
		rEl.SetContent(pcText);
	}
	else if (rVar.BaseType() == PDT_COUNTER)
	{
		TCVCounter iVal;
		iVal = *rVar.GetCounterPtr();

		sprintf(pcText, "%d", iVal);
		rEl.SetContent(pcText);
	}
	else if (rVar.BaseType() == PDT_STRING)
	{
		rEl.SetContent(rVar.ValStr().Str());
	}
	else if (rVar.BaseType() == PDT_MATRIX)
	{
		int iRows, iCols, iPos, iCount;
		string sData;
		TCVScalar *pData;
		TMatrix &xA = *rVar.GetMatrixPtr();

		iRows = xA.Rows();
		iCols = xA.Cols();
		iCount = iRows * iCols;
		pData = xA.Data();

		rEl.AddProp( "rows", iRows );
		rEl.AddProp( "cols", iCols );
	
		for (iPos = 0; iPos < iCount; iPos++)
		{
			sprintf(pcText, "%.17lg ", pData[iPos]);
			sData += pcText;
		}

		rEl.SetContent(sData);
	}
	else if (rVar.BaseType() == PDT_TENSOR ||
			 rVar.BaseType() == PDT_TENSOR_IDX)
	{
		string sData;
		TTensor *pT, T;

		if (rVar.BaseType() == PDT_TENSOR_IDX)
		{
			TTensorIdx &rTIdx = *rVar.GetTensorIdxPtr();
			if (!MakeTensor(T, rTIdx))
				throw CCLUError("Error creating tensor from index.");

			pT = &T;
		}
		else
		{
			pT = rVar.GetTensorPtr();
		}

		TTensor &rT = *pT;

		int iDim, iValence = rT.Valence();
		int iPos, iSize = rT.Size();
		TCVScalar *pData = rT.Data();
		//rEl.AddProp("valence", iValence);

		for(iDim = 0; iDim < iValence; iDim++)
		{
			sprintf(pcText, "%d ", rT.DimSize(iDim));
			sData += pcText;
		}
		rEl.AddProp("dim", sData);

		sData = "";
		for( iPos = 0; iPos < iSize; iPos++ )
		{
			sprintf(pcText, "%.17lg ", pData[iPos]);
			sData += pcText;
		}

		rEl.SetContent(sData);
	}
	else if (rVar.BaseType() == PDT_MULTIV)
	{
		int iPos, iCount, iBID;
		string sData, sBID, sIdx;
		TCVScalar dVal;
		TMultiV &vA = *rVar.GetMultiVPtr();

		if (vA.BasePtr() == 0)
			throw CCLUError("Uninitialized multivector cannot be stored.");

		iCount = (int) vA.GetGADim();
		iBID = vA.BasePtr()->BaseID();

		if (iBID == ID_E3GA)
			sBID = "E3";
		else if (iBID == ID_PGA)
			sBID = "P3";
		else if (iBID == ID_EL2GA)
			sBID = "C2";
		else if (iBID == ID_CONFGA)
			sBID = "N3";

		rEl.AddProp( "bid", sBID );
	
		for (iPos = 0; iPos < iCount; iPos++)
		{
			dVal = vA[iPos];
			if (!::IsZero(dVal, m_fSensitivity))
			{
				sprintf(pcText, "%d ", iPos);
				sIdx += pcText;

				sprintf(pcText, "%.17lg ", dVal);
				sData += pcText;
			}
		}

		rEl.AddProp( "idx", sIdx );
		rEl.SetContent(sData);
	}
	else if (rVar.BaseType() == PDT_VARLIST)
	{
		TVarList &rList = *rVar.GetVarListPtr();
		int iVar, iCount = rList.Count();

		//rEl.AddProp( "size", iCount);

		for(iVar = 0; iVar < iCount; iVar++)
		{
			WriteVariable(rEl.GetSubTree(), rList(iVar), iVar);
		}
	}
	else if (rVar.BaseType() == PDT_VEXLIST)
	{
		CXMLTree &rElTree = rEl.GetSubTree();
		TVexList &rList = *rVar.GetVexListPtr();
		GLenum eType = rList.GetMode();

		switch (eType)
		{
		case GL_POINTS:
			rEl.AddProp("mode", "points");
			break;

		case GL_LINES:
			rEl.AddProp("mode", "lines");
			break;

		case GL_LINE_STRIP:
			rEl.AddProp("mode", "line_strip");
			break;

		case GL_LINE_LOOP:
			rEl.AddProp("mode", "line_loop");
			break;

		case GL_TRIANGLES:
			rEl.AddProp("mode", "triangles");
			break;

		case GL_TRIANGLE_STRIP:
			rEl.AddProp("mode", "triangle_strip");
			break;

		case GL_TRIANGLE_FAN:
			rEl.AddProp("mode", "triangle_fan");
			break;

		case GL_QUADS:
			rEl.AddProp("mode", "quads");
			break;

		case GL_QUAD_STRIP:
			rEl.AddProp("mode", "quad_strip");
			break;

		case GL_POLYGON:
			rEl.AddProp("mode", "polygon");
			break;

		default:
			sError = "Unknown vertex list type.";

			rEl.SetContent(sError);
			rTree.AddElement(rEl);

			throw CCLUError(sError.c_str());
/*
		case GL_:
			rEl.AddProp("mode", "");
			break;
*/
		}

		CXMLElement xmlVex, xmlTex, xmlNorm, xmlCol, xmlIdx;
		string sData;
		int iPos, iSize;

		xmlVex.SetName("vertices");
		xmlTex.SetName("texture_coordinates");
		xmlNorm.SetName("normals");
		xmlCol.SetName("colors");
		xmlIdx.SetName("indices");

		iSize = rList.GetVexList().Count();
		if (iSize > 0)
		{
			xmlVex.AddProp("count", iSize);

			sData = "";
			for( iPos = 0; iPos < iSize; iPos++ )
			{
				float *pData = (float *) rList[iPos];

				sprintf(pcText, "%.7g %.7g %.7g ", pData[0], pData[1], pData[2]);
				sData += pcText;
			}
			xmlVex.SetContent(sData);
			rElTree.AddElement(xmlVex);
		}

		iSize = rList.GetNormList().Count();
		if (iSize > 0)
		{
			xmlNorm.AddProp("count", iSize);

			sData = "";
			for( iPos = 0; iPos < iSize; iPos++ )
			{
				float *pData = (float *) rList.GetNormal(iPos);

				sprintf(pcText, "%.7g %.7g %.7g ", pData[0], pData[1], pData[2]);
				sData += pcText;
			}
			xmlNorm.SetContent(sData);
			rElTree.AddElement(xmlNorm);
		}

		iSize = rList.GetTexList().Count();
		if (iSize > 0)
		{
			xmlTex.AddProp("count", iSize);

			sData = "";
			for( iPos = 0; iPos < iSize; iPos++ )
			{
				float *pData = (float *) rList.GetTex(iPos);

				sprintf(pcText, "%.7g %.7g %.7g ", pData[0], pData[1], pData[2]);
				sData += pcText;
			}
			xmlTex.SetContent(sData);
			rElTree.AddElement(xmlTex);
		}

		iSize = rList.GetColList().Count();
		if (iSize > 0)
		{
			xmlCol.AddProp("count", iSize);

			sData = "";
			for( iPos = 0; iPos < iSize; iPos++ )
			{
				TColor &rData = rList.GetColor(iPos);

				sprintf(pcText, "%.4g %.4g %.4g %.4g ", rData[0], rData[1], rData[2], rData[3]);
				sData += pcText;
			}
			xmlCol.SetContent(sData);
			rElTree.AddElement(xmlCol);
		}

		iSize = rList.GetIdxList().Count();
		if (iSize > 0)
		{
			xmlIdx.AddProp("count", iSize);

			sData = "";
			for( iPos = 0; iPos < iSize; iPos++ )
			{
				sprintf(pcText, "%d ", rList.GetIndex(iPos));
				sData += pcText;
			}
			xmlIdx.SetContent(sData);
			rElTree.AddElement(xmlIdx);
		}
	}
	else
	{
		sError = "Writing variables of type '";
		sError += rVar.TypeStr(rVar.Type()).Str();
		sError += "' is not implemented, yet.";
	
		rEl.SetContent(sError);
		rTree.AddElement(rEl);

		throw CCLUError(sError.c_str());
	}

	rTree.AddElement(rEl);

}



//////////////////////////////////////////////////////////////////////
/// Read Variable Function
///
/// 1. Filename
/// 2. Variable in which to write result (any type)
///
/// Returns string "OK" if data was read, otherwise an error message.

bool CCLUCodeBase::ReadVariableFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	cStr csFilename;
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = mVars.Count();
	
	if (iVarCount != 2)
	{
		int piPar[] = {2};

		m_ErrorList.WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		m_ErrorList.GeneralError("First parameter has to be filename.", iLine, iPos);
		return false;
	}

	if (mVars(1).IsProtected())
	{
		m_ErrorList.GeneralError("Cannot modify variable given as second parameter.", iLine, iPos);
		return false;
	}

	csFilename = mVars(0).ValStr();

	string sError, sVersion;
	char pcCurPath[500];
	CXMLdom xmlDOM;
	CXMLTree xmlTree;

	_getcwd(pcCurPath, 499);
	_chdir(m_sScriptPath.c_str());

	try
	{
		xmlDOM.ParseFile(csFilename.Str());
		xmlDOM.GetElementTree(xmlTree);

		try
		{
			xmlTree.GetElementList("clu_data");
		}
		catch(...)
		{
			throw CCLUError("No CLU data found in file.");
		}
		
		TXMLElementList &xmlRootList = xmlTree.GetElementList("clu_data");
		CXMLElement &xmlEl = xmlRootList[0];

		sVersion = xmlEl.GetProp("version");
		if (sVersion != "1.0")
		{
			sError = "Cannot read data files of version " + sVersion + ".";
			throw CCLUError(sError.c_str());
		}

		TXMLElementList &rElList = xmlRootList[0].GetSubTree().GetElementList("var");
		ReadVariable(mVars(1), rElList[0]);

		// Return with OK message to script
		rVar = "OK";
	}
	catch (CCLUException &rEx)
	{
		rVar = rEx.PrintError().c_str();
	}

	_chdir(pcCurPath);

	return true;

}


//////////////////////////////////////////////////////////////////////
/// Read a single variable

void CCLUCodeBase::ReadVariable(CCodeVar& rVar, CXMLElement &rEl)
	throw (CCLUException)
{
	char pcText[100];
	string sType, sError, sContent;

	sType = rEl.GetProp("type");
	sContent = rEl.GetContent();


	if (sType == CCodeVar::TypeStr(PDT_SCALAR).Str())
	{
		TCVScalar dVal;
		sscanf(sContent.c_str(), "%lg", &dVal);
		rVar = dVal;
	}
	else if (sType == CCodeVar::TypeStr(PDT_COUNTER).Str())
	{
		TCVCounter iVal;
		sscanf(sContent.c_str(), "%d", &iVal);
		rVar = iVal;
	}
	else if (sType == CCodeVar::TypeStr(PDT_STRING).Str())
	{
		rVar = sContent.c_str();
	}
	else if (sType == CCodeVar::TypeStr(PDT_MATRIX).Str())
	{
		stringstream zContent;
		int iRows, iCols, iPos, iCount;
		TCVScalar *pData;
		rVar.New(PDT_MATRIX, rVar.Name().Str());
		TMatrix &xA = *rVar.GetMatrixPtr();

		iRows = atoi(rEl.GetProp("rows").c_str());
		iCols = atoi(rEl.GetProp("cols").c_str());

		if (!xA.Resize(iRows, iCols))
		{
			sprintf(pcText, "Cannot create matrix of dimensions %dx%d.", iRows, iCols);
			throw CCLUError(pcText);
		}

		zContent.str(sContent);
		pData = xA.Data();
		iCount = iRows * iCols;
		for (iPos = 0; iPos < iCount; iPos++)
		{
			pcText[0] = 0;
			zContent.getline(pcText, 99, ' ');
			if (pcText[0] == 0)
				throw CCLUError("Not enough data for matrix.");

			sscanf(pcText, "%lg", &pData[iPos]);
		}
	}
	else if (sType == CCodeVar::TypeStr(PDT_TENSOR).Str())
	{
		stringstream zContent, zDim;
		Mem<int> mDim;
		int iPos, iDim, iValence, iSize;
		TCVScalar *pData;

		rVar.New(PDT_TENSOR);
		TTensor &rT = *rVar.GetTensorPtr();

		zDim.str(rEl.GetProp("dim"));
		iValence = 0; 
		while( true )
		{
			pcText[0] = 0;
			zDim.getline(pcText, 99, ' ');
			if (pcText[0] == 0)
				break;

			sscanf(pcText, "%d", &iDim);
			iValence++;
			mDim.Add(1);
			mDim.Last() = iDim;
		}

		rT.Reset(mDim);
		pData = rT.Data();
		iSize = rT.Size();

		zContent.str(rEl.GetContent());
		iPos = 0;
		while( iPos < iSize )
		{
			pcText[0] = 0;
			zContent.getline(pcText, 99, ' ');
			if (pcText[0] == 0)
				throw CCLUError("Not enough data for tensor.");

			sscanf(pcText, "%lg", &pData[iPos]);
			iPos++;
		}
	}
	else if (sType == CCodeVar::TypeStr(PDT_MULTIV).Str())
	{
		stringstream zContent, zIdx;
		int iPos;
		string sBID, sIdx;

		rVar.New(PDT_MULTIV, rVar.Name().Str());
		TMultiV &vA = *rVar.GetMultiVPtr();

		sIdx = rEl.GetProp("idx");
		sBID = rEl.GetProp("bid");

		if (sBID == "E3")
			vA = E3GA<TCVScalar>::vZero;
		else if (sBID == "P3")
			vA = PGA<TCVScalar>::vZero;
		else if (sBID == "C2")
			vA = El2GA<TCVScalar>::vZero;
		else if (sBID == "N3")
			vA = ConfGA<TCVScalar>::vZero;

		zIdx.str(sIdx);
		zContent.str(sContent);
		while(true)
		{
			pcText[0] = 0;
			zIdx.getline(pcText, 99, ' ');
			if (pcText[0] == 0)
				break;

			sscanf(pcText, "%d", &iPos);

			pcText[0] = 0;
			zContent.getline(pcText, 99, ' ');
			if (pcText[0] == 0)
				throw CCLUError("Not enough data for multivector.");

			sscanf(pcText, "%lg", &vA[iPos]);
		}
	}
	else if (sType == CCodeVar::TypeStr(PDT_VARLIST).Str())
	{
		/*
		int iSize;
		string sSize;
		sSize = rEl.GetProp("size");
		iSize = atoi(sSize.c_str());
		if (iSize <= 0)
		throw CCLUError("Invalid variable list size.");
		*/
		rVar.New(PDT_VARLIST);
		TVarList &rSubList = *rVar.GetVarListPtr();

		try
		{
			TXMLElementList &rElList = rEl.GetSubTree().GetElementList("var");
			int iEl, iElCount = rElList.size();
			rSubList.Set(iElCount);

			for (iEl = 0; iEl < iElCount; iEl++)
			{
				ReadVariable(rSubList(iEl), rElList[iEl]);
			}
		}
		catch (CXMLTree::ElementNotAvailable &rEx)
		{
			// empty list; no problem
		}
		catch (CCLUException &rEx)
		{
			throw rEx;	// throw on
		}
	}
	else if (sType == CCodeVar::TypeStr(PDT_VEXLIST).Str())
	{
		stringstream zContent;
		string sMode;
		int iPos, iCount;

		rVar.New(PDT_VEXLIST);
		TVexList &rVexList = *rVar.GetVexListPtr();


		sMode = rEl.GetProp("mode");
		if (sMode == "points")
			rVexList.SetMode(GL_POINTS);
		else if (sMode == "lines")
			rVexList.SetMode(GL_LINES);
		else if (sMode == "line_strip")
			rVexList.SetMode(GL_LINE_STRIP);
		else if (sMode == "line_loop")
			rVexList.SetMode(GL_LINE_LOOP);
		else if (sMode == "triangles")
			rVexList.SetMode(GL_TRIANGLES);
		else if (sMode == "triangle_strip")
			rVexList.SetMode(GL_TRIANGLE_STRIP);
		else if (sMode == "triangle_loop")
			rVexList.SetMode(GL_TRIANGLE_FAN);
		else if (sMode == "triangle_fan")
			rVexList.SetMode(GL_TRIANGLE_FAN);
		else if (sMode == "quads")
			rVexList.SetMode(GL_QUADS);
		else if (sMode == "quad_strip")
			rVexList.SetMode(GL_QUAD_STRIP);
		else if (sMode == "polygon")
			rVexList.SetMode(GL_POLYGON);

		/////////////////////////////////////////////////////////
		// Read Vertices
		try
		{
			CXMLElement &rSubEl = rEl.GetSubTree().GetElementList("vertices")[0];
			Mem<COGLVertex> &rValList = rVexList.GetVexList();

			iCount = atoi(rSubEl.GetProp("count").c_str());
			rValList.Set(iCount);

			zContent.str(rSubEl.GetContent());
			for (iPos = 0; iPos < iCount; iPos++ )
			{
				float *pData = (float *) rValList[iPos];

				for (int i = 0; i < 3; i++)
				{
					pcText[0] = 0;
					zContent.getline(pcText, 99, ' ');
					if (pcText[0] == 0)
						throw CCLUError("Not enough data for vertex.");

					sscanf(pcText, "%g", &pData[i]);
				}
			}
		}
		catch (CXMLTree::ElementNotAvailable &rEx)
		{
			// empty list; no problem
		}
		catch (CCLUException &rEx)
		{
			throw rEx;	// throw on
		}

		/////////////////////////////////////////////////////////
		// Read Normals
		try
		{
			CXMLElement &rSubEl = rEl.GetSubTree().GetElementList("normals")[0];
			Mem<COGLVertex> &rValList = rVexList.GetNormList();

			iCount = atoi(rSubEl.GetProp("count").c_str());
			rValList.Set(iCount);

			zContent.str(rSubEl.GetContent());
			for (iPos = 0; iPos < iCount; iPos++ )
			{
				float *pData = (float *) rValList[iPos];

				for (int i = 0; i < 3; i++)
				{
					pcText[0] = 0;
					zContent.getline(pcText, 99, ' ');
					if (pcText[0] == 0)
						throw CCLUError("Not enough data for normal.");

					sscanf(pcText, "%g", &pData[i]);
				}
			}
		}
		catch (CXMLTree::ElementNotAvailable &rEx)
		{
			// empty list; no problem
		}
		catch (CCLUException &rEx)
		{
			throw rEx;	// throw on
		}

		/////////////////////////////////////////////////////////
		// Read Texture Coordinates
		try
		{
			CXMLElement &rSubEl = rEl.GetSubTree().GetElementList("texture_coordinates")[0];
			Mem<COGLVertex> &rValList = rVexList.GetTexList();

			iCount = atoi(rSubEl.GetProp("count").c_str());
			rValList.Set(iCount);

			zContent.str(rSubEl.GetContent());
			for (iPos = 0; iPos < iCount; iPos++ )
			{
				float *pData = (float *) rValList[iPos];

				for (int i = 0; i < 3; i++)
				{
					pcText[0] = 0;
					zContent.getline(pcText, 99, ' ');
					if (pcText[0] == 0)
						throw CCLUError("Not enough data for texture coordinate.");

					sscanf(pcText, "%g", &pData[i]);
				}
			}
		}
		catch (CXMLTree::ElementNotAvailable &rEx)
		{
			// empty list; no problem
		}
		catch (CCLUException &rEx)
		{
			throw rEx;	// throw on
		}

		/////////////////////////////////////////////////////////
		// Read Colors
		try
		{
			CXMLElement &rSubEl = rEl.GetSubTree().GetElementList("colors")[0];
			Mem<TColor> &rValList = rVexList.GetColList();

			iCount = atoi(rSubEl.GetProp("count").c_str());
			rValList.Set(iCount);

			zContent.str(rSubEl.GetContent());
			for (iPos = 0; iPos < iCount; iPos++ )
			{
				TColor &rData = rValList[iPos];

				for (int i = 0; i < 4; i++)
				{
					pcText[0] = 0;
					zContent.getline(pcText, 99, ' ');
					if (pcText[0] == 0)
						throw CCLUError("Not enough data for color.");

					sscanf(pcText, "%g", &rData[i]);
				}
			}
		}
		catch (CXMLTree::ElementNotAvailable &rEx)
		{
			// empty list; no problem
		}
		catch (CCLUException &rEx)
		{
			throw rEx;	// throw on
		}

		/////////////////////////////////////////////////////////
		// Read Index List
		try
		{
			CXMLElement &rSubEl = rEl.GetSubTree().GetElementList("indices")[0];
			Mem<int> &rValList = rVexList.GetIdxList();

			iCount = atoi(rSubEl.GetProp("count").c_str());
			rValList.Set(iCount);

			zContent.str(rSubEl.GetContent());
			for (iPos = 0; iPos < iCount; iPos++ )
			{
				pcText[0] = 0;
				zContent.getline(pcText, 99, ' ');
				if (pcText[0] == 0)
					throw CCLUError("Not enough data for normal.");

				sscanf(pcText, "%d", &rValList[iPos]);
			}
		}
		catch (CXMLTree::ElementNotAvailable &rEx)
		{
			// empty list; no problem
		}
		catch (CCLUException &rEx)
		{
			throw rEx;	// throw on
		}
	}
	else
	{
		sError = "Reading variables of type '";
		sError += sType;
		sError += "' is not implemented, yet.";

		throw CCLUError(sError.c_str());
	}
}


//////////////////////////////////////////////////////////////////////
/// Read Data Function
///
/// 1. Filename
/// 2. Separator Symbol (optional). Standard is space. 
///		Return is always end of line. Everything in one line is put in a list.

bool CCLUCodeBase::ReadDataFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	char pcSep[2];
	cStr csFilename;
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = mVars.Count();
	
	if (iVarCount < 1 || iVarCount > 2)
	{
		int piPar[] = {1, 2};

		m_ErrorList.WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		m_ErrorList.InvalidType(mVars(0), iLine, iPos);
		return false;
	}

	if (iVarCount >= 2)
	{
		cStr csVal;
		if (mVars(1).BaseType() != PDT_STRING)
		{
			m_ErrorList.InvalidType(mVars(1), iLine, iPos);
			return false;
		}

		csVal = mVars(1).ValStr();
		pcSep[0] = csVal[0];
	}
	else
	{
		pcSep[0] = ' ';
	}
	pcSep[1] = 0;

	csFilename = mVars(0).ValStr();

	ifstream zData;
	char pcCurPath[500], pcLine[301], pcVal[2], cVal;
	double dVal;

	rVar.New(PDT_VARLIST, "Constant");
	TVarList &DList = *rVar.GetVarListPtr();
	vector<TVarList *> vecVarList;
	TVarList *pCurVarList;

	_getcwd(pcCurPath, 499);
	_chdir(m_sScriptPath.c_str());

	zData.open(csFilename.Str());
	if (!zData.is_open())
	{
		_chdir(pcCurPath);
	
		char pcText[500];
		sprintf(pcText, "Data file '%s' could not be opened.", csFilename.Str());

		m_ErrorList.GeneralError(pcText);
		return false;
	}

	vecVarList.push_back(&DList);
	DList.Add(1);
	DList.Last().New(PDT_VARLIST, "Constant");
	pCurVarList = ((TVarList *) DList.Last().Val());
	vecVarList.push_back(pCurVarList);

	pcVal[1] = 0;
	pcLine[0] = 0;
	do
	{
		cVal = zData.get();
		//zData.getline(pcLine, 299);
		if (zData.eof() && pcLine[0] == 0)
			break;

		else if (zData.eof())
			cVal = pcSep[0];
		
		if ((cVal == pcSep[0] || cVal == '\n') && pcLine[0] != 0)
		{
			if (sscanf(pcLine, "%lg", &dVal) == 1)
			{
				pCurVarList->Add(1);
				pCurVarList->Last() = (TCVScalar) dVal;
			}
			pcLine[0] = 0;
		}
		else if (cVal != pcSep[0] && cVal != '\n')
		{
			pcVal[0] = cVal;
			strcat(pcLine, pcVal);
		}
		
		if (cVal == '\n')
		{
			vecVarList.pop_back();
			pCurVarList = vecVarList.back();
			pCurVarList->Add(1);
			pCurVarList->Last().New(PDT_VARLIST, "Constant");
			pCurVarList = ((TVarList *) pCurVarList->Last().Val());
			vecVarList.push_back(pCurVarList);
			pcLine[0] = 0;
		}

	}
	while(!zData.eof());
	
	zData.close();

	_chdir(pcCurPath);

	return true;
}



//////////////////////////////////////////////////////////////////////
/// Write Data Function
///
/// 1. Variable List
/// 2. Filename
/// 3. Separator Symbol (optional). Standard is space. 
///		Return is always end of line. Everything in one line is put in a list.

bool CCLUCodeBase::WriteDataFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	char pcSep[2];
	int iList, iStr;
	cStr csFilename;
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = mVars.Count();
	
	if (iVarCount < 2 || iVarCount > 3)
	{
		int piPar[] = {2, 3};

		m_ErrorList.WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() == PDT_VARLIST &&
		mVars(1).BaseType() == PDT_STRING)
	{
		iList = 0;
		iStr = 1;
	}
	else if (mVars(1).BaseType() == PDT_VARLIST &&
			 mVars(0).BaseType() == PDT_STRING)
	{
		iList = 1;
		iStr = 0;
	}
	else if (mVars(0).BaseType() != PDT_VARLIST &&
			 mVars(0).BaseType() != PDT_STRING)
	{
		m_ErrorList.InvalidType(mVars(0), iLine, iPos);
		return false;
	}
	else if (mVars(1).BaseType() != PDT_VARLIST &&
			 mVars(1).BaseType() != PDT_STRING)
	{
		m_ErrorList.InvalidType(mVars(1), iLine, iPos);
		return false;
	}

	if (iVarCount >= 3)
	{
		cStr csVal;
		if (mVars(2).BaseType() != PDT_STRING)
		{
			m_ErrorList.InvalidType(mVars(2), iLine, iPos);
			return false;
		}

		csVal = mVars(2).ValStr();
		pcSep[0] = csVal[0];
	}
	else
	{
		pcSep[0] = ' ';
	}
	pcSep[1] = 0;

	csFilename = mVars(iStr).ValStr();

	ofstream zData;
	char pcCurPath[500], pcLine[301];
	TCVScalar dVal;

	TVarList &DList = *mVars(iList).GetVarListPtr();
	int iDataPos, iDataCount = DList.Count();

	_getcwd(pcCurPath, 499);
	_chdir(m_sScriptPath.c_str());

	zData.open(csFilename.Str());
	if (!zData.is_open())
	{
		_chdir(pcCurPath);
	
		char pcText[500];
		sprintf(pcText, "Data file '%s' could not be opened.", csFilename.Str());

		m_ErrorList.GeneralError(pcText);
		return false;
	}

	for (iDataPos = 0; iDataPos < iDataCount; iDataPos++)
	{
		CCodeVar &rDataVar = DList(iDataPos);

		if (rDataVar.CastToScalar(dVal, m_fSensitivity))
		{
			sprintf(pcLine, "%.17lg%s", dVal, pcSep);
			zData << pcLine;
		}
		else if (rDataVar.BaseType() == PDT_VARLIST)
		{
			TVarList& rList = *rDataVar.GetVarListPtr();
			int i, iCount = rList.Count();

			zData << "\n";
			for (i = 0; i < iCount; i++)
			{
				if (!rList(i).CastToScalar(dVal, m_fSensitivity))
				{
					m_ErrorList.GeneralError("Cannot write data.", iLine, iPos);
					zData.close();
					return false;
				}

				sprintf(pcLine, "%.17lg%s", dVal, pcSep);
				zData << pcLine;
			}
		}
		else
		{
			m_ErrorList.GeneralError("Cannot write data.", iLine, iPos);
			zData.close();
			return false;
		}
	}
	
	zData.close();

	_chdir(pcCurPath);

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Read Matrix Function
///
/// 1. Filename
/// 2. Separator Symbol (optional). Standard is space. 
///		Return is always end of line. Everything in one line is put in a list.

bool CCLUCodeBase::ReadMatrixFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	char pcSep[2];
	cStr csFilename;
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = mVars.Count();
	
	if (iVarCount < 1 || iVarCount > 2)
	{
		int piPar[] = {1, 2};

		m_ErrorList.WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		m_ErrorList.InvalidType(mVars(0), iLine, iPos);
		return false;
	}

	if (iVarCount >= 2)
	{
		cStr csVal;
		if (mVars(1).BaseType() != PDT_STRING)
		{
			m_ErrorList.InvalidType(mVars(1), iLine, iPos);
			return false;
		}

		csVal = mVars(1).ValStr();
		pcSep[0] = csVal[0];
	}
	else
	{
		pcSep[0] = ' ';
	}
	pcSep[1] = 0;

	csFilename = mVars(0).ValStr();

	ifstream zData;
	int iLineLen, iRow, iCol, iRowCount, iColCount;
	char pcCurPath[500], pcLine[301], pcVal[2], cVal;
	double dVal;

	rVar.New(PDT_MATRIX, "Constant");
	TMatrix &DMatrix = *rVar.GetMatrixPtr();

	_getcwd(pcCurPath, 499);
	_chdir(m_sScriptPath.c_str());

	zData.open(csFilename.Str());
	if (!zData.is_open())
	{
		_chdir(pcCurPath);
	
		char pcText[500];
		sprintf(pcText, "Matrix data file '%s' could not be opened.", csFilename.Str());

		m_ErrorList.GeneralError(pcText);
		return false;
	}

	iRowCount = 0;
	iColCount = 0;
	iRow = -1;
	iCol = -2;
	pcVal[1] = 0;
	pcLine[0] = 0;
	iLineLen = 0;
	do
	{
		cVal = zData.get();
		//zData.getline(pcLine, 299);
		if (zData.eof() && pcLine[0] == 0)
			break;
		else if (zData.eof())
			cVal = pcSep[0];
		
		if ((cVal == pcSep[0] || cVal == '\n') && pcLine[0] != 0)
		{
			if (sscanf(pcLine, "%lg", &dVal) == 1)
			{
				if (iCol == -2)
					iRowCount = int(dVal);
				else if (iCol == -1)
				{
					iColCount = int(dVal);
					if (iRowCount <= 0)
					{
						m_ErrorList.GeneralError("Invalid row size of matrix.", iLine, iPos);
						return false;
					}
					if (iColCount <= 0)
					{
						m_ErrorList.GeneralError("Invalid column size of matrix.", iLine, iPos);
						return false;
					}
					DMatrix.Resize(iRowCount, iColCount);
				}
				else if (iRow >= 0 && iRow < iRowCount &&
						iCol >= 0 && iCol < iColCount)
				{
					DMatrix(iRow+1, iCol+1) = TCVScalar(dVal);
				}
			}
			iLineLen = 0;
			pcLine[0] = 0;
			iCol++;
		}
		else if (cVal != pcSep[0] && cVal != '\n')
		{
			pcLine[iLineLen] = cVal;
			pcLine[++iLineLen] = 0;
		}
		
		if (cVal == '\n')
		{
			if (iCol < 0)
			{
				m_ErrorList.GeneralError("Matrix dimensions not specified.", iLine, iPos);
				return false;
			}
			iRow++;
			iCol = 0;
		}
	}
	while(!zData.eof());
	
	zData.close();

	_chdir(pcCurPath);

	return true;
}





/////////////////////////////////////////////////////////////////////////////////
// Show a file with standard program defined by OS

bool CCLUCodeBase::ShowFileFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = mVars.Count();
	
	if (iVarCount < 1 || iVarCount > 1)
	{
		int piPar[] = {1};

		m_ErrorList.WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		m_ErrorList.InvalidType(mVars(0), iLine, iPos);
		return false;
	}

# ifdef WIN32
	string sFile;

	sFile = m_sScriptPath;
	sFile += mVars(0).ValStr();

	HINSTANCE hInstance = ShellExecute(NULL,
		"open",
		sFile.c_str(),
		NULL,
		NULL,
		SW_SHOWNORMAL);//SHOWMINIMIZED);

	if ((int) hInstance < 32)
	{
		m_ErrorList.GeneralError("File cannot be found.", iLine, iPos);
		return false;
	}
#endif

	return true;
}


/////////////////////////////////////////////////////////////////////////////////
// Show a file with standard program defined by OS

bool CCLUCodeBase::FileChooserFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	string sTitle, sPat;
	int iVarCount = mVars.Count();
	
	if (iVarCount > 2)
	{
		int piPar[] = {0, 1, 2};

		m_ErrorList.WrongNoOfParams(piPar, 3, iLine, iPos);
		return false;
	}


	if (iVarCount >= 1)
	{
		if (mVars(0).BaseType() != PDT_STRING)
		{
			m_ErrorList.InvalidType(mVars(0), iLine, iPos);
			return false;
		}
	
		sTitle = mVars(0).ValStr().Str();
	}
	else
	{
		sTitle = "Select File";
	}

	if (iVarCount >= 2)
	{
		if (mVars(1).BaseType() != PDT_STRING)
		{
			m_ErrorList.InvalidType(mVars(1), iLine, iPos);
			return false;
		}
	
		sPat = mVars(1).ValStr().Str();
	}
	else
	{
		sPat = "All Files (*)";
	}

	char pcFilename[2];

	pcFilename[0] = 0;
	Fl_File_Chooser::sort = fl_casenumericsort;
	char *pcNewFile = fl_file_chooser(sTitle.c_str(), sPat.c_str(), pcFilename);

	rVar.New(PDT_STRING, "Constant");
	if (pcNewFile)
		*rVar.GetStringPtr() = pcNewFile;

	return true;
}


/////////////////////////////////////////////////////////////////////////////////
// Save Screen to BMP file

bool CCLUCodeBase::SaveScreenFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	string sFilename;
	int iVarCount = mVars.Count();
	
	if (iVarCount != 1)
	{
		int piPar[] = {1};

		m_ErrorList.WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}


	if (iVarCount == 1)
	{
		if (mVars(0).BaseType() != PDT_STRING)
		{
			m_ErrorList.InvalidType(mVars(0), iLine, iPos);
			return false;
		}
	
		sFilename = mVars(0).ValStr().Str();
		sFilename = m_sScriptPath + sFilename;
	}

	if (!m_pCLUDrawBase)
	{
		m_ErrorList.GeneralError("No visualization window present.", iLine, iPos);
		return false;
	}

	if (!m_pCLUDrawBase->SaveScreen2BMP(sFilename.c_str()))
	{
		m_ErrorList.GeneralError("Could not save image.", iLine, iPos);
		return false;
	}

	return true;
}





//////////////////////////////////////////////////////////////////////
/// Read XML File Function
///
/// 1. Filename
/// 2. Variable in which to write result (any type)
///
/// Returns string "OK" if data was read, otherwise an error message.

bool CCLUCodeBase::ReadXMLFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
	throw (CCLUException)
{
	cStr csFilename;
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = mVars.Count();
	
	if (iVarCount != 2)
	{
		int piPar[] = {2};

		m_ErrorList.WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		m_ErrorList.GeneralError("First parameter has to be filename.", iLine, iPos);
		return false;
	}

	if (mVars(1).IsProtected())
	{
		m_ErrorList.GeneralError("Cannot modify variable given as second parameter.", iLine, iPos);
		return false;
	}

	csFilename = mVars(0).ValStr();

	string sError, sVersion, sRootName;
	char pcCurPath[500];
	CXMLdom xmlDOM;
	CXMLTree xmlTree;

	_getcwd(pcCurPath, 499);
	_chdir(m_sScriptPath.c_str());

	try
	{
		xmlDOM.ParseFile(csFilename.Str());
		xmlDOM.GetElementTree(xmlTree);
		
		mVars(1).New(PDT_VARLIST);
		TVarList &rList = *mVars(1).GetVarListPtr();
/*		
		rList.Set(2);
		rList(0) = xmlTree.begin()->first.c_str();

		CCodeVar &rVarRoot = rList(1);
		rVarRoot.New(PDT_VARLIST, "Constant");

		ReadXMLTree(*rVarRoot.GetVarListPtr(), 
					xmlTree.begin()->second[0].GetSubTree(), iLine, iPos);
*/

		ReadXMLTree(rList, xmlTree, iLine, iPos);

		// Return with OK message to script
		rVar = "OK";
	}
	catch (CCLUException &rEx)
	{
		rVar = rEx.PrintError().c_str();
	}

	_chdir(pcCurPath);

	return true;

}


//////////////////////////////////////////////////////////////////////
/// Read XML Tree
///

void CCLUCodeBase::ReadXMLTree(TVarList& rList, CXMLTree& rTree, int iLine, int iPos) 
	throw (CCLUException)
{
	int iTreeSize = rTree.size();
	int iElCount, iEl;

	if (iTreeSize == 0)
		return;

	map<string, TXMLElementList>::iterator itElList;
	
	for (itElList = rTree.begin(); itElList != rTree.end(); ++itElList)
	{
		rList.Add(1);
		CCodeVar &rVar = rList.Last();

		rVar.New(PDT_VARLIST, "Constant");
		TVarList &rSubList = *rVar.GetVarListPtr();

		rSubList.Set(2);
		rSubList(0) = itElList->first.c_str();
		
		rSubList(1).New(PDT_VARLIST, "Constant");
		TVarList &rVarElList = *rSubList(1).GetVarListPtr();

		TXMLElementList &rElList = itElList->second;
		iElCount = rElList.size();
		rVarElList.Set(iElCount);

		for (iEl = 0; iEl < iElCount; iEl++)
		{
			CCodeVar &rVarEl = rVarElList(iEl);
			CXMLElement &rEl = rElList[iEl];
			map<string, string> &rPropList = rEl.GetPropMap();
			map<string, string>::iterator itProp;
			int iPos, iCount = rPropList.size();

			rVarEl.New(PDT_VARLIST, "Constant");
			TVarList &rVarElSubList = *rVarEl.GetVarListPtr();

			rVarElSubList.Set(iCount + 2);
			// Store Property List
			for (itProp = rPropList.begin(), iPos = 0; 
				 itProp != rPropList.end(); 
				 ++itProp, ++iPos)
			{
				CCodeVar &rVarProp = rVarElSubList(iPos);
				rVarProp.New(PDT_VARLIST, "Constant");
				TVarList &rVarPropPair = *rVarProp.GetVarListPtr();

				rVarPropPair.Set(2);
				rVarPropPair(0) = itProp->first.c_str();
				rVarPropPair(1) = itProp->second.c_str();
			}

			// Store Content
			{
				CCodeVar &rVarProp = rVarElSubList(iCount);
				rVarProp.New(PDT_VARLIST, "Constant");
				TVarList &rVarPropPair = *rVarProp.GetVarListPtr();

				rVarPropPair.Set(2);
				rVarPropPair(0) = "_content";
				rVarPropPair(1) = rEl.GetContent().c_str();
			}

			// Store SubTree
			{
				CCodeVar &rVarProp = rVarElSubList(iCount+1);
				rVarProp.New(PDT_VARLIST, "Constant");
				TVarList &rVarPropPair = *rVarProp.GetVarListPtr();

				rVarPropPair.Set(2);
				rVarPropPair(0) = "_subtree";
				
				rVarPropPair(1).New(PDT_VARLIST, "Constant");
				
				ReadXMLTree(*rVarPropPair(1).GetVarListPtr(), rEl.GetSubTree(), iLine, iPos);
			}
		}
	}
}




//////////////////////////////////////////////////////////////////////
/// Read Text Function
///
/// 1. Filename
/// 2. Variable in which file content is stored as string.
///
///		Returns 'OK' if no error occured, and error string otherwise.

bool CCLUCodeBase::ReadTextFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	cStr csFilename;
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = mVars.Count();
	
	if (iVarCount != 2)
	{
		int piPar[] = { 2};

		m_ErrorList.WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		m_ErrorList.InvalidType(mVars(0), iLine, iPos);
		return false;
	}

	mVars(1).New(PDT_STRING);
	TString &rText = *mVars(1).GetStringPtr();

	csFilename = mVars(0).ValStr();

	ifstream zData;
	char pcCurPath[500], pcLine[1025];

	_getcwd(pcCurPath, 499);
	_chdir(m_sScriptPath.c_str());

	zData.open(csFilename.Str());
	if (!zData.is_open())
	{
		_chdir(pcCurPath);
	
		char pcText[500];
		sprintf(pcText, "Text file '%s' could not be opened.", csFilename.Str());
	
		rVar = pcText;
		//m_ErrorList.GeneralError(pcText);
		return true;
	}

	do
	{
		zData.read(pcLine, 1024);
		pcLine[ zData.gcount() ] = 0;
		rText << pcLine;
	}
	while(!zData.eof());
	
	zData.close();

	_chdir(pcCurPath);
	rVar = "OK";

	return true;
}




//////////////////////////////////////////////////////////////////////
/// Write Text Function
///
/// 1. Filename
/// 2. String Variable that is to be written.
///
///		Returns 'OK' if no error occured, and error string otherwise.

bool CCLUCodeBase::WriteTextFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	cStr csFilename;
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = mVars.Count();
	
	if (iVarCount != 2)
	{
		int piPar[] = { 2};

		m_ErrorList.WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		m_ErrorList.GeneralError("No filename given.", iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_STRING)
	{
		m_ErrorList.GeneralError("Expect second parameter to be a string.", iLine, iPos);
		return false;
	}

	TString &rText = *mVars(1).GetStringPtr();

	csFilename = mVars(0).ValStr();

	ofstream zData;
	char pcCurPath[500];

	_getcwd(pcCurPath, 499);
	_chdir(m_sScriptPath.c_str());

	zData.open(csFilename.Str());
	if (!zData.is_open())
	{
		_chdir(pcCurPath);
	
		char pcText[500];
		sprintf(pcText, "Text file '%s' could not be opened for writing.", csFilename.Str());
	
		rVar = pcText;
		//m_ErrorList.GeneralError(pcText);
		return true;
	}

	zData.write(rText.Str(), rText.Len());
	zData.close();

	_chdir(pcCurPath);
	rVar = "OK";

	return true;
}
