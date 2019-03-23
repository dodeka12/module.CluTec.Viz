////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_VisConfig.h
//
// summary:   Declares the function vis configuration class
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

bool EnableMultisampleFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

bool EnableColorStereoFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool SetColorStereoMaskFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool SetColorStereoParsFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

bool SetVisPrecFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

bool EnableTransparencyFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

bool SetRTViewLookAtFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool SetRTViewModeFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool ResetViewFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool SetRTPositionFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool SetRTViewRotationAxisInversionFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
