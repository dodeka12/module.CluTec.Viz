////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      CLUException.cpp
//
// summary:   Implements the clu exception class
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

// CLUException.cpp: Implementierung der Klasse CCLUException.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <iostream>
#include "CLUException.h"

using std::cerr;

// Initialize static variable
int CCluException::sm_iDebugLevel = 1;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion

CCluException::CCluException(int iLevel)
{
	Init(iLevel, "", "", "", -1);
}

CCluException::CCluException(int iLevel, const char* pcMessage)
{
	Init(iLevel, pcMessage, "", "", -1);
}

CCluException::CCluException(int iLevel, const char* pcMessage, const char* pcFile, const char* pcFunction, int iLine)
{
	Init(iLevel, pcMessage, pcFile, pcFunction, iLine);
}

CCluException::~CCluException()
{

}

void CCluException::Init(int iLevel, const char* pcMessage, const char* pcFile, const char* pcFunction, int iLine)
{
	m_iLevel = iLevel;
	m_sErrorText = (pcMessage == 0 ? "" : pcMessage);
	m_sFile = (pcFile == 0 ? "" : pcFile);
	m_sFunction = (pcFunction == 0 ? "" : pcFunction);
	m_iLine = iLine;

	_ASSERTE(iLevel != 0);
}


//////////////////////////////////////////////////////////////////////
/// Print errors in some user readable way.

string CCluException::PrintError()
{
	stringstream zText;

	if (m_iLevel <= sm_iDebugLevel)
	{
		switch(m_iLevel)
		{
		case 0:
			zText << string("Fatal Error:");
			break;

		case 1:
			zText << string("Error:");
			break;

		case 2:
			zText << string("Warning:");
			break;

		default:
			zText << string("Debug ") << (m_iLevel-2) << string(":");
			break;
		}

		zText << m_sNumberText << string(" ") << m_sErrorText;

		if ( m_sFile.length() > 0 )
			zText << string(", ") << m_sFile;

		if ( m_sFunction.length() > 0 )
			zText << string(", ") << m_sFunction;

		if ( m_iLine >= 0 )
			zText << string(", ") << m_iLine;
	}

	return zText.str();
}

//////////////////////////////////////////////////////////////////////
/// Add given number to m_sNumberText

void CCluException::AddNumberToText(uchar ucNumber)
{
	char pcNumber[10];

	sprintf_s(pcNumber, 10, "%02x:", ucNumber);

	m_sNumberText += pcNumber;
}

