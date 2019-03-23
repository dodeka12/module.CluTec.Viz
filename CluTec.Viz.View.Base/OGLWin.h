////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.View.Base
// file:      OGLWin.h
//
// summary:   Declares the ogl Windows Form
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

//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLWIN_H__948387CF_09B2_4CBB_B6D2_2A1473F061B2__INCLUDED_)
	#define AFX_OGLWIN_H__948387CF_09B2_4CBB_B6D2_2A1473F061B2__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif	// _MSC_VER > 1000

	#define ANIMROT_NONE    0
	#define ANIMROT_X               1
	#define ANIMROT_Y               2
	#define ANIMROT_Z               3
	#define ANIMROT_MOUSE   4

#include "CLUVizTypes.h"

#include "CluTec.Viz.Fltk\Fl.H"
#include "CluTec.Viz.Fltk\Fl_Group.H"
#include "CluTec.Viz.Fltk\Fl_Double_Window.H"
#include "CluTec.Viz.Fltk\fl_ask.H"
#include "CluTec.Viz.Fltk\Fl_File_Chooser.H"
#include "CluTec.Viz.Fltk\Fl_Menu_Bar.H"
#include "CluTec.Viz.Fltk\Fl_Input.H"
#include "CluTec.Viz.Fltk\Fl_Button.H"
#include "CluTec.Viz.Fltk\Fl_Return_Button.H"
#include "CluTec.Viz.Fltk\Fl_Text_Buffer.H"
#include "CluTec.Viz.Fltk\Fl_Text_Display.H"
#include "CluTec.Viz.Fltk\Fl_Tile.H"
#include "CluTec.Viz.Fltk\fl_ask.h"
#include "CluTec.Viz.Fltk\Fl_Browser.h"
#include "CluTec.Viz.Fltk\Fl_Hold_Browser.h"

#include "CluTec.Viz.Draw\OGLWinFLTK.h"
#include "CluTec.Viz.Draw\OGLScene.h"
#include "CluTec.Base\Exception.h"

#include <string>
#include <vector>

#ifdef WIN32
		typedef struct _timeb TTimeStruct;
    #ifndef GETTIME
			#define GETTIME(S) _ftime_s(S)
    #endif
    #include "windows.h"
    #include "process.h"
#else
		typedef struct timeb TTimeStruct;
		#define GETTIME(S) ftime(S)
#endif

/////////////////////////////////////////////////////
/// Messages

	#define OGLWIN_MSG_NONE                 0x0000
	#define OGLWIN_MSG_QUIT                 0xFFFF
	#define OGLWIN_MSG_SETSCRIPT    0x0001
	#define OGLWIN_MSG_PARSE                0x0002
	#define OGLWIN_MSG_EXEC                 0x0003

