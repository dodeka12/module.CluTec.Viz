////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      TempLib.cpp
//
// summary:   Implements the temporary library class
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

// Template Library

#include "StdAfx.h"

#include "CluTec.Viz.Base\mem.cxx"
#include "CluTec.Viz.Base\memobj.cxx"

#include "Stack.cpp"
#include "CluTec.Viz.Base\Array2D.cxx"
#include "CodeVar.h"
#include "CodeElementList.h"
#include "CluTec.Viz.Base\MessageList.h"
#include "ParseBase.h"
#include "OCOpStd.h"

template class CStack<CCodeVar*>;
template class CStack<CCodeVarList*>;

template class CArray2D<int>;

template class Mem<SCodeElementPtr>;
template class MemObj<CCodeVar>;
template class MemObj<CCodeVar*>;
template class Mem<CCodeVar*>;
template class Mem<CCodeVar**>;
template class MemObj<CCodeVarList*>;
template class Mem<CCodeVarList**>;
template class MemObj<SMsg>;
template class Mem<SMsg*>;
template class MemObj<CCodeFunction>;
template class Mem<CCodeFunction*>;
template class MemObj<STextLine>;
template class MemObj<STextLine*>;
template class Mem<STextLine*>;
template class Mem<STextLine**>;
template class MemObj<COCOpStd>;
template class Mem<COCOpStd*>;
template class MemObj<TMatrix>;
template class MemObj<COGLColor>;
template class MemObj<TImage>;
template class MemObj<MemObj<TImage> >;
template class Mem<TVarList*>;

