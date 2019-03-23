////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLLighting.cpp
//
// summary:   Implements the ogl lighting class
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

// OGLLight.cpp: Implementierung der Klasse COGLLighting.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OGLLighting.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLLighting::COGLLighting()
{
	m_sTypeName = "Lighting";

	Reset();

	m_bIsEnabled = true;
}

COGLLighting::COGLLighting(const COGLLighting& rLight)
{
	m_sTypeName = "Lighting";

	*this = rLight;
}


COGLLighting& COGLLighting::operator= (const COGLLighting& rLight)
{
	m_bIsEnabled = rLight.m_bIsEnabled;

	return *this;
}


//////////////////////////////////////////////////////////////////////
/// Reset

void COGLLighting::Reset()
{
	m_bIsEnabled = true;
}

//////////////////////////////////////////////////////////////////////
// Apply
bool COGLLighting::Apply( COGLBaseElement::EApplyMode eMode, 
					  COGLBaseElement::SApplyData &rData)
{
	if ( eMode == COGLBaseElement::PICK )
		return true;

	if (m_bIsEnabled)
	{
		glEnable( GL_LIGHTING );
	}
	else
	{
		glDisable( GL_LIGHTING );
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Apply

bool COGLLighting::Apply()
{
	if (m_bIsEnabled)
	{
		glEnable( GL_LIGHTING );
	}
	else
	{
		glDisable( GL_LIGHTING );
	}

	return true;
}

