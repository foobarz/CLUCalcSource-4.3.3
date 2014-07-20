
/// LICENSE START
////////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the CLUCalc program.
// CLUCalc is an interactive Clifford calculator with OpenGL visualiations.
//
// 
// Copyright (C) 2002-2004  Christian B.U. Perwass
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

#include "CLUScriptEditor.h"

#include <sys/types.h>
#include <sys/stat.h>

#ifdef _GNUCPP3_
#   include <unistd.h>
#   define _getcwd getcwd
#   define _chdir chdir
#   define _stat stat
//#   define _remove remove
#else
#   include <direct.h>
#   include <io.h>
#endif

#ifdef WIN32
#	include <ShellApi.h>
#endif

#include <FL/Fl_Color_Chooser.H>

CCLUScriptEditor::CCLUScriptEditor(void)
{
	InitStyle();

	m_bParseAfterNew = true;

	m_sFileChooserLoadTitle = "Load File";
	m_sFileChooserSaveTitle = "Save File";
	m_sFileChooserPattern = "CLUScript File (*.clu)";

	m_poglWin = 0;
	m_pOutputWin = 0;
}

CCLUScriptEditor::CCLUScriptEditor(int iPosX, int iPosY, int iWidth, int iHeight, 
								   const char* pcFilename, const char *pcExecPath,
								   void *pvIcon)
{
	InitStyle();

	m_sFileChooserLoadTitle = "Load File";
	m_sFileChooserSaveTitle = "Save File";
	m_sFileChooserPattern = "CLUScript File (*.clu)";

	m_poglWin = 0;
	m_pOutputWin = 0;

	Create(iPosX, iPosY, iWidth, iHeight, pcFilename, pcExecPath, pvIcon);
}

CCLUScriptEditor::~CCLUScriptEditor(void)
{
}

bool CCLUScriptEditor::Create(int iPosX, int iPosY, int iWidth, int iHeight, 
							  const char *pcFilename, const char *pcExecPath,
							  void *pvIcon)
{
	CFLTKEditor::Create(iPosX, iPosY, iWidth, iHeight, pcFilename, pvIcon);

	//////////////////////////////////////////
	//begin();

	//m_pFileChoice->position(60, 23);

	m_pFileParse = new Fl_Button(0, 2, 25, 15);
	m_pFileParse->box(FL_BORDER_BOX);
	m_pFileParse->labelsize(10);
	m_pFileParse->labelcolor(FL_BLUE);
	m_pFileParse->label("@>");
	m_pFileParse->color(FL_LIGHT2);
	m_pFileParse->color2(FL_WHITE);
	m_pFileParse->tooltip("Run Script (ctrl + p)");
	m_pFileParse->callback((Fl_Callback *) CCLUScriptEditor::CB_Parse, this);

	m_pFileParseForce = new Fl_Button(0, 2, 25, 15);
	m_pFileParseForce->box(FL_BORDER_BOX);
	m_pFileParseForce->labelsize(10);
	m_pFileParseForce->labelcolor(FL_BLUE);
	m_pFileParseForce->label("@>>");
	m_pFileParseForce->color(FL_LIGHT2);
	m_pFileParseForce->color2(FL_WHITE);
	m_pFileParseForce->tooltip("Run Script, Force Latex Rendering (ctrl + shift + p)");
	m_pFileParseForce->callback((Fl_Callback *) CCLUScriptEditor::CB_ParseForce, this);

	m_pToolBar->insert(*m_pFileParseForce, m_pFileChoice);
	m_pToolBar->insert(*m_pFileParse, m_pFileChoice);
	//end();
	//////////////////////////////////////////

	Show();

	string sFile;

	if (pcExecPath)
		m_sExecPath = pcExecPath;
	else
		m_sExecPath = ".";

	sFile = m_sExecPath + "/CLUCalcHelp.html";

	m_pHelpDialog = new Fl_Help_Dialog;
	m_pHelpDialog->load(sFile.c_str());
	m_pHelpDialog->hide();

	return true;
}


