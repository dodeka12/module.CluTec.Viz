////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      CLUException.h
//
// summary:   Declares the clu exception class
//
//            Copyright (c) 2019 by Christian Perwass.
//
//            This file is part of the CluTecLib library.
//
//            The CluTecLib library is free software: you can redistribute it and / or modify
//            it under the terms of the GNU Lesser General Public License as published by
//            the Free Software Foundation, either version 3 of the License, or
//            (at your option) any later version.
//
//            The CluTecLib library is distributed in the hope that it will be useful,
//            but WITHOUT ANY WARRANTY; without even the implied warranty of
//            MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//            GNU Lesser General Public License for more details.
//
//            You should have received a copy of the GNU Lesser General Public License
//            along with the CluTecLib library.
//            If not, see <http://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////////////////////////

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
class CCluException
{
public:
	typedef unsigned char uchar;

public:
	/** \brief Constructor sets the error level and location text. */
	CCluException(int iLevel);

	CCluException(int iLevel, const char* pcMessage);
	CCluException(int iLevel, const char* pcMessage, const char* pcFile, const char* pcFunction, int iLine);

	virtual ~CCluException();

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
	void Init(int iLevel, const char* pcMessage, const char* pcFile, const char* pcFunction, int iLine);

	/** \brief Adds given number to m_sNumberText. */
	void AddNumberToText(uchar ucNumber);

protected:
	int m_iLevel;	///< Error level of current message.

	/** \brief Text describing actual cause of error. */
	string m_sErrorText;

	/** \brief Text containing Error Number */
	string m_sNumberText;

	string m_sFile;
	string m_sFunction;
	int m_iLine;

	static int sm_iDebugLevel;	///< Maximum error level to be displayed.
};

/** \brief Fundamental Exception Class for fatal errors. 
	\author Christian Perwass
	\date June 2002
	\ingroup exception 
*/
class CCluFatalError : public CCluException
{
public:
	CCluFatalError() : CCluException(0) { }
	CCluFatalError(const char* pcText) : CCluException(0, pcText) {  }
	CCluFatalError(const char* pcText, const char* pcFile, const char* pcFunction, int iLine)
		: CCluException(0, pcText, pcFile, pcFunction, iLine) {}
};

/** \brief Fundamental Exception Class for assertions. 
	\author Christian Perwass
	\date June 2002
	\ingroup exception 
*/
class CCluAssertion : public CCluFatalError
{
public:
	CCluAssertion(const char* pcFile, int iLine) 
		: CCluFatalError("Assertion failed: ", pcFile, 0, iLine) {}

	CCluAssertion(const char* pcFile, const char* pcFunction, int iLine)
		: CCluFatalError("Assertion failed: ", pcFile, pcFunction, iLine) {}
};
/** \brief Fundamental Exception Class for normal errors. 
	\author Christian Perwass
	\date June 2002
	\ingroup exception
*/
class CCluError : public CCluException
{
public:
	CCluError() : CCluException(1) { }
	CCluError(const char* pcText) : CCluException(1, pcText) {  }
};

/** \brief Fundamental Exception Class for warnings. 
	\author Christian Perwass
	\date June 2002
	\ingroup exception 
*/
class CCluWarning : public CCluException
{
public:
	CCluWarning() : CCluException(2) { }
	CCluWarning(const char* pcText) : CCluException(2, pcText) { }
};

/** \brief Fundamental Exception Class for out of memory errors. 
	\author Christian Perwass
	\date June 2002
	\ingroup exception 
*/
class CCluOutOfMemory : public CCluFatalError
{
public:
	CCluOutOfMemory() 
		: CCluFatalError("Out of Memory") {}

	CCluOutOfMemory(const char* pcFile, const char* pcFunction, int iLine)
		: CCluFatalError("Out of memory.", pcFile, pcFunction, iLine) {}

	CCluOutOfMemory(const char *pcText)
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
class CCluOutOfRange : public CCluError
{
public:
	CCluOutOfRange()
		: CCluError("Index out of range") {}

	CCluOutOfRange(const char *pcText)
	{
		m_sErrorText = pcText;
		m_sErrorText += ": Index out of range.";
	}

};

#endif // !defined(_CLU_ERROR_H_INCLUDED_)

