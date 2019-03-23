#include "StdAfx.h"
#include "Clu.Viz.WndList.h"

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


#define GET_ACCESS \
	if (WaitForSingleObject(m_hMutexAccess, m_uTimeout) == WAIT_TIMEOUT) \
	{ \
		m_sLastError = "Timeout while waiting for visualization."; \
		return false; \
	} \
	else if (!m_bIsRunning) \
	{ \
		ReleaseMutex(m_hMutexAccess); return false; \
	}

#define GET_ACCESS_MSG \
	if (WaitForMultipleObjects(2, /*(const HANDLE *)*/ m_phMutexAccessMsg, TRUE, m_uTimeout) == WAIT_TIMEOUT) \
	{ \
		m_sLastError = "Timeout while waiting for visualization."; \
		return false; \
	} \
	else if (!m_bIsRunning) \
	{ \
		ReleaseMutex(m_hMutexAccess); ReleaseMutex(m_hMutexMsg); return false; \
	}

#define RELEASE_ACCESS \
	PostThreadMessage(m_dwThreadID, 0 /*WM_APP_MSG*/, 0, 0); \
	ReleaseMutex(m_hMutexAccess);

#define RELEASE_MSG \
	ReleaseMutex(m_hMutexMsg);

#define WAIT_FOR_EVENT_TIME 600000

////////////////////////////////////////////////////////////////
/// Constructor

CCLUVizAppListMT::CCLUVizAppListMT(bool bInitAll)
{
	m_iCurHandle    = 0;
	m_bIsRunning    = false;
	m_uTimeout      = INFINITE;
	m_uEventTimeout = WAIT_FOR_EVENT_TIME;

	m_hMutexAccess = 0;
	m_hMutexRun    = 0;
	m_hMutexMsg    = 0;
	m_hEventMsg    = 0;
	m_dwThreadID   = 0;

	m_iMsgID      = APP_MSG_NONE;
	m_pMsgData    = 0;
	m_bMsgSuccess = false;

	m_bAllowUserEnd = false;

	m_bInitAll = bInitAll;

	if (m_bInitAll)
	{
		// Initialize Image Repository
		::InitImageRepository(new CImageRepository);

		// Initialize GA library
		InitCLUGA();

		// Initialize Image Library
		ilInit();
		//ilEnable(IL_ORIGIN_SET);
		//ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
		iluInit();
		ilutInit();
	}

	m_pCLUCodeBase = new CCLUCodeBase();
	m_uDNG_This    = (unsigned __int64) this;

	_time32((__time32_t*) &m_iRanSeed);

	m_pDevice3DX = 0;
}

////////////////////////////////////////////////////////////////
/// Destructor

CCLUVizAppListMT::~CCLUVizAppListMT()
{
	//MessageBoxA( NULL, "Destruktor: Start", "Debug", MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL | MB_TOPMOST );

	DestroyAll();
	End();

	if (m_bInitAll)
	{
		FinalizeCLUGA();
		::ReleaseImageRepository();
		ilShutDown();
	}
	delete m_pCLUCodeBase;
}

////////////////////////////////////////////////////////////////
// Wait for event message

void CCLUVizAppListMT::WaitForEventMsg(int iMillisec)
{
	while (WaitForSingleObject(m_hEventMsg, iMillisec) == WAIT_TIMEOUT)
	{
		string sText;
		char pcTime[100];

		sprintf_s(pcTime, 100, "%d", iMillisec / 1000);

		sText  = "The CLUViz Visualization Engine does not respond for about ";
		sText += pcTime;
		sText += " seconds.\n"
			 "This could be due to an extended calculation but could also indicate\n"
			 "that an internal error occured. Press:\n"
			 "- 'Abort' to end the visualization engine,\n"
			 "- 'Retry' to wait for another";
		sText += pcTime;
		sText += " seconds,\n"
			 "- 'Ignore' to carry on regardless. This could lead to an unstable state.";

		int iDiagRet = MessageBoxA(NULL, sText.c_str(),
				"Internal Error",
				MB_ABORTRETRYIGNORE | MB_ICONEXCLAMATION | MB_DEFBUTTON2 | MB_TASKMODAL | MB_TOPMOST);

		if (iDiagRet == IDABORT)
		{
			exit(-1);
		}
		else if (iDiagRet == IDIGNORE)
		{
			break;
		}
	}
}


////////////////////////////////////////////////////////////////
// Set Script Path

bool CCLUVizAppListMT::SetScriptPath(const char* pcPath)
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

//////////////////////////////////////////////////////////////////
//
//bool CCLUVizAppListMT::GetFileTypeScript(const char* pcFileType, int iMaxLen, char* pcData)
//{
//	if ((iMaxLen <= 100) || (pcFileType == 0) || (pcData == 0))
//	{
//		return false;
//	}
//
//	HKEY hCLUViz;
//	char pcRegKey[100];
//
//	sprintf_s(pcRegKey, 100, "%s\\%s", CLUVIZ_FILETYPES_REGKEY, pcFileType);
//
//	// Get Path to CLUViz Install Directory
//	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
//				(LPCSTR) pcRegKey,
//				0,
//				KEY_READ,
//				&hCLUViz) == ERROR_SUCCESS)
//	{
//		DWORD dwType, dwSize = (DWORD) iMaxLen;
//
//		if (RegQueryValueExA(hCLUViz, "ScriptPath", NULL,
//					&dwType, (BYTE*) pcData, &dwSize) != ERROR_SUCCESS)
//		{
//			m_sLastError = "No script path given for file type";
//			return false;
//		}
//
//		RegCloseKey(hCLUViz);
//	}
//	else
//	{
//		m_sLastError = "File type not registered";
//		return false;
//	}
//
//	return true;
//}

////////////////////////////////////////////////////////////////
/// Start Thread

bool CCLUVizAppListMT::Start(const char* pcEnvPath, const char* pcModulePath)
{
	if (m_bIsRunning)
	{
		m_sLastError = "Application is already running.";
		return false;
	}

	if (!pcEnvPath)
	{
		m_sLastError = "No environment path given.";
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
		return false;
	}


	unsigned char ucVal;
	ucVal = (unsigned char) 10;	
	for (int i = 0; i < 32; ++i, ++ucVal)
	{
		m_pcMsgID[i] = ucVal;
	}

	// Create Event that signals if message has been executed
	m_hEventMsg = CreateEvent(NULL, FALSE, FALSE, NULL);
	// Create mutex indicating thread is running
	m_hMutexRun = CreateMutex(NULL, FALSE, NULL);

	_beginthread(CCLUVizAppListMT::Run, 0, (void*)m_uDNG_This);

	// Wait for thread to initialize
	WaitForEventMsg(m_uEventTimeout);
	ResetEvent(m_hEventMsg);

	InitViz();

	return true;
}

////////////////////////////////////////////////////////////////
/// Run App List

