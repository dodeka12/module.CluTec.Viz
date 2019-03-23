////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      ExMinFunc.cpp
//
// summary:   Implements the ex minimum function class
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

// ExMinFunc.cpp: Implementierung der Klasse ExMinFunc.
//
//////////////////////////////////////////////////////////////////////


#include<math.h>

#include "ExMinFunc.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

ExMinFunc::ExMinFunc(double a1, double a2, double a3)
{
	a[0] = a1;
	a[1] = a2;
	a[2] = a3;

	m_iDim = 3; // 2 dimensional function
	m_iMaxDeriv = 1; // highest possible derivative
}

ExMinFunc::~ExMinFunc()
{

}


//////////////////////////////////////////////////////////////////////
// Minimization function
//////////////////////////////////////////////////////////////////////

// returns true if no error occured, otherwise false.
// deriv: derivative order, i.e. 0 is function, 1 is 1st deriv. etc
// p: pointer to function parameters
// v: return value or values
bool ExMinFunc::Func(uint deriv, double *p, double *v)
{
	switch(deriv)
	{
	case 0:
		return MinFunc(p, v);

	case 1:
		return MinFuncD1(p, v);

	default:
		return false;
	}
}

//////////////////////////////////////////////////////////////////////
// Minimization function
//////////////////////////////////////////////////////////////////////

bool ExMinFunc::MinFunc(double *p, double *v)
{
	*v = pow(p[0] - a[0], 2.0) * pow(p[1] - a[1], 2.0) * pow(p[2]-a[2], 2.0);

	return true;
}


//////////////////////////////////////////////////////////////////////
// 1st Derivative of Minimization function
//////////////////////////////////////////////////////////////////////

bool ExMinFunc::MinFuncD1(double *p, double *v)
{
	v[0] = 2.0 * (p[0] - a[0]) * pow(p[1]-a[1], 2.0) * pow(p[2]-a[2], 2.0);
	v[1] = 2.0 * (p[1] - a[1]) * pow(p[0]-a[0], 2.0) * pow(p[2]-a[2], 2.0);
	v[2] = 2.0 * (p[2] - a[2]) * pow(p[0]-a[0], 2.0) * pow(p[1]-a[1], 2.0);

	return true;
}

