////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLWinFLTK.h
//
// summary:   Declares the ogl window fltk class
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

// OGLWinFLTK.h: Schnittstelle f�r die Klasse COGLWinFLTK.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_OGLWINFLTK_H__INCLUDED_)
	#define _OGLWINFLTK_H__INCLUDED_

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000

#include "CluTec.Viz.Fltk\Fl_glut.H"
#include "CluTec.Viz.Fltk\Fl.H"
#include "CluTec.Viz.Fltk\Fl_Group.H"
#include "CluTec.Viz.Fltk\fl_ask.H"
#include "CluTec.Viz.Fltk\Fl_Window.H"
#include "CluTec.Viz.Fltk\Fl_Double_Window.H"
#include "CluTec.Viz.Fltk\Fl_Single_Window.H"
#include "CluTec.Viz.Fltk\Fl_Menu_Bar.H"
#include "CluTec.Viz.Fltk\Fl_File_Chooser.H"
#include "CluTec.Viz.Fltk\Fl_Tile.H"
#include "CluTec.Viz.Fltk\Fl_Box.H"
#include "CluTec.Viz.Fltk\Fl_Pack.H"
#include "CluTec.Viz.Fltk\Fl_Scroll.H"
#include "CluTec.Viz.Fltk\Fl_Value_Slider.H"
#include "CluTec.Viz.Fltk\Fl_Round_Button.H"
#include "CluTec.Viz.Fltk\Fl_Choice.H"
#include "CluTec.Viz.Fltk\Fl_Help_View.H"

#include "time.h"
#include "sys/timeb.h"

#include "CLUDrawBase.h"
#include "IOGLWinBase.h"
#include "FLTKToolBox.h"

#include <map>
#include <string>
#include <vector>

	#define CLU_WNDSTYLE_NONE                       0x0000
	#define CLU_WNDSTYLE_SIZEBORDER         0x0001
	#define CLU_WNDSTYLE_CAPTION            0x0002
	#define CLU_WNDSTYLE_TOPMOST            0x0004
	#define CLU_WNDSTYLE_SYSMENU            0x0008
	#define CLU_WNDSTYLE_TOOL                       0x0010
	#define CLU_WNDSTYLE_MULTISAMPLE_4      0x0100
	#define CLU_WNDSTYLE_MULTISAMPLE_8      0x0200
	#define CLU_WNDSTYLE_MULTISAMPLE_16     0x0300
	#define CLU_WNDSTYLE_MULTISAMPLE_MASK 0x0300

	#define CLU_WNDSTYLE_STD                        (CLU_WNDSTYLE_SIZEBORDER | CLU_WNDSTYLE_CAPTION | CLU_WNDSTYLE_SYSMENU)

//class CDevice_3DX;

	using namespace std;

#ifdef WIN32
		// Macro that is used by Windows and only confuses us here.
    #ifdef CreateWindow
	#undef CreateWindow
    #endif
		typedef struct _timeb TTimeStruct;
    #ifndef GETTIME
			#define GETTIME(S) _ftime_s(S)
    #endif
#else
		typedef struct timeb TTimeStruct;
		#define GETTIME(S) ftime(S)
#endif

