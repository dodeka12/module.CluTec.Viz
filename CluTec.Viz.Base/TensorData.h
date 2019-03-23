////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      TensorData.h
//
// summary:   Declares the tensor data class
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

// TensorData Library declaration

#ifndef _TENSOR_DATA_HH_
#define _TENSOR_DATA_HH_

#include "mem.h"
#include "makestr.h"
#include "CStrMem.h"
#include "mathelp.h"
#include "CLUException.h"

template<class CType> class CTensorIdx;

template<class CType>
class CTensorData
{
public:
	class InvalidValence : public CCluError
	{
	public:
		InvalidValence()
		{
			m_sErrorText = "Invalid valence value.";
		}
	};

	class InvalidDim : public CCluError
	{
	public:
		InvalidDim()
		{
			m_sErrorText = "Invalid dimension.";
		}
	};

public:
	CTensorData();
	CTensorData(const CTensorData<CType>& rT);
	CTensorData(const Mem<int>& mDim) throw (CCluException);
	~CTensorData();
	
	CTensorData<CType>& operator= (const CTensorData<CType>& rT);
	void Reset(const Mem<int>& mDim) throw (CCluException); 
	//bool ResizeDim(int iDim, int iSize) throw (CCLUException);

	int Size() const { return m_iSize; }
	int DimSize(int iDim) const { return m_mDim[iDim]; }
	int Valence() const { return m_iValence; }

	Mem<int> GetIdxSteps() { return m_mIdxStep; }
	CType* Data() const { return m_mData.Data(); }

	void RegisterTensorIdx(CTensorIdx<CType>& rTIdx);
	void UnregisterTensorIdx(CTensorIdx<CType>& rTIdx);

protected:
	// Valence of Tensor
	int m_iValence;

	// Dimensions of Tensor
	Mem<int> m_mDim;
	
	// total size of component array
	int m_iSize;              

	// Gives step sizes for every index:
	// Last Index will be 1, 
	// one before last: dimension of last
	// one before that: dim of last * dim of penultimate
	// etc.
	Mem<int> m_mIdxStep;      
	
	// Components of tensor. 
	// Ordered from right index to left index
	Mem<CType> m_mData;       

	// List of pointers to TensorIdx instances
	Mem<CTensorIdx<CType> *> m_mTensorIdx;
	
};



#endif
