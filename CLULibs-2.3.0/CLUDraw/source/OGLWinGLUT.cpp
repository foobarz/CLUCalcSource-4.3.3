/// LICENSE START
////////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the CLUDraw Library.
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


// OGLWinGLUT.cpp: Implementierung der Klasse COGLWinGLUT.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#ifdef CLUDRAW_GLUT
#include "OGLWinGLUT.h"

// Explicit instantiation of map.
//template class map<int, COGLWinGLUT*>;

map<int, COGLWinGLUT*> COGLWinGLUT::m_mapThis;
int COGLWinGLUT::m_iHMainWin = 0;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLWinGLUT::COGLWinGLUT()
{
	//m_pThis = this;	
	m_iHWin = 0;
	m_bNotifyDestroy = false;
}

COGLWinGLUT::~COGLWinGLUT()
{

}

//COGLWinGLUT* COGLWinGLUT::m_pThis = 0;

//////////////////////////////////////////////////////////////////////
/// Create OpenGL Window with GLUT
bool COGLWinGLUT::Create(int *piArg, char **pcArg, char *pcName,
					   int iPosX, int iPosY, int iSizeX, int iSizeY,
					   unsigned int uMode)
{
	m_iSizeX = iSizeX;
	m_iSizeY = iSizeY;

	glutInit(piArg, pcArg);
	glutInitDisplayMode(uMode);
	glutInitWindowSize(iSizeX, iSizeY);
	glutInitWindowPosition(iPosX, iPosY);
	m_iHMainWin = m_iHWin = glutCreateWindow(pcName);

	//printf("Main Window: %d\n", m_iHMainWin);

	// Enter this pointer into map.
	m_mapThis[m_iHWin] = this;

	glutDisplayFunc(GLUTDisplay);
	glutReshapeFunc(GLUTReshape);
	glutMouseFunc(GLUTMouse);
	glutKeyboardFunc(GLUTKey);
	glutMotionFunc(GLUTActiveMouseMove);

	//m_iMainMenu = glutCreateMenu(GLUTMenu);
	//glutAttachMenu(GLUT_MIDDLE_BUTTON);

	if (!Init()) return false;
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Create OpenGL Window with GLUT
bool COGLWinGLUT::CreateWindow(const char *pcName,
					   int iPosX, int iPosY, int iSizeX, int iSizeY,
					   unsigned int uMode)
{
	// Only Create this window once.
	if (m_iHWin != 0)
		return false;
		
	m_iSizeX = iSizeX;
	m_iSizeY = iSizeY;

	//glutInit(piArg, pcArg);
	glutInitDisplayMode(uMode);
	glutInitWindowSize(iSizeX, iSizeY);
	glutInitWindowPosition(iPosX, iPosY);
	m_iHWin = glutCreateWindow(pcName);

	// Enter this pointer into map.
	m_mapThis[m_iHWin] = this;

	glutDisplayFunc(GLUTDisplay);
	glutReshapeFunc(GLUTReshape);
	glutMouseFunc(GLUTMouse);
	glutKeyboardFunc(GLUTKey);
	glutMotionFunc(GLUTActiveMouseMove);

	//m_iMainMenu = glutCreateMenu(GLUTMenu);
	//glutAttachMenu(GLUT_MIDDLE_BUTTON);

	if (!Init()) return false;
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Detach window

bool COGLWinGLUT::DestroyWindow()
{
	if (m_iHWin == 0)
		return true;
		
	if (m_bIsAnimated)
		StopAnimation();
	
	glutDestroyWindow(m_iHWin);
	glutSetWindow(m_iHMainWin);
	m_mapThis.erase(m_iHWin);
	m_iHWin = 0;
	
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Create OpenGL Window with GLUT
/*
bool COGLWinGLUT::Create(int *piArg, char **pcArg, MemObj<SGLUTWin>& mWin)
{


	return true;
}

*/

//////////////////////////////////////////////////////////////////////
/// Post Main Window Redisplay

void COGLWinGLUT::PostMainWindowUpdate()
{
	//printf("PostMainWindowRedisplay (%d)\n", m_iHMainWin);
	COGLWinGLUT *pThis = m_mapThis[m_iHMainWin];
	pThis->PostWindowUpdate();
	//printf("PostMainWindowRedisplay Exit\n");
}

void COGLWinGLUT::PostWindowUpdate()
{
	return;
}


//////////////////////////////////////////////////////////////////////
/// Static functions to call virtual functions

void COGLWinGLUT::GLUTDisplay()
{
	// Get this pointer for current window
	COGLWinGLUT *pThis = m_mapThis[glutGetWindow()];

	//printf("Display: %d\n", glutGetWindow());

	if (pThis)
		pThis->Display();
}

void COGLWinGLUT::GLUTReshape(int iWidth, int iHeight)
{
	// Get this pointer for current window
	COGLWinGLUT *pThis = m_mapThis[glutGetWindow()];
	
	if (pThis)
		pThis->Reshape(iWidth, iHeight);
}

void COGLWinGLUT::GLUTMouse(int iButton, int iState, int iX, int iY)
{
	// Get this pointer for current window
	COGLWinGLUT *pThis = m_mapThis[glutGetWindow()];
	
	if (pThis)
		pThis->Mouse(iButton, iState, iX, iY);
}

void COGLWinGLUT::GLUTKey(unsigned char cKey, int iX, int iY)
{
	// Get this pointer for current window
	COGLWinGLUT *pThis = m_mapThis[glutGetWindow()];
	
	if (pThis)
		pThis->Key(cKey, iX, iY);
}

void COGLWinGLUT::GLUTActiveMouseMove(int iX, int iY)
{
	// Get this pointer for current window
	COGLWinGLUT *pThis = m_mapThis[glutGetWindow()];
	
	if (pThis)
		pThis->ActiveMouseMove(iX, iY);
}

void COGLWinGLUT::GLUTIdleFunc() 
{
	// Idle func is always called first with this pointer
	// of main window!
	// Get this pointer for current window
	COGLWinGLUT *pThis = m_mapThis[m_iHMainWin];
	
	if (pThis)
	{
		glutSetWindow(m_iHMainWin);
		pThis->IdleFunc();
	}
		
	// Now call IdleFunc for all other windows.
	std::map<int, COGLWinGLUT*>::iterator itCur, itEnd;
	
	itCur = m_mapThis.begin();
	itEnd = m_mapThis.end();
	for(;itCur != itEnd; ++itCur)
	{
		if (itCur->first != m_iHMainWin)
		{
			glutSetWindow(itCur->first);
			itCur->second->IdleFunc();

			if (itCur->second->ShouldDestroy())
			{
				itCur->second->DestroyWindow();
				break;
			}
		}
	}
}

void COGLWinGLUT::GLUTMenu(int iIdx)
{
	// Get this pointer for current window
	COGLWinGLUT *pThis = m_mapThis[glutGetWindow()];
	
	if (pThis)
		pThis->Menu(iIdx);
}

//////////////////////////////////////////////////////////////////////
/// Begin Draw

bool COGLWinGLUT::BeginDraw()
{
	glutSetWindow(m_iHWin);

	CCLUDrawBase::BeginDraw();

	return true;
}

//////////////////////////////////////////////////////////////////////
/// End Draw

bool COGLWinGLUT::EndDraw()
{
	CCLUDrawBase::EndDraw();

	glutSwapBuffers();
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Enter Main Loop

void COGLWinGLUT::Run()
{
	glutMainLoop();
}

//////////////////////////////////////////////////////////////////////
/// Start Animation

bool COGLWinGLUT::StartAnimation(unsigned int uiTime) 
{
	if (m_bIsAnimated)
		return false;

	GETTIME(&m_tmStart);
	memcpy(&m_tmPrev, &m_tmStart, sizeof(TTimeStruct));

	m_fTotalTime = 0;	 
	m_fTimeStep = 0;
	m_uAnimateTimeStep = uiTime;

	if (!AtAnimStart())
		return false;

	m_bIsAnimated = true;
	glutTimerFunc(m_uAnimateTimeStep, GLUTTimedFunc, m_iHWin);	

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Stop Animation

bool COGLWinGLUT::StopAnimation() 
{
	m_bIsAnimated = false;

	AtAnimEnd();

	return true;
}

//////////////////////////////////////////////////////////////////////
/// GLUT Timed Func

void COGLWinGLUT::GLUTTimedFunc(int value) 
{
	// Get this pointer for the calling window
	COGLWinGLUT *pThis;
	std::map<int, COGLWinGLUT*>::iterator itCur;
	
	// If Window handle is not found in map then return.
	if ((itCur = m_mapThis.find(value)) == m_mapThis.end())
		return;
	
	pThis = itCur->second;

	if (pThis) 
	{
		pThis->calcTimeElapsed();

		if (pThis->TimedFunc() && pThis->m_bIsAnimated)
			glutTimerFunc(pThis->m_uAnimateTimeStep, COGLWinGLUT::GLUTTimedFunc, value);
	}
}

//////////////////////////////////////////////////////////////////////
/// Get Modifiers
void COGLWinGLUT::GetModifiers()
{
	int iMod = glutGetModifiers();

	if (iMod & GLUT_ACTIVE_SHIFT) m_bShiftDown = true;
	else m_bShiftDown = false;

	if (iMod & GLUT_ACTIVE_CTRL) m_bCtrlDown = true;
	else m_bCtrlDown = false;

	if (iMod & GLUT_ACTIVE_ALT) m_bAltDown = true;
	else m_bAltDown = false;
}

//////////////////////////////////////////////////////////////////////
/// calculate elpased time since last idlefunc call

void COGLWinGLUT::calcTimeElapsed() 
{
	TTimeStruct tmCur;
	GETTIME(&tmCur);

	m_fTotalTime = float(tmCur.time - m_tmStart.time) 
					+ 1e-3 * float(tmCur.millitm - m_tmStart.millitm);

	m_fTimeStep = float(tmCur.time - m_tmPrev.time) 
					+ 1e-3 * float(tmCur.millitm - m_tmPrev.millitm);

	memcpy(&m_tmPrev, &tmCur, sizeof(TTimeStruct));
}

#endif // CLUDRAW_GLUT


