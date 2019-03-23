////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLAnimColor.cpp
//
// summary:   Implements the ogl animation color class
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

// OGLColor.cpp: Implementierung der Klasse COGLAnimColor.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OGLAnimColor.h"
#include "OGLBaseElementList.h"
#include "CluTec.Viz.Fltk\Fl_math.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLAnimColor::COGLAnimColor()
{
	m_sTypeName = "AnimColor";

	for (int i = 0; i < 4; i++)
	{
		m_pfCol[i]    = 0.0f;
		m_pfColDir[i] = 0.0f;
	}

	m_eMode = SINUS;
	m_fFreq = 1;

	m_fPi = 2.0f * (float) asin(1.0);
}

COGLAnimColor::COGLAnimColor(const COGLAnimColor& rCol)
{
	*this = rCol;
}

//////////////////////////////////////////////////////////////////////
// Copy Operator

COGLAnimColor& COGLAnimColor::operator=(const COGLAnimColor& rCol)
{
	COGLColor::operator=(rCol);

	memcpy(m_pfColDir, rCol.m_pfColDir, 4 * sizeof(float));

	m_eMode = rCol.m_eMode;
	m_fFreq = rCol.m_fFreq;
	m_fPi	= rCol.m_fPi;

	return *this;
}

//////////////////////////////////////////////////////////////////////
/// Tell parents that content has changed

void COGLAnimColor::TellParentContentChanged()
{
	list<COGLBaseElement*>::iterator itEl;

	for (itEl = m_listParent.begin();
	     itEl != m_listParent.end();
	     ++itEl)
	{
		COGLBaseElementList* pList = dynamic_cast<COGLBaseElementList*>((COGLBaseElement*) (*itEl));
		if (pList)
		{
			pList->SetContentChanged(true, true, false);
		}
	}
}

//////////////////////////////////////////////////////////////////////
/// Apply Color

bool COGLAnimColor::Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData& rData)
{
	if (eMode == COGLBaseElement::PICK)
	{
		return true;
	}

	m_pfColDiff[0] = 1.0f;
	m_pfColDiff[1] = 1.0f;
	m_pfColDiff[2] = 1.0f;
	m_pfColDiff[3] = 1.0f;

	rData.bNeedAnimate = true;
	TellParentContentChanged();

//	if (rData.bAnimate)
//	{
	float fFac   = 0.0f;
	float* pfVal = m_pfColAnim;

	if (m_eMode == CONSTANT)
	{
		fFac = fmod(2.0f * m_fFreq * float(rData.dTime), 2.0f) - 1.0f;
	}
	else if (m_eMode == SINUS)
	{
		fFac = (float) sin(double(2.0f * m_fPi * m_fFreq * float(rData.dTime)));
	}
	else if (m_eMode == SINUS2)
	{
		fFac = (float) (0.5 + 0.5 * sin(double(2.0f * m_fPi * m_fFreq * float(rData.dTime))));
	}
	else
	{
		rData.bNeedAnimate = false;
	}

	pfVal[0] = m_pfCol[0] + fFac * m_pfColDir[0];
	pfVal[1] = m_pfCol[1] + fFac * m_pfColDir[1];
	pfVal[2] = m_pfCol[2] + fFac * m_pfColDir[2];
	pfVal[3] = m_pfCol[3] + fFac * m_pfColDir[3];

	for (int i = 0; i < 4; i++)
	{
		if (pfVal[i] < 0.0f) { pfVal[i] = 0.0f; }
		else if (pfVal[i] > 1.0f)
		{
			pfVal[i] = 1.0f;
		}
	}

	m_pfColDiff[3] = pfVal[3];

	glColor4fv(pfVal);
	memcpy(rData.pfCurColor, pfVal, 4 * sizeof(float));

	glMaterialfv(GL_FRONT, GL_AMBIENT, pfVal);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, m_pfColDiff);

/*
}
        else
        {
                m_pfColDiff[3] = m_pfCol[3];

                glColor4fv( m_pfCol );
                glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_pfCol);
                glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_pfColDiff);
        }
*/
	return true;
}
