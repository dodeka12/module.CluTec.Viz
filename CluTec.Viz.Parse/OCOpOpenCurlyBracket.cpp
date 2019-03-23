////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      OCOpOpenCurlyBracket.cpp
//
// summary:   Implements the oc operation open curly bracket class
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

// OCOpOpenCurlyBracket.cpp: Implementierung der Klasse COCOpOpenCurlyBracket.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OCOpOpenCurlyBracket.h"

#include "ParseBase.h"
#include "CodeVar.h"
#include "CodeData.h"
#include "CodeElementList.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COCOpOpenCurlyBracket::COCOpOpenCurlyBracket()
{
	m_csIDSymbolSpace = "{";
}

COCOpOpenCurlyBracket::~COCOpOpenCurlyBracket()
{

}



//////////////////////////////////////////////////////////////////////
/// Parse Loop

bool COCOpOpenCurlyBracket::Parse(CParseBase& rParse, CCodeElementList& rCode)
{
	CCodeElementList *pMainList, *pElList;

	bool bReparse = false;
	bool bCloseBracket = false;
	int iLine, iPos;
	rParse.GetCurLinePos(iLine, iPos);

	if (!(pMainList = new CCodeElementList))
	{
		rParse.SetError(CPB_OUTOFMEMORY);
		return false;
	}

	rParse.AddToCurPos(1);
	iPos++;

	// Reset Operator level
	rParse.PushOpLevel(-1);

	while (true)
	{
		bReparse = false;

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
			// end code list.
			if (rParse.GetError() == CPB_CLOSEDCB)
			{
				rParse.ClearError();
				rParse.GetCurLinePos(iLine, iPos);
				bCloseBracket = true;
				break;
			}
			else if (rParse.GetError() == CPB_REPARSE)
			{
				bReparse = true;
			}
			// If end of line was encountered then add next 
			// code element list.
			// Otherwise an error occured. Return false!
			else if (rParse.GetError() != CPB_EOL)
			{
				return false;
			}
		}
		else break;

		rParse.ClearError();
		if (!bReparse && !rParse.NextLine())
			break;
		rParse.GetCurLinePos(iLine, iPos);
	}

	// Recall previous operator level
	rParse.PopOpLevel();

	if (!bCloseBracket)
	{
		rParse.SetError(CPB_MISSINGCCB);
		return false;
	}


	// Create Label with pointer to CodeElementList pMainList
	CCodeData *pData;
	CStrMem csElement;

	csElement << "__Code" << rParse.IncrCEL();

	if (!(pData = new CCodeData))
	{
		rParse.SetError(CPB_OUTOFMEMORY);
		return false;
	}

	pData->SetName(csElement);

	// Set data to main list
	CCodeVar &rVar = pData->RefData();

	rVar = (CCodeElement *) pMainList;
	rVar.Name() = "Constant";
	rVar.EnableProtect();

	if (!rCode.Add(*pData, true, iLine, iPos))
	{
		rParse.SetError(CPB_OUTOFMEMORY);
		delete pMainList;
		return false;
	}

	return true;
}


