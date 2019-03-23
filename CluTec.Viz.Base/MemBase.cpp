////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      MemBase.cpp
//
// summary:   Implements the memory base class
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

// Mem Class Definitions ------------------------------------

#include "malloc.h"

#include<stdlib.h>
#include<memory.h>
#include<string.h>

#include "crtdbg.h"
#include"MemBase.h"


size_t MemBase::sm_uTotalAllocCount = 0;

//////////////////////////////////////////////////////////////////////////////////
// Constructor

MemBase::MemBase(size_t _uElSize, size_t _uElNo)
{
	m_bDirectAlloc = true;

	m_uElNo = 0;
	m_uCapElNo = 0;
	m_uBlockSize = 32;

	m_uElSize = _uElSize;
	m_pcData = 0;

	Set(_uElNo);
}


////////////////////////////////////////////////////////////////////////////
// Copy Constructor

MemBase::MemBase(const MemBase &a)
{
	m_pcData = 0;

	*this = a;
}


////////////////////////////////////////////////////////////////////////////
// Destructor

MemBase::~MemBase()
{
#ifdef _DEBUG
	if (m_pcData) free(m_pcData);
#else
	if (m_pcData) free(m_pcData);
#endif
}


////////////////////////////////////////////////////////////////////////////
// Set Memory Data

MemBase& MemBase::operator=(const MemBase &a)
{
	m_uElSize = a.m_uElSize;
	m_uBlockSize = a.m_uBlockSize;

	m_uElNo = 0;
	m_uCapElNo = 0;

	if ( !(m_bDirectAlloc = a.m_bDirectAlloc) )
	{
		Reserve(a.m_uCapElNo);
	}

	if ( Set(a.m_uElNo) )
	{
		size_t size = m_uElNo * m_uElSize;

		if (size)
			memcpy(m_pcData, a.m_pcData, size);
	}

	return *this;
}


////////////////////////////////////////////////////////////////////////////
// Allocate Memory
bool MemBase::Alloc(size_t nElNo)
{
	++sm_uTotalAllocCount;

	size_t nSize = ((nElNo == 0) ? 1 : nElNo) * m_uElSize;

	if (m_pcData)
	{
		char *pcNewData = 0;

#ifdef CLU_DEBUG_MEM_ALLOC
		if ( !(pcNewData = (char *) _realloc_dbg(m_pcData, nSize, _NORMAL_BLOCK, __FILE__, __LINE__)) ) 
		{
			//_ASSERTE(true);
			return false;
		}
#else
		if ( !(pcNewData = (char *) realloc(m_pcData, nSize)) ) return false;
#endif

		m_pcData = pcNewData;
	}
	else
	{
#ifdef CLU_DEBUG_MEM_ALLOC
		//if ( !(m_pcData = (char *) malloc(uSize)) ) return false;
		if ( !(m_pcData = (char *) _malloc_dbg(nSize, _NORMAL_BLOCK, __FILE__, __LINE__)) )
			return false;
#else
		if ( !(m_pcData = (char *) malloc(nSize)) ) return false;
#endif
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////
size_t MemBase::MapToBlockSize(size_t uCapElNo)
{
	if ( m_uBlockSize > 1 )
	{
		size_t uCurBlockCnt = m_uCapElNo / m_uBlockSize;
		size_t uBlockCnt = uCapElNo / m_uBlockSize;

		if ( (uCapElNo % m_uBlockSize) > 0 )
			++uBlockCnt;

		if ( (m_uCapElNo % m_uBlockSize) > 0 )
			++uCurBlockCnt;

		if ( uBlockCnt == 0 )
		{
			uCapElNo = m_uBlockSize;
		}
		else if ( uBlockCnt < uCurBlockCnt )
		{
			if ( (m_uElNo - uCapElNo) >  m_uBlockSize + (m_uBlockSize >> 1) )
			{
				uCapElNo = uBlockCnt * m_uBlockSize;
			}
			else
			{
				uCapElNo = uCurBlockCnt * m_uBlockSize;
			}
		}
		else
		{
			uCapElNo = uBlockCnt * m_uBlockSize;
		}
	}

	return uCapElNo;
}

////////////////////////////////////////////////////////////////////////////
// Set Memory Size

bool MemBase::Reserve(size_t uCapElNo)
{
	// if asked to reserve less than is already in use
	// reserve the amount that is in use
	if ( uCapElNo < m_uElNo )
	{
		uCapElNo = m_uElNo;
	}

	uCapElNo = MapToBlockSize(uCapElNo);

	// if asked to reserve more than is currently used
	// allocate the new memory
	if ( !Alloc(uCapElNo) )
		return false;

	// Only set capacity element counter to new value
	m_uCapElNo = uCapElNo;

	// switch to indirect allocation mode
	m_bDirectAlloc = false;

	return true;
}

////////////////////////////////////////////////////////////////////////////
// reduce the allocated memory to amount actually used
bool MemBase::Prune()
{
	if ( m_bDirectAlloc || m_uCapElNo == m_uElNo )
		return true;

	size_t uCapElNo = MapToBlockSize(m_uElNo);

	if ( !Alloc(uCapElNo) )
		return false;

	m_uCapElNo = uCapElNo;

	return true;
}

////////////////////////////////////////////////////////////////////////////
// Set Memory Size

bool MemBase::Set(size_t uElNo)
{
	if (uElNo == m_uElNo && m_uElNo > 0) return true;

	size_t uCapElNo = MapToBlockSize(uElNo);

	if ( m_bDirectAlloc )
	{
		if ( uCapElNo != m_uCapElNo )
		{
			if ( !Alloc(uCapElNo) )
				return false;

			m_uCapElNo = uCapElNo;
		}

		m_uElNo = uElNo;
	}
	else
	{
		if ( uCapElNo <= m_uCapElNo )
		{
			m_uElNo = uElNo;
		}
		else if ( uCapElNo > m_uCapElNo )
		{
			if ( !Alloc(uCapElNo) )
				return false;

			m_uElNo = uElNo;
			m_uCapElNo = uCapElNo;
		}
	}

	return true;
}



////////////////////////////////////////////////////////////////////////////
// Move parts of Memory

bool MemBase::Move(size_t dest, size_t start, size_t mno)
{
	if (dest+mno > m_uElNo || start+mno > m_uElNo) return false;

	memmove(&m_pcData[dest*m_uElSize], &m_pcData[start*m_uElSize], mno * m_uElSize);

	return true;
}


////////////////////////////////////////////////////////////////////////////
// Insert Memory

bool MemBase::Insert(size_t pos, size_t mno)
{
	size_t ono = m_uElNo;

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

bool MemBase::Del(size_t pos, size_t mno)
{
	if (pos+1 < m_uElNo)
	{ 
		if (!Move(pos, pos+mno, m_uElNo-pos-mno))
			return false;
	}

	Sub(mno);

	return true;
}




