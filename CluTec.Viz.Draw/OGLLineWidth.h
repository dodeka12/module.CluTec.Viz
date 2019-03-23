////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLLineWidth.h
//
// summary:   Declares the ogl line width class
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

// OGLMaterial.h: Schnittstelle f�r die Klasse COGLMaterial.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLLINEWIDTH_H__INCLUDED_)
#define AFX_OGLLINEWIDTH_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OGLBaseElement.h"


class CLUDRAW_API COGLLineWidth  : public COGLBaseElement
{
public:
	COGLLineWidth();
	COGLLineWidth( float fWidth );
	COGLLineWidth(const COGLLineWidth& rTrans);

	virtual COGLBaseElement* Copy()
	{
		return (COGLBaseElement *) new COGLLineWidth( *this );
	}

	COGLLineWidth& operator= (const COGLLineWidth& rTrans);

	void Reset();

	void Set( float fWidth );

	bool Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData &rData);

protected:
	GLfloat m_fWidth;
};

#endif // !defined(AFX_OGLMATERIAL_H__2296DD19_C205_454A_82DB_D1881903B593__INCLUDED_)
