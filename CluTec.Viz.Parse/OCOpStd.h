////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      OCOpStd.h
//
// summary:   Declares the oc operation standard class
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

// OCOpStd.h: Schnittstelle f�r die Klasse COCOpStd.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCOPSTD_H__19593133_8CC9_4FD2_98B9_FDE6822C5E96__INCLUDED_)
#define AFX_OCOPSTD_H__19593133_8CC9_4FD2_98B9_FDE6822C5E96__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OCOperator.h"
#include "CodeOperator.h"

class COCOpStd : public COCOperator  
{
public:
	COCOpStd();
	virtual ~COCOpStd();

	bool Init(const char *pcSym, int iOpLevelUnary, int iOpLevelBinary, 
			  bool bRightToLeft,  bool bVoidOp, bool bRecurseLists,
			  TOpUnaryPtr UFunc, TOpBinaryPtr BFunc,
			  TOpAUnaryPtr AUFunc, TOpABinaryPtr ABFunc);

protected:
	CCodeOperator m_Operator;
};

#endif // !defined(AFX_OCOPSTD_H__19593133_8CC9_4FD2_98B9_FDE6822C5E96__INCLUDED_)
