////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      memobj.h
//
// summary:   Declares the memobj class
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

// MemObj Class ---------------------------------------------
//
// This class can be used for all objects. It uses new/delete and copies with operator=
// If this is not necessary use Mem class, to increase speed.

#ifndef _MEMOBJ_HH_
#define _MEMOBJ_HH_

#include "mem.h"

typedef unsigned int uint;

template<class CType>
class MemObj
{
public:
	typedef CType *PCType;

	MemObj(size_t nno=0);
	MemObj(const MemObj<CType> &a);
	virtual ~MemObj();  
	
	// If direct alloc is enabled, each Add, Sub, Set command
	// directly changes the allocated memory. This is the default.
	// If this is disabled, then a capacity can be set and 
	// Add, Sub, Set commands use this capacity if possible and do
	// not call realloc().
	void EnableDirectAlloc(bool bEnable = true) 
	{ 
		if ( bEnable ) Prune();
		m_mData.EnableDirectAlloc(bEnable); 
	}

	// reserve memory for the given number of elements
	// calling reserve automatically disables direct alloc
	bool Reserve(size_t uNo);

	// reduce the allocated memory to amount actually used
	bool Prune();

	bool Add(size_t ano) { return Set(m_mData.Count() + ano); }
	bool Sub(size_t ano) { size_t no = m_mData.Count(); return Set((ano >= no ? 0 : no-ano)); }
	bool Set(size_t ano);
	bool PushBack( const CType& xValue );
	bool PushBack( const MemObj<CType> &mData );
	bool PushBack( CType* pValue );

	bool operator+=(size_t ano) { return Add(ano); }
	bool operator-=(size_t ano) { return Sub(ano); }
	bool operator++(int) { return Add(1); }
	bool operator--(int) { return Sub(1); }
	bool operator++() { return Add(1); }
	bool operator--() { return Sub(1); }

	bool operator=(size_t ano) { return Set(ano); }
	MemObj<CType>& operator=(const MemObj<CType> &a);

	CType& operator[] (size_t pos) const { return *m_mData[pos]; }

	CType& Last() const
	{ return (*this)[(m_mData.Count() > 0 ? m_mData.Count()-1 : 0)]; }

	size_t Count() const { return m_mData.Count(); }
	size_t Capacity() const { return m_mData.Capacity(); }

	void SetBlockSize(size_t uBlockSize) { m_mData.SetBlockSize(uBlockSize); }
	size_t GetBlockSize() const { return m_mData.GetBlockSize(); }

	// Moves Objects in list WITHOUT overwriting
	bool Move(size_t dest, size_t start, size_t mno);
	bool Insert(size_t pos, size_t mno=1);
	bool Del(size_t pos, size_t mno=1);
	bool Forget(size_t uPos, size_t uCnt = 1);

	size_t IndexOf(CType* pEl);
	bool Replace(size_t uPos, CType* pData);

	// Swap two elements in list
	bool Swap(size_t uPos1, size_t uPos2);

protected:
	bool InitElements(size_t uElNo);

protected:
	bool m_bDirectAlloc;
	size_t m_uInitElNo;
	Mem<PCType> m_mData; // Data: Pointers to CType 	
};


#endif
