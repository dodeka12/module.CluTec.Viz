////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.OpenGL
// file:      Exception.h
//
// summary:   Declares the exception class
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

#pragma once

#include "CluTec.Types1/ExceptionTypes.h"
#include "CluTec.Types1/IException.h"

namespace Clu
{
	namespace ExceptionType
	{
		class OpenGL : public Clu::ExceptionType::Unknown
		{
		public:
			// {949CC63B-D9D8-4D74-8C76-E51F7C808C66}
			static const CGuid Guid;
			static const char* TypeName;

		public:
			OpenGL() : Unknown(OpenGL::Guid, OpenGL::TypeName)
			{}

		};
	}


#define CLU_EXCEPT_OPENGL(theErrorId, theFuncCall) \
	Clu::CreateExceptionOpenGL(theErrorId, theFuncCall, __FILE__, __FUNCTION__, __LINE__)

#define CLU_EXCEPT_OPENGL_NEST(theErrorId, theFuncCall, theEx) \
	Clu::CreateExceptionOpenGL(theErrorId, theFuncCall, __FILE__, __FUNCTION__, __LINE__, theEx)

	template<typename T>
	inline CIException CreateExceptionOpenGL(unsigned uGlErrorId, const T& xMsg, const char* pcFile, const char* pcFunc, const int & iLine)
	{
		std::string sText = Clu::OpenGL::GetErrorText(uGlErrorId, ToStdString(xMsg));

		return CIException(Clu::ExceptionType::OpenGL(), ToIString(sText), ToIString(pcFile), ToIString(pcFunc), iLine);
	}

	template<typename T>
	inline CIException CreateExceptionOpenGL(unsigned uGlErrorId, const T& xMsg, const char* pcFile, const char* pcFunc, const int & iLine, CIException&& xEx)
	{
		std::string sText = Clu::OpenGL::GetErrorText(uGlErrorId, ToStdString(xMsg));

		return CIException(Clu::ExceptionType::OpenGL(), ToIString(sText), ToIString(pcFile), ToIString(pcFunc), iLine, std::forward<CIException>(xEx));
	}

}
