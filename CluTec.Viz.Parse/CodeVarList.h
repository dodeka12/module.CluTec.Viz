////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      CodeVarList.h
//
// summary:   Declares the code variable list class
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

#if !defined(AFX_CODEVARLIST_H__FD74F99C_5109_452F_B7C3_E4765984BA88__INCLUDED_)
#define AFX_CODEVARLIST_H__FD74F99C_5109_452F_B7C3_E4765984BA88__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <string>
#include "CodeVar.h"

using namespace std;

class CCodeVarList
{
public:
	typedef map<string,CCodeVar> TVarMap;
	typedef map<string,CCodeVar>::iterator TVarMapIt;

public:
	CCodeVarList();
	virtual ~CCodeVarList();

	// Empty variable list.
	void Reset() { m_mapVarList.clear(); }

	// New creates new variable with given name and type
	// returns false if variable already exists
	bool New(const char *pcName, ECodeDataType _nType = PDT_INT);
	bool Delete(const char *pcName);

	// If variable of given name does not exist, returns variable m_VarInvalid.
	// This variable is by default called "_INVALID_" and is of type PDT_NOTYPE.
	CCodeVar& GetVar(const char* pcName);
	//{ int i = GetPos(pcName); if (i < 0) return m_VarInvalid; else return m_mVarList[i]; }

	// Get Position of Var in List. Returns -1 if variable does not exist.
	//int GetPos(const char* pcName);

	TVarMapIt Begin()
	{ return m_mapVarList.begin(); }

	TVarMapIt End()
	{ return m_mapVarList.end(); }

	CCodeVar& operator[] ( const char* pcName )
	{ return GetVar( pcName ); }


	//{ if (i < 0 || i >= (int) m_mVarList.Count()) return m_VarInvalid; else return m_mVarList[i]; }

	int Count() { return (int) m_mapVarList.size(); }

protected:

	TVarMap m_mapVarList;
	TVarMapIt m_mapVarIt;

	CCodeVar m_VarInvalid;
};

#endif // !defined(AFX_CODEVARLIST_H__FD74F99C_5109_452F_B7C3_E4765984BA88__INCLUDED_)
