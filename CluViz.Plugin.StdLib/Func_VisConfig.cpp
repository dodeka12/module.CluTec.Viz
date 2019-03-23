////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_VisConfig.cpp
//
// summary:   Implements the function vis configuration class
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
#include "Func_VisConfig.h"

//////////////////////////////////////////////////////////////////////
/// Set Smooth Point Function

bool EnableMultisampleFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());
	TCVCounter iVal;

	if (iVarCount == 1)
	{
		if (!mVars(0).CastToCounter(iVal))
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		rCB.GetOGLDrawBase()->EnableMultisample((iVal ? true : false));
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Enable Transparency Display

bool EnableTransparencyFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());
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

	rCB.GetCLUDrawBase()->EnableTransparency((iVal != 0));

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Enable Color Stereo Display

bool EnableColorStereoFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());
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

	rCB.GetCLUDrawBase()->EnableColorStereo((iVal != 0));

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Enable Stereo Color Color Masks
///
/// Pars:
///		1. List of four bools for left color mask
///		2. List of four bools for right color mask

bool SetColorStereoMaskFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount != 2)
	{
		rCB.GetErrorList().WrongNoOfParams(2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_VARLIST)
	{
		rCB.GetErrorList().GeneralError("Expect as first parameter a list of four bools giving left color mask.", iLine, iPos);
		return false;
	}

	TVarList& rMaskLeft = *mVars(0).GetVarListPtr();
	if (rMaskLeft.Count() != 4)
	{
		rCB.GetErrorList().GeneralError("Left color mask has to have four entries.", iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_VARLIST)
	{
		rCB.GetErrorList().GeneralError("Expect as second parameter a list of four bools giving right color mask.", iLine, iPos);
		return false;
	}

	TVarList& rMaskRight = *mVars(1).GetVarListPtr();
	if (rMaskRight.Count() != 4)
	{
		rCB.GetErrorList().GeneralError("Right color mask has to have four entries.", iLine, iPos);
		return false;
	}

	int i, piLeft[4], piRight[4];
	for (i = 0; i < 4; ++i)
	{
		if (!rMaskLeft(i).CastToCounter(piLeft[i]))
		{
			rCB.GetErrorList().GeneralError("Invalid value in left color mask.", iLine, iPos);
			return false;
		}

		if (!rMaskRight(i).CastToCounter(piRight[i]))
		{
			rCB.GetErrorList().GeneralError("Invalid value in right color mask.", iLine, iPos);
			return false;
		}
	}

	rCB.GetCLUDrawBase()->SetColorStereoMask(piLeft, piRight);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Color Stereo Parameters
///
///	Pars:
///		1. Camera separation
///		2. Camera angle in radians

bool SetColorStereoParsFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());
	TCVScalar dSep, dAngle;

	if (iVarCount != 2)
	{
		rCB.GetErrorList().WrongNoOfParams(2, iLine, iPos);
		return false;
	}
	if (!mVars(0).CastToScalar(dSep))
	{
		rCB.GetErrorList().GeneralError("Expect as first parameter the camera separation");
		return false;
	}

	if (!mVars(1).CastToScalar(dAngle))
	{
		rCB.GetErrorList().GeneralError("Expect as second parameter the camera angle");
		return false;
	}

	rCB.GetCLUDrawBase()->SetColorStereoSep(float(dSep));
	rCB.GetCLUDrawBase()->SetColorStereoDegAngle(float(dAngle / rCB.GetRadPerDeg()));

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set Visualization Precision FUNCTION
//

bool SetVisPrecFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!rCB.GetFilter() || !rCB.GetOGLDrawBase())
	{
		return false;
	}

	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());
	double dPrec;
	TCVCounter iPow;

	if (iVarCount != 1)
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToCounter(iPow))
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if ((iPow < 0) || (iPow > 12))
	{
		rCB.GetErrorList().GeneralError("Value for precision has to be in the range {0,...,12}", iLine, iPos);
		return false;
	}

	dPrec = pow(0.1, double(iPow));
	rCB.GetFilter()->SetSensitivity(dPrec);
	rCB.GetOGLDrawBase()->SetSensitivity(dPrec);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set RTView Mode (flight mode)
