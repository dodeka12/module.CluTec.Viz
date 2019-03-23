////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLMaterial.cpp
//
// summary:   Implements the ogl material class
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

// OGLMaterial.cpp: Implementierung der Klasse COGLMaterial.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OGLMaterial.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLMaterial::COGLMaterial()
{
	m_sTypeName = "Material";

	Reset();
}

COGLMaterial::COGLMaterial(const COGLMaterial& rMat)
{
	m_sTypeName = "Material";

	*this = rMat;
}

COGLMaterial& COGLMaterial::operator= (const COGLMaterial& rMat)
{
	m_eFace = rMat.m_eFace;

	m_Ambient = rMat.m_Ambient;
	m_Diffuse = rMat.m_Diffuse;
	m_Specular = rMat.m_Specular;
	m_Emission = rMat.m_Emission;

	m_fShininess = rMat.m_fShininess;

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Reset

void COGLMaterial::Reset()
{
	m_eFace = GL_FRONT_AND_BACK;

	m_Ambient.Set(0.2f, 0.2f, 0.2f, 1.0f);
	m_Diffuse.Set(0.8f, 0.8f, 0.8f, 1.0f);
	m_Specular.Set(0.0f, 0.0f, 0.0f, 1.0f);
	m_Emission.Set(0.0f, 0.0f, 0.0f, 1.0f);

	m_fShininess = 0.0;
}

//////////////////////////////////////////////////////////////////////
/// Apply

bool COGLMaterial::Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData &rData)
{
	glMaterialfv(m_eFace, GL_AMBIENT, m_Ambient.Data());
	glMaterialfv(m_eFace, GL_DIFFUSE, m_Diffuse.Data());
	glMaterialfv(m_eFace, GL_SPECULAR, m_Specular.Data());
	glMaterialfv(m_eFace, GL_EMISSION, m_Emission.Data());

	glMaterialf(m_eFace, GL_SHININESS, m_fShininess);

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Apply

bool COGLMaterial::Apply()
{
	glMaterialfv(m_eFace, GL_AMBIENT, m_Ambient.Data());
	glMaterialfv(m_eFace, GL_DIFFUSE, m_Diffuse.Data());
	glMaterialfv(m_eFace, GL_SPECULAR, m_Specular.Data());
	glMaterialfv(m_eFace, GL_EMISSION, m_Emission.Data());

	glMaterialf(m_eFace, GL_SHININESS, m_fShininess);

	return true;
}
