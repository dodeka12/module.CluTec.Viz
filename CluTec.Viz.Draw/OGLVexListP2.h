////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLVexListP2.h
//
// summary:   Declares the ogl vex list p 2 class
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

// OGLVexListP2.h: Schnittstelle f�r die Klasse COGLVexListP2.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLVEXLISTP2_H__88EFD499_27CB_4099_A32C_2C6A0BE22023__INCLUDED_)
#define AFX_OGLVEXLISTP2_H__88EFD499_27CB_4099_A32C_2C6A0BE22023__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OGLVertexList.h"

class CLUDRAW_API COGLVexListP2 : public COGLVertexList  
{
public:
	COGLVexListP2();
	virtual ~COGLVexListP2();

	COGLVertexList& operator<< (const MultiV<float>& vA)
	{ AddMV(vA); return *this; }

	bool AddMV(const MultiV<float>& vA)
	{ return AddVex(vA[1], vA[2], vA[4]); }

};

#endif // !defined(AFX_OGLVEXLISTP2_H__88EFD499_27CB_4099_A32C_2C6A0BE22023__INCLUDED_)
