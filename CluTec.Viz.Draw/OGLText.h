////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLText.h
//
// summary:   Declares the ogl text class
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

#if !defined(AFX_OGLTEXT_H__INCLUDED_)
#define AFX_OGLTEXT_H__INCLUDED_


#include "OGLFont.h"
#include <map>
#include <stack>

using namespace std;


class CLUDRAW_API COGLText
{
public:
	struct SBox
	{
		int iMinX, iMaxX;
		int iMinY, iMaxY;

		SBox()
		{ iMinX = 0; iMaxX = 0; iMinY = 0; iMaxY = 0; }
//		{ iMinX = 0x7FFFFFFFL; iMaxX = 0xFFFFFFFFL; 
//		  iMinY = 0x7FFFFFFFL; iMaxY = 0xFFFFFFFF; }

		SBox( int _iMinX, int _iMaxX, int _iMinY, int _iMaxY )
		{ Set( _iMinX, _iMaxX, _iMinY, _iMaxY ); }

		void Set( int _iMinX, int _iMaxX, int _iMinY, int _iMaxY )
		{ iMinX = _iMinX; iMaxX = _iMaxX; iMinY = _iMinY; iMaxY = _iMaxY; }

		void AddBox( SBox &rBox )
		{
			if ( rBox.iMinX < iMinX ) iMinX = rBox.iMinX;
			if ( rBox.iMaxX > iMaxX ) iMaxX = rBox.iMaxX;
			if ( rBox.iMinY < iMinY ) iMinY = rBox.iMinY;
			if ( rBox.iMaxY > iMaxY ) iMaxY = rBox.iMaxY;
		}

		void MoveBox( int iX, int iY )
		{
			iMinX += iX;
			iMaxX += iX;
			iMinY += iY;
			iMaxY += iY;
		}

		void GetExtent( int &iExtX, int &iExtY )
		{ iExtX = iMaxX - iMinX + 1; iExtY = iMaxY - iMinY + 1; }
	};


	class CObject
	{
	public:
		CObject()
		{ m_iX = 0; m_iY = 0; }

		CObject( int iX, int iY )
		{ m_iX = iX; m_iY = iY; }

		void SetPosition( int iX, int iY )
		{ m_iX = iX; m_iY = iY; }

		virtual bool Apply( COGLImage &rImage, int iOrigX, int iBaseLineY, string &sError ) = 0;

	protected:
		int m_iX, m_iY;
	};

	class CObjectList : public CObject
	{
	public:
		CObjectList() : CObject()
		{ }

		CObjectList( int iX, int iY ) : CObject( iX, iY )
		{ }

		~CObjectList()
		{ 
			vector< CObject* >::iterator itEl;

			for ( itEl = m_vecObjPtr.begin();
					itEl != m_vecObjPtr.end();
					++itEl )
			{
				if ( *itEl )
					delete *itEl;
			}
		}

		void Clear()
		{
			m_vecObjPtr.clear();
		}

		void Add( CObject *pObjPtr ) 
		{ m_vecObjPtr.push_back( pObjPtr ); }

		virtual bool Apply( COGLImage &rImage, int iOrigX, int iBaseLineY, string &sError );

	protected:
		vector< CObject* > m_vecObjPtr;
	};


	class CSymbol : public CObject
	{
	public:
		CSymbol()
		{ m_pFont = 0; }

		CSymbol( COGLFont* pFont, COGLFont::SSymbol &rSymbol, int iX, int iY )
			: CObject( iX, iY )
		{ m_pFont = pFont; m_Symbol = rSymbol; }

		virtual bool Apply( COGLImage &rImage, int iOrigX, int iBaseLineY, string &sError );

	protected:
		COGLFont::SSymbol m_Symbol;
		COGLFont *m_pFont;
	};


	class CHorizLine : public CObject
	{
	public:
		CHorizLine()
		{ m_iLineWidth = 2; }

		CHorizLine( int iLineLen, int iLineWidth, int iX, int iY )
			: CObject( iX, iY )
		{
			m_iLineLen = iLineLen;
			m_iLineWidth = iLineWidth;
		}

		virtual bool Apply( COGLImage &rImage, int iOrigX, int iBaseLineY, string &sError );

	protected:
		int m_iLineWidth;
		int m_iLineLen;
	};

	struct STextEnv
	{
		// Bounding box of current environment
		SBox Box;

		// Last Box added to Box
		SBox LastBox;

		// Position where drawing of environment starts
		int iOrigX, iOrigY;

		// Current position relative to origin
		int iX, iY;

		// Letter separation
		int iSepLetter;

		// Word separation
		int iSepWord;

		// Change in MagStep for script font
		int iScriptMagStepChange;

