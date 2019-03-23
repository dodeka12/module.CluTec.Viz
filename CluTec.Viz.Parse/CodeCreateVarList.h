////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      CodeCreateVarList.h
//
// summary:   Declares the code create variable list class
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

#if !defined(AFX_CODECREATEVARLIST_H__36B61796_A062_43BD_BFF1_C4903D56619D__INCLUDED_)
#define AFX_CODECREATEVARLIST_H__36B61796_A062_43BD_BFF1_C4903D56619D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeElement.h"
#include "CodeVar.h"

class CCodeCreateVarList : public CCodeElement  
{
public:
	CCodeCreateVarList();
	virtual ~CCodeCreateVarList();

	virtual bool Apply(CCodeBase* pCodeBase, SCodeData *pData = 0);

	void SetCodeList(CCodeElementList *pCodeList) { m_pCodeList = pCodeList; }
	CCodeElementList* GetCodeList() { return m_pCodeList; }

protected:
	CCodeElementList *m_pCodeList;
};

#endif // !defined(AFX_CODECREATEVARLIST_H__36B61796_A062_43BD_BFF1_C4903D56619D__INCLUDED_)