void CCLUScriptEditor::InitStyle()
{
	m_mapHighlight["Plot"] = 'E';

    m_mapHighlight["Mouse"] = 'E';

	m_mapHighlight["Ran"] = 'E';
	m_mapHighlight["SetRanSeed"] = 'E';
	m_mapHighlight["Gauss"] = 'E';
	m_mapHighlight["SetGaussPars"] = 'E';

	m_mapHighlight["sum"] = 'E';
	m_mapHighlight["sub"] = 'E';
	m_mapHighlight["prod"] = 'E';
	m_mapHighlight["oprod"] = 'E';
	m_mapHighlight["iprod"] = 'E';
	m_mapHighlight["min"] = 'E';
	m_mapHighlight["max"] = 'E';
	m_mapHighlight["argmin"] = 'E';
	m_mapHighlight["argmax"] = 'E';

	m_mapHighlight["abs"] = 'E';
	m_mapHighlight["pow"] = 'E';
	m_mapHighlight["sqrt"] = 'E';
	m_mapHighlight["inv"] = 'E';
	m_mapHighlight["fact"] = 'E';
	m_mapHighlight["sign"] = 'E';
	m_mapHighlight["sin"] = 'E';
	m_mapHighlight["cos"] = 'E';
	m_mapHighlight["tan"] = 'E';
	m_mapHighlight["asin"] = 'E';
	m_mapHighlight["acos"] = 'E';
	m_mapHighlight["atan"] = 'E';

	m_mapHighlight["floor"] = 'E';
	m_mapHighlight["ceil"] = 'E';
	m_mapHighlight["round"] = 'E';
	m_mapHighlight["exp"] = 'E';
	m_mapHighlight["log"] = 'E';
	m_mapHighlight["proj"] = 'E';
	m_mapHighlight["rej"] = 'E';

	m_mapHighlight["cp"] = 'E';
	m_mapHighlight["acp"] = 'E';

	m_mapHighlight["VecE3"] = 'E';
	m_mapHighlight["VecP3"] = 'E';
	m_mapHighlight["VecN3"] = 'E';
	m_mapHighlight["VecC2"] = 'E';

	m_mapHighlight["DegConic"] = 'E';
	m_mapHighlight["FindAllDegConics"] = 'E';

	m_mapHighlight["RotorC2"] = 'E';
	m_mapHighlight["ReflectorC2"] = 'E';

	m_mapHighlight["RotorE3"] = 'E';
	m_mapHighlight["RotorP3"] = 'E';
	m_mapHighlight["RotorN3"] = 'E';

	m_mapHighlight["SphereN3"] = 'E';
	m_mapHighlight["TranslatorN3"] = 'E';
	m_mapHighlight["DilatorN3"] = 'E';
	m_mapHighlight["DirVecN3"] = 'E';

	m_mapHighlight["DefVarsE3"] = 'E';
	m_mapHighlight["DefVarsP3"] = 'E';
	m_mapHighlight["DefVarsN3"] = 'E';
	m_mapHighlight["DefVarsC2"] = 'E';

	m_mapHighlight["DirVecP3"] = 'E';

	m_mapHighlight["AffineVecP3"] = 'E';

	m_mapHighlight["AnalyzeMV"] = 'E';

	m_mapHighlight["SetVisWinSize"] = 'E';

	m_mapHighlight["SetMode"] = 'E';
	m_mapHighlight["SetMouseMode"] = 'E';
	m_mapHighlight["GetMouseMode"] = 'E';
	m_mapHighlight["SetMouseValue"] = 'E';
	m_mapHighlight["GetMouseValue"] = 'E';
	m_mapHighlight["SetMouseSens"] = 'E';
	m_mapHighlight["GetMouseSens"] = 'E';
	
	m_mapHighlight["Offset"] = 'E';
	m_mapHighlight["FactorizeBlade"] = 'E';

	m_mapHighlight["Matrix"] = 'E';
	m_mapHighlight["IdMatrix"] = 'E';
	m_mapHighlight["ResizeMatrix"] = 'E';
	m_mapHighlight["ReshapeMatrix"] = 'E';
	m_mapHighlight["AddRows"] = 'E';
	m_mapHighlight["AddCols"] = 'E';
	m_mapHighlight["SVD"] = 'E';
	m_mapHighlight["EigenValues"] = 'E';
	m_mapHighlight["Eigen"] = 'E';
	m_mapHighlight["det"] = 'E';
	m_mapHighlight["Sym2Col"] = 'E';
	m_mapHighlight["Col2Sym"] = 'E';

	m_mapHighlight["MV2Mask"] = 'E';
	m_mapHighlight["GetMVProductMatrix"] = 'E';
	m_mapHighlight["MV2Matrix"] = 'E';
	m_mapHighlight["Matrix2MV"] = 'E';
	m_mapHighlight["Diag2Row"] = 'E';
	m_mapHighlight["Row2Diag"] = 'E';
	m_mapHighlight["DiagToRow"] = 'E';
	m_mapHighlight["RowToDiag"] = 'E';
	m_mapHighlight["RowSpace"] = 'E';
	m_mapHighlight["FindMainNRows"] = 'E';
	m_mapHighlight["GetRows"] = 'E';

	m_mapHighlight["Tensor"] = 'E';
	m_mapHighlight["MV2Tensor"] = 'E';
	m_mapHighlight["Tensor2MV"] = 'E';
	m_mapHighlight["GAOpTensor"] = 'E';

	m_mapHighlight["BladeIdxList"] = 'E';
	m_mapHighlight["GradeList"] = 'E';
	m_mapHighlight["Len"] = 'E';
	m_mapHighlight["Size"] = 'E';
	m_mapHighlight["Str"] = 'E';
	m_mapHighlight["String"] = 'E';
	m_mapHighlight["Scalar"] = 'E';
	m_mapHighlight["Latex"] = 'E';
	m_mapHighlight["Type"] = 'E';
	m_mapHighlight["argtrue"] = 'E';
	m_mapHighlight["sort"] = 'E';

	m_mapHighlight["SetTextPrec"] = 'E';
	m_mapHighlight["SetEvalPrec"] = 'E';
	m_mapHighlight["SetVisPrec"] = 'E';

	// List Functions
	m_mapHighlight["List"] = 'E';
	m_mapHighlight["SubList"] = 'E';
	m_mapHighlight["RemList"] = 'E';
	m_mapHighlight["InsList"] = 'E';
	m_mapHighlight["TransList"] = 'E';
	m_mapHighlight["CombIdxList"] = 'E';
	m_mapHighlight["PermIdxList"] = 'E';

	m_mapHighlight["Plot"] = 'E';
	m_mapHighlight["SetPlotMode"] = 'E';

	//m_mapHighlight["Repeat"] = 'E';

	m_mapHighlight["Color"] = 'E';

	m_mapHighlight["SetArrowShape"] = 'E';

	m_mapHighlight["DrawPlane"] = 'E';
	m_mapHighlight["DrawBox"] = 'E';
	m_mapHighlight["DrawLine"] = 'E';
	m_mapHighlight["DrawLineSurface"] = 'E';
	m_mapHighlight["DrawPoint"] = 'E';
	m_mapHighlight["DrawPointSurface"] = 'E';
	m_mapHighlight["DrawPointGrid"] = 'E';
	m_mapHighlight["DrawPointList"] = 'E';
	m_mapHighlight["DrawArrow"] = 'E';
	m_mapHighlight["DrawArc"] = 'E';
	m_mapHighlight["DrawDisk"] = 'E';
	m_mapHighlight["DrawEllipse"] = 'E';
	m_mapHighlight["DrawEllipsoid"] = 'E';
	m_mapHighlight["DrawCircle"] = 'E';
	m_mapHighlight["DrawCircleSurface"] = 'E';
	m_mapHighlight["DrawTriangle"] = 'E';
	m_mapHighlight["DrawCone"] = 'E';
	m_mapHighlight["DrawSphere"] = 'E';
	m_mapHighlight["DrawCylinder"] = 'E';
	m_mapHighlight["DrawIcosahedron"] = 'E';
	m_mapHighlight["DrawQuantumFractal"] = 'E';

	m_mapHighlight["EnableFog"] = 'E';
	m_mapHighlight["SetFogDensity"] = 'E';
	m_mapHighlight["SetFogColor"] = 'E';

	m_mapHighlight["DrawLatex"] = 'E';
	m_mapHighlight["GetLatexImg"] = 'E';
	m_mapHighlight["DrawImg"] = 'E';
	m_mapHighlight["ReadImg"] = 'E';
	m_mapHighlight["WriteImg"] = 'E';
	m_mapHighlight["SetTexture"] = 'E';
	m_mapHighlight["SetTextureRepeat"] = 'E';
	m_mapHighlight["EnableAbsTexCoords"] = 'E';

	m_mapHighlight["DrawText"] = 'E';
	m_mapHighlight["SetTextSize"] = 'E';

	//m_mapHighlight["SetFont"] = 'E';
	//m_mapHighlight["SetFontScale"] = 'E';

	m_mapHighlight["SetLatexMagStep"] = 'E';
	m_mapHighlight["EnableRenderLatex"] = 'E';
	m_mapHighlight["SetImgAlign"] = 'E';
	m_mapHighlight["SetImgPos"] = 'E';
	m_mapHighlight["SetLatexAlign"] = 'E';
	m_mapHighlight["EnableImgTransparency"] = 'E';
	m_mapHighlight["SetImgTransparentColor"] = 'E';
	m_mapHighlight["SetImgScale"] = 'E';
	m_mapHighlight["GetImgCoords"] = 'E';
	m_mapHighlight["Img2Matrix"] = 'E';
	m_mapHighlight["Matrix2Img"] = 'E';
	//m_mapHighlight["RotateImg"] = 'E';
	m_mapHighlight["CopyImg"] = 'E';
	m_mapHighlight["ResizeImg"] = 'E';
	m_mapHighlight["ReshapeImg"] = 'E';
	m_mapHighlight["InsertImg"] = 'E';
	m_mapHighlight["Image"] = 'E';

	m_mapHighlight["StartOverlay"] = 'E';
	m_mapHighlight["EndOverlay"] = 'E';

	m_mapHighlight["StartView"] = 'E';
	m_mapHighlight["EndView"] = 'E';

	m_mapHighlight["PushFrame"] = 'E';
	m_mapHighlight["PopFrame"] = 'E';

	m_mapHighlight["SetPointSize"] = 'E';
	m_mapHighlight["GetPointSize"] = 'E';
	m_mapHighlight["SetLineWidth"] = 'E';
	m_mapHighlight["GetLineWidth"] = 'E';

	m_mapHighlight["EnableSmoothPoint"] = 'E';
	m_mapHighlight["IsSmoothPoint"] = 'E';
	m_mapHighlight["EnableSmoothLine"] = 'E';
	m_mapHighlight["IsSmoothLine"] = 'E';
	m_mapHighlight["EnableSmoothPolygon"] = 'E';
	m_mapHighlight["IsSmoothPolygon"] = 'E';


	m_mapHighlight["SetFrameOrigin"] = 'E';
	m_mapHighlight["SetFrameRotor"] = 'E';
	m_mapHighlight["AddFrameOrigin"] = 'E';
	m_mapHighlight["AddFrameRotor"] = 'E';
	m_mapHighlight["TranslateFrame"] = 'E';
	m_mapHighlight["RotateFrame"] = 'E';
	m_mapHighlight["TranslateView"] = 'E';
	m_mapHighlight["RotateView"] = 'E';
	m_mapHighlight["SetViewAngle"] = 'E';

	m_mapHighlight["_ClearScriptList"] = 'E';
	m_mapHighlight["_AddScriptToList"] = 'E';

	m_mapHighlight["EPOp"] = 'E';
	m_mapHighlight["EPVecN3"] = 'E';
	m_mapHighlight["EPCCVecN3"] = 'E';
	m_mapHighlight["EPPrjVecN3ToE3"] = 'E';

	m_mapHighlight["EPVecC2"] = 'E';

	m_mapHighlight["EPsqrt"] = 'E';
	m_mapHighlight["EPinv"] = 'E';
	m_mapHighlight["EPmul"] = 'E';

	m_mapHighlight["ReadVar"] = 'E';
	m_mapHighlight["WriteVar"] = 'E';
	m_mapHighlight["ReadXML"] = 'E';
	m_mapHighlight["ReadText"] = 'E';
	m_mapHighlight["WriteText"] = 'E';
	m_mapHighlight["ReadData"] = 'E';
	m_mapHighlight["WriteData"] = 'E';
	m_mapHighlight["ReadMatrix"] = 'E';
	m_mapHighlight["ShowFile"] = 'E';
	m_mapHighlight["FileChooser"] = 'E';
	m_mapHighlight["SaveScreen"] = 'E';

	m_mapHighlight["ResetLight"] = 'E';
	m_mapHighlight["SetAmbientLight"] = 'E';
	m_mapHighlight["EnableLight"] = 'E';
	m_mapHighlight["EnableLightFixed"] = 'E';
	m_mapHighlight["SetLightPos"] = 'E';
	m_mapHighlight["SetLightAmbientCol"] = 'E';
	m_mapHighlight["SetLightDiffuseCol"] = 'E';
	m_mapHighlight["SetLightSpecularCol"] = 'E';

	m_mapHighlight["SetLightSpotDir"] = 'E';
	m_mapHighlight["SetLightSpotExp"] = 'E';
	m_mapHighlight["SetLightSpotCutOff"] = 'E';
	m_mapHighlight["SetLightAttenuation"] = 'E';

	m_mapHighlight["EnableAnimate"] = 'E';
	m_mapHighlight["SetAnimateTimeStep"] = 'E';
	m_mapHighlight["GetTime"] = 'E';

	m_mapHighlight["Slider"] = 'E';
	m_mapHighlight["Stepper"] = 'E';
	m_mapHighlight["Input"] = 'E';
	m_mapHighlight["InputText"] = 'E';
	m_mapHighlight["CheckBox"] = 'E';
	m_mapHighlight["Button"] = 'E';
	m_mapHighlight["Choice"] = 'E';

	m_mapHighlight["SetInfoText"] = 'E';
	m_mapHighlight["SetInfoWidth"] = 'E';

	m_mapHighlight["OpenSP"] = 'E';
	m_mapHighlight["WriteSP"] = 'E';
	m_mapHighlight["ReadSP"] = 'E';


	m_mapHighlight["Red"] = 'F';
	m_mapHighlight["Green"] = 'F';
	m_mapHighlight["Blue"] = 'F';

	m_mapHighlight["Cyan"] = 'F';
	m_mapHighlight["Yellow"] = 'F';
	m_mapHighlight["Magenta"] = 'F';

	m_mapHighlight["Orange"] = 'F';
	m_mapHighlight["White"] = 'F';
	m_mapHighlight["Black"] = 'F';

	m_mapHighlight["MRed"] = 'F';
	m_mapHighlight["MGreen"] = 'F';
	m_mapHighlight["MBlue"] = 'F';

	m_mapHighlight["MCyan"] = 'F';
	m_mapHighlight["MYellow"] = 'F';
	m_mapHighlight["MMagenta"] = 'F';

	m_mapHighlight["MOrange"] = 'F';
	m_mapHighlight["MGray"] = 'F';
	m_mapHighlight["MWhite"] = 'F';

	m_mapHighlight["DRed"] = 'F';
	m_mapHighlight["DGreen"] = 'F';
	m_mapHighlight["DBlue"] = 'F';

	m_mapHighlight["DCyan"] = 'F';
	m_mapHighlight["DYellow"] = 'F';
	m_mapHighlight["DMagenta"] = 'F';

	m_mapHighlight["DOrange"] = 'F';
	m_mapHighlight["DGray"] = 'F';
	m_mapHighlight["DWhite"] = 'F';

	m_mapHighlight["Alpha"] = 'F';
	m_mapHighlight["MAlpha"] = 'F';
	m_mapHighlight["DAlpha"] = 'F';

	m_mapHighlight["OPNS"] = 'F';
	m_mapHighlight["IPNS"] = 'F';
	
	m_mapHighlight["E3_DRAW_VEC_AS_LINE"] = 'F';
	m_mapHighlight["E3_DRAW_VEC_AS_ARROW"] = 'F';
	m_mapHighlight["E3_DRAW_VEC_AS_POINT"] = 'F';

	m_mapHighlight["E3_DRAW_PLANE_AS_RECT"] = 'F';
	m_mapHighlight["E3_DRAW_PLANE_AS_DISK"] = 'F';

	m_mapHighlight["N3_OPNS"] = 'F';
	m_mapHighlight["N3_IPNS"] = 'F';
	m_mapHighlight["N3_SOLID"] = 'F';
	m_mapHighlight["N3_WIRE"] = 'F';
	m_mapHighlight["N3_SHOW_IMAG"] = 'F';
	m_mapHighlight["N3_HIDE_IMAG"] = 'F';
	m_mapHighlight["DRAW_POINT_AS_DOT"] = 'F';
	m_mapHighlight["DRAW_POINT_AS_SPHERE"] = 'F';

	// Plot Modes
	m_mapHighlight["POINTS"] = 'F';
	m_mapHighlight["CONNECTED"] = 'F';
	
	// Fonts
/*
	m_mapHighlight["FONT_FixedSys_Std_9"] = 'F';
	m_mapHighlight["FONT_FixedSys_Bold_9"] = 'F';
	m_mapHighlight["FONT_Times_Std_12"] = 'F';
	m_mapHighlight["FONT_Sylfaen_Bold_24"] = 'F';
	m_mapHighlight["FONT_Symbol_Std_12"] = 'F';
	m_mapHighlight["FONT_Symbol_Bold_12"] = 'F';
	m_mapHighlight["FONT_Symbol_Std_24"] = 'F';
	m_mapHighlight["FONT_T2_Std_12"] = 'F';
	m_mapHighlight["FONT_T2_Std_24"] = 'F';
*/
	m_mapHighlight["true"] = 'F';
	m_mapHighlight["false"] = 'F';

	m_mapHighlight["ToolName"] = 'F';
	m_mapHighlight["LinkData"] = 'F';
	m_mapHighlight["ExecMode"] = 'F';
	m_mapHighlight["EM_NEW"] = 'F';
	m_mapHighlight["EM_CHANGE"] = 'F';
	m_mapHighlight["EM_ANIMATE"] = 'F';
	m_mapHighlight["EM_USER"] = 'F';
	m_mapHighlight["EM_TOOL"] = 'F';
	m_mapHighlight["EM_LINK"] = 'F';
	m_mapHighlight["EM_AGAIN"] = 'F';
	m_mapHighlight["EM_KEYUP"] = 'F';
	m_mapHighlight["EM_KEYDOWN"] = 'F';
	m_mapHighlight["EM_RESHAPE"] = 'F';

	m_mapHighlight["MVOP_GEO"] = 'F';
	m_mapHighlight["MVOP_INNER"] = 'F';
	m_mapHighlight["MVOP_OUTER"] = 'F';
	m_mapHighlight["MVOP_ADD"] = 'F';
	m_mapHighlight["MVOP_SUB"] = 'F';
	m_mapHighlight["MVOP_DUAL"] = 'F';
	m_mapHighlight["MVOP_REVERSE"] = 'F';
	m_mapHighlight["MVOP_INVOLUTE"] = 'F';

	m_mapHighlight["GA_E3"] = 'F';
	m_mapHighlight["GA_P3"] = 'F';
	m_mapHighlight["GA_N3"] = 'F';
	m_mapHighlight["GA_C2"] = 'F';

	m_mapHighlight["dTime"] = 'F';
	m_mapHighlight["Time"] = 'F';
	m_mapHighlight["Pi"] = 'F';
	m_mapHighlight["RadPerDeg"] = 'F';

	m_mapHighlight["VERSION"] = 'F';

	m_mapHighlight["_DoAnimate"] = 'F';
	m_mapHighlight["_BGColor"] = 'F';
	m_mapHighlight["_FrameBoxSize"] = 'F';
	m_mapHighlight["_2dView"] = 'F';
	
	m_mapHighlight["if"] = 'I';
	m_mapHighlight["else"] = 'I';
	m_mapHighlight["loop"] = 'I';
	m_mapHighlight["break"] = 'I';

	m_mapSymbol[">"] = 'I';
	m_mapSymbol["<"] = 'I';
	m_mapSymbol[">="] = 'I';
	m_mapSymbol["<="] = 'I';
	m_mapSymbol["=="] = 'I';
	m_mapSymbol["!="] = 'I';
	m_mapSymbol["&&"] = 'I';
	m_mapSymbol["||"] = 'I';
	m_mapSymbol["!!"] = 'I';

	m_mapSymbol[".>"] = 'I';
	m_mapSymbol[".>="] = 'I';
	m_mapSymbol[".<"] = 'I';
	m_mapSymbol[".<="] = 'I';
	m_mapSymbol[".=="] = 'I';
	m_mapSymbol[".!="] = 'I';
	m_mapSymbol[".&&"] = 'I';
	m_mapSymbol[".||"] = 'I';

	m_mapSymbol["::"] = 'G';
	m_mapSymbol["->"] = 'G';
	m_mapSymbol["<<"] = 'G';
	m_mapSymbol[".<<"] = 'G';
	m_mapSymbol["./"] = 'G';
	m_mapSymbol[".*"] = 'G';
	m_mapSymbol[".."] = 'G';
	m_mapSymbol[".^"] = 'G';
	m_mapSymbol[".-"] = 'G';
	m_mapSymbol[".+"] = 'G';
	m_mapSymbol["^^"] = 'G';

	m_sStdSingleCharOps = "-+/={}[]";
	m_sSpcSingleCharOps = ":.*~!%&|?^°'";

	m_sAllowedChars =	"_abcdefghijklmnopqrstuvwxyzäöüß"
						"ABCDEFGHIJKLMNOPQRSTUVWXYZÄÖÜ";

	m_sSymbolChars =	"=!-+/:.*~#!%&|?^{}°<>[]'";
}

