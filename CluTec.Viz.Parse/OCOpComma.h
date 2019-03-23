////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      OCOpComma.h
//
// summary:   Declares the oc operation comma class
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

// OCOpComma.h: Schnittstelle f�r die Klasse COCOpComma.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCOPCOMMA_H__7F34ECB3_5D4E_4FB6_9543_0B82B9D14175__INCLUDED_)
#define AFX_OCOPCOMMA_H__7F34ECB3_5D4E_4FB6_9543_0B82B9D14175__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ObjectClass.h"

class COCOpComma : public CObjectClass  
{
public:
	COCOpComma();
	virtual ~COCOpComma();

	virtual bool Parse(CParseBase& rParse, CCodeElementList& rCode);

};

#endif // !defined(AFX_OCOPCOMMA_H__7F34ECB3_5D4E_4FB6_9543_0B82B9D14175__INCLUDED_)
