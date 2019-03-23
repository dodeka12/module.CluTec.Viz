////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLBaseElement.cpp
//
// summary:   Implements the ogl base element class
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

// Class written by Vladimir Banarer, 3.12.2001.

// OGLBaseElement.cpp: Implementierung der Klasse COGLBaseElement.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "OGLBaseElement.h"
#include "OGLBEReference.h" 

//////////////////////////////////////////////////////////////////////

// Defines sm_uLastUID in external DLL
#include "CluTec.Viz.ImgRepo\CLUUID.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLBaseElement::COGLBaseElement()
{
	m_sTypeName = "BaseElement";

	m_uUID = ++::sm_uLastUID;
}

COGLBaseElement::COGLBaseElement(const string sName)
{
	m_sTypeName = "BaseElement";

	m_uUID = ++::sm_uLastUID;
	m_sName = sName;
}

COGLBaseElement::~COGLBaseElement()
{

}

COGLBaseElement::COGLBaseElement(const COGLBaseElement& rBaseElement)
{
	*this = rBaseElement;
}


bool COGLBaseElement::AddParent( COGLBaseElement* pParent )
{
	list<COGLBaseElement*>::iterator itEl, itEnd;

	itEnd = m_listParent.end();

	for ( itEl = m_listParent.begin();
		itEl != itEnd; 
		++itEl )
	{
		if ( *itEl == pParent )
			return true;
	}

	m_listParent.push_back( pParent );

	return true;
}


bool COGLBaseElement::RemoveParent( COGLBaseElement* pParent )
{
	list<COGLBaseElement*>::iterator itEl, itEnd;

	itEnd = m_listParent.end();

	for ( itEl = m_listParent.begin();
		itEl != itEnd; 
		++itEl )
	{
		if ( *itEl == pParent )
		{
			m_listParent.erase( itEl );
			return true;
		}
	}

	return false;
}
