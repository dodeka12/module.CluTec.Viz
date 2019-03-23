////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_String.cpp
//
// summary:   Implements the function string class
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
#include "CluTec.Viz.Base\TensorSingleLoop.h"

#include "Func_String.h"

typedef unsigned char uchar;

//////////////////////////////////////////////////////////////////////
/// Set Text Precission Function

bool  SetTextPrecFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVCounter iVal;

	if (iVarCount == 1)
	{
		if (!mVars(0).CastToCounter(iVal))
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if ((iVal < 0) || (iVal > 17))
		{
			rCB.GetErrorList().InvalidParVal(mVars(0), 1, iLine, iPos);
			return false;
		}

		rCB.GetTextPrec() = iVal;
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// The String FUNCTION
/// Converts variables to strings

bool  StringFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount == 1)
	{
		rVar.New(PDT_STRING);
		TString& csVal = *rVar.GetStringPtr();

		rCB.CastToString(mVars(0), csVal);
	}
	else if ((iVarCount == 2) && (mVars(0).BaseType() == PDT_VARLIST))
	{
		if (mVars(1).BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().GeneralError(
					"If two parameters are given, and first parameter "
					"is a list, then expect second parameter to be a delimiter string.", iLine, iPos);
			return false;
		}

		TVarList& rList = *mVars(0).GetVarListPtr();
		TString& rStr   = *mVars(1).GetStringPtr();

		CStrMem csEl;
		rVar.New(PDT_STRING);
		TString& csVal = *rVar.GetStringPtr();

		int i, iCount = int(rList.Count());
		for (i = 0; i < iCount; i++)
		{
			rCB.CastToString(rList(i), csEl);
			csVal << csEl;

			if (i + 1 < iCount)
			{
				csVal << rStr;
			}
		}
	}
	else if ((iVarCount == 2) && (mVars(0).BaseType() == PDT_STRING))
	{
		// Format string with parameters.
		if (mVars(1).BaseType() != PDT_VARLIST)
		{
			rCB.GetErrorList().GeneralError("Expect second parameter to be a parameter list.", iLine, iPos);
			return false;
		}

		TVarList& rList = *mVars(1).GetVarListPtr();
		int iParCnt     = int(rList.Count());

		// Analyze String
		bool bEnd        = false, bError = false;
		bool bUseListSep = false;
		size_t nPos      = 0, nPos2 = 0, nCurPos, nSubPos, nSubPos2, nSubPos3;
		int iParIdx;
		string sResult = "";
		string sError;
		string sFormat, sPar, sIdx, sType, sPrec, sSep;
		const char* pcTypeList = "DEFGTHL";
		sFormat = mVars(0).GetStringPtr()->Str();

		while (nPos < sFormat.length())
		{
			// find first occurrence of a single '{'
			nCurPos = nPos;
			do
			{
				if ((nPos2 = sFormat.find_first_of("{}", nCurPos)) == string::npos)
				{
					sResult += sFormat.substr(nPos, sFormat.length() - nPos);
					bEnd     = true;
					break;
				}

				if (sFormat[nPos2] == '{')
				{
					if (nPos2 + 1 == sFormat.length())
					{
						sError  = "Missing '}' for: ";
						sError += sFormat.substr(0, nPos2);
						sError += "@{...";
						bError  = true;
						bEnd    = true;
						break;
					}

					if ((nPos2 + 1 < sFormat.length()) && (sFormat[nPos2 + 1] != '{'))
					{
						break;
					}

					if (nPos2 + 1 == sFormat.length())
					{
						break;
					}
				}
				else	// must be '}'
				{
					if ((nPos2 + 1 == sFormat.length()) || ((nPos2 + 1 < sFormat.length()) && (sFormat[nPos2 + 1] != '}')))
					{
						sError = "Missing '{' for: ";
						if (nPos2 > 0)
						{
							sError += sFormat.substr(0, nPos2 - 1);
						}

						sError += "@}...";
						bError  = true;
						bEnd    = true;
						break;
					}
				}

				sResult += sFormat.substr(nPos, nPos2 - nPos + 1);

				if ((nPos2 + 1 < sFormat.length()) && (((sFormat[nPos2] == '}') && (sFormat[nPos2 + 1] == '}')) || ((sFormat[nPos2] == '{') && (sFormat[nPos2 + 1] == '{'))))
				{
					nPos2 += 2;
				}
				else
				{
					++nPos2;
				}

				nCurPos = nPos = nPos2;

				if (nPos > sFormat.length())
				{
					bEnd = true;
					break;
				}
			}
			while (true);

			if (bEnd)
			{
				break;
			}

			// Store string up to parameter
			sResult += sFormat.substr(nPos, nPos2 - nPos);

			// Find end of parameter
			nPos = nPos2 + 1;
			if ((nPos2 = sFormat.find_first_of("}", nPos)) == string::npos)
			{
				sError  = "Missing '}' for: ";
				sError += sFormat.substr(0, nPos - 1);
				sError += "@{...";
				bError  = true;
				break;
			}

			// Get parameter string
			sPar = sFormat.substr(nPos, nPos2 - nPos);
			if (sPar.length() == 0)
			{
				sError  = "Missing parameter index at: ";
				sError += sFormat.substr(0, nPos);
				sError += "@...";
				bError  = true;
				break;
			}

			// Find ':'
			if ((nSubPos = sPar.find_first_of(':')) == string::npos)
			{
				// no colon
				sIdx  = sPar;
				sType = "";
				sPrec = "";
			}
			else
			{
				if (nSubPos == 0)
				{
					sError  = "Missing parameter index at: ";
					sError += sFormat.substr(0, nPos);
					sError += "@...";
					bError  = true;
					break;
				}

				sIdx = sPar.substr(0, nSubPos);

				if (nSubPos + 1 >= sPar.length())
				{
					sError  = "Missing type specifier at: ";
					sError += sFormat.substr(0, nPos + nSubPos + 1);
					sError += "@...";
					bError  = true;
					break;
				}

				nSubPos2 = sPar.find_first_not_of(pcTypeList, nSubPos + 1);
				sType    = sPar.substr(nSubPos + 1, nSubPos2 - nSubPos - 1);

				// second ':'
				if ((nSubPos3 = sPar.find_first_of(':', nSubPos2)) != string::npos)
				{
					bUseListSep = true;
				}

				if (nSubPos2 < sPar.length())
				{
					sPrec = sPar.substr(nSubPos2, nSubPos3 - nSubPos2);
					if (sPrec.length() > 0)
					{
						if (sPrec.find_first_not_of("-+0 #123456789.") != string::npos)
						{
							sError  = "Invalid precision string at: ";
							sError += sFormat.substr(0, nPos + nSubPos + 2);
							sError += "@...";
							bError  = true;
							break;
						}
					}
				}
				else
				{
					sPrec = "";
				}

				// Get list separator
				if (nSubPos3 + 1 < sPar.length())
				{
					sSep = sPar.substr(nSubPos3 + 1);
				}
				else
				{
					sSep = "";
				}
			}

			if (sscanf_s(sIdx.c_str(), "%d", &iParIdx) != 1)
			{
				sError  = "Invalid parameter index at: ";
				sError += sFormat.substr(0, nPos);
				sError += "@...";
				bError  = true;
				break;
			}

			if ((iParIdx <= 0) || (iParIdx > iParCnt))
			{
				sError  = "Invalid parameter index at: ";
				sError += sFormat.substr(0, nPos);
				sError += "@...";
				bError  = true;
				break;
			}

			// Parameter index in script is 1-based.
			--iParIdx;

			if (sType.length() == 0)
			{
				TString csValue;
				if (bUseListSep && (rList(iParIdx).BaseType() == PDT_VARLIST))
				{
					TVarList& rElList = *rList(iParIdx).GetVarListPtr();
					int iEl, iElCnt = int(rElList.Count());

					for (iEl = 0; iEl < iElCnt; ++iEl)
					{
						if (!rCB.CastToString(rElList(iEl), csValue))
						{
							sError  = "Error formatting parameter value at: ";
							sError += sFormat.substr(0, nPos);
							sError += "@...";
							bError  = true;
							break;
						}

						sResult += csValue.Str();
						if (iEl + 1 < iElCnt)
						{
							sResult += sSep;
						}
					}

					if (bError)
					{
						break;
					}
				}
				else
				{
					rCB.CastToString(rList(iParIdx), csValue);
					sResult += csValue.Str();
				}
			}
			else
			{
				if ((sType == "D") || (sType == "E") || (sType == "F") || (sType == "G"))
				{
					CStrMem csValue;
					char* pcFormat = 0;
					char pcForm[50];

					if (sPrec.length() != 0)
					{
						if (sType[0] == 'D')
						{
							sprintf_s(pcForm, 50, "%%%sd", sPrec.c_str());
						}
						else if (sType[0] == 'E')
						{
							sprintf_s(pcForm, 50, "%%%se", sPrec.c_str());
						}
						else if (sType[0] == 'F')
						{
							sprintf_s(pcForm, 50, "%%%sf", sPrec.c_str());
						}
						else if (sType[0] == 'G')
						{
							sprintf_s(pcForm, 50, "%%%slg", sPrec.c_str());
						}

						pcFormat = pcForm;
					}
					else
					{
						if (sType[0] == 'D')
						{
							sprintf_s(pcForm, 50, "%%d");
						}
						else if (sType[0] == 'E')
						{
							sprintf_s(pcForm, 50, "%%e");
						}
						else if (sType[0] == 'F')
						{
							sprintf_s(pcForm, 50, "%%f");
						}
						else if (sType[0] == 'G')
						{
							sprintf_s(pcForm, 50, "%%lg");
						}

						pcFormat = pcForm;
					}

					if (bUseListSep && (rList(iParIdx).BaseType() == PDT_VARLIST))
					{
						TVarList& rElList = *rList(iParIdx).GetVarListPtr();
						int iEl, iElCnt = int(rElList.Count());

						for (iEl = 0; iEl < iElCnt; ++iEl)
						{
							if (!rCB.CastToString(rElList(iEl), csValue, pcFormat))
							{
								sError  = "Error formatting parameter value at: ";
								sError += sFormat.substr(0, nPos);
								sError += "@...";
								bError  = true;
								break;
							}

							sResult += csValue.Str();
							if (iEl + 1 < iElCnt)
							{
								sResult += sSep;
							}
						}

						if (bError)
						{
							break;
						}
					}
					else
					{
						if (!rCB.CastToString(rList(iParIdx), csValue, pcFormat))
						{
							sError  = "Error formatting parameter value at: ";
							sError += sFormat.substr(0, nPos);
							sError += "@...";
							bError  = true;
							break;
						}

						sResult += csValue.Str();
					}
				}
				else if ((sType == "T") || (sType == "TD") || (sType == "TE") || (sType == "TF") || (sType == "TG"))
				{
					CStrMem csValue;
					char* pcFormat = 0;
					char pcForm[50];

					if (sType.length() == 2)
					{
						if (sPrec.length() != 0)
						{
							if (sType[1] == 'D')
							{
								sprintf_s(pcForm, 50, "%%%sd", sPrec.c_str());
							}
							else if (sType[1] == 'E')
							{
								sprintf_s(pcForm, 50, "%%%se", sPrec.c_str());
							}
							else if (sType[1] == 'F')
							{
								sprintf_s(pcForm, 50, "%%%sf", sPrec.c_str());
							}
							else if (sType[1] == 'G')
							{
								sprintf_s(pcForm, 50, "%%%slg", sPrec.c_str());
							}

							pcFormat = pcForm;
						}
						else
						{
							if (sType[1] == 'D')
							{
								sprintf_s(pcForm, 50, "%%d");
							}
							else if (sType[1] == 'E')
							{
								sprintf_s(pcForm, 50, "%%e");
							}
							else if (sType[1] == 'F')
							{
								sprintf_s(pcForm, 50, "%%f");
							}
							else if (sType[1] == 'G')
							{
								sprintf_s(pcForm, 50, "%%lg");
							}

							pcFormat = pcForm;
						}
					}

					if (bUseListSep && (rList(iParIdx).BaseType() == PDT_VARLIST))
					{
						TVarList& rElList = *rList(iParIdx).GetVarListPtr();
						int iEl, iElCnt = int(rElList.Count());

						for (iEl = 0; iEl < iElCnt; ++iEl)
						{
							if (!rCB.CastToTextString(rElList(iEl), csValue, pcFormat))
							{
								sError  = "Error formatting parameter value at: ";
								sError += sFormat.substr(0, nPos);
								sError += "@...";
								bError  = true;
								break;
							}

							sResult += csValue.Str();
							if (iEl + 1 < iElCnt)
							{
								sResult += sSep;
							}
						}

						if (bError)
						{
							break;
						}
					}
					else
					{
						if (!rCB.CastToTextString(rList(iParIdx), csValue, pcFormat))
						{
							sError  = "Error formatting parameter value at: ";
							sError += sFormat.substr(0, nPos);
							sError += "@...";
							bError  = true;
							break;
						}

						sResult += csValue.Str();
					}
				}
				else if ((sType == "L") || (sType == "LD") || (sType == "LE") || (sType == "LF") || (sType == "LG"))
				{
					CStrMem csValue;
					char* pcFormat = 0;
					char pcForm[50];

					if (sType.length() == 2)
					{
						if (sPrec.length() != 0)
						{
							if (sType[1] == 'D')
							{
								sprintf_s(pcForm, 50, "%%%sd", sPrec.c_str());
							}
							else if (sType[1] == 'E')
							{
								sprintf_s(pcForm, 50, "%%%se", sPrec.c_str());
							}
							else if (sType[1] == 'F')
							{
								sprintf_s(pcForm, 50, "%%%sf", sPrec.c_str());
							}
							else if (sType[1] == 'G')
							{
								sprintf_s(pcForm, 50, "%%%slg", sPrec.c_str());
							}

							pcFormat = pcForm;
						}
						else
						{
							if (sType[1] == 'D')
							{
								sprintf_s(pcForm, 50, "%%d");
							}
							else if (sType[1] == 'E')
							{
								sprintf_s(pcForm, 50, "%%e");
							}
							else if (sType[1] == 'F')
							{
								sprintf_s(pcForm, 50, "%%f");
							}
							else if (sType[1] == 'G')
							{
								sprintf_s(pcForm, 50, "%%lg");
							}

							pcFormat = pcForm;
						}
					}

					if (bUseListSep && (rList(iParIdx).BaseType() == PDT_VARLIST))
					{
						TVarList& rElList = *rList(iParIdx).GetVarListPtr();
						int iEl, iElCnt = int(rElList.Count());

						for (iEl = 0; iEl < iElCnt; ++iEl)
						{
							if (!rCB.CastToLatexString(rElList(iEl), csValue, pcFormat))
							{
								sError  = "Error formatting parameter value at: ";
								sError += sFormat.substr(0, nPos);
								sError += "@...";
								bError  = true;
								break;
							}

							sResult += csValue.Str();
							if (iEl + 1 < iElCnt)
							{
								sResult += sSep;
							}
						}

						if (bError)
						{
							break;
						}
					}
					else
					{
						if (!rCB.CastToLatexString(rList(iParIdx), csValue, pcFormat))
						{
							sError  = "Error formatting parameter value at: ";
							sError += sFormat.substr(0, nPos);
							sError += "@...";
							bError  = true;
							break;
						}

						sResult += csValue.Str();
					}
				}
				else if ((sType == "H") || (sType == "HD") || (sType == "HE") || (sType == "HF") || (sType == "HG"))
				{
					CStrMem csValue;
					char* pcFormat = 0;
					char pcForm[50];

					if (sType.length() == 2)
					{
						if (sPrec.length() != 0)
						{
							if (sType[1] == 'D')
							{
								sprintf_s(pcForm, 50, "%%%sd", sPrec.c_str());
							}
							else if (sType[1] == 'E')
							{
								sprintf_s(pcForm, 50, "%%%se", sPrec.c_str());
							}
							else if (sType[1] == 'F')
							{
								sprintf_s(pcForm, 50, "%%%sf", sPrec.c_str());
							}
							else if (sType[1] == 'G')
							{
								sprintf_s(pcForm, 50, "%%%slg", sPrec.c_str());
							}

							pcFormat = pcForm;
						}
						else
						{
							if (sType[1] == 'D')
							{
								sprintf_s(pcForm, 50, "%%d");
							}
							else if (sType[1] == 'E')
							{
								sprintf_s(pcForm, 50, "%%e");
							}
							else if (sType[1] == 'F')
							{
								sprintf_s(pcForm, 50, "%%f");
							}
							else if (sType[1] == 'G')
							{
								sprintf_s(pcForm, 50, "%%lg");
							}

							pcFormat = pcForm;
						}
					}

					if (bUseListSep && (rList(iParIdx).BaseType() == PDT_VARLIST))
					{
						TVarList& rElList = *rList(iParIdx).GetVarListPtr();
						int iEl, iElCnt = int(rElList.Count());

						for (iEl = 0; iEl < iElCnt; ++iEl)
						{
							if (!rCB.CastToHTMLString(rElList(iEl), csValue, pcFormat))
							{
								sError  = "Error formatting parameter value at: ";
								sError += sFormat.substr(0, nPos);
								sError += "@...";
								bError  = true;
								break;
							}

							sResult += csValue.Str();
							if (iEl + 1 < iElCnt)
							{
								sResult += sSep;
							}
						}

						if (bError)
						{
							break;
						}
					}
					else
					{
						if (!rCB.CastToHTMLString(rList(iParIdx), csValue, pcFormat))
						{
							sError  = "Error formatting parameter value at: ";
							sError += sFormat.substr(0, nPos);
							sError += "@...";
							bError  = true;
							break;
						}

						sResult += csValue.Str();
					}
				}
				else
				{
					sError  = "Unkown format type at: ";
					sError += sFormat.substr(0, nPos + nSubPos + 1);
					sError += "@...";
					bError  = true;
					break;
				}
			}

			nPos = nPos2 + 1;
		}

		if (bError)
		{
			rCB.GetErrorList().GeneralError(sError.c_str(), iLine, iPos);
			return false;
		}

		rVar = TString(sResult.c_str());
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Hex String to Scalar Value (double)
/// Converts a HEX string into a scalar variable
/// Hex string needs to have length 16 (8 bytes)

bool  HexStr2ScalarFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount == 1)
	{
		if (mVars(0).BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().GeneralError("Expect string of length 16 as input.", iLine, iPos);
			return false;
		}

		TString& csVal = *mVars(0).GetStringPtr();
		if (csVal.Len() != 16)
		{
			rCB.GetErrorList().GeneralError("String has to be a hex string of length 16 (8 bytes).", iLine, iPos);
			return false;
		}

		double* pdVal;
		int iVal[8];
		unsigned char cVal[8];

		sscanf_s(csVal.Str(), "%2x%2x%2x%2x%2x%2x%2x%2x",
				&iVal[7], &iVal[6], &iVal[5], &iVal[4],
				&iVal[3], &iVal[2], &iVal[1], &iVal[0]);

		for (int i = 0; i < 8; i++)
		{
			cVal[i] = (unsigned char) iVal[i];
		}

		pdVal = (double*) cVal;
		rVar  = TCVScalar(*pdVal);
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Scalar Value (double) to Hex String
/// Converts a scalar value (double) to a HEX string starting with MSB
/// Hex string will have length 16 (8 bytes)

bool  Scalar2HexStrFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount == 1)
	{
		TCVScalar dVal;

		if (!mVars(0).CastToScalar(dVal, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Value given is not a scalar.", iLine, iPos);
			return false;
		}

		double dDoubleVal = double(dVal);
		unsigned char* pcVal;
		char pcStr[20];

		pcVal = (unsigned char*) &dDoubleVal;

		sprintf_s(pcStr, 20, "%02x%02x%02x%02x%02x%02x%02x%02x",
				pcVal[7], pcVal[6], pcVal[5], pcVal[4],
				pcVal[3], pcVal[2], pcVal[1], pcVal[0]);

		rVar = TString(pcStr);
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// The Scalar FUNCTION
/// Converts strings and other variables to scalars

bool  ScalarFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount == 1)
	{
		return ScalarEval(rCB, rVar, mVars(0), iLine, iPos);
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}
}

