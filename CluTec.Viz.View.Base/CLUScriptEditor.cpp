////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.View.Base
// file:      CLUScriptEditor.cpp
//
// summary:   Implements the clu script editor class
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

#include "stdafx.h"

#include "CLUScriptEditor.h"
#include "CluTec.System/FilePath.h"
#include "CluTec.Viz.Base/Environment.h"

#include <sys/types.h>
#include <sys/stat.h>

#ifdef _GNUCPP3_
    #include <unistd.h>
	#define _getcwd getcwd
	#define _chdir chdir
	#define _stat stat
//#   define _remove remove
#else
    #include <direct.h>
    #include <io.h>
#endif

#ifdef WIN32
    #include <ShellApi.h>
    #include "commdlg.h"
#endif

#include <string>

#include "CluTec.Viz.Fltk\Fl_Color_Chooser.H"
#include "CluTec.Viz.Xml\XML.h"
#include "CluTec.Viz.Parse\Encode.h"

using namespace std;

CCLUScriptEditor::CCLUScriptEditor(void)
{
	InitStyle();

	m_bParseAfterNew = true;

	m_sFileChooserLoadTitle = "Load File";
	m_sFileChooserSaveTitle = "Save File";

	m_sFileChooserPattern = "CLUScript File:*.clu;CLUBinary File:*.cbn;XML File:*.xml;TeX File:*.tex;Text File:*.txt";

	m_poglWin    = 0;
	m_pOutputWin = 0;
}

CCLUScriptEditor::CCLUScriptEditor(int iPosX, int iPosY, int iWidth, int iHeight, const char* pcFilename, const char* pcEnvPath, void* pvIcon)
{
	InitStyle();

	m_sFileChooserLoadTitle = "Load File";
	m_sFileChooserSaveTitle = "Save File";

	m_sFileChooserPattern = "CLUScript File:*.clu;CLUBinary File:*.cbn;XML File:*.xml;TeX File:*.tex;Text File:*.txt";

	m_poglWin    = 0;
	m_pOutputWin = 0;

	Create(iPosX, iPosY, iWidth, iHeight, pcFilename, pcEnvPath, pvIcon);
}

CCLUScriptEditor::~CCLUScriptEditor(void)
{
}

bool CCLUScriptEditor::Create(int iPosX, int iPosY, int iWidth, int iHeight, const char* pcFilename, const char* pcEnvPath, void* pvIcon)
{
	if (!CFLTKEditor::Create(iPosX, iPosY, iWidth, iHeight, pcFilename, pvIcon))
	{
		return false;
	}

	//////////////////////////////////////////
	//begin();

	//m_pFileChoice->position(60, 23);

	m_pFileParse = new Fl_Button(0, 2, 25, 15);
	m_pFileParse->box(FL_BORDER_BOX);
	m_pFileParse->labelsize(10);
	m_pFileParse->labelcolor(FL_BLUE);
	m_pFileParse->label("@>");
	m_pFileParse->color(FL_LIGHT2);
	m_pFileParse->color2(FL_WHITE);
	m_pFileParse->tooltip("Run Script (ctrl + p)");
	m_pFileParse->callback((Fl_Callback*) CCLUScriptEditor::CB_Parse, this);

	m_pFileParseForce = new Fl_Button(0, 2, 25, 15);
	m_pFileParseForce->box(FL_BORDER_BOX);
	m_pFileParseForce->labelsize(10);
	m_pFileParseForce->labelcolor(FL_BLUE);
	m_pFileParseForce->label("@>>");
	m_pFileParseForce->color(FL_LIGHT2);
	m_pFileParseForce->color2(FL_WHITE);
	m_pFileParseForce->tooltip("Run Script, Force Latex Rendering (ctrl + shift + p)");
	m_pFileParseForce->callback((Fl_Callback*) CCLUScriptEditor::CB_ParseForce, this);

	m_pToolBar->insert(*m_pFileParseForce, m_pFileChoice);
	m_pToolBar->insert(*m_pFileParse, m_pFileChoice);
	//end();
	//////////////////////////////////////////

	Show();

	//string sFile;

	if (pcEnvPath)
	{
		m_sEnvPath = pcEnvPath;
	}
	else
	{
		m_sEnvPath = ".";
	}

	//sFile = m_sExecPath + "/CLUCalcHelp.html";

//#ifndef _DEBUG
//	if ( CCLUParse::m_Licence.TestCLUVizFeature( 0x02000000 ) )
//#endif
	{
		m_sFileChooserPattern = "CLUScript File:*.clu;CLUBinary File:*.cbn;XML File:*.xml;TeX File:*.tex;Text File:*.txt";
	}
//#ifndef _DEBUG
//	else
//	{
//		m_sFileChooserPattern = "CLUScript File:*.clu;XML File:*.xml;TeX File:*.tex;Text File:*.txt";
//	}
//#endif

	//m_pHelpDialog = new Fl_Help_Dialog;
	//m_pHelpDialog->load(sFile.c_str());
	//m_pHelpDialog->hide();

	return true;
}

