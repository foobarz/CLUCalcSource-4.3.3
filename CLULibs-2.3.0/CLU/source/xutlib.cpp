/// LICENSE START
////////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the CLU Library.
//
// 
// Copyright (C) 1997-2004  Christian B.U. Perwass
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

#include "cstr.h"

////////////////////////////////////////////////////////////////////////////////////
// Template Library for Mem Class

#include "mem.cxx"

template class Mem<float>;
template class Mem<double>;
template class Mem<char>;
template class Mem<uint>;
template class Mem<uint*>;
template class Mem<int>;
template class Mem<cStr *>;


////////////////////////////////////////////////////////////////////////////////////
// DynList Template Library

#include"dynlist.cxx"

// To instantiate DynList include statements of the following type
//
// template class DynList<[Class]>;
//

////////////////////////////////////////////////////////////////////////////////////
/// Template library for RingBuf Class

#include"ringbuf.cxx"

Instantiate_RingBuf(uint);


////////////////////////////////////////////////////////////////////////////////////
// Template Library for MemObj Class

#include"memobj.cxx"

// Enter here Template Instanciations of the type
//
// template class MemObj<[Class]>;
//
// for all those objects that need to have constructor/destructor called
// and define their own operator= for copying.

template class MemObj<cStr>;
template class MemObj<Mem<uint> >;

////////////////////////////////////////////////////////////////////////////////////
// Template Library for MessageList class

#include "MessageList.cxx"

template struct tMessageElement<cStr>;
template class Mem<tMessageElement<cStr>* >;
template class MemObj<tMessageElement<cStr> >;
template class MessageList<cStr>;



////////////////////////////////////////////////////////////////////////////////////

// The first two number give major and minor version.
// The last three numbers give the date in the format day,month,year.
unsigned _xutlib_version[5] = {1,3,14,8,1999};

char *_xutlib_license_text = 
"This program uses the CLU Library\n"
"Version 1.3, August 1999\n\n"
//
"The library provides utility classes.\n\n"
//
"Copyright (C) 1997-2001  Christian B.U. Perwass\n\n"
//
"This library is free software; you can redistribute it and/or\n"
"modify it under the terms of the GNU Library General Public\n"
"License as published by the Free Software Foundation; either\n"
"version 2 of the License, or (at your option) any later version.\n\n"
//
"This library is distributed in the hope that it will be useful,\n"
"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\n"
"Library General Public License for more details.\n\n"
//
"You should have received a copy of the GNU Library General Public\n"
"License along with this library; if not, write to the\n"
"Free Software Foundation, Inc., 59 Temple Place - Suite 330,\n"
"Boston, MA  02111-1307, USA.\n\n"
//
"The GNU Library General Public License can be found in the file\n"
"license.txt distributed with this library.\n\n"
//
"Please send all queries and comments regarding the XUT Library to\n\n"
//
"email: public@perwass.com\n"
"mail: Institut fuer Informatik, Preusserstr. 1-9, 24105 Kiel, Germany.\n\n";

