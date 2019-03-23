////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLReadBitmap.h
//
// summary:   Declares the ogl read bitmap class
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

// OGLColor.h: Schnittstelle f�r die Klasse COGLReadBitmap.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLREADBITMAP_H__ECC38C81_0DD0_4C4F_B77D_86F72B5AA0DE__INCLUDED_)
#define AFX_OGLREADBITMAP_H__ECC38C81_0DD0_4C4F_B77D_86F72B5AA0DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OGLBEReference.h"
#include "OGLBaseElement.h"
#include "ImageReference.h"


class CLUDRAW_API COGLReadBitmap  : public COGLBaseElement
{
public:
	enum ETarget
	{
		READ_TO_NONE = 0,
		READ_TO_IMAGE,
		READ_TO_TEXTURE
	};

public:
	COGLReadBitmap();
	COGLReadBitmap(const COGLReadBitmap& rCol);

	virtual COGLBaseElement* Copy()
	{
		return (COGLBaseElement *) new COGLReadBitmap( *this );
	}

	COGLReadBitmap& operator= (const COGLReadBitmap& rObj);

	void Reset();

	void Enable( bool bVal = true )
	{ m_bEnabled = bVal; }

	void SetTarget( const ETarget &eTarget )
	{ m_eTarget = eTarget; }

	void SetTexture( COGLBEReference &rTarget )
	{ m_refTexture = rTarget; }

	void SetImage( CImageReference &rTarget )
	{ m_refImage = rTarget; }

	void SetArea( double dX1, double dY1, double dZ1, 
				  double dX2, double dY2, double dZ2 )
	{
		m_dX1 = dX1; m_dY1 = dY1; m_dZ1 = dZ1;
		m_dX2 = dX2; m_dY2 = dY2; m_dZ2 = dZ2;
	}

	void SetTargetOrigin( int iX, int iY )
	{
		m_iTrgX = iX;
		m_iTrgY = iY;
	}

	void SetTargetArea( int iX, int iY, int iWidth, int iHeight )
	{
		m_iTrgX = iX;
		m_iTrgY = iY;
		m_iTrgW = iWidth;
		m_iTrgH = iHeight;
	}

	bool Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData &rData);

protected:
	void GetWinArea( int &iX, int &iY, int &iWidth, int &iHeight );

protected:
	bool m_bEnabled;
	ETarget m_eTarget;

	// The texture to which frame buffer data is copied
	COGLBEReference m_refTexture;

	// The image to which frame buffer data is copied
	CImageReference m_refImage;
	
	double m_dX1, m_dY1, m_dZ1;
	double m_dX2, m_dY2, m_dZ2;

	int m_iTrgX, m_iTrgY;
	int m_iTrgW, m_iTrgH;
};

#endif // !defined(AFX_OGLCOLOR_H__ECC38C81_0DD0_4C4F_B77D_86F72B5AA0DE__INCLUDED_)
