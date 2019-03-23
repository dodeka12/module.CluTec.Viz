////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.View.Base
// file:      CLUVizApp.cpp
//
// summary:   Implements the clu viz application class
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
#include "CLUVizApp.h"
//#include "../resource.h"

#include "CluTec.OpenGL/Vertex.MeshObject.h"
#include "CluTec.Base/Exception.h"

#ifdef _GNUCPP3_
#include <unistd.h>
#define _getcwd getcwd
#define _chdir chdir
#else
#include <direct.h>
#endif

#define BUFSIZE         1024

////////////////////////////////////////////////////////////////
/// Constructor

CCLUVizApp::CCLUVizApp()
{
	m_sLastError = "";
	m_pvIcon = 0;
	m_pOutput = 0;
	m_pEditor = 0;
	m_poglWin = 0;
	m_bIsCreated = false;
	m_bIsEmbedded = false;
}

////////////////////////////////////////////////////////////////
/// Destructor

CCLUVizApp::~CCLUVizApp()
{
	if (m_bIsEmbedded)
	{
		ReleaseWindow();
	}
	else
	{
		Destroy();
	}
}

////////////////////////////////////////////////////////////////
// If variable exists, set it to new type and return it.
// Otherwise, create variable of given type and name.

CCodeVar& CCLUVizApp::CreateCodeVar(const char* pcVarName, ECodeDataType eDataType, bool bForce)
{
	CCodeVar* pVar = &(m_poglWin->m_xParse.GetVar(pcVarName));

	if (pVar->BaseType() == PDT_NOTYPE)
	{
		pVar = &(m_poglWin->m_xParse.NewVar(pcVarName, eDataType));
	}
	else if ((pVar->BaseType() != PDT_IMAGE) || bForce)
	{
		pVar->New(eDataType, pcVarName);
	}

	return *pVar;
}

////////////////////////////////////////////////////////////////
/// Embed Window

bool CCLUVizApp::EmbedWindow(HWND hWnd, const char* pcCLUPath, bool bCreateSRC, HGLRC* phGlRc, bool bLoadIntroScript)
{
	if (m_bIsCreated)
	{
		m_sLastError = "Cannot create/embed a window more than once.";
		return false;
	}

	const char* pcIntro = 0;
	if (bLoadIntroScript)
	{
		pcIntro =
			"// Press CTRL+n to start a new script.\n\n"
			"/*\n\n"
			"Written and conceived by Dr. Christian Perwass.\n"
			"*/\n"
			"_BGColor = White;\n"
			"DefVarsN3();\n\n"
			"\"Change to mouse mode 1 and move the red sphere\";\n"
			"\"by holding down the right mouse button and\";\n"
			"\"moving the mouse in the visualization window.\";\n\n"
			"\" \";\n"
			"\"Press CTRL + h in the editor window for more help.\";\n\n"
			":N3_IPNS;\n"
			":N3_SOLID;\n"
			":N3_SHOW_IMAG;\n"
			"EnableAntiAliasing( true );\n"
			"SetSphereDetailLevel( 10 );\n"
			"EnableStdCtrl( false );\n"
			"EnableMenu( false );\n"
			"\n"
			"AnimColor(Color(0.5,0.5,0.5), \"Sinus\", Color(0.4, 0, 0, 0), 0.5 );\n"
			":Sr = VecN3(VecE3(1) + VecE3(-0.12,0,0.2)) - 0.5*e;\n"
			"\n"
			":Color(0, 0, 0.6, 0.6);\n"
			":Sb = VecN3(0.5, 0, 0) - 0.5*e;\n"
			"\n"
			":Color(0, 0.6, 0, 0.6);\n"
			":Sg = VecN3(-0.5, 0.5, 0) - 0.5*e;\n"
			"\n"
			":Color(0.540, 0.991, 1.000);\n"
			"SetTextMagStep(14);\n"
			"SetImgPos( -1.5, 0.3, 1.5 );\n"
			":GetTextImg( \"{\\bf CLU}{\\it Viz} v" CLUVIZ_VERSION "\" );\n"
			"\n"
			":Magenta;\n"
			":C = Sr^Sb;\n"
			"\n"
			":Cyan;\n"
			":C = Sr^Sg;\n"
			"\n"
			":Yellow;\n"
			":PP = Sr^Sb^Sg;\n\n"
			":VecC2(0,1)^VecC2(1,0)^VecC2(0,-1)^VecC2(-1,0)^VecC2(1.5,0.2);\n\n"
			"\n";
	}

	m_bVizOnly = true;

	if (!(m_poglWin = new COGLWin))
	{
		m_sLastError = "Cannot create OpenGL window. Probably not enough memory.";
		delete m_pOutput;
		return false;
	}

	m_bOGLWinValid = true;
	m_poglWin->SetIsValidPtr(&m_bOGLWinValid);

	//m_poglWin->SetLicensee( m_sLicensee.c_str() );

	//if ( m_sTitle.size() )
	//	m_poglWin->SetFixedTitle( m_sTitle.c_str() );

	if (m_bVizOnly)
	{
		m_poglWin->EnableVizOnlyMode();
	}

	if (pcCLUPath)
	{
		m_poglWin->SetEnvPath(pcCLUPath);
	}

	m_poglWin->EmbedWindow(hWnd, bCreateSRC, phGlRc);

	m_pOutput = 0;
	m_pEditor = 0;

	m_poglWin->SetOutputWin(m_pOutput);
	m_poglWin->SetEditor(m_pEditor);

	m_poglWin->SetVersion(CLUVIZ_VERSION_MAJOR, CLUVIZ_VERSION_MINOR, CLUVIZ_VERSION_REVISION);

	if (pcIntro)
	{
		m_poglWin->SetScript(pcIntro, true, false, false);
	}

	m_bIsEmbedded = true;
	m_bIsCreated = true;
	return true;
}

////////////////////////////////////////////////////////////////
/// Release Window

