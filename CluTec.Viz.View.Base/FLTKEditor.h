////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.View.Base
// file:      FLTKEditor.h
//
// summary:   Declares the fltk editor class
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

#if !defined(_FLTKEDITOR_H__INCLUDED_)
	#define _FLTKEDITOR_H__INCLUDED_

#include "CluTec.Viz.Fltk\Fl.H"
#include "CluTec.Viz.Fltk\Fl_Group.H"
#include "CluTec.Viz.Fltk\Fl_Double_Window.H"
#include "CluTec.Viz.Fltk\fl_ask.H"
#include "CluTec.Viz.Fltk\Fl_File_Chooser.H"
#include "CluTec.Viz.Fltk\Fl_Menu_Bar.H"
#include "CluTec.Viz.Fltk\Fl_Input.H"
#include "CluTec.Viz.Fltk\Fl_Button.H"
#include "CluTec.Viz.Fltk\Fl_Pack.H"
#include "CluTec.Viz.Fltk\Fl_Return_Button.H"
#include "CluTec.Viz.Fltk\Fl_Text_Buffer.H"
#include "CluTec.Viz.Fltk\Fl_Text_Editor.H"

#include "CluTec.Viz.Base\mem.h"
#include "CluTec.Viz.Base\memobj.h"

#include <string>
#include <vector>
#include <list>

	using namespace std;

	class CFLTKEditor : public Fl_Double_Window
	{
	public:

		struct SUndoData
		{
			int iPos;		// Position of Change
			bool bInsert;	// true: inserted text, false: deleted text
			string sText;	// text which was inserted or deleted
		};

		typedef list<SUndoData> TUndoBuf;

		struct SEditorData
		{
			SEditorData();
			~SEditorData();
			bool Create(CFLTKEditor& rParent);
			bool Destroy();

			bool m_bCursorBlinkState;

			bool                            m_bChanged;

			// If true editor text does not contain CLUScript that
			// can be parsed, but m_mCode contains pre-parsed code.
			bool                            m_bIsBinary;

			char                            m_pcSearchString[256];

			string                          m_sFilename;
			string                          m_sName;
			string                          m_sPath;
			string                          m_sTitle;

			// Meta Data
			string                          m_sMetaTitle;
			string                          m_sMetaVersion;
			string                          m_sMetaCopyright;
			string                          m_sMetaAbout;

			TUndoBuf                        m_bufUndo;	// Undo Buffer
			TUndoBuf                        m_bufUndo2;	// Undo Undo Buffer

			Mem<char>* m_pmCode;
			Fl_Text_Buffer* m_pTextBuf;
			Fl_Text_Buffer* m_pStyleBuf;
			Fl_Text_Editor* m_pEditor;
		};

	public:

		CFLTKEditor();
		CFLTKEditor(int iPosX, int iPosY, int iWidth, int iHeight, const char* pcFilename = 0, void* pvIcon = 0);
		~CFLTKEditor(void);

		virtual bool Create(int iPosX, int iPosY, int iWidth, int iHeight, const char* pcFilename = 0, void* pvIcon = 0);
		void Show();
		void Hide();

		// Helper Functions
		virtual bool LoadFile(const char* pcFilename, int iPos = -1, const char* pcPass = 0);
		virtual bool SaveFile(const char* pcFilename);
		virtual bool LoadFileReplace(const char* pcFilename, int iPos = -1, const char* pcPass = 0);

		// Goto given position and highlight line.
		bool HighlightLinePos(const char* pcFilename, int iInputPos);

		vector<Fl_Text_Display::Style_Table_Entry>& GetStyleTable() { return m_vecStyleTable; }

		void UpdateFileList();
		bool SetCurEditor(const char* pcFilename);

//	vector<Fl_Menu_Item>& GetContextMenuItems() { return m_vecCMenuItem; }
		Fl_Menu_Button* GetContextMenu() { return m_pContextMenu; }

		bool IsOK() { return m_bIsOK; }
		virtual string GenMetaWindowTitle();
		bool& IsFileBinary() { return m_mEditorData[m_iCurEditorID].m_bIsBinary; }

		// Quit Editor window
		virtual void Quit();
		// Called before Quit is executed
		virtual bool PreQuit();

	protected:

		bool CheckSave();
		bool SetTitle();
		void SetNameAndPath();

	protected:

		// Virtual Functions that influence Editor behaviour

		// Override this function to create your own menus
		virtual void InitMenu();

		// Override this function to add your own styles
		virtual void InitStyleTable();

	protected:

		// Virtual Functions called by Callbacks
		virtual void Copy();
		virtual void Cut();
		virtual void Delete();
		virtual void Paste();
		virtual void Undo();
		virtual void Undo2();
		virtual void Indent();
		virtual void UnIndent();
		int _RemoveTabChars(int iPosition);

		virtual void Find();
		virtual void Find2();

		virtual void Replace();
		virtual void Replace2();
		virtual void ReplaceAll();
		virtual void ReplaceCan();

		virtual void GotoLine();

		virtual void New();
		virtual void Load();
		virtual void Insert();
		virtual void Close();
		virtual void Save();
		virtual void SaveAs();
		virtual void NextFile();
		virtual void PrevFile();
		virtual void SetFile();

		virtual void Changed(int iVal, int nInserted, int nDeleted, int iVal2, const char* pcVal);

		virtual void StyleUnfinished(int iVal);
		virtual void StyleUpdate(int iPos, int nInserted,       int nDeleted, int nRestyled, const char* pcDeletedText);

		virtual void StyleParse(const char* pcText, char* pcStyle, int iLength);

		// Is called when text is pasted into editor with pasted text in sText.
		// If text need not be adapted returns false. If it returns true,
		// then the text in sNewText is inserted instead of original text.
		virtual bool AdaptText(string& sNewText, string& sText);

	protected:

		// Callbacks
		static void CB_Copy(Fl_Widget* pWidget, void* pData);
		static void CB_Cut(Fl_Widget* pWidget, void* pData);
		static void CB_Delete(Fl_Widget* pWidget, void* pData);
		static void CB_Paste(Fl_Widget* pWidget, void* pData);
		static void CB_Undo(Fl_Widget* pWidget, void* pData);
		static void CB_Undo2(Fl_Widget* pWidget, void* pData);
		static void CB_Indent(Fl_Widget* pWidget, void* pData);
		static void CB_UnIndent(Fl_Widget* pWidget, void* pData);

		static void CB_Find(Fl_Widget* pWidget, void* pData);
		static void CB_Find2(Fl_Widget* pWidget, void* pData);

		static void CB_Replace(Fl_Widget* pWidget, void* pData);
		static void CB_Replace2(Fl_Widget* pWidget, void* pData);
		static void CB_ReplaceAll(Fl_Widget* pWidget, void* pData);
		static void CB_ReplaceCan(Fl_Widget* pWidget, void* pData);

		static void CB_GotoLine(Fl_Widget* pWidget, void* pData);

		static void CB_New(Fl_Widget* pWidget, void* pData);
		static void CB_Load(Fl_Widget* pWidget, void* pData);
		static void CB_Insert(Fl_Widget* pWidget, void* pData);
		static void CB_Close(Fl_Widget* pWidget, void* pData);
		static void CB_Save(Fl_Widget* pWidget, void* pData);
		static void CB_SaveAs(Fl_Widget* pWidget, void* pData);
		static void CB_NextFile(Fl_Widget* pWidget, void* pData);
		static void CB_PrevFile(Fl_Widget* pWidget, void* pData);
		static void CB_SetFile(Fl_Widget* pWidget, void* pData);

		static void CB_Quit(Fl_Widget* pWidget, void* pData);

		static void CB_Changed(int iVal, int nInserted, int nDeleted, int iVal2, const char* pcVal, void* pvVal);

		static void CB_StyleUnfinished(int iVal, void* pvVal);
		static void CB_StyleUpdate(int iPos, int nInserted,     int nDeleted, int nRestyled, const char* deletedText, void* pvData);

		bool OpenFileDialog(const string& sTitle, const string& sPattern, string& sFile);
		bool SaveFileDialog(const string& sTitle, const string& sPattern, string& sFile);

		bool& IsFileChanged() { return m_mEditorData[m_iCurEditorID].m_bChanged; }
		//bool& IsFileBinary() { return m_mEditorData[m_iCurEditorID].m_bIsBinary; }
		char* GetSearchString() { return m_mEditorData[m_iCurEditorID].m_pcSearchString; }
		string& GetFilename() { return m_mEditorData[m_iCurEditorID].m_sFilename; }
		string& GetName() { return m_mEditorData[m_iCurEditorID].m_sName; }
		string& GetPath() { return m_mEditorData[m_iCurEditorID].m_sPath; }
		string& GetTitle() { return m_mEditorData[m_iCurEditorID].m_sTitle; }

		string& GetMetaTitle() { return m_mEditorData[m_iCurEditorID].m_sMetaTitle; }
		string& GetMetaVersion() { return m_mEditorData[m_iCurEditorID].m_sMetaVersion; }
		string& GetMetaCopyright() { return m_mEditorData[m_iCurEditorID].m_sMetaCopyright; }
		string& GetMetaAbout() { return m_mEditorData[m_iCurEditorID].m_sMetaAbout; }

		TUndoBuf& GetUndoBuf() { return m_mEditorData[m_iCurEditorID].m_bufUndo; }
		TUndoBuf& GetUndo2Buf() { return m_mEditorData[m_iCurEditorID].m_bufUndo2; }
		Mem<char>* GetBinaryCode() { return m_mEditorData[m_iCurEditorID].m_pmCode; }
		Fl_Text_Buffer* GetTextBuffer() { return m_mEditorData[m_iCurEditorID].m_pTextBuf; }
		Fl_Text_Buffer* GetStyleBuffer() { return m_mEditorData[m_iCurEditorID].m_pStyleBuf; }
		Fl_Text_Editor* GetEditor() { return m_mEditorData[m_iCurEditorID].m_pEditor; }

		// Creates New Editor
		// Returns -1 if unsuccessful, otherwise the id number of the new editor
		int NewEditor();
		bool DeleteEditor(int iFileID);
		bool SetCurEditor(int iFileID);
		bool ShowCurEditor();
		int GetEditorID(const char* pcPath, const char* pcName);

	protected:

		Fl_Window* m_pReplaceDlg;
		Fl_Input* m_pReplaceFind;
		Fl_Input* m_pReplaceWith;
		Fl_Button* m_pReplaceAll;
		Fl_Return_Button* m_pReplaceNext;
		Fl_Button* m_pReplaceCancel;
		Fl_Choice* m_pFileChoice;
		Fl_Pack* m_pToolBar;
		Fl_Box* m_pHSpaceBox;

		bool                       m_bLoading;
		bool                            m_bIsOK;
		bool                            m_bReparsing;
		bool                            m_bReparseAll;

/*
        bool               m_bChanged;
        char				m_pcSearchString[256];
        string             m_sFilename;
        string			   m_sName;
        string			   m_sPath;
        string             m_sTitle;
        Fl_Text_Buffer     *m_pTextBuf;
        Fl_Text_Editor     *m_pEditor;
        Fl_Text_Buffer     *m_pStyleBuf;
*/

		int m_iCurEditorID;
		MemObj<SEditorData> m_mEditorData;
		static int sm_iNewFileNo;

		int m_iMaxUndoSteps;
		bool m_bInUndo;	// Is set true while undoing changes.

		vector<Fl_Text_Display::Style_Table_Entry> m_vecStyleTable;

		Fl_Menu_Bar* m_pMenuBar;
		Fl_Menu_Button* m_pContextMenu;
		vector<Fl_Menu_Item> m_vecMenuItem;
		vector<Fl_Menu_Item> m_vecCMenuItem;

		string m_sFileChooserLoadTitle;
		string m_sFileChooserSaveTitle;
		string m_sFileChooserPattern;

		string m_sTitle;
	};

#endif	// #define _FLTKEDITOR_H__INCLUDED_
