////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      CodeData.h
//
// summary:   Declares the code data class
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

#if !defined(AFX_CODEDATA_H__EB938ED0_DA28_4FE4_B2BD_7DDC3EB2914C__INCLUDED_)
#define AFX_CODEDATA_H__EB938ED0_DA28_4FE4_B2BD_7DDC3EB2914C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeElement.h"
#include "CodeVar.h"
#include "CodeBase.h"


class CCodeData : public CCodeElement  
{
public:
	CCodeData();
	virtual ~CCodeData();

	CCodeVar& RefData() { return m_VarData; }

	virtual bool Apply(CCodeBase* pCodeBase, SCodeData *pData = 0);

protected:
	CCodeVar m_VarData;


};

#endif // !defined(AFX_CODEDATA_H__EB938ED0_DA28_4FE4_B2BD_7DDC3EB2914C__INCLUDED_)
