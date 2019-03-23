////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLBaseElementList.cpp
//
// summary:   Implements the ogl base element list class
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

// OGLBaseElementList.cpp: Implementierung der Klasse COGLBaseElementList.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "OGLBaseElementList.h"

#include "CluTec.Base/Exception.h"
#include "CluTec.Viz.ImgRepo/CvCoreImgRepo.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLBaseElementList::COGLBaseElementList()
{
	m_sTypeName = "BaseElementList";

	m_bContentChanged         = false;
	m_bNeedContentChangedInfo = false;
}

COGLBaseElementList::COGLBaseElementList(const COGLBaseElementList& rList)
{
	m_sTypeName = "BaseElementList";

	m_bContentChanged         = false;
	m_bNeedContentChangedInfo = false;

	*this = rList;
}

COGLBaseElementList& COGLBaseElementList::operator=(const COGLBaseElementList& rList)
{
	COGLBaseElement::operator=(rList);

	m_listElement    = rList.m_listElement;
	m_mapIDtoElement = rList.m_mapIDtoElement;
	m_mapNametoID    = rList.m_mapNametoID;

	m_bContentChanged         = false;
	m_bNeedContentChangedInfo = rList.m_bNeedContentChangedInfo;

	// Set also this list as parent of elements
	list<COGLBEReference>::iterator itEl;

	for (itEl = m_listElement.begin();
	     itEl != m_listElement.end();
	     ++itEl)
	{
		(*itEl)->AddParent((COGLBaseElement*) this);
	}

	return *this;
}

COGLBaseElementList::~COGLBaseElementList()
{
	Reset();
}

//////////////////////////////////////////////////////////////////////
/// Reset all lists

void COGLBaseElementList::Reset()
{
	list<COGLBEReference>::iterator itEl;

	for (itEl = m_listElement.begin();
	     itEl != m_listElement.end();
	     ++itEl)
	{
		if (itEl->IsValid())
		{
			(*itEl)->RemoveParent((COGLBaseElement*) this);
		}
	}

	m_listElement.clear();
	m_mapIDtoElement.clear();
	m_mapNametoID.clear();

	SetContentChanged(true, true, false);
}

//////////////////////////////////////////////////////////////////////
/// Set Content Changed