void CCLUScriptEditor::InitStyle()
{
	m_mapHighlight.clear();
	m_mapShaderHighlight.clear();
	m_mapShaderSymbol.clear();

	int iIdx, iCnt = (int) m_vecFuncName.size();

	for (iIdx = 0; iIdx < iCnt; ++iIdx)
	{
		m_mapHighlight[m_vecFuncName[iIdx]] = 'E';
	}

	m_mapHighlight["Red"]   = 'F';
	m_mapHighlight["Green"] = 'F';
	m_mapHighlight["Blue"]  = 'F';

	m_mapHighlight["Cyan"]    = 'F';
	m_mapHighlight["Yellow"]  = 'F';
	m_mapHighlight["Magenta"] = 'F';

	m_mapHighlight["Orange"] = 'F';
	m_mapHighlight["White"]  = 'F';
	m_mapHighlight["Black"]  = 'F';

	m_mapHighlight["MRed"]   = 'F';
	m_mapHighlight["MGreen"] = 'F';
	m_mapHighlight["MBlue"]  = 'F';

	m_mapHighlight["MCyan"]    = 'F';
	m_mapHighlight["MYellow"]  = 'F';
	m_mapHighlight["MMagenta"] = 'F';

	m_mapHighlight["MOrange"] = 'F';
	m_mapHighlight["MGray"]   = 'F';
	m_mapHighlight["MWhite"]  = 'F';

	m_mapHighlight["DRed"]   = 'F';
	m_mapHighlight["DGreen"] = 'F';
	m_mapHighlight["DBlue"]  = 'F';

	m_mapHighlight["DCyan"]    = 'F';
	m_mapHighlight["DYellow"]  = 'F';
	m_mapHighlight["DMagenta"] = 'F';

	m_mapHighlight["DOrange"] = 'F';
	m_mapHighlight["DGray"]   = 'F';
	m_mapHighlight["DWhite"]  = 'F';

	m_mapHighlight["Alpha"]  = 'F';
	m_mapHighlight["MAlpha"] = 'F';
	m_mapHighlight["DAlpha"] = 'F';

	m_mapHighlight["OPNS"] = 'F';
	m_mapHighlight["IPNS"] = 'F';

	m_mapHighlight["E3_DRAW_VEC_AS_LINE"]  = 'F';
	m_mapHighlight["E3_DRAW_VEC_AS_ARROW"] = 'F';
	m_mapHighlight["E3_DRAW_VEC_AS_POINT"] = 'F';

	m_mapHighlight["E3_DRAW_PLANE_AS_RECT"] = 'F';
	m_mapHighlight["E3_DRAW_PLANE_AS_DISK"] = 'F';

	m_mapHighlight["N3_OPNS"]      = 'F';
	m_mapHighlight["N3_IPNS"]      = 'F';
	m_mapHighlight["N3_SOLID"]     = 'F';
	m_mapHighlight["N3_WIRE"]      = 'F';
	m_mapHighlight["N3_SHOW_IMAG"] = 'F';
	m_mapHighlight["N3_HIDE_IMAG"] = 'F';

	m_mapHighlight["DRAW_POINT_AS_DOT"]     = 'F';
	m_mapHighlight["DRAW_POINT_AS_SPHERE"]  = 'F';
	m_mapHighlight["DRAW_LINE_AS_LINE"]     = 'F';
	m_mapHighlight["DRAW_LINE_AS_CYLINDER"] = 'F';

	// Plot Modes
	m_mapHighlight["PLOT_POINTS"] = 'F';
	m_mapHighlight["PLOT_LINES"]  = 'F';
	m_mapHighlight["PLOT_FILL"]   = 'F';

	// Fonts
/*
        m_mapHighlight["FONT_FixedSys_Std_9"] = 'F';
        m_mapHighlight["FONT_FixedSys_Bold_9"] = 'F';
        m_mapHighlight["FONT_Times_Std_12"] = 'F';
        m_mapHighlight["FONT_Sylfaen_Bold_24"] = 'F';
        m_mapHighlight["FONT_Symbol_Std_12"] = 'F';
        m_mapHighlight["FONT_Symbol_Bold_12"] = 'F';
        m_mapHighlight["FONT_Symbol_Std_24"] = 'F';
        m_mapHighlight["FONT_T2_Std_12"] = 'F';
        m_mapHighlight["FONT_T2_Std_24"] = 'F';
*/
	m_mapHighlight["true"]  = 'F';
	m_mapHighlight["false"] = 'F';

	m_mapHighlight["ToolName"]       = 'F';
	m_mapHighlight["LinkData"]       = 'F';
	m_mapHighlight["PickData"]       = 'F';
	m_mapHighlight["ExecMode"]       = 'F';
	m_mapHighlight["EM_NEW"]         = 'F';
	m_mapHighlight["EM_CHANGE"]      = 'F';
	m_mapHighlight["EM_ANIMATE"]     = 'F';
	m_mapHighlight["EM_USER"]        = 'F';
	m_mapHighlight["EM_TOOL"]        = 'F';
	m_mapHighlight["EM_LINK"]        = 'F';
	m_mapHighlight["EM_AGAIN"]       = 'F';
	m_mapHighlight["EM_KEYUP"]       = 'F';
	m_mapHighlight["EM_KEYDOWN"]     = 'F';
	m_mapHighlight["EM_RESHAPE"]     = 'F';
	m_mapHighlight["EM_PICK_OVER"]   = 'F';
	m_mapHighlight["EM_PICK_SELECT"] = 'F';
	m_mapHighlight["EM_PICK_DRAG"]   = 'F';
	m_mapHighlight["EM_QUIT"]        = 'F';

	m_mapHighlight["MVOP_GEO"]      = 'F';
	m_mapHighlight["MVOP_INNER"]    = 'F';
	m_mapHighlight["MVOP_OUTER"]    = 'F';
	m_mapHighlight["MVOP_ADD"]      = 'F';
	m_mapHighlight["MVOP_SUB"]      = 'F';
	m_mapHighlight["MVOP_DUAL"]     = 'F';
	m_mapHighlight["MVOP_REVERSE"]  = 'F';
	m_mapHighlight["MVOP_INVOLUTE"] = 'F';

	m_mapHighlight["GA_E3"]  = 'F';
	m_mapHighlight["GA_P3"]  = 'F';
	m_mapHighlight["GA_N3"]  = 'F';
	m_mapHighlight["GA_C2"]  = 'F';
	m_mapHighlight["GA_NC2"] = 'F';

	m_mapHighlight["IMG_8"]      = 'F';
	m_mapHighlight["IMG_U8"]     = 'F';
	m_mapHighlight["IMG_16"]     = 'F';
	m_mapHighlight["IMG_U16"]    = 'F';
	m_mapHighlight["IMG_32"]     = 'F';
	m_mapHighlight["IMG_U32"]    = 'F';
	m_mapHighlight["IMG_FLOAT"]  = 'F';
	m_mapHighlight["IMG_DOUBLE"] = 'F';

	// Image Types
	m_mapHighlight["IMG_RGB"]  = 'F';
	m_mapHighlight["IMG_RGBA"] = 'F';
	m_mapHighlight["IMG_BGR"]  = 'F';
	m_mapHighlight["IMG_BGRA"] = 'F';
	m_mapHighlight["IMG_LUM"]  = 'F';
	m_mapHighlight["IMG_LUMA"] = 'F';

	// Vertex List Types
	m_mapHighlight["OM_POINTS"]                   = 'F';
	m_mapHighlight["OM_LINES"]                    = 'F';
	m_mapHighlight["OM_LINE_STRIP"]               = 'F';
	m_mapHighlight["OM_LINE_LOOP"]                = 'F';
	m_mapHighlight["OM_TRIANGLES"]                = 'F';
	m_mapHighlight["OM_TRIANGLE_STRIP"]           = 'F';
	m_mapHighlight["OM_TRIANGLE_FAN"]             = 'F';
	m_mapHighlight["OM_QUADS"]                    = 'F';
	m_mapHighlight["OM_QUAD_STRIP"]               = 'F';
	m_mapHighlight["OM_POLYGON"]                  = 'F';
	m_mapHighlight["OM_LINES_ADJACENCY"]          = 'F';
	m_mapHighlight["OM_LINE_STRIP_ADJACENCY"]     = 'F';
	m_mapHighlight["OM_TRIANGLES_ADJACENCY"]      = 'F';
	m_mapHighlight["OM_TRIANGLE_STRIP_ADJACENCY"] = 'F';

	m_mapHighlight["dTime"]     = 'F';
	m_mapHighlight["Time"]      = 'F';
	m_mapHighlight["Pi"]        = 'F';
	m_mapHighlight["RadPerDeg"] = 'F';

	m_mapHighlight["VERSION"]      = 'F';
	m_mapHighlight["VERSION_GL"]   = 'F';
	m_mapHighlight["VERSION_GLSL"] = 'F';

	m_mapHighlight["_DoAnimate"]    = 'F';
	m_mapHighlight["_BGColor"]      = 'F';
	m_mapHighlight["_ViewAngle"]    = 'F';
	m_mapHighlight["_FrameBoxSize"] = 'F';
	m_mapHighlight["_2dView"]       = 'F';

	m_mapHighlight["if"]    = 'I';
	m_mapHighlight["else"]  = 'I';
	m_mapHighlight["loop"]  = 'I';
	m_mapHighlight["break"] = 'I';

	m_mapSymbol[">"]  = 'I';
	m_mapSymbol["<"]  = 'I';
	m_mapSymbol[">="] = 'I';
	m_mapSymbol["<="] = 'I';
	m_mapSymbol["=="] = 'I';
	m_mapSymbol["!="] = 'I';
	m_mapSymbol["&&"] = 'I';
	m_mapSymbol["||"] = 'I';
	m_mapSymbol["!!"] = 'I';

	m_mapSymbol[".>"]  = 'I';
	m_mapSymbol[".>="] = 'I';
	m_mapSymbol[".<"]  = 'I';
	m_mapSymbol[".<="] = 'I';
	m_mapSymbol[".=="] = 'I';
	m_mapSymbol[".!="] = 'I';
	m_mapSymbol[".&&"] = 'I';
	m_mapSymbol[".||"] = 'I';

	m_mapSymbol["::"]  = 'G';
	m_mapSymbol["->"]  = 'G';
	m_mapSymbol["<<"]  = 'G';
	m_mapSymbol[".<<"] = 'G';
	m_mapSymbol["./"]  = 'G';
	m_mapSymbol[".*"]  = 'G';
	m_mapSymbol[".."]  = 'G';
	m_mapSymbol[".^"]  = 'G';
	m_mapSymbol[".-"]  = 'G';
	m_mapSymbol[".+"]  = 'G';
	m_mapSymbol["^^"]  = 'G';

	m_sStdSingleCharOps = "-+/={}[]";
	m_sSpcSingleCharOps = ":.*~!%&|?^'";

	m_sAllowedChars =       "_abcdefghijklmnopqrstuvwxyzäöüß"
				"ABCDEFGHIJKLMNOPQRSTUVWXYZÄÖÜ";

	m_sSymbolChars =        "=!-+/:.*~#!%&|?^{}<>[]'";

	// /////////////////////////////////////////////////////////////////
	// Shader

	// Keywords
	m_mapShaderHighlight["if"]              = 'L';
	m_mapShaderHighlight["else"]            = 'L';
	m_mapShaderHighlight["break"]           = 'L';
	m_mapShaderHighlight["for"]             = 'L';
	m_mapShaderHighlight["uniform"]         = 'L';
	m_mapShaderHighlight["in"]              = 'L';
	m_mapShaderHighlight["out"]             = 'L';
	m_mapShaderHighlight["layout"]          = 'L';
	m_mapShaderHighlight["lines_adjacency"] = 'L';
	m_mapShaderHighlight["line_strip"]      = 'L';
	m_mapShaderHighlight["triangle_strip"]  = 'L';
	m_mapShaderHighlight["max_vertices"]    = 'L';
	m_mapShaderHighlight["precision"]       = 'L';
	m_mapShaderHighlight["highp"]           = 'L';
	m_mapShaderHighlight["return"]          = 'L';
	m_mapShaderHighlight["const"]           = 'L';

	// Types
	m_mapShaderHighlight["bool"]  = 'O';
	m_mapShaderHighlight["void"]  = 'O';
	m_mapShaderHighlight["int"]   = 'O';
	m_mapShaderHighlight["float"] = 'O';
	m_mapShaderHighlight["vec2"]  = 'O';
	m_mapShaderHighlight["vec3"]  = 'O';
	m_mapShaderHighlight["vec4"]  = 'O';
	m_mapShaderHighlight["ivec2"] = 'O';
	m_mapShaderHighlight["ivec3"] = 'O';
	m_mapShaderHighlight["ivec4"] = 'O';
	m_mapShaderHighlight["bvec2"] = 'O';
	m_mapShaderHighlight["bvec3"] = 'O';
	m_mapShaderHighlight["bvec4"] = 'O';
	m_mapShaderHighlight["mat2"]  = 'O';
	m_mapShaderHighlight["mat3"]  = 'O';
	m_mapShaderHighlight["mat4"]  = 'O';

	// Functions
	m_mapShaderHighlight["sin"]     = 'P';
	m_mapShaderHighlight["cos"]     = 'P';
	m_mapShaderHighlight["tan"]     = 'P';
	m_mapShaderHighlight["asin"]    = 'P';
	m_mapShaderHighlight["acos"]    = 'P';
	m_mapShaderHighlight["atan"]    = 'P';
	m_mapShaderHighlight["radians"] = 'P';
	m_mapShaderHighlight["degrees"] = 'P';

	m_mapShaderHighlight["pow"]         = 'P';
	m_mapShaderHighlight["exp"]         = 'P';
	m_mapShaderHighlight["log"]         = 'P';
	m_mapShaderHighlight["exp2"]        = 'P';
	m_mapShaderHighlight["log2"]        = 'P';
	m_mapShaderHighlight["sqrt"]        = 'P';
	m_mapShaderHighlight["inversesqrt"] = 'P';

	m_mapShaderHighlight["abs"]        = 'P';
	m_mapShaderHighlight["ceil"]       = 'P';
	m_mapShaderHighlight["clamp"]      = 'P';
	m_mapShaderHighlight["floor"]      = 'P';
	m_mapShaderHighlight["fract"]      = 'P';
	m_mapShaderHighlight["max"]        = 'P';
	m_mapShaderHighlight["min"]        = 'P';
	m_mapShaderHighlight["mix"]        = 'P';
	m_mapShaderHighlight["mod"]        = 'P';
	m_mapShaderHighlight["sign"]       = 'P';
	m_mapShaderHighlight["smoothstep"] = 'P';
	m_mapShaderHighlight["step"]       = 'P';

	m_mapShaderHighlight["cross"]       = 'P';
	m_mapShaderHighlight["distance"]    = 'P';
	m_mapShaderHighlight["dot"]         = 'P';
	m_mapShaderHighlight["faceforward"] = 'P';
	m_mapShaderHighlight["length"]      = 'P';
	m_mapShaderHighlight["normalize"]   = 'P';
	m_mapShaderHighlight["reflect"]     = 'P';
	m_mapShaderHighlight["refract"]     = 'P';

	m_mapShaderHighlight["texture2D"]    = 'P';
	m_mapShaderHighlight["texture2DLod"] = 'P';

	m_mapShaderHighlight["EmitVertex"]   = 'P';
	m_mapShaderHighlight["EndPrimitive"] = 'P';

	m_mapShaderHighlight["main"] = 'P';

	// Variables
	m_mapShaderHighlight["gl_Position"]         = 'N';
	m_mapShaderHighlight["gl_ClipDistance"]     = 'N';
	m_mapShaderHighlight["gl_PointSize"]        = 'N';
	m_mapShaderHighlight["gl_in"]               = 'N';
	m_mapShaderHighlight["gl_MaxClipDistances"] = 'N';
	m_mapShaderHighlight["gl_InstanceID"]       = 'N';

	m_mapShaderHighlight["clu_in_vVertex"] = 'N';
	m_mapShaderHighlight["clu_in_vColor"]  = 'N';
	m_mapShaderHighlight["clu_in_vNormal"] = 'N';
	m_mapShaderHighlight["clu_in_vTex0"]   = 'N';
	m_mapShaderHighlight["clu_in_vTex1"]   = 'N';
	m_mapShaderHighlight["clu_in_vTex2"]   = 'N';
	m_mapShaderHighlight["clu_in_vTex3"]   = 'N';
	m_mapShaderHighlight["clu_in_vTex4"]   = 'N';
	m_mapShaderHighlight["clu_in_vTex5"]   = 'N';
	m_mapShaderHighlight["clu_in_vTex6"]   = 'N';
	m_mapShaderHighlight["clu_in_vTex7"]   = 'N';
	m_mapShaderHighlight["clu_in_iPartId"] = 'N';

	m_mapShaderHighlight["clu_matProjection"]             = 'N';
	m_mapShaderHighlight["clu_matInvProjection"]          = 'N';
	m_mapShaderHighlight["clu_matModelView"]              = 'N';
	m_mapShaderHighlight["clu_matInvModelView"]           = 'N';
	m_mapShaderHighlight["clu_matModelViewProjection"]    = 'N';
	m_mapShaderHighlight["clu_matInvModelViewProjection"] = 'N';
	m_mapShaderHighlight["clu_matTex0"]                   = 'N';
	m_mapShaderHighlight["clu_matTex1"]                   = 'N';
	m_mapShaderHighlight["clu_matTex2"]                   = 'N';
	m_mapShaderHighlight["clu_matTex3"]                   = 'N';
	m_mapShaderHighlight["clu_matTex4"]                   = 'N';
	m_mapShaderHighlight["clu_matTex5"]                   = 'N';
	m_mapShaderHighlight["clu_matTex6"]                   = 'N';
	m_mapShaderHighlight["clu_matTex7"]                   = 'N';

	m_mapShaderHighlight["clu_vOpticalCenter"]             = 'N';
	m_mapShaderHighlight["clu_piViewport"]                 = 'N';
	m_mapShaderHighlight["clu_pfDepthRange"]               = 'N';
	m_mapShaderHighlight["clu_vColor"]                     = 'N';
	m_mapShaderHighlight["clu_fPointSize"]                 = 'N';
	m_mapShaderHighlight["clu_pfPointSizeRange"]           = 'N';
	m_mapShaderHighlight["clu_pfPointDistanceAttenuation"] = 'N';
	m_mapShaderHighlight["clu_fPointFadeThresholdSize"]    = 'N';
	m_mapShaderHighlight["clu_fLineWidth"]                 = 'N';
	m_mapShaderHighlight["clu_iInPickDrawMode"]            = 'N';
	m_mapShaderHighlight["clu_iPickedPartId"]              = 'N';
	m_mapShaderHighlight["clu_pvClipPlanes"]               = 'N';

	// Symbols
	m_mapShaderSymbol["+"]  = 'M';
	m_mapShaderSymbol["-"]  = 'M';
	m_mapShaderSymbol["*"]  = 'M';
	m_mapShaderSymbol["/"]  = 'M';
	m_mapShaderSymbol["<"]  = 'M';
	m_mapShaderSymbol[">"]  = 'M';
	m_mapShaderSymbol["="]  = 'M';
	m_mapShaderSymbol["=="] = 'M';
	m_mapShaderSymbol["!="] = 'M';
	m_mapShaderSymbol[">="] = 'M';
	m_mapShaderSymbol[">="] = 'M';
	m_mapShaderSymbol["+="] = 'M';
	m_mapShaderSymbol["-="] = 'M';
	m_mapShaderSymbol["*="] = 'M';
	m_mapShaderSymbol["/="] = 'M';
	m_mapShaderSymbol["%="] = 'M';
	m_mapShaderSymbol["++"] = 'M';
	m_mapShaderSymbol["--"] = 'M';
	m_mapShaderSymbol["!"]  = 'M';
	m_mapShaderSymbol["%"]  = 'M';
	m_mapShaderSymbol["&"]  = 'M';
	m_mapShaderSymbol["|"]  = 'M';
	m_mapShaderSymbol["^"]  = 'M';
	m_mapShaderSymbol["&&"] = 'M';
	m_mapShaderSymbol["||"] = 'M';
}

