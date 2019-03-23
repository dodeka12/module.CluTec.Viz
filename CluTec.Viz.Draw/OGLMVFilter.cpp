////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLMVFilter.cpp
//
// summary:   Implements the oglmv filter class
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

// OGLMVFilter.cpp: Implementierung der Klasse COGLMVFilter.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OGLMVFilter.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLMVFilter::COGLMVFilter()
{
}

COGLMVFilter::~COGLMVFilter()
{

}


//////////////////////////////////////////////////////////////////////
/// Set Drawing Base

void COGLMVFilter::SetDrawBase( COGLDrawBase *pDrawBase )
{
	COGLMVFilterBase::SetDrawBase( pDrawBase );

	m_E3Filter.SetDrawBase( pDrawBase );
	m_P3Filter.SetDrawBase( pDrawBase );
	m_N3Filter.SetDrawBase( pDrawBase );
	m_El2Filter.SetDrawBase( pDrawBase );
}



//////////////////////////////////////////////////////////////////////
/// DrawMV

bool COGLMVFilter::DrawMV(const MultiV<float>& vA, bool bAnalyzeOnly)
{
	switch(vA.BasePtr()->BaseID())
	{
	case ID_E3GA:
		m_iLastMode = ID_E3GA;
		return m_E3Filter.DrawMV(vA, bAnalyzeOnly);

	case ID_PGA:
		m_iLastMode = ID_PGA;
		return m_P3Filter.DrawMV(vA, bAnalyzeOnly);

	case ID_CONFGA:
		m_iLastMode = ID_CONFGA;
		return m_N3Filter.DrawMV(vA, bAnalyzeOnly);

	case ID_EL2GA:
		m_iLastMode = ID_EL2GA;
		return m_El2Filter.DrawMV(vA, bAnalyzeOnly);

	default:
		return false;
	}
}

//////////////////////////////////////////////////////////////////////
/// DrawMV

bool COGLMVFilter::DrawMV(const MultiV<double>& vA, bool bAnalyzeOnly)
{
	switch(vA.BasePtr()->BaseID())
	{
	case ID_E3GA:
		m_iLastMode = ID_E3GA;
		return m_E3Filter.DrawMV(vA, bAnalyzeOnly);

	case ID_PGA:
		m_iLastMode = ID_PGA;
		return m_P3Filter.DrawMV(vA, bAnalyzeOnly);

	case ID_CONFGA:
		m_iLastMode = ID_CONFGA;
		return m_N3Filter.DrawMV(vA, bAnalyzeOnly);

	case ID_EL2GA:
		m_iLastMode = ID_EL2GA;
		return m_El2Filter.DrawMV(vA, bAnalyzeOnly);

	default:
		return false;
	}
}

//////////////////////////////////////////////////////////////////////
/// Reset Modes

bool COGLMVFilter::ResetModes()
{
	COGLMVFilterBase::ResetModes();
	
	m_E3Filter.ResetModes();
	m_P3Filter.ResetModes();
	m_N3Filter.ResetModes();
	m_El2Filter.ResetModes();

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Mode

bool COGLMVFilter::SetMode(int iMode)
{
	switch(iMode >> 16)
	{
	case ID_E3GA:
		return m_E3Filter.SetMode(iMode);

	case ID_PGA:
		return m_P3Filter.SetMode(iMode);

	case ID_CONFGA:
		return m_N3Filter.SetMode(iMode);

	case ID_EL2GA:
		return m_El2Filter.SetMode(iMode);

	case ID_ALL:
		m_E3Filter.SetMode(iMode);
		m_P3Filter.SetMode(iMode);
		m_N3Filter.SetMode(iMode);
		m_El2Filter.SetMode(iMode);
		return true;

	default:
		return false;
	}
}


//////////////////////////////////////////////////////////////////////
// Set Sensitivity

void COGLMVFilter::SetSensitivity(double dSens)
{
	COGLMVFilterBase::SetSensitivity(dSens);

	m_E3Filter.SetSensitivity(dSens);
	m_P3Filter.SetSensitivity(dSens);
	m_N3Filter.SetSensitivity(dSens);
	m_El2Filter.SetSensitivity(dSens);
}


//////////////////////////////////////////////////////////////////////
/// Get MV Info

CMVInfo<float> COGLMVFilter::GetMVInfo()
{
	switch(m_iLastMode)
	{
	case ID_E3GA:
		return m_E3Filter.GetMVInfo();

	case ID_PGA:
		return m_P3Filter.GetMVInfo();

	case ID_CONFGA:
		return m_N3Filter.GetMVInfo();

	case ID_EL2GA:
		return m_El2Filter.GetMVInfo();

	default:
		return m_MVInfo;
	}
}


//////////////////////////////////////////////////////////////////////
/// Get MV Info

void COGLMVFilter::GetMVInfo(CMVInfo<float>& rInfo)
{
	switch(m_iLastMode)
	{
	case ID_E3GA:
		m_E3Filter.GetMVInfo(rInfo);
		break;

	case ID_PGA:
		m_P3Filter.GetMVInfo(rInfo);

	case ID_CONFGA:
		m_N3Filter.GetMVInfo(rInfo);
		break;

	case ID_EL2GA:
		m_El2Filter.GetMVInfo(rInfo);
		break;

	default:
		rInfo = m_MVInfo;
		break;
	}

}


//////////////////////////////////////////////////////////////////////
/// Get MV Info

void COGLMVFilter::GetMVInfo(CMVInfo<double>& rInfo)
{
	switch(m_iLastMode)
	{
	case ID_E3GA:
		m_E3Filter.GetMVInfo(rInfo);
		break;

	case ID_PGA:
		m_P3Filter.GetMVInfo(rInfo);
		break;

	case ID_CONFGA:
		m_N3Filter.GetMVInfo(rInfo);
		break;

	case ID_EL2GA:
		m_El2Filter.GetMVInfo(rInfo);
		break;

	default:
		rInfo = m_dMVInfo;
		break;
	}

}

