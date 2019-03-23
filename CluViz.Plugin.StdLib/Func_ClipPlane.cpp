////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_ClipPlane.cpp
//
// summary:   Implements the function clip plane class
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
#include "Func_ClipPlane.h"

#include "CluTec.Viz.Draw\OGLClipPlane.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ClipPlaneFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
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
		rCB.GetErrorList().GeneralError("Expect name of clip plane as first parameter.", iLine, iPos);
		return false;
	}

	TCVCounter iGlId = 0;
	if (!mVars(1).CastToCounter(iGlId))
	{
		rCB.GetErrorList().GeneralError("Expect as second parameter the clip plane id.", iLine, iPos);
		return false;
	}

	COGLClipPlane* pClip = new COGLClipPlane();

	if (!pClip)
	{
		rCB.GetErrorList().GeneralError("Out of memory while creating clip plane variable.", iLine, iPos);
		return false;
	}

	// Get the number of remaining clip planes, since we need 4 clip planes for picking.
	// OpenGL guarantees 6 clip planes so this should be a positive integer.
	unsigned uClipPlaneCount = pClip->GetClipPlaneCount() - 4;

	if ((iGlId < 1) || (iGlId > int(uClipPlaneCount)))
	{
		char pcText[200];
		sprintf_s(pcText, "Clip plane id has to lie in range 1 to %d.", uClipPlaneCount);
		rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
		return false;
	}

	// Decrement given GL id by one and add 4 since we start counting with 1 in CLUScript
	// and the first 4 clip planes are reserved.
	iGlId += 3;

	pClip->SetName(mVars(0).GetStringPtr()->Str());
	pClip->Enable(false);

	// We need the first four clip planes for picking.
	pClip->SetGlId(unsigned(iGlId));

	COGLBEReference SceneRef = rCB.GetOGLDrawBase()->GetSceneRepository()->New(pClip);

	rVar = SceneRef;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool SetClipPlaneFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	// ////////////////////////////////////////////////////////////////////////////////////////////////////
	// Check number of parameters
	if (iVarCount != 4)
	{
		int piPar[] = { 4 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	// ////////////////////////////////////////////////////////////////////////////////////////////////////
	// Check that first parameter is a clip plane and get the clip plane object
	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a clip plane variable as first parameter.", iLine, iPos);
		return false;
	}

	TScene& rScene = *mVars(0).GetScenePtr();

	if (!rScene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Given clip plane variable is invalid.", iLine, iPos);
		return false;
	}

	COGLClipPlane* pClip = dynamic_cast< COGLClipPlane* >((COGLBaseElement*) rScene);
	if (!pClip)
	{
		rCB.GetErrorList().GeneralError("Given clip plane variable is invalid.", iLine, iPos);
		return false;
	}

	// ////////////////////////////////////////////////////////////////////////////////////////////////////
	// Get the remaining parameters

	if (mVars(1).BaseType() != PDT_MULTIV)
	{
		rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}

	if (mVars(2).BaseType() != PDT_MULTIV)
	{
		rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
		return false;
	}

	if (mVars(3).BaseType() != PDT_MULTIV)
	{
		rCB.GetErrorList().InvalidParType(mVars(3), 4, iLine, iPos);
		return false;
	}

	TMultiV& vCenter = *mVars(1).GetMultiVPtr();
	TMultiV& vDirX   = *mVars(2).GetMultiVPtr();
	TMultiV& vDirY   = *mVars(3).GetMultiVPtr();
	TMultiV vCenterE3, vDirXE3, vDirYE3;
	COGLVertex xCenter, xDirX, xDirY;

	if (!rCB.CastMVtoE3(vCenter, vCenterE3))
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (!rCB.CastMVtoE3(vDirX, vDirXE3))
	{
		rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}

	if (!rCB.CastMVtoE3(vDirY, vDirYE3))
	{
		rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
		return false;
	}

	xCenter.Set((float) vCenterE3[E3GA < TCVScalar > ::iE1],
			(float) vCenterE3[E3GA < TCVScalar > ::iE2],
			(float) vCenterE3[E3GA < TCVScalar > ::iE3]);

	xDirX.Set((float) vDirXE3[E3GA < TCVScalar > ::iE1],
			(float) vDirXE3[E3GA < TCVScalar > ::iE2],
			(float) vDirXE3[E3GA < TCVScalar > ::iE3]);

	xDirY.Set((float) vDirYE3[E3GA < TCVScalar > ::iE1],
			(float) vDirYE3[E3GA < TCVScalar > ::iE2],
			(float) vDirYE3[E3GA < TCVScalar > ::iE3]);

	pClip->SetClipPlane(xCenter, xDirX, xDirY);

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool EnableClipPlaneFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	// ////////////////////////////////////////////////////////////////////////////////////////////////////
	// Check number of parameters
	if (iVarCount != 2)
	{
		int piPar[] = { 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	// ////////////////////////////////////////////////////////////////////////////////////////////////////
	// Check that first parameter is a clip plane and get the clip plane object
	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a clip plane variable as first parameter.", iLine, iPos);
		return false;
	}

	TScene& rScene = *mVars(0).GetScenePtr();

	if (!rScene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Given clip plane variable is invalid.", iLine, iPos);
		return false;
	}

	COGLClipPlane* pClip = dynamic_cast< COGLClipPlane* >((COGLBaseElement*) rScene);
	if (!pClip)
	{
		rCB.GetErrorList().GeneralError("Given clip plane variable is invalid.", iLine, iPos);
		return false;
	}

	// ////////////////////////////////////////////////////////////////////////////////////////////////////
	// Get the remaining parameters

	TCVCounter iEnable = 0;
	if (!mVars(1).CastToCounter(iEnable))
	{
		rCB.GetErrorList().GeneralError("Expect as second parameter true/false.", iLine, iPos);
		return false;
	}

	pClip->Enable((iEnable != 0 ? true : false));

	return true;
}
