////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      dynlist.cxx
//
// summary:   dynlist class
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

// ------- DynList Library functions

#include"dynlist.h"

////////////////////////////////////////////////////////////////////////////////
// Constructor

template<class CType>
DynList<CType>::DynList()
{
	eret = 0;
}

////////////////////////////////////////////////////////////////////////////////
// Constructor

template<class CType>
DynList<CType>::DynList(const CType &neret)
{
	eret = 0;
}


////////////////////////////////////////////////////////////////////////////////
// Copy Constructor

template<class CType>
DynList<CType>::DynList(const DynList<CType> &a)
{
	eret = 0;
	Set(a);
}


////////////////////////////////////////////////////////////////////////////////
// Destructor

template<class CType>
DynList<CType>::~DynList()
{
	size_t i,cnt = list.Count();
	CType **data = list.Data();
	
	for(i=0;i<cnt;i++)
		if(data[i]) delete data[i];
		
		
	if (eret) 
	{ 
		delete eret;
		eret = 0;
	}
}



////////////////////////////////////////////////////////////////////////////////
// Add Object to List

template<class CType>
size_t DynList<CType>::Add(const CType &a)
{
	CType *nelem = new CType(a);
	
	if (!nelem) return 0;
	
	if (list.Add(1))
	{ 
		size_t pos = list.Count()-1;
		list[pos] = nelem;
		
		return pos+1;		
	}

	return 0;
}


////////////////////////////////////////////////////////////////////////////////
// Insert Object in List

template<class CType>
size_t DynList<CType>::Add(const CType &a, size_t pos)
{    
	size_t cnt = list.Count();
	
	if (pos > cnt) pos = cnt;
	
	CType *nelem = new CType(a);
	
	if (!nelem) return 0;
	
	if (list.Insert(pos))
	{ 
		CType **data = list.Data();
		
		list[pos] = nelem;
		
		return pos+1;
	}
	return 0;
}


////////////////////////////////////////////////////////////////////////////////
// Add Element without initializing it

template<class CType>
size_t DynList<CType>::AddNew()
{    
	CType *nelem = new CType;
	
	if (!nelem) return 0;
	
	if (list.Add(1))
	{ 
		size_t pos = list.Count()-1;
		
		list[pos] = nelem;
		
		return pos+1;		
	}

	return 0;
}


////////////////////////////////////////////////////////////////////////////////
// Delete Blade(s) from List

template<class CType>
size_t DynList<CType>::Sub(size_t pos, size_t mno)
{ 
	if (mno == 0) return 0;
	
	size_t cnt = list.Count();
	CType **data = list.Data();
	
	if (pos+mno > cnt) 
		mno = cnt-pos;
	
	for(size_t i=0;i<mno;i++)
		if (data[pos+i]) delete data[pos+i];
		
	list.Del(pos, mno);
		
	return mno;
}



////////////////////////////////////////////////////////////////////////////////
// Set DynList

template<class CType>
size_t DynList<CType>::Set(const DynList<CType> &a)
{ 
	size_t cnt = a.list.Count();
	size_t i,j;
	
	Empty();
	
	if ((list = cnt))
	{ 
		CType **data = list.Data();
		CType **odata = a.list.Data();
		
		for(i=0;i<cnt;i++) 
		{ 
			if (!(data[i] = new CType(odata[i][0])))
			{ 
				for(j=i;j>0;j--)
					delete data[j-1];
				
				list = 0;
				return 0;
			}
		}
		return cnt;
	}

	return 0;
}


