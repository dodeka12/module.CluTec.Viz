////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLPeek.h
//
// summary:   Declares the ogl peek class
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

// OGLMaterial.h: Schnittstelle f�r die Klasse COGLMaterial.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLPEEK_H__INCLUDED_)
#define AFX_OGLPEEK_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OGLBaseElement.h"


class CLUDRAW_API COGLPeek  : public COGLBaseElement
{
public:
	static const int PT_NONE;
	static const int PT_FRAMES;

	typedef Matrix<double> TMatrix;

public:
	COGLPeek();
	COGLPeek( const string &sName, unsigned int uType );
	COGLPeek(const COGLPeek& rTrans);

	virtual COGLBaseElement* Copy()
	{
		return (COGLBaseElement *) new COGLPeek( *this );
	}

	COGLPeek& operator= (const COGLPeek& rPeek);

	void Reset();

	void SetType( unsigned int uType )
	{ m_uType = uType; }

	unsigned int GetType( ) { return m_uType; }

	bool GetViewExtentInLocalCoords( double pdBL[3], double pdBR[3], double pdTR[3], double pdTL[3], 
									 double dDepth = 0.0 );

	bool GetViewExtentInScreenCoords( int &iX1, int &iY1, int &iX2, int &iY2 );

	bool Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData &rData);

protected:
	unsigned int m_uType;
	TMatrix m_mProjMat, m_mModelMat, m_mTexMat;
	int m_piViewport[4];

};

#endif // !defined(AFX_OGLMATERIAL_H__2296DD19_C205_454A_82DB_D1881903B593__INCLUDED_)
