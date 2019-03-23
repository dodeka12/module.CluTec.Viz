////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      OCKeywordLoop.h
//
// summary:   Declares the oc keyword loop class
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

// OCOpOpenCurlyBracket.h: Schnittstelle f�r die Klasse COCOpOpenCurlyBracket.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCKEYWORD_LOOP_H__A6A5D504_0F9A_40B2_AA9E_7A4BF08B4C02__INCLUDED_)
#define AFX_OCKEYWORD_LOOP_H__A6A5D504_0F9A_40B2_AA9E_7A4BF08B4C02__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ObjectClass.h"
#include "OCIDSymDef.h"

class COCKeywordLoop : public CObjectClass  
{
public:
	COCKeywordLoop();
	virtual ~COCKeywordLoop();

	virtual bool Parse(CParseBase& rParse, CCodeElementList& rCode);
};

#endif // !defined(AFX_OCKEYWORD_IF_H__A6A5D504_0F9A_40B2_AA9E_7A4BF08B4C02__INCLUDED_)
