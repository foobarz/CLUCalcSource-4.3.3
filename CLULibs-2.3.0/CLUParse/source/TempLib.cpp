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

// Template Library

#include "StdAfx.h"

#include "mem.cxx"
#include "memobj.cxx"
#include "Stack.cpp"
#include "CodeVar.h"
#include "CodeElementList.h"
#include "MessageList.h"
#include "ParseBase.h"
#include "OCOpStd.h"

template class CStack<CCodeVar*>;
template class CStack<CCodeVarList*>;

template class Mem<SCodeElementPtr>;
template class MemObj<CCodeVar>;
template class MemObj<CCodeVar*>;
template class Mem<CCodeVar*>;
template class Mem<CCodeVar**>;
template class MemObj<CCodeVarList*>;
template class Mem<CCodeVarList**>;
template class MemObj<SMsg>;
template class Mem<SMsg*>;
template class MemObj<CCodeFunction>;
template class Mem<CCodeFunction*>;
template class MemObj<STextLine>;
template class MemObj<STextLine*>;
template class Mem<STextLine*>;
template class Mem<STextLine**>;
template class MemObj<COCOpStd>;
template class Mem<COCOpStd*>;
template class MemObj<TMatrix>;
template class MemObj<COGLColor>;
template class MemObj<TImage>;
template class MemObj<MemObj<TImage> >;
template class Mem<TVarList*>;
