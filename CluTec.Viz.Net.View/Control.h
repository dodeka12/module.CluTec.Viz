////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Net.View
// file:      Control.h
//
// summary:   Declares the control class
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

#pragma managed (push,off)
#include "CluTec.Viz.View.Base/CLUVizTypes.h"
#pragma managed (pop)

#include "CluTec.Base/Net.DelegateFunctionPointerCast.h"

#include "MouseEventArgs.h"
#include "ScriptMessageArgs.h"

#include "CluScript.h"
#include "ScriptVar.h"

namespace Clu
{
	namespace Viz
	{
		namespace Net
		{
			namespace View
			{
				public delegate void MouseEventHandler(System::Object^ xSender, Clu::Viz::Net::View::MouseEventArgs^ xMouseEventArgs);


				public ref class Control : public System::Windows::Forms::UserControl
				{
				private:
					int m_iHandle;

					bool m_bIsDisposed;
					bool m_bIsSuspended;
					bool m_bIsDrawingEnabled;

					bool m_bMouseEventsFired;

					bool m_bMouseDown;
					bool m_bKeyPress;
					bool m_bCtrlDown;
					bool m_bShiftDown;
					bool m_bAltDown;
					bool m_bNumLock;
					bool m_bCapsLock;
					bool m_bScrollLock;
					int m_iLastKeyPress;

					__int64 m_hSharedContext;


					System::Object^ m_xMouseEventDelegatePointer;

					// Information describing the mouse event
					MouseEventArgs^ m_xMouseEventArgs;

					/// <summary>	Main lock of class to make it thread safe. </summary>
					System::Threading::Mutex^ m_muxUserInput;

					CluScript^ m_xScript;
				private:

					/// <summary>	The components. </summary>
					System::ComponentModel::Container^ components;


				public:
					Control();
					Control(bool bShare);
					Control(Clu::Viz::Net::View::Control^ xShare);
					Control(System::Windows::Forms::Control^ xUserCtrl);
					Control(System::Windows::Forms::Control^ xUserCtrl, bool bShare);
					Control(System::Windows::Forms::Control^ xUserCtrl, Clu::Viz::Net::View::Control^ xShare);

					~Control();
					!Control();

				private:
					void _Init(bool bShare, __int64 hContext);
					System::String^ _GetLastError(System::String^ sText);
					void _SetShape();
					void _UpdateModifierKeys();
					bool _TranslateControlKey(int % iCLUKey, System::Windows::Forms::Keys eKey);

				public:

					////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// Event queue for all listeners interested in Mouse events. This is called while the mouse
					/// event is processed in the visualization so that the handler can modify script variables on
					/// return. However, the handler MUST NOT call any other functions on this control instance.
					/// </summary>
					////////////////////////////////////////////////////////////////////////////////////////////////////

					event MouseEventHandler^ SyncMouseEvent;

					////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// Event queue for all listeners interested in mouse events. The original mouse event in the
					/// visualization only invokes a mouse message in the message queue and then returns to the
					/// visualization. Therefore, this event is triggered after the visualization has already
					/// processed the event. However, you can call any function of this control instance.
					/// </summary>
					////////////////////////////////////////////////////////////////////////////////////////////////////

					event MouseEventHandler^ MouseEvent;

					/// ///////////////////////////////////////////////////////////////////////////////////////////////////
					/// ///////////////////////////////////////////////////////////////////////////////////////////////////
					/// ///////////////////////////////////////////////////////////////////////////////////////////////////
				public:
					property bool IsValid
					{
						bool get()
						{
							return m_iHandle > 0;
						}
					}

					property __int64 SharedContext
					{
						__int64 get()
						{
							return m_hSharedContext;
						}
					}

					property bool FocusOnMouseOver;

					property bool DrawingEnabled
					{
						bool get();
						void set(bool bEnable);
					}

					property bool MouseEventsFired
					{
						bool get();
						void set(bool bEnable);
					}

					property CluScript^ Script
					{
						CluScript^ get()
						{
							return m_xScript;
						}
					}

				public:
					/// ///////////////////////////////////////////////////////////////////////////////////////////////////
					/// Functions /////////////////////////////////////////////////////////////////////////////////////////
					/// ///////////////////////////////////////////////////////////////////////////////////////////////////
					
					/// <summary>	Bind the redering context of the control to this thread and lock the visualization
					/// 			loop of the control. After calling Bind() you can execute OpenGL draw commands
					/// 			from this thread.</summary>
					
					void BindRC();
					void UnbindRC();

					void EnableDrawingAndRedisplay();


					void Update();
					void Update(bool bWait);

					void LockUserInput();
					void UnlockUserInput();

					void LockVisualization();
					void UnlockVisualization();

					/// ///////////////////////////////////////////////////////////////////////////////////////////////////
					/// Event Handler /////////////////////////////////////////////////////////////////////////////////////
					/// ///////////////////////////////////////////////////////////////////////////////////////////////////
				protected:
					void _OnNativeMouseEvent(Clu::Viz::View::SMouseEventData* pvData, void* pvContext);
					void _OnInvokedMouseEvent(System::Object^ xSender, Clu::Viz::Net::View::MouseEventArgs^ xMouseEventArgs);

					/// ///////////////////////////////////////////////////////////////////////////////////////////////////
					/// Event Handler Overrides ///////////////////////////////////////////////////////////////////////////
					/// ///////////////////////////////////////////////////////////////////////////////////////////////////
				public:

					////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>	Paints this window. </summary>
					///
					/// <remarks>	Perwass,. </remarks>
					///
					/// <param name="e">	The System::Windows::Forms::PaintEventArgs^ to process. </param>
					////////////////////////////////////////////////////////////////////////////////////////////////////

					virtual void OnPaint(System::Windows::Forms::PaintEventArgs^ e) override;
					virtual void OnResize(System::EventArgs^ e) override;
					virtual void OnEnabledChanged(System::EventArgs^ e) override;
					virtual void OnMouseEnter(System::EventArgs^ e) override;
					virtual void OnMouseLeave(System::EventArgs^ e) override;
					virtual void OnGotFocus(System::EventArgs^ e) override;
					virtual void OnLostFocus(System::EventArgs^ e) override;
					virtual void OnMouseDown(System::Windows::Forms::MouseEventArgs^ e) override;
					virtual void OnMouseUp(System::Windows::Forms::MouseEventArgs^ e) override;
					virtual void OnMouseMove(System::Windows::Forms::MouseEventArgs^ e) override;
					virtual void OnKeyDown(System::Windows::Forms::KeyEventArgs^ e) override;
					virtual void OnKeyUp(System::Windows::Forms::KeyEventArgs^ e) override;
					virtual void OnKeyPress(System::Windows::Forms::KeyPressEventArgs^ e) override;

					virtual void WndProc(System::Windows::Forms::Message % m) override;




#pragma region Windows Form Designer generated code

					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/// <summary>
					/// 	Initializes the component.
					/// </summary>
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					void InitializeComponent()
					{
						//
						// ViewCtrl
						//
						this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::None;
						this->BackColor = System::Drawing::SystemColors::Highlight;
						this->Cursor = System::Windows::Forms::Cursors::Hand;
						this->Name = L"ViewCtrl";
					}

#pragma endregion

				};

			} // namespace View
		} // namespace Net
	} // namespace Viz
} // namespace Clu

