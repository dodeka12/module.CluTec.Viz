////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLMaterial.h
//
// summary:   Declares the ogl material class
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

// OGLMaterial.h: Schnittstelle f�r die Klasse COGLMaterial.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLMATERIAL_H__2296DD19_C205_454A_82DB_D1881903B593__INCLUDED_)
#define AFX_OGLMATERIAL_H__2296DD19_C205_454A_82DB_D1881903B593__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "OGLBaseElement.h"
#include "OGLColor.h"


class CLUDRAW_API COGLMaterial  : public COGLBaseElement
{
public:
	COGLMaterial();
	COGLMaterial(const COGLMaterial& rMat);

	COGLMaterial& operator= (const COGLMaterial& rMat);

	virtual COGLBaseElement* Copy()
	{
		return (COGLBaseElement *) new COGLMaterial( *this );
	}

	void Reset();

	void Ambient(float fR, float fG, float fB, float fA)
	{ m_Ambient.Set(fR, fG, fB, fA); }
	void Ambient(const COGLColor& rCol)
	{ m_Ambient = rCol; }
	void Ambient(float *pfVal)
	{ m_Ambient = pfVal; }

	void Diffuse(float fR, float fG, float fB, float fA)
	{ m_Diffuse.Set(fR, fG, fB, fA); }
	void Diffuse(const COGLColor& rCol)
	{ m_Diffuse = rCol; }
	void Diffuse(float *pfVal)
	{ m_Diffuse = pfVal; }

	void Specular(float fR, float fG, float fB, float fA)
	{ m_Specular.Set(fR, fG, fB, fA); }
	void Specular(const COGLColor& rCol)
	{ m_Specular = rCol; }
	void Specular(float *pfVal)
	{ m_Specular = pfVal; }

	void Emission(float fR, float fG, float fB, float fA)
	{ m_Emission.Set(fR, fG, fB, fA); }
	void Emission(const COGLColor& rCol)
	{ m_Emission = rCol; }
	void Emission(float *pfVal)
	{ m_Emission = pfVal; }

	void Shininess(float fV) { m_fShininess = fV; }
	void Face(GLenum eFace) { m_eFace = eFace; }

	bool Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData &rData);
	bool Apply();

protected:
	COGLColor m_Ambient, m_Diffuse, m_Specular, m_Emission;
	float m_fShininess;

	GLenum m_eFace;
};

#endif // !defined(AFX_OGLMATERIAL_H__2296DD19_C205_454A_82DB_D1881903B593__INCLUDED_)
