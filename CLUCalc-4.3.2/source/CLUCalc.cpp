/// LICENSE START 
////////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the CLUCalc program.
// CLUCalc is an interactive Clifford calculator with OpenGL visualiations.
//
// 
// Copyright (C) 2002-2005  Christian B.U. Perwass
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

// CLUParseTest.cpp : Definiert den Einsprungpunkt für die Konsolenanwendung.
//

#include "StdAfx.h"

#include "CLUScriptEditor.h"
#include "OGLWin.h"
#include "CLUOutput.h"
#include "resource.h"

#include <FL/Fl_Tooltip.H>

#include <string>


#ifdef _GNUCPP3_
#   include <unistd.h>
#   define _getcwd getcwd
#   define _chdir chdir
#else
#   include <direct.h>
#endif

#define BUFSIZE		1024


#include <vector>
using namespace std;

	char *pcIntro = 
				"// Press CTRL+n to start a new script.\n\n"
				"/*\n\n"
				"Welcome to CLUCalc v"
				CLUCALC_VERSION
				"\n"
				"Build " CLUCALC_BUILD_DATE ", " CLUCALC_BUILD_TIME
				"\n\n"
				"Written and conceived by Dr. Christian Perwass.\n"
				"Additional development by\n"
				"  - Christian Gebken\n"
				"  - Daniel Grest\n\n\n"
				"For support contact C.Perwass under:\n"
				"               help@clucalc.info\n\n"
				"or go to the web page:\n"
				"               www.clucalc.info.\n\n\n"
				"By using this program YOU AGREE\n"
				"to the following license agreement\n\n"
				"Copyright 2002-2005 by Dr. Christian B. U. Perwass.\n"
				"All rights reserved.\n\n"
				"Redistribution and use in binary form of CLUCalc.exe\n"
				"and/or any of its components, without modification,\n"
				"is permitted provided that the following conditions\n"
				"are met:\n\n"
				"Redistributions in binary form must reproduce the\n"
				"above copyright notice, this list of conditions\n"
				"and the following disclaimer in the documentation\n"
				"and/or other materials provided with the distribution.\n"
				"All advertising materials mentioning features or use\n"
				"of this software must display the following\n"
				"acknowledgement:\n"
				"This product includes software developed by \n"
				"Dr. Christian B. U. Perwass.\n"
				"The name Christian B.U. Perwass may not be used\n"
				"to endorse or promote products derived\n"
				"from this software without specific prior\n"
				"written permission.\n\n"
				"THIS SOFTWARE IS PROVIDED BY\n"
				"      Dr. Christian B.U. Perwass\n"
				"``AS IS'' AND ANY EXPRESS OR IMPLIED\n"
				"WARRANTIES, INCLUDING, BUT NOT LIMITED\n"
				"TO, THE IMPLIED WARRANTIES OF \n"
				"MERCHANTABILITY AND FITNESS FOR A\n"
				"PARTICULAR PURPOSE ARE DISCLAIMED.\n"
				"IN NO EVENT SHALL Dr. Christian\n"
				"B.U. Perwass BE LIABLE FOR ANY DIRECT,\n"
				"INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,\n"
				"OR CONSEQUENTIAL DAMAGES (INCLUDING,\n"
				"BUT NOT LIMITED TO, PROCUREMENT OF\n"
				"SUBSTITUTE GOODS OR SERVICES;\n"
				"LOSS OF USE, DATA, OR PROFITS;\n"
				"OR BUSINESS INTERRUPTION)\n"
				"HOWEVER CAUSED AND ON ANY THEORY\n"
				"OF LIABILITY, WHETHER IN CONTRACT,\n"
				"STRICT LIABILITY, OR TORT (INCLUDING\n"
				"NEGLIGENCE OR OTHERWISE) ARISING IN\n"
				"ANY WAY OUT OF THE USE OF THIS\n"
				"SOFTWARE, EVEN IF ADVISED OF THE \n"
				"POSSIBILITY OF SUCH DAMAGE.\n"
				"*/\n\n"
				"DefVarsN3();\n\n"
				"?\"Change to mouse mode 1 and move the red sphere\";\n"
				"?\"by holding down the right mouse button and\";\n"
				"?\"moving the mouse in the visualization window.\";\n\n"
				"?\" \";\n"
				"?\"Press CTRL + h in the editor window for more help.\";\n\n"
				":N3_IPNS;\n"
				":N3_SOLID;\n"
				":N3_SHOW_IMAG;\n"
				"_FrameBoxSize = 0;\n"
				"EnableSmoothLine(1);\n"
				"SetFrameRotor(RotorE3(1,0.7,0, Pi/5));\n"
				"\n"
				":MRed;\n"
				":Sr = VecN3(VecE3(1) + VecE3(-0.12,0,0.2)) - 0.5*e;\n"
				"\n"
				":Color(0, 0, 0.6, 0.6);\n"
				":Sb = VecN3(0.5, 0, 0) - 0.5*e;\n"
				"\n"
				":Color(0, 0.6, 0, 0.6);\n"
				":Sg = VecN3(-0.5, 0.5, 0) - 0.5*e;\n"
				"\n"
				":Color(0.540, 0.991, 1.000);\n"
				"SetLatexMagStep(20);\n"
				"DrawLatex(-1.5, 0.3, 1.1, \"\\\\[\\\\mathsf{CLU}\\\\mathit{Calc}^{\" + \"" CLUCALC_VERSION 
				"\" + \"}\\\\] \", \"title\");\n"
				"\n"
				":Magenta;\n"
				":C = Sr^Sb;\n"
				"\n"
				":Cyan;\n"
				":C = Sr^Sg;\n"
				"\n"
				":Yellow;\n"
				":PP = Sr^Sb^Sg;\n\n"
				":VecC2(0,1)^VecC2(1,0)^VecC2(0,-1)^VecC2(-1,0)^VecC2(1.5,0.2);\n\n"
				"StartOverlay();\n"
				"SetLatexMagStep(10);\n"
				"SetLatexAlign(0.5, 0);\n"
				"DrawLatex(50, 98, 0, \"Get it from \\\\texttt{www.CluCalc.info}\",\n"
				"          \"web\");\n"
				"EndOverlay();\n"
				"\n";



