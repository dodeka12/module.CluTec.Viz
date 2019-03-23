// CLUVizDLL.cpp : Definiert den Einstiegspunkt f�r die DLL-Anwendung.
//

#include "stdafx.h"
#include "resource1.h"

#include "CluTec.Base\Exception.h"
#include "CluTec.System\FilePath.h"
#include <new>

#include "CluTec.Viz.Base\Environment.h"

#include "Clu.Viz.Wnd.h"
#include "Clu.Viz.WndList.h"

#include <time.h>
#include <direct.h>

// The Application Class
CCLUVizAppListMT* pAppList = 0;

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
		namespace Wnd
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
			// Load an Icon
			CLUVIZDLL_API void* GetIconHandle(const char* pcFilename, bool bModule)
			{
				if (!pcFilename)
				{
					return 0;
				}

				void* pvIcon;

				if (bModule)
				{
					pvIcon = (void*)LoadIconA(GetModuleHandleA(pcFilename), MAKEINTRESOURCEA(IDI_APPLICATION));
				}
				else
				{
					pvIcon = (void*)LoadImageA(GetModuleHandle(NULL), pcFilename, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
				}

				return pvIcon;
			}

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			/// 	Gets a file type script.
			//CLUVIZDLL_API void GetFileTypeScript(const char* pcFileType, int iMaxLen, char* pcData)
			//{
			//	if (!pAppList)
			//	{
			//		throw CLU_EXCEPTION( "Nullptr --> Application pointer is invalid");
			//	}

			//	if (!pAppList->GetFileTypeScript(pcFileType, iMaxLen, pcData))
			//	{
			//		throw CLU_EXCEPTION( "Error getting file type script");
			//	}
			//}


			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			void _Start(bool bInitAll)
			{
				if (pAppList)
				{
					throw CLU_EXCEPTION( "App List already initialized");
				}

				if (!(pAppList = new CCLUVizAppListMT(bInitAll)))
				{
					throw CLU_EXCEPTION( "Could not initialize App List");
				}

				std::string sEnvPath, sLicencePath, sModulePath;

				sEnvPath = Clu::Viz::CEnvironment::GetPath(Clu::Viz::EEnvPath::Root) + "\\";
				sLicencePath = Clu::Viz::CEnvironment::GetPath(Clu::Viz::EEnvPath::Licence) + "\\";
				sModulePath = Clu::Viz::CEnvironment::GetPath(Clu::Viz::EEnvPath::Binary) + "\\";

				if (!pAppList->Start(sEnvPath.c_str(), sModulePath.c_str()))
				{
					throw CLU_EXCEPTION( "Could not initialize App List");
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Start Engine
			CLUVIZDLL_API void Start()
			{
				try
				{
					_Start(true);
				}
				catch (Clu::CIException& xEx)
				{
					char pcLastError[256];
					GetLastError(pcLastError, 255);
					std::string sLastError = std::string("Could not start CLUViz Engine") + pcLastError;
					throw CLU_EXCEPTION_NEST(sLastError.c_str(), std::move(xEx));
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Start Engine Again
			CLUVIZDLL_API void StartAgain()
			{
				try
				{
					_Start(false);
				}
				catch (Clu::CIException& xEx)
				{
					throw CLU_EXCEPTION_NEST("Could not start CLUViz Engine again", std::move(xEx));
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// End Engine
			CLUVIZDLL_API void End()
			{
				delete pAppList;
				pAppList = 0;
			}


			//////////////////////////////////////////////////////////////////////
			/// Wait For End Viz
			CLUVIZDLL_API void WaitForEnd()
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Nullptr --> Application pointer is invalid");
				}

				if (!pAppList->WaitForEnd())
				{
					throw CLU_EXCEPTION( "Nullptr --> Error waiting for end");
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Enable user to end CLUViz
			CLUVIZDLL_API void EnableUserEnd(bool bVal)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Nullptr --> Application pointer is invalid");
				}

				if (!pAppList->EnableUserEnd(bVal))
				{
					throw CLU_EXCEPTION( "Error enabling user to end CLUViz");
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Test whether CLUViz is still running
			CLUVIZDLL_API bool IsRunning(int iWait)
			{
				if (!pAppList)
				{
					return false;
				}

				return pAppList->IsRunning(iWait);
			}

			//////////////////////////////////////////////////////////////////////
			/// Test whether view is created
			CLUVIZDLL_API bool IsViewValid(int iHandle)
			{
				if (!pAppList)
				{
					return false;
				}

				return pAppList->IsViewValid(iHandle);
			}

			//////////////////////////////////////////////////////////////////////
			/// Message Box
			CLUVIZDLL_API void InfoBox(const char* pcText)
			{
#if defined(WIN32)
				::MessageBoxA(NULL, pcText, "CLUViz Message", MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST);
#else
				fl_message(pcText);
#endif
			}

			//////////////////////////////////////////////////////////////////////
			/// Set Script Path
			CLUVIZDLL_API void SetScriptPath(const char* pcPath)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Nullptr --> Application pointer is invalid");
				}

				if (!pAppList->SetScriptPath(pcPath))
				{
					throw CLU_EXCEPTION( "Error setting script path");
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Create CLUViz Window
			CLUVIZDLL_API void Create(int& iHandle, int iX, int iY, int iWidth, int iHeight, const char* pcTitle)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Nullptr --> Application pointer is invalid");
				}

				//const char *pcTheTitle;
				string sTheTitle;
				void* pvIcon = 0;

				if (!pcTitle)
				{
					sTheTitle = "CLUViz";
				}
				else
				{
					sTheTitle = pcTitle;
				}

#ifdef WIN32
				// If no script has an associated icon, then load CLUView Icon
				char pcDll[100];
				sprintf_s(pcDll, "Clu.Viz.Wnd.%s.dll", CLUVIZ_MAJOR_MINOR);
				pvIcon = (void*)LoadIconA(GetModuleHandleA(pcDll), MAKEINTRESOURCEA(IDI_ICON1));
#endif

				if (!pAppList->Create(iHandle, iX, iY, iWidth, iHeight, sTheTitle.c_str(), 0, 0, true, true, pvIcon, 0, CLU_WNDSTYLE_STD))
				{
					throw CLU_EXCEPTION( "Error creating CLUViz Window");
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Create CLUViz Window
			CLUVIZDLL_API void CreateEx1(int& iHandle, int iX, int iY, int iWidth, int iHeight, const char* pcTitle, unsigned int uWindowStyle)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Nullptr --> Application pointer is invalid");
				}

				//const char *pcTheTitle;
				string sTheTitle;
				void* pvIcon;

				if (!pcTitle)
				{
					sTheTitle = "CLUViz";
				}
				else
				{
					sTheTitle = pcTitle;
				}


#ifdef WIN32
				// If no script has an associated icon, then load CLUViz Icon
				char pcDll[100];
				sprintf_s(pcDll, "Clu.Viz.Wnd.%s.dll", CLUVIZ_MAJOR_MINOR);
				pvIcon = (void*)LoadIconA(GetModuleHandleA(pcDll), MAKEINTRESOURCEA(IDI_ICON1));
#endif

				if (!pAppList->Create(iHandle, iX, iY, iWidth, iHeight, sTheTitle.c_str(),
					0, 0, true, true, pvIcon, 0, uWindowStyle))
				{
					throw CLU_EXCEPTION( "Error creating CLUViz Window");
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Create CLUViz Window Extended
			CLUVIZDLL_API void CreateEx2(int& iHandle,
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
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Nullptr --> Application pointer is invalid");
				}

				if (!pAppList->Create(iHandle, iX, iY, iWidth, iHeight,
					pcTitle /*sTheTitle.c_str()*/, pcInitScript, pcCLUPath, bVizOnly, bShowIntro,
					pvIcon, pvParentHandle, uWindowStyle))
				{
					throw CLU_EXCEPTION( "Error creating CLUViz window");
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Destroy CLUViz Window
			CLUVIZDLL_API void Destroy(int iHandle)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Nullptr --> Application pointer is invalid");
				}

				if (!pAppList->Destroy(iHandle))
				{
					throw CLU_EXCEPTION( "Error destroying CLUViz window");
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Destroy all CLUViz Windows
			CLUVIZDLL_API void DestroyAll()
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Nullptr --> Application pointer is invalid");
				}

				if (pAppList->DestroyAll())
				{
					throw CLU_EXCEPTION( "Error destroying CLUViz window");
				}
			}

			//////////////////////////////////////////////////////////////////////
			// Show Window
			CLUVIZDLL_API void ShowWindow(int iHandle)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Nullptr --> Application pointer is invalid");
				}
				if (!pAppList->ShowWindow(iHandle))
				{
					throw CLU_EXCEPTION( "Could not show window");
				}
			}

			//////////////////////////////////////////////////////////////////////
			// Hide Window
			CLUVIZDLL_API void HideWindow(int iHandle)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Nullptr --> Application pointer is invalid");
				}

				if (!pAppList->HideWindow(iHandle))
				{
					throw CLU_EXCEPTION( "Could not hide window");
				}
			}

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			/// Set Window Size and Position.
			CLUVIZDLL_API void SetWindowPosSize(int iHandle, int iX, int iY, int iWidth, int iHeight)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Nullptr --> Application pointer is invalid");
				}

				if (!pAppList->SetWindowPosSize(iHandle, iX, iY, iWidth, iHeight))
				{
					throw CLU_EXCEPTION( "Nullptr --> Error setting window position and size");
				}
			}

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			/// Gets the last error.
			CLUVIZDLL_API bool GetLastError(char* pcError, int iMaxLen)
			{
				if (!pAppList || !pcError)
				{
					return false;
				}

				string sError;
				sError = pAppList->GetLastError();

				strncpy_s(pcError, iMaxLen, sError.c_str(), sError.length());

				return true;
			}

			//////////////////////////////////////////////////////////////////////
			// Get Script Output
			CLUVIZDLL_API void GetScriptOutput(int iHandle, char* pcText, int iMaxLen, bool& bIsError)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Nullptr --> Application pointer is invalid");
				}

				std::string sText;
				std::vector<std::string> vecText;

				if (!pAppList->GetScriptOutput(iHandle, vecText, bIsError))
				{
					throw CLU_EXCEPTION( "Error getting script output");
				}

				sText = "";
				size_t nIdx, nCnt = vecText.size();
				for (nIdx = 0; nIdx < nCnt; ++nIdx)
				{
					sText += vecText[nIdx];
					sText += "\n";
				}

				strncpy_s(pcText, iMaxLen, sText.c_str(), iMaxLen);
			}

			//////////////////////////////////////////////////////////////////////
			/// Set Timeout for functions interacting with visualization
			CLUVIZDLL_API void SetTimeout(unsigned int uMilliSeconds)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Nullptr --> Application pointer is invalid");
				}

				pAppList->SetTimeout(uMilliSeconds);
			}

			//////////////////////////////////////////////////////////////////////
			/// Set Script
			CLUVIZDLL_API void SetScript(int iHandle, const char* pcScript)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Nullptr --> Application pointer is invalid");
				}

				if (!pAppList->SetScript(iHandle, pcScript))
				{
					throw CLU_EXCEPTION( "Error setting script");
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Load Script
			CLUVIZDLL_API void LoadScript(int iHandle, const char* pcFilename)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Nullptr --> Application pointer is invalid");
				}

				if (!pAppList->LoadScript(iHandle, pcFilename, 0))
				{
					throw CLU_EXCEPTION( "Error loading script");
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Load Script
			CLUVIZDLL_API void LoadScript(int iHandle, const char* pcFilename, const char* pcPass)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Nullptr --> Application pointer is invalid");
				}

				if (!pAppList->LoadScript(iHandle, pcFilename, pcPass))
				{
					throw CLU_EXCEPTION( "Error loading script");
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Load Script
			/// iArgCnt: Number of arguments for script. pcArg contains iArgCnt strings separated by \r (0x13).
			CLUVIZDLL_API void LoadScript(int iHandle, const char* pcFilename, const char* pcPass, int iArgCnt, const char* pcArg)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Nullptr --> Application pointer is invalid");
				}

				if (!pAppList->LoadScript(iHandle, pcFilename, pcPass, iArgCnt, pcArg))
				{
					throw CLU_EXCEPTION( "Error loading script");
				}
			}

			//////////////////////////////////////////////////////////////////////
			// Exec Script
			CLUVIZDLL_API void ExecUser(int iHandle, const char* pcToolName)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Nullptr --> Application pointer is invalid");
				}

				if (!pAppList->ExecUser(iHandle, pcToolName))
				{
					throw CLU_EXCEPTION( "Error executing user script");
				}
			}

			//////////////////////////////////////////////////////////////////////
			// Full Screen
			CLUVIZDLL_API void FullScreen(int iHandle, bool bVal)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Nullptr --> Application pointer is invalid");
				}

				if (!pAppList->FullScreen(iHandle, bVal, true))
				{
					throw CLU_EXCEPTION( "Nullptr --> Error toggling fullscreen");
				}
			}

			//////////////////////////////////////////////////////////////////////
			// Full Screen
			CLUVIZDLL_API void FullScreen(int iHandle, bool bVal, bool bEnableUserToggle)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Nullptr --> Application pointer is invalid");
				}

				if (!pAppList->FullScreen(iHandle, bVal, bEnableUserToggle))
				{
					throw CLU_EXCEPTION( "Nullptr --> Error toggling fullscreen");
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Set Variable Number Int
			CLUVIZDLL_API void SetVarNumber(int iHandle, const char* pcVarName, int iVal)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Application pointer is invalid");
				}

				if (!pAppList->SetVar(iHandle, pcVarName, iVal))
				{
					throw CLU_EXCEPTION( "Error setting number variable");
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Get Variable Number
			CLUVIZDLL_API void GetVarNumber(int iHandle, const char* pcVarName, int& iVal)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Application pointer is invalid");
				}

				if (!pAppList->GetVar(iHandle, pcVarName, iVal))
				{
					throw CLU_EXCEPTION( "Error getting number variable");
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Set Variable Number
			CLUVIZDLL_API void SetVarNumber(int iHandle, const char* pcVarName, double dVal)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Application pointer is invalid");
				}

				if (!pAppList->SetVar(iHandle, pcVarName, dVal))
				{
					throw CLU_EXCEPTION( "Error setting number variable");
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Get Variable Number
			CLUVIZDLL_API void GetVarNumber(int iHandle, const char* pcVarName, double& dVal)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Nullptr --> Application pointer is invalid");
				}

				if (!pAppList->GetVar(iHandle, pcVarName, dVal))
				{
					throw CLU_EXCEPTION( "Error getting number variable");
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Set Variable String
			CLUVIZDLL_API void SetVarString(int iHandle, const char* pcVarName, const char* pcString)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Nullptr --> Application pointer is invalid");
				}

				string sVal;

				sVal = pcString;

				if (!pAppList->SetVar(iHandle, pcVarName, sVal))
				{
					throw CLU_EXCEPTION( "Error setting string variable");
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Get Variable String
			CLUVIZDLL_API void GetVarString(int iHandle, const char* pcVarName, char* pcString, int iMaxLen)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Nullptr --> Application pointer is invalid");
				}

				string sVal;

				if (pAppList->GetVar(iHandle, pcVarName, sVal))
				{
					strncpy_s(pcString, iMaxLen, sVal.c_str(), sVal.length());
				}
				else
				{
					throw CLU_EXCEPTION( "Error getting string variable");
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Set Var Tensor
			CLUVIZDLL_API void SetVarTensor(int iHandle, const char* pcVarName, int iDimCnt, const int* piDim, const double* pdData)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Nullptr --> Application pointer is invalid");
				}
				if (!pAppList->SetVar(iHandle, pcVarName, iDimCnt, piDim, pdData))
				{
					throw CLU_EXCEPTION( "Error setting array variable");
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Set Var Tensor
			CLUVIZDLL_API void SetVarTensor(int iHandle, const char* pcVarName, int iDimCnt, const int* piDim, const float* pfData)
			{
				double* pdData = nullptr;
				try
				{
					if (!pAppList)
					{
						throw CLU_EXCEPTION( "Nullptr -->  Application pointer is invalid");
					}

					if (iDimCnt <= 0)
					{
						throw CLU_EXCEPTION( "Dimension count is invalid");
					}
					if (piDim == 0)
					{
						throw CLU_EXCEPTION( "Nullptr --> Dimensions invalid");
					}
					if (pfData == 0)
					{
						throw CLU_EXCEPTION( "Nullptr --> Data pointer invalid");
					}

					int iTotalCnt = piDim[0];
					for (int i = 1; i < iDimCnt; ++i)
					{
						iTotalCnt *= piDim[i];
					}

					if (iTotalCnt <= 0)
					{
						throw CLU_EXCEPTION( "Error in tensor data");
					}

					pdData = new double[iTotalCnt];

					for (int i = 0; i < iTotalCnt; ++i)
					{
						pdData[i] = double(pfData[i]);
					}

					if (!pAppList->SetVar(iHandle, pcVarName, iDimCnt, piDim, pdData))
					{
						throw CLU_EXCEPTION( "Error setting tensor variable");
					}

					delete[] pdData;
				}
				catch (std::bad_alloc& xEx)
				{
					delete[] pdData;
					throw CLU_EXCEPT_TYPE(OutOfMemory, xEx.what());
				}
				catch (Clu::CIException& xEx)
				{
					delete[] pdData;
					throw CLU_EXCEPTION_NEST("", std::move(xEx));
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Set Var Tensor
			CLUVIZDLL_API void SetVarTensor(int iHandle, const char* pcVarName, const vector<int>& vecDim, const vector<double>& vecData)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Nullptr --> Application pointer is invalid");
				}

				if (!pAppList->SetVar(iHandle, pcVarName, vecDim, vecData))
				{
					throw CLU_EXCEPTION( "Error setting tensor variable");
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Get Var Tensor
			CLUVIZDLL_API void GetVarTensor(int iHandle, const char* pcVarName, vector<int>& vecDim, vector<double>& vecData)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Nullptr --> Application pointer is invalid");
				}

				if (!pAppList->GetVar(iHandle, pcVarName, vecDim, vecData))
				{
					throw CLU_EXCEPTION( "Error getting tensor variable");
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Get Var Tensor Properties
			CLUVIZDLL_API void GetVarTensorProp(int iHandle, const char* pcVarName, unsigned& uDimCnt, unsigned& uElCnt)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Nullptr --> Application pointer is invalid");
				}

				if (!pAppList->GetVar(iHandle, pcVarName, uDimCnt, uElCnt))
				{
					throw CLU_EXCEPTION( "Error getting tensor properties");
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Get Var Tensor Data
			CLUVIZDLL_API void GetVarTensorData(int iHandle, const char* pcVarName, unsigned* puDim, double* pdData)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Nullptr --> Application pointer is invalid");
				}

				if (!pAppList->GetVar(iHandle, pcVarName, puDim, pdData))
				{
					throw CLU_EXCEPTION( "Error getting tensor data");
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Set Var Image
			CLUVIZDLL_API void SetVarImage(int iHandle, const char* pcVarName, const Clu::CIImage& xImage)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Nullptr --> Application pointer is invalid");
				}

				if (!pAppList->SetVar(iHandle, pcVarName, xImage))
				{
					throw CLU_EXCEPTION( "Error setting image variable");
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Get Var Image Properties
			CLUVIZDLL_API void GetVarImage(int iHandle, const char* pcVarName, Clu::CIImage& xImage)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Nullptr --> Application pointer is invalid");
				}

				if (!pAppList->GetVar(iHandle, pcVarName, xImage))
				{
					throw CLU_EXCEPTION( "Error getting image properties");
				}
			}


			//////////////////////////////////////////////////////////////////////
			/// Set Var Color
			CLUVIZDLL_API void SetVarColor(int iHandle, const char* pcVarName, double dRed, double dGreen, double dBlue, double dAlpha)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Nullptr --> Application pointer is invalid");
				}
				if (!pAppList->SetVar(iHandle, pcVarName, dRed, dGreen, dBlue, dAlpha))
				{
					throw CLU_EXCEPTION( "Error getting image data");
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Set Var Color
			CLUVIZDLL_API void SetVarColor(int iHandle, const char* pcVarName, double* pdColor4)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Nullptr --> Application pointer is invalid");
				}

				if (!pAppList->SetVar(iHandle, pcVarName, pdColor4[0], pdColor4[1], pdColor4[2], pdColor4[3]))
				{
					throw CLU_EXCEPTION( "Error setting color variable");
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Get Var Color
			CLUVIZDLL_API void GetVarColor(int iHandle, const char* pcVarName, double& dRed, double& dGreen, double& dBlue, double& dAlpha)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Nullptr --> Application pointer is invalid");
				}

				if (!pAppList->GetVar(iHandle, pcVarName, dRed, dGreen, dBlue, dAlpha))
				{
					throw CLU_EXCEPTION( "Error getting color variable");
				}
			}

			//////////////////////////////////////////////////////////////////////
			/// Get Var Color
			CLUVIZDLL_API void GetVarColor(int iHandle, const char* pcVarName, double* pdColor4)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Nullptr --> Application pointer is invalid");
				}
				if (!pAppList->GetVar(iHandle, pcVarName, pdColor4[0], pdColor4[1], pdColor4[2], pdColor4[3]))
				{
					throw CLU_EXCEPTION( "Error getting color variable");
				}
			}

			// Set vertex array data
			CLUVIZDLL_API void SetVarObject(int iHandle, const char* pcVarName, SStdVbo* pVexArray)
			{
				if (!pAppList)
				{
					throw CLU_EXCEPTION( "Application pointer is invalid");
				}
				if (!pAppList->SetVar(iHandle, pcVarName, pVexArray))
				{
					throw CLU_EXCEPTION( "Error setting variable to object");
				}
			}
		} // namespace Wnd
	} // namespace Viz
} // namespace Clu
