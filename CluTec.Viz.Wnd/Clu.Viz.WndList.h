#pragma once
#include "CluTec.Types1\IDataContainer.h"
#include "CluTec.Viz.View.Base\CLUVizApp.h"

#include <map>
#include <string>
#include <queue>

#define WM_APP_MSG                      WM_USER + 0x0001
#define APP_MSG_NONE            0x0000
#define APP_MSG_CREATE          0x0001
#define APP_MSG_DESTROY         0x0002
#define APP_MSG_SETSCRIPT       0x0003
#define APP_MSG_LOADSCRIPT      0x0004
#define APP_MSG_EXECUSER        0x0005
#define APP_MSG_SETVAR          0x0006
#define APP_MSG_GETVAR          0x0007
#define APP_MSG_FULLSCREEN      0x0008
#define APP_MSG_WNDPOSSIZE      0x0009
#define APP_MSG_WNDSHOW         0x000A
#define APP_MSG_WNDHIDE         0x000B
#define APP_MSG_INIT            0x000C
#define APP_MSG_FINALIZE        0x000D
#define APP_MSG_EDTSHOW         0x000E
#define APP_MSG_EDTHIDE         0x000F
#define APP_MSG_ISVIEWVALID     0x0010
#define APP_MSG_GETSCRIPTOUTPUT         0x0011

#define APP_MSGQUEUE_SIZE       64

class CDevice_3DX;

using namespace std;

class CCLUVizAppListMT
{
public:

	typedef map<int, CCLUVizApp*> TAppMap;

	class CMsgData
	{
	public:

		CMsgData() { }
		virtual ~CMsgData() { }
	};

	struct SMessage
	{
		SMessage(int _iID, CMsgData* _pData)
		{ iID = _iID; pData = _pData; }

		int iID;
		CMsgData* pData;
	};

	template<class T>
	class CValData : public CMsgData
	{
	public:

		CValData(const T& _Val) { Val = _Val; }
		virtual ~CValData() { }

		operator T() { return Val; }

	public:

		T Val;
	};

	template<class T>
	class CHandleValData : public CMsgData
	{
	public:

		CHandleValData() { iHandle = -1; }
		CHandleValData(int _iHandle, const T& _Val)
		{ iHandle = _iHandle; Val = _Val; }
		virtual ~CHandleValData() { }

		operator T() { return Val; }

	public:

		int iHandle;
		T Val;
	};

	class CCreateData : public CMsgData
	{
	public:

		CCreateData(int _iX,
				int _iY,
				int _iWidth,
				int _iHeight,
				const char* _pcTitle,
				const char* _pcInitScript,
				const char* _pcCLUPath,
				bool _bVizOnly,
				bool _bShowIntro,
				void* _pvIcon,
				void* _pvParentHandle,
				unsigned int _uWindowStyle,
				const char* _pcLicensee)
		{
			Set(_iX, _iY, _iWidth, _iHeight, _pcTitle, _pcInitScript, _pcCLUPath,
					_bVizOnly, _bShowIntro, _pvIcon, _pvParentHandle, _uWindowStyle,
					_pcLicensee);
		}

		virtual ~CCreateData() { }

		void Set(int _iX,
				int _iY,
				int _iWidth,
				int _iHeight,
				const char* _pcTitle,
				const char* _pcInitScript,
				const char* _pcCLUPath,
				bool _bVizOnly,
				bool _bShowIntro,
				void* _pvIcon,
				void* _pvParentHandle,
				unsigned int _uWindowStyle,
				const char* _pcLicensee)
		{
			iX             = _iX;
			iY             = _iY;
			iWidth         = _iWidth;
			iHeight        = _iHeight;
			pcTitle        = _pcTitle;
			pcInitScript   = _pcInitScript;
			pcCLUPath      = _pcCLUPath;
			bVizOnly       = _bVizOnly;
			bShowIntro     = _bShowIntro;
			pvIcon         = _pvIcon;
			pvParentHandle = _pvParentHandle;
			uWindowStyle   = _uWindowStyle;
			pcLicensee     = _pcLicensee;
		}

