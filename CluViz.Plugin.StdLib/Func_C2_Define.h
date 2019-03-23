////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_C2_Define.h
//
// summary:   Declares the function c 2 define class
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

bool DefVarsEl2(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool VecEl2Func(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool VecEl2(CCLUCodeBase &rCB, CCodeVar& rLVar, TCVScalar *pfVec, int iLine, int iPos);
bool VecEl2(CCLUCodeBase &rCB, CCodeVar& rLVar, int iVal, int iLine, int iPos);
bool VecEl2(CCLUCodeBase &rCB, CCodeVar& rLVar, TMultiV& vA, int iLine, int iPos);
bool VecEl2(CCLUCodeBase &rCB, CCodeVar& rLVar, TMatrix& xM, int iLine, int iPos);
bool RotorC2Func(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool ReflectorC2Func(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
