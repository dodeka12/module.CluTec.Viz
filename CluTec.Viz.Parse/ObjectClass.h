////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      ObjectClass.h
//
// summary:   Declares the object class class
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

// ObjectClass.h: Schnittstelle f�r die Klasse CObjectClass.
//
//	Base class for all parse classes.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECTCLASS_H__30982795_F6BD_4D30_A056_1D7F60A882BD__INCLUDED_)
	#define AFX_OBJECTCLASS_H__30982795_F6BD_4D30_A056_1D7F60A882BD__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif	// _MSC_VER > 1000

#include "CluTec.Viz.Base\CStrMem.h"

	class CParseBase;
	class CCodeElementList;

	class CObjectClass
	{
	public:

		CObjectClass();
		virtual ~CObjectClass();

		virtual bool Parse(CParseBase& rParse, CCodeElementList& rCode) = 0;

		// Subspace of Symbols that identify Object class.
		// That is a text element is of this object class if its first
		// symbol is contained in m_csIDSymbolSpace.
		// Between all object classes there has to be a unique identification possible.
		CStrMem m_csIDSymbolSpace;
	};

#endif	// !defined(AFX_OBJECTCLASS_H__30982795_F6BD_4D30_A056_1D7F60A882BD__INCLUDED_)
