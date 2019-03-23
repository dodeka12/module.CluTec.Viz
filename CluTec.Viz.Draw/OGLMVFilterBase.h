////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLMVFilterBase.h
//
// summary:   Declares the oglmv filter base class
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

// OGLFilterBase.h: Schnittstelle f�r die Klasse COGLFilterBase.
//
// Base Class for OpenGL filters. Used to display e.g. Multivectors
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLMVFILTERBASE_H__3D0F9FE0_C4C7_4C2C_9881_815CE941C428__INCLUDED_)
#define AFX_OGLMVFILTERBASE_H__3D0F9FE0_C4C7_4C2C_9881_815CE941C428__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OGLColor.h"
#include "OGLMaterial.h"
#include "OGLVertex.h"
#include "OGLVertexList.h"
#include "OGLDisplayList.h"
#include "OGLBaseElementList.h"

#include "OGLDrawBase.h"
#include "OGLBERepository.h"

#define MODE_OPNS	(ID_ALL << 16) + 0x0001
#define MODE_IPNS	(ID_ALL << 16) + 0x0002
#define DRAW_POINT_AS_DOT		(ID_ALL << 16) + 0x0100
#define DRAW_POINT_AS_SPHERE	(ID_ALL << 16) + 0x0101
#define DRAW_POINT_AS_STD		DRAW_POINT_AS_DOT


#define MAT_STD		0
#define MAT_PLANE	1
#define MAT_CUBE	2
#define MAT_SPHERE	3
#define MAT_ROTOR	4
#define MAT_CIRCLE	5
#define MAT_LINE	6
#define MAT_POINT	7
#define MAT_VECTOR	8
#define MAT_RHOMBUS	9
#define MAT_COUNT	10

class CLUDRAW_API COGLMVFilterBase  
{
public:
	enum TDrawPointType
	{
		PntStd = 0, PntDot = 0, PntSphere
	};

	struct STriplet
	{
	public:
		int& operator[] (int iPos) { return m_pData[iPos]; }
		
		int* Data() { return m_pData; }

	protected:
		int m_pData[3];
	};

	typedef int TTriplet[3];

public:
	COGLMVFilterBase();
	virtual ~COGLMVFilterBase();

	void SetDrawBase( COGLDrawBase *pDrawBase )
	{ m_pDrawBase = pDrawBase; }


	COGLMVFilterBase& operator<< (int iMode)
	{ SetMode(iMode); return *this; }

	COGLMVFilterBase& operator<< (COGLColor& rCol)
	{ if (m_pDrawBase) m_pDrawBase->SetColor(rCol); return *this; }

	COGLMVFilterBase& operator<< (const MultiV<double>& vA)
	{ DrawMV(vA); return *this; }

	COGLMVFilterBase& operator<< (const MultiV<float>& vA)
	{ DrawMV(vA); return *this; }

	COGLMVFilterBase& operator<< (MemObj<MultiV<double> >& mvA)
	{ DrawListOfMV(mvA); return *this; }

	COGLMVFilterBase& operator<< (MemObj<MultiV<float> >& mvA)
	{ DrawListOfMV(mvA); return *this; }

	virtual bool DrawMV(const MultiV<double>& vA, bool bAnalyzeOnly = false) { return true; }
	virtual bool DrawMV(const MultiV<float>& vA, bool bAnalyzeOnly = false) { return true; }
	
	virtual bool DrawListOfMV(MemObj<MultiV<double> >& mvA);
	virtual bool DrawListOfMV(MemObj<MultiV<float> >& mvA);

	virtual bool SetMode(int iMode) { return false; }
	virtual bool ResetModes() { return true; }

	void SetSensitivity(double dSens) { m_fSensitivity = (float) dSens; m_dSensitivity = dSens;}
	float GetFloatSens() { return m_fSensitivity; }
	double GetDoubleSens() { return m_dSensitivity; }

	float Round(float fVal) 
	{ return ::Round(fVal, m_fSensitivity); }

	double Round(double dVal) 
	{ return ::Round(dVal, m_dSensitivity); }

	CMVInfo<float> GetMVInfo() { return m_MVInfo; }
	void GetMVInfo(CMVInfo<float> &rInfo) { rInfo = m_MVInfo; }
	void GetMVInfo(CMVInfo<double> &rInfo) { rInfo = m_dMVInfo; }

protected:
	bool CastMVInfoToFloat(CMVInfo<float>& fMVInfo, CMVInfo<double>& dMVInfo);

protected:
	COGLDrawBase *m_pDrawBase;

	E3GA<float> m_E3Base;
	PGA<float> m_P3Base;
	ConfGA<float> m_N3Base;
	El2GA<float> m_El2Base;

	E3GA<double> m_dE3Base;
	PGA<double> m_dP3Base;
	ConfGA<double> m_dN3Base;
	El2GA<double> m_dEl2Base;

	CMVInfo<float> m_MVInfo;
	CMVInfo<double> m_dMVInfo;

	float m_fPi;
	float m_fSensitivity; // Used as TinyToZero parameter
	double m_dSensitivity;

	bool m_bShowImaginaryObjects;
};

#endif // !defined(AFX_OGLMVFILTERBASE_H__3D0F9FE0_C4C7_4C2C_9881_815CE941C428__INCLUDED_)
