////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLPointSprites.cpp
//
// summary:   Implements the ogl point sprites class
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

// OGLScale.cpp: Implementierung der Klasse COGLPointSprites.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CluTec.Viz.OpenGL.Extensions\Extensions.h"
#include "CluTec.Viz.OpenGL/Api.h"
#include "OGLPointSprites.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLPointSprites::COGLPointSprites()
{
	m_sTypeName = "PointSprites";

	Reset();
}

COGLPointSprites::COGLPointSprites(const COGLPointSprites& rPntPar)
{
	m_sTypeName = "PointParameter";

	*this = rPntPar;
}

COGLPointSprites& COGLPointSprites::operator=(const COGLPointSprites& rPntPar)
{
	m_bDoEnable = rPntPar.m_bDoEnable;
	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Reset

void COGLPointSprites::Reset()
{
	m_bDoEnable = false;
}

//////////////////////////////////////////////////////////////////////
/// Set Point Size

void COGLPointSprites::Enable(bool bValue)
{
	m_bDoEnable = bValue;
}

//////////////////////////////////////////////////////////////////////
/// Apply

bool COGLPointSprites::Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData& rData)
{
	if (m_bDoEnable)
	{
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

		glEnable(GL_POINT_SPRITE);
		glTexEnvf(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
	}
	else
	{
		glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);

		glDisable(GL_POINT_SPRITE);
		glTexEnvf(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_FALSE);
	}
	return true;
}
