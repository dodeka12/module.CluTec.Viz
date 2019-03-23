////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Net.Wnd
// file:      View.cpp
//
// summary:   Implements the view class
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

#include "Defines.h"
#include "View.h"

#pragma managed(push, off)
#include "CluTec.Viz.Wnd/Clu.Viz.Wnd.h"
#include "CluTec.Base/Exception.h"
#pragma managed(pop)


namespace Clu
{
	namespace Viz
	{
		namespace Net
		{
			namespace Wnd
			{

				View::View()
				{
					m_Lock = gcnew System::Threading::Mutex(true);

					AccessTimeout = 1000;
					m_sLastError = "";

					m_iHandle = 0;

					m_Lock->ReleaseMutex();
				}

				View::~View()
				{
					if (m_iHandle > 0)
					{
						this->!View();
					}
				}

				View::!View()
				{
					if (m_iHandle > 0)
					{
						Destroy();
					}
				}

				///////////////////////////////////////////////////////////////////////
				// Check whether view is created

				bool View::IsValid::get()
				{
					if (!m_Lock->WaitOne(AccessTimeout, false))
					{
						CLU_THROW_MANAGED("No signal received");
					}

					if (m_iHandle <= 0)
					{
						return false;
					}

					bool bIsCreated = Clu::Viz::Wnd::IsViewValid(m_iHandle);

					m_Lock->ReleaseMutex();

					return bIsCreated;
				}

				///////////////////////////////////////////////////////////////////////
				/// Set Last Error from CLUViz Lib. to property variable

				System::String^ View::_FormatAndSetLastError(System::String^ sText)
				{
					if (!m_Lock->WaitOne(AccessTimeout, false))
					{
						CLU_THROW_MANAGED("No signal received");
					}

					char pcError[1001];
					Clu::Viz::Wnd::GetLastError(pcError, 1000);
					m_sLastError = gcnew String((const char*)pcError);

					m_Lock->ReleaseMutex();

					return sText + " >> " + m_sLastError;
				}

				///////////////////////////////////////////////////////////////////////
				/// Load an Icon

				unsigned __int64 View::GetIconHandle(String^ sFilename, bool bModule)
				{
					try
					{
						return (unsigned __int64)Clu::Viz::Wnd::GetIconHandle(Clu::Net::Converter::ToIString(sFilename).ToCString(), bModule);
					}
					CLU_CATCH_RETHROW_ALL_MANAGED("Error getting Icon Handle")
				}

				///////////////////////////////////////////////////////////////////////
				/// Create CLUViz View

				void View::Create(int iX, int iY, int iWidth, int iHeight, String^ sTitle, bool bResizable, bool bCaption, bool bSysMenu, bool bTopMost)
				{
					try
					{
						if (!m_Lock->WaitOne(AccessTimeout, false))
						{
							CLU_THROW_MANAGED("No signal received");
						}

						if (m_iHandle > 0)
						{
							CLU_THROW_MANAGED( "Already bound to handle");
						}

						int iHandle;
						unsigned int uWndStyle = CLUVIZ_WNDSTYLE_NONE;

						if (bResizable)
						{
							uWndStyle |= CLUVIZ_WNDSTYLE_SIZEBORDER;
						}

						if (bCaption)
						{
							uWndStyle |= CLUVIZ_WNDSTYLE_CAPTION;
						}

						if (bSysMenu)
						{
							uWndStyle |= CLUVIZ_WNDSTYLE_SYSMENU;
						}

						if (bTopMost)
						{
							uWndStyle |= CLUVIZ_WNDSTYLE_TOPMOST;
						}

						Clu::Viz::Wnd::CreateEx1(iHandle, iX, iY, iWidth, iHeight, Clu::Net::Converter::ToIString(sTitle).ToCString(), uWndStyle);

						m_iHandle = iHandle;

						ClearScript();

					}
					CLU_CATCH_RETHROW_ALL_LE_VIEW_MANAGED("Error creating CluViz View")
					finally
					{
						m_Lock->ReleaseMutex(); 
					}
				}

				///////////////////////////////////////////////////////////////////////
				/// Create CLUViz  Debug View

