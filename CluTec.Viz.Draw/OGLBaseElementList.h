////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLBaseElementList.h
//
// summary:   Declares the ogl base element list class
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

// Class written by Vladimir Banarer, 3.12.2001.

// OGLBaseElementList.h: Schnittstelle f�r die Klasse COGLBaseElementList.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLBASEELEMENTLIST_H__5746B287_573C_4800_9DE1_B801F31D0EB4__INCLUDED_)
	#define AFX_OGLBASEELEMENTLIST_H__5746B287_573C_4800_9DE1_B801F31D0EB4__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif	// _MSC_VER > 1000

#include "OGLBaseElement.h"
#include "OGLBEReference.h"

#include <vector>
#include <map>
#include <string>
	using namespace std;

	class CLUDRAW_API COGLBaseElementList : public COGLBaseElement
	{
	public:

		COGLBaseElementList();
		COGLBaseElementList(const COGLBaseElementList& rList);
		virtual ~COGLBaseElementList();

		COGLBaseElementList& operator=(const COGLBaseElementList& rList);

		virtual COGLBaseElement* Copy()
		{
			return (COGLBaseElement*) new COGLBaseElementList(*this);
		}

		void Reset();

		const list<COGLBEReference>& GetElementList() const
		{ return m_listElement; }

		virtual COGLBEReference GetElementWithUID(uint uUID, bool bRecurseIntoLists = true);
		list<COGLBEReference> GetElementWithName(const string sName);

		void DeleteElementWithName(const string sName);

		// Delete all those references from the list that have a unit reference count
		void PruneUniqueRef();

		// Add an element to the list.
		bool Add(const COGLBEReference& rElement);

		// Delete an element from the list
		bool Delete(const COGLBEReference& rElement);

		// Enable content changed flag distribution
		void EnableContentChangedInfo(bool bVal = true);

		// Set Flag that content has changed and distribute it to parents.
		void SetContentChanged(bool bVal, bool bTellParents, bool bTellChildren);

		// Return flag whether content is changed
		bool IsContentChanged() { return m_bContentChanged; }

		// Apply list
		virtual bool Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData& rData);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief
		/// 	Applies all elements of the given reference list.
		///
		/// \param	xList		 	The reference list.
		/// \param	eMode		 	The apply mode.
		/// \param [in,out]	rData	The apply data.
		///
		/// \return True if it succeeds, false if at least one element could not be applied.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		static bool ApplyList(const list<COGLBEReference>& xList, COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData& rData);

	protected:

		list<COGLBEReference> m_listElement;
		map<uint, COGLBEReference > m_mapIDtoElement;
		map<string, vector<uint> > m_mapNametoID;

		bool m_bContentChanged;
		bool m_bNeedContentChangedInfo;
	};

#endif	// !defined(AFX_OGLBASEELEMENTLIST_H__5746B287_573C_4800_9DE1_B801F31D0EB4__INCLUDED_)
