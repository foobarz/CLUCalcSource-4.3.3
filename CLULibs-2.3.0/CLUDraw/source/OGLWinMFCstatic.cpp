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


// COGLWinMFCStatic.cpp: Implementierungsdatei
//
#include "StdAfx.h"

#ifdef CLUDRAW_MFC
#include "OGLWinMFCstatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
/// Constructor

COGLWinMFCStatic::COGLWinMFCStatic()
{
	InitVars();
}


/////////////////////////////////////////////////////////////////////////////
/// Destructor

COGLWinMFCStatic::~COGLWinMFCStatic()
{

}


/////////////////////////////////////////////////////////////////////////////
/// Init Variables

void COGLWinMFCStatic::InitVars()
{
	m_bMouseMove = false;
	m_iModifiers = 0;

	m_pAnimWnd = 0;
	m_pAnimDC = 0;
}


BEGIN_MESSAGE_MAP(COGLWinMFCStatic, CStatic)
	//{{AFX_MSG_MAP(COGLWinMFCStatic)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
/// On Create

bool COGLWinMFCStatic::Create(int *piArg, char **pcArg, char *pcName,
					   int iPosX, int iPosY, int iSizeX, int iSizeY,
					   unsigned int uMode) 
{
	CRect rect;

	GetClientRect(&rect);

	m_iSizeX = rect.Width();
	m_iSizeY = rect.Height();

	bool b = OGLCreate(*this);		// Initialize OpenGL

	TRACE("COGLWinMFCStatic::OnCreate -> OGLCreate -> %s\n", (b ? "TRUE" : "FALSE"));

	// No more initialisations here because window is still of size 0,0
	// Wait for OnSize with non-zero sizes

	if (!Init())
		return false;

	return b;
}


/////////////////////////////////////////////////////////////////////////////
/// On Destroy

void COGLWinMFCStatic::OnDestroy() 
{
	CStatic::OnDestroy();

	if (m_bIsAnimated)
		StopAnimation();

	PreOGLDestroy();
	OGLDestroy();
}



/////////////////////////////////////////////////////////////////////////////
/// On Size

void COGLWinMFCStatic::OnSize(UINT nType, int cx, int cy) 
{
	CStatic::OnSize(nType, cx, cy);
	
	Lock();

	if (cx && cy)
		COGLWindow::m_dAspect = (double) cx / (double) cy;

	Reshape(cx, cy);

	Unlock();
}


/////////////////////////////////////////////////////////////////////////////
/// On Left Button Down

void COGLWinMFCStatic::OnLButtonDown(UINT nFlags, CPoint point) 
{
	Lock();

	m_bMouseMove = true;
	m_ActMPoint = point;
	SetCapture();
	
	CStatic::OnLButtonDown(nFlags, point);

	SetModifiers(nFlags);
	Mouse(CLU_LEFT_BUTTON(), CLU_DOWN(), point.x, point.y);

	Unlock();
}


/////////////////////////////////////////////////////////////////////////////
/// On Left Button Up

void COGLWinMFCStatic::OnLButtonUp(UINT nFlags, CPoint point) 
{
	Lock();

	m_bMouseMove = false;
	m_ActMPoint = point;
	ReleaseCapture();
	
	CStatic::OnLButtonUp(nFlags, point);

	SetModifiers(nFlags);
	Mouse(CLU_LEFT_BUTTON(), CLU_UP(), point.x, point.y);

	Unlock();
}

/////////////////////////////////////////////////////////////////////////////
/// On Right Button Down

void COGLWinMFCStatic::OnRButtonDown(UINT nFlags, CPoint point) 
{
	Lock();

	m_bMouseMove = true;
	m_ActMPoint = point;
	SetCapture();
	
	CStatic::OnRButtonDown(nFlags, point);

	SetModifiers(nFlags);
	Mouse(CLU_RIGHT_BUTTON(), CLU_DOWN(), point.x, point.y);

	Unlock();
}


/////////////////////////////////////////////////////////////////////////////
/// On Right Button Up

void COGLWinMFCStatic::OnRButtonUp(UINT nFlags, CPoint point) 
{
	Lock();

	m_bMouseMove = false;
	m_ActMPoint = point;
	ReleaseCapture();
	
	CStatic::OnRButtonUp(nFlags, point);

	SetModifiers(nFlags);
	Mouse(CLU_RIGHT_BUTTON(), CLU_UP(), point.x, point.y);

	Unlock();
}



/////////////////////////////////////////////////////////////////////////////
// Override the erase background function to
// do nothing to prevent flashing.

BOOL COGLWinMFCStatic::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}


//////////////////////////////////////////////////////////////////////
/// Get Modifiers

void COGLWinMFCStatic::GetModifiers()
{
	int iMod = m_iModifiers;

	if (iMod & CLU_SHIFT) m_bShiftDown = true;
	else m_bShiftDown = false;

	if (iMod & CLU_CONTROL) m_bCtrlDown = true;
	else m_bCtrlDown = false;

	if (iMod & CLU_ALT) m_bAltDown = true;
	else m_bAltDown = false;

}

