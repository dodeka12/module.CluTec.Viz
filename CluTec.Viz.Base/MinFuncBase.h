////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      MinFuncBase.h
//
// summary:   Declares the minimum function base class
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

// MinFuncBase.h: Schnittstelle f�r die Klasse MinFuncBase.
//
// This class defines the basis for functions which are to be
// minimised by one of the minimisers in this project.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MINFUNCBASE_H__36567180_50D4_11D3_9065_00E098180C06__INCLUDED_)
#define AFX_MINFUNCBASE_H__36567180_50D4_11D3_9065_00E098180C06__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef unsigned int uint;


class MinFuncBase  
{
public:
//	MinFuncBase();
	//virtual ~MinFuncBase();

	// returns true if no error occured, otherwise false.
	// deriv: derivative order, i.e. 0 is function, 1 is 1st deriv. etc
	// p: pointer to function parameters
	// v: return value or values
	virtual bool Func(uint deriv, double *p, double *v) = 0;

	int GetMaxDerivative() { return m_iMaxDeriv; }
	int GetDimension() { return m_iDim; }

protected:
	int m_iDim; // Dimensionality of function
	int m_iMaxDeriv; // maximal derivative available
};

#endif // !defined(AFX_MINFUNCBASE_H__36567180_50D4_11D3_9065_00E098180C06__INCLUDED_)
