////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      CodeCreateVarList.cpp
//
// summary:   Implements the code create variable list class
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
#include "CodeCreateVarList.h"
#include "CodeBase.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCodeCreateVarList::CCodeCreateVarList()
{
	m_pCodeList = 0;
}

CCodeCreateVarList::~CCodeCreateVarList()
{
}

//////////////////////////////////////////////////////////////////////
/// Apply
///
/// Takes all vars from stack and places them into a var list onto the stack.

bool CCodeCreateVarList::Apply(CCodeBase* pCodeBase, SCodeData* pData)
{
	if (!pCodeBase)
	{
		return false;
	}

	int iLine, iPos;
	if (pData)
	{
		iLine = pData->iTextLine;
		iPos  = pData->iTextPos;
	}

	CCodeVar* pVar;

	try
	{
		CCodeVar& rResVar  = pCodeBase->NewTempVar(PDT_VARLIST);
		TVarList& rVarList = *((TVarList*) rResVar.Val());

		rVarList.SetCodeList(m_pCodeList);

		int iStackDepth = pCodeBase->GetActStackDepth();
		int iStackIdx   = iStackDepth - 1;

		if (!rVarList.Set(iStackDepth))
		{
			pCodeBase->m_ErrorList.OutOfMemory(iLine, iPos);
			return false;
		}

		while (pCodeBase->Pop(pVar))
		{
			CCodeVar& rBaseVar = pVar->DereferenceVarPtr(true);

			// Test whether given variable is a temporary variable
			// and if yes, move it from the temp var list to this var list.
			if (pCodeBase->ForgetTempVar(&rBaseVar))
			{
				// all temp vars are protected so that they are not written to
				// However, now the variable becomes part of a var list and may
				// be overwritten.
				rBaseVar.EnableProtect(false);
				rVarList.Replace(iStackIdx, &rBaseVar);
			}
			else
			{
				rVarList[iStackIdx].CopyInstance(rBaseVar);
			}

			// Do not store pointers in a constant var list
			rVarList[iStackIdx].DereferencePtr();

			--iStackIdx;
		}

		// Protect constant list of variables
		rResVar.EnableProtect();

		// Put var list on stack.
		if (!pCodeBase->Push(&rResVar))
		{
			pCodeBase->m_ErrorList.OutOfMemory(iLine, iPos);
			return false;
		}
	}
	catch (CCluOutOfMemory&)
	{
		pCodeBase->m_ErrorList.OutOfMemory(iLine, iPos);
		return false;
	}
	catch (CCluException& xEx)
	{
		pCodeBase->m_ErrorList.GeneralError(xEx.PrintError().c_str(), iLine, iPos);
		return false;
	}

	return true;
}
