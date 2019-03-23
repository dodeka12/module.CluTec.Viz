////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLRotation.cpp
//
// summary:   Implements the ogl rotation class
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

// OGLRotation.cpp: Implementierung der Klasse COGLRotation.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OGLRotation.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLRotation::COGLRotation()
{
	m_sTypeName = "Rotation";

	Reset();
}

COGLRotation::COGLRotation( float fAngle, float fX, float fY, float fZ )
{
	m_sTypeName = "Rotation";

	Set( fAngle, fX, fY, fZ );
}

COGLRotation::COGLRotation(const COGLRotation& rRot)
{
	m_sTypeName = "Rotation";

	*this = rRot;
}

COGLRotation& COGLRotation::operator= (const COGLRotation& rRot)
{
	m_fAngle = rRot.m_fAngle;
	m_fX = rRot.m_fX;
	m_fY = rRot.m_fY;
	m_fZ = rRot.m_fZ;

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Reset

void COGLRotation::Reset()
{	
	m_fAngle = GLfloat( 0 );
	m_fX = GLfloat( 1 );
	m_fY = GLfloat( 0 );
	m_fZ = GLfloat( 0 );
}

//////////////////////////////////////////////////////////////////////
/// Set Rotation

void COGLRotation::Set( float fAngle, float fX, float fY, float fZ )
{
	m_fAngle = GLfloat( fAngle );
	m_fX = GLfloat( fX );
	m_fY = GLfloat( fY );
	m_fZ = GLfloat( fZ );
}

//////////////////////////////////////////////////////////////////////
/// Apply

bool COGLRotation::Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData &rData)
{
	glRotatef( m_fAngle, m_fX, m_fY, m_fZ );

	return true;
}
