////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLBERepository.h
//
// summary:   Declares the oglbe repository class
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

#if !defined(AFX_OGLBEREPOSITORY_H__29DD89BE_9839_4381_A23B_049B46BAD391__INCLUDED_)
#define AFX_OGLBEREPOSITORY_H__29DD89BE_9839_4381_A23B_049B46BAD391__INCLUDED_

#include <list>
#include <map>

//#ifdef _DEBUG
//#	define new _NEW_CRT
//#endif

//#include "OGLBaseElement.h"

using std::list;
using std::map;

class COGLBaseElement;
class COGLBEReference;

class CLUDRAW_API COGLBERepository
{
public:
	friend class COGLBEReference;

public:
	typedef COGLBaseElement TObject;
	typedef TObject* TObjectPtr;
	typedef list<COGLBEReference*> TObjRefList;
	typedef map<TObjectPtr, TObjRefList> TObjToRefMap;

public:
	COGLBERepository();
	virtual ~COGLBERepository();

	// Create a new image and return the reference
	COGLBEReference New(TObjectPtr pObject);

	// Create a new image which is a copy of the one given
	COGLBEReference Copy(const COGLBEReference& rRef);

	// Delete all images
	void Reset();

	size_t GetRefCount( const COGLBEReference &rRef );

	const TObjToRefMap* GetObjToRefMap() { return &m_mapObjToRef; }

protected:
	bool Register(COGLBEReference& rRef);
	bool DeRegister(COGLBEReference& rRef);

protected:
	TObjToRefMap m_mapObjToRef;
};

//CLUDRAW_EXT template class CLUDRAW_API std::list<COGLBEReference*>;
//CLUDRAW_EXT template class CLUDRAW_API std::map<COGLBaseElement*, std::list<COGLBEReference*>>;

#include "OGLBEReference.h"

#endif // !defined(AFX_IMAGEREPOSITORY_H__29DD89BE_9839_4381_A23B_049B46BAD391__INCLUDED_)
