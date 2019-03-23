////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      CodeBase.cpp
//
// summary:   Implements the code base class
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
#include "CodeBase.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCodeBase::CCodeBase()
{
	m_iLoopCountLimit = 100000;
	SetCurrentNamespace(NS_GLOBAL);
}

CCodeBase::~CCodeBase()
{
}

//////////////////////////////////////////////////////////////////////
// Returns Variable of type PDT_NOTYPE if error occured.
// Tries first to create variable in local variable list.
// if there is no such list, then variable is created in global list.

CCodeVar& CCodeBase::NewVar(const char* pcName, ECodeDataType _nType, const char* pcNamespace)
{
	if (pcNamespace && !strcmp(pcNamespace, NS_CURRENT))
	{
		if (m_csCurNamespace.Len() == 0)
		{
			pcNamespace = 0;
		}
		else
		{
			pcNamespace = m_csCurNamespace.Str();
		}
	}

	// Check whether local variable list exists
	TCodeVarListPtr pLocalList = GetStackLocalVarList(0);

	if (!pcNamespace || !strcmp(pcNamespace, NS_LOCAL))
	{
		if (pLocalList)
		{
			pLocalList->New(pcName, _nType);
			return (*pLocalList)[pcName];
		}
		else
		{
			m_mVarList.New(pcName, _nType);
			return m_mVarList[pcName];
		}
	}
	else	// if (!strcmp(pcNamespace, NS_GLOBAL))
	{
		m_mVarList.New(pcName, _nType);
		return m_mVarList[pcName];
	}
}

//////////////////////////////////////////////////////////////////////

bool CCodeBase::DeleteVar(const char* pcName, const char* pcNamespace)
{
	if (pcNamespace && !strcmp(pcNamespace, NS_CURRENT))
	{
		if (m_csCurNamespace.Len() == 0)
		{
			pcNamespace = 0;
		}
		else
		{
			pcNamespace = m_csCurNamespace.Str();
		}
	}

	// Check whether local variable list exists
	TCodeVarListPtr pLocalList = GetStackLocalVarList(0);

	if (!pcNamespace || !strcmp(pcNamespace, NS_LOCAL))
	{
		if (pLocalList)
		{
			return pLocalList->Delete(pcName);
		}
		else
		{
			return m_mVarList.Delete(pcName);
		}
	}
	else
	{
		return m_mVarList.Delete(pcName);
	}
}

//////////////////////////////////////////////////////////////////////
// Returns Variable of type PDT_NOTYPE if error occured.
// Searches first in LocalVarList then in ConstVarList and then in VarList

CCodeVar& CCodeBase::GetVar(const char* pcName, const char* pcNamespace)
{
	TCodeVarListPtr pLocalVar;
	//int i, iCount = m_mLocalVarStack.Count();

	if (pcNamespace && !strcmp(pcNamespace, NS_CURRENT))
	{
		if (m_csCurNamespace.Len() == 0)
		{
			pcNamespace = 0;
		}
		else
		{
			pcNamespace = m_csCurNamespace.Str();
		}
	}

	CCodeVar& rVar = m_mConstVarList.GetVar(pcName);
	if (rVar.Type() != PDT_NOTYPE)
	{
		return rVar;
	}

	if (!pcNamespace || !strcmp(pcNamespace, NS_LOCAL))
	{
		if ((pLocalVar = GetStackLocalVarList(0)) != 0)
		{
			CCodeVar& rLVar = pLocalVar->GetVar(pcName);

			if (rLVar.Type() != PDT_NOTYPE)
			{
				return rLVar;
			}
		}
		else
		{
			return m_mVarList.GetVar(pcName);
		}
	}

	if (!pcNamespace || !strcmp(pcNamespace, NS_GLOBAL))
	{
		return m_mVarList.GetVar(pcName);
	}

	return rVar;
}

///////////////////////////////////////////////////////////////
/// Set Currentnamespace

void CCodeBase::SetCurrentNamespace(const char* pcNamespace)
{
	if (!pcNamespace)
	{
		m_csCurNamespace = "";
	}
	else
	{
		m_csCurNamespace = pcNamespace;
	}
}

/////////////////////////////////////////////////////////////////////////
// Return a temporary variable. Throws exception if error occured.
CCodeVar& CCodeBase::NewTempVar(ECodeDataType _nType)
{
	if (m_mTempVarList.Count() == m_mTempVarList.Capacity())
	{
		m_mTempVarList.Reserve(m_mTempVarList.Count() + 100);
	}

	if (!m_mTempVarList.Add(1))
	{
		throw CCluOutOfMemory(__FILE__, __FUNCTION__, __LINE__);
	}

	CCodeVar& rVar = m_mTempVarList.Last();

	if (_nType != PDT_NOTYPE)
	{
		rVar.New(_nType, "__Temp__");
	}
	else
	{
		rVar.Name() = "__Temp__";
	}

	return rVar;
}

/////////////////////////////////////////////////////////////////////////
// Check whether variable of given name is temporary variable
bool CCodeBase::IsTempVar(CCodeVar* pVar)
{
	int iIdx, iCnt = int(m_mTempVarList.Count());

	if ((iCnt == 0) || (pVar == 0))
	{
		return false;
	}

	if ((iIdx = int(m_mTempVarList.IndexOf(pVar))) >= 0)
	{
		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////
// Delete a list of temporary variables
void CCodeBase::DeleteTempVar(int iStartIdx, int iCount)
{
	int iIdx, iCnt = int(m_mTempVarList.Count());

	if ((iCnt == 0) || (iCount == 0))
	{
		return;
	}

	if ((iStartIdx >= iCnt) || (iStartIdx < 0) || (iStartIdx + iCount > iCnt))
	{
		throw CCluAssertion(__FILE__, __FUNCTION__, __LINE__);
	}

	for (iIdx = iStartIdx; iIdx < iCnt; ++iIdx)
	{
		m_mTempVarList[iIdx].Destroy();
	}

	m_mTempVarList.Del(iStartIdx, iCount);
}

/////////////////////////////////////////////////////////////////////////
bool CCodeBase::ForgetTempVar(CCodeVar* pVar)
{
	int iIdx, iCnt = int(m_mTempVarList.Count());

	if ((iCnt == 0) || (pVar == 0))
	{
		return false;
	}

	if ((iIdx = int(m_mTempVarList.IndexOf(pVar))) >= 0)
	{
		return m_mTempVarList.Forget(iIdx);
	}

	return false;
}
