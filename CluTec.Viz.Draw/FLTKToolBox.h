////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      FLTKToolBox.h
//
// summary:   Declares the fltk tool box class
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

// FLTKToolBox.h: Schnittstelle f�r die Klasse CFLTKToolBox.
//
// This creates a window to which tools can be added and removed.
// The tools are stacked first left to right and then top to bottom.
//////////////////////////////////////////////////////////////////////

#if !defined(_FLTKTOOLBOX_H__INCLUDED_)
#define _FLTKTOOLBOX_H__INCLUDED_

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000

//#include "CvFltk\glut.H"
#include "CluTec.Viz.Fltk\Fl.H"
#include "CluTec.Viz.Fltk\Fl_Group.H"
#include "CluTec.Viz.Fltk\fl_ask.H"
#include "CluTec.Viz.Fltk\Fl_Window.H"
#include "CluTec.Viz.Fltk\Fl_Double_Window.H"
#include "CluTec.Viz.Fltk\Fl_Single_Window.H"
#include "CluTec.Viz.Fltk\Fl_Menu_Bar.H"
#include "CluTec.Viz.Fltk\Fl_File_Chooser.H"
#include "CluTec.Viz.Fltk\Fl_Tile.H"
#include "CluTec.Viz.Fltk\Fl_Box.H"
#include "CluTec.Viz.Fltk\Fl_Pack.H"
#include "CluTec.Viz.Fltk\Fl_Scroll.H"
#include "CluTec.Viz.Fltk\Fl_Value_Slider.H"
#include "CluTec.Viz.Fltk\Fl_Counter.H"
#include "CluTec.Viz.Fltk\Fl_Round_Button.H"
#include "CluTec.Viz.Fltk\Fl_Light_Button.H"
#include "CluTec.Viz.Fltk\Fl_Repeat_Button.H"
#include "CluTec.Viz.Fltk\Fl_Value_Input.H"
#include "CluTec.Viz.Fltk\Fl_Choice.H"

#include "time.h"
#include "sys/timeb.h"

#include "CLUDrawBase.h"
#include "IOGLWinBase.h"
#include "OGLImage.h"

#include <map>
#include <list>
#include <string>
#include <vector>

using namespace std;

#ifdef WIN32
	// Macro that is used by Windows and only confuses us here.
	#ifdef CreateWindow
		#undef CreateWindow
	#endif
	typedef struct _timeb TTimeStruct;
#	ifndef GETTIME
#		define GETTIME(S) _ftime(S)
#	endif
#else
	typedef struct timeb TTimeStruct;
	#define GETTIME(S) ftime(S)
#endif


class CLUDRAW_API CFLTKToolBox
{
public:

	enum EToolType 
	{ 
		TOOL_NONE = 0,
		TOOL_SLIDER,
		TOOL_CHECKBOX,
		TOOL_INPUT,
		TOOL_BUTTON,
		TOOL_CHOICE,
		TOOL_TEXT,
		TOOL_COUNTER,
		TOOL_IMGBUT,
		TOOL_LABEL
	};

	struct STool
	{
		STool()
		{
			eType = TOOL_NONE;
			pWidget = 0;
			pBox = 0;
			pImgAct = 0;
			pImgDeact = 0;
			poglImgAct = 0;
			poglImgDeact = 0;
			sName = "";
			sDesc = "";
			bDoCreateView = false;
			bDoRedrawView = false;
			bInUse = false;
			bFixed = false;
			bActive = true;
			bInitActive = true;
			iInitVal = 0;
			dInitVal = 0.0;
			bInitVal = false;
			sInitVal = "";
			dInitMin = dInitMax = 0.0;
			dInitStep = dInitLStep = 0.0;
		}

		EToolType eType;
		Fl_Widget *pWidget;
		Fl_Group *pBox;
		Fl_Image *pImgAct;
		Fl_Image *pImgDeact;

		COGLImage *poglImgAct;
		COGLImage *poglImgDeact;

		string sName;
		string sDesc;
		string sFormat;
		string sUnit;

		vector<string> vecStr;

		bool bDoCreateView; // Flag to create tool window for this tool at next update
		bool bDoRedrawView; // Flag to redraw tool
		bool bInUse;
		bool bFixed;
		bool bActive;
		bool bInitActive;

		int iInitVal;
		double dInitVal;
		bool bInitVal;
		string sInitVal;

		// Values for Slider
		double dInitMin, dInitMax, dInitStep, dInitLStep;
	};