				void View::CreateDebug(int iX, int iY, int iWidth, int iHeight, String^ sTitle, bool bResizable, bool bCaption, bool bSysMenu, bool bTopMost)
				{
					try
					{
						if (!m_Lock->WaitOne(AccessTimeout, false))
						{
							CLU_THROW_MANAGED("No signal received");
						}

						if (m_iHandle > 0)
						{
							CLU_THROW_MANAGED( "Already bound to handle");
						}

						int iHandle;
						unsigned int uWndStyle = CLUVIZ_WNDSTYLE_NONE;

						if (bResizable)
						{
							uWndStyle |= CLUVIZ_WNDSTYLE_SIZEBORDER;
						}

						if (bCaption)
						{
							uWndStyle |= CLUVIZ_WNDSTYLE_CAPTION;
						}

						if (bSysMenu)
						{
							uWndStyle |= CLUVIZ_WNDSTYLE_SYSMENU;
						}

						if (bTopMost)
						{
							uWndStyle |= CLUVIZ_WNDSTYLE_TOPMOST;
						}

						Clu::Viz::Wnd::CreateEx2(iHandle, iX, iY, iWidth, iHeight, Clu::Net::Converter::ToIString(sTitle).ToCString(), 0, 0, false, true, 0, 0, uWndStyle);

						m_iHandle = iHandle;

						ClearScript();

					}
					CLU_CATCH_RETHROW_ALL_LE_VIEW_MANAGED("Error creating CluViz Debug View")
					finally
					{
						m_Lock->ReleaseMutex();
					}
				}

				///////////////////////////////////////////////////////////////////////
				/// Create CLUViz View with extended options

				void View::Create(int iX, int iY, int iWidth, int iHeight, String^ sTitle, unsigned __int64 uIconHandle, bool bResizable, bool bCaption, bool bSysMenu, bool bTopMost, bool bTool)
				{
					try
					{
						if (!m_Lock->WaitOne(AccessTimeout, false))
						{
							CLU_THROW_MANAGED("No signal received");
						}

						if (m_iHandle > 0)
						{
							CLU_THROW_MANAGED( "Already bound to handle");
						}

						int iHandle;
						unsigned int uWndStyle = CLUVIZ_WNDSTYLE_NONE;

						if (bResizable)
						{
							uWndStyle |= CLUVIZ_WNDSTYLE_SIZEBORDER;
						}

						if (bCaption)
						{
							uWndStyle |= CLUVIZ_WNDSTYLE_CAPTION;
						}

						if (bSysMenu)
						{
							uWndStyle |= CLUVIZ_WNDSTYLE_SYSMENU;
						}

						if (bTopMost)
						{
							uWndStyle |= CLUVIZ_WNDSTYLE_TOPMOST;
						}

						if (bTool)
						{
							uWndStyle |= CLUVIZ_WNDSTYLE_TOOL;
						}

						Clu::Viz::Wnd::CreateEx2(iHandle, iX, iY, iWidth, iHeight, Clu::Net::Converter::ToIString(sTitle).ToCString(), 0, 0, true, true,
							(void*)uIconHandle, 0, uWndStyle);

						m_iHandle = iHandle;

						ClearScript();

					}
					CLU_CATCH_RETHROW_ALL_LE_VIEW_MANAGED("Error creating CLUViz View with extended options")
					finally
					{
						m_Lock->ReleaseMutex();
					}
				}

				///////////////////////////////////////////////////////////////////////
				/// Create CLUViz Debug View

				void View::CreateDebug(int iX, int iY, int iWidth, int iHeight, String^ sTitle, unsigned __int64 uIconHandle, bool bResizable, bool bCaption, bool bSysMenu, bool bTopMost, bool bTool)
				{
					try
					{
						if (!m_Lock->WaitOne(AccessTimeout, false))
						{
							CLU_THROW_MANAGED("No signal received");
						}

						if (m_iHandle > 0)
						{
							CLU_THROW_MANAGED( "Already bound to handle");
						}

						int iHandle;
						unsigned int uWndStyle = CLUVIZ_WNDSTYLE_NONE;

						if (bResizable)
						{
							uWndStyle |= CLUVIZ_WNDSTYLE_SIZEBORDER;
						}

						if (bCaption)
						{
							uWndStyle |= CLUVIZ_WNDSTYLE_CAPTION;
						}

						if (bSysMenu)
						{
							uWndStyle |= CLUVIZ_WNDSTYLE_SYSMENU;
						}

						if (bTopMost)
						{
							uWndStyle |= CLUVIZ_WNDSTYLE_TOPMOST;
						}

						if (bTool)
						{
							uWndStyle |= CLUVIZ_WNDSTYLE_TOOL;
						}

						Clu::Viz::Wnd::CreateEx2(iHandle, iX, iY, iWidth, iHeight, Clu::Net::Converter::ToIString(sTitle).ToCString(), 0, 0, false, true,
							(void*)uIconHandle, 0, uWndStyle);
						m_iHandle = iHandle;

						ClearScript();

					}
					CLU_CATCH_RETHROW_ALL_LE_VIEW_MANAGED("Error creating CLUViz Debug View")
					finally
					{
						m_Lock->ReleaseMutex();
					}
				}

