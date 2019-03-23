////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      TensorData.cxx
//
// summary:   tensor data class
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

// Library for TensorData

#include<math.h>
#include<stdio.h>
#include<string.h>

#include "TensorData.h"


////////////////////////////////////////////////////////////////////////////////////
/// Default Constructor 

template<class CType>
CTensorData<CType>::CTensorData()
{
	Mem<int> mDim;

	mDim.Add(1);
	mDim.Last() = 1;

	Reset(mDim);
}



////////////////////////////////////////////////////////////////////////////////////
/// Constructor

template<class CType>
CTensorData<CType>::CTensorData(const Mem<int>& mDim)
	throw (CCluException)
{
	Reset(mDim);
}


////////////////////////////////////////////////////////////////////////////////////
/// Copy Constructor

template<class CType>
CTensorData<CType>::CTensorData(const CTensorData<CType>& rT)
{
	*this = rT;
}


////////////////////////////////////////////////////////////////////////////////////
/// Destructor

template<class CType>
CTensorData<CType>::~CTensorData()
{
	int i, iCount = int (m_mTensorIdx.Count());

	// Invalidate all Tensor Index instances that are registered
	for (i = 0; i < iCount; i++)
	{
		m_mTensorIdx[i]->Invalidate();
	}
}


////////////////////////////////////////////////////////////////////////////////////
/// Assign Operator

template<class CType>
CTensorData<CType>& CTensorData<CType>::operator= (const CTensorData<CType>& rT)
{
	m_iValence = rT.m_iValence;
	m_mDim = rT.m_mDim;
	m_mIdxStep = rT.m_mIdxStep;
	m_iSize = rT.m_iSize;
	m_mData = rT.m_mData;

	// Unregister all tensor index instances
	int i, iCount = int (m_mTensorIdx.Count());

	// Invalidate all Tensor Index instances that are registered
	for (i = 0; i < iCount; i++)
	{
		m_mTensorIdx[i]->Invalidate();
	}

	return *this;
}


////////////////////////////////////////////////////////////////////////////////////
/// Reset Tensor

template<class CType>
void CTensorData<CType>::Reset(const Mem<int>& mDim)
	throw (CCluException)
{
	int iValence = int (mDim.Count());

	if (iValence <= 0)
		throw InvalidValence();

	if (mDim.Count() != iValence)
		throw InvalidDim();

	int iDim;
	m_iSize = 1;
	m_iValence = iValence;
	m_mDim.Set(iValence);
	m_mIdxStep.Set(iValence);

	for(iDim = m_iValence-1; iDim >= 0; iDim--)
	{	
		if (mDim[iDim] <= 0)
			throw InvalidDim();

		m_mDim[iDim] = mDim[iDim];
		m_mIdxStep[iDim] = m_iSize;
		m_iSize *= mDim[iDim];
	}

	if (!m_mData.Set(m_iSize))
		throw CCluOutOfMemory();

	memset(m_mData.Data(), 0, m_iSize*sizeof(CType));
}




////////////////////////////////////////////////////////////////////////////////////
/// Changes size of a dimension
/*
template<class CType>
void CTensorData<CType>::ResizeDim(int iDim, int iSize)
{
	if (iDim < 0 || iDim >= m_mDim.Count())
		throw InvalidDim();

	if (iSize <= 0)
		throw CCLUError("Invalid size");

	int i;

	Mem<uint> ndims(dims);
	ndims[d] = no;         // Set size of new dimension

	Tensor<CType> T(rank, ndims.Data(), fidx);  // Create Resized Tensor
	if (!T.isOK()) 
	{ delete[] cidx;
	return 0;
	}

	if (no > dims[d])        // Restrict range if new dim bigger than old
		T.IdxRange(d+1, fidx, dims[d] + fidx - 1);

	this->ResetIdxRange();   // Free index range of old tensor

	T.actidx = 0;
	this->actidx = 0;

	for(i=0;i<rank;i++)     // Set counting indices
	{ T,&cidx[i];
	this[0],&cidx[i];
	}

	T = this[0];
	T.ResetIdxRange();
	this[0] << T;

	delete[] cidx;
	return 1;
}
*/


////////////////////////////////////////////////////////////////////////////////////
/// Register a TensorIdx instance

template<class CType>
void CTensorData<CType>::RegisterTensorIdx(CTensorIdx<CType>& rTIdx)
{
	m_mTensorIdx.Add(1);
	m_mTensorIdx.Last() = &rTIdx;
}


////////////////////////////////////////////////////////////////////////////////////
/// Unregister a tensor index

template<class CType>
void CTensorData<CType>::UnregisterTensorIdx(CTensorIdx<CType>& rTIdx)
{
	int i, iCount = int (m_mTensorIdx.Count());

	for (i = 0; i < iCount; i++)
	{
		if (m_mTensorIdx[i] == &rTIdx)
		{
			m_mTensorIdx.Del(i);
			rTIdx.Invalidate();
			break;
		}
	}
}


