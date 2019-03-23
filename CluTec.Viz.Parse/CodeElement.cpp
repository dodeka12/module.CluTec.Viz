////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      CodeElement.cpp
//
// summary:   Implements the code element class
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
#include "CodeElement.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCodeElement::CCodeElement()
{

}

CCodeElement::~CCodeElement()
{

}


//////////////////////////////////////////////////////////////////////
// Generates XML code of element. Needs to implement serialization of additional
// variables in derived classes.

bool CCodeElement::Serialize( CXMLTree &xmlTree )
{
	CXMLElement xmlEl;

	xmlEl.SetName( "code" );
	xmlEl.AddProp( "cls", "CodeElement" );
	xmlEl.AddProp( "name", m_csName.Str() );

	xmlTree.AddElement( xmlEl );

	return true;
}

