////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_Blend.h
//
// summary:   Declares the function blend class
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

#pragma once

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 	Blend function.
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
bool BlendFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
