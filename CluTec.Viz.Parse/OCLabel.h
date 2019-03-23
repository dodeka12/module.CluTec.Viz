////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      OCLabel.h
//
// summary:   Declares the oc label class
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

// OCLabel.h: Schnittstelle f�r die Klasse COCLabel.
//
//	This class defines the parse behaviour of the object class of labels.
//	A label is a continuous string of symbols within a certain
//	subset of symbols. The subset of symbols used is given in
//	m_csSymbolSpace. 
//
//	This class creates an instance of CCodeLabel which implements
//	the functionality of lables in general. 
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCLABEL_H__A469F097_64ED_45D7_BE36_6FEBAE298405__INCLUDED_)
#define AFX_OCLABEL_H__A469F097_64ED_45D7_BE36_6FEBAE298405__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ObjectClass.h"

class COCLabel : public CObjectClass  
{
public:
	COCLabel();
	virtual ~COCLabel();

	void SetSymbolSpace(const char *pcSS) { if (pcSS) m_csSymbolSpace = pcSS; }

	virtual bool Parse(CParseBase& rParse, CCodeElementList& rCode);

	// If Label refers to a function then this function is called
	// to parse the rest.
	bool ParseFunc(CCodeElement *pFunc, CParseBase& rParse, CCodeElementList& rCode);

protected:
	// Symbols that follow the first ID symbol do not need to be unique to this class.
	CStrMem m_csSymbolSpace;

};

#endif // !defined(AFX_OCLABEL_H__A469F097_64ED_45D7_BE36_6FEBAE298405__INCLUDED_)
