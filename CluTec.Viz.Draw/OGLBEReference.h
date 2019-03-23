////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLBEReference.h
//
// summary:   Declares the oglbe reference class
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

// Stack.h: Schnittstelle f�r die Klasse COGLBEReference.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLBEREFERENCE_H__29DD89BE_9839_4381_A23B_049B46BAD391__INCLUDED_)
	#define AFX_OGLBEREFERENCE_H__29DD89BE_9839_4381_A23B_049B46BAD391__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif	// _MSC_VER > 1000

//#include "OGLBaseElement.h"
	class COGLBaseElement;

#include "OGLBERepository.h"

	class CLUDRAW_API COGLBEReference
	{
	public:

		friend class COGLBERepository;

	public:

		typedef COGLBERepository::TObject TObject;
		typedef COGLBERepository::TObjectPtr TObjectPtr;

	public:

		COGLBEReference();
		COGLBEReference(const COGLBEReference& rRef);
		virtual ~COGLBEReference();

		COGLBEReference& operator=(const COGLBEReference& Ref);
		bool operator==(const COGLBEReference& rRef)
		{
			return m_pObject == rRef.m_pObject;
		}

		operator TObjectPtr() const { return m_pObject; }
		TObjectPtr operator->() { return m_pObject; }
		TObjectPtr operator->() const { return m_pObject; }

		COGLBEReference Copy() const
		{
			if (!m_pObjRep) { return COGLBEReference(); }

			return m_pObjRep->Copy(*this);
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Clears this COGLBEReference to its blank/initial state.
		/// </summary>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void Clear()
		{
			if (m_pObjRep)
			{
				m_pObjRep->DeRegister(*this);
				m_pObject = nullptr;
			}
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Query if this COGLBEReference is valid.
		/// </summary>
		///
		/// <returns>	True if valid, false if not. </returns>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		bool IsValid() const
		{
			return m_pObject != nullptr;
		}

		size_t GetRefCount() const;
		COGLBERepository* GetRepositoryPtr() const
		{
			return m_pObjRep;
		}

	protected:

		// Set pointers
		bool Set(TObjectPtr pObject, COGLBERepository* pObjectRep);

		// Set pointers to zero
		void Invalidate();

	protected:

		// Pointer to the actual image
		TObjectPtr m_pObject;

		// Pointer to the Image Repository where this reference is registered
		COGLBERepository* m_pObjRep;
	};

#endif	// !defined(AFX_IMAGEREFERENCE_H__29DD89BE_9839_4381_A23B_049B46BAD391__INCLUDED_)
