////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      Array2DObj.h
//
// summary:   Declares the array 2D object class
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

#ifndef __ARRAY2DOBJ_H__
#define __ARRAY2DOBJ_H__

#include "Point.h"
#include "memobj.h"
// 2D Array of Class Objects

template<class CType>
class CArray2DObj
{
public:
	CArray2DObj();
	CArray2DObj(size_t iCX, size_t iCY);
	CArray2DObj(const CPointND<size_t, 2>& rP2D);
	CArray2DObj(const CArray2DObj<CType>& rA2D);

	CArray2DObj<CType>& operator= (const CArray2DObj<CType>& rA2D);

	void GetDim(size_t &nCX, size_t &nCY) { nCX = m_DimX; nCY = m_nDimY; }
	bool SetDim(int iCX, int iCY);
	bool SetDim(const CPointND<int, 2>& rP2D) { return SetDim(rP2D(0), rP2D(1)); }

	CType& operator() (size_t nX, size_t nY) 
	{ return m_mArray[nY * m_nDimX + nX]; }

	CType& operator() (const CPointND<size_t, 2>& rP2D) 
	{ return m_mArray[rP2D(1) * m_DimX + rP2D(0)]; }

	bool ExistElement(size_t nX, size_t nY)
	{ return (nX >= 0 && nX < m_DimX && nY >= 0 && nY < m_nDimY); }

	bool ExistElement(const CPointND<size_t, 2>& rP2D)
	{ return (rP2D(0) >= 0 && rP2D(0) < m_DimX && rP2D(1) >= 0 && rP2D(1) < m_nDimY); }

	
protected:
	int m_nDimX, m_nDimY;

	MemObj<CType> m_mArray;
};


#endif

