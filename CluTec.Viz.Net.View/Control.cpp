////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Net.View
// file:      Control.cpp
//
// summary:   Implements the control class
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
#include "Control.h"

#pragma managed (push, off)
#include "CluTec.Viz.View/Clu.Viz.View.h"
#pragma managed(pop)

#include "Defines.h"


using namespace System::Windows::Forms;



namespace Clu
{
	namespace Viz
	{
		namespace Net
		{
			namespace View
			{
				public delegate void NativeMouseEventHandler(Clu::Viz::View::SMouseEventData* pvData, void* pvContext);


#pragma region Creation / Destruction
				Control::Control()
				{
					_Init(false, 0);
				}

				Control::Control(bool bShare)
				{
					_Init(bShare, 0);
				}

				Control::Control(Clu::Viz::Net::View::Control^ xShare)
				{
					_Init(false, xShare->SharedContext);
				}

				Control::Control(System::Windows::Forms::Control^ xUserCtrl)
				{
					_Init(false, 0);
					xUserCtrl->Controls->Add(this);
					this->Dock = DockStyle::Fill;
				}

				Control::Control(System::Windows::Forms::Control^ xUserCtrl, bool bShare)
				{
					_Init(bShare, 0);
					xUserCtrl->Controls->Add(this);
					this->Dock = DockStyle::Fill;
				}

				Control::Control(System::Windows::Forms::Control^ xUserCtrl, Clu::Viz::Net::View::Control^ xShare)
				{
					_Init(false, xShare->SharedContext);
					xUserCtrl->Controls->Add(this);
					this->Dock = DockStyle::Fill;
				}

				Control::~Control()
				{
					if (components)
					{
						delete components;
					}

					if (!m_bIsDisposed)
					{
						this->!Control();
						m_bIsDisposed = true;
					}
				}

