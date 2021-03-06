////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      TensorIdx_1.cxx
//
// summary:   tensor index 1 class
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

// Library for use of tensors

#include<math.h>
#include<stdio.h>
#include<string.h>

#include "TensorIdx.h"

////////////////////////////////////////////////////////////////////////////////////
/// Default Constructor

template <class CType>
CTensorIdx<CType>::CTensorIdx()
{
	Reset();
}


////////////////////////////////////////////////////////////////////////////////////
// Constructor

template <class CType>
CTensorIdx<CType>::CTensorIdx(CTensorData<CType>& rT, const Mem<int>& mIdx) 
	throw (CCluException)
{
	Reset();

	Set(rT, mIdx);
}

////////////////////////////////////////////////////////////////////////////////////
/// Copy Constructor

template <class CType>
CTensorIdx<CType>::CTensorIdx(const CTensorIdx<CType>& rTIdx)
{
	*this = rTIdx;
}


////////////////////////////////////////////////////////////////////////////////////
/// Destructor

template <class CType>
CTensorIdx<CType>::~CTensorIdx()
{
	if (m_pTensor)
		m_pTensor->UnregisterTensorIdx(*this);
}


////////////////////////////////////////////////////////////////////////////////////
/// Assign Operator

template <class CType>
CTensorIdx<CType>& CTensorIdx<CType>::operator= (const CTensorIdx<CType>& rTIdx)
{
	if (!rTIdx.IsValid())
	{
		if (m_pTensor)
			m_pTensor->UnregisterTensorIdx(*this);
	}
	else
	{
		Set(*rTIdx.m_pTensor, rTIdx.m_mIdx);
		m_mapFreeIdx = rTIdx.m_mapFreeIdx;
		UpdateStepPos();
	}

	return *this;
}


////////////////////////////////////////////////////////////////////////////////////
/// Reset

template <class CType>
void CTensorIdx<CType>::Reset()
{
	Invalidate();

	m_iFixedPos = 0;
	m_iStepPos = 0;

	m_mapFreeIdx.clear();
	m_mIdx.Set(0);
}

////////////////////////////////////////////////////////////////////////////////////
/// Invalidate Tensor Index, since TensorData has been deleted.

template <class CType>
void CTensorIdx<CType>::Invalidate()
{
	m_bIsValid = false;
	m_pTensor = 0;
	m_mIdx.Set(0);
}


////////////////////////////////////////////////////////////////////////////////////
// Check whether index is constant

template <class CType>
bool CTensorIdx<CType>::IsConstant() const
	throw (CCluException)
{
	if (!m_bIsValid)
		throw InvalidTensorRef();

	if (m_mapFreeIdx.size() == 0)
		return true;

	return false;
}


////////////////////////////////////////////////////////////////////////////////////
/// Set TensorData and Index

template <class CType>
void CTensorIdx<CType>::Set(CTensorData<CType>& rT, const Mem<int>& mIdx) 
	throw (CCluException)
{
	int i, iCount = rT.Valence();

	if (m_pTensor)
		m_pTensor->UnregisterTensorIdx(*this);

	rT.RegisterTensorIdx(*this);
	m_pTensor = &rT;

	try
	{
		// Clear list of free indices
		m_mapFreeIdx.clear();

		// Set size of tensor index list
		m_mIdx.Set(iCount);

		// Reset element position due to fixed indices
		m_iFixedPos = 0;

		// Loop through indices
		for (i = 0; i < iCount; i++)
		{
			m_mIdx[i] = mIdx[i];
			if (mIdx[i] >= rT.DimSize(i))	// Out of range
			{
				throw InvalidIdx();
			}
			else if (mIdx[i] < 0)	// Free index
			{
				SetFreeIdx(mIdx[i], i);		
			}
			else	// Fixed index
			{
				m_iFixedPos += mIdx[i] * rT.GetIdxSteps()[i];
			}
		}
	}
	catch (CCluException &rEx)
	{
		m_pTensor->UnregisterTensorIdx(*this);
		throw rEx;
	}

	m_bIsValid = true;
}


