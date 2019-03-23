////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_Capture.cpp
//
// summary:   Implements the function capture class
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
#include "Func_Capture.h"
#include "CluTec.Viz.Draw\OGLReadBitmap.h"

//////////////////////////////////////////////////////////////////////
// Create Capture Object Function

bool CaptureFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
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
		rCB.GetErrorList().GeneralError("Expect name of capture object as parameter.", iLine, iPos);
		return false;
	}

	COGLReadBitmap* pCapture = new COGLReadBitmap;

	if (!pCapture)
	{
		rCB.GetErrorList().GeneralError("Out of memory while creating capture object.", iLine, iPos);
		return false;
	}

	pCapture->SetName(mVars(0).GetStringPtr()->Str());
	COGLBEReference refCapture = rCB.GetOGLDrawBase()->GetSceneRepository()->New(pCapture);

	rVar = refCapture;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// Enable Capture

bool EnableCaptureFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
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
		rCB.GetErrorList().GeneralError("Expect a capture object variable as parameter.", iLine, iPos);
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
		rCB.GetErrorList().GeneralError("Capture object is not valid.", iLine, iPos);
		return false;
	}

	COGLReadBitmap* pCapture = dynamic_cast< COGLReadBitmap* >((COGLBaseElement*) Scene);
	if (!pCapture)
	{
		rCB.GetErrorList().GeneralError("Capture object is not valid.", iLine, iPos);
		return false;
	}

	if (iEnable)
	{
		pCapture->Enable(true);
	}
	else
	{
		pCapture->Enable(false);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set Capture Object Target

bool SetCaptureTargetFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
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
		rCB.GetErrorList().GeneralError("Expect a capture object variable as parameter.", iLine, iPos);
		return false;
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Capture object is not valid.", iLine, iPos);
		return false;
	}

	COGLReadBitmap* pCapture = dynamic_cast< COGLReadBitmap* >((COGLBaseElement*) Scene);
	if (!pCapture)
	{
		rCB.GetErrorList().GeneralError("Capture object is not valid.", iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() == PDT_SCENE)
	{
		// Should be texture object
		TScene& rScene = *mVars(1).GetScenePtr();

		if (!rScene.IsValid())
		{
			rCB.GetErrorList().GeneralError("Given texture variable is invalid.", iLine, iPos);
			return false;
		}

		COGLTexture* pTex = dynamic_cast< COGLTexture* >((COGLBaseElement*) rScene);
		if (!pTex)
		{
			rCB.GetErrorList().GeneralError("Given texture variable is invalid.", iLine, iPos);
			return false;
		}

		pCapture->SetTarget(COGLReadBitmap::READ_TO_TEXTURE);
		pCapture->SetTexture(rScene);
	}
	else if (mVars(1).BaseType() == PDT_IMAGE)
	{
		// Should be image object
		TImage& rImage = *mVars(1).GetImagePtr();

		if (!rImage.IsValid())
		{
			rCB.GetErrorList().GeneralError("Given image is invalid.", iLine, iPos);
			return false;
		}

		pCapture->SetTarget(COGLReadBitmap::READ_TO_IMAGE);
		pCapture->SetImage(rImage);
	}
	else
	{
		rCB.GetErrorList().GeneralError("Expect as second parameter either an image or a texture object.", iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set Capture Object Source coordinates

bool SetCaptureAreaFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount != 7)
	{
		int piPar[] = { 7 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a capture object variable as parameter.", iLine, iPos);
		return false;
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Capture object is not valid.", iLine, iPos);
		return false;
	}

	COGLReadBitmap* pCapture = dynamic_cast< COGLReadBitmap* >((COGLBaseElement*) Scene);
	if (!pCapture)
	{
		rCB.GetErrorList().GeneralError("Shader is not valid.", iLine, iPos);
		return false;
	}

	TCVScalar pdVal[6];
	int i;

	for (i = 0; i < 6; ++i)
	{
		if (!mVars(i + 1).CastToScalar(pdVal[i], rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Parameter is not a scalar.", iLine, iPos);
			return false;
		}
	}

	pCapture->SetArea(pdVal[0], pdVal[1], pdVal[2], pdVal[3], pdVal[4], pdVal[5]);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set Capture Object Target coordinates

bool SetCaptureTargetOriginFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount != 3)
	{
		int piPar[] = { 3 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a capture object variable as parameter.", iLine, iPos);
		return false;
	}

	COGLBEReference Scene = *mVars(0).GetScenePtr();

	if (!Scene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Capture object is not valid.", iLine, iPos);
		return false;
	}

	COGLReadBitmap* pCapture = dynamic_cast< COGLReadBitmap* >((COGLBaseElement*) Scene);
	if (!pCapture)
	{
		rCB.GetErrorList().GeneralError("Capture object is not valid.", iLine, iPos);
		return false;
	}

	TCVCounter piVal[2];
	int i;

	for (i = 0; i < 2; ++i)
	{
		if (!mVars(i + 1).CastToCounter(piVal[i], true))
		{
			rCB.GetErrorList().GeneralError("Parameter is not a counter.", iLine, iPos);
			return false;
		}
	}

	pCapture->SetTargetOrigin(piVal[0], piVal[1]);

	return true;
}
