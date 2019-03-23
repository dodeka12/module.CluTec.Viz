////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      memobj.cxx
//
// summary:   memobj class
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

// MemObj Class Definitions ------------------------------------

#include<stdlib.h>
#include<memory.h>
#include<string.h>

#include"memobj.h"


//////////////////////////////////////////////////////////////////////////////////
// Constructor

template<class CType>
MemObj<CType>::MemObj(size_t nno)
{
	m_uInitElNo = 0;
	m_bDirectAlloc = true;
	m_mData.EnableDirectAlloc(false);
	Set(nno);
}


////////////////////////////////////////////////////////////////////////////
// Copy Constructor

template<class CType>
MemObj<CType>::MemObj(const MemObj<CType> &_rMemObj)
{
	m_uInitElNo = 0;
	m_bDirectAlloc = true;
	m_mData.EnableDirectAlloc(false);

	*this = _rMemObj;
}


////////////////////////////////////////////////////////////////////////////
// Destructor

template<class CType>
MemObj<CType>::~MemObj()
{
	Set(0);	// deletes all objects in m_mData
	Prune();
}


////////////////////////////////////////////////////////////////////////////
// Set Memory Data

template<class CType>
MemObj<CType>& MemObj<CType>::operator=(const MemObj<CType> &_rMemObj)
{
	size_t no = _rMemObj.Count();

	if ( !(m_bDirectAlloc = _rMemObj.m_bDirectAlloc) )
	{
		Reserve(_rMemObj.m_mData.Capacity());
	}

	if (Set(no))
	{ 
		for(size_t i=0;i<no;i++)
		{
			*m_mData[i] = *_rMemObj.m_mData[i];
		}
	}

	return *this;
}



////////////////////////////////////////////////////////////////////////////
// Move parts of Memory
// Does NOT overwrite

