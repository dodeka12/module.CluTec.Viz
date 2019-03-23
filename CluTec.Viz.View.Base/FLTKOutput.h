////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.View.Base
// file:      FLTKOutput.h
//
// summary:   Declares the fltk output class
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

#if !defined(_FLTKOUTPUT_H__INCLUDED_)
	#define _FLTKOUTPUT_H__INCLUDED_

#include "CluTec.Viz.Fltk\Fl.H"
#include "CluTec.Viz.Fltk\Fl_Group.H"
#include "CluTec.Viz.Fltk\Fl_Double_Window.H"
#include "CluTec.Viz.Fltk\fl_ask.H"
#include "CluTec.Viz.Fltk\Fl_File_Chooser.H"
#include "CluTec.Viz.Fltk\Fl_Menu_Bar.H"
#include "CluTec.Viz.Fltk\Fl_Input.H"
#include "CluTec.Viz.Fltk\Fl_Button.H"
#include "CluTec.Viz.Fltk\Fl_Return_Button.H"
#include "CluTec.Viz.Fltk\Fl_Text_Buffer.H"
#include "CluTec.Viz.Fltk\Fl_Text_Editor.H"
#include "CluTec.Viz.Fltk\Fl_Help_View.H"

#include <string>
#include <vector>

	using namespace std;

	class CFLTKOutput : public Fl_Double_Window
	{
	public:

		CFLTKOutput();
		CFLTKOutput(int iPosX, int iPosY, int iWidth, int iHeight, const char* pcTitle = 0, void* pvIcon = 0);
		~CFLTKOutput(void);

		virtual bool Create(int iPosX, int iPosY, int iWidth, int iHeight, const char* pcTitle = 0, void* pvIcon = 0);
		virtual void SetPosSize(int iPosX, int iPosY, int iWidth, int iHeight);
		void Show();

		void ClearText();

		virtual bool SetText(const char* pcText);
		virtual void Quit();
		virtual bool PreQuit() { return true; }

	protected:

		// Virtual Functions that influence Editor behaviour

		// Override this function to create your own menus
		virtual void InitMenu();

	protected:

//	virtual void Copy();

		virtual const char* InfoLink(Fl_Widget* pWidget, const char* pcURL);

	protected:

		static void CB_Quit(Fl_Widget* pWidget, void* pData);
//	static void CB_Copy(Fl_Widget* pWidget, void *pData);
		static const char* CB_InfoLink(Fl_Widget* pWidget, const char* pcURL, void* pvData);

	protected:

		bool m_bIsOK;
		bool m_bHasMenu;
		string m_sTitle;

		Fl_Help_View* m_pWinOutput;
//	Fl_Text_Editor *m_pWinOutput;
//	Fl_Text_Buffer *m_pTextBuf;

		Fl_Menu_Bar* m_pMenuBar;
		vector<Fl_Menu_Item> m_vecMenuItem;
	};

#endif	// #define _FLTKEDITOR_H__INCLUDED_
