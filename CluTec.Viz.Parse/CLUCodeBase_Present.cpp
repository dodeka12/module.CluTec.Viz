////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      CLUCodeBase_Present.cpp
//
// summary:   Implements the clu code base present class
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

// CLUCodeBase.cpp: Implementierung der Klasse CCLUCodeBase.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
//#include "CvFltk\glut.h"

#include "CLUCodeBase.h"


//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
/// Draw Script Files Text
void CCLUCodeBase::DrawScriptFileIndex()
{
/*
	int iSize;
	if ((iSize = m_vecScriptList.size()) == 0 || m_iCurScript == -1)
		return;

	// Switch to overlay
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 100, 100, 0, -1, 10);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
*/

/*
	string sText1, sText2;
	int i;

	for ( i = 0; i < m_iCurScript; i++ )
	{
		sText1 += m_vecScriptList[i].sTitle;
		if (i < iSize-1)
			sText1 += " - ";
	}

	for ( ; i < iSize; i++ )
	{
		sText2 += m_vecScriptList[i].sTitle;
		if (i < iSize-1)
			sText2 += " - ";
	}



*/	

/*
	glColor3f(1, 0, 0);
	
	float fMagStep = float(pow(1.2, double(10) * 0.5));
	if (m_oglLatex.Add(m_vecScriptList[m_iCurScript].sTitle.c_str(), 50, 0, 0, 
						fMagStep, 1.0f, 0.5f))
	{
		m_oglLatex.Apply(0, (void *)m_vecScriptList[m_iCurScript].sTitle.c_str());
	}


	// return from overlay
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
*/
}


//////////////////////////////////////////////////////////////////////
/// Clear Script List
void CCLUCodeBase::ClearScriptList()
{
	m_vecScriptList.clear();
	m_iCurScript = -1;
}

//////////////////////////////////////////////////////////////////////
/// Add Script File
bool CCLUCodeBase::AddScriptToList(string sTitle, string sFilename)
{
	SScriptFile Script;

	if (!sTitle.size() || !sFilename.size())
		return false;

	Script.sTitle = sTitle;
	Script.sFilename = sFilename;

	m_vecScriptList.push_back(Script);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Next Script File
int CCLUCodeBase::NextScriptFile()
{
	int iSize;

	if ((iSize = int(m_vecScriptList.size())) == 0)
		return -1;

	if (m_iCurScript == -1)
		m_iCurScript = 0;
	else if (m_iCurScript+1 < iSize)
		m_iCurScript++;

	return m_iCurScript;
}

//////////////////////////////////////////////////////////////////////
/// Previous Script File
int CCLUCodeBase::PrevScriptFile()
{
	int iSize;

	if ((iSize = int(m_vecScriptList.size())) == 0)
		return -1;

	if (m_iCurScript == -1)
		m_iCurScript = 0;
	else if (m_iCurScript-1 >= 0)
		m_iCurScript--;

	return m_iCurScript;
}

//////////////////////////////////////////////////////////////////////
/// Set Current Script
bool CCLUCodeBase::SetCurScriptIdx(int iVal)
{
	int iSize;

	if ((iSize = int(m_vecScriptList.size())) == 0)
		return false;

	if (iVal < 0  || iVal >= iSize)
		return false;

	m_iCurScript = iVal;
	return true;
}



//////////////////////////////////////////////////////////////////////
/// Get Script Filename
string CCLUCodeBase::GetCurScriptFilename()
{
	string sFilename;

	if (m_iCurScript == -1)
		return sFilename;
	else
		return m_vecScriptList[m_iCurScript].sFilename;
}

//////////////////////////////////////////////////////////////////////
/// Get Script Title
string CCLUCodeBase::GetCurScriptTitle()
{
	string sTitle;

	if (m_iCurScript == -1)
		return sTitle;
	else
		return m_vecScriptList[m_iCurScript].sTitle;
}