//////////////////////////////////////////////////////////////////////
/// InitMenu

void CCLUScriptEditor::InitMenu()
{
	CFLTKEditor::InitMenu();

	Fl_Menu_Item pMenuItems[] = 
	{
		{ "&Code",              0, 0, 0, FL_SUBMENU },
		{ "&Parse",        FL_CTRL + 'p', (Fl_Callback *) CB_Parse },
		{ "Parse (Force Render Late&x)",  FL_CTRL + FL_SHIFT + 'p', (Fl_Callback *) CB_ParseForce },
		{ "Parse Main",  FL_CTRL + 'm', (Fl_Callback *) CB_ParseMain },
		{ "Parse Main (Force Render Latex)",  FL_CTRL + FL_SHIFT + 'm', (Fl_Callback *) CB_ParseMainForce, 0, FL_MENU_DIVIDER },
		{ "Insert Co&lor",        FL_CTRL + 'l', (Fl_Callback *) CB_InsertCode, (void *) CSE_IC_COLOR },
		{ 0 },

		{ "&Presentation", 0, 0, 0, FL_SUBMENU },
		{ "Ne&xt",        0, (Fl_Callback *) CB_NextScript },
		{ "Pre&vious",    0, (Fl_Callback *) CB_PrevScript, 0, FL_MENU_DIVIDER },
		//{ "&Reload Current",    0, (Fl_Callback *) CB_ReloadScript, 0, FL_MENU_DIVIDER },
		{ "&Show List...",    0, (Fl_Callback *) CB_ShowScriptList },
		{ "&Clear List",    0, (Fl_Callback *) CB_ClearScriptList },
		{ 0 },

		{ "&Window",              0, 0, 0, FL_SUBMENU },
		{ "&Editor", FL_CTRL + FL_SHIFT + 'e', (Fl_Callback *) CB_ShowEditorWin },
		{ "&Output", FL_CTRL + FL_SHIFT + 'o', (Fl_Callback *) CB_ShowOutputWin },
		{ "&Visualization", FL_CTRL + FL_SHIFT + 'v', (Fl_Callback *) CB_ShowVisWin },
		{ 0 },

		{ "&Help",              0, 0, 0, FL_SUBMENU },
		{ "C&ontent", FL_CTRL + 'q', (Fl_Callback *) CB_Help },
		{ "About", 0, (Fl_Callback *) CB_About },
		{ 0 },
		{ 0 }
	};

	Fl_Menu_Item pCMenuItems[] = 
	{
		{ "  Multivector Interpretation  ", 0, 0, 0, FL_SUBMENU },
			{ CSE_IC_OPNS, 0, (Fl_Callback *) CB_InsertCode },
			{ CSE_IC_IPNS, 0, (Fl_Callback *) CB_InsertCode },
			{ 0 },

		{ "  Visualization Styles  ", 0, 0, 0, FL_SUBMENU },
			{ "  Points  ", 0, 0, 0, FL_SUBMENU },
				{ CSE_IC_VS_POINT_DOT, 0, (Fl_Callback *) CB_InsertCode },
				{ CSE_IC_VS_POINT_SPHERE, 0, (Fl_Callback *) CB_InsertCode },
				{ 0 },
			{ "  3D Euclidean Vectors  ", 0, 0, 0, FL_SUBMENU },
				{ CSE_IC_VS_E3_VEC_LINE, 0, (Fl_Callback *) CB_InsertCode },
				{ CSE_IC_VS_E3_VEC_ARROW, 0, (Fl_Callback *) CB_InsertCode },
				{ CSE_IC_VS_E3_VEC_POINT, 0, (Fl_Callback *) CB_InsertCode },
				{ 0 },
			{ "  3D Euclidean Planes  ", 0, 0, 0, FL_SUBMENU },
				{ CSE_IC_VS_E3_PLANE_RECT, 0, (Fl_Callback *) CB_InsertCode },
				{ CSE_IC_VS_E3_PLANE_DISK, 0, (Fl_Callback *) CB_InsertCode },
				{ 0 },
			{ "  3D Conformal Spheres  ", 0, 0, 0, FL_SUBMENU },
				{ CSE_IC_VS_N3_SOLID, 0, (Fl_Callback *) CB_InsertCode },
				{ CSE_IC_VS_N3_WIRE, 0, (Fl_Callback *) CB_InsertCode },
				{ 0 },
			{ "  3D Conformal Imaginary  ", 0, 0, 0, FL_SUBMENU },
				{ CSE_IC_VS_N3_IMAG, 0, (Fl_Callback *) CB_InsertCode },
				{ CSE_IC_VS_N3_NO_IMAG, 0, (Fl_Callback *) CB_InsertCode },
				{ 0 },
			{ 0 },

		{ "  Define Variables  ", 0, 0, 0, FL_SUBMENU },
			{ CSE_IC_E3, 0, (Fl_Callback *) CB_InsertCode },
			{ CSE_IC_P3, 0, (Fl_Callback *) CB_InsertCode },
			{ CSE_IC_N3, 0, (Fl_Callback *) CB_InsertCode },
			{ CSE_IC_C2, 0, (Fl_Callback *) CB_InsertCode },
			{ 0 },

		{ "  Vectors  ", 0, 0, 0, FL_SUBMENU },
			{ CSE_IC_VE3, 0, (Fl_Callback *) CB_InsertCode },
			{ CSE_IC_VP3, 0, (Fl_Callback *) CB_InsertCode },
			{ CSE_IC_VN3, 0, (Fl_Callback *) CB_InsertCode },
			{ CSE_IC_VC2, 0, (Fl_Callback *) CB_InsertCode },
			{ 0 },

		{ "  Color  ", 0, 0, 0, FL_SUBMENU },
			{ CSE_IC_COLOR, 0, (Fl_Callback *) CB_InsertCode },
			{ CSE_IC_RED, 0, (Fl_Callback *) CB_InsertCode },
			{ CSE_IC_GREEN, 0, (Fl_Callback *) CB_InsertCode },
			{ CSE_IC_BLUE, 0, (Fl_Callback *) CB_InsertCode },
			{ CSE_IC_MAGENTA, 0, (Fl_Callback *) CB_InsertCode },
			{ CSE_IC_CYAN, 0, (Fl_Callback *) CB_InsertCode },
			{ CSE_IC_YELLOW, 0, (Fl_Callback *) CB_InsertCode },
			{ CSE_IC_ORANGE, 0, (Fl_Callback *) CB_InsertCode },
			{ CSE_IC_BLACK, 0, (Fl_Callback *) CB_InsertCode },
			{ CSE_IC_WHITE, 0, (Fl_Callback *) CB_InsertCode },
			{ 0 },

		{ "  Execution Modes  ", 0, 0, 0, FL_SUBMENU },
			{ CSE_IC_EM_NEW, 0, (Fl_Callback *) CB_InsertCode },
			{ CSE_IC_EM_CHANGE, 0, (Fl_Callback *) CB_InsertCode },
			{ CSE_IC_EM_ANIMATE, 0, (Fl_Callback *) CB_InsertCode },
			{ CSE_IC_EM_USER, 0, (Fl_Callback *) CB_InsertCode },
			{ CSE_IC_EM_TOOL, 0, (Fl_Callback *) CB_InsertCode },
			{ CSE_IC_EM_KEYUP, 0, (Fl_Callback *) CB_InsertCode },
			{ CSE_IC_EM_KEYDOWN, 0, (Fl_Callback *) CB_InsertCode },
			{ CSE_IC_EM_RESHAPE, 0, (Fl_Callback *) CB_InsertCode },
			{ CSE_IC_EM_LINK, 0, (Fl_Callback *) CB_InsertCode },
			{ 0 },

		{ "  Multivector Operation IDs  ", 0, 0, 0, FL_SUBMENU },
			{ CSE_IC_MVOP_GEO, 0, (Fl_Callback *) CB_InsertCode },
			{ CSE_IC_MVOP_INNER, 0, (Fl_Callback *) CB_InsertCode },
			{ CSE_IC_MVOP_OUTER, 0, (Fl_Callback *) CB_InsertCode },
			{ CSE_IC_MVOP_ADD, 0, (Fl_Callback *) CB_InsertCode },
			{ CSE_IC_MVOP_SUB, 0, (Fl_Callback *) CB_InsertCode },
			{ CSE_IC_MVOP_DUAL, 0, (Fl_Callback *) CB_InsertCode },
			{ CSE_IC_MVOP_REVERSE, 0, (Fl_Callback *) CB_InsertCode },
			{ CSE_IC_MVOP_INVOLUTE, 0, (Fl_Callback *) CB_InsertCode },
			{ 0 },

		{ "  Vector Space IDs  ", 0, 0, 0, FL_SUBMENU },
			{ CSE_IC_GA_E3, 0, (Fl_Callback *) CB_InsertCode },
			{ CSE_IC_GA_P3, 0, (Fl_Callback *) CB_InsertCode },
			{ CSE_IC_GA_N3, 0, (Fl_Callback *) CB_InsertCode },
			{ CSE_IC_GA_C2, 0, (Fl_Callback *) CB_InsertCode },
			{ 0 },

		{ "  Special  ", 0, 0, 0, FL_SUBMENU },
			{ CSE_IC_LOOP1, 0, (Fl_Callback *) CB_InsertCode },
			{ CSE_IC_LOOP2, 0, (Fl_Callback *) CB_InsertCode },
			{ 0 },

		{ 0 }
	};

	Fl_Menu_Item pItem[] = 
	{
		{ "&Read File (no exec.)...",    FL_CTRL + 'r', (Fl_Callback *) CB_LoadNoExec }
	};

	m_vecMenuItem.insert( m_vecMenuItem.begin() + 3, *pItem );

	int iCount = m_vecMenuItem.size();

	m_vecMenuItem.resize(iCount + 22);

	for( int i = 0; i < 23; i++ )
	{
		m_vecMenuItem[iCount + i - 1] = pMenuItems[i];
	}

	// Context Menu
	m_vecCMenuItem.resize(83);
	for( int i = 0; i < 83; i++ )
	{
		m_vecCMenuItem[i] = pCMenuItems[i];
	}
}


