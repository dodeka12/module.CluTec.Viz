////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Net.View
// file:      Env.cpp
//
// summary:   Implements the environment class
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
#include "Env.h"

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
			System::String^ Env::GetPath(Clu::Viz::Net::EnvPaths eEnvPath)
			{
				try
				{
					return Clu::Net::Converter::ToString(Clu::Viz::View::GetPath(Clu::Viz::EEnvPath(eEnvPath)));
				}
				CLU_CATCH_RETHROW_ALL_MANAGED("Error obtaining path")
			}

			Clu::Net::CluImage^ Env::LoadImage(System::String^ sFilename)
			{
				try
				{
					return gcnew Clu::Net::CluImage(Clu::Viz::View::LoadImage(Clu::Net::Converter::ToIString(sFilename)));
				}
				CLU_CATCH_RETHROW_ALL_MANAGED(System::String::Format("Error saving image from file '{0}'", sFilename))
			}

			void Env::SaveImage(System::String^ sFilename, Clu::Net::CluImage^ xImage)
			{
				try
				{
					Clu::Viz::View::SaveImage(Clu::Net::Converter::ToIString(sFilename), xImage->ToIImage());
				}
				CLU_CATCH_RETHROW_ALL_MANAGED(System::String::Format("Error saving image to file '{0}'", sFilename))
			}

		} // namespace Net
	} // namespace Viz
} // namespace Clu
