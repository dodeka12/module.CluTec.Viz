////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_Shader.cpp
//
// summary:   Implements the function shader class
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

#include "stdafx.h"

#include "Func_Shader.h"

#include "CluTec.Viz.Draw\OGLShader.h"
#include "CluTec.Viz.Draw\OGLAnimShader.h"
#include "CluTec.Viz.OpenGL.Extensions/Extensions.h"
#include "CluTec.Viz.OpenGL\Api.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Create Shader Object Function

bool ShaderFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!Clu::OpenGL::Extensions::IsShaderAvailable())
	{
		rCB.GetErrorList().GeneralError("Shader not available on present graphics hardware.", iLine, iPos);
		return false;
	}

	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount != 1)
	{
		int piPar[] = { 1 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("Expect name of shader as parameter.", iLine, iPos);
		return false;
	}

	COGLShader* pShader = new COGLShader;

	if (!pShader)
	{
		rCB.GetErrorList().GeneralError("Out of memory while creating shader.", iLine, iPos);
		return false;
	}

	pShader->SetName(mVars(0).GetStringPtr()->Str());
	COGLBEReference ShaderRef = rCB.GetOGLDrawBase()->GetSceneRepository()->New(pShader);

	rVar = ShaderRef;

	return true;
}

//////////////////////////////////////////////////////////////////////
// Check whether shaders are available on graphics hardware

