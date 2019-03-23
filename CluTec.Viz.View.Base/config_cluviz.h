////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.View.Base
// file:      config_cluviz.h
//
// summary:   Declares the configuration cluviz class
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

#define STRINGIFY(X)	#X

#define CLUVIZ_VERSION				CLU_MODULE_VER_LONG_STR
#define CLUVIZ_VERSION_MAJOR		CLU_MODULE_VER_A
#define CLUVIZ_VERSION_MINOR		CLU_MODULE_VER_B
#define CLUVIZ_VERSION_REVISION		CLU_MODULE_VER_C
#define CLUVIZ_BUILD_DATE			__DATE__
#define CLUVIZ_BUILD_TIME			__TIME__

#define CLUVIZ_MAJOR_MINOR			CLU_MODULE_VER_SHORT_STR
  
#define CLUVIZ_REGKEY				"SOFTWARE\\CluTec\\CluVizEngine " CLU_MODULE_VER_SHORT_STR
#define CLUVIZ_FILETYPES_REGKEY		"SOFTWARE\\CluTec\\CluVizEngine " CLU_MODULE_VER_SHORT_STR "\\FileTypes"
