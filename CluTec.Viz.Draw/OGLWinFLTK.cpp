////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLWinFLTK.cpp
//
// summary:   Implements the ogl window fltk class
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

// OGLWinFLTK.cpp: Implementierung der Klasse COGLWinFLTK.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#define CLUDRAW_FLTK
#ifdef CLUDRAW_FLTK

	#include "OGLWinFLTK.h"

//#include "Device_3DX.cpp"

//#ifdef WIN32
//#	define _WIN32_DCOM
//#	define _ATL_APARTMENT_THREADED
//#	define _ATL_NO_AUTOMATIC_NAMESPACE
//
//#	define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit
//#	define _ATL_ATTRIBUTES 1
//
//#	include <atlbase.h>
//#	include <atlcom.h>
//#	include <atlwin.h>
//#	include <atltypes.h>
//#	include <atlctl.h>
//#	include <atlhost.h>
//
//	using namespace ATL;

//#	import "progid:TDxInput.Device" embedded_idl no_namespace
//#endif

// Explicit instantiation of map.
//template class map<int, COGLWinFLTK*>;

	map<int, COGLWinFLTK*> COGLWinFLTK::sm_mapThis;
	int COGLWinFLTK::sm_iFocusWin = 0;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

	COGLWinFLTK::COGLWinFLTK()
	{
		//m_pThis = this;
		m_iHWin = 0;

		m_iMinToolHeight = 1;
		m_iToolHeight    = 44;
		m_iMenuHeight    = 46;
		m_iMenuBarHeight = 20;
		m_iMenuBarSep    = 4;
		m_iMMChoiceWidth = 150;	// Mouse Mode Choice Width

		m_iMinToolBarHeight  = 1;
		m_iSmallToolHeight   = 24;
		m_iSmallToolBorderH  = 3;
		m_iSmallToolBorderW  = 2;
		m_iToolBarInitHeight = 1;	//m_iSmallToolHeight + 2;

		m_iToolBoxInitX   = m_iToolBoxInitY = 0;
		m_iInfoWinInitX   = m_iInfoWinInitY = 0;
		m_iOGLWinInitX    = m_iOGLWinInitY = 0;
		m_iMinInfoWidth   = 1;
		m_iInfoLeftMargin = 5;

		m_bDoUpdateTools = false;
		m_bDoRedrawTools = false;

		m_bMenuShown        = true;
		m_bStdCtrlShown     = true;
		m_bUserMenuActive   = false;
		m_bNotifyDestroy    = false;
		m_bInFullScreenMode = false;
		// Allow user to toggle FullScreen mode via menu and ESC-key
		m_bUserToggleFullScreen = true;

		m_iMultisampleTarget = 2;
		m_iImgID             = 0;

		m_pMouseModeChoice    = 0;
		m_pFullScreenMenuItem = 0;

		m_pToolBox     = 0;
		m_pToolBar     = 0;
		m_pWindow      = 0;
		m_pTile        = 0;
		m_pTileToolBar = 0;
		m_pDummyBox    = 0;

		m_pGLUTWindow      = 0;
		m_pMenuBar         = 0;
		m_pMouseModeChoice = 0;
		m_pMenuBarSep      = 0;

		m_pInfoBox = 0;
		m_pInfoWin = 0;

		m_pFullScreenMenuItem = 0;
	}

	COGLWinFLTK::~COGLWinFLTK()
	{
		if (m_iHWin == sm_iFocusWin)
		{
			sm_iFocusWin = 0;
		}

		sm_mapThis.erase(m_iHWin);

		if (m_pToolBar)
		{
			m_pToolBar->ResetToolInUseFlags();
			m_pToolBar->PruneToolMap(false);
		}

		if (m_pToolBox)
		{
			m_pToolBox->ResetToolInUseFlags();
			m_pToolBox->PruneToolMap(false);
		}

		if (m_pToolBar)
		{
			delete m_pToolBar;
		}

		if (m_pToolBox)
		{
			delete m_pToolBox;
		}

		if (m_pWindow)
		{
			delete m_pWindow;
		}
	}

	bool COGLWinFLTK::SaveFileDialog(const string& sTitle, const string& sPattern, string& sFile, string& sCustomFilter)
	{
		#ifdef WIN32
			// Parse Pattern list for native Win32 dialog box
			int iIdx, iCnt = int(sPattern.length()) + 2;
			char* pcPat = 0, * pcChar = 0;
			char pcFile[1024];
			char pcCustFilter[256];

			pcCustFilter[0] = 0;
			pcCustFilter[1] = 0;
			//strcpy( pcCustFilter, "\0PNG Files\0*.png\0\0" );

			strncpy_s(pcFile, 1024, sFile.c_str(), 1023);

			pcPat = new char[iCnt];
			if (!pcPat)
			{
				return false;
			}

			strncpy_s(pcPat, iCnt, sPattern.c_str(), iCnt - 2);

			for (iIdx = 0, pcChar = pcPat; iIdx < iCnt - 2; ++iIdx, ++pcChar)
			{
				if ((*pcChar == ':') || (*pcChar == ';'))
				{
					*pcChar = 0;
				}
				else if (*pcChar == ',')
				{
					*pcChar = ';';
				}
			}

			pcChar[0] = 0;
			pcChar[1] = 0;

			// Use nativ Windows dialog box
			OPENFILENAMEA dlgData;

			dlgData.lStructSize       = sizeof(OPENFILENAMEA);
			dlgData.hwndOwner         = NULL;
			dlgData.hInstance         = NULL;
			dlgData.lpstrFilter       = pcPat;
			dlgData.lpstrCustomFilter = NULL;
			dlgData.nMaxCustFilter    = 0;
			dlgData.nFilterIndex      = 1;
			dlgData.lpstrFile         = pcFile;
			dlgData.nMaxFile          = 1020;
			dlgData.lpstrFileTitle    = NULL;
			dlgData.nMaxFileTitle     = 0;
			dlgData.lpstrInitialDir   = NULL;
			dlgData.lpstrTitle        = (LPCSTR) sTitle.c_str();
			dlgData.Flags             = OFN_OVERWRITEPROMPT;
			dlgData.nFileOffset       = 0;
			dlgData.nFileExtension    = 0;
			dlgData.lpstrDefExt       = NULL;
			dlgData.lCustData         = 0;
			dlgData.lpfnHook          = NULL;
			dlgData.lpfnHook          = NULL;
			dlgData.lpTemplateName    = NULL;
			dlgData.pvReserved        = 0;
			dlgData.dwReserved        = 0;
			dlgData.FlagsEx           = 0;

			BOOL bResult = GetSaveFileNameA(&dlgData);

			if (bResult)
			{
				sFile = dlgData.lpstrFile;

				int iPart = 1;
				for (iIdx = 0, pcChar = pcPat; iIdx < iCnt; ++iIdx, ++pcChar)
				{
					if (*pcChar == 0)
					{
						++iPart;
					}

					if (iPart == 2 * dlgData.nFilterIndex)
					{
						break;
					}
				}

				if (iIdx < iCnt)
				{
					sCustomFilter = &pcChar[1];
				}
				else
				{
					sCustomFilter = "";
				}
			}

			delete pcPat;

			return bResult ? true : false;
		#else

		#error "OpenFileDialog not implemented for other systems but Windows"

			char pcFilename[512];

			pcFilename[0]         = 0;
			Fl_File_Chooser::sort = fl_casenumericsort;
			char* pcNewFile = fl_file_chooser(m_sFileChooserLoadTitle.c_str(),
				m_sFileChooserPattern.c_str(), pcFilename);
		#endif
	}

//COGLWinFLTK* COGLWinFLTK::m_pThis = 0;

//////////////////////////////////////////////////////////////////////
/// Create OpenGL Window with GLUT
	bool COGLWinFLTK::Create(int* piArg, char** pcArg, char* pcName, int iPosX, int iPosY, int iSizeX, int iSizeY, unsigned int uMode)
	{
/*
		m_iSizeX = iSizeX;
		m_iSizeY = iSizeY;

		m_pWindow = new CKeyWindow(this, iPosX, iPosY, iSizeX, iSizeY, pcName);
		m_pWindow->begin();

		InitMenu();
		m_pMenuBar = new Fl_Menu_Bar(0, 0, iSizeX, m_iMenuBarHeight);
		m_pMenuBar->copy(&m_vecMenuItem[0], this);

		m_pMouseModeChoice = new Fl_Choice(6, m_iMenuBarHeight+m_iMenuBarSep, m_iMMChoiceWidth, m_iMenuBarHeight);
		m_pMouseModeChoice->box(FL_FLAT_BOX);
		m_pMouseModeChoice->textsize(11);
		m_pMouseModeChoice->callback((Fl_Callback *) CB_MouseMode, this);


		m_pGLUTWindow = new Fl_Glut_Window(0, m_iMenuHeight, iSizeX, iSizeY - m_iMenuHeight, "");

		m_pGLUTWindow->mode(uMode);
		m_pWindow->size_range(100, 100);
		m_pWindow->resizable(*m_pGLUTWindow);
		m_pWindow->show(*piArg, pcArg);


		//glutInit(piArg, pcArg);
		//glutInitDisplayMode(uMode);
		//glutInitWindowSize(iSizeX, iSizeY);
		//glutInitWindowPosition(0, 0);
		m_iHMainWin = m_iHWin = m_pGLUTWindow->number; //glutCreateWindow(pcName);

		//printf("Main Window: %d\n", m_iHMainWin);

		// Enter this pointer into map.
		sm_mapThis[m_iHWin] = this;

		glutDisplayFunc(GLUTDisplay);
		glutReshapeFunc(GLUTReshape);
		glutMouseFunc(GLUTMouse);
		//glutKeyboardFunc(GLUTKey);
		glutMotionFunc(GLUTActiveMouseMove);
		glutPassiveMotionFunc(GLUTPassiveMouseMove);
		m_pWindow->end();

		//m_iMainMenu = glutCreateMenu(GLUTMenu);
		//glutAttachMenu(GLUT_MIDDLE_BUTTON);

		if (!Init()) return false;
		return true;
*/
		return false;
	}

