////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Net.Wnd
// file:      Engine.cpp
//
// summary:   Implements the engine class
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
#include "Engine.h"

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
				using namespace System;

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				Engine::Engine()
				{
					m_bEngineOK = false;
					m_sScriptPath = "";
				}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				Engine::~Engine()
				{
					if (m_bEngineOK)
					{
						this->!Engine();
					}
				}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				Engine::!Engine()
				{
					if (m_bEngineOK)
					{
						End();
					}
				}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				bool Engine::IsRunning::get()
				{
					return Clu::Viz::Wnd::IsRunning(1);
				}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				System::String^ Engine::LastError::get()
				{
					char pcError[4096];
					Clu::Viz::Wnd::GetLastError(pcError, 4096);
					return gcnew System::String(pcError);
				}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				System::String^ Engine::_FormatLastError(System::String^ sText)
				{
					return sText + " >> " + LastError;
				}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				void Engine::ScriptPath::set(System::String^ sValue)
				{
					try
					{
						
						Clu::Viz::Wnd::SetScriptPath(Clu::Net::Converter::ToIString(sValue).ToCString());
						m_sScriptPath = sValue;
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error setting script path.")
				}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				String^ Engine::ScriptPath::get()
				{
					return m_sScriptPath;
				}


				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				void Engine::Start()
				{
					try
					{
						if (m_bEngineOK)
						{
							return;
						}

						Clu::Viz::Wnd::Start();
						m_bEngineOK = true;
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error starting CluViz engine.")
				}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				void Engine::End()
				{
					if (!m_bEngineOK)
					{
						return;	// returned true
					}

					Clu::Viz::Wnd::End();
					m_bEngineOK = false;
				}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				void Engine::DestroyAllViews()
				{
					try
					{
						if (!m_bEngineOK)
						{
							throw gcnew Clu::Net::CluException("CluViz engine not running", __FILE__, __FUNCTION__, __LINE__);
						}
						Clu::Viz::Wnd::DestroyAll();
					}
					CLU_CATCH_RETHROW_ALL_MANAGED("Error destroying all views")
				}
			} // namespace Wnd
		} // namespace Net
	} // namespace Viz
} // namespace Clu
