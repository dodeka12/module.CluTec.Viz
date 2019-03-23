////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLAnimShader.h
//
// summary:   Declares the ogl animation shader class
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

#if !defined(AFX_OGLANIMSHADER_H__INCLUDED_)
#define AFX_OGLANIMSHADER_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OGLBEReference.h"
#include "OGLShader.h"

class CLUDRAW_API COGLAnimShader : public COGLBaseElement
{
public:
	COGLAnimShader();
	COGLAnimShader(const COGLAnimShader& rRot);

	virtual COGLBaseElement* Copy()
	{
		return (COGLBaseElement *) new COGLAnimShader( *this );
	}

	COGLAnimShader& operator= (const COGLAnimShader& rRot);

	void Reset();
	void SetShader( COGLBEReference &refShader, const char* pcTimeVarName, const char* pcTimeStepVarName )
	{
		m_refShader = refShader;
		
		if ( pcTimeVarName ) m_sTimeVarName = pcTimeVarName;
		else m_sTimeVarName = "";

		if ( pcTimeStepVarName ) m_sTimeStepVarName = pcTimeStepVarName;
		else m_sTimeStepVarName = "";
	}

	bool Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData &rData);

protected:
	void TellParentContentChanged();

protected:
	COGLBEReference m_refShader;
	string m_sTimeVarName;
	string m_sTimeStepVarName;
};

#endif // !defined(AFX_OGLMATERIAL_H__2296DD19_C205_454A_82DB_D1881903B593__INCLUDED_)
