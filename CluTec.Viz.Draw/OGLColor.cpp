////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLColor.cpp
//
// summary:   Implements the ogl color class
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

// OGLColor.cpp: Implementierung der Klasse COGLColor.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OGLColor.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLColor::COGLColor()
{
	m_sTypeName = "Color";
	m_pfCol[0]  = 0.0f;
	m_pfCol[1]  = 0.0f;
	m_pfCol[2]  = 0.0f;
	m_pfCol[3]  = 0.0f;
}

COGLColor::COGLColor(const COGLColor& rCol)
{
	m_sTypeName = "Color";

	*this = rCol;
}

COGLColor::COGLColor(float fR, float fG, float fB, float fA)
{
	m_sTypeName = "Color";

	Set(fR, fG, fB, fA);
}

//////////////////////////////////////////////////////////////////////
// Copy Operator

COGLColor& COGLColor::operator=(const COGLColor& rCol)
{
	memcpy(m_pfCol, rCol.m_pfCol, 4 * sizeof(float));

	return *this;
}

//////////////////////////////////////////////////////////////////////
// Copy Operator

COGLColor& COGLColor::operator=(const float* pfCol)
{
	memcpy(m_pfCol, pfCol, 4 * sizeof(float));

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Apply Color

bool COGLColor::Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData& rData)
{
	if (eMode == COGLBaseElement::PICK)
	{
		return true;
	}

	glColor4fv(m_pfCol);
	memcpy(rData.pfCurColor, m_pfCol, 4 * sizeof(float));

/*
        float pfDiff[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        pfDiff[3] = m_pfCol[3];
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_pfCol);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, pfDiff);
*/
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Renorm Color
void COGLColor::Renorm()
{
	float fMax = 0.0;

	for (int i = 0; i < 3; i++)
	{
		if (fMax < m_pfCol[i])
		{
			fMax = m_pfCol[i];
		}
	}

	if (fMax > 1.0f)
	{
		for (int i = 0; i < 3; i++)
		{
			m_pfCol[i] /= fMax;
		}
	}

	if (m_pfCol[3] < 0.0)
	{
		m_pfCol[3] = 0.0;
	}
	else if (m_pfCol[3] > 1.0)
	{
		m_pfCol[3] = 1.0;
	}
}

//////////////////////////////////////////////////////////////////////
/// Clamp Color
void COGLColor::Clamp()
{
	for (int i = 0; i < 4; i++)
	{
		if (m_pfCol[i] < 0.0f)
		{
			m_pfCol[i] = 0.0f;
		}
		else if (m_pfCol[i] > 1.0f)
		{
			m_pfCol[i] = 1.0f;
		}
	}
}

//////////////////////////////////////////////////////////////////////
/// Scale Colours

COGLColor operator*(float fVal, const COGLColor& rCol)
{
//	if (fVal < 0.0)
//		return rCol;

	COGLColor NewCol;

//	float fMax = 0.0;
	if ((rCol.m_pfCol[0] == 0) &&
	    (rCol.m_pfCol[1] == 0) &&
	    (rCol.m_pfCol[2] == 0))
	{
		NewCol.Set(0, 0, 0, rCol.m_pfCol[3] * fVal);
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			NewCol.m_pfCol[i] = rCol.m_pfCol[i] * fVal;
		}

		NewCol.m_pfCol[3] = rCol.m_pfCol[3];
	}

	return NewCol;
}

//////////////////////////////////////////////////////////////////////
/// Multiply Colors

COGLColor operator*(const COGLColor& rCol1, const COGLColor& rCol2)
{
	int i;
	COGLColor NewCol;

	for (i = 0; i < 4; i++)
	{
		NewCol.m_pfCol[i] = rCol1.m_pfCol[i] * rCol2.m_pfCol[i];
	}

	return NewCol;
}

//////////////////////////////////////////////////////////////////////
/// Divide Colors