				Control::!Control()
				{
					try
					{
						if (!m_bIsDisposed)
						{
							Clu::Viz::View::ReleaseWindow(m_iHandle);

							m_iHandle = 0;
							m_bIsDisposed = true;
						}
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error disposing control")
				}

#pragma endregion
				// //////////////////////////////////////////////////////////////////////////////////
				// //////////////////////////////////////////////////////////////////////////////////
				// //////////////////////////////////////////////////////////////////////////////////
				// //////////////////////////////////////////////////////////////////////////////////
#pragma region Properties
				bool Control::DrawingEnabled::get()
				{
					return m_bIsDrawingEnabled;
				}

				void Control::DrawingEnabled::set(bool bEnable)
				{
					try
					{
						m_bIsDrawingEnabled = bEnable;
						Clu::Viz::View::EnableDrawing(m_iHandle, bEnable, false);
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error in enable drawing")
				}

				void Control::EnableDrawingAndRedisplay()
				{
					try
					{
						m_bIsDrawingEnabled = true;
						Clu::Viz::View::EnableDrawing(m_iHandle, true, true);
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error enabling drawing")
				}



				bool Control::MouseEventsFired::get()
				{
					return m_bMouseEventsFired;
				}

				void Control::MouseEventsFired::set(bool bEnable)
				{
					try
					{
						if (bEnable)
						{
							Clu::Viz::View::SetMouseEventHandler(m_iHandle,
								((Clu::Net::DelegateFunctionPointerCast<Clu::Viz::View::TFuncMouseEventCallback, MouseEventHandler>^)m_xMouseEventDelegatePointer)->Pointer,
								0);
							m_bMouseEventsFired = true;
						}
						else
						{
							Clu::Viz::View::RemoveMouseEventHandler(m_iHandle);
							m_bMouseEventsFired = false;
						}
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error enabling/disabling mouse events firing")
				}



#pragma endregion
				// //////////////////////////////////////////////////////////////////////////////////
				// //////////////////////////////////////////////////////////////////////////////////
				// //////////////////////////////////////////////////////////////////////////////////
				// //////////////////////////////////////////////////////////////////////////////////
#pragma region Functions
				//try
				//{

				//}
				//CLU_CATCH_RETHROW_ALL_MANAGED(__FUNCTION__)

				////////////////////////////////////////////////////////////////////////////////////////////////////
				void Control::BindRC()
				{
					try
					{
						Clu::Viz::View::EW_LockVis(m_iHandle, true);
						Clu::Viz::View::EW_MakeCurrent(m_iHandle, true);
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error binding OpenGL rendering context to this thread")
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				void Control::UnbindRC()
				{
					try
					{
						Clu::Viz::View::EW_MakeCurrent(m_iHandle, false);
						Clu::Viz::View::EW_LockVis(m_iHandle, false);
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error un-binding OpenGL rendering context from this thread")
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				void Control::Update()
				{
					try
					{
						Update(true);
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error in Updating")
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				void Control::Update(bool bWait)
				{
					try
					{
						UserControl::Update();
						Clu::Viz::View::EW_Display(m_iHandle, bWait);
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error in Updating")
				}




				////////////////////////////////////////////////////////////////////////////////////////////////////
				void Control::LockUserInput()
				{
					m_muxUserInput->WaitOne();
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				void Control::UnlockUserInput()
				{
					m_muxUserInput->ReleaseMutex();
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				void Control::LockVisualization()
				{
					try
					{
						Clu::Viz::View::EW_LockVis(m_iHandle, true);
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error locking visualization")
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				void Control::UnlockVisualization()
				{
					try
					{
						Clu::Viz::View::EW_LockVis(m_iHandle, false);
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error unlocking visualization")
				}


#pragma endregion

				// //////////////////////////////////////////////////////////////////////////////////
				// //////////////////////////////////////////////////////////////////////////////////
				// //////////////////////////////////////////////////////////////////////////////////
				// //////////////////////////////////////////////////////////////////////////////////
#pragma region Helper
				////////////////////////////////////////////////////////////////////////////////////////////////////
				/// <summary>	Initialises this object. Has to be called from GUI thread. </summary>
				///
				/// <remarks>	Perwass,. </remarks>
				///
				/// <exception cref="System">	Thrown when a system error condition occurs. </exception>
				///
				/// <param name="bShare">  	true to share. </param>
				/// <param name="hContext">	The context. </param>
				////////////////////////////////////////////////////////////////////////////////////////////////////

				void Control::_Init(bool bShare, __int64 hContext)
				{
					try
					{
						m_iHandle = 0;

						m_bIsDisposed = false;

						InitializeComponent();

						SetStyle(ControlStyles::Opaque | ControlStyles::ResizeRedraw | ControlStyles::UserPaint | ControlStyles::AllPaintingInWmPaint, true);
						SetStyle(ControlStyles::EnableNotifyMessage, true);
						SetStyle(ControlStyles::DoubleBuffer, false);
						SetStyle(ControlStyles::OptimizedDoubleBuffer, false);

						DoubleBuffered = false;
						FocusOnMouseOver = false;

						m_muxUserInput = gcnew System::Threading::Mutex(false);

						m_bIsDrawingEnabled = true;
						m_bMouseEventsFired = false;

						m_bIsSuspended = false;
						m_bMouseDown = false;
						m_bKeyPress = false;
						m_bCtrlDown = false;
						m_bShiftDown = false;
						m_bAltDown = false;
						m_bNumLock = false;
						m_bScrollLock = false;
						m_bCapsLock = false;


						m_xMouseEventArgs = gcnew MouseEventArgs();

						// We need this as member so that the garbage collector doesn't delete our message handling
						m_xMouseEventDelegatePointer = (System::Object^) gcnew Clu::Net::DelegateFunctionPointerCast<Clu::Viz::View::TFuncMouseEventCallback, NativeMouseEventHandler>(
							gcnew NativeMouseEventHandler(this, &Control::_OnNativeMouseEvent));

						int iHandle = 0;
						__int64 hGLRC = hContext;

						Clu::Viz::View::EmbedWindow(iHandle, this->Handle.ToInt64(), bShare, &hGLRC, false);

						m_hSharedContext = hGLRC;
						m_iHandle = iHandle;

						m_xScript = gcnew CluScript(m_iHandle);
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error initializing View Control")
				}


				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				System::String^ Control::_GetLastError(System::String^ sText)
				{
					char pcError[4096];
					Clu::Viz::View::GetLastError(pcError, 4096);
					return sText + " >> " + gcnew System::String(pcError);
				}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				bool Control::_TranslateControlKey(int % iCLUKey, System::Windows::Forms::Keys eKey)
				{
					switch (eKey)
					{
					case System::Windows::Forms::Keys::Back:                iCLUKey = CLUVIZ_KEY_BackSpace;
						break;
					case System::Windows::Forms::Keys::Tab:                 iCLUKey = CLUVIZ_KEY_Tab;
						break;
					case System::Windows::Forms::Keys::Enter:               iCLUKey = CLUVIZ_KEY_Enter;
						break;
					case System::Windows::Forms::Keys::Pause:               iCLUKey = CLUVIZ_KEY_Pause;
						break;
					case System::Windows::Forms::Keys::Scroll:              iCLUKey = CLUVIZ_KEY_Scroll_Lock;
						break;
					case System::Windows::Forms::Keys::Escape:              iCLUKey = CLUVIZ_KEY_Escape;
						break;
					case System::Windows::Forms::Keys::Home:                iCLUKey = CLUVIZ_KEY_Home;
						break;
					case System::Windows::Forms::Keys::Left:                iCLUKey = CLUVIZ_KEY_Left;
						break;
					case System::Windows::Forms::Keys::Up:                  iCLUKey = CLUVIZ_KEY_Up;
						break;
					case System::Windows::Forms::Keys::Right:               iCLUKey = CLUVIZ_KEY_Right;
						break;
					case System::Windows::Forms::Keys::Down:                iCLUKey = CLUVIZ_KEY_Down;
						break;
					case System::Windows::Forms::Keys::PageUp:              iCLUKey = CLUVIZ_KEY_Page_Up;
						break;
					case System::Windows::Forms::Keys::PageDown:            iCLUKey = CLUVIZ_KEY_Page_Down;
						break;
					case System::Windows::Forms::Keys::End:                 iCLUKey = CLUVIZ_KEY_End;
						break;
					case System::Windows::Forms::Keys::Print:               iCLUKey = CLUVIZ_KEY_Print;
						break;
					case System::Windows::Forms::Keys::Insert:              iCLUKey = CLUVIZ_KEY_Insert;
						break;
					case System::Windows::Forms::Keys::Menu:                iCLUKey = CLUVIZ_KEY_Menu;
						break;
					case System::Windows::Forms::Keys::Help:                iCLUKey = CLUVIZ_KEY_Help;
						break;
					case System::Windows::Forms::Keys::NumLock:             iCLUKey = CLUVIZ_KEY_Num_Lock;
						break;
					case System::Windows::Forms::Keys::NumPad0:             iCLUKey = CLUVIZ_KEY_KP + '0';
						break;
					case System::Windows::Forms::Keys::NumPad1:             iCLUKey = CLUVIZ_KEY_KP + '1';
						break;
					case System::Windows::Forms::Keys::NumPad2:             iCLUKey = CLUVIZ_KEY_KP + '2';
						break;
					case System::Windows::Forms::Keys::NumPad3:             iCLUKey = CLUVIZ_KEY_KP + '3';
						break;
					case System::Windows::Forms::Keys::NumPad4:             iCLUKey = CLUVIZ_KEY_KP + '4';
						break;
					case System::Windows::Forms::Keys::NumPad5:             iCLUKey = CLUVIZ_KEY_KP + '5';
						break;
					case System::Windows::Forms::Keys::NumPad6:             iCLUKey = CLUVIZ_KEY_KP + '6';
						break;
					case System::Windows::Forms::Keys::NumPad7:             iCLUKey = CLUVIZ_KEY_KP + '7';
						break;
					case System::Windows::Forms::Keys::NumPad8:             iCLUKey = CLUVIZ_KEY_KP + '8';
						break;
					case System::Windows::Forms::Keys::NumPad9:             iCLUKey = CLUVIZ_KEY_KP + '9';
						break;
					case System::Windows::Forms::Keys::Divide:              iCLUKey = CLUVIZ_KEY_KP + '/';
						break;
					case System::Windows::Forms::Keys::Multiply:            iCLUKey = CLUVIZ_KEY_KP + '*';
						break;
					case System::Windows::Forms::Keys::Subtract:            iCLUKey = CLUVIZ_KEY_KP + '-';
						break;
					case System::Windows::Forms::Keys::Add:                 iCLUKey = CLUVIZ_KEY_KP + '+';
						break;
					case System::Windows::Forms::Keys::Decimal:             iCLUKey = CLUVIZ_KEY_KP + '.';
						break;
					case System::Windows::Forms::Keys::F1:                  iCLUKey = CLUVIZ_KEY_F + 1;
						break;
					case System::Windows::Forms::Keys::F2:                  iCLUKey = CLUVIZ_KEY_F + 2;
						break;
					case System::Windows::Forms::Keys::F3:                  iCLUKey = CLUVIZ_KEY_F + 3;
						break;
					case System::Windows::Forms::Keys::F4:                  iCLUKey = CLUVIZ_KEY_F + 4;
						break;
					case System::Windows::Forms::Keys::F5:                  iCLUKey = CLUVIZ_KEY_F + 5;
						break;
					case System::Windows::Forms::Keys::F6:                  iCLUKey = CLUVIZ_KEY_F + 6;
						break;
					case System::Windows::Forms::Keys::F7:                  iCLUKey = CLUVIZ_KEY_F + 7;
						break;
					case System::Windows::Forms::Keys::F8:                  iCLUKey = CLUVIZ_KEY_F + 8;
						break;
					case System::Windows::Forms::Keys::F9:                  iCLUKey = CLUVIZ_KEY_F + 9;
						break;
					case System::Windows::Forms::Keys::F10:                 iCLUKey = CLUVIZ_KEY_F + 10;
						break;
					case System::Windows::Forms::Keys::F11:                 iCLUKey = CLUVIZ_KEY_F + 11;
						break;
					case System::Windows::Forms::Keys::F12:                 iCLUKey = CLUVIZ_KEY_F + 12;
						break;
					case System::Windows::Forms::Keys::LShiftKey:           iCLUKey = CLUVIZ_KEY_Shift_L;
						break;
					case System::Windows::Forms::Keys::RShiftKey:           iCLUKey = CLUVIZ_KEY_Shift_R;
						break;
					case System::Windows::Forms::Keys::LControlKey:         iCLUKey = CLUVIZ_KEY_Control_L;
						break;
					case System::Windows::Forms::Keys::RControlKey:         iCLUKey = CLUVIZ_KEY_Control_R;
						break;
					case System::Windows::Forms::Keys::CapsLock:            iCLUKey = CLUVIZ_KEY_Caps_Lock;
						break;
					case System::Windows::Forms::Keys::LWin:                iCLUKey = CLUVIZ_KEY_Meta_L;
						break;
					case System::Windows::Forms::Keys::RWin:                iCLUKey = CLUVIZ_KEY_Meta_R;
						break;
					case System::Windows::Forms::Keys::LMenu:               iCLUKey = CLUVIZ_KEY_Alt_L;
						break;
					case System::Windows::Forms::Keys::RMenu:               iCLUKey = CLUVIZ_KEY_Alt_R;
						break;
					case System::Windows::Forms::Keys::Delete:              iCLUKey = CLUVIZ_KEY_Delete;
						break;
					case System::Windows::Forms::Keys::ControlKey:          iCLUKey = CLUVIZ_KEY_Control_L;
						break;
					case System::Windows::Forms::Keys::ShiftKey:            iCLUKey = CLUVIZ_KEY_Shift_L;
						break;
					default:
						return false;
					}

					return true;
				}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				void Control::_UpdateModifierKeys()
				{
					try
					{
						bool bNumLock = this->IsKeyLocked(Keys::NumLock);
						bool bCapsLock = this->IsKeyLocked(Keys::CapsLock);
						bool bScrollLock = this->IsKeyLocked(Keys::Scroll);
						bool bCtrl = (this->ModifierKeys & Keys::Control) == Keys::Control;
						bool bShift = (this->ModifierKeys & Keys::Shift) == Keys::Shift;
						bool bAlt = (this->ModifierKeys & Keys::Alt) == Keys::Alt;

						if ((m_bCtrlDown && !bCtrl)
							|| (m_bShiftDown && !bShift)
							|| (m_bAltDown && !bAlt)
							|| (!m_bNumLock && bNumLock) || (m_bNumLock && !bNumLock)
							|| (!m_bCapsLock && bCapsLock) || (m_bCapsLock && !bCapsLock)
							|| (!m_bScrollLock && bScrollLock) || (m_bScrollLock && !bScrollLock))
						{
							Clu::Viz::View::EW_SetKeyModifierStates(m_iHandle, bShift, bCtrl, bAlt, bNumLock, bCapsLock, bScrollLock);

							m_bCtrlDown = bCtrl;
							m_bShiftDown = bShift;
							m_bAltDown = bAlt;

							m_bNumLock = bNumLock;
							m_bCapsLock = bCapsLock;
							m_bScrollLock = bScrollLock;
						}
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error updating modifier keys");
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				void Control::_SetShape()
				{
					try
					{
						if (this->Parent != nullptr)
						{
							if (!this->Enabled)
							{
								// Temporally enable visualization to resize control
								Clu::Viz::View::EnableVisualization(m_iHandle, true);
							}

							//this->Size = this->Parent->Size;
							Clu::Viz::View::EW_Reshape(m_iHandle, this->Parent->Width, this->Parent->Height);

							if (!this->Enabled)
							{
								// Disable visualization if enabled temporally
								Clu::Viz::View::EnableVisualization(m_iHandle, false);
							}
						}
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error setting shape of control")

				}



#pragma endregion
				// //////////////////////////////////////////////////////////////////////////////////
				// //////////////////////////////////////////////////////////////////////////////////
				// //////////////////////////////////////////////////////////////////////////////////

#pragma region Event Handler
				//try
				//{

				//}
				//CLU_CATCH_RETHROW_ALL_MANAGED(__FUNCTION__)

				////////////////////////////////////////////////////////////////////////////////////////////////////
				void Control::_OnNativeMouseEvent(Clu::Viz::View::SMouseEventData* pvData, void* pvContext)
				{
					try
					{
						m_xMouseEventArgs->bIsLeftButtonDown = pvData->bIsLeftButtonDown;
						m_xMouseEventArgs->bIsRightButtonDown = pvData->bIsRightButtonDown;
						m_xMouseEventArgs->bIsCtrlDown = pvData->bIsCtrlDown;
						m_xMouseEventArgs->bIsShiftDown = pvData->bIsShiftDown;
						m_xMouseEventArgs->bIsAltDown = pvData->bIsAltDown;

						m_xMouseEventArgs->bIsScenePickable = pvData->bIsScenePickable;
						m_xMouseEventArgs->sSceneNameList = gcnew System::String(pvData->pcSceneNameList);
						m_xMouseEventArgs->uScenePartId = pvData->uScenePartId;

						m_xMouseEventArgs->eMouseEventType = Clu::Viz::Net::View::MouseEventType(pvData->eMouseEventType);
						m_xMouseEventArgs->eMouseEventButton = Clu::Viz::Net::View::MouseEventButtonType(pvData->eMouseEventButton);

						m_xMouseEventArgs->iScreenPosX = pvData->iScreenPosX;
						m_xMouseEventArgs->iScreenPosY = pvData->iScreenPosY;
						m_xMouseEventArgs->iScreenPosDeltaX = pvData->iScreenPosDeltaX;
						m_xMouseEventArgs->iScreenPosDeltaY = pvData->iScreenPosDeltaY;

						m_xMouseEventArgs->vDragDataStart.Set(pvData->vDragDataStart.x(), pvData->vDragDataStart.y(), pvData->vDragDataStart.z());
						m_xMouseEventArgs->vDragData.Set(pvData->vDragData.x(), pvData->vDragData.y(), pvData->vDragData.z());
						m_xMouseEventArgs->vDragDataDelta.Set(pvData->vDragDataDelta.x(), pvData->vDragDataDelta.y(), pvData->vDragDataDelta.z());

						m_xMouseEventArgs->vMouseDragStart.Set(pvData->vMouseDragStart.x(), pvData->vMouseDragStart.y(), pvData->vMouseDragStart.z());
						m_xMouseEventArgs->vMouseDrag.Set(pvData->vMouseDrag.x(), pvData->vMouseDrag.y(), pvData->vMouseDrag.z());
						m_xMouseEventArgs->vMouseDragDelta.Set(pvData->vMouseDragDelta.x(), pvData->vMouseDragDelta.y(), pvData->vMouseDragDelta.z());

						m_xMouseEventArgs->vMouseDragLocalStart.Set(pvData->vMouseDragLocalStart.x(), pvData->vMouseDragLocalStart.y(), pvData->vMouseDragLocalStart.z());
						m_xMouseEventArgs->vMouseDragLocal.Set(pvData->vMouseDragLocal.x(), pvData->vMouseDragLocal.y(), pvData->vMouseDragLocal.z());
						m_xMouseEventArgs->vMouseDragLocalDelta.Set(pvData->vMouseDragLocalDelta.x(), pvData->vMouseDragLocalDelta.y(), pvData->vMouseDragLocalDelta.z());

						m_xMouseEventArgs->pNativeMouseEventData = (void*)pvData;

						m_xMouseEventArgs->eMouseEventReturnType = MouseEventReturnType::None;
						m_xMouseEventArgs->sExecScriptToolName = "";

						// Fire event and let handler process the event data
						SyncMouseEvent(this, m_xMouseEventArgs);

						// Get return type of the handled event data and see if the user has chosen a return action
						pvData->eMouseEventReturnType = Clu::Viz::View::EMouseEventReturnType(m_xMouseEventArgs->eMouseEventReturnType);

						// Get the tool name in case of the user has chosen the ExecScript return action
						strncpy_s(pvData->pcExecScriptToolName, Clu::Net::Converter::ToIString(m_xMouseEventArgs->sExecScriptToolName).ToCString(), _TRUNCATE);

						MouseEventHandler^ xDelegate = gcnew MouseEventHandler(this, &Control::_OnInvokedMouseEvent);
						BeginInvoke(xDelegate, this, m_xMouseEventArgs);

					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error in handling native mouse event")


				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				void Control::_OnInvokedMouseEvent(System::Object^ xSender, Clu::Viz::Net::View::MouseEventArgs^ xMouseEventArgs)
				{
					MouseEvent(xSender, xMouseEventArgs);
				}


#pragma endregion
				// //////////////////////////////////////////////////////////////////////////////////
				// //////////////////////////////////////////////////////////////////////////////////
				// //////////////////////////////////////////////////////////////////////////////////

#pragma region Event Handler Overrides

				//try
				//{

				//}
				//CLU_CATCH_RETHROW_ALL_MANAGED(__FUNCTION__)

				////////////////////////////////////////////////////////////////////////////////////////////////////
				void Control::OnEnabledChanged(System::EventArgs^ e)
				{
					try
					{
						UserControl::OnEnabledChanged(e);
						Clu::Viz::View::EnableVisualization(m_iHandle, this->Enabled);
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error during enabling/disabling")
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				void Control::OnResize(System::EventArgs^ e)
				{
					try
					{
						if (m_bIsSuspended)
							return;

						UserControl::OnResize(e);
						_SetShape();
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error resizing control")
				}


				////////////////////////////////////////////////////////////////////////////////////////////////////
				void Control::OnPaint(System::Windows::Forms::PaintEventArgs^ xArgs)
				{
					try
					{
						if (m_bIsSuspended)
							return;

						Clu::Viz::View::EW_Display(m_iHandle);
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error painting control")
				}


				////////////////////////////////////////////////////////////////////////////////////////////////////
				void Control::OnMouseEnter(System::EventArgs^ e)
				{
					m_muxUserInput->WaitOne();
					try
					{
						UserControl::OnMouseEnter(e);
						if (FocusOnMouseOver)
						{
							this->Focus();
						}
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error in Mouse Enter")
					finally
					{
						m_muxUserInput->ReleaseMutex();
					}
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				void Control::OnMouseLeave(System::EventArgs^ e)
				{
					m_muxUserInput->WaitOne();
					try
					{
						UserControl::OnMouseLeave(e);
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error in Mouse Leave")
						finally
					{
						m_muxUserInput->ReleaseMutex();
					}
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				void Control::OnGotFocus(System::EventArgs^ e)
				{
					m_muxUserInput->WaitOne();
					try
					{
						_UpdateModifierKeys();
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error in Got Focus")
						finally
					{
						m_muxUserInput->ReleaseMutex();
					}
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				void Control::OnLostFocus(System::EventArgs^ e)
				{
					m_muxUserInput->WaitOne();
					try
					{
						_UpdateModifierKeys();
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error in Lost Focus")
						finally
					{
						m_muxUserInput->ReleaseMutex();
					}
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				void Control::OnMouseDown(System::Windows::Forms::MouseEventArgs^ e)
				{
					m_muxUserInput->WaitOne();
					try
					{
						this->Focus();

						int iButton;
						if (e->Button == System::Windows::Forms::MouseButtons::Left)
						{
							iButton = CLUVIZ_MOUSE_LBUT;
						}
						else if (e->Button == System::Windows::Forms::MouseButtons::Right)
						{
							iButton = CLUVIZ_MOUSE_RBUT;
						}
						else if (e->Button == System::Windows::Forms::MouseButtons::Middle)
						{
							iButton = CLUVIZ_MOUSE_MBUT;
						}
						else
						{
							return;
						}

						_UpdateModifierKeys();

						m_bMouseDown = true;
						Clu::Viz::View::EW_Mouse(m_iHandle, iButton, CLUVIZ_MOUSE_DOWN, e->X, e->Y);

					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error in Mouse Down")
						finally
					{
						m_muxUserInput->ReleaseMutex();
					}
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				void Control::OnMouseUp(System::Windows::Forms::MouseEventArgs^ e)
				{
					m_muxUserInput->WaitOne();
					try
					{
						int iButton;
						if (e->Button == System::Windows::Forms::MouseButtons::Left)
						{
							iButton = CLUVIZ_MOUSE_LBUT;
						}
						else if (e->Button == System::Windows::Forms::MouseButtons::Right)
						{
							iButton = CLUVIZ_MOUSE_RBUT;
						}
						else if (e->Button == System::Windows::Forms::MouseButtons::Middle)
						{
							iButton = CLUVIZ_MOUSE_MBUT;
						}
						else
						{
							return;
						}

						_UpdateModifierKeys();

						m_bMouseDown = false;
						Clu::Viz::View::EW_Mouse(m_iHandle, iButton, CLUVIZ_MOUSE_UP, e->X, e->Y);
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error in Mouse Up")
						finally
					{
						m_muxUserInput->ReleaseMutex();
					}
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				void Control::OnMouseMove(System::Windows::Forms::MouseEventArgs^ e)
				{
					m_muxUserInput->WaitOne();
					try
					{
						if (m_bMouseDown)
						{
							//CLU_LOG("Mouse Move Active!  \n");
							Clu::Viz::View::EW_ActiveMouseMove(m_iHandle, e->X, e->Y);
						}
						else
						{
							//CLU_LOG("Mouse Move Passive!  \n");
							Clu::Viz::View::EW_PassiveMouseMove(m_iHandle, e->X, e->Y);
						}
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error in Mouse Move")
						finally
					{
						m_muxUserInput->ReleaseMutex();
					}
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				void Control::OnKeyDown(System::Windows::Forms::KeyEventArgs^ e)
				{
					m_muxUserInput->WaitOne();
					try
					{
						UserControl::OnKeyDown(e);

						bool bNumLock = this->IsKeyLocked(System::Windows::Forms::Keys::NumLock);
						bool bCapsLock = this->IsKeyLocked(System::Windows::Forms::Keys::CapsLock);
						bool bScrollLock = this->IsKeyLocked(System::Windows::Forms::Keys::Scroll);

						if ((!m_bCtrlDown && e->Control)
							|| (!m_bShiftDown && e->Shift)
							|| (!m_bAltDown && e->Alt)
							|| (!m_bNumLock && bNumLock) || (m_bNumLock && !bNumLock)
							|| (!m_bCapsLock && bCapsLock) || (m_bCapsLock && !bCapsLock)
							|| (!m_bScrollLock && bScrollLock) || (m_bScrollLock && !bScrollLock))
						{
							Clu::Viz::View::EW_SetKeyModifierStates(m_iHandle, e->Shift, e->Control, e->Alt, bNumLock, bCapsLock, bScrollLock);

							m_bCtrlDown = e->Control;
							m_bShiftDown = e->Shift;
							m_bAltDown = e->Alt;

							m_bNumLock = bNumLock;
							m_bCapsLock = bCapsLock;
							m_bScrollLock = bScrollLock;
						}

						int iCLUKey;
						if (_TranslateControlKey(iCLUKey, e->KeyCode))
						{
							Clu::Viz::View::EW_Keyboard(m_iHandle, iCLUKey, true);
							e->SuppressKeyPress = true;
						}

						Update();
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error in OnKeyDown")
						finally
					{
						m_muxUserInput->ReleaseMutex();
					}
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				void Control::OnKeyUp(System::Windows::Forms::KeyEventArgs^ e)
				{
					m_muxUserInput->WaitOne();
					try
					{
						UserControl::OnKeyUp(e);

						bool bNumLock = this->IsKeyLocked(System::Windows::Forms::Keys::NumLock);
						bool bCapsLock = this->IsKeyLocked(System::Windows::Forms::Keys::CapsLock);
						bool bScrollLock = this->IsKeyLocked(System::Windows::Forms::Keys::Scroll);

						if ((m_bCtrlDown && !e->Control)
							|| (m_bShiftDown && !e->Shift)
							|| (m_bAltDown && !e->Alt)
							|| (!m_bNumLock && bNumLock) || (m_bNumLock && !bNumLock)
							|| (!m_bCapsLock && bCapsLock) || (m_bCapsLock && !bCapsLock)
							|| (!m_bScrollLock && bScrollLock) || (m_bScrollLock && !bScrollLock))
						{
							Clu::Viz::View::EW_SetKeyModifierStates(m_iHandle, e->Shift, e->Control, e->Alt, bNumLock, bCapsLock, bScrollLock);

							m_bCtrlDown = e->Control;
							m_bShiftDown = e->Shift;
							m_bAltDown = e->Alt;

							m_bNumLock = bNumLock;
							m_bCapsLock = bCapsLock;
							m_bScrollLock = bScrollLock;
						}

						int iCLUKey;
						if (_TranslateControlKey(iCLUKey, e->KeyCode))
						{
							Clu::Viz::View::EW_Keyboard(m_iHandle, iCLUKey, false);
						}
						else if (m_bKeyPress)
						{
							m_bKeyPress = false;
							Clu::Viz::View::EW_Keyboard(m_iHandle, m_iLastKeyPress, false);
						}
	
						Update();
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error in OnKeyUp")
						finally
					{
						m_muxUserInput->ReleaseMutex();
					}
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				void Control::OnKeyPress(System::Windows::Forms::KeyPressEventArgs^ e)
				{
					m_muxUserInput->WaitOne();
					try
					{
						int iCLUKey;

						iCLUKey = int(e->KeyChar);
						Clu::Viz::View::EW_Keyboard(m_iHandle, iCLUKey, true);

						m_bKeyPress = true;
						m_iLastKeyPress = iCLUKey;
						e->Handled = true;
					}
					CLU_CATCH_RETHROW_ALL_LE_MANAGED("Error in OnKeyPress")
						finally
					{
						m_muxUserInput->ReleaseMutex();
					}
				}


				////////////////////////////////////////////////////////////////////////////////////////////////////
				void Control::WndProc(System::Windows::Forms::Message % m)
				{
					if (m.Msg == 0x0014)
					{
						return;
					}

					__super::WndProc(m);
				}
#pragma endregion


			} // namespace View
		} // namespace Net
	} // namespace Viz
} // namespace Clu
