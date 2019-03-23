////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Xml
// file:      XMLTree.cpp
//
// summary:   Implements the XML tree class
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

#include "XMLTree.h"
#include "XMLElement.h"



CXMLTree::CXMLTree(void)
{
}

CXMLTree::~CXMLTree(void)
{
}


void CXMLTree::AddElement(const CXMLElement& rTEl)
{
	iterator it_TEl;

	if ((it_TEl = find(rTEl.GetName())) != end())
	{
		TXMLElementList &rvecTEl =  it_TEl->second;
		rvecTEl.push_back(rTEl);
	}
	else
	{
		TXMLElementList vecTEl;

		vecTEl.push_back(rTEl);
		(*this)[rTEl.GetName()] = vecTEl;
	}
}

 
TXMLElementList& CXMLTree::GetElementList(const char* pcName) throw (ElementNotAvailable) 
{
	iterator it_TEl;
	it_TEl = find(pcName);
	if (it_TEl != end())
	{
		return it_TEl->second;
	}

	throw ElementNotAvailable(pcName);
}

string CXMLTree::Str()
{
  stringstream szText;
  iterator it_TEl;
  for(it_TEl=begin();it_TEl!=end();++it_TEl)
  {
	TXMLElementList::iterator it_Element;
    for(it_Element=it_TEl->second.begin();it_Element!=it_TEl->second.end();++it_Element)
		{
				szText << it_Element->Str();
		}
	}
  return szText.str();
}

////////////////////////////////////////////////////////////
