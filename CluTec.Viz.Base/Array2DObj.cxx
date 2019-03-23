////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      Array2DObj.cxx
//
// summary:   array 2D object class
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

#include "Array2DObj.h"


////////////////////////////////////////////////////////////////////////////////////
/// Constructor

template<class CType>
CArray2DObj<CType>::CArray2DObj()
{
	m_DimX = 0;
	m_nDimY = 0;
}

////////////////////////////////////////////////////////////////////////////////////
/// Copy Constructor

template<class CType>
CArray2DObj<CType>::CArray2DObj(const CArray2DObj<CType>& rA2D)
{
	*this = rA2D;
}

////////////////////////////////////////////////////////////////////////////////////
/// Init Constructor

template<class CType>
CArray2DObj<CType>::CArray2DObj(size_t nCX, size_t nCY)
{
	SetDim(nCX, nCY);
}

////////////////////////////////////////////////////////////////////////////////////
/// Init Constructor

template<class CType>
CArray2DObj<CType>::CArray2DObj(const CPointND<size_t, 2>& rP2D)
{
	SetDim(rP2D(0), rP2D(1));
}

////////////////////////////////////////////////////////////////////////////////////
/// Operator=

template<class CType>
CArray2DObj<CType>& CArray2DObj<CType>::operator= (const CArray2DObj<CType>& rA2D)
{
	if (!SetDim(rA2D.m_nDimX, rA2D.m_nDimY))
	{
		SetDim(0, 0);
		return *this;
	}

	int i, n = m_mArray.Count();

	for(i=0;i<n;i++)
	{
		m_mArray[i] = rA2D.m_mArray[i];
	}

	return *this;
}


////////////////////////////////////////////////////////////////////////////////////
/// Set Dimension

template<class CType>
bool CArray2DObj<CType>::SetDim(size_t nCX, size_t nCY)
{
	if (nCX < 0 || nCY < 0) return false;

	if (nCX == 0 && nCY == 0)
	{
		m_mArray.Set(0);
		return true;
	}

	if (nCX == m_DimX && nCY == m_nDimY) return true;

	uint uNewCount = uint(nCX) * uint(nCY);

	if (uNewCount > m_mArray.Count())
	{
		if (!m_mArray.Set(uNewCount))
			return false;
	}

	// If X-Dimension changed need to reorder lines
	int iMinCY = (nCY < m_nDimY ? nCY : m_nDimY);
	int i, iOldLine, iNewLine;

	if (nCX < m_DimX)
	{
		iOldLine = m_DimX;
		iNewLine = nCX;
		for(i=1;i<iMinCY;i++, iOldLine+=m_DimX, iNewLine+=nCX)
		{
			m_mArray.Move(iNewLine, iOldLine, nCX);
		}
	}
	else if (nCX > m_DimX)
	{
		// Last of old lines
		iOldLine = m_DimX * (iMinCY - 1);
		// Lst of new lines
		iNewLine = nCX * (iMinCY - 1);
		for(i=1;i<iMinCY;i++, iOldLine-=m_DimX, iNewLine-=nCX)
		{
			m_mArray.Move(iNewLine, iOldLine, m_DimX);
		}
	}
	
	if (uNewCount < m_mArray.Count())
	{
		if (!m_mArray.Set(uNewCount))
			return false;
	}

	m_nDimX = nCX;
	m_nDimY = nCY;

	return true;
}

