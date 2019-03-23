////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Net.View
// file:      Mesh3dTool.cpp
//
// summary:   Implements the mesh 3D tool class
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
#include "Mesh3dTool.h"
#include "Env.h"
#include "EnvPath.h"

namespace Clu
{
	namespace Viz
	{
		namespace Net
		{
			namespace View
			{


				Mesh3dTool::Mesh3dTool() : Tool()
				{
					m_xImageVar = nullptr;
					m_xMeshVar = nullptr;
				}


				void Mesh3dTool::_OnAttached()
				{
					try
					{

						CluScript^ xScript = Ctrl->Script;

						xScript->Filename = Env::GetPath(Clu::Viz::Net::EnvPaths::Script_Root) + "\\CluTec.Viz.Net.View.Mesh3dTool\\Main.clu";

						m_xImageVar = xScript->GetVar("imgX");
						m_xImageVar->SendMessageToScriptOnChange = true;
						m_xImageVar->ScriptMessageName = "SetImage";


						m_xMeshVar = xScript->GetVar("objMesh");
						m_xMeshVar->SendMessageToScriptOnChange = true;
						m_xMeshVar->ScriptMessageName = "SetMesh";
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error attaching Mesh3d tool to control")
				}

				void Mesh3dTool::_OnDetaching()
				{
					delete m_xImageVar;
					m_xImageVar = nullptr;

					delete m_xMeshVar;
					m_xMeshVar = nullptr;
				}

				void Mesh3dTool::SetImage(Clu::Net::CluImage^ xImage)
				{
					try
					{
						if (!IsAttached())
							throw gcnew Clu::Net::CluException("Mesh3dTool is not attached to a control.", __FILE__, __FUNCTION__, __LINE__);

						m_xImageVar->Set(xImage);
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error setting image")
				}

				void Mesh3dTool::SetMesh(Clu::Net::DataContainer^ xMesh)
				{
					try
					{
						if (!IsAttached())
							throw gcnew Clu::Net::CluException("Mesh3dTool is not attached to a control.", __FILE__, __FUNCTION__, __LINE__);

						m_xMeshVar->Set(xMesh);
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error setting mesh")
				}


			} // namespace View
		} // namespace Net
	} // namespace Viz
} // namespace Clu
