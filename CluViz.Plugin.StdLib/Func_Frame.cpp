////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_Frame.cpp
//
// summary:   Implements the function frame class
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

#include "CluTec.Viz.Base\TensorOperators.h"
#include "CluTec.Viz.Draw\OGLAnimTranslation.h"
#include "CluTec.Viz.Draw\OGLAnimScale.h"

#include "Func_Frame.h"

//////////////////////////////////////////////////////////////////////
// Push Frame
//
// Parameter:
//		1. Frame type: "world" (default), "view", "texture", "all"
//
// Return:
//		none

bool PushFrameFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());
	TString csType  = "world";

	if (iVarCount > 1)
	{
		int piVal[] = { 0, 1 };
		rCB.GetErrorList().WrongNoOfParams(piVal, 2, iLine, iPos);
		return false;
	}

	COGLFrameStack* pFrame   = new COGLFrameStack();
	COGLBEReference FrameRef = rCB.GetOGLDrawBase()->GetSceneRepository()->New(pFrame);

	if (iVarCount >= 1)
	{
		if (mVars(0).BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().GeneralError("First parameter has to be type of frame that is to be pushed.", iLine, iPos);
			return false;
		}

		csType = *mVars(0).GetStringPtr();
		csType.ToLowerCase();
	}

	if (csType == "world")
	{
		pFrame->SetFrameMode(COGLFrameStack::MODELVIEW);
	}
	else if (csType == "view")
	{
		pFrame->SetFrameMode(COGLFrameStack::PROJECTION);
	}
	else if (csType == "texture")
	{
		pFrame->SetFrameMode(COGLFrameStack::TEXTURE);
	}
	else if (csType == "all")
	{
		pFrame->SetFrameMode(COGLFrameStack::ALL);
	}
	else
	{
		rCB.GetErrorList().GeneralError("Invalid frame type. It has to be one of: \"world\", \"view\", \"texture\", \"all\".", iLine, iPos);
		return false;
	}

	pFrame->DoPush();

	rCB.GetOGLDrawBase()->DrawBaseElement(FrameRef);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Pop Frame
//
// Parameter:
//		1. Frame type: "world" (default), "view", "texture", "all"
//
// Return:
//		none

bool PopFrameFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());
	TString csType  = "world";

	if (iVarCount > 1)
	{
		int piVal[] = { 0, 1 };
		rCB.GetErrorList().WrongNoOfParams(piVal, 2, iLine, iPos);
		return false;
	}

	COGLFrameStack* pFrame   = new COGLFrameStack();
	COGLBEReference FrameRef = rCB.GetOGLDrawBase()->GetSceneRepository()->New(pFrame);

	if (iVarCount >= 1)
	{
		if (mVars(0).BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().GeneralError("First parameter has to be type of frame that is to be pushed.", iLine, iPos);
			return false;
		}

		csType = *mVars(0).GetStringPtr();
	}

	if (csType == "world")
	{
		pFrame->SetFrameMode(COGLFrameStack::MODELVIEW);
	}
	else if (csType == "view")
	{
		pFrame->SetFrameMode(COGLFrameStack::PROJECTION);
	}
	else if (csType == "texture")
	{
		pFrame->SetFrameMode(COGLFrameStack::TEXTURE);
	}
	else if (csType == "all")
	{
		pFrame->SetFrameMode(COGLFrameStack::ALL);
	}
	else
	{
		rCB.GetErrorList().GeneralError("Invalid frame type. It has to be one of: \"world\", \"view\", \"texture\", \"all\".", iLine, iPos);
		return false;
	}

	pFrame->DoPop();

	rCB.GetOGLDrawBase()->DrawBaseElement(FrameRef);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Create Frame Element
//
// Parameter:
//		1. A name for the frame
//		2. (true (default)/false) whether matrix is multiplied with current matrix or set
//		3. ("world" (default), "view", "texture") denotes which frame mode this frame has
//
// Return:
//		none

bool FrameFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if ((iVarCount < 1) || (iVarCount > 3))
	{
		int piVal[] = { 1, 2, 3 };
		rCB.GetErrorList().WrongNoOfParams(piVal, 3, iLine, iPos);
		return false;
	}

	COGLFrame* pFrame        = new COGLFrame;
	COGLBEReference FrameRef = rCB.GetOGLDrawBase()->GetSceneRepository()->New(pFrame);

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("First parameter has to be name of frame.", iLine, iPos);
		return false;
	}

	pFrame->SetName(mVars(0).GetStringPtr()->Str());

	if (iVarCount >= 2)
	{
		TCVCounter iVal = 1;

		if (!mVars(1).CastToCounter(iVal))
		{
			rCB.GetErrorList().GeneralError("Expect as second optional parameter a boolean value indicating whether "
							"frame should be multiplied.", iLine, iPos);
			return false;
		}

		pFrame->EnableMultiplyMatrix((iVal ? true : false));
	}

	if (iVarCount >= 3)
	{
		if (mVars(2).BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().GeneralError("Expect third optional parameter to give the frame mode: \"world\" or \"view\" or \"texture\".", iLine, iPos);
			return false;
		}

		TString& rMode = *mVars(2).GetStringPtr();
		if (rMode == "world")
		{
			pFrame->SetFrameMode(COGLFrame::MODELVIEW);
		}
		else if (rMode == "view")
		{
			pFrame->SetFrameMode(COGLFrame::PROJECTION);
		}
		else if (rMode == "texture")
		{
			pFrame->SetFrameMode(COGLFrame::TEXTURE);
		}
		else
		{
			rCB.GetErrorList().GeneralError("Expect third optional parameter to give the frame mode: \"world\" or \"view\" or \"texture\".", iLine, iPos);
			return false;
		}
	}

	rVar = FrameRef;

	return true;
}

//////////////////////////////////////////////////////////////////////
// Reset Frame Element
//
// Parameter:
//		1. The frame to be reset
//
// Return:
//		none

