////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.OpenGL
// file:      Defines.h
//
// summary:   Declares the defines class
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

#define OGL_MAX_TEX_UNITS       8

// GLOBAL CLUVIZ DEFINES for vertex attribute arrays (VAA)
#define CLUGL_VAA_VERTEX        0
#define CLUGL_VAA_COLOR         1
#define CLUGL_VAA_NORMAL        2
#define CLUGL_VAA_TEX0          3
#define CLUGL_VAA_TEX1          4
#define CLUGL_VAA_TEX2          5
#define CLUGL_VAA_TEX3          6
#define CLUGL_VAA_TEX4          7
#define CLUGL_VAA_TEX5          8
#define CLUGL_VAA_TEX6          9
#define CLUGL_VAA_TEX7          10
#define CLUGL_VAA_PARTID        11

#ifdef _DEBUG
#define CLU_OGL_CALL(theCall) \
		theCall;                   \
		Clu::OpenGL::EnsureOK(#theCall, __FILE__, __FUNCTION__, __LINE__);
#else
#define CLU_OGL_CALL(theCall) theCall
#endif
