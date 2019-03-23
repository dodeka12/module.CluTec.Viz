////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLAmbientLight.cpp
//
// summary:   Implements the ogl ambient light class
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

// OGLAmbientLight.cpp: Implementierung der Klasse COGLAmbientLight.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OGLAmbientLight.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLAmbientLight::COGLAmbientLight()
{
	m_sTypeName = "AmbientLight";

	Reset();
}

COGLAmbientLight::COGLAmbientLight(const COGLAmbientLight& rLight)
{
	m_sTypeName = "AmbientLight";

	*this = rLight;
}


COGLAmbientLight& COGLAmbientLight::operator= (const COGLAmbientLight& rLight)
{
	m_Ambient = rLight.m_Ambient;

	return *this;
}


//////////////////////////////////////////////////////////////////////
/// Reset

void COGLAmbientLight::Reset()
{
	m_Ambient.Set(0.2f, 0.2f, 0.2f, 1.0f);
}


//////////////////////////////////////////////////////////////////////
// Apply
bool COGLAmbientLight::Apply( COGLBaseElement::EApplyMode eMode, 
					  COGLBaseElement::SApplyData &rData)
{
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, m_Ambient.Data());

	return true;
}