// This functions exptects a single boolean parameter
//
bool SetRTViewModeFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount != 1)
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	int iVar = 0;
	if (!mVars(0).CastToCounter(iVar))
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
	}

	rCB.SetRTViewMode(iVar != 0 ? true : false);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set RTView Mode (flight mode) rotation axis inversion
// This functions exptects 2 boolean parameter (x and y axis)
bool SetRTViewRotationAxisInversionFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount != 2)
	{
		rCB.GetErrorList().WrongNoOfParams(2, iLine, iPos);
		return false;
	}

	int iVar1 = 0;
	int iVar2 = 0;
	if (!mVars(0).CastToCounter(iVar1))
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
	}

	if (!mVars(1).CastToCounter(iVar2))
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
	}

	rCB.SetRotationAxisInversion(iVar1 != 0 ? true : false, iVar2 != 0 ? true : false);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set RTView Look at
// This functions exptects:
// - A Center to look at
// - (Optional) A Bounding radius
bool SetRTViewLookAtFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if ((iVarCount != 3) && (iVarCount != 4))
	{
		rCB.GetErrorList().WrongNoOfParams(4, iLine, iPos);
		return false;
	}

	TCVScalar fX;
	TCVScalar fY;
	TCVScalar fZ;
	TCVScalar fRadius;

	if (!mVars(0).CastToScalar(fX))
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
	}

	if (!mVars(1).CastToScalar(fY))
	{
		rCB.GetErrorList().InvalidParType(mVars(1), 1, iLine, iPos);
	}

	if (!mVars(2).CastToScalar(fZ))
	{
		rCB.GetErrorList().InvalidParType(mVars(2), 1, iLine, iPos);
	}

	if (iVarCount == 3)
	{
		rCB.SetRTViewLookAt((float) fX, (float) fY, (float) fZ);
		return true;
	}

	if (!mVars(3).CastToScalar(fRadius))
	{
		rCB.GetErrorList().InvalidParType(mVars(3), 1, iLine, iPos);
	}

	rCB.SetRTViewLookAt((float) fX, (float) fY, (float) fZ, (float) fRadius);

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 	Sets view camera position
/// </summary>
///
/// <param name="rCB">   [in,out] The cb. </param>
/// <param name="rVar">  [in,out] The variable. </param>
/// <param name="rPars"> [in,out] The pars. </param>
/// <param name="iLine"> Zero-based index of the line. </param>
/// <param name="iPos">  Zero-based index of the position. </param>
///
/// <returns> True if it succeeds, false if it fails. </returns>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool SetRTPositionFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount != 3)
	{
		rCB.GetErrorList().WrongNoOfParams(4, iLine, iPos);
		return false;
	}

	TCVScalar fX;
	TCVScalar fY;
	TCVScalar fZ;

	if (!mVars(0).CastToScalar(fX))
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
	}

	if (!mVars(1).CastToScalar(fY))
	{
		rCB.GetErrorList().InvalidParType(mVars(1), 1, iLine, iPos);
	}

	if (!mVars(2).CastToScalar(fZ))
	{
		rCB.GetErrorList().InvalidParType(mVars(2), 1, iLine, iPos);
	}

	rCB.SetRTViewPosition((float) fX, (float) fY, (float) fZ);

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 	Resets the View to the inital state
/// </summary>
///
/// <param name="rCB">   [in,out] The cb. </param>
/// <param name="rVar">  [in,out] The variable. </param>
/// <param name="rPars"> [in,out] The pars. </param>
/// <param name="iLine"> Zero-based index of the line. </param>
/// <param name="iPos">  Zero-based index of the position. </param>
///
/// <returns> True if it succeeds, false if it fails. </returns>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ResetViewFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount != 0)
	{
		rCB.GetErrorList().WrongNoOfParams(0, iLine, iPos);
		return false;
	}

	rCB.ResetView();

	return true;
}
