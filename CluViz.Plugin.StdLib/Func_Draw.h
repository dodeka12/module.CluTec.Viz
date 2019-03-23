////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_Draw.h
//
// summary:   Declares the function draw class
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

bool PlotFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

bool FlushDrawFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos );

bool DrawFrameFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

bool DrawPlaneFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool DrawPolygonFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool DrawBoxFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool DrawLineFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool DrawLineSurfaceFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool DrawPointFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool DrawPointSurfaceFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool DrawPointGridFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool DrawPointListFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

bool DrawArrowFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool DrawArcFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool DrawDiskFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool DrawEllipseFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool DrawEllipsoidFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool DrawCircleFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool DrawCircleSurfaceFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool DrawEllipseSurfaceFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool DrawTriangleFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool DrawConeFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool DrawSphereFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool DrawCylinderFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool DrawIcosahedronFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

bool DrawQuantumFractalFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
bool DrawQuantumFractalSurfaceFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
