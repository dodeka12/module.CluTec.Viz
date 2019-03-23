// CLUVizDLL.cpp : Definiert den Einstiegspunkt f�r die DLL-Anwendung.
//

#include "stdafx.h"
#include "CluTec.Base\Exception.h"
#include "CluTec.System\FilePath.h"
#include <new>

#include "CluTec.Viz.Base\Environment.h"

#include "Clu.Viz.View.h"
#include "Clu.Viz.ViewList.h"

#include <time.h>

// The Application Class
CCLUVizAppListST* pAppList = 0;

#ifdef _MANAGED
#pragma managed(push, off)
#endif

#pragma warning(disable: 4555)
[module(name = "ATLCLUViz")];

#ifdef _MANAGED
#pragma managed(pop)
#endif


namespace Clu
{
	namespace Viz
	{
		namespace View
		{

			//////////////////////////////////////////////////////////////////////
			// Get the version of CluVizCore
			CLUVIZDLL_API void GetVersion(char* pcVersion, int iBufLen)
			{
				if (int(strlen(CLU_MODULE_VER_LONG_STR)) >= iBufLen)
				{
					throw CLU_EXCEPTION("module version string length exceeds buffer length");
				}

				strcpy_s(pcVersion, iBufLen, CLU_MODULE_VER_LONG_STR);
			}


			//////////////////////////////////////////////////////////////////////////
			CLUVIZDLL_API Clu::CIString GetPath(Clu::Viz::EEnvPath eEnvPath)
			{
				return Clu::ToIString(Clu::Viz::CEnvironment::GetPath(eEnvPath));
			}

			//////////////////////////////////////////////////////////////////////
			/// Start Engine
			CLUVIZDLL_API void Start(bool bAlone)
			{
				try
				{
					if (pAppList)
					{
						throw CLU_EXCEPTION("Application pointer already exists");
					}

					pAppList = new CCLUVizAppListST(bAlone);

					std::string sEnvPath, sLicencePath, sModulePath;

					sEnvPath = Clu::Viz::CEnvironment::GetPath(Clu::Viz::EEnvPath::Root) + "\\";
					sLicencePath = Clu::Viz::CEnvironment::GetPath(Clu::Viz::EEnvPath::Licence) + "\\";
					sModulePath = Clu::Viz::CEnvironment::GetPath(Clu::Viz::EEnvPath::Binary) + "\\";

					if (!pAppList->Start(sEnvPath.c_str(), sModulePath.c_str()))
					{
						throw CLU_EXCEPTION("Could not initialize App List");
					}
				}
				catch (std::bad_alloc& xEx)
				{
					throw CLU_EXCEPT_TYPE(OutOfMemory, xEx.what());
				}
				catch (Clu::CIException& xEx)
				{
					throw CLU_EXCEPTION_NEST("Error starting CLUViz Engine", std::move(xEx));
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// End Engine

			CLUVIZDLL_API void End()
			{
				if (!pAppList)
				{
					return;
				}

				delete pAppList;
				pAppList = 0;
			}

			//////////////////////////////////////////////////////////////////////
			/// Get Last Error

			CLUVIZDLL_API bool GetLastError(char* pcError, int iMaxLen)
			{
				if (!pcError)
				{
					return false;
				}

				string sError = "";
				if (pAppList)
				{
					sError = pAppList->GetLastError();
				}
				else
				{
					sError = "App List doesn't exist";
				}

				strncpy_s(pcError, iMaxLen, sError.c_str(), sError.length());
				return true;
			}

			//////////////////////////////////////////////////////////////////////
			CLUVIZDLL_API void SetScriptPath(const char* pcPath)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}
				if (!pAppList->SetScriptPath(pcPath))
				{
					throw CLU_EXCEPTION("Error setting script path");
				}
			}

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			CLUVIZDLL_API void EmbedWindow(int& iHandle, __int64 hWnd, bool bCreateSRC, __int64* phGlRc, bool bLoadIntroScript)
			{
				try
				{
					if (!pAppList)
					{
						throw CLU_EXCEPTION("Start() has not been called or has been failed");
					}

					if (!pAppList->EmbedWindow(iHandle, HWND(hWnd), bCreateSRC, (HGLRC*)phGlRc, bLoadIntroScript))
					{
						throw CLU_EXCEPTION(pAppList->GetLastError().c_str());
					}
				}
				catch (Clu::CIException& ex)
				{
					throw CLU_EXCEPTION_NEST("Error embedding CLUViz window", std::move(ex));
				}
			}

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			CLUVIZDLL_API void ReleaseWindow(int iHandle)
			{
				if (pAppList)
				{
					if (!pAppList->ReleaseWindow(iHandle))
					{
						throw CLU_EXCEPTION("Error releasing CLUViz window");
					}
				}
			}

