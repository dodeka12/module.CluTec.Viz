////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLBERepository.cpp
//
// summary:   Implements the oglbe repository class
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
#include "OGLBERepository.h"
#include "OGLBaseElement.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLBERepository::COGLBERepository()
{

}


COGLBERepository::~COGLBERepository()
{	
	Reset();
}


//////////////////////////////////////////////////////////////////////
// Register Image Reference

bool COGLBERepository::Register(COGLBEReference& rRef)
{
	TObjToRefMap::iterator it_End, it_El;

	if (rRef.m_pObject == 0)
		return false;

	// Check whether Ref references image that is already in repository
	if ((it_El = m_mapObjToRef.find(rRef.m_pObject)) == m_mapObjToRef.end())
	{
		// if not, then add it to repository
		m_mapObjToRef[rRef.m_pObject].push_back(&rRef);
	}
	else
	{
		// If yes, then add Ref to list
		it_El->second.push_back(&rRef);
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
// DeRegister Image Reference

bool COGLBERepository::DeRegister(COGLBEReference& rRef)
{
	TObjToRefMap::iterator it_End, it_El;

	if (rRef.m_pObject == 0)
		return false;

	// Check whether Ref references image that is in repository
	if ((it_El = m_mapObjToRef.find(rRef.m_pObject)) == m_mapObjToRef.end())
	{
		// if not, then there is a problem
		return false;
	}
	else
	{
		// If yes, then find it and remove it
		TObjRefList &rList = it_El->second;

		// Remove reference from list
		rList.remove(&rRef);

		// Check whether there are any references left
		if (rList.empty())
		{
			// if nothing is left then delete image
			delete it_El->first;

			// remove the entry we found from the map
			m_mapObjToRef.erase(it_El);
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Get number of times given object is referenced

size_t COGLBERepository::GetRefCount( const COGLBEReference &rRef )
{
	TObjToRefMap::iterator it_End, it_El;

	if (rRef.m_pObject == 0)
		return 0;

	// Check whether Ref references image that is in repository
	if ((it_El = m_mapObjToRef.find(rRef.m_pObject)) == m_mapObjToRef.end())
	{
		// if not, then there is a problem
		return 0;
	}
	else
	{
		// If yes, then find it and get number of times it is referenced
		TObjRefList &rList = it_El->second;

		return rList.size();
	}
}

//////////////////////////////////////////////////////////////////////
// Create a new image and return the reference

COGLBEReference COGLBERepository::New( TObjectPtr pObject )
{
	COGLBEReference ObjRef;

	//pObjRef = new COGLBEReference;
	
	if (!pObject)
		return ObjRef;

	ObjRef.Set(pObject, this);

	Register( ObjRef );

	return ObjRef;
}


//////////////////////////////////////////////////////////////////////
// Create a new image which is a copy of the one given

COGLBEReference COGLBERepository::Copy(const COGLBEReference& rRef)
{
	TObjectPtr pObject;
	COGLBEReference ObjRef;
	
	//pObjRef = new COGLBEReference;

	if ( !(pObject = rRef.m_pObject->Copy()) )
		return ObjRef;

	ObjRef.Set(pObject, this);

	Register( ObjRef );

	return ObjRef;	
}


//////////////////////////////////////////////////////////////////////
// Delete all images

void COGLBERepository::Reset()
{
	TObjToRefMap::iterator it_El, it_End;
	TObjRefList::iterator it_LEl, it_LEnd;

	it_End = m_mapObjToRef.end();

	for( it_El = m_mapObjToRef.begin();
		 it_El != it_End;
		 ++it_El)
	{
		TObjRefList rList = it_El->second;
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
	m_mapObjToRef.erase(m_mapObjToRef.begin(), m_mapObjToRef.end());
}

