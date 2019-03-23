// Folgender ifdef-Block ist die Standardmethode zum Erstellen von Makros, die das Exportieren
// aus einer DLL vereinfachen. Alle Dateien in dieser DLL werden mit dem NLVIZDLL_EXPORTS-Symbol
// kompiliert, das in der Befehlszeile definiert wurde. Das Symbol darf nicht f�r ein Projekt definiert werden,
// das diese DLL verwendet. Alle anderen Projekte, deren Quelldateien diese Datei beinhalten, erkennen
// CLUVIZDLL_API-Funktionen als aus einer DLL importiert, w�hrend die DLL
// mit diesem Makro definierte Symbole als exportiert ansieht.
#pragma once

#ifdef CLU_VIZ_WND_EXPORTS
	#define CLUVIZDLL_API __declspec(dllexport)
#else
	#define CLUVIZDLL_API __declspec(dllimport)
#endif

#include <string>
#include <vector>

#include "CluTec.Types1/IImage.h"
#include "CluTec.Types1/IDataContainer.h"

#include "CluTec.Viz.Base/StdVbo.h"
#include "CluTec.Viz.View.Base\CLUVizDefines.h"
#include "CluTec.Viz.View.Base\CLUVizTypes.h"

#include "CluTec.Viz.Base/EnvPath.h"

namespace Clu
{
	namespace Viz
	{
		namespace Wnd
		{

			// Get the version of CluVizCore
			CLUVIZDLL_API void GetVersion(char* pcVersion, int iBufLen);

			CLUVIZDLL_API Clu::CIString GetPath(Clu::Viz::EEnvPath eEnvPath);

			CLUVIZDLL_API Clu::CIImage LoadImage(const Clu::CIString& sFilename);
			CLUVIZDLL_API void SaveImage(const Clu::CIString& sFilename, const Clu::CIImage& xImage);

			// Load an Icon
			CLUVIZDLL_API void* GetIconHandle(const char* pcFilename, bool bModule = false);

			// Get path to script for specific file type
			//CLUVIZDLL_API void GetFileTypeScript(const char* pcFileType, int iMaxLen, char* pcData);

			// Start the CLUViz Engine.
			CLUVIZDLL_API void Start();
			CLUVIZDLL_API void StartAgain();
			void _Start(bool bInitAll);

			//CLUVIZDLL_API void GetStdEnvPath(char* pcPath, int iMaxLen);

			// End the engine
			CLUVIZDLL_API void End();

			CLUVIZDLL_API void WaitForEnd();
			CLUVIZDLL_API void EnableUserEnd(bool bVal);
			CLUVIZDLL_API bool IsRunning(int iWait);
			CLUVIZDLL_API bool IsViewValid(int iHandle);

			CLUVIZDLL_API void InfoBox(const char* pcText);

			CLUVIZDLL_API void SetScriptPath(const char* pcPath);

			CLUVIZDLL_API void Create(int& iHandle, int iX, int iY, int iWidth, int iHeight, const char* pcTitle);

			CLUVIZDLL_API void CreateEx1(int& iHandle, int iX, int iY, int iWidth, int iHeight, const char* pcTitle, unsigned int uWindowStyle);

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
				unsigned int uWindowStyle);

			CLUVIZDLL_API void Destroy(int iHandle);
			CLUVIZDLL_API void DestroyAll();

			CLUVIZDLL_API void ShowWindow(int iHandle);
			CLUVIZDLL_API void HideWindow(int iHandle);
			CLUVIZDLL_API void SetWindowPosSize(int iHandle, int iX, int iY, int iWidth, int iHeight);

			// Get Script Output
			CLUVIZDLL_API void GetScriptOutput(int iHandle, char* pcText, int iMaxLen, bool& bIsError);

			CLUVIZDLL_API void SetScript(int iHandle, const char* pcScript);
			CLUVIZDLL_API void LoadScript(int iHandle, const char* pcFilename);
			CLUVIZDLL_API void LoadScript(int iHandle, const char* pcFilename, const char* pcPass);
			// iArgCnt: Number of arguments for script. pcArg contains iArgCnt strings separated by \r (0x13).
			CLUVIZDLL_API void LoadScript(int iHandle, const char* pcFilename, const char* pcPass, int iArgCnt, const char* pcArg);

			CLUVIZDLL_API bool GetLastError(char* pcError, int iMaxLen);
			CLUVIZDLL_API void SetTimeout(unsigned int uMilliSeconds);

			CLUVIZDLL_API void ExecUser(int iHandle, const char* pcToolName = 0);
			CLUVIZDLL_API void FullScreen(int iHandle, bool bVal = true);
			CLUVIZDLL_API void FullScreen(int iHandle, bool bVal, bool bEnableUserToggle);

			CLUVIZDLL_API void SetVarNumber(int iHandle, const char* pcVarName, int iVal);
			CLUVIZDLL_API void GetVarNumber(int iHandle, const char* pcVarName, int& iVal);

			CLUVIZDLL_API void SetVarNumber(int iHandle, const char* pcVarName, double dVal);
			CLUVIZDLL_API void GetVarNumber(int iHandle, const char* pcVarName, double& dVal);

			CLUVIZDLL_API void SetVarString(int iHandle, const char* pcVarName, const char* pcString);
			CLUVIZDLL_API void GetVarString(int iHandle, const char* pcVarName, char* pcString, int iMaxLen);

			CLUVIZDLL_API void SetVarTensor(int iHandle, const char* pcVarName, int iDimCnt, const int* piDim, const double* pdData);
			CLUVIZDLL_API void SetVarTensor(int iHandle, const char* pcVarName, int iDimCnt, const int* piDim, const float* pfData);
			//CLUVIZDLL_API bool SetVarTensor( int iHandle, const char *pcVarName, const vector<int>& vecDim, const vector<double>& vecData );
			CLUVIZDLL_API void GetVarTensor(int iHandle, const char* pcVarName, std::vector<int>& vecDim, std::vector<double>& vecData);
			CLUVIZDLL_API void GetVarTensorProp(int iHandle, const char* pcVarName, unsigned& uDimCnt, unsigned& uElCnt);
			CLUVIZDLL_API void GetVarTensorData(int iHandle, const char* pcVarName, unsigned* puDim, double* pdData);

			CLUVIZDLL_API void SetVarImage(int iHandle, const char* pcVarName, const Clu::CIImage& xImage);
			CLUVIZDLL_API void GetVarImage(int iHandle, const char* pcVarName, Clu::CIImage& xImage);

			CLUVIZDLL_API void SetVarColor(int iHandle, const char* pcVarName, double dRed, double dGreen, double dBlue, double dAlpha);
			CLUVIZDLL_API void SetVarColor(int iHandle, const char* pcVarName, double* pdColor4);
			CLUVIZDLL_API void GetVarColor(int iHandle, const char* pcVarName, double& dRed, double& dGreen, double& dBlue, double& dAlpha);
			CLUVIZDLL_API void GetVarColor(int iHandle, const char* pcVarName, double* pdColor4);

			CLUVIZDLL_API void SetVarObject(int iHandle, const char* pcVarName, SStdVbo* pData);
		} // namespace Wnd
	} // namesapce Viz
}	// namespace Clu