int main(int argc, char* argv[])
{
	CCLUScriptEditor *pEditor;
	COGLWin *poglWin;
	CCLUOutput *pOutput;
	void *pvIcon = 0;
	bool bVizOnly = false;

	int iScreenWidth = Fl::w();
	int iScreenHeight = Fl::h();

	int iX, iY, iW, iH;

#ifdef WIN32
	pvIcon = (void *) LoadIcon(GetModuleHandle(NULL), (LPCTSTR) IDI_CLUCALC);
#endif

	iX = int(float(iScreenWidth) * 0.01f);
	iY = int(float(iScreenHeight) * 0.03f);
	iW = (iScreenWidth - 2*iX) / 2.5;
	iH = int( float(iScreenHeight - 2*iY) * 0.7f );
//	iW = iScreenWidth / 2.5;
//	iH = int( float(iScreenHeight) * 0.7f );

	cStr csBaseTitle, csTitle;
	csBaseTitle = "CLUCalc v";
	csBaseTitle << CLUCALC_VERSION;
	csBaseTitle << " by C. Perwass";
		
	int i;
	char *pcEnd, pcExecPath[300], *pcScript = 0;
	bool bShowIntro = true;

	if (argc >= 1)
	{
		strcpy(pcExecPath, argv[0]);

# ifdef WIN32
		pcEnd = strrchr(pcExecPath, '\\');
# else
		pcEnd = strrchr(pcExecPath, '/');
# endif

		if (pcEnd)
			*pcEnd = 0;
		else
			strcpy(pcExecPath, ".");
	}

	for (i = 1; i < argc; i++)
	{
		if (strstr(argv[i], "--main-path="))
		{
			strncpy(pcExecPath, &argv[i][12], 299);
		}
		else if (strstr(argv[i], "--no-intro"))
		{
			bShowIntro = false;
		}
		else if (strstr(argv[i], "--viz-only"))
		{
			bVizOnly = true;
		}
		else
		{
			pcScript = argv[i];
		}
	}

	int iVal;

	if (bShowIntro)
		iVal = iW / 3;
	else
		iVal = iW;


	Fl_Tooltip::size(12);

	csTitle = csBaseTitle + " - Output";
	if (bVizOnly)
	{
		if (!(pOutput = new CCLUOutput(-100, 0, 1, 1, " ", 0)))
		{
			printf("Cannot create output window. Probably not enough memory.\n\n");
			exit(-1);
		}
	}
	else
	{
		if (!(pOutput = new CCLUOutput(iX - 3, iY + iH + 40, iW, int(float(iH) / 0.7f * 0.3f) - 30, csTitle.Str(), pvIcon)))
		{
			printf("Cannot create output window. Probably not enough memory.\n\n");
			exit(-1);
		}
	}

	if (!(poglWin = new COGLWin))
	{
		printf("Cannot create OpenGL window. Probably not enough memory.\n\n");
		exit(-1);
	}
	
	if (bVizOnly)
		poglWin->EnableVizOnlyMode();


	csTitle = csBaseTitle + " - Visualization";
	if (bVizOnly)
	{
		poglWin->CreateWindow(csTitle.Str(), 19, iY+11, 
								iScreenWidth - 9, iScreenHeight - iY - 11, pvIcon);

		iX = -100;
		iY = 0;
		iW = 1;
		iH = 1;
	}
	else
	{
		poglWin->CreateWindow(csTitle.Str(), iX + iVal + 18, iY + 10, 
								iScreenWidth - iVal - 18, int(float(iH) / 0.7f),
								pvIcon);

		iX -= 3;
		iY += 10;
	}
	poglWin->SetOutputWin(pOutput);
	poglWin->SetVersion(CLUCALC_VERSION_MAJOR, CLUCALC_VERSION_MINOR, CLUCALC_VERSION_REVISION);

	if (pcScript)
	{
		if (!(pEditor = new CCLUScriptEditor(iX, iY, iW, iH, pcScript, pcExecPath, pvIcon)))
		{
			printf("Cannot create editor window. Probably not enough memory.\n\n");
			exit(-1);
		}
		pEditor->SetOGLWin(poglWin);
		pEditor->SetOutputWin(pOutput);
				
		pOutput->SetOGLWin(poglWin);
		pOutput->SetEditorWin(pEditor);

		poglWin->SetEditor(pEditor);

		pEditor->Parse(true);
	}
	else
	{
		cStr csFilename;
		struct _stat FileStat;
		
		_chdir(pcExecPath);
	
		if (bShowIntro)
		{
			csFilename = pcExecPath;
#	ifdef WIN32
			csFilename << "\\ExampleScripts\\Intro_4_3.clu";
#	else
			csFilename << "/ExampleScripts/Intro_4_3.clu";
#	endif
			if (_stat(csFilename.Str(), &FileStat) < 0)
			{
				pcScript = 0;
			}
			else
			{
				pcScript = csFilename.Str();
			}
		}
		else
		{
			pcScript = 0;
		}

		if (!(pEditor = new CCLUScriptEditor(iX, iY, iW, iH, pcScript, pcExecPath, pvIcon)))
		{
			printf("Cannot create editor window. Probably not enough memory.\n\n");
			exit(-1);
		}

		pEditor->SetOGLWin(poglWin);
		pEditor->SetOutputWin(pOutput);

		pOutput->SetOGLWin(poglWin);
		pOutput->SetEditorWin(pEditor);

		poglWin->SetEditor(pEditor);

		if (bShowIntro)
		{
			if (pcScript)
				pEditor->Parse(true);
			else
				pEditor->SetScript(pcIntro, true);
		}
	}

	if (bVizOnly)
	{
		pOutput->hide();
		pEditor->hide();
	}
	else
	{
		pOutput->Show();
		pEditor->Show();
	}

	poglWin->ShowWindow();

	return Fl::run();
}

	
