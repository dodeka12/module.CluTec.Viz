////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.View.Base
// file:      OGLWin_Mouse.cpp
//
// summary:   Implements the ogl window mouse class
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

#include "StdAfx.h"
#include <stdlib.h>

#include "CluTec.Base/Exception.h"
#include "CluTec.Base/StdAlgo.h"

#include "CluTec.Viz.Base\Environment.h"
#include "CluTec.Viz.OpenGL.Extensions\Extensions.h"
#include "CluTec.Viz.OpenGL\Api.h"

#include "OGLWin.h"
#include "CLUOutput.h"


#include <string>
using namespace std;

#define CLU_LOG(theText, ...) __noop


//////////////////////////////////////////////////////////////////////
void COGLWin::TranslateMouseButtonState(Clu::Viz::View::EMouseEventButton& eMouseEventButton, Clu::Viz::View::EMouseEventType& eMouseEventType, int& iDragIdx, int iButton, int iState)
{
	// Init flags for left/right mouse button and dragging index
	if (iButton == CLU_LEFT_BUTTON())
	{
		iDragIdx          = (m_bCtrlDown ? 1 : 0);
		eMouseEventButton = Clu::Viz::View::EMouseEventButton::Left;
	}
	else if (iButton == CLU_RIGHT_BUTTON())
	{
		iDragIdx          = (m_bCtrlDown ? 3 : 2);
		eMouseEventButton = Clu::Viz::View::EMouseEventButton::Right;
	}
	else
	{
		iDragIdx          = -1;
		eMouseEventButton = Clu::Viz::View::EMouseEventButton::None;
	}

	if (iState == CLU_DOWN())
	{
		eMouseEventType = Clu::Viz::View::EMouseEventType::Down;
	}
	else if (iState == CLU_UP())
	{
		eMouseEventType = Clu::Viz::View::EMouseEventType::Up;
	}
	else
	{
		eMouseEventType = Clu::Viz::View::EMouseEventType::None;
	}
}

//////////////////////////////////////////////////////////////////////
void COGLWin::TranslateMouseButtonEvent(Clu::Viz::View::EMouseEventType& eMouseEventType, Clu::Viz::View::EMouseEventType eRawMouseEventType)
{
	__int64 liTimeCur;
	int iDeltaTime;
	float fMX, fMY, fMouseStep;

	if (eRawMouseEventType == Clu::Viz::View::EMouseEventType::Down)
	{
		//GETTIME( &m_TimeData );
		m_liTimeClick = GetTime64();

		m_iMouseStartX = m_iMousePosX;
		m_iMouseStartY = m_iMousePosY;

		eMouseEventType = Clu::Viz::View::EMouseEventType::Select;
	}
	else if (eRawMouseEventType == Clu::Viz::View::EMouseEventType::Up)
	{
		//GETTIME( &tmCur );
		liTimeCur  = GetTime64();
		iDeltaTime = int(liTimeCur - m_liTimeClick);

		fMX        = float(m_iMouseStartX - m_iMousePosX);
		fMY        = float(m_iMouseStartY - m_iMousePosY);
		fMouseStep = fMX * fMX + fMY * fMY;

		if ((iDeltaTime < 300) && (fMouseStep < 4.0f))
		{
			eMouseEventType = Clu::Viz::View::EMouseEventType::Click;
		}
		else
		{
			eMouseEventType = Clu::Viz::View::EMouseEventType::DragEnd;
		}
	}
	else
	{
		eMouseEventType = Clu::Viz::View::EMouseEventType::None;
	}
}

