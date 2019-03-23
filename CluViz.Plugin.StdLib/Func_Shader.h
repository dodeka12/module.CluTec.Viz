////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_Shader.h
//
// summary:   Declares the function shader class
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

bool ShaderFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool ShaderSupportedFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool EnableShaderFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool EnableShaderForPickingFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool ShaderBuildFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool SetShaderUniformVarFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

bool AnimShaderFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

template<class T>
bool GetValueList( vector<T>& vecData, TVarList &rList, unsigned int uOffset ); 

template<class T>
bool GetMVData( vector<T>& vecData, TMultiV& rMV, uint uDataDim, uint uOffset, int iLine, int iPos );

template<class T>
bool GetColorData( vector<T>& vecData, TOGLColor& rColor, uint uDataDim, uint uOffset, int iLine, int iPos );
