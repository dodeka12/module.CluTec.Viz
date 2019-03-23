////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Net.View
// file:      ScriptMessageArgs.h
//
// summary:   Declares the script message arguments class
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
			namespace View
			{

				public ref class ScriptMessageArgs : public System::EventArgs
				{
				public:
					property System::String^ Message;

					property bool DoReadOutput;

					property bool IsError;
					property System::String^ Output;

				public:
					ScriptMessageArgs()
					{
						Message = "";
						DoReadOutput = false;
						Output = "";
					}

					ScriptMessageArgs(System::String^ _Message, bool _DoReadOutput)
					{
						Message = _Message;
						DoReadOutput = _DoReadOutput;
						Output = "";
					}
				};

				public delegate void ScriptMessageHandler(System::Object^ xSender, Clu::Viz::Net::View::ScriptMessageArgs^ xScriptMessageArgs);

			} // namespace View
		} // namespace Net
	} // namespace Viz
} // namespace Clu