/*
struct SGLUTWin
{
        char *pcName;
        int iPosX, iPosY;
        int iSizeX, iSizeY;
        unsigned int uMode;

        SGLUTWin()
        {
                pcName = "CLUDraw v1.1 by C.Perwass";
                iPosX = 100; iPosY = 100;
                iSizeX = 500; iSizeY = 500;
                uMode = GLUT_DOUBLE | GLUT_RGB;
        }
};
*/

	class CLUDRAW_API COGLWinFLTK : public CCLUDrawBase
	{
	public:

		enum EToolType
		{
			TOOL_NONE = 0,
			TOOL_SLIDER,
			TOOL_CHECKBOX
		};

		struct STool
		{
			EToolType eType;
			Fl_Widget* pWidget;
			Fl_Group* pBox;
			string sName;
			string sDesc;

			bool bInUse;
			bool bFixed;
		};

		typedef map<string, STool> TToolMap;

		class CKeyWindow : public Fl_Window
		{
		public:

			CKeyWindow(COGLWinFLTK* pWin, int iX, int iY, int iW, int iH, const char* pcTitle = 0)
				: Fl_Window(iX, iY, iW, iH, pcTitle)
			{
				m_pOGLWin = pWin;
			}

			virtual int handle(int iEvent)
			{
				//int iX, iY, iW, iH;
				Fl_Window* pWin = m_pOGLWin->GetGLUTChildWindow();
/*
                        iX = x() + pWin->x();
                        iY = y() + pWin->y();
                        iW = pWin->w();
                        iH = pWin->h();
*/

				if (m_pOGLWin &&
					//!Fl::event_alt() &&
					//!Fl::event_ctrl() &&
				    Fl::event_inside(pWin))
				{
					int iKey = Fl::event_key();

					if (iEvent == FL_KEYUP)
					{
						//Fl::event_inside(iX, iY, iW, iH);
						//int iKey = Fl::event_key();
						//if (iKey >= 32 && iKey <= 127 && !Fl::event_alt() && !Fl::event_ctrl() )
						//{
						//	m_pOGLWin->KeyUp((unsigned char) Fl::event_text()[0]);
						//	return 1;
						//}

						if (iKey == FL_Escape)
						{
							if (m_pOGLWin->EscapePressed())
							{
								return 1;
							}
						}

						if (m_pOGLWin->Keyboard(iKey, false))
						{
							return 1;
						}
					}
					else if (iEvent == FL_KEYDOWN)
					{
						//int iKey = Fl::event_key();
						//if (iKey >= 32 && iKey <= 127 && !Fl::event_alt() && !Fl::event_ctrl())
						//{
						//	m_pOGLWin->KeyDown((unsigned char) Fl::event_text()[0]);
						//	return 1;
						//}
						//else
						//{
						if (m_pOGLWin->Keyboard(iKey, true))
						{
							return 1;
						}
						//}
					}
					//else if (iEvent == FL_MOUSEWHEEL)
					//{
					//
					//}
					//else if ( iEvent == FL_FOCUS )
					//{
					//	m_pOGLWin->OnFocus();
					//}
					//else if ( iEvent == FL_UNFOCUS )
					//{
					//	m_pOGLWin->OnUnFocus();
					//}
/*				else if (iEvent == FL_KEYBOARD)
                                {
                                        if (m_pOGLWin->Keyboard( Fl::event_key() ))
                                                return 1;
                                }
*/                              }

				return Fl_Window::handle(iEvent);
			}

		protected:

			COGLWinFLTK* m_pOGLWin;
		};

		friend class CKeyWindow;

	public:

		COGLWinFLTK();
		virtual ~COGLWinFLTK();

		bool SaveFileDialog(const string& sTitle, const string& sPattern, string& sFile, string& sFilter);

		// Enter Main Loop
		void Run();
		void GetModifiers();

		bool IsCreated() { if (m_iHWin != 0) { return true; } return false; }

		void NotifyDestroy() { m_bNotifyDestroy = true; }
		bool ShouldDestroy() { return m_bNotifyDestroy; }

		//void ShowWindow(bool bVal = true);
		void HideMenu(bool bVal = true);

		virtual bool LockVis(int iWait) { return true; }
		virtual void UnlockVis() {         }

		virtual void PostRedisplay(bool bWait = false) { glutPostRedisplay(); }
		void glutPostRedisplay() { m_pGLUTWindow->redraw(); }
		Fl_Glut_Window* GetGLUTChildWindow() { return m_pGLUTWindow; }

		// Tool Functions
		void CheckToolBoxRedraw();
		void CheckToolBarRedraw();

		// Overloaded functions from CLUDrawBase
		virtual bool GetToolValue(const char* pcName, double& rVal);
		virtual bool GetToolValue(const char* pcName, string& rVal);
		virtual bool AddToolSlider(const char* pcName, double dMin, double dMax, double dStep, double dVal, bool bFixed = false, const char* pcDesc = 0);
		virtual bool AddToolCounter(const char* pcName, double dMin, double dMax, double dStep, double dLStep, double dVal, bool bFixed = false, const char* pcDesc = 0);
		virtual bool AddToolInput(const char* pcName, double dMin, double dMax, double dVal, bool bFixed = false, const char* pcDesc = 0);
		virtual bool AddToolInputText(const char* pcName,
				bool bImmediateCallback,
				const char* pcInitText,
				bool bFixed = false,
				const char* pcDesc = 0,
				bool bDoUpdate = true);
		virtual bool AddToolCheckBox(const char* pcName, bool bSet, bool bFixed = false, const char* pcDesc = 0);
		virtual bool AddToolButton(const char* pcName, bool bFixed = false, const char* pcDesc = 0);
		virtual bool AddToolImageButton(const char* pcName, COGLImage* pImgAct, COGLImage* pImgDeact = 0, bool bFixed = false, const char* pcDesc = 0);
		virtual bool AddToolChoice(const char* pcName, vector<string>& vecData, const char* pcCoice, bool bFixed = false, const char* pcDesc = 0, bool bDoUpdate = true);

		virtual bool AddToolbarImageButton(const char* pcName, COGLImage* pImgAct, COGLImage* pImgDeact = 0, bool bFixed = false, const char* pcDesc = 0);

		virtual bool AddToolbarCounter(const char* pcName,
				const char* pcUnit,
				double dMin,
				double dMax,
				double dStep,
				double dLStep,
				double dVal,
				bool bActive = true,
				bool bFixed = false,
				const char* pcDesc = 0);

		virtual bool AddToolbarLabel(const char* pcName, bool bFixed = false, const char* pcDesc = 0);

		virtual bool EnableMenu(bool bVal);
		virtual bool EnableStdCtrl(bool bVal);
		virtual bool SetMenu(vector<SMenuItem>& vecMenuItem);

		virtual bool SetMouseMode(int iMode);
		virtual bool SetOGLSize(double dWidth, double dHeight);
		virtual bool GetOGLSize(double& dWidth, double& dHeight);

		// Info Window Functions
		virtual bool SetInfoText(const char* pcText);
		virtual bool SetInfoWidth(double dWidth);
		virtual const char* InfoLink(Fl_Widget* pWidget, const char* pcURL);

		virtual bool Create(int* piArg = 0,
				char** pcArg = 0,
				char* pcName = "CLUDraw v1.1 by C. Perwass",
				int iPosX = 100,
				int iPosY = 100,
				int iSizeX = 500,
				int iSizeY = 500,
				unsigned int umode = FL_DOUBLE | FL_RGB | FL_DEPTH);

		/// Create a GLUT window without calling glutInit.
		/// This can be used if GLUT has already been
		/// initialized by this process.
		virtual bool CreateWindow(const char* pcName = "CLUDraw v1.1 by C. Perwass",
				int iPosX = 100,
				int iPosY = 100,
				int iSizeX = 500,
				int iSizeY = 500,
				void* vpIcon = 0,
				unsigned int umode = FL_DOUBLE | FL_RGB | FL_DEPTH,
				void* vpParentHandle = 0,
				unsigned int uWindowStyle = CLU_WNDSTYLE_STD);

		/// Detach this window from the GLUT loop.
		virtual bool DestroyWindow();

		virtual bool SetWindowPosSize(int iX, int iY, int iSizeX, int iSizeY);
		virtual bool ShowWindow();
		virtual bool HideWindow();
//	virtual bool Create(int *piArg, char **pcArg, MemObj<SGLUTWin>& mWin);

		virtual bool StartAnimation(unsigned int uiTime);	// idlefunc is called each fTime milliseconds
		virtual bool StopAnimation();

		// Post Main Window Redisplay to run through all idle loops
		//virtual void PostMainWindowUpdate();

		unsigned char CLU_LEFT_BUTTON() { return GLUT_LEFT_BUTTON; };
		unsigned char CLU_RIGHT_BUTTON() { return GLUT_RIGHT_BUTTON; };
		unsigned char CLU_DOWN() { return GLUT_DOWN; };
		unsigned char CLU_UP() { return GLUT_UP; };

		void EnableFullScreen(bool bVal = true);
		void EnableUserToggleFullScreen(bool bVal = true)
		{ m_bUserToggleFullScreen = bVal; }

		static void CB_Mouse3D_KeyDown(int iKeyCode);
		static void CB_Mouse3D_KeyUp(int iKeyCode);
		static void CB_Mouse3D_Move(double dTX, double dTY, double dTZ, double dLength, double dRX, double dRY, double dRZ, double dAngle);

	protected:

		virtual bool BeginDraw();
		virtual bool EndDraw();
		virtual void PostWindowUpdate();

		virtual bool EscapePressed();
		virtual void KeyUp(unsigned char cKey);
		virtual void KeyDown(unsigned char cKey);
		virtual bool Keyboard(int iKey, bool bKeyDown);

		virtual void Key(unsigned char cKey, int iX, int iY);
		virtual void Reshape(int iWidth, int iHeight);

		virtual void OnFocus();
		virtual void OnUnFocus();

		virtual void InitMenu();
		virtual void MenuAnimate(Fl_Widget* pWidget);
		virtual void MenuMouseAnim(Fl_Widget* pWidget);
		virtual void MenuMouseMode(Fl_Widget* pWidget);
		virtual void MenuSaveImage(Fl_Widget* pWidget);
		virtual void MenuQuickSaveImage(Fl_Widget* pWidget);
		virtual void MenuFullScreen(Fl_Widget* pWidget);
		virtual void MenuLocalRot(Fl_Widget* pWidget);
		virtual void MenuViewMode(Fl_Widget* pWidget);
		virtual void MenuResetView(Fl_Widget* pWidget);
		virtual void MenuResetMouseModes(Fl_Widget* pWidget);

		virtual void ToolChanged(string sName);

		virtual void Quit();

		virtual void Mouse3D_KeyDown(int iKeyCode);

		static COGLWinFLTK* COGLWinFLTK::GetCurrentWindowPtr();
		static COGLWinFLTK* COGLWinFLTK::GetFocusWindowPtr();
		static void SetCurrentToFocusWindow();

		static void GLUTDisplay();
		static void GLUTReshape(int iWidth, int iHeight);
		static void GLUTMouse(int iButton, int iState, int iX, int iY);
		static void GLUTKey(unsigned char cKey, int iX, int iY);
		static void GLUTActiveMouseMove(int iX, int iY);
		static void GLUTPassiveMouseMove(int iX, int iY);
		static void GLUTIdleFunc();
		static void GLUTTimedFunc(int value);
		static void GLUTMenu(int iIdx);

		static void CB_Animate(Fl_Widget* pWidget, void* pData);
		static void CB_MouseAnim(Fl_Widget* pWidget, void* pData);
		static void CB_MouseMode(Fl_Widget* pWidget, void* pData);
		static void CB_SaveImage(Fl_Widget* pWidget, void* pData);
		static void CB_QuickSaveImage(Fl_Widget* pWidget, void* pData);
		static void CB_FullScreen(Fl_Widget* pWidget, void* pData);
		static void CB_LocalRot(Fl_Widget* pWidget, void* pData);
		static void CB_ViewMode(Fl_Widget* pWidget, void* pData);
		static void CB_ResetView(Fl_Widget* pWidget, void* pData);
		static void CB_ResetMouseModes(Fl_Widget* pWidget, void* pData);

		static void CB_ToolChanged(Fl_Widget* pWidget, void* pData);
		static void CB_MenuItemSelected(Fl_Widget* pWidget);
		static const char* CB_InfoLink(Fl_Widget* pWidget, const char* pcURL, void* pvData);
		static void CB_Quit(Fl_Widget* pWidget, void* pData);

		void CalcTimeElapsed();
		//double GetTime();

	protected:

		int m_iHWin;
		//TTimeStruct m_tmStart, m_tmPrev;

		CKeyWindow* m_pWindow;
		Fl_Tile* m_pTile, * m_pTileToolBar;
		Fl_Box* m_pDummyBox;

		Fl_Glut_Window* m_pGLUTWindow;
		Fl_Menu_Bar* m_pMenuBar;
		Fl_Choice* m_pMouseModeChoice;
		Fl_Box* m_pMenuBarSep;
		vector<Fl_Menu_Item> m_vecMenuItem;
		vector< CCLUDrawBase::SMenuItem > m_vecCLUMenuItem;
		vector< string > m_vecMenuItemID;
		bool m_bUserMenuActive;

		// Update Tools in idle loop
		bool m_bDoUpdateTools;
		bool m_bDoRedrawTools;

		// Store the initial x,y pos. of the tool box.
		// This is needed by m_pTile to address the correct child.
		int m_iToolBoxInitX, m_iToolBoxInitY;
		CFLTKToolBox* m_pToolBox, * m_pToolBar;

		int m_iInfoWinInitX, m_iInfoWinInitY;
		int m_iMinInfoWidth;
		int m_iInfoLeftMargin;
		Fl_Box* m_pInfoBox;
		Fl_Help_View* m_pInfoWin;

		bool m_bNotifyDestroy;
		int m_iPosX, m_iPosY;

		Fl_Menu_Item* m_pFullScreenMenuItem;
		bool m_bInFullScreenMode;
		bool m_bUserToggleFullScreen;
		bool m_bMenuShown, m_bStdCtrlShown;

		long m_lSaveStyleWnd, m_lSaveExStyleWnd;
		int m_iSaveX, m_iSaveY, m_iSaveW, m_iSaveH, m_iSaveMMCWidth;
		int m_iMinToolHeight, m_iToolHeight;
		int m_iMenuHeight, m_iMenuBarHeight, m_iMenuBarSep, m_iMMChoiceWidth;
		int m_iToolBarInitX, m_iToolBarInitY, m_iToolBarInitWidth, m_iToolBarInitHeight;
		int m_iMinToolBarHeight, m_iSmallToolHeight;
		int m_iSmallToolBorderW, m_iSmallToolBorderH;
		int m_iOGLWinInitX, m_iOGLWinInitY, m_iOGLWinInitWidth, m_iOGLWinInitHeight;

		int m_iMultisampleTarget;

		int m_iImgID;

		// Main Window Handle
		//static int m_iHMainWin;

		//CDevice_3DX *m_pDevice3DX;

		// map from window handle to this pointer.
		static map<int, COGLWinFLTK*> sm_mapThis;

		// ID of window with focus
		static int sm_iFocusWin;
	};

#endif	// !defined(AFX_OGLWINGLUT_H__6EB9C282_EE5B_11D5_BA34_00E07D8AFD6A__INCLUDED_)
