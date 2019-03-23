////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_DrawConfig.cpp
//
// summary:   Implements the function draw configuration class
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
#include "Func_DrawConfig.h"

//////////////////////////////////////////////////////////////////////
// Set Sphere Detail Level

bool SetSphereDetailLevelFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVCounter iVal;

	if (iVarCount == 1)
	{
		if (!mVars(0).CastToCounter(iVal))
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if ((iVal < 1) || (iVal > 10))
		{
			rCB.GetErrorList().GeneralError("Detail level has to lie in range 1 to 10.", iLine, iPos);
			return false;
		}
	}
	else
	{
		int piPar[] = { 1 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (!rCB.GetOGLDrawBase()->SetSphereDetailLevel(iVal - 1))
	{
		rCB.GetErrorList().GeneralError("Invalid detail level.", iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set Cylinder Detail Level

bool SetCylinderDetailLevelFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVCounter iVal;

	if (iVarCount == 1)
	{
		if (!mVars(0).CastToCounter(iVal))
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if ((iVal < 3) || (iVal > 360))
		{
			rCB.GetErrorList().GeneralError("Detail level has to lie in range 3 to 360.", iLine, iPos);
			return false;
		}
	}
	else
	{
		int piPar[] = { 1 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (!rCB.GetOGLDrawBase()->SetCylinderDetailLevel(iVal))
	{
		rCB.GetErrorList().GeneralError("Invalid detail level.", iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Arrow Shape Function

bool SetArrowShapeFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!rCB.GetOGLDrawBase())
	{
		return false;
	}

	int iVar;
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVScalar dVal[3];

	if (iVarCount == 3)
	{
		for (iVar = 0; iVar < 3; iVar++)
		{
			if (!mVars(iVar).CastToScalar(dVal[iVar], rCB.GetSensitivity()))
			{
				rCB.GetErrorList().InvalidParType(mVars(iVar), iVar + 1, iLine, iPos);
				return false;
			}
		}

		if (dVal[0] <= 0)
		{
			rCB.GetErrorList().GeneralError("Arrow length has to be larger than zero.", iLine, iPos);
			return false;
		}

		if ((dVal[1] < 1) || (dVal[1] > 89))
		{
			rCB.GetErrorList().GeneralError("Arrow opening angle has to be in range [1,89] degress.", iLine, iPos);
			return false;
		}

		if (dVal[2] < 0.001)
		{
			rCB.GetErrorList().GeneralError("Arrow line radius has to be larger than 0.001.", iLine, iPos);
			return false;
		}

		rCB.GetOGLDrawBase()->SetArrowShape(
				float(dVal[0]),
				float(dVal[1]) * float(rCB.GetRadPerDeg()),
				float(dVal[2]));
	}
	else
	{
		int piPar[] = { 3 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set OGL Filter Mode FUNCTION

bool SetModeFunc(CCLUCodeBase& rCB, CCodeVar& rLVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!rCB.GetFilter())
	{
		rCB.GetErrorList().Internal(iLine, iPos);
		return false;
	}

	TVarList* pmVars = rPars.GetVarListPtr();
	int iVarCount	 = int(pmVars->Count());

	int iVal;
	int i;

	for (i = 0; i < iVarCount; i++)
	{
		switch ((*pmVars)(i).BaseType())
		{
		    case PDT_INT:
			    iVal = (int) *((int*) ((*pmVars)(i).GetIntPtr()));
			    break;

		    case PDT_UINT:
			    iVal = (int) *((uint*) ((*pmVars)(i).GetUIntPtr()));
			    break;

		    case PDT_LONG:
			    iVal = (int) *((long*) ((*pmVars)(i).GetLongPtr()));
			    break;

		    default:
			    rCB.GetErrorList().InvalidType((*pmVars)(i), iLine, iPos);
			    return false;
		}

		rCB.GetFilter()->SetMode(iVal);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Plot Mode FUNCTION

bool SetPlotModeFunc(CCLUCodeBase& rCB, CCodeVar& rLVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList* pmVars = rPars.GetVarListPtr();
	int iVarCount	 = int(pmVars->Count());

	int iVal;
	int i;

	for (i = 0; i < iVarCount; i++)
	{
		if (!(*pmVars)(i).CastToCounter(iVal))
		{
			rCB.GetErrorList().InvalidType((*pmVars)(i), iLine, iPos);
			return false;
		}

		rCB.SetPlotMode(iVal);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Line Width Function

bool SetLineWidthFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount	= int(mVars.Count());
	TCVScalar dVal;

	if (iVarCount == 1)
	{
		if (!mVars(0).CastToScalar(dVal, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (dVal > 0.0)
		{
			rCB.GetOGLDrawBase()->SetLineWidth(float(dVal));
		}
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Point Size Function

bool SetPointSizeFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount	= int(mVars.Count());
	TCVScalar dSize, dSizeMin, dSizeMax;
	TCVScalar dFadeSize, dConst, dLin, dQuad;

	if (iVarCount == 1)
	{
		if (!mVars(0).CastToScalar(dSize, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("First parameter has to give size of point.", iLine, iPos);
			return false;
		}

		if (dSize > 0.0)
		{
			rCB.GetOGLDrawBase()->SetPointSize(float(dSize));
		}
	}
	else if (iVarCount == 7)
	{
		if (!mVars(0).CastToScalar(dSize, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("First parameter has to give size of point.", iLine, iPos);
			return false;
		}

		if (!mVars(1).CastToScalar(dSizeMin, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Second parameter has to give minimal size of point.", iLine, iPos);
			return false;
		}

		if (!mVars(2).CastToScalar(dSizeMax, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Third parameter has to give maximal size of point.", iLine, iPos);
			return false;
		}

		if (!mVars(3).CastToScalar(dFadeSize, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Fourth parameter has to give fading size of point.", iLine, iPos);
			return false;
		}

		if (!mVars(4).CastToScalar(dConst, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Fifth parameter has to give constant attenuation of point.", iLine, iPos);
			return false;
		}

		if (!mVars(5).CastToScalar(dLin, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Sixth parameter has to give size linear attenuation of point.", iLine, iPos);
			return false;
		}

		if (!mVars(6).CastToScalar(dQuad, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Seventh parameter has to give quadratic attenuation of point.", iLine, iPos);
			return false;
		}

		if (dSize > 0.0)
		{
			rCB.GetOGLDrawBase()->SetPointSize(float(dSize), float(dSizeMin), float(dSizeMax), float(dFadeSize),
					float(dConst), float(dLin), float(dQuad));
		}
	}
	else
	{
		int piPar[] = { 1, 7 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Enable Point Sprites

bool EnablePointSpritesFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount	= int(mVars.Count());
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

	rCB.GetOGLDrawBase()->EnablePointSprites((iVal != 0));

	return true;
}
