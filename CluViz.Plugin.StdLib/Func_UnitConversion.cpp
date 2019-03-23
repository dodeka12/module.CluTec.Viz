////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_UnitConversion.cpp
//
// summary:   Implements the function unit conversion class
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
#include "Func_UnitConversion.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief
/// 	Converts the given value in mm to an appropriate unit (e.g. cm, µm, m, ...)
///
///		1. Paramter: (double) The mm value to convert
///		2. Paramter: (int) The amount of decimal places
///
/// \return True if it succeeds, false if it fails.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//bool ConvertUnitFromMMFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
//{
//	TVarList& mVars = *rPars.GetVarListPtr();
//
//	int iVarCount = int(mVars.Count());
//
//	// Check parameter count
//	if (iVarCount != 2)
//	{
//		int piPar[] = { 1 };
//
//		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
//		return false;
//	}
//
//	double dValueMM    = 0;
//	int iDecimalPlaces = 0;
//
//	// Cast the first parameter to double
//	if (!mVars(0).CastToScalar(dValueMM))
//	{
//		rCB.GetErrorList().GeneralError("Expect as first parameter a scalar.", iLine, iPos);
//		return false;
//	}
//
//	// Cast the second parameter to an integer value
//	if (!mVars(1).CastToCounter(iDecimalPlaces))
//	{
//		rCB.GetErrorList().GeneralError("Expect as second parameter a counter.", iLine, iPos);
//		return false;
//	}
//
//	// Only positiv values are handled
//	fabs(dValueMM);
//
//	// Call the Unit-Conversion function
//	Clu::CString sValue = Clu::Math::CUnitConversion::ToString(dValueMM, Clu::Math::Millimeter, iDecimalPlaces, false);
//
//	// Create the result string
//	rVar.New(PDT_STRING);
//	TString& rString = *rVar.GetStringPtr();
//
//	// And pass the value
//	rString = sValue.ToCString();
//	return true;
//}
