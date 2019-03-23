////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      CLUParse.cpp
//
// summary:   Implements the clu parse class
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

// CLUParse.cpp: Implementierung der Klasse CCLUParse.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CLUParse.h"
#include "Encode.h"
#include <time.h>
#include <algorithm>
#include <string>

#define DNG_KEY1        0xAF37C142
#define DNG_KEY2        0x8E2F6250
#define DNG_KEY3        0x029f7a6b
#define DNG_KEY4        0x8571CDBA

#ifdef _GNUCPP3_
	#include <unistd.h>
	#define _getcwd getcwd
	#define _chdir chdir
#else
	#include <direct.h>
	#include "CluTec.Viz.Base/Environment.h"
#endif

using namespace std;

//////////////////////////////////////////////////////////////////////
// Static Variables

// List of loaded function modules
vector<CCLUParse::SModule> CCLUParse::m_vecModule;
// List of all function names
vector<string> CCLUParse::m_vecFuncName;
// Number of the module functions are bound
int CCLUParse::m_iModuleBindCount = 0;


//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCLUParse::CCLUParse()
{
}

CCLUParse::~CCLUParse()
{
}

//////////////////////////////////////////////////////////////////////
// Load Function Modules

bool CCLUParse::LoadModules(const char* pcModulePath, const char* pcModuleName)
{
	if (m_iModuleBindCount > 0)
	{
		++m_iModuleBindCount;
		return true;
	}

	if (!pcModulePath || !pcModuleName)
	{
		return false;
	}

	vector<string> vecModuleFile;
	string sModulePattern, sModuleFile;
	sModulePattern += pcModulePath;

	sModulePattern += pcModuleName;
	sModulePattern += ".dll";

	if (m_vecModule.size())
	{
		ReleaseModules();
	}

	#ifndef WIN32
		return false;
	#else
		WIN32_FIND_DATAA FindFileData;
		HANDLE hFind;
		HMODULE hModule;
		FARPROC pProc;
		SModule Module;

		hFind = FindFirstFileA(sModulePattern.c_str(), &FindFileData);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				vecModuleFile.push_back(string(FindFileData.cFileName));
			}
			while (FindNextFileA(hFind, &FindFileData));

			FindClose(hFind);
		}

		int iMod, iModCnt = int(vecModuleFile.size());

		if (iModCnt == 0)
		{
			return false;
		}

		for (iMod = 0; iMod < iModCnt; ++iMod)
		{
			sModuleFile  = pcModulePath;
			sModuleFile += vecModuleFile[iMod];

			if (!(hModule = LoadLibrary(sModuleFile.c_str())))
			{
				continue;
			}

			Module.hModule = hModule;

			if (!(pProc = GetProcAddress(hModule, "InitModule")))
			{
				FreeLibrary(hModule);
				continue;
			}

			Module.pfInitModule = (T_InitModule) pProc;

			if (!Module.pfInitModule())
			{
				FreeLibrary(hModule);
				continue;
			}

			if (!(pProc = GetProcAddress(hModule, "FinalizeModule")))
			{
				FreeLibrary(hModule);
				continue;
			}

			Module.pfFinalizeModule = (T_FinalizeModule) pProc;

			if (!(pProc = GetProcAddress(hModule, "InitViz")))
			{
				FreeLibrary(hModule);
				continue;
			}

			Module.pfInitViz = (T_InitViz) pProc;

			if (!(pProc = GetProcAddress(hModule, "GetFuncList")))
			{
				FreeLibrary(hModule);
				continue;
			}

			Module.pfGetFuncList = (T_GetFuncList) pProc;

			m_vecModule.push_back(Module);
		}

	#endif

	if (m_vecModule.size())
	{
		++m_iModuleBindCount;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Release Function Modules

bool CCLUParse::ReleaseModules()
{
	if (m_iModuleBindCount > 1)
	{
		--m_iModuleBindCount;
		return true;
	}

	int iMod, iModCnt = int(m_vecModule.size());

	for (iMod = 0; iMod < iModCnt; ++iMod)
	{
		m_vecModule[iMod].pfFinalizeModule();

		FreeLibrary(m_vecModule[iMod].hModule);
	}

	m_vecModule.clear();
	m_vecFuncName.clear();
	m_iModuleBindCount = 0;

	return true;
}

//////////////////////////////////////////////////////////////////////
// Initialize Modules after RC exists

bool CCLUParse::InitVizModules()
{
	int iMod, iModCnt = int(m_vecModule.size());

	for (iMod = 0; iMod < iModCnt; ++iMod)
	{
		m_vecModule[iMod].pfInitViz();
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Get a function pointer. Return 0 if function does not exist
T_ExtFunc CCLUParse::GetModuleFunction(const char* pcFuncName)
{
	if (m_iModuleBindCount == 0)
	{
		return 0;
	}

	int iMod, iModCnt = int(m_vecModule.size());
	const SMapExtFunc* pMapFunc;
	int iFunc, iFuncCnt;

	for (iMod = 0; iMod < iModCnt; ++iMod)
	{
		if (!m_vecModule[iMod].pfGetFuncList(&pMapFunc, &iFuncCnt))
		{
			continue;
		}

		for (iFunc = 0; iFunc < iFuncCnt; ++iFunc)
		{
			if (strcmp(pMapFunc[iFunc].pcName, pcFuncName) == 0)
			{
				return pMapFunc[iFunc].pFunc;
			}
		}
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////
// Init

bool CCLUParse::Init()
{
	//printf("CLUParse:Init: Start\n");

	// ';' is line separation symbol
	SetLineSepSym(';');

	// Set Code Base
	SetCodeBase(&m_xCodeBase);

	// Register Object Class Object Separator Symbol ' '
	if (!AddObjectClass(&m_ocObjSepSym))
	{
		return false;
	}

	// Initialize Object Class 'Label'
	CStrMem csSet;

	// ID Symbols
	csSet = "abcdefghijklmnopqrstuvwxyz\u00E4\u00F6\u00FC\u00DF";
	csSet << "ABCDEFGHIJKLMNOPQRSTUVWXYZ\u00C4\u00D6\u00DC_";
	//csSet = "abcdefghijklmnopqrstuvwxyzäöüß";
	//csSet << "ABCDEFGHIJKLMNOPQRSTUVWXYZÄÖÜ_";
	csSet << OC_IDSYM_GLOBAL_STR;
	m_ocLabel.m_csIDSymbolSpace = csSet;

	// Symbol Subspace
	csSet << "1234567890";
	m_ocLabel.SetSymbolSpace(csSet);

	// Register Object Class 'Label'
	if (!AddObjectClass(&m_ocLabel))
	{
		return false;
	}

	// Register Object Class 'Number'
	if (!AddObjectClass(&m_ocNumber))
	{
		return false;
	}

	// Register Object Class 'String'
	if (!AddObjectClass(&m_ocString))
	{
		return false;
	}

	// Register Object Class Operator '('
	if (!AddObjectClass(&m_ocOpOpenRoundBracket))
	{
		return false;
	}

	// Register Object Class Operator ')'
	if (!AddObjectClass(&m_ocOpCloseRoundBracket))
	{
		return false;
	}

	// Register Object Class Operator '['
	if (!AddObjectClass(&m_ocOpOpenSquareBracket))
	{
		return false;
	}

	// Register Object Class Operator ']'
	if (!AddObjectClass(&m_ocOpCloseSquareBracket))
	{
		return false;
	}

	// Register Object Class Operator ','
	if (!AddObjectClass(&m_ocOpComma))
	{
		return false;
	}

	// Register Object Class Operator '{'
	if (!AddObjectClass(&m_ocOpOpenCurlyBracket))
	{
		return false;
	}

	// Register Object Class Operator '}'
	if (!AddObjectClass(&m_ocOpCloseCurlyBracket))
	{
		return false;
	}

	// Register Object Class Keyword IF
	if (!AddObjectClass(&m_ocKeywordIF))
	{
		return false;
	}

	// Register Object Class Keyword THEN
	if (!AddObjectClass(&m_ocKeywordTHEN))
	{
		return false;
	}

	// Register Object Class Keyword ELSE
	if (!AddObjectClass(&m_ocKeywordELSE))
	{
		return false;
	}

	// Register Object Class Keyword LOOP
	if (!AddObjectClass(&m_ocKeywordLOOP))
	{
		return false;
	}

	// Register Object Class Keyword BREAK
	if (!AddObjectClass(&m_ocKeywordBREAK))
	{
		return false;
	}

	// Initialize and Register standard Operators.
	int iOpNo = 44;
	if (!m_mOps.Set(iOpNo))
	{
		return false;
	}

	m_mOps[0].Init(
			"=",									// Op. Symbol
			-1,										// Unary level
			OPLEVEL_ASSIGN,							// binary level
			true,									// Right to Left
			false,									// Void Operator
			false,									// recurse lists
			0,										// Unary Func. Ptr.
			0,										// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			OPFUNC(TOpABinaryPtr, OpAssign));	// Binary Assign Func Ptr.

	m_mOps[1].Init(
			"^",									// Op. Symbol
			-1,										// Unary level
			OPLEVEL_WEDGE,							// binary level
			false,									// Right to Left
			false,									// Void Operator
			true,									// recurse lists
			0,										// Unary Func. Ptr.
			OPFUNC(TOpBinaryPtr, OpWedge),	// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[2].Init(
			".",									// Op. Symbol
			-1,										// Unary level
			OPLEVEL_POINT,							// binary level
			false,									// Right to Left
			false,									// Void Operator
			true,									// recurse lists
			0,										// Unary Func. Ptr.
			OPFUNC(TOpBinaryPtr, OpInnerProd),	// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[3].Init(
			"*",									// Op. Symbol
			OPLEVEL_DUAL,							// Unary level
			OPLEVEL_TIMES,							// binary level
			false,									// Right to Left
			false,									// Void Operator
			true,									// recurse lists
			OPFUNC(TOpUnaryPtr, OpDual),		// Unary Func. Ptr.
			OPFUNC(TOpBinaryPtr, OpProd),	// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[4].Init(
			"/",									// Op. Symbol
			-1,										// Unary level
			OPLEVEL_DIV,							// binary level
			false,									// Right to Left
			false,									// Void Operator
			true,									// recurse lists
			0,										// Unary Func. Ptr.
			OPFUNC(TOpBinaryPtr, OpDiv),	// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[5].Init(
			"-",									// Op. Symbol
			OPLEVEL_NEGATE,							// Unary level
			OPLEVEL_MINUS,							// binary level
			false,									// Right to Left
			false,									// Void Operator
			true,									// recurse lists
			OPFUNC(TOpUnaryPtr, OpNegate),	// Unary Func. Ptr.
			OPFUNC(TOpBinaryPtr, OpSubtract),	// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[6].Init(
			"+",									// Op. Symbol
			-1,										// Unary level
			OPLEVEL_PLUS,							// binary level
			false,									// Right to Left
			false,									// Void Operator
			true,									// recurse lists
			0,										// Unary Func. Ptr.
			OPFUNC(TOpBinaryPtr, OpAdd),	// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[7].Init(
			"|",									// Op. Symbol
			-1,										// Unary level
			OPLEVEL_JOIN,							// binary level
			false,									// Right to Left
			false,									// Void Operator
			true,									// recurse lists
			0,										// Unary Func. Ptr.
			OPFUNC(TOpBinaryPtr, OpJoin),	// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[8].Init(
			"&",									// Op. Symbol
			-1,										// Unary level
			OPLEVEL_MEET,							// binary level
			false,									// Right to Left
			false,									// Void Operator
			true,									// recurse lists
			0,										// Unary Func. Ptr.
			OPFUNC(TOpBinaryPtr, OpMeet),	// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[9].Init(
			"%",									// Op. Symbol
			-1,										// Unary level
			OPLEVEL_MODULUS,						// binary level
			false,									// Right to Left
			false,									// Void Operator
			true,									// recurse lists
			0,										// Unary Func. Ptr.
			OPFUNC(TOpBinaryPtr, OpMod),		// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[10].Init(
			OC_IDSYM_CALL_STR,						// Op. Symbol
			-1,										// Unary level
			OPLEVEL_FUNCTION,						// binary level
			false,									// Right to Left
			false,									// Void Operator
			false,									// recurse lists
			0,										// Unary Func. Ptr.
			OPFUNC(TOpBinaryPtr, OpFuncCall),		// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

/*
		m_mOps[10].Init(
				"#",										// Op. Symbol
				-1,											// Unary level
				OPLEVEL_GRADE,								// binary level
				false,										// Right to Left
				false,										// Void Operator
				false,										// recurse lists
				0,											// Unary Func. Ptr.
				OPFUNC(TOpBinaryPtr,OpElementSelect),		// Binary Func. Ptr.
				0,											// Unary Assign Func Ptr
				0);											// Binary Assign Func Ptr.
*/

	m_mOps[11].Init(
			"~",									// Op. Symbol
			OPLEVEL_REVERSE,						// Unary level
			OPLEVEL_STEPLIST,						// binary level
			true,									// Right to Left
			false,									// Void Operator
			true,									// recurse lists
			OPFUNC(TOpUnaryPtr, OpReverse),			// Unary Func. Ptr.
			OPFUNC(TOpBinaryPtr, OpStepList),		// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[12].Init(
			"!",									// Op. Symbol
			OPLEVEL_INVERSE,						// Unary level
			-1,										// binary level
			true,									// Right to Left
			false,									// Void Operator
			true,									// recurse lists
			OPFUNC(TOpUnaryPtr, OpInverse),	// Unary Func. Ptr.
			0,										// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[13].Init(
			"?",									// Op. Symbol
			OPLEVEL_PRINT,							// Unary level
			-1,										// binary level
			true,									// Right to Left
			true,									// Void Operator
			false,									// recurse lists
			OPFUNC(TOpUnaryPtr, OpPrint),	// Unary Func. Ptr.
			0,										// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[14].Init(
			":",									// Op. Symbol
			OPLEVEL_OGLDRAW,						// Unary level
			-1,										// binary level
			true,									// Right to Left
			true,									// Void Operator
			true,									// recurse lists
			OPFUNC(TOpUnaryPtr, OpOGLDraw),	// Unary Func. Ptr.
			0,										// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[15].Init(
			OC_IDSYM_EQUAL_STR,						// Op. Symbol
			-1,										// Unary level
			OPLEVEL_EQUAL,							// binary level
			false,									// Right to Left
			false,									// Void Operator
			true,									// recurse lists
			0,										// Unary Func. Ptr.
			OPFUNC(TOpBinaryPtr, OpEqual),			// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[16].Init(
			OC_IDSYM_NOTEQUAL_STR,					// Op. Symbol
			-1,										// Unary level
			OPLEVEL_EQUAL,							// binary level
			false,									// Right to Left
			false,									// Void Operator
			true,									// recurse lists
			0,										// Unary Func. Ptr.
			OPFUNC(TOpBinaryPtr, OpNotEqual),		// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[17].Init(
			OC_IDSYM_AND_STR,						// Op. Symbol
			-1,										// Unary level
			OPLEVEL_AND,							// binary level
			false,									// Right to Left
			false,									// Void Operator
			true,									// recurse lists
			0,										// Unary Func. Ptr.
			OPFUNC(TOpBinaryPtr, OpAND),			// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[18].Init(
			OC_IDSYM_OR_STR,						// Op. Symbol
			-1,										// Unary level
			OPLEVEL_OR,								// binary level
			false,									// Right to Left
			false,									// Void Operator
			true,									// recurse lists
			0,										// Unary Func. Ptr.
			OPFUNC(TOpBinaryPtr, OpOR),			// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[19].Init(
			OC_IDSYM_NOT_STR,						// Op. Symbol
			OPLEVEL_NOT,							// Unary level
			-1,										// binary level
			false,									// Right to Left
			false,									// Void Operator
			true,									// recurse lists
			OPFUNC(TOpUnaryPtr, OpNOT),				// Unary Func. Ptr.
			0,										// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[20].Init(
			">",									// Op. Symbol
			-1,										// Unary level
			OPLEVEL_EQUAL,							// binary level
			false,									// Right to Left
			false,									// Void Operator
			true,									// recurse lists
			0,										// Unary Func. Ptr.
			OPFUNC(TOpBinaryPtr, OpGreater),		// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[21].Init(
			"<",									// Op. Symbol
			-1,										// Unary level
			OPLEVEL_EQUAL,							// binary level
			false,									// Right to Left
			false,									// Void Operator
			true,									// recurse lists
			0,										// Unary Func. Ptr.
			OPFUNC(TOpBinaryPtr, OpLess),			// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[22].Init(
			OC_IDSYM_LSHIFT_STR,					// Op. Symbol
			-1,										// Unary level
			OPLEVEL_LSHIFT,							// binary level
			false,									// Right to Left
			false,									// Void Operator
			false,									// recurse lists
			0,										// Unary Func. Ptr.
			0,										// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			OPFUNC(TOpABinaryPtr, OpLShift));		// Binary Assign Func Ptr.

	m_mOps[23].Init(
			OC_IDSYM_GE_STR,						// Op. Symbol
			-1,										// Unary level
			OPLEVEL_EQUAL,							// binary level
			false,									// Right to Left
			false,									// Void Operator
			true,									// recurse lists
			0,										// Unary Func. Ptr.
			OPFUNC(TOpBinaryPtr, OpGreaterEqual),	// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[24].Init(
			OC_IDSYM_LE_STR,						// Op. Symbol
			-1,										// Unary level
			OPLEVEL_EQUAL,							// binary level
			false,									// Right to Left
			false,									// Void Operator
			true,									// recurse lists
			0,										// Unary Func. Ptr.
			OPFUNC(TOpBinaryPtr, OpLessEqual),		// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[25].Init(
			"°",									// Op. Symbol
			-1,										// Unary level
			OPLEVEL_GRADE,							// binary level
			false,									// Right to Left
			false,									// Void Operator
			true,									// recurse lists
			0,										// Unary Func. Ptr.
			OPFUNC(TOpBinaryPtr, OpGradeProj),		// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[26].Init(
			OC_IDSYM_REF_STR,						// Op. Symbol
			-1,										// Unary level
			OPLEVEL_ASSIGN,							// binary level
			true,									// Right to Left
			false,									// Void Operator
			false,									// recurse lists
			0,										// Unary Func. Ptr.
			0,										// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			OPFUNC(TOpABinaryPtr, OpReference));		// Binary Assign Func Ptr.

	m_mOps[27].Init(
			"'",									// Op. Symbol
			OPLEVEL_INVOLUTE,						// Unary level
			-1,										// binary level
			true,									// Right to Left
			false,									// Void Operator
			true,									// recurse lists
			OPFUNC(TOpUnaryPtr, OpInvolute),		// Unary Func. Ptr.
			0,										// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[28].Init(
			OC_IDSYM_PAND_STR,						// Op. Symbol
			-1,										// Unary level
			OPLEVEL_PAND,							// binary level
			false,									// Right to Left
			false,									// Void Operator
			false,									// recurse lists
			0,										// Unary Func. Ptr.
			OPFUNC(TOpBinaryPtr, OpPointAND),		// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[29].Init(
			OC_IDSYM_PDIV_STR,						// Op. Symbol
			-1,										// Unary level
			OPLEVEL_PDIV,							// binary level
			false,									// Right to Left
			false,									// Void Operator
			false,									// recurse lists
			0,										// Unary Func. Ptr.
			OPFUNC(TOpBinaryPtr, OpPointDiv),		// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[30].Init(
			OC_IDSYM_PMUL_STR,						// Op. Symbol
			-1,										// Unary level
			OPLEVEL_PMUL,							// binary level
			false,									// Right to Left
			false,									// Void Operator
			false,									// recurse lists
			0,										// Unary Func. Ptr.
			OPFUNC(TOpBinaryPtr, OpPointProd),		// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[31].Init(
			OC_IDSYM_PIP_STR,						// Op. Symbol
			-1,										// Unary level
			OPLEVEL_PIP,							// binary level
			false,									// Right to Left
			false,									// Void Operator
			false,									// recurse lists
			0,										// Unary Func. Ptr.
			OPFUNC(TOpBinaryPtr, OpPointInnerProd),	// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[32].Init(
			OC_IDSYM_PMINUS_STR,					// Op. Symbol
			-1,										// Unary level
			OPLEVEL_PMINUS,							// binary level
			false,									// Right to Left
			false,									// Void Operator
			false,									// recurse lists
			0,										// Unary Func. Ptr.
			OPFUNC(TOpBinaryPtr, OpPointSubtract),	// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[33].Init(
			OC_IDSYM_PGT_STR,						// Op. Symbol
			-1,										// Unary level
			OPLEVEL_EQUAL,							// binary level
			false,									// Right to Left
			false,									// Void Operator
			false,									// recurse lists
			0,										// Unary Func. Ptr.
			OPFUNC(TOpBinaryPtr, OpPointGreater),	// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[34].Init(
			OC_IDSYM_PPLUS_STR,						// Op. Symbol
			-1,										// Unary level
			OPLEVEL_PPLUS,							// binary level
			false,									// Right to Left
			false,									// Void Operator
			false,									// recurse lists
			0,										// Unary Func. Ptr.
			OPFUNC(TOpBinaryPtr, OpPointAdd),		// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[35].Init(
			OC_IDSYM_POP_STR,						// Op. Symbol
			-1,										// Unary level
			OPLEVEL_POP,							// binary level
			false,									// Right to Left
			false,									// Void Operator
			false,									// recurse lists
			0,										// Unary Func. Ptr.
			OPFUNC(TOpBinaryPtr, OpPointWedge),		// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[36].Init(
			OC_IDSYM_PLSHIFT_STR,					// Op. Symbol
			-1,										// Unary level
			OPLEVEL_LSHIFT,							// binary level
			false,									// Right to Left
			false,									// Void Operator
			false,									// recurse lists
			0,										// Unary Func. Ptr.
			0,										// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			OPFUNC(TOpABinaryPtr, OpPointLShift));	// Binary Assign Func Ptr.

	m_mOps[37].Init(
			OC_IDSYM_POR_STR,						// Op. Symbol
			-1,										// Unary level
			OPLEVEL_POR,							// binary level
			false,									// Right to Left
			false,									// Void Operator
			false,									// recurse lists
			0,										// Unary Func. Ptr.
			OPFUNC(TOpBinaryPtr, OpPointOR),		// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[38].Init(
			OC_IDSYM_PLT_STR,						// Op. Symbol
			-1,										// Unary level
			OPLEVEL_EQUAL,							// binary level
			false,									// Right to Left
			false,									// Void Operator
			false,									// recurse lists
			0,										// Unary Func. Ptr.
			OPFUNC(TOpBinaryPtr, OpPointLess),		// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[39].Init(
			OC_IDSYM_PGTE_STR,						// Op. Symbol
			-1,										// Unary level
			OPLEVEL_EQUAL,							// binary level
			false,									// Right to Left
			false,									// Void Operator
			false,									// recurse lists
			0,										// Unary Func. Ptr.
			OPFUNC(TOpBinaryPtr, OpPointGreaterEqual),	// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[40].Init(
			OC_IDSYM_PLTE_STR,						// Op. Symbol
			-1,										// Unary level
			OPLEVEL_EQUAL,							// binary level
			false,									// Right to Left
			false,									// Void Operator
			false,									// recurse lists
			0,										// Unary Func. Ptr.
			OPFUNC(TOpBinaryPtr, OpPointLessEqual),	// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[41].Init(
			OC_IDSYM_PEQUAL_STR,					// Op. Symbol
			-1,										// Unary level
			OPLEVEL_EQUAL,							// binary level
			false,									// Right to Left
			false,									// Void Operator
			false,									// recurse lists
			0,										// Unary Func. Ptr.
			OPFUNC(TOpBinaryPtr, OpPointEqual),		// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[42].Init(
			OC_IDSYM_PNEQUAL_STR,					// Op. Symbol
			-1,										// Unary level
			OPLEVEL_EQUAL,							// binary level
			false,									// Right to Left
			false,									// Void Operator
			false,									// recurse lists
			0,										// Unary Func. Ptr.
			OPFUNC(TOpBinaryPtr, OpPointNotEqual),	// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	m_mOps[43].Init(
			OC_IDSYM_POW_STR,						// Op. Symbol
			-1,										// Unary level
			OPLEVEL_POWER,							// binary level
			false,									// Right to Left
			false,									// Void Operator
			false,									// recurse lists
			0,										// Unary Func. Ptr.
			OPFUNC(TOpBinaryPtr, OpPow),			// Binary Func. Ptr.
			0,										// Unary Assign Func Ptr
			0);										// Binary Assign Func Ptr.

	// Register Operators
	for (int i = 0; i < iOpNo; i++)
	{
		if (!AddObjectClass(&m_mOps[i]))
		{
			return false;
		}
	}

	int iMod, iModCnt = int(m_vecModule.size());
	const SMapExtFunc* pMapFunc;
	int iFunc, iFuncCnt;

	for (iMod = 0; iMod < iModCnt; ++iMod)
	{
		if (!m_vecModule[iMod].pfGetFuncList(&pMapFunc, &iFuncCnt))
		{
			continue;
		}

		for (iFunc = 0; iFunc < iFuncCnt; ++iFunc)
		{
			AddFunc(pMapFunc[iFunc].pcName, pMapFunc[iFunc].pFunc);
			m_vecFuncName.push_back(string(pMapFunc[iFunc].pcName));
		}
	}

	m_xCodeBase.SetCLUParse(this);

	///////////////////////////////////////////////////////
	/// Predefined Variables

	/// Colors
	DefineColor("Red",   1.0f, 0.0f, 0.0f);
	DefineColor("Green", 0.0f, 1.0f, 0.0f);
	DefineColor("Blue",  0.0f, 0.0f, 1.0f);

	DefineColor("Cyan",   0.0f, 1.0f, 1.0f);
	DefineColor("Yellow", 1.0f, 1.0f, 0.0f);
	DefineColor("Magenta",  1.0f, 0.0f, 1.0f);

	DefineColor("Orange",   1.0f, 0.7f, 0.0f);
	DefineColor("White", 1.0f, 1.0f, 1.0f);
	DefineColor("Black",  0.0f, 0.0f, 0.0f);

	DefineColor("MRed",   0.6f, 0.0f, 0.0f);
	DefineColor("MGreen", 0.0f, 0.6f, 0.0f);
	DefineColor("MBlue",  0.0f, 0.0f, 0.6f);

	DefineColor("MCyan",   0.0f, 0.6f, 0.6f);
	DefineColor("MYellow", 0.6f, 0.6f, 0.0f);
	DefineColor("MMagenta",  0.6f, 0.0f, 0.6f);

	DefineColor("MOrange",   0.6f, 0.1f, 0.0f);
	DefineColor("MGray", 0.6f, 0.6f, 0.6f);
	DefineColor("MWhite", 0.6f, 0.6f, 0.6f);

	DefineColor("DRed",   0.3f, 0.0f, 0.0f);
	DefineColor("DGreen", 0.0f, 0.3f, 0.0f);
	DefineColor("DBlue",  0.0f, 0.0f, 0.3f);

	DefineColor("DCyan",   0.0f, 0.3f, 0.3f);
	DefineColor("DYellow", 0.3f, 0.3f, 0.0f);
	DefineColor("DMagenta",  0.3f, 0.0f, 0.3f);

	DefineColor("DOrange",   0.3f, 0.1f, 0.0f);
	DefineColor("DGray", 0.3f, 0.3f, 0.3f);
	DefineColor("DWhite", 0.3f, 0.3f, 0.3f);

	DefineColor("Alpha", 0.0f, 0.0f, 0.0f, -0.2f);
	DefineColor("MAlpha", 0.0f, 0.0f, 0.0f, -0.4f);
	DefineColor("DAlpha", 0.0f, 0.0f, 0.0f, -0.7f);

	DefineString("ToolName", "None");
	DefineString("LinkData", "None");
	DefineList("PickData");

	DefineInt("true", 1);
	DefineInt("false", 0);

	DefineInt("ExecMode", 0);
	DefineInt("EM_NEW", EXEC_MODE_NEW);
	DefineInt("EM_CHANGE", EXEC_MODE_CHANGE);
	DefineInt("EM_ANIMATE", EXEC_MODE_ANIM);
	DefineInt("EM_USER", EXEC_MODE_USER);
	DefineInt("EM_TOOL", EXEC_MODE_TOOL);
	DefineInt("EM_AGAIN", EXEC_MODE_AGAIN);
	DefineInt("EM_KEYUP", EXEC_MODE_KEYUP);
	DefineInt("EM_KEYDOWN", EXEC_MODE_KEYDOWN);
	DefineInt("EM_RESHAPE", EXEC_MODE_RESHAPE);
	DefineInt("EM_LINK", EXEC_MODE_LINK);
	DefineInt("EM_PICK_OVER", EXEC_MODE_PICKOVER);
	DefineInt("EM_PICK_SELECT", EXEC_MODE_PICKSEL);
	DefineInt("EM_PICK_DRAG", EXEC_MODE_PICKDRAG);
	DefineInt("EM_QUIT", EXEC_MODE_QUIT);

	DefineInt("MVOP_GEO", int(MVOP_GEO));
	DefineInt("MVOP_INNER", int(MVOP_INNER));
	DefineInt("MVOP_OUTER", int(MVOP_OUTER));
	DefineInt("MVOP_ADD", int(MVOP_ADD));
	DefineInt("MVOP_SUB", int(MVOP_SUB));
	DefineInt("MVOP_DUAL", int(MVOP_DUAL));
	DefineInt("MVOP_REVERSE", int(MVOP_REVERSE));
	DefineInt("MVOP_INVOLUTE", int(MVOP_INVOLUTE));

	DefineInt("GA_E3", int(ID_E3GA));
	#ifdef USE_GA_E8
		DefineInt("GA_E8", int(ID_E8GA));
	#endif
	DefineInt("GA_P3", int(ID_PGA));
	DefineInt("GA_N3", int(ID_CONFGA));
	DefineInt("GA_C2", int(ID_EL2GA));
	#ifdef USE_GA_CC2
		DefineInt("GA_NC2", int(ID_CC2GA));
	#endif

	DefineInt("OPNS", MODE_OPNS);
	DefineInt("IPNS", MODE_IPNS);

	DefineInt("E3_DRAW_VEC_AS_LINE", E3M_DRAW_VEC_AS_LINE);
	DefineInt("E3_DRAW_VEC_AS_ARROW", E3M_DRAW_VEC_AS_ARROW);
	DefineInt("E3_DRAW_VEC_AS_POINT", E3M_DRAW_VEC_AS_POINT);

	DefineInt("E3_DRAW_PLANE_AS_RECT", E3M_DRAW_PLANE_AS_RECT);
	DefineInt("E3_DRAW_PLANE_AS_DISK", E3M_DRAW_PLANE_AS_DISK);

	DefineInt("N3_OPNS", N3M_OPNS);
	DefineInt("N3_IPNS", N3M_IPNS);
	DefineInt("N3_SOLID", N3M_SOLID);
	DefineInt("N3_WIRE", N3M_WIRE);
	DefineInt("N3_SHOW_IMAG", N3M_SHOW_IMAG);
	DefineInt("N3_HIDE_IMAG", N3M_HIDE_IMAG);

	DefineInt("DRAW_POINT_AS_DOT", DRAW_POINT_AS_DOT);
	DefineInt("DRAW_POINT_AS_SPHERE", DRAW_POINT_AS_SPHERE);

	DefineInt("DRAW_LINE_AS_LINE", DRAW_LINE_AS_LINE);
	DefineInt("DRAW_LINE_AS_CYLINDER", DRAW_LINE_AS_CYLINDER);

	// Plot Modes
	DefineInt("PLOT_POINTS", PLOT_POINTS);
	DefineInt("PLOT_LINES", PLOT_LINES);
	DefineInt("PLOT_FILL", PLOT_FILL);

	// Vertex List Types
	DefineInt("OM_POINTS", GL_POINTS);
	DefineInt("OM_LINES", GL_LINES);
	DefineInt("OM_LINE_STRIP", GL_LINE_STRIP);
	DefineInt("OM_LINE_LOOP", GL_LINE_LOOP);
	DefineInt("OM_TRIANGLES", GL_TRIANGLES);
	DefineInt("OM_TRIANGLE_STRIP", GL_TRIANGLE_STRIP);
	DefineInt("OM_TRIANGLE_FAN", GL_TRIANGLE_FAN);
	DefineInt("OM_QUADS", GL_QUADS);
	DefineInt("OM_QUAD_STRIP", GL_QUAD_STRIP);
	DefineInt("OM_POLYGON", GL_POLYGON);
	DefineInt("OM_LINES_ADJACENCY", GL_LINES_ADJACENCY);
	DefineInt("OM_LINE_STRIP_ADJACENCY", GL_LINE_STRIP_ADJACENCY);
	DefineInt("OM_TRIANGLES_ADJACENCY", GL_TRIANGLES_ADJACENCY);
	DefineInt("OM_TRIANGLE_STRIP_ADJACENCY", GL_TRIANGLE_STRIP_ADJACENCY);

	// Image Data Types
	DefineString("IMG_8", "8");
	DefineString("IMG_U8", "u8");
	DefineString("IMG_16", "16");
	DefineString("IMG_U16", "u16");
	DefineString("IMG_32", "32");
	DefineString("IMG_U32", "u32");
	DefineString("IMG_FLOAT", "float");
	DefineString("IMG_DOUBLE", "double");

	// Image Types
	DefineString("IMG_RGB", "rgb");
	DefineString("IMG_RGBA", "rgba");
	DefineString("IMG_BGR", "bgr");
	DefineString("IMG_BGRA", "bgra");
	DefineString("IMG_LUM", "lum");
	DefineString("IMG_LUMA", "luma");

	//// Image Data Types
	//DefineInt("IMG_8", CLUVIZ_IMG_BYTE );
	//DefineInt("IMG_U8", CLUVIZ_IMG_UNSIGNED_BYTE );
	//DefineInt("IMG_16", CLUVIZ_IMG_SHORT );
	//DefineInt("IMG_U16", CLUVIZ_IMG_UNSIGNED_SHORT );
	//DefineInt("IMG_32", CLUVIZ_IMG_INT );
	//DefineInt("IMG_U32", CLUVIZ_IMG_UNSIGNED_INT );
	//DefineInt("IMG_FLOAT", CLUVIZ_IMG_FLOAT );
	//DefineInt("IMG_DOUBLE", CLUVIZ_IMG_DOUBLE );

	//// Image Types
	//DefineInt("IMG_RGB", CLUVIZ_IMG_RGB );
	//DefineInt("IMG_RGBA", CLUVIZ_IMG_RGBA );
	//DefineInt("IMG_BGR", CLUVIZ_IMG_BGR );
	//DefineInt("IMG_BGRA", CLUVIZ_IMG_BGRA );
	//DefineInt("IMG_LUM", CLUVIZ_IMG_LUMINANCE );
	//DefineInt("IMG_LUMA", CLUVIZ_IMG_LUMINANCE_ALPHA );

	// Fonts
/*
		DefineInt("FONT_FixedSys_Std_9", OGL_BMPFONT_FixedSys_Std_9);
		DefineInt("FONT_FixedSys_Bold_9", OGL_BMPFONT_FixedSys_Bold_9);
		DefineInt("FONT_Times_Std_12", OGL_BMPFONT_Times_Std_12);
		DefineInt("FONT_Sylfaen_Bold_24", OGL_BMPFONT_Sylfaen_Bold_24);
		DefineInt("FONT_Symbol_Std_12", OGL_BMPFONT_Symbol_Std_12);
		DefineInt("FONT_Symbol_Bold_12", OGL_BMPFONT_Symbol_Bold_12);
		DefineInt("FONT_Symbol_Std_24", OGL_BMPFONT_Symbol_Std_24);
		DefineInt("FONT_T2_Std_12", OGL_BMPFONT_T2_Std_12);
		DefineInt("FONT_T2_Std_24", OGL_BMPFONT_T2_Std_24);
*/

	DefineDouble("dTime", 0.0);
	DefineDouble("Time", 0.0);
	DefineDouble("Pi", 3.14159265359);
	DefineDouble("RadPerDeg", 3.14159265359 / 180.0);

	//printf("CLUParse:Init: End\n");
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Define Color

bool CCLUParse::DefineColor(const char* pcName, float fR, float fG, float fB, float fA, bool bProtect)
{
	CCodeVar* pVar;

	if (!pcName)
	{
		return false;
	}

	m_xCodeBase.DeleteConstVar(pcName);

	pVar = &m_xCodeBase.NewConstVar(pcName, PDT_COLOR);
	((COGLColor*) pVar->Val())->Set(fR, fG, fB, fA);

	if (bProtect)
	{
		pVar->EnableProtect();
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Define Float

bool CCLUParse::DefineFloat(const char* pcName, float fVal, bool bProtect)
{
	CCodeVar* pVar;

	if (!pcName)
	{
		return false;
	}

	m_xCodeBase.DeleteConstVar(pcName);
	pVar                    = &m_xCodeBase.NewConstVar(pcName, PDT_FLOAT);
	*((float*) pVar->Val()) = fVal;

	if (bProtect)
	{
		pVar->EnableProtect();
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Define Double

bool CCLUParse::DefineDouble(const char* pcName, double dVal, bool bProtect)
{
	CCodeVar* pVar;

	if (!pcName)
	{
		return false;
	}

	m_xCodeBase.DeleteConstVar(pcName);
	pVar                     = &m_xCodeBase.NewConstVar(pcName, PDT_DOUBLE);
	*((double*) pVar->Val()) = dVal;

	if (bProtect)
	{
		pVar->EnableProtect();
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Define Integer

bool CCLUParse::DefineInt(const char* pcName, int iVal, bool bProtect)
{
	CCodeVar* pVar;

	if (!pcName)
	{
		return false;
	}

	m_xCodeBase.DeleteConstVar(pcName);
	pVar                  = &m_xCodeBase.NewConstVar(pcName, PDT_INT);
	*((int*) pVar->Val()) = iVal;

	if (bProtect)
	{
		pVar->EnableProtect();
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Define String

bool CCLUParse::DefineString(const char* pcName, const char* pcVal, bool bProtect)
{
	CCodeVar* pVar;

	if (!pcName)
	{
		return false;
	}

	m_xCodeBase.DeleteConstVar(pcName);
	pVar                      = &m_xCodeBase.NewConstVar(pcName, PDT_STRING);
	*((TString*) pVar->Val()) = pcVal;

	if (bProtect)
	{
		pVar->EnableProtect();
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Define List

bool CCLUParse::DefineList(const char* pcName, bool bProtect)
{
	CCodeVar* pVar;

	if (!pcName)
	{
		return false;
	}

	m_xCodeBase.DeleteConstVar(pcName);
	pVar = &m_xCodeBase.NewConstVar(pcName, PDT_VARLIST);

	if (bProtect)
	{
		pVar->EnableProtect();
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Define Version

bool CCLUParse::DefineVersion()
{
	CCodeVar* pVar;

	m_xCodeBase.DeleteConstVar("VERSION");
	pVar = &m_xCodeBase.NewConstVar("VERSION", PDT_VARLIST);
	TVarList& rList = *(pVar->GetVarListPtr());

	int iMaj, iMin, iRev;
	GetVersion(iMaj, iMin, iRev);

	rList.Set(3);
	rList[0] = iMaj;
	rList[1] = iMin;
	rList[2] = iRev;

	pVar->EnableProtect();

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Define OpenGL Version

bool CCLUParse::DefineVersionGL(int iMajor, int iMinor)
{
	CCodeVar* pVar;

	m_xCodeBase.DeleteConstVar("VERSION_GL");
	pVar = &m_xCodeBase.NewConstVar("VERSION_GL", PDT_VARLIST);
	TVarList& rList = *(pVar->GetVarListPtr());

	rList.Set(2);
	rList[0] = iMajor;
	rList[1] = iMinor;

	pVar->EnableProtect();

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Define OpenGL Shading Language Version

bool CCLUParse::DefineVersionGLSL(int iMajor, int iMinor)
{
	CCodeVar* pVar;

	m_xCodeBase.DeleteConstVar("VERSION_GLSL");
	pVar = &m_xCodeBase.NewConstVar("VERSION_GLSL", PDT_VARLIST);
	TVarList& rList = *(pVar->GetVarListPtr());

	rList.Set(2);
	rList[0] = iMajor;
	rList[1] = iMinor;

	pVar->EnableProtect();

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Get position of parse error

bool CCLUParse::GetParseErrorPos(int& iLine, int& iPos, int& iInputPos, CStrMem& csFilename)
{
	CStrMem csLine, csError, csStr = "", csText = "";

	iLine     = -1;
	iPos      = -1;
	iInputPos = -1;

	if (!GetLineCount())
	{
		return false;
	}

	GetCurLine(csLine, iLine, iPos);

	STextLine& rLine = m_msText[iLine];
//	if (rLine.iMainFilePos < 0)
//	{
	if (iPos >= int(rLine.vecPos.size()))
	{
		iInputPos = rLine.vecPos.back();
	}
	else
	{
		iInputPos = rLine.vecPos[iPos];
	}
//	}
//	else
//	{
//		iInputPos = rLine.iMainFilePos;
//	}

	if (rLine.csFilename.Len())
	{
		csFilename = rLine.csFilename;
	}
	else
	{
		csFilename = m_sScriptName.c_str();
	}

	if ((csFilename[0] != '\\') &&
		(csFilename[0] != '/') &&
		!strchr(csFilename.Str(), ':'))
	{
		csFilename = m_sScriptPath.c_str() + csFilename;
	}

	iLine = GetTextLineNo(iLine) + 1;
	iPos++;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Print Code Errors

CStrMem CCLUParse::PrintCodeErrors()
{
	CStrMem csLine, csError, csStr = "", csText, csLevel;
	int iPos, iLine, iErrNo, iMsgCount, iMsg, iTextLine, iLen, iTextLineOffset;

	CCodeErrorList& rErrList = m_xCodeBase.m_ErrorList;

	iMsgCount = int(rErrList.Count());

	for (iMsg = 0; iMsg < iMsgCount; iMsg++)
	{
		csText = "";
		csStr  = "";

		SMsg& rMsg = rErrList[iMsg];

		iLine           = rMsg.iLine;
		iTextLineOffset = rMsg.iTextLineOffset;
		iPos            = rMsg.iPos;
		iErrNo          = rMsg.iNo;
		csError         = rMsg.csText;

		switch (rMsg.iLevel)
		{
		case CEL_FATAL:
			csLevel = "Fatal Error";
			break;

		case CEL_ERROR:
			csLevel = "Error";
			break;

		case CEL_WARNING:
			csLevel = "Warning";
			break;

		default:
			csLevel = "Message";
			break;
		}

		//csText << csLevel << " No. " << iErrNo << ": " << csError << "<br>";
		csText << csLevel << " No. " << iErrNo << ":<br>";

		string sError = csError.Str();
		size_t nNewLineIdx, nStartIdx = 0;
		while (true)
		{
			nNewLineIdx = sError.find("\n", nStartIdx);
			if (nNewLineIdx == string::npos)
			{
				csText << sError.substr(nStartIdx, nNewLineIdx - nStartIdx).c_str();
				break;
			}
			else
			{
				if (nNewLineIdx - nStartIdx == 0)
				{
					csText << "<p>";
				}
				else
				{
					csText << sError.substr(nStartIdx, nNewLineIdx - nStartIdx).c_str();
					csText << "<br>";
				}
			}
			nStartIdx = nNewLineIdx + 1;
		}

		csText << "<br><p>";

		if (iLine >= 0)
		{
			if (GetLineCount())
			{
				csLine    = GetLine(iLine);
				iTextLine = GetTextLineNo(iLine) + iTextLineOffset;
			}
			else
			{
				csLine    = "";
				iTextLine = 0;
			}

			//iLen = csLine.Len();

			if ((iTextLine >= 0) && (iPos >= 0) && (iLine < int(m_msText.size())))
			{
				int iRelPos;
				STextLine& rLine = m_msText[iLine];

				if (rLine.csFilename.Len() > 0)
				{
					csText << "In file '<b>" << rLine.csFilename << "</b>',<br>";
				}

				csStr << "<i>Line " << (iTextLine + 1) << " (" << (iPos + 1) << ")</i>: ";

				if (rLine.csInputText.Len())
				{
					/*
					int i, iSize = rLine.vecPos.size();
					for(i=0;i<iSize;i++)
					{
					csStr << rLine.vecPos[i] << ", ";
					}
					csStr << "\n";
					*/
					string sTextLine = rLine.csInputText.Str();
					iLen = int(sTextLine.size());

					if (iPos >= int(rLine.vecPos.size()))
					{
						iRelPos    = rLine.vecPos.back() + 1 - rLine.iInputTextStartPos;
						sTextLine += " ";
					}
					else
					{
						iRelPos = rLine.vecPos[iPos] - rLine.iInputTextStartPos;
					}

					if (iRelPos < 0)
					{
						iRelPos = 0;
					}

					int iStart = int(sTextLine.rfind('\n', iRelPos));
					if (iStart < 0)
					{
						iStart = 0;
					}
					else
					{
						iStart++;
					}

					int iEnd = int(sTextLine.find('\n', iRelPos));
					if (iEnd < 0)
					{
						iEnd = int(sTextLine.size() - 1);
					}
					else
					{
						iEnd--;
					}

					csText << csStr << "<code><b>" << sTextLine.substr(iStart, iRelPos - iStart + 1).c_str();
					csText << "@" << sTextLine.substr(iRelPos + 1, iEnd - iRelPos + 1).c_str() << "</b></code><br>";
				}
			}
		}
	}

	return csText;
}

//////////////////////////////////////////////////////////////////////
std::string CCLUParse::PrintParseErrors()
{
	return CCLUPreParse::PrintParseErrors(m_msText, m_iCurLine, m_iCurPos);
}

//////////////////////////////////////////////////////////////////////
/// Get Code Error Position

bool CCLUParse::GetCodeErrorPos(int& iLine, int& iPos, int& iInputPos, CStrMem& csFilename)
{
	CStrMem csLine, csError, csStr = "", csText, csLevel;
	int iMsgCount;

	iLine = -1;
	iPos  = -1;

	CCodeErrorList& rErrList = m_xCodeBase.m_ErrorList;

	if (m_msText.size() == 0)
	{
		return false;
	}

	if ((iMsgCount = int(rErrList.Count())) <= 0)
	{
		return false;
	}

	SMsg& rMsg = rErrList[iMsgCount - 1];

	iLine = GetTextLineNo(rMsg.iLine) + 1;
	if (iLine <= 0)
	{
		// We are in a binary file
		iLine = rMsg.iLine;
		iPos  = rMsg.iPos + 1;

		csFilename = m_sScriptName.c_str();
	}
	else
	{
		// Text file
		iPos = rMsg.iPos + 1;

		STextLine& rLine = m_msText[rMsg.iLine];

		if (rLine.vecPos.size() == 0)
		{
			iInputPos = -1;
		}
		else if (rMsg.iPos >= int(rLine.vecPos.size()))
		{
			iInputPos = rLine.vecPos.back();
		}
		else
		{
			iInputPos = rLine.vecPos[rMsg.iPos];
		}

		if (rLine.csFilename.Len())
		{
			csFilename = rLine.csFilename;
		}
		else
		{
			csFilename = m_sScriptName.c_str();
		}
	}

	if ((csFilename[0] != '\\') &&
		(csFilename[0] != '/') &&
		!strchr(csFilename.Str(), ':'))
	{
		csFilename = m_sScriptPath.c_str() + csFilename;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Print Vars

CStrMem CCLUParse::PrintVars()
{
	CStrMem csText;

	CCodeVarList::TVarMapIt itEl;

	itEl = m_xCodeBase.GetVarBegin();
	while (itEl != m_xCodeBase.GetVarEnd())
	{
		CCodeVar& rVar = itEl->second;

		csText << rVar.Name().c_str() << ", ";
		csText << rVar.TypeStr().c_str() << ", ";
		csText << rVar.ValStr() << "\n";
	}

	//int iVar, iVarCount = m_xCodeBase.VarCount();

	//for(iVar=0;iVar<iVarCount;iVar++)
	//{
	//	CCodeVar& rVar = m_xCodeBase.GetVar(iVar);

	//	csText << rVar.Name() << ", ";
	//	csText << rVar.TypeStr() << ", ";
	//	csText << rVar.ValStr() << "\n";
	//}

	return csText;
}

//////////////////////////////////////////////////////////////////////
/// Print Stack

CStrMem CCLUParse::PrintStack()
{
	CStrMem csText;

	int iVar, iVarCount = m_xCodeBase.StackCount();

	for (iVar = 0; iVar < iVarCount; iVar++)
	{
		CCodeVar* pVar = m_xCodeBase.GetStackVar(iVar);

		csText << "<b>" << iVar << "</b>: ";
		if (pVar->IsTypeValid())
		{
			csText << pVar->Name().c_str() << ", ";
			csText << pVar->TypeStr().c_str() << ", ";
			csText << pVar->ValStr() << "<br>";
		}
		else
		{
			csText << "<b>Uninitialized variable</b><br>";
		}
	}

	return csText;
}

//////////////////////////////////////////////////////////////////////
// Run Code starting at line iStartLine for iLineCount lines.
// if iStartLine == 0 then start at beginning.
// if iLineCount == -1 then run all lines after iStartLine.

bool CCLUParse::RunCode(int iStartLine, int iLineCount)
{
	// Reset Environment variables like font type and scale, etc.
	m_xCodeBase.ResetEnvVars();

	ResetOutputObjectList();
	//m_xCodeBase.DrawScriptFileIndex();

	bool bRet = CParse::RunCode(iStartLine, iLineCount);

	// Clean up things the user might have forgotten.
	m_xCodeBase.CleanUp();

	return bRet;
}

// Insert Text pcText at position iPos and parse it if bParse is true.
// If iPos == -1 then add text to end.
// Returns number of lines read. Returns -1 if error occured.

int CCLUParse::InsertText(const char* pcText, int iPos, bool bParse)
{
	m_vecIncludeFilename.resize(1);
	m_vecIncludeFilename[0] = m_sScriptName + ".clu";

	return CParse::InsertText(pcText, iPos, bParse);
}

/////////////////////////////////////////////////////////////////
//// Set/Add Text Code

int CCLUParse::SetTextCode(const char* pcCode, const char* pcFilename, bool bParse, bool bAdd)
{
	int iLineCnt = int(m_msText.size());

	int iNewLineCnt = CCLUPreParse::SetTextCode(m_msText, pcCode, pcFilename, bAdd);

	if (bParse)
	{
		if (!Parse(iLineCnt, iNewLineCnt))
		{
			return -1;
		}
	}

	return iNewLineCnt;
}

/////////////////////////////////////////////////////////////////
bool CCLUParse::GenTextCode(std::vector<char>& vecCode)
{
	return CCLUPreParse::GenTextCode(vecCode, m_msText);
}

//////////////////////////////////////////////////////////////////////
/// PreParse Text. Called by InsertText
/// Insert text separated into lines before iPos.
/// If iPos == -1 then lines are added to end.
/// Separate Text into lines using m_cSepSym as separation symbol.

int CCLUParse::PreParse(const char* _pcText, int iPos, const char* pcFilename, int iMainFilePos, bool bUseScriptPath)
{
	return DoPreParse(m_msText, m_iCurLine, m_iCurPos, _pcText, iPos, pcFilename, iMainFilePos, bUseScriptPath);
}
