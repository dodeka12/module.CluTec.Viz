////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      OCOpOpenRoundBracket.cpp
//
// summary:   Implements the oc operation open round bracket class
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

// OCOpOpenRoundBracket.cpp: Implementierung der Klasse COCOpOpenRoundBracket.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OCOpOpenRoundBracket.h"
#include "ParseBase.h"
#include "CodeElementList.h"
#include "OCIDSymDef.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COCOpOpenRoundBracket::COCOpOpenRoundBracket()
{
	m_csIDSymbolSpace = "(";
}

COCOpOpenRoundBracket::~COCOpOpenRoundBracket()
{

}


//////////////////////////////////////////////////////////////////////
/// Parse Loop

bool COCOpOpenRoundBracket::Parse(CParseBase& rParse, CCodeElementList& rCode)
{
	rParse.IncrRBC();

	CCodeElementList *pMainList, *pElList;

	bool bCloseBracket = false;
	bool bCreateRefVarList = false;
	int iLine, iPos;
	rParse.GetCurLinePos(iLine, iPos);

	CObjectClass *pOC = rParse.GetPrevObjectClass();
	if (pOC && pOC->m_csIDSymbolSpace == OC_IDSYM_CALL_STR)
		bCreateRefVarList = true;

	if (!(pMainList = new CCodeElementList))
	{
		rParse.SetError(CPB_OUTOFMEMORY);
		return false;
	}

	if (!rCode.Add(*pMainList, true, iLine, iPos))
	{
		rParse.SetError(CPB_OUTOFMEMORY);
		delete pMainList;
		return false;
	}

	rParse.AddToCurPos(1);
	iPos++;

	// Reset Operator level
	rParse.PushOpLevel(-1);

	while (true)
	{
		if (!(pElList = new CCodeElementList))
		{
			rParse.SetError(CPB_OUTOFMEMORY);
			return false;
		}
			
		if (!pMainList->Add(*pElList, true, iLine, iPos))
		{
			rParse.SetError(CPB_OUTOFMEMORY);
			delete pElList;
			return false;
		}
		
		if (!rParse.BasicParseLoop(pElList))
		{
			// If a closed bracket was encountered then 
			// end list of variables
			if (rParse.GetError() == CPB_CLOSEDRB)
			{
				rParse.ClearError();
				rParse.GetCurLinePos(iLine, iPos);
				bCloseBracket = true;
				break;
			}
			// If a comma was encountered then add next 
			// code element list.
			// Otherwise an error occured. Return false!
			else if (rParse.GetError() != CPB_COMMA)
			{
				rParse.PopOpLevel();
				if (rParse.GetError() == CPB_EOL)
					rParse.SetError(CPB_MISSINGCRB);

				return false;
			}
		}
		else break;

		rParse.ClearError();
		rParse.GetCurLinePos(iLine, iPos);
	}

	// Recall previous operator level
	rParse.PopOpLevel();

	if (!bCloseBracket)
	{
		rParse.SetError(CPB_MISSINGCRB);
		return false;
	}

	if (bCreateRefVarList)
	{
		if (!(m_pCreateRefVarList = new CCodeCreateRefVarList))
		{
			rParse.SetError(CPB_OUTOFMEMORY);
			return false;
		}

		// Save pointer to list of code lines that
		// create code list. Maybe used by functions.
		m_pCreateRefVarList->SetCodeList(pMainList);

		// Bracket was closed
		if (!pMainList->Add(*m_pCreateRefVarList, true, iLine, iPos))
		{
			rParse.SetError(CPB_OUTOFMEMORY);
			delete m_pCreateRefVarList;
			return false;
		}
	}

	return true;
}


