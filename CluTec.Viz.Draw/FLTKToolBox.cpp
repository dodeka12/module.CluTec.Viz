////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      FLTKToolBox.cpp
//
// summary:   Implements the fltk tool box class
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

// OGLWinFLTK.cpp: Implementierung der Klasse CFLTKToolBox.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#define CLUDRAW_FLTK
#ifdef CLUDRAW_FLTK
    #include "FLTKToolBox.h"
    #include "CluTec.Viz.Fltk\fl_draw.H"

    #include <algorithm>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

	CFLTKToolBox::CFLTKToolBox()
	{
		m_pVPackBox         = 0;
		m_pScrollBox        = 0;
		m_pBox              = 0;
		m_pToolCallback     = 0;
		m_pToolCallbackData = 0;
		m_iCurWidth         = 0;
		m_bToolsChanged     = false;

		m_iToolHeight     = 40;
		m_iToolWidth      = 190;
		m_iToolWBorder    = 3;
		m_iToolHBorder    = 1;
		m_iToolbarWBorder = 1;
	}

	CFLTKToolBox::~CFLTKToolBox()
	{
		//Destroy();
	}

//////////////////////////////////////////////////////////////////////
// Create the Tool Box

	bool CFLTKToolBox::Create(int iPosX,
		int iPosY,
		int iSizeX,
		int iSizeY,
		Fl_Callback* pCallback,
		void* pData,
		Fl_Boxtype eBoxType,
		bool bEnableScroll,
		int iToolHeight,
		int iToolBorderW,
		int iToolBorderH)
	{
		if (iToolHeight > 0)
		{
			m_iToolHeight = iToolHeight;
		}

		m_iToolWBorder = iToolBorderW;
		m_iToolHBorder = iToolBorderH;
		m_iTotalHeight = m_iToolHeight + 2 * m_iToolHBorder;

		m_pToolCallback     = pCallback;
		m_pToolCallbackData = pData;

		if (bEnableScroll)
		{
			// Create the outer scroll box
			m_pScrollBox = new Fl_Scroll(iPosX, iPosY, iSizeX, iSizeY);
			m_pScrollBox->box(eBoxType);
			m_pScrollBox->align(FL_ALIGN_CLIP);
			m_pScrollBox->type(Fl_Scroll::VERTICAL);
			m_pScrollBox->begin();

			// Create the vertical Pack Box
			// The tools are placed in horizontal packs which themselves
			// are stacked in this vertical stack box
			m_pVPackBox = new Fl_Pack(5, iPosY + 2, iSizeX, m_iToolHeight);
//		m_pVPackBox = new Fl_Pack(5, 2, iSizeX, iToolHeight);
			m_pVPackBox->type(FL_VERTICAL);
			m_pVPackBox->spacing(2);
			m_pVPackBox->begin();

			m_pVPackBox->end();
			m_pScrollBox->end();
		}
		else
		{
			// Create the outer box
			m_pBox = new Fl_Window(iPosX, iPosY, iSizeX, iSizeY);
			m_pBox->box(eBoxType);
			m_pBox->align(FL_ALIGN_CLIP);
			m_pBox->color(FL_LIGHT2);
			m_pBox->begin();

			// Create the vertical Pack Box
			// The tools are placed in horizontal packs which themselves
			// are stacked in this vertical stack box
			m_pVPackBox = new Fl_Pack(1, 1, iSizeX - 2, iToolHeight);
//		m_pVPackBox = new Fl_Pack(5, 2, iSizeX, iToolHeight);
			m_pVPackBox->type(FL_VERTICAL);
			m_pVPackBox->spacing(1);
			m_pVPackBox->color(FL_LIGHT2);
			m_pVPackBox->begin();

			m_pVPackBox->end();
			m_pBox->end();
		}

		return true;
	}

