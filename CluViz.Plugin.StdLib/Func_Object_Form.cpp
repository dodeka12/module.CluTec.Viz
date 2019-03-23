////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_Object_Form.cpp
//
// summary:   Implements the function object Windows Form
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
#include "Func_Object_Basic.h"


#include "CluTec.Viz.Draw\OGLVertexList.h"
#include "CluTec.Viz.Base\TensorOperators.h"
#include "CluTec.Viz.Draw\OGLDrawBase.h"

//////////////////////////////////////////////////////////////////////
// Set vertex list content
//
// Pars:
// 1. (vexlist) the vertex list
// 2. (string) name of form:
//		- "grid": a regular grid of triangles, which can be used as basis for a surface
//		- "linestrip": a linestrip with equally spaced element lines
// 3. (varlist) parameter list for chosen form
//		- grid:
//			1. x-extent
//			2. y-extent
//			3. number x divisions
//			4. number y divisions
//
//		- linestrip:
//			1. length of line strip along x-axis
//			2. number of divisions along line strip
//
// Return:
//	nothing

bool  SetVexListFormFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
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
		rCB.GetErrorList().GeneralError("First parameter has to be an object.", iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("Second parameter has to be the form name: 'grid'.", iLine, iPos);
		return false;
	}

	if (mVars(2).BaseType() != PDT_VARLIST)
	{
		rCB.GetErrorList().GeneralError("Third parameter has to be a parameter list.", iLine, iPos);
		return false;
	}


	TScene scVL              = *mVars(0).GetScenePtr();
	COGLVertexList* pVexList = dynamic_cast<COGLVertexList*>((COGLBaseElement*) scVL);

	if (!pVexList)
	{
		rCB.GetErrorList().GeneralError("First parameter is not an object.", iLine, iPos);
		return false;
	}

	TString csForm = *mVars(1).GetStringPtr();
	TVarList& rPar = *mVars(2).GetVarListPtr();

	if (csForm == "grid")
	{
		if (!rCB.VexListCreateFormGrid(*pVexList, rPar, iLine, iPos))
		{
			return false;
		}
	}
	else if (csForm == "linestrip")
	{
		if (!rCB.VexListCreateFormLineStrip(*pVexList, rPar, iLine, iPos))
		{
			return false;
		}
	}
	else
	{
		rCB.GetErrorList().GeneralError("Unknown form name.", iLine, iPos);
		return false;
	}

	rVar = scVL;

	return true;
}

//////////////////////////////////////////////////////////////////////
// Adapt vertex list content by image
//
// Pars:
// 1. (vexlist) the vertex list
// 2. (string) adaptation type:
//		- "x": adapt x-coordinate
//		- "y": adapt y-coordinate
//		- "z": adapt z-coordinate
// 3. (any type) the data that is to be used to adapt the form
// 4. Paramters for particular data type
//
// Par. 3 is image:
// 4. (varlist) parameter list for chosen adaptation
//		- "x", "y", "z":
//			1. (scalar) value to which black maps to
//			2. (scalar) value to which white maps to
//			3. (scalar) scale of normal z component
//
// Return:
//	nothing

bool  AdaptVexListFormFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	// We accept 4 or 5 Parameter for this function.
	// The last parameter is a offset which is added to all values
	if ((iVarCount != 4) && (iVarCount != 5))
	{
		int piPar[] = { 4 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("First parameter has to be an object.", iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("Second parameter has to be the form adaptation type: 'x', 'y', 'z'.", iLine, iPos);
		return false;
	}

	TScene scVL              = *mVars(0).GetScenePtr();
	COGLVertexList* pVexList = dynamic_cast<COGLVertexList*>((COGLBaseElement*) scVL);

	if (!pVexList)
	{
		rCB.GetErrorList().GeneralError("First parameter is not an object.", iLine, iPos);
		return false;
	}

	TString csForm = *mVars(1).GetStringPtr();

	if (mVars(2).BaseType() == PDT_IMAGE)
	{
		if (mVars(3).BaseType() != PDT_VARLIST)
		{
			rCB.GetErrorList().GeneralError("Fourth parameter has to be a parameter list for chosen adaptation.", iLine, iPos);
			return false;
		}

		TImage imgA = *mVars(2).GetImagePtr();

		if (!imgA.IsValid())
		{
			rCB.GetErrorList().GeneralError("Given image object is not valid.", iLine, iPos);
			return false;
		}

		TVarList& rPar = *mVars(3).GetVarListPtr();
		int iCoord     = 0;
		if (csForm == "x")
		{
			iCoord = 0;
		}
		else if (csForm == "y")
		{
			iCoord = 1;
		}
		else if (csForm == "z")
		{
			iCoord = 2;
		}
		else
		{
			rCB.GetErrorList().GeneralError("Unknown form adaptation type.", iLine, iPos);
			return false;
		}

		// set the offset to 0
		double dValueOffset = 0;

		// and adapt that offset if there is 5th Parameter
		if (iVarCount == 5)
		{
			mVars(4).CastToScalar(dValueOffset);
		}

		if (pVexList->GetMode() == GL_LINE_STRIP)
		{
			double dValueMin = 0.0, dValueMax = 0.0;

			if (!rCB.VexListAdaptCoordImage1D(*pVexList, imgA, iCoord, rPar, dValueMin, dValueMax, dValueOffset, iLine, iPos))
			{
				return false;
			}

			rVar.New(PDT_VARLIST);
			TVarList& xList = *rVar.GetVarListPtr();
			xList.Add(2);
			xList[0] = dValueMin;
			xList[1] = dValueMax;
		}
		else
		{
			if (!rCB.VexListAdaptCoordImage2D(*pVexList, imgA, iCoord, rPar, iLine, iPos))
			{
				return false;
			}
		}
	}
	else
	{
		rCB.GetErrorList().GeneralError("Invalid variable type for third parameter.", iLine, iPos);
		return false;
	}

	return true;
}
