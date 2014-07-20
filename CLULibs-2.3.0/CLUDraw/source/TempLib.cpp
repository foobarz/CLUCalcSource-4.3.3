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



// Instantiation of templates


#include "StdAfx.h"
#include "CLUDrawBase.h"
#include "mem.cxx"
#include "memobj.cxx"

#include "OGLMaterial.h"
#include "OGLVertexList.h"
#include "OGLBitmapText.h"
#include "OGLBitmap.h"
#include "OGLMVFilterBase.h"


template class Mem<COGLMVFilterBase::STriplet>;
template class Mem<COGLVertex>;
template class Mem<COGLVertex *>;
template class Mem<TColor>;
template class Mem<STransform>;
template class Mem<COGLMaterial *>;
template class Mem<COGLMaterial **>;
template class Mem<COGLBaseElement *>;
template class Mem<CPointND<float, 4>*>;
template class Mem<Mem<int> *>;
template class Mem<COGLBitmap::TBGRAPixel>;
template class Mem<COGLBitmap::TRGBAPixel>;

template class MemObj<COGLVertex>;
template class MemObj<TColor>;
template class MemObj<COGLMaterial>;
template class MemObj<COGLMaterial *>;
template class MemObj<Mem<int> >;
template class MemObj<COGLBitmapText::SFontInfo>;
template class MemObj<COGLVertexList*>;
