////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      CodeData.cpp
//
// summary:   Implements the code data class
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
#include "CodeData.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCodeData::CCodeData()
{
	m_VarData.New(PDT_INT, "Constant");
	m_VarData = (int) 0;
	SetName("Unnamed");

}

CCodeData::~CCodeData()
{
	// If the data stored in m_VarData is the pointer to code,
	// then delete the memory reserved for the code.
	if (m_VarData.BaseType() == PDT_CODEPTR)
	{
		TCodePtr *pCode = m_VarData.GetCodePtrPtr();
		if (*pCode)
			delete (*pCode);
	}
}


//////////////////////////////////////////////////////////////////////
// Apply

bool CCodeData::Apply(CCodeBase* pCodeBase, SCodeData *pData)
{
	if (!pCodeBase) 
		return false;

	int iLine, iPos;
	if (pData)
	{
		iLine = pData->iTextLine;
		iPos = pData->iTextPos;
	}

	if (!pCodeBase->Push(&m_VarData))
	{
		pCodeBase->m_ErrorList.Internal(iLine, iPos);
		return false;
	}

	return true;
}

