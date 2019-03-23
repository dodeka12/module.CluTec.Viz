////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLSceneWindow.h
//
// summary:   Declares the ogl scene Windows Form
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

// Class written by Vladimir Banarer, 3.12.2001.


//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLSCENEWINDOW_H__5746B287_573C_4800_9DE1_B801F31D0EB4__INCLUDED_)
#define AFX_OGLSCENEWINDOW_H__5746B287_573C_4800_9DE1_B801F31D0EB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OGLBaseElementList.h"
#include "OGLBEReference.h"
#include "OGLScene.h"

#include <vector>
#include <map>
#include <string>
using namespace std;


class COGLSceneWindow : public COGLScene
{
public:
	COGLSceneWindow();
	COGLSceneWindow( const COGLSceneWindow &rList );
	virtual ~COGLSceneWindow();

	COGLSceneWindow& operator= (const COGLSceneWindow& rList);

	virtual COGLBaseElement* Copy()
	{
		return (COGLBaseElement *) new COGLSceneWindow( *this );
	}


	// Apply list
	virtual bool Apply(COGLBaseElement::EApplyMode eMode, 
					   COGLBaseElement::SApplyData &rData);

protected:
	virtual void DragDataChanged(int iDataIdx) { return; }

protected:
	/// Window Frame
	COGLBEReference m_BorderRef;

};

#endif // !defined(AFX_OGLBASEELEMENTLIST_H__5746B287_573C_4800_9DE1_B801F31D0EB4__INCLUDED_)
