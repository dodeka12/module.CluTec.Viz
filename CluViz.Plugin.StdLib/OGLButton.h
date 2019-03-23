////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      OGLButton.h
//
// summary:   Declares the ogl button class
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

#pragma once

class COGLButton :
	public COGLTool
{
public:
	COGLButton(CCLUCodeBase& rCodeBase);
	COGLButton( const COGLButton& oglBut );
	~COGLButton(void);

	COGLButton& operator= (const COGLButton& oglBut );

	void SetColorBGAct( const COGLColor& colValue )
	{ m_colBGAct = colValue; }

	void SetColorBGStd( const COGLColor& colValue )
	{ m_colBGStd = colValue; }

	virtual bool CreateTool(COGLBEReference& refTool);


	// Event: Drag Start
	virtual bool OnDragStart( int iButton, bool bShift, bool bCtrl, bool bAlt, 
								double dX, double dY, double dZ, double dLX, double dLY, double dLZ );

	// Event: Drag End
	virtual bool OnDragEnd( int iButton, bool bShift, bool bCtrl, bool bAlt, 
							double dX, double dY, double dZ, double dLX, double dLY, double dLZ );

	// Event: Drag
	virtual bool OnDrag( int iButton, bool bShift, bool bCtrl, bool bAlt, 
							double dX, double dY, double dZ, double dDX, double dDY, double dDZ, 
							double dLX, double dLY, double dLZ, double dLDX, double dLDY, double dLDZ );

	// Event: Click
	virtual bool OnClick( int iButton, bool bShift, bool bCtrl, bool bAlt, 
							double dX, double dY, double dZ, double dLX, double dLY, double dLZ );

protected:
	CCLUCodeBase *m_pCodeBase;

	COGLColor m_colBGAct, m_colBGStd;
};
