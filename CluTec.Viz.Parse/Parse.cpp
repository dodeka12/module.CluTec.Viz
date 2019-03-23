////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      Parse.cpp
//
// summary:   Implements the parse class
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

// Parse.cpp: Implementierung der Klasse CParse.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Parse.h"
#include "CodeBase.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CParse::CParse()
{
	m_pCodeBase   = 0;
	m_bCodeTreeOK = false;
}

CParse::~CParse()
{
}

//////////////////////////////////////////////////////////////////////
// Serialize Code
// Generates XML code of currently parsed code

bool CParse::GenXMLCode(CXMLTree& xmlTree)
{
	if (!m_bCodeTreeOK)
	{
		return false;
	}

	return CCodeElementList::Serialize(xmlTree);
}

//////////////////////////////////////////////////////////////////////
// Insert Text pcText at position iPos and parse it if bParse is true.
// If iPos == -1 then add text to end.

int CParse::InsertText(const char* pcText, int iPos, bool bParse)
{
	m_bCodeTreeOK = false;

	int iStartLine;

	if (iPos < 0)
	{
		iStartLine = int(m_msText.size());
	}
	else
	{
		iStartLine = iPos;
	}

	int iNewLines = CParseBase::InsertText(pcText, iPos);
	//printf("CParse:InsertText: NewLines = %d\n", iNewLines);

	if (iNewLines < 0) { return iNewLines; }
	if (iNewLines == 0) { return 0; }

	if (bParse)
	{
		if (!Parse(iStartLine, iNewLines))
		{
			return -1;
		}
	}

	m_bCodeTreeOK = true;
	return iNewLines;
}

//////////////////////////////////////////////////////////////////////
// Delete Code and Text starting at line iStartLine for iLineCount lines.
// if bDelCodeAndText == false then only delete text.

bool CParse::DeleteText(int iStartLine, int iLineCount, bool bDelCodeAndText)
{
	int iPLC, iCLC;
	bool bDelText = true, bDelCode = true;

	iPLC = int(m_msText.size());
	iCLC = int(m_mElementList.Count());

	if (bDelCodeAndText && (iPLC != iCLC))
	{
		return false;
	}

	if ((iStartLine < 0) || (iStartLine + iLineCount > iPLC))
	{
		return false;
	}

	bDelText = CParseBase::DeleteLine(iStartLine, iLineCount);

	if (bDelCodeAndText)
	{
		bDelCode = CCodeElementList::Delete(iStartLine, iLineCount);
	}

	if (bDelText && bDelCode)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//////////////////////////////////////////////////////////////////////
// Parse text starting at line iStartLine for iLineCount lines.
// If iStartLine == 0 then start at beginning.
// If iLineCount == -1 then parse everything starting from line iStartLine.

bool CParse::Parse(int iStartLine, int iLineCount)
{
	m_bCodeTreeOK = false;

	//printf("CParse:Parse: Start\n");

	int iLineNo = int(m_msText.size());
	//printf("CParse:Parse: LineNo = %d\n", iLineNo);

	//printf("CParse:Parse: StartLine = %d\n", iStartLine);
	if ((iStartLine < 0) || (iStartLine >= iLineNo))
	{
		return false;
	}

	//printf("CParse:Parse: LineCount = %d\n", iLineCount);
	if (iLineCount < 0)
	{
		iLineCount = iLineNo - iStartLine;
	}
	else if (iStartLine + iLineCount > iLineNo)
	{
		return false;
	}

	int iLine, iPos, iCTLine;
	int iMaxLine = iStartLine + iLineCount - 1;

	CCodeElementList* pCodeLine;

	// Reset user defined variables
	if (m_pCodeBase)
	{
		m_pCodeBase->ResetVarList();
	}

	for (iLine = iCTLine = iStartLine; iLine <= iMaxLine; iLine++, iCTLine++)
	{
		// Create CodeElementList for next code Line
		if (!(pCodeLine = new CCodeElementList))
		{
			return false;
		}

		if (!CCodeElementList::Insert(*pCodeLine, iCTLine, true))
		{
			return false;
		}

		CParseBase::SetCurLine(iLine);

		if (!CParseBase::BasicParseLoop(pCodeLine))
		{
			int iErrNo = CParseBase::GetError();

			if (iErrNo < 0)
			{
				if (iErrNo == CPB_COMMA)
				{
					iErrNo = CPB_UNEXPSYMCOMMA;
				}
				else if (iErrNo == CPB_EOL)
				{
					CParseBase::ClearError();
					CParseBase::GetCurLinePos(iLine, iPos);
					continue;
				}
				else if (iErrNo == CPB_REPARSE)
				{
					CParseBase::ClearError();
					CParseBase::GetCurLinePos(iLine, iPos);
					iLine--;
					continue;
				}
				else
				{
					iErrNo = CPB_UNEXPECTEDSYM;
				}
			}
			CParseBase::SetError(iErrNo);
			return false;
		}

		CParseBase::GetCurLinePos(iLine, iPos);
	}

	//printf("CParse:Parse: End\n");
	m_bCodeTreeOK = true;
	return true;
}

//////////////////////////////////////////////////////////////////////
// Run Code starting at line iStartLine for iLineCount lines.
// if iStartLine == 0 then start at beginning.
// if iLineCount == -1 then run all lines after iStartLine.

bool CParse::RunCode(int iStartLine, int iLineCount)
{
	if (!m_pCodeBase || !m_bCodeTreeOK) { return false; }

	int iLineNo = int(m_mElementList.Count());

	if (iLineNo == 0)
	{
		m_pCodeBase->m_ErrorList.NoCode();
		return false;
	}

	if ((iStartLine < 0) || (iStartLine >= iLineNo))
	{
		return false;
	}

	if (iLineCount < 0)
	{
		iLineCount = iLineNo - iStartLine;
	}
	else if (iStartLine + iLineCount > iLineNo)
	{
		return false;
	}

	int iLine;
	int iMaxLine = iStartLine + iLineCount - 1;

	m_pCodeBase->ReserveStack(1000, 1000);
	m_pCodeBase->ReserveTempVars(100);

	for (iLine = iStartLine; iLine <= iMaxLine; iLine++)
	{
		m_pCodeBase->ResetTempVars();
		m_pCodeBase->ResetStack();
		if (!m_mElementList[iLine].pElement->Apply(m_pCodeBase))
		{
			if ((m_pCodeBase->m_ErrorList.Last().iLevel == CEL_INTERNAL) &&
			    (m_pCodeBase->m_ErrorList.Last().iNo == CERR_BREAK))
			{
				break;
			}
			return false;
		}
	}

	return true;
}
