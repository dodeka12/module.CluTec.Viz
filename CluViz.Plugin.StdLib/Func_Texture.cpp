////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_Texture.cpp
//
// summary:   Implements the function texture class
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

#include "Func_Texture.h"

#include "CluTec.Base/Exception.h"

#include "CluTec.Viz.Draw\OGLTexture.h"
#include "CluTec.Viz.Base\TensorOperators.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Create a Texture Variable

bool TextureFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	try
	{
		TVarList& mVars = *rPars.GetVarListPtr();

		int iVarCount = int(mVars.Count());

		if ((iVarCount != 1) && (iVarCount != 2))
		{
			int piPar[] = { 1, 2 };

			rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
			return false;
		}

		if (mVars(0).BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().GeneralError("Expect name of texture as first parameter.", iLine, iPos);
			return false;
		}

		COGLTexture::ETextureType eTexType = COGLTexture::ETextureType::TEX_2D;

		if (iVarCount >= 2)
		{
			if (mVars(1).BaseType() != PDT_STRING)
			{
				rCB.GetErrorList().GeneralError("Expect second parameter to give type of texture: \"tex_2d\" or \"tex_3d\".", iLine, iPos);
				return false;
			}

			CStrMem sTexType = *mVars(1).GetStringPtr();
			if (sTexType == "tex_2d")
			{
				eTexType = COGLTexture::ETextureType::TEX_2D;
			}
			else if (sTexType == "tex_3d")
			{
				eTexType = COGLTexture::ETextureType::TEX_3D;
			}
			else
			{
				rCB.GetErrorList().GeneralError("Expect second parameter to give type of texture: \"tex_2d\" or \"tex_3d\".", iLine, iPos);
				return false;
			}
		}

		COGLTexture* pTex = new COGLTexture(eTexType);

		if (!pTex)
		{
			rCB.GetErrorList().GeneralError("Out of memory while creating texture variable.", iLine, iPos);
			return false;
		}

		pTex->SetName(mVars(0).GetStringPtr()->Str());
		//pTex->GenTexture( 1.0f, 0 );

		COGLBEReference SceneRef = rCB.GetOGLDrawBase()->GetSceneRepository()->New(pTex);

		rVar = SceneRef;

		return true;
	}
	catch (Clu::CIException& ex)
	{
		 throw CLU_EXCEPTION_NEST("Error calling 'Texture'", std::move(ex));
	}
}

/////////////////////////////////////////////////////////////////////////////////////
// Set Texture Image