//////////////////////////////////////////////////////////////////////
/// InitMenu

void CCLUScriptEditor::InitMenu()
{
	CFLTKEditor::InitMenu();

	#if defined(CLUVIZ_VIEWER)
		return;
	#else
		Fl_Menu_Item pMenuItems[] =
		{
			{ "&Code",              0, 0, 0, FL_SUBMENU },
			{ "&Parse",        FL_CTRL + 'p', (Fl_Callback*) CB_Parse },
			{ "Parse (Force Render Late&x)",  FL_CTRL + FL_SHIFT + 'p', (Fl_Callback*) CB_ParseForce },
			{ "Parse Main",  FL_CTRL + 'm', (Fl_Callback*) CB_ParseMain },
			{ "Parse Main (Force Render Latex)",  FL_CTRL + FL_SHIFT + 'm', (Fl_Callback*) CB_ParseMainForce, 0, FL_MENU_DIVIDER },
			{ "Insert Co&lor",        FL_CTRL + 'l', (Fl_Callback*) CB_InsertCode, (void*) CSE_IC_COLOR },
			{ 0 },

			{ "&Presentation", 0, 0, 0, FL_SUBMENU },
			{ "Ne&xt",        0, (Fl_Callback*) CB_NextScript },
			{ "Pre&vious",    0, (Fl_Callback*) CB_PrevScript, 0, FL_MENU_DIVIDER },
			//{ "&Reload Current",    0, (Fl_Callback *) CB_ReloadScript, 0, FL_MENU_DIVIDER },
			{ "&Show List...",    0, (Fl_Callback*) CB_ShowScriptList },
			{ "&Clear List",    0, (Fl_Callback*) CB_ClearScriptList },
			{ 0 },

			{ "&Window",              0, 0, 0, FL_SUBMENU },
			{ "&Editor", FL_CTRL + FL_SHIFT + 'e', (Fl_Callback*) CB_ShowEditorWin },
			{ "&Output", FL_CTRL + FL_SHIFT + 'o', (Fl_Callback*) CB_ShowOutputWin },
			{ "&Visualization", FL_CTRL + FL_SHIFT + 'v', (Fl_Callback*) CB_ShowVisWin },
			{ 0 },

			{ "&Help",              0, 0, 0, FL_SUBMENU },
			{ "C&ontent", FL_F + 1, (Fl_Callback*) CB_Help },
			{ "About", 0, (Fl_Callback*) CB_About },
			{ 0 },
			{ 0 }
		};

		Fl_Menu_Item pCMenuItems[] =
		{
			{ "  Multivector Interpretation  ", 0, 0, 0, FL_SUBMENU },
			{ CSE_IC_OPNS, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_IPNS, 0, (Fl_Callback*) CB_InsertCode },
			{ 0 },

			{ "  Visualization Styles  ", 0, 0, 0, FL_SUBMENU },
			{ "  Plots  ", 0, 0, 0, FL_SUBMENU },
			{ CSE_IC_VS_PLOT_POINTS, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_VS_PLOT_LINES, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_VS_PLOT_FILL, 0, (Fl_Callback*) CB_InsertCode },
			{ 0 },
			{ "  Points  ", 0, 0, 0, FL_SUBMENU },
			{ CSE_IC_VS_POINT_DOT, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_VS_POINT_SPHERE, 0, (Fl_Callback*) CB_InsertCode },
			{ 0 },
			{ "  Lines  ", 0, 0, 0, FL_SUBMENU },
			{ CSE_IC_VS_LINE_LINE, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_VS_LINE_CYLINDER, 0, (Fl_Callback*) CB_InsertCode },
			{ 0 },
			{ "  3D Euclidean Vectors  ", 0, 0, 0, FL_SUBMENU },
			{ CSE_IC_VS_E3_VEC_LINE, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_VS_E3_VEC_ARROW, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_VS_E3_VEC_POINT, 0, (Fl_Callback*) CB_InsertCode },
			{ 0 },
			{ "  3D Euclidean Planes  ", 0, 0, 0, FL_SUBMENU },
			{ CSE_IC_VS_E3_PLANE_RECT, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_VS_E3_PLANE_DISK, 0, (Fl_Callback*) CB_InsertCode },
			{ 0 },
			{ "  3D Conformal Spheres  ", 0, 0, 0, FL_SUBMENU },
			{ CSE_IC_VS_N3_SOLID, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_VS_N3_WIRE, 0, (Fl_Callback*) CB_InsertCode },
			{ 0 },
			{ "  3D Conformal Imaginary  ", 0, 0, 0, FL_SUBMENU },
			{ CSE_IC_VS_N3_IMAG, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_VS_N3_NO_IMAG, 0, (Fl_Callback*) CB_InsertCode },
			{ 0 },
			{ 0 },

			{ "  Define Variables  ", 0, 0, 0, FL_SUBMENU },
			{ CSE_IC_E3, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_P3, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_N3, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_C2, 0, (Fl_Callback*) CB_InsertCode },
			{ 0 },

			{ "  Vectors  ", 0, 0, 0, FL_SUBMENU },
			{ CSE_IC_VE3, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_VP3, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_VN3, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_VC2, 0, (Fl_Callback*) CB_InsertCode },
			{ 0 },

			{ "  Color  ", 0, 0, 0, FL_SUBMENU },
			{ CSE_IC_COLOR, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_RED, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_GREEN, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_BLUE, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_MAGENTA, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_CYAN, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_YELLOW, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_ORANGE, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_BLACK, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_WHITE, 0, (Fl_Callback*) CB_InsertCode },
			{ 0 },

			{ "  Blending  ", 0, 0, 0, FL_SUBMENU },
			{ CSE_IC_BLEND_DEFAULT, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_BLEND_ADD, 0, (Fl_Callback*) CB_InsertCode },
			{ 0 },

			{ "  Execution Modes  ", 0, 0, 0, FL_SUBMENU },
			{ CSE_IC_EM_NEW, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_EM_CHANGE, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_EM_ANIMATE, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_EM_USER, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_EM_TOOL, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_EM_KEYUP, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_EM_KEYDOWN, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_EM_RESHAPE, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_EM_LINK, 0, (Fl_Callback*) CB_InsertCode },
			{ 0 },

			{ "  Multivector Operation IDs  ", 0, 0, 0, FL_SUBMENU },
			{ CSE_IC_MVOP_GEO, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_MVOP_INNER, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_MVOP_OUTER, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_MVOP_ADD, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_MVOP_SUB, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_MVOP_DUAL, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_MVOP_REVERSE, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_MVOP_INVOLUTE, 0, (Fl_Callback*) CB_InsertCode },
			{ 0 },

			{ "  Vector Space IDs  ", 0, 0, 0, FL_SUBMENU },
			{ CSE_IC_GA_E3, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_GA_P3, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_GA_N3, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_GA_C2, 0, (Fl_Callback*) CB_InsertCode },
			{ 0 },

			{ "  Object Mode IDs  ", 0, 0, 0, FL_SUBMENU },
			{ CSE_IC_OM_POINTS, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_OM_LINES, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_OM_LINE_STRIP, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_OM_LINE_LOOP, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_OM_TRIANGLES, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_OM_TRIANGLE_STRIP, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_OM_TRIANGLE_FAN, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_OM_QUADS, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_OM_QUAD_STRIP, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_OM_POLYGON, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_OM_LINES_ADJ, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_OM_LINE_STRIP_ADJ, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_OM_TRIANGLES_ADJ, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_OM_TRIANGLE_STRIP_ADJ, 0, (Fl_Callback*) CB_InsertCode },
			{ 0 },

			{ "  Image Types  ", 0, 0, 0, FL_SUBMENU },
			{ "  Pixel Type  ", 0, 0, 0, FL_SUBMENU },
			{ CSE_IC_IT_LUM, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_IT_LUMA, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_IT_RGB, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_IT_RGBA, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_IT_BGR, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_IT_BGRA, 0, (Fl_Callback*) CB_InsertCode },
			{ 0 },
			{ "  Data Type  ", 0, 0, 0, FL_SUBMENU },
			{ CSE_IC_DT_BYTE, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_DT_UBYTE, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_DT_SHORT, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_DT_USHORT, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_DT_INT, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_DT_UINT, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_DT_FLOAT, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_DT_DOUBLE, 0, (Fl_Callback*) CB_InsertCode },
			{ 0 },
			{ 0 },

			{ "  Special  ", 0, 0, 0, FL_SUBMENU },
			{ CSE_IC_LOOP1, 0, (Fl_Callback*) CB_InsertCode },
			{ CSE_IC_LOOP2, 0, (Fl_Callback*) CB_InsertCode },
			{ 0 },

			{ 0 }
		};

		Fl_Menu_Item pItem[] =
		{
			{ "&Read File (no exec.)...",    FL_CTRL + 'r', (Fl_Callback*) CB_LoadNoExec }
		};
/*
        Fl_Menu_Item pItemSaveBin[] =
        {
                { "Save as binary...",    0, (Fl_Callback *) CB_SaveBinary }
        };
*/
		m_vecMenuItem.insert(m_vecMenuItem.begin() + 3, *pItem);
		//m_vecMenuItem.insert( m_vecMenuItem.begin() + 7, *pItemSaveBin );

		int iCount = (int) m_vecMenuItem.size();

		m_vecMenuItem.resize(iCount + 22);

		for (int i = 0; i < 23; i++)
		{
			m_vecMenuItem[iCount + i - 1] = pMenuItems[i];
		}

		// Context Menu
		m_vecCMenuItem.resize(132);
		for (int i = 0; i < 132; i++)
		{
			m_vecCMenuItem[i] = pCMenuItems[i];
		}

	#endif
}

