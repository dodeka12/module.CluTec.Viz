////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      OCOpCloseRoundBracket.h
//
// summary:   Declares the oc operation close round bracket class
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

// OCOpCloseRoundBracket.h: Schnittstelle f�r die Klasse COCOpCloseRoundBracket.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCOPCLOSEROUNDBRACKET_H__237A6855_4D4B_4672_B7D4_45F4F579CB6D__INCLUDED_)
#define AFX_OCOPCLOSEROUNDBRACKET_H__237A6855_4D4B_4672_B7D4_45F4F579CB6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ObjectClass.h"

class COCOpCloseRoundBracket : public CObjectClass
{
public:
	COCOpCloseRoundBracket();
	virtual ~COCOpCloseRoundBracket();

	virtual bool Parse(CParseBase& rParse, CCodeElementList& rCode);

};

#endif // !defined(AFX_OCOPCLOSEROUNDBRACKET_H__237A6855_4D4B_4672_B7D4_45F4F579CB6D__INCLUDED_)
