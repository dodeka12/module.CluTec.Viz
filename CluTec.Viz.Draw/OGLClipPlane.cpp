////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLClipPlane.cpp
//
// summary:   Implements the ogl clip plane class
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
#include "OGLClipPlane.h"

#include "CluTec.Viz.OpenGL.Extensions\glext.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
COGLClipPlane::COGLClipPlane()
{
	m_sTypeName  = "ClipPlane";
	m_bIsEnabled = false;
	memset(m_pdClipPlane, 0, 4 * sizeof(double));
	m_uCliPlaneGlId = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
COGLClipPlane::COGLClipPlane(const COGLClipPlane& xClipPlane)
{
	m_sTypeName = "ClipPlane";
	*this       = xClipPlane;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
COGLClipPlane& COGLClipPlane::operator=(const COGLClipPlane& xClipPlane)
{
	m_bIsEnabled = xClipPlane.m_bIsEnabled;
	memcpy(m_pdClipPlane, xClipPlane.m_pdClipPlane, 4 * sizeof(double));
	m_uCliPlaneGlId = xClipPlane.m_uCliPlaneGlId;

	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned COGLClipPlane::GetClipPlaneCount()
{
	int iClipPlaneCount = 0;
	glGetIntegerv(GL_MAX_CLIP_PLANES, &iClipPlaneCount);
	if (iClipPlaneCount < 0)
	{
		iClipPlaneCount = 0;
	}

	return unsigned(iClipPlaneCount);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool COGLClipPlane::SetGlId(unsigned uId)
{
	if (uId >= GetClipPlaneCount())
	{
		return false;
	}

	m_uCliPlaneGlId = uId;
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void COGLClipPlane::SetClipPlane(const COGLVertex& vCenter, const COGLVertex& vDirX, const COGLVertex& vDirY)
{
	COGLVertex vDirZ = vDirX ^ vDirY;

	vDirZ.Norm();
	m_pdClipPlane[0] = vDirZ[0];
	m_pdClipPlane[1] = vDirZ[1];
	m_pdClipPlane[2] = vDirZ[2];
	m_pdClipPlane[3] = -vDirZ * vCenter;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool COGLClipPlane::Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData& rData)
{
	if (m_bIsEnabled)
	{
		glEnable(GL_CLIP_PLANE0 + m_uCliPlaneGlId);
		glClipPlane(GL_CLIP_PLANE0 + m_uCliPlaneGlId, m_pdClipPlane);
	}
	else
	{
		glDisable(GL_CLIP_PLANE0 + m_uCliPlaneGlId);
	}

	return true;
}