void CCLUScriptEditor::InitStyleTable()
{
	//CFLTKEditor::InitStyleTable();

	Fl_Text_Display::Style_Table_Entry pStyleTable[] = 
	{	// Style table
		 { FL_BLACK,      FL_COURIER,      14 }, // A - Plain
	     { FL_DARK_GREEN, FL_COURIER_ITALIC, 14 }, // B - Line comments
	     { FL_DARK_GREEN, FL_COURIER_ITALIC, 14 }, // C - Block comments
	     { FL_DARK_CYAN,  FL_COURIER,        14 }, // D - Strings
	     { FL_BLUE,		  FL_COURIER_BOLD,   14 }, // E - Functions
	     { FL_DARK_BLUE,  FL_COURIER_BOLD,   14 }, // F - Predefined Variables
	     { FL_DARK_RED,   FL_COURIER_BOLD,   14 }, // G - Std. Operators
	     { FL_DARK_RED,   FL_COURIER_BOLD,   18 }, // H - Special Operators
	     { FL_RED,        FL_COURIER_BOLD,   14 }, // I - Keywords
	     { FL_DARK_GREEN, FL_COURIER_BOLD,   14 }  // J - Preprocessor directives
	};

	m_vecStyleTable.clear();
	m_vecStyleTable.resize(10);

	for( int i = 0; i < 10; i++ )
	{
		m_vecStyleTable[i] = pStyleTable[i];
	}
}


