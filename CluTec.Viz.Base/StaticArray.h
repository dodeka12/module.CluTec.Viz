////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      StaticArray.h
//
// summary:   Declares the static array class
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

#ifndef __STATICARRAY_H__
#define __STATICARRAY_H__

typedef unsigned int uint;

// Static Array for non-class types

template<class CType, size_t CNo>
class CStaticArray
{
public:
	CStaticArray() 
	{ memset(pData, 0, CNo*sizeof(CType)); }

	CStaticArray(const CStaticArray<CType,CNo>& np) { *this = np; }

	CStaticArray<CType,CNo>& operator= (const CType *na)
		{ memcpy(pData, na, CNo*sizeof(CType)); return *this; }

	CStaticArray<CType,CNo>& operator= (const CStaticArray<CType,CNo>& np)
		{ memcpy(pData, np.pData, CNo*sizeof(CType)); return *this; }

	CType operator() (size_t pos) const 
		{ return pData[(pos >= CNo ? 0 : pos)]; }

	CType& operator[] (size_t pos)
		{ return pData[(pos >= CNo ? 0 : pos)]; }

	const CType& operator[] (size_t pos) const
		{ return pData[(pos >= CNo ? 0 : pos)]; }

	const CType* Data() const { return pData; }
	
	size_t Dim() const { return CNo; }

	
protected:
	CType pData[CNo]; // Elements

};



// Static array for class types that need constructor to be called
// operator= must be defined!

template<class CType, size_t CNo>
class CStaticArrayObj
{
public:
	CStaticArrayObj() 
	{ for(size_t n=0;n<CNo;n++) pData[n] = CType(0); }

	CStaticArrayObj(const CStaticArrayObj<CType,CNo>& np) { *this = np; }

	CStaticArrayObj<CType,CNo>& operator= (CType *na)
		{ for(size_t i=0;i<CNo;i++) pData[i] = na[i]; return *this; }

	CStaticArrayObj<CType,CNo>& operator= (const CStaticArrayObj<CType,CNo>& np)
		{ for(size_t i=0;i<CNo;i++) pData[i] = np.pData[i]; return *this; }

	CType& operator[] (size_t pos)
		{ return pData[(pos >= CNo ? 0 : pos)]; }

	CType* Data() { return pData; }
	
	int Dim() const { return CNo; }

	
protected:
	CType pData[CNo]; // Elements

};



#endif

