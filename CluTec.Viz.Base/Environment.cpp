////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      Environment.cpp
//
// summary:   Implements the environment class
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

#include "Environment.h"
#include "CluTec.Base/Exception.h"
#include "CluTec.System/FilePath.h"

#include "direct.h"

// To check for existence of file
#include <fstream>

#ifdef WIN32
#include "Shlwapi.h"
#include "shlobj.h"
#else
#endif

namespace Clu
{
	namespace Viz
	{

#pragma region Local
		

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>	Gets known folder path. </summary>
		///
		/// <remarks>	Perwass,. </remarks>
		///
		/// <param name="xFolderID">	Identifier for the folder. </param>
		///
		/// <returns>	The known folder path. </returns>
		////////////////////////////////////////////////////////////////////////////////////////////////////

		std::string GetKnownFolderPath(const KNOWNFOLDERID& xFolderID)
		{
			wchar_t* pwcPath = nullptr;

			HRESULT hr = SHGetKnownFolderPath(xFolderID, 0, nullptr, &pwcPath);
			if (hr != S_OK)
			{
				return std::string("");
			}

			char pcPath[MAX_PATH];

			size_t nNumConverted;
			wcstombs_s(&nNumConverted, pcPath, (size_t)MAX_PATH, (const wchar_t*)pwcPath, (size_t)MAX_PATH);
			CoTaskMemFree(static_cast<void*>(pwcPath));

			return std::string(pcPath);
		}


		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>	Gets DLL folder path. </summary>
		///
		/// <remarks>	Perwass,. </remarks>
		///
		/// <returns>	The DLL folder path. </returns>
		////////////////////////////////////////////////////////////////////////////////////////////////////

		std::string GetDllFolderPath()
		{
			// Get the handle to the DLL that has linked in this method
			HMODULE hModule = nullptr;
			if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
				(LPCSTR)&GetDllFolderPath, &hModule))
			{
				return std::string("");
			}

			// Get the location of the DLL
			// Since all executables lies in the same directory we can get the debug shared directory
			char pcFilename[MAX_PATH];
			if (GetModuleFileNameA(hModule, pcFilename, MAX_PATH) == 0)
			{
				return std::string("");
			}

			// Remove file name from path
			std::string sFilename = std::string(pcFilename);
			std::string sPath = sFilename.substr(0, sFilename.find_last_of("\\/"));

			return sPath;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>	Gets environment root dir variable. </summary>
		///
		/// <returns>	The environment root dir variable. </returns>
		////////////////////////////////////////////////////////////////////////////////////////////////////

