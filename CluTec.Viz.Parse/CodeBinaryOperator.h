////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      CodeBinaryOperator.h
//
// summary:   Declares the code binary operator class
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

#if !defined(AFX_CODEBINARYOPERATOR_H__8C57040D_ADF2_4A03_9158_EB04418BB758__INCLUDED_)
#define AFX_CODEBINARYOPERATOR_H__8C57040D_ADF2_4A03_9158_EB04418BB758__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeElement.h"
#include "CLUCodeBase.h"
#include "CodeVar.h"

class CCodeBinaryOperator : public CCodeElement  
{
public:
	CCodeBinaryOperator();
	virtual ~CCodeBinaryOperator();

	virtual bool Apply(CCodeBase* pCodeBase, SCodeData *pData = 0);

	virtual bool Evaluate(CCLUCodeBase* pBase, CCodeVar& rLVar, CCodeVar& rRVar, 
						  CCodeVar& rResVar, int iLine, int iPos);
};

#endif // !defined(AFX_CODEBINARYOPERATOR_H__8C57040D_ADF2_4A03_9158_EB04418BB758__INCLUDED_)