bool ResetFrameFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount != 1)
	{
		int piVal[] = { 1 };
		rCB.GetErrorList().WrongNoOfParams(piVal, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect as first parameter a frame variable.", iLine, iPos);
		return false;
	}

	COGLFrame* pFrame = dynamic_cast< COGLFrame* >((COGLBaseElement*) *mVars(0).GetScenePtr());
	if (!pFrame)
	{
		rCB.GetErrorList().GeneralError("Invalid frame object.", iLine, iPos);
		return false;
	}

	pFrame->ResetContent();

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set Frame Element
//
// Parameter:
//		1. The frame to be set
//		2. The matrix/tensor the frame is to be set to
//
// Return:
//		none

bool SetFrameFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if ((iVarCount != 1) && (iVarCount != 2))
	{
		int piVal[] = { 1, 2 };
		rCB.GetErrorList().WrongNoOfParams(piVal, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect as first parameter a frame variable.", iLine, iPos);
		return false;
	}

	COGLFrame* pFrame = dynamic_cast< COGLFrame* >((COGLBaseElement*) *mVars(0).GetScenePtr());
	if (!pFrame)
	{
		rCB.GetErrorList().GeneralError("Invalid frame object.", iLine, iPos);
		return false;
	}

	if (iVarCount == 1)
	{
		pFrame->SetIdentity();
	}
	else
	{
		if (mVars(1).BaseType() == PDT_MATRIX)
		{
			pFrame->Set(*mVars(1).GetMatrixPtr());
		}
		else if (mVars(1).BaseType() == PDT_TENSOR)
		{
			pFrame->Set(*mVars(1).GetTensorPtr());
		}
		else if (mVars(1).BaseType() == PDT_TENSOR_IDX)
		{
			TTensor TDummy;
			TTensorIdx& rTIdx = *mVars(1).GetTensorIdxPtr();

			if (!MakeTensor(TDummy, rTIdx))
			{
				rCB.GetErrorList().GeneralError("Error generating tensor.", iLine, iPos);
				return false;
			}

			pFrame->Set(TDummy);
		}
		else
		{
			rCB.GetErrorList().GeneralError("Expect as second parameter a matrix or tensor.", iLine, iPos);
			return false;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Multiply Frame Element
//
// Parameter:
//		1. The frame or frame type to be multiplied
//		2. The matrix/tensor/Multivector the frame is to be multiplied with
//
// Return:
//		none

bool MultiplyFrameFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());
	int iVarOff     = 0;

	if ((iVarCount != 1) && (iVarCount != 2))
	{
		int piVal[] = { 1, 2 };
		rCB.GetErrorList().WrongNoOfParams(piVal, 2, iLine, iPos);
		return false;
	}

	COGLFrame* pFrame = 0;

	if ((iVarCount == 2) && (mVars(0).BaseType() == PDT_SCENE))
	{
		pFrame = dynamic_cast< COGLFrame* >((COGLBaseElement*) *mVars(0).GetScenePtr());
		if (!pFrame)
		{
			rCB.GetErrorList().GeneralError("Invalid frame object.", iLine, iPos);
			return false;
		}

		iVarOff = 1;
	}
	else if ((iVarCount == 1) || ((iVarCount == 2) && (mVars(0).BaseType() == PDT_STRING)))
	{
		pFrame = new COGLFrame;

		if (iVarCount == 1)
		{
			pFrame->SetFrameMode(COGLFrame::MODELVIEW);
			iVarOff = 0;
		}
		else
		{
			TString& rMode = *mVars(0).GetStringPtr();
			if (rMode == "world")
			{
				pFrame->SetFrameMode(COGLFrame::MODELVIEW);
			}
			else if (rMode == "view")
			{
				pFrame->SetFrameMode(COGLFrame::PROJECTION);
			}
			else if (rMode == "texture")
			{
				pFrame->SetFrameMode(COGLFrame::TEXTURE);
			}
			else
			{
				delete pFrame;
				rCB.GetErrorList().GeneralError("Expect first parameter to give frame type: \"world\" or \"view\" or \"texture\".", iLine, iPos);
				return false;
			}
			iVarOff = 1;
		}

		COGLBEReference Ref = rCB.GetOGLDrawBase()->GetSceneRepository()->New(pFrame);
		rCB.GetOGLDrawBase()->DrawBaseElement(Ref);
		rVar = Ref;
	}
	else
	{
		rCB.GetErrorList().GeneralError("Invalid combination of parameters. Please have a look at the documentation of this function.", iLine, iPos);
		return false;
	}

	if (mVars(iVarOff).BaseType() == PDT_MATRIX)
	{
		pFrame->Multiply(*mVars(iVarOff).GetMatrixPtr());
	}
	else if (mVars(iVarOff).BaseType() == PDT_TENSOR)
	{
		pFrame->Multiply(*mVars(iVarOff).GetTensorPtr());
	}
	else if (mVars(iVarOff).BaseType() == PDT_TENSOR_IDX)
	{
		TTensor TDummy;
		TTensorIdx& rTIdx = *mVars(iVarOff).GetTensorIdxPtr();

		if (!MakeTensor(TDummy, rTIdx))
		{
			rCB.GetErrorList().GeneralError("Error generating tensor.", iLine, iPos);
			return false;
		}

		pFrame->Multiply(TDummy);
	}
	else if (mVars(iVarOff).BaseType() == PDT_MULTIV)
	{
		pFrame->Multiply(*mVars(iVarOff).GetMultiVPtr());
	}
	else
	{
		if (iVarCount == 1)
		{
			rCB.GetErrorList().GeneralError("Expect as first parameter a matrix, tensor, or multivector.", iLine, iPos);
		}
		else
		{
			rCB.GetErrorList().GeneralError("Expect as second parameter a matrix, tensor, or multivector.", iLine, iPos);
		}
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Auto Scale to Pixel Size Frame

bool AutoBillboardFrameFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());
	int iVarOff;
	TCVScalar pfVec[2] = { 0.0f, 0.0f };

	if ((iVarCount >= 1) && (mVars(0).BaseType() == PDT_SCENE))
	{
		if ((iVarCount != 1) && (iVarCount != 3))
		{
			int piVal[] = { 1, 3 };
			rCB.GetErrorList().WrongNoOfParams(piVal, 2, iLine, iPos);
			return false;
		}

		iVarOff = 1;
		iVarCount--;
	}
	else
	{
		if ((iVarCount != 0) && (iVarCount != 2))
		{
			int piVal[] = { 0, 2 };
			rCB.GetErrorList().WrongNoOfParams(piVal, 2, iLine, iPos);
			return false;
		}

		iVarOff = 0;
	}

	if (iVarCount == 2)
	{
		int i;

		for (i = 0; i < 2; i++)
		{
			if (!(mVars(i + iVarOff).CastToScalar(pfVec[i], rCB.GetSensitivity())))
			{
				rCB.GetErrorList().InvalidType(mVars(i + iVarOff), iLine, iPos);
				return false;
			}
		}
	}

	COGLFrame* pFrame;

	if (iVarOff == 0)
	{
		pFrame = new COGLFrame;

		COGLBEReference Ref = rCB.GetOGLDrawBase()->GetSceneRepository()->New(pFrame);
		rCB.GetOGLDrawBase()->DrawBaseElement(Ref);
		rVar = Ref;
	}
	else
	{
		pFrame = dynamic_cast< COGLFrame* >((COGLBaseElement*) *mVars(0).GetScenePtr());
		if (!pFrame)
		{
			rCB.GetErrorList().GeneralError("Invalid frame object.", iLine, iPos);
			return false;
		}
	}

	pFrame->SetFrameMode(COGLFrame::MODELVIEW);
	pFrame->EnableAutoBillboard(true);

	if (iVarCount >= 2)
	{
		if ((pfVec[0] >= 1.0f) && (pfVec[1] >= 1.0f))
		{
			pFrame->AutoScaleToPixelSize(1.0f, 1.0f, pfVec[0], pfVec[1]);
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Auto Scale Texture frame to adjust for aspect ratio and resultant pixel size

bool AutoTextureFrameFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());
	int iVarOff;
	TCVScalar dLocalW        = 1.0, dLocalH = 1.0, dPixW = 100.0, dPixH = 100.0;
	TCVScalar dAlignH        = 0.5, dAlignV = 0.5;
	TCVCounter iAllowScaleUp = 1, iAllowScaleDown = 1;

	if ((iVarCount >= 1) && (mVars(0).BaseType() == PDT_SCENE))
	{
		if (iVarCount != 5)
		{
			int piVal[] = { 5 };
			rCB.GetErrorList().WrongNoOfParams(piVal, 1, iLine, iPos);
			return false;
		}

		iVarOff = 1;
		iVarCount--;
	}
	else
	{
		if (iVarCount != 4)
		{
			int piVal[] = { 4 };
			rCB.GetErrorList().WrongNoOfParams(piVal, 1, iLine, iPos);
			return false;
		}

		iVarOff = 0;
	}

	if (iVarCount >= 1)
	{
		if (mVars(iVarOff + 0).BaseType() != PDT_VARLIST)
		{
			rCB.GetErrorList().GeneralError("Expect list with local width and height.", iLine, iPos);
			return false;
		}

		TVarList& rList = *mVars(iVarOff + 0).GetVarListPtr();

		if (rList.Count() != 2)
		{
			rCB.GetErrorList().GeneralError("Expect list with local width and height.", iLine, iPos);
			return false;
		}

		if (!rList(0).CastToScalar(dLocalW, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Local width has to be a scalar.", iLine, iPos);
			return false;
		}

		if (!rList(1).CastToScalar(dLocalH, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Local height has to be a scalar.", iLine, iPos);
			return false;
		}
	}

	if (iVarCount >= 2)
	{
		if (mVars(iVarOff + 1).BaseType() != PDT_VARLIST)
		{
			rCB.GetErrorList().GeneralError("Expect list with pixel width and height.", iLine, iPos);
			return false;
		}

		TVarList& rList = *mVars(iVarOff + 1).GetVarListPtr();

		if (rList.Count() != 2)
		{
			rCB.GetErrorList().GeneralError("Expect list with pixel width and height.", iLine, iPos);
			return false;
		}

		if (!rList(0).CastToScalar(dPixW, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Pixel width has to be a scalar.", iLine, iPos);
			return false;
		}

		if (!rList(1).CastToScalar(dPixH, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Pixel height has to be a scalar.", iLine, iPos);
			return false;
		}
	}

	if (iVarCount >= 3)
	{
		if (mVars(iVarOff + 2).BaseType() != PDT_VARLIST)
		{
			rCB.GetErrorList().GeneralError("Expect list with horizontal and vertical align factors.", iLine, iPos);
			return false;
		}

		TVarList& rList = *mVars(iVarOff + 2).GetVarListPtr();

		if (rList.Count() != 2)
		{
			rCB.GetErrorList().GeneralError("Expect list with horizontal and vertical align factors.", iLine, iPos);
			return false;
		}

		if (!rList(0).CastToScalar(dAlignH, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Horizontal align factor has to be a scalar.", iLine, iPos);
			return false;
		}

		if (!rList(1).CastToScalar(dAlignV, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Vertical align factor has to be a scalar.", iLine, iPos);
			return false;
		}
	}

	if (iVarCount >= 4)
	{
		if (mVars(iVarOff + 3).BaseType() != PDT_VARLIST)
		{
			rCB.GetErrorList().GeneralError("Expect list with up- and down-scaling flags.", iLine, iPos);
			return false;
		}

		TVarList& rList = *mVars(iVarOff + 3).GetVarListPtr();

		if (rList.Count() != 2)
		{
			rCB.GetErrorList().GeneralError("Expect list with up- and down-scaling flags.", iLine, iPos);
			return false;
		}

		if (!rList(0).CastToCounter(iAllowScaleUp))
		{
			rCB.GetErrorList().GeneralError("Up-scaling flag has to be a bool.", iLine, iPos);
			return false;
		}

		if (!rList(1).CastToCounter(iAllowScaleDown))
		{
			rCB.GetErrorList().GeneralError("Down-scaling flag has to be a bool.", iLine, iPos);
			return false;
		}
	}

	COGLFrame* pFrame;

	if (iVarOff == 0)
	{
		pFrame = new COGLFrame;

		COGLBEReference Ref = rCB.GetOGLDrawBase()->GetSceneRepository()->New(pFrame);
		rCB.GetOGLDrawBase()->DrawBaseElement(Ref);
		rVar = Ref;
	}
	else
	{
		pFrame = dynamic_cast< COGLFrame* >((COGLBaseElement*) *mVars(0).GetScenePtr());
		if (!pFrame)
		{
			rCB.GetErrorList().GeneralError("Invalid frame object.", iLine, iPos);
			return false;
		}
	}

	pFrame->SetFrameMode(COGLFrame::TEXTURE);
	pFrame->SetOrigin(-dAlignH, -dAlignV, 0.0);
	pFrame->EnableOrigin(true);
	pFrame->AutoScaleToPixelSize(dLocalW, dLocalH, dPixW, dPixH, iAllowScaleUp != 0, iAllowScaleDown != 0);

	return true;
}

bool AutoPixelSizeFrameFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());
	int iVarOff;
	TCVScalar pfVec[2] = { 0.0f, 0.0f };

	if ((iVarCount >= 1) && (mVars(0).BaseType() == PDT_SCENE))
	{
		if ((iVarCount != 1) && (iVarCount != 3))
		{
			int piVal[] = { 1, 3 };
			rCB.GetErrorList().WrongNoOfParams(piVal, 2, iLine, iPos);
			return false;
		}

		iVarOff = 1;
		iVarCount--;
	}
	else
	{
		if ((iVarCount != 0) && (iVarCount != 2))
		{
			int piVal[] = { 0, 2 };
			rCB.GetErrorList().WrongNoOfParams(piVal, 2, iLine, iPos);
			return false;
		}

		iVarOff = 0;
	}

	if (iVarCount == 2)
	{
		int i;

		for (i = 0; i < 2; i++)
		{
			if (!(mVars(i + iVarOff).CastToScalar(pfVec[i], rCB.GetSensitivity())))
			{
				rCB.GetErrorList().InvalidType(mVars(i + iVarOff), iLine, iPos);
				return false;
			}
		}
	}

	COGLFrame* pFrame;

	if (iVarOff == 0)
	{
		pFrame = new COGLFrame;

		COGLBEReference Ref = rCB.GetOGLDrawBase()->GetSceneRepository()->New(pFrame);
		rCB.GetOGLDrawBase()->DrawBaseElement(Ref);
		rVar = Ref;
	}
	else
	{
		pFrame = dynamic_cast<COGLFrame*>((COGLBaseElement*) *mVars(0).GetScenePtr());
		if (!pFrame)
		{
			rCB.GetErrorList().GeneralError("Invalid frame object.", iLine, iPos);
			return false;
		}
	}

	// We disable billboarding and enable auto scaling
	pFrame->SetFrameMode(COGLFrame::MODELVIEW);
	pFrame->EnableAutoBillboard(false);

	if (iVarCount >= 2)
	{
		if ((pfVec[0] >= 1.0f) && (pfVec[1] >= 1.0f))
		{
			pFrame->AutoScaleToPixelSize(1.0f, 1.0f, pfVec[0], pfVec[1]);
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Translate Frame

bool TranslateFrameFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());
	int iVarOff;
	TCVCounter iMultiply = 1;
	TCVScalar pfVec[3];
	bool bNewFrame = false;

	if ((iVarCount >= 1) &&
	    ((mVars(0).BaseType() == PDT_SCENE) || (mVars(0).BaseType() == PDT_STRING)))
	{
		if ((iVarCount != 2) && (iVarCount != 4))
		{
			int piVal[] = { 2, 4 };
			rCB.GetErrorList().WrongNoOfParams(piVal, 2, iLine, iPos);
			return false;
		}

		bNewFrame = false;
		iVarOff   = 1;
		iVarCount--;
	}
	else
	{
		if ((iVarCount != 1) && (iVarCount != 3))
		{
			int piVal[] = { 1, 3 };
			rCB.GetErrorList().WrongNoOfParams(piVal, 2, iLine, iPos);
			return false;
		}

		bNewFrame = true;
		iVarOff   = 0;
	}

	if (iVarCount == 3)
	{
		int i;

		for (i = 0; i < 3; i++)
		{
			if (!(mVars(i + iVarOff).CastToScalar(pfVec[i], rCB.GetSensitivity())))
			{
				rCB.GetErrorList().InvalidType(mVars(i + iVarOff), iLine, iPos);
				return false;
			}
		}

		iVarCount -= 3;
		iVarOff   += 3;
	}
	else if (iVarCount == 1)
	{
		if (mVars(iVarOff).BaseType() != PDT_MULTIV)
		{
			rCB.GetErrorList().InvalidType(mVars(iVarOff), iLine, iPos);
			return false;
		}

		TMultiV& vA = *mVars(iVarOff).GetMultiVPtr();
		TMultiV vR;

		// Only analyze multivector
		rCB.GetFilter()->DrawMV(vA, true);

		// Get MVInfo
		CMVInfo<float> MVInfo = rCB.GetFilter()->GetMVInfo();

		if (MVInfo.m_eType == GA_TRANSLATOR)
		{
			pfVec[0] = MVInfo.m_mVex[0][0];
			pfVec[1] = MVInfo.m_mVex[0][1];
			pfVec[2] = MVInfo.m_mVex[0][2];
		}
		else
		{
			rCB.CastMVtoE3(vA, vR);
			pfVec[0] = vR[1];
			pfVec[1] = vR[2];
			pfVec[2] = vR[3];
		}

		iVarCount--;
		iVarOff++;
	}

	if (iVarCount > 0)
	{
		if (!mVars(iVarOff).CastToCounter(iMultiply))
		{
			rCB.GetErrorList().GeneralError("Last parameter has to be a bool, indicating whether translation is to be multiplied with current frame or not.",
					iLine, iPos);
			return false;
		}
	}

	if (bNewFrame)
	{
		COGLFrame* pFrame = new COGLFrame;

		pFrame->Translate(pfVec[0], pfVec[1], pfVec[2], iMultiply > 0);

		COGLBEReference Ref = rCB.GetOGLDrawBase()->GetSceneRepository()->New(pFrame);
		rCB.GetOGLDrawBase()->DrawBaseElement(Ref);
		rVar = Ref;
	}
	else if (!bNewFrame && (mVars(0).BaseType() == PDT_STRING))
	{
		COGLFrame* pFrame = new COGLFrame;

		TString& rMode = *mVars(0).GetStringPtr();
		if (rMode == "world")
		{
			pFrame->SetFrameMode(COGLFrame::MODELVIEW);
		}
		else if (rMode == "view")
		{
			pFrame->SetFrameMode(COGLFrame::PROJECTION);
		}
		else if (rMode == "texture")
		{
			pFrame->SetFrameMode(COGLFrame::TEXTURE);
		}
		else
		{
			rCB.GetErrorList().GeneralError("Expect first parameter to give frame mode: \"world\" or \"view\" or \"texture\".", iLine, iPos);
			return false;
		}

		pFrame->Translate(pfVec[0], pfVec[1], pfVec[2], iMultiply > 0);

		COGLBEReference Ref = rCB.GetOGLDrawBase()->GetSceneRepository()->New(pFrame);
		rCB.GetOGLDrawBase()->DrawBaseElement(Ref);
		rVar = Ref;
	}
	else
	{
		COGLFrame* pFrame = dynamic_cast< COGLFrame* >((COGLBaseElement*) *mVars(0).GetScenePtr());
		if (!pFrame)
		{
			rCB.GetErrorList().GeneralError("Invalid frame object.", iLine, iPos);
			return false;
		}

		pFrame->Translate(pfVec[0], pfVec[1], pfVec[2], iMultiply > 0);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Rotate Frame Function
//
// Parameters:
//	1. Frame (optional)
//  2. Axis X
//	3. Axis Y
//	4. Axis Z
//	5. Angle in Degrees
//  6. Multiply flag

bool RotateFrameFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());
	int iVarOff;
	TCVScalar pfVec[4];
	TCVCounter iMultiply = 1;
	bool bNewFrame       = false;

	if ((iVarCount >= 1) &&
	    ((mVars(0).BaseType() == PDT_SCENE) || (mVars(0).BaseType() == PDT_STRING)))
	{
		if ((iVarCount != 2) && (iVarCount != 5) && (iVarCount != 6))
		{
			int piVal[] = { 2, 5, 6 };
			rCB.GetErrorList().WrongNoOfParams(piVal, 3, iLine, iPos);
			return false;
		}

		bNewFrame = false;
		iVarOff   = 1;
		iVarCount--;
	}
	else
	{
		if ((iVarCount != 1) && (iVarCount != 4) && (iVarCount != 5))
		{
			int piVal[] = { 1, 4, 5 };
			rCB.GetErrorList().WrongNoOfParams(piVal, 3, iLine, iPos);
			return false;
		}

		bNewFrame = true;
		iVarOff   = 0;
	}

	if (iVarCount >= 4)
	{
		int i;

		for (i = 0; i < 4; i++)
		{
			if (!(mVars(i + iVarOff).CastToScalar(pfVec[i], rCB.GetSensitivity())))
			{
				rCB.GetErrorList().InvalidType(mVars(i + iVarOff), iLine, iPos);
				return false;
			}
		}

		iVarCount -= 4;
		iVarOff   += 4;
	}
	else if (iVarCount >= 1)
	{
		if (mVars(iVarOff).BaseType() != PDT_MULTIV)
		{
			rCB.GetErrorList().InvalidType(mVars(iVarOff), iLine, iPos);
			return false;
		}

		TMultiV vAxis;
		TMultiV& vR = *mVars(iVarOff).GetMultiVPtr();

		// Only analyze multivector
		rCB.GetFilter()->DrawMV(vR, true);

		// Get MVInfo
		CMVInfo<float> MVInfo = rCB.GetFilter()->GetMVInfo();

		if (MVInfo.m_eType == GA_ROTOR)
		{
			pfVec[0] = MVInfo.m_mVex[0][0];
			pfVec[1] = MVInfo.m_mVex[0][1];
			pfVec[2] = MVInfo.m_mVex[0][2];

			pfVec[3] = MVInfo.m_mVal[0] / rCB.GetRadPerDeg();
		}
		else if (MVInfo.m_eType == GA_MOTOR)
		{
			COGLVertex xA;

			xA = MVInfo.m_mVex[0].Data();
			if (xA.Mag() > 1e-6)
			{
				rCB.GetErrorList().GeneralError("Given multivector does not represent a rotor. "
								"For general transformations use MultiplyFrame() instead.", iLine, iPos);
				return false;
			}

			pfVec[0] = MVInfo.m_mVex[1][0];
			pfVec[1] = MVInfo.m_mVex[1][1];
			pfVec[2] = MVInfo.m_mVex[1][2];

			pfVec[3] = MVInfo.m_mVal[0] / rCB.GetRadPerDeg();
		}
		else if ((MVInfo.m_eType == GA_SCALAR) || (MVInfo.m_eType == GA_SPACE))
		{
			pfVec[0] = 0.0;
			pfVec[1] = 1.0;
			pfVec[2] = 0.0;
			pfVec[3] = 0.0;
		}
		else
		{
			rCB.GetErrorList().GeneralError("Given multivector does not represent a rotor. "
							"For general transformations use MultiplyFrame() instead.", iLine, iPos);
			return false;
		}

		iVarCount--;
		iVarOff++;
	}

	if (iVarCount > 0)
	{
		if (!mVars(iVarOff).CastToCounter(iMultiply))
		{
			rCB.GetErrorList().GeneralError("Last parameter has to be a bool, indicating whether rotation is to be multiplied with current frame or not.",
					iLine, iPos);
			return false;
		}
	}

	if (bNewFrame)
	{
		COGLFrame* pFrame = new COGLFrame;

		pFrame->Rotate(pfVec[3] * rCB.GetRadPerDeg(), pfVec[0], pfVec[1], pfVec[2], iMultiply > 0);

		COGLBEReference Ref = rCB.GetOGLDrawBase()->GetSceneRepository()->New(pFrame);
		rCB.GetOGLDrawBase()->DrawBaseElement(Ref);
		rVar = Ref;
	}
	else if (!bNewFrame && (mVars(0).BaseType() == PDT_STRING))
	{
		COGLFrame* pFrame = new COGLFrame;

		TString& rMode = *mVars(0).GetStringPtr();
		if (rMode == "world")
		{
			pFrame->SetFrameMode(COGLFrame::MODELVIEW);
		}
		else if (rMode == "view")
		{
			pFrame->SetFrameMode(COGLFrame::PROJECTION);
		}
		else if (rMode == "texture")
		{
			pFrame->SetFrameMode(COGLFrame::TEXTURE);
		}
		else
		{
			delete pFrame;
			rCB.GetErrorList().GeneralError("Expect first parameter to give frame mode: \"world\" or \"view\" or \"texture\".", iLine, iPos);
			return false;
		}

		pFrame->Rotate(pfVec[3] * rCB.GetRadPerDeg(), pfVec[0], pfVec[1], pfVec[2], iMultiply > 0);

		COGLBEReference Ref = rCB.GetOGLDrawBase()->GetSceneRepository()->New(pFrame);
		rCB.GetOGLDrawBase()->DrawBaseElement(Ref);
		rVar = Ref;
	}
	else
	{
		COGLFrame* pFrame = dynamic_cast< COGLFrame* >((COGLBaseElement*) *mVars(0).GetScenePtr());
		if (!pFrame)
		{
			rCB.GetErrorList().GeneralError("Invalid frame object.", iLine, iPos);
			return false;
		}

		pFrame->Rotate(pfVec[3] * rCB.GetRadPerDeg(), pfVec[0], pfVec[1], pfVec[2], iMultiply > 0);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Reflect Frame

bool ReflectFrameFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());
	int iVarOff;
	TCVScalar pfVec[3];

	if ((iVarCount >= 1) &&
	    ((mVars(0).BaseType() == PDT_SCENE) || (mVars(0).BaseType() == PDT_STRING)))
	{
		if ((iVarCount != 2) && (iVarCount != 4))
		{
			int piVal[] = { 2, 4 };
			rCB.GetErrorList().WrongNoOfParams(piVal, 2, iLine, iPos);
			return false;
		}

		iVarOff = 1;
		iVarCount--;
	}
	else
	{
		if ((iVarCount != 1) && (iVarCount != 3))
		{
			int piVal[] = { 1, 3 };
			rCB.GetErrorList().WrongNoOfParams(piVal, 2, iLine, iPos);
			return false;
		}

		iVarOff = 0;
	}

	if (iVarCount == 3)
	{
		int i;

		for (i = 0; i < 3; i++)
		{
			if (!(mVars(i + iVarOff).CastToScalar(pfVec[i], rCB.GetSensitivity())))
			{
				rCB.GetErrorList().InvalidType(mVars(i + iVarOff), iLine, iPos);
				return false;
			}
		}
	}
	else if (iVarCount == 1)
	{
		if (mVars(iVarOff).BaseType() != PDT_MULTIV)
		{
			rCB.GetErrorList().InvalidType(mVars(iVarOff), iLine, iPos);
			return false;
		}

		TMultiV& vA = *mVars(iVarOff).GetMultiVPtr();
		TMultiV vR;

		rCB.CastMVtoE3(vA, vR);
		pfVec[0] = vR[1];
		pfVec[1] = vR[2];
		pfVec[2] = vR[3];
	}

	if (iVarOff == 0)
	{
		COGLFrame* pFrame = new COGLFrame;

		pFrame->Reflect(pfVec[0], pfVec[1], pfVec[2]);

		COGLBEReference Ref = rCB.GetOGLDrawBase()->GetSceneRepository()->New(pFrame);
		rCB.GetOGLDrawBase()->DrawBaseElement(Ref);
		rVar = Ref;
	}
	else if ((iVarOff == 1) && (mVars(0).BaseType() == PDT_STRING))
	{
		COGLFrame* pFrame = new COGLFrame;

		TString& rMode = *mVars(0).GetStringPtr();
		if (rMode == "world")
		{
			pFrame->SetFrameMode(COGLFrame::MODELVIEW);
		}
		else if (rMode == "view")
		{
			pFrame->SetFrameMode(COGLFrame::PROJECTION);
		}
		else if (rMode == "texture")
		{
			pFrame->SetFrameMode(COGLFrame::TEXTURE);
		}
		else
		{
			delete pFrame;
			rCB.GetErrorList().GeneralError("Expect first parameter to give frame mode: \"world\" or \"view\" or \"texture\".", iLine, iPos);
			return false;
		}

		pFrame->Reflect(pfVec[0], pfVec[1], pfVec[2]);

		COGLBEReference Ref = rCB.GetOGLDrawBase()->GetSceneRepository()->New(pFrame);
		rCB.GetOGLDrawBase()->DrawBaseElement(Ref);
		rVar = Ref;
	}
	else
	{
		COGLFrame* pFrame = dynamic_cast< COGLFrame* >((COGLBaseElement*) *mVars(0).GetScenePtr());
		if (!pFrame)
		{
			rCB.GetErrorList().GeneralError("Invalid frame object.", iLine, iPos);
			return false;
		}

		pFrame->Reflect(pfVec[0], pfVec[1], pfVec[2]);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Scale Frame

bool ScaleFrameFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());
	int iVarOff;
	TCVScalar pfVec[3];

	if ((iVarCount >= 1) &&
	    ((mVars(0).BaseType() == PDT_SCENE) || (mVars(0).BaseType() == PDT_STRING)))
	{
		if ((iVarCount != 2) && (iVarCount != 4))
		{
			int piVal[] = { 2, 4 };
			rCB.GetErrorList().WrongNoOfParams(piVal, 2, iLine, iPos);
			return false;
		}

		iVarOff = 1;
		iVarCount--;
	}
	else
	{
		if ((iVarCount != 1) && (iVarCount != 3))
		{
			int piVal[] = { 1, 3 };
			rCB.GetErrorList().WrongNoOfParams(piVal, 2, iLine, iPos);
			return false;
		}

		iVarOff = 0;
	}

	if (iVarCount == 3)
	{
		int i;

		for (i = 0; i < 3; i++)
		{
			if (!(mVars(i + iVarOff).CastToScalar(pfVec[i], rCB.GetSensitivity())))
			{
				rCB.GetErrorList().InvalidType(mVars(i + iVarOff), iLine, iPos);
				return false;
			}
		}
	}
	else if (iVarCount == 1)
	{
		if (mVars(iVarOff).BaseType() != PDT_MULTIV)
		{
			rCB.GetErrorList().InvalidType(mVars(iVarOff), iLine, iPos);
			return false;
		}

		TMultiV& vA = *mVars(iVarOff).GetMultiVPtr();
		TMultiV vR;

		rCB.CastMVtoE3(vA, vR);
		pfVec[0] = vR[1];
		pfVec[1] = vR[2];
		pfVec[2] = vR[3];
	}

	if (iVarOff == 0)
	{
		COGLFrame* pFrame = new COGLFrame;

		pFrame->Scale(pfVec[0], pfVec[1], pfVec[2]);

		COGLBEReference Ref = rCB.GetOGLDrawBase()->GetSceneRepository()->New(pFrame);
		rCB.GetOGLDrawBase()->DrawBaseElement(Ref);
		rVar = Ref;
	}
	else if ((iVarOff == 1) && (mVars(0).BaseType() == PDT_STRING))
	{
		COGLFrame* pFrame = new COGLFrame;

		TString& rMode = *mVars(0).GetStringPtr();
		if (rMode == "world")
		{
			pFrame->SetFrameMode(COGLFrame::MODELVIEW);
		}
		else if (rMode == "view")
		{
			pFrame->SetFrameMode(COGLFrame::PROJECTION);
		}
		else if (rMode == "texture")
		{
			pFrame->SetFrameMode(COGLFrame::TEXTURE);
		}
		else
		{
			delete pFrame;
			rCB.GetErrorList().GeneralError("Expect first parameter to give frame mode: \"world\" or \"view\" or \"texture\".", iLine, iPos);
			return false;
		}

		pFrame->Scale(pfVec[0], pfVec[1], pfVec[2]);

		COGLBEReference Ref = rCB.GetOGLDrawBase()->GetSceneRepository()->New(pFrame);
		rCB.GetOGLDrawBase()->DrawBaseElement(Ref);
		rVar = Ref;
	}
	else
	{
		COGLFrame* pFrame = dynamic_cast< COGLFrame* >((COGLBaseElement*) *mVars(0).GetScenePtr());
		if (!pFrame)
		{
			rCB.GetErrorList().GeneralError("Invalid frame object.", iLine, iPos);
			return false;
		}

		pFrame->Scale(pfVec[0], pfVec[1], pfVec[2]);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Create an animated rotation element
//
// Parameters:
//	1. (opt) Frame
//	(either) 2.-4. angle and rot. axis.
//  (or) 2. angle, 3. axis multivector
//  (or) 2. rotor
//  - anim type
//  - anim type parameters
//  - (opt if frame is given) bool: direct(true, default)/indirect (false).
//			direct: animation frame becomes part of frame object
//			indirect: animation modifies frame but does not apply the frame.

bool AnimRotateFrameFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	int iVarPos   = 0;
	TCVScalar pfVec[4];
	TCVCounter iDirect = 1;
	COGLFrame* pFrame  = 0;

	if ((iVarCount >= 1) && (mVars(0).BaseType() == PDT_SCENE))
	{
		TScene& rScene = *mVars(0).GetScenePtr();
		if (!rScene.IsValid())
		{
			rCB.GetErrorList().GeneralError("Given frame is invalid.", iLine, iPos);
			return false;
		}

		pFrame = dynamic_cast<COGLFrame*>((COGLBaseElement*) rScene);
		if (!pFrame)
		{
			rCB.GetErrorList().GeneralError("Given frame is invalid.", iLine, iPos);
			return false;
		}

		iVarPos    = 1;
		iVarCount -= 1;
	}

	if ((iVarCount >= 1) && (mVars(iVarPos).BaseType() == PDT_MULTIV))
	{
		TMultiV vAxis;
		TMultiV& vR = *mVars(iVarPos).GetMultiVPtr();
		CMVInfo<TCVScalar> Info;

		rCB.GetFilter()->DrawMV(vR, true);
		rCB.GetFilter()->GetMVInfo(Info);

		if (Info.m_eType != GA_ROTOR)
		{
			rCB.GetErrorList().GeneralError("Given multivector does not represent a rotation.", iLine, iPos);
			return false;
		}

		pfVec[0] = -TCVScalar(Info.m_mVal[0] / rCB.GetRadPerDeg());
		pfVec[1] = TCVScalar(Info.m_mVex[0][0]);
		pfVec[2] = TCVScalar(Info.m_mVex[0][1]);
		pfVec[3] = TCVScalar(Info.m_mVex[0][2]);

		iVarPos   += 1;
		iVarCount -= 1;
	}
	else if ((iVarCount >= 2)
		 && mVars(iVarPos).CastToScalar(pfVec[0], rCB.GetSensitivity())
		 && (mVars(iVarPos + 1).BaseType() == PDT_MULTIV))
	{
		TMultiV& vA = *mVars(iVarPos + 1).GetMultiVPtr();
		TMultiV vAxis;

		if (!rCB.CastMVtoE3(vA, vAxis))
		{
			rCB.GetErrorList().GeneralError("Given multivector does not represent a rotation axis.", iLine, iPos);
			return false;
		}

		pfVec[1] = TCVScalar(vAxis[rCB.GetE3GABase().iE1]);
		pfVec[2] = TCVScalar(vAxis[rCB.GetE3GABase().iE2]);
		pfVec[3] = TCVScalar(vAxis[rCB.GetE3GABase().iE3]);

		iVarPos   += 2;
		iVarCount -= 2;
	}
	else if (iVarCount >= 4)
	{
		int i;

		for (i = 0; i < 4; i++)
		{
			if (!(mVars(iVarPos + i).CastToScalar(pfVec[i], rCB.GetSensitivity())))
			{
				rCB.GetErrorList().InvalidType(mVars(iVarPos + i), iLine, iPos);
				return false;
			}
		}

		iVarPos   += 4;
		iVarCount -= 4;
	}
	else
	{
		rCB.GetErrorList().GeneralError("Expect as first parameters either four scalar values, "
						"a scalar angle and a rotation axis vector, or a rotor.", iLine, iPos);
		return false;
	}

	if (iVarCount < 1)
	{
		rCB.GetErrorList().GeneralError("Expect an animation type.", iLine, iPos);
		return false;
	}

	if (mVars(iVarPos).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("Expect an animation type string.", iLine, iPos);
		return false;
	}

	TString sType = *mVars(iVarPos).GetStringPtr();
	sType.ToLowerCase();

	COGLAnimRotation* pAnim = new COGLAnimRotation;
	pAnim->Set(float(pfVec[0]), float(pfVec[1]), float(pfVec[2]), float(pfVec[3]));

	if ((sType == "sinus") || (sType == "sinus2") || (sType == "cont_sinus2"))
	{
		if (iVarCount < 3)
		{
			rCB.GetErrorList().GeneralError("Expect rotation speed and frequency.", iLine, iPos);
			return false;
		}

		TCVScalar dFreq, dSpeed;

		if (!mVars(iVarPos + 1).CastToScalar(dSpeed, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Speed parameter has to be a scalar.", iLine, iPos);
			return false;
		}

		if (!mVars(iVarPos + 2).CastToScalar(dFreq, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Frequency parameter has to be a scalar.", iLine, iPos);
			return false;
		}

		pAnim->SetSpeed(float(dSpeed));
		pAnim->SetFreq(float(dFreq));

		if (sType == "sinus")
		{
			pAnim->SetMode(COGLAnimRotation::/*EAnimMode::*/ SINUS);
		}
		else if (sType == "sinus2")
		{
			pAnim->SetMode(COGLAnimRotation::/*EAnimMode::*/ SINUS2);
		}
		else if (sType == "cont_sinus2")
		{
			pAnim->SetMode(COGLAnimRotation::/*EAnimMode::*/ CONT_SINUS2);
		}

		iVarPos   += 3;
		iVarCount -= 3;
	}
	else if (sType == "rel_sinus")
	{
		if (iVarCount < 4)
		{
			rCB.GetErrorList().GeneralError("Expect rotation speed, frequency and amplitude.", iLine, iPos);
			return false;
		}

		TCVScalar dFreq, dSpeed, dAmp;

		if (!mVars(iVarPos + 1).CastToScalar(dSpeed, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Speed parameter has to be a scalar.", iLine, iPos);
			return false;
		}

		if (!mVars(iVarPos + 2).CastToScalar(dFreq, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Frequency parameter has to be a scalar.", iLine, iPos);
			return false;
		}

		if (!mVars(iVarPos + 3).CastToScalar(dAmp, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Frequency parameter has to be a scalar.", iLine, iPos);
			return false;
		}

		pAnim->SetSpeed(float(dSpeed));
		pAnim->SetFreq(float(dFreq));
		pAnim->SetAmplitude(float(dAmp));

		pAnim->SetMode(COGLAnimRotation::/*EAnimMode::*/ REL_SINUS);

		iVarPos   += 4;
		iVarCount -= 4;
	}
	else if (sType == "constant")
	{
		if (iVarCount < 2)
		{
			rCB.GetErrorList().GeneralError("Expect rotation speed.", iLine, iPos);
			return false;
		}

		TCVScalar dSpeed;

		if (!mVars(iVarPos + 1).CastToScalar(dSpeed, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Speed parameter has to be a scalar.", iLine, iPos);
			return false;
		}

		pAnim->SetSpeed(float(dSpeed));
		pAnim->SetFreq(0.0f);
		pAnim->SetMode(COGLAnimRotation::/*EAnimMode::*/ CONSTANT);

		iVarPos   += 2;
		iVarCount -= 2;
	}
	else
	{
		delete pAnim;

		rCB.GetErrorList().GeneralError("Unknown animation type.", iLine, iPos);
		return false;
	}

	// Is direct/indirect bool give?
	if (iVarCount > 0)
	{
		if (!mVars(iVarPos).CastToCounter(iDirect))
		{
			rCB.GetErrorList().GeneralError("Expect direct/indirect parameter to be a bool.", iLine, iPos);
			return false;
		}

		if ((iDirect == 0) && !pFrame)
		{
			rCB.GetErrorList().GeneralError("Indirect animated rotation only available if a frame is given as first parameter.", iLine, iPos);
			return false;
		}
	}

	if (pAnim)
	{
		COGLBEReference Ref = rCB.GetOGLDrawBase()->GetSceneRepository()->New(pAnim);

		if (pFrame)
		{
			if (iDirect)
			{
				pFrame->SetAnimateFrame(Ref);
			}
			else
			{
				pAnim->SetFrame(*mVars(0).GetScenePtr());
				pAnim->EnableFrame(true);
				rCB.GetOGLDrawBase()->DrawBaseElement(Ref);
				rVar = Ref;
			}
		}
		else
		{
			rCB.GetOGLDrawBase()->DrawBaseElement(Ref);
			rVar = Ref;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Create an animated translation element

bool AnimTranslateFrameFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	int iVarPos   = 0;
	TCVScalar pfVec[3];
	TCVCounter iDirect = 1;
	COGLFrame* pFrame  = 0;

	if ((iVarCount >= 1) && (mVars(0).BaseType() == PDT_SCENE))
	{
		TScene& rScene = *mVars(0).GetScenePtr();
		if (!rScene.IsValid())
		{
			rCB.GetErrorList().GeneralError("Given frame is invalid.", iLine, iPos);
			return false;
		}

		pFrame = dynamic_cast<COGLFrame*>((COGLBaseElement*) rScene);
		if (!pFrame)
		{
			rCB.GetErrorList().GeneralError("Given frame is invalid.", iLine, iPos);
			return false;
		}

		iVarPos    = 1;
		iVarCount -= 1;
	}

	if ((iVarCount >= 1) && (mVars(iVarPos).BaseType() == PDT_MULTIV))
	{
		TMultiV vEX;
		TMultiV& vX = *mVars(iVarPos).GetMultiVPtr();
		rCB.CastMVtoE3(vX, vEX);

		pfVec[0] = TCVScalar(vEX[rCB.GetE3GABase().iE1]);
		pfVec[1] = TCVScalar(vEX[rCB.GetE3GABase().iE2]);
		pfVec[2] = TCVScalar(vEX[rCB.GetE3GABase().iE3]);

		iVarPos   += 1;
		iVarCount -= 1;
	}
	else if (iVarCount >= 3)
	{
		int i;

		for (i = 0; i < 3; i++)
		{
			if (!(mVars(iVarPos + i).CastToScalar(pfVec[i], rCB.GetSensitivity())))
			{
				rCB.GetErrorList().InvalidType(mVars(iVarPos + i), iLine, iPos);
				return false;
			}
		}

		iVarPos   += 3;
		iVarCount -= 3;
	}
	else
	{
		rCB.GetErrorList().GeneralError("Expect at least a vector or three translation values.", iLine, iPos);
		return false;
	}

	if (iVarCount < 1)
	{
		rCB.GetErrorList().GeneralError("Expect an animation type.", iLine, iPos);
		return false;
	}

	if (mVars(iVarPos).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("Expect an animation type string.", iLine, iPos);
		return false;
	}

	TString sType = *mVars(iVarPos).GetStringPtr();
	sType.ToLowerCase();

	COGLAnimTranslation* pAnim = new COGLAnimTranslation;
	pAnim->Set(float(pfVec[0]), float(pfVec[1]), float(pfVec[2]));

	if ((sType.ToLowerCase() == "sinus") || (sType == "sinus2"))
	{
		if (iVarCount < 1)
		{
			rCB.GetErrorList().GeneralError("Expect a frequency.", iLine, iPos);
			return false;
		}

		TCVScalar dFreq;

		if (!mVars(iVarPos + 1).CastToScalar(dFreq, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Frequency parameter has to be a scalar.", iLine, iPos);
			return false;
		}

		pAnim->SetFreq(float(dFreq));

		if (sType == "sinus")
		{
			pAnim->SetMode(COGLAnimTranslation::/*EAnimMode::*/ SINUS);
		}
		else
		{
			pAnim->SetMode(COGLAnimTranslation::/*EAnimMode::*/ SINUS2);
		}

		iVarPos   += 2;
		iVarCount -= 2;
	}
	else if (sType == "constant")
	{
		if (iVarCount < 1)
		{
			rCB.GetErrorList().GeneralError("Expect translation frequency.", iLine, iPos);
			return false;
		}

		TCVScalar dFreq;

		if (!mVars(iVarPos + 1).CastToScalar(dFreq, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Frequency parameter has to be a scalar.", iLine, iPos);
			return false;
		}

		pAnim->SetFreq(float(dFreq));
		pAnim->SetMode(COGLAnimTranslation::/*EAnimMode::*/ CONSTANT);

		iVarPos   += 2;
		iVarCount -= 2;
	}
	else
	{
		pAnim->SetFreq(0.0f);
		pAnim->SetMode(COGLAnimTranslation::/*EAnimMode::*/ NONE);
	}

	// Is direct/indirect bool give?
	if (iVarCount > 0)
	{
		if (!mVars(iVarPos).CastToCounter(iDirect))
		{
			rCB.GetErrorList().GeneralError("Expect direct/indirect parameter to be a bool.", iLine, iPos);
			return false;
		}

		if ((iDirect == 0) && !pFrame)
		{
			rCB.GetErrorList().GeneralError("Indirect animated scaling only available if a frame is given as first parameter.", iLine, iPos);
			return false;
		}
	}

	if (pAnim)
	{
		COGLBEReference Ref = rCB.GetOGLDrawBase()->GetSceneRepository()->New(pAnim);

		if (pFrame)
		{
			if (iDirect)
			{
				pFrame->SetAnimateFrame(Ref);
			}
			else
			{
				pAnim->SetFrame(*mVars(0).GetScenePtr());
				pAnim->EnableFrame(true);
				rCB.GetOGLDrawBase()->DrawBaseElement(Ref);
				rVar = Ref;
			}
		}
		else
		{
			rCB.GetOGLDrawBase()->DrawBaseElement(Ref);
			rVar = Ref;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Create an animated scaling element

bool AnimScaleFrameFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	int iVarPos   = 0;
	TCVScalar pfVec[3];
	TCVCounter iDirect = 1;
	COGLFrame* pFrame  = 0;

	if ((iVarCount >= 1) && (mVars(0).BaseType() == PDT_SCENE))
	{
		TScene& rScene = *mVars(0).GetScenePtr();
		if (!rScene.IsValid())
		{
			rCB.GetErrorList().GeneralError("Given frame is invalid.", iLine, iPos);
			return false;
		}

		pFrame = dynamic_cast<COGLFrame*>((COGLBaseElement*) rScene);
		if (!pFrame)
		{
			rCB.GetErrorList().GeneralError("Given frame is invalid.", iLine, iPos);
			return false;
		}

		iVarPos    = 1;
		iVarCount -= 1;
	}

	if ((iVarCount >= 1) && (mVars(iVarPos).BaseType() == PDT_MULTIV))
	{
		TMultiV vEX;
		TMultiV& vX = *mVars(iVarPos).GetMultiVPtr();
		rCB.CastMVtoE3(vX, vEX);

		pfVec[0] = TCVScalar(vEX[rCB.GetE3GABase().iE1]);
		pfVec[1] = TCVScalar(vEX[rCB.GetE3GABase().iE2]);
		pfVec[2] = TCVScalar(vEX[rCB.GetE3GABase().iE3]);

		iVarPos   += 1;
		iVarCount -= 1;
	}
	else if (iVarCount >= 3)
	{
		int i;

		for (i = 0; i < 3; i++)
		{
			if (!(mVars(iVarPos + i).CastToScalar(pfVec[i], rCB.GetSensitivity())))
			{
				rCB.GetErrorList().InvalidType(mVars(iVarPos + i), iLine, iPos);
				return false;
			}
		}

		iVarPos   += 3;
		iVarCount -= 3;
	}
	else
	{
		rCB.GetErrorList().GeneralError("Expect at least a vector or three scaling values.", iLine, iPos);
		return false;
	}

	if (iVarCount < 1)
	{
		rCB.GetErrorList().GeneralError("Expect an animation type.", iLine, iPos);
		return false;
	}

	if (mVars(iVarPos).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("Expect an animation type string.", iLine, iPos);
		return false;
	}

	TString sType = *mVars(iVarPos).GetStringPtr();
	sType.ToLowerCase();

	COGLAnimScale* pAnim = new COGLAnimScale;
	pAnim->Set(float(pfVec[0]), float(pfVec[1]), float(pfVec[2]));

	if ((sType == "sinus") || (sType == "sinus2"))
	{
		if (iVarCount < 1)
		{
			rCB.GetErrorList().GeneralError("Expect a frequency.", iLine, iPos);
			return false;
		}

		TCVScalar dFreq;

		if (!mVars(iVarPos + 1).CastToScalar(dFreq, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Frequency parameter has to be a scalar.", iLine, iPos);
			return false;
		}

		pAnim->SetFreq(float(dFreq));

		if (sType == "sinus")
		{
			pAnim->SetMode(COGLAnimScale::/*EAnimMode::*/ SINUS);
		}
		else
		{
			pAnim->SetMode(COGLAnimScale::/*EAnimMode::*/ SINUS2);
		}

		iVarPos   += 2;
		iVarCount -= 2;
	}
	else if (sType == "constant")
	{
		if (iVarCount < 1)
		{
			rCB.GetErrorList().GeneralError("Expect scaling frequency.", iLine, iPos);
			return false;
		}

		TCVScalar dFreq;

		if (!mVars(iVarPos + 1).CastToScalar(dFreq, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Frequency parameter has to be a scalar.", iLine, iPos);
			return false;
		}

		pAnim->SetFreq(float(dFreq));
		pAnim->SetMode(COGLAnimScale::/*EAnimMode::*/ CONSTANT);

		iVarPos   += 2;
		iVarCount -= 2;
	}
	else
	{
		pAnim->SetFreq(0.0f);
		pAnim->SetMode(COGLAnimScale::/*EAnimMode::*/ NONE);
	}

	// Is direct/indirect bool give?
	if (iVarCount > 0)
	{
		if (!mVars(iVarPos).CastToCounter(iDirect))
		{
			rCB.GetErrorList().GeneralError("Expect direct/indirect parameter to be a bool.", iLine, iPos);
			return false;
		}

		if ((iDirect == 0) && !pFrame)
		{
			rCB.GetErrorList().GeneralError("Indirect animated scaling only available if a frame is given as first parameter.", iLine, iPos);
			return false;
		}
	}

	if (pAnim)
	{
		COGLBEReference Ref = rCB.GetOGLDrawBase()->GetSceneRepository()->New(pAnim);
		if (pFrame)
		{
			if (iDirect)
			{
				pFrame->SetAnimateFrame(Ref);
			}
			else
			{
				pAnim->SetFrame(*mVars(0).GetScenePtr());
				pAnim->EnableFrame(true);
				rCB.GetOGLDrawBase()->DrawBaseElement(Ref);
				rVar = Ref;
			}
		}
		else
		{
			rCB.GetOGLDrawBase()->DrawBaseElement(Ref);
			rVar = Ref;
		}
	}

	return true;
}
