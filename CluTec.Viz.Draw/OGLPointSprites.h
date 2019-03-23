////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLPointSprites.h
//
// summary:   Declares the ogl point sprites class
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

//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "OGLBaseElement.h"

class CLUDRAW_API COGLPointSprites  : public COGLBaseElement
{
public:

	COGLPointSprites();
	COGLPointSprites(const COGLPointSprites& rPntPar);

	virtual COGLBaseElement* Copy()
	{
		return (COGLBaseElement*) new COGLPointSprites(*this);
	}

	COGLPointSprites& operator=(const COGLPointSprites& rPntPar);

	void Reset();
	void Enable(bool bValue);

	bool Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData& rData);

protected:

	bool m_bDoEnable;
};
