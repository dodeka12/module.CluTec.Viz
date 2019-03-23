////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.View.Base
// file:      OGLWin_Keyboard.cpp
//
// summary:   Implements the ogl window keyboard class
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
#include "OGLWin.h"
#include <stdlib.h>
#include "CLUOutput.h"
#include "CluTec.Viz.Base\Environment.h"
#include "CluTec.Viz.OpenGL.Extensions\Extensions.h"
#include "CluTec.Viz.OpenGL\Api.h"

#include <string>
using namespace std;

/*
void COGLWin::CB_Quit(Fl_Widget* pWidget, void *pData)
{
        exit(0);
}
*/

void COGLWin::GetModifiers()
{
	if (!m_bIsEmbedded)
	{
		COGLWinFLTK::GetModifiers();
	}
}

//////////////////////////////////////////////////////////////////////
/// Check Key inputs

void COGLWin::Key(unsigned char cKey, int iX, int iY)
{
	/// Reload Script
/*
        if (cKey == 'u' || cKey == 'U')
        {
          m_bScriptLoaded = false;
          return;
        }
*/
	// Avoid ending program through 'q' keystroke.
/*
        if (cKey == 'q' || cKey == 'Q')
                return;


        COGLWinFLTK::Key(cKey, iX, iY);
*/
	char pcKey[2];

	pcKey[0] = cKey;
	pcKey[1] = 0;

	AddExecModeVar(EXEC_MODE_KEYUP);
	SetToolNameVar(pcKey);

	PostRedisplay();

	//if (!m_bIsAnimated)
	//	glutPostRedisplay();
}

void COGLWin::KeyUp(unsigned char cKey)
{
	if (m_iAllowKeyUp > 0)
	{
		char pcKey[2];

		pcKey[0] = cKey;
		pcKey[1] = 0;

		--m_iAllowKeyUp;

		AddExecModeVar(EXEC_MODE_KEYUP);
		SetToolNameVar(pcKey);

		//PassiveMouseMove( m_iMousePosX, m_iSizeY - m_iMousePosY );

		PostRedisplay();
		//if (!m_bIsAnimated)
		//	glutPostRedisplay();
	}
}

void COGLWin::KeyDown(unsigned char cKey)
{
	char pcKey[2];

	pcKey[0] = cKey;
	pcKey[1] = 0;

	// Key Up should only occur after Key Down
	++m_iAllowKeyUp;

	AddExecModeVar(EXEC_MODE_KEYDOWN);
	SetToolNameVar(pcKey);

	//PassiveMouseMove( m_iMousePosX, m_iSizeY - m_iMousePosY );
	PostRedisplay();

	//if (!m_bIsAnimated)
	//	glutPostRedisplay();
}

