////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      ImageRepository.cpp
//
// summary:   Implements the image repository class
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
#include "ImageRepository.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CImageRepository::CImageRepository()
{
	m_hMutexLock = CreateMutex( NULL, FALSE, NULL );
}


CImageRepository::~CImageRepository()
{	
	Reset();

	CloseHandle( m_hMutexLock );
}

//////////////////////////////////////////////////////////////////////
// Lock image repository

bool CImageRepository::Lock( int iWait )
{
	if ( WaitForSingleObject( m_hMutexLock, iWait ) == WAIT_TIMEOUT )
	{
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Unlock image repository

void CImageRepository::Unlock()
{
	ReleaseMutex( m_hMutexLock );
}

//////////////////////////////////////////////////////////////////////
// Register Image Reference

bool CImageRepository::Register(CImageReference& rRef)
{
	TImgToRefMap::iterator it_End, it_El;

	Lock();

	if (rRef.m_pImage == 0)
	{
		Unlock();
		return false;
	}

	// Check whether Ref references image that is already in repository
	if ((it_El = m_mapImgToRef.find(rRef.m_pImage)) == m_mapImgToRef.end())
	{
		// if not, then add it to repository
		m_mapImgToRef[rRef.m_pImage].push_back(&rRef);
	}
	else
	{
		// If yes, then add Ref to list
		it_El->second.push_back(&rRef);
	}

	Unlock();

	return true;
}


//////////////////////////////////////////////////////////////////////
// DeRegister Image Reference

bool CImageRepository::DeRegister(CImageReference& rRef)
{
	TImgToRefMap::iterator it_End, it_El;

	Lock();

	if (rRef.m_pImage == 0)
		return false;

	// Check whether Ref references image that is in repository
	if ((it_El = m_mapImgToRef.find(rRef.m_pImage)) == m_mapImgToRef.end())
	{
		// if not, then there is a problem
		Unlock();
		return false;
	}
	else
	{
		// If yes, then find it and remove it
		TImgRefList &rList = it_El->second;

		// Remove reference from list
		rList.remove(&rRef);

		// Check whether there are any references left
		if (rList.empty())
		{
			// if nothing is left then delete image
			delete it_El->first;

			// remove the entry we found from the map
			m_mapImgToRef.erase(it_El);
		}
	}

	Unlock();

	return true;
}


//////////////////////////////////////////////////////////////////////
// Create a new image and return the reference

CImageReference CImageRepository::New()
{
	TImagePtr pImage;
	CImageReference ImgRef;

	Lock();

	//pImgRef = new CImageReference;
	
	if (!(pImage = new TImage))
	{
		Unlock();
		return ImgRef;
	}

	ImgRef.Set(pImage, this);

	Register(ImgRef);

	Unlock();
	return ImgRef;
}

//////////////////////////////////////////////////////////////////////
// Create a new image which is a copy of the one given.
// Return a pointer to the image.
// IMPORTANT: Need to delete returned pointer manually!

CImageReference CImageRepository::New(CImageReference& rRef)
{
	TImagePtr pImage;
	CImageReference ImgRef;
	
	Lock();
	//pImgRef = new CImageReference;

	if (!(pImage = new TImage(*rRef.m_pImage)))
	{
		Unlock();
		return ImgRef;
	}

	ImgRef.Set(pImage, this);

	Register(ImgRef);

	Unlock();
	return ImgRef;	
}


//////////////////////////////////////////////////////////////////////
// Create a new image which is a copy of the one given.
// Return an image reference.

CImageReference CImageRepository::Copy(CImageReference& rRef)
{
	TImagePtr pImage;
	CImageReference ImgRef;
	
	Lock();
	if (!(pImage = new TImage(*rRef.m_pImage)))
	{
		Unlock();
		return ImgRef;
	}

	ImgRef.Set(pImage, this);

	Register(ImgRef);

	Unlock();
	return ImgRef;	
}


//////////////////////////////////////////////////////////////////////
// Delete all images

void CImageRepository::Reset()
{
	TImgToRefMap::iterator it_El, it_End;
	TImgRefList::iterator it_LEl, it_LEnd;

	Lock();

	it_End = m_mapImgToRef.end();

	for( it_El = m_mapImgToRef.begin();
		 it_El != it_End;
		 ++it_El)
	{
		TImgRefList rList = it_El->second;
		it_LEnd = rList.end();

		// Invalidate all references
		for( it_LEl = rList.begin();
			 it_LEl != it_LEnd;
			 ++it_LEl)
		{
			(*it_LEl)->Invalidate();
		}

		delete it_El->first;
	}

	// Delete the whole map
	m_mapImgToRef.erase(m_mapImgToRef.begin(), m_mapImgToRef.end());

	Unlock();
}

