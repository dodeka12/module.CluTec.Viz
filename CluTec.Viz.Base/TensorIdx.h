////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      TensorIdx.h
//
// summary:   Declares the tensor index class
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

// Tensor Index Library declaration

#ifndef _TENSOR_INDEX_HH_
	#define _TENSOR_INDEX_HH_

#include <map>

#include "mem.h"
#include "makestr.h"
#include "CStrMem.h"
#include "mathelp.h"
#include "CLUException.h"
#include "TensorData.h"

	using std::map;

	template<class CType>
	class CTensorIdx
	{
	public:

		class InvalidTensorRef : public CCluError
		{
		public:

			InvalidTensorRef()
			{
				m_sErrorText = "Invalid tensor reference.";
			}
		};

		class InvalidIdx : public CCluError
		{
		public:

			InvalidIdx()
			{
				m_sErrorText = "Invalid index.";
			}
		};

		class InvalidIdxRange : public CCluError
		{
		public:

			InvalidIdxRange()
			{
				m_sErrorText = "Invalid index range.";
			}
		};

		class InvalidIdxList : public CCluError
		{
		public:

			InvalidIdxList()
			{
				m_sErrorText = "Invalid index list.";
			}
		};

		class InvalidDim : public CCluError
		{
		public:

			InvalidDim()
			{
				m_sErrorText = "Invalid tensor dimension.";
			}
		};

		class Internal : public CCluError
		{
		public:

			Internal(const char* pcWhere)
			{
				m_sErrorText  = pcWhere;
				m_sErrorText += ": Internal Error.";
			}
		};

	public:

		struct SFreeIdxData
		{
			int iIdx;		// Current index value
			int iMin, iMax;	// Index Range
			int iStep;		// Number of elements in tensor to step over
			// if index changes by one.
			int iCount;		// Number of steps in free index.
			Mem<int> mIdx;	// Optionally set list of indices.
			bool bUseIdxList;	// If true, then index list is used.
		};

		typedef map<int, SFreeIdxData> TFreeIdxMap;

	public:

		friend class CTensorData<CType>;

	public:

		CTensorIdx();
		CTensorIdx(CTensorData<CType>& rT, const Mem<int>& mIdx) throw(CCluException);
		CTensorIdx(const CTensorIdx<CType>& rTIdx);
		~CTensorIdx();

		CTensorIdx& operator=(const CTensorIdx<CType>& rTIdx);

		// The tensor index is valied, if a tensor and indices are set.
		bool IsValid() const { return m_bIsValid; }

		// Check whether index is constant
		bool IsConstant() const throw(CCluException);

		// Set tensor to which this index relates, and the index.
		// A negative index gives a free index, and a positive or zero index gives a fixed index.
		void Set(CTensorData<CType>& rT, const Mem<int>& mIdx) throw(CCluException);

		// Set index to a new tensor, with the same ranges or index lists.
		void Set(CTensorData<CType>& rT, const CTensorIdx<CType>& rTIdx) throw(CCluException);

		// Set index range of free index
		void SetIdxRange(int iIdx, int iMin, int iMax) throw(CCluException);

		// Set index list of free index
		void SetIdxList(int iIdx, const Mem<int>& mIdx) throw(CCluException);

		// Get reference to current data value
		CType& GetRef() throw(CCluException);

		// Initialize all loops
		void InitAllLoops() throw(CCluException);

		// Initialize particular loop
		void InitLoop(int iIdx)  throw(CCluException);

		// Step Loop. No check of validity of index and the instance.
		bool StepLoop(int iIdx);

		// Get reference to tensor element at current loop position.
		// No checking! Only use after InitAllLoops() or InitLoop().
		CType& GetStepRef() { return m_pTensor->Data()[m_iStepPos]; }

		// Get number of steps in loop of given free index.
		int GetStepCount(int iIdx);

		/// Get Free Index List
		const TFreeIdxMap& GetFreeIdxMap() const { return m_mapFreeIdx; }

		// Get Index List
		const Mem<int>& GetIdxList() const { return m_mIdx; }

		// Get Reference to Tensor Data
		CTensorData<CType>& GetTensorData() { if (!m_bIsValid) { throw InvalidTensorRef(); } return *m_pTensor; }

	protected:

		// Reset class
		void Reset();

		// Invalidate Tensor Index, since TensorData has been deleted.
		void Invalidate();

		// Set a free index in the index map
		void SetFreeIdx(int iIdx, int iDim) throw(CCluException);

		// Update Step Position
		void UpdateStepPos();

	protected:

		// Is true if indices are valid and a pointer to a tensor is given.
		bool m_bIsValid;

		// Indices
		Mem<int> m_mIdx;

		// Element position in tensor due to fixed indices
		int m_iFixedPos;

		// Element position of tensor at current step position.
		int m_iStepPos;

		// Map of free indices
		TFreeIdxMap m_mapFreeIdx;

		// Reference to Tensor Data
		CTensorData<CType>* m_pTensor;
	};

#endif
