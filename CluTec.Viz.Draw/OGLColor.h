////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLColor.h
//
// summary:   Declares the ogl color class
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

// OGLColor.h: Schnittstelle f�r die Klasse COGLColor.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLCOLOR_H__ECC38C81_0DD0_4C4F_B77D_86F72B5AA0DE__INCLUDED_)
#define AFX_OGLCOLOR_H__ECC38C81_0DD0_4C4F_B77D_86F72B5AA0DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "OGLBaseElement.h"
#include "CluTec.Viz.Base\CLUException.h"


class CLUDRAW_API COGLColor  : public COGLBaseElement
{
public:
	COGLColor();
	COGLColor(const COGLColor& rCol);
	COGLColor(float fR, float fG, float fB, float fA = 1.0f);

	virtual COGLBaseElement* Copy()
	{
		return (COGLBaseElement *) new COGLColor( *this );
	}

	COGLColor& operator= (const COGLColor& rCol);
	COGLColor& operator= (const float *pfCol);

	void Set(float fR, float fG, float fB, float fA = 1.0f)
	{ m_pfCol[0] = fR; m_pfCol[1] = fG; m_pfCol[2] = fB; m_pfCol[3] = fA; }

	void Alpha(float fA) 
	{ m_pfCol[3] = fA; }

	float Alpha() const
	{ return m_pfCol[3]; }

	bool operator== (const COGLColor& rCol)
	{ 
		if (m_pfCol[0] == rCol.m_pfCol[0] &&
			m_pfCol[1] == rCol.m_pfCol[1] &&
			m_pfCol[2] == rCol.m_pfCol[2])
			return true;
		return false;
	}

	friend COGLColor operator* (float fVal, const COGLColor &rCol);
	friend COGLColor operator/ (float fVal, const COGLColor &rCol);
	friend COGLColor operator/ (const COGLColor &rCol, float fVal);
	friend COGLColor operator* (const COGLColor& rCol1, const COGLColor& rCol2);
	friend COGLColor operator/ (const COGLColor& rCol1, const COGLColor& rCol2);
	friend COGLColor operator+ (const COGLColor& rCol1, const COGLColor& rCol2);
	friend COGLColor operator- (const COGLColor& rCol1, const COGLColor& rCol2);
	friend COGLColor operator- (const COGLColor& rCol1);

	COGLColor& operator+= (const COGLColor& rCol);
	COGLColor& operator-= (const COGLColor& rCol);

	COGLColor& operator/= (float fVal);

	float& operator[] (int iCol)
	{ 
		if (iCol >= 0 && iCol <= 3) return m_pfCol[iCol];
		else throw CCluAssertion( __FILE__, __LINE__ );
	}

	const float& operator[] (int iCol) const
	{ 
		if (iCol >= 0 && iCol <= 3) return m_pfCol[iCol];
		else throw CCluAssertion( __FILE__, __LINE__ );
	}

	void Renorm();
	void Clamp();

	bool Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData &rData);

	float* Data() { return m_pfCol; }
	const float* Data() const { return m_pfCol; }

protected:
	float m_pfCol[4];
};

#endif // !defined(AFX_OGLCOLOR_H__ECC38C81_0DD0_4C4F_B77D_86F72B5AA0DE__INCLUDED_)
