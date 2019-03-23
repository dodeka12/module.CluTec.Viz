////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      CLUCodeBase.h
//
// summary:   Declares the clu code base class
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

// CLUCodeBase.h: Schnittstelle f�r die Klasse CCLUCodeBase.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLUCODEBASE_H__154A039C_A470_4AE1_99F2_F7D9072E1496__INCLUDED_)
	#define AFX_CLUCODEBASE_H__154A039C_A470_4AE1_99F2_F7D9072E1496__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif	// _MSC_VER > 1000

#include <map>
#include <string>
#include <vector>
#include <time.h>
#include <sys/timeb.h>


#include "CluTec.Viz.Draw\OGLDirectWrite.h"

#include "CluTec.Viz.Xml\XML.h"
#include "CodeBase.h"
//#include "CLUParse.h"
	class CCLUParse;

#ifdef WIN32
    #include "SyncSerialComm.h"
#endif

#ifdef WIN32
		typedef struct _timeb TTimeStruct;
		#define GETTIME(S) _ftime_s(S)
#else
		typedef struct timeb TTimeStruct;
		#define GETTIME(S) ftime(S)
#endif

	using namespace std;

//////////////////////////////////////////
/// Plot Modes

	#define PLOT_POINTS                     0x0001
	#define PLOT_LINES                      0x0002
	#define PLOT_FILL                       0x0003

//////////////////////////////////////////
/// Execution Modes

	#define EXEC_MODE_NONE          0x0000	// Code executed for no reason ;-)
	#define EXEC_MODE_NEW           0x0001	// Code executed for the first time.
	#define EXEC_MODE_CHANGE        0x0002	// Code has been changed.
	#define EXEC_MODE_ANIM          0x0004	// Code is run due to animation
	#define EXEC_MODE_USER          0x0008	// Code is run because of user interaction
	#define EXEC_MODE_TOOL          0x0010	// Code is run because tool changed value
	#define EXEC_MODE_AGAIN         0x0020	// Code is run because some command in script
	// makes it necessary to rerun code.
	#define EXEC_MODE_KEYDOWN       0x0040	// Code is run because key was pressed down
	#define EXEC_MODE_KEYUP         0x0080	// Code is run because key was released
	#define EXEC_MODE_RESHAPE       0x0100	// Code is run because vis. window was reshaped
	#define EXEC_MODE_LINK          0x0200	// Code is run because link has been activated
	#define EXEC_MODE_PICKOVER      0x0400	// Code is run because a 'Pick Over' event was detected
	#define EXEC_MODE_PICKSEL       0x0800	// Code is run because a 'Pick Select' event was detected
	#define EXEC_MODE_PICKDRAG      0x1000	// Code is run because a 'Pick Drag' evevnt was detected
	#define EXEC_MODE_QUIT          0x2000	// Code is run because CLUCalc is closed

//////////////////////////////////////////

