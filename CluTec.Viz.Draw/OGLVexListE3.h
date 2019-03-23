////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLVexListE3.h
//
// summary:   Declares the ogl vex list e 3 class
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

// OGLE3VexList.h: Schnittstelle f�r die Klasse COGLVexListE3.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLE3VEXLIST_H__F83D17C6_8700_41E2_9BDA_0D9BC690F671__INCLUDED_)
#define AFX_OGLE3VEXLIST_H__F83D17C6_8700_41E2_9BDA_0D9BC690F671__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OGLVertexList.h"

class CLUDRAW_API COGLVexListE3 : public COGLVertexList  
{
public:
	COGLVexListE3();
	virtual ~COGLVexListE3();

	COGLVertexList& operator<< (const MultiV<float>& vA)
	{ AddMV(vA); return *this; }

	bool AddMV(const MultiV<float>& vA)
	{ return AddVex(&vA[1]); }

};

#endif // !defined(AFX_OGLE3VEXLIST_H__F83D17C6_8700_41E2_9BDA_0D9BC690F671__INCLUDED_)