bool CCLUVizApp::ReleaseWindow()
{
	if (!m_bIsCreated || !m_bIsEmbedded)
	{
		return true;
	}

	if (m_bOGLWinValid)
	{
		m_poglWin->Quit();
	}

#ifdef WIN32
	if (m_pvIcon)
	{
		DestroyIcon((HICON)m_pvIcon);
	}
#endif

	m_pOutput = 0;
	m_pEditor = 0;
	m_poglWin = 0;

	m_bIsEmbedded = false;
	m_bIsCreated = false;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sets a mouse event callback function
bool CCLUVizApp::SetMouseEventHandler(Clu::Viz::View::TFuncMouseEventCallback pFunc, void* pvContext)
{
	return m_poglWin->SetMouseEventHandler(pFunc, pvContext);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Removes the mouse event callback function
bool CCLUVizApp::RemoveMouseEventHandler()
{
	return m_poglWin->RemoveMouseEventHandler();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCLUVizApp::SetErrorEventHandler(Clu::Viz::View::TFuncErrorEventCallback pFunc, void* pvContext)
{
	return m_poglWin->SetErrorEventHandler(pFunc, pvContext);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCLUVizApp::RemoveErrorEventHandler()
{
	return m_poglWin->RemoveErrorEventHandler();
}


////////////////////////////////////////////////////////////////
/// Embedded Window Display

bool CCLUVizApp::EW_Display(bool bWait)
{
	if (!m_bIsCreated || !m_bIsEmbedded)
	{
		return true;
	}

	m_poglWin->EW_Display(bWait);

	return true;
}

////////////////////////////////////////////////////////////////
/// Embedded Window Reshape

bool CCLUVizApp::EW_Reshape(int iWidth, int iHeight)
{
	if (!m_bIsCreated || !m_bIsEmbedded)
	{
		return true;
	}

	m_poglWin->EW_Reshape(iWidth, iHeight);

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizApp::EW_GetShape(int& iWidth, int& iHeight)
{
	if (!m_bIsCreated || !m_bIsEmbedded)
	{
		return true;
	}

	m_poglWin->EW_GetShape(iWidth, iHeight);
	return true;
}

////////////////////////////////////////////////////////////////
/// Embedded Window Mouse Button

bool CCLUVizApp::EW_Mouse(int iButton, int iState, int iX, int iY)
{
	if (!m_bIsCreated || !m_bIsEmbedded)
	{
		return true;
	}

	m_poglWin->EW_Mouse(iButton, iState, iX, iY);

	return true;
}

////////////////////////////////////////////////////////////////
/// Embedded Window Active Mouse Move

bool CCLUVizApp::EW_ActiveMouseMove(int iX, int iY)
{
	if (!m_bIsCreated || !m_bIsEmbedded)
	{
		return true;
	}

	m_poglWin->EW_ActiveMouseMove(iX, iY);

	return true;
}

////////////////////////////////////////////////////////////////
/// Embedded Window Passive Mouse Move

bool CCLUVizApp::EW_PassiveMouseMove(int iX, int iY)
{
	if (!m_bIsCreated || !m_bIsEmbedded)
	{
		return true;
	}

	m_poglWin->EW_PassiveMouseMove(iX, iY);

	return true;
}

////////////////////////////////////////////////////////////////
/// Embedded Window Idle Function

bool CCLUVizApp::EW_IdleFunc()
{
	if (!m_bIsCreated || !m_bIsEmbedded)
	{
		return true;
	}

	m_poglWin->EW_IdleFunc();

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/// Embedded Window Set the modifier key states

bool CCLUVizApp::EW_SetKeyModifierStates(bool bShift, bool bCtrl, bool bAlt, bool bNumLock, bool bCapsLock, bool bScrollLock)
{
	if (!m_bIsCreated || !m_bIsEmbedded)
	{
		return true;
	}

	m_poglWin->EW_SetKeyModifierStates(bShift, bCtrl, bAlt, bNumLock, bCapsLock, bScrollLock);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/// Embedded Window Keyboard input

bool CCLUVizApp::EW_Keyboard(int iKey, bool bKeyDown)
{
	if (!m_bIsCreated || !m_bIsEmbedded)
	{
		return true;
	}

	m_poglWin->Keyboard(iKey, bKeyDown);

	return true;
}

////////////////////////////////////////////////////////////////
/// Embedded Window Load Script

bool CCLUVizApp::EW_LoadScript(const char* pcFilename, const char* pcPass)
{
	if (!m_bIsCreated || !m_bIsEmbedded)
	{
		m_sLastError = "Application isn't created nor embedded";
		return false;
	}

	if (!m_poglWin->EW_LoadScript(pcFilename, pcPass))
	{
		m_sLastError = m_poglWin->GetError();
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////
/// Embedded Window Set Script

bool CCLUVizApp::EW_SetScript(const char* pcScript, bool bResetEnv, bool bReparseLatex)
{
	if (!m_bIsCreated || !m_bIsEmbedded)
	{
		return false;
	}

	return m_poglWin->EW_SetScript(pcScript, bResetEnv, bReparseLatex);
}

////////////////////////////////////////////////////////////////
/// Embedded Window Exec User

bool CCLUVizApp::EW_ExecUser(const char* pcToolName)
{
	if (!m_bIsCreated || !m_bIsEmbedded)
	{
		return false;
	}

	if (!m_poglWin->EW_ExecUser(pcToolName))
	{
		m_sLastError = m_poglWin->GetError();
		m_poglWin->ClearError();
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////
/// Lock Visualization

bool CCLUVizApp::EW_LockVis(bool bLock)
{
	if (!m_bIsCreated || !m_bIsEmbedded)
	{
		return false;
	}

	return m_poglWin->EW_LockVis(bLock);
}

////////////////////////////////////////////////////////////////
// Shared RC Functions

bool CCLUVizApp::EW_MakeCurrent(bool bBind)
{
	if (!m_bIsCreated || !m_bIsEmbedded)
	{
		m_sLastError = "No RC created or not embedded";
		return false;
	}

	return m_poglWin->EW_MakeCurrent(bBind);
}

////////////////////////////////////////////////////////////////
/// Get Script Output

bool CCLUVizApp::GetScriptOutput(std::vector<std::string>& vecText, bool& bIsError)
{
	if (!m_bIsCreated)
	{
		return false;
	}

	return m_poglWin->GetScriptOutput(vecText, bIsError);
}

////////////////////////////////////////////////////////////////
/// Enable Visualization

bool CCLUVizApp::EnableVisualization(bool bVal)
{
	if (!m_bOGLWinValid)
	{
		m_sLastError = "Visualization window does not exist.";
		return false;
	}

	m_poglWin->EnableVisualization(bVal);

	return true;
}

////////////////////////////////////////////////////////////////
/// Enable Drawing of Scene Graph
bool CCLUVizApp::EnableDrawSceneGraph(bool bVal, bool bDoRedisplay)
{
	if (!m_bOGLWinValid)
	{
		m_sLastError = "Visualization window does not exist.";
		return false;
	}

	m_poglWin->EnableDrawSceneGraph(bVal, bDoRedisplay);

	return true;
}

////////////////////////////////////////////////////////////////
/// Create CLUViz Window Thread

bool CCLUVizApp::Create(int iX,
	int iY,
	int iWidth,
	int iHeight,
	const char* pcTitle,
	const char* pcInitScript,
	const char* pcEnvPath,
	bool bVizOnly,
	bool bShowIntro,
	void* pvIcon,
	void* pvParentHandle,
	unsigned int uWindowStyle,
	const char* pcLicensee)
{
	if (m_bIsCreated)
	{
		m_sLastError = "Cannot create application more than once.";
		return false;
	}

	if (pcEnvPath)
	{
		m_sEnvPath = pcEnvPath;
	}
	else
	{
		m_sEnvPath = "";
	}

	if (pcTitle)
	{
		m_sTitle = pcTitle;
	}
	else
	{
		m_sTitle = "";
	}

	if (pcInitScript)
	{
		m_sInitScript = pcInitScript;
	}
	else
	{
		m_sInitScript = "";
	}

	if (pcLicensee)
	{
		m_sLicensee = pcLicensee;
	}

	m_pvIcon = pvIcon;

	m_bShowIntro = bShowIntro;
	m_bVizOnly = bVizOnly;
	m_iX = iX;
	m_iY = iY;
	m_iWidth = iWidth;
	m_iHeight = iHeight;
	m_pvParentHandle = pvParentHandle;
	m_uWindowStyle = uWindowStyle;

	if (!Init())
	{
		return false;
	}

	m_bIsCreated = true;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Destroy

bool CCLUVizApp::Destroy()
{
	if (!m_bIsCreated)
	{
		return true;
	}

	if (m_bOGLWinValid)
	{
		m_poglWin->Quit();
	}

#ifdef WIN32
	if (m_pvIcon)
	{
		DestroyIcon((HICON)m_pvIcon);
	}
#endif

	m_pOutput = 0;
	m_pEditor = 0;
	m_poglWin = 0;

	m_bIsCreated = false;

	return true;
}

//////////////////////////////////////////////////////////////////////77
/// Init Application

bool CCLUVizApp::Init()
{
	/// Initial Script
	const char* pcIntro =
		"// Press CTRL+n to start a new script.\n\n"
		"/*\n\n"
		"Written and conceived by Dr. Christian Perwass.\n"
		"*/\n"
		"DefVarsN3();\n\n"
		"\"Change to mouse mode 1 and move the red sphere\";\n"
		"\"by holding down the right mouse button and\";\n"
		"\"moving the mouse in the visualization window.\";\n\n"
		"\" \";\n"
		"\"Press CTRL + h in the editor window for more help.\";\n\n"
		":N3_IPNS;\n"
		":N3_SOLID;\n"
		":N3_SHOW_IMAG;\n"
		"EnableAntiAliasing( true );\n"
		"SetSphereDetailLevel( 10 );\n"
		"EnableStdCtrl( false );\n"
		"EnableMenu( false );\n"
		"\n"
		":MRed;\n"
		":Sr = VecN3(VecE3(1) + VecE3(-0.12,0,0.2)) - 0.5*e;\n"
		"\n"
		":Color(0, 0, 0.6, 0.6);\n"
		":Sb = VecN3(0.5, 0, 0) - 0.5*e;\n"
		"\n"
		":Color(0, 0.6, 0, 0.6);\n"
		":Sg = VecN3(-0.5, 0.5, 0) - 0.5*e;\n"
		"\n"
		":Color(0.540, 0.991, 1.000);\n"
		"SetTextMagStep(14);\n"
		"SetImgPos( -1.5, 0.3, 1.5 );\n"
		":GetTextImg( \"{\\bf CLU}{\\it Viz} v" CLUVIZ_VERSION "\" );\n"
		"\n"
		":Magenta;\n"
		":C = Sr^Sb;\n"
		"\n"
		":Cyan;\n"
		":C = Sr^Sg;\n"
		"\n"
		":Yellow;\n"
		":PP = Sr^Sb^Sg;\n\n"
		":VecC2(0,1)^VecC2(1,0)^VecC2(0,-1)^VecC2(-1,0)^VecC2(1.5,0.2);\n\n"
		"\n";

	if (m_sInitScript.size())
	{
		pcIntro = m_sInitScript.c_str();
	}

	if (!m_bShowIntro)
	{
		pcIntro = "// Write your CLUScript here...\n";
	}

	int iScreenWidth = Fl::w();
	int iScreenHeight = Fl::h();

	int iX, iY, iW, iH;

	//#ifdef WIN32
	//	if ( !m_pvIcon )
	//	{
	//		m_pvIcon = (void *) LoadIconA(GetModuleHandleA("CLUViz.dll"), MAKEINTRESOURCEA(IDI_ICON1) );
	//	}
	//#endif
	//
	iX = int(float(iScreenWidth) * 0.01f);
	iY = int(float(iScreenHeight) * 0.03f);
	iW = int((iScreenWidth - 2 * iX) / 2.5);
	iH = int(float(iScreenHeight - 2 * iY) * 0.7f);
	//	iW = iScreenWidth / 2.5;
	//	iH = int( float(iScreenHeight) * 0.7f );

	CStrMem csBaseTitle, csTitle;
	csBaseTitle = "CLUViz v";
	csBaseTitle << CLUVIZ_VERSION;
	csBaseTitle << " by C. Perwass";

	char pcEnvPath[500], *pcScript = 0;

	if (m_sEnvPath.length())
	{
		int iPos;

		strncpy_s(pcEnvPath, 500, m_sEnvPath.c_str(), 499);
		if (pcEnvPath[iPos = int(strlen(pcEnvPath) - 1)] == '\\')
		{
			pcEnvPath[iPos] = 0;
		}
	}
	else
	{
		strcpy_s(pcEnvPath, 500, "");
	}

	int iVal;

	if (m_bShowIntro)
	{
		iVal = iW / 3;
	}
	else
	{
		iVal = iW;
	}

	Fl_Tooltip::size(12);

	csTitle = csBaseTitle + " - Output";
	if (m_bVizOnly)
	{
		if (!(m_pOutput = new CCLUOutput(-100, 0, 1, 1, " ", 0)))
		{
			m_sLastError = "Cannot create output window. Probably not enough memory.";
			return false;
		}
	}
	else
	{
		if (!(m_pOutput = new CCLUOutput(iX - 3, iY + iH + 40, iW, int(float(iH) / 0.7f * 0.3f) - 30, csTitle.Str(), m_pvIcon)))
		{
			m_sLastError = "Cannot create output window. Probably not enough memory.";
			return false;
		}
	}

	if (!(m_poglWin = new COGLWin))
	{
		m_sLastError = "Cannot create OpenGL window. Probably not enough memory.";
		delete m_pOutput;
		return false;
	}

	m_bOGLWinValid = true;
	m_poglWin->SetIsValidPtr(&m_bOGLWinValid);

	m_poglWin->SetLicensee(m_sLicensee.c_str());

	if (m_sTitle.size())
	{
		m_poglWin->SetFixedTitle(m_sTitle.c_str());
	}

	if (m_bVizOnly)
	{
		m_poglWin->EnableVizOnlyMode();
	}

	m_poglWin->SetEnvPath(pcEnvPath);

	csTitle = csBaseTitle + " - Visualization";
	if (m_bVizOnly)
	{
		if (!m_poglWin->CreateWindow(csTitle.Str(),
			m_iX, m_iY, m_iWidth, m_iHeight,
			//19, iY+11,
			//iScreenWidth - 9, iScreenHeight - iY - 11,
			m_pvIcon, FL_DOUBLE | FL_RGB | FL_DEPTH | FL_MULTISAMPLE,
			m_pvParentHandle, m_uWindowStyle))
		{
			m_sLastError = "Cannot initialize OpenGL window. Please ensure that your graphics driver is set to 32bit color depth.";
			delete m_pOutput;
			delete m_poglWin;
			return false;
		}

		iX = -100;
		iY = 0;
		iW = 1;
		iH = 1;
	}
	else
	{
		if (!m_poglWin->CreateWindow(csTitle.Str(),
			m_iX, m_iY, m_iWidth, m_iHeight,
			//19, iY+11,
			//iScreenWidth - 9, iScreenHeight - iY - 11,
			m_pvIcon, FL_DOUBLE | FL_RGB | FL_DEPTH | FL_MULTISAMPLE,
			m_pvParentHandle, m_uWindowStyle))
		{
			m_sLastError = "Cannot initialize OpenGL window. Please ensure that your graphics driver is set to 32bit color depth.";
			delete m_pOutput;
			delete m_poglWin;
			return false;
		}

		//m_poglWin->CreateWindow(csTitle.Str(), iX + iVal + 18, iY + 10,
		//						iScreenWidth - iVal - 18, int(float(iH) / 0.7f),
		//						m_pvIcon, FL_DOUBLE | FL_RGB | FL_DEPTH | FL_MULTISAMPLE,
		//						m_pvParentHandle );

		iX -= 3;
		iY += 10;
	}

	m_poglWin->SetOutputWin(m_pOutput);
	m_poglWin->SetVersion(CLUVIZ_VERSION_MAJOR, CLUVIZ_VERSION_MINOR, CLUVIZ_VERSION_REVISION);

	if (pcScript)
	{
		if (!(m_pEditor = new CCLUScriptEditor(iX, iY, iW, iH, pcScript, pcEnvPath, m_pvIcon)))
		{
			m_sLastError = "Cannot create editor window. Probably not enough memory.";
			delete m_pOutput;
			delete m_poglWin;
			return false;
		}

		if (!m_pEditor->IsOK())
		{
			m_sLastError = "Error initializing parser.";
			delete m_pOutput;
			delete m_poglWin;
			delete m_pEditor;
			return false;
		}

		m_pEditor->SetOGLWin(m_poglWin);
		m_pEditor->SetOutputWin(m_pOutput);

		m_pOutput->SetOGLWin(m_poglWin);
		m_pOutput->SetEditorWin(m_pEditor);

		m_poglWin->SetEditor(m_pEditor);

		m_pEditor->Parse(true);
	}
	else
	{
		CStrMem csFilename;
		//struct _stat FileStat;

		char cCurrentPath[FILENAME_MAX];
		_getcwd(cCurrentPath, sizeof(cCurrentPath) / sizeof(TCHAR));

		_chdir(pcEnvPath);

		pcScript = 0;

		if (!(m_pEditor = new CCLUScriptEditor(iX, iY, iW, iH, pcScript, pcEnvPath, m_pvIcon)))
		{
			m_sLastError = "Cannot create editor window. Probably not enough memory.";
			delete m_pOutput;
			delete m_poglWin;
			_chdir(cCurrentPath);
			return false;
		}

		_chdir(cCurrentPath);

		if (!m_pEditor->IsOK())
		{
			m_sLastError = "Error initializing parser.";
			delete m_pOutput;
			delete m_poglWin;
			delete m_pEditor;
			return false;
		}

		m_pEditor->SetOGLWin(m_poglWin);
		m_pEditor->SetOutputWin(m_pOutput);

		m_pOutput->SetOGLWin(m_poglWin);
		m_pOutput->SetEditorWin(m_pEditor);

		m_poglWin->SetEditor(m_pEditor);

		m_pEditor->SetScript(pcIntro, true);
	}

	if (m_bVizOnly)
	{
		m_pOutput->hide();
		m_pEditor->hide();
	}
	else
	{
		m_pOutput->Show();
		m_pEditor->Show();
	}

	m_poglWin->ShowWindow();

	return true;
}

////////////////////////////////////////////////////////////////
/// Show Window

bool CCLUVizApp::ShowWindow()
{
	if (!m_bOGLWinValid)
	{
		m_sLastError = "Visualization window does not exist.";
		return false;
	}

	m_poglWin->ShowWindow();

	return true;
}

////////////////////////////////////////////////////////////////
/// Show Window

bool CCLUVizApp::HideWindow()
{
	if (!m_bOGLWinValid)
	{
		m_sLastError = "Visualization window does not exist.";
		return false;
	}

	m_poglWin->HideWindow();

	return true;
}

////////////////////////////////////////////////////////////////
/// Set Window Position and Size

bool CCLUVizApp::SetWindowPosSize(int iX, int iY, int iWidth, int iHeight)
{
	if (!m_bOGLWinValid)
	{
		m_sLastError = "Visualization window does not exist.";
		return false;
	}

	m_poglWin->SetWindowPosSize(iX, iY, iWidth, iHeight);

	return true;
}

////////////////////////////////////////////////////////////////
/// Save View

bool CCLUVizApp::SaveView(const char* pcFilename)
{
	if (!m_bOGLWinValid)
	{
		m_sLastError = "Visualization window does not exist.";
		return false;
	}

	return m_poglWin->SaveView(pcFilename);
}

////////////////////////////////////////////////////////////////
/// Show Editor

bool CCLUVizApp::ShowEditor()
{
	if (!m_bOGLWinValid)
	{
		m_sLastError = "Editor window does not exist.";
		return false;
	}

	m_pEditor->Show();

	return true;
}

////////////////////////////////////////////////////////////////
/// Hide Editor

bool CCLUVizApp::HideEditor()
{
	if (!m_bOGLWinValid)
	{
		m_sLastError = "Editor window does not exist.";
		return false;
	}

	m_pEditor->Hide();

	return true;
}

////////////////////////////////////////////////////////////////
/// Set Script

bool CCLUVizApp::SetScript(const char* pcScript)
{
	if (!m_bOGLWinValid)
	{
		m_sLastError = "Visualization window does not exist.";
		return false;
	}

	if (!pcScript)
	{
		m_sLastError = "Invalid pointer to script.";
		return false;
	}

	m_poglWin->SetScript(pcScript);
	//glutPostRedisplay();

	return true;
}

////////////////////////////////////////////////////////////////
/// Load Script

bool CCLUVizApp::LoadScript(const char* pcFilename, const char* pcPass)
{
	if (!m_bOGLWinValid)
	{
		m_sLastError = "Visualization window does not exist.";
		return false;
	}

	if (!pcFilename)
	{
		m_sLastError = "Invalid pointer to filename.";
		return false;
	}

	/*
	if ( !m_poglWin->LoadScript( pcFilename ) )
	{
	m_sLastError = "Error loading file.";
	return false;
	}
	*/

	m_pEditor->LoadFileReplace(pcFilename, -1, pcPass);
	m_pEditor->Parse(true);

	return true;
}

////////////////////////////////////////////////////////////////
/// Load Script with arguments

bool CCLUVizApp::LoadScript(const char* pcFilename, const char* pcPass, int iArgCnt, const char* pcArg)
{
	if (!m_bOGLWinValid)
	{
		m_sLastError = "Visualization window does not exist.";
		return false;
	}

	if (!pcFilename)
	{
		m_sLastError = "Invalid pointer to filename.";
		return false;
	}

	/*
	if ( !m_poglWin->LoadScript( pcFilename ) )
	{
	m_sLastError = "Error loading file.";
	return false;
	}
	*/

	m_pEditor->LoadFileReplace(pcFilename, -1, pcPass);
	m_pEditor->Parse(true, false, 0, 0, iArgCnt, pcArg);

	return true;
}

////////////////////////////////////////////////////////////////
/// Exec Script

bool CCLUVizApp::ExecUser(const char* pcToolName)
{
	if (!m_bOGLWinValid)
	{
		m_sLastError = "Visualization window does not exist.";
		return false;
	}

	m_poglWin->ExecUser(pcToolName);

	return true;
}

////////////////////////////////////////////////////////////////
/// Full Screen

bool CCLUVizApp::FullScreen(bool bVal, bool bEnableUserToggle)
{
	if (!m_bOGLWinValid)
	{
		m_sLastError = "Visualization window does not exist.";
		return false;
	}

	m_poglWin->EnableUserToggleFullScreen(bEnableUserToggle);
	m_poglWin->EnableFullScreen(bVal);

	return true;
}

bool CCLUVizApp::EnableBorder(bool bVal)
{
	if (!m_bOGLWinValid)
	{
		m_sLastError = "Visualization window does not exist.";
		return false;
	}

	m_poglWin->EnableBorder(bVal);

	return true;
}

////////////////////////////////////////////////////////////////
/// Set Variable Number Int

bool CCLUVizApp::SetVar(const char* pcVarName, int iVal)
{
	if (!m_bOGLWinValid)
	{
		m_sLastError = "Visualization window does not exist.";
		return false;
	}

	if (!pcVarName)
	{
		m_sLastError = "Invalid pointer to variable name.";
		return false;
	}

	m_poglWin->LockVis();

	try
	{
		// try to get variable Pointer
		CCodeVar* pVar = &(m_poglWin->m_xParse.GetVar(pcVarName));

		//if we have a pointer type variable, try to dereference it and go on.
		if (pVar->BaseType() == PDT_PTR_VAR)
		{
			pVar = &pVar->DereferenceVarPtr(true);
		}

		// if variable is of type scene...
		if (pVar->BaseType() == PDT_SCENE)
		{
			// we try to cast it to a scene object
			TScene& rScene = *(pVar->GetScenePtr());

			// this is not a valid scene ? Add an output message and return
			if (!rScene.IsValid())
			{
				m_sLastError = "Given texture variable is invalid.";
				m_poglWin->UnlockVis();
				return false;
			}

			// having a valid scene, we try to a cast to a Texture object
			COGLTexture* pTex = dynamic_cast<COGLTexture*>((COGLBaseElement*)rScene);
			COGLVertexList* pVbo = dynamic_cast<COGLVertexList*>((COGLBaseElement*)rScene);

			// this is not a texture object ? give error message and return
			if (pTex)
			{
				//pass the argument as a new Texture ID to the texture object
				pTex->GenTexture2DExternal(iVal);
			}
			else if (pVbo)
			{
				pVbo->SetVertexBufferID(iVal);
			}
			else
			{
				m_sLastError = "Given variable is invalid. (Not a texture and not a VBO)";
				m_poglWin->UnlockVis();
				return false;
			}
		}
		else
		{
			CCodeVar& rVar = CreateCodeVar(pcVarName, PDT_INT);
			rVar = TCVCounter(iVal);
		}
	}
	catch (Clu::CIException& ex)
	{
		m_sLastError = ex.ToString().ToCString();
		m_poglWin->UnlockVis();
		return false;
	}
	catch (...)
	{
		m_sLastError = "Unexpected error in creation of tensor variable.";
		m_poglWin->UnlockVis();
		return false;
	}

	m_poglWin->UnlockVis();
	return true;
}

////////////////////////////////////////////////////////////////
/// Get Variable Number Int

bool CCLUVizApp::GetVar(const char* pcVarName, int& iVal)
{
	if (!m_bOGLWinValid)
	{
		m_sLastError = "Visualization window does not exist.";
		return false;
	}

	if (!pcVarName)
	{
		m_sLastError = "Invalid pointer to variable name.";
		return false;
	}

	m_poglWin->LockVis();
	CCodeVar& rVar = m_poglWin->m_xParse.GetVar(pcVarName);

	TCVCounter iTheVal;
	if (rVar.BaseType() == PDT_SCENE)
	{
		TScene& rScene = *rVar.GetScenePtr();

		if (!rScene.IsValid())
		{
			m_sLastError = "Given texture variable is invalid.";
			m_poglWin->UnlockVis();
			return false;
		}

		COGLTexture* pTex = dynamic_cast<COGLTexture*>((COGLBaseElement*)rScene);
		COGLVertexList* pVbo = dynamic_cast<COGLVertexList*>((COGLBaseElement*)rScene);

		if (pTex)
		{
			iTheVal = TCVCounter(pTex->GetTextureID());
		}
		else if (pVbo)
		{
			iTheVal = TCVCounter(pVbo->GetVertexBufferID());
			//do something
		}
		else
		{
			m_sLastError = "Given variable is neither a valid Texture nor a valid VBO Object.";
			m_poglWin->UnlockVis();
			return false;
		}
	}
	else if (!rVar.CastToCounter(iTheVal, false))
	{
		m_sLastError = "Variable is not a number or is missing in script code.";
		m_poglWin->UnlockVis();
		return false;
	}

	m_poglWin->UnlockVis();

	iVal = int(iTheVal);

	return true;
}

////////////////////////////////////////////////////////////////
/// Set Variable Number

bool CCLUVizApp::SetVar(const char* pcVarName, double dVal)
{
	if (!m_bOGLWinValid)
	{
		m_sLastError = "Visualization window does not exist.";
		return false;
	}

	if (!pcVarName)
	{
		m_sLastError = "Invalid pointer to variable name.";
		return false;
	}

	m_poglWin->LockVis();

	CCodeVar& rVar = CreateCodeVar(pcVarName, PDT_SCALAR);
	rVar = TCVScalar(dVal);

	m_poglWin->UnlockVis();

	return true;
}

////////////////////////////////////////////////////////////////
/// Get Variable Number

bool CCLUVizApp::GetVar(const char* pcVarName, double& dVal)
{
	if (!m_bOGLWinValid)
	{
		m_sLastError = "Visualization window does not exist.";
		return false;
	}

	if (!pcVarName)
	{
		m_sLastError = "Invalid pointer to variable name.";
		return false;
	}

	m_poglWin->LockVis();

	CCodeVar& rVar = m_poglWin->m_xParse.GetVar(pcVarName);

	TCVScalar dTheVal;
	if (!rVar.CastToScalar(dTheVal))
	{
		m_sLastError = "Variable is not a number or is missing in script code.";
		m_poglWin->UnlockVis();
		return false;
	}

	m_poglWin->UnlockVis();

	dVal = double(dTheVal);

	return true;
}

////////////////////////////////////////////////////////////////
/// Set Variable String

bool CCLUVizApp::SetVar(const char* pcVarName, const string& sVal)
{
	if (!m_bOGLWinValid)
	{
		m_sLastError = "Visualization window does not exist.";
		return false;
	}

	if (!pcVarName)
	{
		m_sLastError = "Invalid pointer to variable name.";
		return false;
	}

	m_poglWin->LockVis();

	CCodeVar& rVar = CreateCodeVar(pcVarName, PDT_STRING);
	rVar = TString(sVal.c_str());

	m_poglWin->UnlockVis();

	return true;
}

////////////////////////////////////////////////////////////////
/// Get Variable Number

bool CCLUVizApp::GetVar(const char* pcVarName, string& sVal)
{
	if (!m_bOGLWinValid)
	{
		m_sLastError = "Visualization window does not exist.";
		return false;
	}

	if (!pcVarName)
	{
		m_sLastError = "Invalid pointer to variable name.";
		return false;
	}

	m_poglWin->LockVis();

	CCodeVar& rVar = m_poglWin->m_xParse.GetVar(pcVarName);

	if (rVar.BaseType() != PDT_STRING)
	{
		m_sLastError = "Variable is not a string or is missing in script code.";
		m_poglWin->UnlockVis();
		return false;
	}

	sVal = rVar.GetStringPtr()->Str();
	m_poglWin->UnlockVis();

	return true;
}

////////////////////////////////////////////////////////////////
/// Set String list
bool CCLUVizApp::SetVar(const char* pcVarName, const vector<string>& vStrings)
{
	size_t iRowCnt = vStrings.size();
	if (!m_bOGLWinValid)
	{
		m_sLastError = "Visualization window does not exist.";
		return false;
	}

	if (!pcVarName)
	{
		m_sLastError = "Invalid pointer to variable name.";
		return false;
	}

	if (iRowCnt <= 0)
	{
		m_sLastError = "Invalid row count.";
		return false;
	}

	m_poglWin->LockVis();

	// create a new code var
	CCodeVar& rVar = CreateCodeVar(pcVarName, PDT_VARLIST);
	// initialize
	rVar.New(PDT_VARLIST, pcVarName);

	// get ptr
	TVarList& rL = *rVar.GetVarListPtr();
	// and set the size
	rL.Set(iRowCnt);

	try
	{
		for (int i = 0; i < (int)iRowCnt; i++)
		{
			TString sVar = TString(vStrings[i].c_str());

			rL(i) = sVar;
		}
	}
	catch (CCluException& rEx)
	{
		m_sLastError = rEx.PrintError();
		m_poglWin->UnlockVis();
		return false;
	}
	catch (...)
	{
		m_sLastError = "Unexpected error in creation of tensor variable.";
		m_poglWin->UnlockVis();
		return false;
	}

	m_poglWin->UnlockVis();
	return true;
}

bool CCLUVizApp::SetVar(const char* pcVarName, int iDimCnt, const int* piDim, const double* pdData)
{
	if (!m_bOGLWinValid)
	{
		m_sLastError = "Visualization window does not exist.";
		return false;
	}

	if (!pcVarName)
	{
		m_sLastError = "Invalid pointer to variable name.";
		return false;
	}

	if (iDimCnt <= 0)
	{
		m_sLastError = "Invalid dimension count.";
		return false;
	}

	m_poglWin->LockVis();

	CCodeVar& rVar = CreateCodeVar(pcVarName, PDT_TENSOR);
	TTensor& rT = *rVar.GetTensorPtr();

	Mem<int> mDim;

	mDim.Set(iDimCnt);
	memcpy(mDim.Data(), piDim, iDimCnt * sizeof(int));

	try
	{
		rT.Reset(mDim);
		memcpy(rT.Data(), pdData, rT.Size() * sizeof(double));
	}
	catch (CCluException& rEx)
	{
		m_sLastError = rEx.PrintError();
		m_poglWin->UnlockVis();
		return false;
	}
	catch (...)
	{
		m_sLastError = "Unexpected error in creation of tensor variable.";
		m_poglWin->UnlockVis();
		return false;
	}

	m_poglWin->UnlockVis();
	return true;
}

////////////////////////////////////////////////////////////////
/// Get Variable Tensor

bool CCLUVizApp::GetVar(const char* pcVarName, vector<int>& vecDim, vector<double>& vecData)
{
	if (!m_bOGLWinValid)
	{
		m_sLastError = "Visualization window does not exist.";
		return false;
	}

	if (!pcVarName)
	{
		m_sLastError = "Invalid pointer to variable name.";
		return false;
	}

	m_poglWin->LockVis();

	CCodeVar& rVar = m_poglWin->m_xParse.GetVar(pcVarName);

	if (rVar.BaseType() != PDT_TENSOR)
	{
		m_sLastError = "Variable is not a tensor or is missing in script code.";
		m_poglWin->UnlockVis();
		return false;
	}

	TTensor& rT = *rVar.GetTensorPtr();

	int i;
	int iValence = rT.Valence();
	int iSize = rT.Size();

	vecDim.resize(iValence);
	for (i = 0; i < iValence; ++i)
	{
		vecDim[i] = rT.DimSize(i);
	}

	vecData.resize(iSize);
	memcpy(&vecData[0], rT.Data(), iSize * sizeof(double));

	m_poglWin->UnlockVis();

	return true;
}

////////////////////////////////////////////////////////////////
// Get Tensor Properties

bool CCLUVizApp::GetVar(const char* pcVarName, unsigned& uDimCnt, unsigned& uElCnt)
{
	if (!m_bOGLWinValid)
	{
		m_sLastError = "Visualization window does not exist.";
		return false;
	}

	if (!pcVarName)
	{
		m_sLastError = "Invalid pointer to variable name.";
		return false;
	}

	m_poglWin->LockVis();

	CCodeVar& rVar = m_poglWin->m_xParse.GetVar(pcVarName);

	if (rVar.BaseType() != PDT_TENSOR)
	{
		m_sLastError = "Variable is not a tensor or is missing in script code.";
		m_poglWin->UnlockVis();
		return false;
	}

	TTensor& rT = *rVar.GetTensorPtr();

	uDimCnt = (unsigned)rT.Valence();
	uElCnt = (unsigned)rT.Size();

	m_poglWin->UnlockVis();

	return true;
}

/////////////////////////////////////////////////////////////////////
// Get Tensor Data

bool CCLUVizApp::GetVar(const char* pcVarName, unsigned* puDim, double* pdData)
{
	if (!m_bOGLWinValid)
	{
		m_sLastError = "Visualization window does not exist.";
		return false;
	}

	if (!pcVarName)
	{
		m_sLastError = "Invalid pointer to variable name.";
		return false;
	}

	m_poglWin->LockVis();

	CCodeVar& rVar = m_poglWin->m_xParse.GetVar(pcVarName);

	if (rVar.BaseType() != PDT_TENSOR)
	{
		m_sLastError = "Variable is not a tensor or is missing in script code.";
		m_poglWin->UnlockVis();
		return false;
	}

	TTensor& rT = *rVar.GetTensorPtr();

	unsigned uDimCnt = (unsigned)rT.Valence();
	unsigned uElCnt = (unsigned)rT.Size();

	unsigned uDim;

	for (uDim = 0; uDim < uDimCnt; ++uDim)
	{
		puDim[uDim] = unsigned(rT.DimSize(int(uDim)));
	}

	memcpy(pdData, rT.Data(), uElCnt * sizeof(double));

	m_poglWin->UnlockVis();

	return true;
}

////////////////////////////////////////////////////////////////
// Set Image Data

bool CCLUVizApp::SetVar(const char* pcVarName, const Clu::CIImage& xImage)
{
	if (!m_bOGLWinValid)
	{
		m_sLastError = "Visualization window does not exist.";
		return false;
	}

	if (!xImage.IsValid())
	{
		m_sLastError = "Invalid data pointer.";
		return false;
	}

	if (!pcVarName)
	{
		m_sLastError = "Invalid pointer to variable name.";
		return false;
	}

	m_poglWin->LockVis();

	CCodeVar& rVar = CreateCodeVar(pcVarName, PDT_IMAGE, false);
	TImage& rImg = *rVar.GetImagePtr();

	try
	{
		rImg->Create(xImage);
	}
	catch (Clu::CIException& xEx)
	{
		m_sLastError = xEx.ToStringComplete().ToCString();
		m_poglWin->UnlockVis();
		return false;
	}

	m_poglWin->UnlockVis();
	return true;
}

////////////////////////////////////////////////////////////////
/// Get data of image

bool CCLUVizApp::GetVar(const char* pcVarName, Clu::CIImage& xImage)
{
	if (!m_bOGLWinValid)
	{
		m_sLastError = "Visualization window does not exist.";
		return false;
	}

	if (!pcVarName)
	{
		m_sLastError = "Invalid pointer to variable name.";
		return false;
	}

	m_poglWin->LockVis();

	CCodeVar& rVar = m_poglWin->m_xParse.GetVar(pcVarName);

	if (rVar.BaseType() != PDT_IMAGE)
	{
		m_sLastError = "Variable is not an image or is missing in script code.";
		m_poglWin->UnlockVis();
		return false;
	}

	TImage& rImg = *rVar.GetImagePtr();

	if (!rImg.IsValid())
	{
		m_sLastError = "Invalid image variable.";
		m_poglWin->UnlockVis();
		return false;
	}

	try
	{
		xImage = rImg->ToIImage();
	}
	catch (Clu::CIException& xEx)
	{
		m_poglWin->UnlockVis();
		throw Clu::CIException("Error getting image.", __FILE__, __FUNCTION__, __LINE__, std::move(xEx));
	}

	m_poglWin->UnlockVis();
	return true;
}


////////////////////////////////////////////////////////////////
// Set Color Data

bool CCLUVizApp::SetVar(const char* pcVarName, double* pdColor)
{
	if (!m_bOGLWinValid)
	{
		m_sLastError = "Visualization window does not exist.";
		return false;
	}

	if (!pdColor)
	{
		m_sLastError = "Invalid data pointer.";
		return false;
	}

	if (!pcVarName)
	{
		m_sLastError = "Invalid pointer to variable name.";
		return false;
	}

	m_poglWin->LockVis();

	CCodeVar& rVar = CreateCodeVar(pcVarName, PDT_COLOR);
	TOGLColor& rCol = *rVar.GetOGLColorPtr();

	rCol.Set(float(pdColor[0]), float(pdColor[1]), float(pdColor[2]), float(pdColor[3]));

	m_poglWin->UnlockVis();

	return true;
}

////////////////////////////////////////////////////////////////
// Get Color Data

bool CCLUVizApp::GetVar(const char* pcVarName, double* pdColor)
{
	if (!m_bOGLWinValid)
	{
		m_sLastError = "Visualization window does not exist.";
		return false;
	}

	if (!pdColor)
	{
		m_sLastError = "Invalid data pointer.";
		return false;
	}

	if (!pcVarName)
	{
		m_sLastError = "Invalid pointer to variable name.";
		return false;
	}

	m_poglWin->LockVis();

	CCodeVar& rVar = m_poglWin->m_xParse.GetVar(pcVarName);

	if (rVar.BaseType() != PDT_COLOR)
	{
		m_sLastError = "Variable is not a color or is missing in script code.";
		m_poglWin->UnlockVis();
		return false;
	}

	TOGLColor& rCol = *rVar.GetOGLColorPtr();

	float* pfCol = rCol.Data();
	pdColor[0] = double(pfCol[0]);
	pdColor[1] = double(pfCol[1]);
	pdColor[2] = double(pfCol[2]);
	pdColor[3] = double(pfCol[3]);

	m_poglWin->UnlockVis();
	return true;
}

////////////////////////////////////////////////////////////////
// Set vertex array data
bool CCLUVizApp::SetVar(const char* pcVarName, const Clu::SStdVbo* pVexArray)
{
	if (!m_bOGLWinValid)
	{
		m_sLastError = "Visualization window does not exist.";
		return false;
	}

	if (!pVexArray)
	{
		m_sLastError = "Vertex array pointer is NULL";
		return false;
	}

	if (pVexArray->iVertexCount <= 0)
	{
		m_sLastError = "No data available.";
		return false;
	}

	//we allow null pointer Data, if there is a valid vbo ID
	if (!pVexArray->pxVertices && (pVexArray->uVertexBufferID == 0))
	{
		m_sLastError = "Invalid data pointer.";
		return false;
	}

	if (!pcVarName)
	{
		m_sLastError = "Invalid pointer to variable name.";
		return false;
	}

	m_poglWin->LockVis();

	COGLVertexList* pVexList = 0;
	CCodeVar& rVar = m_poglWin->m_xParse.GetVar(pcVarName);

	if (rVar.BaseType() == PDT_SCENE)
	{
		TScene& rScene = *rVar.GetScenePtr();

		if (!rScene.IsValid())
		{
			m_sLastError = "Given object variable '" + std::string(pcVarName) + "' is invalid.";
			m_poglWin->UnlockVis();
			return false;
		}

		pVexList = dynamic_cast<COGLVertexList*>((COGLBaseElement*)rScene);
		if (!pVexList)
		{
			m_sLastError = "Given object variable '" + std::string(pcVarName) + "' is invalid.";
			m_poglWin->UnlockVis();
			return false;
		}
	}
	else
	{
		m_sLastError = "Variable '" + std::string(pcVarName) + "' is not an object or is missing in script code.";
		m_poglWin->UnlockVis();
		return false;
	}

	pVexList->Reset();

	try
	{
		pVexList->ApplyVertexData((GLenum)pVexArray->eMode, pVexArray->iVertexCount, (const COGLVertexList::SData*) pVexArray->pxVertices,
			pVexArray->bUseVertex, pVexArray->bUseTexture, pVexArray->bUseNormal,
			pVexArray->bUseColor, pVexArray->bUseFogZ, pVexArray->bUseEdge, pVexArray->bUsePartId, pVexArray->uVertexBufferID);
	}
	catch (Clu::CIException& ex)
	{
		m_poglWin->UnlockVis();
		throw CLU_EXCEPTION_NEST(__FUNCTION__, std::move(ex));
	}

	// Set index lists if available
	if (pVexArray->uIndexListCount > 0)
	{
		if (!pVexList->SetIndexLists((const unsigned** const)pVexArray->ppuIndexLists, pVexArray->puIndexListLengths, pVexArray->uIndexListCount,
			pVexArray->uIndexBufferID))
		{
			m_sLastError = "Error setting index list data";
			m_poglWin->UnlockVis();
			return false;
		}
	}

	m_poglWin->UnlockVis();

	return true;
}



bool CCLUVizApp::SetVar(const char* pcVarName, Clu::CIDataContainer xData)
{
	bool bSet = false;
	Clu::CIString sName;
	Clu::SDataObject xObject;

	if (!m_bOGLWinValid)
	{
		m_sLastError = "Visualization window does not exist.";
		return false;
	}

	if (!pcVarName)
	{
		m_sLastError = "Invalid pointer to variable name.";
		return false;
	}

	if (xData.First(sName, xObject))
	{
		do
		{
			if (xObject.guidType == TMesh::TypeId)
			{
				bSet = _SetVarMesh(pcVarName, *((TMesh *)xObject.pData));
			}
		} 
		while (!bSet && xData.Next(sName, xObject));
	}

	if (!bSet)
	{
		m_sLastError = "No element in data container could be set to variable";
	}

	return bSet;
}


bool CCLUVizApp::_SetVarMesh(const char* pcVarName, const TMesh& xMesh)
{
	m_poglWin->LockVis();

	COGLVertexList* pVexList = 0;
	CCodeVar& rVar = m_poglWin->m_xParse.GetVar(pcVarName);

	if (rVar.BaseType() == PDT_SCENE)
	{
		TScene& rScene = *rVar.GetScenePtr();

		if (!rScene.IsValid())
		{
			m_sLastError = "Given object variable '" + std::string(pcVarName) + "' is invalid.";
			m_poglWin->UnlockVis();
			return false;
		}

		pVexList = dynamic_cast<COGLVertexList*>((COGLBaseElement*)rScene);
		if (!pVexList)
		{
			m_sLastError = "Given object variable '" + std::string(pcVarName) + "' is invalid.";
			m_poglWin->UnlockVis();
			return false;
		}
	}
	else
	{
		m_sLastError = "Variable '" + std::string(pcVarName) + "' is not an object or is missing in script code.";
		m_poglWin->UnlockVis();
		return false;
	}

	pVexList->Reset();

	try
	{
		using namespace Clu::OpenGL::Vertex;

		const TMesh::TObjectData &xObjData = xMesh.Data();
		const TMesh::TDrawConfig &xDrawCfg = xMesh.DrawConfig();

		pVexList->ApplyVertexData((GLenum)xDrawCfg.Mode(), xObjData.ElementCount(), nullptr
			, xObjData.IsEnabled(EAttribute::Position)
			, xObjData.IsEnabled(EAttribute::Tex0)
			, xObjData.IsEnabled(EAttribute::Normal)
			, xObjData.IsEnabled(EAttribute::Color)
			, false
			, false
			, xObjData.IsEnabled(EAttribute::PartId)
			, xObjData.BufferId());

		unsigned uIndexCount = (unsigned)xDrawCfg.ElementCount();
		pVexList->SetIndexLists(nullptr, &uIndexCount, 1, xDrawCfg.BufferId());
	}
	catch (Clu::CIException& ex)
	{
		m_poglWin->UnlockVis();
		throw CLU_EXCEPTION_NEST(__FUNCTION__, std::move(ex));
	}


	m_poglWin->UnlockVis();

	return true;

}
