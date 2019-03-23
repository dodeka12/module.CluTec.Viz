////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLTool.h
//
// summary:   Declares the ogl tool class
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
#include "OGLScene.h"

#include <string>

#define TOOL_BUTTON_LEFT	0x0001
#define TOOL_BUTTON_MIDDLE	0x0002
#define TOOL_BUTTON_RIGHT	0x0004

class COGLTool :
	public COGLScene
{
public:
	typedef CStrMem TString;

	COGLTool(void);
	COGLTool( const COGLTool &oglTool );
	COGLTool& operator= (const COGLTool& oglTool);

	~COGLTool(void);

	TString GetLastError()
	{ return m_sLastError; }

	virtual void SetToolPos( double dX, double dY )
	{ m_dX = dX; m_dY = dY; m_bChangePos = true; }

	virtual void GetToolPos( double &dX, double &dY ) const
	{ dX = m_dX; dY = m_dY; }

	virtual void SetToolSize( double dW, double dH )
	{ m_dWidth = dW; m_dHeight = dH; m_bChangeSize = true; }

	virtual void GetToolSize( double &dW, double &dH ) const
	{ dW = m_dWidth; dH = m_dHeight; }

	virtual void SetToolText( const TString& sText )
	{ m_sText = sText; m_bChangeText = true; }

	virtual TString GetToolText() const
	{ return m_sText; }

	virtual void SetToolColorText( const COGLColor& colText )
	{ m_colText = colText; }

	virtual const COGLColor& GetToolColorText() const
	{ return m_colText; }

	//virtual const CCodeVar& GetValue() const;
	//virtual bool SetValue( const CCodeVar& rValue );

	// Create the tool visualization
	virtual bool CreateTool( COGLBEReference& refTool )
	{ return false; }

	// Update the tool visualization
	virtual bool UpdateTool()
	{ return false; }

	// Event: Drag Start
	virtual bool OnDragStart( int iButton, bool bShift, bool bCtrl, bool bAlt, 
								double dX, double dY, double dZ, double dLX, double dLY, double dLZ )
	{ return false; }

	// Event: Drag End
	virtual bool OnDragEnd( int iButton, bool bShift, bool bCtrl, bool bAlt, 
							double dX, double dY, double dZ, double dLX, double dLY, double dLZ )
	{ return false; }

	// Event: Drag
	virtual bool OnDrag( int iButton, bool bShift, bool bCtrl, bool bAlt, 
							double dX, double dY, double dZ, double dDX, double dDY, double dDZ, 
							double dLX, double dLY, double dLZ, double dLDX, double dLDY, double dLDZ )
	{ return false; }

	// Event: Click
	virtual bool OnClick( int iButton, bool bShift, bool bCtrl, bool bAlt, 
							double dX, double dY, double dZ, double dLX, double dLY, double dLZ )
	{ return false; }

	// Event: Mouse Over
	virtual bool OnMouseOver( bool bShift, bool bCtrl, bool bAlt )
	{ return false; }

protected:
	bool m_bChangePos, m_bChangeSize, m_bChangeText;

	double m_dX, m_dY;
	double m_dWidth, m_dHeight;
	TString m_sText;
	COGLColor m_colText;

	//CCodeVar m_varValue;

	TString m_sLastError;
};
