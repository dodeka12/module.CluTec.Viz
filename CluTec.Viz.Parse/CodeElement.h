////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      CodeElement.h
//
// summary:   Declares the code element class
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

#if !defined(AFX_CODEELEMENT_H__E6C5FE42_FABC_480E_AA2A_AEF202ED542C__INCLUDED_)
#define AFX_CODEELEMENT_H__E6C5FE42_FABC_480E_AA2A_AEF202ED542C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CluTec.Viz.Xml\XML.h"

class CCodeBase;
class CCodeElementList;


struct SCodeData
{
	int iTextLine;
	int iTextPos;
	CCodeElementList *pParentList;

	void Set(int iLine, int iPos, CCodeElementList *pParent)
	{ iTextLine = iLine; iTextPos = iPos; pParentList = pParent; }
};


class CCodeElement  
{
public:
	CCodeElement();
	virtual ~CCodeElement();

	// Executes Code. Needs to implement functionality of code element.
	virtual bool Apply(CCodeBase* pCodeBase, SCodeData *pData = 0) { return true; }

	// Generates XML code of element. Needs to implement serialization of additional
	// variables in derived classes.
	virtual bool Serialize( CXMLTree &xmlTree );

	CStrMem GetName() const { return m_csName; }
	void SetName(const char* pcName) { m_csName = pcName; }

protected:
	CStrMem m_csName;
};

#endif // !defined(AFX_CODEELEMENT_H__E6C5FE42_FABC_480E_AA2A_AEF202ED542C__INCLUDED_)
