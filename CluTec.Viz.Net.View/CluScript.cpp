////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Net.View
// file:      CluScript.cpp
//
// summary:   Implements the clu script class
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
#include "CluScript.h"

#pragma managed (push, off)
#include "CluTec.Viz.View/Clu.Viz.View.h"
#pragma managed(pop)


#include "Defines.h"

namespace Clu
{
	namespace Viz
	{
		namespace Net
		{
			namespace View
			{


				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				CluScript::CluScript()
				{
				}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				CluScript::CluScript(int iHandle)
				{
					m_iHandle = iHandle;

					ClearText = "CluViz " CLU_MODULE_VER_SHORT_STR;
					ClearColor = System::Drawing::Color::Black;

#ifdef CLU_RTM
					ThrowOnError = false;
#else
					ThrowOnError = true;
#endif
				}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				System::String^ CluScript::GetLastError(System::String^ sText)
				{
					char pcError[4096];
					Clu::Viz::View::GetLastError(pcError, 4096);
					return sText + " >> " + gcnew System::String(pcError);
				}


				////////////////////////////////////////////////////////////////////////////////////////////////////
				void CluScript::Text::set(System::String^ sScript)
				{
					try
					{
						Clu::Viz::View::EW_SetScript(m_iHandle, Clu::Net::Converter::ToIString(sScript).ToCString(), ResetVariablesOnNewScript, ReparseLatexOnNewScript);

						if (ThrowOnError)
						{
							System::String^ sErrorText = "", ^sOutput = "";
							if (_CheckError(sOutput, sErrorText))
							{
								throw gcnew System::Exception(sErrorText);
							}
						}
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error setting script")

				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				void CluScript::Filename::set(System::String^ sFilename)
				{
					try
					{
						Clu::Viz::View::EW_LoadScript(m_iHandle, Clu::Net::Converter::ToIString(sFilename).ToCString(), nullptr);

						if (ThrowOnError)
						{
							System::String^ sErrorText = "", ^sOutput = "";
							if (_CheckError(sOutput, sErrorText))
							{
								throw gcnew System::Exception(sErrorText);
							}
						}
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error setting script")
				}


				////////////////////////////////////////////////////////////////////////////////////////////////////
				ScriptVar^ CluScript::GetVar(System::String^ sName)
				{
					return gcnew ScriptVar(sName, this);
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				void CluScript::Clear()
				{
					try
					{
						double pdColor[4];
						pdColor[0] = double(ClearColor.R) / 255.0;
						pdColor[1] = double(ClearColor.G) / 255.0;
						pdColor[2] = double(ClearColor.B) / 255.0;
						pdColor[3] = double(ClearColor.A) / 255.0;

						System::String^ sScript = "_BGColor = Color(" + pdColor[0] + ", " + pdColor[1] + ", " + pdColor[2] + "); " +
							"SetImgPos(0,0,0); SetImgAlign(0.5,0.5); SetRenderTextFont(\"Segoe UI\"); SetRenderTextSize(20); :GetRenderTextImage( \""
							+ ClearText + "\" );";

						Clu::Viz::View::EW_SetScript(m_iHandle, Clu::Net::Converter::ToIString(sScript).ToCString(), true);
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error clearing Clu control")
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				System::String^ CluScript::_GetOutput(bool %bIsError)
				{
					try
					{
						bool bError = false;
						Clu::CIString sOutput;
						Clu::Viz::View::GetScriptOutput(m_iHandle, sOutput, bError);
						bIsError = bError;

						return Clu::Net::Converter::ToString(sOutput);
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error getting script output")
				}


				////////////////////////////////////////////////////////////////////////////////////////////////////
				bool CluScript::_CheckError(System::String^% sOutput, System::String^% sErrorText)
				{
					bool bIsError = false;
					sOutput = _GetOutput(bIsError);
					sErrorText = "";

					if (bIsError)
					{
						using namespace System::Text::RegularExpressions;

						// Parse script output
						Regex^ xR = gcnew Regex(
							"File: <i>(?<file>.+?)</i>.*?<br>(?<msg>.+?)<br>.*?<i>Line (?<line>.+?) \\(.*?<code><b>(?<code>.+?)</b></code>",
							RegexOptions::Singleline);

						Match^ xM = xR->Match(sOutput);

						System::String^ sFile = xM->Groups["file"]->Value;
						System::String^ sLine = xM->Groups["line"]->Value;
						System::String^ sMsg = "Clu.Viz.Net.View.Control: " + xM->Groups["msg"]->Value + " - CODE: " + xM->Groups["code"]->Value;

						// TODO: Throw Clu.Net.Exception
						sErrorText = "Script Function: " + sMsg + " - " + sFile + " (" + sLine + ")";
					}

					return bIsError;
				}


				////////////////////////////////////////////////////////////////////////////////////////////////////
				void CluScript::OnScriptMessage(System::Object^ xSender, ScriptMessageArgs^ xArgs)
				{
					try
					{
						System::String^ sOutput = "";
						xArgs->IsError = !Execute(xArgs->Message, sOutput, xArgs->DoReadOutput);
						xArgs->Output = sOutput;
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error processing script message")
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				bool CluScript::Execute(System::String^ sMsg)
				{
					return Execute(sMsg, m_sOutput, StoreOutput);
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				bool CluScript::Execute(System::String^ sMsg, System::String^% sOutput, bool bDoReadOutput)
				{
					try
					{
						Clu::Viz::View::EW_ExecUser(m_iHandle, Clu::Net::Converter::ToIString(sMsg).ToCString());

						bool bIsError = false;

						if (bDoReadOutput || ThrowOnError)
						{
							System::String^ sErrorText;

							bIsError = _CheckError(sOutput, sErrorText);

							if (ThrowOnError && bIsError)
							{
								try
								{
									throw gcnew System::Exception(sErrorText);
								}
								catch (System::Exception^ ex)
								{
									throw gcnew System::Exception("Script message: '" + sMsg + "' Error: " + sOutput, ex);
								}
							}

						}

						return !bIsError;
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error processing script message")
				}


			} // namespace View
		} // namespace Net
	} // namespace Viz
} // namespace Clu