	public:

		int iX, iY;
		int iWidth, iHeight;
		const char* pcCLUPath;
		const char* pcTitle;
		const char* pcInitScript;
		const char* pcLicensee;
		bool bVizOnly;
		bool bShowIntro;
		void* pvIcon;
		void* pvParentHandle;
		unsigned int uWindowStyle;
	};

	struct SFullScreenData
	{
		SFullScreenData()
		{ Set(false, true); }

		SFullScreenData(bool bEnable, bool bEnableUserToggle)
		{
			Set(bEnable, bEnableUserToggle);
		}

		virtual ~SFullScreenData() { }

		void Set(bool _bEnable, bool _bEnableUserToggle)
		{
			bEnable           = _bEnable;
			bEnableUserToggle = _bEnableUserToggle;
		}

		bool bEnable;
		bool bEnableUserToggle;
	};

	struct SWndPosSizeData
	{
		SWndPosSizeData()
		{ Set(0, 0, 0, 0); }

		SWndPosSizeData(int _iX, int _iY, int _iWidth, int _iHeight)
		{
			Set(_iX, _iY, _iWidth, _iHeight);
		}

		virtual ~SWndPosSizeData() { }

		void Set(int _iX, int _iY, int _iWidth, int _iHeight)
		{
			iX      = _iX;
			iY      = _iY;
			iWidth  = _iWidth;
			iHeight = _iHeight;
		}

		int iX, iY;
		int iWidth, iHeight;
	};

	template<class T>
	struct SVarData
	{
		SVarData() { pcVarName = 0; pVar = 0; }
		SVarData(const char* _pcVarName, T* _pVar)
		{ pcVarName = _pcVarName; pVar = _pVar; }

		const char* pcVarName;
		T* pVar;
	};

	struct SScriptData
	{
		SScriptData() { pcFilename = 0; pcPass = 0; iArgCnt = 0; pcArg = 0; }

		SScriptData(const char* _pcFilename, const char* _pcPass)
		{ pcFilename = _pcFilename; pcPass = _pcPass; iArgCnt = 0; pcArg = 0; }

		SScriptData(const char* _pcFilename, const char* _pcPass, int _iArgCnt, const char* _pcArg)
		{ pcFilename = _pcFilename; pcPass = _pcPass; iArgCnt = _iArgCnt; pcArg = _pcArg; }

		const char* pcFilename;
		const char* pcPass;
		int iArgCnt;
		const char* pcArg;
	};

	struct SColorData
	{
		SColorData() { pdColor[0] = 0; pdColor[1] = 0; pdColor[2] = 0; pdColor[3] = 0; }
		SColorData(double dRed, double dGreen, double dBlue, double dAlpha)
		{ pdColor[0] = dRed; pdColor[1] = dGreen; pdColor[2] = dBlue; pdColor[3] = dAlpha; }

		SColorData(double* pdCol)
		{ if (pdCol) { for (int i = 0; i < 4; ++i) { pdColor[i] = pdCol[i]; } } }

		double pdColor[4];
	};

	struct SSetTensorData
	{
		SSetTensorData() { iDimCnt = 0; piDim = 0; pdData = 0; }
		SSetTensorData(int _iDimCnt, const int* _piDim, const double* _pdData)
		{ iDimCnt = _iDimCnt; piDim = _piDim; pdData = _pdData; }

		int iDimCnt;
		const int* piDim;
		const double* pdData;
	};

	struct SGetTensorData
	{
		SGetTensorData() { pvecDim = 0; pvecData = 0; }
		SGetTensorData(vector<int>* _pvecDim, vector<double>* _pvecData)
		{ pvecDim = _pvecDim; pvecData = _pvecData; }

		vector<int>* pvecDim;
		vector<double>* pvecData;
	};

	struct STensorProp
	{
		STensorProp() { uDimCnt = 0; uElCnt = 0; }

		unsigned uDimCnt;
		unsigned uElCnt;
	};

	struct STensorData
	{
		STensorData() { puDim = 0; pdData = 0; }
		STensorData(unsigned* _puDim, double* _pdData)
		{ puDim = _puDim; pdData = _pdData; }

