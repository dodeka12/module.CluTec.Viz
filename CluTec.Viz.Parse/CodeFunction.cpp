////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      CodeFunction.cpp
//
// summary:   Implements the code function class
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
#include "CodeFunction.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCodeFunction::CCodeFunction()
{
}

CCodeFunction::CCodeFunction(const CCodeFunction& rCodeFunc)
{
	*this = rCodeFunc;
}

CCodeFunction::~CCodeFunction()
{
}

CCodeFunction& CCodeFunction::operator=(const CCodeFunction& rCodeFunc)
{
	SetName(rCodeFunc.GetName());
	m_pFunc = rCodeFunc.m_pFunc;

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Apply
///
/// Standard function apply

bool CCodeFunction::Apply(CCodeBase* pCodeBase, SCodeData* pData)
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

	try
	{
		CCodeVar& rVar = pCodeBase->NewTempVar();
		CCodeVar* pVar;

		if (!pCodeBase->Pop(pVar) ||
		    (pVar->Type() == PDT_NOTYPE))
		{
			pCodeBase->m_ErrorList.NoFuncParams(iLine, iPos);
			return false;
		}

		if (pVar->Type() != PDT_VARLIST)
		{
			pCodeBase->m_ErrorList.NoFuncParams(iLine, iPos);
			return false;
		}

		#if defined(_DEBUG) && defined(CLU_TRACE)
			CStrMem sText;
			sText = "\nCLUViz>  Function call: " + GetName() + "\n";

			::OutputDebugStringA(sText.Str());
		#endif

		// Call Function
		if (!m_pFunc(*((CCLUCodeBase*) pCodeBase), rVar, *pVar, iLine, iPos))
		{
			return false;
		}

		if (rVar.Type() != PDT_NOTYPE)
		{
			rVar.EnableProtect();
			if (!pCodeBase->Push(&rVar))
			{
				pCodeBase->m_ErrorList.OutOfMemory(iLine, iPos);
				return false;
			}
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
