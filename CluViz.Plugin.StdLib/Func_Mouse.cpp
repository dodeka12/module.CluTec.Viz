////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_Mouse.cpp
//
// summary:   Implements the function mouse class
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

#include "Func_Mouse.h"

//////////////////////////////////////////////////////////////////////
/// Set Mouse Mode Function

bool  SetMouseModeFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());
	TCVCounter iMode;

	if (iVarCount != 1)
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToCounter(iMode))
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if ((iMode < 0) || (iMode > 9))
	{
		rCB.GetErrorList().GeneralError("Value for mouse mode has to be in the range {0,...,9}", iLine, iPos);
		return false;
	}

	rCB.GetCLUDrawBase()->SetMouseMode(iMode);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Get Mouse Mode Function

bool  GetMouseModeFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());
	TCVCounter iMode;

	if (iVarCount != 0)
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	rCB.GetCLUDrawBase()->GetMouseMode(iMode);
	rVar = iMode;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Mouse Sensitivity Function

bool  SetMouseSensFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());
	TCVScalar dRot, dTrans;

	if (iVarCount != 2)
	{
		rCB.GetErrorList().WrongNoOfParams(2, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToScalar(dRot))
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToScalar(dTrans))
	{
		rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}

	rCB.GetCLUDrawBase()->SetRotFac(float(dRot));
	rCB.GetCLUDrawBase()->SetTransFac(float(dTrans));

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Get Mouse Sensitivity Function

bool  GetMouseSensFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());
	TCVScalar dRot, dTrans;

	if (iVarCount != 0)
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	dRot   = (float) rCB.GetCLUDrawBase()->GetRotFac();
	dTrans = (float) rCB.GetCLUDrawBase()->GetTransFac();

	rVar.New(PDT_VARLIST);
	TVarList& rList = *rVar.GetVarListPtr();

	rList.Set(2);
	rList[0] = dRot;
	rList[1] = dTrans;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Mouse Value
