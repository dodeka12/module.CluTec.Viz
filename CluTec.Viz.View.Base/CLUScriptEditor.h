////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.View.Base
// file:      CLUScriptEditor.h
//
// summary:   Declares the clu script editor class
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

#if !defined(_CLUSCRIPTEDITOR_H__INCLUDED_)
	#define _CLUSCRIPTEDITOR_H__INCLUDED_

#include "StdAfx.h"
#include "OGLWin.h"
#include "FLTKEditor.h"
#include "FLTKOutput.h"

#include "CluTec.Viz.Fltk\Fl_Help_Dialog.H"

#include <map>

// Insert Code Defines
	#define CSE_IC_COLOR    "  User Defined  "
	#define CSE_IC_RED              "  Red  "
	#define CSE_IC_GREEN    "  Green  "
	#define CSE_IC_BLUE             "  Blue  "
	#define CSE_IC_MAGENTA  "  Magenta  "
	#define CSE_IC_CYAN             "  Cyan  "
	#define CSE_IC_YELLOW   "  Yellow  "
	#define CSE_IC_ORANGE   "  Orange  "
	#define CSE_IC_WHITE    "  White  "
	#define CSE_IC_BLACK    "  Black  "

	#define CSE_IC_E3               "  Euclidean 3D  "
	#define CSE_IC_P3               "  Projective 3D  "
	#define CSE_IC_N3               "  Conformal 3D  "
	#define CSE_IC_C2               "  Conic 2D  "

	#define CSE_IC_VE3              "  Euclidean 3D   "
	#define CSE_IC_VP3              "  Projective 3D   "
	#define CSE_IC_VN3              "  Conformal 3D   "
	#define CSE_IC_VC2              "  Conic 2D   "

	#define CSE_IC_EM_NEW           "  Initial Load  "
	#define CSE_IC_EM_CHANGE        "  Script Changed  "
	#define CSE_IC_EM_ANIMATE       "  Animation Step  "
	#define CSE_IC_EM_USER          "  Action by User  "
	#define CSE_IC_EM_TOOL          "  Tool Changed  "
	#define CSE_IC_EM_KEYUP         "  Key Released  "
	#define CSE_IC_EM_KEYDOWN       "  Key Pressed  "
	#define CSE_IC_EM_RESHAPE       "  Window Size Changed  "
	#define CSE_IC_EM_LINK          "  Info Text Link Selected  "

	#define CSE_IC_MVOP_GEO                 "  Geometric Product  "
	#define CSE_IC_MVOP_INNER               "  Inner Product  "
	#define CSE_IC_MVOP_OUTER               "  Outer Product  "
	#define CSE_IC_MVOP_ADD                 "  Addition  "
	#define CSE_IC_MVOP_SUB                 "  Subtraction  "
	#define CSE_IC_MVOP_DUAL                "  Dual  "
	#define CSE_IC_MVOP_REVERSE             "  Reverse  "
	#define CSE_IC_MVOP_INVOLUTE    "  Main Involution  "

	#define CSE_IC_OM_POINTS                        "  Points  "
	#define CSE_IC_OM_LINES                         "  Lines  "
	#define CSE_IC_OM_LINE_STRIP            "  Line Strip  "
	#define CSE_IC_OM_LINE_LOOP                     "  Line Loop  "
	#define CSE_IC_OM_TRIANGLES                     "  Triangles  "
	#define CSE_IC_OM_TRIANGLE_STRIP        "  Triangle Strip  "
	#define CSE_IC_OM_TRIANGLE_FAN          "  Triangle Fan  "
	#define CSE_IC_OM_QUADS                         "  Quads  "
	#define CSE_IC_OM_QUAD_STRIP            "  Quad Strip  "
	#define CSE_IC_OM_POLYGON                       "  Polygon  "
	#define CSE_IC_OM_LINES_ADJ                     "  Lines w. Adjacency  "
	#define CSE_IC_OM_LINE_STRIP_ADJ        "  Line Strip w. Adjacency  "
	#define CSE_IC_OM_TRIANGLES_ADJ         "  Triangles w. Adjacency  "
	#define CSE_IC_OM_TRIANGLE_STRIP_ADJ    "  Triangle Strip w. Adjacency  "

	#define CSE_IC_IT_LUM                   "  Luminance  "
	#define CSE_IC_IT_LUMA                  "  Luminance + Alpha  "
	#define CSE_IC_IT_RGB                   "  RGB  "
	#define CSE_IC_IT_RGBA                  "  RGB + Alpha  "
	#define CSE_IC_IT_BGR                   "  BGR  "
	#define CSE_IC_IT_BGRA                  "  BGR + Alpha  "

	#define CSE_IC_DT_BYTE                  "   8 bit signed  "
	#define CSE_IC_DT_UBYTE                 "   8 bit unsigned  "
	#define CSE_IC_DT_SHORT                 "  16 bit signed  "
	#define CSE_IC_DT_USHORT                "  16 bit unsigned  "
	#define CSE_IC_DT_INT                   "  32 bit signed  "
	#define CSE_IC_DT_UINT                  "  32 bit unsigned  "
	#define CSE_IC_DT_FLOAT                 "  32 bit floating point  "
	#define CSE_IC_DT_DOUBLE                "  64 bit floating point  "

	#define CSE_IC_GA_E3            "  3D Euclidean Space  "
	#define CSE_IC_GA_P3            "  3D Projective Space  "
	#define CSE_IC_GA_N3            "  3D Conformal Space  "
	#define CSE_IC_GA_C2            "  2D Conic Space  "

	#define CSE_IC_OPNS                     "  Outer Product Null Space  "
	#define CSE_IC_IPNS                     "  Inner Product Null Space  "

	#define CSE_IC_VS_PLOT_POINTS   "  Plot functions as points  "
	#define CSE_IC_VS_PLOT_LINES    "  Plot functions as lines  "
	#define CSE_IC_VS_PLOT_FILL             "  Plot functions filled (default)  "

	#define CSE_IC_VS_POINT_DOT             "  Draw Points as Dots  "
	#define CSE_IC_VS_POINT_SPHERE  "  Draw Points as Spheres  "

	#define CSE_IC_VS_LINE_LINE                     "  Draw Line as Line  "
	#define CSE_IC_VS_LINE_CYLINDER         "  Draw Line as Cylinder  "

	#define CSE_IC_VS_E3_VEC_LINE   "  Draw Vector as Line  "
	#define CSE_IC_VS_E3_VEC_ARROW  "  Draw Vector as Arrow  "
	#define CSE_IC_VS_E3_VEC_POINT  "  Draw Vector as Point  "

	#define CSE_IC_VS_E3_PLANE_RECT "  Draw Plane as Rectangle  "
	#define CSE_IC_VS_E3_PLANE_DISK "  Draw Plane as Disk  "

	#define CSE_IC_VS_N3_SOLID              "  Draw Solid Spheres  "
	#define CSE_IC_VS_N3_WIRE               "  Draw Wire-Frame Spheres  "

	#define CSE_IC_VS_N3_IMAG               "  Draw Imaginary Objects  "
	#define CSE_IC_VS_N3_NO_IMAG    "  Draw NO Imaginary Objects  "

	#define CSE_IC_BLEND_DEFAULT    "  Default blending  "
	#define CSE_IC_BLEND_ADD                "  Default additive  "

	#define CSE_IC_LOOP1                    "  Incremental Loop  "
	#define CSE_IC_LOOP2                    "  Nested Incremental Loop  "

	using namespace std;

	class CCLUScriptEditor :
		public CFLTKEditor
	{
/*
public:
        typedef bool (CCLUScriptEditor::*TTextFuncPtr)();

        struct SCMenuData
        {
                SCMenuData()
                { iFinalPosOffset = 0; pFunc = 0; }

                Set(const char* pcTitle, const char* pcText, int iFinalPosOff, TTextFuncPtr _pFunc)
                {
                        sTitle = pcTitle; sText = pcText; iFinalPosOffset = iFinalPosOff;
                        pFunc = _pFunc;
                }

                string sTitle;	// Name of Menu item which is displayed
                string sText;	// Text to be inserted in editor
                int iFinalPosOffset;  // Number of characters to go back from end of
                                                          // insertion. Can be used to position curser at
                                                          // sensible position.
                TTextFuncPtr pFunc; // If this is not null, this function is called
                                                        // instead of inserting simply the text
        };
*/

	public:

		CCLUScriptEditor(void);
		CCLUScriptEditor(int iPosX, int iPosY, int iWidth, int iHeight, const char* pcFilename = 0, const char* pcEnvPath = 0, void* pvIcon = 0);
		~CCLUScriptEditor(void);

		bool Create(int iPosX, int iPosY, int iWidth, int iHeight, const char* pcFilename = 0, const char* pcEnvPath = 0, void* pvIcon = 0);

		void SetExecPath(const char* pcEnvPath)
		{ m_sEnvPath = pcEnvPath; }

		void SetOGLWin(COGLWin* poglWin)
		{
			m_poglWin = poglWin;
			if (m_poglWin)
			{
				m_poglWin->GetFuncNameList(m_vecFuncName);
				InitStyle();
			}
		}

		void SetOutputWin(CFLTKOutput* pOutputWin)
		{ m_pOutputWin = pOutputWin; }

		void SetScript(const char* pcScript, bool bResetChangedFlag = false);

		void Parse(bool bResetEnv = false, bool bReparseLatex = false, const char* pcPath = 0, const char* pcName = 0, int iArgCnt = 0, const char* pcArg = 0);

		virtual bool LoadFile(const char* pcFilename, int iPos = -1, const char* pcPass = 0);
		virtual bool SaveFile(const char* pcFilename);

		// Quit Editor window
		virtual void Quit();
		// PreQuit is executed before Quit
		//virtual void PreQuit();

	protected:

		void Help();
		void About();
		void InitStyle();

		void MenuInsertCode();
		void InsertColor();

		void MenuShowEditorWin();
		void MenuShowOutputWin();
		void MenuShowVisWin();
		void MenuParseMain(bool bResetEnv = false, bool bReparseLatex = false);

	protected:

		// Overloaded virtual functions
		void InitMenu();
		void New();
		void Load();
		void Insert();
		void Close();
		void SaveAs();

		virtual void LoadNoExec();

		bool SaveBinary(const char* pcFilename);
		bool LoadBinary(const char* pcFilename, const char* pcPass = 0);

		void InitStyleTable();
		void StyleParse(const char* pcText, char* pcStyle, int iLength);

		// Is called when text is pasted into editor with pasted text in sText.
		// If text need not be adapted returns false. If it returns true,
		// then the text in sNewText is inserted instead of original text.
		bool AdaptText(string& sNewText, string& sText);

		uchar GetColorValue(double dVal)
		{
			if (dVal <= 0.0) { return 0; }
			else if (dVal >= 1.0) { return 255; }
			else{ return uchar(dVal * 255.0); }
		}

		double GetColorValue(uchar uVal)
		{
			return double(uVal) / 255.0;
		}

	protected:

		static void CB_LoadNoExec(Fl_Widget* pWidget, void* pData);
		//static void CB_SaveBinary(Fl_Widget *pWidget, void *pvData);

		static void CB_InsertCode(Fl_Widget* pWidget, void* pvData);

		static void CB_Parse(Fl_Widget* pWidget, void* pvData);
		static void CB_ParseForce(Fl_Widget* pWidget, void* pvData);
		static void CB_ParseMain(Fl_Widget* pWidget, void* pvData);
		static void CB_ParseMainForce(Fl_Widget* pWidget, void* pvData);
		static void CB_Help(Fl_Widget* pWidget, void* pvData);
		static void CB_About(Fl_Widget* pWidget, void* pvData);

		static void CB_NextScript(Fl_Widget* pWidget, void* pData);
		static void CB_PrevScript(Fl_Widget* pWidget, void* pData);
		static void CB_ReloadScript(Fl_Widget* pWidget, void* pData);
		static void CB_ClearScriptList(Fl_Widget* pWidget, void* pData);
		static void CB_ShowScriptList(Fl_Widget* pWidget, void* pData);
		static void CB_ScriptListWindow(Fl_Widget* pWidget, void* pData);

		static void CB_ShowEditorWin(Fl_Widget* pWidget, void* pvData);
		static void CB_ShowOutputWin(Fl_Widget* pWidget, void* pvData);
		static void CB_ShowVisWin(Fl_Widget* pWidget, void* pvData);

	protected:

		COGLWin* m_poglWin;
		CFLTKOutput* m_pOutputWin;
		Fl_Button* m_pFileParse;
		Fl_Button* m_pFileParseForce;

		//Fl_Help_Dialog *m_pHelpDialog;

		bool m_bParseAfterNew;

		string m_sAllowedChars;
		string m_sSymbolChars;
		string m_sStdSingleCharOps;
		string m_sSpcSingleCharOps;
		string m_sEnvPath;
		map<string, char> m_mapHighlight;
		map<string, char> m_mapSymbol;

		map<string, char> m_mapShaderHighlight;
		map<string, char> m_mapShaderSymbol;

		//	map<string, SCMenuData> m_mapCMenuItem;

		vector<string> m_vecFuncName;
	};

#endif
