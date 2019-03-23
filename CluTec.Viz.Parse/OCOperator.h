////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      OCOperator.h
//
// summary:   Declares the oc operator class
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

// OCOperator.h: Schnittstelle f�r die Klasse COCOperator.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCOPERATOR_H__A6F486BC_6252_4A90_821A_1D4014F7F303__INCLUDED_)
#define AFX_OCOPERATOR_H__A6F486BC_6252_4A90_821A_1D4014F7F303__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ObjectClass.h"
#include "ParseBase.h"
#include "CodeElement.h"
#include "CodeElementList.h"



class COCOperator : public CObjectClass
{
public:
	COCOperator();
	virtual ~COCOperator();

	void SetVoidOp(bool bVal) { m_bVoidOp = bVal; }
	void SetOrderRightToLeft(bool bRightToLeft) { m_bRightToLeft = bRightToLeft; }

	void SetOpLevel(int iLevelBinary, int iLevelUnary = -1) 
	{ m_iOpLevelBinary = iLevelBinary; m_iOpLevelUnary = iLevelUnary; }

	void SetOpSymbol(const char *pcSym) 
	{ if (pcSym) m_csIDSymbolSpace = pcSym; }

	virtual bool Parse(CParseBase& rParse, CCodeElementList& rCode);

protected:
	// Pointer to Class derived from CCodeElement that
	// implements functionality of Operator.
	// Has to be set by derived class.
	CCodeElement *m_pCodeOp;

	// Level of Binary Operator. 0 binds strongest.
	int m_iOpLevelBinary;

	// Level of Unary Operator. 0 binds strongest.
	// An operator is assumed to be unary, if it is 
	// the first element in a CodeElement list.
	// if this variable is -1 then operator cannot
	// be a unary operator.
	int m_iOpLevelUnary;

	// if this operator is of same strength as current
	// operator level then this operation is performed
	// first if m_bRightToLeft == true. 
	// Otherwise previous operation is performed first.
	bool m_bRightToLeft;

	// if this operator does not return a value
	// then set m_bVoidOp = true
	bool m_bVoidOp;

	// Current Operator level. -1 indicates no previous operator.
	// Current operator level is saved in rParse.
	// static int m_iCurOpLevel;
};

#endif // !defined(AFX_OCOPERATOR_H__A6F486BC_6252_4A90_821A_1D4014F7F303__INCLUDED_)
