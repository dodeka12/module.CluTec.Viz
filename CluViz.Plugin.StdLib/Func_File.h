////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_File.h
//
// summary:   Declares the function file class
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

bool GetDirListFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

bool ReadDataFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool WriteDataFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

bool ReadMatrixFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

bool WriteVariableFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool ReadVariableFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

bool ReadXMLFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos) throw(CCluException);
bool WriteXMLFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos) throw(CCluException);

bool ReadTextFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool WriteTextFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

bool ShowFileFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool FileChooserFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool SaveScreenFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

void WriteVariable(CCLUCodeBase& rCB, Mem<char>& mData, CCodeVar& rVar, int iPIL) throw(CCluException);
void WriteVariable(CCLUCodeBase& rCB, CXMLTree& rXMLTree, CCodeVar& rVar, int iPos) throw(CCluException);
void ReadVariable(CCLUCodeBase& rCB, CCodeVar& rVar, char** ppcData) throw(CCluException);
void ReadVariable(CCLUCodeBase& rCB, CCodeVar& rVar, CXMLElement& rEl) throw(CCluException);
void ReadXMLTree(CCLUCodeBase& rCB, TVarList& rList, CXMLTree& rTree, int iLine, int iPos) throw(CCluException);
void WriteXMLTree(CCLUCodeBase& rCB, TVarList& rList, CXMLTree& rTree, int iLine, int iPos) throw(CCluException);

//// Open a trix file and return a handle
//bool OpenTrixFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
//
//// Get the amount of available frames in the trix
//bool GetTrixFrameCountFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
//
//// Retrieve a set of images of a given frame
//bool GetTrixFrameFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
//
//// Get the min/max values for depth map and (optional) remapping image
//bool GetTrixFrameDepthRangeFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
