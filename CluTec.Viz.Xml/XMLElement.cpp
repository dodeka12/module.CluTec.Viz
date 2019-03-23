////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Xml
// file:      XMLElement.cpp
//
// summary:   Implements the XML element class
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

#include "XMLElement.h"


/////////////////////////////////////////////////////////////
/// CXMLElement functions

void CXMLElement::Set(const char* pcName, const char* pcContent, 
					  const map<string, string>& mapProp)
{
	m_sName = pcName;
	m_sContent = pcContent;
	m_mapProp = mapProp;
}


string& CXMLElement::GetProp(const char* pcName) throw (PropertyNotAvailable)
{
	map<string, string>::iterator it_Prop;

	if ((it_Prop = m_mapProp.find(pcName)) != m_mapProp.end())
	{
		return it_Prop->second;
	}

	throw PropertyNotAvailable(pcName);
}

const string& CXMLElement::GetProp(const char* pcName) const throw (PropertyNotAvailable)
{
	map<string, string>::const_iterator it_Prop;

	if ((it_Prop = m_mapProp.find(pcName)) != m_mapProp.end())
	{
		return it_Prop->second;
	}

	throw PropertyNotAvailable(pcName);
}



string CXMLElement::Str()
{
	stringstream szText;
	szText << "<" << m_sName;
	map<string,string>::iterator it_Prop, it_PropEnd;

	it_Prop = m_mapProp.begin();
	it_PropEnd = m_mapProp.end();

	for(;it_Prop != it_PropEnd; ++it_Prop)
	{
		szText << "  " << it_Prop->first << "=\"" << it_Prop->second << "\"";
	}
	if(m_mapSubTree.size()==0)
	  szText << "/>\n";
    else
	  szText << ">\n";
 	
	szText << m_mapSubTree.Str();

    if(m_mapSubTree.size()!=0)
	  szText << "</" << m_sName << ">\n";
	return szText.str();
}



