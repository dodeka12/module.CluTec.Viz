////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLBaseTransform.cpp
//
// summary:   Implements the ogl base transform class
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

// Class written by Vladimir Banarer, 3.12.2001.


// OGLBaseTransform.cpp: Implementierung der Klasse COGLBaseTransform.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h" 
#include "OGLBaseTransform.h"
#include "CluTec.Viz.Fltk\Fl_math.h"


double COGLBaseTransform::m_dPi = acos(-1.0);
double COGLBaseTransform::m_dRadPerDeg = COGLBaseTransform::m_dPi / 180.0;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLBaseTransform::COGLBaseTransform()
{
	SetTransformation(0,0,0,0,0,0,0);
}

COGLBaseTransform::~COGLBaseTransform()
{
	
}

//////////////////////////////////////////////////////////////////////
/// Set Translation

bool COGLBaseTransform::SetTranslation(float TX, float TY, float TZ)
{
	m_fTX = TX; 
	m_fTY = TY; 
	m_fTZ = TZ;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Transformation

bool COGLBaseTransform::SetRotation(float RAngle, float RX, float RY, float RZ)
{
	m_fRAngle = RAngle; 
	m_fRX = RX; 
	m_fRY = RY; 
	m_fRZ = RZ; 

	return true;
}

bool COGLBaseTransform::SetRotation(float RXAngle, float RYAngle, float RZAngle)
{
	double ca, sa, cb, sb, cc, sc, t;
	double st;

	m_dPi = acos(-1.0);

	ca = cos(RXAngle*m_dRadPerDeg); sa = sin(RXAngle*m_dRadPerDeg);
	cb = cos(RYAngle*m_dRadPerDeg); sb = sin(RYAngle*m_dRadPerDeg);
	cc = cos(RZAngle*m_dRadPerDeg); sc = sin(RZAngle*m_dRadPerDeg);


	t = acos((cb*cc + (ca*cc + sa*sb*sc) + ca*cb -1)/2.0);
	m_fRAngle = float(t / m_dRadPerDeg);
	st = sin(t);
	if(st != 0)
	{
		m_fRX = (float) (0.5/st*(sa*cb +sa*cc - ca*sb*sc)); 
		m_fRY = (float) (0.5/st*(sa*sc + ca*sb*cc - sb));
		m_fRZ = (float) (0.5/st*(cb*sc + ca*sc - sa*sb*cc));
	}
	else
	{
		m_fRX = 0; m_fRY = 0; m_fRZ = 0;
	}

	return true;
}

bool COGLBaseTransform::Apply(int iMode, void *pvData)
{
	glRotatef(m_fRAngle, m_fRX, m_fRY, m_fRZ);
	glTranslatef(m_fTX, m_fTY, m_fTZ);

	return true;
}

