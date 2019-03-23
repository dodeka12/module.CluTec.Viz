////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      CodeVarList.cpp
//
// summary:   Implements the code variable list class
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
#include "CodeVarList.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCodeVarList::CCodeVarList()
{
	m_VarInvalid.New(PDT_NOTYPE, "_INVALID_");
	m_VarInvalid.EnableProtect();
}

CCodeVarList::~CCodeVarList()
{

}



//////////////////////////////////////////////////////////////////////
// New creates new variable with given name and type
// and returns position in List. return value -1 indicates error.

bool CCodeVarList::New(const char *pcName, ECodeDataType _nType)
{
	if (!pcName || *pcName == 0)
		return false;

	TVarMapIt itEl;

	// If variable already exists cannot create it again.
	if ( (itEl = m_mapVarList.find( string(pcName) )) != m_mapVarList.end() )
		return false;

	CCodeVar& rVar = m_mapVarList[ string(pcName) ];

	if ( !rVar.New( _nType, pcName ) )
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////
// Delete Variable from List

bool CCodeVarList::Delete(const char *pcName)
{
	if ( !pcName || *pcName == 0 )
		return false;

	TVarMapIt itEl;

	if ( (itEl = m_mapVarList.find( string(pcName) )) == m_mapVarList.end() )
		return false;

	m_mapVarList.erase( itEl );
	return true;
}


CCodeVar& CCodeVarList::GetVar(const char* pcName)
{
	if ( !pcName || *pcName == 0 )
		return m_VarInvalid;

	TVarMapIt itEl;

	if ( (itEl = m_mapVarList.find( string(pcName) )) == m_mapVarList.end() )
		return m_VarInvalid;

	return itEl->second;
}

//////////////////////////////////////////////////////////////////////
// Get Position of Var in List. Returns -1 if variable does not exist.
// Very simple implementation currently. Should be improved to increase
// speed of operation.

//int CCodeVarList::GetPos(const char* pcName)
//{
//	if (!pcName || *pcName == 0)
//		return -1;
//
//	int i, iLen = m_mVarList.Count();
//	
//	for(i=0;i<iLen;i++)
//	{
//		if (m_mVarList[i].Name() == pcName)
//			return i;
//	}
//
//	return -1;
//}


