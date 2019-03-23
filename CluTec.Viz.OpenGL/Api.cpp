////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.OpenGL
// file:      Api.cpp
//
// summary:   Implements the API class
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

#include "stdafx.h"

#include <cinttypes>
#include <sstream>

#include "Api.h"
#include "CluTec.Base/Exception.h"

// TODO: Unified OpenGL error reporting


namespace Clu
{
	namespace OpenGL
	{
		bool IsOK(GLenum& eErrorId)
		{
			return ((eErrorId = glGetError()) == GL_NO_ERROR);
		}

		std::string GetErrorText(GLenum eErrorId, const std::string& sFuncCall)
		{
			std::stringstream sxText;
			sxText
				<< "OpenGL error ("
				<< static_cast<uint32_t>(eErrorId)
				<< "): "
				<< Clu::OpenGL::GetErrorString(eErrorId)
				<< " during call: "
				<< sFuncCall;

			return sxText.str();
		}

		void EnsureOK(const std::string& sFuncCall, const char* const pcFile, const char* const pcFunc, int const iLine)
		{
			GLenum eErrorId;

			if (!IsOK(eErrorId))
			{
				throw CreateExceptionOpenGL(eErrorId, sFuncCall, pcFile, pcFunc, iLine);
			}

		}

		std::string GetErrorString(GLenum eErrorId)
		{
			switch (eErrorId)
			{
			case GL_NO_ERROR:
				return "No error has been recorded";

			case GL_INVALID_ENUM:
				return "An unacceptable value is specified for an enumerated argument";

			case GL_INVALID_VALUE:
				return "A numeric argument is out of range";

			case GL_INVALID_OPERATION:
				return "The specified operation is not allowed in the current state";

			case GL_INVALID_FRAMEBUFFER_OPERATION:
				return "The frame buffer object is not complete";

			case GL_OUT_OF_MEMORY:
				return "There is not enough memory left to execute the command";

			case GL_STACK_UNDERFLOW:
				return "An attempt has been made to perform an operation that would cause an internal stack to underflow";

			case GL_STACK_OVERFLOW:
				return "An attempt has been made to perform an operation that would cause an internal stack to overflow";

			default:
				return "Unknown OpenGL error";
			}
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Deletes the textures. We use this function to implement a memory tracking for OpenGL at some
		/// point.
		/// </summary>
		///
		/// <remarks>	Perwass. </remarks>
		///
		/// <param name="uCount">	Number of. </param>
		/// <param name="puId">  	[in,out] If non-null, identifier for the pu. </param>
		////////////////////////////////////////////////////////////////////////////////////////////////////

		void DeleteTextures(unsigned uCount, unsigned* puId)
		{
			glDeleteTextures((GLsizei)uCount, (GLuint *)puId);

		}

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>	Deletes the buffers. </summary>
		///
		/// <remarks>	Perwass,. </remarks>
		///
		/// <param name="uCount">	Number of. </param>
		/// <param name="puId">  	[in,out] If non-null, identifier for the pu. </param>
		////////////////////////////////////////////////////////////////////////////////////////////////////

		void DeleteBuffers(unsigned uCount, unsigned* puId)
		{
			glDeleteBuffers(uCount, puId);
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>	Buffer data. </summary>
		///
		/// <remarks>	Perwass,. </remarks>
		///
		/// <param name="eTarget">	Target for the. </param>
		/// <param name="nSize">  	The size. </param>
		/// <param name="pvData"> 	Information describing the pv. </param>
		/// <param name="eUsage"> 	The usage. </param>
		////////////////////////////////////////////////////////////////////////////////////////////////////

		void BufferData(GLenum eTarget, GLsizeiptr nSize, const void* pvData, GLenum eUsage)
		{
			glBufferData(eTarget, nSize, pvData, eUsage);
		}


		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>	Tex image 2D. </summary>
		///
		/// <remarks>	Perwass,. </remarks>
		///
		/// <param name="eTarget">		  	Target for the. </param>
		/// <param name="iLevel">		  	Zero-based index of the level. </param>
		/// <param name="iInternalformat">	Zero-based index of the internalformat. </param>
		/// <param name="uWidth">		  	The width. </param>
		/// <param name="uHeight">		  	The height. </param>
		/// <param name="iBorder">		  	Zero-based index of the border. </param>
		/// <param name="eFormat">		  	Describes the format to use. </param>
		/// <param name="eType">		  	The type. </param>
		/// <param name="pvPixels">		  	The pv pixels. </param>
		////////////////////////////////////////////////////////////////////////////////////////////////////

		void TexImage2D(GLenum eTarget,
			GLint iLevel,
			GLint iInternalformat,
			GLsizei uWidth,
			GLsizei uHeight,
			GLint iBorder,
			GLenum eFormat,
			GLenum eType,
			const GLvoid* pvPixels)
		{
			glTexImage2D(eTarget, iLevel, iInternalformat, uWidth, uHeight, iBorder, eFormat, eType, pvPixels);
		}

	}
}