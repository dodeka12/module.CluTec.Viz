////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      ParseBase.cpp
//
// summary:   Implements the parse base class
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

// ParseBase.cpp: Implementierung der Klasse CParseBase.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ParseBase.h"
#include "OCIDSymDef.h"

char* CParseBase::m_ppcErrStrList[] =
{
	"No Error",
	"Unknown Error",
	"Out of Memory",
	"Invalid Syntax",
	"Missing '('",
	"Missing ')'",
	"Unknown Symbol",
	"Unexpected Symbol",
	"Unexpected Symbol ','",
	"Non-Unary Operator",
	"Non-Binary Operator",
	"Missing '{'",
	"Missing '}'",
	"Missing 'if' condition",
	"Incomplete 'if' statement",
	"Missing 'if' true branch",
	"Missing 'if' false branch",
	"Include file could not be found",
	"Missing '['",
	"Missing ']'",
	"Unexpected Symbol '('",
	"File included recursively"
};

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CParseBase::CParseBase()
{
	m_iCurPos     = 0;
	m_iCurLine    = 0;
	m_cLineSepSym = ';';

	m_iErrNo = CPB_NOERROR;

	m_iRoundBracketCount = 0;
	m_iCodeElementCount  = 0;

	m_pPrevOC = 0;

	// Initialize List of ObjectClass IDs
	for (int i = 0; i < 256; i++)
	{
		m_ppOCIDList[i]        = 0;
		m_ppOCIDListSpecial[i] = 0;
	}
}

CParseBase::~CParseBase()
{
}

//////////////////////////////////////////////////////////////////////
// Get object class of next element
// Reads symbols until a symbol is an ID for an object class
// in m_ppOCIDList. Returns null if error occurred or no object
// could be found.

CObjectClass* CParseBase::GetNextObjectClass()
{
	int iLineCount = int(m_msText.size());
	int iLen       = int(m_msText[m_iCurLine].csText.Len());

	if ((m_iCurLine < 0) || (m_iCurLine >= iLineCount) || (m_iCurPos < 0) || (m_iCurPos >= iLen))
	{
		return 0;
	}

	char* pcLine = m_msText[m_iCurLine].csText.Str();

	m_pPrevOC = m_pCurOC;

	// Test for special symbol, which starts with OC_IDSYM_SPECIAL_CHAR.
	// The symbol following this switch then gives the actual object class.
	if ((unsigned char) pcLine[m_iCurPos] == OC_IDSYM_SPECIAL_CHAR)
	{
		if (m_iCurPos + 1 >= iLen)
		{
			SetError(CPB_UNKNOWNSYM);
			return 0;
		}

		if ((m_pCurOC = m_ppOCIDListSpecial[(unsigned char) pcLine[m_iCurPos + 1]]) != 0)
		{
			return m_pCurOC;
		}
		else
		{
			SetError(CPB_UNKNOWNSYM);
			return 0;
		}
	}
	else if ((m_pCurOC = m_ppOCIDList[(unsigned char) pcLine[m_iCurPos]]) != 0)
	{
		return m_pCurOC;
	}
	else
	{
		SetError(CPB_UNKNOWNSYM);
		return 0;
	}
}

//////////////////////////////////////////////////////////////////////
// Read Symbol Subspace Element
// reads element starting at m_iCurPos in line m_iCurLine which consists
// of symbols given in rsSSS. Element is returned in rsElement.
// Reading of element stops at first symbol not contained in rsSSS.

bool CParseBase::ReadSymSubSpaceElement(CStrMem& rsElement, CStrMem& rsSSS)
{
	int i, iLen;

	if ((m_iCurLine < 0) || (m_iCurLine >= (int) m_msText.size()) ||
	    (m_iCurPos < 0) || (m_iCurPos >= (iLen =  int(m_msText[m_iCurLine].csText.Len()))))
	{
		return false;
	}

	char* pcLine = m_msText[m_iCurLine].csText.Str();
	char* pcSSS  = rsSSS.Str();

	rsElement = "";

	for (i = m_iCurPos; i < iLen; i++)
	{
		if (!strchr(pcSSS, pcLine[i]))	// if cSym is not in Symbol Sub Space
		{
			break;
		}
	}

	// Get Element
	rsElement = m_msText[m_iCurLine].csText(m_iCurPos, i - 1);

	m_iCurPos = i;

	// Goto next line if we searched to the end of this one.
/*
        if ((m_iCurPos = i) >= iLen)
        {
                m_iCurPos = 0;
                m_iCurLine++;
        }
*/
	return true;
}

