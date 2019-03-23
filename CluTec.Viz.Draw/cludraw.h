////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      cludraw.h
//
// summary:   Declares the cludraw class
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

#define CLUDRAW_API
 
//#ifdef CLUDRAW_EXPORTS
//#define CLUDRAW_API __declspec(dllexport)
//#else
//#define CLUDRAW_API __declspec(dllimport)
//#endif
//
//#pragma warning( disable : 4251 )
//

// Includes all headers for CLUDraw

#ifdef WIN32
#ifdef CLUDRAW_MFC
	#if _MSC_VER > 1000
	#pragma once
	#endif // _MSC_VER > 1000

	#define WIN32_LEAN_AND_MEAN		// Selten benutzte Teile der Windows-Header nicht einbinden

	#ifndef _AFXDLL
		#define _AFXDLL
	#endif
	#include <afxwin.h>
	#include <afxmt.h>
	#include "mmsystem.h"
#else
	#define WIN32_LEAN_AND_MEAN		// Selten benutzte Teile der Windows-Header nicht einbinden
	#include <windows.h>
#endif
#endif

#include "CluTec.Viz.Fltk\fl_gl.h"
#include "GL/glu.h"

// Undefs for gcc since they are also defined in DevIL
#ifdef PACKAGE
#    undef PACKAGE
#endif

#ifdef VERSION
#    undef VERSION
#endif

/*
#ifdef CLUDRAW_MFC
	#include "OGLWinMFCstatic.h"
#endif

#ifdef CLUDRAW_GLUT
	#include "OGLWinGLUT.h"
#endif

#ifdef CLUDRAW_FLTK
	#include "OGLWinFLTK.h"
#endif
*/

#include "CLUDrawBase.h"
#include "OGLMVFilterBase.h"
#include "OGLFilterE3.h"
#include "OGLFilterP3.h"
#include "OGLFilterN3.h"
#include "OGLMVFilter.h"
#include "OGLLatexText.h"
//#include "OGLBitmapText.h"
#include "OGLBitmap.h"
#include "OGLTexture.h"
#include "OGLImage.h"
#include "OGLColor.h"
#include "OGLAnimColor.h"
#include "OGLVertex.h"
#include "OGLLight.h"
#include "OGLLighting.h"
#include "OGLAmbientLight.h"
#include "OGLMaterial.h"
#include "OGLVertexList.h"
#include "OGLVexListE3.h"
#include "OGLVexListP2.h"
#include "OGLVexListP3.h"
#include "OGLVexListN3.h"
#include "OGLObjColorCube.h"
//#include "OGLObjWireSphere.h"
#include "OGLDisplayList.h"

#include "OGLBaseElement.h"
#include "OGLBaseElementList.h"
#include "OGLBaseTransform.h"

#include "OGLBERepository.h"
#include "OGLBEReference.h"

#include "OGLPixelZoom.h"

#include "OGLTranslation.h"
#include "OGLRotation.h" 
#include "OGLFrame.h"

#include "OGLAnimRotation.h"

#include "OGLDrawBase.h"

#include "OGLPeek.h"
#include "OGLFrameStack.h"

#include "OGLScene.h"
#include "OGLTool.h"
