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


// OGLWindow.h: Schnittstelle für die Klasse OGLWindow.
// 
// This class does all the initialisation of OpenGL to obtain
// a working context.
// It does not initialize the perspective, etc.
//
//////////////////////////////////////////////////////////////////////



#if !defined(AFX_OGLWINDOW_H__6A0F3602_2E0D_11D2_99B1_845806C10000__INCLUDED_)
#define AFX_OGLWINDOW_H__6A0F3602_2E0D_11D2_99B1_845806C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000



#define OGL_STDPFD { \
        sizeof(PIXELFORMATDESCRIPTOR), \
        1,                             \
        PFD_DRAW_TO_WINDOW |           \
        PFD_SUPPORT_OPENGL |           \
        PFD_DOUBLEBUFFER,              \
        PFD_TYPE_RGBA,                 \
        24,                            \
        0, 0, 0, 0, 0, 0,              \
        0,                             \
        0,                             \
        0,                             \
        0, 0, 0, 0,                    \
				16,	                           \
        0,                             \
				0,														 \
				PFD_MAIN_PLANE,								 \
        0,                             \
        0, 0, 0                        \
  };

//        PFD_MAIN_PLANE,                
//        0,                             

class COGLWindow
{
public:
	COGLWindow();
	virtual ~COGLWindow();
	COGLWindow(CWnd& nWnd);

	virtual bool OGLCreate(CWnd& nWnd);
	virtual bool OGLDestroy();

	virtual bool BeginDraw(HDC *pHDC = 0);
	virtual bool EndDraw();

	bool SwapBuffers();

	bool SetOGLContext(HDC *pHDC = 0);

	HGLRC GetRCHandle() { return m_hRC; }

	bool IsOK() { return m_bIsOK; }
	WORD Width() { return m_wWidth; }
	WORD Height() { return m_wHeight; }
	double AspectRatio() { return m_dAspect; }

	void SetOverlayPlanes(char op)
		{ m_PFD.bReserved = (m_PFD.bReserved & 0xF0) | (op & 0x0F); }
	void SetUnderlayPlanes(char up)
		{ m_PFD.bReserved = (m_PFD.bReserved & 0x0F) | ((up & 0x0F) << 4); }

protected:
	bool SetupPixelFormat();


protected:
	bool m_bIsOK;

	CWnd *m_pWnd;
	HGLRC m_hRC;
	CDC *m_pDC;

	WORD m_wWidth, m_wHeight;
	double m_dAspect;

	PIXELFORMATDESCRIPTOR m_PFD, m_ActPFD;
};

#endif // !defined(AFX_OGLWINDOW_H__6A0F3602_2E0D_11D2_99B1_845806C10000__INCLUDED_)
