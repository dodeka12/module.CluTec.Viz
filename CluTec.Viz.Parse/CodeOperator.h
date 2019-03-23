////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      CodeOperator.h
//
// summary:   Declares the code operator class
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

#if !defined(AFX_CODEOPERATOR_H__FE0003E3_51AB_493F_8BFF_C11861D5C8BB__INCLUDED_)
#define AFX_CODEOPERATOR_H__FE0003E3_51AB_493F_8BFF_C11861D5C8BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeElement.h"
#include "CLUCodeBase.h"
#include "CodeVar.h"

typedef bool (CCodeBase::*TOpUnaryPtr)(CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
typedef bool (CCodeBase::*TOpBinaryPtr)(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);

typedef bool (CCodeBase::*TOpAUnaryPtr)(CCodeVar& rRVar, int iLine, int iPos);
typedef bool (CCodeBase::*TOpABinaryPtr)(CCodeVar& rLVar, CCodeVar& rRVar, int iLine, int iPos);

#ifdef _GNUCPP3_
#define OPFUNC(funcType, funcName) (static_cast<funcType>(&CCLUCodeBase::funcName)) 
#else
#define OPFUNC(funcType, funcName) ((funcType)(&CCLUCodeBase::funcName)) 
#endif

class CCodeOperator : public CCodeElement  
{
public:
	CCodeOperator();
	virtual ~CCodeOperator();

	virtual bool Init(TOpUnaryPtr UFunc, TOpBinaryPtr BFunc, 
					  TOpAUnaryPtr AUFunc, TOpABinaryPtr ABFunc,
					  bool bRecurseLists);

	virtual bool Apply(CCodeBase* pCodeBase, SCodeData *pData = 0);

protected:
	// If true operator modifies one of its parameters 
	// instead of creating a temporary variable with its result.
	bool m_bAssign;

	// If true the operator can be binary, i.e. it expects two parameters on the stack.
	bool m_bBinary;

	// If true the operator can be unary, i.e. it also works with one parameter on the stack.
	bool m_bUnary;

	// If true lists are applied to operator recursively. (default = true)
	bool m_bRecurseLists;
	
	// Pointers to functions implementing operator
	TOpUnaryPtr m_UnaryFunc;		// Unary
	TOpBinaryPtr m_BinaryFunc;		// Binary

	TOpAUnaryPtr m_AUnaryFunc;		// Assign Unary
	TOpABinaryPtr m_ABinaryFunc;	// Assign Binary

	// auxiliary function for apply, written by Daniel Grest Jan. 2002
	// changed 28.1.02 by C.Perwass
	bool Eval(CCodeBase* pCodeBase, SCodeData *pData, 
				CCodeVar &pvL, CCodeVar &pvR, bool isBinary);

};

#endif // !defined(AFX_CODEOPERATOR_H__FE0003E3_51AB_493F_8BFF_C11861D5C8BB__INCLUDED_)
