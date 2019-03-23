////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      OCString.cpp
//
// summary:   Implements the oc string class
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

// OCString.cpp: Implementierung der Klasse COCOString.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OCString.h"

#include "ParseBase.h"
#include "CodeVar.h"
#include "CodeData.h"
#include "CodeElementList.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COCString::COCString()
{
	m_csIDSymbolSpace = "@\"";
}

COCString::~COCString()
{
}

//////////////////////////////////////////////////////////////////////
/// Parse
bool COCString::Parse(CParseBase& rParse, CCodeElementList& rCodeList)
{
	int iPos, iLen, iLine;
	bool bError      = true;
	bool bRaw        = false;
	bool bShaderCode = false;
	CStrMem csLine, csString;
	char cSym, cPrevSym;
	char pcHexVal[3];
	int iHexVal;
	CCodeData* pCode;

	rParse.GetCurLine(csLine, iLine, iPos);
	iLen     = int(csLine.Len());
	csString = "";

	if (csLine[iPos] == '@')
	{
		if ((iLen - iPos >= 8) && (strncmp(&csLine[iPos], "@Shader\"", 8) == 0))
		{
			bShaderCode = true;
			iPos       += 8;
			csString << "//# File Position: " << iLine << " : " << iPos << "\n";
		}
		else if (csLine[iPos + 1] == '\"')
		{
			iPos += 2;	// skip the @"
			bRaw  = true;
		}
		else
		{
			rParse.SetError(CPB_INVALIDSYNTAX);
			return false;
		}
	}
	else
	{
		iPos++;			// skip the "
	}

	cPrevSym = 0;
	for (; iPos < iLen; iPos++)
	{
		cSym = csLine[iPos];
		if ((cSym == '\"') &&
		    (bRaw || ((cPrevSym != '\\') ||
			      ((iPos > 1) && (cPrevSym == '\\') && (csLine[iPos - 2] == '\\')))))
		{
			bError = false;
			break;			// End Search
		}

		if (bRaw || (cSym != '\\'))
		{
			csString += cSym;
		}
		else
		{
			cSym = csLine[iPos + 1];

			if (cSym == '\"')
			{
				csString += cSym;
				iPos++;
			}
			else if (cSym == '\\')
			{
				csString += cSym;
				iPos++;
			}
			else if (cSym == 'n')
			{
				csString += "\n";
				iPos++;
			}
			else if (cSym == 't')
			{
				csString += "\t";
				iPos++;
			}
			else if (cSym == 'x')
			{
				pcHexVal[0] = csLine[iPos + 2];
				pcHexVal[1] = csLine[iPos + 3];
				pcHexVal[2] = 0;
				if (sscanf_s(pcHexVal, "%x", &iHexVal) != 1)
				{
					bError = true;
					break;
				}
				pcHexVal[0] = char(iHexVal);
				pcHexVal[1] = 0;
				csString   += pcHexVal;
				iPos       += 3;
			}
			else
			{
				csString += '\\';
				csString += cSym;
				iPos++;
			}

			cSym = 0;
		}

		cPrevSym = cSym;
	}

	rParse.SetCurPos(++iPos);

	if (bError)
	{
		rParse.SetError(CPB_INVALIDSYNTAX);
		return false;
	}
	if (!(pCode = new CCodeData))
	{
		rParse.SetError(CPB_OUTOFMEMORY);
		return false;
	}

	CCodeVar& rVar = pCode->RefData();
	CStrMem csName;
	csName << "String" << iLine << ", " << iPos;

	pCode->SetName(csName);
	rVar.Name() = "Constant";
	rVar        = csString;

	// Variable is constant and cannot be an L-Value.
	rVar.EnableProtect();

	rParse.GetCurLinePos(iLine, iPos);

	// Add Code Element to end of Code Element List.
	if (!rCodeList.Add(*pCode, true, iLine, iPos))
	{
		delete pCode;
		rParse.SetError(CPB_OUTOFMEMORY);
		return false;
	}

	return true;
}
