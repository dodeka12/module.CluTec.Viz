////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLLighting.h
//
// summary:   Declares the ogl lighting class
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

// OGLLighting.h: Enable/Disable of OpenGL Lighting
//
//////////////////////////////////////////////////////////////////////

#pragma once


#include "OGLBaseElement.h"


class CLUDRAW_API COGLLighting : public COGLBaseElement 
{
public:
	COGLLighting();
	COGLLighting(const COGLLighting& rLight);

	virtual COGLBaseElement* Copy()
	{
		return (COGLBaseElement *) new COGLLighting( *this );
	}

	COGLLighting& operator= (const COGLLighting& rLight);

	void Reset();

	void Enable(bool bAct = true)
	{ m_bIsEnabled = bAct; }

	bool Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData &rData);
	bool Apply();

protected:
	bool m_bIsEnabled;
};

