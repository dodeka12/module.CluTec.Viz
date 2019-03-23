////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Net.View
// file:      Mesh3dTool.h
//
// summary:   Declares the mesh 3D tool class
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
#include "Control.h"
#include "Tool.h"

namespace Clu
{
	namespace Viz
	{
		namespace Net
		{
			namespace View
			{
				public ref class Mesh3dTool : public Tool
				{
				private:
					View::ScriptVar^ m_xImageVar;
					View::ScriptVar^ m_xMeshVar;

				protected:
					virtual void _OnAttached() override;
					virtual void _OnDetaching() override;

				public:
					Mesh3dTool();

					void SetImage(Clu::Net::CluImage^ xImage);
					void SetMesh(Clu::Net::DataContainer^ xMesh);
				};

			} // namespace View
		} // namespace Net
	} // namespace Viz
} // namespace Clu
