////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.View.Base
// file:      FLTKEditor.cpp
//
// summary:   Implements the fltk editor class
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

#include "config_cluviz.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include <string>

#include "FLTKEditor.h"
#include "CluTec.Viz.Base\mem.cxx"
#include "CluTec.Viz.Base\memobj.cxx"

//#include "windows.h"

template class Mem<CFLTKEditor::SEditorData*>;
template class MemObj<CFLTKEditor::SEditorData>;

int CFLTKEditor::sm_iNewFileNo = 1;

using namespace std;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BlinkCursor(void* pxThis)
{
	CFLTKEditor::SEditorData* pxEditorData = (CFLTKEditor::SEditorData*) pxThis;

	pxEditorData->m_bCursorBlinkState = !pxEditorData->m_bCursorBlinkState;
	pxEditorData->m_pEditor->show_cursor(bool(pxEditorData->m_bCursorBlinkState));

	Fl::repeat_timeout(0.5, BlinkCursor, pxThis);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CFLTKEditor::SEditorData::SEditorData()
{
	m_bChanged  = false;
	m_bIsBinary = false;

	#ifdef WIN32
		m_sPath = ".\\";
	#else
		m_sPath = "./";
	#endif

	m_pcSearchString[0] = 0;

	m_pmCode    = 0;
	m_pTextBuf  = 0;
	m_pStyleBuf = 0;
	m_pEditor   = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CFLTKEditor::SEditorData::~SEditorData()
{
	Destroy();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CFLTKEditor::SEditorData::Create(CFLTKEditor& rParent)
{
	m_pmCode = new Mem<char>;

	m_pTextBuf = new Fl_Text_Buffer;
	m_pTextBuf->tab_distance(4);

	m_pStyleBuf = new Fl_Text_Buffer;

	rParent.begin();
	m_pEditor = new Fl_Text_Editor(0, 46, rParent.w(), rParent.h() - 46);
	m_pEditor->buffer(m_pTextBuf);

	m_pEditor->highlight_data(m_pStyleBuf, &(rParent.GetStyleTable()[0]),
			int(rParent.GetStyleTable().size()), 'A',
			CFLTKEditor::CB_StyleUnfinished, &rParent);

	m_pEditor->textfont(FL_COURIER);

	m_pEditor->remove_key_binding('v', FL_CTRL);
	m_pEditor->remove_key_binding('x', FL_CTRL);
	m_pEditor->remove_key_binding('c', FL_CTRL);
	m_pEditor->remove_key_binding('z', FL_CTRL);
	m_pEditor->remove_key_binding('/', FL_CTRL);
	//m_pEditor->remove_key_binding(FL_Clear, 0);
	m_pEditor->remove_key_binding(FL_Tab, FL_CTRL | FL_SHIFT);
	m_pEditor->remove_key_binding(FL_Tab,             FL_CTRL);
	m_pEditor->remove_key_binding(FL_Right,     FL_CTRL);
	m_pEditor->remove_key_binding(FL_Left,      FL_CTRL);
//	m_pEditor->remove_key_binding();
//	m_pEditor->remove_key_binding();
//	m_pEditor->remove_key_binding();

	//m_pEditor->begin();
	//m_pEditor->end();

	m_pEditor->hide();
	rParent.end();
	rParent.insert(*m_pEditor, rParent.GetContextMenu());

	//rParent.add(m_pEditor);
/*
		rParent.begin();
		m_pContextMenu = new Fl_Menu_Button(0, 46, rParent.w(), rParent.h() - 46);
		m_pContextMenu->copy(&(rParent.GetContextMenuItems()[0]), &rParent);
		m_pContextMenu->textsize(11);
		m_pContextMenu->box(FL_FLAT_BOX);
		m_pContextMenu->down_box(FL_FLAT_BOX);
		m_pContextMenu->color(FL_LIGHT2);
		m_pContextMenu->type(Fl_Menu_Button::POPUP3);
		rParent.end();
*/
	m_pTextBuf->add_modify_callback(CFLTKEditor::CB_StyleUpdate, &rParent);
	m_pTextBuf->add_modify_callback(CFLTKEditor::CB_Changed, &rParent);

	m_bCursorBlinkState = false;
//	Fl::add_timeout(0.5, BlinkCursor, this);
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CFLTKEditor::SEditorData::Destroy()
{
	if (m_pEditor)
	{
		m_pEditor->parent()->remove(m_pEditor);
		delete m_pEditor;
		m_pEditor = 0;
	}

	if (m_pTextBuf)
	{
		delete m_pTextBuf;
		m_pTextBuf = 0;
	}

	if (m_pStyleBuf)
	{
		delete m_pStyleBuf;
		m_pStyleBuf = 0;
	}

	if (m_pmCode)
	{
		delete m_pmCode;
		m_pmCode = 0;
	}

	return true;
}

CFLTKEditor::CFLTKEditor() : Fl_Double_Window(100, 100)
{
	m_pReplaceDlg    = 0;
	m_pReplaceFind   = 0;
	m_pReplaceWith   = 0;
	m_pReplaceAll    = 0;
	m_pReplaceNext   = 0;
	m_pReplaceCancel = 0;
	m_pFileChoice    = 0;
	//m_pEditor = 0;
	//m_pTextBuf = 0;

	//m_bChanged = false;
	m_bLoading = false;
	m_bIsOK    = false;

	m_iCurEditorID  = -1;
	m_iMaxUndoSteps = 50;
	m_bInUndo       = false;
	//m_sPath = "./";

	m_sFileChooserLoadTitle = "Load File";
	m_sFileChooserSaveTitle = "Save File";
	m_sFileChooserPattern   = "All Files:*.*";
}

CFLTKEditor::CFLTKEditor(int iPosX, int iPosY, int iWidth, int iHeight, const char* pcFilename, void* pvIcon)
	: Fl_Double_Window(iWidth, iHeight)
{
	m_pReplaceDlg    = 0;
	m_pReplaceFind   = 0;
	m_pReplaceWith   = 0;
	m_pReplaceAll    = 0;
	m_pReplaceNext   = 0;
	m_pReplaceCancel = 0;
	m_pFileChoice    = 0;
	//m_pEditor = 0;

	//m_bChanged = false;
	m_bLoading = false;
	m_bIsOK    = false;

	m_iCurEditorID  = -1;
	m_iMaxUndoSteps = 10;
	m_bInUndo       = false;

	m_sFileChooserLoadTitle = "Load File";
	m_sFileChooserSaveTitle = "Save File";
	m_sFileChooserPattern   = "All Files:*.*";

	Create(iPosX, iPosY, iWidth, iHeight, pcFilename, pvIcon);
	//Show();
}

CFLTKEditor::~CFLTKEditor(void)
{
	if (m_pReplaceDlg)
	{
		delete m_pReplaceDlg;
	}
}

bool CFLTKEditor::Create(int iPosX, int iPosY, int iWidth, int iHeight, const char* pcFilename, void* pvIcon)
{
	if (m_bIsOK)
	{
		return true;
	}

	icon(pvIcon);
	size(iWidth, iHeight);

	if ((iPosX >= 0) && (iPosY >= 0))
	{
		position(iPosX, iPosY);
	}

	m_pReplaceDlg = new Fl_Window(300, 105, "Replace");
	m_pReplaceDlg->begin();

	m_pReplaceFind = new Fl_Input(80, 10, 210, 25, "Find:");
	m_pReplaceFind->align(FL_ALIGN_LEFT);

	m_pReplaceWith = new Fl_Input(80, 40, 210, 25, "Replace:");
	m_pReplaceWith->align(FL_ALIGN_LEFT);

	m_pReplaceAll = new Fl_Button(10, 70, 90, 25, "Replace All");
	m_pReplaceAll->callback((Fl_Callback*) CB_ReplaceAll, this);

	m_pReplaceNext = new Fl_Return_Button(105, 70, 120, 25, "Replace Next");
	m_pReplaceNext->callback((Fl_Callback*) CB_Replace2, this);

	m_pReplaceCancel = new Fl_Button(230, 70, 60, 25, "Cancel");
	m_pReplaceCancel->callback((Fl_Callback*) CB_ReplaceCan, this);

	m_pReplaceDlg->end();
	m_pReplaceDlg->set_non_modal();

	color(FL_LIGHT2);
	begin();

	InitMenu();
	m_pMenuBar = new Fl_Menu_Bar(0, 0, iWidth, 20);
	m_pMenuBar->copy(&m_vecMenuItem[0], this);
	m_pMenuBar->textsize(11);
	m_pMenuBar->box(FL_FLAT_BOX);
	m_pMenuBar->down_box(FL_FLAT_BOX);
	m_pMenuBar->color(FL_LIGHT2);

	m_pToolBar = new Fl_Pack(0, 23, iWidth - 10, 20);
	m_pToolBar->type(FL_HORIZONTAL);
	m_pToolBar->spacing(5);
	m_pToolBar->color(FL_LIGHT2);
	//m_pToolBar->box(FL_BORDER_FRAME);
	m_pToolBar->begin();

	m_pHSpaceBox = new Fl_Box(FL_NO_BOX, 0, 0, 3, 20, "");

	m_pFileChoice = new Fl_Choice(0, 0, 200, 20);
	m_pFileChoice->box(FL_FLAT_BOX);
	m_pFileChoice->textsize(11);
	m_pFileChoice->callback((Fl_Callback*) CFLTKEditor::CB_SetFile, this);

	m_pToolBar->end();

	m_pContextMenu = new Fl_Menu_Button(0, 46, iWidth, iHeight - 46);
	m_pContextMenu->copy(&m_vecCMenuItem[0], this);
	m_pContextMenu->textsize(11);
	m_pContextMenu->box(FL_FLAT_BOX);
	m_pContextMenu->down_box(FL_FLAT_BOX);
	m_pContextMenu->color(FL_LIGHT2);
	m_pContextMenu->type(Fl_Menu_Button::POPUP3);

	InitStyleTable();

	end();

	callback((Fl_Callback*) CFLTKEditor::CB_Quit, this);

	New();

	if (pcFilename)
	{
		m_bIsOK = LoadFile(pcFilename);
	}
	else
	{
		SetTitle();
		m_bIsOK = true;
	}

	UpdateFileList();

	return m_bIsOK;
}

void CFLTKEditor::Show()
{
	show();
}

void CFLTKEditor::Hide()
{
	hide();
}

void CFLTKEditor::UpdateFileList()
{
	m_pFileChoice->clear();
	for (size_t i = 0; i < m_mEditorData.Count(); i++)
	{
		std::string sValue = m_mEditorData[i].m_sName;
		sValue += " - ";
		sValue += m_mEditorData[i].m_sPath;

		size_t nPos1 = 0;
		while ((nPos1 = sValue.find_first_of("\\", nPos1)) != string::npos)
		{
			sValue.insert(nPos1, "\\", 1);
			nPos1 += 2;
		}

		size_t nPos2 = 0;
		while ((nPos2 = sValue.find_first_of("/", nPos2)) != string::npos)
		{
			sValue[nPos2] = '\\';
			sValue.insert(nPos2, "\\", 1);
			nPos2 += 2;
		}

		m_pFileChoice->add(sValue.c_str());
	}

	m_pFileChoice->value(m_iCurEditorID);
}

int CFLTKEditor::NewEditor()
{
	int iPos = int(m_mEditorData.Count());
	m_mEditorData.Add(1);

	if (!m_mEditorData[iPos].Create(*this))
	{
		return -1;
	}

	return iPos;
}

bool CFLTKEditor::DeleteEditor(int iFileID)
{
	if ((iFileID < 0) || (iFileID >= int(m_mEditorData.Count())))
	{
		return false;
	}

	m_mEditorData.Del(iFileID);

	return true;
}

bool CFLTKEditor::ShowCurEditor()
{
	if ((m_iCurEditorID >= 0) && (m_iCurEditorID < int(m_mEditorData.Count())))
	{
		m_mEditorData[m_iCurEditorID].m_pEditor->show();
		m_mEditorData[m_iCurEditorID].m_pEditor->show_insert_position();
		m_mEditorData[m_iCurEditorID].m_pEditor->take_focus();
		return true;
	}

	return false;
}

bool CFLTKEditor::SetCurEditor(int iFileID)
{
	if ((iFileID < 0) || (iFileID >= int(m_mEditorData.Count())))
	{
		return false;
	}

	if (m_iCurEditorID >= 0)
	{
		m_mEditorData[m_iCurEditorID].m_pEditor->hide();
	}

	#if !defined(CLUVIZ_VIEWER)
		m_mEditorData[iFileID].m_pEditor->show();
		resizable(m_mEditorData[iFileID].m_pEditor);
	#endif

	m_iCurEditorID = iFileID;

	GetTextBuffer()->call_modify_callbacks();
	SetTitle();

	return true;
}

bool CFLTKEditor::SetCurEditor(const char* pcFilename)
{
	if (!pcFilename || (*pcFilename == 0))
	{
		return false;
	}

	int i, iCount = int(m_mEditorData.Count());

	for (i = 0; i < iCount; i++)
	{
		if (m_mEditorData[i].m_sFilename == pcFilename)
		{
			return SetCurEditor(i);
		}
	}

	return false;
}

int CFLTKEditor::GetEditorID(const char* pcPath, const char* pcName)
{
	if (!pcPath || !pcName)
	{
		return -1;
	}

	int i, iCount = int(m_mEditorData.Count());

	for (i = 0; i < iCount; i++)
	{
		if ((m_mEditorData[i].m_sPath == string(pcPath)) &&
			(m_mEditorData[i].m_sName == string(pcName)))
		{
			return i;
		}
	}

	return -1;
}

/////////////////////////////////////////////////////////////////////
// Goto given line and position and highlight it

bool CFLTKEditor::HighlightLinePos(const char* pcFilename, int iInputPos)
{
	if (iInputPos < 0)
	{
		return false;
	}

	int iTextPos, iLineStart, iLineEnd;
	char pcSearchName[300], * pcPoint;
	string sName;

	strncpy_s(pcSearchName, 300, pcFilename, 299);
	if ((pcPoint = strstr(pcSearchName, ".clu")))
	{
		*pcPoint = 0;
	}

//	if (GetFilename().size() == 0)
	sName = GetPath() + GetName();
//	else
//		sName = GetFilename();

	if (pcSearchName != sName)
	{
		int i, iCount = int(m_mEditorData.Count());

		for (i = 0; i < iCount; i++)
		{
			//sName = m_mEditorData[i].m_sFilename;
			//if (sName.size() == 0)
			//{
			sName = m_mEditorData[i].m_sPath + m_mEditorData[i].m_sName;
			//}

			if (sName == pcSearchName)
			{
				SetCurEditor(i);
				m_pFileChoice->value(i);
				break;
			}
		}

		// File not opened yet
		if (i == iCount)
		{
			sName = pcSearchName;
			sName = sName + ".clu";
			CFLTKEditor::New();
			CFLTKEditor::LoadFile(sName.c_str());
			UpdateFileList();
		}
	}

	//iTextPos = GetTextBuffer()->skip_lines(0, iLine-1);
	iTextPos = iInputPos + 1;
	if (iTextPos >= GetTextBuffer()->length())
	{
		iTextPos = GetTextBuffer()->length() - 2;
	}

	iLineStart = GetTextBuffer()->line_start(iTextPos);
	iLineEnd   = GetTextBuffer()->line_end(iTextPos);
	GetTextBuffer()->select(iLineStart, iLineEnd);

	GetEditor()->insert_position(iTextPos);
	GetEditor()->show_insert_position();

	GetTextBuffer()->call_modify_callbacks();

	return true;
}

/////////////////////////////////////////////////////////////////////
/// Init Menu

void CFLTKEditor::InitMenu()
{
	#if !defined(CLUVIZ_VIEWER)
		Fl_Menu_Item pMenuItems[] =
		{
			{ "&File",              0, 0, 0, FL_SUBMENU },
			{ "&New File",        FL_CTRL + 'n', (Fl_Callback*) CB_New },
			{ "&Open File...",    FL_CTRL + 'o', (Fl_Callback*) CB_Load },
			{ "&Insert File...",  0, (Fl_Callback*) CB_Insert },
			{ "&Close File",          FL_CTRL + FL_Delete, (Fl_Callback*) CB_Close, 0, FL_MENU_DIVIDER },
			{ "&Save File",       FL_CTRL + 's', (Fl_Callback*) CB_Save },
			{ "Save File &As...", FL_CTRL + FL_SHIFT + 's', (Fl_Callback*) CB_SaveAs, 0, FL_MENU_DIVIDER },
			{ "Next File",        FL_CTRL + FL_Right, (Fl_Callback*) CB_NextFile },
			{ "Previous File",    FL_CTRL + FL_Left, (Fl_Callback*) CB_PrevFile },
			{ "E&xit", 0, (Fl_Callback*) CB_Quit, 0 },
			{ 0 },

			{ "&Edit", 0, 0, 0, FL_SUBMENU },
			{ "Cu&t",        FL_CTRL + 'x', (Fl_Callback*) CB_Cut },
			{ "&Copy",       FL_CTRL + 'c', (Fl_Callback*) CB_Copy },
			{ "&Paste",      FL_CTRL + 'v', (Fl_Callback*) CB_Paste },
			{ "&Delete",     0, (Fl_Callback*) CB_Delete, 0, FL_MENU_DIVIDER },
			{ "&Undo",       FL_CTRL + 'z', (Fl_Callback*) CB_Undo },
			{ "U&ndo Undo",  FL_CTRL + FL_SHIFT + 'z', (Fl_Callback*) CB_Undo2, 0, FL_MENU_DIVIDER },
			{ "&Indent Selection",  FL_CTRL + 'i', (Fl_Callback*) CB_Indent },
			{ "Un-Inden&t Selection",  FL_CTRL + FL_SHIFT + 'i', (Fl_Callback*) CB_UnIndent },
			{ 0 },

			{ "&Search", 0, 0, 0, FL_SUBMENU },
			{ "&Find...",       FL_CTRL + 'f', (Fl_Callback*) CB_Find },
			{ "F&ind Again",    FL_CTRL + 'g', (Fl_Callback*) CB_Find2 },
			{ "&Replace...",    FL_CTRL + 'h', (Fl_Callback*) CB_Replace },
			{ "Re&place Again", FL_CTRL + 'j', (Fl_Callback*) CB_Replace2, 0, FL_MENU_DIVIDER },
			{ "Goto Line", FL_CTRL + FL_SHIFT + 'g', (Fl_Callback*) CB_GotoLine },
			{ 0 },

			{ 0 }
		};

		Fl_Menu_Item pCMenuItems[] =
		{
			{ "&Open File...",    FL_CTRL + 'o', (Fl_Callback*) CB_Load },
			{ 0 },

			{ 0 }
		};

		m_vecMenuItem.clear();
		m_vecMenuItem.resize(29);

		for (int i = 0; i < 29; i++)
		{
			m_vecMenuItem[i] = pMenuItems[i];
		}

		m_vecCMenuItem.clear();
		m_vecCMenuItem.resize(3);

		for (int i = 0; i < 3; i++)
		{
			m_vecCMenuItem[i] = pCMenuItems[i];
		}

	#else
		Fl_Menu_Item pMenuItems[] =
		{
			{ "&File",              0, 0, 0, FL_SUBMENU },
			{ "E&xit", 0, (Fl_Callback*) CB_Quit, 0 },
			{ 0 },

			{ 0 }
		};

		Fl_Menu_Item pCMenuItems[] =
		{
			{ 0 }
		};

		m_vecMenuItem.clear();
		m_vecMenuItem.resize(4);

		for (int i = 0; i < 4; i++)
		{
			m_vecMenuItem[i] = pMenuItems[i];
		}

		m_vecCMenuItem.clear();
		m_vecCMenuItem.resize(1);

		for (int i = 0; i < 1; i++)
		{
			m_vecCMenuItem[i] = pCMenuItems[i];
		}

	#endif
}

void CFLTKEditor::InitStyleTable()
{
	Fl_Text_Display::Style_Table_Entry pStyleTable[] =
	{	// Style table
		{ FL_BLACK,      FL_COURIER,        14 }// A - Plain
	};

	m_vecStyleTable.clear();
	m_vecStyleTable.resize(1);
	m_vecStyleTable[0] = pStyleTable[0];
}

/////////////////////////////////////////////////////////////////////
/// Helper Functions

/////////////////////////////////////////////////////////////////////
/// Set Name And Path

void CFLTKEditor::SetNameAndPath()
{
	char* pcSlash, pcPath[500];

	strncpy_s(pcPath, 500, GetFilename().c_str(), 499);
	pcSlash = (char*) strrchr(pcPath, '/');

	#ifdef WIN32
		if (pcSlash == 0)
		{
			pcSlash = strrchr(pcPath, '\\');
		}
	#endif

	// Get Name of script without extension
	char* pcDot;
	pcDot = (char*) strrchr(pcPath, '.');
	if (pcDot)
	{
		pcDot[0] = 0;
	}

	if (pcSlash != 0)
	{
		GetName() = &pcSlash[1];

		// Get Path to script
		pcSlash[1] = 0;
		GetPath()  = pcPath;
	}
	else
	{
		GetName() = pcPath;
		GetPath() = "./";
	}
}

bool CFLTKEditor::LoadFile(const char* pcFilename, int iPos, const char* pcPass)
{
	m_bLoading = true;

	bool bInsert = false;

	if (iPos >= 0)
	{
		bInsert = true;
	}

	IsFileChanged() = bInsert;

	int iR;
	if (!bInsert)
	{
		GetFilename() = "";
		iR            = GetTextBuffer()->loadfile(pcFilename);
	}
	else
	{
		iR = GetTextBuffer()->insertfile(pcFilename, iPos);
	}

	if (iR)
	{
		char pcError[500];
		strerror_s(pcError, 500, errno);
		fl_alert("Error reading from file \'%s\':\n%s.", pcFilename, pcError);
	}
	else
	{
		if (!bInsert)
		{
			GetFilename() = pcFilename;
		}

		SetNameAndPath();
	}

	m_bLoading = false;
	GetTextBuffer()->call_modify_callbacks();

	if (iR)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool CFLTKEditor::SaveFile(const char* pcFilename)
{
	if (GetTextBuffer()->savefile(pcFilename))
	{
		char pcError[500];
		strerror_s(pcError, 500, errno);
		fl_alert("Error writing to file \'%s\':\n%s.", pcFilename, pcError);
		return false;
	}
	else
	{
		GetFilename() = pcFilename;
		SetNameAndPath();
	}

	IsFileChanged() = false;
	//SetTitle();
	GetTextBuffer()->call_modify_callbacks();

	return true;
}

bool CFLTKEditor::CheckSave()
{
	if (!IsFileChanged())
	{
		return true;
	}

	string sText;

	sText  = "The file '";
	sText += GetName();
	sText += "' has been modified.\n";
	sText += "Would you like to save it now?";

	int iR = fl_choice(sText.c_str(), "Cancel", "Save", "Discard");

	if (iR == 1)
	{
		Save();	// Save the file...
		return !IsFileChanged();
	}

	return (iR == 2) ? true : false;
}

string CFLTKEditor::GenMetaWindowTitle()
{
	string sTitle;

	sTitle = GetMetaTitle();

	if (GetMetaVersion().size())
	{
		sTitle += " v";
		sTitle += GetMetaVersion();
	}

	if (GetMetaCopyright().size())
	{
		sTitle += "  (c) ";
		sTitle += GetMetaCopyright();
	}

	return sTitle;
}

bool CFLTKEditor::SetTitle()
{
	if (IsFileBinary() && GetMetaTitle().size())
	{
		GetTitle() = GenMetaWindowTitle();
	}
	else
	{
		GetTitle() = "";

		if (GetFilename().length() == 0)
		{
			//		char pcName[20];
			//		sprintf(pcName, "Untitled %d", sm_iNewFileNo-1);
			GetTitle() += GetName();
		}
		else
		{
			char* pcSlash, * pcFilename;

			pcFilename = (char*) GetFilename().c_str();
			pcSlash    = (char*) strrchr(pcFilename, '/');

			#ifdef WIN32
				if (pcSlash == 0)
				{
					pcSlash = strrchr(pcFilename, '\\');
				}
			#endif

			if (pcSlash != 0)
			{
				GetTitle() += &pcSlash[1];
			}
			else
			{
				GetTitle() += pcFilename;
			}
		}

		if (IsFileChanged())
		{
			GetTitle() += " (modified)";
		}

		GetTitle() += " - Editor";
	}

	label(GetTitle().c_str());

	return true;
}

/////////////////////////////////////////////////////////////////////
/// Standard Implementations of virtual functions

void CFLTKEditor::Copy()
{
	Fl_Text_Editor::kf_copy(0, GetEditor());
}

void CFLTKEditor::Cut()
{
	Fl_Text_Editor::kf_cut(0, GetEditor());
}

void CFLTKEditor::Paste()
{
	string sText, sNewText;
	int iInsStart           = 0, iInsEnd = 0;
	Fl_Text_Editor* pEditor = GetEditor();
	Fl_Text_Buffer* pBuffer = pEditor->buffer();

	pBuffer->selection_position(&iInsStart, &iInsEnd);
	if (iInsStart == iInsEnd)
	{
		iInsStart = pEditor->insert_position();
		Fl_Text_Editor::kf_paste(0, pEditor);
		iInsEnd = pEditor->insert_position();
	}
	else
	{
		Fl_Text_Editor::kf_paste(0, pEditor);
		iInsEnd = pEditor->insert_position();
	}

	pBuffer->select(iInsStart, iInsEnd);
	sText = pBuffer->selection_text();

	if (AdaptText(sNewText, sText))
	{
		pBuffer->replace_selection(sNewText.c_str());
	}
	pBuffer->unselect();
}

// Is called when text is pasted into editor with pasted text in sText.
// If text need not be adapted returns false. If it returns true,
// then the text in sNewText is inserted instead of original text.
bool CFLTKEditor::AdaptText(string& sNewText, string& sText)
{
	return false;
}

void CFLTKEditor::Delete()
{
	GetTextBuffer()->remove_selection();
}

void CFLTKEditor::Undo()
{
	TUndoBuf& bufUndo = GetUndoBuf();

	if (bufUndo.size() == 0)
	{
		return;
	}

	m_bInUndo = true;
	SUndoData& Undo = bufUndo.back();

	if (Undo.bInsert)
	{
		// We have to remove inserted data
		GetTextBuffer()->remove(Undo.iPos, Undo.iPos + int(Undo.sText.size()));
		GetEditor()->insert_position(Undo.iPos);
		GetEditor()->show_insert_position();
	}
	else
	{
		// We have to insert deleted data
		GetTextBuffer()->insert(Undo.iPos, Undo.sText.c_str());
		GetEditor()->insert_position(Undo.iPos + int(Undo.sText.size()));
		GetEditor()->show_insert_position();
	}

	GetUndo2Buf().push_back(Undo);
	bufUndo.pop_back();

	m_bInUndo = false;
}

void CFLTKEditor::Undo2()
{
	TUndoBuf& bufUndo = GetUndo2Buf();

	if (bufUndo.size() == 0)
	{
		return;
	}

	m_bInUndo = true;
	SUndoData& Undo = bufUndo.back();

	if (!Undo.bInsert)
	{
		// We have to remove inserted data
		GetTextBuffer()->remove(Undo.iPos, Undo.iPos + int(Undo.sText.size()));
		GetEditor()->insert_position(Undo.iPos);
		GetEditor()->show_insert_position();
	}
	else
	{
		// We have to insert deleted data
		GetTextBuffer()->insert(Undo.iPos, Undo.sText.c_str());
		GetEditor()->insert_position(Undo.iPos + int(Undo.sText.size()));
		GetEditor()->show_insert_position();
	}

	GetUndoBuf().push_back(Undo);
	bufUndo.pop_back();

	m_bInUndo = false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CFLTKEditor::Indent()
{
	Fl_Text_Buffer* pcBuffer = GetTextBuffer();

	// Get selection
	int iSelStart = 0, iSelEnd = 0;
	pcBuffer->selection_position(&iSelStart, &iSelEnd);

	// If nothing is selected, place a tab character in front of the current line
	if ((iSelStart == 0) && (iSelEnd == 0))
	{
		// Insert "\t" at line start of insert position (current cursor position)
		pcBuffer->insert(pcBuffer->line_start(GetEditor()->insert_position()), "\t");
	}
	else
	{
		// Jump to the start of the first selected line
		int iCurPos = pcBuffer->line_start(iSelStart);

		// Iterate over each selected line
		while (iCurPos <= iSelEnd)
		{
			// Add a tab character in front of each line
			pcBuffer->insert(iCurPos, "\t");

			// Jump to the start of the next line
			iCurPos = pcBuffer->skip_lines(iCurPos, 1);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CFLTKEditor::UnIndent()
{
	// Get selection
	int iSelStart = 0, iSelEnd = 0;
	GetTextBuffer()->selection_position(&iSelStart, &iSelEnd);

	// If nothing is selected, remove tab of the current line
	if ((iSelStart == 0) && (iSelEnd == 0))
	{
		_RemoveTabChars(GetEditor()->insert_position());
	}
	else
	{
		// Jump to the start of the first selected line
		int iCurPos = GetTextBuffer()->line_start(iSelEnd);

		// Iterate over each selected line
		while (iCurPos >= GetTextBuffer()->line_start(iSelStart))
		{
			// Remove tab chars
			_RemoveTabChars(iCurPos);

			// Jump to the start of the next line
			int iNewCurPos = GetTextBuffer()->rewind_lines(iCurPos, 1);

			// Test whether there was a following line or not
			if (iCurPos == iNewCurPos)
			{
				break;
			}

			iCurPos = iNewCurPos;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int CFLTKEditor::_RemoveTabChars(int iPosition)
{
	int iTabChars  = 0;
	int iTabSize   = GetTextBuffer()->tab_distance();
	int iLineStart = GetTextBuffer()->line_start(iPosition);
	char* pcLine   = GetTextBuffer()->line_text(iLineStart);

	if (pcLine[0] == '\t')
	{
		// 1 tab char found
		iTabChars = 1;
	}
	else if (pcLine[0] == ' ')
	{
		int i = 1;
		for (; i < iTabSize; i++)
		{
			if (pcLine[i] != ' ')
			{
				break;
			}
		}

		if (i == iTabSize)	// same number of spaces as a tab
		{
			// 'iTabSize' tab chars found
			iTabChars = iTabSize;
		}
	}

	if (iTabChars != 0)
	{
		GetTextBuffer()->remove(iLineStart, iLineStart + iTabChars);
	}

	free(pcLine);
	return iTabChars;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CFLTKEditor::Find()
{
	char* pcVal;

	pcVal = (char*) fl_input("Search String:", GetSearchString());

	if (pcVal != NULL)
	{
		// User entered a string - go find it!
		strcpy_s(GetSearchString(), 256, pcVal);
		Find2();
	}
}

void CFLTKEditor::Find2()
{
	if (GetSearchString()[0] == 0)
	{
		// Search string is blank; get a new one...
		Find();
		return;
	}

	int iPos   = GetEditor()->insert_position();
	int iFound = GetTextBuffer()->search_forward(iPos, GetSearchString(), &iPos);

	if (iFound)
	{
		// Found a match; select and update the position...
		GetTextBuffer()->select(iPos, iPos + (int) strlen(GetSearchString()));
		GetEditor()->insert_position(iPos + (int) strlen(GetSearchString()));
		GetEditor()->show_insert_position();
	}
	else
	{
		fl_alert("No occurrences of \'%s\' found!", GetSearchString());
	}
}

void CFLTKEditor::Replace()
{
	m_pReplaceDlg->show();
}

void CFLTKEditor::Replace2()
{
	char* pcFind    = (char*) m_pReplaceFind->value();
	char* pcReplace = (char*) m_pReplaceWith->value();

	if (pcFind[0] == 0)
	{
		// Search string is blank; get a new one...
		m_pReplaceDlg->show();
		return;
	}

	m_pReplaceDlg->hide();

	int iPos   = GetEditor()->insert_position();
	int iFound = GetTextBuffer()->search_forward(iPos, pcFind, &iPos);

	if (iFound)
	{
		// Found a match; update the position and replace text...
		GetTextBuffer()->select(iPos, iPos + (int) strlen(pcFind));
		GetTextBuffer()->remove_selection();
		GetTextBuffer()->insert(iPos, pcReplace);
		GetTextBuffer()->select(iPos, iPos + (int) strlen(pcReplace));
		GetEditor()->insert_position(iPos + (int) strlen(pcReplace));
		GetEditor()->show_insert_position();
	}
	else
	{
		fl_alert("No occurrences of \'%s\' found!", pcFind);
	}
}

void CFLTKEditor::ReplaceAll()
{
	char* pcFind    = (char*) m_pReplaceFind->value();
	char* pcReplace = (char*) m_pReplaceWith->value();

	if (pcFind[0] == 0)
	{
		// Search string is blank; get a new one...
		m_pReplaceDlg->show();
		return;
	}

	m_pReplaceDlg->hide();

	GetEditor()->insert_position(0);

	// Loop through the whole string
	int iFound;
	int iTimes = 0;

	do
	{
		int iPos = GetEditor()->insert_position();
		iFound = GetTextBuffer()->search_forward(iPos, pcFind, &iPos);

		if (iFound)
		{
			// Found a match; update the position and pcReplace text...
			GetTextBuffer()->select(iPos, iPos + (int) strlen(pcFind));
			GetTextBuffer()->remove_selection();
			GetTextBuffer()->insert(iPos, pcReplace);
			GetEditor()->insert_position(iPos + (int) strlen(pcReplace));
			GetEditor()->show_insert_position();
			iTimes++;
		}
	}
	while (iFound);

	if (iTimes)
	{
		fl_message("Replaced %d occurrences.", iTimes);
	}
	else
	{
		fl_alert("No occurrences of \'%s\' found!", pcFind);
	}
}

void CFLTKEditor::ReplaceCan()
{
	m_pReplaceDlg->hide();
}

void CFLTKEditor::GotoLine()
{
	char* pcVal;
	char pcText[100];
	int iLine, iLineCnt;
	iLineCnt = GetEditor()->buffer()->count_lines(0, GetEditor()->buffer()->length());

	sprintf_s(pcText, 100, "Enter line number (1 - %d):", iLineCnt);
	pcVal = (char*) fl_input(pcText, "");

	if (pcVal != NULL)
	{
		// User entered a string - see whether it's a line number!

		if (sscanf_s(pcVal, "%d", &iLine) != 1)
		{
			fl_alert("Entered value is not a number");
			return;
		}

		if ((iLine < 1) || (iLine > iLineCnt))
		{
			fl_alert("Invalid line number");
			return;
		}

		int iPos = GetEditor()->skip_lines(0, iLine - 1, true);
		GetEditor()->insert_position(iPos);
		GetEditor()->buffer()->select(iPos, GetEditor()->line_end(iPos, true));

		GetEditor()->show_insert_position();
	}
}

void CFLTKEditor::New()
{
/*
		if (!CheckSave())
				return;
*/
	int iEditorID = NewEditor();
	char pcName[20];

	if (iEditorID < 0)
	{
		return;
	}

	SetCurEditor(iEditorID);

	GetFilename() = "";
	sprintf_s(pcName, 20, "Untitled %d", sm_iNewFileNo++);
	GetName() = pcName;
	#ifdef WIN32
		GetPath() = ".\\";
	#else
		GetPath() = "./";
	#endif
	GetTextBuffer()->select(0, GetTextBuffer()->length());
	GetTextBuffer()->remove_selection();
	IsFileChanged() = false;
	GetTextBuffer()->call_modify_callbacks();

	SetTitle();

	UpdateFileList();
}

bool CFLTKEditor::LoadFileReplace(const char* pcFilename, int iPos, const char* pcPass)
{
	Close();
	New();

	bool bRetVal = LoadFile(pcFilename, iPos, pcPass);

	UpdateFileList();

	return bRetVal;
}

bool CFLTKEditor::OpenFileDialog(const string& sTitle, const string& sPattern, string& sFile)
{
	#ifdef WIN32
		// Parse Pattern list for native Win32 dialog box
		int iIdx, iCnt = int(sPattern.length()) + 2;
		char* pcPat = 0, * pcChar = 0;
		char pcFile[1024];
		string sInitPath;

		sInitPath = GetPath();
		if (sInitPath.size() < 2)
		{
			sInitPath = "";
		}
		else if (sInitPath[1] != ':')
		{
			sInitPath = "";
		}

		strncpy_s(pcFile, 1024, sFile.c_str(), 1023);

		pcPat = new char[iCnt];
		if (!pcPat)
		{
			return false;
		}

		strncpy_s(pcPat, iCnt, sPattern.c_str(), iCnt - 2);

		for (iIdx = 0, pcChar = pcPat; iIdx < iCnt - 2; ++iIdx, ++pcChar)
		{
			if ((*pcChar == ':') || (*pcChar == ';'))
			{
				*pcChar = 0;
			}
			else if (*pcChar == ',')
			{
				*pcChar = ';';
			}
		}

		pcChar[0] = 0;
		pcChar[1] = 0;

		// Use nativ Windows dialog box
		OPENFILENAMEA dlgData;

		dlgData.lStructSize       = sizeof(OPENFILENAMEA);
		dlgData.hwndOwner         = NULL;
		dlgData.hInstance         = NULL;
		dlgData.lpstrFilter       = pcPat;
		dlgData.lpstrCustomFilter = NULL;
		dlgData.nMaxCustFilter    = 0;
		dlgData.nFilterIndex      = 1;
		dlgData.lpstrFile         = pcFile;
		dlgData.nMaxFile          = 1020;
		dlgData.lpstrFileTitle    = NULL;
		dlgData.nMaxFileTitle     = 0;
		dlgData.lpstrInitialDir   = (LPCSTR) sInitPath.c_str();
		dlgData.lpstrTitle        = sTitle.c_str();
		dlgData.Flags             = OFN_FILEMUSTEXIST;
		dlgData.nFileOffset       = 0;
		dlgData.nFileExtension    = 0;
		dlgData.lpstrDefExt       = NULL;
		dlgData.lCustData         = 0;
		dlgData.lpfnHook          = NULL;
		dlgData.lpfnHook          = NULL;
		dlgData.lpTemplateName    = NULL;
		dlgData.pvReserved        = 0;
		dlgData.dwReserved        = 0;
		dlgData.FlagsEx           = 0;

		BOOL bResult = GetOpenFileNameA(&dlgData);

		if (bResult)
		{
			sFile = dlgData.lpstrFile;

			// Append selected file type extension if none is entered by the user
			if ((dlgData.nFileExtension == 0) || (dlgData.lpstrFile[dlgData.nFileExtension] == 0))
			{
				if (dlgData.nFilterIndex > 0)
				{
					pcChar = pcPat;
					int iIdx = 1;
					while (iIdx < int(dlgData.nFilterIndex))
					{
						pcChar = &pcChar[strlen(pcChar) + 1];
						pcChar = &pcChar[strlen(pcChar) + 1];
						++iIdx;
					}

					pcChar = &pcChar[strlen(pcChar) + 1];
					pcChar = strchr(pcChar, '.');
					if (pcChar)
					{
						char* pcChar2 = strchr(pcChar, ';');
						if (pcChar2)
						{
							*pcChar2 = 0;
						}
						sFile += pcChar;
					}
				}
			}
		}

		delete pcPat;

		return bResult ? true : false;
	#else

	#error "OpenFileDialog not implemented for other systems but Windows"

		char pcFilename[512];

		pcFilename[0]         = 0;
		Fl_File_Chooser::sort = fl_casenumericsort;
		char* pcNewFile = fl_file_chooser(m_sFileChooserLoadTitle.c_str(),
			m_sFileChooserPattern.c_str(), pcFilename);
	#endif
}

bool CFLTKEditor::SaveFileDialog(const string& sTitle, const string& sPattern, string& sFile)
{
	#ifdef WIN32
		// Parse Pattern list for native Win32 dialog box
		int iIdx, iCnt = int(sPattern.length()) + 2;
		char* pcPat = 0, * pcChar = 0;
		char pcFile[1024];

		strncpy_s(pcFile, 1024, sFile.c_str(), 1023);

		pcPat = new char[iCnt];
		if (!pcPat)
		{
			return false;
		}

		strncpy_s(pcPat, iCnt, sPattern.c_str(), iCnt - 2);

		for (iIdx = 0, pcChar = pcPat; iIdx < iCnt - 2; ++iIdx, ++pcChar)
		{
			if ((*pcChar == ':') || (*pcChar == ';'))
			{
				*pcChar = 0;
			}
			else if (*pcChar == ',')
			{
				*pcChar = ';';
			}
		}

		pcChar[0] = 0;
		pcChar[1] = 0;

		// Use nativ Windows dialog box
		OPENFILENAMEA dlgData;

		dlgData.lStructSize       = sizeof(OPENFILENAMEA);
		dlgData.hwndOwner         = NULL;
		dlgData.hInstance         = NULL;
		dlgData.lpstrFilter       = pcPat;
		dlgData.lpstrCustomFilter = NULL;
		dlgData.nMaxCustFilter    = 0;
		dlgData.nFilterIndex      = 1;
		dlgData.lpstrFile         = pcFile;
		dlgData.nMaxFile          = 1020;
		dlgData.lpstrFileTitle    = NULL;
		dlgData.nMaxFileTitle     = 0;
		dlgData.lpstrInitialDir   = NULL;
		dlgData.lpstrTitle        = (LPCSTR) sTitle.c_str();
		dlgData.Flags             = OFN_OVERWRITEPROMPT;
		dlgData.nFileOffset       = 0;
		dlgData.nFileExtension    = 0;
		dlgData.lpstrDefExt       = NULL;
		dlgData.lCustData         = 0;
		dlgData.lpfnHook          = NULL;
		dlgData.lpfnHook          = NULL;
		dlgData.lpTemplateName    = NULL;
		dlgData.pvReserved        = 0;
		dlgData.dwReserved        = 0;
		dlgData.FlagsEx           = 0;

		BOOL bResult = GetSaveFileNameA(&dlgData);

		if (bResult)
		{
			sFile = dlgData.lpstrFile;

			// Append selected file type extension if none is entered by the user
			if ((dlgData.nFileExtension == 0) || (dlgData.lpstrFile[dlgData.nFileExtension] == 0))
			{
				if (dlgData.nFilterIndex > 0)
				{
					pcChar = pcPat;
					int iIdx = 1;
					while (iIdx < int(dlgData.nFilterIndex))
					{
						pcChar = &pcChar[strlen(pcChar) + 1];
						pcChar = &pcChar[strlen(pcChar) + 1];
						++iIdx;
					}

					pcChar = &pcChar[strlen(pcChar) + 1];
					pcChar = strchr(pcChar, '.');
					if (pcChar)
					{
						char* pcChar2 = strchr(pcChar, ';');
						if (pcChar2)
						{
							*pcChar2 = 0;
						}
						sFile += pcChar;
					}
				}
			}
		}

		delete pcPat;

		return bResult ? true : false;
	#else

	#error "OpenFileDialog not implemented for other systems but Windows"

		char pcFilename[512];

		pcFilename[0]         = 0;
		Fl_File_Chooser::sort = fl_casenumericsort;
		char* pcNewFile = fl_file_chooser(m_sFileChooserLoadTitle.c_str(),
			m_sFileChooserPattern.c_str(), pcFilename);
	#endif
}

void CFLTKEditor::Load()
{
/*
		if (!CheckSave())
				return;
*/
	string sFile;

	if (OpenFileDialog(m_sFileChooserLoadTitle, m_sFileChooserPattern, sFile))
	{
		int i, iCount = int(m_mEditorData.Count());
		// Check whether file has already been loaded
		for (i = 0; i < iCount; i++)
		{
			if (sFile == m_mEditorData[i].m_sFilename)
			{
				// if file is found, select it
				SetCurEditor(i);
				break;
			}
		}

		// if file has not been loaded then do load it.
		if (i == iCount)
		{
			New();
			LoadFile(sFile.c_str());
		}

		UpdateFileList();
	}
}

void CFLTKEditor::Insert()
{
	//char pcFilename[512];
	//Fl_File_Chooser::sort = fl_casenumericsort;
	//char *pcNewFile = fl_file_chooser(m_sFileChooserLoadTitle.c_str(),
	//						m_sFileChooserPattern.c_str(), pcFilename);

	//if (pcNewFile != NULL)

	string sFile;

	if (OpenFileDialog(m_sFileChooserLoadTitle, m_sFileChooserPattern, sFile))
	{
		LoadFile(sFile.c_str(), GetEditor()->insert_position());
	}
}

void CFLTKEditor::Close()
{
	if (!CheckSave())
	{
		return;
	}

	if ((m_iCurEditorID < 0) || (m_iCurEditorID >= int(m_mEditorData.Count())))
	{
		return;
	}

	DeleteEditor(m_iCurEditorID);

	if (m_iCurEditorID > 0)
	{
		m_iCurEditorID--;
		SetCurEditor(m_iCurEditorID);
	}
	else if (m_iCurEditorID >= int(m_mEditorData.Count()))
	{
		char pcName[100];

		NewEditor();
		m_iCurEditorID = 0;
		SetCurEditor(m_iCurEditorID);

		GetFilename() = "";
		sprintf_s(pcName, 100, "Untitled %d", sm_iNewFileNo++);
		GetName() = pcName;
		GetPath() = "./";
		GetTextBuffer()->select(0, GetTextBuffer()->length());
		GetTextBuffer()->remove_selection();
		IsFileChanged() = false;
		GetTextBuffer()->call_modify_callbacks();

		SetTitle();
	}
	else
	{
		SetCurEditor(m_iCurEditorID);
	}

	UpdateFileList();
}

void CFLTKEditor::Save()
{
	if (GetFilename().length() == 0)
	{
		SaveAs();
		return;
	}

	SaveFile(GetFilename().c_str());
}

void CFLTKEditor::SaveAs()
{
	//char *pcNewFile;
	//char pcFilename[512];
	string sNewFile;

	//pcFilename[0] = 0;
	//Fl_File_Chooser::sort = fl_casenumericsort;
	//pcNewFile = fl_file_chooser(m_sFileChooserSaveTitle.c_str(),
	//	m_sFileChooserPattern.c_str(), pcFilename);

	//if (pcNewFile != NULL)
	if (SaveFileDialog(m_sFileChooserSaveTitle, m_sFileChooserPattern, sNewFile))
	{
		//sNewFile = pcNewFile;
		if (*fl_filename_ext(sNewFile.c_str()) == 0)
		{
			sNewFile = sNewFile + ".clu";
		}

		// Check whether the name of current editor file is the same as
		// save as file, or whether new name is the same as that of a
		// file that is currently open.
		if (sNewFile != GetFilename())
		{
			int i, iCount = int(m_mEditorData.Count());
			for (i = 0; i < iCount; i++)
			{
				if (sNewFile == m_mEditorData[i].m_sFilename)
				{
					fl_alert("File with chosen name is already open in editor");
					return;
				}
			}
		}

		SaveFile(sNewFile.c_str());
		UpdateFileList();
	}
}

void CFLTKEditor::NextFile()
{
	if (m_iCurEditorID + 1 < int(m_mEditorData.Count()))
	{
		SetCurEditor(m_iCurEditorID + 1);
		m_pFileChoice->value(m_iCurEditorID);
		ShowCurEditor();
	}
}

void CFLTKEditor::PrevFile()
{
	if (m_iCurEditorID > 0)
	{
		SetCurEditor(m_iCurEditorID - 1);
		m_pFileChoice->value(m_iCurEditorID);
		ShowCurEditor();
	}
}

void CFLTKEditor::SetFile()
{
	int iIdx = m_pFileChoice->value();

	if ((iIdx >= 0) && (iIdx < int(m_mEditorData.Count())))
	{
		SetCurEditor(iIdx);
		ShowCurEditor();
	}
}

bool CFLTKEditor::PreQuit()
{
	bool bSaveAll = false;
	int i, iCount = int(m_mEditorData.Count());

	for (i = 0; i < iCount; i++)
	{
		SetCurEditor(i);

		if (!bSaveAll && IsFileChanged())
		{
			string sText;

			sText  = "The file '";
			sText += GetName();
			sText += "' has been modified.\n";
			sText += "Would you like to save it now?";

			int iR = fl_choice(sText.c_str(), "Discard", "Save", "Save all");

			if (iR == 1)
			{
				Save();	// Save the file...
			}
			else if (iR == 2)
			{
				Save();
				bSaveAll = true;
			}
		}
		else if (bSaveAll)
		{
			Save();
		}
	}

	return true;
}

void CFLTKEditor::Quit()
{
	if (CFLTKEditor::PreQuit())
	{
		exit(0);
	}
}

void CFLTKEditor::Changed(int iVal, int nInserted, int nDeleted, int iVal2, const char* pcVal)
{
	if ((nInserted || nDeleted) && !m_bLoading)
	{
		IsFileChanged() = true;

		if (!m_bInUndo)
		{
			bool bNewUndo   = true;
			char* pcEndChar = " \n";

			GetUndo2Buf().clear();

			if (nInserted > 0)
			{
				TUndoBuf& bufUndo = GetUndoBuf();
				char* pcText      = GetTextBuffer()->text_range(iVal, iVal + nInserted);
				int iTextSize     = int(strlen(pcText));

				if (bufUndo.size() > 0)
				{
					SUndoData& PrevUndo = bufUndo.back();
					int iUndoTextSize   = int(PrevUndo.sText.size());
					char cLastChar      = PrevUndo.sText.at(iUndoTextSize - 1);

					if ((iVal == PrevUndo.iPos + iUndoTextSize) &&
						PrevUndo.bInsert &&
						(iTextSize == 1) &&
						(strchr(pcEndChar, cLastChar) == 0))
					{
						PrevUndo.sText += pcText;
						bNewUndo        = false;
					}
				}

				if (bNewUndo)
				{
					int iUndoSize = int(bufUndo.size());
					if (iUndoSize == m_iMaxUndoSteps)
					{
						bufUndo.erase(bufUndo.begin());
					}

					SUndoData Undo;

					Undo.iPos    = iVal;
					Undo.bInsert = true;
					Undo.sText   = pcText;

					bufUndo.push_back(Undo);
				}

				if (pcText)
				{
					free(pcText);
				}
			}
			else if (nDeleted > 0)
			{
				TUndoBuf& bufUndo  = GetUndoBuf();
				const char* pcText = pcVal;
				int iTextSize      = int(strlen(pcText));

				if (bufUndo.size() > 0)
				{
					SUndoData& PrevUndo = bufUndo.back();
					char cFirstChar     = PrevUndo.sText.at(0);

					if ((iVal == PrevUndo.iPos - 1) &&
						!PrevUndo.bInsert &&
						(iTextSize == 1) &&
						(strchr(pcEndChar, cFirstChar) == 0))
					{
						PrevUndo.sText.insert(0, pcText);
						PrevUndo.iPos--;
						bNewUndo = false;
					}
				}

				if (bNewUndo)
				{
					int iUndoSize = int(bufUndo.size());
					if (iUndoSize == m_iMaxUndoSteps)
					{
						bufUndo.erase(bufUndo.begin());
					}

					SUndoData Undo;
					Undo.iPos    = iVal;
					Undo.bInsert = false;
					Undo.sText   = pcText;

					bufUndo.push_back(Undo);
				}
			}
		}
	}

	SetTitle();

	if (m_bLoading)
	{
		GetEditor()->show_insert_position();
	}
}

void CFLTKEditor::StyleUnfinished(int iVal)
{
}

void CFLTKEditor::StyleParse(const char* pcText, char* pcStyle, int iLength)
{
}

void CFLTKEditor::StyleUpdate(int iPos, int nInserted,  int nDeleted, int nRestyled, const char* deletedText)
{
	int     iStart,			// Start of text
		iEnd;			// End of text

	char    cLast,			// Last style on line
	* pcStyle,			// Style data
	* pcText;			// Text data

	// If this is just a selection change, just unselect the style buffer...
	if ((nInserted == 0) && (nDeleted == 0))
	{
		GetStyleBuffer()->unselect();
		return;
	}

	// Track changes in the text buffer...
	if (nInserted > 0)
	{
		// Insert characters into the style buffer...
		pcStyle = new char[nInserted + 1];
		memset(pcStyle, 'A', nInserted);
		pcStyle[nInserted] = '\0';

		GetStyleBuffer()->replace(iPos, iPos + nDeleted	/*nInserted*/, pcStyle);
		delete[] pcStyle;
	}
	else
	{
		// Just delete characters in the style buffer...
		GetStyleBuffer()->remove(iPos, iPos + nDeleted);
	}

	// Select the area that was just updated to avoid unnecessary
	// callbacks...
	if (iPos > 0)
	{
		iPos--;
	}

	GetStyleBuffer()->select(iPos, iPos + nInserted - nDeleted + 1);

	// Re-parse the changed region; we do this by parsing from the
	// beginning of the line of the changed region to the end of
	// the line of the changed region...  Then we check the last
	// style character and keep updating if we have a multi-line
	// comment character...
	iStart = GetTextBuffer()->line_start(iPos);
	iEnd   = GetTextBuffer()->line_end(iPos + nInserted + 1);
	if (iEnd + 1 < GetTextBuffer()->length())
	{
		iEnd++;
	}

	pcText  = GetTextBuffer()->text_range(iStart, iEnd);
	pcStyle = GetStyleBuffer()->text_range(iStart, iEnd);
	cLast   = pcStyle[iEnd - iStart - 1];

	//  printf("iStart = %d, iEnd = %d, pcText = \"%s\", pcStyle = \"%s\"...\n",
	//         iStart, iEnd, pcText, pcStyle);

	m_bReparsing  = false;
	m_bReparseAll = false;
	StyleParse(pcText, pcStyle, iEnd - iStart);

	//  printf("new pcStyle = \"%s\"...\n", pcStyle);

	GetStyleBuffer()->replace(iStart, iEnd, pcStyle);
	GetEditor()->redisplay_range(iStart, iEnd);

	if (m_bReparseAll)	//cLast != pcStyle[iEnd - iStart - 1] || pcStyle[iEnd - iStart - 1] == 'Z')
	{
		// The cLast character on the line changed styles, so reparse the
		// remainder of the buffer...
		free(pcText);
		free(pcStyle);

		iStart  = 0;
		iEnd    = GetTextBuffer()->length();
		pcText  = GetTextBuffer()->text_range(iStart, iEnd);
		pcStyle = GetStyleBuffer()->text_range(iStart, iEnd);

		m_bReparsing = true;
		StyleParse(pcText, pcStyle, iEnd - iStart);

		GetStyleBuffer()->replace(iStart, iEnd, pcStyle);
		GetEditor()->redisplay_range(iStart, iEnd);
	}

	free(pcText);
	free(pcStyle);
}

/////////////////////////////////////////////////////////////////////
/// Callbacks

void CFLTKEditor::CB_Copy(Fl_Widget* pWidget, void* pData)
{
	if (pData)
	{
		((CFLTKEditor*) pData)->Copy();
	}
}

void CFLTKEditor::CB_Cut(Fl_Widget* pWidget, void* pData)
{
	if (pData)
	{
		((CFLTKEditor*) pData)->Cut();
	}
}

void CFLTKEditor::CB_Delete(Fl_Widget* pWidget, void* pData)
{
	if (pData)
	{
		((CFLTKEditor*) pData)->Delete();
	}
}

void CFLTKEditor::CB_Paste(Fl_Widget* pWidget, void* pData)
{
	if (pData)
	{
		((CFLTKEditor*) pData)->Paste();
	}
}

void CFLTKEditor::CB_Undo(Fl_Widget* pWidget, void* pData)
{
	if (pData)
	{
		((CFLTKEditor*) pData)->Undo();
	}
}

void CFLTKEditor::CB_Undo2(Fl_Widget* pWidget, void* pData)
{
	if (pData)
	{
		((CFLTKEditor*) pData)->Undo2();
	}
}

void CFLTKEditor::CB_Indent(Fl_Widget* pWidget, void* pData)
{
	if (pData)
	{
		((CFLTKEditor*) pData)->Indent();
	}
}

void CFLTKEditor::CB_UnIndent(Fl_Widget* pWidget, void* pData)
{
	if (pData)
	{
		((CFLTKEditor*) pData)->UnIndent();
	}
}

void CFLTKEditor::CB_Find(Fl_Widget* pWidget, void* pData)
{
	if (pData)
	{
		((CFLTKEditor*) pData)->Find();
	}
}

void CFLTKEditor::CB_Find2(Fl_Widget* pWidget, void* pData)
{
	if (pData)
	{
		((CFLTKEditor*) pData)->Find2();
	}
}

void CFLTKEditor::CB_Replace(Fl_Widget* pWidget, void* pData)
{
	if (pData)
	{
		((CFLTKEditor*) pData)->Replace();
	}
}

void CFLTKEditor::CB_Replace2(Fl_Widget* pWidget, void* pData)
{
	if (pData)
	{
		((CFLTKEditor*) pData)->Replace2();
	}
}

void CFLTKEditor::CB_ReplaceAll(Fl_Widget* pWidget, void* pData)
{
	if (pData)
	{
		((CFLTKEditor*) pData)->ReplaceAll();
	}
}

void CFLTKEditor::CB_ReplaceCan(Fl_Widget* pWidget, void* pData)
{
	if (pData)
	{
		((CFLTKEditor*) pData)->ReplaceCan();
	}
}

void CFLTKEditor::CB_GotoLine(Fl_Widget* pWidget, void* pData)
{
	if (pData)
	{
		((CFLTKEditor*) pData)->GotoLine();
	}
}

void CFLTKEditor::CB_New(Fl_Widget* pWidget, void* pData)
{
	if (pData)
	{
		((CFLTKEditor*) pData)->New();
	}
}

void CFLTKEditor::CB_Load(Fl_Widget* pWidget, void* pData)
{
	if (pData)
	{
		((CFLTKEditor*) pData)->Load();
	}
}

void CFLTKEditor::CB_Insert(Fl_Widget* pWidget, void* pData)
{
	if (pData)
	{
		((CFLTKEditor*) pData)->Insert();
	}
}

void CFLTKEditor::CB_Close(Fl_Widget* pWidget, void* pData)
{
	if (pData)
	{
		((CFLTKEditor*) pData)->Close();
	}
}

void CFLTKEditor::CB_Save(Fl_Widget* pWidget, void* pData)
{
	if (pData)
	{
		((CFLTKEditor*) pData)->Save();
	}
}

void CFLTKEditor::CB_SaveAs(Fl_Widget* pWidget, void* pData)
{
	if (pData)
	{
		((CFLTKEditor*) pData)->SaveAs();
	}
}

void CFLTKEditor::CB_NextFile(Fl_Widget* pWidget, void* pData)
{
	if (pData)
	{
		((CFLTKEditor*) pData)->NextFile();
	}
}

void CFLTKEditor::CB_PrevFile(Fl_Widget* pWidget, void* pData)
{
	if (pData)
	{
		((CFLTKEditor*) pData)->PrevFile();
	}
}

void CFLTKEditor::CB_SetFile(Fl_Widget* pWidget, void* pData)
{
	if (pData)
	{
		((CFLTKEditor*) pData)->SetFile();
	}
}

void CFLTKEditor::CB_Quit(Fl_Widget* pWidget, void* pData)
{
	if (pData)
	{
		((CFLTKEditor*) pData)->Quit();
	}
}

/*
void CFLTKEditor::CB_View(Fl_Widget* pWidget, void *pData)
{
		if (pData)
				((CFLTKEditor *) pData)->View();
}
*/

void CFLTKEditor::CB_Changed(int iVal, int nInserted, int nDeleted, int iVal2, const char* pcVal, void* pvVal)
{
	if (pvVal)
	{
		((CFLTKEditor*) pvVal)->Changed(iVal, nInserted, nDeleted, iVal2, pcVal);
	}
}

void CFLTKEditor::CB_StyleUnfinished(int iVal, void* pvVal)
{
	if (pvVal)
	{
		((CFLTKEditor*) pvVal)->StyleUnfinished(iVal);
	}
}

void CFLTKEditor::CB_StyleUpdate(int iPos, int nInserted,       int nDeleted, int nRestyled, const char* pcDeletedText, void* pvData)
{
	if (pvData)
	{
		((CFLTKEditor*) pvData)->StyleUpdate(iPos, nInserted, nDeleted, nRestyled, pcDeletedText);
	}
}
