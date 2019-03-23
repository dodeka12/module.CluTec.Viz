////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      MemBase.h
//
// summary:   Declares the memory base class
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

// MemBase Class ---------------------------------------------
//
// This class can be used for objects that do not need to have their constructor/destructor
// called. It uses memcpy, memmove for copying.
//
// This class forms a base to the template class Mem

#ifndef _MEMBASE_HH_
#define _MEMBASE_HH_


typedef unsigned int uint;

class MemBase
{
public:	
	MemBase(size_t _uElSize, size_t nno = 0);
	MemBase(const MemBase &a);
	virtual ~MemBase();  

protected:
	bool Alloc(size_t uElNo);

public:
	MemBase& operator= (const MemBase& a);

	// If direct alloc is enabled, each Add, Sub, Set command
	// directly changes the allocated memory. This is the default.
	// If this is disabled, then a capacity can be set and 
	// Add, Sub, Set commands use this capacity if possible and do
	// not call realloc().
	void EnableDirectAlloc(bool bEnable = true) 
	{ 
		if ( (m_bDirectAlloc = bEnable) )
			Prune();
	}

	bool IsDirectAlloc() { return m_bDirectAlloc; }

	// reserve memory for the given number of elements
	// calling reserve automatically disables direct alloc
	bool Reserve(size_t uNo);

	// reduce the allocated memory to amount actually used
	bool Prune();

	bool Add(size_t ano) { return Set(m_uElNo + ano); }
	bool Sub(size_t ano) { return Set((ano >= m_uElNo ? 0 : m_uElNo-ano)); }
	bool Set(size_t ano);

	bool operator+=(size_t ano) { return Add(ano); }
	bool operator-=(size_t ano) { return Sub(ano); }
	bool operator++(int) { return Add(1); }
	bool operator--(int) { return Sub(1); }
	bool operator++() { return Add(1); }
	bool operator--() { return Sub(1); }

	bool Move(size_t dest, size_t start, size_t mno);
	bool Insert(size_t pos, size_t mno=1);
	bool Del(size_t pos, size_t mno=1);

	size_t GetSizeOfElement() const { return m_uElSize; }
	size_t Count() const { return m_uElNo; }
	size_t Capacity() const { return m_uCapElNo; }

	void SetBlockSize(size_t uBlockSize) { m_uBlockSize = uBlockSize; Set(m_uElNo); }
	size_t GetBlockSize() const { return m_uBlockSize; }
	size_t MapToBlockSize(size_t uElNo);

	void* Data() const { return m_pcData; }
	void* GetElPtr(size_t uEl) const { return &m_pcData[(uEl >= m_uElNo ? 0 : uEl) * m_uElSize]; }

protected:
	bool m_bDirectAlloc;

	size_t m_uElSize;	// No of bytes per element
	size_t m_uElNo;   // No of elements
	size_t m_uCapElNo;	// capacity number of elements

	// Reserve memory only in blocks of this size.
	// If this is zero then don't use blocks.
	size_t m_uBlockSize;	

	char *m_pcData;		// Data pointer	

private:
	static size_t sm_uTotalAllocCount;
};


#endif
