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


#if !defined(AFX_OGLWINMFCSTATIC_H__D83E75D8_EF28_11D5_BA34_00E07D8AFD6A__INCLUDED_)
#define AFX_OGLWINMFCSTATIC_H__D83E75D8_EF28_11D5_BA34_00E07D8AFD6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OGLWindow.h"
#include "CLUDrawBase.h"
// OGLWinMFCstatic.h : Header-Datei
//

#define CLU_CONTROL			1
#define CLU_SHIFT			2
#define CLU_ALT				4

/////////////////////////////////////////////////////////////////////////////
// Fenster COGLWinMFCStatic 
class COGLWinMFCStatic : 
	public CStatic, 
	public COGLWindow, 
	public CCLUDrawBase
{
public:
	COGLWinMFCStatic();
	virtual ~COGLWinMFCStatic();

	virtual bool Create(int *piArg = 0, char **pcArg = 0, 
		char *pcName = "CLUDraw v1.0 by C. Perwass",
		int iPosX = 100, int iPosY = 100,
		int iSizeX = 500, int iSizeY = 500, 
		unsigned int umode = 0);

	//virtual bool Init() { return true; }
	//virtual void Display() { CCLUDrawBase::Display();}
	virtual bool BeginDraw();
	virtual bool EndDraw();

	virtual bool StartAnimation(unsigned int uiTime);
	virtual bool StopAnimation();

	// Called before OGL View is destroyed in order to clean up
	// OpenGL Objects.
	virtual bool PreOGLDestroy() { return true; }

	unsigned char CLU_LEFT_BUTTON() {return 0;};
	unsigned char CLU_RIGHT_BUTTON() {return 1;};	
	unsigned char CLU_DOWN() {return 0;};
	unsigned char CLU_UP() {return 1;};

	void GetModifiers();
	void SetModifiers(UINT nFlags);

	bool Lock() { return (m_Access.Lock() ? true : false); }
	bool Unlock() { return (m_Access.Unlock() ? true : false); }

protected:
	static UINT ThreadTimedFunc(LPVOID pParam);

protected:
	virtual void IdleFunc() {};
	void InitVars();

	BOOL m_bMouseMove;
	CPoint m_ActMPoint;

	int m_iModifiers;

	CWnd *m_pAnimWnd;
	CClientDC *m_pAnimDC;
	CWinThread *m_pAnimateThread;
	CCriticalSection m_Access;
	
	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(COGLWinMFCStatic)		
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_OGLWINMFCSTATIC_H__D83E75D8_EF28_11D5_BA34_00E07D8AFD6A__INCLUDED_
