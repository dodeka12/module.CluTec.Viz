////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLText.cpp
//
// summary:   Implements the ogl text class
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

#include <vector>
#include <ctype.h>

#include "OGLText.h"




COGLText::COGLText(void)
{
	m_sFontBasename = "CLUFont";
	m_sFontPath = "./";

	m_TextEnv.iSepLetter = 1;
	m_TextEnv.iSepWord = 8;
	m_TextEnv.iScriptMagStepChange = 3;

	m_TextEnv.pFont = 0;
	m_TextEnv.iOrigX = 0;
	m_TextEnv.iOrigY = 0;

	m_TextEnv.bMathEnv = false;
	m_TextEnv.bVerbEnv = false;
	m_TextEnv.sMathFace = "sl";

	m_iBorderLeft = 1;
	m_iBorderRight = 1;
	m_iBorderBottom = 1;
	m_iBorderTop = 1;
}


COGLText::~COGLText(void)
{
}



////////////////////////////////////////////////////////////////////////////////////
// Initialize Command Map

void COGLText::InitCmdMap()
{
	// Standard Commands
	m_mapCmd["bar"] = &COGLText::Parse_Bar;
	m_mapCmd["@"] = &COGLText::Parse_Verbatim;

	// Font Face Commands
	m_mapCmd["rm"] = &COGLText::Parse_FontFace;
	m_mapCmd["it"] = &COGLText::Parse_FontFace;
	m_mapCmd["bf"] = &COGLText::Parse_FontFace;
	m_mapCmd["sf"] = &COGLText::Parse_FontFace;
	m_mapCmd["sl"] = &COGLText::Parse_FontFace;

	// Spacing commands
	m_mapCmd["."] = &COGLText::Parse_Spacing;
	m_mapCmd[","] = &COGLText::Parse_Spacing;
	m_mapCmd[";"] = &COGLText::Parse_Spacing;
	m_mapCmd["!"] = &COGLText::Parse_Spacing;

	// Size commands
	m_mapCmd["big"] = &COGLText::Parse_Size;
	m_mapCmd["bigg"] = &COGLText::Parse_Size;
	m_mapCmd["Big"] = &COGLText::Parse_Size;
	m_mapCmd["Bigg"] = &COGLText::Parse_Size;
	m_mapCmd["size"] = &COGLText::Parse_Size;

	// Register Greek Font Commands
	COGLFont *pFontGreek = GetFontPtr( "greek", "std", 15 );
	if ( pFontGreek )
	{
		COGLFont::TSymbolMap &rSymMap = pFontGreek->GetSymbolMap();
		COGLFont::TSymbolMap::iterator itEl;

		for ( itEl = rSymMap.begin();
			  itEl != rSymMap.end();
			  ++itEl )
		{
			m_mapCmd[ itEl->first ] = &COGLText::Parse_Greek;
		}
	}

	// Register Symbol Font Commands
	COGLFont *pFontSymbol = GetFontPtr( "symbols", "std", 15 );
	if ( pFontSymbol )
	{
		COGLFont::TSymbolMap &rSymMap = pFontSymbol->GetSymbolMap();
		COGLFont::TSymbolMap::iterator itEl;

		for ( itEl = rSymMap.begin();
			  itEl != rSymMap.end();
			  ++itEl )
		{
			m_mapCmd[ itEl->first ] = &COGLText::Parse_Symbol;
		}
	}

}

////////////////////////////////////////////////////////////////////////////////////
// Get ID to Font

string COGLText::GetFontID( const string &sFont, const string &sFace, const int iMagStep )
{
	string sFontID;
	char pcVal[10];

	// Construct ID String of Font
	sprintf_s( pcVal, 10, "%04d", iMagStep );
	sFontID = sFont + sFace + pcVal;

	return sFontID;
}


////////////////////////////////////////////////////////////////////////////////////
// Get Pointer to font

COGLFont* COGLText::GetFontPtr( const string &sFont, const string &sFace, const int iMagStep )
{
	TFontMap::iterator itEl;

	string sFontID = GetFontID( sFont, sFace, iMagStep );

	// Look whether font is already loaded
	if ( (itEl = m_mapFont.find( sFontID )) == m_mapFont.end() )
	{
		// Font not available yet. So try to load it.
		COGLFont &rFont = m_mapFont[ sFontID ];

		rFont.SetPath( m_sFontPath );
		rFont.SetBaseName( m_sFontBasename );

		if ( !rFont.LoadFont( sFont, sFace, iMagStep ) )
		{
			// Font could not be loaded, so erase entry in map.
			m_mapFont.erase( sFontID );
			return 0;
		}

		return &rFont;
	}

	return &itEl->second;
}


////////////////////////////////////////////////////////////////////////////////////
// Set the standard font