		// Active Font
		COGLFont *pFont;

		// Environment Level
		int iEnvLevel;

		// Math Environment
		bool bMathEnv;

		// Verbatim Environment
		bool bVerbEnv;

		// Math Font Face
		string sMathFace;
	};

	typedef bool (COGLText::*TParseFuncPtr)
			( char *pcToken, STextEnv &rTextEnv, CObjectList &rObjList, string &sError, 
			  int &iTextPos, char** pcTextPos, int iLevel, int iParseCnt );

	typedef map< string, COGLFont > TFontMap;
	typedef map< string, TParseFuncPtr > TCmdMap;

public:
	COGLText(void);
	~COGLText(void);

	// Set the standard font 
	bool SetFont( const string &sFont, const string &sFace, const int iMagStep );

	// Get the standard font settings
	bool GetFont( string &sFont, string &sFace, int &iMagStep );

	// Set Font Path
	void SetFontPath( const string &sPath )
	{ m_sFontPath = sPath; }

	// Set Font Base Name
	void SetFontBasename( const string &sBaseName )
	{ m_sFontBasename = sBaseName; }

	// Generate image of given text
	bool GetTextImage( COGLImage &rImage, string &sError, const string &sText, double dAlign );

	// Initialize Command map
	void InitCmdMap();

	// Set Text Border
	void SetBorder( int iLeft, int iRight, int iBottom, int iTop )
	{ 
		m_iBorderLeft = iLeft; m_iBorderRight = iRight;
		m_iBorderBottom = iBottom; m_iBorderTop = iTop;
	}

protected:
	// Get pointer to font
	COGLFont* GetFontPtr( const string &sFont, const string &sFace, const int iMagStep );

	// Get font ID string
	string GetFontID( const string &sFont, const string &sFace, const int iMagStep );

	// Generate Text Image
	bool GenImage( COGLImage &rImage, string &sError, STextEnv &rTextEnv, CObjectList &rObjList );

	// Get text token
	int GetToken( bool &bIsCmd, char *pcToken, char **pcTextPos, int iMaxTokenLen, bool bVerbatim = false );

	// Parse Text and generate CObjectList
	bool Parse( STextEnv &rTextEnv, CObjectList &rObjList, string &sError, 
				int &iTextPos, char** pcTextPos, int iLevel, int iParseCnt );

	// Parse 'bar'
	bool Parse_Bar( char *pcToken, STextEnv &rTextEnv, CObjectList &rObjList, string &sError, 
				    int &iTextPos, char** pcTextPos, int iLevel, int iParseCnt );

	// Parse Greek font
	bool Parse_Greek( char *pcToken, STextEnv &rTextEnv, CObjectList &rObjList, string &sError, 
				      int &iTextPos, char** pcTextPos, int iLevel, int iParseCnt );

	// Parse Symbol font
	bool Parse_Symbol( char *pcToken, STextEnv &rTextEnv, CObjectList &rObjList, string &sError, 
				       int &iTextPos, char** pcTextPos, int iLevel, int iParseCnt );

	// Parse font face
	bool Parse_FontFace( char *pcToken, STextEnv &rTextEnv, CObjectList &rObjList, string &sError, 
				         int &iTextPos, char** pcTextPos, int iLevel, int iParseCnt );

	// Parse spacing
	bool Parse_Spacing( char *pcToken, STextEnv &rTextEnv, CObjectList &rObjList, string &sError, 
				        int &iTextPos, char** pcTextPos, int iLevel, int iParseCnt );

	// Parse size
	bool Parse_Size( char *pcToken, STextEnv &rTextEnv, CObjectList &rObjList, string &sError, 
				     int &iTextPos, char** pcTextPos, int iLevel, int iParseCnt );

	// Parse verbatim
	bool Parse_Verbatim( char *pcToken, STextEnv &rTextEnv, CObjectList &rObjList, string &sError, 
				     int &iTextPos, char** pcTextPos, int iLevel, int iParseCnt );

	// Generate Error Text
	bool GenErrorText( string &sError, const char *pcError, 
						int iTextPos, const char *pcText );

	// Set Font Separation
	bool SetFontSeparation( STextEnv &rTextEnv );

protected:
	string m_sFontPath;
	string m_sFontBasename;

	// Border space around text in pixel
	int m_iBorderLeft, m_iBorderRight;
	int m_iBorderBottom, m_iBorderTop;

	// Font stack
//	stack< COGLFont* > m_stackFont;

	// Standard Text Environment
	STextEnv m_TextEnv;

	// Loaded Fonts
	TFontMap m_mapFont;

	// Command Map
	TCmdMap m_mapCmd;
};

#endif
