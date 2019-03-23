////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.View.Base
// file:      CLUOutput.h
//
// summary:   Declares the clu output class
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

#if !defined(_CLUOUTPUT_H__INCLUDED_)
	#define _CLUOUTPUT_H__INCLUDED_

#include "OGLWin.h"
#include "CLUScriptEditor.h"
#include "FLTKOutput.h"

	class CCLUOutput : public CFLTKOutput
	{
	public:

		CCLUOutput();
		CCLUOutput(int iPosX, int iPosY, int iWidth, int iHeight, const char* pcTitle = 0, void* pvIcon = 0);
		~CCLUOutput(void);

		void SetOGLWin(COGLWin* pWin)
		{ m_poglWin = pWin; }

		void SetEditorWin(CCLUScriptEditor* pWin)
		{ m_pEditor = pWin; }

		void SetOutputObjectList(const CCLUCodeBase::TOutObjList& rvecOutObj);

		virtual void Quit();

		void Clear();

	protected:

		void Update();
		bool GenHTML(CStrMem& sText, bool bGenLinks = true, bool bGenFontCmd = true);

		// Virtual Functions that influence Editor behaviour

		// Override this function to create your own menus
		void InitMenu();
		const char* InfoLink(Fl_Widget* pWidget, const char* pcURL);

	protected:

		static void CB_ShowEditorWin(Fl_Widget* pWidget, void* pvData);
		static void CB_ShowOutputWin(Fl_Widget* pWidget, void* pvData);
		static void CB_ShowVisWin(Fl_Widget* pWidget, void* pvData);

		static void CB_ShowVarType(Fl_Widget* pWidget, void* pvData);
		static void CB_SimpleText(Fl_Widget* pWidget, void* pvData);
		static void CB_Copy(Fl_Widget* pWidget, void* pData);

		void MenuShowEditorWin();
		void MenuShowOutputWin();
		void MenuShowVisWin();

		void ShowVarType();
		void SimpleText();
		void Copy();

	protected:

		COGLWin* m_poglWin;
		CCLUScriptEditor* m_pEditor;
		CCLUCodeBase::TOutObjList m_vecOutObj;

		int m_iFontSize;
		bool m_bShowType;
		bool m_bSimpleText;
	};

#endif	// #define _FLTKEDITOR_H__INCLUDED_
