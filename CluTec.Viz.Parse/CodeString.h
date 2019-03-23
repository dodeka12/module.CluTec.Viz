////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      CodeString.h
//
// summary:   Declares the code string class
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

#if !defined(AFX_CODESTRING_H__9AEFFBC1_0536_11D6_BA34_00E07D8AFD6A__INCLUDED_)
#define AFX_CODESTRING_H__9AEFFBC1_0536_11D6_BA34_00E07D8AFD6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeElement.h"
#include "CodeVar.h"
#include "CodeBase.h"

class CCodeString : public CCodeElement  
{
public:
	CCodeString();
	virtual ~CCodeString();
	CCodeVar& Ref() { return m_VarString; }
	virtual bool Apply(CCodeBase* pCodeBase, SCodeData *pData = 0);
protected:
	CCodeVar m_VarString;
};

#endif // !defined(AFX_CODESTRING_H__9AEFFBC1_0536_11D6_BA34_00E07D8AFD6A__INCLUDED_)