///
/// Parameters:
///		1. Mouse Mode
///		2. Mouse button, 1: left, 2:right
///		3. List or Vector of three entries.
///
/// Return:
///		Nothing.
///
bool  SetMouseValueFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int i, iVarCount = int(mVars.Count());
	TCVCounter iMode, iBut;
	TCVScalar pdVal[3];

	if (iVarCount != 3)
	{
		rCB.GetErrorList().WrongNoOfParams(3, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToCounter(iMode))
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToCounter(iBut))
	{
		rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}

	if ((iMode < 1) || (iMode > 9))
	{
		rCB.GetErrorList().GeneralError("Mouse mode has to be in range {1,...,9}.", iLine, iPos);
		return false;
	}

	if ((iBut < 1) || (iBut > 2))
	{
		rCB.GetErrorList().GeneralError("Button ID has to be either 1 (left) or 2 (right).", iLine, iPos);
		return false;
	}

	if (mVars(2).BaseType() == PDT_VARLIST)
	{
		TVarList& rList = *mVars(2).GetVarListPtr();

		if (rList.Count() != 3)
		{
			rCB.GetErrorList().GeneralError("Variable list has to contain three values.", iLine, iPos);
			return false;
		}

		for (i = 0; i < 3; i++)
		{
			CCodeVar& rVar = rList(i);

			if (!rVar.CastToScalar(pdVal[i]))
			{
				rCB.GetErrorList().GeneralError("Value in mouse value list has to be a scalar.", iLine, iPos);
				return false;
			}
		}
	}
	else if (mVars(2).BaseType() == PDT_MULTIV)
	{
		TMultiV vB;
		TMultiV& vA = *mVars(2).GetMultiVPtr();

		if (!rCB.CastMVtoE3(vA, vB))
		{
			rCB.GetErrorList().GeneralError("Cannot extract values from multivector.", iLine, iPos);
			return false;
		}

		for (i = 0; i < 3; i++)
		{
			pdVal[i] = vB[i + 1];
		}
	}
	else
	{
		rCB.GetErrorList().GeneralError("Mouse value has to be given by a list or a multivector.", iLine, iPos);
		return false;
	}

	if (iBut == 1)
	{
		for (i = 0; i < 3; i++)
		{
			rCB.GetTransform()[iMode].pfRot[i] = GLfloat(pdVal[i] / rCB.GetRadPerDeg());
		}
	}
	else
	{
		for (i = 0; i < 3; i++)
		{
			rCB.GetTransform()[iMode].pfTrans[i] = GLfloat(pdVal[i]);
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Get Mouse Value

bool  GetMouseValueFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	int i;
	TCVCounter iMode, iBut;

	if (iVarCount != 2)
	{
		rCB.GetErrorList().WrongNoOfParams(2, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToCounter(iMode))
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToCounter(iBut))
	{
		rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}

	if ((iMode < 1) || (iMode > 9))
	{
		rCB.GetErrorList().GeneralError("Mouse mode has to be in range {1,...,9}.", iLine, iPos);
		return false;
	}

	if ((iBut < 1) || (iBut > 2))
	{
		rCB.GetErrorList().GeneralError("Button ID has to be either 1 (left) or 2 (right).", iLine, iPos);
		return false;
	}

	rVar.New(PDT_VARLIST);
	TVarList& rList = *rVar.GetVarListPtr();

	rList.Set(3);

	if (iBut == 1)
	{
		for (i = 0; i < 3; i++)
		{
			rList[i] = TCVScalar(rCB.GetTransform()[iMode].pfRot[i] * rCB.GetRadPerDeg());
		}
	}
	else
	{
		for (i = 0; i < 3; i++)
		{
			rList[i] = TCVScalar(rCB.GetTransform()[iMode].pfTrans[i]);
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Get Mouse Value FUNCTION
//

bool  MouseFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList* pmVars = rPars.GetVarListPtr();
	int iVarCount    = int(pmVars->Count());

	if (iVarCount == 3)
	{
		int piVal[3];
		int i;

		for (i = 0; i < 3; i++)
		{
			CCodeVar& rVar = (*pmVars)(i);

			if (!rVar.CastToCounter(piVal[i]))
			{
				rCB.GetErrorList().InvalidParType(rVar, i + 1, iLine, iPos);
				return false;
			}
		}

		if (!Mouse(rCB, rVar, piVal, iLine, iPos))
		{
			return false;
		}
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(3, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Get Mouse Value
//
// piVal[0] : Key No.
// piVal[1] : Mouse button 1:left, 2:right
// piVal[2] : Movement axis: 1, 2, 3.

bool  Mouse(CCLUCodeBase& rCB, CCodeVar& rLVar, int* piVal, int iLine, int iPos)
{
	if (!piVal)
	{
		return false;
	}

	if (!rLVar.New(PDT_SCALAR, "Constant"))
	{
		rCB.GetErrorList().Internal(iLine, iPos);
		return false;
	}

	CStrMem csStr;

	if ((piVal[0] < 1) || (piVal[0] > 9))
	{
		CStrMem csVal;
		csVal = piVal[0];
		rCB.GetErrorList().InvalidParVal(csVal.Str(), 1, iLine, iPos);
		return false;
	}

	if ((piVal[1] < 1) || (piVal[1] > 2))
	{
		CStrMem csVal;
		csVal = piVal[1];
		rCB.GetErrorList().InvalidParVal(csVal.Str(), 2, iLine, iPos);
		return false;
	}

	if ((piVal[2] < 1) || (piVal[2] > 3))
	{
		CStrMem csVal;
		csVal = piVal[2];
		rCB.GetErrorList().InvalidParVal(csVal.Str(), 3, iLine, iPos);
		return false;
	}

	//TCVScalar fVal;

	if (piVal[1] == 1)
	{
		rLVar = ((TCVScalar) rCB.GetTransform()[piVal[0]].pfRot[piVal[2] - 1]) * rCB.GetRadPerDeg();
	}
	else
	{
		rLVar = ((TCVScalar) rCB.GetTransform()[piVal[0]].pfTrans[piVal[2] - 1]);
	}

	return true;
}
