////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      CodeNumber.cpp
//
// summary:   Implements the code number class
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
#include "CodeNumber.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCodeNumber::CCodeNumber()
{
	m_VarNumber.New(PDT_INT, "Constant");
	m_VarNumber = (int) 0;
	SetName("0");
}

CCodeNumber::~CCodeNumber()
{

}


//////////////////////////////////////////////////////////////////////
// Apply

bool CCodeNumber::Apply(CCodeBase* pCodeBase, SCodeData *pData)
{
	if (!pCodeBase) 
		return false;

	int iLine, iPos;
	if (pData)
	{
		iLine = pData->iTextLine;
		iPos = pData->iTextPos;
	}

	if (!pCodeBase->Push(&m_VarNumber))
	{
		pCodeBase->m_ErrorList.Internal(iLine, iPos);
		return false;
	}

	return true;
}

