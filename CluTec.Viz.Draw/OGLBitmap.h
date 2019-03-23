////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLBitmap.h
//
// summary:   Declares the ogl bitmap class
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

// OGLBitmap.h: Schnittstelle f�r die Klasse OGLBitmap
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLBITMAP_H__B634C808_40D5_4BE1_8E64_8AC4B1B21B65__INCLUDED_)
#define AFX_OGLBITMAP_H__B634C808_40D5_4BE1_8E64_8AC4B1B21B65__INCLUDED_


#include "ImageReference.h"
#include "OGLBaseElement.h"
#include "OGLVertex.h"
#include "OGLColor.h"
#include "OGLImage.h"


class CLUDRAW_API COGLBitmap : public COGLBaseElement
{
public:
	typedef unsigned char uchar;

/*
	enum TTransMode 
	{
		Exact = 0,
		Luminance,
		Font
	};
*/

public:
	COGLBitmap(void);
	COGLBitmap(const COGLBitmap& oglBitmap);
	virtual ~COGLBitmap(void);

	virtual COGLBaseElement* Copy()
	{
		return (COGLBaseElement *) new COGLBitmap( *this );
	}

	COGLBitmap& operator= (const COGLBitmap& oglBitmap);

	// Get width and height of bitmap in world coordinates.
//	bool GetWorldCoordDims(float &fLeft, float &fRight, float &fBottom, float &fTop,
//							float &fZ1, float &fZ2);

	void SetScale(float fScale)
	{ 
		if (fScale > 0) m_fScale = fScale; 
		TellParentContentChanged();
	}

	void SetPosition(float fX, float fY, float fZ);
	void SetPosition(COGLVertex &rVex);

/*
	void SetTransparentColor(unsigned char r, unsigned char g, unsigned char b)
	{ m_rgbaTransPix.Set(r, g, b, 0); }

	void SetTransparentColor(COGLColor &rCol)
	{ 
		float fCol;
		m_rgbaTransPix.Set( 
			((fCol = rCol[0]*255.0f) >= 255.0f ? uchar(255) : uchar(fCol)),
			((fCol = rCol[1]*255.0f) >= 255.0f ? uchar(255) : uchar(fCol)),
			((fCol = rCol[2]*255.0f) >= 255.0f ? uchar(255) : uchar(fCol)),
			0); 
	}

	void SetTransparencyMode(TTransMode iMode)
	{ m_iTransMode = iMode; }
*/
	void SetVAlign(float fAlign)
	{ 
		/*if (fAlign <= -1.0f) m_fAlign = -1.0f;
		else if (fAlign >= 2.0f) m_fAlign = 2.0f;
		else*/
		m_fVAlign = fAlign;
		TellParentContentChanged();
	}

	void SetHAlign(float fAlign)
	{ 
		/*if (fAlign <= -1.0f) m_fAlign = -1.0f;
		else if (fAlign >= 2.0f) m_fAlign = 2.0f;
		else*/
		m_fHAlign = fAlign;
		TellParentContentChanged();
	}

	CImageReference GetImageRef()
	{ return m_Image; }

	void SetImageRef( CImageReference& rImage )
	{ m_Image = rImage; TellParentContentChanged(); }

/*
	void EnableInvert(bool bVal = true)
	{ m_bInvert = bVal; }

	void EnableTransparency(bool bVal = true)
	{ m_bUseTransparency = bVal; }

	void EnableScaleWithCurColor(bool bVal = true)
	{ m_bScaleWithCurColor = bVal; }
*/

	// iMode alows different types of Apply calls.
	// Arbitrary data may be passed by pvData.
	bool Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData &rData);

	// Return a pointer to the data
	uchar* Data();

protected:
	void ResetVars();
	void TellParentContentChanged();

protected:
	float m_fScale;
	float m_fX, m_fY, m_fZ;

	//bool m_bInvert;
	//bool m_bUseTransparency;
	//bool m_bScaleWithCurColor;

	//TTransMode m_iTransMode;
	//TRGBAPixel m_rgbaTransPix;

	// If m_fVAlign == 0 then bitmaps are aligned with their bottom 
	// to the point given. If Align == 1, then it is aligned with
	// the top.
	float m_fVAlign;

	// If m_fHAlign == 0 then bitmaps are aligned with their left 
	// to the point given. If Align == 1, then it is aligned with
	// the right.
	float m_fHAlign;

	// The actual image is stored in an image reference object
	CImageReference m_Image;
};

#endif