/////////////////////////////////////////////////////////////////////////////
/// Set Modifiers

void COGLWinMFCStatic::SetModifiers(UINT nFlags)
{
	if (nFlags & MK_CONTROL) m_iModifiers |= CLU_CONTROL;
	else m_iModifiers &= ~CLU_CONTROL;

	if (nFlags & MK_SHIFT) m_iModifiers |= CLU_SHIFT;
	else m_iModifiers &= ~CLU_SHIFT;
}

/////////////////////////////////////////////////////////////////////////////
/// Mouse Move

void COGLWinMFCStatic::OnMouseMove(UINT nFlags, CPoint point) 
{
	Lock();

	if (m_bMouseMove)
	{ 		
		SetModifiers(nFlags);
		GetModifiers();
		ActiveMouseMove(point.x, point.y);
	}
	else m_iModifiers = 0;

	Unlock();

	CStatic::OnMouseMove(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
/// On Key Down

void COGLWinMFCStatic::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	Key(nChar, 0, 0);
	
	CStatic::OnKeyDown(nChar, nRepCnt, nFlags);
}


//////////////////////////////////////////////////////////////////////
/// Begin Draw

bool COGLWinMFCStatic::BeginDraw()
{
	Lock();

	HDC hdc, *pActHDC;

	if (m_bIsAnimated)
	{
		if (!m_pAnimDC)
		{
			Unlock();
			return false;
		}
		
		hdc = m_pAnimDC->GetSafeHdc();
		pActHDC = &hdc;
	}
	else pActHDC = 0;
	
	if (!COGLWindow::BeginDraw(pActHDC))
		return false;

	return CCLUDrawBase::BeginDraw();
}

//////////////////////////////////////////////////////////////////////
/// End Draw

bool COGLWinMFCStatic::EndDraw()
{
	CCLUDrawBase::EndDraw();

	COGLWindow::EndDraw();

	Unlock();

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Start Animation

bool COGLWinMFCStatic::StartAnimation(unsigned int uTime)
{
	if (m_bIsAnimated)
		return false;

	// VERY IMPORTANT TO RESET DC IN THIS CONTEXT
	wglMakeCurrent(NULL, NULL);

	m_uAnimateTimeStep = uTime;
	m_bIsAnimated = true;
	m_pAnimateThread = AfxBeginThread(COGLWinMFCStatic::ThreadTimedFunc, (LPVOID) this);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Stop Animation

bool COGLWinMFCStatic::StopAnimation()
{
	if (!m_bIsAnimated)
		return true;

	// Signals to thread to end
	m_bIsAnimated = false;

	// Wait for worker thread to end
	::WaitForSingleObject(m_pAnimateThread->m_hThread, INFINITE);

	m_fTotalTime = 0;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Timed Func

UINT COGLWinMFCStatic::ThreadTimedFunc(LPVOID pParam)
{
	COGLWinMFCStatic *that = (COGLWinMFCStatic *) pParam;	// expect this pointer

	if (!that) return 1; // Error value 1

	
	if (!(that->m_pAnimWnd = new CWnd))
		return 1;

	that->m_pAnimWnd->Attach(that->m_hWnd);

	if (!(that->m_pAnimDC = new CClientDC(that->m_pAnimWnd)))
	{
		that->m_pAnimWnd->Detach();
		delete that->m_pAnimWnd;
		return 1;
	}

	that->m_fTimeStep = 0;
	that->m_fTotalTime = 0;

	that->Lock();
	if (!that->AtAnimStart())
	{
		that->Unlock();
		return -1;
	}
	that->Unlock();

	UINT Time1, Time2, PulseTime;
	
	Time1 = ::timeGetTime();	// Get System Timer in ms

	// Loop while variable is TRUE
	while (that->m_bIsAnimated)
	{
		// Sleep until your time has come...
		while ( (PulseTime = ((Time2 = ::timeGetTime()) - Time1)) < that->m_uAnimateTimeStep) ::Sleep(0);
		
		Time1 = Time2;

		that->m_fTimeStep = 1e-3f * float(PulseTime);
		that->m_fTotalTime += that->m_fTimeStep;

		// Lock variables of class
		that->Lock();

		if (!that->TimedFunc())
		{
			that->Unlock();
			that->m_bIsAnimated = false;
			break;
		}

		that->Unlock();
	}

	that->Lock();
	that->AtAnimEnd();
	that->Unlock();


	wglMakeCurrent(NULL, NULL);

	delete that->m_pAnimDC;
	that->m_pAnimWnd->Detach();
	delete that->m_pAnimWnd;

	that->m_pAnimDC = 0;
	that->m_pAnimWnd = 0;

	return 0;
}


#endif // CLUDRAW_MFC