void CCLUScriptEditor::InitStyleTable()
{
	//CFLTKEditor::InitStyleTable();

	Fl_Text_Display::Style_Table_Entry pStyleTable[] =
	{	// Style table
		{ FL_BLACK,      FL_COURIER,            14 },	// A - Plain
		{ FL_DARK_GREEN, FL_COURIER_ITALIC, 14 },	// B - Line comments
		{ FL_DARK_GREEN, FL_COURIER_ITALIC, 14 },	// C - Block comments
		{ FL_DARK_CYAN,  FL_COURIER,        14 },	// D - Strings
		{ FL_BLUE,       FL_COURIER_BOLD,   14 },	// E - Functions
		{ FL_DARK_BLUE,  FL_COURIER_BOLD,   14 },	// F - Predefined Variables
		{ FL_DARK_RED,   FL_COURIER_BOLD,   14 },	// G - Std. Operators
		{ FL_DARK_RED,   FL_COURIER_BOLD,   18 },	// H - Special Operators
		{ FL_RED,        FL_COURIER_BOLD,   14 },	// I - Keywords
		{ FL_DARK_GREEN, FL_COURIER_BOLD,   14 },	// J - Preprocessor directives
		{ FL_BLUE,       FL_COURIER,            14 },	// K - Code String: Plain
		{ FL_RED,        FL_COURIER_BOLD,       14 },	// L - Code String: Keywords
		{ FL_DARK_RED,   FL_COURIER_BOLD,   14 },	// M - Code String: Std. Operators
		{ FL_DARK_BLUE,  FL_COURIER_BOLD,   14 },	// N - Code String: Predefined Variables
		{ FL_DARK_GREEN, FL_COURIER_BOLD,   14 },	// O - Code String: Predefined Types
		{ FL_BLUE,       FL_COURIER_BOLD,   14 },	// P - Code String: Functions
		{ FL_DARK_GREEN, FL_COURIER_ITALIC, 14 },	// Q - Code String: Line comments
		{ FL_DARK_GREEN, FL_COURIER_ITALIC, 14 },	// R - Code String: Block comments
		{ FL_DARK_GREEN, FL_COURIER_BOLD,   14 },	// S - Code String: Preprocessor directives
	};

	m_vecStyleTable.clear();
	m_vecStyleTable.resize(19);

	for (int i = 0; i < 19; i++)
	{
		m_vecStyleTable[i] = pStyleTable[i];
	}
}

void CCLUScriptEditor::StyleParse(const char* pcText, char* pcStyle, int iLength)
{
	char pcBuf[256];
	bool bRawString      = false;
	bool bInShaderString = false;
	int iPos;

	if (!m_bReparsing)
	{
		if (pcStyle[0] >= 'K')
		{
			bInShaderString = true;

			char* pcTest = pcStyle;
			for (iPos = 0; iPos < iLength; iPos++, pcTest++)
			{
				if ((*pcTest == 'R'))
				{
					m_bReparseAll = true;
					return;
				}
			}
		}
		else
		{
			char* pcTest = pcStyle;
			for (iPos = 0; iPos < iLength; iPos++, pcTest++)
			{
				if ((*pcTest == 'C') || (*pcTest == 'D'))
				{
					m_bReparseAll = true;
					return;
				}
			}
		}
	}

	for (; iLength > 0;
	     iLength--, pcText++)
	{
		// Check for directives, comments, strings, and keywords...
		if (!bInShaderString && (strncmp(pcText, "//#", 3) == 0))
		{
			while (iLength > 0 && *pcText != '\n')
			{
				*pcStyle = 'J';
				pcStyle++;
				iLength--;
				pcText++;
			}

			if (iLength == 0)
			{
				break;
			}

			*pcStyle++ = 'A';

			continue;
		}
		else if (bInShaderString && (*pcText == '#'))
		{
			while (iLength > 0 && *pcText != '\n')
			{
				*pcStyle = 'S';
				pcStyle++;
				iLength--;
				pcText++;
			}

			if (iLength == 0)
			{
				break;
			}

			*pcStyle++ = 'K';

			continue;
		}
		else if (strncmp(pcText, "//", 2) == 0)
		{
			while (iLength > 0 && *pcText != '\n')
			{
				if (bInShaderString)
				{
					*pcStyle = 'Q';
				}
				else
				{
					*pcStyle = 'B';
				}
				pcStyle++;
				iLength--;
				pcText++;
			}

			if (iLength == 0)
			{
				break;
			}

			*pcStyle++ = 'A';

			continue;
		}
		else if (strncmp(pcText, "/*", 2) == 0)
		{
			char cCurStyle;
			if (bInShaderString)
			{
				cCurStyle = 'R';
			}
			else
			{
				cCurStyle = 'C';
			}

			*pcStyle++ = cCurStyle;
			*pcStyle++ = cCurStyle;
			iLength   -= 2;
			pcText    += 2;

			if (iLength == 0)
			{
				m_bReparseAll = true;
				break;
			}

			while (iLength > 0 && strncmp(pcText, "*/", 2) != 0)
			{
				if (iLength == 1)
				{
					m_bReparseAll = true;
				}

				*pcStyle = cCurStyle;

				pcStyle++;
				iLength--;
				pcText++;
			}

			if ((iLength > 0) && (strncmp(pcText, "*/", 2) == 0))
			{
				if (iLength == 2)
				{
					m_bReparseAll = true;
				}

				*pcStyle++ = cCurStyle;
				*pcStyle++ = cCurStyle;
				iLength--;
				pcText++;
			}

			if (iLength == 0)
			{
				break;
			}

			continue;
		}
		else if (!bInShaderString && (strncmp(pcText, "\\\"", 2) == 0))
		{
			// Quoted quote...
			*pcStyle++ = 'A';
			*pcStyle++ = 'A';
			pcText++;
			iLength--;
			continue;
		}
		else if (!bInShaderString && (strncmp(pcText, "\\\\", 2) == 0))
		{
			// Backslash
			*pcStyle++ = 'A';
			*pcStyle++ = 'A';
			pcText++;
			iLength--;
			continue;
		}
		else if ((*pcText == '\"') || (strncmp(pcText, "@\"", 2) == 0) || ((iLength >= 8) && (strncmp(pcText, "@Shader\"", 8) == 0)))
		{
			if (bInShaderString)
			{
				bInShaderString = false;

				if (*pcText == '\"')
				{
					*pcStyle++ = 'H';
					//pcText++;
					//iLength--;
				}
				else if ((*pcText == '@') && (pcText[1] == '\"'))
				{
					*pcStyle++ = 'A';
					*pcStyle++ = 'H';
					pcText    += 1;
					iLength   -= 1;
				}
				else if ((iLength >= 8) && (strncmp(pcText, "@Shader\"", 8) == 0))
				{
					for (int i = 0; i < 7; ++i)
					{
						*pcStyle++ = 'K';
					}

					*pcStyle++ = 'H';
					pcText    += 7;
					iLength   -= 7;
				}

				if (iLength == 0)
				{
					m_bReparseAll = true;
					break;
				}

				continue;
			}

			if ((*pcText == '@') && (pcText[1] == '\"'))
			{
				bRawString = true;
				*pcStyle++ = 'D';
				pcText++;
				iLength--;
			}
			else if ((iLength >= 8) && (strncmp(pcText, "@Shader\"", 8) == 0))
			{
				bInShaderString = true;
				for (int i = 0; i < 7; ++i)
				{
					*pcStyle++ = 'H';
				}

				*pcStyle++ = 'H';
				pcText    += 7;
				iLength   -= 7;

				if (iLength == 0)
				{
					m_bReparseAll = true;
					break;
				}
				continue;
			}

			*pcStyle++ = 'D';
			pcText++;
			iLength--;

			if (iLength == 0)
			{
				m_bReparseAll = true;
				break;
			}

			// Continuing in string...
			if (bRawString)
			{
				while (iLength > 0)
				{
					if (iLength == 1)
					{
						m_bReparseAll = true;
					}

					if (*pcText == '\"')
					{
						// End quote...
						*pcStyle++ = 'D';
						break;
					}

					*pcStyle++ = 'D';
					pcText++;
					iLength--;
				}
			}
			else
			{
				while (iLength > 0)
				{
					if (iLength == 1)
					{
						m_bReparseAll = true;
					}

					if (strncmp(pcText, "\\\"", 2) == 0)
					{
						// Quoted end quote...
						*pcStyle++ = 'D';
						pcText++;
						iLength--;
					}
					else if (strncmp(pcText, "\\\\", 2) == 0)
					{
						// Backslash
						*pcStyle++ = 'D';
						pcText++;
						iLength--;
					}
					else if (*pcText == '\"')
					{
						// End quote...
						*pcStyle++ = 'D';
						break;
					}
					/*
					else if (*pcText == '\n')
					{
					*pcStyle++ = 'A';
					break;
					}
					*/
					*pcStyle++ = 'D';
					pcText++;
					iLength--;
				}
			}

			if (iLength == 0)
			{
				break;
			}

			continue;
		}
		else if (strchr(m_sAllowedChars.c_str(), *pcText) != 0)
		{
			// Might be a keyword...
			map<string, char>::const_iterator itStyle;
			int iCurPos = 0;
			char cCurrent;

			do
			{
				pcBuf[iCurPos] = pcText[iCurPos];
				iCurPos++;
				iLength--;
			}
			while ((isdigit((unsigned char) pcText[iCurPos]) ||
				strchr(m_sAllowedChars.c_str(), pcText[iCurPos]) != 0)
			       && iLength > 0 && iCurPos < 255);

			pcBuf[iCurPos] = 0;
			if (bInShaderString)
			{
				itStyle = m_mapShaderHighlight.find(pcBuf);

				if (itStyle != m_mapShaderHighlight.end())
				{
					cCurrent = itStyle->second;
				}
				else
				{
					cCurrent = 'K';
				}
			}
			else
			{
				itStyle = m_mapHighlight.find(pcBuf);

				if (itStyle != m_mapHighlight.end())
				{
					cCurrent = itStyle->second;
				}
				else
				{
					cCurrent = 'A';
				}
			}

			for (int i = 0; i < iCurPos; i++)
			{
				*pcStyle++ = cCurrent;
				pcText++;
			}

			if (iLength == 0)
			{
				break;
			}

			pcText--;
			iLength++;

			continue;
		}
		else if (strchr(m_sSymbolChars.c_str(), *pcText) != 0)
		{
			// Might be an extended Symbol
			map<string, char>::const_iterator itStyle;
			char cCurrent;
			bool bHasStyle = false;

			if ((pcText[1] != 0) && (pcText[2] != 0))
			{
				pcBuf[0] = pcText[0];
				pcBuf[1] = pcText[1];
				pcBuf[2] = pcText[2];
				pcBuf[3] = 0;

				if (bInShaderString)
				{
					itStyle = m_mapShaderSymbol.find(pcBuf);

					if (itStyle != m_mapShaderSymbol.end())
					{
						cCurrent  = itStyle->second;
						bHasStyle = true;
					}
				}
				else
				{
					itStyle = m_mapSymbol.find(pcBuf);

					if (itStyle != m_mapSymbol.end())
					{
						cCurrent  = itStyle->second;
						bHasStyle = true;
					}
				}

				if (bHasStyle)
				{
					cCurrent = itStyle->second;
					iLength -= 3;

					for (int i = 0; i < 3; i++)
					{
						*pcStyle++ = cCurrent;
						pcText++;
					}

					if (iLength == 0)
					{
						break;
					}

					pcText--;
					iLength++;

					continue;
				}
			}

			if (pcText[1] != 0)
			{
				pcBuf[0] = pcText[0];
				pcBuf[1] = pcText[1];
				pcBuf[2] = 0;

				if (bInShaderString)
				{
					itStyle = m_mapShaderSymbol.find(pcBuf);

					if (itStyle != m_mapShaderSymbol.end())
					{
						cCurrent  = itStyle->second;
						bHasStyle = true;
					}
				}
				else
				{
					itStyle = m_mapSymbol.find(pcBuf);

					if (itStyle != m_mapSymbol.end())
					{
						cCurrent  = itStyle->second;
						bHasStyle = true;
					}
				}

				if (bHasStyle)
				{
					cCurrent = itStyle->second;
					iLength -= 2;

					for (int i = 0; i < 2; i++)
					{
						*pcStyle++ = cCurrent;
						pcText++;
					}

					if (iLength == 0)
					{
						break;
					}

					pcText--;
					iLength++;

					continue;
				}
			}

			pcBuf[0] = pcText[0];
			pcBuf[1] = 0;

			if (bInShaderString)
			{
				itStyle = m_mapShaderSymbol.find(pcBuf);

				if (itStyle != m_mapShaderSymbol.end())
				{
					cCurrent  = itStyle->second;
					bHasStyle = true;
				}
			}
			else
			{
				itStyle = m_mapSymbol.find(pcBuf);

				if (itStyle != m_mapSymbol.end())
				{
					cCurrent  = itStyle->second;
					bHasStyle = true;
				}
			}

			if (bHasStyle)
			{
				cCurrent   = itStyle->second;
				*pcStyle++ = cCurrent;

				if (iLength == 1)
				{
					break;
				}

				continue;
			}

			if (strchr(m_sStdSingleCharOps.c_str(), *pcText) != 0)
			{
				if (bInShaderString)
				{
					*pcStyle++ = 'K';
				}
				else
				{
					*pcStyle++ = 'G';
				}
				continue;
			}
			else if (strchr(m_sSpcSingleCharOps.c_str(), *pcText) != 0)
			{
				if ((*pcText == '.') && (iLength > 1) && !isdigit(pcText[1]))
				{
					if (bInShaderString)
					{
						*pcStyle++ = 'K';
					}
					else
					{
						*pcStyle++ = 'H';
					}
					continue;
				}
				else if (*pcText != '.')
				{
					if (bInShaderString)
					{
						*pcStyle++ = 'K';
					}
					else
					{
						*pcStyle++ = 'H';
					}
					continue;
				}
			}
		}

		// Copy pcStyle info...
		if (bInShaderString)
		{
			*pcStyle++ = 'K';
		}
		else
		{
			*pcStyle++ = 'A';
		}
	}
}

