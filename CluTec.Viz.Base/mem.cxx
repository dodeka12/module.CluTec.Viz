////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      mem.cxx
//
// summary:   memory class
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

#include<stdlib.h>
#include<memory.h>
#include<string.h>

#include"mem.h"


//////////////////////////////////////////////////////////////////////////////////
// Constructor

template<class CType>
Mem<CType>::Mem(size_t nno) : MemBase(sizeof(CType), nno)
{

}


////////////////////////////////////////////////////////////////////////////
// Copy Constructor

template<class CType>
Mem<CType>::Mem(const Mem<CType> &a) : MemBase(a)
{
}


////////////////////////////////////////////////////////////////////////////
// Destructor

template<class CType>
Mem<CType>::~Mem()
{
}



////////////////////////////////////////////////////////////////////////////
// Set Memory Data

template<class CType>
Mem<CType>& Mem<CType>::operator=(const Mem<CType> &a)
{
	MemBase::operator=(a);

	return *this;
}

////////////////////////////////////////////////////////////////////////////
template<class CType>
bool Mem<CType>::operator=(size_t ano) 
{ 
	return Set(ano); 
}

////////////////////////////////////////////////////////////////////////////
template<class CType>
CType& Mem<CType>::operator[] (size_t pos) const
{ 
	return ( (pos < m_uElNo) ? ((CType *)m_pcData)[pos] : ((CType *)m_pcData)[0] ); 
}

////////////////////////////////////////////////////////////////////////////
template<class CType>
CType& Mem<CType>::Last() const
{ 
	return (*this)[(m_uElNo > 0 ? m_uElNo-1 : 0)]; 
}

////////////////////////////////////////////////////////////////////////////
template<class CType>
bool Mem<CType>::PushBack(const CType& xValue)
{
	if ( !Add(1) ) return false;
	Last() = xValue;
	return true;
}

////////////////////////////////////////////////////////////////////////////
template<class CType>
bool Mem<CType>::PushBack(const Mem<CType> &mData)
{
	size_t iIdx = Count();
	size_t iCnt = mData.Count();
	if ( !Add(iCnt) ) return false;

	memcpy(&(Data()[iIdx]), mData.Data(), iCnt * sizeof(CType));
	return true;
}


////////////////////////////////////////////////////////////////////////////
template<class CType>
CType* Mem<CType>::Data() const 
{ 
	return (CType *) m_pcData;
} 
