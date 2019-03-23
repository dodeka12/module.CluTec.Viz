////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLBaseTransform.h
//
// summary:   Declares the ogl base transform class
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

// Class written by Vladimir Banarer, 3.12.2001.


// OGLBaseTransform.h: Schnittstelle f�r die Klasse COGLBaseTransform.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLBASETRANSFORM_H__B9CF66CC_ABAC_40D4_93ED_4B8BBA70D662__INCLUDED_)
#define AFX_OGLBASETRANSFORM_H__B9CF66CC_ABAC_40D4_93ED_4B8BBA70D662__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OGLBaseElement.h"

class COGLBaseTransform : public COGLBaseElement
{
public:
	COGLBaseTransform();
	virtual ~COGLBaseTransform();

	virtual bool Apply(int iMode = 0, void *pvData = 0);

	/// ALL ANGLES ARE GIVEN IN DEGS

	// Set Rotation by Euler Angles
	bool SetRotation(float fRadAngleX, float fRadAngleY, float fRadAngleZ);

	// Set Rotation by angle and axis
	bool SetRotation(float fRadAngle, float fX, float fY, float fZ);

	// Set Translation
	bool SetTranslation(float fX, float fY, float fZ);
	bool AddToTranslation(float fX, float fY, float fZ)
	{ return SetTranslation(m_fTX+fX, m_fTY+fY, m_fTZ+fZ); }

	// Combined Versions
	bool SetTransformation(float RAngle, float RX, float RY, float RZ, float TX, float TY, float TZ)
	{ SetRotation(RAngle, RX, RY, RZ); SetTranslation(TX, TY, TZ); return true; }

	bool SetTransformation(float RXAngle, float RYAngle, float RZAngle, float TX, float TY, float TZ)
	{ SetRotation(RXAngle, RYAngle, RZAngle); SetTranslation(TX, TY, TZ); return true; }

	bool SetTransformation(float *pfTrans, bool bEuler = true)
	{ if (bEuler) return SetTransformation(pfTrans[0], pfTrans[1], pfTrans[2], pfTrans[3], pfTrans[4], pfTrans[5]); 
	  else return SetTransformation(pfTrans[0], pfTrans[1], pfTrans[2], pfTrans[3], pfTrans[4], pfTrans[5], pfTrans[6]);
	}

protected:
	float m_fRAngle, m_fRX, m_fRY, m_fRZ, m_fTX, m_fTY, m_fTZ;

	static double m_dPi, m_dRadPerDeg;
};

#endif // !defined(AFX_OGLBASETRANSFORM_H__B9CF66CC_ABAC_40D4_93ED_4B8BBA70D662__INCLUDED_)
