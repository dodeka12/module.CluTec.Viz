////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_Peek.cpp
//
// summary:   Implements the function peek class
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
#include "Func_Peek.h"

//////////////////////////////////////////////////////////////////////
/// Create a Peek object
/// This object gets internal data of current draw
//
// Parameters:
//	1. Name of peek
//	2. Type of peek: "frames"

bool PeekFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!rCB.GetOGLDrawBase())
	{
		return false;
	}

	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	if (iVarCount != 2)
	{
		int piPar[] = { 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("First parameter has to give name of peek object.", iLine, iPos);
		return false;
	}

	TString sName = *mVars(0).GetStringPtr();

	if (mVars(1).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("Second parameter has to be a string giving the peek types."
						"This can also be a list of types separated by a '+'."
						"Currently available types are: \"frames\".", iLine, iPos);
		return false;
	}

	TString sType      = *mVars(1).GetStringPtr();
	unsigned int uType = 0;

	if (strstr(sType.Str(), "frames"))
	{
		uType |= COGLPeek::PT_FRAMES;
	}

	if (uType == 0)
	{
		rCB.GetErrorList().GeneralError("Second parameter has to be a string giving the peek types."
						"This can also be a list of types separated by a '+'."
						"Currently available types are: \"frames\".", iLine, iPos);
		return false;
	}

	COGLPeek* pPeek = new COGLPeek(sName.Str(), uType);
	if (!pPeek)
	{
		return false;
	}

	COGLBEReference PeekRef = rCB.GetOGLDrawBase()->GetSceneRepository()->New(pPeek);
	rVar = PeekRef;

	return true;
}

//////////////////////////////////////////////////////////////////////
// Get View Extent in local coordinates from peek frames
//
// Parameters:
//	1. Peek variable
//	2. (opt) Depth at which to estimate extent
//
// Returns:
//	List of four vectors giving bottom left, bottom right, top right, top left corners

bool GetPeekViewExtentLocalFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	if ((iVarCount != 1) && (iVarCount != 2))
	{
		int piPar[] = { 1, 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect as first parameter a peek variable.", iLine, iPos);
		return false;
	}

	COGLPeek* pPeek = dynamic_cast< COGLPeek* >((COGLBaseElement*) *mVars(0).GetScenePtr());
	if (!pPeek)
	{
		rCB.GetErrorList().GeneralError("Invalid peek object.", iLine, iPos);
		return false;
	}

	double dDepth = 0.0;
	if (iVarCount > 1)
	{
		if (!mVars(1).CastToScalar(dDepth, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Second parameter has to give depth at which extent is estimated.", iLine, iPos);
			return false;
		}
	}

	double pdBL[3], pdBR[3], pdTR[3], pdTL[3];
	pPeek->GetViewExtentInLocalCoords(pdBL, pdBR, pdTR, pdTL, dDepth);

	rVar.New(PDT_VARLIST);
	TVarList& rList = *rVar.GetVarListPtr();
	rList.Set(4);
	rList(0) = rCB.GetE3GABase().Vec(pdBL[0], pdBL[1], pdBL[2]);
	rList(1) = rCB.GetE3GABase().Vec(pdBR[0], pdBR[1], pdBR[2]);
	rList(2) = rCB.GetE3GABase().Vec(pdTR[0], pdTR[1], pdTR[2]);
	rList(3) = rCB.GetE3GABase().Vec(pdTL[0], pdTL[1], pdTL[2]);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Get View Extent in screen coordinates from peek frames
//
// Parameters:
//	1. Peek variable
//
// Returns:
//	List of two vectors giving bottom left and top right corners

bool GetPeekViewExtentScreenFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	if (iVarCount != 1)
	{
		int piPar[] = { 1 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect as first parameter a peek variable.", iLine, iPos);
		return false;
	}

	COGLPeek* pPeek = dynamic_cast< COGLPeek* >((COGLBaseElement*) *mVars(0).GetScenePtr());
	if (!pPeek)
	{
		rCB.GetErrorList().GeneralError("Invalid peek object.", iLine, iPos);
		return false;
	}

	//double dX1, dY1, dZ1, dX2, dY2, dZ2;
	int iX1, iY1, iX2, iY2;
	pPeek->GetViewExtentInScreenCoords(iX1, iY1, iX2, iY2);

	rVar.New(PDT_VARLIST);
	TVarList& rList = *rVar.GetVarListPtr();
	rList.Set(2);
	rList(0) = rCB.GetE3GABase().Vec(double(iX1), double(iY1), 0.0);
	rList(1) = rCB.GetE3GABase().Vec(double(iX2), double(iY2), 0.0);

	return true;
}