void CCLUVizAppListMT::Run(void* pData)
{
	unsigned char pcMsgID[32];

	CCLUVizAppListMT* pThis = (CCLUVizAppListMT*) pData;

	unsigned char ucVal;
	ucVal = (unsigned char) 10;	
	for (int i = 0; i < 32; ++i, ++ucVal)
	{
		pcMsgID[i] = ucVal;
	}

	pThis->m_iMsgID   = int(pcMsgID[APP_MSG_NONE]);
	pThis->m_pMsgData = 0;

	if (pThis->m_bIsRunning)
	{
		return;
	}

	WaitForSingleObject(pThis->m_hMutexRun, INFINITE);

	pThis->m_dwThreadID = GetCurrentThreadId();

	pThis->m_bIsRunning = true;
	pThis->m_bDoRun     = true;

	// Create Mutex
	#ifdef WIN32
		// Create mutex for OGLWin access
		pThis->m_hMutexAccess = CreateMutex(NULL, TRUE, NULL);
		// Create Mutex for Message Access
		pThis->m_hMutexMsg = CreateMutex(NULL, FALSE, NULL);
	#else
		m_hMutexAccess = 0;
	#endif

	// Store handles to mutexes in an array to wait for both of them
	pThis->m_phMutexAccessMsg[0] = pThis->m_hMutexAccess;
	pThis->m_phMutexAccessMsg[1] = pThis->m_hMutexMsg;

	// Tell other thread that initialization is finished
	SetEvent(pThis->m_hEventMsg);
	SwitchToThread();

	Fl::wait(0);

	// Start thread loop
	while (pThis->m_bDoRun)
	{
		//Fl::wait( 0 );

		ReleaseMutex(pThis->m_hMutexAccess);
		SwitchToThread();

		//GetMessage( &theMsg, NULL, 0, 0 );
		Fl::wait(1.0);	//e20 );

		WaitForSingleObject(pThis->m_hMutexAccess, INFINITE);

		//Fl::wait( 1.0 ); //e20 );
		/*
		if ( theMsg.message != WM_APP_MSG )
		{
										PostMessage( theMsg.hwnd, theMsg.message, theMsg.wParam, theMsg.lParam );
						//TranslateMessage(&theMsg);
										//DispatchMessage(&theMsg);
		}
		*/
		//Fl::wait( 0 );

		if (pThis->m_bAllowUserEnd && !Fl::first_window())
		{
			pThis->m_bDoRun = false;
		}

		if (!pThis->m_bDoRun)
		{
			pThis->m_bIsRunning = false;
			SetEvent(pThis->m_hEventMsg);
			ReleaseMutex(pThis->m_hMutexAccess);
			SwitchToThread();
			pThis->TC_DestroyAll();
			break;
		}

		if (pThis->m_iMsgID == int(pcMsgID[APP_MSG_CREATE]))
		{
			pThis->MsgCreate();
			SetEvent(pThis->m_hEventMsg);
		}
		else if (pThis->m_iMsgID == int(pcMsgID[APP_MSG_DESTROY]))
		{
			pThis->MsgDestroy();
			SetEvent(pThis->m_hEventMsg);
		}
		else if (pThis->m_iMsgID == int(pcMsgID[APP_MSG_SETSCRIPT]))
		{
			pThis->MsgSetScript();
			SetEvent(pThis->m_hEventMsg);
		}
		else if (pThis->m_iMsgID == int(pcMsgID[APP_MSG_LOADSCRIPT]))
		{
			pThis->MsgLoadScript();
			SetEvent(pThis->m_hEventMsg);
		}
		else if (pThis->m_iMsgID == int(pcMsgID[APP_MSG_EXECUSER]))
		{
			pThis->MsgExecUser();
			SetEvent(pThis->m_hEventMsg);
		}
		else if (pThis->m_iMsgID == int(pcMsgID[APP_MSG_SETVAR]))
		{
			pThis->MsgSetVar();
			SetEvent(pThis->m_hEventMsg);
		}
		else if (pThis->m_iMsgID == int(pcMsgID[APP_MSG_GETVAR]))
		{
			pThis->MsgGetVar();
			SetEvent(pThis->m_hEventMsg);
		}
		else if (pThis->m_iMsgID == int(pcMsgID[APP_MSG_FULLSCREEN]))
		{
			pThis->MsgFullScreen();
			SetEvent(pThis->m_hEventMsg);
		}
		else if (pThis->m_iMsgID == int(pcMsgID[APP_MSG_WNDPOSSIZE]))
		{
			pThis->MsgSetWndPosSize();
			SetEvent(pThis->m_hEventMsg);
		}
		else if (pThis->m_iMsgID == int(pcMsgID[APP_MSG_WNDSHOW]))
		{
			pThis->MsgShowWindow();
			SetEvent(pThis->m_hEventMsg);
		}
		else if (pThis->m_iMsgID == int(pcMsgID[APP_MSG_WNDHIDE]))
		{
			pThis->MsgHideWindow();
			SetEvent(pThis->m_hEventMsg);
		}
		else if (pThis->m_iMsgID == int(pcMsgID[APP_MSG_EDTSHOW]))
		{
			pThis->MsgShowEditor();
			SetEvent(pThis->m_hEventMsg);
		}
		else if (pThis->m_iMsgID == int(pcMsgID[APP_MSG_EDTHIDE]))
		{
			pThis->MsgHideEditor();
			SetEvent(pThis->m_hEventMsg);
		}
		else if (pThis->m_iMsgID == int(pcMsgID[APP_MSG_INIT]))
		{
			pThis->MsgInitViz();
			SetEvent(pThis->m_hEventMsg);
		}
		else if (pThis->m_iMsgID == int(pcMsgID[APP_MSG_FINALIZE]))
		{
			pThis->MsgFinalizeViz();
			SetEvent(pThis->m_hEventMsg);
		}
		else if (pThis->m_iMsgID == int(pcMsgID[APP_MSG_ISVIEWVALID]))
		{
			pThis->MsgIsViewValid();
			SetEvent(pThis->m_hEventMsg);
		}
		else if (pThis->m_iMsgID == int(pcMsgID[APP_MSG_GETSCRIPTOUTPUT]))
		{
			pThis->MsgGetScriptOutput();
			SetEvent(pThis->m_hEventMsg);
		}
		else if (pThis->m_iMsgID != int(pcMsgID[APP_MSG_NONE]))
		{
			char pcID[100];
			string sText;

			sprintf_s(pcID, 100, "%d", pThis->m_iMsgID);
			sText  = "Unknown message ID = ";
			sText += pcID;

			MessageBoxA(NULL, sText.c_str(), "Internal Error",
					MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL | MB_TOPMOST);

			SetEvent(pThis->m_hEventMsg);
		}

		pThis->m_iMsgID   = int(pcMsgID[APP_MSG_NONE]);
		pThis->m_pMsgData = 0;
	}

	pThis->m_bIsRunning = false;

	#ifdef WIN32
		CloseHandle(pThis->m_hMutexAccess);
		CloseHandle(pThis->m_hMutexMsg);
		pThis->m_hMutexAccess = 0;
		pThis->m_hMutexMsg    = 0;

		ReleaseMutex(pThis->m_hMutexRun);

		pThis->m_dwThreadID = 0;
	#endif
}


////////////////////////////////////////////////////////////////
/// Exit App List

bool CCLUVizAppListMT::End()
{
	if (!m_bIsRunning)
	{
		return true;
	}

	FinalizeViz();

	//WaitForSingleObject( m_hMutexAccess, INFINITE );
	GET_ACCESS

		m_bDoRun = false;

	RELEASE_ACCESS

	//ReleaseMutex( m_hMutexAccess );
		SwitchToThread();

	WaitForSingleObject(m_hMutexRun, INFINITE);

	CloseHandle(m_hEventMsg);
	CloseHandle(m_hMutexRun);

	m_hEventMsg = 0;
	m_hMutexRun = 0;

	// Delete all fonts used by FL
	Fl::reset_fonts();

	// Only Release module after all member variables of OGLWin have been
	// deleted, since this also includes the Base Element Repository.
	CCLUParse::ReleaseModules();

	return true;
}

////////////////////////////////////////////////////////////////
/// Get Last Error

string CCLUVizAppListMT::GetLastError()
{
	string sText;

	WaitForSingleObject(m_hMutexAccess, INFINITE);

	sText = m_sLastError;

	ReleaseMutex(m_hMutexAccess);

	return sText;
}

////////////////////////////////////////////////////////////////
/// Exit App List

bool CCLUVizAppListMT::WaitForEnd()
{
	if (!m_bIsRunning)
	{
		return true;
	}

	//MessageBoxA( NULL, "WaitForEnd: Start", "Debug", MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL | MB_TOPMOST );

	//WaitForSingleObject( m_hMutexAccess, INFINITE );
	GET_ACCESS

		m_bAllowUserEnd = true;

	RELEASE_ACCESS
	//ReleaseMutex( m_hMutexAccess );
		SwitchToThread();

	WaitForSingleObject(m_hMutexRun, INFINITE);

	CloseHandle(m_hEventMsg);
	CloseHandle(m_hMutexRun);

	m_hEventMsg = 0;
	m_hMutexRun = 0;

	// Delete all fonts used by FL
	Fl::reset_fonts();

	//MessageBoxA( NULL, "WaitForEnd: End", "Debug", MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL | MB_TOPMOST );

	// Only Release module after all member variables of OGLWin have been
	// deleted, since this also includes the Base Element Repository.
	CCLUParse::ReleaseModules();

	return true;
}

////////////////////////////////////////////////////////////////
/// Allow user to end CLUViz

bool CCLUVizAppListMT::EnableUserEnd(bool bVal)
{
	if (!m_bIsRunning)
	{
		return true;
	}

	//WaitForSingleObject( m_hMutexAccess, INFINITE );
	GET_ACCESS

		m_bAllowUserEnd = bVal;

	RELEASE_ACCESS
	//ReleaseMutex( m_hMutexAccess );
		SwitchToThread();

	return true;
}

////////////////////////////////////////////////////////////////
/// Test whether CLUViz is still running

