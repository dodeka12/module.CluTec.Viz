////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLPointParameter.h
//
// summary:   Declares the ogl point parameter class
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

//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLPOINTPARAMETER_H__INCLUDED_)
#define AFX_OGLPOINTPARAMETER_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OGLBaseElement.h"


class CLUDRAW_API COGLPointParameter  : public COGLBaseElement
{
public:
	COGLPointParameter();
	COGLPointParameter(const COGLPointParameter& rPntPar);

	virtual COGLBaseElement* Copy()
	{
		return (COGLBaseElement *) new COGLPointParameter( *this );
	}

	COGLPointParameter& operator= (const COGLPointParameter& rPntPar);

	void Reset();

	void SetSize( float fSize );
	void SetSizeRange( float fMin, float fMax );
	void SetFadeSize( float fVal );
	void SetDistanceAttenuation( float fConst, float fLin, float fQuad );

	bool Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData &rData);

protected:
	GLfloat m_fSize;
	GLfloat m_fSizeMin, m_fSizeMax;
	GLfloat m_fFadeSize;
	GLfloat m_pfAttenuation[3];
};

#endif // !defined(AFX_OGLMATERIAL_H__2296DD19_C205_454A_82DB_D1881903B593__INCLUDED_)