void CCLUScriptEditor::StyleParse(const char *pcText, char *pcStyle, int iLength)
{
	char	 pcBuf[256];
	bool bInBlockComment = false;
	int iPos;

	if (!m_bReparsing)
	{
		for( iPos = 0; iPos < iLength; iPos++ )
		{
			if (pcStyle[iPos] == 'C' || pcStyle[iPos] == 'D')
			{
 				m_bReparseAll = true;
				return;
			}
		}
	}

	for (; iLength > 0; 
		iLength --, pcText++) 
	{
		// Check for directives, comments, strings, and keywords...
		if (strncmp(pcText, "//#", 3) == 0) 
		{
			while (iLength > 0 && *pcText != '\n')
			{
				*pcStyle = 'J';
				pcStyle++;
				iLength--;
				pcText++;
			}

			if (iLength == 0) 
				break;

			*pcStyle++ = 'A';

			continue;
		} 
		else if (strncmp(pcText, "//", 2) == 0) 
		{
			while (iLength > 0 && *pcText != '\n')
			{
				*pcStyle = 'B';
				pcStyle++;
				iLength--;
				pcText++;
			}

			if (iLength == 0) 
				break;

			*pcStyle++ = 'A';

			continue;
		} 
		else if (strncmp(pcText, "/*", 2) == 0) 
		{
			*pcStyle++ = 'C';
			*pcStyle++ = 'C';
			iLength -= 2;
			pcText += 2;

			if (iLength == 0)
			{
				m_bReparseAll = true;
				break;
			}

			while (iLength > 0 && strncmp(pcText, "*/", 2) != 0)
			{
				if (iLength == 1)
					m_bReparseAll = true;

				*pcStyle = 'C';

				pcStyle++;
				iLength--;
				pcText++;
			}

			if (iLength > 0 && strncmp(pcText, "*/", 2) == 0)
			{
				if (iLength == 2)
					m_bReparseAll = true;

				*pcStyle++ = 'C';
				*pcStyle++ = 'C';
				iLength--;
				pcText++;
			}

			if (iLength == 0) 
				break;

			continue;
		}
		else if (strncmp(pcText, "\\\"", 2) == 0) 
		{
			// Quoted quote...
			*pcStyle++ = 'A';
			*pcStyle++ = 'A';
			pcText++;
			iLength--;
			continue;
		} 
		else if (strncmp(pcText, "\\\\", 2) == 0) 
		{
			// Backslash
			*pcStyle++ = 'A';
			*pcStyle++ = 'A';
			pcText++;
			iLength--;
			continue;
		} 
		else if (*pcText == '\"') 
		{
			*pcStyle++ = 'D';
			pcText++;
			iLength--;

			if (iLength == 0)
			{
				m_bReparseAll = true;
				break;
			}

			// Continuing in string...
			while (iLength > 0)
			{
				if (iLength == 1)
					m_bReparseAll = true;

				if (strncmp(pcText, "\\\"", 2) == 0) 
				{
					// Quoted end quote...
					*pcStyle++ = 'D';
					pcText++;
					iLength--;
				} 
				else if (strncmp(pcText, "\\\\", 2) == 0) 
				{
					// Backslash
					*pcStyle++ = 'D';
					pcText++;
					iLength--;
				} 
				else if (*pcText == '\"') 
				{
					// End quote...
					*pcStyle++ = 'D';
					break;
				}
/*				
				else if (*pcText == '\n')
				{
					*pcStyle++ = 'A';
					break;
				}
*/
				*pcStyle++ = 'D';
				pcText++;
				iLength--;
			}

			if (iLength == 0) 
				break;

			continue;
		} 
		else if (strchr(m_sAllowedChars.c_str(), *pcText) != 0)
		{
			// Might be a keyword...
			map<string, char>::const_iterator itStyle;
			int iCurPos = 0;
			char cCurrent;

			do
			{
				pcBuf[iCurPos] = pcText[iCurPos];
				iCurPos++;
				iLength--;
			} while (( isdigit((unsigned char)pcText[iCurPos]) || 
				strchr(m_sAllowedChars.c_str(), pcText[iCurPos]) != 0 )
				&& iLength > 0 && iCurPos < 255);

			pcBuf[iCurPos] = 0;
			itStyle = m_mapHighlight.find(pcBuf);

			if (itStyle != m_mapHighlight.end())
			{
				cCurrent = itStyle->second;
			}
			else
			{
				cCurrent = 'A';
			}

			for( int i = 0; i < iCurPos; i++ )
			{
				*pcStyle++ = cCurrent;
				pcText++;
			}

			if (iLength == 0) 
				break;

			pcText--;
			iLength++;

			continue;
		}
		else if (strchr(m_sSymbolChars.c_str(), *pcText) != 0)
		{
			// Might be an extended Symbol
			map<string, char>::const_iterator itStyle;
			char cCurrent;

			if (pcText[1] != 0 && pcText[2] != 0)
			{
				pcBuf[0] = pcText[0];
				pcBuf[1] = pcText[1];
				pcBuf[2] = pcText[2];
				pcBuf[3] = 0;

				itStyle = m_mapSymbol.find(pcBuf);

				if (itStyle != m_mapSymbol.end())
				{
					cCurrent = itStyle->second;
					iLength -= 3;

					for( int i = 0; i < 3; i++ )
					{
						*pcStyle++ = cCurrent;
						pcText++;
					}

					if (iLength == 0) 
						break;

					pcText--;
					iLength++;

					continue;
				}
			}
			
			if (pcText[1] != 0)
			{
				pcBuf[0] = pcText[0];
				pcBuf[1] = pcText[1];
				pcBuf[2] = 0;

				itStyle = m_mapSymbol.find(pcBuf);

				if (itStyle != m_mapSymbol.end())
				{
					cCurrent = itStyle->second;
					iLength -= 2;

					for( int i = 0; i < 2; i++ )
					{
						*pcStyle++ = cCurrent;
						pcText++;
					}

					if (iLength == 0) 
						break;

					pcText--;
					iLength++;

					continue;
				}
			}

			pcBuf[0] = pcText[0];
			pcBuf[1] = 0;

			itStyle = m_mapSymbol.find(pcBuf);

			if (itStyle != m_mapSymbol.end())
			{
				cCurrent = itStyle->second;
				*pcStyle++ = cCurrent;

				if (iLength == 1) 
					break;

				continue;
			}

			if (strchr(m_sStdSingleCharOps.c_str(), *pcText) != 0)
			{
				*pcStyle++ = 'G';
				continue;
			}
			else if (strchr(m_sSpcSingleCharOps.c_str(), *pcText) != 0)
			{
				if (*pcText == '.' && iLength > 1 && !isdigit(pcText[1]))
				{
					*pcStyle++ = 'H';
					continue;
				}
				else if (*pcText != '.')
				{
					*pcStyle++ = 'H';
					continue;
				}
			}
		}

		// Copy pcStyle info...
		*pcStyle++ = 'A';
	}
}



