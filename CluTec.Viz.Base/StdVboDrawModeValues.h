////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      StdVboDrawModeValues.h
//
// summary:   Declares the standard vbo draw mode values class
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

#ifdef CLU_MANAGED
public enum class GlDrawModes
#else
enum class EGlDrawMode
#endif
{
	POINTS = 0x0000,
	LINES                    = 0x0001,
	LINE_LOOP                = 0x0002,
	LINE_STRIP               = 0x0003,
	TRIANGLES                = 0x0004,
	TRIANGLE_STRIP           = 0x0005,
	TRIANGLE_FAN             = 0x0006,
	QUADS                    = 0x0007,
	QUAD_STRIP               = 0x0008,
	POLYGON                  = 0x0009,
	LINES_ADJACENCY          = 0x000A,
	LINE_STRIP_ADJACENCY     = 0x000B,
	TRIANGLES_ADJACENCY      = 0x000C,
	TRIANGLE_STRIP_ADJACENCY = 0x000D,
};
