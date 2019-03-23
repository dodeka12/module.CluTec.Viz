////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      CLUPreParse.cpp
//
// summary:   Implements the clu pre parse class
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

#include "StdAfx.h"
#include "CLUPreParse.h"

#include <time.h>
#include <algorithm>
#include <string>
#include <fstream>
#include <direct.h>

#define DNG_KEY1        0xAF37C142
#define DNG_KEY2        0x8E2F6250
#define DNG_KEY3        0x029f7a6b
#define DNG_KEY4        0x8571CDBA

#include "EncodeWithoutDraw.h"

#include "CluTec.System/FileInfo.h"
#include "CluTec.System/FilePath.h"
#include "CluTec.Viz.Base/Environment.h"

using namespace std;

char* CCLUPreParse::m_ppcErrStrList[] =
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

CCLUPreParse::CCLUPreParse()
{
	Init();
}

CCLUPreParse::~CCLUPreParse()
{
}

//////////////////////////////////////////////////////////////////////
// Init

bool CCLUPreParse::Init()
{
	// ';' is line separation symbol
	m_cLineSepSym = ';';

	// Initialize Object Class 'Label'
	m_csLabelIDSymbolSpace = "abcdefghijklmnopqrstuvwxyz\u00E4\u00F6\u00FC\u00DF";
	m_csLabelIDSymbolSpace << "ABCDEFGHIJKLMNOPQRSTUVWXYZ\u00C4\u00D6\u00DC_";
	m_csLabelIDSymbolSpace << OC_IDSYM_GLOBAL_STR;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Print Parse Errors

std::string CCLUPreParse::PrintParseErrors(const std::vector<STextLine>& x_msText, int x_iCurLine, int x_iCurPos)
{
	CStrMem csLine, csError, csStr = "", csText = "";
	int iErrNo, iLen, iTextLine, iRelPos;

	iErrNo = GetError();
	if (iErrNo == CPB_NOERROR)
	{
		return std::string(csText.Str());
	}

	csError = GetErrorStr(iErrNo);

	csText << "Error No. " << iErrNo << ": " << csError << "<br>";

	if (iErrNo == CPB_NO_INCLUDEFILE)
	{
		if (m_sErrorFilename.size() > 0)
		{
			csText << "In file '<b>" << m_sErrorFilename.c_str()
			       << "</b>' could not load '<b>" << m_sIncludeFilename.c_str() << "</b>'.<br>";
		}
		else
		{
			csText << "Could not load '<b>" << m_sIncludeFilename.c_str() << "</b>'.<br>";
		}
	}
	else if (iErrNo == CPB_RECURSIVE_INCLUDEFILE)
	{
		if (m_sErrorFilename.size() > 0)
		{
			csText << "In file '<b>" << m_sErrorFilename.c_str()
			       << "</b>' the file '<b>" << m_sIncludeFilename.c_str() << "</b>' is included recursively.<br>";
		}
		else
		{
			csText << "File '<b>" << m_sIncludeFilename.c_str() << "</b>' is included recursively.<br>";
		}
	}
	else
	{
		if (x_iCurLine < (int) x_msText.size())
		{
			const STextLine& rLine = x_msText[x_iCurLine];
			iTextLine = rLine.iLine;

			if (iTextLine >= 0)
			{
				if (rLine.csFilename.Len() > 0)
				{
					csText << "In file '<b>" << rLine.csFilename << "</b>',<br>";
				}

				csStr << "<i>Line " << (iTextLine + 1) << " (" << (x_iCurPos + 1) << ")</i>: ";

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

				if (x_iCurPos >= int(rLine.vecPos.size()))
				{
					iRelPos    = rLine.vecPos.back() + 1 - rLine.iInputTextStartPos;
					sTextLine += " ";
				}
				else
				{
					iRelPos = rLine.vecPos[x_iCurPos] - rLine.iInputTextStartPos;
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

				csText << csStr << "<code><b>" << sTextLine.substr(iStart, iRelPos - iStart).c_str();
				csText << "@" << sTextLine.substr(iRelPos, iEnd - iRelPos + 1).c_str() << "</b></code><br>";
			}
		}
	}

	return std::string(csText.Str());
}

//////////////////////////////////////////////////////////////////////
int CCLUPreParse::PreParseText(std::vector<STextLine>& x_msText, int& x_iCurLine, int& x_iCurPos, const char* pcText)
{
	m_vecIncludeFilename.clear();
	m_vecIncludeFilename.push_back(m_sScriptName + ".clu");

	int iNewLines = DoPreParse(x_msText, x_iCurLine, x_iCurPos, pcText, -1);

	if (iNewLines < 0) { return iNewLines; }
	if (iNewLines == 0) { return 0; }

	return iNewLines;
}

//////////////////////////////////////////////////////////////////////
/// PreParse Text. Called by InsertText
/// Insert text separated into lines before iPos.
/// If iPos == -1 then lines are added to end.
/// Separate Text into lines using m_cSepSym as separation symbol.

int CCLUPreParse::DoPreParse(std::vector<STextLine>& x_msText,
		int& x_iCurLine,
		int& x_iCurPos,
		const char* _pcText,
		int iPos,
		const char* pcFilename,
		int iMainFilePos,
		bool bUseScriptPath)
{
	int iLineCount  = int(x_msText.size());
	int iAddedLines = 0;
	int iCurLine;	// Line Text is to be inserted before.
	int iTextLine;
	int iCommentPos           = 0;
	int iOrigTextPos          = 0;
	int iOrigTextStartPos     = 0;
	const int ciMaxLineLength = 4 * 8192;

	CStrMem csFilename;
	std::ifstream zFile;
	string sVal;
	char pcLine[500], pcCurPath[500];
	char* pcInputText;

	if (pcFilename)
	{
		csFilename = pcFilename;
	}
	else
	{
		csFilename = "";
	}

	if (bUseScriptPath)
	{
		_getcwd(pcCurPath, 499);
		_chdir(m_sScriptPath.c_str());
	}

	//printf("CParseBase:InsertText: Start\n");
	if (!_pcText) { return -1; }

	int iLen = int(strlen(_pcText));
	if (!iLen) { return -1; }

	pcInputText = new char[iLen + 1];
	if (!pcInputText) { return -1; }

	strcpy_s(pcInputText, iLen + 1, _pcText);

	char* pcText = new char[ciMaxLineLength];
	if (!pcText) { return -1; }

	// Make pcOrigText of same size as script, in case the whole
	// script is one big comment of type /* */.
	char* pcOrigText = new char[iLen + 1];
	if (!pcOrigText) { return -1; }

	char* pcCommentLine = new char[ciMaxLineLength];
	if (!pcCommentLine) { return -1; }

	if (iPos < 0)
	{
		iCurLine = iLineCount;
	}
	else
	{
		iCurLine = iPos;
	}

	int i, iLinePos, iBracketCount, iSqBraCount;
	int iInputPos, iInputPosAdj;
	unsigned char ucSym, ucPrevSym;
	bool bGotoReturn    = false;		// if true then goto next return
	bool bGotoEOC       = false;	// if true goto end of comment '*/'
	bool bInString      = false;	// Assumes string is enclosed by ""
	bool bInIfCond      = false;	// We are in IF condition, looking for closing bracket
	bool bStopAfterLine = false;
	bool bRawString     = false;

	m_sErrorFilename   = "";
	m_sIncludeFilename = "";

	vector<int> vecFuncRBC, vecInputPos;

	iSqBraCount = 0;

	iInputPos = -1;
	iLinePos  = 0;
	iTextLine = 0;
	ucSym     = 0;

	for (i = 0; i < iLen; i++)
	{
		// Check whether current line is too long
		if (iLinePos >= ciMaxLineLength - 1)
		{
			delete[] pcText;
			delete[] pcInputText;
			delete[] pcOrigText;
			delete[] pcCommentLine;

			return -1;
		}

		iInputPosAdj = 0;
		ucPrevSym    = ucSym;
		ucSym        = (unsigned char) pcInputText[i];

		pcOrigText[iOrigTextPos++] = ucSym;
		iInputPos++;

		if (ucSym == 10)
		{
			iTextLine++;

			if (bStopAfterLine)
			{
				break;
			}
		}
		//if (ucSym == 10 || ucSym == 13)
		//{
		//	if (ucPrevSym != 10 && ucPrevSym != 13)
		//		iTextLine++;
		//}

		if (bGotoReturn)
		{
			if ((ucSym != 10) && (ucSym != 13))
			{
				pcCommentLine[iCommentPos++] = ucSym;
				continue;
			}
			else
			{
				pcCommentLine[iCommentPos] = 0;

				//pcOrigText[iOrigTextPos] = 0;
				//iOrigTextStartPos += iOrigTextPos;
				//iOrigTextPos = 0;

				bGotoReturn = false;

				// Check for PreParse Statements.
				// They need to start with a '#'.
				if (pcCommentLine[0] == '#')
				{
					char* pcVal, * pcVal2;
					// Check whether preprocessor statement is 'include'
					if (strstr(pcCommentLine, "include"))
					{
						pcOrigText[iOrigTextPos] = 0;
						iOrigTextStartPos       += iOrigTextPos;
						iOrigTextPos             = 0;

						// Look for left quotations
						if ((pcVal = strchr(pcCommentLine, '\"')))
						{
							pcVal++;
							// Look for right quotations
							if ((pcVal2 = strchr(pcVal, '\"')))
							{
								vector<string>::iterator it_El;
								*pcVal2 = 0;

								std::string sCWD, sIncFileDir, sIncFileName, sIncFileExt, sResultPath;
								char pcCWD[_MAX_PATH];
								_getcwd(pcCWD, _MAX_PATH);
								sCWD = pcCWD;

								// Check whether include file has already been included
								m_sIncludeFilename = pcVal;

								if ((csFilename.Len() > 0) && !bUseScriptPath)
								{
									Clu::CFileInfo xInfo(csFilename.Str());

									// Check whether include file is in path of including file or in a subfolder with the
									// name of the including file.
									if (Clu::Viz::CEnvironment::FindFileInPathOrSubfolder(sResultPath, xInfo.GetDirectory(), xInfo.GetName(), m_sIncludeFilename))
									{
										m_sIncludeFilename = sResultPath;
									}
									// if the include file cannot be found there, have a look in the '_global' subfolder of the main script.
									else if (Clu::Viz::CEnvironment::FindFileInPathOrSubfolder(sResultPath, m_sScriptPath, "_global", m_sIncludeFilename))
									{
										m_sIncludeFilename = sResultPath;
									}
									// if the the file is also not there, have a look in the Runtime Environment global script path
									else if (Clu::Viz::CEnvironment::FindFileInPathOrSubfolder(sResultPath, Clu::Viz::CEnvironment::GetPath(Clu::Viz::EEnvPath::Script_Root), "_global",
												 m_sIncludeFilename))
									{
										m_sIncludeFilename = sResultPath;
									}
								}
								else if (bUseScriptPath)
								{
									// Check whether include file is in the script path or the subfolder with the script name.
									if (Clu::Viz::CEnvironment::FindFileInPathOrSubfolder(sResultPath, m_sScriptPath, m_sScriptName, m_sIncludeFilename))
									{
										m_sIncludeFilename = sResultPath;
									}
									// if the include file cannot be found there, have a look in the '_global' subfolder of the main script.
									else if (Clu::Viz::CEnvironment::FindFileInPathOrSubfolder(sResultPath, m_sScriptPath, "_global", m_sIncludeFilename))
									{
										m_sIncludeFilename = sResultPath;
									}
									// if the the file is also not there, have a look in the Runtime Environment global script path
									else if (Clu::Viz::CEnvironment::FindFileInPathOrSubfolder(sResultPath, Clu::Viz::CEnvironment::GetPath(Clu::Viz::EEnvPath::Script_Root), "_global",
												 m_sIncludeFilename))
									{
										m_sIncludeFilename = sResultPath;
									}
								}

								m_sIncludeFilename = Clu::CFilePath::MakeAbsolute(m_sIncludeFilename);

								Clu::CFileInfo xIncInfo(m_sIncludeFilename);
								sIncFileDir = xIncInfo.GetDirectory();
								sIncFileName = xIncInfo.GetName();
								sIncFileExt = xIncInfo.GetExt();

								it_El = find(m_vecIncludeFilename.begin(),
										m_vecIncludeFilename.end(),
										m_sIncludeFilename);

								if (it_El != m_vecIncludeFilename.end())
								{
									SetError(CPB_RECURSIVE_INCLUDEFILE);

									delete[] pcInputText;
									delete[] pcText;
									delete[] pcOrigText;
									delete[] pcCommentLine;

									if (pcFilename)
									{
										m_sErrorFilename = pcFilename;
										m_sErrorPath     = m_sScriptPath;
									}
									else
									{
										m_sErrorFilename = "";
										m_sErrorPath     = "";
									}

									if (iMainFilePos < 0)
									{
										return -(i - iCommentPos) - 2;
									}
									else
									{
										return -iMainFilePos - 2;
									}
								}

								m_vecIncludeFilename.push_back(m_sIncludeFilename);

								if (sIncFileExt == ".cbn")
								{
									const char* pcPass = 0;
									Mem<char> mCode;
									string sError, sPass;
									map<string, string> mapMetaData;

									// Check whether binary include file password is given
									// Expected Syntax: //! include "File.clu","password"
									// Password has to be of length 8
									pcVal = pcVal2 + 1;
									if ((pcVal = strchr(pcVal, ',')))
									{
										++pcVal;
										if ((pcVal = strchr(pcVal, '\"')))
										{
											++pcVal;
											if ((pcVal2 = strchr(pcVal, '\"')))
											{
												*pcVal2 = 0;
												if (strlen(pcVal) == 8)
												{
													sPass  = pcVal;
													pcPass = sPass.c_str();
												}
											}
										}
									}

									if (!ReadCLUBinaryFile(m_sIncludeFilename.c_str(), mCode, mapMetaData, sError, pcPass))
									{
										SetError(CPB_NO_INCLUDEFILE);

										delete[] pcInputText;
										delete[] pcText;
										delete[] pcOrigText;
										delete[] pcCommentLine;

										if (pcFilename)
										{
											m_sErrorFilename = pcFilename;
											m_sErrorPath     = m_sScriptPath;
										}
										else
										{
											m_sErrorFilename = "";
											m_sErrorPath     = "";
										}

										if (iMainFilePos < 0)
										{
											return -(i - iCommentPos) - 2;
										}
										else
										{
											return -iMainFilePos - 2;
										}
									}

									int iIncludeLineCount = SetTextCode(x_msText, mCode.Data(), m_sIncludeFilename.c_str(), true);
									m_vecIncludeFilename.pop_back();

									if (iIncludeLineCount < 0)
									{
										delete[] pcText;
										delete[] pcInputText;
										delete[] pcOrigText;
										delete[] pcCommentLine;

										return iIncludeLineCount;
									}

									iCurLine    += iIncludeLineCount;
									iAddedLines += iIncludeLineCount;
									continue;
								}
								else	// include is clu source file
								{
									// Read include file
									sVal = "";
									zFile.clear();
									zFile.open(m_sIncludeFilename.c_str());
									if (zFile.is_open())
									{
										while (!zFile.eof())
										{
											zFile.getline(pcLine, 499);
											sVal += pcLine;
											sVal += "\n";
										}

										zFile.close();
									}
									else
									{
										SetError(CPB_NO_INCLUDEFILE);

										delete pcInputText;
										delete pcText;
										delete pcOrigText;
										delete pcCommentLine;

										if (pcFilename)
										{
											m_sErrorFilename = pcFilename;
											m_sErrorPath     = m_sScriptPath;
										}
										else
										{
											m_sErrorFilename = "";
											m_sErrorPath     = "";
										}

										if (iMainFilePos < 0)
										{
											return -(i - iCommentPos) - 2;
										}
										else
										{
											return -iMainFilePos - 2;
										}
									}

									//int iOldTextLen = strlen(&pcInputText[i]);
									int iNewTextLen = int(sVal.size());

									// If there is text to be included
									if (iNewTextLen)
									{
										string sDirName = sIncFileDir.substr(0, sIncFileDir.size() - 1);
										// Switch working directory to path of include file

										// Preparse include file
										int iIncludeLineCount = DoPreParse(x_msText, x_iCurLine, x_iCurPos, sVal.c_str(), iCurLine,
												m_sIncludeFilename.c_str(), (iMainFilePos < 0 ? (i - iCommentPos) : iMainFilePos),
												false);

										// Switch back to previous working directory
										_chdir(sCWD.c_str());

										m_vecIncludeFilename.pop_back();

										if (iIncludeLineCount < 0)
										{
											delete[] pcText;
											delete[] pcInputText;
											delete[] pcOrigText;
											delete[] pcCommentLine;

											return iIncludeLineCount;
										}

										iCurLine    += iIncludeLineCount;
										iAddedLines += iIncludeLineCount;
										continue;
									}
								}	// if file is clubinary
							}
						}
					}
				}
				continue;
			}
		}

		if (bGotoEOC)
		{
			if ((ucSym == '*') && (pcInputText[i + 1] == '/'))
			{
				pcOrigText[iOrigTextPos++] = pcInputText[i + 1];
				iInputPos++;
				i++;
				bGotoEOC = false;
			}
			continue;
		}

		if (!bInString && ((ucSym < 32) || (ucSym == 255)) && (ucSym != '\t'))
		{
			continue;
		}

		if ((ucSym == '\"') &&
		    (bRawString ||
		     ((ucPrevSym != '\\') ||
		      ((i > 1) && (ucPrevSym == '\\') && (pcInputText[i - 2] == '\\')))))
		{
			if (bInString)
			{
				bInString  = false;
				bRawString = false;
			}
			else
			{
				bInString = true;
			}
		}

		if (!bInString && (ucSym == '@') && (pcInputText[i + 1] == '\"'))
		{
			pcText[iLinePos++] = ucSym;
			vecInputPos.push_back(iInputPos);
			pcText[iLinePos++] = '\"';
			vecInputPos.push_back(iInputPos);

			pcOrigText[iOrigTextPos++] = '\"';
			iInputPos                 += 2;
			i++;
			ucPrevSym  = '\"';
			bInString  = true;
			bRawString = true;
			continue;
		}
		/*
		if (bInString)
		{
		if (i+1 < iLen && ucSym == '\\' && pcInputText[i+1] == '\\')
		{
		pcOrigText[iOrigTextPos++] = pcInputText[i+1];
		i++;
		iInputPos++;
		iInputPosAdj = -1;
		}
		}
		else
		*/
		if (!bInString)
		{
			// Check for comments
			if ((ucSym == '/') && (pcInputText[i + 1] == '/'))
			{
				iCommentPos                = 0;
				bGotoReturn                = true;
				pcOrigText[iOrigTextPos++] = pcInputText[i + 1];
				iInputPos++;
				i++;
				continue;
			}

			if ((ucSym == '/') && (pcInputText[i + 1] == '*'))
			{
				bGotoEOC                   = true;
				pcOrigText[iOrigTextPos++] = pcInputText[i + 1];
				iInputPos++;
				i++;
				continue;
			}

			if ((ucSym == m_cLineSepSym) || (ucSym == '}'))
			{
				if (bInIfCond || (iSqBraCount > 0))
				{
					SetError(CPB_MISSING_IFCOND);
					bStopAfterLine = true;
					x_iCurLine     = iCurLine;
					x_iCurPos      = iLinePos;

					break;
					//					return -i-2;
				}

				if (ucSym == '}')
				{
					pcText[iLinePos++] = '}';
					vecInputPos.push_back(iInputPos);
				}

				pcText[iLinePos]         = 0;
				pcOrigText[iOrigTextPos] = 0;

				STextLine xTextLine;
				xTextLine.csText             = pcText;
				xTextLine.csInputText        = pcOrigText;
				xTextLine.csFilename         = csFilename;
				xTextLine.iMainFilePos       = iMainFilePos;
				xTextLine.iInputTextStartPos = iOrigTextStartPos;
				xTextLine.iLine              = iTextLine;
				xTextLine.vecPos             = vecInputPos;

				x_msText.push_back(xTextLine);

				iOrigTextStartPos += iOrigTextPos;
				iOrigTextPos       = 0;
				iLinePos           = 0;
				iCurLine++;
				iAddedLines++;
				vecInputPos.erase(vecInputPos.begin(), vecInputPos.end());

				continue;
			}

			if (bInIfCond)
			{
				if (ucSym == '(')
				{
					iBracketCount++;
				}
				else if (ucSym == ')')
				{
					// Is If clause finished here?
					if (--iBracketCount == 0)
					{
						bInIfCond          = false;
						pcText[iLinePos++] = ')';
						vecInputPos.push_back(iInputPos);

						pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
						vecInputPos.push_back(iInputPos);

						// Set THEN ID-symbol
						ucSym = OC_IDSYM_THEN_CHAR;
					}
				}
			}

			if ((i + 2 < iLen) &&
			    (ucSym == 'i') &&
			    (pcInputText[i + 1] == 'f') &&
			    (strchr(m_csLabelIDSymbolSpace.Str(), pcInputText[i + 2]) == 0))
			{
				// Are we already in if condition?
				if (bInIfCond)
				{
					SetError(CPB_MISSING_IFCOND);
					bStopAfterLine = true;
					x_iCurLine     = iCurLine;
					x_iCurPos      = iLinePos;

					break;
					//return -i-2;
				}

				// Set IF ID-symbol
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);
				pcText[iLinePos++] = OC_IDSYM_IF_CHAR;
				vecInputPos.push_back(iInputPos);

				// Look for (
				while (i < iLen && pcInputText[i] != '(')
				{
					i++;
					pcOrigText[iOrigTextPos++] = pcInputText[i];
					iInputPos++;
				}

				if (i == iLen)
				{
					SetError(CPB_MISSING_IFCOND);
					bStopAfterLine = true;
					x_iCurLine     = iCurLine;
					x_iCurPos      = iLinePos;

					break;
					//return -i-2;
				}

				// Initialize bracket count
				iBracketCount = 1;
				bInIfCond     = true;
				ucSym         = pcInputText[i];
			}
			else if ((i + 4 < iLen) && (ucSym == 'e') &&
				 (pcInputText[i + 1] == 'l') &&
				 (pcInputText[i + 2] == 's') &&
				 (pcInputText[i + 3] == 'e') &&
				 (strchr(m_csLabelIDSymbolSpace.Str(), pcInputText[i + 4]) == 0))
			{
				// Set ELSE ID-symbol
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym                      = OC_IDSYM_ELSE_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i + 1];
				pcOrigText[iOrigTextPos++] = pcInputText[i + 2];
				pcOrigText[iOrigTextPos++] = pcInputText[i + 3];
				i                         += 3;
				iInputPos                 += 3;
			}
			else if ((i + 4 < iLen) && (ucSym == 'l') &&
				 (pcInputText[i + 1] == 'o') &&
				 (pcInputText[i + 2] == 'o') &&
				 (pcInputText[i + 3] == 'p') &&
				 (strchr(m_csLabelIDSymbolSpace.Str(), pcInputText[i + 4]) == 0))
			{
				// Set LOOP ID-symbol
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym                      = OC_IDSYM_LOOP_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i + 1];
				pcOrigText[iOrigTextPos++] = pcInputText[i + 2];
				pcOrigText[iOrigTextPos++] = pcInputText[i + 3];
				i                         += 3;
				iInputPos                 += 3;
			}
			else if ((i + 5 < iLen) && (ucSym == 'b') &&
				 (pcInputText[i + 1] == 'r') &&
				 (pcInputText[i + 2] == 'e') &&
				 (pcInputText[i + 3] == 'a') &&
				 (pcInputText[i + 4] == 'k') &&
				 (strchr(m_csLabelIDSymbolSpace.Str(), pcInputText[i + 5]) == 0))
			{
				// Set BREAK ID-symbol
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym                      = OC_IDSYM_BREAK_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i + 1];
				pcOrigText[iOrigTextPos++] = pcInputText[i + 2];
				pcOrigText[iOrigTextPos++] = pcInputText[i + 3];
				pcOrigText[iOrigTextPos++] = pcInputText[i + 4];
				i                         += 4;
				iInputPos                 += 4;
			}
			else if ((ucSym == '(') && IsFuncCall(pcInputText, i, iLen))
			{
				// Function call or component extraction of matrix or list, etc.
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				pcText[iLinePos++] = OC_IDSYM_CALL_CHAR;
				vecInputPos.push_back(iInputPos);
				vecFuncRBC.push_back(1);
			}
			else if (ucSym == '(')
			{
				int iSize = int(vecFuncRBC.size());
				if (iSize > 0)
				{
					++vecFuncRBC[iSize - 1];
				}
			}
			else if (ucSym == ')')
			{
				int iSize = int(vecFuncRBC.size());
				if (iSize > 0)
				{
					if (--vecFuncRBC[iSize - 1] == 0)
					{
						vecFuncRBC.pop_back();
					}
				}
			}
			else if (ucSym == ',')
			{
				int iSize = int(vecFuncRBC.size());
				if (((iSize > 0) && (vecFuncRBC[iSize - 1] != 1) && (iSqBraCount == 0)) ||
				    ((iSize == 0) && (iSqBraCount == 0)))
				{
					SetError(CPB_UNEXPSYMCOMMA);
					x_iCurLine = iCurLine;
					x_iCurPos  = iLinePos;

					bStopAfterLine = true;
				}
			}
			else if (ucSym == '[')
			{
				iSqBraCount++;
			}
			else if (ucSym == ']')
			{
				if (iSqBraCount == 0)
				{
					SetError(CPB_UNEXPECTEDSYM);
					x_iCurLine     = iCurLine;
					x_iCurPos      = iLinePos;
					bStopAfterLine = true;
				}
				else
				{
					iSqBraCount--;
				}
			}
			else if ((i + 1 < iLen) && (ucSym == '^') && (pcInputText[i + 1] == '^'))
			{
				// Set POWER operator ID-symbol
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym                      = OC_IDSYM_POW_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i + 1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			else if ((i + 1 < iLen) && (ucSym == '=') && (pcInputText[i + 1] == '='))
			{
				// Set EQUAL operator ID-symbol
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym                      = OC_IDSYM_EQUAL_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i + 1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			else if ((i + 1 < iLen) && (ucSym == '!') && (pcInputText[i + 1] == '='))
			{
				// Set NOT EQUAL operator ID-symbol
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym                      = OC_IDSYM_NOTEQUAL_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i + 1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			else if ((i + 1 < iLen) && (ucSym == '&') && (pcInputText[i + 1] == '&'))
			{
				// Set AND operator ID-symbol
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym                      = OC_IDSYM_AND_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i + 1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			else if ((i + 1 < iLen) && (ucSym == '|') && (pcInputText[i + 1] == '|'))
			{
				// Set OR operator ID-symbol
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym                      = OC_IDSYM_OR_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i + 1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			else if ((i + 1 < iLen) && (ucSym == '!') && (pcInputText[i + 1] == '!'))
			{
				// Set NOT operator ID-symbol
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym                      = OC_IDSYM_NOT_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i + 1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			else if ((i + 1 < iLen) && (ucSym == '<') && (pcInputText[i + 1] == '='))
			{
				// Set Less Equal operator ID-symbol
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym                      = OC_IDSYM_LE_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i + 1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			else if ((i + 1 < iLen) && (ucSym == '>') && (pcInputText[i + 1] == '='))
			{
				// Set Greater Equal operator ID-symbol
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym                      = OC_IDSYM_GE_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i + 1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			else if ((i + 1 < iLen) && (ucSym == '<') && (pcInputText[i + 1] == '<'))
			{
				// Set Left Shift operator ID-symbol
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos - 1);

				ucSym                      = OC_IDSYM_LSHIFT_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i + 1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			else if ((i + 1 < iLen) && (ucSym == '-') && (pcInputText[i + 1] == '>'))
			{
				// Set Reference Symbol ID
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym                      = OC_IDSYM_REF_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i + 1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			else if ((i + 1 < iLen) && (ucSym == ':') && (pcInputText[i + 1] == ':'))
			{
				// Set Reference Symbol ID
				ucSym                      = OC_IDSYM_GLOBAL_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i + 1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			else if ((i + 2 < iLen) && (ucSym == '.') && (pcInputText[i + 1] == '&') &&
				 (pcInputText[i + 2] == '&'))
			{
				// Set Point AND Symbol ID
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym                      = OC_IDSYM_PAND_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i + 1];
				pcOrigText[iOrigTextPos++] = pcInputText[i + 2];
				i                         += 2;
				iInputPos                 += 2;
				iInputPosAdj               = -2;
			}
			else if ((i + 2 < iLen) && (ucSym == '.') && (pcInputText[i + 1] == '|') &&
				 (pcInputText[i + 2] == '|'))
			{
				// Set Point OR Symbol ID
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym                      = OC_IDSYM_POR_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i + 1];
				pcOrigText[iOrigTextPos++] = pcInputText[i + 2];
				i                         += 2;
				iInputPos                 += 2;
				iInputPosAdj               = -2;
			}
			else if ((i + 2 < iLen) && (ucSym == '.') && (pcInputText[i + 1] == '=') &&
				 (pcInputText[i + 2] == '='))
			{
				// Set Point EQUAL Symbol ID
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym                      = OC_IDSYM_PEQUAL_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i + 1];
				pcOrigText[iOrigTextPos++] = pcInputText[i + 2];
				i                         += 2;
				iInputPos                 += 2;
				iInputPosAdj               = -2;
			}
			else if ((i + 2 < iLen) && (ucSym == '.') && (pcInputText[i + 1] == '!') &&
				 (pcInputText[i + 2] == '='))
			{
				// Set Point NOT EQUAL Symbol ID
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym                      = OC_IDSYM_PNEQUAL_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i + 1];
				pcOrigText[iOrigTextPos++] = pcInputText[i + 2];
				i                         += 2;
				iInputPos                 += 2;
				iInputPosAdj               = -2;
			}
			else if ((i + 2 < iLen) && (ucSym == '.') && (pcInputText[i + 1] == '<') &&
				 (pcInputText[i + 2] == '='))
			{
				// Set Point LESS EQUAL Symbol ID
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym                      = OC_IDSYM_PLTE_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i + 1];
				pcOrigText[iOrigTextPos++] = pcInputText[i + 2];
				i                         += 2;
				iInputPos                 += 2;
				iInputPosAdj               = -2;
			}
			else if ((i + 2 < iLen) && (ucSym == '.') && (pcInputText[i + 1] == '>') &&
				 (pcInputText[i + 2] == '='))
			{
				// Set Point GREATER EQUAL Symbol ID
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym                      = OC_IDSYM_PGTE_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i + 1];
				pcOrigText[iOrigTextPos++] = pcInputText[i + 2];
				i                         += 2;
				iInputPos                 += 2;
				iInputPosAdj               = -2;
			}
			else if ((i + 2 < iLen) && (ucSym == '.') && (pcInputText[i + 1] == '<') &&
				 (pcInputText[i + 2] == '<'))
			{
				// Set Point LShift Symbol ID
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym                      = OC_IDSYM_PLSHIFT_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i + 1];
				pcOrigText[iOrigTextPos++] = pcInputText[i + 2];
				i                         += 2;
				iInputPos                 += 2;
				iInputPosAdj               = -2;
			}
			else if ((i + 1 < iLen) && (ucSym == '.') && (pcInputText[i + 1] == '/'))
			{
				// Set Point Div Symbol ID
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym                      = OC_IDSYM_PDIV_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i + 1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			else if ((i + 1 < iLen) && (ucSym == '.') && (pcInputText[i + 1] == '*'))
			{
				// Set Point Mul Symbol ID
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym                      = OC_IDSYM_PMUL_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i + 1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			else if ((i + 1 < iLen) && (ucSym == '.') && (pcInputText[i + 1] == '.'))
			{
				// Set Point Mul Symbol ID
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym                      = OC_IDSYM_PIP_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i + 1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			else if ((i + 1 < iLen) && (ucSym == '.') && (pcInputText[i + 1] == '^'))
			{
				// Set Point Mul Symbol ID
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym                      = OC_IDSYM_POP_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i + 1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			else if ((i + 1 < iLen) && (ucSym == '.') && (pcInputText[i + 1] == '-'))
			{
				// Set Point Mul Symbol ID
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym                      = OC_IDSYM_PMINUS_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i + 1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			else if ((i + 1 < iLen) && (ucSym == '.') && (pcInputText[i + 1] == '+'))
			{
				// Set Point Add Symbol ID
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym                      = OC_IDSYM_PPLUS_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i + 1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			else if ((i + 1 < iLen) && (ucSym == '.') && (pcInputText[i + 1] == '>'))
			{
				// Set Point GREATER Symbol ID
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym                      = OC_IDSYM_PGT_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i + 1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			else if ((i + 1 < iLen) && (ucSym == '.') && (pcInputText[i + 1] == '<'))
			{
				// Set Point LESS Symbol ID
				pcText[iLinePos++] = OC_IDSYM_SPECIAL_CHAR;
				vecInputPos.push_back(iInputPos);

				ucSym                      = OC_IDSYM_PLT_CHAR;
				pcOrigText[iOrigTextPos++] = pcInputText[i + 1];
				i++;
				iInputPos++;
				iInputPosAdj = -1;
			}
			// Replace e.g. ' .1' by ' 0.1'
			else if ((i + 1 < iLen) && (ucSym == '.') &&
				 (strchr("1234567890", pcInputText[i + 1]) != 0))
			{
				if ((i == 0) ||
				    ((strchr("1234567890", pcInputText[i - 1]) == 0) &&
				     (strchr(m_csLabelIDSymbolSpace.Str(), pcInputText[i - 1]) == 0)))
				{
					pcText[iLinePos++] = '0';
					vecInputPos.push_back(iInputPos);
				}
			}
		}

		pcText[iLinePos++] = (char) ucSym;
		vecInputPos.push_back(iInputPos + iInputPosAdj);
	}

	if (iLinePos > 0)
	{
		pcText[iLinePos]         = 0;
		pcOrigText[iOrigTextPos] = 0;

		STextLine xTextLine;
		xTextLine.csText             = pcText;
		xTextLine.csInputText        = pcOrigText;
		xTextLine.csFilename         = csFilename;
		xTextLine.iMainFilePos       = iMainFilePos;
		xTextLine.iInputTextStartPos = iOrigTextStartPos;
		xTextLine.iLine              = iTextLine;
		xTextLine.vecPos             = vecInputPos;

		x_msText.push_back(xTextLine);

		iOrigTextStartPos += iOrigTextPos;
		iOrigTextPos       = 0;

		iLinePos = 0;
		iCurLine++;
		iAddedLines++;
	}

	//printf("CParseBase:InsertText: End\n");

	if (bUseScriptPath)
	{
		_chdir(pcCurPath);
	}

	delete[] pcText;
	delete[] pcInputText;
	delete[] pcOrigText;
	delete[] pcCommentLine;

	if (bStopAfterLine)
	{
		return -1;
	}

	return iAddedLines;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Test whether current '(' symbol refers to a function call

bool CCLUPreParse::IsFuncCall(const char* pcInputText, int iIdx, int iLen)
{
	if (!pcInputText || (iIdx == 0))
	{
		return false;
	}

	int iPos = iIdx - 1;
	while (iPos >= 0 && strchr(" \t\r\n", pcInputText[iPos]) != 0)
	{
		--iPos;
	}

	if ((strchr(m_csLabelIDSymbolSpace.Str(), pcInputText[iPos]) != 0) ||
	    (strchr("1234567890", pcInputText[iPos]) != 0) ||
	    (pcInputText[iPos] == ')') ||
	    (pcInputText[iPos] == ']') ||
	    (pcInputText[iPos] == '"'))
	{
		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Read CLUBinary File

bool CCLUPreParse::ReadCLUBinaryFile(const char* pcFilename, Mem<char>& mCode, map<string, string>& mapMetaData, string& sError, const char* pcPass)
{
	CEncodeWithoutDraw Encode;
	string sFileID;
	sError = "";

	if (!Encode.ReadFileID(pcFilename, sFileID))
	{
		sError = Encode.GetError();
		return false;
	}

	unsigned uDataLen;
	char pcType[100];
	const char* pcMetaType[] = { "title", "version", "copyright", "about" };
	int iKnownMetaCnt;
	int iMetaIdx, iMetaCnt;
	int iMetaTotalLen;
	int iBinaryVersion = 0;

	if (sFileID == "CLUBinary")
	{
		// CLUBinary does not implement encoding with key
		if (!Encode.ReadFile(pcFilename, sFileID, mCode, 0))
		{
			sError = Encode.GetError();
			return false;
		}

		iKnownMetaCnt = 4;
		iMetaCnt      = int(mCode[0]);
		iMetaTotalLen = 1;
		uDataLen      = int(mCode.Count());

		if ((mCode[uDataLen - 2] != 0) || (mCode[uDataLen - 1] != 0))
		{
			sError  = "File '";
			sError += pcFilename;
			sError += "' appears to be corrupted.";
			mCode.Set(0);
			return false;
		}

		iBinaryVersion = 1;
	}
	else if (sFileID == "CLUBinary2")
	{
		if (!Encode.ReadFile(pcFilename, sFileID, mCode, pcPass))
		{
			sError = Encode.GetError();
			return false;
		}

		iKnownMetaCnt = 4;
		iMetaCnt      = int(mCode[4]);
		iMetaTotalLen = 5;

		// Check that decoded data is correctly decoded
		if ((mCode[0] != char(0x2E)) || (mCode[1] != char(0x41)) || (mCode[2] != char(0x9F)) || (mCode[3] != char(0xD2)))
		{
			sError  = "Incorrect key to decode file '";
			sError += pcFilename;
			sError += "'.";
			mCode.Set(0);
			return false;
		}

		uDataLen = int(mCode.Count());
		int* piCode = (int*) &mCode[uDataLen - sizeof(int)];

		if (piCode[0] != 0)
		{
			sError  = "File '";
			sError += pcFilename;
			sError += "' appears to be corrupted.";
			mCode.Set(0);
			return false;
		}

		iBinaryVersion = 2;
	}
	else
	{
		sError  = "File '";
		sError += pcFilename;
		sError += "' is not a CLUBinary file.";
		return false;
	}

	// Extract Meta Information

	mapMetaData.clear();
	char* pcVal = &mCode[iMetaTotalLen];
	for (iMetaIdx = 0; iMetaIdx < iMetaCnt; iMetaIdx++)
	{
		if (iMetaIdx < iKnownMetaCnt)
		{
			mapMetaData[pcMetaType[iMetaIdx]] = pcVal;
		}
		else
		{
			sprintf_s(pcType, 100, "Meta_%03d", iMetaIdx - iKnownMetaCnt + 1);
			mapMetaData[pcType] = pcVal;
		}

		iMetaTotalLen += int(strlen(pcVal) + 1);
		pcVal         += strlen(pcVal) + 1;
	}

	mCode.Del(0, iMetaTotalLen);

	// Insert as first element in code data the binary version
	mCode.Insert(0, sizeof(int));
	int* piCode = (int*) &mCode[0];
	piCode[0] = iBinaryVersion;

	return true;
}

int CCLUPreParse::SetTextCode(std::vector<STextLine>& x_msText, const char* pcCode, const char* pcFilename, bool bAdd)
{
	char* pcLine = (char*) pcCode;
	int* piLine  = (int*) pcLine;
	int iTotalLen, iLineCnt, iNewLineCnt, iAbsPos;

	iLineCnt = int(x_msText.size());
	iAbsPos  = 0;

	if (piLine[0] == 1)
	{
		// Binary version 1

		// Step to first element of actual code
		piLine++;
		iAbsPos += sizeof(int);
		pcLine   = (char*) piLine;

		while (pcLine[0] != 0)
		{
			STextLine rLine;
			rLine.iLine              = -1;
			rLine.iMainFilePos       = -1;
			rLine.iInputTextStartPos = -1;
			rLine.csFilename         = pcFilename;
			rLine.csText             = pcLine;

			x_msText.push_back(rLine);

			iAbsPos += int(strlen(pcLine) + 1);
			pcLine   = (char*) &pcCode[iAbsPos];
			piLine   = (int*) pcLine;
		}
	}
	else if (piLine[0] == 2)
	{
		// Binary version 2

		// Step to first element of actual code
		piLine++;
		iAbsPos += sizeof(int);
		pcLine   = (char*) piLine;

		// Structure of line data
		// (int) Total lenght of line data in bytes including this element
		// (int) Line index (iLine)
		// (int) Position in main file (iMainFilePos)
		// (int) Number of elements in vecPos
		// Nx(int) Elements in vecPos
		// Nx(char) Filename
		// Nx(char) Code line

		while ((iTotalLen = piLine[0]) > 0)
		{
			STextLine rLine;
			rLine.iLine              = piLine[1];
			rLine.iMainFilePos       = piLine[2];
			rLine.iInputTextStartPos = -1;

			pcLine = (char*) &piLine[3];
			if (pcLine[0])
			{
				rLine.csFilename = pcLine;
			}
			else
			{
				rLine.csFilename = pcFilename;
			}

			pcLine += strlen(pcLine) + 1;

			rLine.csText = pcLine;

			x_msText.push_back(rLine);

			iAbsPos += iTotalLen;
			pcLine   = (char*) &pcCode[iAbsPos];
			piLine   = (int*) pcLine;
		}
	}
	else
	{
		// unknown binary version
		return 0;
	}

	iNewLineCnt = int(x_msText.size()) - iLineCnt;

	return iNewLineCnt;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUPreParse::GenTextCode(std::vector<char>& vecCode, const std::vector<STextLine>& x_msText)
{
	vecCode.resize(0);
	vecCode.shrink_to_fit();

	if (x_msText.size() == 0)
	{
		return false;
	}

	/************************************************************************/
	/* For each parsed line                                                 */
	/************************************************************************/
	for (size_t nIdx = 0; nIdx < x_msText.size(); nIdx++)
	{
		// The parsed text line
		const STextLine& rLine = x_msText[nIdx];

		char* pcCodeLine = rLine.csText.Str();
		char* pcFilename = rLine.csFilename.Str();

		// Length of string including trailing zero
		size_t nCodeLen     = strlen(pcCodeLine) + 1;
		size_t nFilenameLen = strlen(pcFilename) + 1;

		// Total length of code line plus position data
		size_t nTotalLen = nCodeLen + nFilenameLen + 3 * sizeof(int);

		size_t nCodePos = vecCode.size();
		vecCode.resize(vecCode.size() + nTotalLen);

		// Structure of line data
		// (int) Total length of line data in bytes including this element
		// (int) Line index (iLine)
		// (int) Position in main file (iMainFilePos)
		// Nx(char) Filename
		// Nx(char) Code line

		int* piCode = (int*) &vecCode[nCodePos];

		piCode[0] = int(nTotalLen);
		piCode[1] = rLine.iLine;
		piCode[2] = rLine.iMainFilePos;

		char* pcCode = (char*) &piCode[3];

		memcpy(pcCode, pcFilename, nFilenameLen);
		pcCode += nFilenameLen;

		memcpy(pcCode, pcCodeLine, nCodeLen);
	}

	size_t nCodePos = vecCode.size();
	vecCode.resize(vecCode.size() + sizeof(int));

	int* piCode = (int*) &vecCode[nCodePos];
	piCode[0] = 0;

	return true;
}