			//////////////////////////////////////////////////////////////////////
			// Release all CLUViz Windows

			CLUVIZDLL_API void ReleaseAll()
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				pAppList->ReleaseAll();
			}

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			CLUVIZDLL_API void SetMouseEventHandler(int iHandle, Clu::Viz::View::TFuncMouseEventCallback pFunc, void* pvContext)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				if (!pAppList->SetMouseEventHandler(iHandle, pFunc, pvContext))
				{
					throw CLU_EXCEPTION("Error setting mouse event callback function");
				}
			}

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			CLUVIZDLL_API void RemoveMouseEventHandler(int iHandle)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				if (!pAppList->RemoveMouseEventHandler(iHandle))
				{
					throw CLU_EXCEPTION("Error removing mouse event callback function");
				}
			}

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			CLUVIZDLL_API void SetErrorEventHandler(int iHandle, Clu::Viz::View::TFuncErrorEventCallback pFunc, void* pvContext)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				if (!pAppList->SetErrorEventHandler(iHandle, pFunc, pvContext))
				{
					throw CLU_EXCEPTION("Error setting error event callback function");
				}
			}

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			CLUVIZDLL_API Clu::CIImage LoadImage(const Clu::CIString& sFilename)
			{
				COGLImage oglImage;
				oglImage.SetFilename(sFilename.ToCString());
				if (!oglImage.LoadImage(true))
				{
					throw CLU_EXCEPTION("Error loading image");
				}

				return oglImage.ToIImage();
			}

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			CLUVIZDLL_API void SaveImage(const Clu::CIString& sFilename, const Clu::CIImage& xImage)
			{
				COGLImage oglImage;

				oglImage.Create(xImage);

				if (!oglImage.SaveImage(sFilename.ToCString()))
				{
					throw CLU_EXCEPTION("Error saving image");
				}
			}


			//////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////
			// Functions to update OGLWin when window GL Window is embedded in has changed

			//////////////////////////////////////////////////////////////////////
			/// Embedded Window Display

			CLUVIZDLL_API void EW_Display(int iHandle, bool bWait)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				if (!pAppList->EW_Display(iHandle, bWait))
				{
					throw CLU_EXCEPTION("Error on display event");
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Embedded Window Reshape

			CLUVIZDLL_API void EW_Reshape(int iHandle, int iWidth, int iHeight)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				if (!pAppList->EW_Reshape(iHandle, iWidth, iHeight))
				{
					throw CLU_EXCEPTION("Error reshaping window");
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Embedded Window Get Shape

			CLUVIZDLL_API void EW_GetShape(int iHandle, int& iWidth, int& iHeight)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				if (!pAppList->EW_GetShape(iHandle, iWidth, iHeight))
				{
					throw CLU_EXCEPTION("Error getting shape of window");
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Embedded Window Mouse

			CLUVIZDLL_API void EW_Mouse(int iHandle, int iButton, int iState, int iX, int iY)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				if (!pAppList->EW_Mouse(iHandle, iButton, iState, iX, iY))
				{
					throw CLU_EXCEPTION("Error on mouse event");
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Embedded Window Active Mouse Move

			CLUVIZDLL_API void EW_ActiveMouseMove(int iHandle, int iX, int iY)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				if (!pAppList->EW_ActiveMouseMove(iHandle, iX, iY))
				{
					throw CLU_EXCEPTION("Error on active mouse move event");
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Embedded Window Passive Mouse Move

			CLUVIZDLL_API void EW_PassiveMouseMove(int iHandle, int iX, int iY)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				if (!pAppList->EW_PassiveMouseMove(iHandle, iX, iY))
				{
					throw CLU_EXCEPTION("Error on passive mouse move event");
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Embedded Window Idle Function

			CLUVIZDLL_API void EW_IdleFunc(int iHandle)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				if (!pAppList->EW_IdleFunc(iHandle))
				{
					throw CLU_EXCEPTION("Error on passive mouse move event");
				}
			}

			/////////////////////////////////////////////////////////////////////////////
			/// Embedded Window Set the modifier key states

			CLUVIZDLL_API void EW_SetKeyModifierStates(int iHandle, bool bShift, bool bCtrl, bool bAlt, bool bNumLock, bool bCapsLock, bool bScrollLock)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				if (!pAppList->EW_SetKeyModifierStates(iHandle, bShift, bCtrl, bAlt, bNumLock, bCapsLock, bScrollLock))
				{
					throw CLU_EXCEPTION("Error setting the modifier key states");
				}
			}

			/////////////////////////////////////////////////////////////////////////////
			/// Embedded Window Keyboard input

			CLUVIZDLL_API void EW_Keyboard(int iHandle, int iKey, bool bKeyDown)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				if (!pAppList->EW_Keyboard(iHandle, iKey, bKeyDown))
				{
					throw CLU_EXCEPTION("Error handling embedded window keyboard input");
				}
			}

			/////////////////////////////////////////////////////////////////////////////
			/// Embedded Window Load Script

			CLUVIZDLL_API void EW_LoadScript(int iHandle, const char* pcFilename, const char* pcPass)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				if (!pAppList->EW_LoadScript(iHandle, pcFilename, pcPass))
				{
					throw CLU_EXCEPTION(CLU_S "Error loading embedded window user script. " << pAppList->GetLastError().c_str());
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Embedded Window Set Script

			CLUVIZDLL_API void EW_SetScript(int iHandle, const char* pcScript, bool bResetEnv, bool bReparseLatex)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				if (!pAppList->EW_SetScript(iHandle, pcScript, bResetEnv, bReparseLatex))
				{
					throw CLU_EXCEPTION("Error setting embedded window user script");
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Embedded Window Exec Script

			CLUVIZDLL_API void EW_ExecUser(int iHandle, const char* pcToolName)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				if (!pAppList->EW_ExecUser(iHandle, pcToolName))
				{
					throw CLU_EXCEPTION(CLU_S "Error executing embedded window user script '" << pcToolName << "'. " << pAppList->GetLastError().c_str());
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Embedded Window Lock visualization thread

			CLUVIZDLL_API void EW_LockVis(int iHandle, bool bLock)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				if (!pAppList->EW_LockVis(iHandle, bLock))
				{
					throw CLU_EXCEPTION("Error locking visualization thread");
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Embedded Window Shared RC Functions

			CLUVIZDLL_API void EW_MakeCurrent(int iHandle, bool bBind)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				if (!pAppList->EW_MakeCurrent(iHandle, bBind))
				{
					throw CLU_EXCEPTION("Error making current shared rendering context");
				}
			}

			//////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////////////////
			// save current view to file

			CLUVIZDLL_API void SaveView(int iHandle, const char* pcFilename)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				if (!pAppList->SaveView(iHandle, pcFilename))
				{
					throw CLU_EXCEPTION("Error saving current view to file");
				}
			}

			//////////////////////////////////////////////////////////////////////
			// Enable/Disable visualization

			CLUVIZDLL_API void EnableVisualization(int iHandle, bool bVal)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				if (!pAppList->EnableVisualization(iHandle, bVal))
				{
					throw CLU_EXCEPTION("Error enabling/disabling visualization ");
				}
			}

			//////////////////////////////////////////////////////////////////////
			// Enable/Disable drawing

			CLUVIZDLL_API void EnableDrawing(int iHandle, bool bVal, bool bDoRedisplay)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				if (!pAppList->EnableDrawing(iHandle, bVal, bDoRedisplay))
				{
					throw CLU_EXCEPTION("Error enabling/disabling drawing ");
				}
			}

			//////////////////////////////////////////////////////////////////////
			// Get Script Output

			CLUVIZDLL_API void GetScriptOutput(int iHandle, char* pcText, int iMaxLen, bool& bIsError)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				std::string sText;
				std::vector<std::string> vecText;

				if (!pAppList->GetScriptOutput(iHandle, vecText, bIsError))
				{
					throw CLU_EXCEPTION("Error getting script output");
				}

				sText = "";
				int iIdx, iCnt = int(vecText.size());
				for (iIdx = 0; iIdx < iCnt; ++iIdx)
				{
					sText += vecText[iIdx];
					sText += "\n";
				}

				strncpy_s(pcText, iMaxLen, sText.c_str(), iMaxLen);
			}

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			CLUVIZDLL_API void GetScriptOutput(int iHandle, Clu::CIString& sxOutput, bool& bIsError)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				std::vector<std::string> vecText;
				if (!pAppList->GetScriptOutput(iHandle, vecText, bIsError))
				{
					throw CLU_EXCEPTION("Error getting script output");
				}

				std::string sText = "";
				for (size_t nIdx = 0; nIdx < vecText.size(); ++nIdx)
				{
					sText += vecText[nIdx];
					sText += "\n";
				}

				sxOutput = sText.c_str();
			}

			CLUVIZDLL_API void GetSceneGraphPrint(int iHandle, Clu::CIString& sxOutput)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				string sXml = pAppList->GetSceneGraphPrint(iHandle);

				sxOutput = sXml.c_str();
			}

			//////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////
			//// Get/Set Script Variables

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			CLUVIZDLL_API void SetVarNumber(int iHandle, const char* pcVarName, int iVal)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				if (!pAppList->SetVar(iHandle, pcVarName, iVal))
				{
					throw CLU_EXCEPTION(CLU_S "Error setting integer variable '" << pcVarName << "' to " << iVal << ". Reason: " <<
						pAppList->GetLastError().c_str());
				}
			}

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			CLUVIZDLL_API void GetVarNumber(int iHandle, const char* pcVarName, int& iVal)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				if (!pAppList->GetVar(iHandle, pcVarName, iVal))
				{
					throw CLU_EXCEPTION(CLU_S "Error getting integer variable '" << pcVarName << "'. Reason: " << pAppList->GetLastError().c_str());
				}
			}

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			CLUVIZDLL_API void SetVarNumber(int iHandle, const char* pcVarName, double dVal)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				if (!pAppList->SetVar(iHandle, pcVarName, dVal))
				{
					throw CLU_EXCEPTION(CLU_S "Error setting double variable '" << pcVarName << "' to " << dVal << ". Reason: " << pAppList->GetLastError().c_str());
				}
			}

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			CLUVIZDLL_API void GetVarNumber(int iHandle, const char* pcVarName, double& dVal)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				if (!pAppList->GetVar(iHandle, pcVarName, dVal))
				{
					throw CLU_EXCEPTION(CLU_S "Error getting double variable '" << pcVarName << "'. Reason: " << pAppList->GetLastError().c_str());
				}
			}

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			CLUVIZDLL_API void SetVarString(int iHandle, const char* pcVarName, const char* pcString)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				string sString = pcString;
				if (!pAppList->SetVar(iHandle, pcVarName, sString))
				{
					throw CLU_EXCEPTION(
						CLU_S "Error setting string variable '" << pcVarName << "' to '" << pcString << "'. Reason: " << pAppList->GetLastError().c_str());
				}
			}

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//CLUVIZDLL_API void SetVarStringList(int iHandle, const char* pcVarName, const Clu::CArrayString& aString)
			//{
			//	if (!pAppList)
			//	{
			//		throw CLU_EXCEPTION("Start() has not been called or has been failed");
			//	}

			//	vector<string> vsString;

			//	for (int i = 0; i < (int) aString.Length(); i++)
			//	{
			//		vsString.push_back(aString[i].ToCString());
			//	}

			//	if (!pAppList->SetVar(iHandle, pcVarName, vsString))
			//	{
			//		throw CLU_EXCEPTION(
			//				CLU_S "Error setting string list variable '" << pcVarName << " Reason: " << pAppList->GetLastError().c_str());
			//	}
			//}

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			CLUVIZDLL_API void GetVarString(int iHandle, const char* pcVarName, char* pcString, int iMaxLen)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				string sValue;
				if (!pAppList->GetVar(iHandle, pcVarName, sValue))
				{
					throw CLU_EXCEPTION(CLU_S "Error getting string variable '" << pcVarName << "'. Reason: " << pAppList->GetLastError().c_str());
				}

				strncpy_s(pcString, iMaxLen, sValue.c_str(), iMaxLen - 1);
			}

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			CLUVIZDLL_API void SetVarTensor(int iHandle, const char* pcVarName, int iDimCnt, const int* piDim, const double* pdData)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				if (!pAppList->SetVar(iHandle, pcVarName, iDimCnt, piDim, pdData))
				{
					throw CLU_EXCEPTION(CLU_S "Error setting tensor variable '" << pcVarName << "'. Reason: " << pAppList->GetLastError().c_str());
				}
			}

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			CLUVIZDLL_API void GetVarTensorProp(int iHandle, const char* pcVarName, unsigned& uDimCnt, unsigned& uElCnt)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				if (!pAppList->GetVar(iHandle, pcVarName, uDimCnt, uElCnt))
				{
					throw CLU_EXCEPTION(CLU_S "Error getting tensor property variable '" << pcVarName << "'. Reason: " << pAppList->GetLastError().c_str());
				}
			}

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			CLUVIZDLL_API void GetVarTensorData(int iHandle, const char* pcVarName, unsigned* puDim, double* pdData)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				if (!pAppList->GetVar(iHandle, pcVarName, puDim, pdData))
				{
					throw CLU_EXCEPTION(CLU_S "Error getting tensor data variable '" << pcVarName << "'. Reason: " << pAppList->GetLastError().c_str());
				}
			}

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			CLUVIZDLL_API void SetVarImage(int iHandle, const char* pcVarName, const Clu::CIImage& xImage)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				if (!pAppList->SetVar(iHandle, pcVarName, xImage))
				{
					throw CLU_EXCEPTION(CLU_S "Error setting image variable '" << pcVarName << "'. Reason: " << pAppList->GetLastError().c_str());
				}
			}

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			CLUVIZDLL_API void GetVarImage(int iHandle, const char* pcVarName, Clu::CIImage& xImage)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				if (!pAppList->GetVar(iHandle, pcVarName, xImage))
				{
					throw CLU_EXCEPTION(CLU_S "Error getting image property variable '" << pcVarName << "'. Reason: " << pAppList->GetLastError().c_str());
				}
			}


			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			CLUVIZDLL_API void SetVarColor(int iHandle, const char* pcVarName, double* pdColor4)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				if (!pAppList->SetVar(iHandle, pcVarName, pdColor4[0], pdColor4[1], pdColor4[2], pdColor4[3]))
				{
					throw CLU_EXCEPTION(CLU_S "Error setting color variable '" << pcVarName << "'. Reason: " << pAppList->GetLastError().c_str());
				}
			}

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			CLUVIZDLL_API void SetVarColor(int iHandle, const char* pcVarName, double dRed, double dGreen, double dBlue, double dAlpha)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				if (!pAppList->SetVar(iHandle, pcVarName, dRed, dGreen, dBlue, dAlpha))
				{
					throw CLU_EXCEPTION(CLU_S "Error setting color variable '" << pcVarName << "'. Reason: " << pAppList->GetLastError().c_str());
				}
			}

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			CLUVIZDLL_API void GetVarColor(int iHandle, const char* pcVarName, double* pdColor4)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				if (!pAppList->GetVar(iHandle, pcVarName, pdColor4[0], pdColor4[1], pdColor4[2], pdColor4[3]))
				{
					throw CLU_EXCEPTION(CLU_S "Error getting color variable '" << pcVarName << "'. Reason: " << pAppList->GetLastError().c_str());
				}
			}

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			CLUVIZDLL_API void GetVarColor(int iHandle, const char* pcVarName, double& dRed, double& dGreen, double& dBlue, double& dAlpha)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}

				if (!pAppList->GetVar(iHandle, pcVarName, dRed, dGreen, dBlue, dAlpha))
				{
					throw CLU_EXCEPTION(CLU_S "Error getting color variable '" << pcVarName << "'. Reason: " << pAppList->GetLastError().c_str());
				}
			}

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			CLUVIZDLL_API void SetVarObject(int iHandle, const char* pcVarName, const Clu::SStdVbo* pVexArray)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}
				if (!pAppList->SetVar(iHandle, pcVarName, pVexArray))
				{
					throw CLU_EXCEPTION(CLU_S "Error setting object variable '" << pcVarName << "'. Reason: " << pAppList->GetLastError().c_str());
				}
			}


			CLUVIZDLL_API void SetVarData(int iHandle, const char* pcVarName, Clu::CIDataContainer xData)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION("Start() has not been called or has been failed");
				}
				if (!pAppList->SetVar(iHandle, pcVarName, xData))
				{
					throw CLU_EXCEPTION(CLU_S "Error setting data variable '" << pcVarName << "'. Reason: " << pAppList->GetLastError().c_str());
				}
			}


			//////////////////////////////////////////////////////////////////////
		} // namespace View
	} // namespace Viz
}	// namespace Clu
