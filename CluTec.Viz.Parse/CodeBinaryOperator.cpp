////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      CodeBinaryOperator.cpp
//
// summary:   Implements the code binary operator class
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
#include "CodeBinaryOperator.h"
#include "CodeVar.h"
#include "CodeBase.h"
#include "CLUCodeBase.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCodeBinaryOperator::CCodeBinaryOperator()
{

}

CCodeBinaryOperator::~CCodeBinaryOperator()
{

}


//////////////////////////////////////////////////////////////////////
/// Apply
///
/// General implementation of Binary Operator.
/// Calls virtual function Evaluate for actual calculation.

bool CCodeBinaryOperator::Apply(CCodeBase* pCodeBase, SCodeData *pData)
{
	if (!pCodeBase)
		return false;

	int iLine, iPos;
	CCodeVar *pvL, *pvR;

	if (pData)
	{
		iLine = pData->iTextLine;
		iPos = pData->iTextPos;
	}

	try
	{
		CCLUCodeBase *pBase = dynamic_cast<CCLUCodeBase*>(pCodeBase);
	
		if (!pBase)
		{
			pCodeBase->m_ErrorList.Internal(iLine, iPos);
			return false;
		}
	
		if (!pCodeBase->Pop(pvR))
		{
			pCodeBase->m_ErrorList.NoRVal(iLine, iPos);
			return false;
		}
			
		if (!pCodeBase->Pop(pvL))
		{
			pCodeBase->m_ErrorList.NoLVal(iLine, iPos);
			return false;
		}
	
		if (pvL->IsProtected())
		{
			pCodeBase->m_ErrorList.InvalidLVal(*pvL, iLine, iPos);
			return false;
		}
	
		CCodeVar &rResVar = pCodeBase->NewTempVar();
	
		if (!Evaluate(pBase, *pvL, *pvR, rResVar, iLine, iPos))
			return false; // Operator sets error
	
		if (!pCodeBase->Push(&rResVar))
		{
			pCodeBase->m_ErrorList.OutOfMemory(iLine, iPos);
			return false;
		}
	}
	catch(CCluOutOfMemory&)
	{
		pCodeBase->m_ErrorList.OutOfMemory(iLine, iPos);
		return false;
	}
	catch(CCluException& xEx)
	{
		pCodeBase->m_ErrorList.GeneralError(xEx.PrintError().c_str(), iLine, iPos);
		return false;
	}

	return true;
}

