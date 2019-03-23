////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      stdafx.h
//
// summary:   Declares the stdafx class
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

// StdAfx.h : Include-Datei f�r Standard-System-Include-Dateien,
//  oder projektspezifische Include-Dateien, die h�ufig benutzt, aber
//      in unregelm��igen Abst�nden ge�ndert werden.
//

#define CLUDRAW_API

//#define CLUDRAW_GLUT
//#ifdef CLUDRAW_EXPORTS
//#define CLUDRAW_API __declspec(dllexport)
//#define CLUDRAW_EXT
//#else
//#define CLUDRAW_API __declspec(dllimport)
//#define CLUDRAW_EXT extern
//#endif
//
//#pragma warning( disable : 4251 )
//

#ifdef WIN32
#ifdef CLUDRAW_MFC
	#if _MSC_VER > 1000
	#pragma once
	#endif // _MSC_VER > 1000

	#define WIN32_LEAN_AND_MEAN		// Selten benutzte Teile der Windows-Header nicht einbinden

	#define _AFXDLL
	#include <afxwin.h>
	#include <afxmt.h>
	#include "mmsystem.h"
#else
	#define WIN32_LEAN_AND_MEAN		// Selten benutzte Teile der Windows-Header nicht einbinden

#ifndef NOMINMAX
	#define NOMINMAX
#endif

	#include <windows.h>
	#include <Commdlg.h>
	
#endif
#endif

#include "CluTec.Viz.Fltk\fl_gl.h"
#include "GL/glu.h"
#include "CluTec.Viz.Base\clu.h"
#include "CluTec.Viz.GA\CvCoreGA.h"


// Maximum coordinate values allowed
#define OGL_VEX_COORD_MAX	10000.0


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f�gt zus�tzliche Deklarationen unmittelbar vor der vorherigen Zeile ein.

