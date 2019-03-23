////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLBEReference.cpp
//
// summary:   Implements the oglbe reference class
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

// Stack.cpp: Implementierung der Klasse CStack.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OGLBEReference.h"
#include "OGLBaseElement.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLBEReference::COGLBEReference()
{
	Invalidate();
}

COGLBEReference::COGLBEReference(const COGLBEReference& Ref)
{
	Invalidate();

	*this = Ref;
}


COGLBEReference::~COGLBEReference()
{
	if (m_pObjRep)
		m_pObjRep->DeRegister(*this);
}


COGLBEReference& COGLBEReference::operator= (const COGLBEReference& Ref)
{
	if (m_pObjRep)
		m_pObjRep->DeRegister(*this);

	m_pObject = Ref.m_pObject;
	m_pObjRep = Ref.m_pObjRep;

	if (m_pObjRep)
		m_pObjRep->Register(*this);

	return *this;
}


void COGLBEReference::Invalidate()
{
	m_pObjRep = 0;
	m_pObject = 0;
}


size_t COGLBEReference::GetRefCount() const
{
	if (!m_pObjRep || !m_pObject)
		return 0;

	return m_pObjRep->GetRefCount( *this );
}


bool COGLBEReference::Set(TObjectPtr pObj, COGLBERepository *pObjRep)
{
	if (m_pObject && m_pObjRep)
		return false;

	m_pObject = pObj;
	m_pObjRep = pObjRep;

	return true;
}
