////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      OCOpOpenSquareBracket.cpp
//
// summary:   Implements the oc operation open square bracket class
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

// OCOpOpenSquareBracket.cpp: Implementierung der Klasse COCOpOpenSquareBracket.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OCOpOpenSquareBracket.h"
#include "ParseBase.h"
#include "CodeElementList.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COCOpOpenSquareBracket::COCOpOpenSquareBracket()
{
	m_csIDSymbolSpace = "[";
}

COCOpOpenSquareBracket::~COCOpOpenSquareBracket()
{

}


//////////////////////////////////////////////////////////////////////
/// Parse Loop

bool COCOpOpenSquareBracket::Parse(CParseBase& rParse, CCodeElementList& rCode)
{
	rParse.IncrSBC();

	CCodeElementList *pMainList, *pElList;

	bool bCloseBracket = false;
	int iLine, iPos;
	rParse.GetCurLinePos(iLine, iPos);

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
			if (rParse.GetError() == CPB_CLOSEDSB)
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
					rParse.SetError(CPB_MISSINGCSB);

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
		rParse.SetError(CPB_MISSINGCSB);
		return false;
	}

	if (!(m_pCreateVarList = new CCodeCreateVarList))
	{
		rParse.SetError(CPB_OUTOFMEMORY);
		return false;
	}

	// Save pointer to list of code lines that
	// create code list. Maybe used by functions.
	m_pCreateVarList->SetCodeList(pMainList);

	// Bracket was closed
	if (!pMainList->Add(*m_pCreateVarList, true, iLine, iPos))
	{
		rParse.SetError(CPB_OUTOFMEMORY);
		delete m_pCreateVarList;
		return false;
	}

	return true;
}


