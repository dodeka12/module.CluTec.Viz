////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLAnimTranslation.cpp
//
// summary:   Implements the ogl animation translation class
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

// OGLTranslation.cpp: Implementierung der Klasse COGLAnimTranslation.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CluTec.Viz.Fltk\Fl_math.h"
#include "OGLAnimTranslation.h"
#include "OGLBaseElementList.h"
#include "OGLFrame.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLAnimTranslation::COGLAnimTranslation()
{
	m_sTypeName = "AnimTranslation";

	Reset();
}

COGLAnimTranslation::COGLAnimTranslation( float fX, float fY, float fZ )
				 : COGLTranslation(fX, fY, fZ)
{
	m_sTypeName = "AnimTranslation";

	m_fPi = 2.0f * (float) asin(1.0);
	m_fRadPerDeg = m_fPi / 180.0f;
	
	m_fSpeed = 1.0f;
	m_fAmp = 1.0f;
	m_fFreq = 0.01f;
	m_eMode = /*EAnimMode::*/CONSTANT;

	m_bUseFrame = false;
	m_refFrame.Clear();
}

COGLAnimTranslation::COGLAnimTranslation(const COGLAnimTranslation& rRot)
{
	m_sTypeName = "AnimTranslation";

	*this = rRot;
}

COGLAnimTranslation& COGLAnimTranslation::operator= (const COGLAnimTranslation& rRot)
{
	COGLTranslation::operator = (rRot);

	m_fPi = rRot.m_fPi;
	m_fRadPerDeg = rRot.m_fRadPerDeg;

	m_fSpeed = rRot.m_fSpeed;
	m_fAmp = rRot.m_fAmp;
	m_fFreq = rRot.m_fFreq;
	m_eMode = rRot.m_eMode;

	m_bUseFrame = rRot.m_bUseFrame;
	m_refFrame = rRot.m_refFrame;

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Reset

void COGLAnimTranslation::Reset()
{	
	COGLTranslation::Reset();

	m_fPi = 2.0f * (float) asin(1.0);
	m_fRadPerDeg = m_fPi / 180.0f;

	m_fSpeed = 1.0f;
	m_fFreq = 0.01f;
	m_eMode = /*EAnimMode::*/CONSTANT;

	m_bUseFrame = false;
	m_refFrame.Clear();

}

//////////////////////////////////////////////////////////////////////
/// Tell parents that content has changed

void COGLAnimTranslation::TellParentContentChanged()
{
	list<COGLBaseElement*>::iterator itEl;

	for ( itEl = m_listParent.begin();
		  itEl != m_listParent.end();
		  ++itEl )
	{
		COGLBaseElementList *pList = dynamic_cast<COGLBaseElementList*>( (COGLBaseElement*) (*itEl) );
		if ( pList )
			pList->SetContentChanged( true, true, false );
	}
}

//////////////////////////////////////////////////////////////////////
/// Apply

bool COGLAnimTranslation::Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData &rData)
{
	float fFac = 0.0f;

	rData.bNeedAnimate = true;
	TellParentContentChanged();

	if (m_eMode == /*EAnimMode::*/CONSTANT)
	{
		fFac = fmod(2.0f * m_fFreq * float(rData.dTime), 2.0f) - 1.0f;
	}
	else if (m_eMode == /*EAnimMode::*/SINUS)
	{
		fFac = float(sin(double( 2.0f*m_fPi*m_fFreq*float(rData.dTime) ) ) );
	}
	else if (m_eMode == /*EAnimMode::*/SINUS2)
	{
		//fFac = float( 0.5 + 0.5 * sin(double( 2.0f*m_fPi*m_fFreq*float(rData.dTime) ) ) );
		fFac = float(0.5 + 0.5 * sin(double( (2.0f*m_fFreq*float(rData.dTime) - 0.5) * m_fPi ) ) );
	}
	else
	{
		rData.bNeedAnimate = false;
	}

	if ( m_bUseFrame && m_refFrame.IsValid() )
	{
		COGLFrame* pFrame = dynamic_cast<COGLFrame*>( (COGLBaseElement*) m_refFrame );
		if ( pFrame )
		{
			pFrame->Translate( fFac * m_fX, fFac * m_fY, fFac * m_fZ, false );
		}
	}
	else
	{
		glTranslatef( fFac * m_fX, fFac * m_fY, fFac * m_fZ );
	}

	return true;
}

