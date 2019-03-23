////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.OpenGL
// file:      Api.h
//
// summary:   Declares the API class
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

#include <string>

#include "Defines.h"
#include "CluTec.Viz.OpenGL.Extensions/Extensions.h"

#include "Exception.h"

namespace Clu
{
	namespace OpenGL
	{
		bool IsOK(GLenum& eErrorId);
		std::string GetErrorText(GLenum eErrorId, const std::string& sFuncCall);
		void EnsureOK(const std::string& sFuncCall, const char* const pcFile, const char* const pcFunc, int const iLine);

		std::string GetErrorString(GLenum eErrorId);

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>	Deletes the textures. </summary>
		///
		/// <remarks>	Perwass,. </remarks>
		///
		/// <param name="uCount">	Number of. </param>
		/// <param name="puId">  	[in,out] If non-null, identifier for the pu. </param>
		////////////////////////////////////////////////////////////////////////////////////////////////////

		void DeleteTextures(unsigned uCount, unsigned* puId);

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>	Deletes the buffers. </summary>
		///
		/// <remarks>	Perwass,. </remarks>
		///
		/// <param name="uCount">	Number of. </param>
		/// <param name="puId">  	[in,out] If non-null, identifier for the pu. </param>
		////////////////////////////////////////////////////////////////////////////////////////////////////

		void DeleteBuffers(unsigned uCount, unsigned* puId);

		void BufferData(GLenum eTarget, GLsizeiptr nSize, const void* pvData, GLenum eUsage);

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
			const GLvoid* pvPixels);
	}
}
