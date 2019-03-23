////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLLatexText.cpp
//
// summary:   Implements the ogl latex text class
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
//#include "CvFltk\glut.h"
#include "OGLLatexText.h"

#ifdef _GNUCPP3_
	#include <unistd.h>
	#define _getcwd getcwd
	#define _chdir chdir
#else
	#include <direct.h>
	#include <io.h>
#endif

#ifdef WIN32
	#include <ShellApi.h>
#endif

COGLLatexText::COGLLatexText(void)
{
	m_sScriptPath  = "./";
	m_sScriptName  = "Unnamed";
	m_bRenderLatex = true;
}

COGLLatexText::~COGLLatexText(void)
{
}

/////////////////////////////////////////////////////////////////////
// Get Ghostscript Path
//
// This function reads the registry under Win32 to get
// the path to AFPL Ghostscript which is needed to transform
// postscript files to bitmaps.
// Path is written into member variable m_sGSPath.
// If ghostscript is not installed the path variable is reset and
// the function returns false.

bool COGLLatexText::GetGhostscriptPath()
{
	// Reset Ghostscript Path
	m_sGSPath  = "";
	m_sGSError = "";

	#ifdef WIN32
		HKEY hKey, hGSKey = 0, hSubKey;	//, hSubKey2;
		char pcPath[1024];
		char pcSubKeyName[80];
		DWORD dwPathLen = 1024, dwSubKeyLen = 80;
		DWORD dwAccess;
		LONG lError;
		FILETIME ftLastWriteTime;

		// Check for Ghostscript in 32bit and 64bit registry
		for (int i = 0; i < 2; ++i)
		{
			if (i == 0)
			{
				dwAccess = KEY_WOW64_32KEY;
			}
			else
			{
				dwAccess = KEY_WOW64_64KEY;
			}

			if ((lError = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
						 TEXT("SOFTWARE"),	//\\GPL Ghostscript"),
						 0, dwAccess | KEY_ENUMERATE_SUB_KEYS, &hKey)) != ERROR_SUCCESS)
			{
				m_sGSError = "Error accessing registry to check for Ghotscript.\n";
				return false;
			}

			int iKeyIdx = 0;

			// Get the name of the first subkey, which should be
			// the latest version of ghostscript installed.
			while ((lError = RegEnumKeyEx(hKey,		// handle to key to enumerate
						iKeyIdx,// subkey index
						pcSubKeyName,	// subkey name
						&dwSubKeyLen,	// size of subkey buffer
						0,	// reserved
						0,	// class string buffer
						0,	// size of class string buffer
						&ftLastWriteTime// last write time
						)) == ERROR_SUCCESS)
			{
				if (strstr(pcSubKeyName, "Ghostscript") != 0)
				{
					// Now enter the subkey we have found
					if (RegOpenKeyEx(hKey,
								pcSubKeyName,
								0, dwAccess | KEY_ENUMERATE_SUB_KEYS, &hGSKey) == ERROR_SUCCESS)
					{
						break;
					}
				}

				dwSubKeyLen = 80;
				++iKeyIdx;
			}

			if (lError == ERROR_SUCCESS)
			{
				break;
			}
		}

		if (lError != ERROR_SUCCESS)
		{
			m_sGSError = "Ghostscript does not seem to be installed.\n"
					 "You need to install Ghostscript v8.13 or higher\n"
					 "to render Latex text images.";
			return false;
		}

		dwSubKeyLen = 80;

		// Now get first subkey of Ghostscript key
		if ((lError = RegEnumKeyEx(hGSKey,		// handle to key to enumerate
					 0,				// subkey index
					 pcSubKeyName,				// subkey name
					 &dwSubKeyLen,			// size of subkey buffer
					 0,				// reserved
					 0,				// class string buffer
					 0,				// size of class string buffer
					 &ftLastWriteTime		// last write time
					 )) != ERROR_SUCCESS)
		{
			m_sGSError = "Ghostscript seems to be installed,\n"
					 "but registry appears to be corrupted.\n"
					 "Please reinstall Ghostscript.";
			return false;
		}

		// Now enter the subkey we have found
		if (RegOpenKeyEx(hGSKey,
					pcSubKeyName,
					0, dwAccess | KEY_QUERY_VALUE, &hSubKey) != ERROR_SUCCESS)
		{
			m_sGSError = "Internal registry error while looking for Ghostscript.\n"
					 "Please reinstall Ghostscript v8.13 or higher.\n";
			return false;
		}

		// Now let's get the path to the Ghostscript DLL
		if (RegQueryValueEx(hSubKey, "GS_DLL", NULL, NULL,
					(LPBYTE) pcPath, &dwPathLen) != ERROR_SUCCESS)
		{
			m_sGSError = "Internal registry error while looking for Ghostscript path.\n"
					 "Please reinstall Ghostscript v8.13 or higher.\n";
			return false;
		}

		// Close registry keys again
		RegCloseKey(hSubKey);
		RegCloseKey(hGSKey);
		RegCloseKey(hKey);

		// Get last backslash
		char* pcPos = strrchr(pcPath, '\\');

		// If none was found then there's a problem
		if (!pcPos)
		{
			m_sGSError = "Path to ghostscript seems to be invalid.\n"
					 "Please reinstall Ghostscript v8.13 or higher.\n";
			return false;
		}

		// Terminate path behind the last backslash
		pcPos[1] = 0;

		m_sGSPath = pcPath;

		return true;

	#else	// Linux or some other system
		char pcCmd[500];
		char pcLine[300];

		struct stat FileStat;
		char* pcInfo =  "\nPlease install GNU Ghostscript v7.07 or higher,\n"
				"or, even better, AFPL Ghostscript v8.13 or higher.\n"
				"The latter also supports anti-aliasing of text.\n"
				"You can download both from www.ghostscript.com.\n";

		// Assume here that gs is in PATH
		m_sGSPath = ".";

		// Now check that gs is installed at all
		remove("gs_version.txt");

		sprintf(pcCmd, "gs --version > gs_version.txt");

		if (system(pcCmd) == -1)
		{
			m_sGSError  = "Cannot execute 'gs' command.\n";
			m_sGSError += pcInfo;
			return false;
		}

		// Check whether version text file exists
		if ((stat("gs_version.txt", &FileStat) != 0) || (FileStat.st_size == 0))
		{
			m_sGSError  = "Ghostscript does not seem to be installed.\n";
			m_sGSError += pcInfo;
			return false;
		}

		// Now check the version
		ifstream zLog;

		zLog.clear();
		zLog.open("gs_version.txt");
		if (zLog.is_open())
		{
			int iMajor, iMinor;
			zLog.getline(pcLine, 299, 10);
			zLog.close();

			sscanf(pcLine, "%d.%02d", &iMajor, &iMinor);
			remove("gs_version.txt");

			// Now find out whether AFPL or GNU
			sprintf(pcCmd, "gs --help > gs_help.txt");

			if (system(pcCmd) == -1)
			{
				m_sGSError  = "Cannot execute 'gs --help' command.\n";
				m_sGSError += pcInfo;
				return false;
			}

			zLog.clear();
			zLog.open("gs_help.txt");
			if (zLog.is_open())
			{
				zLog.getline(pcLine, 299, 10);
				zLog.close();
				remove("gs_help.txt");

				//printf("%s\n",pcLine);

				// Now check version for given type
				if (strstr(pcLine, "AFPL") != 0)
				{
					//printf("Found AFPL Ghostscript version %d.%02d.\n", iMajor, iMinor);

					if ((iMajor < 8) ||
						((iMajor == 8) && (iMinor < 11)))
					{
						sprintf(pcLine, "Found AFPL Ghostscript version %d.%02d.\n"
								"Need version 8.13 or higher for latex rendering.\n",
								iMajor, iMinor);
						m_sGSError  = pcLine;
						m_sGSError += pcInfo;
						return false;
					}
				}
				else if (strstr(pcLine, "GNU") != 0)
				{
					//printf("Found GNU Ghostscript version %d.%02d.\n", iMajor, iMinor);

					if ((iMajor < 7) ||
						((iMajor == 7) && (iMinor < 7)))
					{
						sprintf(pcLine, "Found GNU Ghostscript version %d.%02d.\n"
								"Need version 7.07 or higher for latex rendering.\n"
								"You may want to install AFPL Ghostscript, since\n"
								"this allows for anti-aliased text rendering.\n",
								iMajor, iMinor);
						m_sGSError  = pcLine;
						m_sGSError += pcInfo;
						return false;
					}
				}
				else if (strstr(pcLine, "ESP") != 0)
				{
					//printf("Found GNU Ghostscript version %d.%02d.\n", iMajor, iMinor);

					if ((iMajor < 8) ||
						((iMajor == 8) && (iMinor < 13)))
					{
						sprintf(pcLine, "Found ESP Ghostscript version %d.%02d.\n"
								"Need version 8.13 or higher for latex rendering.\n"
								"You may want to install AFPL Ghostscript, since\n"
								"this allows for anti-aliased text rendering.\n",
								iMajor, iMinor);
						m_sGSError  = pcLine;
						m_sGSError += pcInfo;
						return false;
					}
				}
				else
				{
					//printf("Found unknown Ghostscript version %d.%02d.\n", iMajor, iMinor);

					sprintf(pcLine, "Found unknown Ghostscript type, version %d.%02d.\n"
							"Need AFPL or ESP version 8.13 or GNU version 7.07 or higher\n"
							"for latex rendering.\n",
							iMajor, iMinor);
					m_sGSError  = pcLine;
					m_sGSError += pcInfo;
					return false;
				}
			}
			else
			{
				m_sGSError  = "Cannot read Ghostscript help.\n";
				m_sGSError += pcInfo;
				return false;
			}
		}
		else
		{
			m_sGSError  = "Cannot read Ghostscript version.\n";
			m_sGSError += pcInfo;
			return false;
		}

		return true;
	#endif
}

