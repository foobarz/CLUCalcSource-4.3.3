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


// COGLWindow.cpp: Implementierung der Klasse COGLWindow.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#ifdef CLUDRAW_MFC
#include "OGLWindow.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLWindow::COGLWindow()
{
	PIXELFORMATDESCRIPTOR nPFD = OGL_STDPFD;

	m_bIsOK = false;
	m_wWidth = m_wHeight = 0;
	m_pDC = 0;
	m_pWnd = 0;

	memcpy(&m_PFD, &nPFD, nPFD.nSize);

}

COGLWindow::COGLWindow(CWnd& nWnd)
{
	m_bIsOK = false;

	PIXELFORMATDESCRIPTOR npfd = OGL_STDPFD;

	memcpy(&m_PFD, &npfd, npfd.nSize);

	ASSERT(true == OGLCreate(nWnd));
}


COGLWindow::~COGLWindow()
{
}


bool COGLWindow::OGLDestroy()
{
	if (m_bIsOK)
	{ 
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext( m_hRC );
		if (m_pDC) delete m_pDC;
		m_pDC = 0;

		TRACE("OGLDestroy: DeleteRC\n");
		
		m_bIsOK = false;
		return true;
	}
	
	return false;
}


bool COGLWindow::OGLCreate(CWnd& nWnd)
{
	CRect rect;

	// Create is only executed if window hasn't been created before
	if (m_bIsOK) return false;

	//Get a DC for the Client Area
	if (!(m_pDC = new CClientDC(&nWnd)))
		return false;

	m_pWnd = &nWnd;
		
	if (!SetupPixelFormat()) 
	{ 
		TRACE("COGLWindow::OGLCreate -> SetupPixelFormat -> false\n");		
		if (m_pDC) delete m_pDC;
		m_pDC = 0;
		return false;
	}
	TRACE("COGLWindow::OGLCreate -> SetupPixelFormat -> true\n");		

	if (!(m_hRC = wglCreateContext( m_pDC->GetSafeHdc() ) )) 
	{ 
		TRACE("COGLWindow::OGLCreate -> m_hRC = NULL\n");		
		delete m_pDC;
		m_pDC = 0;
		return false;
	}
	TRACE("COGLWindow::OGLCreate -> m_hRC OK\n");		

	if (!(wglMakeCurrent( m_pDC->GetSafeHdc(), m_hRC))) 
	{	
		wglDeleteContext( m_hRC );
		delete m_pDC;
		m_pDC = 0;
		TRACE("COGLWindow::OGLCreate -> wglMakeCurrent -> false\n");		
		return false;
	}
	TRACE("COGLWindow::OGLCreate -> wglMakeCurrent -> true\n");		

	m_bIsOK = true;

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
//	glEnable(GL_DEPTH_TEST);
//	glEnableClientState( GL_VERTEX_ARRAY );

	nWnd.GetClientRect(rect);
	glViewport(0, 0, m_wWidth = rect.Width(), m_wHeight = rect.Height());

	m_dAspect = (double) m_wWidth / (double) m_wHeight;

	return true;
}



//////////////////////////////////////////////////////////////////////
// Draw Functions
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Make this RC the current one
// use pHDC when initialising from different thread than initialising thread

bool COGLWindow::SetOGLContext(HDC *pHDC)
{
	return (wglMakeCurrent( (pHDC ? *pHDC : m_pDC->GetSafeHdc()), m_hRC) ? true : false);
//	return true;
}

//////////////////////////////////////////////////////////////////////
/// Begin Draw

bool COGLWindow::BeginDraw(HDC *pHDC)
{
	if (!m_bIsOK) return false;

	if (!SetOGLContext(pHDC)) return false;

	return true;
}


//////////////////////////////////////////////////////////////////////
/// End Draw

bool COGLWindow::EndDraw()
{
	if (!m_bIsOK) return false;

	return (::SwapBuffers( wglGetCurrentDC() ) ? true : false);
}

//////////////////////////////////////////////////////////////////////
// Swap Buffers

bool COGLWindow::SwapBuffers()
{
	if (!m_bIsOK) return false;

	return (::SwapBuffers( wglGetCurrentDC() ) ? true : false);
}



//////////////////////////////////////////////////////////////////////
/// Setup Pixel Format

bool COGLWindow::SetupPixelFormat()
{
    int pixelformat;
	
	HDC hdc = m_pDC->GetSafeHdc();
    
	if ( 0 == (pixelformat = ::ChoosePixelFormat(hdc, &m_PFD)) )
    {
		TRACE("COGLWindow::SetupPixelFormat -> Error selecting Pixelformat\n");
		return false;
	}
	TRACE("COGLWindow::SetupPixelFormat -> Found Pixelformat = %d\n", pixelformat);
	
	if (!::DescribePixelFormat(hdc, pixelformat, sizeof(PIXELFORMATDESCRIPTOR), &m_ActPFD))
	{ 
		TRACE("COGLWindow::SetupPixelFormat -> Descibed Pixelformat -> false\n");
		return false;
	}
	TRACE("COGLWindow::SetupPixelFormat -> Descibed Pixelformat -> true\n");
	
    if ( false == ::SetPixelFormat(hdc, pixelformat, &m_ActPFD) )
    {
		TRACE("COGLWindow::SetupPixelFormat -> SetPixelFormat -> false\n");
		return false;
	}
	TRACE("COGLWindow::SetupPixelFormat -> SetPixelFormat -> true\n");
	
    return true;
}

#endif CLUDRAW_MFC
