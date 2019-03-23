////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.View.Base
// file:      CLUVizApp.h
//
// summary:   Declares the clu viz application class
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

#pragma once

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>

#include "CluTec.Types1/IImage.h"
#include "CluTec.Types1/IDataContainer.h"

#include "CluTec.OpenGL/Vertex.MeshObject.h"

#include "CLUScriptEditor.h"
#include "OGLWin.h"
#include "CLUOutput.h"
#include "CluTec.Viz.Draw\OGLImageTypeDef.h"

#include "CluTec.Viz.Fltk\Fl_Tooltip.H"

#include "CLUVizTypes.h"
#include "CluTec.Viz.Base/StdVbo.h"

#include <vector>
#include <string>

using namespace std;

class CCLUVizApp
{
public:
	using TMesh = Clu::OpenGL::Vertex::CMeshObject<Clu::OpenGL::Vertex::EType::Standard>;

public:

	CCLUVizApp(void);

	bool Create(int iX,
			int iY,
			int iWidth,
			int iHeight,
			const char* pcTitle = 0,
			const char* pcInitScript = 0,
			const char* pcCLUPath = 0,
			bool bVizOnly = true,
			bool bShowIntro = true,
			void* pvIcon = 0,
			void* pvParentHandle = 0,
			unsigned int uWindowStyle = CLU_WNDSTYLE_STD,
			const char* pcLicensee = 0);
	bool Destroy();

	bool EmbedWindow(HWND hWnd, const char* pcCLUPath, bool bCreateSRC = false, HGLRC* phGlRc = 0, bool bLoadIntroScript = true);
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
	bool EW_Display(bool bWait);
	bool EW_Reshape(int iWidth, int iHeight);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// 	Gets the width and the height of the view port of this view.
	/// </summary>
	///
	/// <param name="iWidth">  [out] The width of the view port. </param>
	/// <param name="iHeight"> [out] The height of the view port. </param>
	///
	/// <returns> True if it succeeds, false if it fails. </returns>
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool EW_GetShape(int& iWidth, int& iHeight);

	bool EW_Mouse(int iButton, int iState, int iX, int iY);
	//bool EW_Key(unsigned char cKey, int iX, int iY);
	bool EW_ActiveMouseMove(int iX, int iY);
	bool EW_PassiveMouseMove(int iX, int iY);
	bool EW_IdleFunc();
	bool EW_SetKeyModifierStates(bool bShift, bool bCtrl, bool bAlt, bool bNumLock, bool bCapsLock, bool bScrollLock);
	bool EW_LoadScript(const char* pcFilename, const char* pcPass);
	bool EW_SetScript(const char* pcScript, bool bResetEnv = false, bool bReparseLatex = false);
	bool EW_ExecUser(const char* pcToolName = 0);
	bool EW_Keyboard(int iKey, bool bKeyDown);
	bool EW_LockVis(bool bLock);

	bool EW_MakeCurrent(bool bBind);

	bool GetScriptOutput(std::vector<std::string>& vecText, bool& bIsError);
	bool SaveView(const char* pcFilename);
	bool EnableVisualization(bool bVal);
	bool EnableDrawSceneGraph(bool bVal, bool bDoRedisplay);

	bool IsValid() { if (m_bIsCreated) { return m_bOGLWinValid; }else{ return false; } }

	bool ShowWindow();
	bool HideWindow();
	bool SetWindowPosSize(int iX, int iY, int iWidth, int iHeight);

	bool ShowEditor();
	bool HideEditor();

	bool SetScript(const char* pcScript);
	bool LoadScript(const char* pcFilename, const char* pcPass = 0);
	bool LoadScript(const char* pcFilename, const char* pcPass, int iArgCnt, const char* pcArg);

	bool ExecUser(const char* pcToolName = 0);
	bool FullScreen(bool bVal = true, bool bEnableUserToggle = true);
	bool EnableBorder(bool bVal = true);

	bool SetVar(const char* pcVarName, int dVal);
	bool GetVar(const char* pcVarName, int& dVal);

	bool SetVar(const char* pcVarName, double dVal);
	bool GetVar(const char* pcVarName, double& dVal);

	bool SetVar(const char* pcVarName, const string& sVal);
	bool GetVar(const char* pcVarName, string& sVal);

	bool SetVar(const char* pcVarName, const vector<string>& sVal);

	bool SetVar(const char* pcVarName, int iDimCnt, const int* piDim, const double* pdData);
	bool SetVar(const char* pcVarName, const vector<int>& vecDim, const vector<double>& vecData)
	{ return SetVar(pcVarName, (int) vecDim.size(), &vecDim[0], &vecData[0]); }

	bool GetVar(const char* pcVarName, vector<int>& vecDim, vector<double>& vecData);
	bool GetVar(const char* pcVarName, unsigned& uDimCnt, unsigned& uElCnt);
	bool GetVar(const char* pcVarName, unsigned* puDim, double* pdData);

	// Set Image Data
	bool SetVar(const char* pcVarName, const Clu::CIImage& xImage);

	// Image Properties
	bool GetVar(const char* pcVarName, Clu::CIImage& xImage);

	// Set Color
	bool SetVar(const char* pcVarName, double* pdColor);

	// Get Color
	bool GetVar(const char* pcVarName, double* pdColor);

	// Set vertex array data
	bool SetVar(const char* pcVarName, const Clu::SStdVbo* pData);

	bool SetVar(const char* pcVarName, Clu::CIDataContainer xData);

protected:
	bool _SetVarMesh(const char* pcVarName, const TMesh& xMesh);

public:


	string GetLastError() { return m_sLastError; }

	string GetSceneGraphPrint() { return m_poglWin->GetSceneGraphPrint(); };

public:

	~CCLUVizApp(void);

protected:

	bool Init();
	// If variable exists, set it to new type and return it.
	// Ohterwise, create variable of given type and name.
	CCodeVar& CreateCodeVar(const char* pcVarName, ECodeDataType eDataType, bool bForce = true);

protected:

	string m_sLastError;

	CCLUScriptEditor* m_pEditor;
	COGLWin* m_poglWin;
	CCLUOutput* m_pOutput;

	bool m_bOGLWinValid;
	bool m_bIsCreated;
	bool m_bIsEmbedded;

	void* m_pvIcon;
	void* m_pvParentHandle;

	string m_sEnvPath;
	string m_sTitle;
	string m_sInitScript;
	string m_sLicensee;
	bool m_bVizOnly;
	bool m_bShowIntro;
	unsigned int m_uWindowStyle;
	int m_iX, m_iY, m_iWidth, m_iHeight;
};