/////////////////////////////////////////////////////////////////////
/// Get Scale for a particular Mag.Step

float COGLLatexText::GetMagStepScale(int iMagStep)
{
	float fMagStep;

	fMagStep = float(pow(1.2, double(iMagStep) * 0.5));

	fMagStep = float(floor(double(fMagStep) * 1e3) * 1e-3);

	return fMagStep;
}

/////////////////////////////////////////////////////////////////////
/// Reset In Use Flags

void COGLLatexText::ResetInUseFlags()
{
	TLatexMap::iterator it_Latex, it_End;

	it_End = m_mapLatex.end();
	for (it_Latex = m_mapLatex.begin(); it_Latex != it_End; ++it_Latex)
	{
		it_Latex->second.bInUse = false;
	}
}

/////////////////////////////////////////////////////////////////////
/// Prune Map

void COGLLatexText::PruneMap()
{
	TLatexMap::iterator it_Latex, it_E;

	it_Latex = m_mapLatex.begin();
	while (it_Latex != m_mapLatex.end())
	{
		if (it_Latex->second.bInUse == false)
		{
			m_mapLatex.erase(it_Latex);
			it_Latex = m_mapLatex.begin();
		}
		else
		{
			++it_Latex;
		}
	}
}

/////////////////////////////////////////////////////////////////////
/// Render Latex Bitmap