		unsigned* puDim;
		double* pdData;
	};

	//struct SImgData
	//{
	//	SImgData()
	//	{
	//		iWidth         = 0;
	//		iHeight        = 0;
	//		iImgType       = 0;
	//		iDataType      = 0;
	//		iBytesPerPixel = 0;
	//		pvData         = 0;
	//	}

	//	SImgData(int _iWidth, int _iHeight, int _iImgType, int _iDataType, int _iBytesPerPixel, void* _pvData)
	//	{
	//		iWidth         = _iWidth;
	//		iHeight        = _iHeight;
	//		iImgType       = _iImgType;
	//		iDataType      = _iDataType;
	//		iBytesPerPixel = _iBytesPerPixel;
	//		pvData         = _pvData;
	//	}

	//	int iWidth, iHeight;
	//	int iImgType, iDataType;
	//	int iBytesPerPixel;
	//	void* pvData;
	//};

	struct SGetScriptOutputData
	{
		SGetScriptOutputData() { pvecText = 0; pbIsError = false; }
		SGetScriptOutputData(std::vector<std::string>* _pvecText, bool* _pbIsError)
		{ pvecText = _pvecText; pbIsError = _pbIsError; }

		std::vector<std::string>* pvecText;
		bool* pbIsError;
	};

	class CException
	{
	public:

		CException() { }
	};

public:

	CCLUVizAppListMT(bool bInitAll);

	//bool GetFileTypeScript(const char* pcFileType, int iMaxLen, char* pcData);

	bool Start(const char* pcEnvPath, const char* pcModulePath);
	bool End();
	bool WaitForEnd();

	bool EnableUserEnd(bool bVal = true);
	bool IsRunning(int iWait = 0);
	bool IsViewValid(int iHandle);

	bool Create(int& iHandle,
			int iX,
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
			unsigned int uWindowStyle = CLU_WNDSTYLE_STD);
	bool Destroy(int iHandle);
	bool DestroyAll();

	bool ShowWindow(int iHandle);
	bool HideWindow(int iHandle);
	bool ShowEditor(int iHandle);
	bool HideEditor(int iHandle);
	bool SetWindowPosSize(int iHandle, int iX, int iY, int iWidth, int iHeight);

	bool SetScript(int iHandle, const char* pcScript);
	bool LoadScript(int iHandle, const char* pcFilename, const char* pcPass = 0);
	bool LoadScript(int iHandle, const char* pcFilename, const char* pcPass, int iArgCnt, const char* pcArg);

	bool SetScriptPath(const char* pcPath);

	bool GetScriptOutput(int iHandle, std::vector<std::string>& vecText, bool& bIsError);

	bool ExecUser(int iHandle, const char* pcToolName = 0);
	bool FullScreen(int iHandle, bool bVal = true, bool bEnableUserToggle = true);

	bool SetVar(int iHandle, const char* pcVarName, int iVal);
	bool GetVar(int iHandle, const char* pcVarName, int& iVal);

	bool SetVar(int iHandle, const char* pcVarName, double dVal);
	bool GetVar(int iHandle, const char* pcVarName, double& dVal);

	bool SetVar(int iHandle, const char* pcVarName, const string& sVal);
	bool GetVar(int iHandle, const char* pcVarName, string& sVal);

	bool SetVar(int iHandle, const char* pcVarName, int iDimCnt, const int* piDim, const double* pdData);
	bool SetVar(int iHandle, const char* pcVarName, const vector<int>& vecDim, const vector<double>& vecData)
	{ return SetVar(iHandle, pcVarName, (int) vecDim.size(), &vecDim[0], &vecData[0]); }

	bool GetVar(int iHandle, const char* pcVarName, vector<int>& vecDim, vector<double>& vecData);
	bool GetVar(int iHandle, const char* pcVarName, unsigned& uDimCnt, unsigned& uElCnt);
	bool GetVar(int iHandle, const char* pcVarName, unsigned* puDim, double* pdData);