	class CInput : public Fl_Input
	{
	public:
		CInput(int iX, int iY, int iW, int iH, const char *pcText) 
			: Fl_Input(iX, iY, iW, iH, pcText)
		{ }

		int handle(int iEvent)
		{
			Fl_Widget *pParent = (Fl_Widget *) this->parent();
			Fl_Widget *pParent2 = 0, *pParent3 = 0;
			if (pParent)
				pParent2 = (Fl_Widget *) pParent->parent();

			if (pParent2)
				pParent3 = (Fl_Widget *) pParent2->parent();

			switch(iEvent)
			{
			case FL_KEYBOARD:
				if (this != Fl::belowmouse() &&
					pParent != Fl::belowmouse() &&
					pParent2 != Fl::belowmouse() &&
					pParent3 != Fl::belowmouse())
					return 0;
				break;
			}

			return Fl_Input::handle(iEvent);
		}
	};

	typedef map<string, STool> TToolMap;
	typedef list<Fl_Pack*> TPackList;

public:
	CFLTKToolBox();
	virtual ~CFLTKToolBox();

	bool ToolsChanged() { return m_bToolsChanged; }

	// Create the Tool Box
	// pCallback is pointer to callback function which is called when 
	// any one of the tools has changed. 
	// pData is data passed on to callback
	virtual bool Create(int iPosX, int iPosY, int iSizeX, int iSizeY, 
						Fl_Callback *pCallback, void *pData,
						Fl_Boxtype eBoxType, bool bEnableScroll,
						int iToolHeight = 0 /* uses default value from constructor */,
						int iToolBorderW = 1, int iToolBorderH = 1);

	/// Detach this window from the GLUT loop.
	virtual bool Destroy();


	// Resets falgs of tools which indicates that they are in use
	void ResetToolInUseFlags();
	// Prune, ie. delete all those tools that are not in use
	void PruneToolMap(bool bKeepFixedTools = true, bool bRedraw = true);
	// Remove tool with given name
	bool RemoveTool(const char *pcName, bool bRedraw = true);

	// Update Tool Box Packs
	// This first resets the tool box and then walks through map
	// m_mapTool to redistribute all tools
	bool Update();

	// Get Upper Left corner of scroll box enclosing tools
	bool GetPosition(int &iX, int &iY);
	
	// Get Size of scroll box enclosing tools
	bool GetSize(int &iWidth, int &iHeight);
	
	bool GetTool(const char *pcName, STool &rTool);
	bool GetToolValue(const char *pcName, double &rVal);
	bool GetToolValue(const char *pcName, string &sVal);
	int GetToolCount() { return (int) m_mapTool.size(); }

	// Check all tools whether they need to be redrawn
	bool CheckToolsRedraw();

	// Add a slider tool
	bool AddToolSlider(bool &bNew, bool &bRedraw,
					    const char *pcName, 
						double dMin, double dMax, double dStep, double dVal,
						bool bFixed = false, const char *pcDesc = 0, bool bDoUpdate = true);

	// Create the given slider tool view
	bool CreateToolViewSlider(STool &rTool);
	// Redraw the given slider tool view
	bool RedrawToolViewSlider(STool &rTool);

	// Add a counter tool
	bool AddToolCounter(bool &bNew, bool &bRedraw, const char *pcName, 
						double dMin, double dMax, double dStep, double dLStep, double dVal,
						bool bFixed = false, const char *pcDesc = 0, bool bDoUpdate = true);
	// Create the given slider tool view
	bool CreateToolViewCounter(STool &rTool);
	// Redraw the given slider tool view
	bool RedrawToolViewCounter(STool &rTool);


	// Add a CheckBox tool
	bool AddToolCheckBox(bool &bNew, bool &bRedraw, const char *pcName, bool bSet, bool bFixed = false, 
						const char *pcDesc = 0, bool bDoUpdate = true);
	// Create the given slider tool view
	bool CreateToolViewCheckBox(STool &rTool);
	// Redraw the given slider tool view
	bool RedrawToolViewCheckBox(STool &rTool);

	// Add a Button tool
	bool AddToolButton(bool &bNew, bool &bRedraw, 
						const char *pcName, bool bFixed = false, 
						const char *pcDesc = 0, bool bDoUpdate = true);
	// Create the given slider tool view
	bool CreateToolViewButton(STool &rTool);
	// Redraw the given slider tool view
	bool RedrawToolViewButton(STool &rTool);

