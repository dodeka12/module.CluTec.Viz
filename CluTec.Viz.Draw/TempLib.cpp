////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
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

// Instantiation of templates


#include "StdAfx.h"
#include "CLUDrawBase.h"
#include "CluTec.Viz.Base\mem.cxx"
#include "CluTec.Viz.Base\memobj.cxx"

#include "OGLMaterial.h"
#include "OGLVertexList.h"
//#include "OGLBitmapText.h"
#include "OGLBitmap.h"
#include "OGLMVFilterBase.h"
#include "OGLScene.h"
#include "OGLBEReference.h"

template class Mem<void*>;

template class Mem<bool>;
template class Mem< COGLScene::SDragData >;
template class Mem< COGLScene::SDragBasis >;
template class Mem< COGLScene::SDragRange >;

template class Mem<GLboolean>;
template class Mem<COGLDrawBase::STriplet>;
template class Mem<COGLVertex>;
template class Mem<COGLVertex *>;
template class Mem<TColor>;
template class Mem<STransform>;
template class Mem<COGLMaterial *>;
template class Mem<COGLMaterial **>;
template class Mem<CPointND<float, 4>*>;
template class Mem<Mem<int> *>;
template class Mem<COGLImage::TBGRAPixel>;
template class Mem<COGLImage::TRGBAPixel>;
template class Mem<COGLVertexList::SData>;

template class MemObj<COGLVertex>;
template class MemObj<TColor>;
template class MemObj<COGLMaterial>;
template class MemObj<COGLMaterial *>;
template class MemObj<Mem<int> >;
//template class MemObj<COGLBitmapText::SFontInfo>;
template class MemObj<COGLVertexList*>;
template class MemObj<COGLVertexList>;
template class MemObj<COGLBEReference>;
