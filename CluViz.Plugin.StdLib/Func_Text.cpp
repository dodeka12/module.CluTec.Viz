////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_Text.cpp
//
// summary:   Implements the function text class
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

#include "CluTec.Viz.Draw/OGLDirectWrite.h"

#include "Func_Text.h"

//////////////////////////////////////////////////////////////////////
/// Get Text Image Function

bool  GetTextImageFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if ((iVarCount < 1) || (iVarCount > 2))
	{
		int piPar[] = { 1, 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("Expect as first parameter a string.", iLine, iPos);
		return false;
	}

	TString& csText = *mVars(0).GetStringPtr();

	TCVScalar dAlign = 0.0;

	if (iVarCount > 1)
	{
		if (!mVars(1).CastToScalar(dAlign))
		{
			rCB.GetErrorList().GeneralError("Expect as second parameter a scalar giving the text align for multi-line text.", iLine, iPos);
			return false;
		}
	}

	rVar.New(PDT_IMAGE);
	TImage& rImage = *rVar.GetImagePtr();
	if (!rImage.IsValid())
	{
		rCB.GetErrorList().GeneralError("Could not create image.", iLine, iPos);
		return false;
	}

	string sError;
	if (!rCB.GetOGLText().GetTextImage(*((COGLImage*) rImage), sError, csText.Str(), dAlign))
	{
		sError = "Error: " + sError;
		rCB.GetErrorList().GeneralError(sError.c_str(), iLine, iPos);
		return false;
	}

	// Set text to active color
	rImage->FlushRGB(rCB.GetOGLDrawBase()->GetColor());

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Text Font Function

bool  SetTextFontFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
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
		rCB.GetErrorList().GeneralError("Expect as first parameter a string naming the font.", iLine, iPos);
		return false;
	}

	string sFont, sFace;
	int iMagStep;

	if (!rCB.GetOGLText().GetFont(sFont, sFace, iMagStep))
	{
		sFont    = mVars(0).GetStringPtr()->Str();
		sFace    = "rm";
		iMagStep = 15;
	}
	else
	{
		sFont = mVars(0).GetStringPtr()->Str();
	}

	if (!rCB.GetOGLText().SetFont(sFont, sFace, iMagStep))
	{
		rCB.GetErrorList().GeneralError("Could not load given font.", iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Text MagStep Function

bool  SetTextMagStepFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVCounter iVal;

	if (iVarCount != 1)
	{
		int piPar[] = { 1 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToCounter(iVal))
	{
		rCB.GetErrorList().GeneralError("Expect as first parameter a counter giving the font magnification step.", iLine, iPos);
		return false;
	}

	if (iVal < 1)
	{
		rCB.GetErrorList().GeneralError("Font magnification step has to be greater than zero.", iLine, iPos);
		return false;
	}

	string sFont, sFace;
	int iMagStep;

	if (!rCB.GetOGLText().GetFont(sFont, sFace, iMagStep))
	{
		sFont    = "times";
		sFace    = "rm";
		iMagStep = iVal;
	}
	else
	{
		iMagStep = iVal;
	}

	if (!rCB.GetOGLText().SetFont(sFont, sFace, iMagStep))
	{
		rCB.GetErrorList().GeneralError("Could not load given font size.", iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Text Border Function

bool  SetTextBorderFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVCounter piVal[4];
	int i;

	if (iVarCount != 4)
	{
		int piPar[] = { 4 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	for (i = 0; i < 4; ++i)
	{
		if (!mVars(i).CastToCounter(piVal[i]))
		{
			rCB.GetErrorList().GeneralError("Border values have to be counters.", iLine, iPos);
			return false;
		}

		if (piVal[i] < 0)
		{
			rCB.GetErrorList().GeneralError("Border values have to be greater or equal to zero.", iLine, iPos);
			return false;
		}
	}

	rCB.GetOGLText().SetBorder(piVal[0], piVal[1], piVal[2], piVal[3]);

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Render the given string to an image. We expect one String parameter - the text to be rendered
bool GetRenderTextImageFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	// Do Some basic error checks
	if (iVarCount != 1)
	{
		int piPar[] = { 1 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("Expect as first parameter a string.", iLine, iPos);
		return false;
	}

	TString& csText = *mVars(0).GetStringPtr();

	// Create the result image
	rVar.New(PDT_IMAGE);
	TImage& rImage = *rVar.GetImagePtr();
	if (!rImage.IsValid())
	{
		rCB.GetErrorList().GeneralError("Could not create image.", iLine, iPos);
		return false;
	}

	// Grab the currently used draw color and apply the color to the font parameter
	float* fColData           = rCB.GetOGLDrawBase()->GetColor().Data();
	SFontParameter xFontParam = rCB.GetDirectWrite().GetFontParameter();
	xFontParam.ucFontColor[0] = unsigned char(fColData[0] * 255.0f);
	xFontParam.ucFontColor[1] = unsigned char(fColData[1] * 255.0f);
	xFontParam.ucFontColor[2] = unsigned char(fColData[2] * 255.0f);
	rCB.GetDirectWrite().SetFontParameter(xFontParam);

	// Render the text
	return rCB.GetDirectWrite().RenderTextImage(rImage, csText);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Set the font as String. All Windows fonts can be used
bool SetRenderTextFontFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	// Do Some basic error checks
	if (iVarCount != 1)
	{
		int piPar[] = { 1 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("Expect string first parameter.", iLine, iPos);
		return false;
	}

	TString& sVal = *(mVars(0).GetStringPtr());

	SFontParameter xFontParam = rCB.GetDirectWrite().GetFontParameter();
	xFontParam.sFontName = sVal;

	rCB.GetDirectWrite().SetFontParameter(xFontParam);

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Set the font size. A double parameter is expected
bool SetRenderTextSizeFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVScalar fVal;

	// Do Some basic error checks
	if (iVarCount != 1)
	{
		int piPar[] = { 1 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToScalar(fVal))
	{
		rCB.GetErrorList().GeneralError("FontSize value have to be scalar.", iLine, iPos);
		return false;
	}

	SFontParameter xFontParam = rCB.GetDirectWrite().GetFontParameter();
	xFontParam.fFontSize = (float) fVal;

	rCB.GetDirectWrite().SetFontParameter(xFontParam);

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Set the text style. Valid parameter values are "normal" and "italic"
bool SetRenderTextStyleFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	// Do Some basic error checks
	if (iVarCount != 1)
	{
		int piPar[] = { 1 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("Expect string first parameter.", iLine, iPos);
		return false;
	}

	TString& sVal = *mVars(0).GetStringPtr();

	SFontParameter xFontParam = rCB.GetDirectWrite().GetFontParameter();

	if (sVal == "normal")
	{
		xFontParam.xFontStyle = DWRITE_FONT_STYLE_NORMAL;
	}
	else if (sVal == "italic")
	{
		xFontParam.xFontStyle = DWRITE_FONT_STYLE_ITALIC;
	}
	else
	{
		rCB.GetErrorList().GeneralError("Style parameter has to be 'normal' or 'italic'", iLine, iPos);
		return false;
	}

	rCB.GetDirectWrite().SetFontParameter(xFontParam);

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Set the text weight. Valid values are "normal", "bold" and "thin"
bool SetRenderTextWeightFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	// Do Some basic error checks
	if (iVarCount != 1)
	{
		int piPar[] = { 1 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("Expect string first parameter.", iLine, iPos);
		return false;
	}

	TString& sVal = *mVars(0).GetStringPtr();

	SFontParameter xFontParam = rCB.GetDirectWrite().GetFontParameter();

	if (sVal == "normal")
	{
		xFontParam.xFontWeight = DWRITE_FONT_WEIGHT_NORMAL;
	}
	else if (sVal == "bold")
	{
		xFontParam.xFontWeight = DWRITE_FONT_WEIGHT_BOLD;
	}
	else if (sVal == "thin")
	{
		xFontParam.xFontWeight = DWRITE_FONT_WEIGHT_THIN;
	}
	else
	{
		rCB.GetErrorList().GeneralError("Weight parameter has to be 'normal', 'bold' or 'thin'", iLine, iPos);
		return false;
	}

	rCB.GetDirectWrite().SetFontParameter(xFontParam);

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Set the text streching. Valid Values are "normal", "expanded" and "condensed"
bool SetRenderTextStretchFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	// Do Some basic error checks
	if (iVarCount != 1)
	{
		int piPar[] = { 1 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("Expect string first parameter.", iLine, iPos);
		return false;
	}

	TString& sVal = *mVars(0).GetStringPtr();

	SFontParameter xFontParam = rCB.GetDirectWrite().GetFontParameter();

	if (sVal == "normal")
	{
		xFontParam.xFontStretch = DWRITE_FONT_STRETCH_NORMAL;
	}
	else if (sVal == "expanded")
	{
		xFontParam.xFontStretch = DWRITE_FONT_STRETCH_EXPANDED;
	}
	else if (sVal == "condensed")
	{
		xFontParam.xFontStretch = DWRITE_FONT_STRETCH_CONDENSED;
	}
	else
	{
		rCB.GetErrorList().GeneralError("Style parameter has to be 'normal', 'expanded' or 'condensed'", iLine, iPos);
		return false;
	}

	rCB.GetDirectWrite().SetFontParameter(xFontParam);

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Set the text alignment. Valid values are "left", "right" and "center"
bool SetRenderTextAlignmentFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	// Do Some basic error checks
	if (iVarCount != 1)
	{
		int piPar[] = { 1 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("Expect string first parameter.", iLine, iPos);
		return false;
	}

	TString& sVal = *mVars(0).GetStringPtr();

	SFontParameter xFontParam = rCB.GetDirectWrite().GetFontParameter();

	if (sVal == "left")
	{
		xFontParam.xFontAlignment = DWRITE_TEXT_ALIGNMENT_LEADING;
	}
	else if (sVal == "right")
	{
		xFontParam.xFontAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING;
	}
	else if (sVal == "center")
	{
		xFontParam.xFontAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
	}
	else
	{
		rCB.GetErrorList().GeneralError("Style parameter has to be 'left', 'right' or 'center'", iLine, iPos);
		return false;
	}

	rCB.GetDirectWrite().SetFontParameter(xFontParam);

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Set the font size. A double parameter is expected
bool SetRenderTextWordWrapWidthFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVCounter iVal;

	// Do Some basic error checks
	if (iVarCount != 1)
	{
		int piPar[] = { 1 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToCounter(iVal))
	{
		rCB.GetErrorList().GeneralError("WordWrap value have to be Counter.", iLine, iPos);
		return false;
	}

	SFontParameter xFontParam = rCB.GetDirectWrite().GetFontParameter();
	xFontParam.iWordWrapBoxWidthPx = iVal;

	rCB.GetDirectWrite().SetFontParameter(xFontParam);

	return true;
}

bool SetRenderTextAlphaFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVScalar fVal;

	// Do Some basic error checks
	if (iVarCount != 1)
	{
		int piPar[] = { 1 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToScalar(fVal))
	{
		rCB.GetErrorList().GeneralError("FontSize value have to be scalar.", iLine, iPos);
		return false;
	}

	SFontParameter xFontParam = rCB.GetDirectWrite().GetFontParameter();
	xFontParam.fBaseAlpha = (float) fVal;

	rCB.GetDirectWrite().SetFontParameter(xFontParam);

	return true;
}
