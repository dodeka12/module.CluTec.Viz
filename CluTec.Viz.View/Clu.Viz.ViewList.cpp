#include "StdAfx.h"
#include "Clu.Viz.ViewList.h"

#include "CluTec.Viz.OpenGL.Extensions\Extensions.h"
#include "CluTec.Viz.OpenGL/Api.h"
#include "CluTec.Base/Exception.h"

#include "IL\il.h"
#include "IL\ilu.h"
#include "IL\ilut.h"

#include <time.h>

#ifdef CV_USE_DEVICE_3DX
	#include "CluTec.Viz.View.Base\Device_3DX.cxx"
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCLUVizAppListST::CCLUVizAppListST(bool bInitAll)
{
	m_iCurHandle = 0;
	m_bIsRunning = false;

	m_hMutexAccess = CreateMutex(NULL, FALSE, NULL);

	m_bInitAll = bInitAll;

	if (m_bInitAll)
	{

		// Initialize Image Repository
		::InitImageRepository(new CImageRepository);


		// Initialize GA library
		InitCLUGA();


		// Initialize Image Library
		ilInit();
		iluInit();
		ilutInit();

	}

	m_pCLUCodeBase = new CCLUCodeBase();

	_time32((__time32_t*) &m_iRanSeed);

	m_pDevice3DX = nullptr;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCLUVizAppListST::~CCLUVizAppListST()
{
	ReleaseAll();
	End();

	CloseHandle(m_hMutexAccess);

	if (m_bInitAll)
	{
		FinalizeCLUGA();
		::ReleaseImageRepository();
		ilShutDown();
	}

	if (m_pCLUCodeBase)
	{
		delete m_pCLUCodeBase;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::Lock(int iWait)
{
	if (WaitForSingleObject(m_hMutexAccess, iWait) == WAIT_TIMEOUT)
	{
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::Unlock()
{
	ReleaseMutex(m_hMutexAccess);
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::Start(const char* pcEnvPath, const char* pcModulePath)
{
	Lock();
	if (m_bIsRunning)
	{
		m_sLastError = "Application is already running.";
		Unlock();
		return false;
	}

	if (!pcEnvPath)
	{
		m_sLastError = "No environment path given.";
		Unlock();
		return false;
	}

	if (!pcModulePath)
	{
		m_sLastError = "No module path given.";
		return false;
	}


	m_sEnvPath     = std::string(pcEnvPath);
	m_sModulePath  = std::string(pcModulePath);


	// Load all modules
	if (!CCLUParse::LoadModules(m_sModulePath.c_str(), "CluViz.Plugin.*." CLUVIZ_MAJOR_MINOR))
	{
		m_sLastError = "Error loading function modules in '" + m_sModulePath + "' " "CluViz.Plugin.*." CLUVIZ_MAJOR_MINOR;
		Unlock();
		return false;
	}

	InitViz();

	m_bIsRunning = true;

	Unlock();
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::End()
{
	if (!m_bIsRunning)
	{
		return true;
	}

	Lock();

	FinalizeViz();

	// Delete all fonts used by FL
	Fl::reset_fonts();

	// Only Release module after all member variables of OGLWin have been
	// deleted, since this also includes the Base Element Repository.
	CCLUParse::ReleaseModules();

	m_bIsRunning = false;

	Unlock();
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string CCLUVizAppListST::GetLastError()
{
	string sText;
	Lock();
	sText = m_sLastError;
	Unlock();

	return sText;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::SetScriptPath(const char* pcPath)
{
	if (!pcPath)
	{
		return false;
	}

	m_sScriptPath = pcPath;
	if (m_sScriptPath[m_sScriptPath.length() - 1] != '\\')
	{
		m_sScriptPath += "\\";
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::InitViz()
{
	Lock();
	bool bSuccess = false;

	try
	{
		if (!m_pDevice3DX)
		{
			#ifdef CV_USE_DEVICE_3DX
				m_pDevice3DX = new CDevice_3DX();
				m_pDevice3DX->Init();
				m_pDevice3DX->SetCallback_KeyDown(COGLWinFLTK::CB_Mouse3D_KeyDown);
				m_pDevice3DX->SetCallback_KeyUp(COGLWinFLTK::CB_Mouse3D_KeyUp);
				m_pDevice3DX->SetCallback_Move(COGLWinFLTK::CB_Mouse3D_Move);
			#endif
		}

		bSuccess = true;
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::FinalizeViz()
{
	Lock();
	bool bSuccess = false;
	try
	{
		if (m_pDevice3DX)
		{
			#ifdef CV_USE_DEVICE_3DX
				delete m_pDevice3DX;
			#endif
			m_pDevice3DX = 0;
		}

		bSuccess = true;
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::EmbedWindow(int& iHandle, HWND hWnd, bool bCreateSRC, HGLRC* phGlRc, bool bLoadIntroScript)
{
	Lock();
	bool bSuccess = false;

	if (!m_bIsRunning)
	{
		m_sLastError = "CLUViz is not running.";
		return false;
	}

	try
	{
		CCLUVizApp* pApp = new CCLUVizApp();
		if (!pApp->EmbedWindow(hWnd, m_sEnvPath.c_str(), bCreateSRC, phGlRc, bLoadIntroScript))
		{
			m_sLastError = pApp->GetLastError();
			delete pApp;
			throw CException();
		}

		m_mapApp[++m_iCurHandle] = pApp;

		iHandle  = m_iCurHandle;
		bSuccess = true;
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::ReleaseWindow(int iHandle)
{
	Lock();
	bool bSuccess = false;

	try
	{
		delete _GetApp(iHandle);

		m_mapApp.erase(m_mapApp.find(iHandle));
		bSuccess = true;
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCLUVizAppListST::ReleaseAll()
{
	Lock();

	try
	{
		for (const auto& kv : m_mapApp)
		{
			delete _GetApp(kv.first);
		}

		m_mapApp.clear();
	}
	catch (...)
	{
	}

	Unlock();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string CCLUVizAppListST::GetSceneGraphPrint(int iHandle)
{
	Lock();
	std::string sRes = "";

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);
		sRes = pApp->GetSceneGraphPrint();
	}
	catch (...)
	{
	}

	Unlock();
	return sRes;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::SetMouseEventHandler(int iHandle, Clu::Viz::View::TFuncMouseEventCallback pFunc, void* pvContext)
{
	Lock();
	bool bSuccess = false;

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);
		if (!(bSuccess = pApp->SetMouseEventHandler(pFunc, pvContext)))
		{
			m_sLastError = pApp->GetLastError();
		}
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::RemoveMouseEventHandler(int iHandle)
{
	Lock();
	bool bSuccess = false;

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);
		if (!(bSuccess = pApp->RemoveMouseEventHandler()))
		{
			m_sLastError = pApp->GetLastError();
		}
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::SetErrorEventHandler(int iHandle, Clu::Viz::View::TFuncErrorEventCallback pFunc, void* pvContext)
{
	Lock();
	bool bSuccess = false;

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);
		pApp->SetErrorEventHandler(pFunc, pvContext);
		bSuccess = true;
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::EW_Display(int iHandle, bool bWait)
{
	Lock();
	bool bSuccess = false;

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);
		if (!(bSuccess = pApp->EW_Display(bWait)))
		{
			m_sLastError = pApp->GetLastError();
		}
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::EW_Reshape(int iHandle, int iWidth, int iHeight)
{
	Lock();
	bool bSuccess = false;

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);
		if (!(bSuccess = pApp->EW_Reshape(iWidth, iHeight)))
		{
			m_sLastError = pApp->GetLastError();
		}
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::EW_GetShape(int iHandle, int& iWidth, int& iHeight)
{
	Lock();
	bool bSuccess = false;

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);
		if (!(bSuccess = pApp->EW_GetShape(iWidth, iHeight)))
		{
			m_sLastError = pApp->GetLastError();
		}
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::EW_Mouse(int iHandle, int iButton, int iState, int iX, int iY)
{
	Lock();
	bool bSuccess = false;

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);
		if (!(bSuccess = pApp->EW_Mouse(iButton, iState, iX, iY)))
		{
			m_sLastError = pApp->GetLastError();
		}
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::EW_ActiveMouseMove(int iHandle, int iX, int iY)
{
	Lock();
	bool bSuccess = false;

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);
		if (!(bSuccess = pApp->EW_ActiveMouseMove(iX, iY)))
		{
			m_sLastError = pApp->GetLastError();
		}
	}
	catch (Clu::CIException& xEx)
	{
		throw CLU_EXCEPTION_NEST("Error on active mouse move", std::move(xEx));
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::EW_PassiveMouseMove(int iHandle, int iX, int iY)
{
	Lock();
	bool bSuccess = false;

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);
		if (!(bSuccess = pApp->EW_PassiveMouseMove(iX, iY)))
		{
			m_sLastError = pApp->GetLastError();
		}
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::EW_IdleFunc(int iHandle)
{
	Lock();
	bool bSuccess = false;

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);
		if (!(bSuccess = pApp->EW_IdleFunc()))
		{
			m_sLastError = pApp->GetLastError();
		}
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::EW_SetKeyModifierStates(int iHandle, bool bShift, bool bCtrl, bool bAlt, bool bNumLock, bool bCapsLock, bool bScrollLock)
{
	Lock();
	bool bSuccess = false;

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);
		if (!(bSuccess = pApp->EW_SetKeyModifierStates(bShift, bCtrl, bAlt, bNumLock, bCapsLock, bScrollLock)))
		{
			m_sLastError = pApp->GetLastError();
		}
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::EW_Keyboard(int iHandle, int iKey, bool bKeyDown)
{
	Lock();
	bool bSuccess = false;

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);
		if (!(bSuccess = pApp->EW_Keyboard(iKey, bKeyDown)))
		{
			m_sLastError = pApp->GetLastError();
		}
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::EW_LoadScript(int iHandle, const char* pcFilename, const char* pcPass)
{
	Lock();
	bool bSuccess = false;

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);

		const char* pcPassKey = 0;
		string sFile;

		if (pcPass)
		{
			pcPassKey = pcPass;
		}

		sFile = pcFilename;
		if ((strchr(pcFilename, ':') == 0) && (m_sScriptPath.length() > 0))
		{
			sFile = m_sScriptPath + sFile;
		}

		if (!(bSuccess = pApp->EW_LoadScript(sFile.c_str(), pcPassKey)))
		{
			m_sLastError = pApp->GetLastError();
		}
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::EW_SetScript(int iHandle, const char* pcScript, bool bResetEnv, bool bReparseLatex)
{
	Lock();
	bool bSuccess = false;

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);
		if (!(bSuccess = pApp->EW_SetScript(pcScript, bResetEnv, bReparseLatex)))
		{
			m_sLastError = pApp->GetLastError();
		}
	}
	catch (Clu::CIException& ex)
	{
		Unlock();
		throw CLU_EXCEPTION_NEST("Error setting script", std::move(ex));
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::EW_ExecUser(int iHandle, const char* pcToolName)
{
	Lock();
	bool bSuccess = false;

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);
		if (!(bSuccess = pApp->EW_ExecUser(pcToolName)))
		{
			m_sLastError = pApp->GetLastError();
		}
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::EW_LockVis(int iHandle, bool bLock)
{
	// Only lock app list if visualization should be locked
	if (bLock)
	{
		Lock();
	}

	bool bSuccess = false;

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);
		if (!(bSuccess = pApp->EW_LockVis(bLock)))
		{
			m_sLastError = pApp->GetLastError();
		}
	}
	catch (...)
	{
		bSuccess = false;
	}

	// Don't unlock app list if visualization is now locked
	if (!bLock)
	{
		Unlock();
	}

	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::EW_MakeCurrent(int iHandle, bool bBind)
{
	Lock();
	bool bSuccess = false;

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);
		if (!(bSuccess = pApp->EW_MakeCurrent(bBind)))
		{
			m_sLastError = pApp->GetLastError();
		}
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::GetScriptOutput(int iHandle, std::vector<std::string>& vecText, bool& bIsError)
{
	Lock();
	bool bSuccess = false;

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);
		if (!(bSuccess = pApp->GetScriptOutput(vecText, bIsError)))
		{
			m_sLastError = pApp->GetLastError();
		}
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::EnableVisualization(int iHandle, bool bVal)
{
	Lock();
	bool bSuccess = false;

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);
		if (!(bSuccess = pApp->EnableVisualization(bVal)))
		{
			m_sLastError = pApp->GetLastError();
		}
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::EnableDrawing(int iHandle, bool bVal, bool bDoRedisplay)
{
	Lock();
	bool bSuccess = false;

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);
		if (!(bSuccess = pApp->EnableDrawSceneGraph(bVal, bDoRedisplay)))
		{
			m_sLastError = pApp->GetLastError();
		}
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::SaveView(int iHandle, const char* pcFilename)
{
	Lock();
	bool bSuccess = false;

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);
		if (!(bSuccess = pApp->SaveView(pcFilename)))
		{
			m_sLastError = pApp->GetLastError();
		}
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::GetVar(int iHandle, const char* pcVarName, int& iVal)
{
	Lock();
	bool bSuccess = false;

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);
		if (!(bSuccess = pApp->GetVar(pcVarName, iVal)))
		{
			m_sLastError = pApp->GetLastError();
		}
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::GetVar(int iHandle, const char* pcVarName, double& dVal)
{
	Lock();
	bool bSuccess = false;

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);
		if (!(bSuccess = pApp->GetVar(pcVarName, dVal)))
		{
			m_sLastError = pApp->GetLastError();
		}
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::GetVar(int iHandle, const char* pcVarName, string& sVal)
{
	Lock();
	bool bSuccess = false;

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);
		if (!(bSuccess = pApp->GetVar(pcVarName, sVal)))
		{
			m_sLastError = pApp->GetLastError();
		}
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::GetVar(int iHandle, const char* pcVarName, unsigned& uDimCnt, unsigned& uElCnt)
{
	Lock();
	bool bSuccess = false;

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);
		if (!(bSuccess = pApp->GetVar(pcVarName, uDimCnt, uElCnt)))
		{
			m_sLastError = pApp->GetLastError();
		}
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::GetVar(int iHandle, const char* pcVarName, unsigned* puDim, double* pdData)
{
	Lock();
	bool bSuccess = false;

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);
		if (!(bSuccess = pApp->GetVar(pcVarName, puDim, pdData)))
		{
			m_sLastError = pApp->GetLastError();
		}
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::GetVar(int iHandle, const char* pcVarName, Clu::CIImage& xImage)
{
	Lock();
	bool bSuccess = false;

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);
		if (!(bSuccess = pApp->GetVar(pcVarName, xImage)))
		{
			m_sLastError = pApp->GetLastError();
		}
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::GetVar(int iHandle, const char* pcVarName, double& dRed, double& dGreen, double& dBlue, double& dAlpha)
{
	Lock();
	bool bSuccess = false;

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);
		double pdColor[4];

		if (!(bSuccess = pApp->GetVar(pcVarName, pdColor)))
		{
			m_sLastError = pApp->GetLastError();
		}

		dRed   = pdColor[0];
		dGreen = pdColor[1];
		dBlue  = pdColor[2];
		dAlpha = pdColor[3];
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::SetVar(int iHandle, const char* pcVarName, int iVal)
{
	Lock();
	bool bSuccess = false;

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);
		if (!(bSuccess = pApp->SetVar(pcVarName, iVal)))
		{
			m_sLastError = pApp->GetLastError();
		}
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::SetVar(int iHandle, const char* pcVarName, double dVal)
{
	Lock();
	bool bSuccess = false;

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);
		if (!(bSuccess = pApp->SetVar(pcVarName, dVal)))
		{
			m_sLastError = pApp->GetLastError();
		}
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::SetVar(int iHandle, const char* pcVarName, const string& sVal)
{
	Lock();
	bool bSuccess = false;

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);
		if (!(bSuccess = pApp->SetVar(pcVarName, sVal)))
		{
			m_sLastError = pApp->GetLastError();
		}
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::SetVar(int iHandle, const char* pcVarName, const vector<string>& vsVal)
{
	Lock();
	bool bSuccess = false;

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);
		if (!(bSuccess = pApp->SetVar(pcVarName, vsVal)))
		{
			m_sLastError = pApp->GetLastError();
		}
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::SetVar(int iHandle, const char* pcVarName, int iDimCnt, const int* piDim, const double* pdData)
{
	Lock();
	bool bSuccess = false;

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);
		if (!(bSuccess = pApp->SetVar(pcVarName, iDimCnt, piDim, pdData)))
		{
			m_sLastError = pApp->GetLastError();
		}
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::SetVar(int iHandle, const char* pcVarName, const Clu::CIImage& xImage)
{
	Lock();
	bool bSuccess = false;

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);
		if (!(bSuccess = pApp->SetVar(pcVarName, xImage)))
		{
			m_sLastError = pApp->GetLastError();
		}
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::SetVar(int iHandle, const char* pcVarName, double dRed, double dGreen, double dBlue, double dAlpha)
{
	Lock();
	bool bSuccess = false;

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);

		double pdColor[4];
		pdColor[0] = dRed;
		pdColor[1] = dGreen;
		pdColor[2] = dBlue;
		pdColor[3] = dAlpha;
		if (!(bSuccess = pApp->SetVar(pcVarName, pdColor)))
		{
			m_sLastError = pApp->GetLastError();
		}
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::SetVar(int iHandle, const char* pcVarName, const Clu::SStdVbo* pVexArray)
{
	Lock();
	bool bSuccess = false;

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);
		if (!(bSuccess = pApp->SetVar(pcVarName, pVexArray)))
		{
			m_sLastError = pApp->GetLastError();
		}
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUVizAppListST::SetVar(int iHandle, const char* pcVarName, Clu::CIDataContainer xData)
{
	Lock();
	bool bSuccess = false;

	try
	{
		CCLUVizApp* pApp = _GetApp(iHandle);
		if (!(bSuccess = pApp->SetVar(pcVarName, xData)))
		{
			m_sLastError = pApp->GetLastError();
		}
	}
	catch (...)
	{
		bSuccess = false;
	}

	Unlock();
	return bSuccess;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCLUVizApp* CCLUVizAppListST::_GetApp(int iHandle)
{
	if (!m_bIsRunning)
	{
		m_sLastError = "CLUViz is not running.";
		throw CException();
	}

	TAppMap::iterator itEl;
	if ((itEl = m_mapApp.find(iHandle)) == m_mapApp.end())
	{
		// Handle not found
		m_sLastError = "Invalid handle.";
		throw CException();
	}

	return itEl->second;
}
