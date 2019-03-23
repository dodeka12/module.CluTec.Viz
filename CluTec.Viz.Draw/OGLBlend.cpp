////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLBlend.cpp
//
// summary:   Implements the ogl blend class
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

#include "StdAfx.h"
#include "OGLBlend.h"

#include "CluTec.Viz.OpenGL.Extensions\glext.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
COGLBlend::COGLBlend()
{
	m_sTypeName     = "Blend";
	m_bIsDefault    = true;
	m_uBlendModeSrc = GL_ZERO;
	m_uBlendModeDst = GL_ZERO;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
COGLBlend::COGLBlend(const COGLBlend& xBlend)
{
	m_sTypeName = "Blend";
	*this       = xBlend;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
COGLBlend& COGLBlend::operator=(const COGLBlend& xBlend)
{
	m_uBlendModeSrc = xBlend.m_uBlendModeSrc;
	m_uBlendModeDst = xBlend.m_uBlendModeDst;
	m_bIsDefault    = xBlend.m_bIsDefault;
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool COGLBlend::SetBlendType(CStrMem& sBlendModeDst)
{
	// Convert to lower case
	CStrMem& sLower = sBlendModeDst.ToLowerCase();

	// Select correct blend mode
	if (sLower == "additive")
	{
		m_bIsDefault    = false;
		m_uBlendModeSrc = GL_ONE;
		m_uBlendModeDst = GL_ONE;
	}
	else if (sLower == "default")
	{
		m_bIsDefault    = true;
		m_uBlendModeSrc = GL_ZERO;
		m_uBlendModeDst = GL_ZERO;
	}
	else if (sLower == "inverse")
	{
		m_bIsDefault    = false;
		m_uBlendModeSrc = GL_ONE_MINUS_DST_COLOR;
		m_uBlendModeDst = GL_ZERO;
	}
	else
	{
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool COGLBlend::Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData& rData)
{
	// Do nothing in 'TransparencyPass' - Use default blending
	if (rData.bInTransparencyPass)
	{
		return true;
	}

	// Disable blending while picking
	if (eMode == COGLBaseElement::PICK)
	{
		glDisable(GL_BLEND);
		return true;
	}

	if (m_bIsDefault)
	{
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
	}
	else
	{
		// BlendingFactorDest
		// ###################################################
		// #define GL_ZERO                           0
		// #define GL_ONE                            1       - Additive
		// #define GL_SRC_COLOR                      0x0300
		// #define GL_ONE_MINUS_SRC_COLOR            0x0301
		// #define GL_SRC_ALPHA                      0x0302
		// #define GL_ONE_MINUS_SRC_ALPHA            0x0303  - Default
		// #define GL_DST_ALPHA                      0x0304
		// #define GL_ONE_MINUS_DST_ALPHA            0x0305

		// BlendingFactorSrc
		// ###################################################
		//         GL_ZERO
		//         GL_ONE
		// #define GL_DST_COLOR                      0x0306
		// #define GL_ONE_MINUS_DST_COLOR            0x0307
		// #define GL_SRC_ALPHA_SATURATE             0x0308
		//         GL_SRC_ALPHA
		//         GL_ONE_MINUS_SRC_ALPHA
		//         GL_DST_ALPHA
		//         GL_ONE_MINUS_DST_ALPHA

		glEnable(GL_BLEND);
		glBlendFunc(m_uBlendModeSrc, m_uBlendModeDst);
		glDepthMask(GL_FALSE);
	}

	return true;
}
