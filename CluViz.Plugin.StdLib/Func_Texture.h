////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_Texture.h
//
// summary:   Declares the function texture class
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

bool TextureFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool SetTextureImageFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool SetTexture3dFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool SetTextureWrapTypeFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool SetTextureBlendTypeFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool SetTextureUnitFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool EnableTextureInterpolateFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool EnableTextureForPickingFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool GetTextureTypeFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
