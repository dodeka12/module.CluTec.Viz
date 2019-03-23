////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.View.Base
// file:      CLUVizTypes.h
//
// summary:   Declares the clu viz types class
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

#include "CluTec.Math/Static.Vector.h"
#include "CluTec.Math/Static.Matrix.h"

#include "CluTec.Base/Exception.h"

namespace Clu
{
	namespace Viz
	{
		namespace View
		{

			// Mouse event type enum
			enum class EMouseEventType
			{
				None = 0,
				Over,
				Select,
				Drag,
				DragEnd,
				Click,
				Down,
				Up,
			};

			// The return event type.
			enum class EMouseEventReturnType
			{
				None = 0,
				PostRedisplay,
				ExecScript,
			};

			enum class EMouseEventButton
			{
				None = 0,
				Left,
				Middle,
				Right,
			};

			// Mouse Callback Info
			struct SMouseEventData
			{
				bool bIsScenePickable;
				bool bIsSceneDragEnabled;
				bool bIsSceneNotifyEnabled;
				bool bIsSceneTool;

				char pcSceneNameList[1024];
				unsigned uScenePartId;

				bool bIsLeftButtonDown;
				bool bIsRightButtonDown;

				bool bIsCtrlDown;
				bool bIsShiftDown;
				bool bIsAltDown;

				EMouseEventType eMouseEventType;
				EMouseEventButton eMouseEventButton;

				int iScreenPosX, iScreenPosY;
				int iScreenPosDeltaX, iScreenPosDeltaY;

				Clu::SVector3<double> vMouseDragStart;
				Clu::SVector3<double> vMouseDrag;
				Clu::SVector3<double> vMouseDragDelta;

				Clu::SVector3<double> vMouseDragLocalStart;
				Clu::SVector3<double> vMouseDragLocal;
				Clu::SVector3<double> vMouseDragLocalDelta;

				Clu::SVector3<double> vDragDataStart;
				Clu::SVector3<double> vDragData;
				Clu::SVector3<double> vDragDataDelta;

				/// <summary>	Transposed projection matrix. </summary>
				Clu::SMatrix<double, 4> matProjectionT;

				/// <summary>	Transposed matrix of the coordinate frame outside the picked scene. </summary>
				Clu::SMatrix<double, 4> matFrameT;

				/// <summary>	Transposed matrix of the coordinate frame inside the picked scene, i.e. after any automatic translation/rotation applied by the scene. </summary>
				Clu::SMatrix<double, 4> matFrameLocalT;

				/// <summary>	The viewport. </summary>
				Clu::SVector4<int> vViewport;

				EMouseEventReturnType eMouseEventReturnType;
				char pcExecScriptToolName[256];
			};

			/// <summary> Function pointer type of the mouse event callback function. </summary>
			typedef void(__stdcall * TFuncMouseEventCallback)(SMouseEventData* pvData, void* pvContext);
			/// <summary> Function pointer type of the error event callback function. </summary>
			typedef void(__stdcall * TFuncErrorEventCallback)(const Clu::CIException& xException, void* pvContext);
		}
	}
}
