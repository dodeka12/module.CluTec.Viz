////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLPointParameter.cpp
//
// summary:   Implements the ogl point parameter class
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

// OGLScale.cpp: Implementierung der Klasse COGLPointParameter.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CluTec.Viz.OpenGL.Extensions\Extensions.h"
#include "CluTec.Viz.OpenGL/Api.h"
#include "OGLPointParameter.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLPointParameter::COGLPointParameter()
{
	m_sTypeName = "PointParameter";

	Reset();
}

COGLPointParameter::COGLPointParameter(const COGLPointParameter& rPntPar)
{
	m_sTypeName = "PointParameter";

	*this = rPntPar;
}

COGLPointParameter& COGLPointParameter::operator=(const COGLPointParameter& rPntPar)
{
	memcpy(m_pfAttenuation, rPntPar.m_pfAttenuation, 3 * sizeof(GLfloat));

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Reset

void COGLPointParameter::Reset()
{
	m_fSize     = 2.0f;
	m_fSizeMin  = 1.0f;
	m_fSizeMax  = 10.0f;
	m_fFadeSize = 1.0f;

	m_pfAttenuation[0] = GLfloat(1);
	m_pfAttenuation[1] = GLfloat(0);
	m_pfAttenuation[2] = GLfloat(0);
}

//////////////////////////////////////////////////////////////////////
/// Set Point Size

void COGLPointParameter::SetSize(float fSize)
{
	m_fSize = fSize;
}

/////////////////////////////////////////////////////////////////////////
/// Set Size Range

void COGLPointParameter::SetSizeRange(float fMin, float fMax)
{
	m_fSizeMin = fMin;
	m_fSizeMax = fMax;
}

//////////////////////////////////////////////////////////////////////
/// Set Fade Size

void COGLPointParameter::SetFadeSize(float fVal)
{
	m_fFadeSize = fVal;
}

//////////////////////////////////////////////////////////////////////
/// Set Scale

void COGLPointParameter::SetDistanceAttenuation(float fConst, float fLin, float fQuad)
{
	m_pfAttenuation[0] = GLfloat(fConst);
	m_pfAttenuation[1] = GLfloat(fLin);
	m_pfAttenuation[2] = GLfloat(fQuad);
}

//////////////////////////////////////////////////////////////////////
/// Apply

bool COGLPointParameter::Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData& rData)
{
	if (glPointParameterf)
	{
		glPointSize(m_fSize);
		glPointParameterf(GL_POINT_SIZE_MIN, m_fSizeMin);
		glPointParameterf(GL_POINT_SIZE_MAX, m_fSizeMax);
		glPointParameterf(GL_POINT_FADE_THRESHOLD_SIZE, m_fFadeSize);
		glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, m_pfAttenuation);
	}
	else
	{
		glPointSize(m_fSize);
	}

	return true;
}
