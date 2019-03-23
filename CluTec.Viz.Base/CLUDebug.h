////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      CLUDebug.h
//
// summary:   Declares the clu debug class
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
#define TRACE(iLevel, Stream) {if (iLevel <= CCluException::GetDebugLevel()) std::cerr << Stream;}

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
#define ASSERT(theCond) if (!(theCond)) throw CLU::CCluAssertion(__FILE__, __LINE__);

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

