////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLAnimScale.h
//
// summary:   Declares the ogl animation scale class
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

#if !defined(AFX_OGLANIMSCALE_H__INCLUDED_)
#define AFX_OGLANIMSCALE_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OGLScale.h"

class CLUDRAW_API COGLAnimScale  : public COGLScale
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
	COGLAnimScale();
	COGLAnimScale( float fX, float fY, float fZ );
	COGLAnimScale(const COGLAnimScale& rRot);

	virtual COGLBaseElement* Copy()
	{
		return (COGLBaseElement *) new COGLAnimScale( *this );
	}

	COGLAnimScale& operator= (const COGLAnimScale& rRot);

	void Reset();

	void SetFreq( float fFreq ) { m_fFreq = fFreq; }
	void SetSpeed( float fSpeed ) { m_fSpeed = fSpeed; }
	void SetAmplitude( float fAmplitude ) { m_fAmp = fAmplitude; }
	void SetMode( EAnimMode eMode ) { m_eMode = eMode; }

	void SetFrame( COGLBEReference refFrame ) { m_refFrame = refFrame; }
	void EnableFrame( bool bVal = true ) { m_bUseFrame = bVal; }

	bool Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData &rData);

protected:
	void TellParentContentChanged();

protected:
	float m_fFreq;
	float m_fSpeed;
	float m_fAmp;
	float m_fPi;
	float m_fRadPerDeg;

	EAnimMode m_eMode;

	// Animate given frame instead of modifying current OGL frame.
	bool m_bUseFrame;

	// BE Reference to Frame that is to be animated
	COGLBEReference m_refFrame;

};

#endif // !defined(AFX_OGLMATERIAL_H__2296DD19_C205_454A_82DB_D1881903B593__INCLUDED_)