				///////////////////////////////////////////////////////////////////////
				/// Destroy CLUViz view

				void View::Destroy()
				{
					try
					{
						if (!m_Lock->WaitOne(AccessTimeout, false))
						{
							CLU_THROW_MANAGED("No signal received");
						}

						if (m_iHandle > 0)
						{
							Clu::Viz::Wnd::Destroy(int(m_iHandle));
						}

						m_iHandle = 0;

					}
					CLU_CATCH_RETHROW_ALL_LE_VIEW_MANAGED("Error destroying CLUViz view")
					finally
					{
						m_Lock->ReleaseMutex();
					}
				}

				///////////////////////////////////////////////////////////////////////
				// Show Window

				void View::Show()
				{
					try
					{
						if (!m_Lock->WaitOne(AccessTimeout, false))
						{
							CLU_THROW_MANAGED("No signal received");
						}

						if (m_iHandle == 0)
						{
							CLU_THROW_MANAGED( "Handle is invalid");
						}

						Clu::Viz::Wnd::ShowWindow(m_iHandle);
					}
					CLU_CATCH_RETHROW_ALL_LE_VIEW_MANAGED("Error showing window")
					finally
					{
						m_Lock->ReleaseMutex();
					}
				}

				///////////////////////////////////////////////////////////////////////
				// Hide Window

				void View::Hide()
				{
					try
					{
						if (!m_Lock->WaitOne(AccessTimeout, false))
						{
							CLU_THROW_MANAGED("No signal received");
						}

						if (m_iHandle == 0)
						{
							CLU_THROW_MANAGED( "Handle is invalid");
						}

						Clu::Viz::Wnd::HideWindow(m_iHandle);

					}
					CLU_CATCH_RETHROW_ALL_LE_VIEW_MANAGED("Error hiding view")
					finally
					{
						m_Lock->ReleaseMutex();
					}
				}

				///////////////////////////////////////////////////////////////////////
				// Set Position and Size of Window

				void View::SetPosSize(int iX, int iY, int iWidth, int iHeight)
				{
					try
					{
						if (!m_Lock->WaitOne(AccessTimeout, false))
						{
							CLU_THROW_MANAGED("No signal received");
						}

						if (m_iHandle == 0)
						{
							CLU_THROW_MANAGED( "Handle is invalid");
						}

						Clu::Viz::Wnd::SetWindowPosSize(m_iHandle, iX, iY, iWidth, iHeight);

					}
					CLU_CATCH_RETHROW_ALL_LE_VIEW_MANAGED("Error hiding view")
					finally
					{
						m_Lock->ReleaseMutex();
					}
				}

				///////////////////////////////////////////////////////////////////////
				// Enable/Disable Fullscreen view

				void View::FullScreen(bool bEnable, bool bEnableUserToggle)
				{
					try
					{
						if (!m_Lock->WaitOne(AccessTimeout, false))
						{
							CLU_THROW_MANAGED("No signal received");
						}

						if (m_iHandle == 0)
						{
							CLU_THROW_MANAGED( "Handle is invalid");
						}

						Clu::Viz::Wnd::FullScreen(m_iHandle, bEnable, bEnableUserToggle);

					}
					CLU_CATCH_RETHROW_ALL_LE_VIEW_MANAGED("Error enabling/disabling fullscreen view")
					finally
					{
						m_Lock->ReleaseMutex();
					}
				}

				///////////////////////////////////////////////////////////////////////
				/// Clear Script

