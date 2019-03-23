////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLMultisample.cpp
//
// summary:   Implements the ogl multisample class
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

// OGLTranslation.cpp: Implementierung der Klasse COGLMultisample.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CluTec.Viz.OpenGL.Extensions\glext.h"
#include "OGLMultisample.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLMultisample::COGLMultisample()
{
	m_sTypeName = "Multisample";

	Reset();
}

COGLMultisample::COGLMultisample(bool bVal)
{
	m_sTypeName = "Multisample";

	Enable(bVal);
}

COGLMultisample::COGLMultisample(const COGLMultisample& rTrans)
{
	m_sTypeName = "Multisample";

	*this = rTrans;
}

COGLMultisample& COGLMultisample::operator=(const COGLMultisample& rTrans)
{
	m_bDoMultisample = rTrans.m_bDoMultisample;

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Reset

void COGLMultisample::Reset()
{
	m_bDoMultisample = false;
}

//////////////////////////////////////////////////////////////////////
/// Set Translation

void COGLMultisample::Enable(bool bVal)
{
	m_bDoMultisample = bVal;
}

//////////////////////////////////////////////////////////////////////
/// Apply

bool COGLMultisample::Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData& rData)
{
	if (eMode == COGLBaseElement::PICK)
	{
		return true;
	}

	if (m_bDoMultisample)
	{
		glEnable(GL_MULTISAMPLE);
	}
	else
	{
		glDisable(GL_MULTISAMPLE);
	}

	return true;
}
