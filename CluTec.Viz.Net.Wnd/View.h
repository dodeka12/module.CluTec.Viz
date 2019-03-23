////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Net.Wnd
// file:      View.h
//
// summary:   Declares the view class
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

#pragma once


namespace Clu
{
	namespace Viz
	{
		namespace Net
		{
			namespace Wnd
			{
				using namespace System;

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				/// <summary>
				/// 	A standalone CluViz view shown in its own window.
				/// </summary>
				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				public ref class View
				{
				public:

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Default constructor.
					/// </summary>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					View();

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Destructor.
					/// </summary>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					~View();

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Finalizer.
					/// </summary>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					!View();

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Gets a value indicating whether this view has been created.
					/// </summary>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					property bool IsValid
					{
						bool get();
					}

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Gets the last error.
					/// </summary>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					property String^ LastError
					{
						String^ get()
						{
							return m_sLastError;
						}
					}

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Gets or sets the access timeout. If an operation has to wait longer than this value, an exception is thrown.
					/// </summary>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					property int AccessTimeout;

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Loads an icon file and returns the handle to the loaded icon. This handle can be passed to Create to show a window icon.
					/// </summary>
					///
					/// <param name="sFilename"> [in] The file name. </param>
					/// <param name="bModule">   True if the given file is a module and not an icon file. </param>
					///
					/// <returns> The icon handle. </returns>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					unsigned __int64 GetIconHandle(String^ sFilename, bool bModule);

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Creates this view.
					/// </summary>
					///
					/// <param name="iX">		  The X position of the window on the screen. </param>
					/// <param name="iY">		  The Y position of the window on the screen. </param>
					/// <param name="iWidth">	  The width of the window. </param>
					/// <param name="iHeight">    The height of the window. </param>
					/// <param name="sTitle">	  [in] The window title. </param>
					/// <param name="bResizable"> The window has a sizing border. </param>
					/// <param name="bCaption">   The window has a title bar. </param>
					/// <param name="bSysMenu">   The window has a window menu on its title bar. The bCaption must also be true. </param>
					/// <param name="bTopMost">   The window should be placed above all non-topmost windows and should stay above them, even when the
					/// 						  window is deactivated. </param>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					void Create(int iX, int iY, int iWidth, int iHeight, String^ sTitle, bool bResizable, bool bCaption, bool bSysMenu, bool bTopMost);

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Creates this view.
					/// </summary>
					///
					/// <param name="iX">		   The X position of the window on the screen. </param>
					/// <param name="iY">		   The Y position of the window on the screen. </param>
					/// <param name="iWidth">	   The width of the window. </param>
					/// <param name="iHeight">	   The height of the window. </param>
					/// <param name="sTitle">	   [in] The window title. </param>
					/// <param name="uIconHandle"> Handle of the icon. </param>
					/// <param name="bResizable">  The window has a sizing border. </param>
					/// <param name="bCaption">    The window has a title bar. </param>
					/// <param name="bSysMenu">    The window has a window menu on its title bar. The bCaption must also be true. </param>
					/// <param name="bTopMost">    The window should be placed above all non-topmost windows and should stay above them, even when
					/// 						   the window is deactivated. </param>
					/// <param name="bTool">	   True if the window is a tool window. The window is intended to be used as a floating toolbar. A
					/// 						   tool window has a title bar that is shorter than a normal title bar, and the window title is
					/// 						   drawn using a smaller font. A tool window does not appear in the task bar or in the dialog that
					/// 						   appears when the user presses ALT+TAB. If a tool window has a system menu, its icon is not
					/// 						   displayed on the title bar. However, you can display the system menu by right-clicking or by
					/// 						   typing ALT+SPACE. </param>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					void Create(int iX, int iY, int iWidth, int iHeight, String^ sTitle, unsigned __int64 uIconHandle, bool bResizable,
						bool bCaption, bool bSysMenu, bool bTopMost, bool bTool);

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Creates this view as debug.
					/// </summary>
					///
					/// <param name="iX">		  The X position of the window on the screen. </param>
					/// <param name="iY">		  The Y position of the window on the screen. </param>
					/// <param name="iWidth">	  The width of the window. </param>
					/// <param name="iHeight">    The height of the window. </param>
					/// <param name="sTitle">	  [in] The window title. </param>
					/// <param name="bResizable"> The window has a sizing border. </param>
					/// <param name="bCaption">   The window has a title bar. </param>
					/// <param name="bSysMenu">   The window has a window menu on its title bar. The bCaption must also be true. </param>
					/// <param name="bTopMost">   The window should be placed above all non-topmost windows and should stay above them, even when the
					/// 						  window is deactivated. </param>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					void CreateDebug(int iX, int iY, int iWidth, int iHeight, String^ sTitle, bool bResizable, bool bCaption, bool bSysMenu, bool bTopMost);

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Creates this view as debug.
					/// </summary>
					///
					/// <param name="iX">		   The X position of the window on the screen. </param>
					/// <param name="iY">		   The Y position of the window on the screen. </param>
					/// <param name="iWidth">	   The width of the window. </param>
					/// <param name="iHeight">	   The height of the window. </param>
					/// <param name="sTitle">	   [in] The window title. </param>
					/// <param name="uIconHandle"> Handle of the icon. </param>
					/// <param name="bResizable">  The window has a sizing border. </param>
					/// <param name="bCaption">    The window has a title bar. </param>
					/// <param name="bSysMenu">    The window has a window menu on its title bar. The bCaption must also be true. </param>
					/// <param name="bTopMost">    The window should be placed above all non-topmost windows and should stay above them, even when
					/// 						   the window is deactivated. </param>
					/// <param name="bTool">	   True if the window is a tool window. The window is intended to be used as a floating toolbar. A
					/// 						   tool window has a title bar that is shorter than a normal title bar, and the window title is
					/// 						   drawn using a smaller font. A tool window does not appear in the task bar or in the dialog that
					/// 						   appears when the user presses ALT+TAB. If a tool window has a system menu, its icon is not
					/// 						   displayed on the title bar. However, you can display the system menu by right-clicking or by
					/// 						   typing ALT+SPACE. </param>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					void CreateDebug(int iX, int iY, int iWidth, int iHeight, String^ sTitle, unsigned __int64 uIconHandle, bool bResizable,
						bool bCaption, bool bSysMenu, bool bTopMost, bool bTool);

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Destroys and closes this view.
					/// </summary>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					void Destroy();

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Shows this view.
					/// </summary>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					void Show();

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Hides this view.
					/// </summary>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					void Hide();

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Sets the position and the size of this view.
					/// </summary>
					///
					/// <param name="iX">	   The X position of the window on the screen. </param>
					/// <param name="iY">	   The Y position of the window on the screen. </param>
					/// <param name="iWidth">  The width of the window. </param>
					/// <param name="iHeight"> The height of the window. </param>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					void SetPosSize(int iX, int iY, int iWidth, int iHeight);

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Enables or disables full screen mode.
					/// </summary>
					///
					/// <param name="bEnable">			 True to enable, false to disable. </param>
					/// <param name="bEnableUserToggle"> True to enable disabling of full screen via ESC key. </param>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					void FullScreen(bool bEnable, bool bEnableUserToggle);

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Clears the script and loads a default script.
					/// </summary>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					void ClearScript();

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Sets the script as a string.
					/// </summary>
					///
					/// <param name="sScript"> [in] The script content. </param>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					void SetScript(String^ sScript);

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Loads a script.
					/// </summary>
					///
					/// <param name="sScriptFile"> [in] The script file. </param>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					void LoadScript(String^ sScriptFile);

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Loads a script.
					/// </summary>
					///
					/// <param name="sScriptFile"> [in] The script file. </param>
					/// <param name="sPass">	   [in] The password of the script (if encrypted). </param>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					void LoadScript(String^ sScriptFile, String^ sPass);

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Loads a script.
					/// </summary>
					///
					/// <param name="sScriptFile"> [in] The script file. </param>
					/// <param name="sPass">	   [in] The password of the script (if encrypted). </param>
					/// <param name="asArgs">	   [in] Array of arguments passed to the script. Can be null. </param>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					void LoadScript(String^ sScriptFile, String^ sPass, array<String^>^ asArgs);

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Wait for variable to become "true".
					/// </summary>
					///
					/// <param name="sVarName"> [in] The name of the variable. </param>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					void WaitForVarTrue(String^ sVarName);

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Executes the script operation.
					/// </summary>
					///
					/// <param name="sMessage"> [in] The message to execute. </param>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					void ExecScript(String^ sMessage);

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Gets the value of variable.
					/// </summary>
					///
					/// <param name="sVarName"> [in] The name of the variable. </param>
					/// <param name="iValue">   [out] The value. </param>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					void GetVarNumber(String^ sVarName, int % iValue);

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Gets the value of variable.
					/// </summary>
					///
					/// <param name="sVarName"> [in] The name of the variable. </param>
					/// <param name="dValue">   [out] The value. </param>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					void GetVarNumber(String^ sVarName, double % dValue);

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Gets the value of variable.
					/// </summary>
					///
					/// <param name="sVarName"> [in] The name of the variable. </param>
					/// <param name="sValue">   [out] The value. </param>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					void GetVarString(String^ sVarName, String^ % sValue);

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Gets the value of variable.
					/// </summary>
					///
					/// <param name="sVarName"> [in] The name of the variable. </param>
					/// <param name="colValue"> [out] The value. </param>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					void GetVarColor(String^ sVarName, Drawing::Color^ % colValue);

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Gets the value of variable.
					/// </summary>
					///
					/// <param name="sVarName"> [in] The name of the variable. </param>
					/// <param name="aiDim">    [out] The dimension of the tensor. </param>
					/// <param name="adValue">  [out] The tensor values. </param>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					void GetVarTensor(String^ sVarName, array<int>^ % aiDim, array<double>^ % adValue);

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Gets the value of variable.
					/// </summary>
					///
					/// <param name="sVarName"> [in] The name of the variable. </param>
					/// <param name="imgValue"> [out] The value. </param>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					void GetVarImage(String^ sVarName, Clu::Net::CluImage^ imgValue);

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Sets the value of a variable.
					/// </summary>
					///
					/// <param name="sVarName"> [in] The name of the variable. </param>
					/// <param name="iValue">   [in] The value. </param>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					void SetVarNumber(String^ sVarName, int iValue);

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Sets the value of a variable.
					/// </summary>
					///
					/// <param name="sVarName"> [in] The name of the variable. </param>
					/// <param name="dValue">   [in] The value. </param>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					void SetVarNumber(String^ sVarName, double dValue);

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Sets the value of a variable.
					/// </summary>
					///
					/// <param name="sVarName"> [in] The name of the variable. </param>
					/// <param name="sValue">   [in] The value. </param>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					void SetVarString(String^ sVarName, String^ sValue);

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Sets the value of a variable.
					/// </summary>
					///
					/// <param name="sVarName"> [in] The name of the variable. </param>
					/// <param name="colValue"> [in] The value. </param>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					void SetVarColor(String^ sVarName, Drawing::Color^ colValue);

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Sets the value of a variable.
					/// </summary>
					///
					/// <param name="sVarName"> [in] The name of the variable. </param>
					/// <param name="aiDim">    [in] The dimension of the tensor. </param>
					/// <param name="adData">   [in] The values of the tensor. </param>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					void SetVarTensor(String^ sVarName, array<int>^ aiDim, array<double>^ adData);

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Sets the value of a variable.
					/// </summary>
					///
					/// <param name="sVarName"> [in] The name of the variable. </param>
					/// <param name="imgValue"> [in] The value. </param>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					void SetVarImage(String^ sVarName, Clu::Net::CluImage^ imgValue);

				protected:

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Sets the last error.
					/// </summary>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					System::String^ _FormatAndSetLastError(System::String^ sText);

				protected:

					/// <summary> The internal handle. </summary>
					int m_iHandle;
					/// <summary> The internal lock. </summary>
					System::Threading::Mutex^ m_Lock;

					System::String^ m_sLastError;
				};
			} // namespace Wnd
		} // namespace Net
	} // namespace Viz
} // namespace Clu