template<class CType>
bool MemObj<CType>::Move(size_t dest, size_t start, size_t mno)
{
	PCType pDummy;
	size_t no = m_mData.Count();

	if (dest+mno > no || start+mno > no) return false;
	if (!mno) return true;

	int i,j,k;

	if (dest > start)
	{ 
		for(i=int(start+mno-1),j=int(dest+mno-1);i>=int(start);i--,j--)
		{
			pDummy = m_mData[j];
			m_mData[j] = m_mData[i];
			m_mData[i] = pDummy;
		}
	}
	else if (start > dest)
	{ 
		for(i=0,j=int(start),k=int(dest);i<int(mno);i++,j++,k++)
		{
			pDummy = m_mData[k];
			m_mData[k] = m_mData[j];
			m_mData[j] = pDummy;
		}
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////
// Insert Memory

template<class CType>
bool MemObj<CType>::Insert(size_t pos, size_t mno)
{
	size_t ono = m_mData.Count();

	if (Add(mno))
	{ 
		if (Move(pos+mno, pos, ono-pos))
			return true;

		Sub(mno);
	}

	return false;
}



////////////////////////////////////////////////////////////////////////////
// Delete Memory

template<class CType>
bool MemObj<CType>::Del(size_t pos, size_t mno)
{
	size_t no = m_mData.Count();

	if (pos+1 < no)
	{ 
		if (!Move(pos, pos+mno, no-pos-mno))
			return false;
	}

	Sub(mno);

	return true;
}

////////////////////////////////////////////////////////////////////////////
// ATTENTION! This function will lead to memory leaks if the caller did not
// care for the memory that had been allocated by this class.
template<class CType>
bool MemObj<CType>::Forget(size_t uPos, size_t uNo)
{
	size_t uIdx, uCnt = m_mData.Count();

	if ( uPos + uNo > uCnt )
		return false;

	for ( uIdx = uPos; uIdx < uPos + uNo; ++uIdx )
	{
		m_mData[uIdx] = (CType*) 0;
	}

	return Del(uPos, uNo);
}

////////////////////////////////////////////////////////////////////////////
template<class CType>
size_t MemObj<CType>::IndexOf(CType* pEl)
{
	size_t uIdx, uCnt = m_mData.Count();
	if ( uCnt == 0 )
		return -1;

	if ( pEl == 0 )
		return -1;

	CType** ppData = m_mData.Data();

	for ( uIdx = 0; uIdx < uCnt; ++uIdx, ppData += 1 )
	{
		if ((*ppData) == pEl) 
			return uIdx;
	}

	return -1;
}

////////////////////////////////////////////////////////////////////////////
template<class CType>
bool MemObj<CType>::Replace(size_t uPos, CType* pData)
{
	if ( uPos >= m_mData.Count() )
		return false;

	PCType pEl = ((PCType*) m_mData.Data())[uPos];
	if ( pEl )
		delete pEl;

	m_mData[uPos] = pData;
	return true;
}

////////////////////////////////////////////////////////////////////////////
template<class CType>
bool MemObj<CType>::InitElements(size_t uElCnt)
{
	size_t uIdx;

	if ( m_uInitElNo < uElCnt )
	{
		for ( uIdx = m_uInitElNo; uIdx < uElCnt; ++uIdx )
		{
			((PCType*) m_mData.Data())[uIdx] = new CType;
		}
	}
	else if ( m_uInitElNo > uElCnt )
	{
		for ( uIdx = m_uInitElNo; uIdx > uElCnt; --uIdx )
		{
			PCType pEl = ((PCType*) m_mData.Data())[uIdx-1];
			if ( pEl )
				delete pEl;
		}
	}

	m_uInitElNo = uElCnt;

	//uint uIdx, uCurCnt = m_mData.Capacity();

	//if ( uCapElNo == uCurCnt ) 
	//	return true;

	//if ( uCapElNo > uCurCnt )
	//{
	//	if ( m_mData.Reserve(uCapElNo) )
	//	{
	//		for ( uIdx = uCurCnt; uIdx < uCapElNo; ++uIdx )
	//		{
	//			((PCType*) m_mData.Data())[uIdx] = new CType;
	//		}
	//	}
	//	else return false;
	//}
	//else
	//{
	//	for ( uIdx = uCurCnt; uIdx > uCapElNo; --uIdx )
	//	{
	//		if ( ((PCType*) m_mData.Data())[uIdx-1] )
	//			delete ((PCType*) m_mData.Data())[uIdx-1];
	//	}

	//	//if ( uElNo < m_mData.Count() )
	//	//	m_mData.Set(uElNo);

	//	m_mData.Reserve(uCapElNo);
	//}

	return true;
}

////////////////////////////////////////////////////////////////////////////
// Set Memory Size

template<class CType>
bool MemObj<CType>::Reserve(size_t uCapElNo)
{
	if ( !m_mData.Reserve(uCapElNo) )
		return false;

	// switch to indirect allocation mode
	m_bDirectAlloc = false;

	return true;
}

////////////////////////////////////////////////////////////////////////////
// reduce the allocated memory to amount actually used
template<class CType>
bool MemObj<CType>::Prune()
{
	if ( m_bDirectAlloc || m_mData.Capacity() == m_mData.Count() )
		return true;

	if ( !Reserve(m_mData.Count()) )
		return false;

	return true;
}

////////////////////////////////////////////////////////////////////////////
// Set Memory Size

template<class CType>
bool MemObj<CType>::Set(size_t uElNo)
{
	if (uElNo == m_mData.Count()) return true;

	if ( m_uInitElNo > uElNo )
	{
		if ( !InitElements(uElNo) )
			return false;

		if ( !m_mData.Set(uElNo) )
			return false;
	}
	else if ( m_uInitElNo < uElNo )
	{
		if ( !m_mData.Set(uElNo) )
			return false;

		if ( !InitElements(uElNo) )
			return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////
template<class CType>
bool MemObj<CType>::PushBack( const CType& xValue )
{
	size_t uElNo = m_mData.Count();
	CType *pValue = new CType( xValue );

	if ( !pValue )
		return false;

	if ( !m_mData.Set(uElNo + 1) )
	{
		delete pValue;
		return false;
	}

	m_mData[uElNo] = pValue;
	m_uInitElNo = uElNo + 1;

	return true;
}

////////////////////////////////////////////////////////////////////////////
template<class CType>
bool MemObj<CType>::PushBack( const MemObj<CType> &mData )
{
	size_t uCurElNo = m_mData.Count();
	size_t uCnt = mData.Count();
	bool bResult = true;

	if ( !m_mData.Set( uCurElNo + uCnt ) )
		return false;

	for ( size_t uPos = 0; uPos < uCnt; ++uPos )
	{
		CType* pValue = new CType(mData[uPos]);
		if ( !pValue )
			bResult = false;

		m_mData[uCurElNo + uPos] = pValue;
	}

	m_uInitElNo = uCurElNo + uCnt;

	if ( !bResult )
		Set(uCurElNo);

	return bResult;
}

////////////////////////////////////////////////////////////////////////////
template<class CType>
bool MemObj<CType>::PushBack( CType* pValue )
{
	size_t uElNo = m_mData.Count();

	if ( !pValue )
		return false;

	if ( !m_mData.Set( uElNo + 1 ) )
	{
		delete pValue;
		return false;
	}

	m_mData[uElNo] = pValue;
	m_uInitElNo = uElNo + 1;

	return true;
}



////////////////////////////////////////////////////////////////////////////
// Swap two elements in list

template<class CType>
bool MemObj<CType>::Swap(size_t uPos1, size_t uPos2)
{
	PCType pDummy;
	size_t uCount = m_mData.Count();

	if (uPos1 >= uCount || uPos2 >= uCount) 
		return false;

	if (uPos1 == uPos2) 
		return true;

	pDummy = m_mData[uPos1];
	m_mData[uPos1] = m_mData[uPos2];
	m_mData[uPos2] = pDummy;

	return true;
}

