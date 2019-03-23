////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_GLTool.cpp
//
// summary:   Implements the function gl tool class
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

//#include "CluTec.Viz.Base\TensorOperators.h"
//#include "CluTec.Viz.Draw\OGLAnimTranslation.h"
//#include "CluTec.Viz.Draw\OGLAnimScale.h"

#include "Func_GLTool.h"
#include "SceneDraw.h"
#include "OGLButton.h"

//////////////////////////////////////////////////////////////////////
// Create a GL Tool scene element
//
// Parameter:
//		1. (string) The tool name
//		2. (string) The tool type name
//		3. (list) The parameter list depending on the tool created
//
// Return:
//		The scene element drawing the tool
//
//	Tool Types:
/*
        "button":
                "size": [width, height] in pixel
                "text": text shown on button

*/

bool ToolFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount != 3)
	{
		int piVal[] = { 3 };
		rCB.GetErrorList().WrongNoOfParams(piVal, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("The first parameter has to be the tool name.", iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("The second parameter has to be the tool type name.", iLine, iPos);
		return false;
	}

	if (mVars(2).BaseType() != PDT_VARLIST)
	{
		rCB.GetErrorList().GeneralError("The third parameter has to be a variable list describing the tool.", iLine, iPos);
		return false;
	}

	COGLBEReference refTool;
	TString sName     = *mVars(0).GetStringPtr();
	TString sType     = *mVars(1).GetStringPtr();
	TVarList& rParams = *mVars(2).GetVarListPtr();

	sType.ToLowerCase();
	if (sType == "button")
	{
		if (!CreateToolButton(refTool, rCB, sName, rParams, iLine, iPos))
		{
			return false;
		}
	}
	else
	{
		rCB.GetErrorList().GeneralError("Unknown tool type.", iLine, iPos);
		return false;
	}

	rVar = refTool;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Create GL Tool BUTTON
/*
        "button":
                "pos": [x,y] position relative or absolute
                "size": [width, height] relative or absolute
                "text": text shown on button
                (optional)
                "color_text": color of text

*/

bool CreateToolButton(COGLBEReference& refTool, CCLUCodeBase& rCB, TString sName, TVarList& rPars, int iLine, int iPos)
{
	CCodeVar varEl;
	TCVScalar dWidth, dHeight, dX, dY;
	TString sText, sToolID;
	TOGLColor colText, colBGAct, colBGStd;
	Mem<TCVScalar> mData;

	//////////////////////////////////////////////////
	/// Create Tool ID
	sToolID = sName;

	//////////////////////////////////////////////////
	/// Initialize optional parameters

	// Default Text color is black
	colText.Set(0.0f, 0.0f, 0.0f, 1.0f);
	colBGAct.Set(0.9f, 0.9f, 0.9f, 1.0f);
	colBGStd.Set(0.6f, 0.6f, 0.6f, 1.0f);
	//////////////////////////////////////////////////

	//////////////////////////////////////////////////
	/// Get required parameter values

	// pos
	if (!rCB.GetVarListElement(varEl, rPars, TString("pos")))
	{
		rCB.GetErrorList().GeneralError("Parameter 'pos' not given in parameter list.", iLine, iPos);
		return false;
	}

	if (!rCB.GetVarData(mData, varEl.DereferenceVarPtr()) || (mData.Count() != 2))
	{
		rCB.GetErrorList().GeneralError("Parameter 'pos' has to be a list giving [x,y] position.", iLine, iPos);
		return false;
	}

	dX = mData[0];
	dY = mData[1];

	// Size
	if (!rCB.GetVarListElement(varEl, rPars, TString("size")))
	{
		rCB.GetErrorList().GeneralError("Parameter 'size' not given in parameter list.", iLine, iPos);
		return false;
	}

	if (!rCB.GetVarData(mData, varEl.DereferenceVarPtr()) || (mData.Count() != 2))
	{
		rCB.GetErrorList().GeneralError("Parameter 'size' has to be a list giving width and height in pixel.", iLine, iPos);
		return false;
	}

	dWidth  = mData[0];
	dHeight = mData[1];

	// Text
	if (!rCB.GetVarListElement(varEl, rPars, TString("text")))
	{
		rCB.GetErrorList().GeneralError("Parameter 'text' not given in parameter list.", iLine, iPos);
		return false;
	}

	varEl = varEl.DereferenceVarPtr();

	if (varEl.BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("Parameter 'text' has to be a string.", iLine, iPos);
		return false;
	}

	sText = *varEl.GetStringPtr();

	//////////////////////////////////////////////////

	//////////////////////////////////////////////////
	/// Get optional parameter values

	// Text Color
	if (rCB.GetVarListElement(varEl, rPars, TString("color_text")))
	{
		if (!rCB.GetVarData(colText, varEl.DereferenceVarPtr()))
		{
			rCB.GetErrorList().GeneralError("Parameter 'color_text' has to be a color.", iLine, iPos);
			return false;
		}
	}

	//////////////////////////////////////////////////

	COGLDrawBase& rDB = *rCB.GetOGLDrawBase();

	COGLButton* pBut = new COGLButton(rCB);
	if (!pBut)
	{
		rCB.GetErrorList().GeneralError("Out of memory while creating button tool.", iLine, iPos);
		return false;
	}

	pBut->SetName(sToolID.Str());
	pBut->SetToolPos(dX, dY);
	pBut->SetToolSize(dWidth, dHeight);
	pBut->SetToolText(sText);
	pBut->SetToolColorText(colText);
	pBut->SetColorBGAct(colBGAct);
	pBut->SetColorBGStd(colBGStd);

	if (!pBut->CreateTool(refTool))
	{
		rCB.GetErrorList().GeneralError(pBut->GetLastError().Str(), iLine, iPos);
		return false;
	}

	rDB.DrawBaseElement(refTool);

	return true;
}
