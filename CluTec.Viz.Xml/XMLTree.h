////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Xml
// file:      XMLTree.h
//
// summary:   Declares the XML tree class
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

#if !defined (_CLU_XML_TREE_H_INCLUDED_)
#define _CLU_XML_TREE_H_INCLUDED_

extern "C" {
#include "libxml\xmlmemory.h"
#include "libxml\parser.h"
}

#include <iostream>
#include <sstream>
#include <vector>
#include <stack>
#include <map>

#include "CluTec.Viz.Base\CLUException.h"

using namespace std;


class CXMLElement;

typedef vector<CXMLElement> TXMLElementList;

class CXMLTree : public map<string, TXMLElementList>
{
public:
	class ElementNotAvailable : public CCluError
	{
	public:
		ElementNotAvailable(const char *pcName)
		{
			m_sErrorText = "Element '";
			m_sErrorText += pcName;
			m_sErrorText += "' is not available.";
		}
	};

public:

public:
	CXMLTree(void);
	~CXMLTree(void);

	// \brief Print tree.
	string Str();

	void AddElement(const CXMLElement& rTEl);
	TXMLElementList& GetElementList(const char* pcName) throw (ElementNotAvailable);
};


#endif
