// Folgender ifdef-Block ist die Standardmethode zum Erstellen von Makros, die das Exportieren
// aus einer DLL vereinfachen. Alle Dateien in dieser DLL werden mit dem NLVIZDLL_EXPORTS-Symbol
// kompiliert, das in der Befehlszeile definiert wurde. Das Symbol darf nicht f�r ein Projekt definiert werden,
// das diese DLL verwendet. Alle anderen Projekte, deren Quelldateien diese Datei beinhalten, erkennen
// CLUVIZDLL_API-Funktionen als aus einer DLL importiert, w�hrend die DLL
// mit diesem Makro definierte Symbole als exportiert ansieht.

#ifdef CLU_VIZ_SINGLE_EXPORTS
	#define CLUVIZDLL_API __declspec(dllexport)
#else
	#define CLUVIZDLL_API __declspec(dllimport)
#endif

#include <string>
#include <vector>
using namespace std;

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
		namespace View
		{
			CLUVIZDLL_API Clu::CIString GetPath(Clu::Viz::EEnvPath eEnvPath);

			// Get the version of CluVizCore
			CLUVIZDLL_API void GetVersion(char* pcVersion, int iBufLen);

			// Start the CLUViz Engine.
			CLUVIZDLL_API void Start(bool bAlone = true);

			// End the engine
			CLUVIZDLL_API void End();

			// Get the error text of the last error that occurred
			CLUVIZDLL_API bool GetLastError(char* pcError, int iMaxLen);

			// Embed a CLUViz window into the window given by hWnd
			CLUVIZDLL_API void EmbedWindow(int& iHandle, __int64 hWnd, bool bCreateSRC = false, __int64* phGlRc = 0, bool bLoadIntroScript = true);

			// Release a CLUViz Window
			CLUVIZDLL_API void ReleaseWindow(int iHandle);
			// Release all CLUViz Windows
			CLUVIZDLL_API void ReleaseAll();

			// Sets a mouse event callback function
			CLUVIZDLL_API void SetMouseEventHandler(int iHandle, Clu::Viz::View::TFuncMouseEventCallback pFunc, void* pvContext);
			// Removes the mouse event callback function
			CLUVIZDLL_API void RemoveMouseEventHandler(int iHandle);

			CLUVIZDLL_API Clu::CIImage LoadImage(const Clu::CIString& sFilename);
			CLUVIZDLL_API void SaveImage(const Clu::CIString& sFilename, const Clu::CIImage& xImage);


			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>
			/// 	Sets the error event handler.
			/// </summary>
			///
			/// <param name="iHandle">   The handle of the view. </param>
			/// <param name="pFunc">	 If non-null, the callback function. </param>
			/// <param name="pvContext"> [in,out] If non-null, a context pointer. </param>
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			CLUVIZDLL_API void SetErrorEventHandler(int iHandle, Clu::Viz::View::TFuncErrorEventCallback pFunc, void* pvContext);

			// Functions to update OGLWin when window GL Window is embedded in has changed
			CLUVIZDLL_API void EW_Display(int iHandle, bool bWait = false);
			CLUVIZDLL_API void EW_Reshape(int iHandle, int iWidth, int iHeight);

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>
			/// 	Gets the width and the height of the view port associated with the given handle.
			/// </summary>
			///
			/// <param name="iHandle"> The handle of the view. </param>
			/// <param name="iWidth">  [out] The width of the view port. </param>
			/// <param name="iHeight"> [out] The height of the view port. </param>
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			CLUVIZDLL_API void EW_GetShape(int iHandle, int& iWidth, int& iHeight);

			CLUVIZDLL_API void EW_Mouse(int iHandle, int iButton, int iState, int iX, int iY);
			//bool EW_Key(int iHandle, unsigned char cKey, int iX, int iY);
			CLUVIZDLL_API void EW_ActiveMouseMove(int iHandle, int iX, int iY);
			CLUVIZDLL_API void EW_PassiveMouseMove(int iHandle, int iX, int iY);
			CLUVIZDLL_API void EW_IdleFunc(int iHandle);
			CLUVIZDLL_API void EW_SetKeyModifierStates(int iHandle, bool bShift, bool bCtrl, bool bAlt, bool bNumLock, bool bCapsLock, bool bScrollLock);
			CLUVIZDLL_API void EW_Keyboard(int iHandle, int iKey, bool bKeyDown);
			CLUVIZDLL_API void EW_LoadScript(int iHandle, const char* pcFilename, const char* pcPass = 0);
			CLUVIZDLL_API void EW_SetScript(int iHandle, const char* pcScript, bool bResetEnv = false, bool bReparseLatex = false);
			CLUVIZDLL_API void EW_ExecUser(int iHandle, const char* pcToolName = 0);
			CLUVIZDLL_API void EW_LockVis(int iHandle, bool bLock);

			CLUVIZDLL_API void EW_MakeCurrent(int iHandle, bool bBind);

			// Further View access functions
			CLUVIZDLL_API void SaveView(int iHandle, const char* pcFilename);

			// Enable/Disable visualization
			CLUVIZDLL_API void EnableVisualization(int iHandle, bool bVal);
			CLUVIZDLL_API void EnableDrawing(int iHandle, bool bVal, bool bDoRedisplay);

			// Get Script Output
			CLUVIZDLL_API void GetScriptOutput(int iHandle, char* pcText, int iMaxLen, bool& bIsError);
			CLUVIZDLL_API void GetScriptOutput(int iHandle, Clu::CIString& sxOutput, bool& bIsError);
			CLUVIZDLL_API void GetSceneGraphPrint(int iHandle, Clu::CIString& sxOutput);

			CLUVIZDLL_API void SetScriptPath(const char* pcPath);

			// Get/Set CLUScript Number
			CLUVIZDLL_API void SetVarNumber(int iHandle, const char* pcVarName, int iVal);
			CLUVIZDLL_API void GetVarNumber(int iHandle, const char* pcVarName, int& iVal);

			CLUVIZDLL_API void SetVarNumber(int iHandle, const char* pcVarName, double dVal);
			CLUVIZDLL_API void GetVarNumber(int iHandle, const char* pcVarName, double& dVal);

			CLUVIZDLL_API void SetVarString(int iHandle, const char* pcVarName, const char* pcString);
			CLUVIZDLL_API void GetVarString(int iHandle, const char* pcVarName, char* pcString, int iMaxLen);

			// TODO: Implement String arrays
			//CLUVIZDLL_API void SetVarStringList(int iHandle, const char* pcVarName, const Clu::CArrayString& aString);

			CLUVIZDLL_API void SetVarTensor(int iHandle, const char* pcVarName, int iDimCnt, const int* piDim, const double* pdData);
			CLUVIZDLL_API void GetVarTensorProp(int iHandle, const char* pcVarName, unsigned& uDimCnt, unsigned& uElCnt);
			CLUVIZDLL_API void GetVarTensorData(int iHandle, const char* pcVarName, unsigned* puDim, double* pdData);

			CLUVIZDLL_API void SetVarImage(int iHandle, const char* pcVarName, const Clu::CIImage& xImage);
			CLUVIZDLL_API void GetVarImage(int iHandle, const char* pcVarName, Clu::CIImage& xImage);

			CLUVIZDLL_API void SetVarColor(int iHandle, const char* pcVarName, double dRed, double dGreen, double dBlue, double dAlpha);
			CLUVIZDLL_API void SetVarColor(int iHandle, const char* pcVarName, double* pdColor4);
			CLUVIZDLL_API void GetVarColor(int iHandle, const char* pcVarName, double& dRed, double& dGreen, double& dBlue, double& dAlpha);
			CLUVIZDLL_API void GetVarColor(int iHandle, const char* pcVarName, double* pdColor4);

			CLUVIZDLL_API void SetVarObject(int iHandle, const char* pcVarName, const Clu::SStdVbo* pData);

			CLUVIZDLL_API void SetVarData(int iHandle, const char* pcVarName, Clu::CIDataContainer xData);

		} // namespace SingleView
	} // namespace Viz
}	// namespace Clu
