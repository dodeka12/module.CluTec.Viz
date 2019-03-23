////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLAnimColor.h
//
// summary:   Declares the ogl animation color class
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

// OGLAnimColor.h: Schnittstelle f�r die Klasse COGLAnimColor.
//
// Animated Color Element
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLANIMCOLOR_H__ECC38C81_0DD0_4C4F_B77D_86F72B5AA0DE__INCLUDED_)
#define AFX_OGLANIMCOLOR_H__ECC38C81_0DD0_4C4F_B77D_86F72B5AA0DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "OGLBaseElement.h"
#include "OGLColor.h"


class CLUDRAW_API COGLAnimColor  : public COGLColor
{
public:
	enum EAnimMode
	{
		NONE = 0,
		CONSTANT,
		SINUS,
		SINUS2
	};

public:
	COGLAnimColor();
	COGLAnimColor(const COGLAnimColor& rCol);

	virtual COGLBaseElement* Copy()
	{
		return (COGLBaseElement *) new COGLAnimColor( *this );
	}

	COGLAnimColor& operator= (const COGLAnimColor& rCol);
	COGLAnimColor& operator= (const float *pfCol);

	void Set( const COGLColor &rCol )
	{ memcpy( m_pfCol, rCol.Data(), 4*sizeof(float) ); }

	void SetDir(float fR, float fG, float fB, float fA = 1.0f)
	{ m_pfColDir[0] = fR; m_pfColDir[1] = fG; m_pfColDir[2] = fB; m_pfColDir[3] = fA; }

	void SetDir(const COGLColor &rCol)
	{ memcpy( m_pfColDir, rCol.Data(), 4*sizeof(float) ); }

	void SetFreq( float fFreq )
	{ m_fFreq = fFreq; }

	void SetMode( EAnimMode eMode )
	{ m_eMode = eMode; }

	bool Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData &rData);

protected:
	void TellParentContentChanged();

protected:
	float m_pfColDir[4], m_pfColAnim[4], m_pfColDiff[4];

	float m_fFreq;
	float m_fPi;

	EAnimMode m_eMode;
};

#endif // !defined(AFX_OGLCOLOR_H__ECC38C81_0DD0_4C4F_B77D_86F72B5AA0DE__INCLUDED_)
