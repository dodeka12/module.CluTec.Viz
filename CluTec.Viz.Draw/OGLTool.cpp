////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLTool.cpp
//
// summary:   Implements the ogl tool class
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

#include "StdAfx.h"
#include "OGLTool.h"
#include "OGLColor.h"

COGLTool::COGLTool(void)
{
	m_sTypeName = "Tool";

	m_bChangePos = false;
	m_bChangeSize = false;
	m_bChangeText = false;

	m_dX = 0.0;
	m_dY = 0.0;
	m_dWidth = 0.0;
	m_dHeight = 0.0;
	m_sText = "";
	m_colText.Set(0.0f, 0.0f, 0.0f);
}

COGLTool::COGLTool( const COGLTool &oglTool )
{
	m_sTypeName = "Tool";

	*this = oglTool;
}

COGLTool::~COGLTool(void)
{
}

//////////////////////////////////////////////////////////////////////
// Copy constructor

COGLTool& COGLTool::operator= (const COGLTool& oglTool)
{
	//m_varValue = oglTool.m_varValue;

	m_bChangePos = true;
	m_bChangeSize = true;
	m_bChangeText = true;

	m_dX = oglTool.m_dX;
	m_dY = oglTool.m_dY;
	m_dWidth = oglTool.m_dWidth;
	m_dHeight = oglTool.m_dHeight;

	m_sText = oglTool.m_sText;
	m_colText = oglTool.m_colText;
	m_sLastError = "";

	return *this; 
}


//////////////////////////////////////////////////////////////////////
/// Get the current tool value

//const CCodeVar& COGLTool::GetValue() const
//{
//	return m_varValue;
//}

//////////////////////////////////////////////////////////////////////
/// Set current tool value

//bool COGLTool::SetValue( const CCodeVar& rValue )
//{
//	m_varValue = rValue;
//}