void COGLBaseElementList::SetContentChanged(bool bVal, bool bTellParents, bool bTellChildren)
{
	// Only distribute content changed info if this list actually needs it.
	if (!m_bNeedContentChangedInfo)
	{
		return;
	}

	if (bVal != m_bContentChanged)
	{
		m_bContentChanged = bVal;

		if (bTellParents)
		{
			// Loop over all parents and set changed flag
			list<COGLBaseElement*>::iterator itEl;

			for (itEl = m_listParent.begin();
			     itEl != m_listParent.end();
			     ++itEl)
			{
				COGLBaseElementList* pList = dynamic_cast< COGLBaseElementList* >((COGLBaseElement*) (*itEl));
				if (pList)
				{
					pList->SetContentChanged(true, true, false);
				}
			}
		}

		if (bTellChildren)
		{
			// Loop over all children and reset changed flag
			list<COGLBEReference>::iterator itEl;

			for (itEl = m_listElement.begin();
			     itEl != m_listElement.end();
			     ++itEl)
			{
				if ((*itEl).IsValid())
				{
					COGLBaseElementList* pList = dynamic_cast< COGLBaseElementList* >((COGLBaseElement*) (*itEl));
					if (pList)
					{
						pList->SetContentChanged(false, false, true);
					}
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool COGLBaseElementList::Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData& rData)
{
	try
	{
		// Save all matrices
		rData.xMatrixStack.PushAll();

		if (eMode == COGLBaseElement::PICK)
		{
			rData.PushPickName(GetUID());
		}

		// Apply list. Returns false if one element in list couldn't not be applied
		bool bApplied = COGLBaseElementList::ApplyList(m_listElement, eMode, rData);

		if (eMode == COGLBaseElement::PICK)
		{
			rData.PopPickName();
			rData.PopPickName();
		}

		// Restore all matrices
		rData.xMatrixStack.PopAll();
		return bApplied;
	}
	catch (Clu::CIException& ex)
	{
		throw CLU_EXCEPTION_NEST("Error applying base element list", std::move(ex));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool COGLBaseElementList::ApplyList(const list<COGLBEReference>& xList, COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData& rData)
{
	try
	{
		// For each reference in element list
		for (list<COGLBEReference>::const_iterator itEl = xList.cbegin(); itEl != xList.cend(); ++itEl)
		{
			// Get reference from iterator
			const COGLBEReference& xOGLBEReference = *itEl;

			// Is reference valid?
			if (xOGLBEReference.IsValid())
			{
				// Get object from reference
				const COGLBEReference::TObjectPtr xRefObject = (COGLBEReference::TObjectPtr) *itEl;

				// Apply object
				if (xRefObject->Apply(eMode, rData))
				{
					// Continue to next object if apply was successfully
					continue;
				}
			}

			// Element isn't valid or apply wasn't successfully
			return false;
		}

		return true;
	}
	catch (Clu::CIException& ex)
	{
		throw CLU_EXCEPTION_NEST("Error applying list of base element list", std::move(ex));
	}
}

//////////////////////////////////////////////////////////////////////
/// Tell all children that they have to notify changes

void COGLBaseElementList::EnableContentChangedInfo(bool bVal)
{
	list<COGLBEReference>::iterator itEl;

	m_bNeedContentChangedInfo = bVal;

	for (itEl = m_listElement.begin();
	     itEl != m_listElement.end();
	     ++itEl)
	{
		if (itEl->IsValid())
		{
			COGLBaseElementList* pList = dynamic_cast<COGLBaseElementList*>((COGLBaseElement*) (*itEl));
			if (pList)
			{
				pList->EnableContentChangedInfo(m_bNeedContentChangedInfo);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////
// Add

bool COGLBaseElementList::Add(const COGLBEReference& rElement)
{
	if (!rElement.IsValid())
	{
		return false;
	}

	m_listElement.push_back(rElement);
	m_mapIDtoElement[rElement->GetUID()] = rElement;

	if (rElement->GetName().size())
	{
		m_mapNametoID[rElement->GetName()].push_back(rElement->GetUID());
	}

	// Add this object as parent to element using the same repository as the element uses
	rElement->AddParent((COGLBaseElement*) this);

	// If this list need content changed info, then all sublists need it too.
	if (m_bNeedContentChangedInfo)
	{
		COGLBaseElementList* pList = dynamic_cast<COGLBaseElementList*>((COGLBaseElement*) rElement);
		if (pList)
		{
			pList->EnableContentChangedInfo(m_bNeedContentChangedInfo);
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Delete an element from the list

bool COGLBaseElementList::Delete(const COGLBEReference& rElement)
{
	if (!rElement.IsValid())
	{
		return false;
	}

	uint uUID    = rElement->GetUID();
	string sName = rElement->GetName();
	list<COGLBEReference>::iterator itEl;

	for (itEl = m_listElement.begin();
	     itEl != m_listElement.end();
	     ++itEl)
	{
		if (*itEl == rElement)
		{
			break;
		}
	}

	if (itEl == m_listElement.end())
	{
		return false;
	}

	(*itEl)->RemoveParent((COGLBaseElement*) this);

	m_listElement.erase(itEl);

	map<uint, COGLBEReference >::iterator itMapID;
	map<string, vector<uint> >::iterator itMapName;

	if ((itMapID = m_mapIDtoElement.find(uUID)) != m_mapIDtoElement.end())
	{
		m_mapIDtoElement.erase(itMapID);
	}

	if ((itMapName = m_mapNametoID.find(sName)) != m_mapNametoID.end())
	{
		vector<uint>::iterator itUID;
		vector<uint>& vecUID = itMapName->second;

		for (itUID = vecUID.begin();
		     itUID != vecUID.end();
		     ++itUID)
		{
			if (*itUID == uUID)
			{
				vecUID.erase(itUID);
				break;
			}
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Return reference to element with give UID

COGLBEReference COGLBaseElementList::GetElementWithUID(uint uUID, bool bRecurseIntoLists)
{
	map<uint, COGLBEReference >::iterator itEl;

	if ((itEl = m_mapIDtoElement.find(uUID)) == m_mapIDtoElement.end())
	{
		if (!bRecurseIntoLists)
		{
			return COGLBEReference();
		}

		list<COGLBEReference>::iterator itEl;
		COGLBEReference ElRef;
		COGLBaseElementList* pList;

		for (itEl = m_listElement.begin();
		     itEl != m_listElement.end();
		     ++itEl)
		{
			if (!itEl->IsValid())
			{
				continue;
			}

			pList = dynamic_cast< COGLBaseElementList* >((COGLBaseElement*) *itEl);
			if (pList)
			{
				ElRef = pList->GetElementWithUID(uUID);
				if (ElRef.IsValid())
				{
					return ElRef;
				}
			}
		}

		return COGLBEReference();
	}

	return itEl->second;
}

//////////////////////////////////////////////////////////////////////
/// Get list of elements with particular name

list<COGLBEReference> COGLBaseElementList::GetElementWithName(const string sName)
{
	list<COGLBEReference> listRef;
	map<uint, COGLBEReference >::iterator itEl;

	map<string, vector<uint> >::iterator itName;

	if ((itName = m_mapNametoID.find(sName)) == m_mapNametoID.end())
	{
		return listRef;
	}

	vector<uint>& vecUID = itName->second;

	vector<uint>::iterator itUID;

	for (itUID = vecUID.begin();
	     itUID != vecUID.end();
	     ++itUID)
	{
		if ((itEl = m_mapIDtoElement.find(*itUID)) != m_mapIDtoElement.end())
		{
			listRef.push_back(itEl->second);
		}
	}

	return listRef;
}

//////////////////////////////////////////////////////////////////////
/// Delete all elements with particular name

void COGLBaseElementList::DeleteElementWithName(const string sName)
{
	list<COGLBEReference> listRef;
	map<uint, COGLBEReference >::iterator itEl;

	map<string, vector<uint> >::iterator itName;

	if ((itName = m_mapNametoID.find(sName)) == m_mapNametoID.end())
	{
		return;
	}

	vector<uint> vecUID = itName->second;
	vector<uint>::iterator itUID;

	for (itUID = vecUID.begin();
	     itUID != vecUID.end();
	     ++itUID)
	{
		if ((itEl = m_mapIDtoElement.find(*itUID)) != m_mapIDtoElement.end())
		{
			Delete(itEl->second);
		}
	}
}

//////////////////////////////////////////////////////////////////////
// Delete all those references from the list that have a unit reference count

void COGLBaseElementList::PruneUniqueRef()
{
	list<COGLBEReference>::iterator itEl, itElCur;

	itEl = m_listElement.begin();
	while (itEl != m_listElement.end())
	{
		if (itEl->GetRefCount() == 2)
		{
			itElCur = itEl;
			++itEl;
			Delete(*itElCur);
		}
		else{ ++itEl; }
	}
}
