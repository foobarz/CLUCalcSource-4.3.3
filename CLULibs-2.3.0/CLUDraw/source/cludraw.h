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


// Includes all headers for CLUDraw

#ifdef WIN32
#ifdef CLUDRAW_MFC
	#if _MSC_VER > 1000
	#pragma once
	#endif // _MSC_VER > 1000

	#define WIN32_LEAN_AND_MEAN		// Selten benutzte Teile der Windows-Header nicht einbinden

	#ifndef _AFXDLL
		#define _AFXDLL
	#endif
	#include <afxwin.h>
	#include <afxmt.h>
	#include "mmsystem.h"
#else
	#define WIN32_LEAN_AND_MEAN		// Selten benutzte Teile der Windows-Header nicht einbinden
	#include <windows.h>
#endif
#endif

#include "GL/gl.h"
#include "GL/glu.h"

// Undefs for gcc since they are also defined in DevIL
#ifdef PACKAGE
#    undef PACKAGE
#endif

#ifdef VERSION
#    undef VERSION
#endif

/*
#ifdef CLUDRAW_MFC
	#include "OGLWinMFCstatic.h"
#endif

#ifdef CLUDRAW_GLUT
	#include "OGLWinGLUT.h"
#endif

#ifdef CLUDRAW_FLTK
	#include "OGLWinFLTK.h"
#endif
*/

#include "CLUDrawBase.h"
#include "OGLMVFilterBase.h"
#include "OGLFilterE3.h"
#include "OGLFilterP2.h"
#include "OGLFilterP3.h"
#include "OGLFilterN3.h"
#include "OGLMVFilter.h"
#include "OGLLatexText.h"
#include "OGLBitmapText.h"
#include "OGLBitmap.h"
#include "OGLColor.h"
#include "OGLVertex.h"
#include "OGLLight.h"
#include "OGLMaterial.h"
#include "OGLVertexList.h"
#include "OGLVexListE3.h"
#include "OGLVexListP2.h"
#include "OGLVexListP3.h"
#include "OGLVexListN3.h"
#include "OGLObjColorCube.h"
//#include "OGLObjWireSphere.h"
#include "OGLDisplayList.h"

#include "OGLBaseElement.h"
#include "OGLBaseElementList.h"
#include "OGLBaseTransform.h"