bool COGLLatexText::RenderLatex(SLatexText& rLatex)
{
	char pcCmd[500], pcCurPath[500];
	char pcLine[300];

	ofstream zLatex, zBatch;
	ifstream zLog, zEnd;
	string sFilename;
	string sBMPFilename  = rLatex.sBMPName;
	bool bHasBMPFilename = (sBMPFilename.length() > 0 ? true : false);
	bool bRenderError    = false;
	bool bLatexError     = false;

	_getcwd(pcCurPath, 499);
	_chdir(m_sScriptPath.c_str());

	if (bHasBMPFilename)
	{
		// .rlb stands for 'rendered latex bitmap'
		// I am using png instead now.
		sBMPFilename        = m_sScriptPath + m_sScriptName + "_" + sBMPFilename + ".png";
		rLatex.sBMPFilename = sBMPFilename;
	}
	else
	{
		sBMPFilename        = m_sScriptPath + "clucalc_dummy.png";
		rLatex.sBMPFilename = "";
	}

	try
	{
		m_sRenderError = "";
		if (m_bRenderLatex)
		{
			// Open log file to delete its contents
			zLatex.open("clucalc_tex.log");
			zLatex << "No Errors\n";
			zLatex.close();

			if (rLatex.bIsFile)
			{
				//sFilename = rLatex.sLatexFilename;
				sFilename = "clucalc_dummy.tex";
				#ifdef WIN32
					sprintf_s(pcCmd, 500, "copy \"%s\" clucalc_dummy.tex", rLatex.sLatexFilename.c_str());
				#else
					sprintf(pcCmd, "cp \"%s\" clucalc_dummy.tex", rLatex.sLatexFilename.c_str());
				#endif
				system(pcCmd);
			}
			else
			{
				sFilename = "clucalc_dummy.tex";

				// Create Latex File
				zLatex.open(sFilename.c_str());
				if (!zLatex.is_open())
				{
					throw(CRenderError("Cannot create LaTeX file"));
				}

				if ((rLatex.sText[0] == '\\') && (rLatex.sText.find("\\plain:") != string::npos))
				{
					zLatex << &(rLatex.sText.c_str()[7]);
				}
				else
				{
					char pcFontSize[200];
					sprintf_s(pcFontSize, "\\fontsize{%fpt}{1em}\\selectfont\n", rLatex.fScale);

					zLatex <<
						"\\documentclass{article}\n"
						"\\usepackage{latexsym}\n"
						"\\usepackage{amsmath}\n"
						"\\usepackage{amsfonts}\n"
						"\\usepackage{amssymb}\n"
						"\\usepackage{fontspec}\n"
						"\\setmainfont{" << rLatex.sMainFont << "}\n"
										"\\begin{document}\n"
										"\\pagestyle{empty}\n";
					zLatex << pcFontSize;
					zLatex << rLatex.sText;
					zLatex << "\n\\end{document}\n";
				}

				zLatex.close();
			}

			// If no path for ghostscript currently exists, then check
			// whether ghostscript is installed at all, and get its path.
			if ((m_sGSPath.size() == 0) && !GetGhostscriptPath())
			{
				throw(CRenderError(m_sGSError.c_str()));
			}

			// Call latex
			#ifdef WIN32

				remove("clucalc_end.txt");
				remove("clucalc_gs.log");
				//remove("clucalc_dummy.dvi");
				remove("clucalc_dummy.pdf");
				remove(sBMPFilename.c_str());

				zBatch.open("clucalc_latex.bat");
				if (!zBatch.is_open())
				{
					throw(CRenderError("Cannot create batch file"));
				}

				sprintf_s(pcCmd, 500, "lualatex -interaction=nonstopmode "
							  "clucalc_dummy.tex > clucalc_tex.log");
				zBatch << pcCmd << "\n";

				sprintf_s(pcCmd, 500, "\"%sgswin32c\" -sDEVICE=pnggray -sOutputFile=\"%s\" "
							  "-dNOPAUSE -dBATCH -dQUIET -dTextAlphaBits=4 "
							  "-r120 clucalc_dummy.pdf > clucalc_gs.log",
					m_sGSPath.c_str(), sBMPFilename.c_str());
				zBatch << pcCmd << "\n";

				zBatch << "echo finished > clucalc_end.txt\n";

				zBatch.close();

				HINSTANCE hInstance = ShellExecute(NULL,
					"open",
					"cmd.exe",
					"/C clucalc_latex.bat",
					m_sScriptPath.c_str(),
					SW_HIDE);		//SHOWMINIMIZED);

				if ((size_t) hInstance <= 32)
				{
					throw(CRenderError("Cannot execute LaTeX command"));
				}

				// Wait for batch file to finish execution
				while (remove("clucalc_end.txt") < 0)
					::Sleep(40);

				// Open Latex log file
				zLog.open("clucalc_tex.log");
				m_sRenderError = "";
				while (!zLog.eof())
				{
					zLog.getline(pcLine, 299);
					m_sRenderError += pcLine;
					m_sRenderError += "\n";
					if (pcLine[0] == '!')
					{
						bLatexError = true;
					}
				}

				zLog.close();

				if (bLatexError)
				{
					throw CRenderError();
				}

				m_sRenderError = "";

				// Check whether ps file exists
				if (_access("clucalc_dummy.pdf", 0x04 /*Read Access*/) == -1)
				{
					bLatexError = true;
					throw(CRenderError(
								  "The program 'dvips' seems to have produced an error.\n"
								  "Please make sure you are using dvips version 5.90a or higher.\n"
								  "An appropriate version comes with the MikTex Latex distribution.\n"));
				}

				// Check whether bmp file exists
				// if not return with error.
				if (_access(sBMPFilename.c_str(), 0x04 /*Read Access*/) == -1)
				{
					zLog.clear();
					zLog.open("clucalc_gs.log");
					if (zLog.is_open())
					{
						m_sRenderError = "";
						while (!zLog.eof())
						{
							zLog.getline(pcLine, 299, 10);
							m_sRenderError += pcLine;
							m_sRenderError += "\n";
							if (strstr(pcLine, "Error") != 0)
							{
								bLatexError = true;
							}

							if ((pcLine[0] == 0) && zLog.fail())
							{
								break;
							}
						}

						zLog.close();

						if (bLatexError)
						{
							m_sRenderError = "Ghostscript generated the following error:\n\n"
									 + m_sRenderError;
							throw(CRenderError());
						}
					}

					bLatexError = true;
					throw(CRenderError(
								  "Either the program 'gswin32c.exe' could not be found or you are\n"
								  "using special characters in the path or filename, like the German\n"
								  "Umlaute. If the latter is the case, please rename the path or file.\n"
								  "If this is not the case then download the latest version of Ghostscript from\n"
								  " http://www.ghostscript.com/.\n"
								  "Also try to execute the automatically generated file 'clucalc_latex.bat' \n"
								  "in a DOS shell. You may be given some error messages that CLUCalc does not display correctly.\n"
								  "\n"));
				}

			#else	// Linux or something else
				struct stat FileStat;

				remove("clucalc_end.txt");
				remove("clucalc_gs.log");
				remove("clucalc_dummy.dvi");
				remove("clucalc_dummy.ps");
				remove(sBMPFilename.c_str());

				sprintf(pcCmd, "latex -interaction=nonstopmode "
						   "clucalc_dummy.tex > clucalc_tex.log",
					sFilename.c_str());

				if (system(pcCmd) == -1)
				{
					throw(CRenderError("Cannot execute LaTeX commands"));
				}

				// Check whether log file exists
				if ((stat("clucalc_tex.log", &FileStat) != 0) || (FileStat.st_size == 0))
				{
					bLatexError = true;
					throw(CRenderError(
								  "The program 'latex' does not seem to be installed.\n"
								  "You need to install LaTeX before you can use the\n"
								  "function DrawLatex()\n"));
				}

				// Open Latex log file
				zLog.open("clucalc_tex.log");
				m_sRenderError = "";
				while (!zLog.eof())
				{
					zLog.getline(pcLine, 299);
					m_sRenderError += pcLine;
					m_sRenderError += "\n";
					if (pcLine[0] == '!')
					{
						bLatexError = true;
					}
				}

				zLog.close();

				if (bLatexError)
				{
					throw CRenderError();
				}

				m_sRenderError = "";

				sprintf(pcCmd, "dvips '-q*' '-E*' -D 72 -x %f -oclucalc_dummy.ps clucalc_dummy.dvi",
					floor(rLatex.fScale * 1000.0f + 0.5f));

				//printf("%s\n", pcCmd);

				if (system(pcCmd) == -1)
				{
					throw(CRenderError("Cannot execute 'dvips' command"));
				}

				// Check whether ps file exists
				if (access("clucalc_dummy.ps", R_OK /*Read Access*/) == -1)
				{
					bLatexError = true;
					throw(CRenderError(
								  "The program 'dvips' seems to have produced an error,\n"
								  "or may not be installed.\n"
								  "Please make sure you are using dvips version 5.90a or higher.\n"
								  "An appropriate version comes with the MikTex Latex distribution.\n"));
				}

				sprintf(pcCmd, "gs -sDEVICE=pnggray -sOutputFile=\"%s\" "
						   "-dNOPAUSE -dBATCH -dQUIET -dEPSCrop -r72 "
						   "-dTextAlphaBits=4 clucalc_dummy.ps > clucalc_gs.log",
					sBMPFilename.c_str());

				if (system(pcCmd) == -1)
				{
					throw(CRenderError("Cannot execute 'gs' command"));
				}

				// Check whether bmp file exists
				// if not return with error.
				if (access(sBMPFilename.c_str(), R_OK /*Read Access*/) == -1)
				{
					zLog.clear();
					zLog.open("clucalc_gs.log");
					if (zLog.is_open())
					{
						m_sRenderError = "";
						while (!zLog.eof())
						{
							zLog.getline(pcLine, 299, 10);
							m_sRenderError += pcLine;
							m_sRenderError += "\n";
							if (strstr(pcLine, "Error") != 0)
							{
								bLatexError = true;
							}

							if ((pcLine[0] == 0) && zLog.fail())
							{
								break;
							}
						}

						zLog.close();

						if (bLatexError)
						{
							m_sRenderError = "Ghostscript generated the following error:\n\n"
									 + m_sRenderError;
							throw(CRenderError(""));
						}
					}

					bLatexError = true;
					throw(CRenderError(
								  "The program 'gs' (ghostscript) could not be found!\n"
								  "This program is needed to transform '.ps' files into '.bmp' file,\n"
								  "which can be displayed by CLUCalc.\n"
								  "Please download the latest version of Ghostscript from\n"
								  "            http://www.ghostscript.com/\n"
								  "\n"
								  "If ghostscript is already installed make sure your 'PATH'\n"
								  "environment variable is set correctly"));
				}

			#endif

			Mem<int> mBoundingBox;
			COGLColor TransCol;
			COGLImage imgA, imgB;
			int iWidth, iHeight;
			COGLColor colClear;
			colClear.Set(0.0f, 0.0f, 0.0f, 0.0f);

			TransCol.Set(1.0f, 1.0f, 1.0f, 0.0f);

			imgA.SetFilename(sBMPFilename.c_str());
			if (!imgA.LoadImage(true))
			{
				return false;
			}

			imgA.ConvertType(CLUVIZ_IMG_RGBA, CLUVIZ_IMG_UNSIGNED_BYTE);
			imgA.GetBoundingBox(mBoundingBox, TransCol);

			imgB.SetFilename(sBMPFilename.c_str());
			imgB.ExtractSubImage(imgA, mBoundingBox[0], mBoundingBox[1], mBoundingBox[2], mBoundingBox[3]);
			imgB.MakeTransparent(COGLImage::/*ETransparencyMode::*/ FONT, TransCol);

			imgB.GetSize(iWidth, iHeight);
			imgB.ResizeCanvas(iWidth + 2, iHeight + 2, &colClear);

			imgB.SaveImage();

			rLatex.bIsRendered = true;
		}	// if m_bRenderLatex
	}	// try
	catch (const CRenderError& rEx)
	{
		bRenderError = true;
		if (rEx.m_sErrorText.size() > 0)
		{
			m_sRenderError = rEx.m_sErrorText;
		}
	}

	_chdir(pcCurPath);

	if (!bLatexError &&
		((!bRenderError && m_bRenderLatex) ||
		 (bRenderError && bHasBMPFilename && !m_bRenderLatex) ||
		 (!m_bRenderLatex && bHasBMPFilename))
		)
	{
		// Load Bitmap

		CImageReference& rImgRef = rLatex.ImageRef;

		if (!rImgRef.IsValid())
		{
			return false;
		}

		rLatex.oglColor.Set(1.0f, 1.0f, 1.0f, 0.0f);

		rImgRef->SetFilename(sBMPFilename.c_str());
		if (!rImgRef->LoadImage(true))
		{
			return false;
		}

		return true;
	}
	else
	{
		return false;
	}
}

