////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.View.Base
// file:      OGLWin.cpp
//
// summary:   Implements the ogl Windows Form
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

// OGLWin.cpp: Implementierung der Klasse COGLWin.
//
//////////////////////////////////////////////////////////////////////

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

#define BUFSIZE 1024

COGLWin* COGLWin::m_pThisWin = 0;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLWin::COGLWin()
{
	m_bIsEmbedded = false;
	m_hGLWnd      = 0;

	m_iPixelFormat    = 0;
	m_bHasSharedGLWnd = false;
	m_hShareGLWnd     = 0;
	m_hShareGLDC      = 0;
	m_hShareGLRC      = 0;
	m_hShareExtGLRC   = 0;

	m_bInitFirst  = false;
	m_bRunVisLoop = false;

	m_bHasError   = false;
	m_bHasOutput  = false;
	m_bHasMessage = false;
	m_bInDisplay  = false;

	m_bThrowExceptionOnScriptError = false;

	m_bAnimCode        = false;
	m_bCodeChanged     = true;
	m_bOutputChanged   = false;
	m_bScriptLoaded    = false;
	m_bCheckForAnimate = false;

	m_bResetTools = false;
	m_bPruneTools = false;

	m_bShowErrorInEditor = false;

	m_bBGColChanged               = false;
	m_bFrameTransChanged          = false;
	m_bCodeChangedForAnim         = false;
	m_bKeepBGColor                = false;
	m_bPresentationAction         = false;
	m_bParseFromOGLWin            = false;
	m_bWaitWithIdleForNextDisplay = false;
	m_bCheckToolBoxSize           = false;
	m_bScriptChangedOGLSize       = false;
	m_bAnimPauseChanged           = false;
	m_bHasPick                    = false;

	m_bShowWindowMenu = true;
	m_bVizOnlyMode    = false;

	m_bProcessPickHits                           = true;
	m_bPickBeforeDrawOnce                        = false;
	m_bPickAfterDrawOnce                         = false;
	m_bProcessStartDrag                          = false;
	m_bMouseClickEventNeedsProcessingByTimedFunc = false;
	m_bMouseCallbackActive                       = false;

	m_bDoDisplayInIdle     = false;
	m_bDoRedisplay         = false;
	m_bDoPicking           = false;
	m_bDoReshape           = false;
	m_bDoAnimDisplay       = false;
	m_bResetAnimTime       = false;
	m_bVisActive           = true;
	m_bDoDisplaySceneGraph = true;

	m_bEnableMenuChanged    = false;
	m_bEnableStdCtrlChanged = false;
	m_bMenuChanged          = false;
	m_bNewMenuState         = true;
	m_bNewStdCtrlState      = true;

	m_b2DViewVariableChanged = false;

	m_bDoSaveScreen = false;

	m_pbExtern_IsValid = 0;

	m_iAllowKeyUp = 0;

	m_dFrameBoxSize = 3.0;
	m_dViewAngle    = 60.0;

	m_vRot = m_E3Base.vZero;

	m_pEditor     = 0;
	m_pOutput     = 0;
	m_pBrowserWin = 0;
	m_pBrowser    = 0;
	m_pThisWin    = this;

	m_iReshapeW = 0;
	m_iReshapeH = 0;

	m_iMousePosX   = 0;
	m_iMousePosY   = 0;
	m_iMouseStartX = 0;
	m_iMouseStartY = 0;

	m_iExecMode        = 0;
	m_iCurToolCount    = 0;
	m_iCurToolbarCount = 0;
	m_dNewInfoWidth    = -1.0;

	m_dNewOGLWidth  = 0;
	m_dNewOGLHeight = 0;

	m_pMainScene = new COGLBaseElementList;
	m_pMainScene->SetName("MainScene");
	m_MainSceneRef = m_SceneRepository.New(m_pMainScene);

//	m_pMouseScene = new COGLBaseElementList;
//	m_MouseSceneRef = m_SceneRepository.New( m_pMouseScene );

	m_DrawBase.SetSceneRepository(&m_SceneRepository);

	// Initialize Standard Mouse Pointer
/*
        m_pStdMousePtr = new COGLBaseElementList;
        m_StdMousePtrRef = m_SceneRepository.New( m_pStdMousePtr );
        m_DrawBase.SetScene( m_StdMousePtrRef );
        COGLVertex xP, xA, xB;
        xP.Set( 0, 0, 0);
        xA.Set( 9,0,0);
        xB.Set(0,9,0);
        m_DrawBase.DrawPlane( xP, xA, xB );
        m_pMouseScene->Add( m_StdMousePtrRef );
*/

	// Set Main Scene as standard scene
	m_DrawBase.SetScene(m_MainSceneRef);

	m_xFilter.SetDrawBase(&m_DrawBase);

	// Reserve max memory for Pick Name Stack
	m_mCurSelBuf.Reserve(m_SceneApplyData.cuMaxPickNameStackLen);
	m_uCurSelPartId = (unsigned) -1;

	m_mCurPick.Set(1);
	m_mCurPick[0].vecID.reserve(m_SceneApplyData.cuMaxPickNameStackLen);

	m_pMouseEventCallbackFunc      = nullptr;
	m_pMouseEventCallbackContext   = nullptr;
	m_bWaitForMouseEventExecScript = false;

	m_pErrorEventCallbackFunc    = nullptr;
	m_pErrorEventCallbackContext = nullptr;

	EnablePickMouseOver();
}

COGLWin::~COGLWin()
{
	if (m_pBrowser)
	{
		delete m_pBrowser;
	}

	if (m_pBrowserWin)
	{
		delete m_pBrowserWin;
	}

	m_pBrowserWin = 0;
	m_pBrowser    = 0;

	m_dlSpace.Destroy();

	if (m_pbExtern_IsValid)
	{
		*m_pbExtern_IsValid = false;
	}
}

////////////////////////////////////////////////////////
/// Set Title

