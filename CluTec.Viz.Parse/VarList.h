////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      VarList.h
//
// summary:   Declares the variable list class
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

#if !defined(AFX_VARLIST_H__049E04F1_8531_48E2_89AA_1D6DCF05B309__INCLUDED_)
#define AFX_VARLIST_H__049E04F1_8531_48E2_89AA_1D6DCF05B309__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeElementList.h"
#include "CluTec.Viz.Base\memobj.h"
//#include "CodeVar.h"

class CCodeVar;

class CVarList : public MemObj<CCodeVar>  
{
public:
	CVarList();
	CVarList(const CVarList& rVarList);
	virtual ~CVarList();

	CVarList& operator= (const CVarList& rVarList);
	// Return reference to dereferences var ptr variable
	CCodeVar& operator() (size_t iPos);
//	{ return (*this)[iPos].DereferenceVarPtr(true); }

	void SetCodeList(CCodeElementList *pCodeList) { m_pCodeList = pCodeList; }
	CCodeElementList* GetCodeList() { return m_pCodeList; }

	// Dereference Pointer variables in list
	CVarList& DereferencePtr();

	bool Order(vector<int> &rIdxList);

protected:
	// m_pCodeList gives pointer to CodeElementList which
	// contains the code lines that created elements of
	// Variable List. This parameter does not need to be set.
	// It is zero if CodeElementList is not available.
	CCodeElementList *m_pCodeList;
};

#endif // !defined(AFX_VARLIST_H__049E04F1_8531_48E2_89AA_1D6DCF05B309__INCLUDED_)
