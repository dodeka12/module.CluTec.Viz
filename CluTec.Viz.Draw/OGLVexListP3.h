////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLVexListP3.h
//
// summary:   Declares the ogl vex list p 3 class
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

// OGLP3VexList.h: Schnittstelle f�r die Klasse COGLVexListP3.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLP3VEXLIST_H__BA08CC8F_FAE3_42EC_BFF9_0DA39D097EC7__INCLUDED_)
#define AFX_OGLP3VEXLIST_H__BA08CC8F_FAE3_42EC_BFF9_0DA39D097EC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OGLVertexList.h"

class CLUDRAW_API COGLVexListP3 : public COGLVertexList  
{
public:
	COGLVexListP3();
	virtual ~COGLVexListP3();

	COGLVertexList& operator<< (const MultiV<float>& vA)
	{ AddMV(vA); return *this; }

	bool AddMV(const MultiV<float>& vA);

};

#endif // !defined(AFX_OGLP3VEXLIST_H__BA08CC8F_FAE3_42EC_BFF9_0DA39D097EC7__INCLUDED_)