bool COGLWin::Keyboard(int iKey, bool bKeyDown)
{
	bool bIsFunctionKey = false;
	bool bIsControlKey  = false;

	// Read modifiers from FLTK if this is not an embedded window
	// Otherwise expect that the modifiers have been set by
	// EW_SetModiferKeys()
	if (!m_bIsEmbedded)
	{
		COGLWinFLTK::Keyboard(iKey, bKeyDown);
	}

	// Check whether key is a function key
	if ((iKey > CLU_KEY_F) && (iKey <= CLU_KEY_F_Last))
	{
		bIsFunctionKey = true;

		if (!m_bSendFunctionKeyEvents)
		{
			return false;
		}
	}
	else if (iKey >= 0xFF00)
	{
		bIsControlKey = true;

		// check the current active mode
		int iActMode = (m_iRTMode == 0) ? m_iStdRTMode : m_iRTMode;
		// if we are in RTView Mode (aka flightMode), process the key and return
		if (iActMode == 10)
		{
			return KeyViewRotateMode(iKey);
		}

		if (!m_bSendControlKeyEvents && bKeyDown)
		{
			return KeyRotTrans(iKey);
		}
		// Send a redisplay event if a control key is released.
		// This is needed to allow the scenes to update their pick scene.
		else if (!m_bSendControlKeyEvents && !bKeyDown)
		{
			PostRedisplay();
			return true;
		}
	}
	else if ((iKey >= 0) && (iKey <= 255))
	{
		char cKey        = char(iKey & 0xFF);
		bool bIsAlphaNum =
			((cKey >= 'a' && cKey <= 'z')
			 ||  (cKey >= 'A' && cKey <= 'Z')
			 ||  (cKey >= '1' && cKey <= '9')
			 ||      cKey == '0');

		if (!bKeyDown)
		{
			if ((bIsAlphaNum && !m_bAltDown && !m_bCtrlDown) || !bIsAlphaNum)
			{
				KeyUp((unsigned char) (iKey & 0xFF));
				return true;
			}
		}
		else
		{
			if ((bIsAlphaNum && !m_bAltDown && !m_bCtrlDown) || !bIsAlphaNum)
			{
				KeyDown((unsigned char) (iKey & 0xFF));
				return true;
			}
		}
		return false;
	}

	CStrMem sKey;
	bool bModifier = false;

	if (bIsFunctionKey)
	{
		char pcText[100];

		sprintf_s(pcText, 100, "F%d", int(iKey - CLU_KEY_F));
		sKey = pcText;
	}
	else if (bIsControlKey)
	{
		switch (iKey)
		{
		case CLU_KEY_BackSpace:
			sKey = "Backspace";
			break;

		case CLU_KEY_Tab:
			sKey = "Tabulator";
			break;

		case CLU_KEY_KP_Enter:
		case CLU_KEY_Enter:
			sKey = "Enter";
			break;

		case CLU_KEY_Pause:
			sKey = "Pause";
			break;

		case CLU_KEY_Scroll_Lock:
			sKey = "ScrollLock";
			break;

		case CLU_KEY_Escape:
			sKey = "Escape";
			break;

		case CLU_KEY_Home:
			sKey = "Home";
			break;

		case CLU_KEY_Left:
			sKey = "Left";
			break;

		case CLU_KEY_Up:
			sKey = "Up";
			break;

		case CLU_KEY_Right:
			sKey = "Right";
			break;

		case CLU_KEY_Down:
			sKey = "Down";
			break;

		case CLU_KEY_Page_Up:
			sKey = "PageUp";
			break;

		case CLU_KEY_Page_Down:
			sKey = "PageDown";
			break;

		case CLU_KEY_End:
			sKey = "End";
			break;

		case CLU_KEY_Print:
			sKey = "Print";
			break;

		case CLU_KEY_Insert:
			sKey = "Insert";
			break;

		case CLU_KEY_Delete:
			sKey = "Delete";
			break;

		case CLU_KEY_Help:
			sKey = "Help";
			break;

		case CLU_KEY_Num_Lock:
			sKey = "NumLock";
			break;

		case CLU_KEY_KP + '0':
			if (m_bNumLock) { sKey = "0"; }
			else{ sKey = "Insert"; }
			break;

		case CLU_KEY_KP + '1':
			if (m_bNumLock) { sKey = "1"; }
			else{ sKey = "End"; }
			break;

		case CLU_KEY_KP + '2':
			if (m_bNumLock) { sKey = "2"; }
			else{ sKey = "Down"; }
			break;

		case CLU_KEY_KP + '3':
			if (m_bNumLock) { sKey = "3"; }
			else{ sKey = "PageDown"; }
			break;

		case CLU_KEY_KP + '4':
			if (m_bNumLock) { sKey = "4"; }
			else{ sKey = "Left"; }
			break;

		case CLU_KEY_KP + '5':
			if (m_bNumLock) { sKey = "5"; }
			else{ sKey = "Center"; }
			break;

		case CLU_KEY_KP + '6':
			if (m_bNumLock) { sKey = "6"; }
			else{ sKey = "Right"; }
			break;

		case CLU_KEY_KP + '7':
			if (m_bNumLock) { sKey = "7"; }
			else{ sKey = "Home"; }
			break;

		case CLU_KEY_KP + '8':
			if (m_bNumLock) { sKey = "8"; }
			else{ sKey = "Up"; }
			break;

		case CLU_KEY_KP + '9':
			if (m_bNumLock) { sKey = "9"; }
			else{ sKey = "PageUp"; }
			break;

		case CLU_KEY_KP + '.':
			if (m_bNumLock) { sKey = "."; }
			else{ sKey = "Delete"; }
			break;

		case CLU_KEY_KP + '+':
			sKey = "+";
			break;

		case CLU_KEY_KP + '-':
			sKey = "-";
			break;

		case CLU_KEY_KP + '*':
			sKey = "*";
			break;

		case CLU_KEY_KP + '/':
			sKey = "/";
			break;

		// if shift, ctrl or alt are pressed post a
		// redisplay to allow scenes to update their pick scenes
		case CLU_KEY_Shift_L:
		case CLU_KEY_Shift_R:
			bModifier = true;
			sKey      = "Shift";
			break;

		case CLU_KEY_Control_L:
		case CLU_KEY_Control_R:
			bModifier = true;
			sKey      = "Control";
			break;

		case CLU_KEY_Alt_L:
		case CLU_KEY_Alt_R:
		case CLU_KEY_Menu:
			bModifier = true;
			sKey      = "Alt";
			break;

		default:
			return false;
		}
	}

	if (bKeyDown)
	{
		++m_iAllowKeyUp;
		AddExecModeVar(EXEC_MODE_KEYDOWN);
	}
	else
	{
		--m_iAllowKeyUp;
		AddExecModeVar(EXEC_MODE_KEYUP);
	}

	if (!bModifier)
	{
		if (m_bCtrlDown)
		{
			sKey += ":ctrl";
		}

		if (m_bShiftDown)
		{
			sKey += ":shift";
		}

		if (m_bAltDown)
		{
			sKey += ":alt";
		}
	}

	SetToolNameVar(sKey.Str());

	PostRedisplay();
	//if (!m_bIsAnimated)
	//	glutPostRedisplay();

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 	Process key events in ViewRotate Mode.
/// 	The Arrow Keys (up, down, left, right) are used to translate the camera transform into the corresponding view direction.
/// </summary>
///
/// <param name="iKey"> Zero-based index of the key. </param>
///
/// <returns> True if it succeeds, false if it fails. </returns>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool COGLWin::KeyViewRotateMode(int iKey)
{
	float fX    = 0, fY = 0;
	float fStep = 0.25;

	// Set the the desired translation depending on the pressed key
	if ((iKey == CLU_KEY_Left) || (iKey == (CLU_KEY_KP + '4')))
	{
		fX = -fStep;
	}
	else if ((iKey == CLU_KEY_Right) || (iKey == (CLU_KEY_KP + '6')))
	{
		fX = fStep;
	}
	else if ((iKey == CLU_KEY_Up) || (iKey == (CLU_KEY_KP + '8')))
	{
		fY = -fStep;
	}
	else if ((iKey == CLU_KEY_Down) || (iKey == (CLU_KEY_KP + '2')))
	{
		fY = fStep;
	}

	// Perform translation in transform class
	m_xCameraTransform.TranslateInViewDirection(fY);
	m_xCameraTransform.TranslateStrafeToViewDirection(fX);
	return true;
}

bool COGLWin::KeyRotTrans(int iKey)
{
	bool bKeyUsed = true;
	float fTrans  = 0.001f, fRot = 0.01f, fStep;
	float fX      = 0, fY = 0, fZ = 0, fAngle;

	LockVis();

	// See whether a scene is picked
	MemObj<COGLBEReference> mSceneRef = GetSceneElementList(m_SceneApplyData.vecCurPickUID);

	bool bDoDrag   = false;
	bool bDoRotate = false;
	COGLScene* pScene = nullptr;
	int iDragIdx = -1;

	// Check whether scene is draggable with current mouse button
	if (m_bShiftDown)
	{
		bDoRotate = true;
		iDragIdx  = (m_bCtrlDown ? 1 : 0);
	}
	else
	{
		bDoRotate = false;
		iDragIdx  = (m_bCtrlDown ? 3 : 2);
	}

	if (mSceneRef.Count())
	{
		int iScene, iSceneCnt = int(mSceneRef.Count());

		for (iScene = 0; iScene < iSceneCnt; ++iScene)
		{
			pScene = dynamic_cast<COGLScene*>((COGLBaseElement*) mSceneRef[iScene]);
			if (pScene->IsPickable() && pScene->IsDragEnabled(iDragIdx))
			{
				bDoDrag = true;
				break;
			}
		}
	}

	if (bDoRotate)
	{
		fStep = fRot;
	}
	else
	{
		fStep = fTrans;
	}

	if ((iKey == CLU_KEY_Left) || (iKey == (CLU_KEY_KP + '4')))
	{
		fX = -fStep;
	}
	else if ((iKey == CLU_KEY_Right) || (iKey == (CLU_KEY_KP + '6')))
	{
		fX = fStep;
	}
	else if ((iKey == CLU_KEY_Up) || (iKey == (CLU_KEY_KP + '8')))
	{
		fY = fStep;
	}
	else if ((iKey == CLU_KEY_Down) || (iKey == (CLU_KEY_KP + '2')))
	{
		fY = -fStep;
	}
	else if ((iKey == CLU_KEY_Home) || (iKey == (CLU_KEY_KP + '7')))
	{
		fZ = fStep;
	}
	else if ((iKey == CLU_KEY_End) || (iKey == (CLU_KEY_KP + '1')))
	{
		fZ = -fStep;
	}
	else
	{
		bKeyUsed = false;
	}

	if (!bDoDrag && bKeyUsed)
	{
		if (bDoRotate)
		{
			m_vxTransform[0].pfRot[0] += fX;
			m_vxTransform[0].pfRot[2] += fY;
			m_vxTransform[0].pfRot[1] += fZ;

			m_vU[5] = fX;
			m_vU[4] = -fY;
			m_vU[6] = fZ;

			fAngle   = float(sqrt(fX * fX + fY * fY + fZ * fZ)) * m_fRadPerDeg;
			m_vRMain = m_E3Base.Rotor(m_vU, fAngle, false) & m_vRMain;
		}
		else
		{
			m_vxTransform[0].pfTrans[0] += fX;
			m_vxTransform[0].pfTrans[1] += fY;
			m_vxTransform[0].pfTrans[2] += fZ;
		}
	}
	else if (bKeyUsed)	// && bDoDrag
	{
		if (bDoRotate)
		{
			if (m_b2DViewEnabled)
			{
				pScene->AddToDragData(iDragIdx, fX, fY, 0.0f);
			}
			else
			{
				pScene->AddToDragData(iDragIdx, fX, fZ, -fY);
			}
		}
		else
		{
			pScene->AddToDragData(iDragIdx, fX, fY, fZ);
		}

		if (pScene->DoNotifyMouseDrag())
		{
			float fDX, fDY, fDZ;
			double dMX, dMY, dMZ, dX, dY, dZ;

			pScene->GetDragData(iDragIdx, fDX, fDY, fDZ);
			m_pfDragDataStart[0] = fDX - fX;
			m_pfDragDataStart[1] = fDY - fY;
			m_pfDragDataStart[2] = fDZ - fZ;

			AddExecModeVar(EXEC_MODE_PICKDRAG);

			TVarList& rList = *m_xParse.GetVar("PickData").GetVarListPtr();
			rList.Set(0);
			AddSceneNameVarList(rList, mSceneRef);

			AddSceneDragDataList(rList, iDragIdx + 1, fDX, fDY, fDZ, fX, fY, fZ);

			pScene->ProjectScreenPos(0, 0, 0, dMX, dMY, dMZ);
			pScene->UnProjectScreenPos(m_iMousePosX, m_iMousePosY, dMZ, dX, dY, dZ);
			m_pdMouseDragStart[0] = 0;
			m_pdMouseDragStart[1] = 0;
			m_pdMouseDragStart[2] = 0;
			AddSceneDragMouseList(rList, dX, dY, dZ, 0, 0, 0);	//double(iX), double(iY), 0.0 );

			pScene->ATProjectScreenPos(0, 0, 0, dMX, dMY, dMZ);
			pScene->ATUnProjectScreenPos(m_iMousePosX, m_iMousePosY, dMZ, dX, dY, dZ);
			m_pdMouseDragLocalStart[0] = 0;
			m_pdMouseDragLocalStart[1] = 0;
			m_pdMouseDragLocalStart[2] = 0;
			AddSceneDragMouseLocalList(rList, dX, dY, dZ, 0, 0, 0);		//double(iX), double(iY), 0.0 );

			AddSceneDragEventList(rList, "drag");
		}
	}

	if (bKeyUsed ||
	    Fl::event_shift() ||
	    Fl::event_ctrl() ||
	    Fl::event_alt())
	{
		PostRedisplay();
		//if (!m_bIsAnimated)
		//	glutPostRedisplay();
	}

	UnlockVis();

	return bKeyUsed;
}