////////////////////////////////////////////////////////////////////////////////////
/// Set Free Index

template <class CType>
void CTensorIdx<CType>::SetFreeIdx(int iIdx, int iDim)
	throw (CCluException)
{
	if (!m_pTensor)
		throw InvalidTensorRef();

	if (iIdx >= 0)
		throw InvalidIdx();

	// Check whether index already exists in map
	if ( m_mapFreeIdx.find(iIdx) == m_mapFreeIdx.end() )
	{
		// Index does not yet exist

		// Create index
		SFreeIdxData &rIdx = m_mapFreeIdx[iIdx];

		// Initialize Free Index Range
		rIdx.iMin = 0;
		rIdx.iMax = m_pTensor->DimSize(iDim) - 1;

		// Initialize Allowed Indices
		rIdx.mIdx.Set(0);

		// Initialize Index Step
		rIdx.iStep = m_pTensor->GetIdxSteps()[iDim];

		// Initialize current index position
		rIdx.iIdx = 0;
	}
	else
	{
		// Free index already exists, i.e. a number of dimensions have same free index.

		// Get index
		SFreeIdxData &rIdx = m_mapFreeIdx[iIdx];

		// Increase step size in tensor
		rIdx.iStep += m_pTensor->GetIdxSteps()[iDim];

		// Adjust index range to smallest range
		if (rIdx.iMax >= m_pTensor->DimSize(iDim))
			rIdx.iMax = m_pTensor->DimSize(iDim) - 1;
	}
}


////////////////////////////////////////////////////////////////////////////////////
/// Set index range of free index

template <class CType>
void CTensorIdx<CType>::SetIdxRange(int iIdx, int iMin, int iMax)
	throw (CCluException)
{
	// This function only sets the index range for free indices.
	// It also resets the current index position to iMin.
	int iDim, iValence, iMinDimSize;

	if (!m_bIsValid)
		throw InvalidTensorRef();

	// Only allow free indices
	if (iIdx >= 0)
		throw InvalidIdx();

	// Check whether index exists in map
	if ( m_mapFreeIdx.find(iIdx) == m_mapFreeIdx.end() )
		throw InvalidIdx();

	// Get minimal max value of index range
	iMinDimSize = 0;
	iValence = m_pTensor->Valence();
	for (iDim = 0; iDim < iValence; iDim++)
	{
		if (m_mIdx[iDim] == iIdx)
		{
			if (iMinDimSize == 0 || iMinDimSize > m_pTensor->DimSize(iDim))
				iMinDimSize = m_pTensor->DimSize(iDim);
		}
	}

	// Check Index Range
	if (iMin < 0 || iMin >= iMinDimSize ||
		iMax < iMin || iMax >= iMinDimSize)
		throw InvalidIdxRange();

	// Get index data
	SFreeIdxData &rIdx = m_mapFreeIdx[iIdx];

	// Set Index range
	rIdx.iMin = iMin;
	rIdx.iMax = iMax;

	// Reset Position
	rIdx.iIdx = iMin;
}


////////////////////////////////////////////////////////////////////////////////////
// Set list of allowed indices of free index

