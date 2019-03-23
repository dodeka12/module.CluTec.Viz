////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLMVFilter.h
//
// summary:   Declares the oglmv filter class
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

// OGLMVFilter.h: Schnittstelle f�r die Klasse COGLMVFilter.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLMVFILTER_H__BBFD3218_CC4C_4518_8232_DCEE325D9EB2__INCLUDED_)
#define AFX_OGLMVFILTER_H__BBFD3218_CC4C_4518_8232_DCEE325D9EB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OGLMVFilterBase.h"
#include "OGLFilterE3.h"
#include "OGLFilterP3.h"
#include "OGLFilterN3.h"
#include "OGLFilterEl2.h"


class CLUDRAW_API COGLMVFilter : public COGLMVFilterBase  
{
public:
	COGLMVFilter();
	virtual ~COGLMVFilter();

	void SetDrawBase( COGLDrawBase *pDrawBase );

	virtual bool DrawMV(const MultiV<float>& vA, bool bAnalyzeOnly = false);
	virtual bool DrawMV(const MultiV<double>& vA, bool bAnalyzeOnly = false);
	virtual bool ResetModes();
	virtual bool SetMode(int iMode);
		
	void SetSensitivity(double dSens);

	CMVInfo<float> GetMVInfo();
	void GetMVInfo(CMVInfo<float> &rInfo);
	void GetMVInfo(CMVInfo<double> &rInfo);

	int GetLastMVID() { return m_iLastMode; }

public:
	COGLFilterE3 m_E3Filter;
	COGLFilterP3 m_P3Filter;
	COGLFilterN3 m_N3Filter;
	COGLFilterEl2 m_El2Filter;

protected:
	int m_iLastMode;
};

#endif // !defined(AFX_OGLMVFILTER_H__BBFD3218_CC4C_4518_8232_DCEE325D9EB2__INCLUDED_)