void CCLUScriptEditor::New()
{
	CFLTKEditor::New();

	if (m_bParseAfterNew)
		Parse(true);
}

void CCLUScriptEditor::Load()
{
	CFLTKEditor::Load();

	Parse(true);
}

void CCLUScriptEditor::LoadNoExec()
{
	m_bParseAfterNew = false;
	CFLTKEditor::Load();
	m_bParseAfterNew = true;
}

void CCLUScriptEditor::Insert()
{
	CFLTKEditor::Insert();

	Parse(true);
}

void CCLUScriptEditor::Close()
{
	CFLTKEditor::Close();

	Parse(true);
}

void CCLUScriptEditor::SaveAs()
{
	CFLTKEditor::SaveAs();

	Parse(true);
}


//////////////////////////////////////////////////////////////////////
/// Help Callback

void CCLUScriptEditor::Help()
{
# ifdef WIN32
	string sFile;

	sFile = m_sExecPath + "\\Documentation\\index.html";

	HINSTANCE hInstance = ShellExecute(NULL,
		"open",
		sFile.c_str(),
		NULL,
		NULL,
		SW_SHOWNORMAL);//SHOWMINIMIZED);
/*
	if ((uint) hInstance <= 32)
		fl_alert("Cannot start web-browser to view help manual.");
*/
# else // Linux or other system
	char pcCmd[500];
	char pcLine[300];
	struct stat FileStat;
	ifstream zLog;
	char *pBrowser[] = { "firefox", "konqueror", "mozilla", "netscape" };
	int iPos, iCount = 3;

	// Check whether Mozilla is installed
	::remove("/tmp/clucalc_which.txt");
	
	for( iPos = 0; iPos < iCount; iPos++)
	{
		sprintf(pcCmd, "which %s > /tmp/clucalc_which.txt", pBrowser[iPos]);
		system(pcCmd);
	
		// Check whether text file exists
		if (stat("/tmp/clucalc_which.txt", &FileStat) == 0 && FileStat.st_size != 0)
		{
			// We found something
			break;
		}
	}

	::remove("/tmp/clucalc_which.txt");
	
	if (iPos == iCount)
	{
		// Could not find anything
		fl_alert("Could not find any HTML browser to display the help pages.\n"
				"Please start one manually. You can find the help files under\n"
				"[CLUCalc directory]/Documentation/index.html");
	}
	else
	{
		sprintf(pcCmd, "%s \"%s/Documentation/index.html\" &", pBrowser[iPos], m_sExecPath.c_str());
		system(pcCmd);
	}
	
# endif

}



void CCLUScriptEditor::CB_Help(Fl_Widget *pWidget, void *pvData)
{
	if (pvData)
		((CCLUScriptEditor *) pvData)->Help();
}



//////////////////////////////////////////////////////////////////////
/// About Callback

void CCLUScriptEditor::About()
{
	char *pcText =
		"\tCLUCalc v" CLUCALC_VERSION "\n\n"
		"\tBuild " CLUCALC_BUILD_DATE ", " CLUCALC_BUILD_TIME "\n\n"
		"\tCopyright 2002-2005 by Dr. Christian B. U. Perwass.\n"
		"\tAll right reserved.\n\n"
		"\tProgramming by:\n"
		"\t\tChristian Perwass, Christian Gebken, Daniel Grest.";

	fl_message_title("About CLUCalc");
	fl_message(pcText);
	fl_message_title("");
}

void CCLUScriptEditor::CB_About(Fl_Widget *pWidget, void *pvData)
{
	if (pvData)
		((CCLUScriptEditor *) pvData)->About();
}


//////////////////////////////////////////////////////////////////////
/// Insert Color Callback

