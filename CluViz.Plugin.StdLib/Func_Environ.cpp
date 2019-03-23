////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_Environ.cpp
//
// summary:   Implements the function environ class
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

#include "Func_Environ.h"

//////////////////////////////////////////////////////////////////////
/// Enable Events
///
/// Parameters:
///		1. Name of event
///		2. true/false for enable/disable

bool  EnableEventFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos )
{
	TVarList &mVars = *rPars.GetVarListPtr();
	int iVarCount = int(mVars.Count());
	TCVCounter iVal;
	CStrMem sEvent;

	if (iVarCount != 2)
	{
		rCB.GetErrorList().WrongNoOfParams(2, iLine, iPos);
		return false;
	}

	if ( mVars(0).BaseType() != PDT_STRING )
	{
		rCB.GetErrorList().GeneralError( "Expect as first parameter name of event.", iLine, iPos );
		return false;
	}

	sEvent = *mVars(0).GetStringPtr();

	if (!mVars(1).CastToCounter(iVal))
	{
		rCB.GetErrorList().GeneralError( "Second parameter has to be true/false.", iLine, iPos);
		return false;
	}

	if ( sEvent == "control_keys" )
	{
		rCB.GetCLUDrawBase()->EnableSendControlKeyEvents( (iVal != 0) );
	}
	else if ( sEvent == "function_keys" )
	{
		rCB.GetCLUDrawBase()->EnableSendFunctionKeyEvents( (iVal != 0) );
	}
	else
	{
		rCB.GetErrorList().GeneralError( "Unknown event name.", iLine, iPos );
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Get Time

bool  GetTimeFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList &mVars = *rPars.GetVarListPtr();
	int iVarCount = int(mVars.Count());

	if (iVarCount != 0)
	{
		rCB.GetErrorList().WrongNoOfParams(0, iLine, iPos);
		return false;
	}

	//TTimeStruct tmCur;
	//GETTIME(&tmCur);
	//TCVScalar dTime;

	//dTime = TCVScalar(tmCur.time - rCB.GetStartTime().time) 
	//				+ TCVScalar(1e-3) * TCVScalar(tmCur.millitm - rCB.GetStartTime().millitm);

	double dTime = 1e-3 * (rCB.GetCLUDrawBase()->GetTime() - rCB.GetCLUDrawBase()->GetTimeStart());

	rVar = (TCVScalar) dTime;

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Clear Script List

bool ClearScriptListFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = int(mVars.Count());

	if (iVarCount != 0)
	{
		rCB.GetErrorList().WrongNoOfParams(0, iLine, iPos);
		return false;
	}

	rCB.ClearScriptList();

	return true;
} 



//////////////////////////////////////////////////////////////////////
/// Add Script To List

bool AddScriptToListFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = int(mVars.Count());
	
	if (iVarCount == 2)
	{
		CCodeVar &rVar1 = mVars(0);
		if (rVar1.BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().InvalidParVal(rVar1, 1, iLine, iPos);
			return false;
		}
		CCodeVar &rVar2 = mVars(1);
		if (rVar2.BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().InvalidParVal(rVar2, 2, iLine, iPos);
			return false;
		}
	
		string sFilename, sTitle;
		sFilename = rCB.GetScriptPath() + rVar2.ValStr().Str();
		sTitle = rVar1.ValStr().Str();

		rCB.AddScriptToList(sTitle, sFilename);
	} 
	else
	{
		rCB.GetErrorList().WrongNoOfParams(2, iLine, iPos);
		return false;
	}
	
	return true;
} 


//////////////////////////////////////////////////////////////////////
// Set Animation Time Step FUNCTION
//

bool SetAnimationTimeStepFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList &mVars = *rPars.GetVarListPtr();
	int iVarCount = int(mVars.Count());
	TCVCounter iTimeStep;

	if (iVarCount != 1)
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToCounter(iTimeStep))
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (iTimeStep <= 0)
	{
		rCB.GetErrorList().GeneralError("Timestep value has to be greater than zero.", iLine, iPos);
		return false;
	}

	rCB.GetCLUDrawBase()->SetAnimationTimeStep(iTimeStep);
	
	return true;
}

//////////////////////////////////////////////////////////////////////
// Set Visualization Time Step FUNCTION
//

bool SetVisualizationTimeStepFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList &mVars = *rPars.GetVarListPtr();
	int iVarCount = int(mVars.Count());
	TCVCounter iTimeStep;

	if (iVarCount != 1)
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToCounter(iTimeStep))
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (iTimeStep < 5 || iTimeStep > 1000)
	{
		rCB.GetErrorList().GeneralError("Timestep value has to be in range [5, 1000].", iLine, iPos);
		return false;
	}

	rCB.GetCLUDrawBase()->SetVisualizationTimeStep(iTimeStep);
	
	return true;
}



//////////////////////////////////////////////////////////////////////
// Enable Animation FUNCTION
//

bool EnableAnimationFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList &mVars = *rPars.GetVarListPtr();
	int iVarCount = int(mVars.Count());
	TCVCounter iVal;

	if (iVarCount != 1)
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToCounter(iVal))
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	CCodeVar &rAVar = rCB.GetVar("_DoAnimate");
	if (rAVar.BaseType() == PDT_NOTYPE)
	{
		CCodeVar &rNewVar = rCB.NewVar("_DoAnimate", PDT_INT);
		rNewVar = iVal;
	}
	else
	{
		rAVar = iVal;
	}
	
	return true;
}


