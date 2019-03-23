////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_Blend.cpp
//
// summary:   Implements the function blend class
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
#include "Func_Blend.h"

#include "CluTec.Viz.Draw\OGLBlend.h"
#include "CluTec.Viz.Base\TensorOperators.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool BlendFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	// Get function arguments list
	TVarList& mVars = *rPars.GetVarListPtr();

	// Get number of function arguments passed to the call
	size_t nVarCount = mVars.Count();

	// Pointer to blend element
	COGLBlend* pBlend;

	// Expected 0 or 1 parameters
	if (nVarCount == 0)
	{
		pBlend = new COGLBlend();
		pBlend->SetName("Default");
	}
	else if (nVarCount == 1)
	{
		// First argument must be a string
		if (mVars(0).BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().GeneralError("Expect the blend type as parameter.", iLine, iPos);
			return false;
		}

		// Set the blend type
		pBlend = new COGLBlend();
		pBlend->SetName(mVars(0).GetStringPtr()->Str());

		if (!pBlend->SetBlendType(*mVars(0).GetStringPtr()))
		{
			delete pBlend;

			rCB.GetErrorList().GeneralError("Unknown blend type.", iLine, iPos);
			return false;
		}
	}
	else
	{
		int piPar[] = { 0, 1 };
		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	COGLBEReference SceneRef = rCB.GetOGLDrawBase()->GetSceneRepository()->New(pBlend);
	rVar = SceneRef;

	return true;
}
