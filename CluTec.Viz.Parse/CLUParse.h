////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      CLUParse.h
//
// summary:   Declares the clu parse class
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


// CLUParse.h: Schnittstelle f�r die Klasse CCLUParse.
//
// This Class initializes the CParseBase with the appropriate syntax.
//
//////////////////////////////////////////////////////////////////////

#include "OCIDSymDef.h"

#include "Parse.h"
#include "CLUCodeBase.h"
#include "CLUPreParse.h"

#include "OCLabel.h"
#include "OCNumber.h"
#include "OCString.h"
#include "OCObjSepSym.h"

#include "OCOpOpenRoundBracket.h"
#include "OCOpCloseRoundBracket.h"
#include "OCOpComma.h"

#include "OCOpOpenSquareBracket.h"
#include "OCOpCloseSquareBracket.h"

#include "OCOpOpenCurlyBracket.h"
#include "OCOpCloseCurlyBracket.h"

#include "OCKeywordIf.h"
#include "OCKeywordThen.h"
#include "OCKeywordElse.h"
#include "OCKeywordLoop.h"
#include "OCKeywordBreak.h"

#include "OCOpStd.h"

#include "CodeFunction.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 	The CLU script parser class.
/// </summary>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CCLUParse : public CCLUPreParse, public CParse
{
public:

	struct SModule
	{
		HMODULE hModule;
		T_InitModule pfInitModule;
		T_FinalizeModule pfFinalizeModule;
		T_InitViz pfInitViz;
		T_GetFuncList pfGetFuncList;
	};

public:

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// 	Default constructor.
	/// </summary>
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CCLUParse();

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// 	Destructor.
	/// </summary>
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual ~CCLUParse();

	virtual bool Init();

	CStrMem PrintVars();
	CStrMem PrintStack();

	CStrMem PrintCodeErrors();
	std::string PrintParseErrors();

	bool GetParseErrorPos(int& iLine, int& iPos, int& iInputPos, CStrMem& csFilename);
	bool GetCodeErrorPos(int& iLine, int& iPos, int& iInputPos, CStrMem& csFilename);

	void ResetErrors() { m_xCodeBase.m_ErrorList.Reset(); }

	CCLUCodeBase& GetCodeBase() { return m_xCodeBase; }
	void ResetCodeBase() { m_xCodeBase.Reset(); }

	void PostResetResourceHandles() { m_xCodeBase.PostResetResourceHandles();  }
	void TryResetResourceHandles() { m_xCodeBase.TryResetResourceHandles(); }

	void CollectGarbage() { m_xCodeBase.CollectGarbage(); }

	void ResetOutputObjectList() { m_xCodeBase.ResetOutputObjectList(); }
	const CCLUCodeBase::TOutObjList& GetOutputObjectList() { return m_xCodeBase.GetOutputObjectList(); }
	void InsertOutputObject(const CCLUCodeBase::SOutputObject& rObj) { m_xCodeBase.InsertOutputObject(rObj); }

	void SetVersion(int iMajor, int iMinor, int iRevision) { m_xCodeBase.SetVersion(iMajor, iMinor, iRevision); }
	void GetVersion(int& iMajor, int& iMinor, int& iRevision) { m_xCodeBase.GetVersion(iMajor, iMinor, iRevision); }

	void SetRenderLatex(bool bVal = true) { m_xCodeBase.SetRenderLatex(bVal); }
	void OverrideSetRenderLatex(bool bVal = true) { m_xCodeBase.OverrideSetRenderLatex(bVal); }

	void SetMVFilter(COGLMVFilter* pFilter) { m_xCodeBase.SetMVFilter(pFilter); }
	void SetDrawBase(COGLDrawBase* pDrawBase) { m_xCodeBase.SetDrawBase(pDrawBase); }
	void SetTransform(std::vector<STransform>* pxTransform) { m_xCodeBase.SetTransform(pxTransform); }
	void SetFrameRotor(MultiV<float>* pFrameRotor) { m_xCodeBase.SetFrameRotor(pFrameRotor); }
	void SetCLUDrawBase(CCLUDrawBase* pCLUDrawBase) { m_xCodeBase.SetCLUDrawBase(pCLUDrawBase); }

	void ResetTempImageList() { m_xCodeBase.ResetTempImageList(); }
	void ToggleTempImageList() { m_xCodeBase.ToggleTempImageList(); }

	void SetScriptPath(const char* pcScriptPath)
	{
		CCLUPreParse::SetScriptPath(pcScriptPath);
		m_xCodeBase.SetScriptPath(pcScriptPath);
	}

	void SetScriptName(const char* pcScriptName)
	{
		CCLUPreParse::SetScriptName(pcScriptName);
		m_xCodeBase.SetScriptName(pcScriptName);
	}

	bool DefineColor(const char* pcName, float fR, float fG, float fB, float fA = 1.0f, bool Protect = true);
	bool DefineFloat(const char* pcName, float fVal, bool Protect = true);
	bool DefineDouble(const char* pcName, double dVal, bool Protect = true);
	bool DefineInt(const char* pcName, int iVal, bool Protect = true);
	bool DefineString(const char* pcName, const char* pcVal, bool Protect = true);
	bool DefineList(const char* pcName, bool Protect = true);
	bool DefineVersion();
	bool DefineVersionGL(int iMajor, int iMinor);
	bool DefineVersionGLSL(int iMajor, int iMinor);

	bool GetVar(const char* pcName, CCodeVar& rVar)
	{ rVar = m_xCodeBase.GetVar(pcName); if (rVar.Type() == PDT_NOTYPE) { return false; } else{ return true; } }

	CCodeVar& GetVar(const char* pcName) { return m_xCodeBase.GetVar(pcName); }

	CCodeVar& NewVar(const char* pcName, ECodeDataType _nType = PDT_INT)
	{ return m_xCodeBase.NewVar(pcName, _nType); }

	void DeleteVar(const char* pcName)
	{ m_xCodeBase.DeleteVar(pcName); }

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// 	Runs the code starting at line iStartLine for iLineCount lines. If iStartLine is 0 then start at beginning. If
	/// 	iLineCount is -1 then run all lines after iStartLine.
	/// </summary>
	///
	/// <param name="iStartLine"> (Optional) The start line. </param>
	/// <param name="iLineCount"> (Optional) The number of lines to run. </param>
	///
	/// <returns> True if it succeeds, false if it fails. </returns>
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool RunCode(int iStartLine = 0, int iLineCount = -1);

	// Insert Text pcText at position iPos and parse it if bParse is true.
	// If iPos == -1 then add text to end.
	// Returns number of lines read. Returns -1 if error occurred.
	virtual int InsertText(const char* pcText, int iPos = -1, bool bParse = true);

	int SetTextCode(const char* pcCode, const char* pcFilename, bool bParse = true, bool bAdd = false);

	bool GenTextCode(std::vector<char>& vecCode);

	// Load Function Modules
	static bool LoadModules(const char* pcModulePath, const char* pcModuleName);

	// Release Function Modules
	static bool ReleaseModules();

	// Get a function pointer. Return 0 if function does not exist
	static T_ExtFunc GetModuleFunction(const char* pcFuncName);

	// Get list of function names
	bool GetFuncNameList(vector<string>& vecFuncName)
	{ vecFuncName = m_vecFuncName; return true; }

	// Initialize Modules after RC exists
	static bool InitVizModules();


protected:

	// Insert lines of text before iPos.
	// If iPos == -1 then add lines to end
	// Automatically separates Text into lines.
	// Returns number of lines contained in text.
	// Returns -1 if error occurred.
	// Overloaded from ParseBase.
	int PreParse(const char* pcText, int iPos = -1, const char* pcFilename = 0, int iMainFilePos = -1, bool bUseScriptPath = true);

protected:

	COCObjSepSym m_ocObjSepSym;

	COCLabel m_ocLabel;
	COCNumber m_ocNumber;
	COCString m_ocString;

	COCOpOpenRoundBracket m_ocOpOpenRoundBracket;
	COCOpCloseRoundBracket m_ocOpCloseRoundBracket;
	COCOpComma m_ocOpComma;

	COCOpOpenSquareBracket m_ocOpOpenSquareBracket;
	COCOpCloseSquareBracket m_ocOpCloseSquareBracket;

	COCOpOpenCurlyBracket m_ocOpOpenCurlyBracket;
	COCOpCloseCurlyBracket m_ocOpCloseCurlyBracket;

	COCKeywordIf m_ocKeywordIF;
	COCKeywordThen m_ocKeywordTHEN;
	COCKeywordElse m_ocKeywordELSE;
	COCKeywordLoop m_ocKeywordLOOP;
	COCKeywordBreak m_ocKeywordBREAK;

	// Standard Operators
	MemObj<COCOpStd> m_mOps;

	CCLUCodeBase m_xCodeBase;

	// List of loaded function modules
	static vector<SModule> m_vecModule;
	// List of all function names
	static vector<string> m_vecFuncName;
	// Number of the module functions are bound
	static int m_iModuleBindCount;
};
