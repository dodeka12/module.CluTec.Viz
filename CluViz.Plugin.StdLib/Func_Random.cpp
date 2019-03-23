////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_Random.cpp
//
// summary:   Implements the function random class
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

#include "Func_Random.h"

//////////////////////////////////////////////////////////////////////
/// Random FUNCTION
///
/// returns random value in the range of 0 and 1.

bool  RandomFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = int(mVars.Count());
	
	if (iVarCount == 0)
	{		
		rVar = TCVScalar(rCB.GetRandom().ran3());
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(0, iLine, iPos);
		return false;
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Gauss FUNCTION
///
/// returns gaussian distribution about mean and with given variance.

bool  GaussFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = int(mVars.Count());
	
	if (iVarCount == 0)
	{		
		rVar = TCVScalar(rCB.GetRandom().Gauss());
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(0, iLine, iPos);
		return false;
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Gauss Parameter FUNCTION
///
/// sets gaussian parameters mean and variance.

bool  GaussParametersFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = int(mVars.Count());

	if (iVarCount == 2)
	{		
		TCVScalar fMean, fVar;

		if (!mVars(0).CastToScalar(fMean, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}								

		if (!mVars(1).CastToScalar(fVar, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 1, iLine, iPos);
			return false;
		}								

		rCB.GetRandom().GaussMean(fMean);
		rCB.GetRandom().GaussDev(fVar);

		// Call Gauss twice, so that new setting can take effect
		rCB.GetRandom().Gauss();
		rCB.GetRandom().Gauss();
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(0, iLine, iPos);
		return false;
	}
	
	return true;
}



//////////////////////////////////////////////////////////////////////
/// Random Seed FUNCTION
///
/// Sets seed for random number generator. For negative numbers uses time.

bool  RandomSeedFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = int(mVars.Count());
	int iVal;

	if (iVarCount == 1)
	{		
		if (!mVars(0).CastToCounter(iVal))
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}								

		if (iVal < 0)
		{
			time_t TimeVal;
			time(&TimeVal);
			rCB.GetRandom().seed3(lint(TimeVal));
			rVar = int(TimeVal);
		}
		else
		{
			rCB.GetRandom().seed3(iVal);
			rVar = iVal;
		}
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(0, iLine, iPos);
		return false;
	}
	
	return true;
}

