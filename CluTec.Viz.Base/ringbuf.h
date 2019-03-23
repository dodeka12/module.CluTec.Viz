////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      ringbuf.h
//
// summary:   Declares the ringbuf class
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

//////////////////////////////////////////////////////////////////
/// RingBuf Class Declaration
/// Defines ring buffer
//////////////////////////////////////////////////////////////////

#ifndef _RINGBUF_H_
#define _RINGBUF_H_

#include "mem.h"
#include "CStrMem.h"
#include "makestr.h"

#define RING_OVERWRITE 1
#define RING_NOOVERWRITE 2

#define RING_ABSOLUTE_STR 1
#define RING_RELATIVE_STR 2

#ifdef _GNUCPP3_
	#ifndef _TMPL_
	#define _TMPL_ <>
	#endif
#else
	#ifndef _TMPL_
	#define _TMPL_
	#endif
#endif

typedef unsigned int uint;

/*
#ifdef _GNUCPP3_
template<class CType> class RingBuf;

	// Add element to ring
	template<class CType> RingBuf<CType>& operator, (RingBuf<CType>& R, const CType& d);

	// Retrieve element from Ring
	template<class CType> RingBuf<CType>& operator, (CType& d, RingBuf<CType>& R);

	// Reset Ring Buffer 
	template<class CType> RingBuf<CType>& operator! (RingBuf<CType>& R);
#endif
*/

template<class CType>
class RingBuf
{
	public:

	RingBuf();
	RingBuf(uint nno, uint ntype = RING_OVERWRITE);
	RingBuf(const RingBuf<CType>& R);

	// Resize Ring
	uint Resize(uint nno);

	// Reset Ring
	uint Reset(uint nno, uint ntype = RING_OVERWRITE);

	// Equal operator
	RingBuf<CType>& operator= (const RingBuf<CType>& R);

	// Add element to ring
	RingBuf<CType>& operator<< (const CType& d);

	// Retrieve element from Ring
	RingBuf<CType>& operator>> (CType& d);

	// Reset Ring Buffer 
	RingBuf<CType>& operator! ();

	// Return element in ring, relative to fpos;
	CType operator[] (uint pos);

	// Return Pointer to Data array
	operator CType*() { return data.Data(); }

	// Create String Output
	CStrMem Str(uint how = RING_ABSOLUTE_STR);

	protected:
		void Empty();
		uint add(const CType& d);
		CType take();
		
		uint full; // if buffer is full set to 1
		uint no; // No. of elements
		uint lpos; // Pos. of next new element
		uint fpos; // Pos. of first element
		uint type; // Type of Ring

		CStrMem str; // String for output
		Mem<CType> data; // Contents of Ring
};

#endif

