////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      CodeFunction.h
//
// summary:   Declares the code function class
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

#if !defined(AFX_CODEFUNCTION_H__9C9569AC_1CA2_4F36_9812_43D67E5B164F__INCLUDED_)
#define AFX_CODEFUNCTION_H__9C9569AC_1CA2_4F36_9812_43D67E5B164F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeElement.h"
#include "CodeBase.h"
#include "CodeVar.h"
#include "CLUCodeBase.h"

#ifdef _GNUCPP3_
#define STDFUNC(funcType, funcName) (static_cast<funcType>(&CCLUCodeBase::funcName)) 
#else
#define STDFUNC(funcType, funcName) ((funcType)(&CCLUCodeBase::funcName)) 
#endif
typedef bool (CCodeBase::*TFuncPtr)(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

typedef bool (*T_ExtFunc) ( CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos );

struct SMapExtFunc
{
	const char *pcName;
	T_ExtFunc pFunc;
};

typedef bool (*T_InitModule) ();
typedef bool (*T_FinalizeModule) ();
typedef bool (*T_InitViz) ();
typedef bool (*T_GetFuncList) ( const SMapExtFunc** pMapFunc, int *piFuncCnt );


class CCodeFunction : public CCodeElement  
{
public:
	CCodeFunction();
	CCodeFunction( const CCodeFunction& rCodeFunc );
	virtual ~CCodeFunction();

	CCodeFunction& operator= (const CCodeFunction& rCodeFunc );

	T_ExtFunc GetFunc() { return m_pFunc; }

	void SetFunc(const char* pcName, T_ExtFunc pFunc)
	{ SetName(pcName); m_pFunc = pFunc; }

	virtual bool Apply(CCodeBase* pCodeBase, SCodeData *pData = 0);

protected:
	T_ExtFunc m_pFunc;
};

#endif // !defined(AFX_CODEFUNCTION_H__9C9569AC_1CA2_4F36_9812_43D67E5B164F__INCLUDED_)
