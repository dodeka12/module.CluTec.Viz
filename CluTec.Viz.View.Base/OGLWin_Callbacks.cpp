////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.View.Base
// file:      OGLWin_Callbacks.cpp
//
// summary:   Implements the ogl window callbacks class
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

#include "StdAfx.h"
#include "OGLWin.h"
#include <stdlib.h>
#include "CLUOutput.h"
#include "CluTec.Viz.Base\Environment.h"
#include "CluTec.Viz.OpenGL.Extensions\Extensions.h"
#include "CluTec.Viz.OpenGL\Api.h"

#include <string>
using namespace std;

void COGLWin::CB_ShowEditorWin(Fl_Widget* pWidget, void* pvData)
{
	if (pvData)
	{
		((COGLWin*) pvData)->MenuShowEditorWin();
	}
}

void COGLWin::CB_ShowOutputWin(Fl_Widget* pWidget, void* pvData)
{
	if (pvData)
	{
		((COGLWin*) pvData)->MenuShowOutputWin();
	}
}

void COGLWin::CB_ShowVisWin(Fl_Widget* pWidget, void* pvData)
{
	if (pvData)
	{
		((COGLWin*) pvData)->MenuShowVisWin();
	}
}

//////////////////////////////////////////////////////////////////////
/// Script List Menu Functions

void COGLWin::CB_NextScript(Fl_Widget* pWidget, void* pData)
{
	if (pData)
	{
		((COGLWin*) pData)->MenuNextScript(pWidget);
	}
}

void COGLWin::CB_PrevScript(Fl_Widget* pWidget, void* pData)
{
	if (pData)
	{
		((COGLWin*) pData)->MenuPrevScript(pWidget);
	}
}

void COGLWin::CB_ReloadScript(Fl_Widget* pWidget, void* pData)
{
	if (pData)
	{
		((COGLWin*) pData)->MenuReloadScript(pWidget);
	}
}

void COGLWin::CB_ClearScriptList(Fl_Widget* pWidget, void* pData)
{
	if (pData)
	{
		((COGLWin*) pData)->MenuClearScriptList(pWidget);
	}
}

void COGLWin::CB_ShowScriptList(Fl_Widget* pWidget, void* pData)
{
	if (pData)
	{
		((COGLWin*) pData)->MenuShowScriptList(pWidget);
	}
}

void COGLWin::CB_ScriptListWindow(Fl_Widget* pWidget, void* pData)
{
	if (pData)
	{
		((COGLWin*) pData)->ScriptListWindowEvent(pWidget);
	}
}

void COGLWin::CB_Parse(Fl_Widget* pWidget, void* pvData)
{
	if (pvData)
	{
		((COGLWin*) pvData)->MenuParse();
	}
}

void COGLWin::CB_ParseForce(Fl_Widget* pWidget, void* pvData)
{
	if (pvData)
	{
		((COGLWin*) pvData)->MenuParseForce();
	}
}
