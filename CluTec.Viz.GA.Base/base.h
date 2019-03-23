////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.GA.Base
// file:      base.h
//
// summary:   Declares the base class
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

#ifndef _BASE_H_ 
#define _BASE_H_


#include "CluTec.Viz.Base\CStrMem.h"

typedef unsigned uint;
typedef unsigned long ulong;


// Base Class -------------------------------------------------------


template<class CType>
class Base
{
public:
	
	Base(uint nvsdim, uint nfidx=1, const char* nvname = "e", unsigned nid=0);
	virtual ~Base();
	
	uint BaseID() const { return id; }
	uint VSDim() const { return vsdim; }
	uint GADim() const { return gadim; }
	char* VName() const { return vname; }
	uint FirstIdx() const { return firstIdx; }
	
	virtual CType signature(uint i) = 0;// Signature, to be defined by derived class
	
protected:
	uint id;				// Id of Basis
	uint vsdim;			// Dimensionality of Vector Space
	uint gadim;			// Dimensionality of corresponding algebra
	CStrMem vname;			// Name of basis vectors
	uint firstIdx;	// Index of first Basis Vector
	
};


#endif // _BASE_H_

