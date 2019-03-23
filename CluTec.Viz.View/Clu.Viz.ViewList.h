#pragma once
#include "CluTec.Types1\IDataContainer.h"
#include "CluTec.Viz.View.Base\CLUVizApp.h"

#include <map>
#include <string>

class CDevice_3DX;

using namespace std;

class CCLUVizAppListST
{
public:

	typedef map<int, CCLUVizApp*> TAppMap;

	class CException
	{
	public:

		CException() { }
	};

public:

	CCLUVizAppListST(bool bInitAll = true);

	bool Start(const char* pcEnvPath, const char* pcModulePath);
	bool End();

	bool Lock(int iWait = INFINITE);
	bool Unlock();

	// Embed a CLUViz window into the window given by hWnd
	bool EmbedWindow(int& iHandle, HWND hWnd, bool bCreateSRC = false, HGLRC* phGlRc = 0, bool bLoadIntroScript = true);

	// Release a CLUViz Window
	bool ReleaseWindow(int iHandle);
	void ReleaseAll();

	// Sets a mouse event callback function
	bool SetMouseEventHandler(int iHandle, Clu::Viz::View::TFuncMouseEventCallback pFunc, void* pvContext);
	// Removes the mouse event callback function
	bool RemoveMouseEventHandler(int iHandle);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// 	Sets the error event handler.
	/// </summary>
	///
	/// <param name="iHandle">   The handle of the view. </param>
	/// <param name="pFunc">	 If non-null, the callback function. </param>
	/// <param name="pvContext"> [in,out] If non-null, a context pointer. </param>
	///
	/// <returns> True if it succeeds, false if it fails. </returns>
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool SetErrorEventHandler(int iHandle, Clu::Viz::View::TFuncErrorEventCallback pFunc, void* pvContext);

	// Functions to update OGLWin when window GL Window is embedded in has changed
	bool EW_Display(int iHandle, bool bWait);
	bool EW_Reshape(int iHandle, int iWidth, int iHeight);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// 	Gets the width and the height of the view port associated with the given handle.
	/// </summary>
	///
	/// <param name="iHandle"> The handle of the view. </param>
	/// <param name="iWidth">  [out] The width of the view port. </param>
	/// <param name="iHeight"> [out] The height of the view port. </param>
	///
	/// <returns> True if it succeeds, false if it fails. </returns>
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool EW_GetShape(int iHandle, int& iWidth, int& iHeight);

	bool EW_Mouse(int iHandle, int iButton, int iState, int iX, int iY);
	//bool EW_Key(unsigned char cKey, int iX, int iY);
	bool EW_ActiveMouseMove(int iHandle, int iX, int iY);
	bool EW_PassiveMouseMove(int iHandle, int iX, int iY);
	bool EW_IdleFunc(int iHandle);
	bool EW_SetKeyModifierStates(int iHandle, bool bShift, bool bCtrl, bool bAlt, bool bNumLock, bool bCapsLock, bool bScrollLock);
	bool EW_Keyboard(int iHandle, int iKey, bool bKeyDown);
	bool EW_LoadScript(int iHandle, const char* pcFilename, const char* pcPass = 0);
	bool EW_SetScript(int iHandle, const char* pcScript, bool bResetEnv = false, bool bReparseLatex = false);
	bool EW_ExecUser(int iHandle, const char* pcToolName = 0);
	bool EW_LockVis(int iHandle, bool bLock);

	bool EW_MakeCurrent(int iHandle, bool bBind);

	bool GetScriptOutput(int iHandle, std::vector<std::string>& vecText, bool& bIsError);
	bool SetScriptPath(const char* pcPath);

	// Further View access functions
	bool SaveView(int iHandle, const char* pcFilename);

	// Enable/Disable visualization
	bool EnableVisualization(int iHandle, bool bVal);
	bool EnableDrawing(int iHandle, bool bVal, bool bDoRedisplay);

	// Get/Set Script Variables
	bool SetVar(int iHandle, const char* pcVarName, int iVal);
	bool GetVar(int iHandle, const char* pcVarName, int& iVal);

	bool SetVar(int iHandle, const char* pcVarName, double dVal);
	bool GetVar(int iHandle, const char* pcVarName, double& dVal);

	bool SetVar(int iHandle, const char* pcVarName, const string& sVal);
	bool GetVar(int iHandle, const char* pcVarName, string& sVal);

	// string list
	bool SetVar(int iHandle, const char* pcVarName, const vector<string>& vsVal);

	bool SetVar(int iHandle, const char* pcVarName, int iDimCnt, const int* piDim, const double* pdData);
	bool GetVar(int iHandle, const char* pcVarName, unsigned& uDimCnt, unsigned& uElCnt);
	bool GetVar(int iHandle, const char* pcVarName, unsigned* puDim, double* pdData);

	// Set Image Data
	bool SetVar(int iHandle, const char* pcVarName, const Clu::CIImage& xImage);

	// Get image properties
	bool GetVar(int iHandle, const char* pcVarName, Clu::CIImage& xImage);

	// Set Color data
	bool SetVar(int iHandle, const char* pcVarName, double dRed, double dGreen, double dBlue, double dAlpha);

	// Get color data
	bool GetVar(int iHandle, const char* pcVarName, double& dRed, double& dGreen, double& dBlue, double& dAlpha);

	// Set vertex array data
	bool SetVar(int iHandle, const char* pcVarName, const Clu::SStdVbo* pVexArray);

	bool SetVar(int iHandle, const char* pcVarName, Clu::CIDataContainer xData);

	// Get Last Error that occurred
	string GetLastError();

	string GetSceneGraphPrint(int iHandle);

public:

	~CCLUVizAppListST(void);

protected:

protected:

	bool InitViz();
	bool FinalizeViz();

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// 	Gets the application for the given app handle.
	/// </summary>
	///
	/// <param name="iHandle"> Zero-based index of the handle. </param>
	///
	/// <returns> Null if it fails, else the application. </returns>
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CCLUVizApp* _GetApp(int iHandle);

protected:

	bool m_bInitAll;
	bool m_bIsRunning;
	HANDLE m_hMutexAccess;

	int m_iCurHandle;
	TAppMap m_mapApp;

	string m_sLastError;
	string m_sEnvPath;
	string m_sLicencePath;
	string m_sScriptPath;
	string m_sModulePath;

	CCLUCodeBase* m_pCLUCodeBase;

	CDevice_3DX* m_pDevice3DX;

	int m_iRanSeed;
};