template <class CType>
void CTensorIdx<CType>::SetIdxList(int iIdx, Mem<int> mIdx) 
	throw (CCluException)
{
	// This function only sets the index range for free indices.
	// It also resets the current index position to iMin.
	int iDim, iValence, iMinDimSize;

	if (!m_bIsValid)
		throw InvalidTensorRef();

	// Only allow free indices
	if (iIdx >= 0)
		throw InvalidIdx();

	// Check whether index exists in map
	if ( m_mapFreeIdx.find(iIdx) == m_mapFreeIdx.end() )
		throw InvalidIdx();

	// Get minimal max value of index range
	iMinDimSize = 0;
	iValence = m_pTensor->Valence();
	for (iDim = 0; iDim < iValence; iDim++)
	{
		if (m_mIdx[iDim] == iIdx)
		{
			if (iMinDimSize == 0 || iMinDimSize > m_pTensor->DimSize(iDim))
				iMinDimSize = m_pTensor->DimSize(iDim);
		}
	}

	// Check Index Range
	if (iMin < 0 || iMin >= iMinDimSize ||
		iMax < iMin || iMax >= iMinDimSize)
		throw InvalidIdxRange();

	// Get index data
	SFreeIdxData &rIdx = m_mapFreeIdx[iIdx];

	// Set Index range
	rIdx.iMin = iMin;
	rIdx.iMax = iMax;

	// Reset Position
	rIdx.iIdx = iMin;

}

////////////////////////////////////////////////////////////////////////////////////
/// Get reference to current data value

template <class CType>
CType& CTensorIdx<CType>::GetRef()
	throw (CCluException)
{
	if (!m_bIsValid)
		throw InvalidTensorRef();

	int iPos = m_iFixedPos;
	TFreeIdxMap::iterator it_El, it_End;

	it_End = m_mapFreeIdx.end();
	for (it_El = m_mapFreeIdx.begin();
		 it_El != it_End;
		 ++it_El)
	{
		iPos += it_El->second.iIdx * it_El->second.iStep;
	}

	if (iPos >= m_pTensor->Size())
		throw Internal("GetRef");

	return m_pTensor->Data()[iPos];
}


////////////////////////////////////////////////////////////////////////////////////
// Update Step Position
template <class CType>
void CTensorIdx<CType>::UpdateStepPos()
{
	TFreeIdxMap::iterator it_El, it_End;

	it_End = m_mapFreeIdx.end();

	// Update step position
	m_iStepPos = m_iFixedPos;

	for (it_El = m_mapFreeIdx.begin();
		 it_El != it_End;
		 ++it_El)
	{
		m_iStepPos += it_El->second.iIdx * it_El->second.iStep;
	}
}


////////////////////////////////////////////////////////////////////////////////////
// Initialize all loops

template <class CType>
void CTensorIdx<CType>::InitAllLoops() 
	throw (CCluException)
{
	if (!m_bIsValid)
		throw InvalidTensorRef();

	TFreeIdxMap::iterator it_El, it_End;

	it_End = m_mapFreeIdx.end();
	for (it_El = m_mapFreeIdx.begin();
		 it_El != it_End;
		 ++it_El)
	{
		SFreeIdxData &rIdx = it_El->second;

		rIdx.iIdx = 0;
	}

	UpdateStepPos();
}


////////////////////////////////////////////////////////////////////////////////////
// Initialize particular loop

template <class CType>
void CTensorIdx<CType>::InitLoop(int iIdx)
	throw (CCluException)
{
	if (iIdx >= 0)
		throw InvalidIdx();

	TFreeIdxMap::iterator it_El, it_End;
	it_End = m_mapFreeIdx.end();

	if ( (it_El = m_mapFreeIdx.find(iIdx)) == m_mapFreeIdx.end() )
	{
		throw InvalidIdx();
	}

	SFreeIdxData &rIdx = it_El->second;

	rIdx.iIdx = 0;

	UpdateStepPos();
}


////////////////////////////////////////////////////////////////////////////////////
// Step Loop. No check of validity of index and the instance.
// Returns true if step was made and false if no step could be made,
// because loop already at last position.

template <class CType>
bool CTensorIdx<CType>::StepLoop(int iIdx)
{
	SFreeIdxData &rIdx = m_mapFreeIdx[iIdx];
		
	if (rIdx.iIdx+1 == rIdx.mRange.Count())
		return false;	// Loop finished

	rIdx.iIdx++;
	m_iStepPos = rIdx.mRange[rIdx.iIdx] * rIdx.iStep;

	return true;
}




