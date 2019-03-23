////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Net.View
// file:      CluScript.h
//
// summary:   Declares the clu script class
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

#include "ScriptVar.h"

namespace Clu
{
	namespace Viz
	{
		namespace Net
		{
			namespace View
			{

				public ref class CluScript
				{
				private:
					int m_iHandle;
					System::String^ m_sOutput;

				private:
					CluScript();

				public:
					CluScript(int iHandle);

				public:
					property int Handle
					{
						int get()
						{
							return m_iHandle;
						}
					}

					property System::String^ ClearText;
					property System::Drawing::Color ClearColor;
					property System::String^ Output
					{
						System::String^ get()
						{
							return m_sOutput;
						}
					}

					property bool StoreOutput;
					property bool ResetVariablesOnNewScript;
					property bool ReparseLatexOnNewScript;

					property System::String^ Text
					{
						void set(System::String^ sScript);
					}

					property System::String^ Filename
					{
						void set(System::String^ sFilename);
					}

					property bool ThrowOnError;

				protected:
					System::String^ _GetOutput(bool %bIsError);
					bool _CheckError(System::String^% sOutput, System::String^% sErrorText);

				public:
					virtual void OnScriptMessage(System::Object^ xSender, ScriptMessageArgs^ xArgs);

				public:
					void Clear();
					static System::String^ GetLastError(System::String^ sText);

					ScriptVar^ GetVar(System::String^ sName);

					bool Execute(System::String^ sMsg);
					bool Execute(System::String^ sMsg, System::String^% sOutput, bool bDoReadOutput);
				};

			} // namespace View
		} // namespace Net
	} // namespace Viz
} // namespace Clu
