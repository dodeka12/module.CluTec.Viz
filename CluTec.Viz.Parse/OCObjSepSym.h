////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      OCObjSepSym.h
//
// summary:   Declares the oc object separator symbol class
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

// OCObjSepSym.h: Schnittstelle f�r die Klasse COCObjSepSym.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCOBJSEPSYM_H__1F56E4F5_B408_4C46_8C07_3840D75B86EF__INCLUDED_)
#define AFX_OCOBJSEPSYM_H__1F56E4F5_B408_4C46_8C07_3840D75B86EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ObjectClass.h"

class COCObjSepSym : public CObjectClass  
{
public:
	COCObjSepSym();
	virtual ~COCObjSepSym();

	virtual bool Parse(CParseBase& rParse, CCodeElementList& rCode);

};

#endif // !defined(AFX_OCOBJSEPSYM_H__1F56E4F5_B408_4C46_8C07_3840D75B86EF__INCLUDED_)