				void View::ClearScript()
				{
					try
					{
						if (!m_Lock->WaitOne(AccessTimeout, false))
						{
							CLU_THROW_MANAGED("No signal received");
						}

						if (m_iHandle == 0)
						{
							CLU_THROW_MANAGED( "Handle is invalid");
						}

						Clu::Viz::Wnd::SetScript(m_iHandle,
							"EnableMenu( true ); EnableStdCtrl( true ); SetImgPos(0,0,0); SetImgAlign(0.5,0.5); "
							"SetTextMagStep(16); :GetTextImg( \"CLUViz\" );");

					}
					CLU_CATCH_RETHROW_ALL_LE_VIEW_MANAGED("Error clearing script")
						finally
					{
						m_Lock->ReleaseMutex();
					}
				}

				///////////////////////////////////////////////////////////////////////
				// Set Script

				void View::SetScript(String^ sScript)
				{
					try
					{
						if (!m_Lock->WaitOne(AccessTimeout, false))
						{
							CLU_THROW_MANAGED("No signal received");
						}

						if (m_iHandle == 0)
						{
							CLU_THROW_MANAGED( "Current handle is invalid");
						}

						Clu::Viz::Wnd::SetScript(int(m_iHandle), Clu::Net::Converter::ToIString(sScript).ToCString());
					}
					CLU_CATCH_RETHROW_ALL_LE_VIEW_MANAGED("Error setting script")
						finally
					{
						m_Lock->ReleaseMutex();
					}
				}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				void View::LoadScript(String^ sScriptFile)
				{
					LoadScript(sScriptFile, "", nullptr);
				}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				void View::LoadScript(String^ sScriptFile, String^ sPass)
				{
					LoadScript(sScriptFile, sPass, nullptr);
				}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				void View::LoadScript(String^ sScriptFile, String^ sPass, array<String^>^ asArgs)
				{
					try
					{
						if (!m_Lock->WaitOne(AccessTimeout, false))
						{
							CLU_THROW_MANAGED("No signal received");
						}

						if (m_iHandle == 0)
						{
							CLU_THROW_MANAGED( "Current handle is invalid");
						}

						int iArgCnt = 0;
						std::string strScriptFile, strArgs;
						std::string strPass = Clu::Net::Converter::ToIString(sPass).ToCString();

						if (asArgs != nullptr)
						{
							iArgCnt = asArgs->Length;
							for (int iIdx = 0; iIdx < iArgCnt; ++iIdx)
							{
								strArgs += Clu::Net::Converter::ToIString(asArgs[iIdx]).ToCString();
								strArgs += "\r";
							}
						}

						const char* pcPass = 0;
						if (strPass.length() == 8)
						{
							pcPass = strPass.c_str();
						}

						Clu::Viz::Wnd::LoadScript(int(m_iHandle), Clu::Net::Converter::ToIString(sScriptFile).ToCString(), pcPass, iArgCnt, strArgs.c_str());
					}
					CLU_CATCH_RETHROW_ALL_LE_VIEW_MANAGED("Error loading script")
						finally
					{
						m_Lock->ReleaseMutex();
					}
				}

				///////////////////////////////////////////////////////////////////////
				// Wait for variable to become "true"

				void View::WaitForVarTrue(String^ sVarName)
				{
					try
					{
						if (!m_Lock->WaitOne(AccessTimeout, false))
						{
							CLU_THROW_MANAGED("No signal received");
						}

						if (m_iHandle == 0)
						{
							CLU_THROW_MANAGED( "Current handle is invalid");
						}

						while (true)
						{
							double dVal;
							Clu::Viz::Wnd::GetVarNumber(m_iHandle, Clu::Net::Converter::ToIString(sVarName).ToCString(), dVal);

							if (dVal > 1e-4)
							{
								break;
							}

							System::Threading::Thread::Sleep(100);
						}

					}
					CLU_CATCH_RETHROW_ALL_LE_VIEW_MANAGED("Error waiting for variable")
						finally
					{
						m_Lock->ReleaseMutex();
					}
				}

				///////////////////////////////////////////////////////////////////////
				/// Execute Script with given "ToolName"

				void View::ExecScript(String^ sMessage)
				{
					try
					{
						if (!m_Lock->WaitOne(AccessTimeout, false))
						{
							CLU_THROW_MANAGED("No signal received");
						}

						if (m_iHandle == 0)
						{
							CLU_THROW_MANAGED( "Current handle is invalid");
						}

						Clu::Viz::Wnd::ExecUser(m_iHandle, Clu::Net::Converter::ToIString(sMessage).ToCString());
					}
					CLU_CATCH_RETHROW_ALL_LE_VIEW_MANAGED("Error executing script")
						finally
					{
						m_Lock->ReleaseMutex();
					}
				}

