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

/** \file CLUDebug.h
	\author Christian Perwass
	\date June 2002
	\brief Macro definitions for Debug support.

	These Macros are only active, if CLU_DEBUG is defined
*/
#include "CLUException.h"

#if !defined(_CLU_DEBUG_H_INCLUDED_)
#define _CLU_DEBUG_H_INCLUDED_

#ifdef TRACE
#undef TRACE
#endif

#ifdef FATAL
#undef FATAL
#endif

#ifdef ERR
#undef ERR
#endif

#ifdef WARN
#undef WARN
#endif

#ifdef DEBUG
#undef DEBUG
#endif

#ifdef ASSERT
#undef ASSERT
#endif


#ifdef CLU_DEBUG

#include <iostream>

/** Macro for debug output.
	These macros only produce output if CLU_DEBUG is defined. 
	The parameters are:
		- \b iLevel: Gives the debug level. Should start at 1. The macro compares this
			value with CCLUException::sm_iDebugLevel. If it is smaller or equal, then the
			Stream is piped to cerr. Debug levels 0 to 2 are reserved for 
			Fatal Error, Error and Warning. You can set the debug level with CCLUException::SetDebugLevel(iLevel).

		- \b Stream: This is the stream to be printed.

	For example: 
		TRACE(1, "Test " << i); 
		produces
		if (1+2 <= CCLUError::sm_iDebugLevel) clog << "Test " << i << flush;
*/
#define TRACE(iLevel, Stream) {if (iLevel <= CCLUException::GetDebugLevel()) std::cerr << Stream;}

/// Output Fatal Error text
#define FATAL(Stream) TRACE(0, "Fatal: " << Stream)

/// Output Error text
#define ERR(Stream) TRACE(1, "Error: " << Stream)

/// Output Warning text
#define WARN(Stream) TRACE(2, "Warning: " << Stream)

/// Output Debug text
#define DEBUG(iLevel, Stream) TRACE(iLevel+2, "Debug " << iLevel << ": " << Stream)

/// Stream of filename and line
#define FILEPOS_STREAM "(" << __FILE__ << ", " << __LINE__ << ")"

/// Assert macro. Throws CCLUAssertion if condition is false.
#define ASSERT(theCond) if (!(theCond)) throw CLU::CCLUAssertion(__FILE__, __LINE__);

#else // No Debug

#define TRACE(iLevel, Stream)
#define FATAL(Stream)
#define ERR(Stream)
#define WARN(Stream)
#define DEBUG(iLevel, Stream)
#define FILEPOS_STREAM
#define ASSERT(theCond)

#endif


#endif

