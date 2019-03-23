////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      ImageReference.h
//
// summary:   Declares the image reference class
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

// Stack.h: Schnittstelle f�r die Klasse CStack.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGEREFERENCE_H__29DD89BE_9839_4381_A23B_049B46BAD391__INCLUDED_)
#define AFX_IMAGEREFERENCE_H__29DD89BE_9839_4381_A23B_049B46BAD391__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ImageRepository.h"

class CLUDRAW_API CImageReference
{
public:
	friend class CImageRepository;

public:
	typedef CImageRepository::TImage TImage;
	typedef CImageRepository::TImagePtr TImagePtr;

public:
	CImageReference();
	CImageReference(const CImageReference& rRef);
	virtual ~CImageReference();

	CImageReference& operator= (const CImageReference& Ref);

	operator TImagePtr() const { return m_pImage; }
	TImagePtr operator-> () { return m_pImage; }

	bool IsValid() { return (m_pImage ? true : false); }
	CImageRepository* GetImageRepositoryPtr() { return m_pImgRep; }

protected:
	// Set pointers
	bool Set(TImagePtr pImg, CImageRepository *pImgRep);

	// Set pointers to zero
	void Invalidate();

protected:
	// Pointer to the actual image
	TImagePtr m_pImage;

	// Pointer to the Image Repository where this reference is registered
	CImageRepository *m_pImgRep;

};

#endif // !defined(AFX_IMAGEREFERENCE_H__29DD89BE_9839_4381_A23B_049B46BAD391__INCLUDED_)