bool COGLText::SetFont( const string &sFont, const string &sFace, const int iMagStep )
{
	if ( !(m_TextEnv.pFont = GetFontPtr( sFont, sFace, iMagStep )) )
		return false;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////
// Get the standard font settings

bool COGLText::GetFont( string &sFont, string &sFace, int &iMagStep )
{
	if ( !m_TextEnv.pFont )
		return false;

	sFont = m_TextEnv.pFont->GetFont();
	sFace = m_TextEnv.pFont->GetFace();
	iMagStep = m_TextEnv.pFont->GetMagStep();

	return true;
}

////////////////////////////////////////////////////////////////////////////////////
// Get text token

int COGLText::GetToken( bool &bIsCmd, char *pcToken, char **pcTextPos, 
						int iMaxTokenLen, bool bVerbatim )
{
	int iLen = 0;
	bIsCmd = false;
	int iTextStep = 0;
	
	if ( **pcTextPos == 0 )
	{
		*pcToken = 0;
		return 0;
	}

	if ( **pcTextPos != '\\' || 
		(**pcTextPos == '\\' && (*pcTextPos)[1] != '@' && bVerbatim) )
	{
		*pcToken = **pcTextPos;
		pcToken[1] = 0;

		++(*pcTextPos);
		return 1;
	}

	// Token is a command
	bIsCmd = true;

	// Step to symbol behind backslash
	++(*pcTextPos);
	++iTextStep;

	// read token consisting of alpha symbols
	while( isalnum( (unsigned char) **pcTextPos ) && iLen < iMaxTokenLen )
	{
		pcToken[ iLen ] = **pcTextPos;

		++(*pcTextPos);
		++iTextStep;
		++iLen;
	}

	// Check whether first symbol behind backslash was not alpha
	if ( iLen == 0 )
	{
		// if this is the case then only return this one symbol
		*pcToken = **pcTextPos;
		pcToken[1] = 0;

		++(*pcTextPos);
		++iTextStep;
		return iTextStep;
	}

	// Check whether token is of maximal length and is still not finished
	if ( iLen == iMaxTokenLen && isalnum( **pcTextPos ) )
		return -1; // if this is the case return with an error.

	pcToken[ iLen ] = 0;

	// if next symbol behind command is ' ', then jump over this symbol
	if ( **pcTextPos == ' ' )
	{
		++(*pcTextPos);
		++iTextStep;
	}

	return iTextStep;
}


////////////////////////////////////////////////////////////////////////////////////
// Generate image of given text

bool COGLText::GetTextImage( COGLImage &rImage, string &sError, const string &sText, double dAlign )
{
	int iTextPos;
	CObjectList ObjList;
	STextEnv TextEnv, SaveTextEnv;
	char *pcText;
	std::list<std::string> lPart; 
	size_t nLineIdx = 0;
	COGLColor colClear(0.0f, 0.0f, 0.0f, 0.0f);

	if ( dAlign < 0.0 ) dAlign = 0.0;
	else if ( dAlign > 1.0 ) dAlign = 1.0;

	SaveTextEnv = m_TextEnv;

	// Split string into lines separated by \n
	while (true)
	{
		size_t nIdx = sText.find("\n", nLineIdx);

		if ( nIdx == std::string::npos )
		{
			lPart.push_back(sText.substr(nLineIdx));
			break;
		}

		if ( nIdx > nLineIdx )
		{
			lPart.push_back(sText.substr(nLineIdx, nIdx - nLineIdx));
		}
		else
		{
			lPart.push_back("");
		}
		
		nLineIdx = nIdx + 1;
	}

	if ( lPart.size() == 0 )
	{
		lPart.push_back(sText);
	}

	size_t nLineCnt = lPart.size();

	std::list<std::string>::iterator itEl;
	std::vector<COGLImage> vecImg(nLineCnt);
	std::vector<int> vecLineSpacing(nLineCnt);
	int iPrevLineHeight = 1;

	for ( nLineIdx = 0, itEl = lPart.begin(); nLineIdx < nLineCnt; ++nLineIdx, ++itEl )
	{
		COGLImage &rImg = vecImg[nLineIdx];

		if ( itEl->size() == 0 )
		{
			rImg.ResizeCanvas(1, iPrevLineHeight);
			rImg.ClearImage(&colClear);
		}
		else
		{
			ObjList.Clear();
			rImg.ResizeCanvas(1,1);
			rImg.ClearImage(&colClear);

			m_TextEnv.iOrigX = 0;
			m_TextEnv.iOrigY = 0;
			m_TextEnv.iEnvLevel = 0;
			m_TextEnv.bMathEnv = false;
			m_TextEnv.bVerbEnv = false;
			m_TextEnv.sMathFace = "sl";

			TextEnv = m_TextEnv;

			// Reset position in text
			iTextPos = 0;
			pcText = (char *) itEl->c_str();

			if ( !Parse( TextEnv, ObjList, sError, iTextPos, &pcText, 0, -1 ) )
			{
				m_TextEnv = SaveTextEnv;
				return false;
			}

			if ( TextEnv.pFont != 0 )
				m_TextEnv.pFont = TextEnv.pFont;

			if ( TextEnv.bMathEnv )
			{
				GenErrorText( sError, "Mathematical environment not ended with '$'", 
					iTextPos, pcText );
				m_TextEnv = SaveTextEnv;
				return false;
			}

			if ( !GenImage( rImg, sError, TextEnv, ObjList ) )
			{
				m_TextEnv = SaveTextEnv;
				return false;
			}

			int iW;
			rImg.GetSize(iW, iPrevLineHeight);
		}

		int iSpacing = int(0.2 * double(iPrevLineHeight));
		if (iSpacing == 0) 
			iSpacing = 1;

		vecLineSpacing[nLineIdx] = iSpacing;
	}

	if ( nLineCnt > 1 )
	{
		int iW, iH;
		int iTotalHeight = 0;
		int iMaxWidth = 0;

		// Find total height and maximum width of final image
		for ( size_t nLineIdx = 0; nLineIdx < nLineCnt; ++nLineIdx )
		{
			COGLImage &rImg = vecImg[nLineIdx];

			rImg.GetSize(iW, iH);
			iTotalHeight += iH;

			if ( nLineIdx + 1 < nLineCnt )
				iTotalHeight += vecLineSpacing[nLineIdx];

			if ( iW > iMaxWidth )
				iMaxWidth = iW;
		}

		// Set size of final image
		rImage.ResizeCanvas( iMaxWidth, iTotalHeight );
		rImage.ClearImage( &colClear );

		double dTotalWidth = double(iMaxWidth);
		double dBottom = double(iTotalHeight);
		double dLeft;

		for ( size_t nLineIdx = 0; nLineIdx < nLineCnt; ++nLineIdx )
		{
			COGLImage &rImg = vecImg[nLineIdx];
			rImg.GetSize(iW, iH);

			dLeft = ceil( dAlign * (dTotalWidth - double(iW)) );

			dBottom -= double(iH);
			rImage.InsertImage(rImg, int(dLeft), int(dBottom));

			dBottom -= double(vecLineSpacing[nLineIdx]); 
		}
	}
	else
	{
		rImage = vecImg[0];
	}

	m_TextEnv = SaveTextEnv;

	return true;
}


////////////////////////////////////////////////////////////////////////////////////
// Generate Error Text

bool COGLText::GenErrorText( string &sError, const char *pcError, 
							 int iTextPos, const char *pcText )
{
	char *pcVal;
	char pcPart[50], pcPart2[50];
	char *pcPre;
	char *pcPost;
	int iPreText = 10;

	if ( iTextPos < 10 )
		iPreText = iTextPos;

	memset( pcPart, 0, 50 );
	memset( pcPart2, 0, 50 );

	strncpy_s( pcPart, 50, pcText - iPreText, iPreText );
	if ( iTextPos > 10 )
		pcPre = "...";
	else
		pcPre = "";

	strcat_s( pcPart, 50, "@" );

	strncpy_s( pcPart2, 50, pcText, 10 );
	if ( strlen( pcText ) > 10 )
		pcPost = "...";
	else
		pcPost = "";

	pcVal = new char[ strlen( pcError ) + strlen( pcPart ) + strlen( pcPart2 ) + 200 ];
	sprintf_s( pcVal, strlen( pcError ) + strlen( pcPart ) + strlen( pcPart2 ) + 200, "Text error at pos. %d: %s. (\"%s%s%s%s\").", iTextPos, 
				pcError, pcPre, pcPart, pcPart2, pcPost );

	sError = pcVal;
	delete[] pcVal;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////
// Set Font Separation

bool COGLText::SetFontSeparation( STextEnv &rTextEnv )
{
	int iWidthX, iHeightX, iBaseX;
	string sFace;

	// Check whether a font is loaded and active
	if ( !rTextEnv.pFont )
	{
		return false;
	}

	// Get Font Face
	sFace = rTextEnv.pFont->GetFace();

	// Get Size of current 'X'
	rTextEnv.pFont->GetSymbolExtent( iWidthX, iHeightX, iBaseX, "X" );
	
	// Slanted fonts have zero letter separation
	if ( sFace == "it" || sFace == "sl" )
	{
		// Set letter separation
		rTextEnv.iSepLetter = 0; //-iWidthX / 60;

		// Set word separation
		rTextEnv.iSepWord = (2 * iWidthX) / 3; // + rTextEnv.iSepLetter;
	}
	else
	{
		// Set letter separation
		if ( (rTextEnv.iSepLetter = iWidthX / 15) < 1 )
			rTextEnv.iSepLetter = 1;

		// Set word separation
		rTextEnv.iSepWord = (2 * iWidthX) / 3;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////
// Parse Text and generate CObjectList

bool COGLText::Parse( STextEnv &rTextEnv, CObjectList &rObjList, string &sError, int &iTextPos, 
					  char** pcTextPos, int iLevel, int iParseCnt )
{
	int iParseIdx;					// Number of parse step
	int iTextStep;					// Step within text
	int iBot;						// bottom position of symbol
//	int iTokenLen;					// Length of Token
	char pcToken[256];				// Extracted Token
	char *pcTextTokenPos;			// Position in text of token
	bool bIsCmd;					// Is Token a command?
	COGLFont::SSymbol FontSymbol;	// Font Symbol Data
	SBox ObjBox;					// Symbol Extent
	TCmdMap::iterator itCmd;		// Command iterator

	// Reset Error Text
	sError = "";

	// Reset parse index
	iParseIdx = 0;

	// Reset Bounding Box
	rTextEnv.Box.Set( 0, -1, 0, -1 );
	rTextEnv.LastBox.Set( 0, -1, 0, -1 );
	rTextEnv.iX = 0;
	rTextEnv.iY = 0;

	int &iX = rTextEnv.iX;
	int &iY = rTextEnv.iY;

	// Check whether a font is loaded and active
	if ( !rTextEnv.pFont )
	{
		sError = "No font loaded.";
		return false;
	}

	// Set letter and word separation
	SetFontSeparation( rTextEnv );

	pcTextTokenPos = *pcTextPos;

	while( (iParseCnt < 0 || iParseIdx < iParseCnt) &&
		(iTextStep = GetToken( bIsCmd, pcToken, pcTextPos, 255, rTextEnv.bVerbEnv )) > 0 )
	{
		if ( !bIsCmd || *pcToken == ' ' )
		{
			// Token is a standard symbol
			bool bSymProc = false;

			// Check whether symbol is word separation symbol
			if ( *pcToken == ' ' )
			{
				int iWidth, iHeight, iBase;

				rTextEnv.pFont->GetSymbolExtent( iWidth, iHeight, iBase, "X" );

				ObjBox.Set( iX, iX + rTextEnv.iSepWord - 1,
							iY, 
							iY + iHeight - 1 );

				rTextEnv.Box.AddBox( ObjBox );
				rTextEnv.LastBox = ObjBox;

				iX = rTextEnv.Box.iMaxX + rTextEnv.iSepLetter + 1;

				//rTextEnv.LastBox.Set( 0, -1, 0, -1 );
				//iX = rTextEnv.Box.iMaxX + rTextEnv.iSepWord;

				bSymProc = true;
			}
			// Start subenvironment
			else if ( !rTextEnv.bVerbEnv && *pcToken == '{' )
			{
				STextEnv SubEnv = rTextEnv;
				int iSubTextPos = iTextPos + 1;
				int iHeight, iWidth;
	
				SubEnv.iOrigX = iX + rTextEnv.iOrigX;
				SubEnv.iOrigY = iY + rTextEnv.iOrigY;

				++SubEnv.iEnvLevel;

				if ( !Parse(SubEnv, rObjList, sError, iSubTextPos, 
							pcTextPos, iLevel+1, -1) )
				{
					return false;
				}

				if ( *(*pcTextPos - 1) != '}' )
				{
					GenErrorText( sError, "Text ended before end of environment", 
									iTextPos, pcTextTokenPos );
					return false;
				}

				SubEnv.Box.MoveBox( iX, iY );
				rTextEnv.Box.AddBox( SubEnv.Box );
				rTextEnv.LastBox = SubEnv.Box;

				// Empty environment has height of 'X' as 'LastBox'
				rTextEnv.LastBox.GetExtent( iWidth, iHeight );
				if ( iWidth == 0 && iHeight == 0 )
				{
					int iBase;
					rTextEnv.pFont->GetSymbolExtent( iWidth, iHeight, iBase, "X" );
					rTextEnv.LastBox.Set( iX, iX, iY - iBase, iY - iBase + iHeight );
				}

				iX = rTextEnv.Box.iMaxX + rTextEnv.iSepLetter + 1;

				iTextPos = iSubTextPos;
				iTextStep = 0;
				bSymProc = true;
			}
			// End environment
			else if ( !rTextEnv.bVerbEnv && *pcToken == '}' )
			{
				// Environment ended without corresponding open environment?
				if ( !rTextEnv.iEnvLevel )
				{
					GenErrorText( sError, "Extra end of environment symbol '}'", 
									iTextPos, pcTextTokenPos );
					return false;
				}

				// Reduce environment level
				--rTextEnv.iEnvLevel;

				iTextPos += iTextStep;
				bSymProc = true;

				// Otherwise return
				return true;
			}
			// Superscript
			else if ( !rTextEnv.bVerbEnv && (*pcToken == '^' || *pcToken == '_'))
			{
				STextEnv SubEnv = rTextEnv;
				int iSubTextPos = iTextPos + 1;
				int iWidth, iHeight, iNewX, iNewY, iOffY, iBase;
				int iBoxW, iBoxH;
				bool bSuperScript;
										
				rTextEnv.pFont->GetSymbolExtent( iWidth, iHeight, iBase, "X" );

				if ( *pcToken == '^' )
				{
					bSuperScript = true;
					//iOffY = (iHeight * 2) / 3;
				}
				else
				{
					bSuperScript = false;
					//iOffY = -(iHeight * 2) / 5;
				}
	
				if ( !iHeight )
				{
					if ( bSuperScript )
						GenErrorText( sError, "Invalid superscript", 
										iTextPos, *pcTextPos );
					else
						GenErrorText( sError, "Invalid subscript", 
										iTextPos, *pcTextPos );

					return false;
				}

				iNewX = rTextEnv.LastBox.iMaxX + rTextEnv.iSepLetter + 1;
				iNewY = iY;
				SubEnv.iOrigX = iNewX + rTextEnv.iOrigX;
				SubEnv.iOrigY = iNewY + rTextEnv.iOrigY;
				SubEnv.pFont = GetFontPtr( rTextEnv.pFont->GetFont(), 
											rTextEnv.pFont->GetFace(),
											rTextEnv.pFont->GetMagStep() 
											- rTextEnv.iScriptMagStepChange );
				
				if ( !SubEnv.pFont )
				{
					// If script font is not available use current font.
					SubEnv.pFont = rTextEnv.pFont;
/*
					if ( bSuperScript )
						GenErrorText( sError, "Superscript font not available.",
										iTextPos, *pcTextPos );
					else
						GenErrorText( sError, "Subscript font not available.",
										iTextPos, *pcTextPos );
					return false;
*/
				}

				CObjectList *pSubObjList = new CObjectList;

				if ( !Parse(SubEnv, *pSubObjList, sError, iSubTextPos, 
							pcTextPos, iLevel+1, 1) )
				{
					return false;
				}

				SubEnv.Box.GetExtent( iBoxW, iBoxH );

				if ( bSuperScript )
					iOffY = (3 * iHeight) / 4;// - iBoxH / 3;
				else
					iOffY = -iBoxH + iHeight / 3;

				iNewY += iOffY;

				pSubObjList->SetPosition( 0, iOffY );
				rObjList.Add( pSubObjList );

				SubEnv.Box.MoveBox( iNewX, iNewY );
				rTextEnv.Box.AddBox( SubEnv.Box );
				//rTextEnv.LastBox = SubEnv.Box;

				// Avoid double superscripts
				//rTextEnv.LastBox.Set( 0, -1, 0, -1 );

				iX = rTextEnv.Box.iMaxX + rTextEnv.iSepLetter + 1;

				iTextPos = iSubTextPos;
				iTextStep = 0;
				bSymProc = true;
			}
			// Math environment
			else if ( !rTextEnv.bVerbEnv && *pcToken == '$' )
			{
				// Check whether we are already in math. env.
				if ( rTextEnv.bMathEnv )
				{
					rTextEnv.bMathEnv = false;

					// End Math Environment
					iTextPos += iTextStep;

					// Otherwise return
					return true;
				}

				STextEnv SubEnv = rTextEnv;
				int iSubTextPos = iTextPos + 1;

				SubEnv.iOrigX = iX;
				SubEnv.iOrigY = iY;
				SubEnv.bMathEnv = true;
				SubEnv.pFont = GetFontPtr( rTextEnv.pFont->GetFont(), 
											rTextEnv.sMathFace,
											rTextEnv.pFont->GetMagStep() );
				
				if ( !SubEnv.pFont )
				{
					// If math font is not available use current font.
					SubEnv.pFont = rTextEnv.pFont;
				}

				if ( !Parse(SubEnv, rObjList, sError, iSubTextPos, 
							pcTextPos, iLevel+1, -1) )
				{
					return false;
				}

				SubEnv.Box.MoveBox( iX, iY );
				rTextEnv.Box.AddBox( SubEnv.Box );
				rTextEnv.LastBox = SubEnv.Box;

				iX = rTextEnv.Box.iMaxX + rTextEnv.iSepLetter + 1;

				iTextPos = iSubTextPos;
				iTextStep = 0;
				bSymProc = true;
			}
			// Check whether symbol exists in current font
			else if ( rTextEnv.pFont->GetSymbolData( FontSymbol, pcToken ) )
			{
				rObjList.Add( new CSymbol( rTextEnv.pFont, FontSymbol, 
											iX + rTextEnv.iOrigX, 
											iY + rTextEnv.iOrigY ) );

				iBot = iY - FontSymbol.iBaseOffset;

				ObjBox.Set( iX, iX + FontSymbol.iWidth - 1,
							iBot, 
							iBot + FontSymbol.iHeight - 1 );

				rTextEnv.Box.AddBox( ObjBox );
				rTextEnv.LastBox = ObjBox;

				iX = rTextEnv.Box.iMaxX + rTextEnv.iSepLetter + 1;
				bSymProc = true;
			}
			
			// Symbol could not be interpreted
			if ( !bSymProc )
			{
				iTextPos += iTextStep;

				// Check whether symbol exists in symbol font
				if ( !Parse_Symbol( pcToken, rTextEnv, rObjList, sError, 
									iTextPos, pcTextPos, iLevel, iParseCnt ) )
				{
					GenErrorText( sError, "Unknown symbol", 
								iTextPos, pcTextTokenPos );
					return false;
				}
			}
			else
			{
				iTextPos += iTextStep;
			}
		}
		// Token is command
		else
		{
			// Check whether command is known
			if ( (itCmd = m_mapCmd.find( pcToken )) != m_mapCmd.end() )
			{
				iTextPos += iTextStep;

				TParseFuncPtr pFunc = itCmd->second;

				if ( !(this->*pFunc)( pcToken, rTextEnv, rObjList, sError, 
									  iTextPos, pcTextPos, iLevel, iParseCnt ) )
				{
					return false;
				}
			}
			// unknown command
			else
			{
				GenErrorText( sError, "Unknown command", 
								iTextPos, pcTextTokenPos );

				return false;
			}
		}

		pcTextTokenPos = *pcTextPos;
		++iParseIdx;
	}
	
	if ( iTextStep < 0 )
	{
		sError = "Token exceeds maximum length.";
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////
// Parse '\bar'

bool COGLText::Parse_Bar( char *pcToken, STextEnv &rTextEnv, CObjectList &rObjList, string &sError, 
						  int &iTextPos, char** pcTextPos, int iLevel, int iParseCnt )
{
	SBox ObjBox;

	// Copy Environment
	STextEnv SubEnv = rTextEnv;

	// Set origin of sub-environment to current position
	SubEnv.iOrigX = rTextEnv.iX + rTextEnv.iOrigX;
	SubEnv.iOrigY = rTextEnv.iY + rTextEnv.iOrigY;

	// Parse next token with sub environment
	if ( !Parse( SubEnv, rObjList, sError, iTextPos, pcTextPos, iLevel+1, 1 ) )
	{
		return false;
	}

	// Construct bounding box
	ObjBox = SubEnv.Box;

	// Move box to absolute position
	ObjBox.MoveBox( rTextEnv.iX, rTextEnv.iY );

	int iLineWidth, iLineOff, iWidth, iHeight, iBase;
	
	rTextEnv.pFont->GetSymbolExtent( iWidth, iHeight, iBase, "X" );
	if ( !(iLineWidth = iHeight / 10) )
		iLineWidth = 1;

	iLineOff = 2 * iLineWidth;

	// Store position of line
	int iLinePosY = ObjBox.iMaxY + iLineOff + rTextEnv.iOrigY;
	int iLinePosX = ObjBox.iMinX + rTextEnv.iOrigX;
	int iLineLen = ObjBox.iMaxX - ObjBox.iMinX + 1;

	// Add space for line to box
	ObjBox.iMaxY += iLineOff + iLineWidth;

	// Add Horizontal Line Object 
	rObjList.Add( new CHorizLine( iLineLen, iLineWidth, iLinePosX, iLinePosY ) );


	// Add box to this environment
	rTextEnv.Box.AddBox( ObjBox );
	rTextEnv.LastBox = ObjBox;

	// Step to next letter
	rTextEnv.iX = rTextEnv.Box.iMaxX + rTextEnv.iSepLetter;

	return true;
}


////////////////////////////////////////////////////////////////////////////////////
// Parse Greek font

bool COGLText::Parse_Greek( char *pcToken, STextEnv &rTextEnv, CObjectList &rObjList, string &sError, 
						    int &iTextPos, char** pcTextPos, int iLevel, int iParseCnt )
{
	COGLFont *pGreekFont = GetFontPtr( "greek", "std", rTextEnv.pFont->GetMagStep() );
	if ( !pGreekFont )
	{
		sError = "Greek font not available in current size.";
		return false;
	}

	int iBot;
	SBox ObjBox;
	COGLFont::SSymbol FontSymbol;	// Font Symbol Data

	int &iX = rTextEnv.iX;
	int &iY = rTextEnv.iY;

	if ( pGreekFont->GetSymbolData( FontSymbol, pcToken ) )
	{
		rObjList.Add( new CSymbol( pGreekFont, FontSymbol, 
									iX + rTextEnv.iOrigX, 
									iY + rTextEnv.iOrigY) );

		iBot = iY - FontSymbol.iBaseOffset;

		ObjBox.Set( iX, iX + FontSymbol.iWidth - 1,
			iBot, 
			iBot + FontSymbol.iHeight - 1 );

		rTextEnv.Box.AddBox( ObjBox );
		rTextEnv.LastBox = ObjBox;

		iX = rTextEnv.Box.iMaxX + rTextEnv.iSepLetter + 1;
	}
	else
	{
		char pcError[100];

		sprintf_s( pcError, 100, "Greek symbol '%s' not available.", pcToken );
		sError = pcError;
		return false;
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////////
// Parse Symbol font

bool COGLText::Parse_Symbol( char *pcToken, STextEnv &rTextEnv, CObjectList &rObjList, string &sError, 
						    int &iTextPos, char** pcTextPos, int iLevel, int iParseCnt )
{
	COGLFont *pSymbolFont = GetFontPtr( "symbols", "std", rTextEnv.pFont->GetMagStep() );
	if ( !pSymbolFont )
	{
		sError = "Symbol font not available in current size.";
		return false;
	}

	int iBot;
	SBox ObjBox;
	COGLFont::SSymbol FontSymbol;	// Font Symbol Data

	int &iX = rTextEnv.iX;
	int &iY = rTextEnv.iY;

	if ( pSymbolFont->GetSymbolData( FontSymbol, pcToken ) )
	{
		rObjList.Add( new CSymbol( pSymbolFont, FontSymbol, 
									iX + rTextEnv.iOrigX, 
									iY + rTextEnv.iOrigY) );

		iBot = iY - FontSymbol.iBaseOffset;

		ObjBox.Set( iX, iX + FontSymbol.iWidth - 1,
			iBot, 
			iBot + FontSymbol.iHeight - 1 );

		rTextEnv.Box.AddBox( ObjBox );
		rTextEnv.LastBox = ObjBox;

		iX = rTextEnv.Box.iMaxX + rTextEnv.iSepLetter + 1;
	}
	else
	{
		char pcError[100];

		sprintf_s( pcError, 100, "Symbol '%s' not available.", pcToken );
		sError = pcError;
		return false;
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////////
// Parse font face

bool COGLText::Parse_FontFace( char *pcToken, STextEnv &rTextEnv, CObjectList &rObjList, string &sError, 
						       int &iTextPos, char** pcTextPos, int iLevel, int iParseCnt )
{
	
	COGLFont *pNewFont = GetFontPtr( rTextEnv.pFont->GetFont(), pcToken, rTextEnv.pFont->GetMagStep() );
	if ( !pNewFont )
	{
		GenErrorText( sError, "Font not available", iTextPos, *pcTextPos );
		return false;
	}

	// Make new font active
	rTextEnv.pFont = pNewFont;

	// Set letter and word separation
	SetFontSeparation( rTextEnv );

	return true;
}


////////////////////////////////////////////////////////////////////////////////////
// Parse spacing

bool COGLText::Parse_Spacing( char *pcToken, STextEnv &rTextEnv, CObjectList &rObjList, string &sError, 
						      int &iTextPos, char** pcTextPos, int iLevel, int iParseCnt )
{
	int iSpace;

	if ( *pcToken == '.' )
	{
		iSpace = 1;
	}
	else if ( *pcToken == ',' )
	{
		iSpace = 2;
	}
	else if ( *pcToken == ';' )
	{
		iSpace = 4;
	}
	else if ( *pcToken == '!' )
	{
		iSpace = -2;
	}

	rTextEnv.iX += iSpace;

	if ( iSpace > 0 )
	{
		rTextEnv.Box.iMaxX += iSpace;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////
// Parse size

bool COGLText::Parse_Size( char *pcToken, STextEnv &rTextEnv, CObjectList &rObjList, string &sError, 
						   int &iTextPos, char** pcTextPos, int iLevel, int iParseCnt )
{
	int iSize = 0, iFontSize, iSign = 1;
	string sToken;
	bool bRelative = true;
	bool bPersistent = false;

	sToken = pcToken;

	if ( sToken == "big" )
	{
		iSize = 1;
	}
	else if ( sToken == "bigg" )
	{
		iSize = 2;
	}
	else if ( sToken == "Big" )
	{
		iSize = 3;
	}
	else if ( sToken == "Bigg" )
	{
		iSize = 4;
	}
	else if ( sToken ==  "size" )
	{
		if (**pcTextPos == '+' )
		{
			iSign = 1;
			++(*pcTextPos);
		}
		else if ( **pcTextPos == '-')
		{
			++(*pcTextPos);
			iSign = -1;
		}
		else
		{
			bRelative = false;
		}

		bool bIsCmd = false;
		char pcSize[20];

		if ( GetToken( bIsCmd, pcSize, pcTextPos, 20 ) == 0 )
		{
			sError = "'size' without value";
			return false;
		}

		if ( bIsCmd )
		{
			sError = "'size' invalid value: ";
			sError += pcSize;
			return false;
		}

		if ( sscanf_s(pcSize, "%d", &iSize) != 1 )
		{
			sError = "'size' invalid value";
			return false;
		}

		// jump over trailing space
		if ( **pcTextPos == ' ' )
			++(*pcTextPos);

		if ( bRelative )
			iSize *= iSign;

		bPersistent = true;
	}

	COGLFont *pCurFont = rTextEnv.pFont;
	iFontSize = pCurFont->GetMagStep();
	
	if ( bRelative )
		iFontSize += iSize;
	else
		iFontSize = iSize;

	if ( pCurFont && iSize )
	{
		COGLFont *pFont = GetFontPtr( pCurFont->GetFont(), 
									  pCurFont->GetFace(),
									  iFontSize
									   );
		if ( pFont )
		{
			if ( bPersistent )
			{
				// Make new font active
				rTextEnv.pFont = pFont;

				// Set letter and word separation
				SetFontSeparation( rTextEnv );
			}
			else
			{
				SBox ObjBox;

				// Copy Environment
				STextEnv SubEnv = rTextEnv;

				// Set origin of sub-environment to current position
				SubEnv.iOrigX = rTextEnv.iX + rTextEnv.iOrigX;
				SubEnv.iOrigY = rTextEnv.iY + rTextEnv.iOrigY;

				// Set new font
				SubEnv.pFont = pFont;

				// Parse next token with sub environment
				if ( !Parse( SubEnv, rObjList, sError, iTextPos, pcTextPos, iLevel+1, 1 ) )
				{
					return false;
				}

				// Construct bounding box
				ObjBox = SubEnv.Box;

				// Move box to absolute position
				ObjBox.MoveBox( rTextEnv.iX, rTextEnv.iY );

				// Add box to this environment
				rTextEnv.Box.AddBox( ObjBox );
				rTextEnv.LastBox = ObjBox;

				// Step to next letter
				rTextEnv.iX = rTextEnv.Box.iMaxX + rTextEnv.iSepLetter;
			}
		}
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////////
// Parse verbatim

bool COGLText::Parse_Verbatim( char *pcToken, STextEnv &rTextEnv, CObjectList &rObjList, string &sError, 
						   int &iTextPos, char** pcTextPos, int iLevel, int iParseCnt )
{
	rTextEnv.bVerbEnv = !rTextEnv.bVerbEnv;

	return true;
}




////////////////////////////////////////////////////////////////////////////////////
// Generate Text Image

bool COGLText::GenImage( COGLImage &rImage, string &sError, STextEnv &rTextEnv, CObjectList &rObjList )
{
	COGLColor colClear;
	int iWidth, iHeight, iBaseLineY;
	SBox Box = rTextEnv.Box;

	sError = "";

	// Always include baseline in image
	if ( Box.iMinY > 0 )
		Box.iMinY = 0;

	Box.GetExtent( iWidth, iHeight );

	if ( iWidth <= 0 || iHeight <= 0 )
	{
		sError = "Invalid text size.";
		return false;
	}

	if ( m_iBorderLeft < 0 )
	{
		sError = "Invalid left border size.";
		return false;
	}

	if ( m_iBorderRight < 0 )
	{
		sError = "Invalid right border size.";
		return false;
	}

	if ( m_iBorderBottom < 0 )
	{
		sError = "Invalid bottom border size.";
		return false;
	}

	if ( m_iBorderTop < 0 )
	{
		sError = "Invalid top border size.";
		return false;
	}

	iWidth += m_iBorderLeft + m_iBorderRight;
	iHeight += m_iBorderBottom + m_iBorderTop;

	// Define the clear color
	colClear.Set( 0.0f, 0.0f, 0.0f, 0.0f );

	// Create image of appropriate size
	rImage.ConvertType( CLUVIZ_IMG_RGBA, CLUVIZ_IMG_UNSIGNED_BYTE );
	rImage.ResizeCanvas( iWidth, iHeight, &colClear );
	rImage.ClearImage( &colClear);

	// Evaluate baseline relative to bottom of image
	iBaseLineY = m_iBorderBottom - Box.iMinY;

	// Set Image origin to baseline
	rImage.SetOrigin( m_iBorderLeft, iBaseLineY );

	// Draw the text
	if ( !rObjList.Apply( rImage, m_iBorderLeft, iBaseLineY, sError ) )
		return false;

	return true;
}


////////////////////////////////////////////////////////////////////////////////////
/// Apply of CObjectList

bool COGLText::CObjectList::Apply( COGLImage &rImage, int iOrigX, int iBaseLineY, string &sError )
{
	vector< CObject* >::iterator itEl;

	for ( itEl = m_vecObjPtr.begin();
		  itEl != m_vecObjPtr.end();
		  ++itEl )
	{
		if ( !(*itEl)->Apply( rImage, iOrigX + m_iX, iBaseLineY + m_iY, sError ) )
			return false;
	}

	return true;
}



////////////////////////////////////////////////////////////////////////////////////
/// Apply of CSymbol

bool COGLText::CSymbol::Apply( COGLImage &rImage, int iOrigX, int iBaseLineY, string &sError )
{
	if ( !m_pFont->CopySymbol( rImage, m_iX + iOrigX, m_iY + iBaseLineY, m_Symbol ) )
	{
		sError = "Font symbol could not be copied.";
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////
/// Apply of CHorizLine

bool COGLText::CHorizLine::Apply( COGLImage &rImage, int iOrigX, int iBaseLineY, string &sError )
{
	::LockImageAccess();
	COGLImage::TRGBAPixel *pData = (COGLImage::TRGBAPixel *) rImage.GetDataPtr();

	int iY, iX, iPos, iIdx;
	int iWidth, iHeight;
	rImage.GetSize( iWidth, iHeight );

	iX = m_iX + iOrigX;
	iY = m_iY + iBaseLineY;

	if ( iY < 0 || iY + m_iLineWidth - 1 >= iHeight ||
		 iX < 0 || iX + m_iLineLen - 1 >= iWidth )
	{
		sError = "Line coordinates outside of image.";
		::UnlockImageAccess();
		return false;
	}

	iPos = (iHeight - iY - 1 - m_iLineWidth + 1) * iWidth + iX;

	for ( iIdx = 0; iIdx < m_iLineWidth; ++iIdx, iPos += iWidth )
	{
		memset( &pData[iPos], 0xFF, m_iLineLen * sizeof( COGLImage::TRGBAPixel ) );
	}

	::UnlockImageAccess();
	return true;
}



