////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      CodeElementList.cpp
//
// summary:   Implements the code element list class
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

#include "StdAfx.h"
#include "CodeElementList.h"
#include "CodeBase.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCodeElementList::CCodeElementList()
{

}

CCodeElementList::~CCodeElementList()
{
	// Delete all owned elements.
	Delete(0, (int)m_mElementList.Count());
}


//////////////////////////////////////////////////////////////////////
// Serialize

bool CCodeElementList::Serialize( CXMLTree &xmlTree )
{
	CXMLElement xmlEl, xmlListEl;
	int iIdx, iCnt = Count();

	xmlEl.SetName( "code" );
	xmlEl.AddProp( "cls", "CodeElementList" );
	xmlEl.AddProp( "name", m_csName.Str() );
	xmlEl.AddProp( "count", iCnt );

	CXMLTree &xmlElTree = xmlEl.GetSubTree();

	for ( iIdx = 0; iIdx < iCnt; ++iIdx )
	{
		SCodeElementPtr &rEl = m_mElementList[ iIdx ];

		xmlListEl.Reset();
		xmlListEl.SetName( "el" );
		xmlListEl.AddProp( "owner", (rEl.bOwner ? 1 : 0) );
		xmlListEl.AddProp( "line", rEl.iTextLine );
		xmlListEl.AddProp( "pos", rEl.iTextPos );

		if (rEl.pElement == 0)
			return false;
		
		rEl.pElement->Serialize( xmlListEl.GetSubTree() );

		xmlElTree.AddElement( xmlListEl );
	}

	xmlTree.AddElement( xmlEl );

	return true;	
}


//////////////////////////////////////////////////////////////////////
// Apply

bool CCodeElementList::Apply(CCodeBase* pCodeBase, SCodeData *pData)
{
	if (!pCodeBase) 
		return false;

	int i, n = int(m_mElementList.Count());

	if (pCodeBase->LockStack() < 0)
		return false;

	SCodeData sData;

	for(i=0;i<n;i++)
	{
		SCodeElementPtr &rEl = m_mElementList[i];

		sData.Set(rEl.iTextLine, rEl.iTextPos, this);

		if (!rEl.pElement->Apply(pCodeBase, &sData))
		{
			pCodeBase->UnlockStack();
			return false;
		}
	}

	pCodeBase->UnlockStack();

	return true;
}

//////////////////////////////////////////////////////////////////////
// Insert

bool CCodeElementList::Insert(CCodeElement& rElement, int iPos, bool bOwner,
							  int iTextLine, int iTextPos)
{
	if (iPos < 0)
		iPos = int(m_mElementList.Count());
	else if (iPos > (int) m_mElementList.Count())
		return false;

	if (!m_mElementList.Insert(iPos))
		return false;

	SCodeElementPtr &rEl = m_mElementList[iPos];

	rEl.pElement = &rElement;
	rEl.bOwner = bOwner;
	rEl.iTextLine = iTextLine;
	rEl.iTextPos = iTextPos;


	return true;
}


//////////////////////////////////////////////////////////////////////
// Delete

bool CCodeElementList::Delete(CCodeElement& rElement, int DeleteLists)
{
	int n = int(m_mElementList.Count()), i;
	CCodeElementList *pDummy;

	for(i = 0; i < n; i++)
	{
		if(m_mElementList[i].pElement == &rElement)
		{
			if (m_mElementList[i].bOwner)
				delete m_mElementList[i].pElement;

			m_mElementList.Del(i,1);
		}
		else
		{
			if(DeleteLists != 0)
			{
				pDummy = dynamic_cast <CCodeElementList*>(m_mElementList[i].pElement);
				if(pDummy)
				{
					pDummy->Delete(rElement,DeleteLists-1);
				}
			}
		}
	}


	return true;

}

//////////////////////////////////////////////////////////////////////
/// Delete

bool CCodeElementList::Delete(CStrMem& rName, int DeleteLists)
{
	int n = int(m_mElementList.Count()), i;
	CCodeElementList *pDummy;


	for(i = 0; i < n; i++)
	{
		if(m_mElementList[i].pElement->GetName() == rName)
		{
			if (m_mElementList[i].bOwner)
				delete m_mElementList[i].pElement;

			m_mElementList.Del(i,1);
		}
		else
		{
			if(DeleteLists != 0)
			{
				pDummy = dynamic_cast<CCodeElementList*>(m_mElementList[i].pElement);
				if(pDummy)
				{
					pDummy->Delete(rName,DeleteLists-1);
				}
			}
		}
	}

	return true;
}



//////////////////////////////////////////////////////////////////////
// Delete iNo elements starting from iPos

bool CCodeElementList::Delete(int iPos, int iNo)
{
	int i, iLen = int(m_mElementList.Count());
	int iMax = iPos + iNo;

	if (iPos < 0 || iMax > iLen)
		return false;

	for(i=iPos;i<iMax;i++)
	{
		if (m_mElementList[i].bOwner)
			delete m_mElementList[i].pElement;
	}

	m_mElementList.Del(iPos, iNo);

	return true;
}


