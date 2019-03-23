////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLLineWidth.cpp
//
// summary:   Implements the ogl line width class
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

// OGLTranslation.cpp: Implementierung der Klasse COGLLineWidth.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OGLLineWidth.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLLineWidth::COGLLineWidth()
{
	m_sTypeName = "LineWidth";

	Reset();
}

COGLLineWidth::COGLLineWidth( float fWidth )
{
	m_sTypeName = "LineWidth";

	Set( fWidth );
}

COGLLineWidth::COGLLineWidth(const COGLLineWidth& rTrans )
{
	m_sTypeName = "LineWidth";

	*this = rTrans;
}

COGLLineWidth& COGLLineWidth::operator= (const COGLLineWidth& rTrans)
{
	m_fWidth = rTrans.m_fWidth;

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Reset

void COGLLineWidth::Reset()
{	
	m_fWidth = GLfloat( 1 );
}

//////////////////////////////////////////////////////////////////////
/// Set Translation

void COGLLineWidth::Set( float fWidth )
{
	if ( fWidth > 0.0f )
		m_fWidth = GLfloat( fWidth );
}

//////////////////////////////////////////////////////////////////////
/// Apply

bool COGLLineWidth::Apply(COGLBaseElement::EApplyMode eMode, 
							COGLBaseElement::SApplyData &rData)
{
	glLineWidth( m_fWidth );

	return true;
}
