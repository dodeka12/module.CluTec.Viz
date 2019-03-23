////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      dynlist.h
//
// summary:   Declares the dynlist class
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

// DynList Class (Dynamic List) -------------------------------------
#ifndef _DYNLIST_HH_
#define _DYNLIST_HH_

#include"mem.h"




template<class CType>
class DynList
{
public:
	
	typedef CType* PCType;

	DynList();
	DynList(const CType &neret);
	DynList(const DynList<CType> &a);
	virtual ~DynList();
	
	CType& operator[](size_t pos) const     // Returns Object at pos in List
	{ return ( (pos < list.Count()) ? *(list[pos]) : *eret ); }
	
	void Empty() { Sub(0, list.Count()); }
	size_t Count() const { return list.Count(); }
	
//protected:
	Mem<PCType> list;
	CType *eret;                  // Instance returned if error
	
	size_t Set(const DynList<CType> &a);
	
	// The add functions return 0 if unsuccessful and the position
	// of the newly added element +1 if successful.
	size_t Add(const CType &a);
	size_t Add(const CType &a, size_t pos);
	size_t AddNew(); // Adds element without initializing it
	
	size_t Sub(size_t pos, size_t mno=1);
};

#endif