bool COGLWin::SetTitle()
{
	if (m_bIsEmbedded)
	{
		return true;
	}

	if (m_sFixedTitle.size())
	{
		m_pWindow->label(m_sFixedTitle.c_str());
		return true;
	}

	string sMetaTitle;

	if (m_pEditor)
	{
		sMetaTitle = m_pEditor->GenMetaWindowTitle();
	}

	if (sMetaTitle.size())
	{
		m_sTitle       = sMetaTitle;
		m_sOutputTitle = m_sTitle;
	}
	else
	{
		string sVal     = m_xParse.GetCodeBase().GetCurScriptTitle();
		string sFile    = m_xParse.GetCodeBase().GetCurScriptFilename();
		string sCurFile = m_sScriptName + ".clu";
		if ((sVal.size() > 0) && (sFile.size() > 0) &&
		    (sFile.find(sCurFile) != string::npos))
		{
			m_sTitle = sVal;
		}
		else
		{
			if (m_sScriptName.size() == 0)
			{
				m_sTitle = "Untitled";
			}
			else
			{
				m_sTitle = m_sScriptName;
			}
		}

		m_sOutputTitle = m_sTitle;
		m_sTitle      += " - Visualization";
	}

	if (m_sLicensee.size())
	{
		m_sTitle += " - licenced to ";
		m_sTitle += m_sLicensee;
	}

	m_pWindow->label(m_sTitle.c_str());

	if (m_pOutput)
	{
		m_sOutputTitle += " - Output";
		m_pOutput->label(m_sOutputTitle.c_str());
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool COGLWin::GetScriptOutput(std::vector<std::string>& vecText, bool& bIsError)
{
	vecText.clear();
	LockVis(1000);

	if (!m_bHasOutput)
	{
		UnlockVis();
		return true;
	}

	// Get the output object list
	const CCLUCodeBase::TOutObjList& xParseOutputList = m_xParse.GetOutputObjectList();

	// Allocate memory for all output objects
	vecText.reserve(xParseOutputList.size());

	// Iterate over all output objects and create string representations
	for (const CCLUCodeBase::SOutputObject& xObject : xParseOutputList)
	{
		CStrMem sText = xObject.sName + " (" + xObject.sType;

		if (xObject.bShowInfo)
		{
			sText += ", ";
			sText += xObject.sInfo;
		}

		sText += ") ";

		if (xObject.bShowValue)
		{
			sText += xObject.sRelSym;
			sText += xObject.sValue;
		}

		vecText.push_back(sText.Str());
	}

	bIsError     = m_bHasError;
	m_bHasOutput = false;
	UnlockVis();

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Save current view

bool COGLWin::SaveView(const char* pcFilename)
{
	if (!pcFilename)
	{
		return false;
	}

	std::string sFilename;
	sFilename = pcFilename;
	sFilename = Clu::CFilePath::MakeAbsolute(sFilename);

	LockVis();
	m_bDoSaveScreen       = true;
	m_sSaveScreenFilename = sFilename.c_str();
	UnlockVis();

	PostRedisplay(true);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/// Enable/Disable Visualization

void COGLWin::EnableVisualization(bool bVal)
{
	LockVis();
	m_bVisActive = bVal;
	UnlockVis();
}

/////////////////////////////////////////////////////////////////////////////
/// Enable/Disable Drawing of Scene Graph

void COGLWin::EnableDrawSceneGraph(bool bVal, bool bDoRedisplay)
{
	LockVis();
	m_bDoDisplaySceneGraph = bVal;
	m_bDoRedisplay         = bDoRedisplay;
	UnlockVis();
}

/////////////////////////////////////////////////////////////////////////////
/// Get Window handle of GL Window

HWND COGLWin::GetGLWindowHandle()
{
	if (m_bIsEmbedded)
	{
		return m_hGLWnd;
	}
	else
	{
		return (HWND) m_pGLUTWindow->GetWindowHandle();
	}
}

bool COGLWin::EnableStdCtrl(bool bVal)
{
	m_bEnableStdCtrlChanged = true;
	m_bNewStdCtrlState      = bVal;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Enable/Disable Window Border

void COGLWin::EnableBorder(bool bVal)
{
	if (!bVal)
	{
		m_lSaveStyleWnd = (long) GetWindowLongPtr((HWND) m_pWindow->GetWindowHandle(), GWL_STYLE);
		SetWindowLongPtrA((HWND) m_pWindow->GetWindowHandle(), GWL_STYLE, (LONG) (m_lSaveStyleWnd & ~WS_CAPTION & ~WS_SIZEBOX));
	}
	else
	{
		SetWindowLongPtrA((HWND) m_pWindow->GetWindowHandle(), GWL_STYLE, (LONG) m_lSaveStyleWnd);
	}
}

void COGLWin::InitScriptListWindow()
{
	if (m_bIsEmbedded)
	{
		return;
	}

	int iHeight = Fl::h() - 80;
	if (!(m_pBrowserWin = new Fl_Window(Fl::w() - 220, 40, 200, iHeight,
			      "Presentation Scripts")))
	{
		fl_alert("Could not create script list window");
		return;
	}

	//m_pBrowserWin->set_modal();
	m_pBrowserWin->begin();

	if (!(m_pBrowser = new CScriptBrowser(0, 0, 200, iHeight)))
	{
		delete m_pBrowserWin;
		fl_alert("Could not create script browser window");
		return;
	}

	m_pBrowserWin->end();
	m_pBrowser->callback(CB_ScriptListWindow, (void*) this);

	//m_pBrowserWin->set_modal();
	m_pBrowserWin->hide();
}

void COGLWin::ScriptListWindowEvent(Fl_Widget* pWidget)
{
	if (!m_pBrowserWin || !m_pBrowser || !m_pEditor)
	{
		return;
	}

	int iSel = m_pBrowser->value();
	if (iSel <= 0)
	{
		return;
	}

	m_pEditor->SetCurEditor(m_xParse.GetCodeBase().GetCurScriptFilename().c_str());

	m_xParse.GetCodeBase().SetCurScriptIdx(iSel - 1);

	if ((Fl::event() != FL_KEYBOARD) ||
	    ((Fl::event() == FL_KEYBOARD) &&
	     (Fl::event_key() == FL_Enter) &&
	     !Fl::event_state(FL_CTRL)))
	{
		m_pBrowserWin->set_non_modal();
		m_pBrowserWin->hide();
		MenuReloadScript(0);
	}
	else
	{
		MenuReloadScript(0);
		m_pBrowserWin->show();
	}
}

//////////////////////////////////////////////////////////////////////
/// Init
bool COGLWin::Init(bool bFirst)
{
	return Init(bFirst, false, 0);
}

bool COGLWin::Init(bool bFirst, bool bCreateSharedRC, HGLRC* phGlRc)
{
	m_bInitFirst = bFirst;

	// Lighting has to be enabled BEFORE
	// you call CGLUTBase::Init() !!!
	EnableLighting();

	// Can change sensitivity of mouse
	// for rotation...
	SetRotFac(0.5f);// default = 1.0

	// and for translation
	SetTransFac(0.02f);	// default = 0.05

	// Enable lighting for filter
	m_DrawBase.EnableLighting();

	// Reduce sensitivity
	m_DrawBase.SetSensitivity(1e-5);//1e-4);
	m_xFilter.SetSensitivity(1e-5);	//1e-4);

	// Initialize Settings for E3Filter
	m_DrawBase.Init();

//	// Load function modules for parser
//	string sModulePath;
//#ifdef _DEBUG
//	sModulePath = "Q:\\10_Code\\Debug_FLTK\\bin\\";
//#else
//	sModulePath = m_sExecPath + "\\bin\\";
//#endif
//
//	if ( !CCLUParse::LoadModules( sModulePath.c_str() ) )
//		return false;

	// Initialize Parser
	if (!m_xParse.Init())
	{
		return false;
	}

	m_xParse.SetMVFilter(&m_xFilter);
	m_xParse.SetDrawBase(&m_DrawBase);
	m_xParse.SetTransform(&m_vxTransform);
	m_xParse.SetFrameRotor(&m_vRMain);
	m_xParse.SetCLUDrawBase((CCLUDrawBase*) this);
	//m_xParse.InitBitmapFont();

	string sFontPath;
	sFontPath = m_sEnvPath + "\\Font\\";
	m_xParse.GetCodeBase().SetFontPath(sFontPath.c_str());
	m_xParse.GetCodeBase().SetFont("times", "rm", 15);
	m_xParse.GetCodeBase().InitText();

	InitScriptListWindow();

	//AddToolSlider("Mouse Mode", 0, 9, 1, 0, true, "Set the mouse mode here");
	//AddToolSlider("Test2", -10, 2, 0.02, "World");

	SetTitle();

	// Start Animation. This is the main visualization loop and not GLUTDisplay.
	//StartAnimation(m_iAnimPause);

	/////////////////////////////////////////////////
	/// Start visualization loop

	// Get the DC of the GLUT window
	//m_hGLDC = GetDC( (HWND) m_pGLUTWindow->GetWindowHandle() );

	// Initialize OpenGL extensions only once
	if (bFirst)
	{
		InitGLExt();
	}

	// Choose a pixel format
	if (!InitPixelFormat())
	{
		return false;
	}

	if (bCreateSharedRC)
	{
		// Create a RC in the calling thread context
		// This RC will be shared with the vis. thread RC
		// so that textures can be exchanged.
		CreateSharedRC();
		*phGlRc = m_hShareGLRC;
	}
	else if (phGlRc != 0)
	{
		// An external rendering context is given with which
		// this RC should share textures.
		m_hShareExtGLRC = *phGlRc;
	}

	// Create mutex indicating thread is running
	m_hMutexVisRun = CreateMutex(NULL, FALSE, NULL);

	// Create Event used for signaling various things with visualization loop
	m_hEventVis = CreateEvent(NULL, FALSE, FALSE, NULL);

	// Create event to visualize right away without waiting for time loop
	m_hEventVisNow = CreateEvent(NULL, FALSE, FALSE, NULL);

	// Create force visualize event
	m_hEventForceVis = CreateEvent(NULL, FALSE, FALSE, NULL);

	// Start visualization timer loop
	// m_hEventVisTimer = CreateEvent(NULL, FALSE, FALSE, NULL);
	//_beginthread(COGLWin::RunVisTimerLoop, 0, (void*) this);

	// Create visualization timer event
	m_hEventVisTimer = CreateWaitableTimer(NULL, false, NULL);
	LARGE_INTEGER liDueTime;
	liDueTime.QuadPart = 0LL;
	SetWaitableTimer(m_hEventVisTimer, &liDueTime, m_iVisTimeStep, NULL, NULL, 0);

	// Start visualization loop
	_beginthread(COGLWin::RunVisLoop, 0, (void*) this);

	#ifdef _DEBUG
		// Set thread name for debugging reasons
		//Threading::SetThreadName(m_uThreadID, "PIV Thread");
	#endif

	// Wait for visualization thread to initialize
	WaitForSingleObject(m_hEventVis, INFINITE);
	ResetEvent(m_hEventVis);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// React to close button pressed in windows menu

void COGLWin::Quit()
{
	if (!m_bVizOnlyMode && m_pEditor)
	{
		// Only hide the visualization window
		// if not in VizOnly Mode
		//m_pWindow->iconize();
		this->HideWindow();
	}
	else
	{
		DoQuit();
	}
}

//////////////////////////////////////////////////////////////////////
/// Do Quit Program
/// Actually end program

void COGLWin::DoQuit()
{
	SetScript("A = 1;", true, false, false);

	if (!m_bVizOnlyMode && m_pEditor)
	{
		if (!m_pEditor->PreQuit())
		{
			return;
		}
	}

	if (!m_bVizOnlyMode && m_pOutput)
	{
		if (!m_pOutput->PreQuit())
		{
			return;
		}
	}

	// End main visualization loop
	AddExecModeVar(EXEC_MODE_QUIT);
	PostRedisplay(true, true, true);

	// Tell visualization thread to stop
	LockVis();
	m_bRunVisLoop = false;
	UnlockVis();

	// Wait for visualization thread to end
	WaitForSingleObject(m_hMutexVisRun, INFINITE);
	// Close mutex handle
	CloseHandle(m_hMutexVisRun);
	CloseHandle(m_hEventVis);
	CloseHandle(m_hEventVisTimer);
	CloseHandle(m_hEventVisNow);
	CloseHandle(m_hEventForceVis);

	if (m_bIsEmbedded)
	{
		ReleaseWindow();
		delete this;
	}
	else
	{
		DestroyWindow();

		if (m_pEditor)
		{
			delete m_pEditor;
		}
		if (m_pOutput)
		{
			delete m_pOutput;
		}

		delete this;
	}

	//// Only Release module after all member variables of OGLWin have been
	//// deleted, since this also includes the Base Element Repository.
	//CCLUParse::ReleaseModules();

	//exit( 0 );
}

//////////////////////////////////////////////////////////////////////
// Callback function of info window

const char* COGLWin::InfoLink(Fl_Widget* pWidget, const char* pcURL)
{
	if (strncmp(pcURL, "clu://", 6) == 0)
	{
		if (strncmp(&pcURL[6], "data/", 5) == 0)
		{
			string sData;

			sData = &pcURL[11];

			AddExecModeVar(EXEC_MODE_LINK);
			SetLinkDataVar(sData.c_str());

			PostRedisplay();
			//if (!m_bIsAnimated)
			//	glutPostRedisplay();
		}
	}
	else if ((strncmp(pcURL, "http://", 7) == 0) ||
		 (strncmp(pcURL, "ftp://", 6) == 0))
	{
		#ifdef WIN32
			/*
			HINSTANCE hInstance = ShellExecuteA(NULL,
			                "open",
			                pcURL,
			                NULL,
			                NULL,
			                SW_SHOWNORMAL);	//SHOWMINIMIZED);

			        if ((uint) hInstance <= 32)
			        fl_alert("Cannot start web-browser to view help manual");
			        */
		#else	// Linux or other system
			char pcCmd[500];
			char pcLine[300];
			struct stat FileStat;
			ifstream zLog;
			char* pBrowser[] = { "firefox", "konqueror", "mozilla", "netscape" };
			int iPos, iCount = 3;

			// Check whether Mozilla is installed
			::remove("/tmp/clucalc_which.txt");

			for (iPos = 0; iPos < iCount; iPos++)
			{
				sprintf(pcCmd, "which %s > /tmp/clucalc_which.txt", pBrowser[iPos]);
				system(pcCmd);

				// Check whether text file exists
				if ((stat("/tmp/clucalc_which.txt", &FileStat) == 0) && (FileStat.st_size != 0))
				{
					// We found something
					break;
				}
			}

			::remove("/tmp/clucalc_which.txt");

			if (iPos == iCount)
			{
				// Could not find anything
				fl_alert("Could not find any HTML browser to display the URL");
			}
			else
			{
				sprintf(pcCmd, "%s \"%s\" &", pBrowser[iPos], pcURL);
				system(pcCmd);
			}

		#endif
	}

	return 0;	// Do nothing when link is pressed
}

//////////////////////////////////////////////////////////////////////
/// Callback when tool has changed

void COGLWin::ToolChanged(string sName)
{
	//printf("%s\n", sName.c_str());
/*
        if (sName == "Mouse Mode")
        {
                CFLTKToolBox::STool theTool;

                if (m_pToolBox->GetTool(sName.c_str(), theTool))
                {
                        Fl_Valuator *pValuator = (Fl_Valuator *) theTool.pWidget;

                        CCLUDrawBase::Key('0' + char(pValuator->value()), 0, 0);
                }
        }
        else
        {
*/
	AddExecModeVar(EXEC_MODE_TOOL);
	SetToolNameVar(sName.c_str());

	PostRedisplay();
	//if (!m_bIsAnimated)
	//	glutPostRedisplay();

//	printf("Tool\n\n");
	//Display();
//	}

	//if (!m_bIsAnimated)
	//	Display();
}

//////////////////////////////////////////////////////////////////////
/// Load Script and Display it.
/// Returns number of lines parsed.
/// < 0 if error occured.
/// -1: general error
/// -2: error opening file
/// -3: Parse Error

int COGLWin::LoadScript()
{
	char pcText[BUFSIZE + 1];
	int iRead, iLines;
	CStrMem csText;
	FILE* pFile;

	fopen_s(&pFile, m_csScriptFilename, "r");

	if (!pFile)
	{
		return -2;
	}

	while (!feof(pFile))
	{
		iRead         = (int) fread(pcText, 1, BUFSIZE, pFile);
		pcText[iRead] = 0;
		csText << pcText;
	}

	fclose(pFile);

	LockVis();

	ResetTransformVars();

	CCodeVar& rVarCol = m_xParse.GetVar("_BGColor");
	if (rVarCol.Type() != PDT_NOTYPE)
	{
		rVarCol = Black;
		SetBGColor(Black);
	}

	CCodeVar& rVarFBS = m_xParse.GetVar("_FrameBoxSize");
	if (rVarFBS.Type() != PDT_NOTYPE)
	{
		rVarFBS = TCVScalar(3);
	}

	m_xParse.Reset();
	iLines = m_xParse.InsertText(csText);
	UnlockVis();

	if (iLines < 0)
	{
		return -3;
	}

	return iLines;
}

//////////////////////////////////////////////////////////////////////
/// Set Tool Name Variabel

void COGLWin::SetToolNameVar(const char* pcName)
{
	LockVis();
	CCodeVar& rVar = m_xParse.GetVar("ToolName");

	if (rVar.Type() == PDT_STRING)
	{
		*((TString*) rVar.Val()) = (pcName == 0 ? "None" : pcName);
	}
	UnlockVis();
}

//////////////////////////////////////////////////////////////////////
/// Set Link Data Variabel

void COGLWin::SetLinkDataVar(const char* pcData)
{
	LockVis();
	CCodeVar& rVar = m_xParse.GetVar("LinkData");

	if (rVar.Type() == PDT_STRING)
	{
		*((TString*) rVar.Val()) = (pcData == 0 ? "None" : pcData);
	}
	UnlockVis();
}

//////////////////////////////////////////////////////////////////////
/// Set Execution Mode

void COGLWin::SetExecModeVar(int iMode)
{
	LockVis();
	CCodeVar& rVar = m_xParse.GetVar("ExecMode");

	if (rVar.Type() == PDT_INT)
	{
		*((int*) rVar.Val()) = iMode;
	}

	m_iExecMode = iMode;
	UnlockVis();
}

//////////////////////////////////////////////////////////////////////
/// Add to Execution Mode

void COGLWin::AddExecModeVar(int iMode)
{
	LockVis();
	CCodeVar& rVar = m_xParse.GetVar("ExecMode");

	if (rVar.Type() == PDT_INT)
	{
		*((int*) rVar.Val()) |= iMode;
	}

	m_iExecMode |= iMode;
	UnlockVis();
}

//////////////////////////////////////////////////////////////////////
/// Remove from Execution Mode

void COGLWin::RemExecModeVar(int iMode)
{
	LockVis();
	CCodeVar& rVar = m_xParse.GetVar("ExecMode");

	if (rVar.Type() == PDT_INT)
	{
		*((int*) rVar.Val()) &= ~iMode;
	}

	m_iExecMode &= ~iMode;
	UnlockVis();
}

//////////////////////////////////////////////////////////////////////
/// Set Script

int COGLWin::SetScript(const char* pcScript, bool bResetEnv, bool bReparseLatex, bool bIsBinary, int iArgCnt, const char* pcArg)
{
	m_bHasError = false;

	// Only execute checks on change of global variables in idle loop after the next display operation.
	m_bWaitWithIdleForNextDisplay = true;

	SetTitle();
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

		if (m_pToolBox)
		{
			m_pToolBox->ResetToolInUseFlags();
			m_pToolBox->PruneToolMap(false, true);
			m_pToolBar->ResetToolInUseFlags();
			m_pToolBar->PruneToolMap(false, true);
		}

		EnableMenu(true);
		EnableStdCtrl(true);

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
		if (m_pToolBox)
		{
			m_pToolBox->ResetToolInUseFlags();
			m_pToolBox->PruneToolMap(false, true);
			m_pToolBar->ResetToolInUseFlags();
			m_pToolBar->PruneToolMap(false, true);
		}

		EnableMenu(true);
		EnableStdCtrl(true);

		SetExecModeVar(EXEC_MODE_CHANGE);
	}

	m_xParse.PostResetResourceHandles();

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

	SetInfoText("Use <code>SetInfoText([string])</code> to put some text here. The text may include some 'simple' HTML tags");
	SetInfoWidth(0);

	// Show output NOT in fullscreen or visualization only mode
	bool bShowOutput = m_pOutput && (!m_bInFullScreenMode && !m_bVizOnlyMode);

	if (bShowOutput)
	{
		m_pOutput->SetText("<body size=\"16\"><b>Parsing code...</b></body>");
		m_pOutput->show();
	}

	int iLines;
	if (bIsBinary)
	{
		iLines = m_xParse.SetTextCode(pcScript, m_sScriptName.c_str());
	}
	else
	{
		iLines = (pcScript[0] != 0) ? m_xParse.InsertText(pcScript) : m_xParse.InsertText("_NoCode = true;");
	}

	if (bShowOutput)
	{
		m_pOutput->SetText("<body size=\"14\">Parsing code...<b>finished</b>.<br>Running code after change. <b>Please wait...</b></body>");
		m_pOutput->show();
	}

	if (!m_bInFullScreenMode && !m_bIsEmbedded)
	{
		if (m_bPresentationAction || m_bParseFromOGLWin || m_bVizOnlyMode)
		{
			ShowWindow();
		}
		else
		{
			m_pEditor->show();
		}
	}

	m_bPresentationAction = false;
	m_bParseFromOGLWin    = false;

	if (iLines >= 0)
	{
		m_bCheckForAnimate    = true;
		m_bCodeChanged        = true;
		m_bCodeChangedForAnim = true;
		m_bCheckToolBoxSize   = true;

		if (iArgCnt > 0)
		{
			// Set array _P with command line args
			CCodeVar& rParVar = m_xParse.NewVar("_P", PDT_VARLIST);
			TVarList& rPar    = *rParVar.GetVarListPtr();

			std::string sArg = pcArg;
			int iPos1        = 0;

			rPar.Set(iArgCnt);
			for (int i = 0; i < iArgCnt; ++i)
			{
				int iPos2 = int(sArg.find_first_of("\r", iPos1));

				std::string sValue = (iPos2 < 0) ? sArg.substr(iPos1) : sArg.substr(iPos1, iPos2 - iPos1);

				iPos1 = iPos2 + 1;

				rPar[i].New(PDT_STRING, "Constant");
				*(rPar[i].GetStringPtr()) = sValue.c_str();
			}
		}
	}
	else
	{
		ClearError();
		m_sError  = "<b>Parse Errors:</b><p>";
		m_sError += m_xParse.PrintParseErrors();
		m_sError += "</p>";

		m_bHasError = true;

		if (bShowOutput)
		{
			m_pOutput->SetText(m_sError.c_str());
			m_pOutput->show();
		}

		if (m_pEditor && !m_bVizOnlyMode && !m_pEditor->IsFileBinary())
		{
			CStrMem csFilename;
			int iInputPos;

			if (iLines == -1)
			{
				int iLine, iPos;
				m_xParse.GetParseErrorPos(iLine, iPos, iInputPos, csFilename);
			}
			else
			{
				csFilename = m_xParse.GetErrorFilename().c_str();

				if (csFilename.Len() == 0)
				{
					csFilename = m_sScriptPath.c_str();
					csFilename << m_sScriptName.c_str() << ".clu";
				}

				char* pcName = csFilename.Str();
				if (!strchr(pcName, ':') && !strchr(pcName, '/') && !strchr(pcName, '\\'))
				{
					csFilename  = m_xParse.GetErrorPath().c_str() + csFilename;
					csFilename |= int('.' < csFilename) - int(csFilename.Len()) - 1;
				}

				iInputPos = -iLines - 2;
			}

			m_pEditor->HighlightLinePos(csFilename.Str(), iInputPos);
			m_pEditor->show();
		}

		m_xParse.ClearError();
	}

	UnlockVis();

	// Redisplay and wait until script has been executed at least once
	if (!m_bHasError)
	{
		PostRedisplay(true, true, true);
	}

	return iLines;
}

//////////////////////////////////////////////////////////////////////
/// Reload Script

int COGLWin::CheckScriptChanged()
{
	// Not implemented yet

	return -1;
}

//////////////////////////////////////////////////////////////////////
/// Call Display Output

void COGLWin::CallDisplayOutput()
{
	if (m_pThisWin)
	{
		m_pThisWin->DisplayOutput();
	}
}

//////////////////////////////////////////////////////////////////////
/// Call Reshape Output

void COGLWin::CallReshapeOutput(int iWidth, int iHeight)
{
	if (m_pThisWin)
	{
		m_pThisWin->ReshapeOutput(iWidth, iHeight);
	}
}

//////////////////////////////////////////////////////////////////////
/// Call Idle Func

void COGLWin::CallIdleFunc()
{
	if (m_pThisWin)
	{
		m_pThisWin->IdleFunc();
	}
}

//////////////////////////////////////////////////////////////////////
/// Reshape

void COGLWin::ReshapeOutput(int iWidth, int iHeight)
{
	glViewport(0, 0, m_iOutSizeX = iWidth, m_iOutSizeY = iHeight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, m_iOutSizeX, m_iOutSizeY, 0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//////////////////////////////////////////////////////////////////////

void COGLWin::Sleep(unsigned int uMilliSec)
{
	#ifdef WIN32
		::Sleep(uMilliSec);
	#else
		timespec waittime;

		waittime.tv_sec  = uMilliSec / 1000;
		waittime.tv_nsec = long(1000000) * long(uMilliSec % 1000);

		nanosleep(&waittime, 0);
	#endif
}

//////////////////////////////////////////////////////////////////////
/// Reshape Windpw

void COGLWin::Reshape(int iWidth, int iHeight)
{
	//if ( iWidth == m_iReshapeW && iHeight == m_iReshapeH )
	//	return;

	LockVis();

	m_bDoReshape     = true;
	m_iReshapeW      = iWidth;
	m_iReshapeH      = iHeight;
	m_bDoUpdateTools = true;

	EnableMenu(m_bMenuShown);
	EnableStdCtrl(m_bStdCtrlShown);

	UnlockVis();

	// Need wait for a redisplay to avoid a flickering of the visualization
	// during resize.
	PostRedisplay(true);

	//COGLWinFLTK::Reshape(iWidth, iHeight);

	//AddExecModeVar(EXEC_MODE_RESHAPE);
}

//////////////////////////////////////////////////////////////////////
// Get OGL Window Size

bool COGLWin::GetOGLSize(double& dWidth, double& dHeight)
{
	if (!m_bIsEmbedded)
	{
		COGLWinFLTK::GetOGLSize(dWidth, dHeight);
	}
	else
	{
		LockVis();
		RECT xRect;
		::GetWindowRect(m_hGLWnd, &xRect);
		dWidth  = xRect.right - xRect.left + 1;
		dHeight = xRect.bottom - xRect.top + 1;
		UnlockVis();
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set Size of OGL Window in pixel or percent

bool COGLWin::SetOGLSize(double dWidth, double dHeight)
{
	if (!m_bIsEmbedded)
	{
		LockVis();
		m_bScriptChangedOGLSize = true;
		m_dNewOGLWidth          = dWidth;
		m_dNewOGLHeight         = dHeight;
		UnlockVis();
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Info Window Width
/// Called by script

bool COGLWin::SetInfoWidth(double dWidth)
{
	LockVis();
	m_dNewInfoWidth = dWidth;
	UnlockVis();
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Menu Animate

void COGLWin::MenuAnimate(Fl_Widget* pWidget)
{
	const Fl_Menu_Item* pItem = ((Fl_Menu_*) pWidget)->mvalue();

	LockVis();
	if (pItem->checked())
	{
		m_bAnimCode = true;
	}
	else
	{
		m_bAnimCode = false;
	}
	UnlockVis();
}

bool COGLWin::LoadScript(const char* pcFilename, bool bResetEnv, const char* pcPass)
{
	if (!m_pEditor || !pcFilename)
	{
		return false;
	}

	std::string sFilename;
	sFilename = pcFilename;
	sFilename = Clu::CFilePath::MakeAbsolute(sFilename);

	if (!m_pEditor->LoadFile(sFilename.c_str(), -1, pcPass))
	{
		return false;
	}

	m_bParseFromOGLWin = true;
	m_pEditor->Parse(bResetEnv);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// (Re-)Execute Script in User Exec-Mode

void COGLWin::ExecUser(const char* pcToolName)
{
	AddExecModeVar(EXEC_MODE_USER);

	if (pcToolName)
	{
		SetToolNameVar(pcToolName);
	}

	// Force execution of script, even if the window is disabled.
	// Also ensure that script is run right away without waiting
	// for next timer loop signal.
	PostRedisplay(true, true, true);
}

//////////////////////////////////////////////////////////////////////
/// Idle Func

void COGLWin::IdleFunc()
{
/*
        // If message to quit is set, do it.
        if ( m_uMsgID == OGLWIN_MSG_QUIT )
        {
                Quit();
                //ReleaseMutex( m_hMutexMsg );
                return;
        }
        else if ( m_uMsgID == OGLWIN_MSG_SETSCRIPT )
        {
                SetScript( m_sMsgData.c_str() );
        }
        else if ( m_uMsgID == OGLWIN_MSG_PARSE )
        {
                m_bParseFromOGLWin = true;
                m_pEditor->LoadFile( m_sMsgData.c_str() );
                m_pEditor->Parse( m_bMsgData );
        }
        else if ( m_uMsgID == OGLWIN_MSG_EXEC )
        {
                AddExecModeVar(EXEC_MODE_USER);
                glutPostRedisplay();
        }
*/

	if (m_pOutput)
	{
		if (m_bHasOutput)
		{
			if (!m_bVizOnlyMode || m_bHasError)
			{
				LockVis();
				m_pOutput->SetOutputObjectList(m_xParse.GetOutputObjectList());
				UnlockVis();
				ClearOutput();
			}

			if (m_bVizOnlyMode && m_bHasError)
			{
				m_pOutput->show();
				m_pOutput->SetPosSize(50, 50, 500, 300);
			}
		}
		//else
		//{
		//	m_pOutput->Clear();
		//}
	}

	if (m_bInFullScreenMode)
	{
		if (m_pOutput && m_pOutput->shown())
		{
			m_pOutput->iconize();
		}
	}

	if (m_bDoDisplayInIdle)
	{
		m_bDoDisplayInIdle = false;
		PostRedisplay();

		//DoDisplay();
	}

	// Have to check global variables of script only after script
	// has been executed once.
	if (!m_bWaitWithIdleForNextDisplay)
	{
		LockVis();

		if (m_bShowErrorInEditor)
		{
			CStrMem csFilename;
			int iLine, iPos, iInputPos;

			if (m_pEditor)
			{
				if (m_xParse.GetCodeErrorPos(iLine, iPos, iInputPos, csFilename))
				{
					m_pEditor->HighlightLinePos(csFilename.Str(), iInputPos);
				}

				m_pEditor->show();
			}

			m_xParse.ResetErrors();

			m_bShowErrorInEditor = false;
		}

		if (!m_bUserMenuActive)
		{
			m_pMenuBar->value(1);
			Fl_Menu_Item* pItem = (Fl_Menu_Item*) m_pMenuBar->mvalue();

			if (m_bAnimCode && (pItem->checked() == 0))
			{
				pItem->check();
				m_pMenuBar->redraw();
			}
			else if (!m_bAnimCode && (pItem->checked() > 0))
			{
				pItem->uncheck();
				m_pMenuBar->redraw();
			}
		}

		if (m_bScriptChangedOGLSize)
		{
			COGLWinFLTK::SetOGLSize(m_dNewOGLWidth, m_dNewOGLHeight);
			m_bScriptChangedOGLSize = false;
			m_bCheckToolBoxSize     = true;
		}

		if (m_bResetTools)
		{
			m_bResetTools = false;

			if (m_pToolBox)
			{
				m_pToolBox->ResetToolInUseFlags();
				// Keep fixed tools but do not redraw here
				m_pToolBox->PruneToolMap(true, false);

				m_pToolBar->ResetToolInUseFlags();
				// Keep fixed tools but do not redraw here
				m_pToolBar->PruneToolMap(true, false);
			}
		}

		if (m_bPruneTools)
		{
			m_bPruneTools = false;
			if (m_pToolBox)
			{
				m_pToolBox->PruneToolMap(true, false);
				m_pToolBar->PruneToolMap(true, false);

				m_bDoUpdateTools |= m_pToolBox->ToolsChanged() | m_pToolBar->ToolsChanged();
			}
		}

		if (m_bCheckToolBoxSize || m_bDoUpdateTools)
		{
			if (m_pToolBox)
			{
				m_pToolBox->Update();
				m_pToolBar->Update();
				CheckToolBoxSize();
				CheckToolBarSize();
			}
			m_bCheckToolBoxSize = false;
			m_bDoUpdateTools    = false;
		}

		if (m_bDoRedrawTools)
		{
			if (m_pToolBox)
			{
				m_pToolBox->CheckToolsRedraw();
				m_pToolBar->CheckToolsRedraw();
			}
			m_bDoRedrawTools = false;
		}

		if (m_dNewInfoWidth >= 0)
		{
			COGLWinFLTK::SetInfoWidth(m_dNewInfoWidth);
			m_dNewInfoWidth = -1.0;
		}

		if (m_bEnableMenuChanged)
		{
			COGLWinFLTK::EnableMenu(m_bNewMenuState);
			m_bEnableMenuChanged = false;
		}

		if (m_bEnableStdCtrlChanged)
		{
			COGLWinFLTK::EnableStdCtrl(m_bNewStdCtrlState);
			m_bEnableStdCtrlChanged = false;
		}

		if (m_bMenuChanged)
		{
			COGLWinFLTK::SetMenu(m_vecNewMenuItem);
			m_bMenuChanged = false;
		}

		UnlockVis();
	}
/*
        else
        {
                printf("Waiting to check global vars.\n");
        }
*/
}

//////////////////////////////////////////////////////////////////////
/// Display Output

void COGLWin::DisplayOutput()
{
	/*
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	int i;
	int iLine = m_iOutLineHeight;
	int iLen = m_csOutput.Len();
	char *pcText = m_csOutput.Str();
	unsigned char cSym;

	if (!iLen || !pcText)
	        return;

	glRasterPos2f(0, iLine);

	for(i=0;i<iLen;i++)
	{
	        cSym = pcText[i];

	        if (cSym == 10)
	        {
	                iLine += m_iOutLineHeight;
	                glRasterPos2f(0, iLine);
	        }
	        else if (cSym >= 32 && cSym < 255)
	        {
	                glutBitmapCharacter(m_pOutputFont, cSym);
	        }
	}

	glutSwapBuffers();
	*/
}

// //////////////////////////////////////////////////////////////////////////////////////////////
TString COGLWin::CreateSceneNameString(MemObj<COGLBEReference>& mSceneList, bool bListOnlyPickableScenes)
{
	int iScene, iSceneCnt = int(mSceneList.Count());
	bool bStoreName   = false;
	TString sNameList = "";

	for (iScene = 0; iScene < iSceneCnt; iScene++)
	{
		COGLBEReference& SceneRef = mSceneList[iScene];

		if (SceneRef.IsValid())
		{
			if (!bStoreName)
			{
				COGLScene* pScene = dynamic_cast<COGLScene*>((COGLBaseElement*) SceneRef);
				if (pScene && (pScene->IsPickable() || !bListOnlyPickableScenes))
				{
					bStoreName = true;
				}
			}

			if (bStoreName)
			{
				if (sNameList.Len() > 0)
				{
					sNameList << ";";
				}
				sNameList << TString(SceneRef->GetName().c_str());
			}
		}
	}

	return sNameList;
}

//////////////////////////////////////////////////////////////////////
bool COGLWin::AddScenePartName(TVarList& rList, unsigned uPartName)
{
	rList.Add(1);
	CCodeVar& rVar = rList.Last();
	rVar.New(PDT_VARLIST, "Constant");
	TVarList& rPartNameList = *rVar.GetVarListPtr();
	rPartNameList.Set(2);
	rPartNameList(0) = TString("part_id");
	rPartNameList(1) = TCVCounter(uPartName);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Create TVarList of names of scenes

TString COGLWin::AddSceneNameVarList(TVarList& rList, MemObj<COGLBEReference>& mSceneList)
{
	int iScene, iSceneCnt = int(mSceneList.Count());
	bool bStoreName = false;

	rList.Add(1);
	CCodeVar& rVar = rList.Last();
	rVar.New(PDT_VARLIST, "Constant");
	TVarList& rNameList = *rVar.GetVarListPtr();
	rNameList.Set(2);
	rNameList(0) = TString("name");
	rNameList(1).New(PDT_VARLIST, "Constant");

	TVarList& rElList = *rNameList(1).GetVarListPtr();
	rElList.Set(0);

	for (iScene = 0; iScene < iSceneCnt; iScene++)
	{
		COGLBEReference& SceneRef = mSceneList[iScene];

		if (SceneRef.IsValid())
		{
			if (!bStoreName)
			{
				COGLScene* pScene = dynamic_cast<COGLScene*>((COGLBaseElement*) SceneRef);
				if (pScene && pScene->IsPickable() && pScene->DoNotify())
				{
					bStoreName = true;
				}
			}

			if (bStoreName)
			{
				rElList.Add(1);
				rElList.Last() = TString(SceneRef->GetName().c_str());
			}
		}
		else
		{
			rElList[iScene] = "";
		}
	}

	if (iSceneCnt > 0)
	{
		return *rElList[0].GetStringPtr();
	}
	else
	{
		return TString();
	}
}

//////////////////////////////////////////////////////////////////////
/// Add TVarList of drag data to given varlist

bool COGLWin::AddSceneDragDataList(TVarList& rList, int iDragIdx, float fX, float fY, float fZ,                                        float fDX, float fDY, float fDZ)
{
	rList.Add(1);
	CCodeVar& rVar = rList.Last();
	rVar.New(PDT_VARLIST, "Constant");
	TVarList& rDataList = *rVar.GetVarListPtr();
	rDataList.Set(2);
	rDataList(0) = "drag";
	rDataList(1).New(PDT_VARLIST, "Constant");

	TVarList& rSubList = *rDataList(1).GetVarListPtr();
	rSubList.Set(4);

	rSubList(0) = TCVCounter(iDragIdx);

	{
		rSubList(1).New(PDT_VARLIST, "Constant");
		TVarList& rElList = *rSubList(1).GetVarListPtr();
		rElList.Set(3);
		rElList(0) = TCVScalar(m_pfDragDataStart[0]);
		rElList(1) = TCVScalar(m_pfDragDataStart[1]);
		rElList(2) = TCVScalar(m_pfDragDataStart[2]);
	}

	{
		rSubList(2).New(PDT_VARLIST, "Constant");
		TVarList& rElList = *rSubList(2).GetVarListPtr();
		rElList.Set(3);

		rElList(0) = TCVScalar(fX);
		rElList(1) = TCVScalar(fY);
		rElList(2) = TCVScalar(fZ);
	}

	{
		rSubList(3).New(PDT_VARLIST, "Constant");
		TVarList& rElList = *rSubList(3).GetVarListPtr();
		rElList.Set(3);

		rElList(0) = TCVScalar(fDX);
		rElList(1) = TCVScalar(fDY);
		rElList(2) = TCVScalar(fDZ);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add TVarList of drag data to given varlist

bool COGLWin::AddSceneDragEventList(TVarList& rList, const char* pcEvent)
{
	rList.Add(1);
	CCodeVar& rVar = rList.Last();
	rVar.New(PDT_VARLIST, "Constant");
	TVarList& rDataList = *rVar.GetVarListPtr();
	rDataList.Set(2);
	rDataList(0) = "event";
	rDataList(1) = pcEvent;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add TVarList of mouse drag positions to given varlist

bool COGLWin::AddSceneDragMouseList(TVarList& rList, double dX, double dY, double dZ,                          double dDX, double dDY, double dDZ)
{
	rList.Add(1);
	CCodeVar& rVar = rList.Last();
	rVar.New(PDT_VARLIST, "Constant");
	TVarList& rDataList = *rVar.GetVarListPtr();
	rDataList.Set(2);
	rDataList(0) = "pos";
	rDataList(1).New(PDT_VARLIST, "Constant");

	TVarList& rSubList = *rDataList(1).GetVarListPtr();
	rSubList.Set(3);

	{
		rSubList(0).New(PDT_VARLIST, "Constant");
		TVarList& rElList = *rSubList(0).GetVarListPtr();
		rElList.Set(3);
		rElList(0) = TCVScalar(m_pdMouseDragStart[0]);
		rElList(1) = TCVScalar(m_pdMouseDragStart[1]);
		rElList(2) = TCVScalar(m_pdMouseDragStart[2]);
	}

	{
		rSubList(1).New(PDT_VARLIST, "Constant");
		TVarList& rElList = *rSubList(1).GetVarListPtr();
		rElList.Set(3);
		rElList(0) = TCVScalar(dX);
		rElList(1) = TCVScalar(dY);
		rElList(2) = TCVScalar(dZ);
	}

	{
		rSubList(2).New(PDT_VARLIST, "Constant");
		TVarList& rElList = *rSubList(2).GetVarListPtr();
		rElList.Set(3);
		rElList(0) = TCVScalar(dDX);
		rElList(1) = TCVScalar(dDY);
		rElList(2) = TCVScalar(dDZ);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add TVarList of mouse drag positions relative to frame after auto translation to given varlist

bool COGLWin::AddSceneDragMouseLocalList(TVarList& rList, double dX, double dY, double dZ,                     double dDX, double dDY, double dDZ)
{
	rList.Add(1);
	CCodeVar& rVar = rList.Last();
	rVar.New(PDT_VARLIST, "Constant");
	TVarList& rDataList = *rVar.GetVarListPtr();
	rDataList.Set(2);
	rDataList(0) = "local";
	rDataList(1).New(PDT_VARLIST, "Constant");

	TVarList& rSubList = *rDataList(1).GetVarListPtr();
	rSubList.Set(3);

	{
		rSubList(0).New(PDT_VARLIST, "Constant");
		TVarList& rElList = *rSubList(0).GetVarListPtr();
		rElList.Set(3);
		rElList(0) = TCVScalar(m_pdMouseDragLocalStart[0]);
		rElList(1) = TCVScalar(m_pdMouseDragLocalStart[1]);
		rElList(2) = TCVScalar(m_pdMouseDragLocalStart[2]);
	}

	{
		rSubList(1).New(PDT_VARLIST, "Constant");
		TVarList& rElList = *rSubList(1).GetVarListPtr();
		rElList.Set(3);
		rElList(0) = TCVScalar(dX);
		rElList(1) = TCVScalar(dY);
		rElList(2) = TCVScalar(dZ);
	}

	{
		rSubList(2).New(PDT_VARLIST, "Constant");
		TVarList& rElList = *rSubList(2).GetVarListPtr();
		rElList.Set(3);
		rElList(0) = TCVScalar(dDX);
		rElList(1) = TCVScalar(dDY);
		rElList(2) = TCVScalar(dDZ);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Get scene element lowest in the current pick hierarchy

COGLBEReference COGLWin::GetSceneElement(Mem<uint>& mUID)
{
	COGLBEReference Ref;

	int iID, iIDCnt = int(mUID.Count());
	COGLScene* pScene;

	for (iID = iIDCnt - 1; iID > 0; --iID)
	{
		Ref = m_pMainScene->GetElementWithUID(mUID[iID]);
		if ((pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Ref)))
		{
			return Ref;
		}
	}

	return COGLBEReference();
}

//////////////////////////////////////////////////////////////////////
/// Get list of scene elements in the current pick hierarchy

MemObj<COGLBEReference> COGLWin::GetSceneElementList(const std::vector<uint>& vecUID)
{
	COGLBEReference Ref;
	MemObj<COGLBEReference> mRef;

	int iIDCnt = int(vecUID.size());
	COGLScene* pScene;
	mRef.Reserve(iIDCnt);

	for (int iID = iIDCnt - 1; iID > 0; --iID)
	{
		Ref = m_pMainScene->GetElementWithUID(vecUID[iID]);
		if ((pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) Ref)))
		{
			mRef.PushBack(Ref);
		}
	}

	return mRef;
}

//////////////////////////////////////////////////////////////////////
/// Check whether the size of the tool box should be enlarged or reduced.

void COGLWin::CheckToolBoxSize()
{
	if (!m_pToolBox)
	{
		return;
	}

	int iX, iY, iWidth, iHeight, iDeltaHeight;
	int iToolCount = m_pToolBox->GetToolCount();

	iX           = iY = 0;
	iWidth       = iHeight = 0;
	iDeltaHeight = 0;

	if ((m_iCurToolCount == 0) && (iToolCount > 0))
	{
		m_pToolBox->GetPosition(iX, iY);
		m_pToolBox->GetSize(iWidth, iHeight);
		iDeltaHeight = iHeight - m_iMinToolHeight - m_iToolHeight;

		m_pTile->position(m_iToolBoxInitX, m_iToolBoxInitY, iX, iY + iDeltaHeight);
		CCLUDrawBase::Reshape(m_iSizeX, m_iSizeY + iDeltaHeight);
	}
	else if ((m_iCurToolCount > 0) && (iToolCount == 0))
	{
		m_pToolBox->GetPosition(iX, iY);
		m_pToolBox->GetSize(iWidth, iHeight);
		iDeltaHeight = iHeight - m_iMinToolHeight;

		m_pTile->position(m_iToolBoxInitX, m_iToolBoxInitY, iX, iY + iDeltaHeight);
		CCLUDrawBase::Reshape(m_iSizeX, m_iSizeY + iDeltaHeight);
	}

	m_iCurToolCount = iToolCount;
}

//////////////////////////////////////////////////////////////////////
/// Check whether the size of the toolbar should be enlarged or reduced.

void COGLWin::CheckToolBarSize()
{
	if (!m_pToolBar)
	{
		return;
	}

	int iX, iY, iWidth, iHeight, iDeltaHeight;
	int iToolCount = m_pToolBar->GetToolCount();

	iX           = iY = 0;
	iWidth       = iHeight = 0;
	iDeltaHeight = 0;

	if ((m_iCurToolbarCount == 0) && (iToolCount > 0))
	{
		//m_pToolBar->GetPosition(iX, iY);
		iX = m_pGLUTWindow->x();
		iY = m_pGLUTWindow->y();

		m_pToolBar->GetSize(iWidth, iHeight);
		iDeltaHeight = m_iSmallToolHeight + 2 * m_iSmallToolBorderH - iHeight + 2;	// - m_iToolBarInitHeight;
/*
                printf("Add Toolbar\n");
                printf("iX, iY: %d, %d\n", iX, iY);
                printf("iWidth, iHeight: %d, %d\n", iWidth, iHeight);
                printf("m_iMinToolHeight, m_iToolHeight: %d, %d\n", m_iMinToolHeight, m_iToolHeight);
                printf("iDeltaHeight: %d\n", iDeltaHeight);
                printf("m_iSizeX, m_iSizeY: %d, %d\n\n", m_iSizeX, m_iSizeY);
*/
		m_pTile->position(m_iOGLWinInitX, m_iOGLWinInitY, iX, iY + iDeltaHeight);
		CCLUDrawBase::Reshape(m_iSizeX, m_iSizeY - iDeltaHeight);

//		printf("New m_iSizeX, m_iSizeY: %d, %d\n\n", m_iSizeX, m_iSizeY);
	}
	else if ((m_iCurToolbarCount > 0) && (iToolCount == 0))
	{
		//m_pToolBar->GetPosition(iX, iY);
		iX = m_pGLUTWindow->x();
		iY = m_pGLUTWindow->y();

		m_pToolBar->GetSize(iWidth, iHeight);
		iDeltaHeight = m_iToolBarInitHeight - iHeight;
/*
                printf("Remove toolbar\n");
                printf("iX, iY: %d, %d\n", iX, iY);
                printf("iWidth, iHeight: %d, %d\n", iWidth, iHeight);
                printf("m_iMinToolHeight, m_iToolHeight: %d, %d\n", m_iMinToolHeight, m_iToolHeight);
                printf("iDeltaHeight: %d\n", iDeltaHeight);
                printf("m_iSizeX, m_iSizeY: %d, %d\n\n", m_iSizeX, m_iSizeY);
*/
		m_pTile->position(m_iOGLWinInitX, m_iOGLWinInitY, iX, iY + iDeltaHeight);
		CCLUDrawBase::Reshape(m_iSizeX, m_iSizeY - iDeltaHeight);

//		printf("New m_iSizeX, m_iSizeY: %d, %d\n\n", m_iSizeX, m_iSizeY);
	}

	m_iCurToolbarCount = iToolCount;
}

string COGLWin::GetSceneGrapgSubtreePrint(COGLBaseElement* pBaseElement, int iLevel)
{
	string sResult = "";
	string sPrefix = "";

	for (int i = 0; i < iLevel; i++)
	{
		sPrefix += "| ";
	}

	sResult += sPrefix + "|-[" + pBaseElement->GetName() + "," + pBaseElement->GetTypeName() + "]\n";

	COGLBaseElementList* pElementList = 0;
	if ((pElementList = dynamic_cast<COGLBaseElementList*>(pBaseElement)))
	{
		const std::list<COGLBEReference>& rElList = pElementList->GetElementList();
		std::list<COGLBEReference>::const_iterator itEl;

		for (itEl = rElList.begin(); itEl != rElList.end(); ++itEl)
		{
			sResult += GetSceneGrapgSubtreePrint((COGLBaseElement*) (*itEl), iLevel + 1);
		}
	}

	return sResult;
}

string COGLWin::GetSceneGraphPrint()
{
	string sResult;

	COGLBaseElementList* pBEList = m_pMainScene;

	sResult += GetSceneGrapgSubtreePrint(pBEList, 0);

	return sResult;
}
