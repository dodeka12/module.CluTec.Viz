////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_Capture.h
//
// summary:   Declares the function capture class
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

bool CaptureFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool EnableCaptureFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool SetCaptureTargetFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool SetCaptureAreaFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool SetCaptureTargetOriginFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