				///////////////////////////////////////////////////////////////////////
				/// Get number variable (int)

				void View::GetVarNumber(String^ sVarName, int % iValue)
				{
					try
					{
						if (!m_Lock->WaitOne(AccessTimeout, false))
						{
							CLU_THROW_MANAGED("No signal received");
						}

						if (m_iHandle == 0)
						{
							CLU_THROW_MANAGED( "Current handle is invalid");
						}

						int iVal;
						Clu::Viz::Wnd::GetVarNumber(m_iHandle, Clu::Net::Converter::ToIString(sVarName).ToCString(), iVal);

						iValue = iVal;
					}
					CLU_CATCH_RETHROW_ALL_LE_VIEW_MANAGED("Error getting number variable")
						finally
					{
						m_Lock->ReleaseMutex();
					}
				}

				///////////////////////////////////////////////////////////////////////
				/// Get number variable

				void View::GetVarNumber(String^ sVarName, double % dValue)
				{
					try
					{
						if (!m_Lock->WaitOne(AccessTimeout, false))
						{
							CLU_THROW_MANAGED("No signal received");
						}

						if (m_iHandle == 0)
						{
							CLU_THROW_MANAGED( "Current handle is invalid");
						}

						double dVal;
						Clu::Viz::Wnd::GetVarNumber(m_iHandle, Clu::Net::Converter::ToIString(sVarName).ToCString(), dVal);

						dValue = dVal;
					}
					CLU_CATCH_RETHROW_ALL_LE_VIEW_MANAGED("Error getting number variable")
						finally
					{
						m_Lock->ReleaseMutex();
					}
				}

				///////////////////////////////////////////////////////////////////////
				/// Get array variable

				void View::GetVarTensor(String^ sVarName, array<int>^ % aiDim, array<double>^ % adData)
				{
					try
					{
						if (!m_Lock->WaitOne(AccessTimeout, false))
						{
							CLU_THROW_MANAGED("No signal received");
						}

						if (m_iHandle == 0)
						{
							CLU_THROW_MANAGED( "Current handle is invalid");
						}

						std::vector<int> vecDim;
						std::vector<double> vecData;
						Clu::Viz::Wnd::GetVarTensor(m_iHandle, Clu::Net::Converter::ToIString(sVarName).ToCString(), vecDim, vecData);

						int iIdx;
						int iDimCnt = int(vecDim.size());
						int iDataCnt = int(vecData.size());

						aiDim = gcnew array<int>(iDimCnt);
						for (iIdx = 0; iIdx < iDimCnt; ++iIdx)
						{
							aiDim[iIdx] = vecDim[iIdx];
						}

						adData = gcnew array<double>(iDataCnt);
						for (iIdx = 0; iIdx < iDataCnt; ++iIdx)
						{
							adData[iIdx] = vecData[iIdx];
						}

					}
					CLU_CATCH_RETHROW_ALL_LE_VIEW_MANAGED("Error getting array variable")
						finally
					{
						m_Lock->ReleaseMutex();
					}

				}

				///////////////////////////////////////////////////////////////////////
				/// Get string variable

				void View::GetVarString(String^ sVarName, String^ % sValue)
				{
					try
					{
						if (!m_Lock->WaitOne(AccessTimeout, false))
						{
							CLU_THROW_MANAGED("No signal received");
						}

						if (m_iHandle == 0)
						{
							CLU_THROW_MANAGED( "Current handle is invalid");
						}

						char pcVal[1024];
						Clu::Viz::Wnd::GetVarString(m_iHandle, Clu::Net::Converter::ToIString(sVarName).ToCString(), pcVal, 1023);

						sValue = gcnew String((const char*)pcVal);
					}
					CLU_CATCH_RETHROW_ALL_LE_VIEW_MANAGED("Error getting string variable")
						finally
					{
						m_Lock->ReleaseMutex();
					}
				}

				/////////////////////////////////////////////////////////////////////////
				/// Get color variable