/////////////////////////////////////////////////////////////////////
/// Add Latex Text

bool COGLLatexText::Add(const char* pcText, float fScale, const char* pcMainFont, COGLColor& rColor, CImageRepository& rImgRep, const char* pcBMPName)
{
	string sNewText = pcText;
	TLatexMap::iterator it_Latex;

	// Check whether text has already been rendered
	if ((it_Latex = m_mapLatex.find(sNewText)) == m_mapLatex.end())
	{
		// Create new entry in map
		SLatexText& rLatex = m_mapLatex[sNewText];

		// Create new image
		rLatex.ImageRef = rImgRep.New();

		rLatex.bIsRendered = false;
		rLatex.bInUse      = true;
		rLatex.fScale      = fScale;
		rLatex.sText       = sNewText;

		if (pcMainFont)
		{
			rLatex.sMainFont = pcMainFont;
		}
		else
		{
			rLatex.sMainFont = "Times New Roman";
		}

		if (pcBMPName)
		{
			rLatex.sBMPName = pcBMPName;
		}
		else
		{
			rLatex.sBMPName = "";
		}
		rLatex.sBMPFilename = "";
		rLatex.sScriptName  = m_sScriptName;
		rLatex.sScriptPath  = m_sScriptPath;

		if ((rLatex.sText[0] == '\\') && (rLatex.sText.find("\\file:") != string::npos))
		{
			rLatex.bIsFile        = true;
			rLatex.sLatexFilename = m_sScriptPath + &(rLatex.sText.c_str()[6]);
			if (_stat(rLatex.sLatexFilename.c_str(), &rLatex.FileInfo) == -1)
			{
				rLatex.bIsFile = false;
				rLatex.bInUse  = false;
				m_sRenderError = "Latex file '" + rLatex.sLatexFilename + "' not found.";
				return false;
			}
		}
		else
		{
			rLatex.bIsFile = false;
		}

		if (!RenderLatex(rLatex))
		{
			rLatex.bInUse = false;
			return false;
		}

		it_Latex = m_mapLatex.find(sNewText);
	}
	else
	{
		bool bRender       = false;
		SLatexText& rLatex = it_Latex->second;

		if (m_bRenderLatex && !rLatex.bIsRendered)
		{
			bRender = true;
		}

		// If text is to be rendered at different scale, then do it-
		if (((rLatex.fScale != fScale) || (rLatex.sMainFont != pcMainFont)) && m_bRenderLatex)
		{
			bRender            = true;
			rLatex.bIsRendered = false;
			rLatex.fScale      = fScale;
			rLatex.sMainFont   = pcMainFont;
		}

		bool bNewName     = false;
		bool bNewFilename = false;

		if ((!pcBMPName && (rLatex.sBMPName.size() > 0)) ||
			(pcBMPName && (rLatex.sBMPName != pcBMPName)))
		{
			bNewName = true;
		}

		if ((pcBMPName || (rLatex.sBMPName.size() > 0)) &&
			(		/*(m_sScriptName != rLatex.sScriptName) ||*/ /* Otherwise it copies unnecassarily */
				(m_sScriptPath != rLatex.sScriptPath)))
		{
			bNewFilename = true;
		}

		if (bNewName || bNewFilename)
		{
			if (m_bRenderLatex)
			{
				// Delete old file here ...
				if (rLatex.sBMPFilename.size() > 0)
				{
					remove(rLatex.sBMPFilename.c_str());
				}
			}
			else if (bNewFilename && pcBMPName && strlen(pcBMPName) &&
				 rLatex.sBMPFilename.size())
			{
				// Copy File
				string sBMPFilename;
				sBMPFilename = m_sScriptPath + m_sScriptName + "_" + pcBMPName + ".png";

				char pcCmd[500];
				char pcCopy[10];
				#ifdef WIN32
					strcpy_s(pcCopy, 10, "copy");
				#else
					strcpy(pcCopy, "cp");
				#endif
				sprintf_s(pcCmd, 500, "%s \"%s\" \"%s\"", pcCopy,
						rLatex.sBMPFilename.c_str(), sBMPFilename.c_str());
				system(pcCmd);
			}

			bRender             = true;
			rLatex.sBMPName     = (pcBMPName == 0 ? "" : pcBMPName);
			rLatex.sBMPFilename = "";
			rLatex.sScriptName  = m_sScriptName;
			rLatex.sScriptPath  = m_sScriptPath;
		}

		// Check whether file has been modified
		if (rLatex.bIsFile)
		{
			struct _stat FileInfo;
			if (_stat(rLatex.sLatexFilename.c_str(), &FileInfo) == -1)
			{
				rLatex.bIsFile = false;
				rLatex.bInUse  = false;
				m_sRenderError = "Latex file '" + rLatex.sLatexFilename + "' not found.";
				return false;
			}

			if (FileInfo.st_mtime != rLatex.FileInfo.st_mtime)
			{
				if (!m_bRenderLatex)
				{
					m_sRenderError = "Latex file '" + rLatex.sLatexFilename +
							 "' modified but Latex rendering switched off.";
					return false;
				}
				bRender = true;
			}
		}

		if (bRender)
		{
			if (!RenderLatex(it_Latex->second))
			{
				it_Latex->second.bInUse = false;
				return false;
			}
		}
	}

	SLatexText& rLatex = it_Latex->second;

	// If everything went to plan, then set flag that this text is being used.
	rLatex.bInUse = true;

	// Set Latex Image Color
	if (!(rLatex.oglColor == rColor))
	{
		rLatex.oglColor = rColor;
		rLatex.ImageRef->FlushRGB(rColor);
	}

	return true;
}