//////////////////////////////////////////////////////////////////////
// Add Object Class
// An object class is entered into m_ppOCIDList depending on its
// ID chars. Checks whether IDs are unique. Returns false if not!

bool CParseBase::AddObjectClass(CObjectClass* pObjectClass)
{
	if (!pObjectClass) { return false; }

	CStrMem& csIDSS = pObjectClass->m_csIDSymbolSpace;
	int i, iLen = int(csIDSS.Len());

	if ((unsigned char) csIDSS[0] == OC_IDSYM_SPECIAL_CHAR)
	{
		if (iLen != 2)
		{
			return false;	// Invalid Special Symbol
		}
		CObjectClass** ppID = &m_ppOCIDListSpecial[(unsigned char) csIDSS[1]];

		if (*ppID != 0)
		{
			return false;	// ID-Set not unique
		}
		*ppID = pObjectClass;

		return true;
	}

	for (i = 0; i < iLen; i++)
	{
		CObjectClass** ppID = &m_ppOCIDList[(unsigned char) csIDSS[i]];

		if (*ppID != 0)
		{
			return false;	// ID-Set not unique
		}
		*ppID = pObjectClass;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Insert Text
/// Insert text separated into lines before iPos.
/// If iPos == -1 then lines are added to end.
/// Separate Text into lines using m_cSepSym as separation symbol.

int CParseBase::InsertText(const char* _pcText, int iPos)
{
	// Has to be implemented by derived class
	return PreParse(_pcText, iPos);
}

int CParseBase::PreParse(const char* _pcText, int iPos, const char* pcFilename, int iMainFilePos, bool bUseScriptPath)
{
	return -1;
}

//////////////////////////////////////////////////////////////////////
/// Basic Parse Loop
/// Finds next Object Class and calls parse on it within current line.

bool CParseBase::BasicParseLoop(CCodeElementList* pCodeList)
{
	CObjectClass* pOC;

	while ((pOC = GetNextObjectClass()) != 0)
	{
		if (!pOC->Parse(*this, *pCodeList))
		{
			return false;
		}
	}

	if (m_iCurPos >= (int) m_msText[m_iCurLine].csText.Len())
	{
		SetError(CPB_EOL);	// End of Line
		return false;
	}

	if (GetError() != CPB_NOERROR)
	{
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Add a new operator level.

bool CParseBase::PushOpLevel(int iLevel)
{
	if (!m_mCurOpLevel.Add(1))
	{
		return false;
	}

	m_mCurOpLevel.Last() = iLevel;

	return true;
}

//////////////////////////////////////////////////////////////////////
// Remove operator level from top of stack.

int CParseBase::PopOpLevel()
{
	int iC;

	if ((iC = int(m_mCurOpLevel.Count())) == 0)
	{
		return -1;
	}

	int iVal = m_mCurOpLevel.Last();

	m_mCurOpLevel.Del(iC - 1);

	return iVal;
}

//////////////////////////////////////////////////////////////////////
// Add Function to Functionlist

bool CParseBase::AddFunc(const char* pcName, T_ExtFunc pFunc)
{
	string sName;
	CCodeFunction CodeFunc;

	CodeFunc.SetFunc(pcName, pFunc);
	sName = pcName;

	m_mapFunc[sName].push_back(CodeFunc);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Check whether given label is a function

CCodeFunction* CParseBase::GetFunc(const char* pcName)
{
	map < string, vector < CCodeFunction >> ::iterator itEl;

	if ((itEl = m_mapFunc.find(string(pcName))) == m_mapFunc.end())
	{
		return 0;
	}

	return &(itEl->second[0]);
}