bool CCLUVizAppListMT::IsRunning(int iWait)
{
	if (WaitForSingleObject(m_hMutexRun, iWait) == WAIT_OBJECT_0)
	{
		CloseHandle(m_hEventMsg);
		CloseHandle(m_hMutexRun);

		m_hEventMsg = 0;
		m_hMutexRun = 0;
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Init Viz Thread

bool CCLUVizAppListMT::InitViz()
{
	if (!m_bIsRunning)
	{
		m_sLastError = "CLUViz is not running.";
		return false;
	}

	CValData<int> Data(0);

	// Wait for access to run loop and for access to be allowed to set a message
	GET_ACCESS_MSG

	// Set the message
		m_iMsgID = int(m_pcMsgID[APP_MSG_INIT]);
	m_pMsgData = (CMsgData*) &Data;

	// Release access to run loop but do not release access to message
	RELEASE_ACCESS
		SwitchToThread();

	// Wait for message to be processed
	WaitForEventMsg(m_uEventTimeout);
	ResetEvent(m_hEventMsg);

	// Only now release message
	RELEASE_MSG

	return m_bMsgSuccess;
	//return true;
}

//////////////////////////////////////////////////////////////////////
/// Message Init

bool CCLUVizAppListMT::MsgInitViz()
{
	try
	{
		volatile CValData<int>* pData = dynamic_cast<volatile CValData<int>*>(m_pMsgData);

		if (!pData)
		{
			m_sLastError = "Invalid data pointer for create process.";
			throw CException();
		}

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

		m_bMsgSuccess = true;
	}
	catch (...)
	{
		m_bMsgSuccess = false;
	}

	return m_bMsgSuccess;
}

//////////////////////////////////////////////////////////////////////
/// Finalize Viz Thread

bool CCLUVizAppListMT::FinalizeViz()
{
	if (!m_bIsRunning)
	{
		m_sLastError = "CLUViz is not running.";
		return false;
	}

	CValData<int> Data(0);

	GET_ACCESS_MSG

		m_iMsgID = int(m_pcMsgID[APP_MSG_FINALIZE]);
	m_pMsgData = (CMsgData*) &Data;

	RELEASE_ACCESS
		SwitchToThread();

	WaitForEventMsg(m_uEventTimeout);
	ResetEvent(m_hEventMsg);

	RELEASE_MSG

	return m_bMsgSuccess;
}

//////////////////////////////////////////////////////////////////////
/// Message Init

bool CCLUVizAppListMT::MsgFinalizeViz()
{
	try
	{
		volatile CValData<int>* pData = dynamic_cast<volatile CValData<int>*>(m_pMsgData);

		if (!pData)
		{
			m_sLastError = "Invalid data pointer for finalize process.";
			throw CException();
		}

		if (m_pDevice3DX)
		{
			#ifdef CV_USE_DEVICE_3DX
				delete m_pDevice3DX;
			#endif
			m_pDevice3DX = 0;
		}

		m_bMsgSuccess = true;
	}
	catch (...)
	{
		m_bMsgSuccess = false;
	}

	return m_bMsgSuccess;
}

////////////////////////////////////////////////////////////////
/// Create CLUViz Window Thread

bool CCLUVizAppListMT::Create(int& iHandle,
		int iX,
		int iY,
		int iWidth,
		int iHeight,
		const char* pcTitle,
		const char* pcInitScript,
		const char* pcCLUPath,
		bool bVizOnly,
		bool bShowIntro,
		void* pvIcon,
		void* pvParentHandle,
		unsigned int uWindowStyle)
{
	if (!m_bIsRunning)
	{
		m_sLastError = "CLUViz is not running.";
		return false;
	}

	if (!pcCLUPath)
	{
		pcCLUPath = m_sEnvPath.c_str();
	}


	CCreateData Data(iX, iY, iWidth, iHeight, pcTitle,
			 pcInitScript, pcCLUPath, bVizOnly, bShowIntro,
			 pvIcon, pvParentHandle, uWindowStyle, "Public");

	GET_ACCESS_MSG

		m_iMsgID = int(m_pcMsgID[APP_MSG_CREATE]);
		m_pMsgData = (CMsgData*) &Data;

	RELEASE_ACCESS
	SwitchToThread();

	WaitForEventMsg(m_uEventTimeout);
	ResetEvent(m_hEventMsg);

	if (m_bMsgSuccess)
	{
		iHandle = m_iCurHandle;
	}
	else
	{
		iHandle = -1;
	}

	RELEASE_MSG

	return m_bMsgSuccess;
}

//////////////////////////////////////////////////////////////////////
/// Execute Message Create

bool CCLUVizAppListMT::MsgCreate()
{
	try
	{
		volatile CCreateData* pData = dynamic_cast<volatile CCreateData*>(m_pMsgData);

		if (!pData)
		{
			m_sLastError = "Invalid data pointer for create process.";
			throw CException();
		}

		CCLUVizApp* pApp = new CCLUVizApp();

		if (!pApp)
		{
			m_sLastError = "Out of memory while creating new visualization.";
			throw CException();
		}

		if (!pApp->Create(pData->iX, pData->iY,
					pData->iWidth, pData->iHeight,
					pData->pcTitle, pData->pcInitScript,
					pData->pcCLUPath, pData->bVizOnly,
					pData->bShowIntro,
					pData->pvIcon,
					pData->pvParentHandle,
					pData->uWindowStyle,
					pData->pcLicensee))
		{
			m_sLastError = pApp->GetLastError();
			delete pApp;
			throw CException();
		}

		m_mapApp[++m_iCurHandle] = pApp;

		m_bMsgSuccess = true;
	}
	catch (...)
	{
		m_bMsgSuccess = false;
	}

	return m_bMsgSuccess;
}

//////////////////////////////////////////////////////////////////////
/// Destroy

bool CCLUVizAppListMT::Destroy(int iHandle)
{
	if (!m_bIsRunning)
	{
		m_sLastError = "CLUViz is not running.";
		return false;
	}

	CValData<int> Data(iHandle);

	GET_ACCESS_MSG

		m_iMsgID = int(m_pcMsgID[APP_MSG_DESTROY]);
	m_pMsgData = (CMsgData*) &Data;

	RELEASE_ACCESS
		SwitchToThread();

	WaitForEventMsg(m_uEventTimeout);
	ResetEvent(m_hEventMsg);

	RELEASE_MSG

	return m_bMsgSuccess;
}

//////////////////////////////////////////////////////////////////////
/// Message Destroy

bool CCLUVizAppListMT::MsgDestroy()
{
	try
	{
		volatile CValData<int>* pData = dynamic_cast<volatile CValData<int>*>(m_pMsgData);

		if (!pData)
		{
			m_sLastError = "Invalid data pointer for create process.";
			throw CException();
		}

		int iHandle = int(pData->Val);
		TAppMap::iterator itEl;

		if ((itEl = m_mapApp.find(iHandle)) == m_mapApp.end())
		{
			// Handle not found
			m_sLastError = "Invalid handle.";
			throw CException();
		}

		CCLUVizApp* pApp = itEl->second;

		if (pApp)
		{
			delete pApp;
		}

		m_mapApp.erase(itEl);
		m_bMsgSuccess = true;
	}
	catch (...)
	{
		m_bMsgSuccess = false;
	}

	return m_bMsgSuccess;
}

//////////////////////////////////////////////////////////////////////
/// Destroy All

bool CCLUVizAppListMT::DestroyAll()
{
	if (!m_bIsRunning)
	{
		m_sLastError = "CLUViz is not running.";
		return false;
	}

	TAppMap::iterator itEl;

	//for ( itEl = m_mapApp.begin();
	//		itEl != m_mapApp.end();
	//		++itEl )
	while (true)
	{
		itEl = m_mapApp.begin();
		if (itEl != m_mapApp.end())
		{
			Destroy(itEl->first);
		}
		else
		{
			break;
		}
	}

	m_mapApp.clear();

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Destroy All called from thread context

bool CCLUVizAppListMT::TC_DestroyAll()
{
	TAppMap::iterator itEl;

	for (itEl = m_mapApp.begin();
		 itEl != m_mapApp.end();
		 ++itEl)
	{
		if (itEl->second)
		{
			delete itEl->second;
		}
	}

	m_mapApp.clear();

	return true;
}

//////////////////////////////////////////////////////////////////////
// Test whether view with given handle is created

bool CCLUVizAppListMT::IsViewValid(int iHandle)
{
	if (!m_bIsRunning)
	{
		m_sLastError = "CLUViz is not running.";
		return false;
	}

	CHandleValData<bool> Data(iHandle, false);

	GET_ACCESS_MSG

		m_iMsgID = int(m_pcMsgID[APP_MSG_ISVIEWVALID]);
	m_pMsgData = (CMsgData*) &Data;

	RELEASE_ACCESS
		SwitchToThread();

	WaitForEventMsg(m_uEventTimeout);
	ResetEvent(m_hEventMsg);

	RELEASE_MSG

	if (m_bMsgSuccess)
	{
		return Data.Val;
	}

	return m_bMsgSuccess;
}

//////////////////////////////////////////////////////////////////////
/// Message IsViewCreated

bool CCLUVizAppListMT::MsgIsViewValid()
{
	try
	{
		volatile CHandleValData<bool>* pData = dynamic_cast<volatile CHandleValData<bool>*>(m_pMsgData);

		if (!pData)
		{
			m_sLastError = "Invalid data pointer for IsViewValid().";
			throw CException();
		}

		int iHandle = int(pData->iHandle);
		TAppMap::iterator itEl;

		if ((itEl = m_mapApp.find(iHandle)) == m_mapApp.end())
		{
			// Handle not found
			m_sLastError = "Invalid handle.";
			throw CException();
		}

		CCLUVizApp* pApp = itEl->second;

		pData->Val = pApp->IsValid();

		m_bMsgSuccess = true;
	}
	catch (...)
	{
		m_bMsgSuccess = false;
	}

	return m_bMsgSuccess;
}

//////////////////////////////////////////////////////////////////////
/// Show Editor

bool CCLUVizAppListMT::ShowEditor(int iHandle)
{
	if (!m_bIsRunning)
	{
		m_sLastError = "CLUViz is not running.";
		return false;
	}

	CValData<int> Data(iHandle);

	GET_ACCESS_MSG

		m_iMsgID = int(m_pcMsgID[APP_MSG_EDTSHOW]);
	m_pMsgData = (CMsgData*) &Data;

	RELEASE_ACCESS
		SwitchToThread();

	WaitForEventMsg(m_uEventTimeout);
	ResetEvent(m_hEventMsg);

	RELEASE_MSG

	return m_bMsgSuccess;
}

//////////////////////////////////////////////////////////////////////
/// Message Show Editor

bool CCLUVizAppListMT::MsgShowEditor()
{
	try
	{
		volatile CValData<int>* pData = dynamic_cast<volatile CValData<int>*>(m_pMsgData);

		if (!pData)
		{
			m_sLastError = "Invalid data pointer for create process.";
			throw CException();
		}

		int iHandle = int(pData->Val);
		TAppMap::iterator itEl;

		if ((itEl = m_mapApp.find(iHandle)) == m_mapApp.end())
		{
			// Handle not found
			m_sLastError = "Invalid handle.";
			throw CException();
		}

		CCLUVizApp* pApp = itEl->second;

		if (!pApp->ShowEditor())
		{
			m_sLastError = pApp->GetLastError();
			throw CException();
		}

		m_bMsgSuccess = true;
	}
	catch (...)
	{
		m_bMsgSuccess = false;
	}

	return m_bMsgSuccess;
}

//////////////////////////////////////////////////////////////////////
/// Hide Editor

bool CCLUVizAppListMT::HideEditor(int iHandle)
{
	if (!m_bIsRunning)
	{
		m_sLastError = "CLUViz is not running.";
		return false;
	}

	CValData<int> Data(iHandle);

	GET_ACCESS_MSG

		m_iMsgID = int(m_pcMsgID[APP_MSG_EDTHIDE]);
	m_pMsgData = (CMsgData*) &Data;

	RELEASE_ACCESS
		SwitchToThread();

	WaitForEventMsg(m_uEventTimeout);
	ResetEvent(m_hEventMsg);

	RELEASE_MSG

	return m_bMsgSuccess;
}

//////////////////////////////////////////////////////////////////////
/// Message Hide Editor

bool CCLUVizAppListMT::MsgHideEditor()
{
	try
	{
		volatile CValData<int>* pData = dynamic_cast<volatile CValData<int>*>(m_pMsgData);

		if (!pData)
		{
			m_sLastError = "Invalid data pointer for create process.";
			throw CException();
		}

		int iHandle = int(pData->Val);
		TAppMap::iterator itEl;

		if ((itEl = m_mapApp.find(iHandle)) == m_mapApp.end())
		{
			// Handle not found
			m_sLastError = "Invalid handle.";
			throw CException();
		}

		CCLUVizApp* pApp = itEl->second;

		if (!pApp->HideEditor())
		{
			m_sLastError = pApp->GetLastError();
			throw CException();
		}

		m_bMsgSuccess = true;
	}
	catch (...)
	{
		m_bMsgSuccess = false;
	}

	return m_bMsgSuccess;
}

//////////////////////////////////////////////////////////////////////
/// Show Window

bool CCLUVizAppListMT::ShowWindow(int iHandle)
{
	if (!m_bIsRunning)
	{
		m_sLastError = "CLUViz is not running.";
		return false;
	}

	CValData<int> Data(iHandle);

	GET_ACCESS_MSG

		m_iMsgID = int(m_pcMsgID[APP_MSG_WNDSHOW]);
	m_pMsgData = (CMsgData*) &Data;

	RELEASE_ACCESS
		SwitchToThread();

	WaitForEventMsg(m_uEventTimeout);
	ResetEvent(m_hEventMsg);

	RELEASE_MSG

	return m_bMsgSuccess;
}

//////////////////////////////////////////////////////////////////////
/// Message Show Window

bool CCLUVizAppListMT::MsgShowWindow()
{
	try
	{
		volatile CValData<int>* pData = dynamic_cast<volatile CValData<int>*>(m_pMsgData);

		if (!pData)
		{
			m_sLastError = "Invalid data pointer for create process.";
			throw CException();
		}

		int iHandle = int(pData->Val);
		TAppMap::iterator itEl;

		if ((itEl = m_mapApp.find(iHandle)) == m_mapApp.end())
		{
			// Handle not found
			m_sLastError = "Invalid handle.";
			throw CException();
		}

		CCLUVizApp* pApp = itEl->second;

		if (!pApp->ShowWindow())
		{
			m_sLastError = pApp->GetLastError();
			throw CException();
		}

		m_bMsgSuccess = true;
	}
	catch (...)
	{
		m_bMsgSuccess = false;
	}

	return m_bMsgSuccess;
}

//////////////////////////////////////////////////////////////////////
/// Hide Window

bool CCLUVizAppListMT::HideWindow(int iHandle)
{
	if (!m_bIsRunning)
	{
		m_sLastError = "CLUViz is not running.";
		return false;
	}

	CValData<int> Data(iHandle);

	GET_ACCESS_MSG

		m_iMsgID = int(m_pcMsgID[APP_MSG_WNDHIDE]);
	m_pMsgData = (CMsgData*) &Data;

	RELEASE_ACCESS
		SwitchToThread();

	WaitForEventMsg(m_uEventTimeout);
	ResetEvent(m_hEventMsg);

	RELEASE_MSG

	return m_bMsgSuccess;
}

//////////////////////////////////////////////////////////////////////
/// Message Hide Window

bool CCLUVizAppListMT::MsgHideWindow()
{
	try
	{
		volatile CValData<int>* pData = dynamic_cast<volatile CValData<int>*>(m_pMsgData);

		if (!pData)
		{
			m_sLastError = "Invalid data pointer for create process.";
			throw CException();
		}

		int iHandle = int(pData->Val);
		TAppMap::iterator itEl;

		if ((itEl = m_mapApp.find(iHandle)) == m_mapApp.end())
		{
			// Handle not found
			m_sLastError = "Invalid handle.";
			throw CException();
		}

		CCLUVizApp* pApp = itEl->second;

		if (!pApp->HideWindow())
		{
			m_sLastError = pApp->GetLastError();
			throw CException();
		}

		m_bMsgSuccess = true;
	}
	catch (...)
	{
		m_bMsgSuccess = false;
	}

	return m_bMsgSuccess;
}

/////////////////////////////////////////////////////////////////////
/// Set Window Position and Size

bool CCLUVizAppListMT::SetWindowPosSize(int iHandle, int iX, int iY, int iWidth, int iHeight)
{
	if (!m_bIsRunning)
	{
		m_sLastError = "CLUViz is not running.";
		return false;
	}

	CHandleValData<SWndPosSizeData> Data(iHandle, SWndPosSizeData(iX, iY, iWidth, iHeight));

	GET_ACCESS_MSG

		m_iMsgID = int(m_pcMsgID[APP_MSG_WNDPOSSIZE]);
	m_pMsgData = (CMsgData*) &Data;

	RELEASE_ACCESS
		SwitchToThread();

	WaitForEventMsg(m_uEventTimeout);
	ResetEvent(m_hEventMsg);

	RELEASE_MSG

	return m_bMsgSuccess;
}

//////////////////////////////////////////////////////////////////////
/// Execute Message Set Window Position and Size

bool CCLUVizAppListMT::MsgSetWndPosSize()
{
	try
	{
		volatile CHandleValData<SWndPosSizeData>* pData = dynamic_cast< volatile CHandleValData<SWndPosSizeData>* >(m_pMsgData);

		if (!pData)
		{
			m_sLastError = "Invalid data pointer for window position and size process.";
			throw CException();
		}

		int iHandle = pData->iHandle;

		TAppMap::iterator itEl;

		if ((itEl = m_mapApp.find(iHandle)) == m_mapApp.end())
		{
			// Handle not found
			m_sLastError = "Invalid handle.";
			throw CException();
		}

		CCLUVizApp* pApp = itEl->second;

		if (!pApp->SetWindowPosSize(pData->Val.iX, pData->Val.iY, pData->Val.iWidth, pData->Val.iHeight))
		{
			m_sLastError = pApp->GetLastError();
			throw CException();
		}

		m_bMsgSuccess = true;
	}
	catch (...)
	{
		m_bMsgSuccess = false;
	}

	return m_bMsgSuccess;
}

/////////////////////////////////////////////////////////////////////
/// Set Script

bool CCLUVizAppListMT::SetScript(int iHandle, const char* pcScript)
{
	if (!m_bIsRunning)
	{
		m_sLastError = "CLUViz is not running.";
		return false;
	}

	CHandleValData<const char*> Data(iHandle, pcScript);

	GET_ACCESS_MSG

		m_iMsgID = int(m_pcMsgID[APP_MSG_SETSCRIPT]);
	m_pMsgData = (CMsgData*) &Data;

	RELEASE_ACCESS
		SwitchToThread();

	WaitForEventMsg(m_uEventTimeout);
	ResetEvent(m_hEventMsg);

	RELEASE_MSG

	return m_bMsgSuccess;
}

//////////////////////////////////////////////////////////////////////
/// Message Set Script

bool CCLUVizAppListMT::MsgSetScript()
{
	try
	{
		volatile CHandleValData<const char*>* pData =
			dynamic_cast<volatile CHandleValData<const char*>*>(m_pMsgData);

		if (!pData)
		{
			m_sLastError = "Invalid data pointer for create process.";
			throw CException();
		}

		int iHandle = pData->iHandle;

		TAppMap::iterator itEl;

		if ((itEl = m_mapApp.find(iHandle)) == m_mapApp.end())
		{
			// Handle not found
			m_sLastError = "Invalid handle.";
			throw CException();
		}

		CCLUVizApp* pApp = itEl->second;

		if (!pApp->SetScript(pData->Val))
		{
			m_sLastError = pApp->GetLastError();
			throw CException();
		}

		m_bMsgSuccess = true;
	}
	catch (...)
	{
		m_bMsgSuccess = false;
	}

	return m_bMsgSuccess;
}

///////////////////////////////////////////////////////////////////////
/// Get Script Output
bool CCLUVizAppListMT::GetScriptOutput(int iHandle, std::vector<std::string>& vecText, bool& bIsError)
{
	if (!m_bIsRunning)
	{
		m_sLastError = "CLUViz is not running.";
		return false;
	}

	SGetScriptOutputData ScriptData(&vecText, &bIsError);
	CHandleValData<SGetScriptOutputData> Data(iHandle, ScriptData);

	GET_ACCESS_MSG

		m_iMsgID = int(m_pcMsgID[APP_MSG_GETSCRIPTOUTPUT]);
	m_pMsgData = (CMsgData*) &Data;

	RELEASE_ACCESS
		SwitchToThread();

	WaitForEventMsg(m_uEventTimeout);
	ResetEvent(m_hEventMsg);

	RELEASE_MSG

	return m_bMsgSuccess;
}

bool CCLUVizAppListMT::MsgGetScriptOutput()
{
	try
	{
		volatile CHandleValData<SGetScriptOutputData>* pData =
			dynamic_cast<volatile CHandleValData<SGetScriptOutputData>*>(m_pMsgData);

		if (!pData)
		{
			m_sLastError = "Invalid data pointer for create process.";
			throw CException();
		}

		int iHandle = pData->iHandle;

		TAppMap::iterator itEl;

		if ((itEl = m_mapApp.find(iHandle)) == m_mapApp.end())
		{
			// Handle not found
			m_sLastError = "Invalid handle.";
			throw CException();
		}

		CCLUVizApp* pApp = itEl->second;

		if (!pApp->GetScriptOutput(*pData->Val.pvecText, *pData->Val.pbIsError))
		{
			m_sLastError = pApp->GetLastError();
			throw CException();
		}

		m_bMsgSuccess = true;
	}
	catch (...)
	{
		m_bMsgSuccess = false;
	}

	return m_bMsgSuccess;
}

/////////////////////////////////////////////////////////////////////
/// Load Script

bool CCLUVizAppListMT::LoadScript(int iHandle, const char* pcFilename, const char* pcPass)
{
	if (!m_bIsRunning)
	{
		m_sLastError = "CLUViz is not running.";
		return false;
	}

	if (!pcFilename)
	{
		m_sLastError = "Invalid script filename";
		return false;
	}

	const char* pcPassKey = 0;
	string sFile, sPass;
	sFile = pcFilename;

	if (pcPass)
	{
		sPass     = pcPass;
		pcPassKey = sPass.c_str();
	}

	if ((strchr(pcFilename, ':') == 0) && (m_sScriptPath.length() > 0))
	{
		sFile = m_sScriptPath + sFile;
	}

	CHandleValData<SScriptData> Data(iHandle, SScriptData(sFile.c_str(), pcPassKey));

	GET_ACCESS_MSG

		m_iMsgID = int(m_pcMsgID[APP_MSG_LOADSCRIPT]);
	m_pMsgData = (CMsgData*) &Data;

	RELEASE_ACCESS
		SwitchToThread();

	WaitForEventMsg(m_uEventTimeout);
	ResetEvent(m_hEventMsg);

	RELEASE_MSG

	return m_bMsgSuccess;
}

/////////////////////////////////////////////////////////////////////
/// Load Script

bool CCLUVizAppListMT::LoadScript(int iHandle, const char* pcFilename, const char* pcPass, int iArgCnt, const char* pcArg)
{
	if (!m_bIsRunning)
	{
		m_sLastError = "CLUViz is not running.";
		return false;
	}

	if (!pcFilename)
	{
		m_sLastError = "Invalid script filename";
		return false;
	}

	const char* pcPassKey = 0;
	string sFile, sPass, sArg;
	sFile = pcFilename;

	if (pcPass)
	{
		sPass     = pcPass;
		pcPassKey = sPass.c_str();
	}

	if ((strchr(pcFilename, ':') == 0) && (m_sScriptPath.length() > 0))
	{
		sFile = m_sScriptPath + sFile;
	}

	sArg = pcArg;

	CHandleValData<SScriptData> Data(iHandle, SScriptData(sFile.c_str(), pcPassKey, iArgCnt, sArg.c_str()));

	GET_ACCESS_MSG

		m_iMsgID = int(m_pcMsgID[APP_MSG_LOADSCRIPT]);
	m_pMsgData = (CMsgData*) &Data;

	RELEASE_ACCESS
		SwitchToThread();

	WaitForEventMsg(m_uEventTimeout);
	ResetEvent(m_hEventMsg);

	RELEASE_MSG

	return m_bMsgSuccess;
}

//////////////////////////////////////////////////////////////////////
/// Message Load Script

bool CCLUVizAppListMT::MsgLoadScript()
{
	try
	{
		volatile CHandleValData<SScriptData>* pData
			= dynamic_cast<volatile CHandleValData<SScriptData>*>(m_pMsgData);

		if (!pData)
		{
			m_sLastError = "Invalid data pointer for create process.";
			throw CException();
		}

		int iHandle = pData->iHandle;

		TAppMap::iterator itEl;

		if ((itEl = m_mapApp.find(iHandle)) == m_mapApp.end())
		{
			// Handle not found
			m_sLastError = "Invalid handle.";
			throw CException();
		}

		CCLUVizApp* pApp = itEl->second;

		if (!pApp->LoadScript(pData->Val.pcFilename, pData->Val.pcPass, pData->Val.iArgCnt, pData->Val.pcArg))
		{
			m_sLastError = pApp->GetLastError();
			throw CException();
		}

		m_bMsgSuccess = true;
	}
	catch (...)
	{
		m_bMsgSuccess = false;
	}

	return m_bMsgSuccess;
}

/////////////////////////////////////////////////////////////////////
/// Exec User

bool CCLUVizAppListMT::ExecUser(int iHandle, const char* pcToolName)
{
	if (!m_bIsRunning)
	{
		m_sLastError = "CLUViz is not running.";
		return false;
	}

	CHandleValData<const char*> Data(iHandle, pcToolName);

	GET_ACCESS_MSG

		m_iMsgID = int(m_pcMsgID[APP_MSG_EXECUSER]);
	m_pMsgData = (CMsgData*) &Data;

	RELEASE_ACCESS
		SwitchToThread();

	WaitForEventMsg(m_uEventTimeout);
	ResetEvent(m_hEventMsg);

	RELEASE_MSG

	return m_bMsgSuccess;
}

//////////////////////////////////////////////////////////////////////
/// Message Load Script

bool CCLUVizAppListMT::MsgExecUser()
{
	try
	{
		volatile CHandleValData<const char*>* pData
			= dynamic_cast<volatile CHandleValData<const char*>*>(m_pMsgData);

		if (!pData)
		{
			m_sLastError = "Invalid data pointer for create process.";
			throw CException();
		}

		int iHandle = pData->iHandle;

		TAppMap::iterator itEl;

		if ((itEl = m_mapApp.find(iHandle)) == m_mapApp.end())
		{
			// Handle not found
			m_sLastError = "Invalid handle.";
			throw CException();
		}

		CCLUVizApp* pApp = itEl->second;

		if (!pApp->ExecUser(pData->Val))
		{
			m_sLastError = pApp->GetLastError();
			throw CException();
		}

		m_bMsgSuccess = true;
	}
	catch (...)
	{
		m_bMsgSuccess = false;
	}

	return m_bMsgSuccess;
}

/////////////////////////////////////////////////////////////////////
/// Full Screen

bool CCLUVizAppListMT::FullScreen(int iHandle, bool bVal, bool bEnableUserToggle)
{
	if (!m_bIsRunning)
	{
		m_sLastError = "CLUViz is not running.";
		return false;
	}

	CHandleValData<SFullScreenData> Data(iHandle, SFullScreenData(bVal, bEnableUserToggle));

	GET_ACCESS_MSG

		m_iMsgID = int(m_pcMsgID[APP_MSG_FULLSCREEN]);
	m_pMsgData = (CMsgData*) &Data;

	RELEASE_ACCESS
		SwitchToThread();

	WaitForEventMsg(m_uEventTimeout);
	ResetEvent(m_hEventMsg);

	RELEASE_MSG

	return m_bMsgSuccess;
}

//////////////////////////////////////////////////////////////////////
/// Message Load Script

bool CCLUVizAppListMT::MsgFullScreen()
{
	try
	{
		volatile CHandleValData<SFullScreenData>* pData
			= dynamic_cast<volatile CHandleValData<SFullScreenData>*>(m_pMsgData);

		if (!pData)
		{
			m_sLastError = "Invalid data pointer for create process.";
			throw CException();
		}

		int iHandle = pData->iHandle;

		TAppMap::iterator itEl;

		if ((itEl = m_mapApp.find(iHandle)) == m_mapApp.end())
		{
			// Handle not found
			m_sLastError = "Invalid handle.";
			throw CException();
		}

		CCLUVizApp* pApp = itEl->second;

		if (!pApp->FullScreen(pData->Val.bEnable, pData->Val.bEnableUserToggle))
		{
			m_sLastError = pApp->GetLastError();
			throw CException();
		}

		m_bMsgSuccess = true;
	}
	catch (...)
	{
		m_bMsgSuccess = false;
	}

	return m_bMsgSuccess;
}

/////////////////////////////////////////////////////////////////////
/// Set Var Int

bool CCLUVizAppListMT::SetVar(int iHandle, const char* pcVarName, int iVal)
{
	if (!m_bIsRunning)
	{
		m_sLastError = "CLUViz is not running.";
		return false;
	}

	CHandleValData<SVarData<int> >
	Data(iHandle, SVarData<int>(pcVarName, &iVal));

	GET_ACCESS_MSG

		m_iMsgID = int(m_pcMsgID[APP_MSG_SETVAR]);
	m_pMsgData = (CMsgData*) &Data;

	RELEASE_ACCESS
		SwitchToThread();

	WaitForEventMsg(m_uEventTimeout);
	ResetEvent(m_hEventMsg);

	RELEASE_MSG

	return m_bMsgSuccess;
}

/////////////////////////////////////////////////////////////////////
/// Set Var Double

bool CCLUVizAppListMT::SetVar(int iHandle, const char* pcVarName, double dVal)
{
	if (!m_bIsRunning)
	{
		m_sLastError = "CLUViz is not running.";
		return false;
	}

	CHandleValData<SVarData<double> >
	Data(iHandle, SVarData<double>(pcVarName, &dVal));

	GET_ACCESS_MSG

		m_iMsgID = int(m_pcMsgID[APP_MSG_SETVAR]);
	m_pMsgData = (CMsgData*) &Data;

	RELEASE_ACCESS
		SwitchToThread();

	WaitForEventMsg(m_uEventTimeout);
	ResetEvent(m_hEventMsg);

	RELEASE_MSG

	return m_bMsgSuccess;
}

/////////////////////////////////////////////////////////////////////
/// Set Var String

bool CCLUVizAppListMT::SetVar(int iHandle, const char* pcVarName, const string& sVal)
{
	if (!m_bIsRunning)
	{
		m_sLastError = "CLUViz is not running.";
		return false;
	}

	CHandleValData<SVarData<const string> >
	Data(iHandle, SVarData<const string>(pcVarName, &sVal));

	GET_ACCESS_MSG

		m_iMsgID = int(m_pcMsgID[APP_MSG_SETVAR]);
	m_pMsgData = (CMsgData*) &Data;

	RELEASE_ACCESS
		SwitchToThread();

	WaitForEventMsg(m_uEventTimeout);
	ResetEvent(m_hEventMsg);

	RELEASE_MSG

	return m_bMsgSuccess;
}

/////////////////////////////////////////////////////////////////////
/// Set Var Tensor

bool CCLUVizAppListMT::SetVar(int iHandle, const char* pcVarName, int iDimCnt, const int* piDim, const double* pdData)
{
	if (!m_bIsRunning)
	{
		m_sLastError = "CLUViz is not running.";
		return false;
	}

	SSetTensorData TensorData(iDimCnt, piDim, pdData);

	CHandleValData<SVarData<SSetTensorData> >
	Data(iHandle, SVarData<SSetTensorData>(pcVarName, &TensorData));

	GET_ACCESS_MSG

		m_iMsgID = int(m_pcMsgID[APP_MSG_SETVAR]);
	m_pMsgData = (CMsgData*) &Data;

	RELEASE_ACCESS
		SwitchToThread();

	WaitForEventMsg(m_uEventTimeout);
	ResetEvent(m_hEventMsg);

	RELEASE_MSG

	return m_bMsgSuccess;
}

//////////////////////////////////////////////////////////////////////
// Set Image Data

bool CCLUVizAppListMT::SetVar(int iHandle, const char* pcVarName, const Clu::CIImage& xImage)
{
	if (!m_bIsRunning)
	{
		m_sLastError = "CLUViz is not running.";
		return false;
	}

	CHandleValData<SVarData<Clu::CIImage> >
	Data(iHandle, SVarData<Clu::CIImage>(pcVarName, (Clu::CIImage*) &xImage));

	GET_ACCESS_MSG

		m_iMsgID = int(m_pcMsgID[APP_MSG_SETVAR]);
	m_pMsgData = (CMsgData*) &Data;

	RELEASE_ACCESS
		SwitchToThread();

	WaitForEventMsg(m_uEventTimeout);
	ResetEvent(m_hEventMsg);

	RELEASE_MSG

	return m_bMsgSuccess;
}

//////////////////////////////////////////////////////////////////////
// Set Color Data

bool CCLUVizAppListMT::SetVar(int iHandle, const char* pcVarName, double dRed, double dGreen, double dBlue, double dAlpha)
{
	if (!m_bIsRunning)
	{
		m_sLastError = "CLUViz is not running.";
		return false;
	}

	SColorData ColData(dRed, dGreen, dBlue, dAlpha);

	CHandleValData<SVarData<SColorData> >
	Data(iHandle, SVarData<SColorData>(pcVarName, &ColData));

	GET_ACCESS_MSG

		m_iMsgID = int(m_pcMsgID[APP_MSG_SETVAR]);
	m_pMsgData = (CMsgData*) &Data;

	RELEASE_ACCESS
		SwitchToThread();

	WaitForEventMsg(m_uEventTimeout);
	ResetEvent(m_hEventMsg);

	RELEASE_MSG

	return m_bMsgSuccess;
}

///////////////////////////////////////////////////////////////////////
// Set vertex array data
bool CCLUVizAppListMT::SetVar(int iHandle, const char* pcVarName, Clu::SStdVbo* pVexArray)
{
	if (!m_bIsRunning)
	{
		m_sLastError = "CLUViz is not running.";
		return false;
	}

	CHandleValData<SVarData<Clu::SStdVbo> >
	Data(iHandle, SVarData<Clu::SStdVbo>(pcVarName, pVexArray));

	GET_ACCESS_MSG

		m_iMsgID = int(m_pcMsgID[APP_MSG_SETVAR]);
	m_pMsgData = (CMsgData*) &Data;

	RELEASE_ACCESS
		SwitchToThread();

	WaitForEventMsg(m_uEventTimeout);
	ResetEvent(m_hEventMsg);

	RELEASE_MSG

	return m_bMsgSuccess;
}

//////////////////////////////////////////////////////////////////////
/// Message Set Variable

bool CCLUVizAppListMT::MsgSetVar()
{
	if (dynamic_cast<volatile CHandleValData<SVarData<int> >*>(m_pMsgData))
	{
		return DoSetVar(dynamic_cast<volatile CHandleValData<SVarData<int> >*>(m_pMsgData));
	}
	else if (dynamic_cast<volatile CHandleValData<SVarData<double> >*>(m_pMsgData))
	{
		return DoSetVar(dynamic_cast<volatile CHandleValData<SVarData<double> >*>(m_pMsgData));
	}
	else if (dynamic_cast<volatile CHandleValData<SVarData<const string> >*>(m_pMsgData))
	{
		return DoSetVar(dynamic_cast<volatile CHandleValData<SVarData<const string> >*>(m_pMsgData));
	}
	else if (dynamic_cast<volatile CHandleValData<SVarData<SSetTensorData> >*>(m_pMsgData))
	{
		return DoSetVar(dynamic_cast<volatile CHandleValData<SVarData<SSetTensorData> >*>(m_pMsgData));
	}
	else if (dynamic_cast<volatile CHandleValData<SVarData<Clu::CIImage> >*>(m_pMsgData))
	{
		return DoSetVar(dynamic_cast<volatile CHandleValData<SVarData<Clu::CIImage> >*>(m_pMsgData));
	}
	else if (dynamic_cast<volatile CHandleValData<SVarData<SColorData> >*>(m_pMsgData))
	{
		return DoSetVar(dynamic_cast<volatile CHandleValData<SVarData<SColorData> >*>(m_pMsgData));
	}
	else if (dynamic_cast<volatile CHandleValData<SVarData<Clu::SStdVbo> >*>(m_pMsgData))
	{
		return DoSetVar(dynamic_cast<volatile CHandleValData<SVarData<Clu::SStdVbo> >*>(m_pMsgData));
	}
	else
	{
		m_sLastError = "Variable data type not supported.";
		return false;
	}
}

template<class T>
bool CCLUVizAppListMT::DoSetVar(volatile CHandleValData<SVarData< T > >* pData)
{
	try
	{
		int iHandle = pData->iHandle;

		TAppMap::iterator itEl;

		if ((itEl = m_mapApp.find(iHandle)) == m_mapApp.end())
		{
			// Handle not found
			m_sLastError = "Invalid handle.";
			throw CException();
		}

		CCLUVizApp* pApp = itEl->second;

		if (!pApp->SetVar(pData->Val.pcVarName, *pData->Val.pVar))
		{
			m_sLastError = pApp->GetLastError();
			throw CException();
		}

		m_bMsgSuccess = true;
	}
	catch (...)
	{
		m_bMsgSuccess = false;
	}

	return m_bMsgSuccess;
}

//template<class T>
bool CCLUVizAppListMT::DoSetVar(volatile CHandleValData<SVarData< SSetTensorData > >* pData)
{
	try
	{
		int iHandle = pData->iHandle;

		TAppMap::iterator itEl;

		if ((itEl = m_mapApp.find(iHandle)) == m_mapApp.end())
		{
			// Handle not found
			m_sLastError = "Invalid handle.";
			throw CException();
		}

		CCLUVizApp* pApp = itEl->second;

		SSetTensorData* pTData = pData->Val.pVar;

		if (!pApp->SetVar(pData->Val.pcVarName, pTData->iDimCnt, pTData->piDim, pTData->pdData))
		{
			m_sLastError = pApp->GetLastError();
			throw CException();
		}

		m_bMsgSuccess = true;
	}
	catch (...)
	{
		m_bMsgSuccess = false;
	}

	return m_bMsgSuccess;
}

bool CCLUVizAppListMT::DoSetVar(volatile CHandleValData<SVarData< Clu::CIImage > >* pData)
{
	try
	{
		int iHandle = pData->iHandle;

		TAppMap::iterator itEl;

		if ((itEl = m_mapApp.find(iHandle)) == m_mapApp.end())
		{
			// Handle not found
			m_sLastError = "Invalid handle.";
			throw CException();
		}

		CCLUVizApp* pApp = itEl->second;

		const Clu::CIImage* pImage = pData->Val.pVar;

		if (!pApp->SetVar(pData->Val.pcVarName, *pImage))
		{
			m_sLastError = pApp->GetLastError();
			throw CException();
		}

		m_bMsgSuccess = true;
	}
	catch (...)
	{
		m_bMsgSuccess = false;
	}

	return m_bMsgSuccess;
}

bool CCLUVizAppListMT::DoSetVar(volatile CHandleValData<SVarData< SColorData > >* pData)
{
	try
	{
		int iHandle = pData->iHandle;

		TAppMap::iterator itEl;

		if ((itEl = m_mapApp.find(iHandle)) == m_mapApp.end())
		{
			// Handle not found
			m_sLastError = "Invalid handle.";
			throw CException();
		}

		CCLUVizApp* pApp = itEl->second;

		SColorData* pColData = pData->Val.pVar;

		if (!pApp->SetVar(pData->Val.pcVarName, pColData->pdColor))
		{
			m_sLastError = pApp->GetLastError();
			throw CException();
		}

		m_bMsgSuccess = true;
	}
	catch (...)
	{
		m_bMsgSuccess = false;
	}

	return m_bMsgSuccess;
}

bool CCLUVizAppListMT::DoSetVar(volatile CHandleValData<SVarData< Clu::SStdVbo > >* pData)
{
	try
	{
		int iHandle = pData->iHandle;

		TAppMap::iterator itEl;

		if ((itEl = m_mapApp.find(iHandle)) == m_mapApp.end())
		{
			// Handle not found
			m_sLastError = "Invalid handle.";
			throw CException();
		}

		CCLUVizApp* pApp = itEl->second;

		Clu::SStdVbo* pObjData = pData->Val.pVar;

		if (!pApp->SetVar(pData->Val.pcVarName, pObjData))
		{
			m_sLastError = pApp->GetLastError();
			throw CException();
		}

		m_bMsgSuccess = true;
	}
	catch (...)
	{
		m_bMsgSuccess = false;
	}

	return m_bMsgSuccess;
}

/////////////////////////////////////////////////////////////////////
/// Get Var Int

bool CCLUVizAppListMT::GetVar(int iHandle, const char* pcVarName, int& iVal)
{
	if (!m_bIsRunning)
	{
		m_sLastError = "CLUViz is not running.";
		return false;
	}

	CHandleValData<SVarData<int> >
	Data(iHandle, SVarData<int>(pcVarName, &iVal));

	GET_ACCESS_MSG

		m_iMsgID = int(m_pcMsgID[APP_MSG_GETVAR]);
	m_pMsgData = (CMsgData*) &Data;

	RELEASE_ACCESS
		SwitchToThread();

	WaitForEventMsg(m_uEventTimeout);
	ResetEvent(m_hEventMsg);

	RELEASE_MSG

	return m_bMsgSuccess;
}

/////////////////////////////////////////////////////////////////////
/// Get Var Double

bool CCLUVizAppListMT::GetVar(int iHandle, const char* pcVarName, double& dVal)
{
	if (!m_bIsRunning)
	{
		m_sLastError = "CLUViz is not running.";
		return false;
	}

	CHandleValData<SVarData<double> >
	Data(iHandle, SVarData<double>(pcVarName, &dVal));

	GET_ACCESS_MSG

		m_iMsgID = int(m_pcMsgID[APP_MSG_GETVAR]);
	m_pMsgData = (CMsgData*) &Data;

	RELEASE_ACCESS
		SwitchToThread();

	WaitForEventMsg(m_uEventTimeout);
	ResetEvent(m_hEventMsg);

	RELEASE_MSG

	return m_bMsgSuccess;
}

/////////////////////////////////////////////////////////////////////
/// Get Var String

bool CCLUVizAppListMT::GetVar(int iHandle, const char* pcVarName, string& sVal)
{
	if (!m_bIsRunning)
	{
		m_sLastError = "CLUViz is not running.";
		return false;
	}

	CHandleValData<SVarData<string> >
	Data(iHandle, SVarData<string>(pcVarName, &sVal));

	GET_ACCESS_MSG

		m_iMsgID = int(m_pcMsgID[APP_MSG_GETVAR]);
	m_pMsgData = (CMsgData*) &Data;

	RELEASE_ACCESS
		SwitchToThread();

	WaitForEventMsg(m_uEventTimeout);
	ResetEvent(m_hEventMsg);

	RELEASE_MSG

	return m_bMsgSuccess;
}

/////////////////////////////////////////////////////////////////////
/// Get Var Tensor

bool CCLUVizAppListMT::GetVar(int iHandle, const char* pcVarName, vector<int>& vecDim, vector<double>& vecData)
{
	if (!m_bIsRunning)
	{
		m_sLastError = "CLUViz is not running.";
		return false;
	}

	SGetTensorData TensorData(&vecDim, &vecData);

	CHandleValData<SVarData<SGetTensorData> >
	Data(iHandle, SVarData<SGetTensorData>(pcVarName, &TensorData));

	GET_ACCESS_MSG

		m_iMsgID = int(m_pcMsgID[APP_MSG_GETVAR]);
	m_pMsgData = (CMsgData*) &Data;

	RELEASE_ACCESS
		SwitchToThread();

	WaitForEventMsg(m_uEventTimeout);
	ResetEvent(m_hEventMsg);

	RELEASE_MSG

	return m_bMsgSuccess;
}

//////////////////////////////////////////////////////////////////////
// Get Tensor Properties
bool CCLUVizAppListMT::GetVar(int iHandle, const char* pcVarName, unsigned& uDimCnt, unsigned& uElCnt)
{
	if (!m_bIsRunning)
	{
		m_sLastError = "CLUViz is not running.";
		return false;
	}

	STensorProp TensorProp;

	CHandleValData<SVarData<STensorProp> >
	Data(iHandle, SVarData<STensorProp>(pcVarName, &TensorProp));

	GET_ACCESS_MSG

		m_iMsgID = int(m_pcMsgID[APP_MSG_GETVAR]);
	m_pMsgData = (CMsgData*) &Data;

	RELEASE_ACCESS
		SwitchToThread();

	WaitForEventMsg(m_uEventTimeout);
	ResetEvent(m_hEventMsg);

	RELEASE_MSG

		uDimCnt = TensorProp.uDimCnt;
	uElCnt = TensorProp.uElCnt;

	return m_bMsgSuccess;
}

//////////////////////////////////////////////////////////////////////
// Get Tensor Data
bool CCLUVizAppListMT::GetVar(int iHandle, const char* pcVarName, unsigned* puDim, double* pdData)
{
	if (!m_bIsRunning)
	{
		m_sLastError = "CLUViz is not running.";
		return false;
	}

	STensorData TensorData(puDim, pdData);

	CHandleValData<SVarData<STensorData> >
	Data(iHandle, SVarData<STensorData>(pcVarName, &TensorData));

	GET_ACCESS_MSG

		m_iMsgID = int(m_pcMsgID[APP_MSG_GETVAR]);
	m_pMsgData = (CMsgData*) &Data;

	RELEASE_ACCESS
		SwitchToThread();

	WaitForEventMsg(m_uEventTimeout);
	ResetEvent(m_hEventMsg);

	RELEASE_MSG

	return m_bMsgSuccess;
}

//////////////////////////////////////////////////////////////////////
// Get image properties

bool CCLUVizAppListMT::GetVar(int iHandle, const char* pcVarName, Clu::CIImage& xImage)
{
	if (!m_bIsRunning)
	{
		m_sLastError = "CLUViz is not running.";
		return false;
	}

	CHandleValData<SVarData<Clu::CIImage> >
	Data(iHandle, SVarData<Clu::CIImage>(pcVarName, &xImage));

	GET_ACCESS_MSG

		m_iMsgID = int(m_pcMsgID[APP_MSG_GETVAR]);
	m_pMsgData = (CMsgData*) &Data;

	RELEASE_ACCESS
		SwitchToThread();

	WaitForEventMsg(m_uEventTimeout);
	ResetEvent(m_hEventMsg);


	RELEASE_MSG

	return m_bMsgSuccess;
}

//////////////////////////////////////////////////////////////////////
// Get image data

bool CCLUVizAppListMT::GetVar(int iHandle, const char* pcVarName, void* pvData)
{
	if (!m_bIsRunning)
	{
		m_sLastError = "CLUViz is not running.";
		return false;
	}

	CHandleValData<SVarData<void*> >
	Data(iHandle, SVarData<void*>(pcVarName, &pvData));

	GET_ACCESS_MSG

		m_iMsgID = int(m_pcMsgID[APP_MSG_GETVAR]);
	m_pMsgData = (CMsgData*) &Data;

	RELEASE_ACCESS
		SwitchToThread();

	WaitForEventMsg(m_uEventTimeout);
	ResetEvent(m_hEventMsg);

	RELEASE_MSG

	return m_bMsgSuccess;
}

//////////////////////////////////////////////////////////////////////
// Get color data

bool CCLUVizAppListMT::GetVar(int iHandle, const char* pcVarName, double& dRed, double& dGreen, double& dBlue, double& dAlpha)
{
	if (!m_bIsRunning)
	{
		m_sLastError = "CLUViz is not running.";
		return false;
	}

	SColorData ColData;

	CHandleValData<SVarData<SColorData> >
	Data(iHandle, SVarData<SColorData>(pcVarName, &ColData));

	GET_ACCESS_MSG

		m_iMsgID = int(m_pcMsgID[APP_MSG_GETVAR]);
	m_pMsgData = (CMsgData*) &Data;

	RELEASE_ACCESS
		SwitchToThread();

	WaitForEventMsg(m_uEventTimeout);
	ResetEvent(m_hEventMsg);

	dRed   = ColData.pdColor[0];
	dGreen = ColData.pdColor[1];
	dBlue  = ColData.pdColor[2];
	dAlpha = ColData.pdColor[3];

	RELEASE_MSG

	return m_bMsgSuccess;
}

//////////////////////////////////////////////////////////////////////
/// Message Get Var

bool CCLUVizAppListMT::MsgGetVar()
{
	if (dynamic_cast<volatile CHandleValData<SVarData<int> >*>(m_pMsgData))
	{
		return DoGetVar(dynamic_cast<volatile CHandleValData<SVarData<int> >*>(m_pMsgData));
	}
	else if (dynamic_cast<volatile CHandleValData<SVarData<double> >*>(m_pMsgData))
	{
		return DoGetVar(dynamic_cast<volatile CHandleValData<SVarData<double> >*>(m_pMsgData));
	}
	else if (dynamic_cast<volatile CHandleValData<SVarData<string> >*>(m_pMsgData))
	{
		return DoGetVar(dynamic_cast<volatile CHandleValData<SVarData<string> >*>(m_pMsgData));
	}
	else if (dynamic_cast<volatile CHandleValData<SVarData<SGetTensorData> >*>(m_pMsgData))
	{
		return DoGetVar(dynamic_cast<volatile CHandleValData<SVarData<SGetTensorData> >*>(m_pMsgData));
	}
	else if (dynamic_cast<volatile CHandleValData<SVarData<STensorProp> >*>(m_pMsgData))
	{
		return DoGetVar(dynamic_cast<volatile CHandleValData<SVarData<STensorProp> >*>(m_pMsgData));
	}
	else if (dynamic_cast<volatile CHandleValData<SVarData<STensorData> >*>(m_pMsgData))
	{
		return DoGetVar(dynamic_cast<volatile CHandleValData<SVarData<STensorData> >*>(m_pMsgData));
	}
	else if (dynamic_cast<volatile CHandleValData<SVarData<Clu::CIImage> >*>(m_pMsgData))
	{
		return DoGetVar(dynamic_cast<volatile CHandleValData<SVarData<Clu::CIImage> >*>(m_pMsgData));
	}
	//else if (dynamic_cast<volatile CHandleValData<SVarData<void*> >*>(m_pMsgData))
	//{
	//	return DoGetVar(dynamic_cast<volatile CHandleValData<SVarData<void*> >*>(m_pMsgData));
	//}
	else if (dynamic_cast<volatile CHandleValData<SVarData<SColorData> >*>(m_pMsgData))
	{
		return DoGetVar(dynamic_cast<volatile CHandleValData<SVarData<SColorData> >*>(m_pMsgData));
	}
	else
	{
		m_sLastError = "Variable data type not supported.";
		return false;
	}
}


template<class T>
bool CCLUVizAppListMT::DoGetVar(volatile CHandleValData<SVarData< T > >* pData)
{
	try
	{
		int iHandle = pData->iHandle;

		TAppMap::iterator itEl;

		if ((itEl = m_mapApp.find(iHandle)) == m_mapApp.end())
		{
			// Handle not found
			m_sLastError = "Invalid handle.";
			throw CException();
		}

		CCLUVizApp* pApp = itEl->second;

		if (!pApp->GetVar(pData->Val.pcVarName, *pData->Val.pVar))
		{
			m_sLastError = pApp->GetLastError();
			throw CException();
		}

		m_bMsgSuccess = true;
	}
	catch (...)
	{
		m_bMsgSuccess = false;
	}

	return m_bMsgSuccess;
}


bool CCLUVizAppListMT::DoGetVar(volatile CHandleValData<SVarData< SGetTensorData > >* pData)
{
	try
	{
		int iHandle = pData->iHandle;

		TAppMap::iterator itEl;

		if ((itEl = m_mapApp.find(iHandle)) == m_mapApp.end())
		{
			// Handle not found
			m_sLastError = "Invalid handle.";
			throw CException();
		}

		CCLUVizApp* pApp = itEl->second;

		SGetTensorData* pTData = pData->Val.pVar;

		if (!pApp->GetVar(pData->Val.pcVarName, *pTData->pvecDim, *pTData->pvecData))
		{
			m_sLastError = pApp->GetLastError();
			throw CException();
		}

		m_bMsgSuccess = true;
	}
	catch (...)
	{
		m_bMsgSuccess = false;
	}

	return m_bMsgSuccess;
}

bool CCLUVizAppListMT::DoGetVar(volatile CHandleValData<SVarData< STensorProp > >* pData)
{
	try
	{
		int iHandle = pData->iHandle;

		TAppMap::iterator itEl;

		if ((itEl = m_mapApp.find(iHandle)) == m_mapApp.end())
		{
			// Handle not found
			m_sLastError = "Invalid handle.";
			throw CException();
		}

		CCLUVizApp* pApp = itEl->second;

		STensorProp* pTData = pData->Val.pVar;

		if (!pApp->GetVar(pData->Val.pcVarName, pTData->uDimCnt, pTData->uElCnt))
		{
			m_sLastError = pApp->GetLastError();
			throw CException();
		}

		m_bMsgSuccess = true;
	}
	catch (...)
	{
		m_bMsgSuccess = false;
	}

	return m_bMsgSuccess;
}

bool CCLUVizAppListMT::DoGetVar(volatile CHandleValData<SVarData< STensorData > >* pData)
{
	try
	{
		int iHandle = pData->iHandle;

		TAppMap::iterator itEl;

		if ((itEl = m_mapApp.find(iHandle)) == m_mapApp.end())
		{
			// Handle not found
			m_sLastError = "Invalid handle.";
			throw CException();
		}

		CCLUVizApp* pApp = itEl->second;

		STensorData* pTData = pData->Val.pVar;

		if (!pApp->GetVar(pData->Val.pcVarName, pTData->puDim, pTData->pdData))
		{
			m_sLastError = pApp->GetLastError();
			throw CException();
		}

		m_bMsgSuccess = true;
	}
	catch (...)
	{
		m_bMsgSuccess = false;
	}

	return m_bMsgSuccess;
}

bool CCLUVizAppListMT::DoGetVar(volatile CHandleValData<SVarData< Clu::CIImage > >* pData)
{
	try
	{
		int iHandle = pData->iHandle;

		TAppMap::iterator itEl;

		if ((itEl = m_mapApp.find(iHandle)) == m_mapApp.end())
		{
			// Handle not found
			m_sLastError = "Invalid handle.";
			throw CException();
		}

		CCLUVizApp* pApp = itEl->second;

		Clu::CIImage* pImage = pData->Val.pVar;

		if (!pApp->GetVar(pData->Val.pcVarName, *pImage))
		{
			m_sLastError = pApp->GetLastError();
			throw CException();
		}

		m_bMsgSuccess = true;
	}
	catch (...)
	{
		m_bMsgSuccess = false;
	}

	return m_bMsgSuccess;
}

bool CCLUVizAppListMT::DoGetVar(volatile CHandleValData<SVarData< SColorData > >* pData)
{
	try
	{
		int iHandle = pData->iHandle;

		TAppMap::iterator itEl;

		if ((itEl = m_mapApp.find(iHandle)) == m_mapApp.end())
		{
			// Handle not found
			m_sLastError = "Invalid handle.";
			throw CException();
		}

		CCLUVizApp* pApp = itEl->second;

		SColorData* pColData = pData->Val.pVar;

		if (!pApp->GetVar(pData->Val.pcVarName, pColData->pdColor))
		{
			m_sLastError = pApp->GetLastError();
			throw CException();
		}

		m_bMsgSuccess = true;
	}
	catch (...)
	{
		m_bMsgSuccess = false;
	}

	return m_bMsgSuccess;
}
