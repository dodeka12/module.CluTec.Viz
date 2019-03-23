////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLAmbientLight.h
//
// summary:   Declares the ogl ambient light class
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

// OGLAmbientLight.h: Schnittstelle f�r die Klasse COGLAmbientLight.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLAMBIENTLIGHT_H__0AA89723_687A_4A9B_9FC0_50A77C5D2A73__INCLUDED_)
#define AFX_OGLAMBIENTLIGHT_H__0AA89723_687A_4A9B_9FC0_50A77C5D2A73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OGLColor.h"
#include "OGLBaseElement.h"

typedef CPointND<float, 4> TPoint4D;
typedef CPointND<float, 3> TPoint3D;

class CLUDRAW_API COGLAmbientLight : public COGLBaseElement 
{
public:
	COGLAmbientLight();
	COGLAmbientLight(const COGLAmbientLight& rLight);

	virtual COGLBaseElement* Copy()
	{
		return (COGLBaseElement *) new COGLAmbientLight( *this );
	}

	COGLAmbientLight& operator= (const COGLAmbientLight& rLight);

	void Reset();

	void Set(float fR, float fG, float fB, float fA)
	{ m_Ambient.Set(fR, fG, fB, fA); }

	void Set(const COGLColor& rCol)
	{ m_Ambient = rCol; }

	void Set(float *pfVal)
	{ m_Ambient = pfVal; }


	bool Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData &rData);
	bool Apply();

protected:
	COGLColor m_Ambient;
};

#endif // !defined(AFX_OGLAmbientLight_H__0AA89723_687A_4A9B_9FC0_50A77C5D2A73__INCLUDED_)