	// Set Image Data
	bool SetVar(int iHandle, const char* pcVarName, const Clu::CIImage& xImage);

	// Get image properties
	bool GetVar(int iHandle, const char* pcVarName, Clu::CIImage& xImage);

	// Get image data
	bool GetVar(int iHandle, const char* pcVarName, void* pvData);

	// Set Color data
	bool SetVar(int iHandle, const char* pcVarName, double dRed, double dGreen, double dBlue, double dAlpha);

	// Get color data
	bool GetVar(int iHandle, const char* pcVarName, double& dRed, double& dGreen, double& dBlue, double& dAlpha);

	// Set vertex array data
	bool SetVar(int iHandle, const char* pcVarName, Clu::SStdVbo* pVexArray);

	// Set Timeout in milliseconds
	void SetTimeout(unsigned int uTimeout)
	{ m_uTimeout = uTimeout; }

	// Get Last Error that occurred
	string GetLastError();

public:

	~CCLUVizAppListMT(void);

protected:

	void WaitForEventMsg(int iMillisec);

	//inline bool AddMsg( int iMsgID, CMsgData* pData );
	//inline bool GetMsg( int &iMsgID, CMsgData** ppData );

	bool InitViz();
	bool FinalizeViz();

	bool MsgInitViz();
	bool MsgFinalizeViz();

	bool MsgCreate();
	bool MsgDestroy();
	bool MsgIsViewValid();
	bool MsgShowWindow();
	bool MsgHideWindow();
	bool MsgSetWndPosSize();
	bool MsgShowEditor();
	bool MsgHideEditor();
	bool MsgSetScript();
	bool MsgLoadScript();
	bool MsgExecUser();
	bool MsgFullScreen();
	bool MsgSetVar();
	bool MsgGetVar();
	bool MsgGetScriptOutput();

	template<class T>
	bool DoSetVar(volatile CHandleValData<SVarData< T > >* pData);
	bool DoSetVar(volatile CHandleValData<SVarData< SSetTensorData > >* pData);
	bool DoSetVar(volatile CHandleValData<SVarData< Clu::CIImage > >* pData);
	bool DoSetVar(volatile CHandleValData<SVarData< SColorData > >* pData);
	bool DoSetVar(volatile CHandleValData<SVarData< Clu::SStdVbo > >* pData);

	template<class T>
	bool DoGetVar(volatile CHandleValData<SVarData< T > >* pData);
	bool DoGetVar(volatile CHandleValData<SVarData< SGetTensorData > >* pData);
	bool DoGetVar(volatile CHandleValData<SVarData< STensorProp > >* pData);
	bool DoGetVar(volatile CHandleValData<SVarData< STensorData > >* pData);
	bool DoGetVar(volatile CHandleValData<SVarData< Clu::CIImage > >* pData);
	bool DoGetVar(volatile CHandleValData<SVarData< SColorData > >* pData);

	// DestryAll called from thread context
	bool TC_DestroyAll();

protected:

	static void Run(void* pcData);

protected:

	int m_iCurHandle;
	TAppMap m_mapApp;

	string m_sLastError;
	string m_sEnvPath;
	string m_sLicencePath;
	string m_sScriptPath;
	string m_sModulePath;

	CCLUCodeBase* m_pCLUCodeBase;

	bool m_bIsRunning;
	volatile bool m_bDoRun;
	volatile bool m_bAllowUserEnd;
	HANDLE m_hMutexAccess;
	HANDLE m_hMutexRun;
	HANDLE m_hMutexMsg;
	HANDLE m_phMutexAccessMsg[2];

	unsigned int m_uTimeout, m_uEventTimeout;

	//volatile queue<SMessage> m_MsgQ;

	volatile int m_iMsgID;
	volatile CMsgData* m_pMsgData;
	volatile bool m_bMsgSuccess;
	HANDLE m_hEventMsg;
	DWORD m_dwThreadID;

	CDevice_3DX* m_pDevice3DX;

	unsigned __int64 m_uDNG_This;

	unsigned char m_pcMsgID[32];

	int m_iRanSeed;
	bool m_bInitAll;
};
