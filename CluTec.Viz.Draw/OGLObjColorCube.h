////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLObjColorCube.h
//
// summary:   Declares the ogl object color cube class
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

// OGLObjColorCube.h: Schnittstelle f�r die Klasse COGLObjColorCube.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLOBJCOLORCUBE_H__D388CB5B_C598_478B_A941_A92B08B422E6__INCLUDED_)
#define AFX_OGLOBJCOLORCUBE_H__D388CB5B_C598_478B_A941_A92B08B422E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OGLVertexList.h"


class COGLObjColorCube  
{
public:
	COGLObjColorCube();
	virtual ~COGLObjColorCube();

	void Init();
	bool Apply();

protected:
	int m_iListIdx;

	COGLVertexList m_VexList;
};

#endif // !defined(AFX_OGLOBJCOLORCUBE_H__D388CB5B_C598_478B_A941_A92B08B422E6__INCLUDED_)
