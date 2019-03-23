////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.OpenGL
// file:      Exception.cpp
//
// summary:   Implements the exception class
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

#include "Exception.h"

namespace Clu
{
	namespace ExceptionType
	{

		// {949CC63B-D9D8-4D74-8C76-E51F7C808C66}
		const CGuid OpenGL::Guid = CGuid(0x949cc63b, 0xd9d8, 0x4d74, 0x8c, 0x76, 0xe5, 0x1f, 0x7c, 0x80, 0x8c, 0x66);
		const char* OpenGL::TypeName = "OpenGL Runtime Error";

	}
}
