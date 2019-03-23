////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLLight.h
//
// summary:   Declares the ogl light class
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

// OGLLight.h: Schnittstelle f�r die Klasse COGLLight.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLLIGHT_H__0AA89723_687A_4A9B_9FC0_50A77C5D2A73__INCLUDED_)
#define AFX_OGLLIGHT_H__0AA89723_687A_4A9B_9FC0_50A77C5D2A73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OGLColor.h"
#include "OGLBaseElement.h"

typedef CPointND<float, 4> TPoint4D;
typedef CPointND<float, 3> TPoint3D;

class CLUDRAW_API COGLLight : public COGLBaseElement 
{
public:
	enum EChange
	{
		NONE		= 0x0000,
		AMBIENT		= 0x0001,
		DIFFUSE		= 0x0002,
		SPECULAR	= 0x0004,
		POSITION	= 0x0008,
		SPOTDIR		= 0x0010,
		SPOTEXP		= 0x0020,
		SPOTCUTOFF	= 0x0040,
		CONSTATT	= 0x0080,
		LINATT		= 0x0100,
		QUADATT		= 0x0200,
		ALL			= 0xFFFF
	};

public:
	COGLLight();
	COGLLight(const COGLLight& rLight);

	virtual COGLBaseElement* Copy()
	{
		return (COGLBaseElement *) new COGLLight( *this );
	}

	COGLLight& operator= (const COGLLight& rLight);

	void Reset();

	void Ambient(float fR, float fG, float fB, float fA)
	{ m_Ambient.Set(fR, fG, fB, fA); m_uChangeFlag |= uint(/*EChange::*/AMBIENT); }
	void Ambient(const COGLColor& rCol)
	{ m_Ambient = rCol; m_uChangeFlag |= uint(/*EChange::*/AMBIENT); }
	void Ambient(float *pfVal)
	{ m_Ambient = pfVal; m_uChangeFlag |= uint(/*EChange::*/AMBIENT); }

	void Diffuse(float fR, float fG, float fB, float fA)
	{ m_Diffuse.Set(fR, fG, fB, fA); m_uChangeFlag |= uint(/*EChange::*/DIFFUSE); }
	void Diffuse(const COGLColor& rCol)
	{ m_Diffuse = rCol; m_uChangeFlag |= uint(/*EChange::*/DIFFUSE); }
	void Diffuse(float *pfVal)
	{ m_Diffuse = pfVal; m_uChangeFlag |= uint(/*EChange::*/DIFFUSE); }

	void Specular(float fR, float fG, float fB, float fA)
	{ m_Specular.Set(fR, fG, fB, fA); m_uChangeFlag |= uint(/*EChange::*/SPECULAR); }
	void Specular(const COGLColor& rCol)
	{ m_Specular = rCol; m_uChangeFlag |= uint(/*EChange::*/SPECULAR); }
	void Specular(float *pfVal)
	{ m_Specular = pfVal; m_uChangeFlag |= uint(/*EChange::*/SPECULAR); }

	void Position(float fX, float fY, float fZ, float fW)
	{ 
		m_Position[0] = fX; m_Position[1] = fY;
		m_Position[2] = fZ; m_Position[3] = fW; 
		m_uChangeFlag |= uint(/*EChange::*/POSITION); 
	}
	void Position(float *pfVal)
	{ m_Position = pfVal; m_uChangeFlag |= uint(/*EChange::*/POSITION); }

	void SpotDir(float fX, float fY, float fZ)
	{ 
		m_SpotDir[0] = fX; m_SpotDir[1] = fY;
		m_SpotDir[2] = fZ; 
		m_uChangeFlag |= uint(/*EChange::*/SPOTDIR); 
	}
	void SpotDir(float *pfVal)
	{ m_SpotDir = pfVal; m_uChangeFlag |= uint(/*EChange::*/SPOTDIR); }

	void SpotExp(float fExp) { m_fSpotExp = fExp; m_uChangeFlag |= uint(/*EChange::*/SPOTEXP); }
	void SpotCutOff(float fCO) { m_fSpotCutOff = fCO; m_uChangeFlag |= uint(/*EChange::*/SPOTCUTOFF); }
	void ConstAtt(float fV) { m_fConstAtt = fV; m_uChangeFlag |= uint(/*EChange::*/CONSTATT); }
	void LinAtt(float fV) { m_fLinAtt = fV; m_uChangeFlag |= uint(/*EChange::*/LINATT); }
	void QuadAtt(float fV) { m_fQuadAtt = fV; m_uChangeFlag |= uint(/*EChange::*/QUADATT); }

	void LightNo(int iNo)
	{ if (iNo >= 0) m_eLight = GLenum(GL_LIGHT0+iNo); }

	void Enable(bool bAct = true)
	{ m_bIsEnabled = bAct; }

	bool Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData &rData);
	bool Apply();

protected:
	GLenum m_eLight;

	uint m_uChangeFlag;

	COGLColor m_Ambient, m_Diffuse, m_Specular;
	TPoint4D m_Position;
	TPoint3D m_SpotDir;

	float m_fSpotExp;		// Spot Exponent
	float m_fSpotCutOff;	// Spot Cut Off
	float m_fConstAtt;		// Constant Attenuation
	float m_fLinAtt;		// Linear Attenuation
	float m_fQuadAtt;		// Quadratic Attenuation

	bool m_bIsEnabled;
};

#endif // !defined(AFX_OGLLIGHT_H__0AA89723_687A_4A9B_9FC0_50A77C5D2A73__INCLUDED_)