/////////////////////////////////////////////////////
// CLU Key IDs. Identical to FL key IDs

	#define CLU_KEY_BackSpace               0xff08
	#define CLU_KEY_Tab                             0xff09
	#define CLU_KEY_Enter                   0xff0d
	#define CLU_KEY_Pause                   0xff13
	#define CLU_KEY_Scroll_Lock             0xff14
	#define CLU_KEY_Escape                  0xff1b
	#define CLU_KEY_Home                    0xff50
	#define CLU_KEY_Left                    0xff51
	#define CLU_KEY_Up                              0xff52
	#define CLU_KEY_Right                   0xff53
	#define CLU_KEY_Down                    0xff54
	#define CLU_KEY_Page_Up                 0xff55
	#define CLU_KEY_Page_Down               0xff56
	#define CLU_KEY_End                             0xff57
	#define CLU_KEY_Print                   0xff61
	#define CLU_KEY_Insert                  0xff63
	#define CLU_KEY_Menu                    0xff67	// the "menu/apps" key on XFree86
	#define CLU_KEY_Help                    0xff68	// the 'help' key on Mac keyboards
	#define CLU_KEY_Num_Lock                0xff7f
	#define CLU_KEY_KP                              0xff80	// use CLU_KEY_KP+'x' for 'x' on numeric keypad
	#define CLU_KEY_KP_Enter                0xff8d	// same as Fl_KP+'\r'
	#define CLU_KEY_KP_Last                 0xffbd	// use to range-check keypad
	#define CLU_KEY_F                               0xffbd	// use CLU_KEY_F+n for function key n
	#define CLU_KEY_F_Last                  0xffe0	// use to range-check function keys
	#define CLU_KEY_Shift_L                 0xffe1
	#define CLU_KEY_Shift_R                 0xffe2
	#define CLU_KEY_Control_L               0xffe3
	#define CLU_KEY_Control_R               0xffe4
	#define CLU_KEY_Caps_Lock               0xffe5
	#define CLU_KEY_Meta_L                  0xffe7	// the left MSWindows key on XFree86
	#define CLU_KEY_Meta_R                  0xffe8	// the right MSWindows key on XFree86
	#define CLU_KEY_Alt_L                   0xffe9
	#define CLU_KEY_Alt_R                   0xffea
	#define CLU_KEY_Delete                  0xffff