//////////////////////////////////////////////////////////////////////
/// Casting of elements to a scalar

bool  ScalarEval(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPar, int iLine, int iPos)
{
	TCVScalar dVal;

	if (rPar.BaseType() == PDT_STRING)
	{
		TString& rStr = *rPar.GetStringPtr();

		if (sscanf_s(rStr.Str(), "%lg", &dVal) == 0)
		{
			rCB.GetErrorList().GeneralError("String could not be converted to scalar.", iLine, iPos);
			return false;
		}

		rVar = dVal;
	}
	else if (rPar.CastToScalar(dVal, rCB.GetSensitivity()))
	{
		rVar = dVal;
	}
	else if (rPar.BaseType() == PDT_VARLIST)
	{
		TVarList& rList = *rPar.GetVarListPtr();
		int i, iCount = int(rList.Count());

		rVar.New(PDT_VARLIST);
		TVarList& rVarList = *rVar.GetVarListPtr();
		rVarList.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!ScalarEval(rCB, rVarList(i), rList(i), iLine, iPos))
			{
				return false;
			}
		}
	}
	else
	{
		rCB.GetErrorList().InvalidParType(rPar, 1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// The Counter FUNCTION
/// Converts strings and other variables to counters

bool  CounterFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount == 1)
	{
		return CounterEval(rCB, rVar, mVars(0), iLine, iPos);
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}
}

