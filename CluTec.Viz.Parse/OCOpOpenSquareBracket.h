////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      OCOpOpenSquareBracket.h
//
// summary:   Declares the oc operation open square bracket class
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

//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCOPOPENSQUAREBRACKET_H__33EE4C70_DDB1_446C_AE35_F85D0F0D391B__INCLUDED_)
#define AFX_OCOPOPENSQUAREBRACKET_H__33EE4C70_DDB1_446C_AE35_F85D0F0D391B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ObjectClass.h"
#include "CodeCreateVarList.h"

class COCOpOpenSquareBracket : public CObjectClass
{
public:
	COCOpOpenSquareBracket();
	virtual ~COCOpOpenSquareBracket();

	virtual bool Parse(CParseBase& rParse, CCodeElementList& rCode);

protected:
	CCodeCreateVarList *m_pCreateVarList;

};

#endif // !defined(AFX_OCOPOPENSQUAREBRACKET_H__33EE4C70_DDB1_446C_AE35_F85D0F0D391B__INCLUDED_)
