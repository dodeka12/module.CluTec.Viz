////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      StdVbo.h
//
// summary:   Declares the standard vbo class
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

#include "StdVboDrawMode.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// namespace: Clu
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Clu
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// 	A standard vertex.
	/// </summary>
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct SStdVertex
	{
		/// <summary> Vertex coordinate vector. </summary>
		float pfVex[3];
		/// <summary> Texture coordinate vector. </summary>
		float pfTex[3];
		/// <summary> Normal coordinate vector. </summary>
		float pfNorm[3];
		/// <summary> RGBA color value. </summary>
		float pfCol[4];
		/// <summary> Fog z-component. </summary>
		float fFogZ;
		/// <summary> Part ID for picking of object parts. </summary>
		unsigned uPartId;
		/// <summary> Edge flag. </summary>
		char bEdge;
		/// <summary> Padding (reserved for future use). </summary>
		char pbPad[3];

		// Vertex array offsets for different data types.
		// Can be used in calls to glVertexPointer(), glColorPointer() etc.
		static const int iOffsetVex    = 0;
		static const int iOffsetTex    = 12;
		static const int iOffsetNorm   = 24;
		static const int iOffsetCol    = 36;
		static const int iOffsetFog    = 52;
		static const int iOffsetPartId = 56;
		static const int iOffsetEdge   = 60;
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// 	A standard vertex buffer object (VBO).
	/// </summary>
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct SStdVbo
	{
		/// <summary> Vertex array mode. </summary>
		EGlDrawMode eMode;
		/// <summary> Number of vertices in this VBO. </summary>
		int iVertexCount;
		/// <summary> Pointer to the array of vertices. </summary>
		SStdVertex* pxVertices;

		/// <summary> Array of pointers to index lists. </summary>
		unsigned** ppuIndexLists;
		/// <summary> Array of corresponding index list lengths. </summary>
		unsigned* puIndexListLengths;
		/// <summary> Number of index lists. </summary>
		unsigned uIndexListCount;

		/// <summary> External created Vertex Buffer Object. </summary>
		unsigned uVertexBufferID;
		/// <summary> External created Index Buffer Objects. </summary>
		unsigned uIndexBufferID;

		// List of enabled client vertex arrays
		bool bUseVertex;
		bool bUseTexture;
		bool bUseNormal;
		bool bUseColor;
		bool bUseFogZ;
		bool bUseEdge;
		bool bUsePartId;
		bool bReserved2;
		bool bReserved3;
		bool bReserved4;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Resets this object.
		/// </summary>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void Reset()
		{
			eMode        = EGlDrawMode::POINTS;
			iVertexCount = 0;
			pxVertices   = nullptr;

			ppuIndexLists      = nullptr;
			puIndexListLengths = nullptr;
			uIndexListCount    = 0;

			uIndexBufferID  = 0;
			uVertexBufferID = 0;

			bUseVertex  = false;
			bUseTexture = false;
			bUseNormal  = false;
			bUseColor   = false;
			bUseFogZ    = false;
			bUseEdge    = false;
			bUsePartId  = false;
			bReserved2  = false;
			bReserved3  = false;
			bReserved4  = false;
		}
	};
}

#pragma make_public(Clu::SStdVbo)
#pragma make_public(Clu::SStdVertex)
