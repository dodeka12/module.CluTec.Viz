////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLAnimScale.cpp
//
// summary:   Implements the ogl animation scale class
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

// OGLScale.cpp: Implementierung der Klasse COGLAnimScale.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CluTec.Viz.Fltk\Fl_math.h"
#include "OGLAnimScale.h"
#include "OGLBaseElementList.h"
#include "OGLFrame.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLAnimScale::COGLAnimScale()
{
	m_sTypeName = "AnimScale";

	Reset();
}

COGLAnimScale::COGLAnimScale( float fX, float fY, float fZ )
				 : COGLScale(fX, fY, fZ)
{
	m_sTypeName = "AnimScale";

	m_fPi = 2.0f * (float) asin(1.0);
	m_fRadPerDeg = m_fPi / 180.0f;
	
	m_fSpeed = 1.0f;
	m_fAmp = 1.0f;
	m_fFreq = 0.01f;
	m_eMode = /*EAnimMode::*/CONSTANT;

	m_bUseFrame = false;
	m_refFrame.Clear();

}

COGLAnimScale::COGLAnimScale(const COGLAnimScale& rRot)
{
	m_sTypeName = "AnimScale";

	*this = rRot;
}

COGLAnimScale& COGLAnimScale::operator= (const COGLAnimScale& rRot)
{
	COGLScale::operator = (rRot);

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

void COGLAnimScale::Reset()
{	
	COGLScale::Reset();

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

void COGLAnimScale::TellParentContentChanged()
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

bool COGLAnimScale::Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData &rData)
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
		//fFac = float(0.5 + 0.5 * sin(double( 2.0f*m_fPi*m_fFreq*float(rData.dTime) ) ) );
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
			pFrame->Scale( 1.0f + fFac * m_fX, 1.0f + fFac * m_fY, 1.0f + fFac * m_fZ, false );
		}
	}
	else
	{
		glScalef( 1.0f + fFac * m_fX, 1.0f + fFac * m_fY, 1.0f + fFac * m_fZ );
	}

	return true;
}

