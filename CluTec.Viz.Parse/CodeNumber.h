////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      CodeNumber.h
//
// summary:   Declares the code number class
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

#if !defined(AFX_CODENUMBER_H__D52ED04E_4051_4BF7_9828_44C4BA89E2AD__INCLUDED_)
#define AFX_CODENUMBER_H__D52ED04E_4051_4BF7_9828_44C4BA89E2AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeElement.h"
#include "CodeVar.h"
#include "CodeBase.h"

class CCodeNumber : public CCodeElement  
{
public:
	CCodeNumber();
	virtual ~CCodeNumber();

	CCodeVar& RefNumber() { return m_VarNumber; }

	virtual bool Apply(CCodeBase* pCodeBase, SCodeData *pData = 0);

protected:
	CCodeVar m_VarNumber;

};

#endif // !defined(AFX_CODENUMBER_H__D52ED04E_4051_4BF7_9828_44C4BA89E2AD__INCLUDED_)
