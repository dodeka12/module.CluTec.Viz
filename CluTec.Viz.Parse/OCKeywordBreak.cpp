////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      OCKeywordBreak.cpp
//
// summary:   Implements the oc keyword break class
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

// OCOpOpenCurlyBracket.cpp: Implementierung der Klasse COCKeywordBreak.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OCKeywordBreak.h"

#include "ParseBase.h"
#include "CodeVar.h"
#include "CodeData.h"
#include "CodeBreak.h"
#include "CodeElementList.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COCKeywordBreak::COCKeywordBreak()
{
	m_csIDSymbolSpace = OC_IDSYM_BREAK_STR;
}

COCKeywordBreak::~COCKeywordBreak()
{

}



//////////////////////////////////////////////////////////////////////
/// Parse Loop

bool COCKeywordBreak::Parse(CParseBase& rParse, CCodeElementList& rCode)
{
	CCodeElementList *pMainList;

	int iLine, iPos;
	CCodeBreak *pBreakCode;

	rParse.GetCurLinePos(iLine, iPos);
	rParse.AddToCurPos(2);

	if (!(pMainList = new CCodeElementList))
	{
		rParse.SetError(CPB_OUTOFMEMORY);
		return false;
	}

	// Now add code to execute break statement
	if (!(pBreakCode = new CCodeBreak))
	{
		rParse.SetError(CPB_OUTOFMEMORY);
		delete pMainList;
		return false;
	}

	if (!pMainList->Add(*pBreakCode, true, iLine, iPos))
	{
		rParse.SetError(CPB_OUTOFMEMORY);
		delete pMainList;
		return false;
	}

	if (!rCode.Add(*pMainList, true, iLine, iPos))
	{
		rParse.SetError(CPB_OUTOFMEMORY);
		delete pMainList;
		return false;
	}

	return true;
}


