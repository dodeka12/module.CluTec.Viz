////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLFilterE3.cpp
//
// summary:   Implements the ogl filter e 3 class
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

// OGLFilterE3GA.cpp: Implementierung der Klasse COGLFilterE3.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OGLFilterE3.h"
#include "CluTec.Viz.Fltk\Fl_math.h"

//MultiV<float> vDummy;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLFilterE3::COGLFilterE3()
{
	ResetModes();
}

COGLFilterE3::~COGLFilterE3()
{

}

//////////////////////////////////////////////////////////////////////
/// Reset Modes

bool COGLFilterE3::ResetModes()
{
	COGLMVFilterBase::ResetModes();
	
	m_bDrawOPNS = true;

	m_eDrawVecType = StdVec;
	m_eDrawPlaneType = StdPlane;

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Set Mode

bool COGLFilterE3::SetMode(int iMode)
{
	if (COGLMVFilterBase::SetMode(iMode))
		return true;

	switch(iMode)
	{
	case MODE_OPNS:
		m_bDrawOPNS = true;
		break;

	case MODE_IPNS:
		m_bDrawOPNS = false;
		break;

	case E3M_DRAW_VEC_AS_LINE:
		m_eDrawVecType = Line;
		break;

	case E3M_DRAW_VEC_AS_ARROW:
		m_eDrawVecType = Arrow;
		break;

	case E3M_DRAW_VEC_AS_POINT:
		m_eDrawVecType = Point;
		break;

	case E3M_DRAW_PLANE_AS_RECT:
		m_eDrawPlaneType = Rectangle;
		break;

	case E3M_DRAW_PLANE_AS_DISK:
		m_eDrawPlaneType = Disk;
		break;

	default:
		return false;
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Draw Multivector

bool COGLFilterE3::DrawMV(const MultiV<float>& vX, bool bAnalyzeOnly)
{
	MultiV<float> vA(vX);

	vA.TinyToZero(m_fSensitivity);

	m_MVInfo.Reset();
	m_dMVInfo.Reset();

	if (!m_E3Base.AnalyzeMV(vA, m_MVInfo, m_bDrawOPNS, m_fSensitivity) || 
		m_MVInfo.m_eType == GA_NONE)
		return false;

	if (bAnalyzeOnly)
		return true;
	else
		return DrawMVInfo(m_MVInfo);
}


//////////////////////////////////////////////////////////////////////
/// Draw Multivector

bool COGLFilterE3::DrawMV(const MultiV<double>& vX, bool bAnalyzeOnly)
{
	MultiV<double> vA(vX);

	vA.TinyToZero((double) m_fSensitivity);

	m_MVInfo.Reset();
	m_dMVInfo.Reset();

	if (!m_dE3Base.AnalyzeMV(vA, m_dMVInfo, m_bDrawOPNS, m_fSensitivity) || 
		m_dMVInfo.m_eType == GA_NONE)
		return false;

	if (!CastMVInfoToFloat(m_MVInfo, m_dMVInfo))
		return false;

	if (bAnalyzeOnly)
		return true;
	else
		return DrawMVInfo(m_MVInfo);
}

//////////////////////////////////////////////////////////////////////
/// Draw Multivector from Info

bool COGLFilterE3::DrawMVInfo(CMVInfo<float>& rInfo)
{
	COGLVertex pVex[3];
	COGLVertex xN;
	float fMag;

	if ( !m_pDrawBase )
		return false;

	if (rInfo.m_eType == GA_POINT)
	{
		pVex[0].Set(0.0, 0.0, 0.0);
		pVex[1] = rInfo.m_mVex[0].Data();

		switch(m_eDrawVecType)
		{
		case Line:
			m_pDrawBase->DrawPoint(pVex[1]);
			m_pDrawBase->DrawLine(pVex[0], pVex[1], true);
			break;

		case Arrow:
			m_pDrawBase->DrawVector(pVex[0], pVex[1]);
			break;

		case Point:
			m_pDrawBase->DrawPoint(pVex[1]);
			break;
		}
	}
	else if (rInfo.m_eType == GA_PLANE)
	{
		pVex[0].Set(0.0f, 0.0f, 0.0f);
		pVex[1] = rInfo.m_mVex[0].Data();
		pVex[2] = rInfo.m_mVex[1].Data();

		switch(m_eDrawPlaneType)
		{
		case Rectangle:
			fMag = (float) sqrt(rInfo.m_mVal[0]);
			m_pDrawBase->DrawPlane(pVex[0], pVex[1]*fMag, pVex[2]*fMag);
			break;

		case Disk:
			fMag = (float) sqrt(rInfo.m_mVal[0] / m_fPi);
			m_pDrawBase->DrawDisk(pVex[0], pVex[1], pVex[2], fMag);
			break;
		}
	}
	else if (rInfo.m_eType == GA_SPACE)
	{
		fMag = float( pow(double(rInfo.m_mVal[0]), 1.0/3.0) );

		m_pDrawBase->DrawCube(fMag);
	}
	else if (rInfo.m_eType == GA_ROTOR)
	{
		pVex[0].Set(0.0f, 0.0f, 0.0f);
		pVex[1] = rInfo.m_mVex[0].Data(); // Rot. Axis

		m_pDrawBase->DrawRotor(pVex[0], pVex[1], rInfo.m_mVal[0]);
	}

	return true;
}