#include "CluTec.Viz.Draw\OGLText.h"
#include "CluTec.Viz.Draw\OGLMVFilter.h"
#include "CluTec.Viz.Draw\OGLDrawBase.h"
#include "CluTec.Viz.Draw\OGLLatexText.h"
#include "CluTec.Viz.Draw\CLUDrawBase.h"


	class CCLUCodeBase : public CCodeBase
	{
	public:

		class SortError : public CCluError
		{
		public:

			SortError() : CCluError("Error in sorting operation") { }
		};

	public:

		struct SScriptFile
		{
			string sTitle;
			string sFilename;
		};

		struct SViewData
		{
			int iX, iY, iW, iH;
		};

		enum ECurSpaceVars
		{
			SPACEVARS_NONE,
			SPACEVARS_E3,
			SPACEVARS_P3,
			SPACEVARS_N3,
			SPACEVARS_C2,
			SPACEVARS_NC2,
			SPACEVARS_E8
		};

		enum ETensorSettings
		{
			TENSOR_MAX_VALENCE = 10
		};

		//struct SFontData
		//{
		//	string sName;
		//	int iFont;
		//	vector<int> vecSize;
		//};

		enum EStringType
		{
			STRTYPE_NORMAL,
			STRTYPE_LATEX,
			STRTYPE_HTML,
			STRTYPE_TEXT
		};

		struct SOutputObject
		{
			CStrMem sName;
			CStrMem sValue;
			CStrMem sType;	// The type string
			CStrMem sRelSym;	// A symbol like '=' or '->'
			CStrMem sInfo;	// Additional info, like the size of a matrix or list

			int iSize;	// give relative size of font to be used
			// if this is 0 then use standard, +1 is one step higher,
			// -1 is one step lower, etc.

			bool bShowInfo;
			bool bShowValue;
			bool bBRBeforeValue;
			bool bBRAfterValue;
			bool bCanHideValue;
			bool bCanChangeSize;
		};

		typedef vector<SOutputObject> TOutObjList;
		#ifdef WIN32
			typedef map<string, CSyncSerialComm> TSerialIOMap;
		#endif

	public:

		CCLUCodeBase();
		virtual ~CCLUCodeBase();

		void Reset();
		// Reset variables like font type, latex mag step, etc.
		void ResetEnvVars();
		void ResetSpaceVars();
		void ResetInfoVars();
		void ResetSerialIO();
		void ResetOutputObjectList();
		void TryResetResourceHandles();
		void PostResetResourceHandles();

		void CollectGarbage();

		// Clean up stuff the user might have forgotten.
		void CleanUp();

		// Return reference to Output Object List
		const TOutObjList& GetOutputObjectList() { return m_vecOutputObject; }
		void InsertOutputObject(const SOutputObject& rObj)
		{ m_vecOutputObject.insert(m_vecOutputObject.begin(), rObj); }

		// Check that scalar is in particular range, taking sensitivity into account.
		bool IsInRange(TCVScalar& dVal, TCVScalar dMin, TCVScalar dMax, int iLine, int iPos);

		// Script List functions
		void DrawScriptFileIndex();
		void ClearScriptList();
		bool AddScriptToList(string sTitle, string sFilename);
		int NextScriptFile();
		int PrevScriptFile();
		string GetCurScriptFilename();
		string GetCurScriptTitle();
		vector<SScriptFile>& GetScriptList() { return m_vecScriptList; }
		int GetCurScriptIdx() { return m_iCurScript; }
		bool SetCurScriptIdx(int iVal);

		void SetFontPath(const char* pcPath)
		{
			m_Text.SetFontPath(pcPath);
		}

		bool SetFont(const string& sFont, const string& sFace, int iMagStep)
		{
			return m_Text.SetFont(sFont, sFace, iMagStep);
		}

		void InitText()
		{
			m_Text.InitCmdMap();

			m_xDirectWrite.Init();
		}

		void SetScriptPath(const char* pcScriptPath)
		{
			m_sScriptPath = pcScriptPath;
			m_oglLatex.SetScriptPath(pcScriptPath);
		}

		string GetScriptPath() { return m_sScriptPath; }

		void SetScriptName(const char* pcScriptName)
		{
			m_sScriptName = pcScriptName;
			m_oglLatex.SetScriptName(pcScriptName);
		}

		string GetScriptName() { return m_sScriptName; }

		//void InitBitmapFont();
		//bool SetBitmapFont(int iFont);
		//bool SetBitmapFontSize(int iSize);

		void SetMVFilter(COGLMVFilter* pFilter) { m_pFilter = pFilter; }
		void SetDrawBase(COGLDrawBase* pDrawBase)
		{ m_pDrawBase = pDrawBase; if (m_pDrawBase) { m_MainSceneRef = m_pDrawBase->GetScene(); } }

		void SetTransform(std::vector<STransform>* pxTransform)
		{
			m_pxTransform = pxTransform;
		}

		std::vector<STransform>& GetTransform()
		{
			return *m_pxTransform;
		}

		void SetFrameRotor(MultiV<float>* pFrameRot) { m_pFrameRotor = pFrameRot; }
		void SetCLUDrawBase(CCLUDrawBase* pCLUDrawBase) { m_pCLUDrawBase = pCLUDrawBase; }

		void SetCLUParse(CCLUParse* pCLUParse) { m_pCLUParse = pCLUParse; }

		void SetVersion(int iMajor, int iMinor, int iRevision);
		void GetVersion(int& iMajor, int& iMinor, int& iRevision);

		double TextRound(double dVal);

		CCodeErrorList& GetErrorList() { return m_ErrorList; }
		COGLDrawBase* GetOGLDrawBase() { return m_pDrawBase; }
		CCLUDrawBase* GetCLUDrawBase() { return m_pCLUDrawBase; }
		COGLMVFilter* GetFilter() { return m_pFilter; }
		TCVScalar& GetSensitivity() { return m_fSensitivity; }
		COGLBEReference GetMainSceneRef() { return m_MainSceneRef; }
		COGLText& GetOGLText() { return m_Text; }
		OGLDirectWrite& GetDirectWrite() { return m_xDirectWrite;  }

		E3GA<TCVScalar>& GetE3GABase() { return m_E3GABase; }
		#ifdef USE_GA_E8
			E8GA<TCVScalar>& GetE8GABase() { return m_E8GABase; }
		#endif
		PGA<TCVScalar>& GetP3GABase() { return m_P3GABase; }
		El2GA<TCVScalar>& GetEl2GABase() { return m_El2GABase; }
		ConfGA<TCVScalar>& GetN3GABase() { return m_N3GABase; }

		#ifdef USE_GA_CC2
			CC2GA<TCVScalar>& GetCC2GABase() { return m_CC2GABase(); }
		#endif

		#ifdef WIN32
			TSerialIOMap& GetMapSerialIO() { return m_mapSerialIO; }
		#endif

		int GetPlotMode() { return m_iPlotMode; }

		float& GetVBMPAlign() { return m_fVBMPAlign; }
		float& GetHBMPAlign() { return m_fHBMPAlign; }
		float& GetVLatexAlign() { return m_fVLatexAlign; }
		float& GetHLatexAlign() { return m_fHLatexAlign; }
		float& GetCurBitmapScale() { return m_fCurBitmapScale; }

		int& GetLatexMagStep() { return m_iLatexMagStep; }
		string& GetLatexMainFont() { return m_sLatexMainFont; }

		COGLLatexText& GetOGLLatex() { return m_oglLatex; }

		int& GetTextPrec() { return m_iTextPrec; }

		COGLVertex& GetBMPPos() { return m_xBMPPos; }

		Rand& GetRandom() { return m_Random; }
		TCVScalar GetPi() { return m_fPi; }
		TCVScalar GetRadPerDeg() { return m_fRadPerDeg; }

		ECurSpaceVars& GetCurSpaceVars() { return m_eCurSpaceVars; }

		TCVScalar& GetCurrentInfoWidth() { return m_dCurrentInfoWidth; }

		TTimeStruct& GetStartTime() { return m_tmStart; }


		// String of vector in null basis
		bool VecN3String(TMultiV& vA, CStrMem& csVal, EStringType eStrType = STRTYPE_NORMAL, const char* pcFormat = 0);

		// String of projective vector
		bool VecP3String(TMultiV& vA, CStrMem& csVal, EStringType eStrType = STRTYPE_NORMAL, const char* pcFormat = 0);

		// String of Euclidean vector
		bool VecE3String(TMultiV& vA, CStrMem& csVal, EStringType eStrType = STRTYPE_NORMAL, const char* pcFormat = 0);

		// String of multivector in El2
		bool VecEl2String(TMultiV& vA, CStrMem& csVal, EStringType eStrType = STRTYPE_NORMAL, const char* pcFormat = 0);

		#ifdef USE_GA_CC2
			// String of multivector in CC2
			bool VecNC2String(TMultiV& vA, CStrMem& csVal, EStringType eStrType = STRTYPE_NORMAL);
		#endif

		#ifdef USE_GA_E8
			// String of multivector in E8
			bool VecE8String(TMultiV& vA, CStrMem& csVal, EStringType eStrType = STRTYPE_NORMAL, const char* pcFormat = 0);
		#endif

		// Transform matrix into string
		bool MatrixString(TMatrix& M, CStrMem& csVal, EStringType eStrType = STRTYPE_NORMAL, const char* pcFormat = 0);

		// Transform tensor into string
		bool TensorString(TTensor& T, CStrMem& csVal, EStringType eStrType = STRTYPE_NORMAL, const char* pcFormat = 0);

		// Transform tensor index into string
		bool TensorIdxString(TTensorIdx& TIdx, CStrMem& csVal, EStringType eStrType = STRTYPE_NORMAL, const char* pcFormat = 0);

		// Transform variable into latex string.
		// Used to implement special string transformations
		bool CastToLatexString(CCodeVar& rVar, CStrMem& csVal, const char* pcFormat = 0);

		// Transform variable into text string.
		// Used to implement special string transformations
		bool CastToTextString(CCodeVar& rVar, CStrMem& csVal, const char* pcFormat = 0);

		// Transform variable into HTML string.
		// Used to implement special string transformations
		bool CastToHTMLString(CCodeVar& rVar, CStrMem& csVal, const char* pcFormat = 0);

		// Transform variable into string.
		// Used to implement special string transformations
		bool CastToString(CCodeVar& rVar, CStrMem& csVal, const char* pcFormat = 0);

		bool CastToFloat(CCodeVar& rVar, float& rfVal);
		bool CastToDouble(CCodeVar& rVar, double& rdVal);
		bool CastMVtoE3(const TMultiV& vA, TMultiV& vR);
		bool CastMVtoP3(const TMultiV& vA, TMultiV& vR);
		bool CastMVtoEl2(TMultiV& vA, TMultiV& vR);

		#ifdef USE_GA_CC2
			bool CastMVtoNC2(TMultiV& vA, TMultiV& vR);
		#endif

		#ifdef USE_GA_E8
			bool CastMVtoE8(TMultiV& vA, TMultiV& vR);
		#endif

		// Math functions that need to be here because they are used in operators
		bool Pow(CCodeVar& rVar, CCodeVar& rLVar, CCodeVar& rRVar, int iLine, int iPos);

		bool OpLShift(CCodeVar& rLVar, CCodeVar& rRVar, int iLine, int iPos);
		bool OpPointLShift(CCodeVar& rLVar, CCodeVar& rRVar, int iLine, int iPos);

		bool OpEqual(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
		bool OpPointEqual(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
		bool OpNotEqual(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
		bool OpPointNotEqual(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
		bool OpAND(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
		bool OpPointAND(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
		bool OpOR(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
		bool OpPointOR(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
		bool OpNOT(CCodeVar& rVar, CCodeVar& rResVar, int iLine, int iPos);

		bool OpGreater(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
		bool OpPointGreater(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);

		bool OpLess(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
		bool OpPointLess(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);

		bool OpGreaterEqual(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
		bool OpPointGreaterEqual(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);

		bool OpLessEqual(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
		bool OpPointLessEqual(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);

		bool OpAssign(CCodeVar& rLVar, CCodeVar& rRVar, int iLine, int iPos);
		bool OpReference(CCodeVar& rLVar, CCodeVar& rRVar, int iLine, int iPos);

		bool OpReverse(CCodeVar& rVar, CCodeVar& rResVar, int iLine, int iPos);

		bool OpWedge(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
		bool OpPointWedge(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);

		bool OpSubtract(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
		bool OpPointSubtract(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);

		bool OpNegate(CCodeVar& rVar, CCodeVar& rResVar, int iLine, int iPos);
		bool OpDual(CCodeVar& rVar, CCodeVar& rResVar, int iLine, int iPos);
		bool OpInvolute(CCodeVar& rVar, CCodeVar& rResVar, int iLine, int iPos);
		bool OpInverse(CCodeVar& rVar, CCodeVar& rResVar, int iLine, int iPos);

		bool OpInnerProd(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
		bool OpPointInnerProd(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);

		bool OpProd(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
		bool OpPointProd(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
		bool OpAdd(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
		bool OpPointAdd(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);

		bool OpDiv(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
		bool OpPointDiv(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);

		bool OpPow(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
		bool OpGradeProj(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
//	bool OpElementSelect(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
		bool OpJoin(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
		bool OpMeet(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);

//	bool ModFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);
		bool OpMod(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);

		bool OpStepList(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);

		bool OpOGLDraw(CCodeVar& rVar, CCodeVar& rResVar, int iLine, int iPos);
		bool OpPrint(CCodeVar& rVar, CCodeVar& rResVar, int iLine, int iPos);

		bool OpFuncCall(CCodeVar& rLVar, CCodeVar& rRVar, CCodeVar& rResVar, int iLine, int iPos);
		bool CallSceneMemberFunc(CCodeVar& rVar, CCodeVar& rObj, TVarList& rIdxList, int iLine, int iPos);
		bool GetObjectElements(CCodeVar& rVar, CCodeVar& rObj, TVarList& rIdxList, bool bInList, bool bIsIdxList, int iLine, int iPos);
		bool GetVarListElement(CCodeVar& rElVar, TVarList& rList, TVarList& rIdxList, int iIdxPos, int iLine, int iPos);
		bool GetVarListElement(CCodeVar& rElVar, TVarList& rList, TString& sID);

		bool GetVarData(Mem<TCVScalar>& rmData, CCodeVar& rVar);
		bool GetVarData(TOGLColor& colData, CCodeVar& rVar);

		bool RemoveVarListElement(TVarList& rList, TVarList& rIdxList, int iIdxPos, int iLine, int iPos);

		bool CastToMatrix(TMatrix& rMat, CCodeVar& rVar, int iLine, int iPos);
		bool SetPlotMode(int iMode);

		bool RepeatFunc(CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos);

		void SetRenderLatex(bool bVal = true);
		void OverrideSetRenderLatex(bool bVal = true) { m_bOverrideSetRenderLatex = bVal; }

		////////////////////////////////////////////////////////////////////////////////////////
		//// Vertex List Functions
		bool VexListGetSize(COGLVertexList& rVexList, CCodeVar& rVar, int iLine, int iPos);
		bool VexListAddData(COGLVertexList* pVexList, CCodeVar& rVar, int iDataType, int iIdxListID, int iLine, int iPos);
		bool VexListCreateFormGrid(COGLVertexList& rVexList, TVarList& rPar, int iLine, int iPos);
		bool VexListCreateFormLineStrip(COGLVertexList& rVexList, TVarList& rPar, int iLine, int iPos);
		bool VexListAdaptCoordImage1D(COGLVertexList& rVexList,
				TImage& rImg,
				int iCoord,
				TVarList& rPar,
				double& dValueMin,
				double& dValueMax,
				double dValueOffset,
				int iLine,
				int iPos);
		bool VexListAdaptCoordImage2D(COGLVertexList& rVexList, TImage& rImg, int iCoord, TVarList& rPar, int iLine, int iPos);

		bool DrawMatrix(TMatrix& xA, int iLine, int iPos);

		// Execute a user defined function
		bool ExecUserFunc(CCodeVar& rResVar, TCodePtr pCodePtr, TVarList& rParList, int iLine, int iPos);

		void ResetTempImageList() { m_mTempImageList[m_iActTempImageList].Set(0); }
		MemObj<TImage>& GetActTempImageList() { return m_mTempImageList[m_iActTempImageList]; }
		void ToggleTempImageList()
		{ if (m_iActTempImageList) { m_iActTempImageList = 0; } else{ m_iActTempImageList = 1; } }

	public:

		static bool StaticListSortCompare(int iLIdx, int iRIdx);
		bool ListSortCompare(int iLIdx, int iRIdx);
		static bool GetImgTypeID(int& iImgType, int& iDataType, const CStrMem& pcImgType, const CStrMem& pcDataType);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// TODO: Trix functions
		//int AddTrixReader();
		//Clu::FileIO::CTrixReader* GetTrixReader(int iTrixReaderHandle);
		//bool IsValidTrixReaderHandle(int iTrixReaderHandle);
		//void ResetTrixReaderMap();

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Enable or disable the RTView Mode (a.k.a flight mode)
		/// </summary>
		///
		/// <param name="bVal"> true to value. </param>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void SetRTViewMode(bool bVal)
		{
			m_pCLUDrawBase->EnableRTViewMode(bVal);
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Sets the camera view center. This functions ensures that a bounding sphere defined by fRadius around the target is
		/// 	visible.
		/// </summary>
		///
		/// <param name="fX">	   The f x coordinate. </param>
		/// <param name="fY">	   The f y coordinate. </param>
		/// <param name="fZ">	   The f z coordinate. </param>
		/// <param name="fRadius"> The radius. </param>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void SetRTViewLookAt(float fX, float fY, float fZ)
		{
			m_pCLUDrawBase->RTViewLookAt(fX, fY, fZ);
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Sets the camera look at position.
		/// </summary>
		///
		/// <param name="fX">	   The f x coordinate. </param>
		/// <param name="fY">	   The f y coordinate. </param>
		/// <param name="fZ">	   The f z coordinate. </param>
		/// <param name="fRadius"> The radius. </param>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void SetRTViewLookAt(float fX, float fY, float fZ, float fRadius)
		{
			m_pCLUDrawBase->RTViewLookAt(fX, fY, fZ, fRadius);
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Sets camera position.
		/// </summary>
		///
		/// <param name="fX"> The f x coordinate. </param>
		/// <param name="fY"> The f y coordinate. </param>
		/// <param name="fZ"> The f z coordinate. </param>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void SetRTViewPosition(float fX, float fY, float fZ)
		{
			m_pCLUDrawBase->RTViewSetPosition(fX, fY, fZ);
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Resets the global transform
		/// </summary>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void ResetView()
		{
			m_pCLUDrawBase->Reset3dView();
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Sets rotation axis inversion.
		/// </summary>
		///
		/// <param name="bAxisX"> true to invert the x Axis Rotation. </param>
		/// <param name="bAxisY"> true to invert the y Axis Rotation. </param>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void SetRotationAxisInversion(bool bAxisX, bool bAxisY)
		{
			m_pCLUDrawBase->RTViewSetRotationAxisInversion(bAxisX, bAxisY);
		}

	public:

		static CCLUCodeBase* sm_pCurCodeBase;
		TVarList* m_pSortVarList;
		bool m_bSortAscend;
		TCodePtr m_pSortCode;
		int m_iSortLine, m_iSortPos;

	protected:

		TCVScalar m_fPi, m_fRadPerDeg;
		TCVScalar m_fSensitivity;

		int m_iPlotMode;

		E3GA<TCVScalar> m_E3GABase;
		#ifdef USE_GA_E8
			E8GA<TCVScalar> m_E8GABase;
		#endif
		PGA<TCVScalar> m_P3GABase;
		ConfGA<TCVScalar> m_N3GABase;
		El2GA<TCVScalar> m_El2GABase;
		#ifdef USE_GA_CC2
			CC2GA<TCVScalar> m_CC2GABase;
		#endif

		COGLMVFilter* m_pFilter;
		COGLDrawBase* m_pDrawBase;
		COGLBEReference m_MainSceneRef;

		std::vector<STransform>* m_pxTransform;
		MultiV<float>* m_pFrameRotor;
		CCLUDrawBase* m_pCLUDrawBase;
		CCLUParse* m_pCLUParse;

		MemObj<MemObj<TImage> > m_mTempImageList;
		int m_iActTempImageList;

		COGLLatexText m_oglLatex;
		int m_iLatexMagStep;
		string m_sLatexMainFont;

		float m_fVBMPAlign, m_fHBMPAlign;
		float m_fVLatexAlign, m_fHLatexAlign;
		COGLVertex m_xBMPPos;
		map<string, COGLBitmap> m_mapBitmap;

		vector<SScriptFile> m_vecScriptList;
		int m_iCurScript;

		bool m_bOverrideSetRenderLatex;

		bool m_bUseBMPTrans;
		COGLColor m_oglBMPTransCol;

		//int m_iCurBitmapFont;
		//int m_iCurBitmapFontSize;
		float m_fCurBitmapScale;
		//vector<SFontData> m_vecFont;

		TTimeStruct m_tmStart;

		TOutObjList m_vecOutputObject;

		ECurSpaceVars m_eCurSpaceVars;

		string m_sScriptPath;
		string m_sScriptName;

		Rand m_Random;

		vector<int> m_vecVersion;

		// Number of digit behind the decimal point
		int m_iTextPrec;

		TCVScalar m_dCurrentInfoWidth;

		// List that contains all pickable objects
		COGLBaseElementList m_PickList;

		// Text
		COGLText m_Text;

		// New Text Renderer
		OGLDirectWrite m_xDirectWrite;

		// TODO: Implement Trix reader
		//int m_iTrixHandleIdx;
		//// std::map to store all open instances of the trix loader class
		//std::map<int, Clu::FileIO::CTrixReader*> m_mapTrixReader;

		bool m_bNeedResourceHandleReset;

		#ifdef WIN32
			TSerialIOMap m_mapSerialIO;
		#endif
	};

#endif	// !defined(AFX_CLUCODEBASE_H__154A039C_A470_4AE1_99F2_F7D9072E1496__INCLUDED_)
