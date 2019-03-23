////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_Visualize.cpp
//
// summary:   Implements the function visualize class
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

#include "Func_Visualize.h"
#include "CluTec.Base/Exception.h"

typedef unsigned char uchar;

//////////////////////////////////////////////////////////////////////
/// Set Horizontal and Vertical Align Function for Images

bool  SetBMPAlignFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());
	TCVScalar dHVal, dVVal;

	if (iVarCount == 2)
	{
		if (!mVars(0).CastToScalar(dHVal, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (!mVars(1).CastToScalar(dVVal, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		rCB.GetHBMPAlign() = float(dHVal);
		rCB.GetVBMPAlign() = float(dVVal);
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(2, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Origin Position for Images

bool  SetBMPPosFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int i, iVarCount = int(mVars.Count());
	TCVScalar pdVal[3];

	if (iVarCount == 3)
	{
		for (i = 0; i < 3; i++)
		{
			if (!mVars(i).CastToScalar(pdVal[i], rCB.GetSensitivity()))
			{
				rCB.GetErrorList().InvalidParType(mVars(i), i + 1, iLine, iPos);
				return false;
			}
		}

		rCB.GetBMPPos().Set(float(pdVal[0]), float(pdVal[1]), float(pdVal[2]));
	}
	else if (iVarCount == 1)
	{
		if (mVars(0).BaseType() != PDT_MULTIV)
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		TMultiV& vA = *mVars(0).GetMultiVPtr();
		TMultiV vB;

		rCB.CastMVtoE3(vA, vB);
		rCB.GetBMPPos().Set(float(vB[rCB.GetE3GABase().iE1]),
				float(vB[rCB.GetE3GABase().iE2]),
				float(vB[rCB.GetE3GABase().iE3]));
	}
	else
	{
		int piPar[] = { 1, 3 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Bitmap as Texture Function

bool  SetBMPTextureFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	try
	{
		CStrMem csFilename;
		TVarList& mVars = *rPars.GetVarListPtr();

		int iVarCount = int(mVars.Count());
		COGLVertex xA;
		TCVCounter iDoRepeat     = 1;
		TCVCounter iIsText       = 0;
		TCVCounter iUseAspect    = 0;
		TCVCounter iAllowScaleUp = 1, iAllowScaleDown = 1;
		TCVScalar dTextImgScale  = 1.0, dWidth = 1.0, dHeight = 1.0;
		TCVScalar dAlignX        = 0.5, dAlignY = 0.5;
		bool bUseScale           = false;

		if (iVarCount > 8)
		{
			int piPar[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };

			rCB.GetErrorList().WrongNoOfParams(piPar, 9, iLine, iPos);
			return false;
		}

		if (iVarCount == 0)
		{
			// Set empty texture element to reset texture
			COGLTexture* pTex = new COGLTexture;

			// Reset Texture Frame
			COGLFrame* pFrame = new COGLFrame();
			pFrame->SetFrameMode(COGLFrame::TEXTURE);
			pFrame->EnableMultiplyMatrix(false);

			rCB.GetOGLDrawBase()->DrawBaseElement(rCB.GetOGLDrawBase()->GetSceneRepository()->New(pFrame));
			rCB.GetOGLDrawBase()->DrawBaseElement(rCB.GetOGLDrawBase()->GetSceneRepository()->New(pTex));

			return true;
		}

		if (mVars(0).BaseType() == PDT_IMAGE)
		{
			if (iVarCount >= 2)
			{
				if (!mVars(1).CastToCounter(iDoRepeat))
				{
					rCB.GetErrorList().GeneralError(
							"Expect second parameter to indicate whether image is repetitive texture (true), "
							"or single image texture (false).", iLine, iPos);
					return false;
				}
			}

			if (iVarCount >= 3)
			{
				if (!mVars(2).CastToCounter(iIsText))
				{
					rCB.GetErrorList().GeneralError(
							"Expect third parameter to indicate whether given image is a text image (true),"
							" or not (false).", iLine, iPos);
					return false;
				}
			}

			if (iVarCount >= 4)
			{
				if (!mVars(3).CastToCounter(iUseAspect))
				{
					rCB.GetErrorList().GeneralError(
							"Expect fourth parameter to indicate whether aspect ratio of given image is to be corrected for (true), "
							"or not (false).", iLine, iPos);
					return false;
				}
			}

			if (iVarCount >= 5)
			{
				if (!mVars(4).CastToScalar(dTextImgScale))
				{
					rCB.GetErrorList().GeneralError(
							"Expect fifth parameter to give overall texture scale."
							, iLine, iPos);
					return false;
				}

				if (dTextImgScale <= 0.0)
				{
					rCB.GetErrorList().GeneralError("Invalid texture scale.", iLine, iPos);
					return false;
				}

				bUseScale = true;
			}

			if (iVarCount >= 6)
			{
				if (iUseAspect == 0)
				{
					rCB.GetErrorList().GeneralError(
							"Aspect ratio correction width and height parameters may only be given if aspect ratio correction is selected."
							, iLine, iPos);
					return false;
				}

				if (mVars(5).BaseType() != PDT_VARLIST)
				{
					rCB.GetErrorList().GeneralError(
							"Expect 6th parameter to be a list containing width and height for aspect ratio correction."
							, iLine, iPos);
					return false;
				}

				TVarList& rList = *mVars(5).GetVarListPtr();
				if (rList.Count() != 2)
				{
					rCB.GetErrorList().GeneralError(
							"Expect 6th parameter to give width for aspect ratio correction."
							, iLine, iPos);
					return false;
				}

				if (!rList(0).CastToScalar(dWidth))
				{
					rCB.GetErrorList().GeneralError(
							"Expect 1st element in 6th parameter to give width for aspect ratio correction."
							, iLine, iPos);
					return false;
				}

				if (!rList(1).CastToScalar(dHeight))
				{
					rCB.GetErrorList().GeneralError(
							"Expect 2nd element in 6th parameter to give height for aspect ratio correction."
							, iLine, iPos);
					return false;
				}

				if (dWidth <= 0.0)
				{
					rCB.GetErrorList().GeneralError("Invalid aspect correction width.", iLine, iPos);
					return false;
				}

				if (dHeight <= 0.0)
				{
					rCB.GetErrorList().GeneralError("Invalid aspect correction height.", iLine, iPos);
					return false;
				}
			}

			if (iVarCount >= 7)
			{
				if (iUseAspect == 0)
				{
					rCB.GetErrorList().GeneralError(
							"Align parameters may only be given if aspect ratio correction is selected."
							, iLine, iPos);
					return false;
				}

				if (mVars(6).BaseType() != PDT_VARLIST)
				{
					rCB.GetErrorList().GeneralError(
							"Expect 7th parameter to be a list containing horizontal and vertical align factors."
							, iLine, iPos);
					return false;
				}

				TVarList& rList = *mVars(6).GetVarListPtr();
				if (rList.Count() != 2)
				{
					rCB.GetErrorList().GeneralError(
							"Expect 7th parameter to be a list containing horizontal and vertical align factors."
							, iLine, iPos);
					return false;
				}

				if (!rList(0).CastToScalar(dAlignX))
				{
					rCB.GetErrorList().GeneralError(
							"Expect 1st element of 7th parameter to give horizontal align factor in range [0,1]."
							, iLine, iPos);
					return false;
				}

				if (!rList(1).CastToScalar(dAlignY))
				{
					rCB.GetErrorList().GeneralError(
							"Expect 2nd element of 7th parameter to give vectical align factor in range [0,1]."
							, iLine, iPos);
					return false;
				}

				if ((dAlignX < 0.0) || (dAlignX > 1.0))
				{
					rCB.GetErrorList().GeneralError("Horizontal align factor has to be in range [0,1].", iLine, iPos);
					return false;
				}

				if ((dAlignY < 0.0) || (dAlignY > 1.0))
				{
					rCB.GetErrorList().GeneralError("Vertical align factor has to be in range [0,1].", iLine, iPos);
					return false;
				}
			}

			if (iVarCount >= 8)
			{
				if (iUseAspect == 0)
				{
					rCB.GetErrorList().GeneralError(
							"Scale enable parameters may only be given if aspect ratio correction is selected."
							, iLine, iPos);
					return false;
				}

				if (mVars(7).BaseType() != PDT_VARLIST)
				{
					rCB.GetErrorList().GeneralError(
							"Expect 8th parameter to be a list containing scale enable flags for up- and down-scaling."
							, iLine, iPos);
					return false;
				}

				TVarList& rList = *mVars(7).GetVarListPtr();
				if (rList.Count() != 2)
				{
					rCB.GetErrorList().GeneralError(
							"Expect 8th parameter to be a list containing scale enable flags for up- and down-scaling."
							, iLine, iPos);
					return false;
				}

				if (!rList(0).CastToCounter(iAllowScaleUp))
				{
					rCB.GetErrorList().GeneralError(
							"Expect 1st element of 8th parameter to give up-scaling flag."
							, iLine, iPos);
					return false;
				}

				if (!rList(1).CastToCounter(iAllowScaleDown))
				{
					rCB.GetErrorList().GeneralError(
							"Expect 2nd element of 8th parameter to give down-scaling flag."
							, iLine, iPos);
					return false;
				}
			}

			TImage& oglImage = *mVars(0).GetImagePtr();
			if (oglImage.IsValid())
			{
				COGLTexture* pTex      = new COGLTexture;
				COGLBEReference TexRef = rCB.GetOGLDrawBase()->GetSceneRepository()->New(pTex);

				if (iIsText)
				{
					pTex->SetTexture(oglImage, 1.0f, 0, false, true, true);
					pTex->SetWrapType((iDoRepeat ? COGLTexture::EWrapType::REPEAT : COGLTexture::EWrapType::CLAMP));
					pTex->SetBlendType(COGLTexture::EBlendType::INTERPOLATE, COGLTexture::EBlendType::ADD);
					pTex->EnableInterpolate(false);
					rCB.GetOGLDrawBase()->DrawBaseElement(TexRef);
				}
				else
				{
					pTex->SetTexture(oglImage, 1.0f, 0, true, true, true);
					pTex->SetWrapType((iDoRepeat ? COGLTexture::EWrapType::REPEAT : COGLTexture::EWrapType::CLAMP));
					pTex->SetBlendType(COGLTexture::EBlendType::MODULATE, COGLTexture::EBlendType::MODULATE);
					rCB.GetOGLDrawBase()->DrawBaseElement(TexRef);
				}

				if (iUseAspect || bUseScale)
				{
					COGLFrame* pFrame = new COGLFrame();
					pFrame->SetFrameMode(COGLFrame::TEXTURE);
					pFrame->SetOrigin(-dAlignX, -dAlignY, 0);
					pFrame->EnableOrigin(true);
					//pFrame->Translate( 0.5, 0.5, 0 );

					if (iUseAspect)
					{
						int iWidth, iHeight;
						pTex->GetTextureSize(iWidth, iHeight);
						pFrame->AutoScaleToPixelSize(dWidth, dHeight, double(iWidth), double(iHeight),
								iAllowScaleUp != 0, iAllowScaleDown != 0);
						pFrame->Scale(1.0 / dTextImgScale, 1.0 / dTextImgScale, 1.0);

						//double dAspect = double( iHeight ) / double( iWidth );
						//pFrame->Scale( 1.0 / dTextImgScale, 1.0 / (dTextImgScale * dAspect), 1.0 );
					}
					else
					{
						pFrame->Scale(1.0 / dTextImgScale, 1.0 / dTextImgScale, 1.0);
					}
					//pFrame->Translate( -0.5, -0.5, 0 );

					///////////////////////////////////////////////////////////
					//// !!!!!!!! IMPORTANT !!!!!!!!!!!!!!!!!
					//// Need to set first texture and then texture frame to ensure that the
					//// correct texture unit is selected.
					//rCB.GetOGLDrawBase()->DrawBaseElement( TexRef );
					rCB.GetOGLDrawBase()->DrawBaseElement(rCB.GetOGLDrawBase()->GetSceneRepository()->New(pFrame));
				}
			}
			else
			{
				rCB.GetErrorList().GeneralError("Invalid Image.", iLine, iPos);
				return false;
			}
		}
		else if (mVars(0).BaseType() == PDT_STRING)
		{
			TCVScalar dAspect, dScale = 0.5;

			if (iVarCount >= 2)
			{
				if (!mVars(1).CastToScalar(dScale))
				{
					rCB.GetErrorList().GeneralError("Expect second optional parameter to give scale of texture.", iLine, iPos);
					return false;
				}
			}

			TString& csText = *mVars(0).GetStringPtr();
			TImage imgText  = ((CImageRepository*)::GetImageRepositoryPtr())->New();

			if (!imgText.IsValid())
			{
				rCB.GetErrorList().GeneralError("Could not create text image.", iLine, iPos);
				return false;
			}

			string sError;
			if (!rCB.GetOGLText().GetTextImage(*((COGLImage*) imgText), sError, csText.Str(), 0.0))
			{
				sError = "Error: " + sError;
				rCB.GetErrorList().GeneralError(sError.c_str(), iLine, iPos);
				return false;
			}

			// Set text to active color

			imgText->FlushRGB(rCB.GetOGLDrawBase()->GetColor());

			COGLTexture* pTex = new COGLTexture();

			if (!pTex)
			{
				rCB.GetErrorList().GeneralError("Out of memory while creating texture.", iLine, iPos);
				return false;
			}

			int iWidth, iHeight;
			pTex->SetName(csText.Str());
			pTex->SetTexture(imgText, 1.0f, 0, false, true, true);
			pTex->SetBlendType(COGLTexture::EBlendType::INTERPOLATE, COGLTexture::EBlendType::ADD);
			pTex->GetTextureSize(iWidth, iHeight);
			dAspect = double(iHeight) / double(iWidth);

			COGLFrame* pFrame = new COGLFrame();
			pFrame->SetFrameMode(COGLFrame::TEXTURE);
			pFrame->Translate(0.5, 0.5, 0);
			pFrame->Scale(1.0 / dScale, 1.0 / (dScale * dAspect), 1.0);
			pFrame->Translate(-0.5, -0.5, 0);

			//COGLScene *pScene = new COGLScene();
			//pScene->SetName( "Text Texture" );
			//pScene->Add( rCB.GetOGLDrawBase()->GetSceneRepository()->New( pTex ) );
			//pScene->Add( rCB.GetOGLDrawBase()->GetSceneRepository()->New( pFrame ) );

			//rCB.GetOGLDrawBase()->DrawBaseElement( rCB.GetOGLDrawBase()->GetSceneRepository()->New( pScene ) );
			rCB.GetOGLDrawBase()->DrawBaseElement(rCB.GetOGLDrawBase()->GetSceneRepository()->New(pFrame));
			rCB.GetOGLDrawBase()->DrawBaseElement(rCB.GetOGLDrawBase()->GetSceneRepository()->New(pTex));
		}
		else
		{
			rCB.GetErrorList().GeneralError("Expect first element to be either an image or a string.", iLine, iPos);
			return false;
		}
	}
	catch (Clu::CIException& ex)
	{
		rCB.GetErrorList().GeneralError(CLU_S "Error while setting bitmap as texture. Reason: " << ex.ToString(), iLine, iPos);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Bitmap Scale Function

bool  SetBMPScaleFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int i, iVarCount = int(mVars.Count());
	TCVScalar pdVal[2];

	if ((iVarCount < 1) || (iVarCount > 2))
	{
		int piPar[] = { 1, 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	for (i = 0; i < iVarCount; ++i)
	{
		if (!mVars(i).CastToScalar(pdVal[i], rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(mVars(i), i + 1, iLine, iPos);
			return false;
		}

		if (pdVal[i] <= TCVScalar(0))
		{
			rCB.GetErrorList().InvalidParVal(mVars(i), i + 1, iLine, iPos);
			return false;
		}
	}

	if (iVarCount == 1)
	{
		pdVal[1] = pdVal[0];
	}

	COGLPixelZoom* pPixelZoom = new COGLPixelZoom;

	pPixelZoom->Set(float(pdVal[0]), float(pdVal[1]));

	COGLBEReference Ref = rCB.GetOGLDrawBase()->GetSceneRepository()->New(pPixelZoom);
	rCB.GetOGLDrawBase()->DrawBaseElement(Ref);
	rVar = Ref;

	//m_fCurBitmapScale = float(dVal);

	return true;
}
