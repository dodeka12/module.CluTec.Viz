////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Xml
// file:      XMLElement.h
//
// summary:   Declares the XML element class
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

#if !defined (_CLU_XML_ELEMENT_H_INCLUDED_)
#define _CLU_XML_ELEMENT_H_INCLUDED_

extern "C" {
#include "libxml\xmlmemory.h"
#include "libxml\parser.h"
}

#include <iostream>
#include <sstream>
#include <vector>
#include <stack>
#include <map>

#include "CluTec.Viz.Base/CLUException.h"

#include "XMLTree.h"

// Under Windows GetProp is Macro
#ifdef GetProp
#	undef GetProp
#endif

using namespace std;


class CXMLElement
{
public:
	typedef map<string, string> TPropMap;

public:
	class PropertyNotAvailable : public CCluError
	{
	public:
		PropertyNotAvailable(const char *pcName)
		{
			m_sErrorText = "Property '";
			m_sErrorText += pcName;
			m_sErrorText += "' is not available.";
		}
	};

public:
	CXMLElement(void) { }
	CXMLElement(const CXMLElement& El)
	{ *this = El; }

	CXMLElement& operator= (const CXMLElement& El)
	{
		m_sName = El.m_sName;
		m_sContent = El.m_sContent;
		m_mapProp = El.m_mapProp;
		m_mapSubTree = El.m_mapSubTree;
		return *this;
	}

	void Reset()
	{ 
		m_sName = "";
		m_sContent = "";
		m_mapProp.clear();
		m_mapSubTree.clear();
	}

	void SetName(const char* pcName)
	{ m_sName = pcName; }

	void AddProp(const char* pcProp)
	{ m_mapProp[pcProp] = ""; }

	void AddProp(const char* pcProp, const char* pcVal)
	{ m_mapProp[pcProp] = pcVal; }

	void AddProp(const string &rsProp, const string &rsVal)
	{ m_mapProp[rsProp] = rsVal; }
	
	void AddProp(const char* pcProp, int iVal)
	{ char pcVal[30]; sprintf_s(pcVal, 30, "%d", iVal); AddProp(pcProp, pcVal); }

	void AddProp(const char* pcProp, float fVal)
	{ char pcVal[30]; sprintf_s(pcVal, 30, "%g", fVal); AddProp(pcProp, pcVal); }

	void AddProp(const char* pcProp, double dVal)
	{ char pcVal[30]; sprintf_s(pcVal, 30, "%lg", dVal); AddProp(pcProp, pcVal); }

	void SetProp(const char* pcProp, int iVal)
	{ char pcVal[30]; sprintf_s(pcVal, 30, "%d", iVal); (*this)[pcProp] = pcVal; }

	void SetProp(const char* pcProp, float fVal)
	{ char pcVal[30]; sprintf_s(pcVal, 30, "%g", fVal); (*this)[pcProp] = pcVal; }

	void SetProp(const char* pcProp, double dVal)
	{ char pcVal[30]; sprintf_s(pcVal, 30, "%lg", dVal); (*this)[pcProp] = pcVal; }

	string& operator[] (const char* pcProp)
	{ return m_mapProp[pcProp]; }

	string& GetName() 
	{ return m_sName; }

	const string& GetName() const
	{ return m_sName; }

	string& GetProp(const char* pcName) throw (PropertyNotAvailable);
	const string& GetProp(const char* pcName) const throw (PropertyNotAvailable);

	map<string, string>& GetPropMap()
	{ return m_mapProp; }

	const map<string, string>& GetPropMap() const
	{ return m_mapProp; }

	string& GetContent()
	{ return m_sContent; }

	const string& GetContent() const
	{ return m_sContent; }

	void SetContent(const string& sVal)
	{ m_sContent = sVal; }

	// \brief Set element data.
	void Set(const char* pcName, const char* pcContent, const map<string, string>& mapProp);

	// \brief Get Sub-Tree
	CXMLTree& GetSubTree()
	{ return m_mapSubTree; }

	const CXMLTree& GetSubTree() const
	{ return m_mapSubTree; }

	// \brief Print element.
	string Str();

protected:
	/// \brief Name of Element
	string m_sName;

	/// \brief Names and values of properties.
	map<string, string> m_mapProp;

	/// \brief Content between element tags.
	string m_sContent;

	/// \brief Sub-Elements
	CXMLTree m_mapSubTree;
};


#endif
