////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      OCOpComma.cpp
//
// summary:   Implements the oc operation comma class
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

// OCOpComma.cpp: Implementierung der Klasse COCOpComma.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OCOpComma.h"
#include "ParseBase.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COCOpComma::COCOpComma()
{
	m_csIDSymbolSpace = ",";
}

COCOpComma::~COCOpComma()
{

}


//////////////////////////////////////////////////////////////////////
/// Parse Loop

bool COCOpComma::Parse(CParseBase& rParse, CCodeElementList& rCode)
{
	// Go past comma
	rParse.AddToCurPos(1);

	// Notify higher elements of comma
	rParse.SetError(CPB_COMMA);
	return false;
}


