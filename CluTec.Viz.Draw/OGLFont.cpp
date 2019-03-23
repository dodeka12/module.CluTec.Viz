////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLFont.cpp
//
// summary:   Implements the ogl font class
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

#undef _CRTDBG_MAP_ALLOC

#include "stdafx.h"
#include "OGLFont.h"
#include "CluTec.Viz.Xml\XML.h"

COGLFont::COGLFont(void)
{
	m_sPath = "";
	m_sBaseName = "CLUFont";

	m_sFont = "";
	m_sFace = "";
	m_iMagStep = 0;

	m_bValid = false;
}


COGLFont::~COGLFont(void)
{
}


////////////////////////////////////////////////////////////////////////////////////
// Reset Font data

void COGLFont::ResetFont()
{
	m_bValid = false;

	m_mapSymbol.clear();
	m_Image.SetSize( 0, 0 );

	m_sFont = "";
	m_sFace = "";
	m_iMagStep = 0;
}


////////////////////////////////////////////////////////////////////////////////////
// Load Font

bool COGLFont::LoadFont( const string& sFont, const string& sFace, const int iMagStep )
{
	if ( m_bValid )	
	{
		ResetFont();
	}

	char pcVal[20];
	string sFilename, sXMLFilename, sImgFilename, sVersion;
	CXMLdom xmlDOM;
	CXMLElement xmlEl;

	sprintf_s( pcVal, 20, "%02d", iMagStep );
	sFilename = m_sPath + m_sBaseName + "_" + sFont + "_" + sFace + "_" + pcVal;

	sXMLFilename = sFilename + ".xml";
	sImgFilename = sFilename + ".png";

	try
	{
		xmlDOM.ParseFile( sXMLFilename.c_str() );
		xmlDOM.GetElementTree( xmlEl );
		CXMLTree &xmlTree = xmlEl.GetSubTree();

		try
		{
			xmlTree.GetElementList("CLUFont");
		}
		catch(...)
		{
			return false;
		}
		
		TXMLElementList &xmlRootList = xmlTree.GetElementList( "CLUFont" );
		CXMLElement &xmlEl = xmlRootList[0];

		sVersion = xmlEl.GetProp("version");
		if (sVersion != "1.0")
		{
			return false;
		}

		TXMLElementList &rFontList = xmlRootList[0].GetSubTree().GetElementList("font");
		TXMLElementList &rSymList = rFontList[0].GetSubTree().GetElementList("sym");

		TXMLElementList::iterator itEl;
		int iX, iY, iWidth, iHeight, iBase, iIsNo, iID;
		uchar pcID[2] = { 0, 0 };
		string sID;

		for (itEl = rSymList.begin(); itEl != rSymList.end(); ++itEl)
		{
			sscanf_s( itEl->GetProp( "x" ).c_str(), "%d", &iX );
			sscanf_s( itEl->GetProp( "y" ).c_str(), "%d", &iY );
			sscanf_s( itEl->GetProp( "w" ).c_str(), "%d", &iWidth );
			sscanf_s( itEl->GetProp( "h" ).c_str(), "%d", &iHeight );
			sscanf_s( itEl->GetProp( "b" ).c_str(), "%d", &iBase );
			sscanf_s( itEl->GetProp( "id_is_num" ).c_str(), "%d", &iIsNo );
			
			if ( iIsNo )
			{
				sscanf_s( itEl->GetProp( "id" ).c_str(), "%d", &iID );
				pcID[0] = uchar( iID );
				sID = (char *) pcID;
			}
			else
			{
				sID = itEl->GetProp( "id" );
			}

			SSymbol &rSym = m_mapSymbol[ sID ];

			rSym.iX = iX;
			rSym.iY = iY;
			rSym.iWidth = iWidth;
			rSym.iHeight = iHeight;
			rSym.iBaseOffset = iBase;
		}
	}
	catch (CCluException &rEx)
	{
		rEx.GetErrorLevel();
		return false;
	}

	m_Image.SetFilename( sImgFilename.c_str() );
	if ( !m_Image.LoadImage( true ) )
	{
		ResetFont();
	}

	m_sFont = sFont;
	m_sFace = sFace;
	m_iMagStep = iMagStep;

	m_bValid = true;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////
// Get Size of Symbol

bool COGLFont::GetSymbolExtent( int &iWidth, int &iHeight, int &iBaseOffset,
								const string &sID )
{
	map< string, SSymbol >::iterator itEl;

	if ( (itEl = m_mapSymbol.find( sID )) == m_mapSymbol.end() )
		return false;

	iWidth = itEl->second.iWidth;
	iHeight = itEl->second.iHeight;
	iBaseOffset = itEl->second.iBaseOffset;

	return true;
}


////////////////////////////////////////////////////////////////////////////////////
// Get Data of Symbol

bool COGLFont::GetSymbolData( SSymbol &rSymbol, const string &sID )
{
	map< string, SSymbol >::iterator itEl;

	if ( (itEl = m_mapSymbol.find( sID )) == m_mapSymbol.end() )
		return false;

	rSymbol = itEl->second;

	return true;
}


////////////////////////////////////////////////////////////////////////////////////
// Copy Font Symbol sID into given image with its baseline at given position.

bool COGLFont::CopySymbol( COGLImage &rImg, int iX, int iY, const string &sID )
{
	map< string, SSymbol >::iterator itEl;

	if ( (itEl = m_mapSymbol.find( sID )) == m_mapSymbol.end() )
		return false;

	return CopySymbol( rImg, iX, iY, itEl->second );
}


