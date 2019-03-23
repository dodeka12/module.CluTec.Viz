////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_RenderTarget.cpp
//
// summary:   Implements the function render target class
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
#include "Func_RenderTarget.h"
#include "CluTec.Viz.Draw\OGLRenderTarget.h"

//////////////////////////////////////////////////////////////////////
// Create Capture Object Function

bool RenderTargetFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount != 1)
	{
		int piPar[] = { 1 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("Expect name of render target object as parameter.", iLine, iPos);
		return false;
	}

	COGLRenderTarget* pRT = new COGLRenderTarget;

	if (!pRT)
	{
		rCB.GetErrorList().GeneralError("Out of memory while creating render target object.", iLine, iPos);
		return false;
	}

	pRT->SetName(mVars(0).GetStringPtr()->Str());
	COGLBEReference refRT = rCB.GetOGLDrawBase()->GetSceneRepository()->New(pRT);

	rVar = refRT;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// Enable Capture

bool EnableRenderTargetFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount      = int(mVars.Count());
	TCVCounter iEnable = 1;

	if ((iVarCount < 1) || (iVarCount > 2))
	{
		int piPar[] = { 1, 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a render target object variable as parameter.", iLine, iPos);
		return false;
	}

	if (iVarCount > 1)
	{
		if (!mVars(1).CastToCounter(iEnable))
		{
			rCB.GetErrorList().GeneralError("Expect true or false as second parameter.", iLine, iPos);
			return false;
		}
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Render target object is not valid.", iLine, iPos);
		return false;
	}

	COGLRenderTarget* pRT = dynamic_cast< COGLRenderTarget* >((COGLBaseElement*) Scene);
	if (!pRT)
	{
		rCB.GetErrorList().GeneralError("Render target object is not valid.", iLine, iPos);
		return false;
	}

	if (iEnable)
	{
		pRT->Enable(true);
	}
	else
	{
		pRT->Enable(false);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// Enable single capture to file

bool EnableRenderTargetSnapshotFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	//TCVCounter iEnable = 1;

	if (iVarCount != 3)
	{
		int piPar[] = { 3 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a render target object variable as parameter.", iLine, iPos);
		return false;
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Render target object is not valid.", iLine, iPos);
		return false;
	}

	COGLRenderTarget* pRT = dynamic_cast<COGLRenderTarget*>((COGLBaseElement*) Scene);
	if (!pRT)
	{
		rCB.GetErrorList().GeneralError("Render target object is not valid.", iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a render scene as second parameter.", iLine, iPos);
		return false;
	}

	COGLBEReference refRenderScene = *mVars(1).GetScenePtr();

	if (!refRenderScene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Render scene is not valid.", iLine, iPos);
		return false;
	}

	COGLScene* pRenderScene = dynamic_cast<COGLScene*>((COGLBaseElement*) refRenderScene);
	if (!pRenderScene)
	{
		rCB.GetErrorList().GeneralError("Render scene is not valid.", iLine, iPos);
		return false;
	}

	if (mVars(2).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("Expect second parameter to give the filename where to store the rendered image.", iLine, iPos);
		return false;
	}

	TString csFilename = *mVars(2).GetStringPtr();

	pRT->EnableSingleRenderToFile(refRenderScene, csFilename.Str());

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set Capture Object Source coordinates

bool SetRenderTargetSizeFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if ((iVarCount != 3) && (iVarCount != 4))
	{
		int piPar[] = { 3, 4 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a render target object variable as parameter.", iLine, iPos);
		return false;
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Render target object is not valid.", iLine, iPos);
		return false;
	}

	COGLRenderTarget* pRT = dynamic_cast< COGLRenderTarget* >((COGLBaseElement*) Scene);
	if (!pRT)
	{
		rCB.GetErrorList().GeneralError("Render target object is not valid.", iLine, iPos);
		return false;
	}

	const int iValueCount         = 3;
	TCVCounter piVal[iValueCount] = { 0, 0, 0 };
	int i;

	for (i = 0; i < iVarCount - 1; ++i)
	{
		if (!mVars(i + 1).CastToCounter(piVal[i]))
		{
			rCB.GetErrorList().GeneralError("Parameter is not a counter.", iLine, iPos);
			return false;
		}
	}

	try
	{
		pRT->Create(unsigned(piVal[0]), unsigned(piVal[1]), unsigned(piVal[2]), rCB.GetOGLDrawBase()->GetSceneRepository());
	}
	catch (std::exception& xEx)
	{
		rCB.GetErrorList().GeneralError(xEx.what(), iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set Capture Object Target coordinates

bool GetRenderTargetImageFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount != 2)
	{
		int piPar[] = { 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a render target object variable as first parameter.", iLine, iPos);
		return false;
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Render target object is not valid.", iLine, iPos);
		return false;
	}

	COGLRenderTarget* pRT = dynamic_cast< COGLRenderTarget* >((COGLBaseElement*) Scene);
	if (!pRT)
	{
		rCB.GetErrorList().GeneralError("Render target object is not valid.", iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_IMAGE)
	{
		rCB.GetErrorList().GeneralError("Expect an image object variable as second parameter.", iLine, iPos);
		return false;
	}

	TImage refImage = *mVars(1).GetImagePtr();

	try
	{
		pRT->GetImage(*((COGLImage*) refImage));
	}
	catch (std::exception& xEx)
	{
		rCB.GetErrorList().GeneralError(xEx.what(), iLine, iPos);
		return false;
	}

	return true;
}