bool SetTextureImageFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if ((iVarCount < 2) || (iVarCount > 7))
	{
		int piPar[] = { 2, 3, 4, 5, 6, 7 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 6, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a texture variable as first parameter.", iLine, iPos);
		return false;
	}

	TScene& rScene = *mVars(0).GetScenePtr();

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

	if (mVars(1).BaseType() != PDT_IMAGE)
	{
		rCB.GetErrorList().GeneralError("Expect second parameter to be an image.", iLine, iPos);
		return false;
	}

	TImage& oglImage = *mVars(1).GetImagePtr();
	if (!oglImage.IsValid())
	{
		rCB.GetErrorList().GeneralError("Given image is invalid.", iLine, iPos);
		return false;
	}

	TCVCounter iTexUnit = 0;
	TCVScalar dMipMap   = 1.0;
	TCVScalar dBorder   = 0.0;
	TCVScalar dNorm     = 1.0;
	TCVScalar dSplit    = 0.0;

	if (iVarCount > 2)
	{
		if (!mVars(2).CastToScalar(dMipMap, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Expect as third parameter true/false whether image is to be mipmapped.", iLine, iPos);
			return false;
		}
	}

	if (iVarCount > 3)
	{
		if (!mVars(3).CastToCounter(iTexUnit))
		{
			rCB.GetErrorList().GeneralError("Expect as fourth parameter the texture unit.", iLine, iPos);
			return false;
		}

		if ((iTexUnit < 1) || (uint(iTexUnit) > pTex->GetTextureUnitCnt()))
		{
			char pcText[200];
			sprintf_s(pcText, "Texture unit has to lie in range [1, %d].", pTex->GetTextureUnitCnt());
			rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
			return false;
		}

		// In CLUScript Texture units start counting with 1
		--iTexUnit;
	}

	if (iVarCount > 4)
	{
		if (!mVars(4).CastToScalar(dBorder, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Expect as fifth parameter the border flag.", iLine, iPos);
			return false;
		}
	}

	if (iVarCount > 5)
	{
		if (!mVars(5).CastToScalar(dSplit, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Expect as sixth parameter the split texture flag. This value has no effect as the feature is deprecated.", iLine, iPos);
			return false;
		}
	}

	if (iVarCount > 6)
	{
		if (!mVars(6).CastToScalar(dNorm, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Expect as seventh parameter the normalized texture flag.", iLine, iPos);
			return false;
		}
	}

	try
	{
		pTex->SetTexture(oglImage, 1.0f, (uint) iTexUnit, (dMipMap != 0.0), (dBorder != 0.0), (dNorm != 0.0));
	}
	catch (Clu::CIException& xEx)
	{
		rCB.GetErrorList().GeneralError("Error while setting texture. Reason:", iLine, iPos);

		Clu::CIException xNested(std::move(xEx));
		while (xNested.IsValid())
		{
			rCB.GetErrorList().GeneralError(xNested.ToString(), iLine, iPos);
			xNested = xNested.GetNested();
		}

		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 	Sets a texture 3D function.
/// </summary>
///
/// <param name="rCB">  	[in,out] The cb. </param>
/// <param name="rVar"> 	[in,out] The variable. </param>
/// <param name="rPars">	[in,out] The pars. </param>
/// <param name="iLine">	Zero-based index of the line. </param>
/// <param name="iPos"> 	Zero-based index of the position. </param>
///
/// <returns>	True if it succeeds, false if it fails. </returns>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool SetTexture3dFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if ((iVarCount < 2) || (iVarCount > 5))
	{
		int piPar[] = { 2, 3, 4, 5 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 4, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a texture variable as first parameter.", iLine, iPos);
		return false;
	}

	TScene& rScene = *mVars(0).GetScenePtr();

	if (!rScene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Given texture variable is invalid.", iLine, iPos);
		return false;
	}

	COGLTexture* pTex = dynamic_cast<COGLTexture*>((COGLBaseElement*) rScene);
	if (!pTex)
	{
		rCB.GetErrorList().GeneralError("Given texture variable is invalid.", iLine, iPos);
		return false;
	}

	TTensor* pTensor;
	TTensor xTensorDummy;

	if (mVars(1).BaseType() == PDT_TENSOR)
	{
		pTensor = mVars(1).GetTensorPtr();
	}
	else if (mVars(1).BaseType() == PDT_TENSOR_IDX)
	{
		TTensorIdx& rTIdx = *mVars(1).GetTensorIdxPtr();

		if (!MakeTensor(xTensorDummy, rTIdx))
		{
			rCB.GetErrorList().GeneralError("Error generating tensor.", iLine, iPos);
			return false;
		}
		pTensor = &xTensorDummy;
	}
	else
	{
		rCB.GetErrorList().GeneralError("Expect as second parameter a tensor with the 3d texture data.", iLine, iPos);
		return false;
	}

	TTensor& rTensor = *pTensor;

	int iValence = rTensor.Valence();
	if ((iValence != 3) && (iValence != 4))
	{
		rCB.GetErrorList().GeneralError("Valence of tensor has to be 3 or 4.", iLine, iPos);
		return false;
	}

	TCVCounter iTexUnit = 0;
	TCVScalar dMipMap   = 0.0;
	TCVScalar dNorm     = 1.0;

	if (iVarCount > 2)
	{
		if (!mVars(2).CastToScalar(dMipMap, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Expect as third parameter true/false whether image is to be mipmapped.", iLine, iPos);
			return false;
		}
	}

	if (iVarCount > 3)
	{
		if (!mVars(3).CastToCounter(iTexUnit))
		{
			rCB.GetErrorList().GeneralError("Expect as fourth parameter the texture unit.", iLine, iPos);
			return false;
		}

		if ((iTexUnit < 1) || (uint(iTexUnit) > pTex->GetTextureUnitCnt()))
		{
			char pcText[200];
			sprintf_s(pcText, "Texture unit has to lie in range [1, %d].", pTex->GetTextureUnitCnt());
			rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
			return false;
		}

		// In CLUScript Texture units start counting with 1
		--iTexUnit;
	}

	if (iVarCount > 4)
	{
		if (!mVars(4).CastToScalar(dNorm, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Expect as fifth parameter the normalized texture flag.", iLine, iPos);
			return false;
		}
	}

	if (!pTex->SetTexture3d(rTensor, (uint) iTexUnit, (dMipMap != 0.0), (dNorm != 0.0)))
	{
		rCB.GetErrorList().GeneralError("Error creating 3d texture.", iLine, iPos);
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////
// Set Texture Wrap Type

bool SetTextureWrapTypeFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
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
		rCB.GetErrorList().GeneralError("Expect a texture variable as first parameter.", iLine, iPos);
		return false;
	}

	TScene& rScene = *mVars(0).GetScenePtr();

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

	string sWrapType;
	if (mVars(1).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("Expect as second parameter string indicating wrapping type of texture ('clamp'|'repeat').", iLine, iPos);
		return false;
	}

	sWrapType = mVars(1).GetStringPtr()->Str();

	if (sWrapType == "clamp")
	{
		pTex->SetWrapType(COGLTexture::EWrapType::CLAMP);
	}
	else if (sWrapType == "repeat")
	{
		pTex->SetWrapType(COGLTexture::EWrapType::REPEAT);
	}
	else
	{
		rCB.GetErrorList().GeneralError("Expect as second parameter string indicating wrapping type of texture ('clamp'|'repeat').", iLine, iPos);
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////
// Set Texture Blend Type

bool SetTextureBlendTypeFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
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
		rCB.GetErrorList().GeneralError("Expect a texture variable as first parameter.", iLine, iPos);
		return false;
	}

	TScene& rScene = *mVars(0).GetScenePtr();

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

	string sBlendTypeColor, sBlendTypeAlpha;
	COGLTexture::EBlendType eBTColor, eBTAlpha;

	if (mVars(1).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("Expect as second parameter string indicating blending type of texture color: "
						"\"interpolate\" or \"modulate\" or \"add\" or \"mix\" or \"inverse modulate\".", iLine, iPos);
		return false;
	}

	if (mVars(2).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("Expect as third parameter string indicating blending type of texture transparency (alpha): "
						"\"interpolate\" or \"modulate\" or \"add\" or \"mix\" or \"inverse modulate\".", iLine, iPos);
		return false;
	}

	sBlendTypeColor = mVars(1).GetStringPtr()->Str();
	sBlendTypeAlpha = mVars(2).GetStringPtr()->Str();

	if (sBlendTypeColor == "interpolate")
	{
		eBTColor = COGLTexture::EBlendType::INTERPOLATE;
	}
	else if (sBlendTypeColor == "modulate")
	{
		eBTColor = COGLTexture::EBlendType::MODULATE;
	}
	else if (sBlendTypeColor == "add")
	{
		eBTColor = COGLTexture::EBlendType::ADD;
	}
	else if (sBlendTypeColor == "mix")
	{
		eBTColor = COGLTexture::EBlendType::MIX;
	}
	else if (sBlendTypeColor == "inverse modulate")
	{
		eBTColor = COGLTexture::EBlendType::INVMOD;
	}
	else
	{
		rCB.GetErrorList().GeneralError(
				"Expect as second parameter string indicating blending type of texture color: \"interpolate\" or \"modulate\" or \"add\" or \"mix\" or \"inverse modulate\".",
				iLine,
				iPos);
		return false;
	}

	if (sBlendTypeAlpha == "interpolate")
	{
		eBTAlpha = COGLTexture::EBlendType::INTERPOLATE;
	}
	else if (sBlendTypeAlpha == "modulate")
	{
		eBTAlpha = COGLTexture::EBlendType::MODULATE;
	}
	else if (sBlendTypeAlpha == "add")
	{
		eBTAlpha = COGLTexture::EBlendType::ADD;
	}
	else if (sBlendTypeAlpha == "mix")
	{
		eBTAlpha = COGLTexture::EBlendType::MIX;
	}
	else if (sBlendTypeAlpha == "inverse modulate")
	{
		eBTAlpha = COGLTexture::EBlendType::INVMOD;
	}
	else
	{
		rCB.GetErrorList().GeneralError(
				"Expect as third parameter string indicating blending type of texture transparency (alpha): \"interpolate\" or \"modulate\" or \"add\" or \"mix\" or \"inverse modulate\".",
				iLine,
				iPos);
		return false;
	}

	pTex->SetBlendType(eBTColor, eBTAlpha);

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////
// Set Texture Unit

bool SetTextureUnitFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
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
		rCB.GetErrorList().GeneralError("Expect a texture variable as first parameter.", iLine, iPos);
		return false;
	}

	TScene& rScene = *mVars(0).GetScenePtr();

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

	TCVCounter iTexUnit = 0;
	if (!mVars(1).CastToCounter(iTexUnit))
	{
		rCB.GetErrorList().GeneralError("Expect as second parameter the texture unit.", iLine, iPos);
		return false;
	}

	if ((iTexUnit < 1) || (uint(iTexUnit) > pTex->GetTextureUnitCnt()))
	{
		char pcText[200];
		sprintf_s(pcText, "Texture unit has to lie in range [1, %d].", pTex->GetTextureUnitCnt());
		rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
		return false;
	}

	// In CLUScript the texture unit starts counting at 1.
	--iTexUnit;

	pTex->SetTextureUnit((uint) iTexUnit);

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////
// Enable Texture Interpolate

bool EnableTextureInterpolateFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
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
		rCB.GetErrorList().GeneralError("Expect a texture variable as first parameter.", iLine, iPos);
		return false;
	}

	TScene& rScene = *mVars(0).GetScenePtr();

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

	TCVCounter iInterpolate = 1;
	if (!mVars(1).CastToCounter(iInterpolate))
	{
		rCB.GetErrorList().GeneralError("Expect as second parameter true/false.", iLine, iPos);
		return false;
	}

	pTex->EnableInterpolate(iInterpolate != 0);

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////
bool EnableTextureForPickingFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
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
		rCB.GetErrorList().GeneralError("Expect a texture variable as first parameter.", iLine, iPos);
		return false;
	}

	TScene& rScene = *mVars(0).GetScenePtr();

	if (!rScene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Given texture variable is invalid.", iLine, iPos);
		return false;
	}

	COGLTexture* pTex = dynamic_cast<COGLTexture*>((COGLBaseElement*) rScene);
	if (!pTex)
	{
		rCB.GetErrorList().GeneralError("Given texture variable is invalid.", iLine, iPos);
		return false;
	}

	TCVCounter iEnable = 1;
	if (!mVars(1).CastToCounter(iEnable))
	{
		rCB.GetErrorList().GeneralError("Expect as second parameter true/false.", iLine, iPos);
		return false;
	}

	pTex->EnableForPicking(iEnable != 0);

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////
// Get the textures internal data & pixel type. In addition a flag representing Normalized Data is returned as third result

bool GetTextureTypeFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	// check the var count, we need only 1 Parameter for this function
	if (iVarCount != 1)
	{
		int piPar[] = { 1 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	// See if have a matching parameter (every texture is of base type "PDT_SCENE"
	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("Expect a texture variable as first parameter.", iLine, iPos);
		return false;
	}

	TScene& rScene = *mVars(0).GetScenePtr();

	// check if this is a valid scene
	if (!rScene.IsValid())
	{
		rCB.GetErrorList().GeneralError("Given texture variable is invalid.", iLine, iPos);
		return false;
	}

	// Try to cast the scene to a valid OGLTexture Object
	COGLTexture* pTex = dynamic_cast<COGLTexture*>((COGLBaseElement*) rScene);
	if (!pTex)
	{
		rCB.GetErrorList().GeneralError("Given texture variable is invalid.", iLine, iPos);
		return false;
	}

	int iDataType;
	int iPixType;
	bool bNorm;

	// Retrieve Pixel & Data Type and the Normalized flag
	pTex->GetTextureType(iPixType, iDataType, bNorm);

	rVar.New(PDT_VARLIST);
	TVarList& rListName = *rVar.GetVarListPtr();

	// translate the result into strings and store them into the result list.
	rListName.Set(3);

	switch (iPixType)
	{
	case CLUVIZ_IMG_RGB:
		rListName(0) = CStrMem("rgb");
		break;

	case CLUVIZ_IMG_RGBA:
		rListName(0) = CStrMem("rgba");
		break;

	case CLUVIZ_IMG_BGR:
		rListName(0) = CStrMem("bgr");
		break;

	case CLUVIZ_IMG_BGRA:
		rListName(0) = CStrMem("bgra");
		break;

	case CLUVIZ_IMG_LUMINANCE:
		rListName(0) = CStrMem("lum");
		break;

	case CLUVIZ_IMG_LUMINANCE_ALPHA:
		rListName(0) = CStrMem("luma");
		break;

	default:
		rListName(0) = CStrMem("unknown");
		break;
	}

	switch (iDataType)
	{
	case CLUVIZ_IMG_BYTE:
		rListName(1) = CStrMem("8");
		break;

	case CLUVIZ_IMG_UNSIGNED_BYTE:
		rListName(1) = CStrMem("u8");
		break;

	case CLUVIZ_IMG_SHORT:
		rListName(1) = CStrMem("16");
		break;

	case CLUVIZ_IMG_UNSIGNED_SHORT:
		rListName(1) = CStrMem("u16");
		break;

	case CLUVIZ_IMG_INT:
		rListName(1) = CStrMem("32");
		break;

	case CLUVIZ_IMG_UNSIGNED_INT:
		rListName(1) = CStrMem("u32");
		break;

	case CLUVIZ_IMG_FLOAT:
		rListName(1) = CStrMem("float");
		break;

	case CLUVIZ_IMG_DOUBLE:
		rListName(1) = CStrMem("double");
		break;

	default:
		rListName(1) = CStrMem("unknown");
		break;
	}

	rListName(2) = bNorm;

	return true;
}