bool ShaderSupportedFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount != 0)
	{
		int piPar[] = { 0 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	rVar = TCVCounter(Clu::OpenGL::Extensions::IsShaderAvailable());

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// Enable Shader

bool EnableShaderFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount      = int(mVars.Count());
	TCVCounter iEnable = 1;

	if ((iVarCount < 1) || (iVarCount > 2))
	{
		int piPar[] = { 1, 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a shader variable as parameter.", iLine, iPos);
		return false;
	}

	if (iVarCount > 1)
	{
		if (!mVars(1).CastToCounter(iEnable))
		{
			rCB.GetErrorList().GeneralError("Expect true or false as second parameter.", iLine, iPos);
			return false;
		}
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Shader is not valid.", iLine, iPos);
		return false;
	}

	COGLShader* pShader = dynamic_cast< COGLShader* >((COGLBaseElement*) Scene);
	if (!pShader)
	{
		rCB.GetErrorList().GeneralError("Shader is not valid.", iLine, iPos);
		return false;
	}

	if (iEnable)
	{
		pShader->Enable(true);
	}
	else
	{
		pShader->Enable(false);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// Enable Shader For Picking

bool EnableShaderForPickingFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount      = int(mVars.Count());
	TCVCounter iEnable = 1;

	if ((iVarCount < 1) || (iVarCount > 2))
	{
		int piPar[] = { 1, 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a shader variable as parameter.", iLine, iPos);
		return false;
	}

	if (iVarCount > 1)
	{
		if (!mVars(1).CastToCounter(iEnable))
		{
			rCB.GetErrorList().GeneralError("Expect true or false as second parameter.", iLine, iPos);
			return false;
		}
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Shader is not valid.", iLine, iPos);
		return false;
	}

	COGLShader* pShader = dynamic_cast< COGLShader* >((COGLBaseElement*) Scene);
	if (!pShader)
	{
		rCB.GetErrorList().GeneralError("Shader is not valid.", iLine, iPos);
		return false;
	}

	if (iEnable)
	{
		pShader->EnableForPicking(true);
	}
	else
	{
		pShader->EnableForPicking(false);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
void CreateShaderErrorOutput(string& sText, int& iCodeStartLine, int& iCodeLineOffset, int& iCodePosOffset, const vector<string>& vecCode, const string& sLog)
{
	size_t nNewLineIdx, nStartIdx = 0, nIdx, nIdx2;
	int iLineIdx;
	string sCodeIdx, sLineIdx, sCodeLine;
	string sDigit = "0123456789";
	bool bValidIdx, bValidCodeLine;

	sText           = "";
	iCodeStartLine  = -1;
	iCodeLineOffset = -1;
	iCodePosOffset  = -1;

	bool bIsFirstError = true;

	// ////////////////////////////////////////////////////////////////////////////////
	// Analyze Code lines
	// vecFilePos: List of file positions where shader code strings are defined. One entry for each shader code in list.
	// vecLastLine: If a line number is smaller or equal than the entry n in this list, the line is in code n.
	// vecLineEndIdx: The list of character indices of the line break characters in all code strings. The first entry in this list
	//					gives the start index in the code string of the first line. The index is always relative the
	//					code string in question. That is, you first have to find out in which code string a line is with vecLastLine,
	//					and then find the position within that string with this list.
	size_t nCodeCnt = vecCode.size();
	vector<size_t> vecFileLine, vecLineEndIdx, vecLastLine;
	size_t nLineCnt = 0;

	for (size_t nCodeIdx = 0; nCodeIdx < nCodeCnt; ++nCodeIdx)
	{
		const string& sCode = vecCode[nCodeIdx];

		// ///////////////////////////////////////////////////////////
		// Find position of shader code in file
		size_t nIdx, nIdx2, nLineOffset;
		string sValue;
		int iValue = -1;
		if ((nIdx = sCode.find("//# File Position:")) != string::npos)
		{
			nIdx += 18;
			if ((nIdx2 = sCode.find(":", nIdx)) != string::npos)
			{
				sValue = sCode.substr(nIdx, nIdx2 - nIdx);
				sscanf_s(sValue.c_str(), "%d", &iValue);
			}
			nLineOffset = 1;
		}
		vecFileLine.push_back(iValue);

		// ///////////////////////////////////////////////////////////
		// Find all line ends in shader code
		nIdx = 0;
		if (nLineOffset == 0)
		{
			vecLineEndIdx.push_back(0);
		}

		while ((nIdx2 = sCode.find("\n", nIdx)) != string::npos)
		{
			if (nLineOffset == 0)
			{
				vecLineEndIdx.push_back(nIdx2);
				++nLineCnt;
			}
			else
			{
				--nLineOffset;
				if (nLineOffset == 0)
				{
					vecLineEndIdx.push_back(nIdx2 + 1);
				}
			}
			nIdx = nIdx2 + 1;
		}

		if (vecLineEndIdx.back() < sCode.length() - 1)
		{
			vecLineEndIdx.push_back(sCode.length() - 1);
			++nLineCnt;
		}

		vecLastLine.push_back(nLineCnt);
	}

	// ////////////////////////////////////////////////////////////////////////////////
	// loop over lines in log
	do
	{
		string sLogLine;

		// find start and end index of line in log
		if ((nNewLineIdx = sLog.find("\n", nStartIdx)) != string::npos)
		{
			sLogLine  = sLog.substr(nStartIdx, nNewLineIdx - nStartIdx);
			nStartIdx = nNewLineIdx + 1;
		}
		else
		{
			sLogLine  = sLog.substr(nStartIdx);
			nStartIdx = string::npos;
		}

		if (sLogLine.length() == 0)
		{
			if (nStartIdx == string::npos)
			{
				break;
			}
			else
			{
				continue;
			}
		}

		bValidIdx      = false;
		bValidCodeLine = false;
		nIdx2          = string::npos;

		// find code index and line number of log message
		if ((nIdx = sLogLine.find_first_of(sDigit)) != string::npos)
		{
			if ((nIdx2 = sLogLine.find_first_not_of(sDigit, nIdx)) != string::npos)
			{
				sCodeIdx = sLogLine.substr(nIdx, nIdx2 - nIdx);
			}
		}

		if (nIdx2 != string::npos)
		{
			if ((nIdx = sLogLine.find_first_of(sDigit, nIdx2)) != string::npos)
			{
				if ((nIdx2 = sLogLine.find_first_not_of(sDigit, nIdx)) != string::npos)
				{
					sLineIdx  = sLogLine.substr(nIdx, nIdx2 - nIdx);
					bValidIdx = true;
				}
			}
		}

		sText += sLogLine;

		if (bValidIdx)
		{
			if ((sscanf_s(sLineIdx.c_str(), "%d", &iLineIdx) == 1))
			{
				// Find in which code string the error occurred.
				size_t nCodeIdx;
				for (nCodeIdx = 0; nCodeIdx < nCodeCnt; ++nCodeIdx)
				{
					if (iLineIdx <= int(vecLastLine[nCodeIdx]))
					{
						break;
					}
				}

				// Line outside all code strings. This should not happen.
				if (nCodeIdx >= nCodeCnt)
				{
					continue;
				}

				// Get code string
				const string& sCode = vecCode[nCodeIdx];

				// If this is the first error return its line index
				if (bIsFirstError)
				{
					// Set return variables with line in file
					iCodeStartLine  = int(vecFileLine[nCodeIdx]);
					iCodeLineOffset = iLineIdx;
				}

				size_t nFirstLine = 1;
				if (nCodeIdx > 0)
				{
					nFirstLine = vecLastLine[nCodeIdx - 1] + 1;
				}

				// Print error line and lines above and below it.
				size_t nLineStartIdx, nLineEndIdx;
				int iPrintLineStart, iPrintLineEnd;

				// iLineIdx is 1-based and vecLineEndIdx has one more entry than lines, since the start index is the first entry.
				iPrintLineStart = (iLineIdx == int(nFirstLine) ? int(nFirstLine) : iLineIdx - 1);
				iPrintLineEnd   = (iLineIdx == vecLineEndIdx.size() - 1 ? iLineIdx : iLineIdx + 1);

				for (int iPrintLineIdx = iPrintLineStart; iPrintLineIdx <= iPrintLineEnd; ++iPrintLineIdx)
				{
					nLineEndIdx   = vecLineEndIdx[iPrintLineIdx];
					nLineStartIdx = vecLineEndIdx[iPrintLineIdx - 1] + (iPrintLineIdx == 1 ? 0 : 1);

					sCodeLine = sCode.substr(nLineStartIdx, nLineEndIdx - nLineStartIdx);
					if (iPrintLineIdx == iLineIdx)
					{
						sText += "\n#! ";
						if (bIsFirstError)
						{
							iCodePosOffset = (int) nLineStartIdx;
						}
					}
					else
					{
						sText += "\n#  ";
					}
					sText += sCodeLine;
				}

				bIsFirstError = false;
			}
		}

		sText += "\n\n";
	}
	while (nStartIdx != string::npos);
}

//////////////////////////////////////////////////////////////////////
// Build Shader Program Function
//
// Parameter:
//	1. Shader Object Reference
//	2. List of strings of vertex shader code
//	3. List of strings of fragment shader code
//	4. (opt) List of strings of geometry shader code
//

bool ShaderBuildFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if ((iVarCount != 3) && (iVarCount != 4))
	{
		int piPar[] = { 3, 4 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a shader variable as first parameter.", iLine, iPos);
		return false;
	}

	TScene& rScene = *mVars(0).GetScenePtr();

	if (!rScene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Given shader is invalid.", iLine, iPos);
		return false;
	}

	COGLShader* pShader = dynamic_cast< COGLShader* >((COGLBaseElement*) rScene);
	if (!pShader)
	{
		rCB.GetErrorList().GeneralError("Given shader is invalid.", iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_VARLIST)
	{
		rCB.GetErrorList().GeneralError("Expect a list of vertex shader code strings as second parameter.", iLine, iPos);
		return false;
	}

	if (mVars(2).BaseType() != PDT_VARLIST)
	{
		rCB.GetErrorList().GeneralError("Expect a list of fragment shader code strings as second parameter.", iLine, iPos);
		return false;
	}

	TVarList& rVSList = *mVars(1).GetVarListPtr();
	TVarList& rFSList = *mVars(2).GetVarListPtr();

	vector<string> vecVSCode, vecFSCode, vecGSCode;
	int iIdx, iCnt;

	iCnt = int(rVSList.Count());
	vecVSCode.resize(iCnt);
	for (iIdx = 0; iIdx < iCnt; ++iIdx)
	{
		if (rVSList(iIdx).BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().GeneralError("Element in vertex shader code list is not a string.", iLine, iPos);
			return false;
		}

		vecVSCode[iIdx] = rVSList(iIdx).GetStringPtr()->Str();
	}

	iCnt = int(rFSList.Count());
	vecFSCode.resize(iCnt);
	for (iIdx = 0; iIdx < iCnt; ++iIdx)
	{
		if (rFSList(iIdx).BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().GeneralError("Element in fragment shader code list is not a string.", iLine, iPos);
			return false;
		}

		vecFSCode[iIdx] = rFSList(iIdx).GetStringPtr()->Str();
	}

	if (iVarCount >= 4)
	{
		if (mVars(3).BaseType() != PDT_VARLIST)
		{
			rCB.GetErrorList().GeneralError("Expect a list of geometry shader code strings as third parameter.", iLine, iPos);
			return false;
		}

		TVarList& rGeoList = *mVars(3).GetVarListPtr();

		iCnt = int(rGeoList.Count());
		vecGSCode.resize(iCnt);
		for (iIdx = 0; iIdx < iCnt; ++iIdx)
		{
			if (rGeoList(iIdx).BaseType() != PDT_STRING)
			{
				rCB.GetErrorList().GeneralError("Element in geometry shader code list is not a string.", iLine, iPos);
				return false;
			}

			vecGSCode[iIdx] = rGeoList(iIdx).GetStringPtr()->Str();
		}
	}

	if (!pShader->Build(vecVSCode, vecFSCode, vecGSCode))
	{
		int iCodeStartLine = -1, iCodeLineOffset = -1, iCodePosOffset = -1;
		string sCodeError;

		if (vecVSCode.size() && !pShader->IsVSOK())
		{
			string sError = "Error compiling vertex shader:\n";
			CreateShaderErrorOutput(sCodeError, iCodeStartLine, iCodeLineOffset, iCodePosOffset, vecVSCode, pShader->GetVSCompileLog());

			sError += sCodeError;
			if ((iCodeStartLine >= 0) && (iCodeLineOffset >= 0) && (iCodePosOffset >= 0))
			{
				rCB.GetErrorList().GeneralError(sError.c_str(), iCodeStartLine, iCodePosOffset, iCodeLineOffset);
			}
			else
			{
				rCB.GetErrorList().GeneralError(sError.c_str(), iLine, iPos);
			}
			return false;
		}

		if (vecFSCode.size() && !pShader->IsFSOK())
		{
			string sError = "Error compiling fragment shader:\n";
			CreateShaderErrorOutput(sCodeError, iCodeStartLine, iCodeLineOffset, iCodePosOffset, vecFSCode, pShader->GetFSCompileLog());

			sError += sCodeError;
			if ((iCodeStartLine >= 0) && (iCodeLineOffset >= 0) && (iCodePosOffset >= 0))
			{
				rCB.GetErrorList().GeneralError(sError.c_str(), iCodeStartLine, iCodePosOffset, iCodeLineOffset);
			}
			else
			{
				rCB.GetErrorList().GeneralError(sError.c_str(), iLine, iPos);
			}
			return false;
		}

		if (iVarCount >= 4)
		{
			if (vecGSCode.size() && !pShader->IsGSOK())
			{
				string sError = "Error compiling geometry shader:\n";
				CreateShaderErrorOutput(sCodeError, iCodeStartLine, iCodeLineOffset, iCodePosOffset, vecGSCode, pShader->GetGSCompileLog());

				sError += sCodeError;
				if ((iCodeStartLine >= 0) && (iCodeLineOffset >= 0) && (iCodePosOffset >= 0))
				{
					rCB.GetErrorList().GeneralError(sError.c_str(), iCodeStartLine, iCodePosOffset, iCodeLineOffset);
				}
				else
				{
					rCB.GetErrorList().GeneralError(sError.c_str(), iLine, iPos);
				}
				return false;
			}
		}

		if (!pShader->IsProgOK())
		{
			string sError = "Error linking shader program:\n";
			sError += pShader->GetLinkLog();
			rCB.GetErrorList().GeneralError(sError.c_str(), iLine, iPos);
			return false;
		}

		rCB.GetErrorList().GeneralError("Unexpected error while building shader.", iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// Helper Functions for Shader

template<class T>
bool GetValueList(CCLUCodeBase& rCB,  vector<T>& vecData, TVarList& rList, unsigned int uOffset)
{
	TCVScalar dVal;
	unsigned int uIdx, uCnt = unsigned int(rList.Count());

	for (uIdx = 0; uIdx < uCnt; ++uIdx)
	{
		if (!rList(uIdx).CastToScalar(dVal, rCB.GetSensitivity()))
		{
			return false;
		}

		vecData[uOffset + uIdx] = (T) dVal;
	}

	return true;
}

//template<>
//bool GetValueList(rCB, vector<bool>& vecData, TVarList &rList, unsigned int uOffset )
//{
//	TCVScalar dVal;
//	unsigned int uIdx, uCnt = rList.Count();
//
//	for ( uIdx = 0; uIdx < uCnt; ++uIdx )
//	{
//		if ( !rList(uIdx).CastToScalar( dVal, rCB.GetSensitivity() ) )
//			return false;
//
//		vecData[ uOffset + uIdx ] = (bool) (dVal != 0.0);
//	}
//
//	return true;
//}

template<class T>
bool GetMVData(CCLUCodeBase& rCB,  vector<T>& vecData, TMultiV& rMV, uint uDataDim, uint uOffset, int iLine, int iPos)
{
	uint uBaseID = rMV.GetBase().BaseID();

	if (uBaseID == ID_E3GA)
	{
		if (uDataDim == 4)
		{
			rCB.GetErrorList().GeneralError("Expect 4D vector.", iLine, iPos);
			return false;
		}

		if (uDataDim == 3)
		{
			vecData[uOffset]     = (T) rMV[E3GA < TCVScalar > ::iE1];
			vecData[uOffset + 1] = (T) rMV[E3GA < TCVScalar > ::iE2];
			vecData[uOffset + 2] = (T) rMV[E3GA < TCVScalar > ::iE3];
		}
		else				// if ( uDataDim == 2 )
		{
			vecData[uOffset]     = (T) rMV[E3GA < TCVScalar > ::iE1];
			vecData[uOffset + 1] = (T) rMV[E3GA < TCVScalar > ::iE2];
		}
	}
	else if (uBaseID == ID_PGA)
	{
		if (uDataDim == 4)
		{
			vecData[uOffset]     = (T) rMV[PGA < TCVScalar > ::iE1];
			vecData[uOffset + 1] = (T) rMV[PGA < TCVScalar > ::iE2];
			vecData[uOffset + 2] = (T) rMV[PGA < TCVScalar > ::iE3];
			vecData[uOffset + 3] = (T) rMV[PGA < TCVScalar > ::iE4];
		}
		else if (uDataDim == 3)
		{
			vecData[uOffset]     = (T) rMV[PGA < TCVScalar > ::iE1];
			vecData[uOffset + 1] = (T) rMV[PGA < TCVScalar > ::iE2];
			vecData[uOffset + 2] = (T) rMV[PGA < TCVScalar > ::iE3];
		}
		else				// if ( uDataDim == 2 )
		{
			vecData[uOffset]     = (T) rMV[PGA < TCVScalar > ::iE1];
			vecData[uOffset + 1] = (T) rMV[PGA < TCVScalar > ::iE2];
		}
	}
	else
	{
		rCB.GetErrorList().GeneralError("Application of given multivector type not supported.", iLine, iPos);
		return false;
	}

	return true;
}

//template<>
//bool GetMVData( vector<bool>& vecData, TMultiV& rMV, uint uDataDim, uint uOffset, int iLine, int iPos )
//{
//	uint uBaseID = rMV.GetBase().BaseID();
//
//	if ( uBaseID == ID_E3GA )
//	{
//		if ( uDataDim == 4 )
//		{
//			rCB.GetErrorList().GeneralError( "Expect 4D vector.", iLine, iPos );
//			return false;
//		}
//
//		if ( uDataDim == 3 )
//		{
//			vecData[uOffset] = (bool) (rMV[ E3GA<TCVScalar>::iE1 ] != 0.0f);
//			vecData[uOffset + 1] = (bool) (rMV[ E3GA<TCVScalar>::iE2 ] != 0.0f);
//			vecData[uOffset + 2] = (bool) (rMV[ E3GA<TCVScalar>::iE3 ] != 0.0f);
//		}
//		else // if ( uDataDim == 2 )
//		{
//			vecData[uOffset] = (bool) (rMV[ E3GA<TCVScalar>::iE1 ] != 0.0f);
//			vecData[uOffset + 1] = (bool) (rMV[ E3GA<TCVScalar>::iE2 ] != 0.0f);
//		}
//	}
//	else if ( uBaseID == ID_PGA )
//	{
//		if ( uDataDim == 4 )
//		{
//			vecData[uOffset] = (bool) (rMV[ PGA<TCVScalar>::iE1 ] != 0.0f);
//			vecData[uOffset + 1] = (bool) (rMV[ PGA<TCVScalar>::iE2 ] != 0.0f);
//			vecData[uOffset + 2] = (bool) (rMV[ PGA<TCVScalar>::iE3 ] != 0.0f);
//			vecData[uOffset + 3] = (bool) (rMV[ PGA<TCVScalar>::iE4 ] != 0.0f);
//		}
//		else if ( uDataDim == 3 )
//		{
//			vecData[uOffset] = (bool) (rMV[ PGA<TCVScalar>::iE1 ] != 0.0f);
//			vecData[uOffset + 1] = (bool) (rMV[ PGA<TCVScalar>::iE2 ] != 0.0f);
//			vecData[uOffset + 2] = (bool) (rMV[ PGA<TCVScalar>::iE3 ] != 0.0f);
//		}
//		else // if ( uDataDim == 2 )
//		{
//			vecData[uOffset] = (bool) (rMV[ PGA<TCVScalar>::iE1 ] != 0.0f);
//			vecData[uOffset + 1] = (bool) (rMV[ PGA<TCVScalar>::iE2 ] != 0.0f);
//		}
//	}
//	else
//	{
//		rCB.GetErrorList().GeneralError( "Application of given multivector type not supported.", iLine, iPos );
//		return false;
//	}
//
//	return true;
//}

template<class T>
bool GetColorData(CCLUCodeBase& rCB,  vector<T>& vecData, TOGLColor& rColor, uint uDataDim, uint uOffset, int iLine, int iPos)
{
	if (uDataDim == 2)
	{
		rCB.GetErrorList().GeneralError("Expect 2D vector.", iLine, iPos);
		return false;
	}

	if (uDataDim == 3)
	{
		vecData[uOffset]     = (T) rColor[0];
		vecData[uOffset + 1] = (T) rColor[1];
		vecData[uOffset + 2] = (T) rColor[2];
	}
	else				// if ( uDataDim == 4 )
	{
		vecData[uOffset]     = (T) rColor[0];
		vecData[uOffset + 1] = (T) rColor[1];
		vecData[uOffset + 2] = (T) rColor[2];
		vecData[uOffset + 3] = (T) rColor[3];
	}

	return true;
}

//template<>
//bool GetColorData(rCB, vector<bool>& vecData, TOGLColor &rColor, uint uDataDim, uint uOffset, int iLine, int iPos )
//{
//	if ( uDataDim == 2 )
//	{
//		rCB.GetErrorList().GeneralError( "Expect 2D vector.", iLine, iPos );
//		return false;
//	}
//
//	if ( uDataDim == 3 )
//	{
//		vecData[uOffset] = (bool) (rColor[ 0 ] != 0.0f);
//		vecData[uOffset + 1] = (bool) (rColor[ 1 ] != 0.0f);
//		vecData[uOffset + 2] = (bool) (rColor[ 2 ] != 0.0f);
//	}
//	else // if ( uDataDim == 4 )
//	{
//		vecData[uOffset] = (bool) (rColor[ 0 ] != 0.0f);
//		vecData[uOffset + 1] = (bool) (rColor[ 1 ] != 0.0f);
//		vecData[uOffset + 2] = (bool) (rColor[ 2 ] != 0.0f);
//		vecData[uOffset + 3] = (bool) (rColor[ 3 ] != 0.0f);
//	}
//
//	return true;
//}

//////////////////////////////////////////////////////////////////////////////////////////////
/// Set Shader Uniform Variable
//
// Parameter:
//	1. Shader Object Reference
//	2. Variable Name
//	3. VarList of Values
//  4. (opt) index in array

bool SetShaderUniformVarFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if ((iVarCount < 3) || (iVarCount > 4))
	{
		int piPar[] = { 3, 4 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a shader variable as first parameter.", iLine, iPos);
		return false;
	}

	TScene& rScene = *mVars(0).GetScenePtr();

	if (!rScene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Given shader is invalid.", iLine, iPos);
		return false;
	}

	COGLShader* pShader = dynamic_cast< COGLShader* >((COGLBaseElement*) rScene);
	if (!pShader)
	{
		rCB.GetErrorList().GeneralError("Given shader is invalid.", iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("Expect the name of a uniform shader variable as second parameter.", iLine, iPos);
		return false;
	}

	string sVarName;
	sVarName = mVars(1).GetStringPtr()->Str();

	if (!pShader->HasUniformVar(sVarName))
	{
		rCB.GetErrorList().GeneralError("Uniform variable does not exist in shader program.", iLine, iPos);
		return false;
	}

	string sVarType;
	unsigned int uVarArrayIdx, uVarArraySize;
	pShader->GetUniformVarType(sVarType, sVarName);
	pShader->GetUniformVarArraySize(uVarArraySize, sVarName);

	TCVCounter iVarIdx = 1;
	if (iVarCount > 3)
	{
		if (!mVars(3).CastToCounter(iVarIdx))
		{
			rCB.GetErrorList().GeneralError("Optional fourth parameters has to give index in variable array.", iLine, iPos);
			return false;
		}

		if (iVarIdx <= 0)
		{
			rCB.GetErrorList().GeneralError("Invalid array index.", iLine, iPos);
			return false;
		}
	}

	uVarArrayIdx = (unsigned int) (iVarIdx - 1);

	if ((sVarType == "float") ||
	    (sVarType == "int") ||
	    (sVarType == "bool"))
	{
		TCVScalar dVal;
		if (mVars(2).CastToScalar(dVal, rCB.GetSensitivity()))
		{
			if (uVarArrayIdx >= uVarArraySize)
			{
				rCB.GetErrorList().GeneralError("Array index is out of bounds.", iLine, iPos);
				return false;
			}

			if (sVarType == "float")
			{
				float fVal = (float) dVal;
				pShader->SetUniformVar<float, 1>(sVarName, &fVal, uVarArrayIdx);
			}
			else if (sVarType == "int")
			{
				int iVal = (int) dVal;
				pShader->SetUniformVar<int, 1>(sVarName, &iVal, uVarArrayIdx);
			}
			else				//if ( sVarType == "bool" )
			{
				uint uVal = (uint) dVal;
				pShader->SetUniformVar<uint, 1>(sVarName, &uVal, uVarArrayIdx);
			}
		}
		else if (mVars(2).BaseType() == PDT_VARLIST)
		{
			TVarList& rList   = *mVars(2).GetVarListPtr();
			unsigned int uCnt = unsigned int(rList.Count());

			if (uCnt == 0)
			{
				return true;
			}

			if (uVarArrayIdx + uCnt > uVarArraySize)
			{
				rCB.GetErrorList().GeneralError("Too many values given for array.", iLine, iPos);
				return false;
			}

			if (sVarType == "float")
			{
				vector<float> vecData;
				vecData.resize(uCnt);

				if (!GetValueList(rCB, vecData, rList, 0))
				{
					rCB.GetErrorList().GeneralError("Element in data array is not a scalar.", iLine, iPos);
					return false;
				}

				pShader->SetUniformVar<float, 1>(sVarName, &vecData[0], uVarArrayIdx, uCnt);
			}
			else if (sVarType == "int")
			{
				vector<int> vecData;
				vecData.resize(uCnt);

				if (!GetValueList(rCB, vecData, rList, 0))
				{
					rCB.GetErrorList().GeneralError("Element in data array is not a scalar.", iLine, iPos);
					return false;
				}

				pShader->SetUniformVar<int, 1>(sVarName, &vecData[0], uVarArrayIdx, uCnt);
			}
			else if (sVarType == "bool")
			{
				vector<uint> vecData;
				vecData.resize(uCnt);

				if (!GetValueList(rCB, vecData, rList, 0))
				{
					rCB.GetErrorList().GeneralError("Element in data array is not a scalar.", iLine, iPos);
					return false;
				}

				pShader->SetUniformVar<uint, 1>(sVarName, &vecData[0], uVarArrayIdx, uCnt);
			}
		}
		else
		{
			string sText;
			sText  = "Expect data compatible with uniform variable of type '";
			sText += sVarType;
			sText += "'.";

			rCB.GetErrorList().GeneralError(sText.c_str(), iLine, iPos);
			return false;
		}
	}
	else if (sVarType.find("vec") != string::npos)
	{
		enum EDataType { FLOAT, INT, BOOL } eDataType;
		unsigned int uDataDim;
		unsigned int uLen  = unsigned int(sVarType.length());
		const char* pcType = sVarType.c_str();

		if (pcType[uLen - 1] == '2')
		{
			uDataDim = 2;
		}
		else if (pcType[uLen - 1] == '3')
		{
			uDataDim = 3;
		}
		else
		{
			uDataDim = 4;
		}

		if (pcType[0] == 'v')
		{
			eDataType = FLOAT;
		}
		else if (pcType[0] == 'i')
		{
			eDataType = INT;
		}
		else
		{
			eDataType = BOOL;
		}

		if (mVars(2).BaseType() == PDT_VARLIST)
		{
			TVarList& rList = *mVars(2).GetVarListPtr();
			unsigned int uIdx, uCnt = unsigned int(rList.Count());

			if (uCnt == 0)
			{
				return true;
			}

			if ((rList(0).BaseType() == PDT_SCALAR) ||
			    (rList(0).BaseType() == PDT_COUNTER))
			{
				// List of scalars sets elements in vecx variable

				if (uVarArrayIdx >= uVarArraySize)
				{
					rCB.GetErrorList().GeneralError("Array index out of bound.", iLine, iPos);
					return false;
				}

				if (uCnt != uDataDim)
				{
					char pcText[1024];
					sprintf_s(pcText, "Expect list of %d scalars.", uCnt);
					rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
					return false;
				}

				if (eDataType == FLOAT)
				{
					vector<float> vecData;
					vecData.resize(uDataDim);

					if (!GetValueList(rCB, vecData, rList, 0))
					{
						rCB.GetErrorList().GeneralError("Element in data array is not a scalar.", iLine, iPos);
						return false;
					}

					if (uDataDim == 2)
					{
						pShader->SetUniformVar<float, 2>(sVarName, &vecData[0], uVarArrayIdx, 1);
					}
					else if (uDataDim == 3)
					{
						pShader->SetUniformVar<float, 3>(sVarName, &vecData[0], uVarArrayIdx, 1);
					}
					else
					{
						pShader->SetUniformVar<float, 4>(sVarName, &vecData[0], uVarArrayIdx, 1);
					}
				}
				else if (eDataType == INT)
				{
					vector<int> vecData;
					vecData.resize(uDataDim);

					if (!GetValueList(rCB, vecData, rList, 0))
					{
						rCB.GetErrorList().GeneralError("Element in data array is not a scalar.", iLine, iPos);
						return false;
					}

					if (uDataDim == 2)
					{
						pShader->SetUniformVar<int, 2>(sVarName, &vecData[0], uVarArrayIdx, 1);
					}
					else if (uDataDim == 3)
					{
						pShader->SetUniformVar<int, 3>(sVarName, &vecData[0], uVarArrayIdx, 1);
					}
					else
					{
						pShader->SetUniformVar<int, 4>(sVarName, &vecData[0], uVarArrayIdx, 1);
					}
				}
				else				//if ( eDataType == BOOL )
				{
					vector<uint> vecData;
					vecData.resize(uDataDim);

					if (!GetValueList(rCB, vecData, rList, 0))
					{
						rCB.GetErrorList().GeneralError("Element in data array is not a scalar.", iLine, iPos);
						return false;
					}

					if (uDataDim == 2)
					{
						pShader->SetUniformVar<uint, 2>(sVarName, &vecData[0], uVarArrayIdx, 1);
					}
					else if (uDataDim == 3)
					{
						pShader->SetUniformVar<uint, 3>(sVarName, &vecData[0], uVarArrayIdx, 1);
					}
					else
					{
						pShader->SetUniformVar<uint, 4>(sVarName, &vecData[0], uVarArrayIdx, 1);
					}
				}
			}
			else if (rList(0).BaseType() == PDT_VARLIST)
			{
				if (uVarArrayIdx + uCnt > uVarArraySize)
				{
					rCB.GetErrorList().GeneralError("Too many values given for array.", iLine, iPos);
					return false;
				}

				if (eDataType == FLOAT)
				{
					unsigned int uOffset;
					vector<float> vecData;
					vecData.resize(uCnt * uDataDim);

					for (uIdx = 0, uOffset = 0; uIdx < uCnt; ++uIdx, uOffset += uDataDim)
					{
						if (rList(uIdx).BaseType() != PDT_VARLIST)
						{
							char pcText[1024];
							sprintf_s(pcText, "Element %d in value list is not a list of scalar values.", uIdx + 1);
							rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
							return false;
						}

						TVarList& rSubList   = *rList(uIdx).GetVarListPtr();
						unsigned int uSubCnt = unsigned int(rSubList.Count());

						if (uSubCnt != uDataDim)
						{
							char pcText[1024];
							sprintf_s(pcText, "Expect list of %d scalars in sublist %d.", uDataDim, uIdx + 1);
							rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
							return false;
						}

						if (!GetValueList(rCB, vecData, rSubList, uOffset))
						{
							char pcText[1024];
							sprintf_s(pcText, "Element in sublist %d is not a scalar.", uIdx + 1);
							rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
							rCB.GetErrorList().GeneralError("Element in data array is not a scalar.", iLine, iPos);
							return false;
						}
					}

					if (uDataDim == 2)
					{
						pShader->SetUniformVar<float, 2>(sVarName, &vecData[0], uVarArrayIdx, uCnt);
					}
					else if (uDataDim == 3)
					{
						pShader->SetUniformVar<float, 3>(sVarName, &vecData[0], uVarArrayIdx, uCnt);
					}
					else
					{
						pShader->SetUniformVar<float, 4>(sVarName, &vecData[0], uVarArrayIdx, uCnt);
					}
				}
				else if (eDataType == INT)
				{
					unsigned int uOffset;
					vector<int> vecData;
					vecData.resize(uCnt * uDataDim);

					for (uIdx = 0, uOffset = 0; uIdx < uCnt; ++uIdx, uOffset += uDataDim)
					{
						if (rList(uIdx).BaseType() != PDT_VARLIST)
						{
							char pcText[1024];
							sprintf_s(pcText, "Element %d in value list is not a list of scalar values.", uIdx + 1);
							rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
							return false;
						}

						TVarList& rSubList   = *rList(uIdx).GetVarListPtr();
						unsigned int uSubCnt = unsigned int(rSubList.Count());

						if (uSubCnt != uDataDim)
						{
							char pcText[1024];
							sprintf_s(pcText, "Expect list of %d scalars in sublist %d.", uDataDim, uIdx + 1);
							rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
							return false;
						}

						if (!GetValueList(rCB, vecData, rSubList, uOffset))
						{
							char pcText[1024];
							sprintf_s(pcText, "Element in sublist %d is not a scalar.", uIdx + 1);
							rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
							rCB.GetErrorList().GeneralError("Element in data array is not a scalar.", iLine, iPos);
							return false;
						}
					}

					if (uDataDim == 2)
					{
						pShader->SetUniformVar<int, 2>(sVarName, &vecData[0], uVarArrayIdx, uCnt);
					}
					else if (uDataDim == 3)
					{
						pShader->SetUniformVar<int, 3>(sVarName, &vecData[0], uVarArrayIdx, uCnt);
					}
					else
					{
						pShader->SetUniformVar<int, 4>(sVarName, &vecData[0], uVarArrayIdx, uCnt);
					}
				}
				else				//if ( eDataType == BOOL )
				{
					unsigned int uOffset;
					vector<uint> vecData;
					vecData.resize(uCnt * uDataDim);

					for (uIdx = 0, uOffset = 0; uIdx < uCnt; ++uIdx, uOffset += uDataDim)
					{
						if (rList(uIdx).BaseType() != PDT_VARLIST)
						{
							char pcText[1024];
							sprintf_s(pcText, "Element %d in value list is not a list of scalar values.", uIdx + 1);
							rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
							return false;
						}

						TVarList& rSubList   = *rList(uIdx).GetVarListPtr();
						unsigned int uSubCnt = unsigned int(rSubList.Count());

						if (uSubCnt != uDataDim)
						{
							char pcText[1024];
							sprintf_s(pcText, "Expect list of %d scalars in sublist %d.", uDataDim, uIdx + 1);
							rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
							return false;
						}

						if (!GetValueList(rCB, vecData, rSubList, uOffset))
						{
							char pcText[1024];
							sprintf_s(pcText, "Element in sublist %d is not a scalar.", uIdx + 1);
							rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
							rCB.GetErrorList().GeneralError("Element in data array is not a scalar.", iLine, iPos);
							return false;
						}
					}

					if (uDataDim == 2)
					{
						pShader->SetUniformVar<uint, 2>(sVarName, &vecData[0], uVarArrayIdx, uCnt);
					}
					else if (uDataDim == 3)
					{
						pShader->SetUniformVar<uint, 3>(sVarName, &vecData[0], uVarArrayIdx, uCnt);
					}
					else
					{
						pShader->SetUniformVar<uint, 4>(sVarName, &vecData[0], uVarArrayIdx, uCnt);
					}
				}
			}
			else if (rList(0).BaseType() == PDT_MULTIV)
			{
				if (uVarArrayIdx + uCnt > uVarArraySize)
				{
					rCB.GetErrorList().GeneralError("Too many values given for array.", iLine, iPos);
					return false;
				}

				if (eDataType == FLOAT)
				{
					uint uOffset;
					vector<float> vecData;
					vecData.resize(uCnt * uDataDim);

					for (uIdx = 0, uOffset = 0; uIdx < uCnt; ++uIdx, uOffset += uDataDim)
					{
						if (rList(uIdx).BaseType() != PDT_MULTIV)
						{
							rCB.GetErrorList().GeneralError("Element in list is not a multivector.", iLine, iPos);
							return false;
						}

						TMultiV& rMV = *rList(uIdx).GetMultiVPtr();

						if (!GetMVData(rCB, vecData, rMV, uDataDim, uOffset, iLine, iPos))
						{
							return false;
						}
					}

					if (uDataDim == 2)
					{
						pShader->SetUniformVar<float, 2>(sVarName, &vecData[0], uVarArrayIdx, uCnt);
					}
					else if (uDataDim == 3)
					{
						pShader->SetUniformVar<float, 3>(sVarName, &vecData[0], uVarArrayIdx, uCnt);
					}
					else if (uDataDim == 4)
					{
						pShader->SetUniformVar<float, 4>(sVarName, &vecData[0], uVarArrayIdx, uCnt);
					}
				}
				else if (eDataType == INT)
				{
					uint uOffset;
					vector<int> vecData;
					vecData.resize(uCnt * uDataDim);

					for (uIdx = 0, uOffset = 0; uIdx < uCnt; ++uIdx, uOffset += uDataDim)
					{
						if (rList(uIdx).BaseType() != PDT_MULTIV)
						{
							rCB.GetErrorList().GeneralError("Element in list is not a multivector.", iLine, iPos);
							return false;
						}

						TMultiV& rMV = *rList(uIdx).GetMultiVPtr();

						if (!GetMVData(rCB, vecData, rMV, uDataDim, uOffset, iLine, iPos))
						{
							return false;
						}
					}

					if (uDataDim == 2)
					{
						pShader->SetUniformVar<int, 2>(sVarName, &vecData[0], uVarArrayIdx, uCnt);
					}
					else if (uDataDim == 3)
					{
						pShader->SetUniformVar<int, 3>(sVarName, &vecData[0], uVarArrayIdx, uCnt);
					}
					else if (uDataDim == 4)
					{
						pShader->SetUniformVar<int, 4>(sVarName, &vecData[0], uVarArrayIdx, uCnt);
					}
				}
				if (eDataType == BOOL)
				{
					uint uOffset;
					vector<uint> vecData;
					vecData.resize(uCnt * uDataDim);

					for (uIdx = 0, uOffset = 0; uIdx < uCnt; ++uIdx, uOffset += uDataDim)
					{
						if (rList(uIdx).BaseType() != PDT_MULTIV)
						{
							rCB.GetErrorList().GeneralError("Element in list is not a multivector.", iLine, iPos);
							return false;
						}

						TMultiV& rMV = *rList(uIdx).GetMultiVPtr();

						if (!GetMVData(rCB, vecData, rMV, uDataDim, uOffset, iLine, iPos))
						{
							return false;
						}
					}

					if (uDataDim == 2)
					{
						pShader->SetUniformVar<uint, 2>(sVarName, &vecData[0], uVarArrayIdx, uCnt);
					}
					else if (uDataDim == 3)
					{
						pShader->SetUniformVar<uint, 3>(sVarName, &vecData[0], uVarArrayIdx, uCnt);
					}
					else if (uDataDim == 4)
					{
						pShader->SetUniformVar<uint, 4>(sVarName, &vecData[0], uVarArrayIdx, uCnt);
					}
				}
			}
			else if (rList(0).BaseType() == PDT_COLOR)
			{
				if (uVarArrayIdx + uCnt > uVarArraySize)
				{
					rCB.GetErrorList().GeneralError("Too many values given for array.", iLine, iPos);
					return false;
				}

				if (eDataType == FLOAT)
				{
					uint uOffset;
					vector<float> vecData;
					vecData.resize(uCnt * uDataDim);

					for (uIdx = 0, uOffset = 0; uIdx < uCnt; ++uIdx, uOffset += uDataDim)
					{
						if (rList(uIdx).BaseType() != PDT_COLOR)
						{
							rCB.GetErrorList().GeneralError("Element in list is not a color.", iLine, iPos);
							return false;
						}

						TOGLColor& rColor = *rList(uIdx).GetOGLColorPtr();

						if (!GetColorData(rCB, vecData, rColor, uDataDim, uOffset, iLine, iPos))
						{
							return false;
						}
					}

					if (uDataDim == 2)
					{
						pShader->SetUniformVar<float, 2>(sVarName, &vecData[0], uVarArrayIdx, uCnt);
					}
					else if (uDataDim == 3)
					{
						pShader->SetUniformVar<float, 3>(sVarName, &vecData[0], uVarArrayIdx, uCnt);
					}
					else if (uDataDim == 4)
					{
						pShader->SetUniformVar<float, 4>(sVarName, &vecData[0], uVarArrayIdx, uCnt);
					}
				}
				else if (eDataType == INT)
				{
					uint uOffset;
					vector<int> vecData;
					vecData.resize(uCnt * uDataDim);

					for (uIdx = 0, uOffset = 0; uIdx < uCnt; ++uIdx, uOffset += uDataDim)
					{
						if (rList(uIdx).BaseType() != PDT_COLOR)
						{
							rCB.GetErrorList().GeneralError("Element in list is not a color.", iLine, iPos);
							return false;
						}

						TOGLColor& rColor = *rList(uIdx).GetOGLColorPtr();

						if (!GetColorData(rCB, vecData, rColor, uDataDim, uOffset, iLine, iPos))
						{
							return false;
						}
					}

					if (uDataDim == 2)
					{
						pShader->SetUniformVar<int, 2>(sVarName, &vecData[0], uVarArrayIdx, uCnt);
					}
					else if (uDataDim == 3)
					{
						pShader->SetUniformVar<int, 3>(sVarName, &vecData[0], uVarArrayIdx, uCnt);
					}
					else if (uDataDim == 4)
					{
						pShader->SetUniformVar<int, 4>(sVarName, &vecData[0], uVarArrayIdx, uCnt);
					}
				}
				if (eDataType == BOOL)
				{
					uint uOffset;
					vector<uint> vecData;
					vecData.resize(uCnt * uDataDim);

					for (uIdx = 0, uOffset = 0; uIdx < uCnt; ++uIdx, uOffset += uDataDim)
					{
						if (rList(uIdx).BaseType() != PDT_COLOR)
						{
							rCB.GetErrorList().GeneralError("Element in list is not a color.", iLine, iPos);
							return false;
						}

						TOGLColor& rColor = *rList(uIdx).GetOGLColorPtr();

						if (!GetColorData(rCB, vecData, rColor, uDataDim, uOffset, iLine, iPos))
						{
							return false;
						}
					}

					if (uDataDim == 2)
					{
						pShader->SetUniformVar<uint, 2>(sVarName, &vecData[0], uVarArrayIdx, uCnt);
					}
					else if (uDataDim == 3)
					{
						pShader->SetUniformVar<uint, 3>(sVarName, &vecData[0], uVarArrayIdx, uCnt);
					}
					else if (uDataDim == 4)
					{
						pShader->SetUniformVar<uint, 4>(sVarName, &vecData[0], uVarArrayIdx, uCnt);
					}
				}
			}
			else
			{
				string sText;
				sText  = "Expect data compatible with uniform variable of type '";
				sText += sVarType;
				sText += "'.";

				rCB.GetErrorList().GeneralError(sText.c_str(), iLine, iPos);
				return false;
			}
		}
		else if (mVars(2).BaseType() == PDT_MULTIV)
		{
			if (uVarArrayIdx >= uVarArraySize)
			{
				rCB.GetErrorList().GeneralError("Array index out of bound.", iLine, iPos);
				return false;
			}

			TMultiV& rMV = *mVars(2).GetMultiVPtr();
			uint uBaseID = rMV.GetBase().BaseID();
			if (uBaseID == ID_E3GA)
			{
				if (uDataDim == 4)
				{
					rCB.GetErrorList().GeneralError("Expect 4D vector.", iLine, iPos);
					return false;
				}

				if (eDataType == FLOAT)
				{
					if (uDataDim == 3)
					{
						vector<float> vecData;
						vecData.resize(3);
						vecData[0] = (float) rMV[E3GA < TCVScalar > ::iE1];
						vecData[1] = (float) rMV[E3GA < TCVScalar > ::iE2];
						vecData[2] = (float) rMV[E3GA < TCVScalar > ::iE3];

						pShader->SetUniformVar<float, 3>(sVarName, &vecData[0], uVarArrayIdx, 1);
					}
					else				// if ( uDataDim == 2 )
					{
						vector<float> vecData;
						vecData.resize(2);
						vecData[0] = (float) rMV[E3GA < TCVScalar > ::iE1];
						vecData[1] = (float) rMV[E3GA < TCVScalar > ::iE2];

						pShader->SetUniformVar<float, 2>(sVarName, &vecData[0], uVarArrayIdx, 1);
					}
				}
				else if (eDataType == INT)
				{
					if (uDataDim == 3)
					{
						vector<int> vecData;
						vecData.resize(3);
						vecData[0] = (int) rMV[E3GA < TCVScalar > ::iE1];
						vecData[1] = (int) rMV[E3GA < TCVScalar > ::iE2];
						vecData[2] = (int) rMV[E3GA < TCVScalar > ::iE3];

						pShader->SetUniformVar<int, 3>(sVarName, &vecData[0], uVarArrayIdx, 1);
					}
					else				// if ( uDataDim == 2 )
					{
						vector<int> vecData;
						vecData.resize(2);
						vecData[0] = (int) rMV[E3GA < TCVScalar > ::iE1];
						vecData[1] = (int) rMV[E3GA < TCVScalar > ::iE2];

						pShader->SetUniformVar<int, 2>(sVarName, &vecData[0], uVarArrayIdx, 1);
					}
				}
				else				// if ( eDataType == BOOL )
				{
					if (uDataDim == 3)
					{
						vector<uint> vecData;
						vecData.resize(3);
						vecData[0] = (uint) rMV[E3GA < TCVScalar > ::iE1];
						vecData[1] = (uint) rMV[E3GA < TCVScalar > ::iE2];
						vecData[2] = (uint) rMV[E3GA < TCVScalar > ::iE3];

						pShader->SetUniformVar<uint, 3>(sVarName, &vecData[0], uVarArrayIdx, 1);
					}
					else				// if ( uDataDim == 2 )
					{
						vector<uint> vecData;
						vecData.resize(2);
						vecData[0] = (uint) rMV[E3GA < TCVScalar > ::iE1];
						vecData[1] = (uint) rMV[E3GA < TCVScalar > ::iE2];

						pShader->SetUniformVar<uint, 2>(sVarName, &vecData[0], uVarArrayIdx, 1);
					}
				}
			}
			else if (uBaseID == ID_PGA)
			{
				if (eDataType == FLOAT)
				{
					if (uDataDim == 4)
					{
						vector<float> vecData;
						vecData.resize(4);
						vecData[0] = (float) rMV[PGA < TCVScalar > ::iE1];
						vecData[1] = (float) rMV[PGA < TCVScalar > ::iE2];
						vecData[2] = (float) rMV[PGA < TCVScalar > ::iE3];
						vecData[3] = (float) rMV[PGA < TCVScalar > ::iE4];

						pShader->SetUniformVar<float, 4>(sVarName, &vecData[0], uVarArrayIdx, 1);
					}
					else if (uDataDim == 3)
					{
						vector<float> vecData;
						vecData.resize(3);
						vecData[0] = (float) rMV[PGA < TCVScalar > ::iE1];
						vecData[1] = (float) rMV[PGA < TCVScalar > ::iE2];
						vecData[2] = (float) rMV[PGA < TCVScalar > ::iE3];

						pShader->SetUniformVar<float, 3>(sVarName, &vecData[0], uVarArrayIdx, 1);
					}
					else				// if ( uDataDim == 2 )
					{
						vector<float> vecData;
						vecData.resize(2);
						vecData[0] = (float) rMV[PGA < TCVScalar > ::iE1];
						vecData[1] = (float) rMV[PGA < TCVScalar > ::iE2];

						pShader->SetUniformVar<float, 2>(sVarName, &vecData[0], uVarArrayIdx, 1);
					}
				}
				else if (eDataType == INT)
				{
					if (uDataDim == 4)
					{
						vector<int> vecData;
						vecData.resize(4);
						vecData[0] = (int) rMV[PGA < TCVScalar > ::iE1];
						vecData[1] = (int) rMV[PGA < TCVScalar > ::iE2];
						vecData[2] = (int) rMV[PGA < TCVScalar > ::iE3];
						vecData[3] = (int) rMV[PGA < TCVScalar > ::iE4];

						pShader->SetUniformVar<int, 4>(sVarName, &vecData[0], uVarArrayIdx, 1);
					}
					else if (uDataDim == 3)
					{
						vector<int> vecData;
						vecData.resize(3);
						vecData[0] = (int) rMV[PGA < TCVScalar > ::iE1];
						vecData[1] = (int) rMV[PGA < TCVScalar > ::iE2];
						vecData[2] = (int) rMV[PGA < TCVScalar > ::iE3];

						pShader->SetUniformVar<int, 3>(sVarName, &vecData[0], uVarArrayIdx, 1);
					}
					else				// if ( uDataDim == 2 )
					{
						vector<int> vecData;
						vecData.resize(2);
						vecData[0] = (int) rMV[PGA < TCVScalar > ::iE1];
						vecData[1] = (int) rMV[PGA < TCVScalar > ::iE2];

						pShader->SetUniformVar<int, 2>(sVarName, &vecData[0], uVarArrayIdx, 1);
					}
				}
				else				// if ( eDataType == BOOL )
				{
					if (uDataDim == 4)
					{
						vector<uint> vecData;
						vecData.resize(4);
						vecData[0] = (uint) rMV[PGA < TCVScalar > ::iE1];
						vecData[1] = (uint) rMV[PGA < TCVScalar > ::iE2];
						vecData[2] = (uint) rMV[PGA < TCVScalar > ::iE3];
						vecData[3] = (uint) rMV[PGA < TCVScalar > ::iE4];

						pShader->SetUniformVar<uint, 4>(sVarName, &vecData[0], uVarArrayIdx, 1);
					}
					else if (uDataDim == 3)
					{
						vector<uint> vecData;
						vecData.resize(3);
						vecData[0] = (uint) rMV[PGA < TCVScalar > ::iE1];
						vecData[1] = (uint) rMV[PGA < TCVScalar > ::iE2];
						vecData[2] = (uint) rMV[PGA < TCVScalar > ::iE3];

						pShader->SetUniformVar<uint, 3>(sVarName, &vecData[0], uVarArrayIdx, 1);
					}
					else				// if ( uDataDim == 2 )
					{
						vector<uint> vecData;
						vecData.resize(2);
						vecData[0] = (uint) rMV[PGA < TCVScalar > ::iE1];
						vecData[1] = (uint) rMV[PGA < TCVScalar > ::iE2];

						pShader->SetUniformVar<uint, 2>(sVarName, &vecData[0], uVarArrayIdx, 1);
					}
				}
			}
			else
			{
				rCB.GetErrorList().GeneralError("Application of given multivector type not supported.", iLine, iPos);
				return false;
			}
		}
		else if (mVars(2).BaseType() == PDT_COLOR)
		{
			if (uVarArrayIdx >= uVarArraySize)
			{
				rCB.GetErrorList().GeneralError("Array index out of bound.", iLine, iPos);
				return false;
			}

			TOGLColor rColor = *mVars(2).GetOGLColorPtr();

			if (uDataDim == 2)
			{
				rCB.GetErrorList().GeneralError("Expect 2D vector.", iLine, iPos);
				return false;
			}

			if (eDataType == FLOAT)
			{
				if (uDataDim == 3)
				{
					vector<float> vecData;
					vecData.resize(3);
					vecData[0] = (float) rColor[0];
					vecData[1] = (float) rColor[1];
					vecData[2] = (float) rColor[2];

					pShader->SetUniformVar<float, 3>(sVarName, &vecData[0], uVarArrayIdx, 1);
				}
				else				// if ( uDataDim == 4 )
				{
					vector<float> vecData;
					vecData.resize(4);
					vecData[0] = (float) rColor[0];
					vecData[1] = (float) rColor[1];
					vecData[2] = (float) rColor[2];
					vecData[3] = (float) rColor[3];

					pShader->SetUniformVar<float, 4>(sVarName, &vecData[0], uVarArrayIdx, 1);
				}
			}
			else if (eDataType == INT)
			{
				if (uDataDim == 3)
				{
					vector<int> vecData;
					vecData.resize(3);
					vecData[0] = (int) rColor[0];
					vecData[1] = (int) rColor[1];
					vecData[2] = (int) rColor[2];

					pShader->SetUniformVar<int, 3>(sVarName, &vecData[0], uVarArrayIdx, 1);
				}
				else				// if ( uDataDim == 4 )
				{
					vector<int> vecData;
					vecData.resize(4);
					vecData[0] = (int) rColor[0];
					vecData[1] = (int) rColor[1];
					vecData[2] = (int) rColor[2];
					vecData[3] = (int) rColor[3];

					pShader->SetUniformVar<int, 4>(sVarName, &vecData[0], uVarArrayIdx, 1);
				}
			}
			else				//if ( eDataType == BOOL )
			{
				if (uDataDim == 3)
				{
					vector<uint> vecData;
					vecData.resize(3);
					vecData[0] = (uint) rColor[0];
					vecData[1] = (uint) rColor[1];
					vecData[2] = (uint) rColor[2];

					pShader->SetUniformVar<uint, 3>(sVarName, &vecData[0], uVarArrayIdx, 1);
				}
				else				// if ( uDataDim == 4 )
				{
					vector<uint> vecData;
					vecData.resize(4);
					vecData[0] = (uint) rColor[0];
					vecData[1] = (uint) rColor[1];
					vecData[2] = (uint) rColor[2];
					vecData[3] = (uint) rColor[3];

					pShader->SetUniformVar<uint, 4>(sVarName, &vecData[0], uVarArrayIdx, 1);
				}
			}
		}
		else
		{
			string sText;
			sText  = "Expect data compatible with uniform variable of type '";
			sText += sVarType;
			sText += "'.";

			rCB.GetErrorList().GeneralError(sText.c_str(), iLine, iPos);
			return false;
		}
	}
	else if (sVarType.find("mat") != string::npos)
	{
		if (mVars(2).BaseType() == PDT_MATRIX)
		{
			string sNo;
			uint uMatType, uMatSize;
			uint uColCnt, uRowCnt;
			TMatrix& rMat = *mVars(2).GetMatrixPtr();
			uColCnt = rMat.Cols();
			uRowCnt = rMat.Rows();

			sNo      = sVarType.substr(sVarType.length() - 1, 1);
			uMatType = atoi(sNo.c_str());
			if ((uColCnt != uMatType) || (uRowCnt != uMatType))
			{
				rCB.GetErrorList().GeneralError("Given matrix is of invalid size.", iLine, iPos);
				return false;
			}

			uMatSize = uMatType * uMatType;

			TCVScalar* pdData = rMat.Data();
			vector<float> vecData;
			vecData.resize(uMatSize);

			for (uint i = 0; i < uMatSize; ++i)
			{
				vecData[i] = (float) pdData[i];
			}

			if (uMatType == 2)
			{
				pShader->SetUniformVarMatrix<2>(sVarName, &vecData[0], uVarArrayIdx, 1);
			}
			else if (uMatType == 3)
			{
				pShader->SetUniformVarMatrix<3>(sVarName, &vecData[0], uVarArrayIdx, 1);
			}
			else if (uMatType == 4)
			{
				pShader->SetUniformVarMatrix<4>(sVarName, &vecData[0], uVarArrayIdx, 1);
			}
		}
		else if (mVars(2).BaseType() == PDT_VARLIST)
		{
			TVarList& rList = *mVars(2).GetVarListPtr();
			unsigned int uIdx, uCnt = unsigned int(rList.Count());

			if (uCnt == 0)
			{
				return true;
			}

			if (uVarArrayIdx + uCnt > uVarArraySize)
			{
				rCB.GetErrorList().GeneralError("Too many values given for array.", iLine, iPos);
				return false;
			}

			string sNo;
			uint uMatType, uMatSize, uOffset;
			uint uColCnt, uRowCnt;
			vector<float> vecData;

			sNo      = sVarType.substr(sVarType.length() - 1, 1);
			uMatType = atoi(sNo.c_str());
			uMatSize = uMatType * uMatType;
			vecData.resize(uCnt * uMatSize);

			for (uIdx = 0, uOffset = 0; uIdx < uCnt; ++uIdx, uOffset += uMatSize)
			{
				if (rList(uIdx).BaseType() == PDT_MATRIX)
				{
					TMatrix& rMat = *rList(uIdx).GetMatrixPtr();
					uColCnt = rMat.Cols();
					uRowCnt = rMat.Rows();

					if ((uColCnt != uMatType) || (uRowCnt != uMatType))
					{
						rCB.GetErrorList().GeneralError("Given matrix is of invalid size.", iLine, iPos);
						return false;
					}

					TCVScalar* pdData = rMat.Data();

					for (uint i = 0; i < uMatSize; ++i)
					{
						vecData[uOffset + i] = (float) pdData[i];
					}
				}
				else
				{
					rCB.GetErrorList().GeneralError("Element in given list of invalid type.", iLine, iPos);
					return false;
				}
			}

			if (uMatType == 2)
			{
				pShader->SetUniformVarMatrix<2>(sVarName, &vecData[0], uVarArrayIdx, uCnt);
			}
			else if (uMatType == 3)
			{
				pShader->SetUniformVarMatrix<3>(sVarName, &vecData[0], uVarArrayIdx, uCnt);
			}
			else if (uMatType == 4)
			{
				pShader->SetUniformVarMatrix<4>(sVarName, &vecData[0], uVarArrayIdx, uCnt);
			}
		}
		else
		{
			string sText;
			sText  = "Expect data compatible with uniform variable of type '";
			sText += sVarType;
			sText += "'.";

			rCB.GetErrorList().GeneralError(sText.c_str(), iLine, iPos);
			return false;
		}
	}
	else if (sVarType.find("sampler") != string::npos)
	{
		if (mVars(2).BaseType() == PDT_SCENE)
		{
			if (uVarArrayIdx >= uVarArraySize)
			{
				rCB.GetErrorList().GeneralError("Array index is out of bounds.", iLine, iPos);
				return false;
			}

			TScene& rScene = *mVars(2).GetScenePtr();

			if (!rScene.IsValid())
			{
				rCB.GetErrorList().GeneralError("Given texture variable is invalid.", iLine, iPos);
				return false;
			}

			COGLTexture* pTex = dynamic_cast< COGLTexture* >((COGLBaseElement*) rScene);
			if (!pTex)
			{
				rCB.GetErrorList().GeneralError("Given texture variable is invalid.", iLine, iPos);
				return false;
			}

			uint uTexID = pTex->GetTextureUnit();

			pShader->SetUniformVarSampler(sVarName, &uTexID, uVarArrayIdx, 1);
		}
		else if (mVars(2).BaseType() == PDT_VARLIST)
		{
			TVarList& rList = *mVars(2).GetVarListPtr();
			unsigned int uIdx, uCnt = unsigned int(rList.Count());

			if (uCnt == 0)
			{
				return true;
			}

			if (uVarArrayIdx + uCnt > uVarArraySize)
			{
				rCB.GetErrorList().GeneralError("Too many values given for array.", iLine, iPos);
				return false;
			}

			vector<uint> vecTexID;
			vecTexID.resize(uCnt);

			for (uIdx = 0; uIdx < uCnt; ++uIdx)
			{
				if (rList(uIdx).BaseType() != PDT_SCENE)
				{
					rCB.GetErrorList().GeneralError("Element in list is not a texture variable.", iLine, iPos);
					return false;
				}

				TScene& rScene = *rList(uIdx).GetScenePtr();

				if (!rScene.IsValid())
				{
					rCB.GetErrorList().GeneralError("Given texture variable is invalid.", iLine, iPos);
					return false;
				}

				COGLTexture* pTex = dynamic_cast< COGLTexture* >((COGLBaseElement*) rScene);
				if (!pTex)
				{
					rCB.GetErrorList().GeneralError("Given texture variable is invalid.", iLine, iPos);
					return false;
				}

				vecTexID[uIdx] = pTex->GetTextureUnit();
			}

			pShader->SetUniformVarSampler(sVarName, &vecTexID[0], uVarArrayIdx, uCnt);
		}
		else
		{
			string sText;
			sText  = "Expect data compatible with uniform variable of type '";
			sText += sVarType;
			sText += "'.";

			rCB.GetErrorList().GeneralError(sText.c_str(), iLine, iPos);
			return false;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Create Anim Shader Object Function

bool AnimShaderFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount != 3)
	{
		int piPar[] = { 3 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a shader variable as first parameter.", iLine, iPos);
		return false;
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Shader is not valid.", iLine, iPos);
		return false;
	}

	COGLShader* pShader = dynamic_cast< COGLShader* >((COGLBaseElement*) Scene);
	if (!pShader)
	{
		rCB.GetErrorList().GeneralError("Shader is not valid.", iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("Expect as second parameter name of uniform time variable of shader.", iLine, iPos);
		return false;
	}

	if (mVars(2).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("Expect as third parameter name of uniform time step variable of shader.", iLine, iPos);
		return false;
	}

	TString& sTime     = *mVars(1).GetStringPtr();
	TString& sTimeStep = *mVars(2).GetStringPtr();

	COGLAnimShader* pAnimShader = new COGLAnimShader();
	if (!pAnimShader)
	{
		rCB.GetErrorList().GeneralError("Out of memory while creating animation shader.", iLine, iPos);
		return false;
	}

	TString sName;

	sName = "Anim_";
	sName = sName + pShader->GetName().c_str();

	pAnimShader->SetName(sName.Str());
	COGLBEReference AnimShaderRef = rCB.GetOGLDrawBase()->GetSceneRepository()->New(pAnimShader);

	pAnimShader->SetShader(Scene, sTime.Str(), sTimeStep.Str());
	rVar = AnimShaderRef;

	return true;
}
