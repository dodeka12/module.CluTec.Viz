////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLVertex.cpp
//
// summary:   Implements the ogl vertex class
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

// OGLVertex.cpp: Implementierung der Klasse COGLVertex.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OGLVertex.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLVertex::COGLVertex()
{
	memset(m_pfVex, 0, 3*sizeof(float));
}

COGLVertex::COGLVertex(const COGLVertex& rVex)
{
	*this = rVex;
}

COGLVertex::COGLVertex( float fX, float fY, float fZ )
{
	m_pfVex[0] = fX;
	m_pfVex[1] = fY;
	m_pfVex[2] = fZ;
}

COGLVertex::COGLVertex( const CPointND<float,3>& Point )
{
	memcpy( m_pfVex, Point.Data(), 3*sizeof(float) );
}

COGLVertex::COGLVertex( const CPointND<double,3>& Point )
{
	m_pfVex[0] = float( Point[0] );
	m_pfVex[1] = float( Point[1] );
	m_pfVex[2] = float( Point[2] );
}

//////////////////////////////////////////////////////////////////////
/// COGLVertex Functions

COGLVertex& COGLVertex::Clamp()
{
	int i;

	for(i=0;i<3;i++)
	{
		if (m_pfVex[i] > OGL_VEX_COORD_MAX)
			m_pfVex[i] = OGL_VEX_COORD_MAX;
		else if (m_pfVex[i] < -OGL_VEX_COORD_MAX)
			m_pfVex[i] = -OGL_VEX_COORD_MAX;
	}

	return *this;
}


COGLVertex operator+ (const COGLVertex& rVex1, const COGLVertex& rVex2)
{
	int i;
	COGLVertex Vex;

	for(i=0;i<3;i++)
	{
		Vex.m_pfVex[i] = rVex1.m_pfVex[i] + rVex2.m_pfVex[i];
	}

	return Vex;
}

COGLVertex operator- (const COGLVertex& rVex1, const COGLVertex& rVex2)
{
	int i;
	COGLVertex Vex;

	for(i=0;i<3;i++)
	{
		Vex.m_pfVex[i] = rVex1.m_pfVex[i] - rVex2.m_pfVex[i];
	}

	return Vex;
}

COGLVertex operator* (const float fVal, const COGLVertex& rVex)
{
	int i;
	COGLVertex Vex;

	for(i=0;i<3;i++)
	{
		Vex.m_pfVex[i] = fVal * rVex.m_pfVex[i];
	}

	return Vex;
}

COGLVertex operator* (const COGLVertex& rVex, const float fVal)
{
	int i;
	COGLVertex Vex;

	for(i=0;i<3;i++)
	{
		Vex.m_pfVex[i] = fVal * rVex.m_pfVex[i];
	}

	return Vex;
}

COGLVertex operator/ (const COGLVertex& rVex, const float fVal)
{
	if (fVal == 0.0f)
		return rVex;

	int i;
	COGLVertex Vex;

	for(i=0;i<3;i++)
	{
		Vex.m_pfVex[i] = rVex.m_pfVex[i] / fVal;
	}

	return Vex;
}

COGLVertex operator- (const COGLVertex& rVex)
{
	int i;
	COGLVertex Vex;

	for(i=0;i<3;i++)
	{
		Vex.m_pfVex[i] = -rVex.m_pfVex[i];
	}

	return Vex;
}


// Vector scalar product
float operator* (const COGLVertex& rVex1, const COGLVertex& rVex2)
{
	return ( rVex1[0]*rVex2[0] + rVex1[1]*rVex2[1] + rVex1[2]*rVex2[2] );
}

// Vector cross product
COGLVertex operator^ (const COGLVertex& rVex1, const COGLVertex& rVex2)
{
	COGLVertex Vex;

	Vex[0] = rVex1[1]*rVex2[2] - rVex1[2]*rVex2[1];
	Vex[1] = rVex1[2]*rVex2[0] - rVex1[0]*rVex2[2];
	Vex[2] = rVex1[0]*rVex2[1] - rVex1[1]*rVex2[0];

	return Vex;
}

