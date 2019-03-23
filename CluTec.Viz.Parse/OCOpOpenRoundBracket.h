////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      OCOpOpenRoundBracket.h
//
// summary:   Declares the oc operation open round bracket class
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

// OCOpOpenRoundBracket.h: Schnittstelle f�r die Klasse COCOpOpenRoundBracket.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCOPOPENROUNDBRACKET_H__33EE4C70_DDB1_446C_AE35_F85D0F0D391B__INCLUDED_)
#define AFX_OCOPOPENROUNDBRACKET_H__33EE4C70_DDB1_446C_AE35_F85D0F0D391B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ObjectClass.h"
#include "CodeCreateRefVarList.h"

class COCOpOpenRoundBracket : public CObjectClass
{
public:
	COCOpOpenRoundBracket();
	virtual ~COCOpOpenRoundBracket();

	virtual bool Parse(CParseBase& rParse, CCodeElementList& rCode);

protected:
	CCodeCreateRefVarList *m_pCreateRefVarList;

};

#endif // !defined(AFX_OCOPOPENROUNDBRACKET_H__33EE4C70_DDB1_446C_AE35_F85D0F0D391B__INCLUDED_)