//////////////////////////////////////////////////////////////////////
/// Casting of elements to a counter

bool  CounterEval(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPar, int iLine, int iPos)
{
	TCVCounter iVal;

	if (rPar.BaseType() == PDT_STRING)
	{
		TString& rStr = *rPar.GetStringPtr();

		if (sscanf_s(rStr.Str(), "%d", &iVal) == 0)
		{
			rCB.GetErrorList().GeneralError("String could not be converted to counter.", iLine, iPos);
			return false;
		}

		rVar = iVal;
	}
	else if (rPar.CastToCounter(iVal, false))
	{
		rVar = iVal;
	}
	else if (rPar.BaseType() == PDT_VARLIST)
	{
		TVarList& rList = *rPar.GetVarListPtr();
		int i, iCount = int(rList.Count());

		rVar.New(PDT_VARLIST);
		TVarList& rVarList = *rVar.GetVarListPtr();
		rVarList.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!CounterEval(rCB, rVarList(i), rList(i), iLine, iPos))
			{
				return false;
			}
		}
	}
	else
	{
		rCB.GetErrorList().InvalidParType(rPar, 1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// String to ASCII ID (counter)

bool  String2ASCIIFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount != 1)
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("Expect string first parameter.", iLine, iPos);
		return false;
	}

	TString& csVal = *mVars(0).GetStringPtr();
	int iIdx, iCnt = int(csVal.Len());

	if (!iCnt)
	{
		return true;
	}

	rVar.New(PDT_VARLIST);
	TVarList& rList = *rVar.GetVarListPtr();
	rList.Set(iCnt);

	for (iIdx = 0; iIdx < iCnt; iIdx++)
	{
		rList(iIdx) = TCVCounter(uchar(csVal[iIdx]));
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// ASCII ID to String

bool  ASCII2StringFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());
	TCVCounter iVal;

	if (iVarCount != 1)
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() == PDT_VARLIST)
	{
		TVarList& rList = *mVars(0).GetVarListPtr();
		int iIdx, iCnt = int(rList.Count());

		rVar.New(PDT_STRING);
		TString& rStr = *rVar.GetStringPtr();

		char* pStr = new char[iCnt + 1];
		pStr[iCnt] = 0;

		for (iIdx = 0; iIdx < iCnt; ++iIdx)
		{
			if (!rList(iIdx).CastToCounter(iVal))
			{
				rCB.GetErrorList().GeneralError("Expect list of counters.", iLine, iPos);
				return false;
			}

			if ((iVal > 0) && (iVal <= 255))
			{
				pStr[iIdx] = char(iVal);
			}
			else
			{
				pStr[iIdx] = ' ';
			}
		}

		rStr = pStr;
		delete[] pStr;
	}
	else if (mVars(0).CastToCounter(iVal))
	{
		rVar.New(PDT_STRING);
		TString& rStr = *rVar.GetStringPtr();

		char pcVal[2] = { 0, 0 };

		if ((iVal > 0) && (iVal <= 255))
		{
			*pcVal = char(iVal);
		}
		else
		{
			*pcVal = ' ';
		}

		rStr = pcVal;
	}
	else
	{
		rCB.GetErrorList().GeneralError("Expect first parameter to be a counter or a list of counters.", iLine, iPos);
		return false;
	}

	return true;
}
