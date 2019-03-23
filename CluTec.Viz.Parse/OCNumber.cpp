////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      OCNumber.cpp
//
// summary:   Implements the oc number class
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

// OCNumber.cpp: Implementierung der Klasse COCNumber.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OCNumber.h"

#include "ParseBase.h"
#include "CodeVar.h"
#include "CodeData.h"
#include "CodeElementList.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COCNumber::COCNumber()
{
	m_csIDSymbolSpace = "1234567890";

	m_csSymbolSpace = m_csIDSymbolSpace + ".-edf";
	m_csErrorSpace = "abcghijklmnopqrstuvwxyz����ABCDEFGHIJKLMNOPQRSTUVWXYZ���?(";

	m_csSymbolSpaceHex = m_csIDSymbolSpace + "abcdefABCDEF";
	m_csErrorSpaceHex = "ghijklmnopqrstuvwxyz����GHIJKLMNOPQRSTUVWXYZ���?(";
}

COCNumber::~COCNumber()
{

}


//////////////////////////////////////////////////////////////////////
/// Parse
bool COCNumber::Parse(CParseBase& rParse, CCodeElementList& rCodeList)
{
	int iPos, iLen, iLine, iNPos;
	bool bError = false;
	bool bHex = false;
	CStrMem csLine, csErrorSpace;
	char cSym, cPrevSym, pcNumber[100];
	ECodeDataType eType = PDT_INT;
	CCodeData *pCode;

	rParse.GetCurLine(csLine, iLine, iPos);
	iLen = int(csLine.Len());

	cPrevSym = 0;

	if ( iLen - iPos > 2 && csLine[iPos] == '0' && csLine[iPos+1] == 'x' )
	{
		// Number in Hex Format
		bHex = true;
		iPos += 2;
		for( iNPos = 0; iPos < iLen && iNPos < 100; iPos++, iNPos++ )
		{
			cSym = csLine[iPos];

			if ( iNPos >= 8 )
			{
				if ( !strchr(m_csSymbolSpaceHex, cSym) == 0 )
					bError = true;

				break;
			}

			if ( strchr(m_csErrorSpaceHex, cSym) != 0 )
			{
				bError = true;
				break; // Error End
			}

			if ( strchr(m_csSymbolSpaceHex, cSym) == 0)
				break; // End parsing number

			pcNumber[iNPos] = cSym;
			cPrevSym = cSym;
		}
	}
	else
	{
		// Standard Number

		for(iNPos=0;iPos<iLen && iNPos < 100;iPos++,iNPos++)
		{
			cSym = csLine[iPos];
			if ( strchr(m_csErrorSpace, cSym) != 0 )
			{
				bError = true;
				break; // Error End
			}

			if (strchr(m_csSymbolSpace, cSym) == 0)
				break; // End Search

			if (cSym == '-' && cPrevSym != 'e')
				break;

			if (cSym == 'e')
			{
				if (cPrevSym != '.' && strchr(m_csIDSymbolSpace, cPrevSym) == 0)
				{
					bError = true;
					break;
				}
				else
				{
					eType = PDT_DOUBLE;
				}
			}

			if (cSym == 'd')
			{
				if (cPrevSym != '.' && strchr(m_csIDSymbolSpace, cPrevSym) == 0)
				{
					bError = true;
					break;
				}
				else
				{
					eType = PDT_DOUBLE;
					iPos++;

					if ( strchr(m_csErrorSpace, csLine[iPos]) != 0 || csLine[iPos] == 'e' || csLine[iPos] == 'f' )
					{
						bError = true;
						break; // Error End
					}
					break;
				}
			}

			if (cSym == 'f')
			{
				if (cPrevSym != '.' && strchr(m_csIDSymbolSpace, cPrevSym) == 0)
				{
					bError = true;
					break;
				}
				else
				{
					eType = PDT_FLOAT;
					iPos++;
					if ( strchr(m_csErrorSpace, csLine[iPos]) != 0 || csLine[iPos] == 'e' || csLine[iPos] == 'd')
					{
						bError = true;
						break; // Error End
					}
					break;
				}
			}

			if (cSym == '.')
			{
				if (strchr(m_csIDSymbolSpace, cPrevSym) == 0)
				{
					bError = true;
					break;
				}
				else
				{
					eType = PDT_DOUBLE;
				}
			}

			pcNumber[iNPos] = cSym;
			cPrevSym = cSym;
		}
	}

	rParse.SetCurPos(iPos);

	if (bError)
	{
		rParse.SetError(CPB_INVALIDSYNTAX);
		return false;
	}

	pcNumber[iNPos] = 0;

	if (!(pCode = new CCodeData))
	{
		rParse.SetError(CPB_OUTOFMEMORY);
		return false;
	}

    CCodeVar &rVar = pCode->RefData();

	pCode->SetName(pcNumber);
	rVar.Name() = "Constant";

	if (eType == PDT_INT)
	{
		int iVal;

		if ( bHex )
		{
			if (sscanf_s(pcNumber, "%x", &iVal) != 1)
			{
				rParse.SetError(CPB_UNKNOWN);
				delete pCode;
				return false;
			}
		}
		else
		{
			if (sscanf_s(pcNumber, "%d", &iVal) != 1)
			{
				rParse.SetError(CPB_UNKNOWN);
				delete pCode;
				return false;
			}
		}

		rVar = iVal;
	}
	else if (eType == PDT_FLOAT)
	{
		float fVal;

		if (sscanf_s(pcNumber, "%g", &fVal) != 1)
		{
			rParse.SetError(CPB_UNKNOWN);
			delete pCode;
			return false;
		}

		rVar = fVal;
	}
	else if (eType == PDT_DOUBLE)
	{
		double dVal;

		if (sscanf_s(pcNumber, "%lg", &dVal) != 1)
		{
			rParse.SetError(CPB_UNKNOWN);
			delete pCode;
			return false;
		}

		rVar = dVal;
	}

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