void CCLUScriptEditor::New()
{
	CFLTKEditor::New();

	if (m_bParseAfterNew)
	{
		Parse(true);
	}
}

void CCLUScriptEditor::Load()
{
	m_bParseAfterNew = false;
	CFLTKEditor::Load();
	m_bParseAfterNew = true;

	if (strstr(GetFilename().c_str(), ".clu") ||
	    strstr(GetFilename().c_str(), ".cbn"))
	{
		Parse(true);
	}
}

bool CCLUScriptEditor::LoadFile(const char* pcFilename, int iPos, const char* pcPass)
{
	if (!pcFilename)
	{
		return false;
	}

	std::string sFilename;
	sFilename = pcFilename;
	sFilename = Clu::CFilePath::MakeAbsolute(sFilename);

	if (sFilename.find(".cbn") != std::string::npos)
	{
		return LoadBinary(sFilename.c_str(), pcPass);
	}
	else
	{
		return CFLTKEditor::LoadFile(sFilename.c_str(), iPos);
	}
}

void CCLUScriptEditor::LoadNoExec()
{
	m_bParseAfterNew = false;
	CFLTKEditor::Load();
	m_bParseAfterNew = true;
}

void CCLUScriptEditor::Insert()
{
	CFLTKEditor::Insert();

	if (strstr(GetFilename().c_str(), ".clu"))
	{
		Parse(true);
	}
}

void CCLUScriptEditor::Close()
{
	CFLTKEditor::Close();

	//Parse(true);
}