//////////////////////////////////////////////////////////////////////
/// Detach window

	bool CFLTKToolBox::Destroy()
	{
		if (m_pScrollBox)
		{
			delete m_pScrollBox;
		}

		if (m_pBox)
		{
			delete m_pBox;
		}

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Get Upper Left corner of scroll box enclosing tools

	bool CFLTKToolBox::GetPosition(int& iX, int& iY)
	{
		if (m_pScrollBox)
		{
			iX = m_pScrollBox->x();
			iY = m_pScrollBox->y();
		}
		else if (m_pBox)
		{
			iX = m_pBox->x();
			iY = m_pBox->y();
		}
		else
		{
			iX = iY = 0;
			return false;
		}

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Get Size of scroll box enclosing tools

	bool CFLTKToolBox::GetSize(int& iWidth, int& iHeight)
	{
		if (m_pScrollBox)
		{
			iWidth  = m_pScrollBox->w();
			iHeight = m_pScrollBox->h();
		}
		else if (m_pBox)
		{
			iWidth  = m_pBox->w();
			iHeight = m_pBox->h();
		}
		else
		{
			iWidth = iHeight = 0;
			return false;
		}

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Check all tools whether they need to be redrawn

	bool CFLTKToolBox::CheckToolsRedraw()
	{
		list<string>::iterator it_El, it_End;

		// Iterate over all tools
		it_End = m_listToolName.end();

		for (it_El = m_listToolName.begin();
		     it_El != it_End;
		     ++it_El)
		{
			STool& rTool = m_mapTool[*it_El];

			if (rTool.bDoRedrawView)
			{
				rTool.bDoRedrawView = false;

				if (rTool.eType == TOOL_SLIDER)
				{
					RedrawToolViewSlider(rTool);
				}
				else if (rTool.eType == TOOL_COUNTER)
				{
					RedrawToolbarViewCounter(rTool);
				}
				else if (rTool.eType == TOOL_INPUT)
				{
					RedrawToolViewInput(rTool);
				}
				else if (rTool.eType == TOOL_TEXT)
				{
					RedrawToolViewInputText(rTool);
				}
				else if (rTool.eType == TOOL_CHECKBOX)
				{
					RedrawToolViewCheckBox(rTool);
				}
				else if (rTool.eType == TOOL_BUTTON)
				{
					RedrawToolViewButton(rTool);
				}
				else if (rTool.eType == TOOL_CHOICE)
				{
					RedrawToolViewChoice(rTool);
				}
				else if (rTool.eType == TOOL_IMGBUT)
				{
					RedrawToolbarViewImageButton(rTool);
				}
				else if (rTool.eType == TOOL_LABEL)
				{
					RedrawToolbarViewLabel(rTool);
				}
				else
				{
					continue;
				}
			}
		}

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Update Tool Box Packs
// This first resets the tool box and then walks through map
// m_mapTool to redistribute all tools

	bool CFLTKToolBox::Update()
	{
		// Get current width of scroll box.
		int iSizeX;

		if (m_pScrollBox)
		{
			iSizeX = m_pScrollBox->w();
		}
		else
		{
			iSizeX = m_pBox->w();
		}

		// If neither the tools changed nor the size of the window,
		// then do not do any updating.
		if (!m_bToolsChanged && (m_iCurWidth == iSizeX))
		{
			return true;
		}

		// Reset flags
		m_bToolsChanged = false;

		// Reset all packs
		ResetPacks();

		// Now build packs up again from map of tools

		int iTotalToolWidth = 0;
		int iToolWidth;
		int iHPackCount = 0;
		list<string>::iterator it_El, it_End;

		// Check whether there are tool at all.
		if (m_mapTool.size() == 0)
		{
			// Set Height of vertical pack to appropriate value
			m_pVPackBox->size(iSizeX, m_iToolHeight);
			m_pVPackBox->redraw();

			if (m_pScrollBox)
			{
				m_pScrollBox->redraw();
				m_iCurWidth = m_pScrollBox->w();
			}
			else
			{
				m_pBox->redraw();
				m_iCurWidth = m_pBox->w();
			}

			return true;
		}

		// Create first horizontal pack
		Fl_Pack* pHPack;

		m_pVPackBox->init_sizes();
		m_pVPackBox->begin();
		pHPack = new Fl_Pack(0, 0, iSizeX, m_iTotalHeight);
		pHPack->type(FL_HORIZONTAL);
		pHPack->color(FL_LIGHT2);
		pHPack->spacing(2);
		pHPack->begin();
		pHPack->end();
		pHPack->init_sizes();
		m_pVPackBox->end();
		m_listHPackBox.push_back(pHPack);
		// Now there is one horizontal pack
		iHPackCount = 1;

		// Iterate over all tools
		it_End = m_listToolName.end();

		for (it_El = m_listToolName.begin();
		     it_El != it_End;
		     ++it_El)
		{
			STool& rTool = m_mapTool[*it_El];

			// Check whether tool view needs to be created
			if (rTool.bDoCreateView)
			{
				rTool.bDoCreateView = false;
				if (rTool.eType == TOOL_SLIDER)
				{
					if (!CreateToolViewSlider(rTool)) { continue; }
				}
				else if (rTool.eType == TOOL_COUNTER)
				{
					if (!CreateToolbarViewCounter(rTool)) { continue; }
				}
				else if (rTool.eType == TOOL_INPUT)
				{
					if (!CreateToolViewInput(rTool)) { continue; }
				}
				else if (rTool.eType == TOOL_TEXT)
				{
					if (!CreateToolViewInputText(rTool)) { continue; }
				}
				else if (rTool.eType == TOOL_CHECKBOX)
				{
					if (!CreateToolViewCheckBox(rTool)) { continue; }
				}
				else if (rTool.eType == TOOL_BUTTON)
				{
					if (!CreateToolViewButton(rTool)) { continue; }
				}
				else if (rTool.eType == TOOL_CHOICE)
				{
					if (!CreateToolViewChoice(rTool)) { continue; }
				}
				else if (rTool.eType == TOOL_IMGBUT)
				{
					if (!CreateToolbarViewImageButton(rTool)) { continue; }
				}
				else if (rTool.eType == TOOL_LABEL)
				{
					if (!CreateToolbarViewLabel(rTool)) { continue; }
				}
				else
				{
					continue;
				}
			}

			// Get Width of tool
			iToolWidth = rTool.pBox->w();

			// If at least one tool already exists in current horizontal pack,
			// and adding a new tool to this horizontal pack would extend over
			// the width of the scroll window, then add a new horizontal pack.
			if ((iTotalToolWidth > 0) && (iTotalToolWidth + iToolWidth + 30 > iSizeX))
			{
				pHPack->redraw();

				// Add new horizontal pack
				m_pVPackBox->init_sizes();
				m_pVPackBox->begin();
				pHPack = new Fl_Pack(0, 0, iSizeX, m_iTotalHeight);
				pHPack->type(FL_HORIZONTAL);
				pHPack->color(FL_LIGHT2);
				pHPack->spacing(2);
				pHPack->begin();
				pHPack->end();
				pHPack->init_sizes();
				m_pVPackBox->end();
				m_listHPackBox.push_back(pHPack);
				// Now there is one horizontal pack more
				++iHPackCount;
				// Reset total tool width
				iTotalToolWidth = 0;
			}

			// Add tool to horizontal pack
			pHPack->add(*rTool.pBox);

			// Increase total tool width by width of this tool
			iTotalToolWidth += iToolWidth;
		}

		pHPack->redraw();

		// Set Height of vertical pack to appropriate value
		m_pVPackBox->size(iSizeX, iHPackCount * m_iTotalHeight);
		m_pVPackBox->redraw();

		if (m_pScrollBox)
		{
			m_pScrollBox->redraw();
			m_iCurWidth = m_pScrollBox->w();
		}
		else
		{
			m_pBox->redraw();
			m_iCurWidth = m_pBox->w();
		}

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Reset Packs
// Remove tools from packs and remove horizonzal packs from vertical pack

	bool CFLTKToolBox::ResetPacks()
	{
		TPackList::iterator it_El, it_End;

		it_End = m_listHPackBox.end();
		for (it_El = m_listHPackBox.begin();
		     it_El != it_End;
		     ++it_El)
		{
			Fl_Pack* pPack = *it_El;
			if (!pPack)
			{
				continue;
			}

			// Remove tools from horizontal pack
			int iTool, iToolCount = pPack->children();
			for (iTool = 0; iTool < iToolCount; iTool++)
			{
				// Always delete child at pos 0, since
				// the list is reduced after a remove operation.
				pPack->remove(*pPack->child(0));
			}

			// Remove horizontal pack from vertical pack
			m_pVPackBox->remove(*pPack);

			// Delete horizontal pack
			delete pPack;
		}

		// Empty vector of horizontal packs
		m_listHPackBox.erase(m_listHPackBox.begin(), it_End);

		return true;
	}

//////////////////////////////////////////////////////////////////////
/// Reset Tool bInUse Flags

	void CFLTKToolBox::ResetToolInUseFlags()
	{
		TToolMap::iterator it_El, it_End;

		it_End = m_mapTool.end();

		for (it_El = m_mapTool.begin();
		     it_El != it_End;
		     ++it_El)
		{
			it_El->second.bInUse = false;
		}
	}

//////////////////////////////////////////////////////////////////////
/// Reset Tool bInUse Flags

	void CFLTKToolBox::PruneToolMap(bool bKeepFixedTools, bool bRedraw)
	{
		TToolMap::iterator it_El, it_End;

		it_End = m_mapTool.end();

		it_El = m_mapTool.begin();
		while (it_El != it_End)
		{
			if ((it_El->second.bInUse == false) &&
			    ((it_El->second.bFixed == false) ||
			     ((it_El->second.bFixed == true) && (bKeepFixedTools == false))))
			{
				RemoveTool(it_El->second.sName.c_str(), false);
				it_El           = m_mapTool.begin();
				m_bToolsChanged = true;
			}
			else
			{
				++it_El;
			}
		}

		// Update packs
		if (bRedraw)
		{
			Update();
		}
	}

//////////////////////////////////////////////////////////////////////
/// Remove Tool

	bool CFLTKToolBox::RemoveTool(const char* pcName, bool bRedraw)
	{
		list<string>::iterator it_ID;
		TToolMap::iterator it_El;
		TPackList::iterator it_ListEl, it_End;

		if ((it_El = m_mapTool.find(pcName)) == m_mapTool.end())
		{
			return false;
		}

		STool& rTool = it_El->second;

		// Look through horizontal packs and remove tool
		it_End = m_listHPackBox.end();
		for (it_ListEl = m_listHPackBox.begin();
		     it_ListEl != it_End;
		     ++it_ListEl)
		{
			(*it_ListEl)->remove(rTool.pBox);
		}

		delete rTool.pBox;

		if (rTool.pImgAct)
		{
			if (rTool.pImgAct == rTool.pImgDeact)
			{
				delete rTool.pImgAct;

				if (rTool.poglImgAct)
				{
					delete rTool.poglImgAct;
				}
			}
			else
			{
				delete rTool.pImgAct;

				if (rTool.pImgDeact)
				{
					delete rTool.pImgDeact;
				}

				if (rTool.poglImgAct == rTool.poglImgDeact)
				{
					if (rTool.poglImgAct)
					{
						delete rTool.poglImgAct;
					}
				}
				else
				{
					if (rTool.poglImgAct)
					{
						delete rTool.poglImgAct;
					}
					if (rTool.poglImgDeact)
					{
						delete rTool.poglImgDeact;
					}
				}
			}
		}

		it_ID = std::find(m_listToolName.begin(), m_listToolName.end(), string(pcName));
		m_listToolName.erase(it_ID);

		m_mapTool.erase(it_El);

		m_bToolsChanged = true;

		// Update the pack
		if (bRedraw)
		{
			Update();
		}

		return true;
	}

//////////////////////////////////////////////////////////////////////
/// Get Tool

	bool CFLTKToolBox::GetTool(const char* pcName, STool& rTool)
	{
		TToolMap::iterator it_El;

		if ((it_El = m_mapTool.find(pcName)) == m_mapTool.end())
		{
			return false;
		}

		rTool = it_El->second;

		return true;
	}

//////////////////////////////////////////////////////////////////////
/// Get the double value of a tool
/// returns false if the tool has no double value

	bool CFLTKToolBox::GetToolValue(const char* pcName, double& rVal)
	{
		TToolMap::iterator it_El;

		if ((it_El = m_mapTool.find(pcName)) == m_mapTool.end())
		{
			return false;
		}

		STool& rTool = it_El->second;

		if (rTool.eType == TOOL_SLIDER)
		{
			if (rTool.pWidget && !rTool.bDoRedrawView)
			{
				rVal = ((Fl_Value_Slider*) rTool.pWidget)->value();
			}
			else
			{
				rVal = rTool.dInitVal;
			}

			rTool.bInUse = true;
		}
		else if (rTool.eType == TOOL_COUNTER)
		{
			if (rTool.pWidget && !rTool.bDoRedrawView)
			{
				rVal = ((Fl_Counter*) rTool.pWidget)->value();
			}
			else
			{
				rVal = rTool.dInitVal;
			}

			rTool.bInUse = true;
		}
		else if (rTool.eType == TOOL_INPUT)
		{
			if (rTool.pWidget && !rTool.bDoRedrawView)
			{
				rVal = ((Fl_Value_Input*) rTool.pWidget)->value();
			}
			else
			{
				rVal = rTool.dInitVal;
			}

			rTool.bInUse = true;
		}
		else if (rTool.eType == TOOL_CHECKBOX)
		{
			if (rTool.pWidget && !rTool.bDoRedrawView)
			{
				rVal = ((Fl_Button*) rTool.pWidget)->value();
			}
			else
			{
				rVal = (rTool.bInitVal ? 1.0 : 0.0);
			}

			rTool.bInUse = true;
		}
		else if (rTool.eType == TOOL_BUTTON)
		{
			rVal         = 0;
			rTool.bInUse = true;
		}
		else if (rTool.eType == TOOL_CHOICE)
		{
			if (rTool.pWidget && !rTool.bDoRedrawView)
			{
				rVal = ((Fl_Choice*) rTool.pWidget)->value() + 1;
			}
			else
			{
				rVal = rTool.iInitVal + 1;
			}

			rTool.bInUse = true;
		}
		else if (rTool.eType == TOOL_IMGBUT)
		{
			rVal         = 0;
			rTool.bInUse = true;
		}
		else
		{
			return false;
		}

		return true;
	}

//////////////////////////////////////////////////////////////////////
/// Get the string value of a tool
/// returns false if the tool has no string value

	bool CFLTKToolBox::GetToolValue(const char* pcName, string& rVal)
	{
		TToolMap::iterator it_El;

		if ((it_El = m_mapTool.find(pcName)) == m_mapTool.end())
		{
			return false;
		}

		STool& rTool = it_El->second;

		if (rTool.eType == TOOL_TEXT)
		{
			if (rTool.pWidget && !rTool.bDoRedrawView)
			{
				rVal = ((Fl_Input*) rTool.pWidget)->value();
			}
			else
			{
				rVal = rTool.sInitVal.c_str();
			}

			rTool.bInUse = true;
		}
		else
		{
			return false;
		}

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Add a tool to the tool window

	bool CFLTKToolBox::AddToolSlider(bool& bNew, bool& bRedraw, const char* pcName,	double dMin, double dMax, double dStep, double dVal, bool bFixed, const char* pcDesc, bool bDoUpdate)
	{
		bNew    = false;
		bRedraw = false;

		if (!m_pVPackBox)
		{
			return false;
		}

		TToolMap::iterator it_El;

		// Check whether slider already exists
		if ((it_El = m_mapTool.find(pcName)) != m_mapTool.end())
		{
			// If already a different tool with the same name exists
			// then return with false.
			if (it_El->second.eType != TOOL_SLIDER)
			{
				return false;
			}

			// if tool exists but has not yet been created, then simply return
			if (it_El->second.bDoCreateView)
			{
				it_El->second.bInUse = true;
				return true;
			}

			// don't create anew if yes
			// Get reference to tool data
			STool& rTool = it_El->second;

			if ((dVal != rTool.dInitVal) || (dMin != rTool.dInitMin) ||
			    (dMax != rTool.dInitMax) || (dStep != rTool.dInitStep))
			{
				rTool.dInitVal  = dVal;
				rTool.dInitMin  = dMin;
				rTool.dInitMax  = dMax;
				rTool.dInitStep = dStep;

				rTool.bDoRedrawView = true;
				bRedraw             = true;
			}

			it_El->second.bInUse = true;
			return true;
		}
		else
		{
			m_listToolName.push_back(pcName);
		}

		STool& rTool = m_mapTool[pcName];

		bNew = true;
		// Flag to create tool view at next update
		rTool.bDoCreateView = true;
		rTool.bDoRedrawView = false;
		rTool.sName         = pcName;

		rTool.eType = TOOL_SLIDER;
		//rTool.pWidget = (Fl_Widget *) pSlider;
		//rTool.pBox = pBox;
		rTool.bInUse    = true;
		rTool.bFixed    = bFixed;
		rTool.dInitVal  = dVal;
		rTool.dInitMin  = dMin;
		rTool.dInitMax  = dMax;
		rTool.dInitStep = dStep;
		rTool.pImgAct   = 0;
		rTool.pImgDeact = 0;

		if (pcDesc)
		{
			rTool.sDesc = pcDesc;
		}

		m_bToolsChanged = true;

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Create the given slider tool view

	bool CFLTKToolBox::CreateToolViewSlider(STool& rTool)
	{
		if (!m_pVPackBox)
		{
			return false;
		}

		Fl_Group* pBox;
		Fl_Value_Slider* pSlider;

		if (!(pBox = new Fl_Group(0, 0, m_iToolWidth, m_iTotalHeight)))
		{
			return false;
		}
		pBox->box(FL_DOWN_BOX);
		pBox->begin();

		if (!(pSlider = new Fl_Value_Slider(2 * m_iToolWBorder, 16,
				      m_iToolWidth - 6 * m_iToolWBorder, 20, rTool.sName.c_str())))
		{
			return false;
		}

		rTool.pWidget = (Fl_Widget*) pSlider;
		rTool.pBox    = pBox;

		pSlider->type(FL_HOR_NICE_SLIDER);
		pSlider->box(FL_FLAT_BOX);
		pSlider->slider_size(0.1f);
		pSlider->labelsize(12);
		pSlider->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
		pSlider->color(0x9F9F9F00);
		pSlider->selection_color(1);
		pSlider->range(rTool.dInitMin, rTool.dInitMax);
		pSlider->step(rTool.dInitStep);
		pSlider->value(pSlider->round(rTool.dInitVal));
		pSlider->redraw();

		if (rTool.sDesc.length())
		{
			pSlider->tooltip(rTool.sDesc.c_str());
		}

		pSlider->callback(m_pToolCallback, m_pToolCallbackData);

		pBox->end();

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Create the given slider tool view

	bool CFLTKToolBox::RedrawToolViewSlider(STool& rTool)
	{
		if (!m_pVPackBox)
		{
			return false;
		}

		if (rTool.eType != TOOL_SLIDER)
		{
			return false;
		}

		Fl_Value_Slider* pSlider = (Fl_Value_Slider*) rTool.pWidget;

		pSlider->range(rTool.dInitMin, rTool.dInitMax);
		pSlider->step(rTool.dInitStep);
		pSlider->value(pSlider->round(rTool.dInitVal));
		pSlider->redraw();

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Add a tool to the tool window

	bool CFLTKToolBox::AddToolCounter(bool& bNew,
		bool& bRedraw,
		const char* pcName,
		double dMin,
		double dMax,
		double dStep,
		double dLStep,
		double dVal,
		bool bFixed,
		const char* pcDesc,
		bool bDoUpdate)
	{
		bNew    = false;
		bRedraw = false;

		if (!m_pVPackBox)
		{
			return false;
		}

		TToolMap::iterator it_El;

		// Check whether slider already exists
		if ((it_El = m_mapTool.find(pcName)) != m_mapTool.end())
		{
			// If already a different tool with the same name exists
			// then return with false.
			if (it_El->second.eType != TOOL_COUNTER)
			{
				return false;
			}

			// if tool exists but has not yet been created, then simply return
			if (it_El->second.bDoCreateView)
			{
				it_El->second.bInUse = true;
				return true;
			}

			// don't create anew if yes
			// Get reference to tool data
			STool& rTool = it_El->second;

			if ((dVal != rTool.dInitVal) || (dMin != rTool.dInitMin) ||
			    (dMax != rTool.dInitMax) || (dStep != rTool.dInitStep) ||
			    (dLStep != rTool.dInitLStep))
			{
				rTool.dInitVal   = dVal;
				rTool.dInitMin   = dMin;
				rTool.dInitMax   = dMax;
				rTool.dInitStep  = dStep;
				rTool.dInitLStep = dLStep;

				rTool.bDoRedrawView = true;
				bRedraw             = true;
			}

			it_El->second.bInUse = true;
			return true;
		}
		else
		{
			m_listToolName.push_back(pcName);
		}

		STool& rTool = m_mapTool[pcName];

		bNew = true;
		// Flag to create tool view at next update
		rTool.bDoCreateView = true;
		rTool.bDoRedrawView = false;
		rTool.sName         = pcName;

		rTool.eType = TOOL_COUNTER;
		//rTool.pWidget = (Fl_Widget *) pCounter;
		//rTool.pBox = pBox;
		rTool.bInUse     = true;
		rTool.bFixed     = bFixed;
		rTool.dInitVal   = dVal;
		rTool.dInitMin   = dMin;
		rTool.dInitMax   = dMax;
		rTool.dInitStep  = dStep;
		rTool.dInitLStep = dLStep;
		rTool.pImgAct    = 0;
		rTool.pImgDeact  = 0;

		if (pcDesc)
		{
			rTool.sDesc = pcDesc;
		}

		m_bToolsChanged = true;

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Create tool view counter

	bool CFLTKToolBox::CreateToolViewCounter(STool& rTool)
	{
		if (!m_pVPackBox)
		{
			return false;
		}

		Fl_Group* pBox;
		Fl_Counter* pCounter;

		if (!(pBox = new Fl_Group(0, 0, m_iToolWidth, 30)))
		{
			return false;
		}
		pBox->box(FL_DOWN_BOX);
		pBox->begin();

		if (!(pCounter = new Fl_Counter(3, 12, m_iToolWidth - 6 * m_iToolWBorder,
				      15, rTool.sName.c_str())))
		{
			return false;
		}

		rTool.pWidget = (Fl_Widget*) pCounter;
		rTool.pBox    = pBox;

		pCounter->type(FL_NORMAL_COUNTER);
		pCounter->box(FL_FLAT_BOX);
		pCounter->labelsize(12);
		pCounter->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
		pCounter->color(0x9F9F9F00);
		pCounter->selection_color(1);
		pCounter->range(rTool.dInitMin, rTool.dInitMax);
		pCounter->step(rTool.dInitStep);
		pCounter->lstep(rTool.dInitLStep);
		pCounter->value(pCounter->round(rTool.dInitVal));
		pCounter->redraw();

		if (rTool.sDesc.length())
		{
			pCounter->tooltip(rTool.sDesc.c_str());
		}

		pCounter->callback(m_pToolCallback, m_pToolCallbackData);

		pBox->end();

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Redraw tool view counter

	bool CFLTKToolBox::RedrawToolViewCounter(STool& rTool)
	{
		if (!m_pVPackBox)
		{
			return false;
		}

		if (rTool.eType != TOOL_COUNTER)
		{
			return false;
		}

		Fl_Counter* pCounter = (Fl_Counter*) rTool.pWidget;

		pCounter->range(rTool.dInitMin, rTool.dInitMax);
		pCounter->step(rTool.dInitStep);
		pCounter->lstep(rTool.dInitLStep);
		pCounter->value(pCounter->round(rTool.dInitVal));
		pCounter->redraw();

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Add a tool to the tool window

	bool CFLTKToolBox::AddToolInput(bool& bNew, bool& bRedraw, const char* pcName, double dMin, double dMax, double dVal, bool bFixed, const char* pcDesc, bool bDoUpdate)
	{
		bNew    = false;
		bRedraw = false;

		if (!m_pVPackBox)
		{
			return false;
		}

		TToolMap::iterator it_El;

		// Check whether slider already exists
		if ((it_El = m_mapTool.find(pcName)) != m_mapTool.end())
		{
			// If already a different tool with the same name exists
			// then return with false.
			if (it_El->second.eType != TOOL_INPUT)
			{
				return false;
			}

			// if tool exists but has not yet been created, then simply return
			if (it_El->second.bDoCreateView)
			{
				it_El->second.bInUse = true;
				return true;
			}

			// Get reference to tool data
			STool& rTool = it_El->second;

			if ((dVal != rTool.dInitVal) || (dMin != rTool.dInitMin) ||
			    (dMax != rTool.dInitMax))
			{
				rTool.dInitVal = dVal;
				rTool.dInitMin = dMin;
				rTool.dInitMax = dMax;

				rTool.bDoRedrawView = true;
				bRedraw             = true;
			}

			it_El->second.bInUse = true;
			return true;
		}
		else
		{
			m_listToolName.push_back(pcName);
		}

		STool& rTool = m_mapTool[pcName];

		bNew = true;
		// Flag to create tool view at next update
		rTool.bDoCreateView = true;
		rTool.bDoRedrawView = false;
		rTool.sName         = pcName;

		rTool.eType = TOOL_INPUT;
		//rTool.pWidget = (Fl_Widget *) pInput;
		//rTool.pBox = pBox;
		rTool.bInUse    = true;
		rTool.bFixed    = bFixed;
		rTool.dInitMin  = dMin;
		rTool.dInitMax  = dMax;
		rTool.dInitVal  = dVal;
		rTool.pImgAct   = 0;
		rTool.pImgDeact = 0;

		if (pcDesc)
		{
			rTool.sDesc = pcDesc;
		}

		m_bToolsChanged = true;

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Create a tool to the tool window

	bool CFLTKToolBox::CreateToolViewInput(STool& rTool)
	{
		if (!m_pVPackBox)
		{
			return false;
		}

		Fl_Group* pBox;
		Fl_Value_Input* pInput;

		if (!(pBox = new Fl_Group(0, 0, m_iToolWidth, 30)))
		{
			return false;
		}
		pBox->box(FL_DOWN_BOX);
		pBox->begin();

		if (!(pInput = new Fl_Value_Input(3, 12, m_iToolWidth - 6 * m_iToolWBorder, 15, rTool.sName.c_str())))
		{
			return false;
		}

		rTool.pWidget = (Fl_Widget*) pInput;
		rTool.pBox    = pBox;

		pInput->box(FL_FLAT_BOX);
		pInput->labelsize(12);
		pInput->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
		//pInput->color(0x9F9F9F00);
		pInput->range(rTool.dInitMin, rTool.dInitMax);
		pInput->value(pInput->round(rTool.dInitVal));
		pInput->redraw();

		if (rTool.sDesc.length())
		{
			pInput->tooltip(rTool.sDesc.c_str());
		}

		pInput->when(FL_WHEN_ENTER_KEY);
		pInput->callback(m_pToolCallback, m_pToolCallbackData);

		pBox->end();

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Redraw a tool to the tool window

	bool CFLTKToolBox::RedrawToolViewInput(STool& rTool)
	{
		if (!m_pVPackBox)
		{
			return false;
		}

		if (rTool.eType != TOOL_INPUT)
		{
			return false;
		}

		Fl_Value_Input* pInput = (Fl_Value_Input*) rTool.pWidget;

		pInput->range(rTool.dInitMin, rTool.dInitMax);
		pInput->value(pInput->clamp(rTool.dInitVal));
		//pInput->value(pInput->clamp(pInput->value()));
		pInput->redraw();

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Add a tool to the tool window

	bool CFLTKToolBox::AddToolInputText(bool& bNew, bool& bRedraw,		const char* pcName, bool bImmediateCallback, const char* pcInitText, bool bFixed, const char* pcDesc, bool bDoUpdate)
	{
		bNew    = false;
		bRedraw = false;

		if (!m_pVPackBox)
		{
			return false;
		}

		TToolMap::iterator it_El;

		// Check whether slider already exists
		if ((it_El = m_mapTool.find(pcName)) != m_mapTool.end())
		{
			// If already a different tool with the same name exists
			// then return with false.
			if (it_El->second.eType != TOOL_TEXT)
			{
				return false;
			}

			// if tool exists but has not yet been created, then simply return
			if (it_El->second.bDoCreateView)
			{
				it_El->second.bInUse = true;
				return true;
			}

			// Get reference to tool data
			STool& rTool = it_El->second;

			if ((rTool.sInitVal != pcInitText) ||
			    (rTool.bInitVal != bImmediateCallback))
			{
				rTool.sInitVal = pcInitText;
				rTool.bInitVal = bImmediateCallback;

				rTool.bDoRedrawView = true;
				bRedraw             = true;
			}

			it_El->second.bInUse = true;
			return true;
		}
		else
		{
			m_listToolName.push_back(pcName);
		}

		STool& rTool = m_mapTool[pcName];

		bNew = true;
		// Flag to create tool view at next update
		rTool.bDoCreateView = true;
		rTool.bDoRedrawView = false;
		rTool.sName         = pcName;

		rTool.eType = TOOL_TEXT;
		//rTool.pWidget = (Fl_Widget *) pInput;
		//rTool.pBox = pBox;
		rTool.bInUse    = true;
		rTool.bFixed    = bFixed;
		rTool.sInitVal  = pcInitText;
		rTool.bInitVal  = bImmediateCallback;
		rTool.pImgAct   = 0;
		rTool.pImgDeact = 0;

		if (pcDesc)
		{
			rTool.sDesc = pcDesc;
		}

		m_bToolsChanged = true;

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Create a tool to the tool window

	bool CFLTKToolBox::CreateToolViewInputText(STool& rTool)
	{
		if (!m_pVPackBox)
		{
			return false;
		}

		Fl_Group* pBox;
		CInput* pInput;

		if (!(pBox = new Fl_Group(0, 0, m_iToolWidth, 30)))
		{
			return false;
		}
		pBox->box(FL_DOWN_BOX);
		pBox->begin();

		if (!(pInput = new CInput(3, 12, m_iToolWidth - 6 * m_iToolWBorder, 15, rTool.sName.c_str())))
		{
			return false;
		}

		rTool.pWidget = (Fl_Widget*) pInput;
		rTool.pBox    = pBox;

		pInput->box(FL_FLAT_BOX);
		pInput->labelsize(12);
		pInput->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
		//pInput->color(0x9F9F9F00);
		pInput->value(rTool.sInitVal.c_str());
		pInput->redraw();

		if (rTool.sDesc.length())
		{
			pInput->tooltip(rTool.sDesc.c_str());
		}

		if (rTool.bInitVal)
		{
			pInput->when(FL_WHEN_CHANGED);
		}
		else
		{
			pInput->when(FL_WHEN_ENTER_KEY);
		}

		pInput->callback(m_pToolCallback, m_pToolCallbackData);

		pBox->end();

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Redraw a tool to the tool window

	bool CFLTKToolBox::RedrawToolViewInputText(STool& rTool)
	{
		if (!m_pVPackBox)
		{
			return false;
		}

		if (rTool.eType != TOOL_TEXT)
		{
			return false;
		}

		CInput* pInput = (CInput*) rTool.pWidget;

		pInput->value(rTool.sInitVal.c_str());
		if (rTool.bInitVal)
		{
			pInput->when(FL_WHEN_CHANGED);
		}
		else
		{
			pInput->when(FL_WHEN_ENTER_KEY);
		}

		pInput->redraw();

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Add a check box tool to the tool window

	bool CFLTKToolBox::AddToolCheckBox(bool& bNew, bool& bRedraw, const char* pcName, bool bSet, bool bFixed, const char* pcDesc, bool bDoUpdate)
	{
		bNew    = false;
		bRedraw = false;

		if (!m_pVPackBox)
		{
			return false;
		}

		TToolMap::iterator it_El;

		// Check whether slider already exists
		if ((it_El = m_mapTool.find(pcName)) != m_mapTool.end())
		{
			// If already a different tool with the same name exists
			// then return with false.
			if (it_El->second.eType != TOOL_CHECKBOX)
			{
				return false;
			}

			// if tool exists but has not yet been created, then simply return
			if (it_El->second.bDoCreateView)
			{
				it_El->second.bInUse = true;
				return true;
			}

			// don't create anew if yes

			if (bSet != it_El->second.bInitVal)
			{
				it_El->second.bInitVal = bSet;

				it_El->second.bDoRedrawView = true;
				bRedraw                     = true;
			}

			it_El->second.bInUse = true;
			return true;
		}
		else
		{
			m_listToolName.push_back(pcName);
		}

		STool& rTool = m_mapTool[pcName];

		bNew = true;
		// Flag to create tool view at next update
		rTool.bDoCreateView = true;
		rTool.bDoRedrawView = false;
		rTool.sName         = pcName;

		rTool.eType = TOOL_CHECKBOX;
		//rTool.pWidget = (Fl_Widget *) pButton;
		//rTool.pBox = pBox;
		rTool.bInUse    = true;
		rTool.bFixed    = bFixed;
		rTool.bInitVal  = bSet;
		rTool.pImgAct   = 0;
		rTool.pImgDeact = 0;

		if (pcDesc)
		{
			rTool.sDesc = pcDesc;
		}

		m_bToolsChanged = true;

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Create check box tool view

	bool CFLTKToolBox::CreateToolViewCheckBox(STool& rTool)
	{
		if (!m_pVPackBox)
		{
			return false;
		}

		Fl_Group* pBox;
		Fl_Light_Button* pButton;

		//m_pVPackBox->init_sizes();

		if (!(pBox = new Fl_Group(0, 0, m_iToolWidth, 30)))
		{
			return false;
		}
		pBox->box(FL_DOWN_BOX);
		pBox->begin();

		if (!(pButton = new Fl_Light_Button(3, 4, m_iToolWidth - 12 * m_iToolWBorder, 23, rTool.sName.c_str())))
		{
			return false;
		}

		rTool.pWidget = (Fl_Widget*) pButton;
		rTool.pBox    = pBox;

		pButton->box(FL_FLAT_BOX);
		pButton->labelsize(12);
		//pButton->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
		//pButton->color(0x9F9F9F00);
		pButton->value((rTool.bInitVal ? 1 : 0));
		pButton->redraw();

		if (rTool.sDesc.length())
		{
			pButton->tooltip(rTool.sDesc.c_str());
		}

		pButton->callback(m_pToolCallback, m_pToolCallbackData);

		pBox->end();

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Redraw check box tool view

	bool CFLTKToolBox::RedrawToolViewCheckBox(STool& rTool)
	{
		if (!m_pVPackBox)
		{
			return false;
		}

		if (rTool.eType != TOOL_CHECKBOX)
		{
			return false;
		}

		Fl_Light_Button* pButton;

		pButton = (Fl_Light_Button*) rTool.pWidget;
		pButton->value((rTool.bInitVal ? 1 : 0));
		pButton->redraw();

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Add a button tool to the tool window

	bool CFLTKToolBox::AddToolButton(bool& bNew, bool& bRedraw, const char* pcName,	bool bFixed, const char* pcDesc, bool bDoUpdate)
	{
		bNew    = false;
		bRedraw = false;

		if (!m_pVPackBox)
		{
			return false;
		}

		TToolMap::iterator it_El;

		// Check whether slider already exists
		if ((it_El = m_mapTool.find(pcName)) != m_mapTool.end())
		{
			// If already a different tool with the same name exists
			// then return with false.
			if (it_El->second.eType != TOOL_BUTTON)
			{
				return false;
			}

			// don't create anew if yes
			it_El->second.bInUse = true;
			bRedraw              = true;
			return true;
		}
		else
		{
			m_listToolName.push_back(pcName);
		}

		STool& rTool = m_mapTool[pcName];

		bNew = true;
		// Flag to create tool view at next update
		rTool.bDoCreateView = true;
		rTool.bDoRedrawView = false;
		rTool.sName         = pcName;

		rTool.eType = TOOL_BUTTON;
		//rTool.pWidget = (Fl_Widget *) pButton;
		//rTool.pBox = pBox;
		rTool.bInUse    = true;
		rTool.bFixed    = bFixed;
		rTool.pImgAct   = 0;
		rTool.pImgDeact = 0;

		if (pcDesc)
		{
			rTool.sDesc = pcDesc;
		}

		m_bToolsChanged = true;

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Create a button tool view

	bool CFLTKToolBox::CreateToolViewButton(STool& rTool)
	{
		if (!m_pVPackBox)
		{
			return false;
		}

		Fl_Group* pBox;
		Fl_Button* pButton;

		//m_pVPackBox->init_sizes();

		if (!(pBox = new Fl_Group(0, 0, m_iToolWidth, m_iTotalHeight)))
		{
			return false;
		}
		pBox->box(FL_DOWN_BOX);
		pBox->begin();

		if (!(pButton = new Fl_Button(m_iToolWBorder, m_iToolHBorder,
				      m_iToolWidth - 2 * m_iToolWBorder, m_iToolHeight, rTool.sName.c_str())))
		{
			return false;
		}

		rTool.pWidget = (Fl_Widget*) pButton;
		rTool.pBox    = pBox;

		//pButton->box(FL_FLAT_BOX);
		pButton->labelsize(12);
		pButton->when(FL_WHEN_RELEASE);
		//pButton->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
		//pButton->color(0x9F9F9F00);
		//pButton->value((bSet ? 1 : 0));
		pButton->redraw();

		if (rTool.sDesc.length())
		{
			pButton->tooltip(rTool.sDesc.c_str());
		}

		pButton->callback(m_pToolCallback, m_pToolCallbackData);

		pBox->end();

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Redraw a button tool view

	bool CFLTKToolBox::RedrawToolViewButton(STool& rTool)
	{
		if (!m_pVPackBox)
		{
			return false;
		}

		if (rTool.eType != TOOL_BUTTON)
		{
			return false;
		}

		return true;
	}

//////////////////////////////////////////////////////////////////////////
// Add an Image Button tool

	bool CFLTKToolBox::AddToolImageButton(const char* pcName, COGLImage* pImgAct, COGLImage* pImgDeact, bool bFixed, const char* pcDesc, bool bDoUpdate)
	{
		if (!m_pVPackBox || !pImgAct)
		{
			return false;
		}

		bool bResetImg = false;
		int iWidth, iHeight;
		Fl_Group* pBox = nullptr;
		Fl_Button* pButton = nullptr;
		TToolMap::iterator it_El;

		pImgAct->GetSize(iWidth, iHeight);

		if (iWidth < 32)
		{
			iWidth = 32;
		}
		else if (iWidth > m_iToolWidth)
		{
			iWidth = m_iToolWidth;
		}

		//int iToolWidth = 32 + 2*m_iToolWBorder;
		int iToolWidth = iWidth + 10 + 2 * m_iToolWBorder;

		// Check whether slider already exists
		if ((it_El = m_mapTool.find(pcName)) != m_mapTool.end())
		{
			// If already a different tool with the same name exists
			// then return with false.
			if (it_El->second.eType != TOOL_IMGBUT)
			{
				return false;
			}

			// don't create anew if yes
			it_El->second.bInUse = true;

			bResetImg = true;
			//return true;
		}
		else
		{
			m_listToolName.push_back(pcName);
		}

		STool& rTool = m_mapTool[pcName];

		if (!bResetImg)
		{
			m_pVPackBox->init_sizes();

			if (!(pBox = new Fl_Group(0, 0, iToolWidth, 30)))
			{
				return false;
			}
			pBox->box(FL_DOWN_BOX);
			pBox->begin();

			rTool.sName = pcName;

			if (!(pButton = new Fl_Button(3, 4, iToolWidth - 6 * m_iToolWBorder, 23, pcName)))
			{
				return false;
			}
		}
		else
		{
			pButton = (Fl_Button*) rTool.pWidget;
			pButton->image(0);
			pButton->deimage(0);

			if (rTool.pImgAct)
			{
				if (rTool.pImgAct == rTool.pImgDeact)
				{
					delete rTool.pImgAct;
				}
				else
				{
					delete rTool.pImgAct;

					if (rTool.pImgDeact)
					{
						delete rTool.pImgDeact;
					}
				}

				rTool.pImgAct   = 0;
				rTool.pImgDeact = 0;
			}
		}

		COGLImage oglImgAct(*pImgAct);

		//oglImgAct.FlipImage( false );
		oglImgAct.GetSize(iWidth, iHeight);
		::LockImageAccess();
		if (!(rTool.pImgAct = new Fl_RGB_Image(oglImgAct.GetDataPtr(), iWidth, iHeight, 4)))
		{
			::UnlockImageAccess();
			delete pButton;
			return false;
		}
		::UnlockImageAccess();

		if (pImgDeact)
		{
			COGLImage oglImgDeact(*pImgDeact);

			//oglImgDeact.FlipImage( false );
			oglImgDeact.GetSize(iWidth, iHeight);
			::LockImageAccess();
			if (!(rTool.pImgDeact = new Fl_RGB_Image(oglImgDeact.GetDataPtr(), iWidth, iHeight, 4)))
			{
				::UnlockImageAccess();
				delete rTool.pImgAct;
				delete pButton;
				return false;
			}
			::UnlockImageAccess();
		}
		else
		{
			rTool.pImgDeact = rTool.pImgAct;
		}

		pButton->image(rTool.pImgAct);
		pButton->deimage(rTool.pImgDeact);

		if (!bResetImg)
		{
			rTool.eType   = TOOL_IMGBUT;
			rTool.pWidget = (Fl_Widget*) pButton;
			rTool.pBox    = pBox;
			rTool.bInUse  = true;
			rTool.bFixed  = bFixed;

			if (pcDesc)
			{
				rTool.sDesc = pcDesc;
			}

			//pButton->box(FL_FLAT_BOX);
			pButton->labelsize(12);
			pButton->when(FL_WHEN_RELEASE);
			pButton->labeltype(_FL_ICON_LABEL);

			//pButton->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
			//pButton->color(0x9F9F9F00);
			//pButton->value((bSet ? 1 : 0));
			pButton->redraw();

			if (pcDesc)
			{
				pButton->tooltip(rTool.sDesc.c_str());
			}

			pButton->callback(m_pToolCallback, m_pToolCallbackData);

			pBox->end();
		}

		m_bToolsChanged = true;

		// Now update the packs
		if (bDoUpdate)
		{
			Update();
		}

		return true;
	}

//////////////////////////////////////////////////////////////////////////
// Add a ComboBox tool

	bool CFLTKToolBox::AddToolChoice(bool& bNew, bool& bRedraw,	const char* pcName, vector<string>& vecData, const char* pcChoice, bool bFixed, const char* pcDesc, bool bDoUpdate)
	{
		bNew    = false;
		bRedraw = false;

		if (!m_pVPackBox)
		{
			return false;
		}

		TToolMap::iterator it_El;

		// Check whether slider already exists
		if ((it_El = m_mapTool.find(pcName)) != m_mapTool.end())
		{
			// If already a different tool with the same name exists
			// then return with false.
			if (it_El->second.eType != TOOL_CHOICE)
			{
				return false;
			}

			// if tool exists but has not yet been created, then simply return
			if (it_El->second.bDoCreateView)
			{
				it_El->second.bInUse = true;
				return true;
			}

			// Get reference to tool data
			STool& rTool = it_El->second;

			bool bUpdateSel = false;
			int iStr, iStrCnt = (int) vecData.size();

			// Use bInitVal as flag whether the whole list needs to be reset
			//rTool.bInitVal = false;

			if (iStrCnt != rTool.vecStr.size())
			{
				rTool.vecStr = vecData;
				// Use bInitVal as flag whether the whole list needs to be reset
				rTool.bInitVal = true;
				bUpdateSel     = true;
			}
			else
			{
				for (iStr = 0; iStr < iStrCnt; ++iStr)
				{
					if (rTool.vecStr[iStr] != vecData[iStr])
					{
						break;
					}
				}

				if (iStr < iStrCnt)
				{
					rTool.vecStr = vecData;
					// Use bInitVal as flag whether the whole list needs to be reset
					rTool.bInitVal = true;
					bUpdateSel     = true;
				}
			}

			if (pcChoice && !(rTool.sInitVal == pcChoice))
			{
				rTool.sInitVal = pcChoice;
				bUpdateSel     = true;
			}

			if (bUpdateSel)
			{
				rTool.iInitVal = 0;
				for (iStr = 0; iStr < iStrCnt; ++iStr)
				{
					if (rTool.vecStr[iStr] == rTool.sInitVal)
					{
						rTool.iInitVal = iStr;
						break;
					}
				}
			}

			if (bUpdateSel)
			{
				rTool.bDoRedrawView = true;
				bRedraw             = true;
			}

			// don't create anew if yes
			it_El->second.bInUse = true;
			return true;
		}
		else
		{
			m_listToolName.push_back(pcName);
		}

		STool& rTool = m_mapTool[pcName];

		bNew = true;
		// Flag to create tool view at next update
		rTool.bDoCreateView = true;
		rTool.bDoRedrawView = false;
		rTool.sName         = pcName;

		rTool.eType = TOOL_CHOICE;
		//rTool.pWidget = (Fl_Widget *) pChoice;
		//rTool.pBox = pBox;
		rTool.bInUse    = true;
		rTool.bFixed    = bFixed;
		rTool.pImgAct   = 0;
		rTool.pImgDeact = 0;
		if (pcChoice)
		{
			rTool.sInitVal = pcChoice;
		}
		else
		{
			rTool.sInitVal = "";
		}

		if (pcDesc)
		{
			rTool.sDesc = pcDesc;
		}

		// Use bInitVal as flag whether the whole list needs to be reset
		rTool.bInitVal = true;

		rTool.vecStr = vecData;

		int i, iCount = int(rTool.vecStr.size());
		int iSel = 0;
		for (i = 0; i < iCount; i++)
		{
			if (pcChoice && (rTool.vecStr[i] == pcChoice))
			{
				iSel = i;
				break;
			}
		}

		rTool.iInitVal = iSel;

		m_bToolsChanged = true;

		return true;
	}

//////////////////////////////////////////////////////////////////////////
// Create a ComboBox tool

	bool CFLTKToolBox::CreateToolViewChoice(STool& rTool)
	{
		if (!m_pVPackBox)
		{
			return false;
		}

		Fl_Group* pBox;
		Fl_Choice* pChoice;

		//m_pVPackBox->init_sizes();

		if (!(pBox = new Fl_Group(0, 0, m_iToolWidth, 30)))
		{
			return false;
		}
		pBox->box(FL_DOWN_BOX);
		pBox->begin();

		if (!(pChoice = new Fl_Choice(4, 12, m_iToolWidth - 6 * m_iToolWBorder, 15, rTool.sName.c_str())))
		{
			return false;
		}

		rTool.pWidget = (Fl_Widget*) pChoice;
		rTool.pBox    = pBox;

		//pButton->box(FL_FLAT_BOX);
		pChoice->labelsize(10);
		pChoice->textsize(12);
		pChoice->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
		//pChoice->when(FL_WHEN_RELEASE);
		//pButton->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
		//pButton->color(0x9F9F9F00);
		//pButton->value((bSet ? 1 : 0));

		// Use bInitVal as flag whether the whole list needs to be reset
		rTool.bInitVal = false;
		int iIdx, iCnt = int(rTool.vecStr.size());

		pChoice->clear();
		for (iIdx = 0; iIdx < iCnt; iIdx++)
		{
			pChoice->add(rTool.vecStr[iIdx].c_str());
		}

		pChoice->value(rTool.iInitVal);
		pChoice->redraw();

		if (rTool.sDesc.length())
		{
			pChoice->tooltip(rTool.sDesc.c_str());
		}

		pChoice->callback(m_pToolCallback, m_pToolCallbackData);

		pBox->end();

		return true;
	}

//////////////////////////////////////////////////////////////////////////
// Redraw a ComboBox tool

	bool CFLTKToolBox::RedrawToolViewChoice(STool& rTool)
	{
		if (!m_pVPackBox)
		{
			return false;
		}

		if (rTool.eType != TOOL_CHOICE)
		{
			return false;
		}

		// don't create anew if yes
		Fl_Choice* pChoice = (Fl_Choice*) rTool.pWidget;

		// Reset Choice list if needed
		if (rTool.bInitVal)
		{
			int iIdx, iCnt = int(rTool.vecStr.size());

			pChoice->clear();
			for (iIdx = 0; iIdx < iCnt; iIdx++)
			{
				pChoice->add(rTool.vecStr[iIdx].c_str());
			}

			rTool.bInitVal = false;
		}

		pChoice->value(rTool.iInitVal);
		pChoice->redraw();

		return true;
	}

//////////////////////////////////////////////////////////////////////////
// Add an Image Button Toolbar Object

	bool CFLTKToolBox::AddToolbarImageButton(bool& bNew, bool& bRedraw,		const char* pcName, COGLImage* pImgAct, COGLImage* pImgDeact, bool bFixed, const char* pcDesc, bool bDoUpdate)
	{
		bNew    = false;
		bRedraw = false;

		if (!m_pVPackBox || !pImgAct)
		{
			return false;
		}

		TToolMap::iterator it_El;

		// Check whether slider already exists
		if ((it_El = m_mapTool.find(pcName)) != m_mapTool.end())
		{
			// If already a different tool with the same name exists
			// then return with false.
			if (it_El->second.eType != TOOL_IMGBUT)
			{
				return false;
			}

			// if tool exists but has not yet been created, then simply return
			if (it_El->second.bDoCreateView)
			{
				it_El->second.bInUse = true;
				return true;
			}

			STool& rTool = it_El->second;

			if (rTool.poglImgAct == rTool.poglImgDeact)
			{
				if (rTool.poglImgAct)
				{
					delete rTool.poglImgAct;
				}
			}
			else
			{
				if (rTool.poglImgAct)
				{
					delete rTool.poglImgAct;
				}

				if (rTool.poglImgDeact)
				{
					delete rTool.poglImgDeact;
				}
			}

			if (pImgAct)
			{
				rTool.poglImgAct = new COGLImage(*pImgAct);
			}

			if (pImgDeact)
			{
				rTool.poglImgDeact = new COGLImage(*pImgDeact);
			}
			else
			{
				rTool.poglImgDeact = rTool.poglImgAct;
			}

			// don't create anew if yes
			it_El->second.bInUse = true;

			rTool.bDoRedrawView = true;
			bRedraw             = true;
			return true;
		}
		else
		{
			m_listToolName.push_back(pcName);
		}

		STool& rTool = m_mapTool[pcName];

		rTool.sName = pcName;

		if (!(rTool.poglImgAct = new COGLImage(*pImgAct)))
		{
			return false;
		}

		if (pImgDeact)
		{
			if (!(rTool.poglImgDeact = new COGLImage(*pImgDeact)))
			{
				delete rTool.poglImgAct;
				rTool.poglImgAct = 0;
				return false;
			}
		}
		else
		{
			rTool.poglImgDeact = rTool.poglImgAct;
		}

		bNew = true;
		// Flag to create tool view at next update
		rTool.bDoCreateView = true;
		rTool.bDoRedrawView = false;
		rTool.eType         = TOOL_IMGBUT;
		//rTool.pWidget = (Fl_Widget *) pButton;
		//rTool.pBox = pBox;
		rTool.bInUse = true;
		rTool.bFixed = bFixed;

		if (pcDesc)
		{
			rTool.sDesc = pcDesc;
		}

		m_bToolsChanged = true;

		return true;
	}

//////////////////////////////////////////////////////////////////////////
// Create an Image Button View

	bool CFLTKToolBox::CreateToolbarViewImageButton(STool& rTool)
	{
		if (!m_pVPackBox)
		{
			return false;
		}

		int iToolWidth = 24 + 2 * m_iToolbarWBorder;

		int iWidth, iHeight;
		Fl_Group* pBox;
		Fl_Button* pButton;

		//m_pVPackBox->init_sizes();

		if (!(pBox = new Fl_Group(0, 0, iToolWidth, m_iToolHeight + 2 * m_iToolHBorder)))
		{
			return false;
		}
		pBox->box(FL_FLAT_BOX);
		pBox->color(FL_LIGHT2);
		pBox->begin();

		if (!(pButton = new Fl_Button(m_iToolbarWBorder, m_iToolHBorder, 24, 24, rTool.sName.c_str())))
		{
			return false;
		}

		COGLImage& oglImgAct = *rTool.poglImgAct;
		int iImgType, iDataType, iBytesPerPixel;
		int iChannel;

		//oglImgAct.FlipImage( false );
		oglImgAct.GetSize(iWidth, iHeight);
		oglImgAct.GetType(iImgType, iDataType, iBytesPerPixel);

		if ((iImgType == CLUVIZ_IMG_RGB) &&
		    ((iDataType == CLUVIZ_IMG_BYTE) || (iDataType == CLUVIZ_IMG_UNSIGNED_BYTE)))
		{
			iChannel = 3;
		}
		else if ((iImgType == CLUVIZ_IMG_RGBA) &&
			 ((iDataType == CLUVIZ_IMG_BYTE) || (iDataType == CLUVIZ_IMG_UNSIGNED_BYTE)))
		{
			iChannel = 4;
		}
		else
		{
			oglImgAct.ConvertType(CLUVIZ_IMG_RGB, CLUVIZ_IMG_UNSIGNED_BYTE);
		}

		::LockImageAccess();
		if (!(rTool.pImgAct = new Fl_RGB_Image(oglImgAct.GetDataPtr(), iWidth, iHeight, iChannel)))
		{
			::UnlockImageAccess();
			if (rTool.poglImgAct == rTool.poglImgDeact)
			{
				delete rTool.poglImgAct;
			}
			else
			{
				delete rTool.poglImgAct;
				if (rTool.poglImgDeact)
				{
					delete rTool.poglImgDeact;
				}
			}

			delete pButton;
			delete pBox;

			pButton            = 0;
			rTool.poglImgAct   = 0;
			rTool.poglImgDeact = 0;

			return false;
		}
		::UnlockImageAccess();

		if (rTool.poglImgDeact)
		{
			COGLImage& oglImgDeact = *rTool.poglImgDeact;

			//oglImgDeact.FlipImage( false );
			oglImgDeact.GetSize(iWidth, iHeight);
			::LockImageAccess();
			if (!(rTool.pImgDeact = new Fl_RGB_Image(oglImgDeact.GetDataPtr(), iWidth, iHeight, 4)))
			{
				::UnlockImageAccess();
				if (rTool.poglImgAct == rTool.poglImgDeact)
				{
					delete rTool.poglImgAct;
				}
				else
				{
					delete rTool.poglImgAct;
					if (rTool.poglImgDeact)
					{
						delete rTool.poglImgDeact;
					}
				}

				delete rTool.pImgAct;
				delete pButton;
				delete pBox;

				pButton            = 0;
				rTool.pImgAct      = 0;
				rTool.poglImgAct   = 0;
				rTool.poglImgDeact = 0;
				return false;
			}
			::UnlockImageAccess();
		}
		else
		{
			rTool.pImgDeact = rTool.pImgAct;
		}

		pButton->image(rTool.pImgAct);
		pButton->deimage(rTool.pImgDeact);

		rTool.pWidget = (Fl_Widget*) pButton;
		rTool.pBox    = pBox;

		pButton->box(FL_FLAT_BOX);
		pButton->labelsize(12);
		pButton->when(FL_WHEN_RELEASE);
		pButton->labeltype(_FL_ICON_LABEL);

		//pButton->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
		//pButton->color(0x9F9F9F00);
		//pButton->value((bSet ? 1 : 0));
		pButton->redraw();

		if (rTool.sDesc.c_str())
		{
			pButton->tooltip(rTool.sDesc.c_str());
		}

		pButton->callback(m_pToolCallback, m_pToolCallbackData);

		pBox->end();

		return true;
	}

//////////////////////////////////////////////////////////////////////////
// Redraw an Image Button View

	bool CFLTKToolBox::RedrawToolbarViewImageButton(STool& rTool)
	{
		if (!m_pVPackBox)
		{
			return false;
		}

		int iWidth, iHeight;
		Fl_Button* pButton;

		pButton = (Fl_Button*) rTool.pWidget;
		pButton->image(0);
		pButton->deimage(0);

		if (rTool.pImgAct)
		{
			if (rTool.pImgAct == rTool.pImgDeact)
			{
				delete rTool.pImgAct;
			}
			else
			{
				delete rTool.pImgAct;

				if (rTool.pImgDeact)
				{
					delete rTool.pImgDeact;
				}
			}

			rTool.pImgAct   = 0;
			rTool.pImgDeact = 0;
		}

		if (!rTool.poglImgAct)
		{
			return false;
		}

		COGLImage& oglImgAct = *rTool.poglImgAct;
		int iImgType, iDataType, iBytesPerPixel;
		int iChannel;

		//oglImgAct.FlipImage( false );
		oglImgAct.GetSize(iWidth, iHeight);
		oglImgAct.GetType(iImgType, iDataType, iBytesPerPixel);

		if ((iImgType == CLUVIZ_IMG_RGB) &&
		    ((iDataType == CLUVIZ_IMG_BYTE) || (iDataType == CLUVIZ_IMG_UNSIGNED_BYTE)))
		{
			iChannel = 3;
		}
		else if ((iImgType == CLUVIZ_IMG_RGBA) &&
			 ((iDataType == CLUVIZ_IMG_BYTE) || (iDataType == CLUVIZ_IMG_UNSIGNED_BYTE)))
		{
			iChannel = 4;
		}
		else
		{
			oglImgAct.ConvertType(CLUVIZ_IMG_RGB, CLUVIZ_IMG_UNSIGNED_BYTE);
		}

		::LockImageAccess();
		if (!(rTool.pImgAct = new Fl_RGB_Image(oglImgAct.GetDataPtr(), iWidth, iHeight, iChannel)))
		{
			::UnlockImageAccess();
			return false;
		}
		::UnlockImageAccess();

		if (rTool.poglImgDeact)
		{
			COGLImage& oglImgDeact = *rTool.poglImgDeact;

			//oglImgDeact.FlipImage( false );
			oglImgDeact.GetSize(iWidth, iHeight);
			::LockImageAccess();
			if (!(rTool.pImgDeact = new Fl_RGB_Image(oglImgDeact.GetDataPtr(), iWidth, iHeight, 4)))
			{
				::UnlockImageAccess();
				delete rTool.pImgAct;
				rTool.pImgAct = 0;
				return false;
			}
			::UnlockImageAccess();
		}
		else
		{
			rTool.pImgDeact = rTool.pImgAct;
		}

		pButton->image(rTool.pImgAct);
		pButton->deimage(rTool.pImgDeact);

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Add a tool to the toolbar

	bool CFLTKToolBox::AddToolbarCounter(bool& bNew,
		bool& bRedraw,
		const char* pcName,
		const char* pcUnit,
		double dMin,
		double dMax,
		double dStep,
		double dLStep,
		double dVal,
		bool bActive,
		bool bFixed,
		const char* pcDesc,
		bool bDoUpdate)
	{
		bNew    = false;
		bRedraw = false;

		if (!m_pVPackBox)
		{
			return false;
		}

		TToolMap::iterator it_El;

		// Check whether slider already exists
		if ((it_El = m_mapTool.find(pcName)) != m_mapTool.end())
		{
			// If already a different tool with the same name exists
			// then return with false.
			if (it_El->second.eType != TOOL_COUNTER)
			{
				return false;
			}

			// if tool exists but has not yet been created, then simply return
			if (it_El->second.bDoCreateView)
			{
				it_El->second.bInUse = true;
				return true;
			}

			// Get reference to tool data
			STool& rTool = it_El->second;

			if (bActive != rTool.bInitActive)
			{
				rTool.bActive       = bActive;
				rTool.bDoRedrawView = true;
				bRedraw             = true;
			}

			if ((dVal != rTool.dInitVal) || (dMin != rTool.dInitMin) ||
			    (dMax != rTool.dInitMax) || (dStep != rTool.dInitStep) ||
			    (dLStep != rTool.dInitLStep))
			{
				rTool.dInitVal    = dVal;
				rTool.dInitMin    = dMin;
				rTool.dInitMax    = dMax;
				rTool.dInitStep   = dStep;
				rTool.dInitLStep  = dLStep;
				rTool.bInitActive = bActive;

				rTool.bDoRedrawView = true;
				bRedraw             = true;
			}

			it_El->second.bInUse = true;
			return true;
		}
		else
		{
			m_listToolName.push_back(pcName);
		}

		STool& rTool = m_mapTool[pcName];

		char pcFormat[50];
		int iDigits, iPow;
		double dDigits, dPow;

		dDigits = log(dStep) / log(10.0);
		if (dDigits < 0)
		{
			iDigits = int(floor(dDigits));
		}
		else
		{
			iDigits = int(ceil(dDigits));
		}

		dPow = log(dMax) / log(10.0);
		if (dPow < 0)
		{
			iPow = int(floor(dPow));
		}
		else
		{
			iPow = int(ceil(dPow));
		}

		if (iPow < 0)
		{
			if (iPow < iDigits)
			{
				iDigits = iPow;
			}
			iPow = 1;
		}

		if (iDigits < 0)
		{
			iDigits = -iDigits;
		}
		else if (iDigits > 0)
		{
			iDigits = 0;
		}

		sprintf_s(pcFormat, 50, "%%%d.%df", iPow, iDigits);

		bNew = true;
		// Flag to create tool view at next update
		rTool.bDoCreateView = true;
		rTool.bDoRedrawView = false;
		rTool.sName         = pcName;

		rTool.eType = TOOL_COUNTER;
		//rTool.pWidget = (Fl_Widget *) pCounter;
		//rTool.pBox = pBox;
		rTool.bInUse     = true;
		rTool.bFixed     = bFixed;
		rTool.dInitVal   = dVal;
		rTool.dInitMin   = dMin;
		rTool.dInitMax   = dMax;
		rTool.dInitStep  = dStep;
		rTool.dInitLStep = dLStep;
		rTool.pImgAct    = 0;
		rTool.pImgDeact  = 0;

		rTool.bActive     = bActive;
		rTool.bInitActive = bActive;

		rTool.sFormat = pcFormat;
		if (pcUnit)
		{
			rTool.sUnit = pcUnit;
		}

		if (pcDesc)
		{
			rTool.sDesc = pcDesc;
		}

		m_bToolsChanged = true;

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Create a tool to the toolbar

	bool CFLTKToolBox::CreateToolbarViewCounter(STool& rTool)
	{
		if (!m_pVPackBox)
		{
			return false;
		}

		Fl_Group* pBox;
		Fl_Counter* pCounter;
		Fl_Label Label;

		char pcNo[50];
		int iW, iH;	//, iOffset;

		sprintf_s(pcNo, 50, rTool.sFormat.c_str(), rTool.dInitMax);
		if (rTool.sUnit.length())
		{
			strcat_s(pcNo, 50, rTool.sUnit.c_str());
		}

		Label.font    = FL_HELVETICA;
		Label.size    = 12;
		Label.type    = FL_NORMAL_LABEL;
		Label.value   = pcNo;
		Label.image   = 0;
		Label.deimage = 0;
		Label.color   = FL_BLACK;
		Label.measure(iW, iH);

		//fl_font( 1, 12 );
		//fl_measure( pcNo, iW, iH, 0 );

		int iToolWidth       = 24 + iW;	//10 * strlen( pcNo ) + 5 + iOffset;
		int iTotalToolWidth  = iToolWidth + 2 * m_iToolbarWBorder;
		int iToolHeight      = 24;
		int iTotalToolHeight = iToolHeight + 2 * m_iToolHBorder;

		if (!(pBox = new Fl_Group(0, 0, iTotalToolWidth, iTotalToolHeight)))
		{
			return false;
		}
		pBox->box(FL_FLAT_BOX);
		pBox->color(FL_LIGHT2);
		pBox->begin();

		if (!(pCounter = new Fl_Counter(m_iToolbarWBorder, m_iToolHBorder,
				      iToolWidth, iToolHeight, rTool.sName.c_str())))
		{
			return false;
		}

		rTool.pWidget = (Fl_Widget*) pCounter;
		rTool.pBox    = pBox;

		pCounter->type(FL_UPDOWN_COUNTER);
		pCounter->box(FL_FLAT_BOX);
		pCounter->labelsize(12);
		pCounter->labeltype(FL_NO_LABEL);
		pCounter->align(FL_ALIGN_CENTER | FL_ALIGN_LEFT);
		pCounter->color(FL_LIGHT2);	//0x9F9F9F00);
		pCounter->selection_color(1);
		pCounter->range(rTool.dInitMin, rTool.dInitMax);
		pCounter->step(rTool.dInitStep);
		pCounter->lstep(rTool.dInitLStep);
		pCounter->value(pCounter->round(rTool.dInitVal));
		pCounter->SetFormatStr(rTool.sFormat.c_str());
		if (rTool.sUnit.length())
		{
			pCounter->SetUnitStr(rTool.sUnit.c_str());
		}

		if (rTool.bInitActive)
		{
			pCounter->activate();
		}
		else
		{
			pCounter->deactivate();
		}

		pCounter->redraw();

		if (rTool.sDesc.length())
		{
			pCounter->tooltip(rTool.sDesc.c_str());
		}

		pCounter->callback(m_pToolCallback, m_pToolCallbackData);

		pBox->end();

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Redraw a tool to the toolbar

	bool CFLTKToolBox::RedrawToolbarViewCounter(STool& rTool)
	{
		if (!m_pVPackBox)
		{
			return false;
		}

		if (rTool.eType != TOOL_COUNTER)
		{
			return false;
		}

		Fl_Counter* pCounter = (Fl_Counter*) rTool.pWidget;

		// If these values differ, only update activation status
		if (rTool.bActive != rTool.bInitActive)
		{
			rTool.bInitActive = rTool.bActive;
		}
		else
		{
			pCounter->range(rTool.dInitMin, rTool.dInitMax);
			pCounter->step(rTool.dInitStep);
			pCounter->lstep(rTool.dInitLStep);
			pCounter->value(pCounter->round(rTool.dInitVal));
		}

		if (rTool.bInitActive)
		{
			pCounter->activate();
		}
		else
		{
			pCounter->deactivate();
		}

		pCounter->redraw();

		return true;
	}

///////////////////////////////////////////////////////////////////////////////////////
/// Add Toolbar Label

	bool CFLTKToolBox::AddToolbarLabel(bool& bNew, bool& bRedraw,		const char* pcName, bool bFixed, const char* pcDesc, bool bDoUpdate)
	{
		bNew    = false;
		bRedraw = false;

		if (!m_pVPackBox)
		{
			return false;
		}

		TToolMap::iterator it_El;

		// Check whether slider already exists
		if ((it_El = m_mapTool.find(pcName)) != m_mapTool.end())
		{
			// If already a different tool with the same name exists
			// then return with false.
			if (it_El->second.eType != TOOL_LABEL)
			{
				return false;
			}

			// if tool exists but has not yet been created, then simply return
			if (it_El->second.bDoCreateView)
			{
				it_El->second.bInUse = true;
				return true;
			}

			it_El->second.bInUse = true;
			return true;
		}
		else
		{
			m_listToolName.push_back(pcName);
		}

		STool& rTool = m_mapTool[pcName];

		bNew = true;
		// Flag to create tool view at next update
		rTool.bDoCreateView = true;
		rTool.bDoRedrawView = false;
		rTool.sName         = pcName;

		rTool.eType = TOOL_LABEL;
		//rTool.pWidget = (Fl_Widget *) pLabel;
		//rTool.pBox = pBox;
		rTool.bInUse    = true;
		rTool.bFixed    = bFixed;
		rTool.pImgAct   = 0;
		rTool.pImgDeact = 0;

		if (pcDesc)
		{
			rTool.sDesc = pcDesc;
		}

		m_bToolsChanged = true;

		return true;
	}

///////////////////////////////////////////////////////////////////////////////////////
/// Create View Toolbar Label

	bool CFLTKToolBox::CreateToolbarViewLabel(STool& rTool)
	{
		if (!m_pVPackBox)
		{
			return false;
		}

		Fl_Group* pBox;
		Fl_Box* pLabel, * pSep;

		int iSepWidth      = 2;
		int iTotalSepWidth = iSepWidth + 4 + m_iToolbarWBorder;

		int iToolWidth       = int(rTool.sName[0] == ' ' ? 1 : 9 * rTool.sName.length());
		int iTotalToolWidth  = iTotalSepWidth + iToolWidth + m_iToolbarWBorder;
		int iToolHeight      = 24;
		int iTotalToolHeight = iToolHeight + 2 * m_iToolHBorder;

		if (!(pBox = new Fl_Group(0, 0, iTotalToolWidth, iTotalToolHeight)))
		{
			return false;
		}
		pBox->box(FL_FLAT_BOX);
		pBox->color(FL_LIGHT2);
		pBox->begin();

		// Create Separater
		if (!(pSep = new Fl_Box(FL_FLAT_BOX, m_iToolbarWBorder + 4, m_iToolHBorder, iSepWidth, iToolHeight,
				      (char*) 0	/*rTool.sName.c_str()*/)))
		{
			return false;
		}

		pSep->color(FL_DARK1);
		pSep->align(FL_ALIGN_CENTER | FL_ALIGN_RIGHT);
		pSep->redraw();

		// Create Label
		char* pcLabel = 0;
		if ((rTool.sName.length() > 0) && (rTool.sName[0] != ' '))
		{
			pcLabel = (char*) rTool.sName.c_str();
		}

		if (!(pLabel = new Fl_Box(FL_FLAT_BOX, iTotalSepWidth + 3, m_iToolHBorder, iToolWidth, iToolHeight,
				      pcLabel)))
		{
			return false;
		}

		rTool.pWidget = (Fl_Widget*) pLabel;
		rTool.pBox    = pBox;

		pLabel->labelsize(12);
		pLabel->align(FL_ALIGN_CENTER | FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
		pLabel->color(FL_LIGHT2);	//0x9F9F9F00);
		pLabel->redraw();

		if (rTool.sDesc.length())
		{
			pLabel->tooltip(rTool.sDesc.c_str());
		}

		pLabel->callback(m_pToolCallback, m_pToolCallbackData);

		pBox->end();

		return true;
	}

///////////////////////////////////////////////////////////////////////////////////////
/// Redraw View Toolbar Label

	bool CFLTKToolBox::RedrawToolbarViewLabel(STool& rTool)
	{
		if (!m_pVPackBox)
		{
			return false;
		}

		if (rTool.eType != TOOL_LABEL)
		{
			return false;
		}

		return true;
	}

#endif	// CLUDRAW_FLTK
