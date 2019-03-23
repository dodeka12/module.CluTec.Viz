////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLTranslation.cpp
//
// summary:   Implements the ogl translation class
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

// OGLTranslation.cpp: Implementierung der Klasse COGLTranslation.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OGLTranslation.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLTranslation::COGLTranslation()
{
	m_sTypeName = "Translation";

	Reset();
}

COGLTranslation::COGLTranslation( float fX, float fY, float fZ )
{
	m_sTypeName = "Translation";

	Set( fX, fY, fZ );
	m_bWindowCoords = false;
}

COGLTranslation::COGLTranslation(const COGLTranslation& rTrans )
{
	m_sTypeName = "Translation";

	*this = rTrans;
}

COGLTranslation& COGLTranslation::operator= (const COGLTranslation& rTrans)
{
	m_fX = rTrans.m_fX;
	m_fY = rTrans.m_fY;
	m_fZ = rTrans.m_fZ;

	m_bWindowCoords = rTrans.m_bWindowCoords;

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Reset

void COGLTranslation::Reset()
{	
	m_fX = GLfloat( 0 );
	m_fY = GLfloat( 0 );
	m_fZ = GLfloat( 0 );

	m_bWindowCoords = false;
}

//////////////////////////////////////////////////////////////////////
/// Set Translation

void COGLTranslation::Set( float fX, float fY, float fZ )
{
	m_fX = GLfloat( fX );
	m_fY = GLfloat( fY );
	m_fZ = GLfloat( fZ );
}

//////////////////////////////////////////////////////////////////////
/// Apply

bool COGLTranslation::Apply(COGLBaseElement::EApplyMode eMode, 
							COGLBaseElement::SApplyData &rData)
{
	if ( !m_bWindowCoords )
	{
		glTranslatef( m_fX, m_fY, m_fZ );
	}
	else
	{
		glTranslatef( m_fX, m_fY, m_fZ );
	}

	return true;
}