void CCLUScriptEditor::InsertColor()
{
	double dR, dG, dB;
	int iPos, iPos2;
	char *pcColText;
	bool bReplace = false;
	Fl_Text_Buffer *pTextBuf;

	// Get current insert position
	iPos = GetEditor()->insert_position();
	// Go to beginning of current word
	iPos = GetEditor()->word_start(iPos);
	// Get Text buffer
	pTextBuf = GetEditor()->buffer();
	// Get Text
	pcColText = pTextBuf->text_range(iPos, iPos+5);

	// Was the cursor places on Color?
	if (pcColText && strcmp(pcColText, "Color") == 0)
	{
		free(pcColText);
		iPos2 = 0;
		pTextBuf->search_forward(iPos, ")", &iPos2);
		if (iPos2 > 0)
		{
			pcColText = pTextBuf->text_range(iPos+6, iPos2);
			if (pcColText)
			{
				int iColCount = sscanf(pcColText, "%lf, %lf, %lf", &dR, &dG, &dB);
				if (iColCount != 3)
				{
					dR = dG = dB = 1;
				}
				bReplace = true;
				free(pcColText);
			}
		}
		else
		{
			dR = dG = dB = 1;
			iPos2 = iPos+4;
			bReplace = true;
		}	
	}
	else
	{
		if (pcColText)
			free(pcColText);

		dR = dG = dB = 1;
	}

	if (fl_color_chooser("Choose Color", dR, dG, dB))
	{
		if (!bReplace)
		{
			char pcText[200];

			sprintf(pcText, "Color(%5.3lf, %5.3lf, %5.3lf)", dR, dG, dB);
			GetEditor()->insert(pcText);
		}
		else
		{
			char pcText[200];

			sprintf(pcText, "Color(%5.3lf, %5.3lf, %5.3lf)", dR, dG, dB);
			pTextBuf->replace(iPos, iPos2+1, pcText);
		}
	}
}


/////////////////////////////////////////////////////////////////////
//// Menu DefVarsE3() Callback

void CCLUScriptEditor::MenuInsertCode()
{
	int iPos, iFinalPosOff = 0;
	Fl_Text_Buffer *pTextBuf;
	string sText;

	const Fl_Menu_Item *pItem = m_pContextMenu->mvalue();
	string sLabel = pItem->text;

	// Reset Value so that next call to context menu starts at top again.
	m_pContextMenu->value(0);

	// Check for Inserting color
	if (sLabel == CSE_IC_COLOR)
	{
		InsertColor();
	}

	else if (sLabel == CSE_IC_RED)
	{
		sText = "Red";
	}
	else if (sLabel == CSE_IC_GREEN)
	{
		sText = "Green";
	}
	else if (sLabel == CSE_IC_BLUE)
	{
		sText = "Blue";
	}
	else if (sLabel == CSE_IC_MAGENTA)
	{
		sText = "Magenta";
	}
	else if (sLabel == CSE_IC_CYAN)
	{
		sText = "Cyan";
	}
	else if (sLabel == CSE_IC_YELLOW)
	{
		sText = "Yellow";
	}
	else if (sLabel == CSE_IC_ORANGE)
	{
		sText = "Orange";
	}
	else if (sLabel == CSE_IC_BLACK)
	{
		sText = "Black";
	}
	else if (sLabel == CSE_IC_WHITE)
	{
		sText = "White";
	}

	// Check for defining variables
	else if (sLabel == CSE_IC_E3)
	{
		sText = "DefVarsE3();\n";
	}
	else if (sLabel == CSE_IC_P3)
	{
		sText = "DefVarsP3();\n";
	}
	else if (sLabel == CSE_IC_N3)
	{
		sText = "DefVarsN3();\n";
	}
	else if (sLabel == CSE_IC_C2)
	{
		sText = "DefVarsC2();\n";
	}

	else if (sLabel == CSE_IC_VE3)
	{
		sText = "VecE3()";
		iFinalPosOff = 1;
	}
	else if (sLabel == CSE_IC_VP3)
	{
		sText = "VecP3()";
		iFinalPosOff = 1;
	}
	else if (sLabel == CSE_IC_VN3)
	{
		sText = "VecN3()";
		iFinalPosOff = 1;
	}
	else if (sLabel == CSE_IC_VC2)
	{
		sText = "VecC2()";
		iFinalPosOff = 1;
	}

	else if (sLabel == CSE_IC_EM_NEW)
	{
		sText = "EM_NEW";
	}
	else if (sLabel == CSE_IC_EM_CHANGE)
	{
		sText = "EM_CHANGE";
	}
	else if (sLabel == CSE_IC_EM_ANIMATE)
	{
		sText = "EM_ANIMATE";
	}
	else if (sLabel == CSE_IC_EM_USER)
	{
		sText = "EM_USER";
	}
	else if (sLabel == CSE_IC_EM_TOOL)
	{
		sText = "EM_TOOL";
	}
	else if (sLabel == CSE_IC_EM_KEYUP)
	{
		sText = "EM_KEYUP";
	}
	else if (sLabel == CSE_IC_EM_KEYDOWN)
	{
		sText = "EM_KEYDOWN";
	}
	else if (sLabel == CSE_IC_EM_RESHAPE)
	{
		sText = "EM_RESHAPE";
	}
	else if (sLabel == CSE_IC_EM_LINK)
	{
		sText = "EM_LINK";
	}

	else if (sLabel == CSE_IC_MVOP_GEO)
	{
		sText = "MVOP_GEO";
	}
	else if (sLabel == CSE_IC_MVOP_INNER)
	{
		sText = "MVOP_INNER";
	}
	else if (sLabel == CSE_IC_MVOP_OUTER)
	{
		sText = "MVOP_OUTER";
	}
	else if (sLabel == CSE_IC_MVOP_ADD)
	{
		sText = "MVOP_ADD";
	}
	else if (sLabel == CSE_IC_MVOP_SUB)
	{
		sText = "MVOP_SUB";
	}
	else if (sLabel == CSE_IC_MVOP_DUAL)
	{
		sText = "MVOP_DUAL";
	}
	else if (sLabel == CSE_IC_MVOP_REVERSE)
	{
		sText = "MVOP_REVERSE";
	}
	else if (sLabel == CSE_IC_MVOP_INVOLUTE)
	{
		sText = "MVOP_INVOLUTE";
	}

	else if (sLabel == CSE_IC_GA_E3)
	{
		sText = "GA_E3";
	}
	else if (sLabel == CSE_IC_GA_P3)
	{
		sText = "GA_P3";
	}
	else if (sLabel == CSE_IC_GA_N3)
	{
		sText = "GA_N3";
	}
	else if (sLabel == CSE_IC_GA_C2)
	{
		sText = "GA_C2";
	}

	else if (sLabel == CSE_IC_OPNS)
	{
		sText = ":OPNS;\n";
	}
	else if (sLabel == CSE_IC_IPNS)
	{
		sText = ":IPNS;\n";
	}

	else if (sLabel == CSE_IC_VS_POINT_DOT)
	{
		sText = ":DRAW_POINT_AS_DOT;\n";
	}
	else if (sLabel == CSE_IC_VS_POINT_SPHERE)
	{
		sText = ":DRAW_POINT_AS_SPHERE;\n";
	}

	else if (sLabel == CSE_IC_VS_E3_VEC_LINE)
	{
		sText = ":E3_DRAW_VEC_AS_LINE;\n";
	}
	else if (sLabel == CSE_IC_VS_E3_VEC_ARROW)
	{
		sText = ":E3_DRAW_VEC_AS_ARROW;\n";
	}
	else if (sLabel == CSE_IC_VS_E3_VEC_POINT)
	{
		sText = ":E3_DRAW_VEC_AS_POINT;\n";
	}

	else if (sLabel == CSE_IC_VS_E3_PLANE_RECT)
	{
		sText = ":E3_DRAW_PLANE_AS_RECT;\n";
	}
	else if (sLabel == CSE_IC_VS_E3_PLANE_DISK)
	{
		sText = ":E3_DRAW_PLANE_AS_DISK;\n";
	}

	else if (sLabel == CSE_IC_VS_N3_SOLID)
	{
		sText = ":N3_SOLID;\n";
	}
	else if (sLabel == CSE_IC_VS_N3_WIRE)
	{
		sText = ":N3_WIRE;\n";
	}

	else if (sLabel == CSE_IC_VS_N3_IMAG)
	{
		sText = ":N3_SHOW_IMAG;\n";
	}
	else if (sLabel == CSE_IC_VS_N3_NO_IMAG)
	{
		sText = ":N3_HIDE_IMAG;\n";
	}

	else if (sLabel == CSE_IC_LOOP1)
	{
		sText = 
			"i = 1;\n"
			"loop\n"
			"{\n"
			"    if (i > 10) break;\n\n"
			"    \n\n"
			"    i = i + 1;\n"
			"}\n";

		iFinalPosOff = 19;
	}
	else if (sLabel == CSE_IC_LOOP2)
	{
		sText = 
			"i = 1;\n"
			"loop\n"
			"{\n"
			"    if (i > 10) break;\n\n"
			"    j = 1;\n"
			"    loop\n"
			"    {\n"
			"        if (j > 10) break;\n\n"
			"        \n\n"
			"        j = j + 1;\n"
			"    }\n\n"
			"    i = i + 1;\n"
			"}\n";

		iFinalPosOff = 45;
	}

	else return;

	if (sText.size())
	{
		// Get current insert position
		iPos = GetEditor()->insert_position();
		// Go to beginning of current word
		iPos = GetEditor()->word_start(iPos);
		// Get Text buffer
		pTextBuf = GetEditor()->buffer();

		GetEditor()->insert_position(iPos);
		GetEditor()->insert(sText.c_str());

		iPos = GetEditor()->insert_position();
		GetEditor()->insert_position(iPos - iFinalPosOff);
	}

	Fl::focus(GetEditor());
	GetEditor()->show_insert_position();
	GetTextBuffer()->call_modify_callbacks();
}

