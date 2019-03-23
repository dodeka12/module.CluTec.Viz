////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLFrameStack.h
//
// summary:   Declares the ogl frame stack class
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

// OGLFrame.h: Schnittstelle f�r die Klasse COGLFrameStack.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLFRAMESTACK_H__INCLUDED_)
#define AFX_OGLFRAMESTACK_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OGLBaseElement.h"
//#include "CluTec.Viz.Base\TensorData.h"
//#include "CluTec.Viz.Base\clu.h"

class CLUDRAW_API COGLFrameStack  : public COGLBaseElement
{
public:
	enum EFrameMode
	{
		ALL = 0,
		MODELVIEW,
		PROJECTION,
		TEXTURE
	};

public:
	COGLFrameStack();
	COGLFrameStack( EFrameMode eFrameMode, bool bDoPush );
	COGLFrameStack(const COGLFrameStack& rTrans);

	virtual COGLBaseElement* Copy()
	{
		return (COGLBaseElement *) new COGLFrameStack( *this );
	}

	COGLFrameStack& operator= (const COGLFrameStack& rTrans);

	// Set matrix to unit matrix
	void Reset();

	void DoPush()
	{ m_bDoPush = true; }

	void DoPop()
	{ m_bDoPush = false; }

	void SetFrameMode( EFrameMode eFrameMode )
	{ m_eFrameMode = eFrameMode; }

	bool Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData &rData);

protected:

	// Push or Pop
	bool m_bDoPush;

	// Frame Mode
	EFrameMode m_eFrameMode;

};

#endif // !defined(AFX_OGLMATERIAL_H__2296DD19_C205_454A_82DB_D1881903B593__INCLUDED_)
