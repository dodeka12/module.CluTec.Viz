////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      Array2D.h
//
// summary:   Declares the array 2D class
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

#ifndef __ARRAY2D_H__
#define __ARRAY2D_H__

#include "Point.h"
#include "memobj.h"
// 2D Array of Class Objects

template<class CType>
class CArray2D
{
public:
	CArray2D();
	CArray2D(size_t nCX, size_t nCY);
	CArray2D(const CPointND<size_t, 2>& rP2D);
	CArray2D(const CArray2D<CType>& rA2D);

	CArray2D<CType>& operator= (const CArray2D<CType>& rA2D);

	void GetDim(size_t &nCX, size_t &nCY) { nCX = m_nDimX; nCY = m_nDimY; }
	bool SetDim(size_t nCX, size_t nCY);
	bool SetDim(const CPointND<size_t, 2>& rP2D) { return SetDim(rP2D(0), rP2D(1)); }

	CType& operator() (size_t nX, size_t nY) 
	{ return m_mArray[nY * m_nDimX + nX]; }

	CType& operator() (const CPointND<size_t, 2>& rP2D) 
	{ return m_mArray[rP2D(1) * m_nDimX + rP2D(0)]; }

	bool ExistElement(size_t nX, size_t nY)
	{ return (nX >= 0 && nX < m_nDimX && nY >= 0 && nY < m_nDimY); }

	bool ExistElement(const CPointND<size_t, 2>& rP2D)
	{ return (rP2D(0) >= 0 && rP2D(0) < m_nDimX && rP2D(1) >= 0 && rP2D(1) < m_nDimY); }

	
protected:
	size_t m_nDimX, m_nDimY;

	Mem<CType> m_mArray;
};


#endif

