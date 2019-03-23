////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      ImageReference.cpp
//
// summary:   Implements the image reference class
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
#include "ImageReference.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CImageReference::CImageReference()
{
	Invalidate();
}

CImageReference::CImageReference(const CImageReference& Ref)
{
	Invalidate();

	*this = Ref;
}


CImageReference::~CImageReference()
{
	if (m_pImgRep)
		m_pImgRep->DeRegister(*this);
}


CImageReference& CImageReference::operator= (const CImageReference& Ref)
{
	if (m_pImgRep)
		m_pImgRep->DeRegister(*this);

	m_pImage = Ref.m_pImage;
	m_pImgRep = Ref.m_pImgRep;

	if (m_pImgRep)
		m_pImgRep->Register(*this);

	return *this;
}


void CImageReference::Invalidate()
{
	m_pImgRep = 0;
	m_pImage = 0;
}

bool CImageReference::Set(TImagePtr pImg, CImageRepository *pImgRep)
{
	if (m_pImage && m_pImgRep)
		return false;

	m_pImage = pImg;
	m_pImgRep = pImgRep;

	return true;
}
