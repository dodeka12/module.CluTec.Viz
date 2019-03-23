////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      CodeElementList.h
//
// summary:   Declares the code element list class
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

#if !defined(AFX_CODEELEMENTLIST_H__5746B287_573C_4800_9DE1_B801F31D0EB4__INCLUDED_)
#define AFX_CODEELEMENTLIST_H__5746B287_573C_4800_9DE1_B801F31D0EB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeElement.h"

class CCodeBase;

struct SCodeElementPtr
{
	// if bOwner == true, then list owns this element 
	// pointed to by pElement. In this case Delete
	// calls delete pElement; (!)
	bool bOwner;

	// Text Line in which code element appears
	int iTextLine;

	// Text Position at which code element starts.
	int iTextPos;

	CCodeElement *pElement;
};

class CCodeElementList : public CCodeElement  
{
public:
	CCodeElementList();
	virtual ~CCodeElementList();

	int Count() { return (int) m_mElementList.Count(); }
	bool Reset() { return Delete(0, (int)m_mElementList.Count()); }

	CCodeElementList& operator << (CCodeElement& rElement) { Add(rElement); return *this; }
	CCodeElement* operator[] (int iPos) { return m_mElementList[iPos].pElement; }

	// if bOwner == true then list can call delete on Element.
	bool Add(CCodeElement& rElement, bool bOwner = false,
			int iTextLine = -1, int iTextPos = -1) 
	{ return Insert(rElement, -1, bOwner, iTextLine, iTextPos); }

	bool Insert(CCodeElement& rElement, int iPos = -1, bool bOwner = false,
				int iTextLine = -1, int iTextPos = -1);

	// DeleteInSubLists gives levels of sub lists in which element is to be deleted.
	// All instances of an element are deleted.
	bool Delete(CCodeElement& rElement, int DeleteInSubLists = 0);
	bool Delete(CStrMem& rName, int DeleteInSubLists = 0);

	// Delete iNo elements starting from iPos
	bool Delete(int iPos, int iNo = 1);

	virtual bool Apply(CCodeBase* pCodeBase, SCodeData *pData = 0);
	virtual bool Serialize( CXMLTree &xmlTree );

protected:

	Mem<SCodeElementPtr> m_mElementList;
};

#endif // !defined(AFX_OGLBASEELEMENTLIST_H__5746B287_573C_4800_9DE1_B801F31D0EB4__INCLUDED_)
