////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      mem.h
//
// summary:   Declares the memory class
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

// Mem Class ---------------------------------------------
//
// This class can be used for objects that do not need to have their constructor/destructor
// called. It uses memcpy, memmove for copying.

#ifndef _MEM_HH_
#define _MEM_HH_


#include "MemBase.h"

typedef unsigned int uint;

template<class CType>
class Mem : public MemBase
{
public:
	Mem(size_t nno = 0);
	Mem(const Mem<CType> &a);
	virtual ~Mem();  
	
	inline bool operator=(size_t ano);
	inline Mem<CType>& operator=(const Mem<CType> &a);
	
	inline CType& operator[] (size_t pos) const;
	
	inline CType& Last() const;
	inline bool PushBack(const CType& xValue);
	inline bool PushBack(const Mem<CType> &mData);

	inline CType* Data() const;
};


#endif
