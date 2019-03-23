////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      CodeCreateRefVarList.cpp
//
// summary:   Implements the code create reference variable list class
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
#include "CodeCreateRefVarList.h"
#include "CodeBase.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCodeCreateRefVarList::CCodeCreateRefVarList()
{
	m_pCodeList = 0;
}

CCodeCreateRefVarList::~CCodeCreateRefVarList()
{
}

//////////////////////////////////////////////////////////////////////
/// Apply
///
/// Takes all vars from stack an places them into a var list onto the stack.

bool CCodeCreateRefVarList::Apply(CCodeBase* pCodeBase, SCodeData* pData)
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
			// Check whether variable from stack is actually a non-temporary
			// variable. If this is the case, then only store reference
			// to variable in list.
			if (pVar->Type() == PDT_PTR_VAR)
			{
				CCodeVar& rV = pVar->DereferenceVarPtr();
				if (rV.Type() == PDT_PTR_VAR)
				{
					rVarList[iStackIdx].CopyInstance(rV);
				}
				else
				{
					rVarList[iStackIdx].CopyInstance(*pVar);
				}
			}
			// If pVar is a temporary variable it is forgotten in the temp var list
			// and replaces a variable on the stack.
			// Otherwise the variable is placed on the stack.
			if (pCodeBase->ForgetTempVar(pVar))
			{
				// all temp vars are protected so that they are not written to
				// However, now the variable becomes part of a var list and may
				// be overwritten.
				pVar->EnableProtect(false);
				rVarList.Replace(iStackIdx, pVar);
			}
			else
			{
				rVarList[iStackIdx]        = pVar;
				rVarList[iStackIdx].Name() = "Unnamed";
			}

			--iStackIdx;
		}

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
