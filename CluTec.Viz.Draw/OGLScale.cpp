////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLScale.cpp
//
// summary:   Implements the ogl scale class
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

// OGLScale.cpp: Implementierung der Klasse COGLScale.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OGLScale.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLScale::COGLScale()
{
	m_sTypeName = "Scale";

	Reset();
}

COGLScale::COGLScale( float fX, float fY, float fZ )
{
	m_sTypeName = "Scale";

	Set( fX, fY, fZ );
}

COGLScale::COGLScale(const COGLScale& rTrans )
{
	m_sTypeName = "Scale";

	*this = rTrans;
}

COGLScale& COGLScale::operator= (const COGLScale& rTrans)
{
	m_fX = rTrans.m_fX;
	m_fY = rTrans.m_fY;
	m_fZ = rTrans.m_fZ;

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Reset

void COGLScale::Reset()
{	
	m_fX = GLfloat( 1 );
	m_fY = GLfloat( 1 );
	m_fZ = GLfloat( 1 );
}

//////////////////////////////////////////////////////////////////////
/// Set Scale

void COGLScale::Set( float fX, float fY, float fZ )
{
	m_fX = GLfloat( fX );
	m_fY = GLfloat( fY );
	m_fZ = GLfloat( fZ );
}

//////////////////////////////////////////////////////////////////////
/// Apply

bool COGLScale::Apply(COGLBaseElement::EApplyMode eMode, 
							COGLBaseElement::SApplyData &rData)
{
	glScalef( m_fX, m_fY, m_fZ );

	return true;
}
