////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Net.View
// file:      Engine.h
//
// summary:   Declares the engine class
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

#pragma managed(push, off)
#include "CluTec.Viz.View/Clu.Viz.View.h"
#include "CluTec.Base/Exception.h"
#pragma managed(pop)


namespace Clu
{
	namespace Viz
	{
		namespace Net
		{
			namespace View
			{

				public ref class Engine
				{
				private:
					static Engine^ m_xEngine = nullptr;

					bool m_bEngineStarted;

				private:
					Engine()
					{
						m_bEngineStarted = false;
					}

				public:

					property bool Running
					{
						bool get()
						{
							return m_bEngineStarted;
						}
					}

				public:
					static Engine^ GetEngine()
					{
						if (m_xEngine == nullptr)
						{
							m_xEngine = gcnew Engine();
						}

						return m_xEngine;
					}

					~Engine()
					{
						this->!Engine();
					}

					!Engine()
					{
						Clu::Viz::View::End();
					}

					void Start()
					{
						try
						{
							Clu::Viz::View::Start();
							m_bEngineStarted = true;
						}
						catch (Clu::CIException& xEx)
						{
							throw gcnew System::Exception(Clu::Net::Converter::ToString(xEx.ToStringComplete()));
						}
					}

					void Stop()
					{
						try
						{
							if (m_bEngineStarted)
							{
								Clu::Viz::View::End();
								m_bEngineStarted = false;
							}
						}
						catch (Clu::CIException& xEx)
						{
							throw gcnew System::Exception(Clu::Net::Converter::ToString(xEx.ToStringComplete()));
						}
					}

				};

			} // namespace View
		} // namespace Net
	} // namespace Viz
} // namespace Clu