				void View::GetVarColor(String^ sVarName, Drawing::Color^ % colValue)
				{
					try
					{
						if (!m_Lock->WaitOne(AccessTimeout, false))
						{
							CLU_THROW_MANAGED("No signal received");
						}

						if (m_iHandle == 0)
						{
							CLU_THROW_MANAGED( "Current handle is invalid");
						}

						double pdColor[4];
						Clu::Viz::Wnd::GetVarColor(m_iHandle, Clu::Net::Converter::ToIString(sVarName).ToCString(), pdColor);

						double dVal;
						unsigned char cVal, cRed, cGreen, cBlue, cAlpha;

						dVal = floor(255.0 * pdColor[0]);
						if (dVal < 0.0)
						{
							cVal = 0;
						}
						else if (dVal > 255.0)
						{
							cVal = 255;
						}
						else
						{
							cVal = (unsigned char)dVal;
						}

						cRed = cVal;

						dVal = floor(255.0 * pdColor[1]);
						if (dVal < 0.0)
						{
							cVal = 0;
						}
						else if (dVal > 255.0)
						{
							cVal = 255;
						}
						else
						{
							cVal = (unsigned char)dVal;
						}

						cGreen = cVal;

						dVal = floor(255.0 * pdColor[2]);
						if (dVal < 0.0)
						{
							cVal = 0;
						}
						else if (dVal > 255.0)
						{
							cVal = 255;
						}
						else
						{
							cVal = (unsigned char)dVal;
						}

						cBlue = cVal;

						dVal = floor(255.0 * pdColor[3]);
						if (dVal < 0.0)
						{
							cVal = 0;
						}
						else if (dVal > 255.0)
						{
							cVal = 255;
						}
						else
						{
							cVal = (unsigned char)dVal;
						}

						cAlpha = cVal;

						colValue = Drawing::Color::FromArgb(cAlpha, cRed, cGreen, cBlue);
					}
					CLU_CATCH_RETHROW_ALL_LE_VIEW_MANAGED("Error getting color variable")
						finally
					{
						m_Lock->ReleaseMutex();
					}
				}

				///////////////////////////////////////////////////////////////////////
				/// Get Image Variable

				void View::GetVarImage(String^ sVarName, Clu::Net::CluImage^ imgValue)
				{
					try
					{
						if (!m_Lock->WaitOne(AccessTimeout, false))
						{
							CLU_THROW_MANAGED( "No signal received");
						}

						if (m_iHandle == 0)
						{
							CLU_THROW_MANAGED( "Current handle is invalid");
						}

						if (imgValue == nullptr)
						{
							CLU_THROW_MANAGED( "Image parameter is invalid");
						}

						Clu::CIImage xImage;
						Clu::Viz::Wnd::GetVarImage(m_iHandle, Clu::Net::Converter::ToIString(sVarName).ToCString(), xImage);
						imgValue = gcnew Clu::Net::CluImage(xImage);
					}
					CLU_CATCH_RETHROW_ALL_LE_VIEW_MANAGED("Error getting image variable")
						finally
					{
						m_Lock->ReleaseMutex();
					}
				}

				///////////////////////////////////////////////////////////////////////
				/// Set Number Variable (int)

				void View::SetVarNumber(String^ sVarName, int iValue)
				{
					try
					{
						if (!m_Lock->WaitOne(AccessTimeout, false))
						{
							CLU_THROW_MANAGED("No signal received");
						}

						if (m_iHandle == 0)
						{
							CLU_THROW_MANAGED( "Current handle is invalid");
						}

						Clu::Viz::Wnd::SetVarNumber(m_iHandle, Clu::Net::Converter::ToIString(sVarName).ToCString(), iValue);
					}
					CLU_CATCH_RETHROW_ALL_LE_VIEW_MANAGED("Error setting var: " + sVarName + " to number")
						finally
					{
						m_Lock->ReleaseMutex();
					}
				}

				///////////////////////////////////////////////////////////////////////
				/// Set Number Variable

				void View::SetVarNumber(String^ sVarName, double dValue)
				{
					try
					{
						if (!m_Lock->WaitOne(AccessTimeout, false))
						{
							CLU_THROW_MANAGED("No signal received");
						}

						if (m_iHandle == 0)
						{
							CLU_THROW_MANAGED( "Current handle is invalid");
						}

						Clu::Viz::Wnd::SetVarNumber(m_iHandle, Clu::Net::Converter::ToIString(sVarName).ToCString(), dValue);
					}
					CLU_CATCH_RETHROW_ALL_LE_VIEW_MANAGED("Error setting var: " + sVarName + " to number")
						finally
					{
						m_Lock->ReleaseMutex();
					}
				}

