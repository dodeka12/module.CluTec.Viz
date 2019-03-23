////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      Environment.h
//
// summary:   Declares the environment class
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

#include <string>
#include "EnvPath.h"

#define CLUVIZ_ENV_ROOT_DIR_NAME "CLUVIZ_DIR_ENV_ROOT"


namespace Clu
{
	namespace Viz
	{
		class CEnvironment
		{
		public:
			static std::string GetPath(EEnvPath ePath);

			// Checks if the given path contains the file - tries sub folder and returns relative path if it is found there
			static std::string CheckSubfolderForFile(const std::string& sScriptName, const std::string& sPath, const std::string& sFile);

			static bool FindFileInPathOrSubfolder(std::string& sResultPath, const std::string& sPath, const std::string& sSubfolder, const std::string& sFile);

			// Checks if the given path contains the file - tries all sub folders and returns relative path if it is found there
			static bool FindFileInAllSubfolders(std::string& sResultPath, const std::string& sPath, const std::string& sFile);

			static bool FindResourceFile(std::string& sResultPath, const std::string& sMainScriptPath, const std::string& sScriptName, const std::string& sFilename);
		};
	}
}
