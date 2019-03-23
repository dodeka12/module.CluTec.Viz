////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_Fog.cpp
//
// summary:   Implements the function fog class
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
#include "Func_Fog.h"

//////////////////////////////////////////////////////////////////////
// Enable Fog Function

//bool EnableFogFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
//{
//	TVarList& mVars = *rPars.GetVarListPtr();
//	
//	int iVarCount = mVars.Count();
//	TCVCounter iVal;
//
//	if (iVarCount == 1)
//	{
//		CCodeVar &rVar = mVars(0);
//		if (!rVar.CastToCounter(iVal))
//		{
//			rCB.GetErrorList().InvalidParType(rVar, 1, iLine, iPos);
//			return false;
//		}
//	}
//	else
//	{
//		int piPar[] = {1};
//
//		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
//		return false;
//	}
//
//	if (iVal)
//		glEnable(GL_FOG);
//	else
//		glDisable(GL_FOG);
//
//	return true;
//}
//
//
////////////////////////////////////////////////////////////////////////
///// Set Fog Color
//
//bool SetFogColorFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
//{
//	int iVar;
//	TVarList& mVars = *rPars.GetVarListPtr();
//	
//	int iVarCount = mVars.Count();
//	TCVScalar dVal[3];
//	COGLColor Col;
//	
//	if (iVarCount == 3)
//	{
//		for (iVar = 0; iVar < 3; iVar++)
//		{
//			if (!mVars(iVar).CastToScalar(dVal[iVar], rCB.GetSensitivity()))
//			{
//				rCB.GetErrorList().InvalidParType(mVars(iVar), iVar+1, iLine, iPos);
//				return false;
//			}								
//		}
//
//		Col.Set(float(dVal[0]), float(dVal[1]), float(dVal[2]));
//	} 
//	// If a single multivector is given as position vector, cast it to E3
//	// and use that position.
//	else if (iVarCount == 1)
//	{
//		if (mVars(0).BaseType() == PDT_COLOR)
//		{
//			Col = *mVars(0).GetOGLColorPtr();
//		}
//		else
//		{
//			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
//			return false;
//		}								
//
//	}
//	else
//	{
//		int piPar[] = {1, 3};
//
//		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
//		return false;
//	}
//
//	glFogfv(GL_FOG_COLOR, Col.Data());
//
//	return true;
//}
//
////////////////////////////////////////////////////////////////////////
///// Set Fog Density
//
//bool SetFogDensityFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
//{
//	TVarList& mVars = *rPars.GetVarListPtr();
//	
//	int iVarCount = mVars.Count();
//	TCVScalar dVal;
//	
//	if (iVarCount == 1)
//	{
//		if (!mVars(0).CastToScalar(dVal, rCB.GetSensitivity()))
//		{
//			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
//			return false;
//		}								
//	} 
//	else
//	{
//		int piPar[] = {1};
//
//		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
//		return false;
//	}
//
//	if (dVal < 0)
//	{
//		rCB.GetErrorList().GeneralError("Invalid fog density value. Needs to be >= 0.", iLine, iPos);
//		return false;
//	}
//
//	glFogf(GL_FOG_DENSITY, float(dVal));
//
//	return true;
//}
//

