////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLDisplayList.h
//
// summary:   Declares the ogl display list class
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

// OGLDisplayList.h: Schnittstelle f�r die Klasse COGLDisplayList.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLDISPLAYLIST_H__26F79924_3F4D_44A1_A6D3_57EDF183A8EC__INCLUDED_)
#define AFX_OGLDISPLAYLIST_H__26F79924_3F4D_44A1_A6D3_57EDF183A8EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "OGLBaseElement.h"

class CLUDRAW_API COGLDisplayList : public COGLBaseElement
{
public:
	COGLDisplayList();
	COGLDisplayList( const COGLDisplayList& rList );
	virtual ~COGLDisplayList();

	virtual COGLBaseElement* Copy()
	{
		return (COGLBaseElement *) new COGLDisplayList( *this );
	}

	COGLDisplayList& operator= ( const COGLDisplayList& rList );

	bool Create();
	bool Destroy();

	bool BeginDraw(bool bDraw = false);
	bool EndDraw();

	bool Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData &rData);

	bool IsOK() { if (m_uIdx) return true; else return false; }

protected:
	GLuint m_uIdx;

};

#endif // !defined(AFX_OGLDISPLAYLIST_H__26F79924_3F4D_44A1_A6D3_57EDF183A8EC__INCLUDED_)
