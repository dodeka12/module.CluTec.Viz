////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      VarList.cpp
//
// summary:   Implements the variable list class
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

#include "StdAfx.h"
#include "VarList.h"
#include "CodeVar.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CVarList::CVarList()
{
	m_pCodeList = 0;
	SetBlockSize(32);
}

CVarList::CVarList(const CVarList& rVarList)
{
	SetBlockSize(rVarList.GetBlockSize());
	*this = rVarList;
}

CVarList::~CVarList()
{

}


CVarList& CVarList::operator= (const CVarList& rVarList)
{
	int i, iNo = (int) rVarList.Count();
	
	Set(iNo);

	for(i=0;i<iNo;i++)
	{
		(*this)[i].CopyInstance(rVarList[i]);	
	}

	return *this;
}

//////////////////////////////////////////////////////////////////////
// Return reference to dereferences var ptr variable

CCodeVar& CVarList::operator() (size_t nPos)
{
	return (*this)[nPos].DereferenceVarPtr(true);
}


//////////////////////////////////////////////////////////////////////
// Dereference Pointer variables in list

CVarList& CVarList::DereferencePtr()
{
	int i, iLen = int(Count());
	
	for(i=0;i<iLen;i++)
	{
		(*this)[i].DereferencePtr();
	}

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Order List according to index list

bool CVarList::Order(vector<int> &rIdxList)
{
	Mem<CCodeVar*> mNewData; // Data: Pointers to CType 	
	int iPos, iCount = int(rIdxList.size());

	if (iCount != m_mData.Count())
		return false;

	mNewData.Set(iCount);
	for (iPos = 0; iPos < iCount; iPos++)
	{
		mNewData[iPos] = m_mData[rIdxList[iPos]];
	}

	m_mData = mNewData;

	return true;
}

