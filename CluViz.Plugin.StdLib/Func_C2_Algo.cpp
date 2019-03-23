////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_C2_Algo.cpp
//
// summary:   Implements the function c 2 algo class
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

#include "stdafx.h"
#include "Func_C2_Algo.h"

//////////////////////////////////////////////////////////////////////
// Project Conic to nearest degenerate conic
//
// Parameters:
//	1. Conic
//
// Returns
//	List of
//		1. degenerate conic
//		2. distance from initial to degenerate conic

bool  MakeConicDegenerateFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount != 1)
	{
		int piVal[] = { 1 };
		rCB.GetErrorList().WrongNoOfParams(piVal, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_MULTIV)
	{
		rCB.GetErrorList().InvalidType(mVars(0), iLine, iPos);
		return false;
	}

	TCVScalar fDist;
	TMultiV& vA = *mVars(0).GetMultiVPtr();

	rVar.New(PDT_VARLIST);
	TVarList& rList = *rVar.GetVarListPtr();

	rList.Add(2);

	rList[0].New(PDT_MULTIV);
	TMultiV& vB = *rList[0].GetMultiVPtr();

	m_pEl2GABase->MakeConicDegenerate(vA, vB, fDist, rCB.GetFilter()->m_El2Filter.IsOPNSEnabled());

	rList[1] = fDist;

	return true;
}

//////////////////////////////////////////////////////////////////////
// Find all degenerate conics that can be generated through
// linear combinations of two given conics.
//
// Parameters:
//	1. 1st Conic
//	2. 2nd Conic
//
// Returns
//		List of degenerate conics (1 or 3)

bool  FindAllDegConicsFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount != 2)
	{
		int piVal[] = { 2 };
		rCB.GetErrorList().WrongNoOfParams(piVal, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_MULTIV)
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_MULTIV)
	{
		rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}

	TMultiV& vA = *mVars(0).GetMultiVPtr();
	TMultiV& vB = *mVars(1).GetMultiVPtr();

	if (vA.GetBase().BaseID() != ID_EL2GA)
	{
		rCB.GetErrorList().GeneralError("First multivector is not from conic space.", iLine, iPos);
		return false;
	}

	if (vB.GetBase().BaseID() != ID_EL2GA)
	{
		rCB.GetErrorList().GeneralError("Second multivector is not from conic space.", iLine, iPos);
		return false;
	}

	bool bOPNS;
	Mem<uint> mList;
	MemObj<TMultiV> mConicList;

	if (!GradeList(vA, mList, rCB.GetSensitivity()) || (mList.Count() != 1))
	{
		rCB.GetErrorList().GeneralError("The first multivector does not represent a conic.", iLine, iPos);
		return false;
	}

	if (mList[0] == 5)
	{
		bOPNS = true;
	}
	else if (mList[0] == 1)
	{
		bOPNS = false;
	}
	else
	{
		rCB.GetErrorList().GeneralError("The first multivector does not represent a conic.", iLine, iPos);
		return false;
	}

	if (!GradeList(vB, mList, rCB.GetSensitivity()) || (mList.Count() != 1))
	{
		rCB.GetErrorList().GeneralError("The second multivector does not represent a conic.", iLine, iPos);
		return false;
	}

	if ((bOPNS && (mList[0] != 5)) ||
	    (!bOPNS && (mList[0] != 1)))
	{
		rCB.GetErrorList().GeneralError("The second multivector does not represent a conic.", iLine, iPos);
		return false;
	}

	if (bOPNS)
	{
		if (!m_pEl2GABase->FindAllDegConics(*vA, *vB, mConicList, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Error evaluating degenerate conics.", iLine, iPos);
			return false;
		}
	}
	else
	{
		if (!m_pEl2GABase->FindAllDegConics(vA, vB, mConicList, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Error evaluating degenerate conics.", iLine, iPos);
			return false;
		}
	}

	int i, iCnt = int(mConicList.Count());
	rVar.New(PDT_VARLIST);
	TVarList& rList = *rVar.GetVarListPtr();

	rList.Add(iCnt);
	for (i = 0; i < iCnt; i++)
	{
		if (bOPNS)
		{
			rList[i] = *mConicList[i];
		}
		else
		{
			rList[i] = mConicList[i];
		}
	}

	return true;
}
