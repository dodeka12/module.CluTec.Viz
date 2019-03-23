////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.View.Base
// file:      OGLWin_EmbedWindow.cpp
//
// summary:   Implements the ogl window embed Windows Form
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

#include "CluTec.System\FilePath.h"
#include "CluTec.System\FileInfo.h"

#include <string>
using namespace std;

/////////////////////////////////////////////////////////////////////////////
/// Embed the OpenGL window in an external window

bool COGLWin::EmbedWindow(HWND hWnd, bool bCreateSharedRC, HGLRC* phGlRc)
{
	if (m_bIsEmbedded)
	{
		return false;
	}

	m_iHWin = 1;

	// Enter this pointer into map.
	sm_mapThis[m_iHWin] = this;

	m_hGLWnd      = hWnd;
	m_bIsEmbedded = true;
	m_pWindow     = 0;

	if (!Init(true, bCreateSharedRC, phGlRc))
	{
		m_hGLWnd      = 0;
		m_bIsEmbedded = false;
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/// Release Window if it is embedded

bool COGLWin::ReleaseWindow()
{
	if (!m_bIsEmbedded)
	{
		return false;
	}

	m_hGLWnd      = 0;
	m_bIsEmbedded = false;
	DestroySharedRC();

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sets a mouse event callback function
bool COGLWin::SetMouseEventHandler(Clu::Viz::View::TFuncMouseEventCallback pFunc, void* pvContext)
{
	if (pFunc == nullptr)
	{
		return false;
	}

	m_pMouseEventCallbackFunc    = pFunc;
	m_pMouseEventCallbackContext = pvContext;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Removes the mouse event callback function
bool COGLWin::RemoveMouseEventHandler()
{
	if (m_pMouseEventCallbackFunc == nullptr)
	{
		return true;
	}

	m_pMouseEventCallbackFunc    = nullptr;
	m_pMouseEventCallbackContext = nullptr;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void COGLWin::SetErrorEventHandler(Clu::Viz::View::TFuncErrorEventCallback pFunc, void* pvContext)
{
	if (pFunc == nullptr)
		return;

	m_pErrorEventCallbackFunc    = pFunc;
	m_pErrorEventCallbackContext = pvContext;

	m_bThrowExceptionOnScriptError = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void COGLWin::RemoveErrorEventHandler()
{
	m_pErrorEventCallbackFunc = nullptr;
	m_pErrorEventCallbackContext = nullptr;

	m_bThrowExceptionOnScriptError = false;
}

/////////////////////////////////////////////////////////////////////////
// Make the shared rendering context current

bool COGLWin::EW_MakeCurrent(bool bBind)
{
	if (!m_bIsEmbedded)
	{
		return false;
	}

	BOOL bResult;
	if (bBind)
	{
		if (m_bHasSharedGLWnd)
		{
			bResult = wglMakeCurrent(m_hShareGLDC, m_hShareGLRC);
		}
		else
		{
			bResult = wglMakeCurrent(m_hGLDC, m_hGLRC);
		}
	}
	else
	{
		bResult = wglMakeCurrent(0, 0);
	}

	return (bResult == TRUE ? true : false);
}

/////////////////////////////////////////////////////////////////////////////
/// Embedded Window Display

void COGLWin::EW_Display(bool bWait)
{
	if (!m_bIsEmbedded)
	{
		return;
	}

	PostRedisplay(bWait);
}

/////////////////////////////////////////////////////////////////////////////
/// Embedded Window Display

void COGLWin::EW_Reshape(int iWidth, int iHeight)
{
	if (!m_bIsEmbedded)
	{
		return;
	}

	Reshape(iWidth, iHeight);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void COGLWin::EW_GetShape(int& iWidth, int& iHeight)
{
	if (!m_bIsEmbedded)
	{
		return;
	}

	int iX, iY;
	GetViewport(iX, iY, iWidth, iHeight);
}

/////////////////////////////////////////////////////////////////////////////
/// Embedded Window Mouse Click

void COGLWin::EW_Mouse(int _iButton, int _iState, int iX, int iY)
{
	if (!m_bIsEmbedded)
	{
		return;
	}

	int iButton = -1, iState = -1;

	if (_iButton == 0)
	{
		iButton = CLU_LEFT_BUTTON();
	}
	else if (_iButton == 2)
	{
		iButton = CLU_RIGHT_BUTTON();
	}
	else
	{
		return;
	}

	if (_iState == 0)
	{
		iState = CLU_UP();
	}
	else if (_iState == 1)
	{
		iState = CLU_DOWN();
	}
	else
	{
		return;
	}

	Mouse(iButton, iState, iX, iY);
}

/////////////////////////////////////////////////////////////////////////////
/// Embedded Window Key pressed

void COGLWin::EW_Key(unsigned char cKey, int iX, int iY)
{
	if (!m_bIsEmbedded)
	{
		return;
	}
}

/////////////////////////////////////////////////////////////////////////////
/// Embedded Window Active Mouse Move

void COGLWin::EW_ActiveMouseMove(int iX, int iY)
{
	if (!m_bIsEmbedded)
	{
		return;
	}

	ActiveMouseMove(iX, iY);
}

/////////////////////////////////////////////////////////////////////////////
/// Embedded Window Passive Mouse Move

void COGLWin::EW_PassiveMouseMove(int iX, int iY)
{
	if (!m_bIsEmbedded)
	{
		return;
	}

	PassiveMouseMove(iX, iY);
}

/////////////////////////////////////////////////////////////////////////////
/// Embedded Window Idle Func

void COGLWin::EW_IdleFunc()
{
	if (!m_bIsEmbedded)
	{
		return;
	}

	IdleFunc();
}

/////////////////////////////////////////////////////////////////////////////
/// Embedded Window Set the modifier key states

void COGLWin::EW_SetKeyModifierStates(bool bShift, bool bCtrl, bool bAlt, bool bNumLock, bool bCapsLock, bool bScrollLock)
{
	if (!m_bIsEmbedded)
	{
		return;
	}

	LockVis();
	m_bShiftDown = bShift;
	m_bCtrlDown  = bCtrl;
	m_bAltDown   = bAlt;

	m_bNumLock    = bNumLock;
	m_bCapsLock   = bCapsLock;
	m_bScrollLock = bScrollLock;
	UnlockVis();
}

/////////////////////////////////////////////////////////////////////////////

bool COGLWin::EW_LoadScript(const char* pcFilename, const char* pcPass)
{
	if (!m_bIsEmbedded)
	{
		m_sError = "Window hasn't been embedded";
		return false;
	}

	std::string sText, sFilename, sPath, sName, sExt;
	ifstream zData;
	bool bIsBinary = false;
	char pcLine[4096];

	if (!pcFilename)
	{
		m_sError = "File name is null";
		return false;
	}

	sFilename = pcFilename;
	sFilename = Clu::CFilePath::MakeAbsolute(sFilename);

	Clu::CFileInfo xInfo(sFilename);
	sPath = xInfo.GetDirectory();
	sName = xInfo.GetName();
	sExt = xInfo.GetExt();

	// Clx for PowerPoint rx3d/trix stubs
	if ((sExt == ".clu") || (sExt == ".clx"))
	{
		bIsBinary = false;
	}
	else if (sExt == ".cbn")
	{
		bIsBinary = true;
	}
	else
	{
		m_sError = "Unknown extension '" + sExt + "'";
		return false;
	}

	if (bIsBinary)
	{
		string sError;
		map<string, string> mapMetaData;
		Mem<char> mCode;

		if (!CCLUParse::ReadCLUBinaryFile(sFilename.c_str(), mCode, mapMetaData, sError, pcPass))
		{
			m_sError = "Error reading binary file '" + sFilename + "'";
			return false;
		}

		SetScriptPath(sPath.c_str());
		SetScriptName(sName.c_str());
		return SetScript((const char*) mCode.Data(), true, false, bIsBinary) >= 0;
	}
	else
	{
		zData.open(sFilename.c_str());
		if (!zData.is_open())
		{
			m_sError = "Error opening file stream of file '" + sFilename + "'";
			return false;
		}

		do
		{
			zData.read(pcLine, 4090);
			pcLine[zData.gcount()] = 0;
			sText                 += pcLine;
		}
		while (!zData.eof());

		zData.close();

		SetToolNameVar("None");

		SetScriptPath(sPath.c_str());
		SetScriptName(sName.c_str());
		return SetScript(sText.c_str(), true, false, bIsBinary) >= 0;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool COGLWin::EW_SetScript(const char* pcScript, bool bResetEnv, bool bReparseLatex)
{
	try
	{
		if (!m_bIsEmbedded)
		{
			return false;
		}

		m_bHasError = false;

		LockVis();

		if (bReparseLatex)
		{
			m_xParse.OverrideSetRenderLatex(false);
			m_xParse.SetRenderLatex(true);
			m_xParse.OverrideSetRenderLatex(true);
		}

		if (bResetEnv)
		{
			SetExecModeVar(EXEC_MODE_NEW);
			AddExecModeVar(EXEC_MODE_CHANGE);

			ResetTransformVars();

			if (!m_bKeepBGColor)
			{
				CCodeVar& rVarCol = m_xParse.GetVar("_BGColor");
				if (rVarCol.Type() != PDT_NOTYPE)
				{
					rVarCol = Black;
					SetBGColor(Black);
				}
			}
			else
			{
				m_bKeepBGColor = false;
			}

			// Reset visualization mode to OPNS for all spaces.
			m_xFilter.SetMode(MODE_OPNS);

			// Reset Code Base
			m_xParse.ResetCodeBase();
		}
		else
		{
			SetExecModeVar(EXEC_MODE_CHANGE);
		}

		m_xParse.PostResetResourceHandles();

		SetToolNameVar("None");

		int piMaskLeft[]  = { 1, 0, 0, 1 };
		int piMaskRight[] = { 0, 1, 1, 1 };

		EnableColorStereo(false);
		SetColorStereoSep(0.04f);
		SetColorStereoDegAngle(0.48f);
		SetColorStereoMask(piMaskLeft, piMaskRight);

		EnableTransparency(true);
		EnableLighting(true);

		m_xParse.CollectGarbage();
		m_xParse.Reset();
		m_xParse.GetCodeBase().ResetInfoVars();

		m_bPresentationAction = false;
		m_bParseFromOGLWin    = false;

		if (m_xParse.InsertText(pcScript) >= 0)
		{
			m_bCheckForAnimate    = true;
			m_bCodeChanged        = true;
			m_bCodeChangedForAnim = true;
		}
		else
		{
			ClearError();
			m_sError  = "<b>Parse Errors:</b><p>";
			m_sError += m_xParse.PrintParseErrors();
			m_sError += "</p>";

			m_bHasError = true;

			m_xParse.ClearError();
		}

		UnlockVis();

		// Redisplay and wait until script has been executed at least once
		PostRedisplay(true, true);
		return true;
	}
	catch (Clu::CIException& ex)
	{
		throw CLU_EXCEPTION_NEST(__FUNCTION__, std::move(ex));
	}
}

/////////////////////////////////////////////////////////////////////////////
// Exec User

bool COGLWin::EW_ExecUser(const char* pcToolName)
{
	if (!m_bIsEmbedded)
	{
		return false;
	}

	// We do not allow to execute a script within a mouse event. The flag m_bMouseCallbackActive is set when the mouse callback is called
	if (m_bMouseCallbackActive)
	{
		m_sError = "A user script cannot be executed within a mouse event";
		return false;
	}

	AddExecModeVar(EXEC_MODE_USER);

	if (pcToolName)
	{
		SetToolNameVar(pcToolName);
	}

	// Force execution of script, even if the window is disabled.
	// Also ensure that script is run right away without waiting
	// for next timer loop signal.
	PostRedisplay(true, true, true);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Embedded Window Lock/Unlock Visualization thread and make rendering context current in calling thread.

bool COGLWin::EW_LockVis(bool bLock)
{
	if (!m_bIsEmbedded)
	{
		return false;
	}

	if (bLock)
	{
		// Lock visualization thread
		LockVis(INFINITE);

		// Make rendering context current for calling thread
		//wglMakeCurrent( m_hGLDC, m_hGLRC );
	}
	else
	{
		// Unlock visualization thread
		UnlockVis();

		// rendering context is made current by visualization thread
	}

	return true;
}
