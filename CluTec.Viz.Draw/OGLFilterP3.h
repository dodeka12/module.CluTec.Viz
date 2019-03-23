////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLFilterP3.h
//
// summary:   Declares the ogl filter p 3 class
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

// OGLFilterPGA.h: Schnittstelle f�r die Klasse COGLFilterPGA.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLFILTERPGA_H__FD68843D_22F1_4BED_9292_87133E154BD4__INCLUDED_)
#define AFX_OGLFILTERPGA_H__FD68843D_22F1_4BED_9292_87133E154BD4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OGLMVFilterBase.h"
#include "OGLDrawBase.h"
#include "OGLVexListP3.h"



class CLUDRAW_API COGLFilterP3 : public COGLMVFilterBase  
{
public:
	COGLFilterP3();
	virtual ~COGLFilterP3();

	virtual bool SetMode(int iMode);
	virtual bool ResetModes();

	virtual bool DrawMV(const MultiV<double>& vA, bool bAnalyzeOnly = false);
	virtual bool DrawMV(const MultiV<float>& vA, bool bAnalyzeOnly = false);
	bool DrawMVInfo(CMVInfo<float>& rInfo);

protected:
	bool m_bDrawOPNS;
	//COGLVexListP3 m_VexList;
};

#endif // !defined(AFX_OGLFILTERPGA_H__FD68843D_22F1_4BED_9292_87133E154BD4__INCLUDED_)
