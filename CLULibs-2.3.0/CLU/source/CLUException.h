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

/**	\file CLUException.h
   	\author Christian Perwass
	\date June 2002
	\brief Classes for dealing with exceptions in CLU.
*/

#if !defined(_CLU_EXCEPTION_H_INCLUDED_)
#define _CLU_EXCEPTION_H_INCLUDED_

#include <string>
#include <sstream>

// Avoid warnings that declaration of possible exceptions thrown
// by functions is not supported by Visual C++ .NET.
#ifdef WIN32
#pragma warning( disable : 4290 )
#endif

using std::string;
using std::stringstream;

/** \brief Fundamental Exception Class for CLU Library.

	This class implements the basic functionality of all exception or error 
	classes in the CLU Library. It also offers debug functionality with debug levels.

	Every error has to be created with an error level. They are defined
	as follows:
		- <b>Level 0:</b> Fatal error. This type of error is unrecoverable. 
			That is, the program cannot keep on running, e.g. not enough memory,
			memory leak detected, unexpected behaviour.

		- <b>Level 1:</b> Error. This type of error indicates that an asked
			for functionality could not be performed successfully, e.g. loading a
			file, loading a module.

		- <b>Level 2:</b> Warning. A warning is given if the program can still
			function but may not function in a way expected by the user.

		- <b>Level 3, 4, ...:</b> Debug message. Message passed for debug purposes.

	Every error also has a 32bit number. The number has the following structure:
		- The highest 8bit give the number of the CLU Library.
		- The remaining bits give the error number defined locally in the library.

	\author Christian Perwass
	\date June 2002
	\ingroup exception
*/
class CCLUException
{
public:
	typedef unsigned char uchar;

public:
	/** \brief Constructor sets the error level and location text. */
	CCLUException(int iLevel);

	virtual ~CCLUException();

	/** \brief Get the current error level.	*/
	int GetErrorLevel() { return m_iLevel; }

	/** \brief Get the error string. */
	const char* GetErrorText() const { return m_sErrorText.c_str(); }

	/** \brief Get the number string. */
	const char* GetNumberText() const { return m_sNumberText.c_str(); }

	/** \brief Print errors in some user readable way. */
	virtual string PrintError();

	/** \brief Sets the debug level for all CLU Errors. */
	static void SetDebugLevel(int iLevel) { sm_iDebugLevel = iLevel; }
	static int GetDebugLevel() { return sm_iDebugLevel; }

protected:
	/** \brief Adds given number to m_sNumberText. */
	void AddNumberToText(uchar ucNumber);

protected:
	int m_iLevel;	///< Error level of current message.

	/** \brief Text describing actual cause of error. */
	string m_sErrorText;

	/** \brief Text containing Error Number */
	string m_sNumberText;

	static int sm_iDebugLevel;	///< Maximum error level to be displayed.
};

/** \brief Fundamental Exception Class for fatal errors. 
	\author Christian Perwass
	\date June 2002
	\ingroup exception 
*/
class CCLUFatalError : public CCLUException
{
public:
	CCLUFatalError() : CCLUException(0) { }
	CCLUFatalError(const char* pcText) : CCLUException(0) { m_sErrorText = pcText; }

};

/** \brief Fundamental Exception Class for assertions. 
	\author Christian Perwass
	\date June 2002
	\ingroup exception 
*/
class CCLUAssertion : public CCLUFatalError
{
public:
	CCLUAssertion(const char* pcFile, int iLine)
	{
		stringstream sText;

		sText << "Assertion failed: " << pcFile
			<< ", line " << iLine << ".";

		m_sErrorText = sText.str();
	}
};
/** \brief Fundamental Exception Class for normal errors. 
	\author Christian Perwass
	\date June 2002
	\ingroup exception
*/
class CCLUError : public CCLUException
{
public:
	CCLUError() : CCLUException(1) { }
	CCLUError(const char* pcText) : CCLUException(1) { m_sErrorText = pcText; }
};

/** \brief Fundamental Exception Class for warnings. 
	\author Christian Perwass
	\date June 2002
	\ingroup exception 
*/
class CCLUWarning : public CCLUException
{
public:
	CCLUWarning() : CCLUException(2) { }
	CCLUWarning(const char* pcText) : CCLUException(2) { m_sErrorText = pcText; }
};

/** \brief Fundamental Exception Class for out of memory errors. 
	\author Christian Perwass
	\date June 2002
	\ingroup exception 
*/
class CCLUOutOfMemory : public CCLUError
{
public:
	CCLUOutOfMemory()
	{
		m_sErrorText = "Out of Memory.";
	}

	CCLUOutOfMemory(const char *pcText)
	{
		m_sErrorText = pcText;
		m_sErrorText += ": Out of Memory.";
	}
};

/** \brief Fundamental Exception Class for out of memory errors. 
	\author Christian Perwass
	\date January 2003
	\ingroup exception 
*/
class CCLUOutOfRange : public CCLUError
{
public:
	CCLUOutOfRange()
	{
		m_sErrorText = "Index out of range.";
	}

	CCLUOutOfRange(const char *pcText)
	{
		m_sErrorText = pcText;
		m_sErrorText += ": Index out of range.";
	}

};

#endif // !defined(_CLU_ERROR_H_INCLUDED_)