				///////////////////////////////////////////////////////////////////////
				/// Set String Variable

				void View::SetVarString(String^ sVarName, String^ sValue)
				{
					try
					{
						if (!m_Lock->WaitOne(AccessTimeout, false))
						{
							CLU_THROW_MANAGED("No signal received");
						}

						if (m_iHandle == 0)
						{
							CLU_THROW_MANAGED( "Current handle is invalid");
						}

						Clu::Viz::Wnd::SetVarString(m_iHandle, Clu::Net::Converter::ToIString(sVarName).ToCString(), Clu::Net::Converter::ToIString(sValue).ToCString());
					}
					CLU_CATCH_RETHROW_ALL_LE_VIEW_MANAGED("Error setting var: " + sVarName + " to string")
						finally
					{
						m_Lock->ReleaseMutex();
					}
				}

				///////////////////////////////////////////////////////////////////////
				/// Set Var Color

				void View::SetVarColor(String^ sVarName, Drawing::Color^ colValue)
				{
					try
					{
						if (!m_Lock->WaitOne(AccessTimeout, false))
						{
							CLU_THROW_MANAGED("No signal received");
						}

						if (m_iHandle == 0)
						{
							CLU_THROW_MANAGED( "Current handle is invalid");
						}

						double pdColor[4];
						pdColor[0] = double(colValue->R) / 255.0;
						pdColor[1] = double(colValue->G) / 255.0;
						pdColor[2] = double(colValue->B) / 255.0;
						pdColor[3] = double(colValue->A) / 255.0;

						Clu::Viz::Wnd::SetVarColor(m_iHandle, Clu::Net::Converter::ToIString(sVarName).ToCString(), pdColor);
					}
					CLU_CATCH_RETHROW_ALL_LE_VIEW_MANAGED("Error setting var: " + sVarName + " to color")
						finally
					{
						m_Lock->ReleaseMutex();
					}
				}

				///////////////////////////////////////////////////////////////////////
				/// Set Number Variable

				void View::SetVarTensor(String^ sVarName, array<int>^ aiDim, array<double>^ adData)
				{
					try
					{
						if (!m_Lock->WaitOne(AccessTimeout, false))
						{
							CLU_THROW_MANAGED("No signal received");
						}

						if (m_iHandle == 0)
						{
							CLU_THROW_MANAGED( "Current handle is invalid");
						}

						std::vector<int> vecDim;
						std::vector<double> vecData;

						int iDim, iDimCnt = aiDim->Length;
						int iData, iDataCnt = adData->Length;

						vecDim.resize(iDimCnt);
						vecData.resize(iDataCnt);

						for (iDim = 0; iDim < iDimCnt; ++iDim)
						{
							vecDim[iDim] = aiDim[iDim];
						}

						for (iData = 0; iData < iDataCnt; ++iData)
						{
							vecData[iData] = adData[iData];
						}

						Clu::Viz::Wnd::SetVarTensor(m_iHandle, Clu::Net::Converter::ToIString(sVarName).ToCString(), iDimCnt, &vecDim[0], &vecData[0]);
					}
					CLU_CATCH_RETHROW_ALL_LE_VIEW_MANAGED("Error setting var: " + sVarName + " to tensor")
						finally
					{
						m_Lock->ReleaseMutex();
					}
				}

				///////////////////////////////////////////////////////////////////////

				void View::SetVarImage(String^ sVarName, Clu::Net::CluImage^ imgValue)
				{
					try
					{
						if (!m_Lock->WaitOne(AccessTimeout, false))
						{
							CLU_THROW_MANAGED("No signal received");
						}

						if (m_iHandle == 0)
						{
							CLU_THROW_MANAGED( "Current handle is invalid");
						}

						Clu::Viz::Wnd::SetVarImage(m_iHandle, Clu::Net::Converter::ToIString(sVarName).ToCString(), imgValue->ToIImage());

					}
					CLU_CATCH_RETHROW_ALL_LE_VIEW_MANAGED("Error setting var: " + sVarName + " to image")
						finally
					{
						m_Lock->ReleaseMutex();
					}
				}

				///////////////////////////////////////////////////////////////////////
				///////////////////////////////////////////////////////////////////////
			} // namespace Wnd
		} // namespace Net
	} // namespace Viz
} // namespace Clu
