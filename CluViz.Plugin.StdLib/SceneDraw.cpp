////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      SceneDraw.cpp
//
// summary:   Implements the scene draw class
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

#include "SceneDraw.h"
#include "CluTec.Base/Exception.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Get image of text string

bool GetTextImage(TImage& refImage, CCLUCodeBase& rCB, TString& sText, TOGLColor& colText, TString& sError, double dAlign)
{
	sError = "";

	// Create Text texture
	refImage = ((CImageRepository*)::GetImageRepositoryPtr())->New();

	if (!refImage.IsValid())
	{
		sError = "Could not create text image.";
		return false;
	}

	string sTextError;
	if (!rCB.GetOGLText().GetTextImage(*((COGLImage*) refImage), sTextError, sText.Str(), dAlign))
	{
		sTextError = "Error: " + sTextError;
		sError     = sTextError.c_str();
		return false;
	}

	// Set text to active color
	refImage->FlushRGB(colText);

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Set a text texture

bool SetTextureText(CCLUCodeBase& rCB, TString& sText, TOGLColor& colText, double dWidth, double dHeight, TString& sError, double dAlign)
{
	try
	{
		sError = "";

		TImage refImage;
		COGLDrawBase& rDB     = *rCB.GetOGLDrawBase();
		COGLBERepository& rSR = *rDB.GetSceneRepository();

		if (!GetTextImage(refImage, rCB, sText, colText, sError, dAlign))
		{
			return false;
		}

		COGLTexture* pTex = new COGLTexture;
		pTex->SetTexture(refImage, 1.0f, 0, false, false, false);
		pTex->SetWrapType(COGLTexture::EWrapType::CLAMP);
		pTex->SetBlendType(COGLTexture::EBlendType::INTERPOLATE, COGLTexture::EBlendType::ADD);

		COGLFrame* pFrame = new COGLFrame();
		pFrame->SetFrameMode(COGLFrame::TEXTURE);
		pFrame->SetOrigin(-0.5, -0.5, 0);
		pFrame->EnableOrigin(true);

		int iWidth, iHeight;
		pTex->GetTextureSize(iWidth, iHeight);
		pFrame->AutoScaleToPixelSize(dWidth, dHeight, double(iWidth), double(iHeight), true, true);
		//pFrame->Scale( 1.0 / dTextImgScale, 1.0 / dTextImgScale, 1.0 );

		rDB.DrawBaseElement(rSR.New(pTex));
		rDB.DrawBaseElement(rSR.New(pFrame));

		return true;
	}
	catch (Clu::CIException& ex)
	{
		 throw CLU_EXCEPTION_NEST("Error setting texture text", std::move(ex));
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Switch off texture mapping

bool SetTextureEmpty(CCLUCodeBase& rCB, TString& sError)
{
	sError = "";

	COGLDrawBase& rDB     = *rCB.GetOGLDrawBase();
	COGLBERepository& rSR = *rDB.GetSceneRepository();

	rDB.DrawBaseElement(rSR.New(new COGLTexture));

	return true;
}
