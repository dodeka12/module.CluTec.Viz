////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Net.View
// file:      MouseEventArgs.h
//
// summary:   Declares the mouse event arguments class
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

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				/// <summary>
				/// 	Values that represent the type of the mouse event.
				/// </summary>
				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				public enum class MouseEventType
				{
					/// <summary> No event type. </summary>
					None = 0,
					/// <summary> Mouse is over an object. </summary>
					Over,
					/// <summary> An object has been selected. </summary>
					Select,
					/// <summary> A mouse button has been pressed to drag. </summary>
					Drag,
					/// <summary> The mouse button has been released after dragging. </summary>
					DragEnd,
					/// <summary> A mouse button has been clicked. </summary>
					Click,
					/// <summary> A mouse button has been pressed. </summary>
					Down,
					/// <summary> A mouse button has been released. </summary>
					Up,
				};

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				/// <summary>
				/// 	Values that represent an action that can be processed after handling mouse events.
				/// </summary>
				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				public enum class MouseEventReturnType
				{
					/// <summary> Do nothing. </summary>
					None = 0,
					/// <summary> Update the visualization. </summary>
					PostRedisplay,
					/// <summary> Execute the script with a given tool name. </summary>
					ExecScript,
				};

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				/// <summary>
				/// 	Values that represent the pressed mouse button of a mouse event.
				/// </summary>
				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				public enum class MouseEventButtonType
				{
					/// <summary> No mouse button has triggered the event. </summary>
					None = 0,
					/// <summary> Left mouse button has triggered the event. </summary>
					Left,
					/// <summary> Middle mouse button has triggered the event. </summary>
					Middle,
					/// <summary> Right mouse button has triggered the event. </summary>
					Right,
				};

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				/// <summary>
				/// 	Information about mouse events.
				/// </summary>
				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				public ref struct MouseEventArgs : public System::EventArgs
				{
					/// <summary> A value indicating whether a pickable scene has been clicked with the left or right mouse button. </summary>
					bool bIsScenePickable;

					/// <summary> A semicolon ';' separated list of scenes that are part of the mouse event. </summary>
					System::String^ sSceneNameList;

					/// <summary> The part ID of the picked scene. </summary>
					unsigned uScenePartId;

					/// <summary> True if the left mouse button is pressed. </summary>
					bool bIsLeftButtonDown;
					/// <summary> True if the right mouse button is pressed. </summary>
					bool bIsRightButtonDown;

					/// <summary> True if the control key is pressed. </summary>
					bool bIsCtrlDown;
					/// <summary> True if the shift key is pressed. </summary>
					bool bIsShiftDown;
					/// <summary> True if the alt key is pressed. </summary>
					bool bIsAltDown;

					/// <summary> The current X coordinate of the mouse cursor on the screen in pixels. </summary>
					int iScreenPosX;
					/// <summary> The current Y coordinate of the mouse cursor on the screen in pixels. </summary>
					int iScreenPosY;
					/// <summary> The offset of the current mouse cursor to the last drag start position in X direction in pixels. </summary>
					int iScreenPosDeltaX;
					/// <summary> The offset of the current mouse cursor to the last drag start position in Y direction in pixels. </summary>
					int iScreenPosDeltaY;
					
					/// <summary> The 3D mouse position where the drag action has been started in the global coordinate system. </summary>
					Clu::Net::Vector3 vMouseDragStart;
					/// <summary> The current 3D mouse position while dragging in the global coordinate system. </summary>
					Clu::Net::Vector3 vMouseDrag;
					/// <summary> The 3D mouse position offset from the start of the drag action to the current position in the global coordinate system. </summary>
					Clu::Net::Vector3 vMouseDragDelta;

					/// <summary> The 3D position where the drag action has been started in the global coordinate system. </summary>
					Clu::Net::Vector3 vDragDataStart;
					/// <summary> The current 3D position while dragging in the global coordinate system. </summary>
					Clu::Net::Vector3 vDragData;
					/// <summary> The 3D position offset from the start of the drag action to the current position in the global coordinate system. </summary>
					Clu::Net::Vector3 vDragDataDelta;

					/// <summary> The 3D mouse position where the drag action has been started in the coordinate system of the scene. </summary>
					Clu::Net::Vector3 vMouseDragLocalStart;
					/// <summary> The current 3D mouse position while dragging in the coordinate system of the scene. </summary>
					Clu::Net::Vector3 vMouseDragLocal;
					/// <summary> The 3D mouse position offset from the start of the drag action to the current position in the coordinate system of the scene. </summary>
					Clu::Net::Vector3 vMouseDragLocalDelta;

					/// <summary> The type of the mouse event to identify whether the mouse has been moved, clicked, dragged, etc. </summary>
					MouseEventType eMouseEventType;
					/// <summary> The mouse button that triggered the event. </summary>
					MouseEventButtonType eMouseEventButton;

					/// <summary> The return type of the event. This defines an action that should be executed and can be set in any event handler. </summary>
					MouseEventReturnType eMouseEventReturnType;

					/// <summary> The tool name that should be executed after processing the event handler if eMouseEventReturnType is set to ExecScript. </summary>
					System::String^ sExecScriptToolName;

					/// <summary> Pointer to internal event data. </summary>
					void* pNativeMouseEventData;

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Convert this object into a string representation.
					/// </summary>
					///
					/// <returns> This object as a string. </returns>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					virtual System::String^ ToString() override
					{
						return "Pickable: " + bIsScenePickable + ", " +
							"LeftButton: " + bIsLeftButtonDown + ", " +
							"RightButton: " + bIsRightButtonDown + ", " +
							"Ctrl: " + bIsCtrlDown + ", " +
							"Shift: " + bIsShiftDown + ", " +
							"Alt: " + bIsAltDown + "\n" +
							"MouseEventType: " + eMouseEventType.ToString() + ", " +
							"eMouseEventButton: " + eMouseEventButton.ToString() + "\n" +
							"Drag Start: " + vMouseDragLocalStart + ", " +
							"Drag Current: " + vMouseDragLocal;
					}
				};

			} // namespace View
		} // namespace Net
	} // namespace Viz
} // namespace Clu
