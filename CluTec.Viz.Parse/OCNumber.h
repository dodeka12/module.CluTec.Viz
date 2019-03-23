////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      OCNumber.h
//
// summary:   Declares the oc number class
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

// OCNumber.h: Schnittstelle f�r die Klasse COCNumber.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCNUMBER_H__506665ED_70FA_4632_BC16_E72539B64CC3__INCLUDED_)
#define AFX_OCNUMBER_H__506665ED_70FA_4632_BC16_E72539B64CC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ObjectClass.h"

class COCNumber : public CObjectClass  
{
public:
	COCNumber();
	virtual ~COCNumber();

	void SetSymbolSpace(const char *pcSS) { if (pcSS) m_csSymbolSpace = pcSS; }

	virtual bool Parse(CParseBase& rParse, CCodeElementList& rCode);

protected:
	// Symbols that follow the first ID symbol do not need to be unique to this class.
	CStrMem m_csSymbolSpace;
	CStrMem m_csSymbolSpaceHex;
	CStrMem m_csErrorSpace;
	CStrMem m_csErrorSpaceHex;

};

#endif // !defined(AFX_OCNUMBER_H__506665ED_70FA_4632_BC16_E72539B64CC3__INCLUDED_)
