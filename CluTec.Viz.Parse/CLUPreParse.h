////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      CLUPreParse.h
//
// summary:   Declares the clu pre parse class
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

#pragma once

#include "OCIDSymDef.h"
#include "Defines.h"
#include "ParseTypes.h"

#include <string>
#include <map>
#include <vector>

#include "CluTec.Viz.Base\mem.h"
#include "CluTec.Viz.Base\CStrMem.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief
/// 	The CLU script parser class
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CCLUPreParse
{
public:

	CCLUPreParse();
	virtual ~CCLUPreParse();

	bool Init();

	std::string PrintParseErrors(const std::vector<STextLine>& x_msText, int x_iCurLine, int x_iCurPos);

	std::string GetErrorFilename() { return m_sErrorFilename; }
	std::string GetErrorPath() { return m_sErrorPath; }

	void SetScriptPath(const char* pcScriptPath)
	{
		m_sScriptPath = pcScriptPath;
	}

	void SetScriptName(const char* pcScriptName)
	{
		m_sScriptName = pcScriptName;
	}

	// Insert Text pcText at position iPos and parse it if bParse is true.
	// If iPos == -1 then add text to end.
	// Returns number of lines read. Returns -1 if error occurred.
	int PreParseText(std::vector<STextLine>& x_msText, int& x_iCurLine, int& x_iCurPos, const char* pcText);

	static bool ReadCLUBinaryFile(const char* pcFilename, Mem<char>& mCode, std::map<std::string, std::string>& mapMetaData, std::string& sError, const char* pcPass = 0);

	int GetError() { return m_iErrNo; }
	void SetError(int iErrNo) { m_iErrNo = iErrNo; }

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// 	Gets error string.
	/// </summary>
	///
	/// <param name="iErrNo"> Zero-based index of the error number. </param>
	///
	/// <returns> Null if it fails, else the error string. </returns>
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const char* GetErrorStr(int iErrNo) const
	{
		if (iErrNo < 0)
		{
			iErrNo = CPB_UNEXPECTEDSYM;
		}

		return m_ppcErrStrList[iErrNo];
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// 	Gets the last included file.
	/// </summary>
	///
	/// <returns> Null if it fails, else the last included file. </returns>
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const char* GetLastIncludedFile() const
	{
		return m_sIncludeFilename.c_str();
	}

	bool GenTextCode(std::vector<char>& vecCode, const std::vector<STextLine>& x_msText);

protected:

	// Insert lines of text before iPos.
	// If iPos == -1 then add lines to end
	// Automatically separates Text into lines.
	// Returns number of lines contained in text.
	// Returns -1 if error occurred.
	// Overloaded from ParseBase.
	int DoPreParse(std::vector<STextLine>& x_msText,
			int& x_iCurLine,
			int& x_iCurPos,
			const char* pcText,
			int iPos = -1,
			const char* pcFilename = 0,
			int iMainFilePos = -1,
			bool bUseScriptPath = true);

	// Test whether current '(' symbol refers to a function call
	bool IsFuncCall(const char* pcInputText, int iIdx, int iLen);

	/************************************************************************/
	/* BASE                                                                 */
	/************************************************************************/

	int SetTextCode(std::vector<STextLine>& x_msText, const char* pcCode, const char* pcFilename, bool bAdd);

protected:

	std::string m_sScriptPath;
	std::string m_sScriptName;
	std::string m_sIncludeFilename;
	std::string m_sErrorFilename;
	std::string m_sErrorPath;

	std::vector<std::string> m_vecIncludeFilename;

	/************************************************************************/
	/* BASE                                                                 */
	/************************************************************************/
	/// \brief Line separation symbol.
	char m_cLineSepSym;
	/// \brief Error Number.
	int m_iErrNo;
	static char* m_ppcErrStrList[];

	CStrMem m_csLabelIDSymbolSpace;
};
