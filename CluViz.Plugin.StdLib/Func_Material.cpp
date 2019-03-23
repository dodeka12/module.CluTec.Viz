////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_Material.cpp
//
// summary:   Implements the function material class
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
#include "Func_Material.h"

//////////////////////////////////////////////////////////////////////
/// Get Material as base element reference
///
/// Parameters:
///		1. Name
///
///	Returns:
///		Material variable as BaseElement Reference

bool MaterialFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
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
		rCB.GetErrorList().GeneralError("First parameter has to be the name of the material.", iLine, iPos);
		return false;
	}

	COGLMaterial* pMaterial = new COGLMaterial;

	pMaterial->SetName(mVars(0).GetStringPtr()->Str());
	pMaterial->Ambient(1.0f, 1.0f, 1.0f, 1.0f);
	pMaterial->Diffuse(1.0f, 1.0f, 1.0f, 1.0f);
	pMaterial->Emission(0.0f, 0.0f, 0.0f, 1.0f);
	pMaterial->Specular(0.0f, 0.0f, 0.0f, 1.0f);
	pMaterial->Shininess(20.0f);
	pMaterial->Face(GL_FRONT_AND_BACK);

	rVar = rCB.GetOGLDrawBase()->GetSceneRepository()->New(pMaterial);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Material Emission Color Function
///
/// Parameters:
///		1. Color

bool SetMaterialEmissionFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
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
		rCB.GetErrorList().GeneralError("First parameter has to be a material variable.", iLine, iPos);
		return false;
	}

	TScene Ref = *mVars(0).GetScenePtr();
	if (!Ref.IsValid())
	{
		rCB.GetErrorList().GeneralError("Invalid material variable.", iLine, iPos);
		return false;
	}

	COGLMaterial* pMaterial = dynamic_cast<COGLMaterial*>((COGLBaseElement*) Ref);
	if (!pMaterial)
	{
		rCB.GetErrorList().GeneralError("Invalid material variable.", iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_COLOR)
	{
		rCB.GetErrorList().GeneralError("Second parameter has to be a color.", iLine, iPos);
		return false;
	}

	pMaterial->Emission(*mVars(1).GetOGLColorPtr());

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Material Specular Color Function
///
/// Parameters:
///		1. Color

bool SetMaterialSpecularFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
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
		rCB.GetErrorList().GeneralError("First parameter has to be a material variable.", iLine, iPos);
		return false;
	}

	TScene Ref = *mVars(0).GetScenePtr();
	if (!Ref.IsValid())
	{
		rCB.GetErrorList().GeneralError("Invalid material variable.", iLine, iPos);
		return false;
	}

	COGLMaterial* pMaterial = dynamic_cast<COGLMaterial*>((COGLBaseElement*) Ref);
	if (!pMaterial)
	{
		rCB.GetErrorList().GeneralError("Invalid material variable.", iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_COLOR)
	{
		rCB.GetErrorList().GeneralError("Second parameter has to be a color.", iLine, iPos);
		return false;
	}

	pMaterial->Specular(*mVars(1).GetOGLColorPtr());

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Material Shininess Function
///
/// Parameters:
///		1. scalar exponent

bool SetMaterialShininessFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVScalar fVal;

	if (iVarCount != 2)
	{
		int piPar[] = { 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("First parameter has to be a material variable.", iLine, iPos);
		return false;
	}

	TScene Ref = *mVars(0).GetScenePtr();
	if (!Ref.IsValid())
	{
		rCB.GetErrorList().GeneralError("Invalid material variable.", iLine, iPos);
		return false;
	}

	COGLMaterial* pMaterial = dynamic_cast<COGLMaterial*>((COGLBaseElement*) Ref);
	if (!pMaterial)
	{
		rCB.GetErrorList().GeneralError("Invalid material variable.", iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToScalar(fVal))
	{
		rCB.GetErrorList().GeneralError("Second parameter has to be a scalar.", iLine, iPos);
		return false;
	}

	pMaterial->Shininess(float(fVal));

	return true;
}
