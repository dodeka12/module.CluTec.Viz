////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLDisplayList.cpp
//
// summary:   Implements the ogl display list class
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

// OGLDisplayList.cpp: Implementierung der Klasse COGLDisplayList.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OGLDisplayList.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLDisplayList::COGLDisplayList()
{
	m_sTypeName = "DisplayList";

	m_uIdx = 0;
}

COGLDisplayList::COGLDisplayList( const COGLDisplayList& rList )
{
	m_sTypeName = "DisplayList";

	*this = rList;
}

COGLDisplayList::~COGLDisplayList()
{
	if (m_uIdx) glDeleteLists(m_uIdx, 1);
}


COGLDisplayList& COGLDisplayList::operator= ( const COGLDisplayList& rList )
{
	m_uIdx = rList.m_uIdx;

	return *this;
}


//////////////////////////////////////////////////////////////////////
/// Create Display List

bool COGLDisplayList::Create()
{
	if (!(m_uIdx = glGenLists(1)))
		return false;

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Destroy Display List

bool COGLDisplayList::Destroy()
{
	if (!m_uIdx)
		return false;

	glDeleteLists(m_uIdx, 1);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Begin Draw

bool COGLDisplayList::BeginDraw(bool bDraw)
{
	if (!m_uIdx) return false;

	if (bDraw)
		glNewList(m_uIdx, GL_COMPILE_AND_EXECUTE);
	else
		glNewList(m_uIdx, GL_COMPILE);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// End Draw

bool COGLDisplayList::EndDraw()
{
	if (!m_uIdx) return false;

	glEndList();

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Apply

bool COGLDisplayList::Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData &rData)
{
	if (!m_uIdx) return false;

	glCallList(m_uIdx);

	return true;
}

