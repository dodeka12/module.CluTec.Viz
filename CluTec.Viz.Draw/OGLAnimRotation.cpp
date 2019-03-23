////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLAnimRotation.cpp
//
// summary:   Implements the ogl animation rotation class
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

// OGLRotation.cpp: Implementierung der Klasse COGLAnimRotation.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CluTec.Viz.Fltk\Fl_math.h"
#include "OGLAnimRotation.h"
#include "OGLBaseElementList.h"
#include "OGLFrame.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLAnimRotation::COGLAnimRotation()
{
	m_sTypeName = "AnimRotation";

	Reset();
}

COGLAnimRotation::COGLAnimRotation( float fAngle, float fX, float fY, float fZ )
				 : COGLRotation(fAngle, fX, fY, fZ)
{
	m_sTypeName = "AnimRotation";

	m_fPi = 2.0f * (float) asin(1.0);
	m_fRadPerDeg = m_fPi / 180.0f;
	
	m_fSpeed = 1.0f;
	m_fAmp = 1.0f;
	m_fFreq = 0.01f;
	m_eMode = /*EAnimMode::*/CONSTANT;

	m_bUseFrame = false;
	m_refFrame.Clear();

	//m_bUseLocalTime = false;
	//m_dLocalTime = true;
}

COGLAnimRotation::COGLAnimRotation(const COGLAnimRotation& rRot)
{
	m_sTypeName = "AnimRotation";

	*this = rRot;
}

COGLAnimRotation& COGLAnimRotation::operator= (const COGLAnimRotation& rRot)
{
	COGLRotation::operator = (rRot);

	m_fPi = rRot.m_fPi;
	m_fRadPerDeg = rRot.m_fRadPerDeg;

	m_fSpeed = rRot.m_fSpeed;
	m_fAmp = rRot.m_fAmp;
	m_fFreq = rRot.m_fFreq;
	m_eMode = rRot.m_eMode;

	m_bUseFrame = rRot.m_bUseFrame;
	m_refFrame = rRot.m_refFrame;

	//m_bUseLocalTime = rRot.m_bUseLocalTime;
	//m_dLocalTime = rRot.m_dLocalTime;

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Reset

void COGLAnimRotation::Reset()
{	
	COGLRotation::Reset();

	m_fPi = 2.0f * (float) asin(1.0);
	m_fRadPerDeg = m_fPi / 180.0f;

	m_fSpeed = 1.0f;
	m_fFreq = 0.01f;
	m_eMode = /*EAnimMode::*/CONSTANT;

	m_bUseFrame = false;
	m_refFrame.Clear();

	//m_bUseLocalTime = false;
	//m_dLocalTime = true;
}

//////////////////////////////////////////////////////////////////////
/// Tell parents that content has changed

void COGLAnimRotation::TellParentContentChanged()
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

bool COGLAnimRotation::Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData &rData)
{
	rData.bNeedAnimate = true;
	TellParentContentChanged();

	double dTime = rData.dTime;

	// Check whether local time is to be used
	//if ( m_bUseLocalTime )
	//{
	//	if ( eMode == COGLBaseElement::DRAW &&
	//		 rData.bInFirstRenderPass )
	//	{
	//		m_dLocalTime += rData.dTimeStep;
	//	}
	//}


	if (m_eMode == /*EAnimMode::*/CONSTANT)
	{
		m_fAngle = m_fSpeed * float(dTime);
	}
	else if (m_eMode == /*EAnimMode::*/SINUS)
	{
		m_fAngle = m_fSpeed * float(sin(double( 2.0f*m_fPi*m_fFreq*float(dTime) ) ) );
	}
	else if (m_eMode == /*EAnimMode::*/SINUS2)
	{
		m_fAngle = m_fSpeed * float(0.5 + 0.5 * sin(double( (2.0f*m_fFreq*float(dTime) - 0.5) * m_fPi ) ) );
	}
	else if (m_eMode == /*EAnimMode::*/REL_SINUS)
	{
		m_fAngle = m_fSpeed * ( float(dTime) - m_fAmp * float(sin(double( 2.0f*m_fPi*m_fFreq*float(dTime) ) ) ) );
	}
	else if (m_eMode == /*EAnimMode::*/CONT_SINUS2)
	{
		float fTheta = 2.0f*m_fFreq*float(dTime);
		float fCnt = floor( fTheta );
		fTheta = (fTheta - fCnt - 0.5f) * m_fPi;

		m_fAngle = fmod( m_fSpeed * ( float( 0.5 + 0.5 * sin(double( fTheta )) ) + fCnt ), 360.0f );
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
			// Make the given frame a rotation by the given values
			pFrame->Rotate(m_fAngle/* *m_fPi/180.0f */, m_fX, m_fY, m_fZ, false );
		}
	}
	else
	{
		glRotatef( m_fAngle, m_fX, m_fY, m_fZ );
	}

	return true;
}