		std::string GetEnvRootDirVar()
		{
			static const unsigned uBufSize = 4096;
			char pcEnvRootDir[uBufSize];

			DWORD dwRet = GetEnvironmentVariableA(CLUVIZ_ENV_ROOT_DIR_NAME, pcEnvRootDir, uBufSize);
			if (dwRet == 0)
			{
				return std::string("");
			}

			return std::string(pcEnvRootDir);
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>	Gets path in RTM case. </summary>
		///
		/// <remarks>	Perwass,. </remarks>
		///
		/// <param name="ePath">	Full pathname of the file. </param>
		///
		/// <returns>	The path rtm. </returns>
		////////////////////////////////////////////////////////////////////////////////////////////////////

		std::string _GetPathRTM(EEnvPath ePath)
		{
			std::string sPath;

			switch (ePath)
			{
			case Clu::Viz::EEnvPath::Root:
				sPath = GetEnvRootDirVar();
				break;
			case Clu::Viz::EEnvPath::Licence:
				sPath = _GetPathRTM(EEnvPath::Root) + R"(\licence)";
				break;
			case Clu::Viz::EEnvPath::Plugin:
				sPath = _GetPathRTM(EEnvPath::Binary);
				break;
			case Clu::Viz::EEnvPath::Binary:
				sPath = GetDllFolderPath();
				break;
			case Clu::Viz::EEnvPath::Script_Root:
				sPath = _GetPathRTM(EEnvPath::Root) + R"(\script)";
				break;
			case Clu::Viz::EEnvPath::Script_Global:
				sPath = _GetPathRTM(EEnvPath::Script_Root) + R"(\_global)";
				break;
			case Clu::Viz::EEnvPath::Script_Example:
				sPath = _GetPathRTM(EEnvPath::Script_Root) + R"(\example)";
				break;
			default:
				break;
			}

			return sPath;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>	Gets a path in Debug or Relase case. </summary>
		///
		/// <remarks>	Perwass,. </remarks>
		///
		/// <param name="ePath">	Full pathname of the file. </param>
		///
		/// <returns>	The path. </returns>
		////////////////////////////////////////////////////////////////////////////////////////////////////

		std::string _GetPath(EEnvPath ePath)
		{
			std::string sPath;

			switch (ePath)
			{
			case Clu::Viz::EEnvPath::Root:
				sPath = _GetPath(EEnvPath::Binary) + R"(\..\..\..\CluTec.Viz.)" CLU_MODULE_VER_SHORT_STR R"(\_env)";
				break;
			case Clu::Viz::EEnvPath::Licence:
				sPath = _GetPath(EEnvPath::Root) + R"(\licence)";
				break;
			case Clu::Viz::EEnvPath::Plugin:
				sPath = _GetPath(EEnvPath::Binary);
				break;
			case Clu::Viz::EEnvPath::Binary:
				sPath = GetDllFolderPath();
				break;
			case Clu::Viz::EEnvPath::Script_Root:
				sPath = _GetPath(EEnvPath::Root) + R"(\script)";
				break;
			case Clu::Viz::EEnvPath::Script_Global:
				sPath = _GetPath(EEnvPath::Script_Root) + R"(\_global)";
				break;
			case Clu::Viz::EEnvPath::Script_Example:
				sPath = _GetPath(EEnvPath::Script_Root) + R"(\example)";
				break;
			default:
				break;
			}

			return sPath;
		}

#pragma endregion

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>	Gets a path. </summary>
		///
		/// <remarks>	Perwass,. </remarks>
		///
		/// <param name="ePath">	Full pathname of the file. </param>
		///
		/// <returns>	The path. </returns>
		////////////////////////////////////////////////////////////////////////////////////////////////////

		std::string CEnvironment::GetPath(EEnvPath ePath)
		{

#ifdef CLU_RTM
			return Clu::CFilePath::Normalize(_GetPathRTM(ePath));
#else
			return Clu::CFilePath::Normalize(_GetPath(ePath));
#endif
		}


		//////////////////////////////////////////////////////////////////////////
		std::string CEnvironment::CheckSubfolderForFile(const std::string& sScriptName, const std::string& sPath, const std::string& sFile)
		{
			// If path contains / or \ return unchanged
			if ((sFile.find_first_of("/\\") != std::string::npos))
			{
				return sFile;
			}

			// Otherwise check if file can be found - if not try subdirectory named like the script name without extension
			std::string sRet = sFile;
			char pcCurPath[MAX_PATH];

			_getcwd(pcCurPath, MAX_PATH);
			_chdir(sPath.c_str());

			std::ifstream fileStream(sFile.c_str());

			if (fileStream.good())
			{
				sRet = sFile;
			}
			else
			{
				std::string sTemp = CFilePath::ConcatPaths(sScriptName, sFile);
				std::ifstream fileStreamDeeper(sTemp.c_str());
				if (fileStreamDeeper.good())
				{
					// good
					sRet = sTemp;
				}
				else
				{
					// fail - nothing to be done
				}
			}

			_chdir(pcCurPath);
			return sRet;
		}

		//////////////////////////////////////////////////////////////////////////
		bool CEnvironment::FindFileInPathOrSubfolder(std::string& sResultPath, const std::string& sPath, const std::string& sSubfolder, const std::string& sFile)
		{
			bool bFound = false;
			sResultPath = "";

			// If path contains / or \ return unchanged
			if ((sFile.find_first_of("/\\") != std::string::npos))
			{
				return false;
			}

			WIN32_FIND_DATAA xFindData;
			HANDLE hFind = INVALID_HANDLE_VALUE;

			std::string sNormPath = Clu::CFilePath::Normalize(sPath);
			std::string sTestFile = sNormPath;
			CFilePath::AddToPath(sTestFile, sFile);

			hFind = FindFirstFileA(sTestFile.c_str(), &xFindData);
			if (hFind == INVALID_HANDLE_VALUE)
			{
				sTestFile = sNormPath;
				CFilePath::AddToPath(sTestFile, sSubfolder);
				CFilePath::AddToPath(sTestFile, sFile);

				hFind = FindFirstFileA(sTestFile.c_str(), &xFindData);
				if (hFind != INVALID_HANDLE_VALUE)
				{
					FindClose(hFind);
					sResultPath = sTestFile;
					bFound = true;
				}
			}
			else
			{
				FindClose(hFind);
				sResultPath = sTestFile;
				bFound = true;
			}

			return bFound;
		}

		//////////////////////////////////////////////////////////////////////////
		bool CEnvironment::FindFileInAllSubfolders(std::string& sResultPath, const std::string& sPath, const std::string& sFile)
		{
			bool bFound = false;
			sResultPath = "";

			// If path contains / or \ return unchanged
			if ((sFile.find_first_of("/\\") != std::string::npos))
			{
				return false;
			}

			WIN32_FIND_DATAA xFindData;
			HANDLE hFind = INVALID_HANDLE_VALUE;

			std::string sNormPath = Clu::CFilePath::Normalize(sPath);
			std::string sTestFile = sNormPath;
			CFilePath::AddToPath(sTestFile, sFile);

			hFind = FindFirstFileA(sTestFile.c_str(), &xFindData);
			if (hFind == INVALID_HANDLE_VALUE)
			{
				std::string sSearchPath = CFilePath::ConcatPaths(sNormPath, "*");
				hFind = FindFirstFileA(sSearchPath.c_str(), &xFindData);
				if (hFind != INVALID_HANDLE_VALUE)
				{
					do
					{
						if (xFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
						{
							std::string sDir = xFindData.cFileName;
							if ((sDir != "") && (sDir != "."))
							{
								std::string sTestDir = CFilePath::ConcatPaths(sNormPath, xFindData.cFileName);
								if (FindFileInAllSubfolders(sResultPath, sTestDir, sFile))
								{
									bFound = true;
									break;
								}
							}
						}
					} while (FindNextFileA(hFind, &xFindData) != 0);

					FindClose(hFind);
				}
			}
			else
			{
				FindClose(hFind);
				sResultPath = sTestFile;
				bFound = true;
			}

			return bFound;
		}

		//////////////////////////////////////////////////////////////////////////
		bool CEnvironment::FindResourceFile(std::string& sResultPath, const std::string& sMainScriptPath, const std::string& sScriptName, const std::string& sFilename)
		{
			if (!CEnvironment::FindFileInPathOrSubfolder(sResultPath, sMainScriptPath, sScriptName, sFilename))
			{
				std::string sSubFolder = CFilePath::ConcatPaths(Clu::CFilePath::Normalize(sMainScriptPath), sScriptName);
				if (!CEnvironment::FindFileInAllSubfolders(sResultPath, sSubFolder, sFilename))
				{
					if (!CEnvironment::FindFileInPathOrSubfolder(sResultPath, sMainScriptPath, "_global", sFilename))
					{
						// if the the file is also not there, have a look in the Runtime Environment global script path
						if (!CEnvironment::FindFileInPathOrSubfolder(sResultPath, CEnvironment::GetPath(EEnvPath::Root), "_global", sFilename))
						{
							sResultPath = sFilename;
							return false;
						}
					}
				}
			}

			return true;
		}


	} // namespace Viz
} // namespace Clu

