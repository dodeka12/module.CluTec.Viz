////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLFont.h
//
// summary:   Declares the ogl font class
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

//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLFONT_H__INCLUDED_)
#define AFX_OGLFONT_H__INCLUDED_

#include <map>
#include "OGLImage.h"

//#ifdef _DEBUG
//#	define new _NEW_CRT
//#endif

using namespace std;


class CLUDRAW_API COGLFont
{
public:
	struct SSymbol
	{
		int iX, iY;
		int iWidth, iHeight;
		int iBaseOffset;

		string sID;
	};

	typedef map< string, SSymbol > TSymbolMap;
	typedef unsigned char uchar;

public:
	COGLFont(void);
	~COGLFont(void);

	// Set Path to font data
	void SetPath( const string& sPath )
	{ m_sPath = sPath; }

	// Set Base Name of Font. By default this is CLUFont.
	void SetBaseName( const string& sName )
	{ m_sBaseName = sName; }

	// Load Font
	bool LoadFont( const string& sFont, const string& sFace, const int iMagStep );

	// Copy Font Symbol sID into given image with its baseline at given position.
	bool CopySymbol( COGLImage &rImg, int iX, int iY, const string &sID );

	// Copy Font Symbol rSymbol into given image with its baseline at given position.
	bool CopySymbol( COGLImage &rImg, int iX, int iY, const SSymbol &rSymbol )
	{
		return rImg.InsertSubImage( m_Image, 
								rSymbol.iX, rSymbol.iY,
								rSymbol.iWidth, rSymbol.iHeight,
								iX, iY - rSymbol.iBaseOffset );
	}

	// Get Size of Symbol
	bool GetSymbolExtent( int &iWidth, int &Height, int &iBaseOffset,
							const string &sID );

	// Get Data of Symbol
	bool GetSymbolData( SSymbol &rSymbol, const string &sID );

	// Get reference to symbol map
	TSymbolMap& GetSymbolMap() 
	{ return m_mapSymbol; }
	
	// Is font loaded?
	bool IsValid() { return m_bValid; }

	// Reset Font data
	void ResetFont();

	// Get Font Name
	string GetFont() { return m_sFont; }

	// Get Font Face
	string GetFace() { return m_sFace; }

	// Get Mag Step
	int GetMagStep() { return m_iMagStep; }

protected:
	// Path to Font data
	string m_sPath;

	// Base Name of Fonts
	string m_sBaseName;

	// True if font loaded
	bool m_bValid;

	// Data specifying font
	string m_sFont;
	string m_sFace;
	int m_iMagStep;

	// Image that contains the symbols' images
	COGLImage m_Image;

	// Mapping symbol IDs to Symbol Data
	TSymbolMap m_mapSymbol;
};

#endif
