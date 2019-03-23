////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      ImageRepository.h
//
// summary:   Declares the image repository class
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

#if !defined(AFX_IMAGEREPOSITORY_H__29DD89BE_9839_4381_A23B_049B46BAD391__INCLUDED_)
#define AFX_IMAGEREPOSITORY_H__29DD89BE_9839_4381_A23B_049B46BAD391__INCLUDED_

#include <list>
#include <map>

#include "OGLImage.h"

using std::list;
using std::map;

class CImageReference;

class CLUDRAW_API CImageRepository
{
public:
	friend class CImageReference;

public:
	typedef COGLImage TImage;
	typedef TImage* TImagePtr;
	typedef list<CImageReference*> TImgRefList;
	typedef map<TImagePtr, TImgRefList> TImgToRefMap;

public:
	CImageRepository();
	virtual ~CImageRepository();

	// Create a new image and return the reference
	CImageReference New();

	// Create a new image which is a copy of the one given
	// Returned pointer has to be deleted manually!
	CImageReference New(CImageReference& rRef);

	// Create a new image which is a copy of the one given
	CImageReference Copy(CImageReference& rRef);

	// Delete all images
	void Reset();

	const TImgToRefMap* GetImgToRefMap() { return &m_mapImgToRef; }

	bool Lock( int iWait = INFINITE );
	void Unlock();

protected:
	bool Register(CImageReference& rRef);
	bool DeRegister(CImageReference& rRef);

protected:
	TImgToRefMap m_mapImgToRef;

	HANDLE m_hMutexLock;
};

#include "ImageReference.h"

#endif // !defined(AFX_IMAGEREPOSITORY_H__29DD89BE_9839_4381_A23B_049B46BAD391__INCLUDED_)
