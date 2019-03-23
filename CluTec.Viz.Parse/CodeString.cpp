////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      CodeString.cpp
//
// summary:   Implements the code string class
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

#include "StdAfx.h"
#include "CodeString.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCodeString::CCodeString()
{
	m_VarString.New(PDT_STRING, "Constant");
	m_VarString = "";
	SetName("<Empty>");
}

CCodeString::~CCodeString()
{

}
//////////////////////////////////////////////////////////////////////
// Apply

bool CCodeString::Apply(CCodeBase* pCodeBase, SCodeData *pData)
{
	if (!pCodeBase) 
		return false;

	int iLine, iPos;
	if (pData)
	{
		iLine = pData->iTextLine;
		iPos = pData->iTextPos;
	}

	if (!pCodeBase->Push(&m_VarString))
	{
		pCodeBase->m_ErrorList.Internal(iLine, iPos);
		return false;
	}

	return true;
}
