////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLVexListP3.cpp
//
// summary:   Implements the ogl vex list p 3 class
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

// OGLP3VexList.cpp: Implementierung der Klasse COGLVexListP3.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OGLVexListP3.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLVexListP3::COGLVexListP3()
{

}

COGLVexListP3::~COGLVexListP3()
{

}

//////////////////////////////////////////////////////////////////////
/// Add Vector

bool COGLVexListP3::AddMV(const MultiV<float>& vA)
{ 
	float fVal = vA[4]; 
	
	if (fVal == 0.0f)
		fVal = 1.0f;

	return AddVex(vA[1]/fVal, vA[2]/fVal, vA[3]/fVal); 
}