void CCLUScriptEditor::CB_InsertCode(Fl_Widget *pWidget, void *pvData)
{
	if (pvData)
		((CCLUScriptEditor *) pvData)->MenuInsertCode();
}


//////////////////////////////////////////////////////////////////////
/// Window Menu Callbacks

void CCLUScriptEditor::MenuShowEditorWin()
{
	show();
}

void CCLUScriptEditor::MenuShowOutputWin()
{
	if (m_pOutputWin)
		m_pOutputWin->show();
}

void CCLUScriptEditor::MenuShowVisWin()
{
	if (m_poglWin)
		m_poglWin->ShowWindow();
}



void CCLUScriptEditor::CB_ShowEditorWin(Fl_Widget *pWidget, void *pvData)
{
	if (pvData)
		((CCLUScriptEditor *) pvData)->MenuShowEditorWin();
}

void CCLUScriptEditor::CB_ShowOutputWin(Fl_Widget *pWidget, void *pvData)
{
	if (pvData)
		((CCLUScriptEditor *) pvData)->MenuShowOutputWin();
}

void CCLUScriptEditor::CB_ShowVisWin(Fl_Widget *pWidget, void *pvData)
{
	if (pvData)
		((CCLUScriptEditor *) pvData)->MenuShowVisWin();
}





void CCLUScriptEditor::SetScript(const char* pcScript, bool bResetChangedFlag)
{ 
	Fl_Text_Buffer *pTextBuf = GetTextBuffer();

	pTextBuf->text(pcScript);  	
	pTextBuf->call_modify_callbacks();

	if (bResetChangedFlag)
	{
		IsFileChanged() = false;
		SetTitle();
	}

	Parse();
}

//////////////////////////////////////////////////////////////////////
/// Parse Callback

void CCLUScriptEditor::Parse(bool bResetEnv, bool bReparseLatex,
							 const char *pcPath, const char *pcName)
{
	if (!m_poglWin)
		return;

	char *pcText;
	const char *pcScriptPath, *pcScriptName;
	int iIdx = GetEditorID(pcPath, pcName);

	if (iIdx < 0)	// Index not found, or pcPath or pcName not given.
	{
		pcText = GetTextBuffer()->text();
		pcScriptPath = GetPath().c_str();
		pcScriptName = GetName().c_str();
	}
	else
	{
		pcText = m_mEditorData[iIdx].m_pTextBuf->text();
		pcScriptPath = pcPath;
		pcScriptName = pcName;
	}

	m_poglWin->SetScriptPath(pcScriptPath);
	m_poglWin->SetScriptName(pcScriptName);
	m_poglWin->SetScript(pcText, bResetEnv, bReparseLatex);

	free(pcText);
}


void CCLUScriptEditor::MenuParseMain(bool bResetEnv, bool bReparseLatex)
{
	Save();
	Parse(bResetEnv, bReparseLatex, m_poglWin->GetScriptPath().c_str(), 
									m_poglWin->GetScriptName().c_str());

}


void CCLUScriptEditor::CB_LoadNoExec(Fl_Widget *pWidget, void *pvData)
{
	if (pvData)
		((CCLUScriptEditor *) pvData)->LoadNoExec();
}

void CCLUScriptEditor::CB_Parse(Fl_Widget *pWidget, void *pvData)
{
	if (pvData)
		((CCLUScriptEditor *) pvData)->Parse();
}

void CCLUScriptEditor::CB_ParseForce(Fl_Widget *pWidget, void *pvData)
{
	if (pvData)
		((CCLUScriptEditor *) pvData)->Parse(false, true);
}

void CCLUScriptEditor::CB_ParseMain(Fl_Widget *pWidget, void *pvData)
{
	if (pvData)
		((CCLUScriptEditor *) pvData)->MenuParseMain();
}

void CCLUScriptEditor::CB_ParseMainForce(Fl_Widget *pWidget, void *pvData)
{
	if (pvData)
		((CCLUScriptEditor *) pvData)->MenuParseMain(false, true);
}



//////////////////////////////////////////////////////////////////////
/// Script List Menu Functions

void CCLUScriptEditor::CB_NextScript(Fl_Widget* pWidget, void *pData)
{
	if (pData)
	{
		((CCLUScriptEditor *)pData)->m_poglWin->MenuNextScript(pWidget);
	}
}

void CCLUScriptEditor::CB_PrevScript(Fl_Widget* pWidget, void *pData)
{
	if (pData)
	{
		((CCLUScriptEditor *)pData)->m_poglWin->MenuPrevScript(pWidget);
	}
}

void CCLUScriptEditor::CB_ReloadScript(Fl_Widget* pWidget, void *pData)
{
	if (pData)
	{
		((CCLUScriptEditor *)pData)->m_poglWin->MenuReloadScript(pWidget);
	}
}

void CCLUScriptEditor::CB_ClearScriptList(Fl_Widget* pWidget, void *pData)
{
	if (pData)
	{
		((CCLUScriptEditor *)pData)->m_poglWin->MenuClearScriptList(pWidget);
	}
}

void CCLUScriptEditor::CB_ShowScriptList(Fl_Widget* pWidget, void *pData)
{
	if (pData)
	{
		((CCLUScriptEditor *)pData)->m_poglWin->MenuShowScriptList(pWidget);
	}
}


void CCLUScriptEditor::CB_ScriptListWindow(Fl_Widget* pWidget, void *pData)
{
	if (pData)
	{
		((CCLUScriptEditor *)pData)->m_poglWin->ScriptListWindowEvent(pWidget);
	}
}


////////////////////////////////////////////////////////////////////////////
// Is called when text is pasted into editor with pasted text in sText.
// If text need not be adapted returns false. If it returns true,
// then the text in sNewText is inserted instead of original text.

bool CCLUScriptEditor::AdaptText(string &sNewText, string &sText)
{
	const char *pcText = sText.c_str();

	// Test for Mathematica input
	if (pcText[0] == '\\' && pcText[1] == '!')
	{
		char cVal;
		int iPos;
		int iLen = sText.length();

		sNewText.reserve(iLen);

		for (iPos = 2; iPos < iLen; iPos++)
		{
			if (pcText[iPos] == '\\')
			{
				++iPos;
				cVal = pcText[iPos];

				if (cVal == ' ')
					sNewText += "*";
				else if (cVal == '/')
					sNewText += "/";
				else if (cVal == '^')
					sNewText += "^^";
			}
			else
			{
				cVal = pcText[iPos];

				if (cVal == '{')
					sNewText += '[';
				else if (cVal == '}')
					sNewText += ']';
				else
					sNewText += cVal;
			}
		}
		return true;
	}

	return false;
}
