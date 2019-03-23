////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.View.Base
// file:      OGLWin_Menu.cpp
//
// summary:   Implements the ogl window menu class
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

#ifdef WIN32
    #include <ShellApi.h>
    #include <crtdbg.h>
    #include <shlobj.h>

    #include <Shlwapi.h>
    #pragma comment( lib, "Shlwapi" )
#endif

void COGLWin::InitMenu()
{
	COGLWinFLTK::InitMenu();

	int iCnt;
	int iSize = int(m_vecMenuItem.size());

	if (m_bShowWindowMenu && !m_bVizOnlyMode)
	{
		iCnt = 15;

		Fl_Menu_Item pMenuItems[] =
		{
			{ "&Code",              0, 0, 0, FL_SUBMENU },
			{ "&Parse",        FL_CTRL + 'p', (Fl_Callback*) CB_Parse },
			{ "Parse (Force Render Late&x)",  FL_CTRL + FL_SHIFT + 'p', (Fl_Callback*) CB_ParseForce },
			{ 0 },

			{ "&Presentation", 0, 0, 0, FL_SUBMENU },
			{ "Ne&xt",        FL_Page_Down, (Fl_Callback*) CB_NextScript },
			{ "Pre&vious",    FL_Page_Up, (Fl_Callback*) CB_PrevScript, 0, FL_MENU_DIVIDER },
			//		{ "&Reload Current",    FL_Enter, (Fl_Callback *) CB_ReloadScript, 0, FL_MENU_DIVIDER },
			{ "&Show List...",    FL_CTRL + FL_Enter, (Fl_Callback*) CB_ShowScriptList },
			{ "&Clear List",    0, (Fl_Callback*) CB_ClearScriptList },
			{ 0 },

			{ "&Window",              0, 0, 0, FL_SUBMENU },
			{ "&Editor", FL_CTRL + FL_SHIFT + 'e', (Fl_Callback*) CB_ShowEditorWin },
			{ "&Output", FL_CTRL + FL_SHIFT + 'o', (Fl_Callback*) CB_ShowOutputWin },
			{ "&Visualization", FL_CTRL + FL_SHIFT + 'v', (Fl_Callback*) CB_ShowVisWin },
			{ 0 },

			{ 0 }
		};

		m_vecMenuItem.resize(iSize + iCnt);

		for (int i = iSize - 1; i < iSize + iCnt; i++)
		{
			m_vecMenuItem[i] = pMenuItems[i - iSize + 1];
		}
	}
	else
	{
		iCnt = 6;

		Fl_Menu_Item pMenuItems[] =
		{
			{ "&Presentation", 0, 0, 0, FL_SUBMENU },
			{ "Ne&xt",        FL_Page_Down, (Fl_Callback*) CB_NextScript },
			{ "Pre&vious",    FL_Page_Up, (Fl_Callback*) CB_PrevScript, 0, FL_MENU_DIVIDER },
			//		{ "&Reload Current",    FL_Enter, (Fl_Callback *) CB_ReloadScript, 0, FL_MENU_DIVIDER },
			{ "&Show List...",    FL_CTRL + FL_Enter, (Fl_Callback*) CB_ShowScriptList },
			{ "&Clear List",    0, (Fl_Callback*) CB_ClearScriptList },
			{ 0 },

			{ 0 }
		};

		m_vecMenuItem.resize(iSize + iCnt);

		for (int i = iSize - 1; i < iSize + iCnt; i++)
		{
			m_vecMenuItem[i] = pMenuItems[i - iSize + 1];
		}
	}
}

bool COGLWin::SetMenu(vector<SMenuItem>& vecMenuItem)
{
	m_bMenuChanged   = true;
	m_vecNewMenuItem = vecMenuItem;

	return true;
}

