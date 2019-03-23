////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLFilterP3.cpp
//
// summary:   Implements the ogl filter p 3 class
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

// OGLFilterPGA.cpp: Implementierung der Klasse COGLFilterP3.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OGLFilterP3.h"
#include "CluTec.Viz.Fltk\Fl_math.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLFilterP3::COGLFilterP3()
{
	ResetModes();
}

COGLFilterP3::~COGLFilterP3()
{
}

//////////////////////////////////////////////////////////////////////
/// Reset Modes

bool COGLFilterP3::ResetModes()
{
	COGLMVFilterBase::ResetModes();

	m_bDrawOPNS = true;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Mode

bool COGLFilterP3::SetMode(int iMode)
{
	if (COGLMVFilterBase::SetMode(iMode))
	{
		return true;
	}

	switch (iMode)
	{
	case MODE_OPNS:
		m_bDrawOPNS = true;
		break;

	case MODE_IPNS:
		m_bDrawOPNS = false;
		break;

	default:
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Multivector

bool COGLFilterP3::DrawMV(const MultiV<float>& vX, bool bAnalyzeOnly)
{
	MultiV<float> vA(vX);

	vA.TinyToZero(m_fSensitivity);

	m_MVInfo.Reset();
	m_dMVInfo.Reset();

	if (!m_P3Base.AnalyzeMV(vA, m_MVInfo, m_bDrawOPNS) || (m_MVInfo.m_eType == GA_NONE))
	{
		return false;
	}

	if (bAnalyzeOnly)
	{
		return true;
	}
	else
	{
		return DrawMVInfo(m_MVInfo);
	}
}

//////////////////////////////////////////////////////////////////////
/// Draw Multivector

bool COGLFilterP3::DrawMV(const MultiV<double>& vX, bool bAnalyzeOnly)
{
	MultiV<double> vA(vX);

	vA.TinyToZero((double) m_fSensitivity);

	m_MVInfo.Reset();
	m_dMVInfo.Reset();

	if (!m_dP3Base.AnalyzeMV(vA, m_dMVInfo, m_bDrawOPNS) || (m_dMVInfo.m_eType == GA_NONE))
	{
		return false;
	}

	if (!CastMVInfoToFloat(m_MVInfo, m_dMVInfo))
	{
		return false;
	}

	if (bAnalyzeOnly)
	{
		return true;
	}
	else
	{
		return DrawMVInfo(m_MVInfo);
	}
}

//////////////////////////////////////////////////////////////////////
/// Draw MV Info

bool COGLFilterP3::DrawMVInfo(CMVInfo<float>& rInfo)
{
	COGLVertex pVex[3], pActVex[3];
	float fMag;
	//COGLColor Dark, Arrow;

	//m_VexList.Reset();

	//Dark = m_fBackFaceColFac * m_ActiveColor;
	//Arrow = m_fArrowColFac * m_ActiveColor;

	if (rInfo.m_eType == GA_POINT)
	{
		pVex[0] = rInfo.m_mVex[0].Data();

		m_pDrawBase->DrawPoint(pVex[0]);
	}
	else if (rInfo.m_eType == GA_DIR1D)
	{
		pVex[0].Set(0.0, 0.0, 0.0);
		pVex[1] = rInfo.m_mVex[0].Data();
		fMag    = rInfo.m_mVal[0];

		m_pDrawBase->DrawVector(pVex[0], fMag * pVex[1]);
	}
	else if (rInfo.m_eType == GA_LINE)
	{
		pVex[0] = rInfo.m_mVex[0].Data();
		pVex[1] = rInfo.m_mVex[1].Data();

		m_pDrawBase->DrawLine(pVex[0], pVex[1] * rInfo.m_mVal[0]);
	}
	else if (rInfo.m_eType == GA_DIR2D)
	{
		pVex[0] = rInfo.m_mVex[0].Data();
		pVex[1] = rInfo.m_mVex[1].Data();
		pVex[2].Set(0.0, 0.0, 0.0);
		fMag = 0.5f * rInfo.m_mVal[0];

		pActVex[0] = pVex[0] * fMag;
		pActVex[1] = pVex[1] * fMag;

		//Arrow.Apply();
		m_pDrawBase->DrawVector(-pActVex[0], pActVex[0]);
		//Arrow.Apply();
		m_pDrawBase->DrawVector(-pActVex[1], pActVex[1]);

		pActVex[0] = pVex[0] * (2.0f * fMag - 0.12f * 1.8f);
		pActVex[1] = pVex[1] * (2.0f * fMag - 0.12f * 1.8f);

		m_pDrawBase->DrawPlane(pVex[2], pActVex[0], pActVex[1], true);
	}
	else if (rInfo.m_eType == GA_PLANE)
	{
		fMag = (float) sqrt(rInfo.m_mVal[0] / m_fPi);

		pVex[0] = rInfo.m_mVex[0].Data();
		pVex[1] = rInfo.m_mVex[1].Data();
		pVex[2] = rInfo.m_mVex[2].Data();

		//m_pDrawBase->DrawPlane(pVex[0], pVex[1]*fMag, pVex[2]*fMag);
		m_pDrawBase->DrawDisk(pVex[0], pVex[1], pVex[2], fMag);
	}
	else if (rInfo.m_eType == GA_DIR3D)
	{
		pVex[0] = rInfo.m_mVex[0].Data();
		pVex[1] = rInfo.m_mVex[1].Data();
		pVex[2] = rInfo.m_mVex[2].Data();

		fMag = 0.5f * rInfo.m_mVal[0];

		pActVex[0] = pVex[0] * fMag;
		pActVex[1] = pVex[1] * fMag;
		pActVex[2] = pVex[2] * fMag;

		//Arrow.Apply();
		m_pDrawBase->DrawVector(-pActVex[0], pActVex[0]);
		//Arrow.Apply();
		m_pDrawBase->DrawVector(-pActVex[1], pActVex[1]);
		//Arrow.Apply();
		m_pDrawBase->DrawVector(-pActVex[2], pActVex[2]);

		fMag = float(pow(double(rInfo.m_mVal[0]), double(1.0 / 3.0))) - 0.12f * 1.8f;
		m_pDrawBase->DrawRhombus(fMag);
	}
	else if (rInfo.m_eType == GA_SPACE)
	{
		fMag = (float) pow(double(rInfo.m_mVal[0]), 1.0 / 3.0);

		if (rInfo.m_mVex.Count() == 3)
		{
			m_pDrawBase->DrawCube(fMag, rInfo.m_mVex[0].Data(), rInfo.m_mVex[1].Data(), rInfo.m_mVex[2].Data());
		}
		else
		{
			m_pDrawBase->DrawCube(fMag);
		}
	}
	else if (rInfo.m_eType == GA_ROTOR)
	{
		pVex[0].Set(0.0f, 0.0f, 0.0f);
		pVex[1] = rInfo.m_mVex[0].Data();	// Rot. Axis

		m_pDrawBase->DrawRotor(pVex[0], pVex[1], rInfo.m_mVal[0]);
	}

	return true;
}
