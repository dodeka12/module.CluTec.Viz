////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLFilterE3.h
//
// summary:   Declares the ogl filter e 3 class
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

// OGLFilterE3GA.h: Schnittstelle f�r die Klasse COGLFilterE3GA.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLFILTERE3GA_H__2557741E_7C0C_48B2_9343_592EAF6B11C0__INCLUDED_)
#define AFX_OGLFILTERE3GA_H__2557741E_7C0C_48B2_9343_592EAF6B11C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OGLMVFilterBase.h"
#include "OGLVexListE3.h"


#define E3M_DRAW_VEC_AS_LINE	(ID_E3GA << 16) + 0x0100
#define E3M_DRAW_VEC_AS_ARROW	(ID_E3GA << 16) + 0x0101
#define E3M_DRAW_VEC_AS_POINT	(ID_E3GA << 16) + 0x0102
#define E3M_DRAW_VEC_AS_STD		E3M_DRAW_VEC_AS_LINE

#define E3M_DRAW_PLANE_AS_RECT	(ID_E3GA << 16) + 0x0200
#define E3M_DRAW_PLANE_AS_DISK	(ID_E3GA << 16) + 0x0201
#define E3M_DRAW_PLANE_AS_STD	E3M_DRAW_PLANE_AS_DISK


class CLUDRAW_API COGLFilterE3 : public COGLMVFilterBase  
{
public:
	enum TDrawVecType
	{
		StdVec = 0, Line = 0, Arrow, Point
	};

	enum TDrawPlaneType
	{
		StdPlane = 0, Disk = 0, Rectangle
	};

public:
	COGLFilterE3();
	virtual ~COGLFilterE3();

	virtual bool SetMode(int iMode);
	virtual bool ResetModes();

	virtual bool DrawMV(const MultiV<float>& vA, bool bAnalyzeOnly = false);
	virtual bool DrawMV(const MultiV<double>& vA, bool bAnalyzeOnly = false);

	bool DrawMVInfo(CMVInfo<float>& rInfo);

protected:
	bool m_bDrawOPNS;
	TDrawVecType m_eDrawVecType;
	TDrawPlaneType m_eDrawPlaneType;

	//COGLVexListE3 m_VexList;
};

#endif // !defined(AFX_OGLFILTERE3GA_H__2557741E_7C0C_48B2_9343_592EAF6B11C0__INCLUDED_)