/////////////////////////////////////////////////////////////////////
// Get pointer to OGLBitmap for text, if text has been rendered.

CImageReference COGLLatexText::GetImageRef(const char* pcText)
{
	string sNewText = pcText;
	TLatexMap::iterator it_Latex;

	// Check whether text has already been rendered
	if ((it_Latex = m_mapLatex.find(sNewText)) == m_mapLatex.end())
	{
		// if not, then return NULL
		return CImageReference();
	}

	return it_Latex->second.ImageRef;
}

/////////////////////////////////////////////////////////////////////
/// Apply Text
// iMode alows different types of Apply calls.
// Arbitrary data may be passed by pvData.
/*
bool COGLLatexText::Apply(int iMode, void *pvData)
{
		if (pvData == 0)
		{
				TLatexMap::iterator it_Latex, it_End;

				it_End = m_mapLatex.end();
				for(it_Latex = m_mapLatex.begin(); it_Latex != it_End; ++it_Latex)
				{
						if (it_Latex->second.bInUse)
						{
								COGLColor rCurCol, rBMPCol;
								glGetFloatv(GL_CURRENT_COLOR, rCurCol.Data());
								rBMPCol = it_Latex->second.oglColor;

								if (rBMPCol[0] != rCurCol[0] ||
										rBMPCol[1] != rCurCol[1] ||
										rBMPCol[2] != rCurCol[2])
								{
//!					it_Latex->second.oglImage.FlushRGB(rCurCol);
										it_Latex->second.oglColor = rCurCol;
								}

//				it_Latex->second.oglImage.Apply();
						}
				}
		}
		else
		{
				string sText;
				sText = (char *) pvData;
				TLatexMap::iterator it_Latex;

				if ((it_Latex = m_mapLatex.find(sText)) != m_mapLatex.end())
				{
						COGLColor rCurCol, rBMPCol;
						glGetFloatv(GL_CURRENT_COLOR, rCurCol.Data());
						rBMPCol = it_Latex->second.oglColor;

						if (rBMPCol[0] != rCurCol[0] ||
								rBMPCol[1] != rCurCol[1] ||
								rBMPCol[2] != rCurCol[2])
						{
//!				it_Latex->second.oglImage.FlushRGB(rCurCol);
								it_Latex->second.oglColor = rCurCol;
						}

//			it_Latex->second.oglImage.Apply();
				}
		}

		return true;
}

*/

/////////////////////////////////////////////////////////////////////
// Get extension of Latex Bitmap in world coordinates
// for text given in pcText.
/*
bool COGLLatexText::GetWorldCoordDims(const char *pcText, float &fLeft, float &fRight,
														float &fBottom, float &fTop,
														float &fZ1, float &fZ2)
{
		if (!pcText)
				return false;

		string sText;
		sText = pcText;
		TLatexMap::iterator it_Latex;

		if ((it_Latex = m_mapLatex.find(sText)) != m_mapLatex.end() &&
				(it_Latex->second.bIsRendered || it_Latex->second.sBMPName.size()))
		{
//!		it_Latex->second.oglImage.GetWorldCoordDims(fLeft, fRight, fBottom, fTop, fZ1, fZ2);
				return true;
		}
		else
		{
				return false;
		}
}
*/