	// Add an Input tool
	bool AddToolInput(bool &bNew, bool &bRedraw, 
						const char *pcName, 
						double dMin, double dMax, double dVal,
						bool bFixed = false, const char *pcDesc = 0, bool bDoUpdate = true);
	// Create the given input tool view
	bool CreateToolViewInput(STool &rTool);
	// Redraw the given input tool view
	bool RedrawToolViewInput(STool &rTool);


	// Add a Text Input tool
	bool AddToolInputText(bool &bNew, bool &bRedraw, 
						  const char *pcName, 
						  bool bImmediateCallback, 
						  const char* pcInitText,
						  bool bFixed = false, const char *pcDesc = 0, bool bDoUpdate = true);
	// Create the given input tool view
	bool CreateToolViewInputText(STool &rTool);
	// Redraw the given input tool view
	bool RedrawToolViewInputText(STool &rTool);


	// Add a ComboBox tool
	bool AddToolChoice(bool &bNew, bool &bRedraw, const char *pcName, 
						vector<string>& vecData, const char* pcCoice,
						bool bFixed = false, const char *pcDesc = 0, 
						bool bDoUpdate = true);
	// Create the given input tool view
	bool CreateToolViewChoice(STool &rTool);
	// Redraw the given input tool view
	bool RedrawToolViewChoice(STool &rTool);


	// Add an Image Button tool
	bool AddToolImageButton(const char *pcName, COGLImage *pImgAct, COGLImage *pImgDeact = 0, 
						bool bFixed = false, const char *pcDesc = 0, bool bDoUpdate = true);


	///////////////////////////////////////////////////////////////////////////////////////////////
	/// Toolbar objects

	// Add an Image Button tool
	bool AddToolbarImageButton(bool &bNew, bool &bRedraw, 
								const char *pcName, 
								COGLImage *pImgAct, 
								COGLImage *pImgDeact = 0, 
						bool bFixed = false, const char *pcDesc = 0, 
						bool bDoUpdate = true);
	// Create the given input tool view
	bool CreateToolbarViewImageButton(STool &rTool);
	// Redraw the given input tool view
	bool RedrawToolbarViewImageButton(STool &rTool);


	// Add a counter tool
	bool AddToolbarCounter(bool &bNew, bool &bRedraw,
						const char *pcName, const char *pcUnit,
						double dMin, double dMax, double dStep,
						double dLStep, double dVal,
						bool bActive = true,
						bool bFixed = false, const char *pcDesc = 0, 
						bool bDoUpdate = true);
	// Create the given input tool view
	bool CreateToolbarViewCounter(STool &rTool);
	// Redraw the given input tool view
	bool RedrawToolbarViewCounter(STool &rTool);


	// Add a label to the toolbar
	bool AddToolbarLabel(bool &bNew, bool &bRedraw,
						const char *pcName, bool bFixed = false, 
						const char *pcDesc = 0,
						bool bDoUpdate = true);
	// Create the given input tool view
	bool CreateToolbarViewLabel(STool &rTool);
	// Redraw the given input tool view
	bool RedrawToolbarViewLabel(STool &rTool);


protected:
	// Reset Tool Box
	// This removes all tools from the packs.
	bool ResetPacks();

protected:
	// Vertical Fl_Pack. This stacks horizontal Fl_Packs
	Fl_Pack *m_pVPackBox;

	// The Scroll box surrounding the m_pVPackBox
	Fl_Scroll *m_pScrollBox;

	// A Standard Window if no scrolling is needed
	Fl_Window *m_pBox;

	// Vector of horizontal packs
	list<Fl_Pack*> m_listHPackBox;

	// Map of tool name to tool data
	map<string, STool> m_mapTool;

	// List of tools in correct order
	list<string> m_listToolName;

	// The height and width of a tool
	int m_iToolHeight, m_iToolWidth, m_iToolWBorder, m_iToolHBorder;
	int m_iToolbarWBorder;
	int m_iTotalHeight;

	// The tool callback function
	Fl_Callback *m_pToolCallback;

	// The callback data
	void *m_pToolCallbackData;

	// Store current width
	int m_iCurWidth;

	// Flag to indicate whether any tools were added or removed
	bool m_bToolsChanged;
};


#endif // !defined(AFX_OGLWINGLUT_H__6EB9C282_EE5B_11D5_BA34_00E07D8AFD6A__INCLUDED_)
