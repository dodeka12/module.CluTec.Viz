////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      OCKeywordElse.h
//
// summary:   Declares the oc keyword else class
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

// OCOpCloseCurlyBracket.h: Schnittstelle f�r die Klasse COCOpCloseCurlyBracket.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCKEYWORD_ELSE_H__DD24910F_A459_41D7_9A78_85DAC43AAAD8__INCLUDED_)
#define AFX_OCKEYWORD_ELSE_H__DD24910F_A459_41D7_9A78_85DAC43AAAD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ObjectClass.h"
#include "OCIDSymDef.h"

class COCKeywordElse : public CObjectClass  
{
public:
	COCKeywordElse();
	virtual ~COCKeywordElse();

	virtual bool Parse(CParseBase& rParse, CCodeElementList& rCode);
};

#endif // !defined(AFX_OCKEYWORD_ELSE_H__DD24910F_A459_41D7_9A78_85DAC43AAAD8__INCLUDED_)
