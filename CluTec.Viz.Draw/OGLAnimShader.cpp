////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLAnimShader.cpp
//
// summary:   Implements the ogl animation shader class
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

// OGLScale.cpp: Implementierung der Klasse COGLAnimShader.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CluTec.Viz.Fltk\Fl_math.h"
#include "OGLAnimShader.h"
#include "OGLBaseElementList.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLAnimShader::COGLAnimShader()
{
	m_sTypeName = "AnimShader";

	Reset();
}


COGLAnimShader::COGLAnimShader(const COGLAnimShader& rRot)
{
	m_sTypeName = "AnimScale";

	*this = rRot;
}

COGLAnimShader& COGLAnimShader::operator= (const COGLAnimShader& rShader)
{
	COGLBaseElement::operator = (rShader);

	m_refShader = rShader.m_refShader;
	m_sTimeVarName = rShader.m_sTimeVarName;
	m_sTimeStepVarName = rShader.m_sTimeStepVarName;

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Reset

void COGLAnimShader::Reset()
{	
	m_refShader.Clear();
	m_sTimeVarName = "";
	m_sTimeStepVarName = "";
}


//////////////////////////////////////////////////////////////////////
/// Tell parents that content has changed

void COGLAnimShader::TellParentContentChanged()
{
	list<COGLBaseElement*>::iterator itEl;

	for ( itEl = m_listParent.begin();
		  itEl != m_listParent.end();
		  ++itEl )
	{
		COGLBaseElementList *pList = dynamic_cast<COGLBaseElementList*>( (COGLBaseElement*) (*itEl) );
		if ( pList )
			pList->SetContentChanged( true, true, false );
	}
}


//////////////////////////////////////////////////////////////////////
/// Apply

bool COGLAnimShader::Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData &rData)
{
	rData.bNeedAnimate = true;
	TellParentContentChanged();

	if ( m_refShader.IsValid() )
	{
		COGLShader *pShader = dynamic_cast< COGLShader* >( (COGLBaseElement*) m_refShader );
		if ( !pShader )
			return false;

		if ( m_sTimeVarName.length() )
		{
			float fTime;
			fTime = (float) rData.dTime;
			pShader->SetUniformVar<float,1>( m_sTimeVarName, &fTime );
		}

		if ( m_sTimeStepVarName.length() )
		{
			float fTimeStep;
			fTimeStep = (float) rData.dTimeStep;
			pShader->SetUniformVar<float,1>( m_sTimeStepVarName, &fTimeStep );
		}
	}
	else
	{
		rData.bNeedAnimate = false;
	}

	return true;
}

