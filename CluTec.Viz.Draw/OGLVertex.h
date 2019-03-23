////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLVertex.h
//
// summary:   Declares the ogl vertex class
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

// OGLVertex.h: Schnittstelle f�r die Klasse COGLVertex.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLVERTEX_H__B634C808_40D5_4BE1_8E64_8AC4B1B21B65__INCLUDED_)
	#define AFX_OGLVERTEX_H__B634C808_40D5_4BE1_8E64_8AC4B1B21B65__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif	// _MSC_VER > 1000

#include "CluTec.Viz.Fltk\Fl_math.h"
#include <algorithm>

	class CLUDRAW_API COGLVertex
	{
	public:

		COGLVertex();
		COGLVertex(const COGLVertex& rVex);
		COGLVertex(float fX, float fY, float fZ);
		COGLVertex(const CPointND<float, 3>& Point);
		COGLVertex(const CPointND<double, 3>& Point);

		operator float*() { return m_pfVex; }
		operator const float*() const { return m_pfVex; }
		float& operator[](int i) { if ((i >= 0) && (i < 3)) { return m_pfVex[i]; } else{ return m_pfVex[0]; } }

		float Mag() const
		{ return (float) sqrt(m_pfVex[0] * m_pfVex[0] + m_pfVex[1] * m_pfVex[1] + m_pfVex[2] * m_pfVex[2]); }

		// Check that vertex coords. are within allowed bounds.
		COGLVertex& Clamp();

		// Normalize vector
		COGLVertex& Norm()
		{ float fMag = Mag(); if (fMag != 0.0f) { *this /= fMag; } return *this; }

		COGLVertex& operator=(const COGLVertex& rVex)
		{ memcpy(m_pfVex, rVex.m_pfVex, 3 * sizeof(float)); return *this; }

		COGLVertex& operator=(const float* pfVal)
		{ memcpy(m_pfVex, pfVal, 3 * sizeof(float)); return *this; }

		void Set(float fX, float fY, float fZ)
		{ m_pfVex[0] = fX; m_pfVex[1] = fY; m_pfVex[2] = fZ; }

		COGLVertex& operator-=(const float fVal)
		{
			for (int i = 0; i < 3; i++)
			{
				m_pfVex[i] -= fVal;
			}

			return *this;
		}

		COGLVertex& operator+=(const float fVal)
		{
			for (int i = 0; i < 3; i++)
			{
				m_pfVex[i] += fVal;
			}

			return *this;
		}

		COGLVertex& operator*=(const float fVal)
		{
			for (int i = 0; i < 3; i++)
			{
				m_pfVex[i] *= fVal;
			}

			return *this;
		}

		COGLVertex& operator/=(const float fVal)
		{
			for (int i = 0; i < 3; i++)
			{
				m_pfVex[i] /= fVal;
			}

			return *this;
		}

		friend COGLVertex operator+(const COGLVertex& rVex1, const COGLVertex& rVex2);
		friend COGLVertex operator-(const COGLVertex& rVex1, const COGLVertex& rVex2);
		friend float operator*(const COGLVertex& rVex1, const COGLVertex& rVex2);
		friend COGLVertex operator*(const float fVal, const COGLVertex& rVex);
		friend COGLVertex operator*(const COGLVertex& rVex, const float fVal);
		friend COGLVertex operator/(const COGLVertex& rVex, const float fVal);
		friend COGLVertex operator-(const COGLVertex& rVex);

		// Vector cross product
		friend COGLVertex operator^(const COGLVertex& rVex1, const COGLVertex& rVex2);

	protected:

		float m_pfVex[3];
	};

#endif	// !defined(AFX_OGLVERTEX_H__B634C808_40D5_4BE1_8E64_8AC4B1B21B65__INCLUDED_)