COGLColor operator/(const COGLColor& rCol1, const COGLColor& rCol2)
{
	int i;
	COGLColor NewCol;

	for (i = 0; i < 4; i++)
	{
		if (rCol2.m_pfCol[i] == 0.0f)
		{
			if (rCol1.m_pfCol[i] < 0.0f)
			{
				NewCol.m_pfCol[i] = -FLT_MAX;
			}
			else
			{
				NewCol.m_pfCol[i] = FLT_MAX;
			}
		}
		else
		{
			NewCol.m_pfCol[i] = rCol1.m_pfCol[i] / rCol2.m_pfCol[i];
		}
	}

	return NewCol;
}

//////////////////////////////////////////////////////////////////////
/// Divide Colors

COGLColor operator/(float fVal, const COGLColor& rCol2)
{
	int i;
	COGLColor NewCol;

	for (i = 0; i < 4; i++)
	{
		if (rCol2.m_pfCol[i] == 0.0f)
		{
			if (fVal < 0.0f)
			{
				NewCol.m_pfCol[i] = -FLT_MAX;
			}
			else
			{
				NewCol.m_pfCol[i] = FLT_MAX;
			}
		}
		else
		{
			NewCol.m_pfCol[i] = fVal / rCol2.m_pfCol[i];
		}
	}

	return NewCol;
}

//////////////////////////////////////////////////////////////////////
/// Divide by scalar

COGLColor operator/(const COGLColor& rCol, float fVal)
{
	int i;
	COGLColor NewCol;

	if (fVal == 0.0)
	{
		for (i = 0; i < 4; i++)
		{
			if (rCol.m_pfCol[i] < 0.0f)
			{
				NewCol.m_pfCol[i] = -FLT_MAX;
			}
			else
			{
				NewCol.m_pfCol[i] = FLT_MAX;
			}
		}
	}
	else
	{
		for (i = 0; i < 4; i++)
		{
			NewCol.m_pfCol[i] = rCol.m_pfCol[i] / fVal;
		}
	}

	return NewCol;
}

//////////////////////////////////////////////////////////////////////
/// Add Colors

COGLColor operator+(const COGLColor& rCol1, const COGLColor& rCol2)
{
	int i;
	COGLColor NewCol;

	for (i = 0; i < 4; i++)
	{
		NewCol.m_pfCol[i] = rCol1.m_pfCol[i] + rCol2.m_pfCol[i];
	}

	return NewCol;
}

//////////////////////////////////////////////////////////////////////
/// Subtract Colors

COGLColor operator-(const COGLColor& rCol1, const COGLColor& rCol2)
{
	int i;
	COGLColor NewCol;

	for (i = 0; i < 4; i++)
	{
		NewCol.m_pfCol[i] = rCol1.m_pfCol[i] - rCol2.m_pfCol[i];
	}

	return NewCol;
}

//////////////////////////////////////////////////////////////////////
/// Negate Color

COGLColor operator-(const COGLColor& rCol1)
{
	int i;
	COGLColor NewCol;

	for (i = 0; i < 4; i++)
	{
		NewCol.m_pfCol[i] = -rCol1.m_pfCol[i];
	}

	return NewCol;
}

//////////////////////////////////////////////////////////////////////
// Add

COGLColor& COGLColor::operator+=(const COGLColor& rCol)
{
	int i;

	for (i = 0; i < 4; i++)
	{
		m_pfCol[i] += rCol.m_pfCol[i];
	}

	return *this;
}

//////////////////////////////////////////////////////////////////////
// Subtract

COGLColor& COGLColor::operator-=(const COGLColor& rCol)
{
	int i;

	for (i = 0; i < 4; i++)
	{
		m_pfCol[i] -= rCol.m_pfCol[i];
	}

	return *this;
}

//////////////////////////////////////////////////////////////////////
// Divide

COGLColor& COGLColor::operator/=(float fVal)
{
	int i;

	for (i = 0; i < 3; i++)
	{
		m_pfCol[i] /= fVal;
	}

	return *this;
}