void CCLUScriptEditor::SaveAs()
{
	CFLTKEditor::SaveAs();

	if (strstr(GetFilename().c_str(), ".clu"))
	{
		Parse(true);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUScriptEditor::SaveFile(const char* pcFilename)
{
	if (!pcFilename)
	{
		return false;
	}

	// Get absolute file path
	std::string sFilename = Clu::CFilePath::MakeAbsolute(pcFilename);

	// Save as binary?
	if (sFilename.find(".cbn") != std::string::npos)
	{
		// Is already in binary format?
		if (IsFileBinary())
		{
			return true;
		}
		else
		{
			// Save as binary
			return SaveBinary(sFilename.c_str());
		}
	}
	else
	{
		// Save as text
		return CFLTKEditor::SaveFile(sFilename.c_str());
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUScriptEditor::SaveBinary(const char* pcFilename)
{
	if (!m_poglWin)
	{
		return false;
	}

	// Store pre-parsed code in mCode
	std::vector<char> vecCode;
	m_poglWin->m_xParse.GenTextCode(vecCode);

	// Get meta information from source code of main file
	int iEdID = GetEditorID(m_poglWin->GetScriptPath().c_str(), m_poglWin->GetScriptName().c_str());

	std::string sPass;
	int iCurPos = 0, iStartPos, iEndPos;

	MemObj<CStrMem> mMeta;
	mMeta.Set(4);

	// Get pointer to text buffer
	Fl_Text_Buffer* pTextBuf = m_mEditorData[iEdID].m_pTextBuf;

	// Parse binary comments
	while (pTextBuf->search_forward(iCurPos, "//!", &iStartPos))
	{
		// Find end position of current binary comment
		iStartPos += 3;
		if (!pTextBuf->search_forward(iStartPos, ":", &iEndPos))
		{
			iCurPos = iStartPos;
			continue;
		}

		// Read comment label
		char* pcVal = pTextBuf->text_range(iStartPos, iEndPos);
		_strlwr_s(pcVal, strlen(pcVal) + 1);
		std::string sLabel = pcVal;
		free(pcVal);

		// Trim white spaces
		int iFirst = (int) sLabel.find_first_not_of(' ');
		int iLast  = (int) sLabel.find_last_not_of(' ');
		sLabel = sLabel.substr(iFirst, iLast - iFirst + 1);

		// Get line end position
		int iLineEndPos = pTextBuf->line_end(iEndPos);

		// Read until the line end
		char* pcData      = pTextBuf->text_range(pTextBuf->word_start(iEndPos + 1), iLineEndPos);
		std::string sData = pcData;
		free(pcData);

		// Trim white spaces
		iFirst = (int) sData.find_first_not_of(' ');
		iLast  = (int) sData.find_last_not_of(' ');
		sData  = sData.substr(iFirst, iLast - iFirst + 1);

		// Switch binary comment type
		if (sLabel == "title")
		{
			mMeta[0] = sData.c_str();
		}
		else if (sLabel == "version")
		{
			mMeta[1] = sData.c_str();
		}
		else if (sLabel == "copyright")
		{
			mMeta[2] = sData.c_str();
		}
		else if (sLabel == "about")
		{
			mMeta[3] = sData.c_str();
		}
		else if (sLabel == "key")
		{
			sPass = sData;
		}

		// Update current parse position
		iCurPos = iLineEndPos + 1;
	}

	// Copy each binary comment to code memory
	for (int iIdx = 3; iIdx >= 0; iIdx--)
	{
		vecCode.insert(vecCode.begin(), mMeta[iIdx].Len() + 1, 0);
		memcpy(vecCode.data(), mMeta[iIdx].Str(), mMeta[iIdx].Len() + 1);
	}

	vecCode.insert(vecCode.begin(), 5, 0);

	// Write key to identity correctly decoded binary file
	vecCode[0] = char(0x2E);
	vecCode[1] = char(0x41);
	vecCode[2] = char(0x9F);
	vecCode[3] = char(0xD2);
	// Number of meta data entries
	vecCode[4] = char(4);

	// Encode Pre-parsed code
	CEncode xEncode;
	if (!xEncode.WriteFile(pcFilename, "CLUBinary2", vecCode.data(), vecCode.size(), ((sPass.length() == 8) ? sPass.c_str() : 0)))
	{
		fl_alert(xEncode.GetError().c_str());
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CCLUScriptEditor::LoadBinary(const char* pcFilename, const char* pcPass)
{
	m_bLoading = true;

	IsFileChanged() = false;

	GetFilename()  = "";
	IsFileBinary() = false;
	CStrMem csEditorText;
	GetTextBuffer()->remove(0, GetTextBuffer()->length());

	string sError;
	map<string, string> mapMetaData;

	if (!CCLUParse::ReadCLUBinaryFile(pcFilename, *GetBinaryCode(), mapMetaData, sError, pcPass))
	{
		fl_alert(sError.c_str());
		sError = "// " + sError + "\n";

		GetTextBuffer()->insert(0, sError.c_str());
		m_bLoading = false;
		GetTextBuffer()->call_modify_callbacks();
		return false;
	}

	GetMetaTitle()     = mapMetaData["title"];
	GetMetaVersion()   = mapMetaData["version"];
	GetMetaCopyright() = mapMetaData["copyright"];
	GetMetaAbout()     = mapMetaData["about"];

	if (GetMetaTitle().size())
	{
		csEditorText << "// " << GetMetaTitle().c_str();
	}

	if (GetMetaVersion().size())
	{
		csEditorText << " v" << GetMetaVersion().c_str() << "\n";
	}
	else
	{
		csEditorText << "\n";
	}

	if (GetMetaCopyright().size())
	{
		csEditorText << "// Copyright by " << GetMetaCopyright().c_str() << "\n";
	}

	if (GetMetaAbout().size())
	{
		csEditorText << "//\n"
			     << "// " << GetMetaAbout().c_str() << "\n"
			     << "// ";
	}

	csEditorText << "\n// CLUBinary File - Source Code not available\n";

	GetTextBuffer()->insert(0, csEditorText.Str());
	GetFilename()  = pcFilename;
	IsFileBinary() = true;

	SetNameAndPath();
	m_bLoading = false;
	GetTextBuffer()->call_modify_callbacks();

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Help Callback

void CCLUScriptEditor::Help()
{
	#ifdef WIN32
		string sFile;

		sFile = m_sEnvPath + "\\Docs\\CluScript.chm";

		HINSTANCE hInstance = ShellExecuteA(NULL,
			"open",
			sFile.c_str(),
			NULL,
			NULL,
			SW_SHOWNORMAL);	//SHOWMINIMIZED);

		unsigned uResult = (unsigned) hInstance;
		if (uResult <= 32)
		{
			switch (uResult)
			{
			case ERROR_FILE_NOT_FOUND:
				fl_alert("CLU-Script help file not found");
				break;

			case ERROR_PATH_NOT_FOUND:
				fl_alert("CLU-Script help file path not found");
				break;

			default:
				fl_alert("Error showing CLU-Script help. Please find the help file via the Windows Start menu");
			}
		}

	#else	// Linux or other system
		char pcCmd[500];
		char pcLine[300];
		struct stat FileStat;
		ifstream zLog;
		char* pBrowser[] = { "firefox", "konqueror", "mozilla", "netscape" };
		int iPos, iCount = 3;

		// Check whether Mozilla is installed
		::remove("/tmp/clucalc_which.txt");

		for (iPos = 0; iPos < iCount; iPos++)
		{
			sprintf(pcCmd, "which %s > /tmp/clucalc_which.txt", pBrowser[iPos]);
			system(pcCmd);

			// Check whether text file exists
			if ((stat("/tmp/clucalc_which.txt", &FileStat) == 0) && (FileStat.st_size != 0))
			{
				// We found something
				break;
			}
		}

		::remove("/tmp/clucalc_which.txt");

		if (iPos == iCount)
		{
			// Could not find anything
			fl_alert("Could not find any HTML browser to display the help pages.\n"
				 "Please start one manually. You can find the help files under\n"
				 "[CLUCalc directory]/Documentation/index.html");
		}
		else
		{
			sprintf(pcCmd, "%s \"%s/Documentation/index.html\" &", pBrowser[iPos], m_sEnvPath.c_str());
			system(pcCmd);
		}

	#endif
}

void CCLUScriptEditor::Quit()
{
	if (m_poglWin)
	{
		this->m_poglWin->DoQuit();
	}
	else
	{
		CFLTKEditor::Quit();
	}
}

void CCLUScriptEditor::CB_Help(Fl_Widget* pWidget, void* pvData)
{
	if (pvData)
	{
		((CCLUScriptEditor*) pvData)->Help();
	}
}

//////////////////////////////////////////////////////////////////////
/// About Callback

void CCLUScriptEditor::About()
{
	#if defined(CLUVIZ_CLUCALC)
		char* pcText =
			"\tCLUCalc v" CLUVIZ_VERSION "\n\n"
						     "\tBuild " CLUVIZ_BUILD_DATE ", " CLUVIZ_BUILD_TIME "\n\n"
													 "\tCopyright 2002-2016 by Dr. Christian B. U. Perwass.\n"
													 "\tAll rights reserved.\n\n"
													 "\tDeveloped by:\n"
													 "\t\tChristian Perwass.";

		fl_message_title("About CLUCalc");

	#elif defined(CLUVIZ_VIEWER)
		char* pcText =
			"\tCLUView v" CLUVIZ_VERSION "\n\n"
						     "\tBuild " CLUVIZ_BUILD_DATE ", " CLUVIZ_BUILD_TIME "\n\n"
													 "\tCopyright 2002-2016 by Dr. Christian B. U. Perwass.\n"
													 "\tAll rights reserved.\n\n"
													 "\tDeveloped by:\n"
													 "\t\tChristian Perwass.";

		fl_message_title("About CLUView");

	#else
		char* pcText =
			"CluTec Runtime v" CLUVIZ_VERSION "\n\n"
								    "\tBuild " CLUVIZ_BUILD_DATE ", " CLUVIZ_BUILD_TIME "\n\n"
															"\tCopyright 2002-2016 by Dr. Christian B. U. Perwass.\n"
															"\tAll rights reserved.\n\n"
															"\tDeveloped by:\n"
															"\t\tChristian Perwass.";

		fl_message_title("About CLUViz Pro");

	#endif

	fl_message(pcText);
	fl_message_title("");
}

void CCLUScriptEditor::CB_About(Fl_Widget* pWidget, void* pvData)
{
	if (pvData)
	{
		((CCLUScriptEditor*) pvData)->About();
	}
}

//////////////////////////////////////////////////////////////////////
/// Insert Color Callback

void CCLUScriptEditor::InsertColor()
{
	double dR, dG, dB, dA = 1.0;
	size_t nLineStart, nLineEnd, nCurPos, nColorStart = 0, nColorEnd = 0;
	char* pcColText;
	std::string sLine;
	bool bReplace = false;
	Fl_Text_Buffer* pTextBuf;

	// Get current insert position
	nCurPos = (size_t) GetEditor()->insert_position();
	// Get start of line
	nLineStart = (size_t) GetEditor()->line_start(int(nCurPos));
	// Get end of line
	nLineEnd = (size_t) GetEditor()->line_end(int(nCurPos), false);

	// Go to beginning of current word
	//iPos = GetEditor()->word_start(iPos);

	// Get Text buffer
	pTextBuf = GetEditor()->buffer();
	// Get Text
	pcColText = pTextBuf->text_range((int) nLineStart, (int) nLineEnd);
	if (!pcColText)
	{
		return;
	}

	sLine = pcColText;
	free(pcColText);

	// Find Color command that encloses current cursor position
	size_t nPos = 0;
	while ((nColorStart = sLine.find("Color", nPos)) != std::string::npos)
	{
		if ((nColorEnd = sLine.find_first_of(")", nColorStart)) == std::string::npos)
		{
			nColorEnd = sLine.size() - 1;
		}

		if ((nCurPos >= nLineStart + nColorStart) && (nCurPos <= nLineStart + nColorEnd))
		{
			break;
		}

		nPos      = nColorEnd;
		nColorEnd = -1;
	}

	// Initialize Color Components
	dR = dG = dB = dA = 1.0;

	// If Color command is present, then read current color values
	size_t nColorParStart = std::string::npos;
	if ((nColorEnd > nColorStart) && ((nColorParStart = sLine.find_first_of("(", nColorStart)) != std::string::npos))
	{
		bReplace = true;

		string sPar = sLine.substr(nColorParStart + 1, nColorEnd - 1);

		int iColCount = sscanf_s(sPar.c_str(), "%lf, %lf, %lf, %lf", &dR, &dG, &dB, &dA);
		if (iColCount != 4)
		{
			dA        = 1.0;
			iColCount = sscanf_s(sPar.c_str(), "%lf, %lf, %lf", &dR, &dG, &dB);
			if (iColCount != 3)
			{
				dR = dG = dB = 1.0;
			}
		}
	}

	bool bSetColor = false;

	#ifdef WIN32
		static COLORREF pColRef[16]
			= {
			RGB(0, 0, 0), RGB(64, 64, 64), RGB(192, 192, 192), RGB(255, 255, 255),
			RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255),
			RGB(255, 255, 0), RGB(255, 0, 255), RGB(0, 255, 255),
			RGB(192, 0, 0), RGB(0, 192, 0), RGB(0, 0, 192),
			RGB(192, 192, 0), RGB(192, 0, 192), RGB(0, 192, 192)
			};

		static const char* pcColNameList[16]
			= {
			"Black", "DWhite", "MWhite", "White",
			"Red", "Green", "Blue",
			"Yellow", "Magenta", "Cyan",
			"MRed", "MGreen", "MBlue",
			"MYellow", "MMagenta", "MCyan"
			};

		CHOOSECOLORA dlgColor;

		uchar uR, uG, uB;
		uR = GetColorValue(dR);
		uG = GetColorValue(dG);
		uB = GetColorValue(dB);

		dlgColor.lStructSize    = sizeof(CHOOSECOLORA);
		dlgColor.hwndOwner      = NULL;
		dlgColor.hInstance      = 0;
		dlgColor.rgbResult      = RGB(uR, uG, uB);
		dlgColor.lpCustColors   = pColRef;
		dlgColor.Flags          = CC_FULLOPEN | CC_RGBINIT;
		dlgColor.lCustData      = 0;
		dlgColor.lpfnHook       = 0;
		dlgColor.lpTemplateName = 0;

		if (ChooseColorA(&dlgColor) == TRUE)
		{
			bSetColor = true;
			dR        = GetColorValue(GetRValue(dlgColor.rgbResult));
			dG        = GetColorValue(GetGValue(dlgColor.rgbResult));
			dB        = GetColorValue(GetBValue(dlgColor.rgbResult));
		}

	#else
		// Color chooser for other systems
		if (fl_color_chooser("Choose Color", dR, dG, dB))
		{
			bSetColor = true;
		}
	#endif

	if (bSetColor)
	{
		// Check whether chosen color is a known color
		int iCol;
		const char* pcColName = 0;
		for (iCol = 0; iCol < 16; ++iCol)
		{
			if (pColRef[iCol] == dlgColor.rgbResult)
			{
				pcColName = pcColNameList[iCol];
				break;
			}
		}

		char pcText[200];

		if (pcColName)
		{
			if (dA < 1.0)
			{
				sprintf_s(pcText, 200, "%s + %5.3lf * Alpha", pcColName, 5.0 * (1.0 - dA));
			}
			else
			{
				strcpy_s(pcText, 200, pcColName);
			}
		}
		else
		{
			if (dA < 1.0)
			{
				sprintf_s(pcText, 200, "Color(%5.3lf, %5.3lf, %5.3lf, %5.3lf)", dR, dG, dB, dA);
			}
			else
			{
				sprintf_s(pcText, 200, "Color(%5.3lf, %5.3lf, %5.3lf)", dR, dG, dB);
			}
		}

		if (!bReplace)
		{
			//iPos = GetEditor()->word_start(iPos);
			GetEditor()->insert_position((int) nCurPos);
			GetEditor()->insert(pcText);
		}
		else
		{
			pTextBuf->replace(int(nLineStart + nColorStart), int(nLineStart + nColorEnd + 1), pcText);
		}
	}
}

/////////////////////////////////////////////////////////////////////
//// Menu DefVarsE3() Callback

void CCLUScriptEditor::MenuInsertCode()
{
	bool bInsertAsLine = true;
	int iPos, iFinalPosOff = 0;
	Fl_Text_Buffer* pTextBuf;
	string sText;

	const Fl_Menu_Item* pItem = m_pContextMenu->mvalue();
	string sLabel             = pItem->text;

	// Reset Value so that next call to context menu starts at top again.
	m_pContextMenu->value(0);

	// Check for Inserting color
	if (sLabel == CSE_IC_COLOR)
	{
		InsertColor();
	}
	else if (sLabel == CSE_IC_RED)
	{
		sText         = "Red";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_GREEN)
	{
		sText         = "Green";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_BLUE)
	{
		sText         = "Blue";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_MAGENTA)
	{
		sText         = "Magenta";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_CYAN)
	{
		sText         = "Cyan";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_YELLOW)
	{
		sText         = "Yellow";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_ORANGE)
	{
		sText         = "Orange";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_BLACK)
	{
		sText         = "Black";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_WHITE)
	{
		sText         = "White";
		bInsertAsLine = false;
	}
	// Check for defining variables
	else if (sLabel == CSE_IC_E3)
	{
		sText = "DefVarsE3();\n";
	}
	else if (sLabel == CSE_IC_P3)
	{
		sText = "DefVarsP3();\n";
	}
	else if (sLabel == CSE_IC_N3)
	{
		sText = "DefVarsN3();\n";
	}
	else if (sLabel == CSE_IC_C2)
	{
		sText = "DefVarsC2();\n";
	}
	else if (sLabel == CSE_IC_VE3)
	{
		sText         = "VecE3()";
		iFinalPosOff  = 1;
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_VP3)
	{
		sText         = "VecP3()";
		iFinalPosOff  = 1;
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_VN3)
	{
		sText         = "VecN3()";
		iFinalPosOff  = 1;
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_VC2)
	{
		sText         = "VecC2()";
		iFinalPosOff  = 1;
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_EM_NEW)
	{
		sText         = "EM_NEW";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_EM_CHANGE)
	{
		sText         = "EM_CHANGE";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_EM_ANIMATE)
	{
		sText         = "EM_ANIMATE";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_EM_USER)
	{
		sText         = "EM_USER";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_EM_TOOL)
	{
		sText         = "EM_TOOL";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_EM_KEYUP)
	{
		sText         = "EM_KEYUP";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_EM_KEYDOWN)
	{
		sText         = "EM_KEYDOWN";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_EM_RESHAPE)
	{
		sText         = "EM_RESHAPE";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_EM_LINK)
	{
		sText         = "EM_LINK";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_MVOP_GEO)
	{
		sText         = "MVOP_GEO";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_MVOP_INNER)
	{
		sText         = "MVOP_INNER";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_MVOP_OUTER)
	{
		sText         = "MVOP_OUTER";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_MVOP_ADD)
	{
		sText         = "MVOP_ADD";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_MVOP_SUB)
	{
		sText         = "MVOP_SUB";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_MVOP_DUAL)
	{
		sText         = "MVOP_DUAL";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_MVOP_REVERSE)
	{
		sText         = "MVOP_REVERSE";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_MVOP_INVOLUTE)
	{
		sText         = "MVOP_INVOLUTE";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_GA_E3)
	{
		sText         = "GA_E3";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_GA_P3)
	{
		sText         = "GA_P3";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_GA_N3)
	{
		sText         = "GA_N3";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_GA_C2)
	{
		sText         = "GA_C2";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_OPNS)
	{
		sText = ":OPNS;\n";
	}
	else if (sLabel == CSE_IC_IPNS)
	{
		sText = ":IPNS;\n";
	}
	else if (sLabel == CSE_IC_VS_PLOT_POINTS)
	{
		sText = ":PLOT_POINTS;\n";
	}
	else if (sLabel == CSE_IC_VS_PLOT_LINES)
	{
		sText = ":PLOT_LINES;\n";
	}
	else if (sLabel == CSE_IC_VS_PLOT_FILL)
	{
		sText = ":PLOT_FILL;\n";
	}
	else if (sLabel == CSE_IC_VS_POINT_DOT)
	{
		sText = ":DRAW_POINT_AS_DOT;\n";
	}
	else if (sLabel == CSE_IC_VS_POINT_SPHERE)
	{
		sText = ":DRAW_POINT_AS_SPHERE;\n";
	}
	else if (sLabel == CSE_IC_VS_LINE_LINE)
	{
		sText = ":DRAW_LINE_AS_LINE;\n";
	}
	else if (sLabel == CSE_IC_VS_LINE_CYLINDER)
	{
		sText = ":DRAW_LINE_AS_CYLINDER;\n";
	}
	else if (sLabel == CSE_IC_VS_E3_VEC_LINE)
	{
		sText = ":E3_DRAW_VEC_AS_LINE;\n";
	}
	else if (sLabel == CSE_IC_VS_E3_VEC_ARROW)
	{
		sText = ":E3_DRAW_VEC_AS_ARROW;\n";
	}
	else if (sLabel == CSE_IC_VS_E3_VEC_POINT)
	{
		sText = ":E3_DRAW_VEC_AS_POINT;\n";
	}
	else if (sLabel == CSE_IC_VS_E3_PLANE_RECT)
	{
		sText = ":E3_DRAW_PLANE_AS_RECT;\n";
	}
	else if (sLabel == CSE_IC_VS_E3_PLANE_DISK)
	{
		sText = ":E3_DRAW_PLANE_AS_DISK;\n";
	}
	else if (sLabel == CSE_IC_VS_N3_SOLID)
	{
		sText = ":N3_SOLID;\n";
	}
	else if (sLabel == CSE_IC_VS_N3_WIRE)
	{
		sText = ":N3_WIRE;\n";
	}
	else if (sLabel == CSE_IC_VS_N3_IMAG)
	{
		sText = ":N3_SHOW_IMAG;\n";
	}
	else if (sLabel == CSE_IC_VS_N3_NO_IMAG)
	{
		sText = ":N3_HIDE_IMAG;\n";
	}
	else if (sLabel == CSE_IC_BLEND_DEFAULT)
	{
		sText         = "\"Default\"";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_BLEND_ADD)
	{
		sText         = "\"Additive\"";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_OM_POINTS)
	{
		sText         = "OM_POINTS";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_OM_LINES)
	{
		sText         = "OM_LINES";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_OM_LINE_STRIP)
	{
		sText         = "OM_LINE_STRIP";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_OM_LINE_LOOP)
	{
		sText         = "OM_LINE_LOOP";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_OM_TRIANGLES)
	{
		sText         = "OM_TRIANGLES";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_OM_TRIANGLE_STRIP)
	{
		sText         = "OM_TRIANGLE_STRIP";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_OM_TRIANGLE_FAN)
	{
		sText         = "OM_TRIANGLE_FAN";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_OM_QUADS)
	{
		sText         = "OM_QUADS";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_OM_QUAD_STRIP)
	{
		sText         = "OM_QUAD_STRIP";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_OM_POLYGON)
	{
		sText         = "OM_POLYGON";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_OM_LINES_ADJ)
	{
		sText         = "OM_LINES_ADJACENCY";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_OM_LINE_STRIP_ADJ)
	{
		sText         = "OM_LINE_STRIP_ADJACENCY";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_OM_TRIANGLES_ADJ)
	{
		sText         = "OM_TRIANGLES_ADJACENCY";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_OM_TRIANGLE_STRIP_ADJ)
	{
		sText         = "OM_TRIANGLE_STRIP_ADJACENCY";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_IT_LUM)
	{
		sText         = "IMG_LUM";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_IT_LUMA)
	{
		sText         = "IMG_LUMA";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_IT_RGB)
	{
		sText         = "IMG_RGB";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_IT_RGBA)
	{
		sText         = "IMG_RGBA";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_IT_BGR)
	{
		sText         = "IMG_BGR";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_IT_BGRA)
	{
		sText         = "IMG_BGRA";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_DT_BYTE)
	{
		sText         = "IMG_8";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_DT_UBYTE)
	{
		sText         = "IMG_U8";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_DT_SHORT)
	{
		sText         = "IMG_16";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_DT_USHORT)
	{
		sText         = "IMG_U16";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_DT_INT)
	{
		sText         = "IMG_32";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_DT_UINT)
	{
		sText         = "IMG_U32";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_DT_FLOAT)
	{
		sText         = "IMG_FLOAT";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_DT_DOUBLE)
	{
		sText         = "IMG_DOUBLE";
		bInsertAsLine = false;
	}
	else if (sLabel == CSE_IC_LOOP1)
	{
		int iWordStart, iWordEnd;
		string sVar = "i";

		// Get current insert position
		iPos = GetEditor()->insert_position();
		// Get beginning of current word
		iWordStart = GetEditor()->word_start(iPos);
		// Get end of current word
		iWordEnd = GetEditor()->word_end(iPos);

		if ((iWordEnd <= iWordStart) && (GetEditor()->line_start(iPos) < iPos))
		{
			// Get beginning of current word
			iWordStart = GetEditor()->word_start(iPos - 1);
			// Get end of current word
			iWordEnd = GetEditor()->word_end(iPos - 1);
		}

		if (iWordEnd > iWordStart)
		{
			char* pcWord = GetEditor()->buffer()->text_range(iWordStart, iWordEnd);
			sVar = pcWord;
			free(pcWord);
			GetEditor()->buffer()->remove(iWordStart, iWordEnd);
		}

		sText =
			"iCnt = 10;\n" +
			sVar + " = 0;\n"
			       "loop\n"
			       "{\n"
			       "    " + sVar + " = " + sVar + " + 1;\n"
							      "    if (" + sVar + " > iCnt) break;\n\n"
										  "    \n\n"
										  "}\n";

		iFinalPosOff = 4;
	}
	else if (sLabel == CSE_IC_LOOP2)
	{
		int iWordStart, iWordEnd;
		string sVar1 = "i", sVar2 = "j";

		// Get current insert position
		iPos = GetEditor()->insert_position();
		// Get beginning of current word
		iWordStart = GetEditor()->word_start(iPos);
		// Get end of current word
		iWordEnd = GetEditor()->word_end(iPos);

		if ((iWordEnd <= iWordStart) && (GetEditor()->line_start(iPos) < iPos))
		{
			// Get beginning of current word
			iWordStart = GetEditor()->word_start(iPos - 1);
			// Get end of current word
			iWordEnd = GetEditor()->word_end(iPos - 1);
		}

		if (iWordEnd > iWordStart)
		{
			char* pcWord = GetEditor()->buffer()->text_range(iWordStart, iWordEnd);
			sVar1 = pcWord;
			free(pcWord);
			GetEditor()->buffer()->remove(iWordStart, iWordEnd);

			sVar2  = sVar1 + "2";
			sVar1 += "1";
		}

		sText =
			"iCnt1 = 10;\n"
			"iCnt2 = 10;\n"
			+ sVar1 + " = 0;\n"
				  "loop\n"
				  "{\n"
				  "    " + sVar1 + " = " + sVar1 + " + 1;\n"
								   "    if (" + sVar1 + " > iCnt1) break;\n\n"
											"    " + sVar2 + " = 0;\n"
													 "    loop\n"
													 "    {\n"
													 "        " + sVar2 + " = " + sVar2 + " + 1;\n"
																	      "        if (" + sVar2 +
			" > iCnt2) break;\n\n"
			"        \n\n"
			"    }\n\n"
			"}\n";

		iFinalPosOff = 11;
	}
	else
	{
		// Check whether call came from standard menu
		const Fl_Menu_Item* pItem = m_pMenuBar->mvalue();
		string sLabel             = pItem->text;

		// Reset Value so that next call to context menu starts at top again.
		m_pMenuBar->value(0);

		// Check for Inserting color
		if (sLabel == "Insert Co&lor")
		{
			InsertColor();
		}
		else{ return; }
	}

	if (sText.size())
	{
		if (bInsertAsLine)
		{
			// Get current insert position
			iPos = GetEditor()->insert_position();
			// Go to beginning of current line
			iPos = GetEditor()->line_start(iPos);
			// Get Text buffer
			pTextBuf = GetEditor()->buffer();

			GetEditor()->insert_position(iPos);
			GetEditor()->insert(sText.c_str());
		}
		else
		{
			// Get current insert position
			iPos = GetEditor()->insert_position();
			// Go to beginning of current word
			iPos = GetEditor()->word_start(iPos);
			// Get Text buffer
			pTextBuf = GetEditor()->buffer();

			GetEditor()->insert_position(iPos);
			GetEditor()->insert(sText.c_str());
		}

		iPos = GetEditor()->insert_position();
		GetEditor()->insert_position(iPos - iFinalPosOff);
	}

	Fl::focus(GetEditor());
	GetEditor()->show_insert_position();
	GetTextBuffer()->call_modify_callbacks();
}

void CCLUScriptEditor::CB_InsertCode(Fl_Widget* pWidget, void* pvData)
{
	if (pvData)
	{
		((CCLUScriptEditor*) pvData)->MenuInsertCode();
	}
}

//////////////////////////////////////////////////////////////////////
/// Window Menu Callbacks

void CCLUScriptEditor::MenuShowEditorWin()
{
	show();
}

void CCLUScriptEditor::MenuShowOutputWin()
{
	if (m_pOutputWin)
	{
		m_pOutputWin->show();
	}
}

void CCLUScriptEditor::MenuShowVisWin()
{
	if (m_poglWin)
	{
		m_poglWin->ShowWindow();
	}
}

void CCLUScriptEditor::CB_ShowEditorWin(Fl_Widget* pWidget, void* pvData)
{
	if (pvData)
	{
		((CCLUScriptEditor*) pvData)->MenuShowEditorWin();
	}
}

void CCLUScriptEditor::CB_ShowOutputWin(Fl_Widget* pWidget, void* pvData)
{
	if (pvData)
	{
		((CCLUScriptEditor*) pvData)->MenuShowOutputWin();
	}
}

void CCLUScriptEditor::CB_ShowVisWin(Fl_Widget* pWidget, void* pvData)
{
	if (pvData)
	{
		((CCLUScriptEditor*) pvData)->MenuShowVisWin();
	}
}

void CCLUScriptEditor::SetScript(const char* pcScript, bool bResetChangedFlag)
{
	Fl_Text_Buffer* pTextBuf = GetTextBuffer();

	pTextBuf->text(pcScript);
	pTextBuf->call_modify_callbacks();

	if (bResetChangedFlag)
	{
		IsFileChanged() = false;
		SetTitle();
	}

	Parse();
}

//////////////////////////////////////////////////////////////////////
/// Parse Callback

void CCLUScriptEditor::Parse(bool bResetEnv, bool bReparseLatex, const char* pcPath, const char* pcName, int iArgCnt, const char* pcArg)
{
	if (!m_poglWin)
	{
		return;
	}

	char* pcText;
	const char* pcScriptPath, * pcScriptName;
	int iIdx       = GetEditorID(pcPath, pcName);
	bool bIsBinary = IsFileBinary();

	if (iIdx < 0)	// Index not found, or pcPath or pcName not given.
	{
		if (bIsBinary)
		{
			pcText = GetBinaryCode()->Data();
		}
		else
		{
			pcText = GetTextBuffer()->text();
		}

		pcScriptPath = GetPath().c_str();
		pcScriptName = GetName().c_str();
	}
	else
	{
		if (bIsBinary)
		{
			pcText = m_mEditorData[iIdx].m_pmCode->Data();
		}
		else
		{
			pcText = m_mEditorData[iIdx].m_pTextBuf->text();
		}

		pcScriptPath = pcPath;
		pcScriptName = pcName;
	}

	m_poglWin->SetScriptPath(pcScriptPath);
	m_poglWin->SetScriptName(pcScriptName);
	m_poglWin->SetScript(pcText, bResetEnv, bReparseLatex, bIsBinary, iArgCnt, pcArg);

	if (!bIsBinary)
	{
		free(pcText);
	}
}

void CCLUScriptEditor::MenuParseMain(bool bResetEnv, bool bReparseLatex)
{
	Save();
	Parse(bResetEnv, bReparseLatex, m_poglWin->GetScriptPath().c_str(),
			m_poglWin->GetScriptName().c_str());
}

void CCLUScriptEditor::CB_LoadNoExec(Fl_Widget* pWidget, void* pvData)
{
	if (pvData)
	{
		((CCLUScriptEditor*) pvData)->LoadNoExec();
	}
}

/*
void CCLUScriptEditor::CB_SaveBinary(Fl_Widget *pWidget, void *pvData)
{
        if (pvData)
                ((CCLUScriptEditor *) pvData)->SaveBinary();
}
*/

void CCLUScriptEditor::CB_Parse(Fl_Widget* pWidget, void* pvData)
{
	if (pvData)
	{
		((CCLUScriptEditor*) pvData)->Parse();
	}
}

void CCLUScriptEditor::CB_ParseForce(Fl_Widget* pWidget, void* pvData)
{
	if (pvData)
	{
		((CCLUScriptEditor*) pvData)->Parse(false, true);
	}
}

void CCLUScriptEditor::CB_ParseMain(Fl_Widget* pWidget, void* pvData)
{
	if (pvData)
	{
		((CCLUScriptEditor*) pvData)->MenuParseMain();
	}
}

void CCLUScriptEditor::CB_ParseMainForce(Fl_Widget* pWidget, void* pvData)
{
	if (pvData)
	{
		((CCLUScriptEditor*) pvData)->MenuParseMain(false, true);
	}
}

//////////////////////////////////////////////////////////////////////
/// Script List Menu Functions

void CCLUScriptEditor::CB_NextScript(Fl_Widget* pWidget, void* pData)
{
	if (pData)
	{
		((CCLUScriptEditor*) pData)->m_poglWin->MenuNextScript(pWidget);
	}
}

void CCLUScriptEditor::CB_PrevScript(Fl_Widget* pWidget, void* pData)
{
	if (pData)
	{
		((CCLUScriptEditor*) pData)->m_poglWin->MenuPrevScript(pWidget);
	}
}

void CCLUScriptEditor::CB_ReloadScript(Fl_Widget* pWidget, void* pData)
{
	if (pData)
	{
		((CCLUScriptEditor*) pData)->m_poglWin->MenuReloadScript(pWidget);
	}
}

void CCLUScriptEditor::CB_ClearScriptList(Fl_Widget* pWidget, void* pData)
{
	if (pData)
	{
		((CCLUScriptEditor*) pData)->m_poglWin->MenuClearScriptList(pWidget);
	}
}

void CCLUScriptEditor::CB_ShowScriptList(Fl_Widget* pWidget, void* pData)
{
	if (pData)
	{
		((CCLUScriptEditor*) pData)->m_poglWin->MenuShowScriptList(pWidget);
	}
}

void CCLUScriptEditor::CB_ScriptListWindow(Fl_Widget* pWidget, void* pData)
{
	if (pData)
	{
		((CCLUScriptEditor*) pData)->m_poglWin->ScriptListWindowEvent(pWidget);
	}
}

////////////////////////////////////////////////////////////////////////////
// Is called when text is pasted into editor with pasted text in sText.
// If text need not be adapted returns false. If it returns true,
// then the text in sNewText is inserted instead of original text.

bool CCLUScriptEditor::AdaptText(string& sNewText, string& sText)
{
	const char* pcText = sText.c_str();

	// Test for Mathematica input
	if ((pcText[0] == '\\') && (pcText[1] == '!'))
	{
		char cVal;
		int iPos;
		int iLen = (int) sText.length();

		sNewText.reserve(iLen);

		for (iPos = 2; iPos < iLen; iPos++)
		{
			if (pcText[iPos] == '\\')
			{
				++iPos;
				cVal = pcText[iPos];

				if (cVal == ' ')
				{
					sNewText += "*";
				}
				else if (cVal == '/')
				{
					sNewText += "/";
				}
				else if (cVal == '^')
				{
					sNewText += "^^";
				}
			}
			else
			{
				cVal = pcText[iPos];

				if (cVal == '{')
				{
					sNewText += '[';
				}
				else if (cVal == '}')
				{
					sNewText += ']';
				}
				else
				{
					sNewText += cVal;
				}
			}
		}

		return true;
	}

	return false;
}
