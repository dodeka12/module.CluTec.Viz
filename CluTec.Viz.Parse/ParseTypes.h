////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      ParseTypes.h
//
// summary:   Declares the parse types class
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

#include <vector>
#include "CluTec.Viz.Base\CStrMem.h"

struct STextLine
{
	CStrMem csText;		// Preparsed text
	CStrMem csInputText;	// original input text
	CStrMem csFilename;	// Name of File from which line originates
	int iMainFilePos;	// Position in main file where include statement is found
	int iLine;			// Line in Text
	int iInputTextStartPos;	// Position in original input text where csInputText starts.
	std::vector<int> vecPos;
};