/////////////////////////////////////////////////////

	using namespace std;

	class CCLUScriptEditor;
	class CCLUOutput;

	class COGLWin : public COGLWinFLTK
	{
	public:

		enum EPickSceneType
		{
			PST_None = 0,
			PST_Pick,
			PST_Drag,
			PST_NotifyDrag,
			PST_NotifyClick,
			PST_Tool,
		};

	protected:

		class CScriptBrowser : public Fl_Hold_Browser
		{
		public:

			CScriptBrowser(int iX, int iY, int iWidth, int iHeight, const char* pcText = 0)
				: Fl_Hold_Browser(iX, iY, iWidth, iHeight, pcText)
			{
			}

			virtual int handle(int iEvent)
			{
				if ((iEvent == FL_KEYUP) &&
				    (Fl::event_key() == FL_Enter) &&
				    !Fl::event_state(FL_CTRL))
				{
					do_callback();
					return 1;
				}
				return Fl_Hold_Browser::handle(iEvent);
			}
		};

		struct SPickData
		{
			float fMinDepth;
			std::vector<uint> vecID;
			uint uPartID;
		};

	public:

		COGLWin();
		virtual ~COGLWin();

		virtual bool Init(bool bFirst);
		bool Init(bool bFirst, bool bCreateSharedRC = false, HGLRC* phGlRc = 0);
		bool CLUDrawBaseInit(bool bFirst);
		void InitMenu();
		bool SetTitle();

		bool EmbedWindow(HWND hWnd, bool bCreateSRC = false, HGLRC* phGlRc = 0);
		bool ReleaseWindow();

		// Sets a mouse event callback function
		bool SetMouseEventHandler(Clu::Viz::View::TFuncMouseEventCallback pFunc, void* pvContext);
		// Removes the mouse event callback function
		bool RemoveMouseEventHandler();

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Sets the error event handler.
		/// </summary>
		///
		/// <param name="pFunc">	 If non-null, the callback function. </param>
		/// <param name="pvContext"> [in,out] If non-null, a context pointer. </param>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void SetErrorEventHandler(Clu::Viz::View::TFuncErrorEventCallback pFunc, void* pvContext);
		void RemoveErrorEventHandler();

		// Functions to update OGLWin when window GL Window is embedded in has changed
		void EW_Display(bool bWait);
		void EW_Reshape(int iWidth, int iHeight);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Gets the width and the height of the view port of this view.
		/// </summary>
		///
		/// <param name="iWidth">  [out] The width of the view port. </param>
		/// <param name="iHeight"> [out] The height of the view port. </param>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void EW_GetShape(int& iWidth, int& iHeight);

		// Called when a mouse button is pressed/released
		// iButton = 0,1,2 for left, middle, right button
		// iState = 0,1 for released/pressed
		void EW_Mouse(int iButton, int iState, int iX, int iY);
		void EW_Key(unsigned char cKey, int iX, int iY);
		void EW_ActiveMouseMove(int iX, int iY);
		void EW_PassiveMouseMove(int iX, int iY);
		void EW_IdleFunc();
		void EW_SetKeyModifierStates(bool bShift, bool bCtrl, bool bAlt, bool bNumLock, bool bCapsLock, bool bScrollLock);
		bool EW_LoadScript(const char* pcFilename, const char* pcPass);
		bool EW_SetScript(const char* pcScript, bool bResetEnv = false, bool bReparseLatex = false);
		bool EW_ExecUser(const char* pcToolName = 0);
		bool EW_LockVis(bool bLock);

		// Create an OpenGL rendering context that shares its textures and display lists with
		// the rendering context running in the separate thread.
		bool CreateSharedRC();
		bool DestroySharedRC();
		bool EW_MakeCurrent(bool bBind);

		bool GetScriptOutput(std::vector<std::string>& vecText, bool& bIsError);
		bool SaveView(const char* pcFilename);

		HWND GetGLWindowHandle();

		void EnableVisualization(bool bVal);
		void EnableDrawSceneGraph(bool bVal, bool bDoRedisplay);
		void EnableBorder(bool bVal);

		static void CB_NextScript(Fl_Widget* pWidget, void* pData);
		static void CB_PrevScript(Fl_Widget* pWidget, void* pData);
		static void CB_ReloadScript(Fl_Widget* pWidget, void* pData);
		static void CB_ClearScriptList(Fl_Widget* pWidget, void* pData);
		static void CB_ShowScriptList(Fl_Widget* pWidget, void* pData);
		static void CB_ScriptListWindow(Fl_Widget* pWidget, void* pData);

		void MenuNextScript(Fl_Widget* pWidget);
		void MenuPrevScript(Fl_Widget* pWidget);
		void MenuReloadScript(Fl_Widget* pWidget);
		void MenuClearScriptList(Fl_Widget* pWidget);
		void MenuShowScriptList(Fl_Widget* pWidget);
		void ScriptListWindowEvent(Fl_Widget* pWidget);
		void InitScriptListWindow();

		virtual void MenuQuickSaveImage(Fl_Widget* pWidget);
		virtual void MenuSaveImage(Fl_Widget* pWidget);

		void MenuAnimate(Fl_Widget* pWidget);

		static void CB_ShowEditorWin(Fl_Widget* pWidget, void* pvData);
		static void CB_ShowOutputWin(Fl_Widget* pWidget, void* pvData);
		static void CB_ShowVisWin(Fl_Widget* pWidget, void* pvData);

		void MenuShowEditorWin();
		void MenuShowOutputWin();
		void MenuShowVisWin();

		static void CB_Parse(Fl_Widget* pWidget, void* pvData);
		static void CB_ParseForce(Fl_Widget* pWidget, void* pvData);

		void MenuParse();
		void MenuParseForce();

		virtual bool SetOGLSize(double dWidth, double dHeight);

		void SetEditor(CCLUScriptEditor* pEditor)
		{ m_pEditor = pEditor; }

		void SetOutputWin(CCLUOutput* pOutput)
		{ m_pOutput = pOutput; }

		void SetScriptFilename(const char* pcFileName) { m_csScriptFilename = pcFileName; }
		void SetScriptPath(const char* pcScriptPath)
		{
			LockVis();
			m_sScriptPath = pcScriptPath;
			m_xParse.SetScriptPath(pcScriptPath);
			UnlockVis();
		}

		void SetScriptName(const char* pcScriptName)
		{
			LockVis();
			m_sScriptName = pcScriptName;
			m_xParse.SetScriptName(pcScriptName);
			UnlockVis();
		}

		string GetScriptName() { return m_sScriptName; }
		string GetScriptPath() { return m_sScriptPath; }

		int SetScript(const char* pcScript, bool bResetEnv = false, bool bReparseLatex = false, bool bIsBinary = false, int iArgCnt = 0, const char* pcArg = 0);

		bool LoadScript(const char* pcFilename, bool bResetEnv = true, const char* pcPass = 0);
		int LoadScript();
		int CheckScriptChanged();

		void SetAnimRotType(int iRotType) { m_iAnimRotType = iRotType; }
		void NotifyCodeChanged() { m_bCodeChanged = true; }

		bool CanAnimateScript()
		{
			LockVis();

			CCodeVar Var;
			TCVScalar iVal;
			bool bResult = (m_xParse.GetVar("_DoAnimate", Var) && Var.CastToScalar(iVal) && (iVal != 0));

			UnlockVis();
			return bResult;
		}

		void SetToolNameVar(const char* pcName);
		void SetLinkDataVar(const char* pcData);
		void SetExecModeVar(int iMode);
		void AddExecModeVar(int iMode);
		void RemExecModeVar(int iMode);

		CCLUParse m_xParse;

		bool HasError() { return m_bHasError || m_sError.size() > 0; }
		bool HasOutput() { return m_bHasOutput; }
		bool HasMessage() { return m_bHasMessage; }

		string GetError() { return m_sError; }
		string GetOutput() { return m_sOutput; }
		string GetMessage() { return m_sMessage; }

		void ClearError() { m_sError = ""; m_bHasError = false; }
		void ClearOutput() { LockVis(); m_xParse.ResetOutputObjectList(); m_bHasOutput = false; UnlockVis(); }
		void ClearMessage() { m_sMessage = ""; m_bHasMessage = false; }

		const char* InfoLink(Fl_Widget* pWidget, const char* pcURL);

		void EnableWindowMenu(bool bVal = true) { m_bShowWindowMenu = bVal; }
		void EnableVizOnlyMode(bool bVal = true) { m_bVizOnlyMode = bVal; }

		void SetVersion(int iMajor, int iMinor, int iRevision)
		{ LockVis(); m_xParse.SetVersion(iMajor, iMinor, iRevision); m_xParse.DefineVersion(); UnlockVis(); }
		void GetVersion(int& iMajor, int& iMinor, int& iRevision) { LockVis(); m_xParse.GetVersion(iMajor, iMinor, iRevision); UnlockVis(); }

		void SetLicensee(const char* pcLicensee)
		{ m_sLicensee = pcLicensee; }

		bool SetAnimationTimeStep(int iTimeStep);

		void SetEnvPath(const string& sEnvPath)
		{ m_sEnvPath = sEnvPath; }

		virtual void Quit();
		void DoQuit();

		void SetIsValidPtr(bool* pVal)
		{ m_pbExtern_IsValid = pVal; }

		void SetFixedTitle(const char* pcTitle)
		{ if (pcTitle) { m_sFixedTitle = pcTitle; } }

		void ExecUser(const char* pcToolName = 0);

		bool GetFuncNameList(vector<string>& vecFuncName)
		{ LockVis(); m_xParse.GetFuncNameList(vecFuncName); UnlockVis(); return true; }

		virtual void PostRedisplay(bool bWait = false, bool bForce = false, bool bNow = false);
		virtual bool LockVis(int iWait = 5000);
		virtual void UnlockVis();

		bool Keyboard(int iKey, bool bKeyDown);

		string GetSceneGraphPrint();

	protected:

		string GetSceneGrapgSubtreePrint(COGLBaseElement* pBaseElement, int iLevel);

		void _GetScriptVisualizationSettings();

		// Visualization loop thread
		static void RunVisLoop(void* pData);
		// Visualization timer loop thread
		static void RunVisTimerLoop(void* pData);

		static void CallDisplayOutput();
		static void CallIdleFunc();
		static void CallReshapeOutput(int iWidth, int iHeight);

		void KeyUp(unsigned char cKey);
		void KeyDown(unsigned char cKey);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Perform Model rotation on Key event
		/// </summary>
		///
		/// <param name="iKey"> Zero-based index of the key. </param>
		///
		/// <returns> True if it succeeds, false if it fails. </returns>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		bool KeyRotTrans(int iKey);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Perform Camera transformations on Key events.
		/// </summary>
		///
		/// <param name="iKey"> Zero-based index of the key. </param>
		///
		/// <returns> True if it succeeds, false if it fails. </returns>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		bool KeyViewRotateMode(int iKey);

		void Key(unsigned char cKey, int iX, int iY);
		void ReshapeOutput(int iWidth, int iHeight);
		void DisplayOutput();
		void IdleFunc();

		void TranslateMouseButtonState(Clu::Viz::View::EMouseEventButton& eMouseEventButton,
				Clu::Viz::View::EMouseEventType& eMouseEventType,
				int& iDragIdx,
				int iButton,
				int iState);
		void TranslateMouseButtonEvent(Clu::Viz::View::EMouseEventType& eMouseEventType, Clu::Viz::View::EMouseEventType eRawMouseEventType);
		bool HasPickableScene(const MemObj<COGLBEReference>& mSceneRef);
		COGLScene* SelectMouseEventScene(EPickSceneType& ePickSceneType, Clu::Viz::View::EMouseEventType eMouseEventType, int iDragIdx,
				const MemObj<COGLBEReference>& mSceneRef);

		virtual bool GetOGLSize(double& dWidth, double& dHeight);

		virtual void GetModifiers();

		//virtual bool DestroyWindow();
		virtual void Reshape(int iWidth, int iHeight);

		// Mouse click
		virtual void Mouse(int iButton, int iState, int iX, int iY);

		// Called to draw for picking
		virtual bool PickDraw();
		// Process the picking data
		virtual bool PickProcessHits(EMousePickType ePickType, uint uPickColor, float fPickMinDepth);

		// Check for mouse dragging while object is picked
		virtual void ActiveMouseMove(int iX, int iY);

		// Check for mouse moving in OGL Window
		virtual void PassiveMouseMove(int iX, int iY);

		// Mouse 3D Move
		virtual void Mouse3D_Move(double dTX, double dTY, double dTZ, double dLength, double dRX, double dRY, double dRZ, double dAngle);

		/// Get scene element lowest in the current pick hierarchy
		COGLBEReference GetSceneElement(Mem<uint>& mUID);

		/// Get list of scene elements in the current pick hierarchy
		MemObj<COGLBEReference> GetSceneElementList(const std::vector<uint>& vecUID);

		/// Create a string of names of scenes separated by ';'
		TString CreateSceneNameString(MemObj<COGLBEReference>& mSceneList, bool bListOnlyPickableScenes);

		/// Add TVarList of names of scenes to given varlist
		TString AddSceneNameVarList(TVarList& rList, MemObj<COGLBEReference>& mSceneList);

		/// Add PartName from picking to var list.
		bool AddScenePartName(TVarList& rList, unsigned uPartName);

		/// Add TVarList of drag data to given varlist
		bool AddSceneDragDataList(TVarList& rList, int iDragIdx, float fX, float fY, float fZ,                           float fDX, float fDY, float fDZ);

		/// Add TVarList of mouse drag positions to given varlist
		bool AddSceneDragMouseList(TVarList& rList, double dX, double dY, double dZ,            double dDX, double dDY, double dDZ);

		/// Add TVarList of mouse drag positions after auto transform to given varlist
		bool AddSceneDragMouseLocalList(TVarList& rList, double dX, double dY, double dZ,       double dDX, double dDY, double dDZ);

		/// Add TVarList of drag event to given varlist
		bool AddSceneDragEventList(TVarList& rList, const char* pcEvent);

		virtual bool TimedFunc();
		virtual bool AtAnimStart();
		virtual bool AtAnimEnd();

		virtual bool EnableMenu(bool bVal);
		virtual bool EnableStdCtrl(bool bVal);
		virtual bool SetMenu(vector<SMenuItem>& vecMenuItem);

		// Flush everything that has currently been drawn to the screen
		virtual bool FlushDraw();

		void DoDisplay();
		void Display();
		void DisplaySceneGraph();
		void Draw();
		void CheckToolBoxSize();
		void CheckToolBarSize();
		void CleanFrameStack();

		void Sleep(unsigned int uMilliSec);

		void ToolChanged(string sName);
		virtual bool SetInfoWidth(double dWidth);

		bool CreateMainRC();
		bool InitPixelFormat();
		bool InitGLExt();
		bool PrunePixelFormatList(std::vector<int>& vecPrune, const std::vector<int>& vecList, int iAttribute, int iExpValue);

	protected:

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Creates error output.
		/// </summary>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void _CreateErrorOutput();

	protected:

		volatile bool m_bRunVisLoop;
		volatile bool m_bRunVisTimerLoop;
		HANDLE m_hMutexVis;
		HANDLE m_hMutexVisRun;
		HANDLE m_hEventVis;
		HANDLE m_hEventVisTimer;
		HANDLE m_hEventVisNow;
		HANDLE m_hEventForceVis;
		HDC m_hGLDC;
		HGLRC m_hGLRC;

		bool m_bIsEmbedded;
		HWND m_hGLWnd;
		int m_iPixelFormat;
		bool m_bHasSharedGLWnd;
		HWND m_hShareGLWnd;
		HDC m_hShareGLDC;
		HGLRC m_hShareGLRC;
		HGLRC m_hShareExtGLRC;

		bool m_bInitFirst;
		bool m_bExecCode;
		bool m_bOutputChanged;
		bool m_bScriptLoaded;
		bool m_bCheckForAnimate;
		bool m_bBGColChanged;
		bool m_bFrameTransChanged;
		bool m_bCodeChangedForAnim;
		bool m_bKeepBGColor;
		bool m_bPresentationAction;
		bool m_bParseFromOGLWin;
		bool m_bWaitWithIdleForNextDisplay;
		bool m_bCheckToolBoxSize;
		bool m_bScriptChangedOGLSize;
		bool m_bEnableMenuChanged;
		bool m_bEnableStdCtrlChanged;
		bool m_bMenuChanged;
		bool m_bNewMenuState;
		bool m_bNewStdCtrlState;

		// flag to determine if we are executing a mouse event
		bool m_bMouseCallbackActive;

		bool m_bShowWindowMenu;
		bool m_bVizOnlyMode;

		bool m_bProcessPickHits;
		bool m_bPickBeforeDrawOnce;
		bool m_bPickAfterDrawOnce;
		bool m_bProcessStartDrag;
		bool m_bMouseClickEventNeedsProcessingByTimedFunc;
		bool m_bDoDisplayInIdle;

		volatile bool m_bDoSaveScreen;
		CStrMem m_sSaveScreenFilename;

		volatile bool m_bVisActive;
		volatile bool m_bDoDisplaySceneGraph;
		volatile bool m_bCodeChanged;
		volatile bool m_bAnimCode;
		volatile bool m_bDoAnimCodeStep;
		volatile bool m_bDoRedisplay;
		volatile bool m_bDoPicking;
		volatile bool m_bDoReshape;
		volatile bool m_bDoAnimDisplay;
		volatile bool m_bResetAnimTime;

		volatile bool m_bResetTools;
		volatile bool m_bPruneTools;

		volatile bool m_bShowErrorInEditor;

		volatile bool m_bMsgProcessed;

		double m_dNewOGLWidth, m_dNewOGLHeight;

		TCVScalar m_dFrameBoxSize;
		TCVScalar m_dViewAngle;

		int m_iExecMode;
		int m_iHWinOutput;

		int m_iReshapeW, m_iReshapeH;
		int m_iPickX, m_iPickY;
		int m_iMousePosX, m_iMousePosY;
		int m_iMouseStartX, m_iMouseStartY;

		int m_iOutSizeX;
		int m_iOutSizeY;
		int m_iOutLineHeight;
		int m_iCurToolCount;
		int m_iCurToolbarCount;
		double m_dNewInfoWidth;

		void* m_pOutputFont;

		// Script handling
		CStrMem m_csScriptFilename;

		bool m_bHasError;
		bool m_bHasOutput;
		bool m_bHasMessage;
		bool m_bInDisplay;
		bool m_bAnimPauseChanged;
		bool m_bHasPick;

		/// <summary> True if 2D view variable has been changed since last execution. </summary>
		bool m_b2DViewVariableChanged;

		int m_iAllowKeyUp;

		vector<SMenuItem> m_vecNewMenuItem;

		__int64 m_liTimeClick;

		std::string m_sError;
		std::string m_sOutput;
		std::string m_sMessage;
		std::string m_sScriptPath;
		std::string m_sScriptName;
		std::string m_sTitle;
		std::string m_sEnvPath;
		std::string m_sFixedTitle;
		std::string m_sLicensee;
		std::string m_sOutputTitle;

		TString m_sPickedSceneName;

		MemObj<SPickData> m_mCurPick;
		Mem<uint> m_mCurSelBuf;
		uint m_uCurSelPartId;

		MultiV<float> m_vRot;

		COGLDisplayList m_dlSpace;

		CCLUCodeBase::TOutObjList m_vecOutObj;

		double m_pdMouseDragStart[3];
		double m_pdMouseDragLocalStart[3];
		float m_pfDragDataStart[3];

		E3GA<float> m_E3Base;
		E3GA<double> m_E3BaseDouble;
		PGA<float> m_P3Base;
		ConfGA<float> m_N3Base;

		// MV Drawing Filter
		COGLMVFilter m_xFilter;

		// Drawing Functions
		COGLDrawBase m_DrawBase;

		// Base Element Repository for Scenes
		COGLBERepository m_SceneRepository;

		// Main Scene
		COGLBaseElementList* m_pMainScene;
		COGLBEReference m_MainSceneRef;

		// Mouse Pointer Scene
		COGLBaseElementList* m_pMouseScene;
		COGLBEReference m_MouseSceneRef;

		// Standard Mouse Pointer
		COGLBaseElementList* m_pStdMousePtr;
		COGLBEReference m_StdMousePtrRef;

		Fl_Window* m_pBrowserWin;
		CScriptBrowser* m_pBrowser;

		CCLUScriptEditor* m_pEditor;
		CCLUOutput* m_pOutput;

		bool* m_pbExtern_IsValid;

		/// <summary> The mouse event callback function. </summary>
		Clu::Viz::View::TFuncMouseEventCallback m_pMouseEventCallbackFunc;
		/// <summary> The mouse event callback context. </summary>
		void* m_pMouseEventCallbackContext;

		/// <summary> The error event callback function. </summary>
		Clu::Viz::View::TFuncErrorEventCallback m_pErrorEventCallbackFunc;
		/// <summary> The error event callback context. </summary>
		void* m_pErrorEventCallbackContext;
		/// <summary> True to throw an exception on script errors. </summary>
		bool m_bThrowExceptionOnScriptError;

		Clu::Viz::View::SMouseEventData m_xMouseEventData;
		bool m_bWaitForMouseEventExecScript;

		static COGLWin* m_pThisWin;
	};

#include "CLUScriptEditor.h"

#endif	// !defined(AFX_OGLWIN_H__948387CF_09B2_4CBB_B6D2_2A1473F061B2__INCLUDED_)