//////////////////////////////////////////////////////////////////////
bool COGLWin::HasPickableScene(const MemObj<COGLBEReference>& mSceneRef)
{
	COGLScene* pScene = 0;
	int iScene, iSceneCnt = int(mSceneRef.Count());

	for (iScene = 0; iScene < iSceneCnt; ++iScene)
	{
		pScene = dynamic_cast<COGLScene*>((COGLBaseElement*) mSceneRef[iScene]);
		if (pScene && pScene->IsPickable())
		{
			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////
COGLScene* COGLWin::SelectMouseEventScene(EPickSceneType& ePickSceneType, Clu::Viz::View::EMouseEventType eMouseEventType, int iDragIdx, const MemObj<COGLBEReference>& mSceneRef)
{
	// The picked scene is selected in the following order:
	// 1. The top tool scene
	// 2. The top notify click scene, if event is a click.
	// 3. The top notify drag scene, if event is select, drag or drag end and the drag is enabled for current drag index.
	// 4. The top drag scene, if the drag is enabled for current drag index.
	// 5. The top pick scene

	COGLTool* pTool              = nullptr;
	COGLScene* pScene            = nullptr;
	COGLScene* pSceneNotifyClick = nullptr;
	COGLScene* pSceneNotifyDrag  = nullptr;
	COGLScene* pSceneDrag        = nullptr;
	COGLScene* pScenePick        = nullptr;
	COGLScene* pSceneTool        = nullptr;
	int iScene, iSceneCnt = int(mSceneRef.Count());

	for (iScene = 0; iScene < iSceneCnt; ++iScene)
	{
		pScene = dynamic_cast<COGLScene*>((COGLBaseElement*) mSceneRef[iScene]);
		if (pScene && pScene->IsPickable())
		{
			if (pScenePick == 0)
			{
				pScenePick = pScene;
			}

			if ((pSceneNotifyClick == 0)
			    &&
			    (eMouseEventType == Clu::Viz::View::EMouseEventType::Click)
			    &&
			    pScene->DoNotifyMouseClick())
			{
				pSceneNotifyClick = pScene;
			}

			if ((pSceneNotifyDrag == 0)
			    &&
			    ((eMouseEventType == Clu::Viz::View::EMouseEventType::Select) ||
			     (eMouseEventType == Clu::Viz::View::EMouseEventType::Drag) ||
			     (eMouseEventType == Clu::Viz::View::EMouseEventType::DragEnd))
			    &&
			    ((pScene->DoNotifyMouseDrag() && pScene->IsDragEnabled(iDragIdx)) ||
			     pScene->IsAnimDragEnabled(iDragIdx)))
			{
				pSceneNotifyDrag = pScene;
			}

			if ((pSceneDrag == 0)
			    &&
			    ((eMouseEventType == Clu::Viz::View::EMouseEventType::Select) ||
			     (eMouseEventType == Clu::Viz::View::EMouseEventType::Drag) ||
			     (eMouseEventType == Clu::Viz::View::EMouseEventType::DragEnd))
			    &&
			    (pScene->IsDragEnabled(iDragIdx) || pScene->IsAnimDragEnabled(iDragIdx)))
			{
				pSceneDrag = pScene;
			}

			if ((pSceneTool == 0)
			    &&
			    ((pTool = dynamic_cast<COGLTool*>(pScene)) != 0)
			    &&
			    (pScene->IsDragEnabled(iDragIdx) || pScene->IsAnimDragEnabled(iDragIdx)))
			{
				pSceneTool = pScene;
				break;
			}
		}
	}

	if (pSceneTool)
	{
		pScene         = (COGLScene*) pSceneTool;
		ePickSceneType = PST_Tool;
	}
	else if (pSceneNotifyClick)
	{
		pScene         = pSceneNotifyClick;
		ePickSceneType = PST_NotifyClick;
	}
	else if (pSceneNotifyDrag)
	{
		pScene         = pSceneNotifyDrag;
		ePickSceneType = PST_NotifyDrag;
	}
	else if (pSceneDrag)
	{
		pScene         = pSceneDrag;
		ePickSceneType = PST_Drag;
	}
	else if (pScenePick)
	{
		pScene         = pScenePick;
		ePickSceneType = PST_Pick;
	}
	else
	{
		pScene         = nullptr;
		ePickSceneType = PST_None;
	}

	return pScene;
}

//////////////////////////////////////////////////////////////////////
/// Mouse 3D Move

void COGLWin::Mouse3D_Move(double dTX, double dTY, double dTZ, double dLength, double dRX, double dRY, double dRZ, double dAngle)
{
	COGLScene* pScene = nullptr;
	bool bDoDrag = false;

	// Do not process mouse events if drawing is disabled
	if (!m_bDoDisplaySceneGraph)
	{
		return;
	}

	LockVis();

	// See whether a scene is picked
	MemObj<COGLBEReference> mSceneRef = GetSceneElementList(m_SceneApplyData.vecCurPickUID);

	if (mSceneRef.Count())
	{
		int iScene, iSceneCnt = int(mSceneRef.Count());

		for (iScene = 0; iScene < iSceneCnt; ++iScene)
		{
			pScene = dynamic_cast<COGLScene*>((COGLBaseElement*) mSceneRef[iScene]);
			if (pScene->IsPickable() && pScene->IsDragEnabled())
			{
				bDoDrag = true;
				break;
			}
		}
	}

	// No Scene is picked that could be dragged
	if (!bDoDrag)
	{
		// Standard Mouse Move
		COGLWinFLTK::Mouse3D_Move(dTX, dTY, dTZ, dLength, dRX, dRY, dRZ, dAngle);
		UnlockVis();
		return;
	}

	pScene->Mouse3D_Move(dTX, dTY, dTZ, dLength, dRX, dRY, dRZ, dAngle);

	PostRedisplay();
	UnlockVis();
}

//////////////////////////////////////////////////////////////////////
// Check for mouse dragging while object is picked

void COGLWin::ActiveMouseMove(int iX, int iY)
{
	CLU_LOG("Clu ActiveMouseMove BEGIN %d : %d \n", iX, iY);
	// Do not process active mouse moves if drawing is disabled
	if (!m_bDoDisplaySceneGraph)
	{
		CLU_LOG("Dropped because of !m_bDoDisplaySceneGraph \n");
		return;
	}

	// Do not process active mouse move events if a mouse click event
	// still awaits processing
	if (m_bMouseClickEventNeedsProcessingByTimedFunc)
	{
		CLU_LOG("Dropped because of m_bMouseClickEventNeedsProcessingByTimedFunc \n");
		return;
	}

	// Do not process active mouse move events while a previous
	// Mouse Event callback is still waiting for the script to execute.
	if (m_bWaitForMouseEventExecScript)
	{
		CLU_LOG("Dropped because of m_bWaitForMouseEventExecScript \n");
		return;
	}

	LockVis();

	//double dX, dY, dZ;
	//int iSumX, iSumY;
	std::deque<int>::iterator itStep;

	m_iMousePosX = iX;
	m_iMousePosY = m_iSizeY - iY - 1;

	m_SceneApplyData.iMouseX = iX;
	m_SceneApplyData.iMouseY = m_iSizeY - iY - 1;

	int iStepX = iX - m_sMousePos.iX;
	int iStepY = iY - m_sMousePos.iY;

	if (!iStepX && !iStepY)
	{
		CLU_LOG("Dropped because of iStepX && iStepY == zero \n");
		UnlockVis();
		return;
	}

	bool bHasMouseEventCallback = (m_pMouseEventCallbackFunc != nullptr);

	m_xMouseEventData.iScreenPosX      = m_iMousePosX;
	m_xMouseEventData.iScreenPosY      = m_iMousePosY;
	m_xMouseEventData.iScreenPosDeltaX = iStepX;
	m_xMouseEventData.iScreenPosDeltaY = iStepY;

	m_xMouseEventData.bIsLeftButtonDown  = m_bMouseLDown;
	m_xMouseEventData.bIsRightButtonDown = m_bMouseRDown;
	m_xMouseEventData.bIsCtrlDown        = m_bCtrlDown;
	m_xMouseEventData.bIsShiftDown       = m_bShiftDown;
	m_xMouseEventData.bIsAltDown         = m_bAltDown;

	m_xMouseEventData.eMouseEventType   = Clu::Viz::View::EMouseEventType::None;
	m_xMouseEventData.eMouseEventButton = Clu::Viz::View::EMouseEventButton::None;

	m_xMouseEventData.eMouseEventReturnType   = Clu::Viz::View::EMouseEventReturnType::None;
	m_xMouseEventData.pcExecScriptToolName[0] = 0;

	// See whether a scene is picked
	MemObj<COGLBEReference> mSceneRef = GetSceneElementList(m_SceneApplyData.vecCurPickUID);

	COGLScene* pScene = 0;
	COGLTool* pTool   = 0;
	double dMX, dMY, dMZ;
	int iDragIdx                         = -1;
	bool bDoDrag                         = false;
	bool bIsScenePickable                = false;
	bool bDoPostRedisplay                = false;
	bool bDoMouseEventCallbackExecScript = false;
	Clu::Viz::View::EMouseEventButton eMouseButton;
	EPickSceneType ePickSceneType;

	// Check whether scene is dragable with current mouse button
	if (m_bMouseLDown && !m_bMouseRDown)
	{
		iDragIdx     = (m_bCtrlDown ? 1 : 0);
		eMouseButton = Clu::Viz::View::EMouseEventButton::Left;
	}
	else if (!m_bMouseLDown && m_bMouseRDown)
	{
		iDragIdx     = (m_bCtrlDown ? 3 : 2);
		eMouseButton = Clu::Viz::View::EMouseEventButton::Right;
	}
	else
	{
		iDragIdx     = -1;
		eMouseButton = Clu::Viz::View::EMouseEventButton::None;
	}

	// Only honor click on pick scene if this occurred with either left or right mouse button
	bIsScenePickable = HasPickableScene(mSceneRef) && (eMouseButton == Clu::Viz::View::EMouseEventButton::Left || eMouseButton == Clu::Viz::View::EMouseEventButton::Right);

	m_xMouseEventData.bIsScenePickable      = bIsScenePickable;
	m_xMouseEventData.bIsSceneTool          = false;
	m_xMouseEventData.bIsSceneDragEnabled   = false;
	m_xMouseEventData.bIsSceneNotifyEnabled = false;

	pScene = SelectMouseEventScene(ePickSceneType, Clu::Viz::View::EMouseEventType::Drag, iDragIdx, mSceneRef);

	CLU_LOG("bIsScenePickable: %d \n", bIsScenePickable);
	CLU_LOG("eMouseButton: %d \n", (int) eMouseButton);
	if (pScene != NULL)
	{
		CLU_LOG("scene Name: %s \n", pScene->GetName().c_str());
	}
	else
	{
		CLU_LOG("No scene picked, pScene is null");
	}

	CLU_LOG("m_bMouseLDown %s \n", m_bMouseLDown ? "true" : "false");
	CLU_LOG("m_bMouseRDown %s \n", m_bMouseRDown ? "true" : "false");

	if (bIsScenePickable)
	{
		bDoDrag                                 = pScene->IsDragEnabled(iDragIdx);
		m_xMouseEventData.bIsSceneDragEnabled   = (bDoDrag || pScene->IsAnimDragEnabled(iDragIdx));
		m_xMouseEventData.bIsSceneNotifyEnabled = ((pScene->DoNotifyMouseDrag() && pScene->IsDragEnabled(iDragIdx)) || pScene->DoNotifyMouseClick());
	}

	if (bHasMouseEventCallback)
	{
		if (bIsScenePickable)
		{
			strncpy_s(m_xMouseEventData.pcSceneNameList, CreateSceneNameString(mSceneRef, true).Str(), _TRUNCATE);
			m_xMouseEventData.uScenePartId = m_SceneApplyData.uCurPickPartID;

			float fDragX, fDragY, fDragZ, fDragDeltaX, fDragDeltaY, fDragDeltaZ;
			double dX, dY, dZ, dLX, dLY, dLZ;
			double dDX, dDY, dDZ, dLDX, dLDY, dLDZ;
			double dX2, dY2, dZ2;

			pScene->GetDragData(iDragIdx, fDragX, fDragY, fDragZ);
			pScene->GetDragDataStep(iDragIdx, fDragDeltaX, fDragDeltaY, fDragDeltaZ);

			pScene->ProjectScreenPos(0, 0, 0, dMX, dMY, dMZ);
			pScene->UnProjectScreenPos(m_iMousePosX, m_iMousePosY, dMZ, dX, dY, dZ);
			pScene->UnProjectScreenPos(m_iMousePosX - iStepX, m_iMousePosY + iStepY, dMZ, dX2, dY2, dZ2);
			dDX = dX - dX2;
			dDY = dY - dY2;
			dDZ = dZ - dZ2;

			pScene->ATProjectScreenPos(0, 0, 0, dMX, dMY, dMZ);
			pScene->ATUnProjectScreenPos(m_iMousePosX, m_iMousePosY, dMZ, dLX, dLY, dLZ);
			pScene->ATUnProjectScreenPos(m_iMousePosX - iStepX, m_iMousePosY + iStepY, dMZ, dX2, dY2, dZ2);
			dLDX = dLX - dX2;
			dLDY = dLY - dY2;
			dLDZ = dLZ - dZ2;

			m_xMouseEventData.vDragDataStart.x() = (double) m_pfDragDataStart[0];
			m_xMouseEventData.vDragDataStart.y() = (double) m_pfDragDataStart[1];
			m_xMouseEventData.vDragDataStart.z() = (double) m_pfDragDataStart[2];

			Clu::MemCpy<3>(&m_xMouseEventData.vMouseDragStart.x(), m_pdMouseDragStart);
			Clu::MemCpy<3>(&m_xMouseEventData.vMouseDragLocalStart.x(), m_pdMouseDragLocalStart);

			m_xMouseEventData.vDragData.x() = (double) fDragX;
			m_xMouseEventData.vDragData.y() = (double) fDragY;
			m_xMouseEventData.vDragData.z() = (double) fDragZ;

			m_xMouseEventData.vDragDataDelta.x() = (double) fDragDeltaX;
			m_xMouseEventData.vDragDataDelta.y() = (double) fDragDeltaY;
			m_xMouseEventData.vDragDataDelta.z() = (double) fDragDeltaZ;

			m_xMouseEventData.vMouseDrag.x() = dX;
			m_xMouseEventData.vMouseDrag.y() = dY;
			m_xMouseEventData.vMouseDrag.z() = dZ;

			m_xMouseEventData.vMouseDragDelta.x() = dDX;
			m_xMouseEventData.vMouseDragDelta.y() = dDY;
			m_xMouseEventData.vMouseDragDelta.z() = dDZ;

			m_xMouseEventData.vMouseDragLocal.x() = dLX;
			m_xMouseEventData.vMouseDragLocal.y() = dLY;
			m_xMouseEventData.vMouseDragLocal.z() = dLZ;

			m_xMouseEventData.vMouseDragLocalDelta.x() = dLDX;
			m_xMouseEventData.vMouseDragLocalDelta.y() = dLDY;
			m_xMouseEventData.vMouseDragLocalDelta.z() = dLDZ;

			pScene->GetProjectionMatrices((double*) m_xMouseEventData.matProjectionT.DataPointer(),
					(double*) m_xMouseEventData.matFrameT.DataPointer(),
					(double*) m_xMouseEventData.matFrameLocalT.DataPointer(),
					&m_xMouseEventData.vViewport.x());
		}
		else
		{
			m_xMouseEventData.pcSceneNameList[0] = 0;

			m_xMouseEventData.vDragDataStart.SetZero();
			m_xMouseEventData.vDragData.SetZero();
			m_xMouseEventData.vDragDataDelta.SetZero();

			m_xMouseEventData.vMouseDragStart.SetZero();
			m_xMouseEventData.vMouseDrag.SetZero();
			m_xMouseEventData.vMouseDragDelta.SetZero();

			m_xMouseEventData.vMouseDragLocalStart.SetZero();
			m_xMouseEventData.vMouseDragLocal.SetZero();
			m_xMouseEventData.vMouseDragLocalDelta.SetZero();
		}

		m_xMouseEventData.eMouseEventType = Clu::Viz::View::EMouseEventType::Drag;

		m_pMouseEventCallbackFunc(&m_xMouseEventData, m_pMouseEventCallbackContext);

		switch (m_xMouseEventData.eMouseEventReturnType)
		{
		case Clu::Viz::View::EMouseEventReturnType::None:
			break;

		case Clu::Viz::View::EMouseEventReturnType::PostRedisplay:
			bDoPostRedisplay = true;
			break;

		case Clu::Viz::View::EMouseEventReturnType::ExecScript:
			bDoMouseEventCallbackExecScript = true;
			break;
		}
	}

	CLU_LOG("m_bProcessStartDrag: %d    \t  bDoDrag: %d \n", m_bProcessStartDrag, bDoDrag);

	// Test whether the start drag message still has to be processed by the script
	if (!m_bProcessStartDrag && bDoDrag)
	{
		if (!m_bIsEmbedded)
		{
			GetModifiers();
		}

		m_sMousePos.iX = iX;
		m_sMousePos.iY = iY;

		CLU_LOG("Fire ActiveMouseMove for scene ! \n");
		pScene->ActiveMouseMove(iDragIdx, m_iMousePosX, m_iMousePosY, iStepX, iStepY,
				m_bShiftDown, m_bCtrlDown, m_bAltDown);

		if (pScene->DoNotifyMouseDrag())
		{
			double dX, dY, dZ, dLX, dLY, dLZ;
			double dDX, dDY, dDZ, dLDX, dLDY, dLDZ;
			double dX2, dY2, dZ2;
			float fX, fY, fZ, fDX, fDY, fDZ;

			// If the scene gets a notification then don't allow the mouse event callback function to
			// execute the script.
			bDoMouseEventCallbackExecScript = false;

			if (pTool)
			{
				pScene->ProjectScreenPos(0, 0, 0, dMX, dMY, dMZ);
				pScene->UnProjectScreenPos(m_iMousePosX, m_iMousePosY, dMZ, dX, dY, dZ);
				pScene->UnProjectScreenPos(m_iMousePosX - iStepX, m_iMousePosY + iStepY, dMZ, dX2, dY2, dZ2);
				dDX = dX - dX2;
				dDY = dY - dY2;
				dDZ = dZ - dZ2;

				pScene->ATProjectScreenPos(0, 0, 0, dMX, dMY, dMZ);
				pScene->ATUnProjectScreenPos(m_iMousePosX, m_iMousePosY, dMZ, dLX, dLY, dLZ);
				pScene->ATUnProjectScreenPos(m_iMousePosX - iStepX, m_iMousePosY + iStepY, dMZ, dX2, dY2, dZ2);
				dLDX = dLX - dX2;
				dLDY = dLY - dY2;
				dLDZ = dLZ - dZ2;

				int iButton = 0;
				if (m_bMouseLDown)
				{
					iButton |= TOOL_BUTTON_LEFT;
				}

				if (m_bMouseRDown)
				{
					iButton |= TOOL_BUTTON_RIGHT;
				}

				if (pTool->OnDrag(iButton, m_bShiftDown, m_bCtrlDown, m_bAltDown,
						    dX, dY, dZ, dDX, dDY, dDZ,
						    dLX, dLY, dLZ, dLDX, dLDY, dLDZ))
				{
					AddExecModeVar(EXEC_MODE_TOOL);
					SetToolNameVar(pTool->GetName().c_str());
				}
			}
			else
			{
				AddExecModeVar(EXEC_MODE_PICKDRAG);

				TVarList& rList = *m_xParse.GetVar("PickData").GetVarListPtr();
				rList.Set(0);
				AddScenePartName(rList, m_SceneApplyData.uCurPickPartID);
				AddSceneNameVarList(rList, mSceneRef);

				pScene->GetDragData(iDragIdx, fX, fY, fZ);
				pScene->GetDragDataStep(iDragIdx, fDX, fDY, fDZ);
				AddSceneDragDataList(rList, iDragIdx + 1, fX, fY, fZ, fDX, fDY, fDZ);

				pScene->ProjectScreenPos(0, 0, 0, dMX, dMY, dMZ);
				pScene->UnProjectScreenPos(m_iMousePosX, m_iMousePosY, dMZ, dX, dY, dZ);
				pScene->UnProjectScreenPos(m_iMousePosX - iStepX, m_iMousePosY + iStepY, dMZ, dX2, dY2, dZ2);
				AddSceneDragMouseList(rList, dX, dY, dZ, dX - dX2, dY - dY2, dZ - dZ2);	//double(iX), double(iY), 0.0 );

				pScene->ATProjectScreenPos(0, 0, 0, dMX, dMY, dMZ);
				pScene->ATUnProjectScreenPos(m_iMousePosX, m_iMousePosY, dMZ, dX, dY, dZ);
				pScene->ATUnProjectScreenPos(m_iMousePosX - iStepX, m_iMousePosY + iStepY, dMZ, dX2, dY2, dZ2);
				AddSceneDragMouseLocalList(rList, dX, dY, dZ, dX - dX2, dY - dY2, dZ - dZ2);	//double(iX), double(iY), 0.0 );

				AddSceneDragEventList(rList, "drag");
			}
		}

		bDoPostRedisplay = true;
	}	// if (!m_bProcessStartDrag && bDoDrag)

	if (!bDoDrag)
	{
		// Standard Mouse Move
		COGLWinFLTK::ActiveMouseMove(iX, iY);
	}

	if (!m_bProcessStartDrag && bDoMouseEventCallbackExecScript)
	{
		AddExecModeVar(EXEC_MODE_USER);
		SetToolNameVar(m_xMouseEventData.pcExecScriptToolName);
		bDoPostRedisplay = true;
	}

	if (bDoPostRedisplay)
	{
		PostRedisplay();
	}

	UnlockVis();

	// Read the last script output. If an error occurred, we need to throw the
	// Message in an exception
	std::vector<std::string> vMsg;
	bool bIsError = false;
	GetScriptOutput(vMsg, bIsError);

	if (bIsError)
	{
		// An error occurred
		std::stringstream sxMsg;
		for (unsigned int i = 0; i < vMsg.size(); i++)
		{
			sxMsg << vMsg[i].c_str();
			sxMsg << "\n";
		}

		throw CLU_EXCEPTION(sxMsg);
	}

}

//////////////////////////////////////////////////////////////////////
// React to Mouse click

void COGLWin::Mouse(int iButton, int iState, int iX, int iY)
{
	CLU_LOG("COGLWin::Mouse BEGIN \n");
	double dMX, dMY, dMZ, dLMX, dLMY, dLMZ;
	double dX, dY, dZ, dLX, dLY, dLZ;
	float fDragX, fDragY, fDragZ;

	COGLScene* pScene          = 0;
	COGLTool* pTool            = 0;
	bool bClickOnPick          = false;
	bool bUpdatePickList       = false;
	bool bNotifyClick          = false;
	bool bNotifyDrag           = false;
	bool bResetAnimDragOnClick = false;
	bool bDoPostRedisplay      = false;

	Clu::Viz::View::EMouseEventType eRawMouseEvent = Clu::Viz::View::EMouseEventType::None;
	Clu::Viz::View::EMouseEventType eMouseEvent    = Clu::Viz::View::EMouseEventType::None;
	Clu::Viz::View::EMouseEventButton eMouseButton = Clu::Viz::View::EMouseEventButton::None;
	EPickSceneType ePickSceneType              = PST_None;
	int iDragIdx                               = 0;

	// Do not process mouse events if drawing is disabled
	if (!m_bDoDisplaySceneGraph)
	{
		CLU_LOG("COGLWin::Mouse Dropped out because of !m_bDoDisplaySceneGraph \n");
		return;
	}

	LockVis();

	CCLUDrawBase::Mouse(iButton, iState, iX, iY);

	m_iMousePosX = iX;
	m_iMousePosY = m_iSizeY - iY - 1;

	bool bHasMouseEventCallback = (m_pMouseEventCallbackFunc != nullptr);
	m_xMouseEventData.iScreenPosX        = m_iMousePosX;
	m_xMouseEventData.iScreenPosY        = m_iMousePosY;
	m_xMouseEventData.iScreenPosDeltaX   = 0;
	m_xMouseEventData.iScreenPosDeltaY   = 0;
	m_xMouseEventData.bIsLeftButtonDown  = m_bMouseLDown;
	m_xMouseEventData.bIsRightButtonDown = m_bMouseRDown;

	TVarList& rPickDataVarList        = *m_xParse.GetVar("PickData").GetVarListPtr();
	MemObj<COGLBEReference> mSceneRef = GetSceneElementList(m_SceneApplyData.vecCurPickUID);

	// /////////////////////////////////////////////////////////////////////////
	// /////////////////////////////////////////////////////////////////////////
	// Analyze Mouse Event state and set appropriate flags
	if (!m_bIsEmbedded)
	{
		GetModifiers();
	}

	TranslateMouseButtonState(eMouseButton, eRawMouseEvent, iDragIdx, iButton, iState);
	m_xMouseEventData.eMouseEventButton = eMouseButton;

	// Only honor click on pick scene if this occurred with either left or right mouse button
	bClickOnPick = HasPickableScene(mSceneRef) && (eMouseButton == Clu::Viz::View::EMouseEventButton::Left || eMouseButton == Clu::Viz::View::EMouseEventButton::Right);

	// Find out whether we have a drag start, drag, drag end or click
	if (!bClickOnPick)
	{
		m_xMouseEventData.bIsScenePickable      = false;
		m_xMouseEventData.bIsSceneTool          = false;
		m_xMouseEventData.bIsSceneDragEnabled   = false;
		m_xMouseEventData.bIsSceneNotifyEnabled = false;
		m_xMouseEventData.uScenePartId          = (unsigned) -1;
		m_xMouseEventData.bIsShiftDown          = m_bShiftDown;
		m_xMouseEventData.bIsCtrlDown           = m_bCtrlDown;
		m_xMouseEventData.bIsAltDown            = m_bAltDown;
	}
	else
	{
		// Translate a mouse up/down into select, drag end or click
		TranslateMouseButtonEvent(eMouseEvent, eRawMouseEvent);

		// Get the picked scene with highest priority
		pScene = SelectMouseEventScene(ePickSceneType, eMouseEvent, iDragIdx, mSceneRef);

		if (ePickSceneType == PST_Tool)
		{
			pTool = dynamic_cast<COGLTool*>(pScene);
		}

		bNotifyClick          = pScene->DoNotifyMouseClick();
		bNotifyDrag           = (pScene->DoNotifyMouseDrag() && pScene->IsDragEnabled(iDragIdx));
		bResetAnimDragOnClick = pScene->IsAnimDragActive(iDragIdx);

		m_xMouseEventData.bIsScenePickable      = true;
		m_xMouseEventData.bIsSceneTool          = (ePickSceneType == PST_Tool);
		m_xMouseEventData.bIsSceneDragEnabled   = (pScene->IsDragEnabled(iDragIdx) || pScene->IsAnimDragEnabled(iDragIdx));
		m_xMouseEventData.bIsSceneNotifyEnabled = (bNotifyDrag || bNotifyClick);
		m_xMouseEventData.uScenePartId          = (unsigned) -1;

		pScene->ProjectScreenPos(0, 0, 0, dMX, dMY, dMZ);
		pScene->UnProjectScreenPos(m_iMousePosX, m_iMousePosY, dMZ, dX, dY, dZ);

		pScene->ATProjectScreenPos(0, 0, 0, dLMX, dLMY, dLMZ);
		pScene->ATUnProjectScreenPos(m_iMousePosX, m_iMousePosY, dLMZ, dLX, dLY, dLZ);

		pScene->GetDragData(iDragIdx, fDragX, fDragY, fDragZ);

		if (eRawMouseEvent == Clu::Viz::View::EMouseEventType::Down)
		{
			// Store drag start
			m_pdMouseDragStart[0] = dX;
			m_pdMouseDragStart[1] = dY;
			m_pdMouseDragStart[2] = dZ;

			m_pdMouseDragLocalStart[0] = dLX;
			m_pdMouseDragLocalStart[1] = dLY;
			m_pdMouseDragLocalStart[2] = dLZ;

			m_pfDragDataStart[0] = fDragX;
			m_pfDragDataStart[1] = fDragY;
			m_pfDragDataStart[2] = fDragZ;

			// Remember selected scenes
			m_SceneApplyData.vecCurPickSelUID  = m_SceneApplyData.vecCurPickUID;
			m_SceneApplyData.uCurPickPartSelID = m_SceneApplyData.uCurPickPartID;

			m_SceneApplyData.iMouseStartX = m_iMousePosX;
			m_SceneApplyData.iMouseStartY = m_iMousePosY;
			m_SceneApplyData.iMousePrevX  = m_iMousePosX;
			m_SceneApplyData.iMousePrevY  = m_iMousePosY;
			m_SceneApplyData.bDoDrag      = true;

			pScene->SetMouseDragStartWorld(dX, dY, dZ);
			pScene->SetMouseDragStartLocal(dLX, dLY, dLZ);

			bUpdatePickList = true;
		}

		if (eMouseEvent == Clu::Viz::View::EMouseEventType::Select)
		{
			if (bNotifyDrag)
			{
				// Ensure that fast clicking did not add EXEC_MODE_PICKSEL in a previous step
				RemExecModeVar(EXEC_MODE_PICKSEL);

				if (pTool)
				{
					// the scene is a tool.

					int iBut = 0;
					if (eMouseButton == Clu::Viz::View::EMouseEventButton::Left)
					{
						iBut |= TOOL_BUTTON_LEFT;
					}
					else
					{
						iBut |= TOOL_BUTTON_RIGHT;
					}

					// Give info to tool
					if (pTool->OnDragStart(iBut, m_bShiftDown, m_bCtrlDown, m_bAltDown,
							    dX, dY, dZ, dLX, dLY, dLZ))
					{
						AddExecModeVar(EXEC_MODE_TOOL);
						SetToolNameVar(pTool->GetName().c_str());
					}
				}
				else
				{
					float fX, fY, fZ;
					AddExecModeVar(EXEC_MODE_PICKDRAG);

					//TVarList &rList = *m_xParse.GetVar("PickData").GetVarListPtr();
					rPickDataVarList.Set(0);
					AddScenePartName(rPickDataVarList, m_SceneApplyData.uCurPickPartID);

					AddSceneNameVarList(rPickDataVarList, mSceneRef);

					pScene->GetDragData(iDragIdx, fX, fY, fZ);
					AddSceneDragDataList(rPickDataVarList, iDragIdx + 1, fX, fY, fZ, 0, 0, 0);

					//pScene->ProjectScreenPos( 0, 0, 0, dMX, dMY, dMZ );
					//pScene->UnProjectScreenPos( m_iMousePosX, m_iMousePosY, dMZ, dX, dY, dZ );
					AddSceneDragMouseList(rPickDataVarList, dX, dY, dZ, 0, 0, 0);		//double(iX), double(iY), 0.0 );

					//pScene->ATProjectScreenPos( 0, 0, 0, dLMX, dLMY, dLMZ );
					//pScene->ATUnProjectScreenPos( m_iMousePosX, m_iMousePosY, dMZ, dLX, dLY, dLZ );
					AddSceneDragMouseLocalList(rPickDataVarList, dLX, dLY, dLZ, 0, 0, 0);		//double(iX), double(iY), 0.0 );

					AddSceneDragEventList(rPickDataVarList, "start");
				}

				m_bProcessPickHits = false;
				// Wait with active mouse move until the start message is processed
				m_bProcessStartDrag = true;

				bDoPostRedisplay = true;
			}		// if bNotifyDrag
		}
		else if (eMouseEvent == Clu::Viz::View::EMouseEventType::DragEnd)
		{
			if (bNotifyDrag)
			{
				if (pTool)
				{
					// the scene is a tool.

					int iBut = 0;
					if (eMouseButton == Clu::Viz::View::EMouseEventButton::Left)
					{
						iBut |= TOOL_BUTTON_LEFT;
					}
					else
					{
						iBut |= TOOL_BUTTON_RIGHT;
					}

					// Give info to tool
					if (pTool->OnDragEnd(iBut, m_bShiftDown, m_bCtrlDown, m_bAltDown,
							    dX, dY, dZ, dLX, dLY, dLZ))
					{
						AddExecModeVar(EXEC_MODE_TOOL);
						SetToolNameVar(pTool->GetName().c_str());
					}
				}
				else
				{
					float fX, fY, fZ;
					pScene->GetDragData(iDragIdx, fX, fY, fZ);

					AddExecModeVar(EXEC_MODE_PICKDRAG);

					//TVarList &rList = *m_xParse.GetVar("PickData").GetVarListPtr();
					rPickDataVarList.Set(0);
					AddScenePartName(rPickDataVarList, m_SceneApplyData.uCurPickPartID);
					AddSceneNameVarList(rPickDataVarList, mSceneRef);

					AddSceneDragDataList(rPickDataVarList, iDragIdx + 1, fX, fY, fZ, 0, 0, 0);

					//pScene->ProjectScreenPos( 0, 0, 0, dMX, dMY, dMZ );
					//pScene->UnProjectScreenPos( m_iMousePosX, m_iMousePosY, dMZ, dX, dY, dZ );
					AddSceneDragMouseList(rPickDataVarList, dX, dY, dZ, 0, 0, 0);		//double(iX), double(iY), 0.0 );

					//pScene->ATProjectScreenPos( 0, 0, 0, dLMX, dLMY, dLMZ );
					//pScene->ATUnProjectScreenPos( m_iMousePosX, m_iMousePosY, dMZ, dLX, dLY, dLZ );
					AddSceneDragMouseLocalList(rPickDataVarList, dLX, dLY, dLZ, 0, 0, 0);		//double(iX), double(iY), 0.0 );

					AddSceneDragEventList(rPickDataVarList, "end");
				}

				m_bProcessPickHits = false;
				bDoPostRedisplay   = true;
			}		// if bNotifyDrag
		}
		else if (eMouseEvent == Clu::Viz::View::EMouseEventType::Click)
		{
			if (bResetAnimDragOnClick)
			{
				pScene->ResetAnimDrag(iDragIdx);
			}

			if (bNotifyClick)
			{
				// Ensure that fast clicking did not add EXEC_MODE_PICKDRAG in a previous step
				RemExecModeVar(EXEC_MODE_PICKDRAG);

				if (pTool)
				{
					// the scene is a tool.

					int iBut = 0;
					if (eMouseButton == Clu::Viz::View::EMouseEventButton::Left)
					{
						iBut |= TOOL_BUTTON_LEFT;
					}
					else
					{
						iBut |= TOOL_BUTTON_RIGHT;
					}

					// Give info to tool
					if (pTool->OnClick(iBut, m_bShiftDown, m_bCtrlDown, m_bAltDown,
							    dX, dY, dZ, dLX, dLY, dLZ))
					{
						AddExecModeVar(EXEC_MODE_TOOL);
						SetToolNameVar(pTool->GetName().c_str());
					}
				}
				else
				{
					AddExecModeVar(EXEC_MODE_PICKSEL);

					rPickDataVarList.Set(0);

					AddScenePartName(rPickDataVarList, m_SceneApplyData.uCurPickPartID);
					AddSceneNameVarList(rPickDataVarList, mSceneRef);

					// Mouse Position
					{
						rPickDataVarList.Add(1);
						rPickDataVarList.Last().New(PDT_VARLIST, "Constant");
						TVarList& rSubList = *rPickDataVarList.Last().GetVarListPtr();

						rSubList.Set(2);
						rSubList(0) = "pos";
						rSubList(1).New(PDT_VARLIST, "Constant");
						TVarList& rElList = *rSubList(1).GetVarListPtr();

						rElList.Set(3);
						rElList(0) = TCVScalar(dX);
						rElList(1) = TCVScalar(dY);
						rElList(2) = TCVScalar(dZ);
					}

					// Local Mouse Position
					{
						rPickDataVarList.Add(1);
						rPickDataVarList.Last().New(PDT_VARLIST, "Constant");
						TVarList& rSubList = *rPickDataVarList.Last().GetVarListPtr();

						rSubList.Set(2);
						rSubList(0) = "local";
						rSubList(1).New(PDT_VARLIST, "Constant");
						TVarList& rElList = *rSubList(1).GetVarListPtr();

						rElList.Set(3);
						rElList(0) = TCVScalar(dLX);
						rElList(1) = TCVScalar(dLY);
						rElList(2) = TCVScalar(dLZ);
					}

					// Mouse Button
					{
						rPickDataVarList.Add(1);
						rPickDataVarList.Last().New(PDT_VARLIST, "Constant");
						TVarList& rSubList = *rPickDataVarList.Last().GetVarListPtr();

						rSubList.Set(2);
						rSubList(0) = "click";

						if (eMouseButton == Clu::Viz::View::EMouseEventButton::Left)
						{
							rSubList(1) = "left";
						}
						else
						{
							rSubList(1) = "right";
						}
					}

					// Shift
					{
						rPickDataVarList.Add(1);
						rPickDataVarList.Last().New(PDT_VARLIST, "Constant");
						TVarList& rSubList = *rPickDataVarList.Last().GetVarListPtr();

						rSubList.Set(2);
						rSubList(0) = "shift";

						if (m_bShiftDown)
						{
							rSubList(1) = TCVCounter(1);
						}
						else
						{
							rSubList(1) = TCVCounter(0);
						}
					}

					// CTRL
					{
						rPickDataVarList.Add(1);
						rPickDataVarList.Last().New(PDT_VARLIST, "Constant");
						TVarList& rSubList = *rPickDataVarList.Last().GetVarListPtr();

						rSubList.Set(2);
						rSubList(0) = "ctrl";

						if (m_bCtrlDown)
						{
							rSubList(1) = TCVCounter(1);
						}
						else
						{
							rSubList(1) = TCVCounter(0);
						}
					}

					// Alt
					{
						rPickDataVarList.Add(1);
						rPickDataVarList.Last().New(PDT_VARLIST, "Constant");
						TVarList& rSubList = *rPickDataVarList.Last().GetVarListPtr();

						rSubList.Set(2);
						rSubList(0) = "alt";

						if (m_bAltDown)
						{
							rSubList(1) = TCVCounter(1);
						}
						else
						{
							rSubList(1) = TCVCounter(0);
						}
					}
				}

				m_bProcessPickHits = false;
			}
			bDoPostRedisplay = true;
		}
	}		// if ( bClickOnPick)

	if (eRawMouseEvent == Clu::Viz::View::EMouseEventType::Up)
	{
		m_SceneApplyData.vecCurPickSelUID.clear();
		m_SceneApplyData.bDoDrag = false;

		bUpdatePickList = true;
	}

	if (bUpdatePickList)
	{
		m_iMousePosX = iX;
		m_iMousePosY = m_iSizeY - iY - 1;

		m_SceneApplyData.iMouseX = iX;
		m_SceneApplyData.iMouseY = m_iSizeY - iY - 1;
		m_SceneApplyData.iPickW  = 2;
		m_SceneApplyData.iPickH  = 2;

		m_iPickX = iX;
		m_iPickY = iY;

		m_bDoPicking = true;

		// Ensure that new pick state is evaluated before drawing, one time.
		if (m_bProcessPickHits)
		{
			m_bPickBeforeDrawOnce = true;
		}

		// Ensure that this mouse event is processed by the timed function before
		// allowing any other mouse events
		m_bMouseClickEventNeedsProcessingByTimedFunc = true;

		bDoPostRedisplay = true;
	}

	if (bHasMouseEventCallback)
	{
		bool bDoMouseEventCallbackExecScript = false;
		m_xMouseEventData.eMouseEventType = eMouseEvent;

		if (m_xMouseEventData.bIsScenePickable)
		{
			strncpy_s(m_xMouseEventData.pcSceneNameList, CreateSceneNameString(mSceneRef, true).Str(), _TRUNCATE);
			m_xMouseEventData.uScenePartId = m_SceneApplyData.uCurPickPartID;

			m_xMouseEventData.vDragDataStart.x() = (double) m_pfDragDataStart[0];
			m_xMouseEventData.vDragDataStart.y() = (double) m_pfDragDataStart[1];
			m_xMouseEventData.vDragDataStart.z() = (double) m_pfDragDataStart[2];

			Clu::MemCpy<3>(&m_xMouseEventData.vMouseDragStart.x(), m_pdMouseDragStart);
			Clu::MemCpy<3>(&m_xMouseEventData.vMouseDragLocalStart.x(), m_pdMouseDragLocalStart);

			m_xMouseEventData.vDragData.x() = (double) fDragX;
			m_xMouseEventData.vDragData.y() = (double) fDragY;
			m_xMouseEventData.vDragData.z() = (double) fDragZ;

			m_xMouseEventData.vDragDataDelta.x() = (double) 0.0;
			m_xMouseEventData.vDragDataDelta.y() = (double) 0.0;
			m_xMouseEventData.vDragDataDelta.z() = (double) 0.0;

			m_xMouseEventData.vMouseDrag.x() = dX;
			m_xMouseEventData.vMouseDrag.y() = dY;
			m_xMouseEventData.vMouseDrag.z() = dZ;

			m_xMouseEventData.vMouseDragDelta.x() = 0.0;
			m_xMouseEventData.vMouseDragDelta.y() = 0.0;
			m_xMouseEventData.vMouseDragDelta.z() = 0.0;

			m_xMouseEventData.vMouseDragLocal.x() = dLX;
			m_xMouseEventData.vMouseDragLocal.y() = dLY;
			m_xMouseEventData.vMouseDragLocal.z() = dLZ;

			m_xMouseEventData.vMouseDragLocalDelta.x() = 0.0;
			m_xMouseEventData.vMouseDragLocalDelta.y() = 0.0;
			m_xMouseEventData.vMouseDragLocalDelta.z() = 0.0;

			pScene->GetProjectionMatrices((double*) m_xMouseEventData.matProjectionT.DataPointer(),
					(double*) m_xMouseEventData.matFrameT.DataPointer(),
					(double*) m_xMouseEventData.matFrameLocalT.DataPointer(),
					&m_xMouseEventData.vViewport.x());
		}
		else
		{
			m_xMouseEventData.pcSceneNameList[0] = 0;
			m_xMouseEventData.uScenePartId       = (unsigned) -1;

			m_xMouseEventData.vDragDataStart.SetZero();
			m_xMouseEventData.vDragData.SetZero();
			m_xMouseEventData.vDragDataDelta.SetZero();

			m_xMouseEventData.vMouseDragStart.SetZero();
			m_xMouseEventData.vMouseDrag.SetZero();
			m_xMouseEventData.vMouseDragDelta.SetZero();

			m_xMouseEventData.vMouseDragLocalStart.SetZero();
			m_xMouseEventData.vMouseDragLocal.SetZero();
			m_xMouseEventData.vMouseDragLocalDelta.SetZero();
		}

		m_bMouseCallbackActive = true;
		m_pMouseEventCallbackFunc(&m_xMouseEventData, m_pMouseEventCallbackContext);
		m_bMouseCallbackActive = false;

		switch (m_xMouseEventData.eMouseEventReturnType)
		{
		case Clu::Viz::View::EMouseEventReturnType::None:
			break;

		case Clu::Viz::View::EMouseEventReturnType::PostRedisplay:
			bDoPostRedisplay = true;
			break;

		case Clu::Viz::View::EMouseEventReturnType::ExecScript:
			bDoMouseEventCallbackExecScript = true;
			break;
		}

		if (bDoMouseEventCallbackExecScript && !bNotifyClick && !bNotifyDrag)
		{
			AddExecModeVar(EXEC_MODE_USER);
			SetToolNameVar(m_xMouseEventData.pcExecScriptToolName);
			bDoPostRedisplay = true;
		}
	}

	UnlockVis();

	// We want to force the timed_function to run immediately. This will cause a click command to select the underlying scene object to be
	// picked.
	// If we do not force the timed_function to run NOW, its possible to miss mouveMove events after this click.
	if (bDoPostRedisplay)
	{
		PostRedisplay(true, true, true);
	}

	CLU_LOG("COGLWin::Mouse END \n");
}

//////////////////////////////////////////////////////////////////////
// Check for mouse moving in OGL Window

void COGLWin::PassiveMouseMove(int iX, int iY)
{
	// Do not process passive mouse moves if drawing is disabled
	if (!m_bDoDisplaySceneGraph)
	{
		return;
	}

	// Do not process active mouse move events if a mouse click event
	// still awaits processing
	if (m_bMouseClickEventNeedsProcessingByTimedFunc)
	{
		return;
	}

	if (!m_bProcessPickHits)
	{
		return;
	}

	LockVis();

	m_iMousePosX = iX;
	m_iMousePosY = m_iSizeY - iY - 1;

	CLU_LOG("Mouse Pos: %d, %d\n", m_iMousePosX, m_iMousePosY);

	m_SceneApplyData.iMouseX = iX;
	m_SceneApplyData.iMouseY = m_iSizeY - iY - 1;
	m_SceneApplyData.iPickW  = 2;
	m_SceneApplyData.iPickH  = 2;

	if (m_SceneApplyData.vecCurPickSelUID.size())
	{
		m_SceneApplyData.vecCurPickUID  = m_SceneApplyData.vecCurPickSelUID;
		m_SceneApplyData.uCurPickPartID = m_SceneApplyData.uCurPickPartSelID;

		#ifdef CLU_LOG_ACTIVE
			CStrMem sData;
			for (unsigned i = 0; i < m_SceneApplyData.vecCurPickUID.size(); ++i)
			{
				sData << m_SceneApplyData.vecCurPickUID[i] << ", ";
			}

			CLU_LOG("uCurPickPartID, vecCurPickUID: %d -- %s\n", m_SceneApplyData.uCurPickPartID, sData.Str());
		#endif
	}
	else
	{
		m_bDoPicking = true;
		m_iPickX     = iX;
		m_iPickY     = iY;

		CLU_LOG("Do Picking at: %d, %d\n", m_iPickX, m_iPickY);
	}
	UnlockVis();
}

//////////////////////////////////////////////////////////////////////
/// Draw for picking

bool COGLWin::PickDraw()
{
	m_SceneApplyData.InitPickNames();
	return m_pMainScene->Apply(COGLBaseElement::PICK, m_SceneApplyData);
}

//////////////////////////////////////////////////////////////////////
// Process the picking data

bool COGLWin::PickProcessHits(EMousePickType ePickType, uint uPickColor, float fPickMinDepth)
{
	int iNameCnt;
	uint uPickNameStackIdx, uPartName;

	// Convert the pick color into the index of the pick name stack.
	// If the index is zero, then nothing has been picked.
	bool bValidPick = m_SceneApplyData.ConvertColorToName(uPickNameStackIdx, uPartName, uPickColor);

	CLU_LOG("Color, Base, Part, Valid: %08X, %d, %d, %s\n", uPickColor, uPickNameStackIdx, uPartName, bValidPick ? "TRUE" : "FALSE");
	CLU_LOG("Pick Stack Idx/Next Idx: %d, %d\n", uPickNameStackIdx, m_SceneApplyData.uNextPickNameStackListIdx);
	CLU_LOG("Cur. Pick Count: %d\n", m_mCurPick.Count());

	if (((uPickNameStackIdx >= m_SceneApplyData.uNextPickNameStackListIdx) && (m_mCurPick.Count() == 0)))
	{
		CLU_LOG("EXIT 1");
		return false;
	}

	if (bValidPick && m_SceneApplyData.LoadPickNameStack(uPickNameStackIdx))
	{
		iNameCnt = (int) m_SceneApplyData.uNextPickNameStackIdx;
		if (iNameCnt > m_SceneApplyData.cuMaxPickNameStackLen)
		{
			iNameCnt = m_SceneApplyData.cuMaxPickNameStackLen;
		}
	}
	else
	{
		iNameCnt  = 0;
		uPartName = 0;
	}

	bool bNotify     = false, bPartIdChanged = false;
	bool bExecScript = false, bRedraw = false;
	int iCurListCnt;
	float fMinPickDepth = 2.0f;

	bool bHadPick = m_bHasPick;
	m_bHasPick = false;

	if (m_bProcessPickHits)
	{
		if (iNameCnt != m_mCurSelBuf.Count())
		{
			bNotify = true;
		}
		else
		{
			if ((memcmp(m_SceneApplyData.vecPickNameStack.data(), m_mCurSelBuf.Data(), iNameCnt * sizeof(uint)) != 0))
			{
				bNotify = true;
			}

			if (m_uCurSelPartId != uPartName)
			{
				bNotify        = true;
				bPartIdChanged = true;
			}
		}

		m_mCurSelBuf.Set(iNameCnt);
	}
	m_uCurSelPartId = (unsigned) -1;
	if (iNameCnt > 0)
	{
		memcpy(m_mCurSelBuf.Data(), m_SceneApplyData.vecPickNameStack.data(), iNameCnt * sizeof(uint));
		m_uCurSelPartId = uPartName;
	}

	if (!bNotify)
	{
		m_bHasPick = bHadPick;
	}
	else
	{
		COGLBEReference ElRef;
		MemObj<COGLBEReference> mSceneRef;
		COGLScene* pScene = 0;

		m_mCurPick.Set(1);
		m_SceneApplyData.vecCurPickUID.clear();
		m_SceneApplyData.vecCurPickDrawFlag.clear();
		m_SceneApplyData.uCurPickPartID = 0x000FFFFF;

		TVarList& rList = *m_xParse.GetVar("PickData").GetVarListPtr();
		iCurListCnt = int(rList.Count());
		rList.Set(0);

		SPickData& rPick = m_mCurPick[0];

		rPick.uPartID = 0;
		rPick.vecID.resize(iNameCnt);
		if (iNameCnt > 0)
		{
			rPick.fMinDepth = fPickMinDepth;
			rPick.uPartID   = uPartName;
			memcpy(rPick.vecID.data(), m_SceneApplyData.vecPickNameStack.data(), iNameCnt * sizeof(uint));
		}
		else
		{
			rPick.fMinDepth = 0.0f;
		}

		if (rPick.fMinDepth < fMinPickDepth)
		{
			fMinPickDepth                   = rPick.fMinDepth;
			m_SceneApplyData.uCurPickPartID = rPick.uPartID;
			m_SceneApplyData.vecCurPickUID  = rPick.vecID;
		}

		mSceneRef = GetSceneElementList(m_SceneApplyData.vecCurPickUID);
		int iScene, iSceneCnt = int(mSceneRef.Count());
		TString sSelSceneName;

		for (iScene = 0; iScene < iSceneCnt; ++iScene)
		{
			pScene = dynamic_cast<COGLScene*>((COGLBaseElement*) mSceneRef[iScene]);
			if (pScene && pScene->IsPickable() && pScene->DoNotifyMouseOver())
			{
				sSelSceneName = AddSceneNameVarList(rList, mSceneRef);
				AddScenePartName(rList, uPartName);
				if (m_sPickedSceneName != sSelSceneName)
				{
					m_sPickedSceneName         = sSelSceneName;
					m_SceneApplyData.dPickTime = 0.0;
					bExecScript                = true;
					bRedraw                    = true;
				}
				else if (bPartIdChanged)
				{
					bExecScript = true;
					bRedraw     = true;
				}
				m_bHasPick = true;
				break;
			}
			else if (pScene && pScene->IsPickable())
			{
				sSelSceneName = pScene->GetName().c_str();
				if (m_sPickedSceneName != sSelSceneName)
				{
					m_sPickedSceneName         = sSelSceneName;
					m_SceneApplyData.dPickTime = 0.0;
					bRedraw                    = true;
				}
				else if (bPartIdChanged)
				{
					bRedraw = true;
				}
				m_bHasPick = true;
				break;
			}
		}

		if (!m_bHasPick)
		{
			m_SceneApplyData.vecCurPickUID.clear();
			m_SceneApplyData.uCurPickPartID = 0x000FFFFF;
			m_sPickedSceneName              = "";
		}

		m_SceneApplyData.vecCurPickDrawFlag.resize(m_SceneApplyData.vecCurPickUID.size());

		// Fill with 0
		std::fill(m_SceneApplyData.vecCurPickDrawFlag.begin(), m_SceneApplyData.vecCurPickDrawFlag.end(), 0);

		if ((rList.Count() == 0) && (iCurListCnt != 0))
		{
			bExecScript = true;
			bRedraw     = true;
		}

		if (bHadPick && !m_bHasPick)
		{
			bRedraw = true;
		}

		if (bExecScript)
		{
			COGLTool* pTool = dynamic_cast<COGLTool*>(pScene);
			if (pTool)
			{
				if (pTool->OnMouseOver(m_bShiftDown, m_bCtrlDown, m_bAltDown))
				{
					AddExecModeVar(EXEC_MODE_TOOL);
					SetToolNameVar(pTool->GetName().c_str());
				}
			}
			else
			{
				AddExecModeVar(EXEC_MODE_PICKOVER);
			}
		}

		if (bRedraw)
		{
			PostRedisplay();
		}
	}

	return bRedraw || m_bIsAnimated;
}
