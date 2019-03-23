////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      OCOpStd.cpp
//
// summary:   Implements the oc operation standard class
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

// OCOpStd.cpp: Implementierung der Klasse COCOpStd.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OCOpStd.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COCOpStd::COCOpStd()
{

}

COCOpStd::~COCOpStd()
{

}


//////////////////////////////////////////////////////////////////////
/// Init

bool COCOpStd::Init(const char *pcSym, int iOpLevelUnary, int iOpLevelBinary, 
					bool bRightToLeft, bool bVoidOp, bool bRecurseLists,
					TOpUnaryPtr UFunc, TOpBinaryPtr BFunc,
					TOpAUnaryPtr AUFunc, TOpABinaryPtr ABFunc)
{
	m_pCodeOp = &m_Operator;

	SetOpLevel(iOpLevelBinary, iOpLevelUnary);
	SetOrderRightToLeft(bRightToLeft);
	SetVoidOp(bVoidOp);
	SetOpSymbol(pcSym);

	m_Operator.Init(UFunc, BFunc, AUFunc, ABFunc, bRecurseLists);

	return true;
}
