////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLPixelZoom.cpp
//
// summary:   Implements the ogl pixel zoom class
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

// OGLPixelZoom.cpp: Implementierung der Klasse COGLPixelZoom.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OGLPixelZoom.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLPixelZoom::COGLPixelZoom()
{
	m_sTypeName = "PixelZoom";

	Reset();
}

COGLPixelZoom::COGLPixelZoom( float fX, float fY )
{
	m_sTypeName = "PixelZoom";

	Set( fX, fY );
}

COGLPixelZoom::COGLPixelZoom(const COGLPixelZoom& rTrans )
{
	m_sTypeName = "Scale";

	*this = rTrans;
}

COGLPixelZoom& COGLPixelZoom::operator= (const COGLPixelZoom& rTrans)
{
	m_fX = rTrans.m_fX;
	m_fY = rTrans.m_fY;

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Reset

void COGLPixelZoom::Reset()
{	
	m_fX = GLfloat( 1 );
	m_fY = GLfloat( 1 );
}

//////////////////////////////////////////////////////////////////////
/// Set Scale

void COGLPixelZoom::Set( float fX, float fY )
{
	m_fX = GLfloat( fX );
	m_fY = GLfloat( fY );
}

//////////////////////////////////////////////////////////////////////
/// Apply

bool COGLPixelZoom::Apply(COGLBaseElement::EApplyMode eMode, 
							COGLBaseElement::SApplyData &rData)
{
	glPixelZoom( m_fX, m_fY );

	return true;
}
