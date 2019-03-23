////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLLatexText.h
//
// summary:   Declares the ogl latex text class
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

// OGLLatexText.h: Schnittstelle f�r die Klasse COGLLatexText
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLLATEXTEXT_H__B634C808_40D5_4BE1_8E64_8AC4B1B21B65__INCLUDED_)
#define AFX_OGLLATEXTEXT_H__B634C808_40D5_4BE1_8E64_8AC4B1B21B65__INCLUDED_

#include <map>
#include <string>
#include <fstream>

using namespace std;

#include "OGLBaseElement.h"
#include "OGLVertex.h"
#include "OGLImage.h"
#include "ImageReference.h"

#include <sys/types.h>
#include <sys/stat.h>

#ifdef _GNUCPP3_
#   define _stat stat
#endif


class CLUDRAW_API COGLLatexText //: public COGLBaseElement
{
public:
	struct SLatexText
	{
		string sText;
		string sBMPName;
		string sBMPFilename;
		string sScriptName;
		string sScriptPath;
		string sMainFont;
		float fScale;
		CImageReference ImageRef;
		COGLColor oglColor;
		bool bInUse;
		bool bIsRendered;
		// Variables used if latex file is given.
		bool bIsFile;
		string sLatexFilename;
		struct _stat FileInfo; // Used in case latex filename is given.
	};

	typedef map<string, SLatexText> TLatexMap;

	class CRenderError
	{
	public:
		CRenderError()
		{ m_sErrorText = ""; }

		CRenderError(const char* pcText)
		{ m_sErrorText = pcText; }

		string m_sErrorText;
	};

public:
	COGLLatexText(void);
	~COGLLatexText(void);

	// Reset All In Use flags
	void ResetInUseFlags();

	// Remove all elements from map that are not in use
	void PruneMap();

	// Set Name of current script without extension
	void SetScriptName(const char* pcScriptName)
	{ m_sScriptName = pcScriptName; }

	// Set Path to current script
	void SetScriptPath(const char* pcScriptPath)
	{ m_sScriptPath = pcScriptPath; }

	void EnableRenderLatex(bool bVal = true)
	{ m_bRenderLatex = bVal; }

	string GetRenderErrorText()
	{ return m_sRenderError; }

	float GetMagStepScale(int iMagStep);
/*
	bool Add(const char* pcText, float fX, float fY, float fZ, 
			float fScale = 1.0f, float fVAlign = 0.0f, float fHAlign = 0.0f, 
			const char *pcBMPName = 0);
	bool Add(const char* pcText, COGLVertex &rVex, 
			float fScale = 1.0f, float fVAlign = 0.0f, float fHAlign = 0.0f, 
			const char *pcBMPName = 0)
	{ return Add(pcText, rVex[0], rVex[1], rVex[2], fScale, fVAlign, fHAlign, pcBMPName); }
*/
	bool Add( const char *pcText, float fScale, const char* pcMainFont, COGLColor &rColor,
				CImageRepository &rImgRep, const char *pcBMPName = 0 );

	// iMode alows different types of Apply calls.
	// Arbitrary data may be passed by pvData.
	// If pvData == 0 then all bitmaps are displayed.
	// If pvData != 0 it has to point to a string which
	// identifies the element to be displayed.
	// bool Apply(int iMode = 0, void *pvData = 0);

	// Get extension of Latex Bitmap in world coordinates
	// for text given in pcText.
/*
	bool GetWorldCoordDims(const char *pcText, float &fLeft, float &fRight, 
							float &fBottom, float &fTop,
							float &fZ1, float &fZ2);
*/

	// Get reference to COGLImage of text, if text has been rendered.
	CImageReference GetImageRef(const char *pcText);

protected:
	bool RenderLatex(SLatexText &rLatex);
	bool GetGhostscriptPath();

protected:
	TLatexMap m_mapLatex;
	string m_sScriptPath;
	string m_sScriptName;
	string m_sRenderError;
	
	string m_sGSPath;
	string m_sGSError; // String contains error message if ghostscript could not be located

	bool m_bRenderLatex;

};

#endif
