////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      OCObjSepSym.cpp
//
// summary:   Implements the oc object separator symbol class
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

// OCObjSepSym.cpp: Implementierung der Klasse COCObjSepSym.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OCObjSepSym.h"
#include "ParseBase.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COCObjSepSym::COCObjSepSym()
{
	m_csIDSymbolSpace = " \t";
}

COCObjSepSym::~COCObjSepSym()
{

}


//////////////////////////////////////////////////////////////////////
/// Parse
///
/// Object separation symbol is typically ' '.
/// Simply advance a single step and return.

bool COCObjSepSym::Parse(CParseBase& rParse, CCodeElementList& rCodeList)
{
	rParse.AddToCurPos(1);

	return true;
}