//////////////////////////////////////////////////////////////////////
/// Create OpenGL Window with GLUT
	bool COGLWinFLTK::CreateWindow(const char* pcName,
		int iPosX,
		int iPosY,
		int iSizeX,
		int iSizeY,
		void* pvIcon,
		unsigned int uMode,
		void* pvParentHandle,
		unsigned int uWindowStyle)
	{
		// Only Create this window once.
		if (m_iHWin != 0)
		{
			return false;
		}

		m_iPosX  = iPosX;
		m_iPosY  = iPosY;
		m_iSizeX = iSizeX;
		m_iSizeY = iSizeY;

//	m_pWindow = new Fl_Window(iPosX, iPosY, iSizeX, iSizeY, pcName);
		m_pWindow = new CKeyWindow(this, iPosX, iPosY, iSizeX, iSizeY, pcName);
		m_pWindow->callback((Fl_Callback*) COGLWinFLTK::CB_Quit, this);
		m_pWindow->color(FL_LIGHT2);
		m_pWindow->icon(pvIcon);
		m_pWindow->SetParentHandle(pvParentHandle);
		//m_pWindow->border(0);
		m_pWindow->begin();

		InitMenu();
		m_pMenuBar = new Fl_Menu_Bar(0, 0, iSizeX, m_iMenuBarHeight);
		m_pMenuBar->copy(&m_vecMenuItem[0], this);
		m_pMenuBar->textsize(11);
		m_pMenuBar->box(FL_FLAT_BOX);
		m_pMenuBar->down_box(FL_FLAT_BOX);
		m_pMenuBar->color(FL_LIGHT2);

		m_pMenuBarSep = new Fl_Box(FL_THIN_DOWN_BOX, 0, m_iMenuBarHeight + 1, iSizeX, 2, (char*) 0);
		m_pMenuBarSep->color(FL_LIGHT2);

		m_iMMChoiceWidth = (iSizeX / 3 < m_iMMChoiceWidth ? m_iMMChoiceWidth : iSizeX / 3);

		m_pMouseModeChoice = new Fl_Choice(6, m_iMenuBarHeight + m_iMenuBarSep, m_iMMChoiceWidth, m_iMenuBarHeight);
		m_pMouseModeChoice->box(FL_FLAT_BOX);
		m_pMouseModeChoice->textsize(11);
		m_pMouseModeChoice->callback((Fl_Callback*) CB_MouseMode, this);

		m_pMouseModeChoice->add("Standard Mouse Mode", FL_CTRL + '0', 0);
		m_pMouseModeChoice->add("Mouse Mode 1", FL_CTRL + '1', 0);
		m_pMouseModeChoice->add("Mouse Mode 2", FL_CTRL + '2', 0);
		m_pMouseModeChoice->add("Mouse Mode 3", FL_CTRL + '3', 0);
		m_pMouseModeChoice->add("Mouse Mode 4", FL_CTRL + '4', 0);
		m_pMouseModeChoice->add("Mouse Mode 5", FL_CTRL + '5', 0);
		m_pMouseModeChoice->add("Mouse Mode 6", FL_CTRL + '6', 0);
		m_pMouseModeChoice->add("Mouse Mode 7", FL_CTRL + '7', 0);
		m_pMouseModeChoice->add("Mouse Mode 8", FL_CTRL + '8', 0);
		m_pMouseModeChoice->add("Mouse Mode 9", FL_CTRL + '9', 0);

		m_pMouseModeChoice->value(0);

		// Tile Window for toolbar
		//m_pTileToolBar = new Fl_Tile(0, m_iMenuHeight, iSizeX, iSizeY - m_iMenuHeight);
		//m_pTileToolBar->begin();

		// Tile Window for tools
//	m_pTile = new Fl_Tile(0, m_iToolBarInitY + m_iToolBarInitHeight,
//							iSizeX, iSizeY - m_iToolBarInitY - m_iToolBarInitHeight);
		m_pTile = new Fl_Tile(0, m_iMenuHeight,
				iSizeX, iSizeY - m_iMenuHeight);
		m_pTile->begin();

		// Create Toolbar
		m_pToolBar          = new CFLTKToolBox;
		m_iToolBarInitWidth = iSizeX - m_iMinInfoWidth;
		m_iToolBarInitX     = m_iMinInfoWidth;
		m_iToolBarInitY     = m_iMenuHeight;
		m_pToolBar->Create(m_iToolBarInitX, m_iToolBarInitY, m_iToolBarInitWidth, m_iToolBarInitHeight,
				(Fl_Callback*) CB_ToolChanged, (void*) this,
				FL_FLAT_BOX, false,
				m_iSmallToolHeight, m_iSmallToolBorderW, m_iSmallToolBorderH);

		// OpenGL part
		// VERY IMPORTANT:
		// If this window extends over the size of the actual window, a WM_PAINT
		// message can lock the rest of the program, since it is constantly regenerated.
		// This is probably because the OGL-Window and the window frame drawn by Windows
		// overlap and thus try to update the same region.
		m_iOGLWinInitX      = m_iMinInfoWidth;
		m_iOGLWinInitY      = m_iMenuHeight + m_iToolBarInitHeight;
		m_iOGLWinInitWidth  = iSizeX - m_iMinInfoWidth;
		m_iOGLWinInitHeight = iSizeY - m_iMenuHeight - m_iMinToolHeight - m_iToolBarInitHeight;
		m_pGLUTWindow       = new Fl_Glut_Window(m_iOGLWinInitX, m_iOGLWinInitY,
				m_iOGLWinInitWidth, m_iOGLWinInitHeight, "");

		m_pGLUTWindow->mode(uMode);
		m_pGLUTWindow->align(FL_ALIGN_CLIP);
		m_pGLUTWindow->box(FL_DOWN_BOX);

		// Create Toolbox
		m_pToolBox      = new CFLTKToolBox;
		m_iToolBoxInitX = m_iMinInfoWidth;
		m_iToolBoxInitY = iSizeY - m_iMinToolHeight;
		m_pToolBox->Create(m_iToolBoxInitX, m_iToolBoxInitY, iSizeX, m_iMinToolHeight,
				(Fl_Callback*) CB_ToolChanged, (void*) this,
				FL_DOWN_BOX, true, m_iToolHeight - 4);

		// Create Info Window
		m_iInfoWinInitX = m_iMinInfoWidth;
		m_iInfoWinInitY = m_iMenuHeight;
		m_pInfoWin      = new Fl_Help_View(0, m_iMenuHeight,
				m_iMinInfoWidth,
				iSizeY - m_iMenuHeight, "");
		m_pInfoWin->selection_color(FL_BLUE);
		m_pInfoWin->link((Fl_Help_Func*) CB_InfoLink, (void*) this);
		m_pInfoWin->value(" ");
		m_pInfoWin->box(FL_EMBOSSED_BOX);
/*
				"Use <code>SetInfoText([string])</code> to put some text here. "
				"The text may include some 'simple' HTML tags");
*/
		m_pDummyBox = new Fl_Box(m_iMinInfoWidth, m_iMinToolBarHeight + m_iToolHeight,
				iSizeX - 50,
				iSizeY - m_iMinToolBarHeight - m_iToolHeight - m_iMinToolHeight);
		//Fl_Box DummyBox(0, 0, iSizeX, iSizeY);
		m_pDummyBox->box(FL_NO_BOX);
		// This is very important since otherwise FL_MOVE events are
		// not sent to the actual widgets in the tile window.
		m_pDummyBox->hide();

		m_pTile->resizable(*m_pDummyBox);
//	m_pTile->resizable(*m_pGLUTWindow);
		m_pTile->end();
		// End tile window

		//m_pTileToolBar->end();

		m_pWindow->size_range(100, 100);
		m_pWindow->resizable(*m_pGLUTWindow);
		m_pWindow->show();
		//m_pWindow->hide();

		m_iSaveX        = m_pWindow->x();
		m_iSaveY        = m_pWindow->y();
		m_iSaveW        = m_pWindow->w();
		m_iSaveH        = m_pWindow->h();
		m_iSaveMMCWidth = m_pMouseModeChoice->w();

		#ifdef WIN32
			m_lSaveStyleWnd   = (long) GetWindowLongPtr((HWND) m_pWindow->GetWindowHandle(), GWL_STYLE);
			m_lSaveExStyleWnd = (long) GetWindowLongPtr((HWND) m_pWindow->GetWindowHandle(), GWL_EXSTYLE);

			if (!(uWindowStyle & CLU_WNDSTYLE_SIZEBORDER))
			{
				m_lSaveStyleWnd &= ~WS_SIZEBOX;
			}

			if (!(uWindowStyle & CLU_WNDSTYLE_CAPTION))
			{
				m_lSaveStyleWnd &= ~WS_CAPTION;
			}

			if (!(uWindowStyle & CLU_WNDSTYLE_SYSMENU))
			{
				m_lSaveStyleWnd &= ~WS_SYSMENU;
			}

			if (!(uWindowStyle & CLU_WNDSTYLE_TOOL))
			{
				m_lSaveExStyleWnd &= ~WS_EX_TOOLWINDOW;
				//m_lSaveStyleWnd &= ~WS_POPUP;
			}
			else
			{
				m_lSaveExStyleWnd |= WS_EX_TOOLWINDOW;
				//m_lSaveStyleWnd |= WS_POPUP;
			}

			if ((uWindowStyle & CLU_WNDSTYLE_TOPMOST))
			{
				m_lSaveExStyleWnd |= WS_EX_TOPMOST;
				SetWindowPos((HWND) m_pWindow->GetWindowHandle(), HWND_TOPMOST, 0, 0, 0, 0,
						SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
			}

			SetWindowLongPtrA((HWND) m_pWindow->GetWindowHandle(), GWL_STYLE, (LONG) m_lSaveStyleWnd);
			SetWindowLongPtrA((HWND) m_pWindow->GetWindowHandle(), GWL_EXSTYLE, (LONG) m_lSaveExStyleWnd);

			// Make sure window style changes are applied
			SetWindowPos((HWND) m_pWindow->GetWindowHandle(), NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

			::ShowWindow((HWND) m_pWindow->GetWindowHandle(), SW_HIDE);
			//::ShowWindow( (HWND) m_pWindow->GetWindowHandle(), SW_SHOW );

			m_lSaveStyleWnd   = (long) GetWindowLongPtr((HWND) m_pWindow->GetWindowHandle(), GWL_STYLE);
			m_lSaveExStyleWnd = (long) GetWindowLongPtr((HWND) m_pWindow->GetWindowHandle(), GWL_EXSTYLE);
		#endif

		m_iMultisampleTarget = 0x0001 << ((((uWindowStyle & CLU_WNDSTYLE_MULTISAMPLE_MASK) >> 8)) + 1);
		if (m_iMultisampleTarget < 2)
		{
			m_iMultisampleTarget = 2;
		}

		//glutInit(piArg, pcArg);
		//glutInitDisplayMode(uMode);
		//glutInitWindowSize(iSizeX, iSizeY);
		//glutInitWindowPosition(0, 0);
		m_iHWin = m_pGLUTWindow->number;//glutCreateWindow(pcName);

		//printf("Main Window: %d\n", m_iHMainWin);

		// Enter this pointer into map.
		sm_mapThis[m_iHWin] = this;
		glutSetWindow(m_iHWin);
		SetCurrentToFocusWindow();

		glutDisplayFunc(GLUTDisplay);
		glutReshapeFunc(GLUTReshape);
		glutMouseFunc(GLUTMouse);
		//glutKeyboardFunc(GLUTKey);
		glutIdleFunc(GLUTIdleFunc);
		glutMotionFunc(GLUTActiveMouseMove);
		glutPassiveMotionFunc(GLUTPassiveMouseMove);
		m_pWindow->end();

		#ifdef WIN32
			//BOOL bOK;
			//if ( pvParentHandle )
			//{
			//	DWORD ProcIDWnd, ProcIDParentWnd;

			//	//GetWindowThreadProcessId( (HWND) m_pWindow->GetWindowHandle(), &ProcIDWnd );
			//	//GetWindowThreadProcessId( (HWND) pvParentHandle, &ProcIDParentWnd );
			//	bOK = AttachThreadInput( GetCurrentThreadId(), GetWindowThreadProcessId( (HWND) pvParentHandle, &ProcIDParentWnd ), TRUE );
			//}
		#endif

		//m_iMainMenu = glutCreateMenu(GLUTMenu);
		//glutAttachMenu(GLUT_MIDDLE_BUTTON);

		bool bFirst = (sm_mapThis.size() == 1);

		if (!Init(bFirst))
		{
			return false;
		}

		return true;
	}

//////////////////////////////////////////////////////////////////////
/// Detach window

	bool COGLWinFLTK::DestroyWindow()
	{
		if (m_iHWin == 0)
		{
			return true;
		}

		if (m_bIsAnimated)
		{
			StopAnimation();
		}

		sm_mapThis.erase(m_iHWin);
		m_iHWin = 0;

		if (sm_mapThis.size() > 0)
		{
			glutSetWindow(sm_mapThis.begin()->first);
		}

		return true;
	}

//////////////////////////////////////////////////////////////////////
/// Get Current window
///
/// returns 0 if current window is invalid

	COGLWinFLTK* COGLWinFLTK::GetCurrentWindowPtr()
	{
		map<int, COGLWinFLTK*>::iterator itCur;

		if ((itCur = sm_mapThis.find(glutGetWindow())) == sm_mapThis.end())
		{
			return 0;
		}

		return itCur->second;
	}

//////////////////////////////////////////////////////////////////////
/// Get Focus window
///
/// returns 0 if focus window is invalid

	COGLWinFLTK* COGLWinFLTK::GetFocusWindowPtr()
	{
		map<int, COGLWinFLTK*>::iterator itCur;

		if ((itCur = sm_mapThis.find(sm_iFocusWin)) == sm_mapThis.end())
		{
			return 0;
		}

		return itCur->second;
	}

//////////////////////////////////////////////////////////////////////
/// Set focus to current window

	void COGLWinFLTK::SetCurrentToFocusWindow()
	{
		sm_iFocusWin = glutGetWindow();
	}

//////////////////////////////////////////////////////////////////////
/// Callback for 3D Mouse KeyDown Event

	void COGLWinFLTK::CB_Mouse3D_KeyDown(int iKeyCode)
	{
		COGLWinFLTK* pThis = GetFocusWindowPtr();

		if (pThis)
		{
			pThis->Mouse3D_KeyDown(iKeyCode);
		}
	}

//////////////////////////////////////////////////////////////////////
/// Callback for 3D Mouse KeyUp Event

	void COGLWinFLTK::CB_Mouse3D_KeyUp(int iKeyCode)
	{
		COGLWinFLTK* pThis = GetFocusWindowPtr();

		if (pThis)
		{
			pThis->Mouse3D_KeyUp(iKeyCode);
		}
	}

//////////////////////////////////////////////////////////////////////
/// Callback for 3D Mouse Move Event

	void COGLWinFLTK::CB_Mouse3D_Move(double dTX, double dTY, double dTZ, double dLength, double dRX, double dRY, double dRZ, double dAngle)
	{
		COGLWinFLTK* pThis = GetFocusWindowPtr();

		if (pThis)
		{
			pThis->Mouse3D_Move(dTX, dTY, dTZ, dLength, dRX, dRY, dRZ, dAngle);
		}
	}

//////////////////////////////////////////////////////////////////////
/// On Focus Event

	void COGLWinFLTK::OnFocus()
	{
		//if ( m_sbCOMInitOK )
		//	Init3DMouseDriver();
	}

//////////////////////////////////////////////////////////////////////
/// On UnFocus Event

	void COGLWinFLTK::OnUnFocus()
	{
		//if ( m_sbCOMInitOK )
		//	Finalize3DMouseDriver();
	}

//////////////////////////////////////////////////////////////////////
/// Set Window Position and Size

	bool COGLWinFLTK::SetWindowPosSize(int iX, int iY, int iSizeX, int iSizeY)
	{
		if (!m_pWindow)
		{
			return false;
		}

		#ifdef WIN32
			SetWindowPos((HWND) m_pWindow->GetWindowHandle(), HWND_TOP, iX, iY, iSizeX, iSizeY, SWP_NOZORDER);
		#else
			m_pWindow->position(iX, iY);
			m_pWindow->size(iSizeX, iSizeY);
		#endif

		m_iSaveX        = m_pWindow->x();
		m_iSaveY        = m_pWindow->y();
		m_iSaveW        = m_pWindow->w();
		m_iSaveH        = m_pWindow->h();
		m_iSaveMMCWidth = m_pMouseModeChoice->w();

		#ifdef WIN32
			m_lSaveStyleWnd   = (long) GetWindowLongPtr((HWND) m_pWindow->GetWindowHandle(), GWL_STYLE);
			m_lSaveExStyleWnd = (long) GetWindowLongPtr((HWND) m_pWindow->GetWindowHandle(), GWL_EXSTYLE);
		#endif

		return true;
	}

	bool COGLWinFLTK::ShowWindow()
	{
		if (!m_pWindow)
		{
			return false;
		}

		#ifdef WIN32
			::ShowWindow((HWND) m_pWindow->GetWindowHandle(), SW_SHOW);
			SetWindowPos((HWND) m_pWindow->GetWindowHandle(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
			//SetForegroundWindow( (HWND) m_pWindow->GetWindowHandle() );
		#endif

		return true;
	}

	bool COGLWinFLTK::HideWindow()
	{
		if (!m_pWindow)
		{
			return false;
		}

		#ifdef WIN32
			::ShowWindow((HWND) m_pWindow->GetWindowHandle(), SW_HIDE);
			//SetWindowPos( (HWND) m_pWindow->GetWindowHandle(), HWND_TOP, 0, 0, 0, 0, SWP_NOZORDER | SWP_HIDEWINDOW );
		#endif

		return true;
	}

//////////////////////////////////////////////////////////////////////
/// Get Time as double

//double COGLWinFLTK::GetTime()
//{
//	TTimeStruct Time;
//	GETTIME( &Time );
//
//	return ( double( Time.time ) + 1e-3 * double( Time.millitm ) );
//}

//////////////////////////////////////////////////////////////////////
/// Reset View When button on 3D Mouse is pressed
	void COGLWinFLTK::Mouse3D_KeyDown(int iKeyCode)
	{
		switch (iKeyCode)
		{
		case 31:// Fit
			MenuResetView(0);
			break;
		}
	}

//////////////////////////////////////////////////////////////////////
/// Callback Quit

	void COGLWinFLTK::Quit()
	{
		exit(0);
	}

//////////////////////////////////////////////////////////////////////
// Callback function of info window

	const char* COGLWinFLTK::InfoLink(Fl_Widget* pWidget, const char* pcURL)
	{
		return 0;	// Do nothing when link is pressed
	}

//////////////////////////////////////////////////////////////////////
/// Callback when window is closed

	void COGLWinFLTK::CB_Quit(Fl_Widget* pWidget, void* pData)
	{
		if (pData)
		{
			((COGLWinFLTK*) pData)->Quit();
		}
	}

//////////////////////////////////////////////////////////////////////
/// Callback when link is pressed in info window

	const char* COGLWinFLTK::CB_InfoLink(Fl_Widget* pWidget, const char* pcURL, void* pvData)
	{
		if (pvData)
		{
			return ((COGLWinFLTK*) pvData)->InfoLink(pWidget, pcURL);
		}

		return 0;
	}

//////////////////////////////////////////////////////////////////////
/// Callback when tool has changed
// Entered by functions AddTool... as callback function
// Hence this is called each time a tool changes.

	void COGLWinFLTK::CB_ToolChanged(Fl_Widget* pWidget, void* pData)
	{
		if (pData)
		{
			string sName = pWidget->label();

			((COGLWinFLTK*) pData)->ToolChanged(sName);
		}
	}

//////////////////////////////////////////////////////////////////////
/// Callback when menu item has been selected

	void COGLWinFLTK::CB_MenuItemSelected(Fl_Widget* pWidget)
	{
		if (pWidget)
		{
			const Fl_Menu_Item* pItem = ((Fl_Menu_*) pWidget)->mvalue();
			int iIdx                  = ((Fl_Menu_*) pWidget)->value();

			string sName = ((COGLWinFLTK*) pItem->user_data_)->m_vecMenuItemID[iIdx];

			if (pItem->flags & (FL_MENU_TOGGLE | FL_MENU_RADIO))
			{
				if (pItem->checked())
				{
					sName += ":1";
				}
				else
				{
					sName += ":0";
				}
			}

			((COGLWinFLTK*) pItem->user_data_)->ToolChanged(sName);
		}
	}

//////////////////////////////////////////////////////////////////////
/// Callback when tool has changed

	void COGLWinFLTK::ToolChanged(string sName)
	{
		//printf("%s\n", sName.c_str());

		if (!m_bIsAnimated)
		{
			Display();
		}
	}

//////////////////////////////////////////////////////////////////////
/// Loop over tools and redraw them if necessary

	void COGLWinFLTK::CheckToolBoxRedraw()
	{
		if (m_pToolBox)
		{
			m_pToolBox->CheckToolsRedraw();
		}
	}

//////////////////////////////////////////////////////////////////////
/// Loop over tools and redraw them if necessary

	void COGLWinFLTK::CheckToolBarRedraw()
	{
		if (m_pToolBox)
		{
			m_pToolBar->CheckToolsRedraw();
		}
	}

//////////////////////////////////////////////////////////////////////
/// Get value of a tool

	bool COGLWinFLTK::GetToolValue(const char* pcName, double& rVal)
	{
		bool bOK;

		if (!m_pToolBox)
		{
			rVal = 0.0;
			return true;
		}

		if (!(bOK = m_pToolBox->GetToolValue(pcName, rVal)))
		{
			return m_pToolBar->GetToolValue(pcName, rVal);
		}

		return bOK;
	}

//////////////////////////////////////////////////////////////////////
/// Get value of a tool

	bool COGLWinFLTK::GetToolValue(const char* pcName, string& rVal)
	{
		bool bOK;

		if (!m_pToolBox)
		{
			rVal = "";
			return true;
		}

		if (!(bOK = m_pToolBox->GetToolValue(pcName, rVal)))
		{
			return m_pToolBar->GetToolValue(pcName, rVal);
		}

		return bOK;
	}

//////////////////////////////////////////////////////////////////////
/// Add a slider tool

	bool COGLWinFLTK::AddToolSlider(const char* pcName, double dMin, double dMax, double dStep, double dVal, bool bFixed, const char* pcDesc)
	{
		bool bNew = false, bRedraw = false;

		if (!m_pToolBox)
		{
			return true;
		}

		// Do not allow tool to update packs
		// This is done explicitly later
		//m_pToolBar->AddToolSlider(pcName, dMin, dMax, dStep, dVal, bFixed, pcDesc, false);
		if (m_pToolBox->AddToolSlider(bNew, bRedraw, pcName, dMin, dMax, dStep, dVal, bFixed, pcDesc, false))
		{
			m_bDoUpdateTools |= bNew;
			m_bDoRedrawTools |= bRedraw;
			return true;
		}

		return false;
	}

//////////////////////////////////////////////////////////////////////
/// Add a counter tool

	bool COGLWinFLTK::AddToolCounter(const char* pcName, double dMin, double dMax, double dStep, double dLStep, double dVal, bool bFixed, const char* pcDesc)
	{
		bool bNew = false, bRedraw = false;

		if (!m_pToolBox)
		{
			return true;
		}

		// Do not allow tool to update packs
		// This is done explicitly later
		if (m_pToolBox->AddToolCounter(bNew, bRedraw, pcName, dMin, dMax, dStep, dLStep, dVal, bFixed, pcDesc, false))
		{
			m_bDoUpdateTools |= bNew;
			m_bDoRedrawTools |= bRedraw;
			return true;
		}

		return false;
	}

//////////////////////////////////////////////////////////////////////
/// Add a slider tool

	bool COGLWinFLTK::AddToolInput(const char* pcName, double dMin, double dMax, double dVal, bool bFixed, const char* pcDesc)
	{
		bool bNew = false, bRedraw = false;

		if (!m_pToolBox)
		{
			return true;
		}

		// Do not allow tool to update packs
		// This is done explicitly later
		if (m_pToolBox->AddToolInput(bNew, bRedraw, pcName, dMin, dMax, dVal, bFixed, pcDesc, false))
		{
			m_bDoUpdateTools |= bNew;
			m_bDoRedrawTools |= bRedraw;
			return true;
		}

		return false;
	}

//////////////////////////////////////////////////////////////////////
/// Add a text input tool

	bool COGLWinFLTK::AddToolInputText(const char* pcName,          bool bImmediateCallback, const char* pcInitText, bool bFixed, const char* pcDesc, bool bDoUpdate)
	{
		bool bNew = false, bRedraw = false;

		if (!m_pToolBox)
		{
			return true;
		}

		if (m_pToolBox->AddToolInputText(bNew, bRedraw,
					pcName, bImmediateCallback, pcInitText,
					bFixed, pcDesc, bDoUpdate))
		{
			m_bDoUpdateTools |= bNew;
			m_bDoRedrawTools |= bRedraw;
			return true;
		}

		return false;
	}

//////////////////////////////////////////////////////////////////////
/// Add a check box tool

	bool COGLWinFLTK::AddToolCheckBox(const char* pcName, bool bSet, bool bFixed, const char* pcDesc)
	{
		bool bNew = false, bRedraw = false;

		if (!m_pToolBox)
		{
			return true;
		}

		if (m_pToolBox->AddToolCheckBox(bNew, bRedraw, pcName, bSet, bFixed, pcDesc, false))
		{
			m_bDoUpdateTools |= bNew;
			m_bDoRedrawTools |= bRedraw;
			return true;
		}

		return false;
	}

//////////////////////////////////////////////////////////////////////
/// Add a button tool

	bool COGLWinFLTK::AddToolButton(const char* pcName, bool bFixed,        const char* pcDesc)
	{
		bool bNew = false, bRedraw = false;

		if (!m_pToolBox)
		{
			return true;
		}

		if (m_pToolBox->AddToolButton(bNew, bRedraw, pcName, bFixed, pcDesc, false))
		{
			m_bDoUpdateTools |= bNew;
			m_bDoRedrawTools |= bRedraw;
			return true;
		}

		return false;
	}

//////////////////////////////////////////////////////////////////////
/// Add an image button tool

	bool COGLWinFLTK::AddToolImageButton(const char* pcName, COGLImage* pImgAct, COGLImage* pImgDeact, bool bFixed, const char* pcDesc)
	{
		if (!m_pToolBox)
		{
			return true;
		}

		return m_pToolBox->AddToolImageButton(pcName, pImgAct, pImgDeact, bFixed, pcDesc, false);
	}

//////////////////////////////////////////////////////////////////////
/// Add a choice tool

	bool COGLWinFLTK::AddToolChoice(const char* pcName, vector<string>& vecData, const char* pcChoice, bool bFixed, const char* pcDesc, bool bDoUpdate)
	{
		bool bNew = false, bRedraw = false;

		if (!m_pToolBox)
		{
			return true;
		}

		if (m_pToolBox->AddToolChoice(bNew, bRedraw, pcName, vecData, pcChoice, bFixed, pcDesc, bDoUpdate))
		{
			m_bDoUpdateTools |= bNew;
			m_bDoRedrawTools |= bRedraw;
			return true;
		}

		return false;
	}

//////////////////////////////////////////////////////////////////////
/// Add an image button toolbar

	bool COGLWinFLTK::AddToolbarImageButton(const char* pcName, COGLImage* pImgAct, COGLImage* pImgDeact, bool bFixed, const char* pcDesc)
	{
		bool bNew = false, bRedraw = false;

		if (!m_pToolBar)
		{
			return true;
		}

		if (m_pToolBar->AddToolbarImageButton(bNew, bRedraw, pcName, pImgAct, pImgDeact, bFixed, pcDesc, false))
		{
			m_bDoUpdateTools |= bNew;
			m_bDoRedrawTools |= bRedraw;
			return true;
		}

		return false;
	}

//////////////////////////////////////////////////////////////////////
/// Add a counter tool

	bool COGLWinFLTK::AddToolbarCounter(const char* pcName,
		const char* pcUnit,
		double dMin,
		double dMax,
		double dStep,
		double dLStep,
		double dVal,
		bool bActive,
		bool bFixed,
		const char* pcDesc)
	{
		bool bNew = false, bRedraw = false;

		if (!m_pToolBar)
		{
			return true;
		}

		// Do not allow tool to update packs
		// This is done explicitly later
		if (m_pToolBar->AddToolbarCounter(bNew, bRedraw, pcName, pcUnit, dMin, dMax, dStep, dLStep, dVal,
					bActive, bFixed, pcDesc, false))
		{
			m_bDoUpdateTools |= bNew;
			m_bDoRedrawTools |= bRedraw;
			return true;
		}

		return false;
	}

//////////////////////////////////////////////////////////////////////
/// Add a label to the toolbar

	bool COGLWinFLTK::AddToolbarLabel(const char* pcName, bool bFixed, const char* pcDesc)
	{
		bool bNew = false, bRedraw = false;

		if (!m_pToolBar)
		{
			return true;
		}

		if (m_pToolBar->AddToolbarLabel(bNew, bRedraw, pcName, bFixed, pcDesc, false))
		{
			m_bDoUpdateTools |= bNew;
			m_bDoRedrawTools |= bRedraw;
			return true;
		}

		return false;
	}

//////////////////////////////////////////////////////////////////////
// Info Window Functions

	bool COGLWinFLTK::SetInfoText(const char* pcText)
	{
		if (!m_pInfoWin)
		{
			return true;
		}

		if (!pcText)
		{
			return false;
		}

		m_pInfoWin->value(pcText);

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Set Width of Info window in percent

	bool COGLWinFLTK::SetInfoWidth(double dWidth)
	{
		if (!m_pInfoWin)
		{
			return true;
		}

		int iPixWidth = m_pTile->w();
		int iDeltaWidth;
		int iCurWidth = m_pInfoWin->w();
		int iPosY     = m_pInfoWin->y();

		if (dWidth <= 0)
		{
			dWidth = 0;
		}

		if ((dWidth >= 0) && (dWidth < 1))
		{
			// interpret width as percentage of max width
			iPixWidth = int(double(iPixWidth) * dWidth);
		}
		else
		{
			// interpret width as pixel width
			if (int(dWidth) >= iPixWidth)
			{
				iPixWidth--;
			}
			else
			{
				iPixWidth = int(dWidth);
			}
		}

		if (iPixWidth < m_iMinInfoWidth)
		{
			iPixWidth = m_iMinInfoWidth;
		}

		iDeltaWidth = iPixWidth - iCurWidth;

		m_pTile->position(m_iInfoWinInitX, m_iInfoWinInitY, iPixWidth, iPosY);

		//Reshaping is not allowed in this function. We are in thread, that has no ownership of the OpenGL Context
		//CCLUDrawBase::Reshape(m_pGLUTWindow->w() - iDeltaWidth, m_pGLUTWindow->h());

//	if (!m_bIsAnimated)
//		glutPostRedisplay();

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Get Size of OGL Window in pixel

	bool COGLWinFLTK::GetOGLSize(double& dWidth, double& dHeight)
	{
		if (!m_pGLUTWindow || !m_pWindow)
		{
			return false;
		}

		dWidth  = double(m_pGLUTWindow->w());
		dHeight = double(m_pGLUTWindow->h());

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Set Size of OGL Window in pixel or percent

	bool COGLWinFLTK::SetOGLSize(double dWidth, double dHeight)
	{
		if (!m_pGLUTWindow || !m_pWindow)
		{
			return false;
		}

		int iScrWidth  = Fl::w();
		int iScrHeight = Fl::h();
		int iCurWidth  = m_pWindow->w();
		int iCurHeight = m_pWindow->h();
		int iInfoWidth = m_pInfoWin->w();
		int iToolWidth, iToolHeight;
		int iToolbarWidth, iToolbarHeight;
		int iNewWidth, iNewHeight;

		m_pToolBox->GetSize(iToolWidth, iToolHeight);
		m_pToolBar->GetSize(iToolbarWidth, iToolbarHeight);

		if (dWidth < 0)
		{
			dWidth = 0;
		}

		if (dHeight < 0)
		{
			dHeight = 0;
		}

		if ((dWidth >= 0) && (dWidth <= 1))
		{
			iNewWidth = int(double(iScrWidth) * dWidth);
		}
		else
		{
			iNewWidth = int(dWidth);
		}

		if ((dHeight >= 0) && (dHeight <= 1))
		{
			iNewHeight = int(double(iScrHeight) * dHeight);
		}
		else
		{
			iNewHeight = int(dHeight);
		}

		iNewWidth  += iInfoWidth;
		iNewHeight += iToolHeight + iToolbarHeight + m_iMenuHeight;

		// Minimal Width
		if (iNewWidth < 50)
		{
			iNewWidth = 50;
		}
		// Maximal Width
		//else if (iNewWidth > iScrWidth-10)
		//iNewWidth = iScrWidth-10;

		// Minimal Height
		if (iNewHeight < 50)
		{
			iNewHeight = 50;
		}
		// Maximal Height
		//else if (iNewHeight > iScrHeight-10)
		//iNewHeight = iScrHeight-10;

		if ((iNewWidth == iCurWidth) &&
			(iNewHeight == iCurHeight))
		{
			return true;
		}

		m_pWindow->size(iNewWidth, iNewHeight);
//	m_pWindow->show();
//	CCLUDrawBase::Reshape(iNewWidth, iNewHeight);
//	CCLUDrawBase::Reshape(m_pGLUTWindow->w(), m_pGLUTWindow->h());

//	if (!m_bIsAnimated)
//		glutPostRedisplay();

/*
		int iPixWidth = m_pTile->w();
		int iPixHeight = m_pTile->h();
		int iDeltaWidth, iDeltaHeight;
		int iCurWidth = m_pGLUTWindow->w();
		int iCurHeight = m_pGLUTWindow->h();
		int iPosX = m_pGLUTWindow->x();
		int iPosY = m_pGLUTWindow->y();

		if (dWidth < 0)
				dWidth = 0;

		if (dHeight < 0)
				dHeight = 0;

		if (dWidth >= 0 && dWidth < 1)
		{
				// interpret width as percentage of max width
				iPixWidth = int( double(iPixWidth) * dWidth );
		}
		else
		{
				// interpret width as pixel width
				if (int(dWidth) >= iPixWidth)
						iPixWidth--;
				else
						iPixWidth = int(dWidth);
		}

				iPixHeight = int( double(iPixHeight) * dHeight );

		if (iPixWidth < m_iMinInfoWidth)
				iPixWidth = m_iMinInfoWidth;

		iDeltaWidth = iPixWidth - iCurWidth;

		m_pTile->position(m_iInfoWinInitX, m_iInfoWinInitY, iPixWidth, iPosY);
		CCLUDrawBase::Reshape(m_pGLUTWindow->w() - iDeltaWidth, m_pGLUTWindow->h());

//	if (!m_bIsAnimated)
//		glutPostRedisplay();
*/
		return true;
	}

//////////////////////////////////////////////////////////////////////
/// Show Window

//void COGLWinFLTK::ShowWindow(bool bVal)
//{
//	if (m_pWindow)
//	{
//		if (bVal)
//			m_pWindow->show();
//		else
//			m_pWindow->hide();
//	}
//}

//////////////////////////////////////////////////////////////////////
/// Hide Menu

	void COGLWinFLTK::HideMenu(bool bVal)
	{
		if (m_pMenuBar)
		{
			if (bVal)
			{
				m_pMenuBar->resize(0, 0, m_iSizeX, 1);
				m_pGLUTWindow->resize(0, 1, m_iSizeX, m_iSizeY + 29);
			}
			else
			{
				m_pGLUTWindow->resize(0, 30, m_iSizeX, m_iSizeY - 30);
				m_pMenuBar->resize(0, 0, m_iSizeX, 30);
			}
		}
	}

//////////////////////////////////////////////////////////////////////
/// Create OpenGL Window with GLUT
/*
bool COGLWinFLTK::Create(int *piArg, char **pcArg, MemObj<SGLUTWin>& mWin)
{


		return true;
}

*/

	bool COGLWinFLTK::EscapePressed()
	{
		if (m_bInFullScreenMode && m_bUserToggleFullScreen)
		{
			EnableFullScreen(false);
			if (m_pFullScreenMenuItem)
			{
				m_pFullScreenMenuItem->uncheck();
			}

			return true;
		}

		return false;
	}

	void COGLWinFLTK::Key(unsigned char cKey, int iX, int iY)
	{
		// Do not react to keys.
		// Functionality is now available through menu,
	}

	void COGLWinFLTK::KeyUp(unsigned char cKey)
	{
	}

	void COGLWinFLTK::KeyDown(unsigned char cKey)
	{
	}

	bool COGLWinFLTK::Keyboard(int iKey, bool bKeyDown)
	{
		m_bShiftDown = ((Fl::event_state() & FL_SHIFT) != 0);
		m_bCtrlDown  = ((Fl::event_state() & FL_CTRL) != 0);
		m_bAltDown   = ((Fl::event_state() & FL_ALT) != 0);

		m_bNumLock    = ((Fl::event_state() & FL_NUM_LOCK) != 0);
		m_bScrollLock = ((Fl::event_state() & FL_SCROLL_LOCK) != 0);
		m_bCapsLock   = ((Fl::event_state() & FL_CAPS_LOCK) != 0);

		return true;
	}

	void COGLWinFLTK::InitMenu()
	{
		Fl_Menu_Item pMenuItems[] =
		{
			{ "&Visualization", 0, 0, 0, FL_SUBMENU },
			{ "&Animate Code",        FL_CTRL + 'a', (Fl_Callback*) CB_Animate, 0, FL_MENU_TOGGLE },
			{ "&Mouse animation",    FL_CTRL + 'm', (Fl_Callback*) CB_MouseAnim, 0, FL_MENU_TOGGLE },
			{ "&Local Rotation",    FL_CTRL + 'l', (Fl_Callback*) CB_LocalRot, 0, FL_MENU_TOGGLE },
			{ "&View Rotation",    FL_CTRL + 'v', (Fl_Callback*) CB_ViewMode, 0, FL_MENU_TOGGLE },
			{ "&Full Screen",        FL_CTRL + 'f', (Fl_Callback*) CB_FullScreen, 0, FL_MENU_TOGGLE },
			{ "&Reset View",        FL_CTRL + 'r', (Fl_Callback*) CB_ResetView },
			{ "Reset Mouse M&odes",        FL_CTRL + 'o', (Fl_Callback*) CB_ResetMouseModes },
			{ "Save &Image...", FL_CTRL + 's', (Fl_Callback*) CB_SaveImage },
			{ "&Quick Save Image", FL_CTRL + 'q', (Fl_Callback*) CB_QuickSaveImage },
			{ 0 },
/*
				{ "&Mouse Mode", 0, 0, 0, FL_SUBMENU },
				{ "Standard", FL_CTRL + '0', (Fl_Callback *) CB_MouseMode, (void *) '0', FL_MENU_RADIO |FL_MENU_VALUE },
				{ "Mode &1", FL_CTRL + '1', (Fl_Callback *) CB_MouseMode, (void *) '1', FL_MENU_RADIO },
				{ "Mode &2", FL_CTRL + '2', (Fl_Callback *) CB_MouseMode, (void *) '2', FL_MENU_RADIO },
				{ "Mode &3", FL_CTRL + '3', (Fl_Callback *) CB_MouseMode, (void *) '3', FL_MENU_RADIO },
				{ "Mode &4", FL_CTRL + '4', (Fl_Callback *) CB_MouseMode, (void *) '4', FL_MENU_RADIO },
				{ "Mode &5", FL_CTRL + '5', (Fl_Callback *) CB_MouseMode, (void *) '5', FL_MENU_RADIO },
				{ "Mode &6", FL_CTRL + '6', (Fl_Callback *) CB_MouseMode, (void *) '6', FL_MENU_RADIO },
				{ "Mode &7", FL_CTRL + '7', (Fl_Callback *) CB_MouseMode, (void *) '7', FL_MENU_RADIO },
				{ "Mode &8", FL_CTRL + '8', (Fl_Callback *) CB_MouseMode, (void *) '8', FL_MENU_RADIO },
				{ "Mode &9", FL_CTRL + '9', (Fl_Callback *) CB_MouseMode, (void *) '9', FL_MENU_RADIO },
				{ 0 },
*/
			{ 0 }
		};

		m_vecMenuItem.clear();
		m_vecMenuItem.resize(12);

		for (int i = 0; i < 12; i++)
		{
			m_vecMenuItem[i] = pMenuItems[i];
		}
	}

///////////////////////////////////////////////////////////////////////////
/// Set Menu

	bool COGLWinFLTK::SetMenu(vector<SMenuItem>& vecMenuItem)
	{
		int iIdx, iCnt = int(vecMenuItem.size());
		vector< Fl_Menu_Item > vecFlMenuItem;
		vector<string> vecMenuID;
		vector<string>::iterator it_ID;

		// Reset to standard menu when no menu items are given
		if (iCnt == 0)
		{
			m_pMenuBar->clear();
			m_pMenuBar->copy(&m_vecMenuItem[0], this);
			m_pMenuBar->redraw();
			m_bUserMenuActive = false;

			return true;
		}

		// Copy given menu items to store strings while menu is shown
		m_vecCLUMenuItem = vecMenuItem;

		// Set size of menu item array
		vecFlMenuItem.resize(iCnt + 1);
		// Set size of menu item ID array
		m_vecMenuItemID.resize(iCnt + 1);
		// Initialize Menu Item ID
		vecMenuID.push_back("Menu");

		for (iIdx = 0; iIdx < iCnt + 1; ++iIdx)
		{
			Fl_Menu_Item& rFlItem = vecFlMenuItem[iIdx];
			string& rItemID       = m_vecMenuItemID[iIdx];

			// Standard values
			rFlItem.labeltype_  = 0;
			rFlItem.labelfont_  = 0;
			rFlItem.labelsize_  = 0;
			rFlItem.labelcolor_ = 0;

			if ((iIdx == iCnt) || (m_vecCLUMenuItem[iIdx].eType == MI_END))
			{
				rFlItem.text       = 0;
				rFlItem.shortcut_  = 0;
				rFlItem.callback_  = 0;
				rFlItem.user_data_ = 0;
				rFlItem.flags      = 0;

				if (vecMenuID.size() > 1)
				{
					vecMenuID.pop_back();
				}
			}
			else
			{
				CCLUDrawBase::SMenuItem& rItem = m_vecCLUMenuItem[iIdx];

				rFlItem.text = rItem.sName.c_str();

				if (rItem.cKey > char(/*EMenuKeySpecial::*/ MKEY_MAX))
				{
					rFlItem.shortcut_ = unsigned long(rItem.cKey);
				}
				else
				{
					int iKeyVal = int(EMenuKeySpecial(rItem.cKey));

					if (EMenuKeySpecial(rItem.cKey) == MKEY_ESCAPE)
					{
						rFlItem.shortcut_ = FL_Escape;
					}
					else if ((iKeyVal >= int(MKEY_F1)) && (iKeyVal < int(MKEY_FEND)))
					{
						rFlItem.shortcut_ = FL_F + iKeyVal - int(MKEY_F1) + 1;
					}
					else
					{
						rFlItem.shortcut_ = 0;
					}
				}

				if (rItem.eModifier & MKEY_ALT)
				{
					rFlItem.shortcut_ += FL_ALT;
				}

				if (rItem.eModifier & MKEY_CTRL)
				{
					rFlItem.shortcut_ += FL_CTRL;
				}

				if (rItem.eModifier & MKEY_SHIFT)
				{
					rFlItem.shortcut_ += FL_SHIFT;
				}

				if (rItem.sCallback.size() == 0)
				{
					rFlItem.callback_ = (Fl_Callback*) CB_MenuItemSelected;
				}
				else if (rItem.sCallback == "CB::Animate")
				{
					rFlItem.callback_ = (Fl_Callback*) CB_Animate;
				}
				else if (rItem.sCallback == "CB::MouseAnimate")
				{
					rFlItem.callback_ = (Fl_Callback*) CB_MouseAnim;
				}
				else if (rItem.sCallback == "CB::LocalRotation")
				{
					rFlItem.callback_ = (Fl_Callback*) CB_LocalRot;
				}
				else if (rItem.sCallback == "CB::FullScreen")
				{
					rFlItem.callback_ = (Fl_Callback*) CB_FullScreen;
				}
				else if (rItem.sCallback == "CB::ResetView")
				{
					rFlItem.callback_ = (Fl_Callback*) CB_ResetView;
				}
				else if (rItem.sCallback == "CB::ResetMouseModes")
				{
					rFlItem.callback_ = (Fl_Callback*) CB_ResetMouseModes;
				}
				else if (rItem.sCallback == "CB::SaveImage")
				{
					rFlItem.callback_ = (Fl_Callback*) CB_SaveImage;
				}
				else if (rItem.sCallback == "CB::QuickSaveImage")
				{
					rFlItem.callback_ = (Fl_Callback*) CB_QuickSaveImage;
				}
				else if (rItem.sCallback == "CB::Quit")
				{
					rFlItem.callback_ = (Fl_Callback*) CB_Quit;
				}
				else
				{
					rFlItem.callback_ = (Fl_Callback*) 0;
				}

				/*
				else if ( rItem.sCallback == "CB::" )
				{
				rFlItem.callback_ = (Fl_Callback *) CB_;
				}
				*/

				rFlItem.user_data_ = (void*) this;

				switch (rItem.eType)
				{
				case MI_TEXT:
					rFlItem.flags = 0;
					break;

				case MI_CHECKBOX:
					rFlItem.flags = FL_MENU_TOGGLE;
					break;

				case MI_RADIO:
					rFlItem.flags = FL_MENU_RADIO;
					break;

				case MI_SUBMENU:
					rFlItem.flags = FL_SUBMENU;
					break;

				default:
					rFlItem.flags = 0;
					break;
				}

				if (rItem.bInactive)
				{
					rFlItem.flags |= FL_MENU_INACTIVE;
				}

				if (rItem.bInvisible)
				{
					rFlItem.flags |= FL_MENU_INVISIBLE;
				}

				if (rItem.bDivider)
				{
					rFlItem.flags |= FL_MENU_DIVIDER;
				}

				if (rItem.bSelected)
				{
					rFlItem.flags |= FL_MENU_VALUE;
				}

				rItemID = "";
				for (it_ID = vecMenuID.begin(); it_ID != vecMenuID.end(); ++it_ID)
				{
					rItemID += *it_ID;
					rItemID += ":";
				}

				rItemID += rFlItem.text;

				if (rFlItem.flags == FL_SUBMENU)
				{
					vecMenuID.push_back(string(rFlItem.text));
				}
			}
		}

		// Set new menu
		m_pMenuBar->clear();
		m_pMenuBar->copy(&vecFlMenuItem[0], this);
		m_pMenuBar->redraw();
		m_bUserMenuActive = true;

		return true;
	}

///////////////////////////////////////////////////////////////////////////
/// Enable Menu

	bool COGLWinFLTK::EnableMenu(bool bVal)
	{
//	if ( m_bInFullScreenMode )
//		return false;

		int iCurY, iNewY;

		if (bVal)	//&& !m_bMenuShown)
		{
			if (m_bStdCtrlShown)
			{
				iCurY = m_iMenuBarHeight + m_iMenuBarSep;
				iNewY = 2 * iCurY;
			}
			else
			{
				iCurY = 0;
				iNewY = m_iMenuBarHeight + m_iMenuBarSep;
			}

			m_pTile->resize(0, iNewY, m_pWindow->w(), m_pWindow->h() - iNewY);

			Fl_Widget* pDummyBox = m_pTile->resizable();
			pDummyBox->resize(1, iNewY,
					m_pWindow->w() - 50,
					m_pWindow->h() - iNewY - m_iMinToolHeight);

			m_pMenuBar->resize(0, 0, m_pWindow->w(), m_iMenuBarHeight);
			m_pMenuBar->show();

			if (m_bStdCtrlShown)
			{
				m_pMouseModeChoice->resize(6, iCurY,
						m_pMouseModeChoice->w(), m_iMenuBarHeight);
			}

			m_bMenuShown = true;
		}
		else if (!bVal)	//&& m_bMenuShown )
		{
			if (m_bStdCtrlShown)
			{
				iCurY = 2 * (m_iMenuBarHeight + m_iMenuBarSep);
				iNewY = m_iMenuBarHeight + 2 * m_iMenuBarSep;
			}
			else
			{
				iNewY = 0;
				iCurY = m_iMenuBarHeight + m_iMenuBarSep;
			}

			m_pMenuBar->resize(0, 0, m_pMenuBar->w(), 0);
			m_pMenuBar->hide();

			if (m_bStdCtrlShown)
			{
				m_pMouseModeChoice->resize(6, m_iMenuBarSep,
						m_pMouseModeChoice->w(), m_iMenuBarHeight);
			}

			m_pTile->resize(0, iNewY, m_pWindow->w(), m_pWindow->h() - iNewY);

			Fl_Widget* pDummyBox = m_pTile->resizable();
			pDummyBox->resize(1, iNewY,
					m_pWindow->w() - 50, m_pWindow->h() - iNewY - m_iMinToolHeight);

			m_bMenuShown = false;
		}

		//glutPostRedisplay();
		//m_pWindow->draw();//redraw();
//#ifdef WIN32
//	::ShowWindow( (HWND) m_pWindow->GetWindowHandle(), SW_HIDE );
//	::ShowWindow( (HWND) m_pWindow->GetWindowHandle(), SW_SHOW );
//#endif

		return true;
	}

///////////////////////////////////////////////////////////////////////////
/// Enable Standard Control

	bool COGLWinFLTK::EnableStdCtrl(bool bVal)
	{
//	if ( m_bInFullScreenMode )
//		return false;

		int iCurY, iNewY;

		if (bVal)	//&& !m_bStdCtrlShown )
		{
			if (m_bMenuShown)
			{
				iCurY = m_iMenuBarHeight + m_iMenuBarSep;
				iNewY = 2 * iCurY;
			}
			else
			{
				iCurY = 0;
				iNewY = m_iMenuBarHeight + 2 * m_iMenuBarSep;
			}

			m_pTile->resize(0, iNewY, m_pWindow->w(), m_pWindow->h() - iNewY);

			Fl_Widget* pDummyBox = m_pTile->resizable();
			pDummyBox->resize(1, iNewY,
					m_pWindow->w() - 50,
					m_pWindow->h() - iNewY - m_iMinToolHeight);

			//pDummyBox->resize(m_iMinInfoWidth, m_iMinToolBarHeight + iNewY - 2,
			//	m_pWindow->w() - 50,
			//	m_pWindow->h() - m_iMinToolBarHeight - iNewY + 2 - m_iMinToolHeight);

			//m_pTile->resizable( pDummyBox );
			//m_pTile->resize(0, iNewY, m_pWindow->w(), m_pWindow->h() - iNewY);

			m_pMouseModeChoice->show();
			m_pMouseModeChoice->resize(6, iCurY,
					m_pMouseModeChoice->w(), m_iMenuBarHeight);
			//m_pMouseModeChoice->redraw();
			//m_pWindow->redraw();

			m_bStdCtrlShown = true;
		}
		else if (!bVal)	// && m_bStdCtrlShown )
		{
			if (m_bMenuShown)
			{
				iCurY = 2 * (m_iMenuBarHeight + m_iMenuBarSep);
				iNewY = m_iMenuBarHeight + m_iMenuBarSep;
			}
			else
			{
				iCurY = m_iMenuBarHeight + 2 * m_iMenuBarSep;
				iNewY = 0;
			}

			m_pMouseModeChoice->hide();
			m_pMouseModeChoice->resize(0, 0, m_pMouseModeChoice->w(), 0);
			//m_pMouseModeChoice->redraw();
			//m_pWindow->redraw();

			m_pTile->resize(0, iNewY, m_pWindow->w(), m_pWindow->h() - iNewY);

			Fl_Widget* pDummyBox = m_pTile->resizable();
			pDummyBox->resize(1, iNewY,
					m_pWindow->w() - 50, m_pWindow->h() - iNewY - m_iMinToolHeight);

			//pDummyBox->resize(m_iMinInfoWidth, m_iMinToolBarHeight + iNewY - 2,
			//	m_pWindow->w() - 50,
			//	m_pWindow->h() - m_iMinToolBarHeight - iNewY + 2 - m_iMinToolHeight);

			//m_pTile->resizable( pDummyBox );

			m_bStdCtrlShown = false;
		}

//#ifdef WIN32
//	::ShowWindow( (HWND) m_pWindow->GetWindowHandle(), SW_HIDE );
//	::ShowWindow( (HWND) m_pWindow->GetWindowHandle(), SW_SHOW );
//#endif

		//glutPostRedisplay();
		//m_pWindow->draw();

		return true;
	}

	void COGLWinFLTK::MenuAnimate(Fl_Widget* pWidget)
	{
		CCLUDrawBase::Key('a', 0, 0);
	}

	void COGLWinFLTK::MenuMouseAnim(Fl_Widget* pWidget)
	{
		CCLUDrawBase::Key('m', 0, 0);
	}

	void COGLWinFLTK::MenuMouseMode(Fl_Widget* pWidget)
	{
		if (pWidget)
		{
			int iVal = ((Fl_Choice*) pWidget)->value();
			SetRTMode(iVal);
		}
	}

	bool COGLWinFLTK::SetMouseMode(int iMode)
	{
		if (m_pMouseModeChoice)
		{
			if (SetRTMode(iMode))
			{
				m_pMouseModeChoice->value(iMode);
				return true;
			}
		}

		return false;
	}

	void COGLWinFLTK::MenuSaveImage(Fl_Widget* pWidget)
	{
		//char *pcNewFile, *pcFilter = 0, *pcExt;
		//char pcFilename[512];

		//pcFilename[0] = 0;
		//Fl_File_Chooser::sort = fl_casenumericsort;
		//pcNewFile = fl_file_chooser("Save Screen Image",
		//	"PNG Files (*.png)\t"
		//	"JPEG Files (*.jpg)\t"
		//	"Bitmap Files (*.bmp)\t"
		//	"Postscript Files (*.eps)\t"
		//	"Save as JPEG + EPS (*.jps)\t"
		//	"Save PNG with Bounding Box (*.bpng)"
		//	, pcFilename, 0, &pcFilter);

		//if (pcNewFile != NULL)
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

			GLUTDisplay();
			CCLUDrawBase::SaveScreen2BMP(sFile.c_str());
		}
	}

	void COGLWinFLTK::MenuQuickSaveImage(Fl_Widget* pWidget)
	{
		char pcFilename[512];

		sprintf_s(pcFilename, 512, "CLUCalcImg_%04d.jpg", ++m_iImgID);

		GLUTDisplay();
		CCLUDrawBase::SaveScreen2BMP(pcFilename);
	}

	void COGLWinFLTK::MenuLocalRot(Fl_Widget* pWidget)
	{
		if (pWidget)
		{
			const Fl_Menu_Item* pItem = ((Fl_Menu_*) pWidget)->mvalue();
			if (pItem->checked())
			{
				EnableLocalRot(true);
			}
			else
			{
				EnableLocalRot(false);
			}
		}
	}

	void COGLWinFLTK::MenuViewMode(Fl_Widget* pWidget)
	{
		if (pWidget)
		{
			const Fl_Menu_Item* pItem = ((Fl_Menu_*) pWidget)->mvalue();
			if (pItem->checked())
			{
				EnableRTViewMode(true);
			}
			else
			{
				EnableRTViewMode(false);
			}
		}
	}

	void COGLWinFLTK::MenuFullScreen(Fl_Widget* pWidget)
	{
		if (pWidget && m_bUserToggleFullScreen)
		{
			const Fl_Menu_Item* pItem = ((Fl_Menu_*) pWidget)->mvalue();

			// Store for use when ESC is pressed in full screen mode.
			m_pFullScreenMenuItem = (Fl_Menu_Item*) pItem;

			if (pItem->checked())
			{
				EnableFullScreen(true);
			}
			else
			{
				EnableFullScreen(false);
			}
		}
	}

	void COGLWinFLTK::EnableFullScreen(bool bVal)
	{
		int iNewY;

		if (m_bInFullScreenMode == bVal)
		{
			return;
		}

		if (m_bMenuShown && m_bStdCtrlShown)
		{
			iNewY = 2 * (m_iMenuBarHeight + m_iMenuBarSep);
		}
		else if (m_bMenuShown)
		{
			iNewY = m_iMenuBarHeight + m_iMenuBarSep;
		}
		else if (m_bStdCtrlShown)
		{
			iNewY = m_iMenuBarHeight + m_iMenuBarSep;
		}
		else
		{
			iNewY = 0;
		}

		if (bVal)
		{
			m_iSaveX        = m_pWindow->x();
			m_iSaveY        = m_pWindow->y();
			m_iSaveW        = m_pWindow->w();
			m_iSaveH        = m_pWindow->h();
			m_iSaveMMCWidth = m_pMouseModeChoice->w();

			#ifndef WIN32
				m_pWindow->fullscreen();
			#else
				RECT rectWnd;
				MONITORINFO infoMonitor;
				infoMonitor.cbSize = sizeof(MONITORINFO);

				rectWnd.left   = m_iSaveX;
				rectWnd.top    = m_iSaveY;
				rectWnd.right  = m_iSaveX + m_iSaveW - 1;
				rectWnd.bottom = m_iSaveY + m_iSaveH - 1;

				HMONITOR hMonitor = MonitorFromRect(&rectWnd, MONITOR_DEFAULTTONEAREST);
				GetMonitorInfo(hMonitor, &infoMonitor);

				m_lSaveStyleWnd = (long) GetWindowLongPtr((HWND) m_pWindow->GetWindowHandle(), GWL_STYLE);
				SetWindowLongPtrA((HWND) m_pWindow->GetWindowHandle(), GWL_STYLE, (LONG) (m_lSaveStyleWnd & ~WS_CAPTION & ~WS_SIZEBOX));

				m_pWindow->border(0);
				m_pWindow->resize(infoMonitor.rcWork.left, infoMonitor.rcWork.top,
					infoMonitor.rcWork.right - infoMonitor.rcWork.left,
					infoMonitor.rcWork.bottom - infoMonitor.rcWork.top);

				m_pTile->resize(0, iNewY, m_pWindow->w(), m_pWindow->h() - iNewY);

				Fl_Widget* pDummyBox = m_pTile->resizable();
				pDummyBox->resize(1, iNewY,
					m_pWindow->w() - 50,
					m_pWindow->h() - iNewY - m_iMinToolHeight);

				//SetWindowPos( (HWND) m_pWindow->GetWindowHandle(), HWND_TOP,
				//				infoMonitor.rcWork.left, infoMonitor.rcWork.top,
				//				infoMonitor.rcWork.right - infoMonitor.rcWork.left,
				//				infoMonitor.rcWork.bottom - infoMonitor.rcWork.top,
				//				SWP_NOZORDER );

			#endif

/*
				//m_pMenuBar->resize(0, 0, m_iSaveW, 0);
				m_pMouseModeChoice->resize(0, 0, m_iSaveMMCWidth, 0);
				m_pTile->resize(0, m_iMenuBarHeight, m_pWindow->w(), m_pWindow->h() - m_iMenuBarHeight);

				Fl_Widget* pDummyBox = m_pTile->resizable();
				//pDummyBox->resize(1, m_iToolHeight,
						//m_pWindow->w() - 50, m_pWindow->h() - m_iToolHeight - m_iMinToolHeight);
				pDummyBox->resize(1, m_iMenuBarHeight,
						m_pWindow->w() - 50, m_pWindow->h() - m_iMenuBarHeight);
*/

			m_bInFullScreenMode = true;
		}
		else
		{
			//m_pWindow->border( 1 );
			//m_pWindow->show();
			#ifdef WIN32
				SetWindowLongPtrA((HWND) m_pWindow->GetWindowHandle(), GWL_STYLE, (LONG) m_lSaveStyleWnd);
			#endif

			m_pWindow->border(1);
			m_pWindow->fullscreen_off(m_iSaveX, m_iSaveY, m_iSaveW, m_iSaveH);

			m_pTile->resize(0, iNewY, m_pWindow->w(), m_pWindow->h() - iNewY);

			Fl_Widget* pDummyBox = m_pTile->resizable();
			pDummyBox->resize(1, iNewY,
					m_pWindow->w() - 50,
					m_pWindow->h() - iNewY - m_iMinToolHeight);

			#ifdef WIN32
				//SetWindowPos( (HWND) m_pWindow->GetWindowHandle(), HWND_TOP,
				//				m_iSaveX, m_iSaveY, m_iSaveW, m_iSaveH,
				//				SWP_NOZORDER );

				::ShowWindow((HWND) m_pWindow->GetWindowHandle(), SW_HIDE);
				::ShowWindow((HWND) m_pWindow->GetWindowHandle(), SW_SHOW);
			#endif
			/*
					m_pTile->resize(0, m_iMenuHeight, m_pWindow->w(), m_pWindow->h() - m_iMenuHeight);

					Fl_Widget* pDummyBox = m_pTile->resizable();
					pDummyBox->resize(1, m_iMenuBarHeight + m_iToolHeight,
							m_pWindow->w() - 50,
							m_pWindow->h() - m_iMenuBarHeight - m_iToolHeight - m_iMinToolHeight);

					//m_pMenuBar->resize(0, 0, m_pWindow->w(), m_iMenuBarHeight);
					m_pMouseModeChoice->resize(6, m_iMenuBarHeight+m_iMenuBarSep,
							m_iSaveMMCWidth, m_iMenuBarHeight);
					//m_pGLUTWindow->show();
					//m_pMenuBar->show();
			*/

			//m_pWindow->fullscreen_off(m_iSaveX, m_iSaveY, m_iSaveW, m_iSaveH);
			//m_pWindow->show();
			m_bInFullScreenMode = false;

			//HideMenu(false);
		}
	}

	void COGLWinFLTK::MenuResetView(Fl_Widget* pWidget)
	{
		m_vxTransform[0].Reset();
		m_xCameraTransform.Reset();

		m_vRMain     = m_E3Base.vSC;
		m_vRViewMain = m_E3Base.vSC;
		m_vU         = m_E3Base.vSC;
		m_vAxis      = m_E3Base.vSC;

		m_vU    = 0;
		m_vAxis = 0;

		PostRedisplay();
		//if (!m_bIsAnimated)
		//	glutPostRedisplay();

		//UpdateMatrices();
	}

	void COGLWinFLTK::MenuResetMouseModes(Fl_Widget* pWidget)
	{
		ResetTransformVars();
		SetTransformVarsChanged();

		PostRedisplay();
		//if (!m_bIsAnimated)
		//	glutPostRedisplay();
	}

	void COGLWinFLTK::CB_Animate(Fl_Widget* pWidget, void* pData)
	{
		if (pData)
		{
			((COGLWinFLTK*) pData)->MenuAnimate(pWidget);
		}
	}

	void COGLWinFLTK::CB_LocalRot(Fl_Widget* pWidget, void* pData)
	{
		if (pData)
		{
			((COGLWinFLTK*) pData)->MenuLocalRot(pWidget);
		}
	}

	void COGLWinFLTK::CB_ViewMode(Fl_Widget* pWidget, void* pData)
	{
		if (pData)
		{
			((COGLWinFLTK*) pData)->MenuViewMode(pWidget);
		}
	}

	void COGLWinFLTK::CB_MouseAnim(Fl_Widget* pWidget, void* pData)
	{
		if (pData)
		{
			((COGLWinFLTK*) pData)->MenuMouseAnim(pWidget);
		}
	}

	void COGLWinFLTK::CB_MouseMode(Fl_Widget* pWidget, void* pData)
	{
		if (pData)
		{
			((COGLWinFLTK*) pData)->MenuMouseMode(pWidget);
		}
	}

	void COGLWinFLTK::CB_SaveImage(Fl_Widget* pWidget, void* pData)
	{
		if (pData)
		{
			((COGLWinFLTK*) pData)->MenuSaveImage(pWidget);
		}
	}

	void COGLWinFLTK::CB_QuickSaveImage(Fl_Widget* pWidget, void* pData)
	{
		if (pData)
		{
			((COGLWinFLTK*) pData)->MenuQuickSaveImage(pWidget);
		}
	}

	void COGLWinFLTK::CB_FullScreen(Fl_Widget* pWidget, void* pData)
	{
		if (pData)
		{
			((COGLWinFLTK*) pData)->MenuFullScreen(pWidget);
		}
	}

	void COGLWinFLTK::CB_ResetView(Fl_Widget* pWidget, void* pData)
	{
		if (pData)
		{
			((COGLWinFLTK*) pData)->MenuResetView(pWidget);
		}
	}

	void COGLWinFLTK::CB_ResetMouseModes(Fl_Widget* pWidget, void* pData)
	{
		if (pData)
		{
			((COGLWinFLTK*) pData)->MenuResetMouseModes(pWidget);
		}
	}

//////////////////////////////////////////////////////////////////////
/// Reshape Windpw

	void COGLWinFLTK::Reshape(int iWidth, int iHeight)
	{
		CCLUDrawBase::Reshape(iWidth, iHeight);

		m_pToolBox->Update();
		m_pToolBar->Update();

		EnableMenu(m_bMenuShown);
		EnableStdCtrl(m_bStdCtrlShown);

//	m_pInfoWin->leftline(m_iInfoLeftMargin);
//	m_pInfoWin->format();
	}

//////////////////////////////////////////////////////////////////////
/// Post Main Window Redisplay

//void COGLWinFLTK::PostMainWindowUpdate()
//{
//	//printf("PostMainWindowRedisplay (%d)\n", m_iHMainWin);
//	//COGLWinFLTK *pThis = sm_mapThis[m_iHMainWin];
//	//pThis->PostWindowUpdate();
//	//printf("PostMainWindowRedisplay Exit\n");
//}

	void COGLWinFLTK::PostWindowUpdate()
	{
	}

//////////////////////////////////////////////////////////////////////
/// Static functions to call virtual functions

	void COGLWinFLTK::GLUTDisplay()
	{
		// Get this pointer for current window
		COGLWinFLTK* pThis = GetCurrentWindowPtr();

		//printf("Display: %d\n", glutGetWindow());

		if (pThis)
		{
			pThis->Display();
		}
	}

	void COGLWinFLTK::GLUTReshape(int iWidth, int iHeight)
	{
		// Get this pointer for current window
		COGLWinFLTK* pThis = GetCurrentWindowPtr();

		if (pThis)
		{
			SetCurrentToFocusWindow();
			pThis->Reshape(iWidth, iHeight);
		}
	}

	void COGLWinFLTK::GLUTMouse(int iButton, int iState, int iX, int iY)
	{
		// Get this pointer for current window
		COGLWinFLTK* pThis = GetCurrentWindowPtr();

		if (pThis)
		{
			SetCurrentToFocusWindow();
			pThis->Mouse(iButton, iState, iX, iY);
		}
	}

	void COGLWinFLTK::GLUTKey(unsigned char cKey, int iX, int iY)
	{
		// Get this pointer for current window
		COGLWinFLTK* pThis = GetCurrentWindowPtr();

		if (pThis)
		{
			SetCurrentToFocusWindow();
			pThis->Key(cKey, iX, iY);
		}
	}

	void COGLWinFLTK::GLUTActiveMouseMove(int iX, int iY)
	{
		// Get this pointer for current window
		COGLWinFLTK* pThis = GetCurrentWindowPtr();

		if (pThis)
		{
			SetCurrentToFocusWindow();
			pThis->ActiveMouseMove(iX, iY);
		}
	}

	void COGLWinFLTK::GLUTPassiveMouseMove(int iX, int iY)
	{
		// Get this pointer for current window
		COGLWinFLTK* pThis = GetCurrentWindowPtr();

		if (pThis)
		{
			SetCurrentToFocusWindow();
			pThis->PassiveMouseMove(iX, iY);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void COGLWinFLTK::GLUTIdleFunc()
	{
		// Now call IdleFunc for all other windows.
		std::map<int, COGLWinFLTK*>::iterator itCur, itEnd;

		itCur = sm_mapThis.begin();
		itEnd = sm_mapThis.end();
		for (; itCur != itEnd; ++itCur)
		{
			if (itCur->second)
			{
				glutSetWindow(itCur->first);
				itCur->second->IdleFunc();

				if (itCur->second->ShouldDestroy())
				{
					itCur->second->DestroyWindow();
					break;
				}
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void COGLWinFLTK::GLUTMenu(int iIdx)
	{
		// Get this pointer for current window
		COGLWinFLTK* pThis = GetCurrentWindowPtr();

		if (pThis)
		{
			SetCurrentToFocusWindow();
			pThis->Menu(iIdx);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool COGLWinFLTK::BeginDraw()
	{
		try
		{
			CCLUDrawBase::BeginDraw();
			return true;
		}
		catch (Clu::CIException& xEx)
		{
			throw CLU_EXCEPTION_NEST(__FUNCTION__, std::move(xEx));
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool COGLWinFLTK::EndDraw()
	{
		CCLUDrawBase::EndDraw();
		return true;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void COGLWinFLTK::Run()
	{
		Fl::run();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool COGLWinFLTK::StartAnimation(unsigned int uiTime)
	{
		if (m_bIsAnimated)
		{
			return false;
		}

		m_dTimeStart = m_dTimePrev = GetTime();

		m_fTotalTime       = 0;
		m_fTimeStep        = 0;
		m_uAnimateTimeStep = uiTime;

		if (!AtAnimStart())
		{
			return false;
		}

		m_bIsAnimated = true;
		glutTimerFunc((unsigned int) m_iAnimPause, GLUTTimedFunc, m_iHWin);
		return true;
	}

//////////////////////////////////////////////////////////////////////
/// Stop Animation

	bool COGLWinFLTK::StopAnimation()
	{
		m_bIsAnimated = false;

		//AtAnimEnd();

		//m_pMenuBar->value(1);
		//((Fl_Menu_Item *) m_pMenuBar->mvalue())->uncheck();

		return true;
	}

//////////////////////////////////////////////////////////////////////
/// GLUT Timed Func

	void COGLWinFLTK::GLUTTimedFunc(int value)
	{
		// Get this pointer for the calling window
		COGLWinFLTK* pThis;
		std::map<int, COGLWinFLTK*>::iterator itCur;

		// If Window handle is not found in map then return.
		if ((itCur = sm_mapThis.find(value)) == sm_mapThis.end())
		{
			return;
		}

		pThis = itCur->second;

		if (pThis)
		{
			pThis->CalcTimeElapsed();

			if (pThis->TimedFunc() && pThis->m_bIsAnimated)
			{
				glutTimerFunc((unsigned int) pThis->m_iAnimPause /*m_uAnimateTimeStep*/, COGLWinFLTK::GLUTTimedFunc, value);
			}
			else
			{
				pThis->AtAnimEnd();	// Clean up animation after last timer call.
			}
		}
	}

//////////////////////////////////////////////////////////////////////
/// Get Modifiers
	void COGLWinFLTK::GetModifiers()
	{
		int iMod = glutGetModifiers();

		if (iMod & GLUT_ACTIVE_SHIFT) { m_bShiftDown = true; }
		else{ m_bShiftDown = false; }

		if (iMod & GLUT_ACTIVE_CTRL) { m_bCtrlDown = true; }
		else{ m_bCtrlDown = false; }

		if (iMod & GLUT_ACTIVE_ALT) { m_bAltDown = true; }
		else{ m_bAltDown = false; }
	}

//////////////////////////////////////////////////////////////////////
/// calculate elpased time since last idlefunc call

	void COGLWinFLTK::CalcTimeElapsed()
	{
		//TTimeStruct tmCur;
		//GETTIME(&tmCur);

		//m_fTotalTime = float(tmCur.time - m_tmStart.time)
		//	+ float(1e-3) * float(tmCur.millitm - m_tmStart.millitm);

		//m_fTimeStep = float(tmCur.time - m_tmPrev.time)
		//	+ float(1e-3) * float(tmCur.millitm - m_tmPrev.millitm);

		//memcpy(&m_tmPrev, &tmCur, sizeof(TTimeStruct));

		double dTimeCur = GetTime();

		m_fTotalTime = float(1e-3 * (dTimeCur - m_dTimeStart));
		m_fTimeStep  = float(1e-3 * (dTimeCur - m_dTimePrev));

		m_dTimePrev = dTimeCur;
	}

#endif	// CLUDRAW_GLUT