bool COGLWin::EnableMenu(bool bVal)
{
	m_bEnableMenuChanged = true;
	m_bNewMenuState      = bVal;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Code Menu Callbacks

void COGLWin::MenuParse()
{
	if (m_pEditor)
	{
		m_bParseFromOGLWin = true;
		m_pEditor->Parse(false, false, m_sScriptPath.c_str(), m_sScriptName.c_str());
	}
}

void COGLWin::MenuParseForce()
{
	if (m_pEditor)
	{
		m_bParseFromOGLWin = true;
		m_pEditor->Parse(false, true, m_sScriptPath.c_str(), m_sScriptName.c_str());
	}
}

void COGLWin::MenuQuickSaveImage(Fl_Widget* pWidget)
{
	// For windows use desktop folder
	#ifdef WIN32
		char pcFilename[512];
		CHAR szPath[MAX_PATH];

		if (SUCCEEDED(SHGetFolderPathA(NULL,
						    CSIDL_DESKTOP,
						    NULL,
						    SHGFP_TYPE_CURRENT,
						    szPath)))
		{
			sprintf_s(pcFilename, 512, "%s\\CLUImage_%04d.png", szPath, ++m_iImgID);

			//GLUTDisplay();
			//CCLUDrawBase::SaveScreen2BMP(pcFilename);

			LockVis();
			m_bDoSaveScreen       = true;
			m_sSaveScreenFilename = pcFilename;
			UnlockVis();
			PostRedisplay(true);
		}
	#else
		// for other systems use script folder
		char pcFilename[512];

		sprintf_s(pcFilename, 512, "%s/CLUCalcImg_%04d.jpg", m_sScriptPath.c_str(), ++m_iImgID);

		m_bDoSaveScreen       = true;
		m_sSaveScreenFilename = sFile.c_str();
		PostRedisplay();

		//GLUTDisplay();
		//CCLUDrawBase::SaveScreen2BMP(pcFilename);
	#endif
}

void COGLWin::MenuSaveImage(Fl_Widget* pWidget)
{
	string sFile, sPattern, sTitle, sFilter;

	sTitle   = "Save Screen Image";
	sPattern =
		"PNG Files:*.png;JPEG Files:*.jpg;Bitmap Files:*.bmp;Postscript Files:*.eps;"
		"Save as JPEG + EPS:*.jps;"
		"Save PNG with Bounding Box:*.bpng;";

	if (SaveFileDialog(sTitle, sPattern, sFile, sFilter))
	{
		if (*fl_filename_ext(sFile.c_str()) == 0)
		{
			char* pcExt = (char*) fl_filename_ext(sFilter.c_str());

			if ((pcExt == 0) || (*pcExt == 0))
			{
				//strcat(pcNewFile, ".png");
				sFile += ".png";
			}
			else
			{
				//strcat(pcNewFile, pcExt);
				sFile += pcExt;
			}
		}

		LockVis();
		m_bDoSaveScreen       = true;
		m_sSaveScreenFilename = sFile.c_str();
		UnlockVis();

		PostRedisplay(true);

		//GLUTDisplay();
		//CCLUDrawBase::SaveScreen2BMP(sFile.c_str());
	}
}

//////////////////////////////////////////////////////////////////////
/// Window Menu Callbacks

void COGLWin::MenuShowEditorWin()
{
	if (m_pEditor)
	{
		m_pEditor->show();
	}
}

void COGLWin::MenuShowOutputWin()
{
	if (m_pOutput)
	{
		m_pOutput->show();
	}
}

void COGLWin::MenuShowVisWin()
{
	ShowWindow();
}

void COGLWin::MenuNextScript(Fl_Widget* pWidget)
{
	if (!m_pEditor)
	{
		return;
	}

	m_pEditor->SetCurEditor(m_xParse.GetCodeBase().GetCurScriptFilename().c_str());

	if ((m_xParse.GetCodeBase().NextScriptFile() != -1)
	    && !m_bWaitWithIdleForNextDisplay)
	{
		// In presentations do not reset BGColor
		m_bKeepBGColor        = true;
		m_bPresentationAction = true;

		m_pEditor->LoadFile(m_xParse.GetCodeBase().GetCurScriptFilename().c_str());
		m_pEditor->Parse(true);
		m_pEditor->UpdateFileList();
	}
}

void COGLWin::MenuPrevScript(Fl_Widget* pWidget)
{
	if (!m_pEditor)
	{
		return;
	}

	m_pEditor->SetCurEditor(m_xParse.GetCodeBase().GetCurScriptFilename().c_str());

	if ((m_xParse.GetCodeBase().PrevScriptFile() != -1)
	    && !m_bWaitWithIdleForNextDisplay)
	{
		// In presentations do not reset BGColor
		m_bKeepBGColor        = true;
		m_bPresentationAction = true;
		m_pEditor->LoadFile(m_xParse.GetCodeBase().GetCurScriptFilename().c_str());
		m_pEditor->Parse(true);
		m_pEditor->UpdateFileList();
		return;
	}
}

void COGLWin::MenuReloadScript(Fl_Widget* pWidget)
{
	if (m_pEditor && !m_bWaitWithIdleForNextDisplay)
	{
		string sFilename;
		sFilename = m_xParse.GetCodeBase().GetCurScriptFilename();
		if (sFilename.size() > 0)
		{
			// In presentations do not reset BGColor
			m_bKeepBGColor        = true;
			m_bPresentationAction = true;

			m_pEditor->LoadFile(sFilename.c_str());
			m_pEditor->Parse(true);
			m_pEditor->UpdateFileList();
		}
	}
}

void COGLWin::MenuClearScriptList(Fl_Widget* pWidget)
{
	if (m_pEditor)
	{
		m_xParse.GetCodeBase().ClearScriptList();
		SetTitle();
		return;
	}
}

void COGLWin::MenuShowScriptList(Fl_Widget* pWidget)
{
	if (!m_pBrowserWin || !m_pBrowser)
	{
		fl_alert("Script list windows not available");
		return;
	}

	if (m_pEditor)
	{
		vector<CCLUCodeBase::SScriptFile>& vecScriptList = m_xParse.GetCodeBase().GetScriptList();
		int iCurScript                                   = m_xParse.GetCodeBase().GetCurScriptIdx();
		int iScript, iScriptCount = int(vecScriptList.size());

		if (iScriptCount == 0)
		{
			fl_alert("No scripts in list");
			return;
		}

		m_pBrowser->clear();
		for (iScript = 0; iScript < iScriptCount; iScript++)
		{
			m_pBrowser->add(vecScriptList[iScript].sTitle.c_str(), (void*) iScript);
		}

		m_pBrowser->select(iCurScript + 1);

		// In presentations do not reset BGColor
		m_bKeepBGColor        = true;
		m_bPresentationAction = true;
		m_pBrowserWin->set_modal();
		m_pBrowserWin->show();

		return;
	}
}
