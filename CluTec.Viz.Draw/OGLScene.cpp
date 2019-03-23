////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLScene.cpp
//
// summary:   Implements the ogl scene class
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

//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "OGLScene.h"
#include "OGLFrame.h"
#include "CluTec.Base/Exception.h"
#include "CluTec.Viz.OpenGL.Extensions\Extensions.h"
#include "CluTec.Viz.OpenGL/Api.h"

#include "CluTec.Viz.Fltk\Fl_math.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLScene::COGLScene()
{
	m_sTypeName = "Scene";

	m_mAllowDrag.Set(4);
	m_mScreenPlaneDrag.Set(4);
	m_mProjDirDrag.Set(4);

	m_mAnimDragData.Set(4);
	m_mAnimDragAttenuation.Set(4);
	m_mAnimDragEnable.Set(4);
	m_mAnimDragActive.Set(4);

	m_mDragData.Set(4);
	m_mDragDataStep.Set(4);
	m_mDragFactor.Set(4);
	m_mDragBasis.Set(4);
	m_mDragRange.Set(4);
	m_mUseDragRange.Set(4);
	m_mRMain.Set(4);

	m_mPickSceneRef.Set(16);
	m_mIntroPickSceneRef.Set(16);
	m_mOutroPickSceneRef.Set(16);

	m_mSinglePickSceneRef.Set(16);
	m_mSingleIntroPickSceneRef.Set(16);
	m_mSingleOutroPickSceneRef.Set(16);

	m_mIntroPickTimer.Set(16);
	m_mOutroPickTimer.Set(16);
	memset(m_mIntroPickTimer.Data(), 0, 16 * sizeof(double));
	memset(m_mOutroPickTimer.Data(), 0, 16 * sizeof(double));

	m_mIntroPickTimeFactor.Set(16);
	m_mOutroPickTimeFactor.Set(16);
	memset(m_mIntroPickTimeFactor.Data(), 0, 16 * sizeof(double));
	memset(m_mOutroPickTimeFactor.Data(), 0, 16 * sizeof(double));

	m_iIntroPickIdx = -1;
	m_iOutroPickIdx = -1;

	m_dIntroTimer = 0.0;
	m_dOutroTimer = 0.0;

	m_dIntroTimeFactor = 1.0;
	m_dOutroTimeFactor = 1.0;

	memset(m_pdMouseProjMat, 0, 16 * sizeof(GLdouble));
	memset(m_pdMouseFrameMat, 0, 16 * sizeof(GLdouble));
	memset(m_pdATMouseFrameMat, 0, 16 * sizeof(GLdouble));
	memset(m_pdARMouseFrameMat, 0, 16 * sizeof(GLdouble));
	memset(m_piMouseViewport, 0, 4 * sizeof(GLint));
	memset(m_piViewport, 0, 4 * sizeof(GLint));

	// By default PickUID contains the scene's UID
	m_mapPickUID[GetUID()] = 1;

	COGLVertex vexX(1.0f, 0.0f, 0.0f);
	COGLVertex vexY(0.0f, 1.0f, 0.0f);
	COGLVertex vexZ(0.0f, 0.0f, 1.0f);

	m_bProjMatCurrent  = false;
	m_bFrameMatCurrent = false;
	m_bViewportCurrent = false;

	for (int i = 0; i < 4; i++)
	{
		m_mAllowDrag[i]       = false;
		m_mScreenPlaneDrag[i] = false;
		m_mProjDirDrag[i]     = false;

		m_mAnimDragData[i].Set(0.0f, 0.0f, 0.0f);
		m_mAnimDragAttenuation[i].Set(0.9f, 0.9f, 0.9f);
		m_mAnimDragEnable[i] = false;
		m_mAnimDragActive[i] = false;

		m_mDragData[i].Set(0.0f, 0.0f, 0.0f);

		//if (i > 1)
		//	m_mDragFactor[ i ].Set( 0.01f, 0.01f, 0.01f );
		//else
		m_mDragFactor[i].Set(1.0f, 1.0f, 1.0f);

		m_mDragBasis[i].Set(vexX, vexY, vexZ);

		m_mDragRange[i].Set(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		m_mUseDragRange[i] = false;

		m_mRMain[i] = m_E3Base.vSC;
	}

	m_bAutoAdaptFrontFace = false;
	m_bIsPickable         = false;
	m_bDoPickDraw         = true;
	m_bDoNotify           = false;
	m_bDoNotifyMouseOver  = false;
	m_bDoNotifyMouseClick = false;
	m_bDoNotifyMouseDrag  = false;

	m_bAutoAdjustFrame           = false;
	m_bAutoTranslate             = false;
	m_bAutoRotate1               = false;
	m_bAutoRotate2               = false;
	m_bAutoScale                 = false;
	m_bAutoPixelZoom             = false;
	m_bAutoScaleAboutLocalOrigin = false;
	m_bAutoScaleAboutMouseOrigin = false;

	m_bAdaptiveRedraw = false;
	m_bHasSceneImage  = false;

	m_bDrawOpaque      = true;
	m_bDrawTransparent = true;

	m_eScaleFunc = /*EScaleFunc::*/ LINEAR;

	m_iDragIdxScale     = 3;
	m_iDragIdxRotate1   = 1;
	m_iDragIdxTranslate = 2;
	m_iDragIdxRotate2   = 0;

	m_bLocalFrame = true;
	m_bLocalView  = false;
	m_bLocalProj  = false;

	m_bUseViewScissor = true;

	m_bResetFrame   = false;
	m_bPickableView = false;
	m_bDrawScene    = true;

	m_bPickIntro = false;
	m_bPickOutro = false;

	m_bNormalize = false;

	m_eProjType                 = /*EProjType::*/ CENTRAL;
	m_bOrthoViewportAspectRatio = false;

	m_iRepeatCount    = 1;
	m_iCurPickIdx     = -1;
	m_iPickIdx        = -1;
	m_bCurPickChanged = false;

	m_bIsPicked     = false;
	m_uPickDrawFlag = 0;

	m_dDepthNear = 0.0;
	m_dDepthFar  = 1.0;

	m_bNeedAnimate = false;

	m_bUseLocalTime = false;
	m_dLocalTime    = 0.0;

	m_Perspective.Set(50.0f, 1.0f, -1.0f, 100.0f);
	m_OrthoProj.Set(0.0f, 100.0f, 100.0f, 0.0f, 0.0f, 10.0f);
	m_Viewport.Set(0.0f, 0.0f, 1.0f, 1.0f, true, true, true, true);

	// Depth level data
	// The viewport depth level.
	// Gives relative or absolute depth level.
	// Positive depth levels lie in front of standard scene
	m_Viewport.dDepthLevel = 1.0;
	// DL_Relative: Depth level is relative to previous scene
	// DL_Absolute: Depth level is absolute
	// DL_Local: Depth level depends on local depth coordinate
	m_Viewport.eDepthLevelType = SViewport::DL_Relative;
	// Range of local depth variable that is mapped to
	// depth level range
	m_Viewport.dDLLocalNear = 1.0, m_Viewport.dDLLocalFar = -1.0;
	// Range of depth levels to which local depth value range is mapped
	m_Viewport.dDLNear = 10.0, m_Viewport.dDLFar = -10.0;

	m_vU    = m_E3Base.vSC;
	m_vAxis = m_E3Base.vSC;

	m_vU    = 0;
	m_vAxis = 0;

	m_fPi        = float(asin(1.0) * 2.0);
	m_fRadPerDeg = m_fPi / 180.0f;

	m_uDispListID = 0;

	m_bMouseMoveSmooth     = true;
	m_iMouseMoveQueueLen   = 5;
	m_fMouseMoveSmoothTime = 0.05f;

	m_AutoScale.Set(0.0f, 0.0f, 0.0f);
	m_AutoScaleOffset.Set(0.0f, 0.0f, 0.0f);

	memset(m_pdMouseDragStartWorld, 0, sizeof(double));
	memset(m_pdMouseDragStartLocal, 0, sizeof(double));
}

COGLScene::COGLScene(const COGLScene& rList)
{
	m_sTypeName = "Scene";

	*this = rList;
}

COGLScene& COGLScene::operator=(const COGLScene& rList)
{
	if (&rList == this)
	{
		return *this;
	}

	COGLBaseElementList::operator=(rList);

	int i;
	m_mPickSceneRef.Set(8);
	m_mSinglePickSceneRef.Set(8);
	for (i = 0; i < 8; ++i)
	{
		m_mPickSceneRef[i]       = rList.m_mPickSceneRef[i].Copy();
		m_mSinglePickSceneRef[i] = rList.m_mSinglePickSceneRef[i].Copy();
	}

	m_refAutoFrame         = rList.m_refAutoFrame;
	m_refFrameContextScene = rList.m_refFrameContextScene;
	m_refFrameContextUser  = rList.m_refFrameContextUser;

	m_mAllowDrag       = rList.m_mAllowDrag;
	m_mScreenPlaneDrag = rList.m_mScreenPlaneDrag;
	m_mProjDirDrag     = rList.m_mProjDirDrag;

	m_mAnimDragData        = rList.m_mAnimDragData;
	m_mAnimDragAttenuation = rList.m_mAnimDragAttenuation;
	m_mAnimDragEnable      = rList.m_mAnimDragEnable;
	m_mAnimDragActive      = rList.m_mAnimDragActive;

	m_mDragData     = rList.m_mDragData;
	m_mDragFactor   = rList.m_mDragFactor;
	m_mDragRange    = rList.m_mDragRange;
	m_mUseDragRange = rList.m_mUseDragRange;

	m_bDrawOpaque      = rList.m_bDrawOpaque;
	m_bDrawTransparent = rList.m_bDrawTransparent;

	// Copy Pick UID set
	m_mapPickUID = rList.m_mapPickUID;
	// Erase Pick UID of rList
	m_mapPickUID.erase(rList.GetUID());
	// Insert this UID
	m_mapPickUID[GetUID()];

	m_bAutoAdaptFrontFace = rList.m_bAutoAdaptFrontFace;
	m_bIsPickable         = rList.m_bIsPickable;
	m_bDoNotify           = rList.m_bDoNotify;
	m_bDoNotifyMouseOver  = rList.m_bDoNotifyMouseOver;
	m_bDoNotifyMouseClick = rList.m_bDoNotifyMouseClick;
	m_bDoNotifyMouseDrag  = rList.m_bDoNotifyMouseDrag;

	m_bAutoAdjustFrame           = rList.m_bAutoAdjustFrame;
	m_bAutoTranslate             = rList.m_bAutoTranslate;
	m_bAutoRotate1               = rList.m_bAutoRotate1;
	m_bAutoRotate2               = rList.m_bAutoRotate2;
	m_bAutoScale                 = rList.m_bAutoScale;
	m_bAutoPixelZoom             = rList.m_bAutoPixelZoom;
	m_bAutoScaleAboutLocalOrigin = rList.m_bAutoScaleAboutLocalOrigin;
	m_bAutoScaleAboutMouseOrigin = rList.m_bAutoScaleAboutMouseOrigin;

	m_bAdaptiveRedraw = rList.m_bAdaptiveRedraw;
	m_bHasSceneImage  = false;
	m_imgScene.ResizeImage(1, 1);

	m_eScaleFunc = rList.m_eScaleFunc;

	m_iDragIdxScale     = rList.m_iDragIdxScale;
	m_iDragIdxRotate1   = rList.m_iDragIdxRotate1;
	m_iDragIdxTranslate = rList.m_iDragIdxTranslate;
	m_iDragIdxRotate2   = rList.m_iDragIdxRotate2;

	m_bLocalFrame = rList.m_bLocalFrame;
	m_bLocalView  = rList.m_bLocalView;
	m_bLocalProj  = rList.m_bLocalProj;

	m_bResetFrame   = rList.m_bResetFrame;
	m_bPickableView = rList.m_bPickableView;
	m_bDrawScene    = rList.m_bDrawScene;

	m_bNormalize = rList.m_bNormalize;

	m_eProjType                 = rList.m_eProjType;
	m_bOrthoViewportAspectRatio = rList.m_bOrthoViewportAspectRatio;

	m_iRepeatCount    = rList.m_iRepeatCount;
	m_iCurPickIdx     = -1;
	m_bCurPickChanged = false;

	m_bNeedAnimate = rList.m_bNeedAnimate;

	m_bUseLocalTime = rList.m_bUseLocalTime;
	m_dLocalTime    = rList.m_dLocalTime;

	m_Perspective = rList.m_Perspective;
	m_OrthoProj   = rList.m_OrthoProj;
	m_Viewport    = rList.m_Viewport;

	m_mRMain = rList.m_mRMain;
	m_vU     = rList.m_vU;
	m_vAxis  = rList.m_vAxis;

	m_fPi        = rList.m_fPi;
	m_fRadPerDeg = rList.m_fRadPerDeg;

	m_uDispListID = rList.m_uDispListID;

	m_qMouseStepX = rList.m_qMouseStepX;
	m_qMouseStepY = rList.m_qMouseStepY;

	m_bMouseMoveSmooth     = rList.m_bMouseMoveSmooth;
	m_iMouseMoveQueueLen   = rList.m_iMouseMoveQueueLen;
	m_fMouseMoveSmoothTime = rList.m_fMouseMoveSmoothTime;

	for (int i = 0; i < 3; ++i)
	{
		m_pdMouseDragStartWorld[i] = rList.m_pdMouseDragStartWorld[i];
		m_pdMouseDragStartLocal[i] = rList.m_pdMouseDragStartLocal[i];
	}

	return *this;
}

COGLScene::~COGLScene()
{
	if (m_uDispListID)
	{
		glDeleteLists(m_uDispListID, 1);
	}
}

//////////////////////////////////////////////////////////////////////
// Function called when mouse movement influences scene

bool COGLScene::ActiveMouseMove(int iDragIdx, int iMouseX, int iMouseY, int iStepX, int iStepY, bool bShiftDown, bool bCtrlDown, bool bAltDown)
{
	if ((iDragIdx < 0) || !IsDragEnabled(iDragIdx))
	{
		return false;
	}

	// Get current time
	double dTime;
	TTimeStruct tmCur;
	GETTIME(&tmCur);

	dTime = double(tmCur.time) + 1e-3 * double(tmCur.millitm);

	double dMouseX = double(iMouseX);
	double dMouseY = double(iMouseY);
	double dStepX  = double(iStepX);
	double dStepY  = double(iStepY);
	float fStepX   = float(iStepX);
	float fStepY   = float(iStepY);

/*
                m_fAnimStepX = float( iStepX );
                m_fAnimStepY = float( iStepY );
                float fAnimVal = m_fAnimStepX*m_fAnimStepX + m_fAnimStepY*m_fAnimStepY;

                if ( fAnimVal > 1.0f )
                {
                                m_bAnimActiveMouse = true;
                }
                else if (fAnimVal < 0.5f )
                {
                                m_bAnimActiveMouse = false;
                }
*/

	// If mouse movement smoothing is enabled, do it!
	if (m_bMouseMoveSmooth)
	{
		m_qMouseStepX.push_front(fStepX);
		m_qMouseStepY.push_front(fStepY);
		m_qMouseTime.push_front(dTime);

		if (int(m_qMouseStepX.size()) > m_iMouseMoveQueueLen)
		{
			m_qMouseStepX.pop_back();
			m_qMouseStepY.pop_back();
			m_qMouseTime.pop_back();
		}

		int iStep, iStepCnt = int(m_qMouseStepX.size());
		float fSumX, fSumY;
		float fFactor, fFactorSum = 0.0f;
		float fVar = m_fMouseMoveSmoothTime * m_fMouseMoveSmoothTime;

		fSumX      = fStepX;
		fSumY      = fStepY;
		fFactorSum = 1.0f;

		for (iStep = 1; iStep < iStepCnt; ++iStep)
		{
			fFactor     = float(dTime - m_qMouseTime[iStep]);
			fFactor     = float(exp(-0.5 * double(fFactor * fFactor / fVar)));
			fFactorSum += fFactor;

			fSumX += fFactor * m_qMouseStepX[iStep];
			fSumY += fFactor * m_qMouseStepY[iStep];
		}

		fStepX = fSumX / fFactorSum;
		fStepY = fSumY / fFactorSum;
	}

	if (IsScreenPlaneDragEnabled(iDragIdx))
	{
		double dSX, dSY, dSZ;
		double dX, dY, dZ, dX2, dY2, dZ2;

		ProjectScreenPos(0, 0, 0, dSX, dSY, dSZ);

		if ((iDragIdx == GetDragIdxRotate2()) ||
		    (iDragIdx == GetDragIdxRotate1()))
		{
			COGLVertex vexX, vexY, vexZ;
			float fFactor = 0.005f;

			UnProjectScreenPos(dMouseX + 100.0, dMouseY, dSZ, dX2, dY2, dZ2);
			UnProjectScreenPos(dMouseX, dMouseY, dSZ, dX, dY, dZ);
			vexX.Set(float(dX2 - dX), float(dY2 - dY), float(dZ2 - dZ));
			vexX.Norm();

			UnProjectScreenPos(dMouseX, dMouseY + 100.0, dSZ, dX2, dY2, dZ2);
			UnProjectScreenPos(dMouseX, dMouseY, dSZ, dX, dY, dZ);
			vexY.Set(float(dX2 - dX), float(dY2 - dY), float(dZ2 - dZ));
			vexY.Norm();

			vexZ = vexX ^ vexY;
			vexZ.Norm();

			SetDragBasis(iDragIdx, vexX, vexY, vexZ);

			if (IsAnimDragEnabled(iDragIdx))
			{
				AddToAnimDragData(iDragIdx, fFactor * fStepX, 0.0f, fFactor * fStepY);
			}
			else
			{
				AddToDragData(iDragIdx, fFactor * fStepX, 0.0f, fFactor * fStepY);
			}
		}
		else
		{
			float fDX, fDY, fDZ;
			double dDX, dDY, dDZ;
			double dDX1, dDY1, dDZ1, dDX2, dDY2, dDZ2, dFac;

			UnProjectScreenPos(dMouseX, dMouseY, dSZ, dX, dY, dZ);
			if (bShiftDown)		// Translate perpendicular to image plane
			{
				// Get direction of translation perpendicular to screen
				UnProjectScreenPos(dMouseX - 1.0, dMouseY, dSZ, dX2, dY2, dZ2);
				dDX1 = dX - dX2;
				dDY1 = dY - dY2;
				dDZ1 = dZ - dZ2;

				UnProjectScreenPos(dMouseX, dMouseY + 1.0, dSZ, dX2, dY2, dZ2);
				dDX2 = dX - dX2;
				dDY2 = dY - dY2;
				dDZ2 = dZ - dZ2;

				dDX  = dDY1 * dDZ2 - dDZ1 * dDY2;
				dDY  = dDZ1 * dDX2 - dDX1 * dDZ2;
				dDZ  = dDX1 * dDY2 - dDY1 * dDX2;
				dFac = 1.0 / sqrt(dDX * dDX + dDY * dDY + dDZ * dDZ);

				// Get translation length
				UnProjectScreenPos(dMouseX, dMouseY + dStepY, dSZ, dX2, dY2, dZ2);

				dX -= dX2;
				dY -= dY2;
				dZ -= dZ2;

				dFac *= (dStepY < 0 ? 1.0 : -1.0) * sqrt(dX * dX + dY * dY + dZ * dZ);

				fDX = float(dDX * dFac);
				fDY = float(dDY * dFac);
				fDZ = float(dDZ * dFac);
			}
			else
			{
				UnProjectScreenPos(dMouseX - dStepX, dMouseY + dStepY, dSZ, dX2, dY2, dZ2);

				fDX = float(dX - dX2);
				fDY = float(dY - dY2);
				fDZ = float(dZ - dZ2);
			}

			if (IsAnimDragEnabled(iDragIdx))
			{
				AddToAnimDragData(iDragIdx, fDX, fDY, fDZ);
			}
			else
			{
				AddToDragData(iDragIdx, fDX, fDY, fDZ);
			}
		}
	}
	else if (IsProjDirDragEnabled(iDragIdx))
	{
		double dSX, dSY, dSZ;
		double dX, dY, dZ, dX2, dY2, dZ2;
		double dSX1, dSY1, dSZ1;
		double dSX2, dSY2, dSZ2;
		double dSX3, dSY3, dSZ3;
		double dNSX1 = 0, dNSY1 = 0;
		double dNSX2 = 0, dNSY2 = 0;
		double dNSX3 = 0, dNSY3 = 0;
		double dSDX, dSDY, dSDZ;
		double dDX, dDY;
		double dFX, dFY, dFZ;
		double dAFX, dAFY, dAFZ;
		double dMag, dMag1, dMag2, dMag3;
		int iDragBasis = -1;
		COGLVertex vexX, vexY, vexZ;

		GetDragBasis(iDragIdx, vexX, vexY, vexZ);

		vexX.Norm();
		vexY.Norm();
		vexZ.Norm();

		ProjectScreenPos(0, 0, 0, dSX, dSY, dSZ);
		//pScene->UnProjectScreenPos( dSX + 100, dSY, dSZ, dX, dY, dZ );

		//dMag = sqrt( dX*dX + dY*dY + dZ*dZ );
		//vexX *= dMag;
		//vexY *= dMag;
		//vexZ *= dMag;

		ProjectScreenPos(double(vexX[0]), double(vexX[1]), double(vexX[2]),
				dSX1, dSY1, dSZ1);
		ProjectScreenPos(double(vexY[0]), double(vexY[1]), double(vexY[2]),
				dSX2, dSY2, dSZ2);
		ProjectScreenPos(double(vexZ[0]), double(vexZ[1]), double(vexZ[2]),
				dSX3, dSY3, dSZ3);

		dSX1 -= dSX;
		dSY1 -= dSY;
		dSZ1 -= dSZ;
		dSX2 -= dSX;
		dSY2 -= dSY;
		dSZ2 -= dSZ;
		dSX3 -= dSX;
		dSY3 -= dSY;
		dSZ3 -= dSZ;

		dMag1 = sqrt(dSX1 * dSX1 + dSY1 * dSY1);
		dMag2 = sqrt(dSX2 * dSX2 + dSY2 * dSY2);
		dMag3 = sqrt(dSX3 * dSX3 + dSY3 * dSY3);
		dMag  = dMag1 + dMag2 + dMag3;

		if (dMag1 / dMag > 0.2)
		{
			dNSX1 = dSX1 / dMag1;
			dNSY1 = dSY1 / dMag1;
		}

		if (dMag2 / dMag > 0.2)
		{
			dNSX2 = dSX2 / dMag2;
			dNSY2 = dSY2 / dMag2;
		}

		if (dMag3 / dMag > 0.2)
		{
			dNSX3 = dSX3 / dMag3;
			dNSY3 = dSY3 / dMag3;
		}

		dDX  = double(iStepX);
		dDY  = -double(iStepY);
		dMag = sqrt(dDX * dDX + dDY * dDY);
		dDX /= dMag;
		dDY /= dMag;

		dFX = (dDX * dNSX1 + dDY * dNSY1);
		dFY = (dDX * dNSX2 + dDY * dNSY2);
		dFZ = (dDX * dNSX3 + dDY * dNSY3);

		dAFX = Mag(dFX);
		dAFY = Mag(dFY);
		dAFZ = Mag(dFZ);

		if ((dAFX >= dAFY) && (dAFX >= dAFZ))
		{
			dMag      *= dFX / sqrt(dSX1 * dSX1 + dSY1 * dSY1 + dSZ1 * dSZ1);
			dSDX       = dMag * dSX1;
			dSDY       = dMag * dSY1;
			dSDZ       = dMag * dSZ1;
			iDragBasis = 0;
		}
		else if ((dAFY >= dAFX) && (dAFY >= dAFZ))
		{
			dMag      *= dFY / sqrt(dSX2 * dSX2 + dSY2 * dSY2 + dSZ2 * dSZ2);
			dSDX       = dMag * dSX2;
			dSDY       = dMag * dSY2;
			dSDZ       = dMag * dSZ2;
			iDragBasis = 1;
		}
		else
		{
			dMag      *= dFZ / sqrt(dSX3 * dSX3 + dSY3 * dSY3 + dSZ3 * dSZ3);
			dSDX       = dMag * dSX3;
			dSDY       = dMag * dSY3;
			dSDZ       = dMag * dSZ3;
			iDragBasis = 2;
		}

		UnProjectScreenPos(dMouseX, dMouseY, dSZ, dX, dY, dZ);
		UnProjectScreenPos(dMouseX - dSDX, dMouseY - dSDY, dSZ - dSDZ, dX2, dY2, dZ2);

		if ((iDragIdx == GetDragIdxRotate2()) ||
		    (iDragIdx == GetDragIdxRotate1()))
		{
			//COGLVertex vexX, vexY, vexZ;
			double dFactor = 0.009;

			if (IsAnimDragEnabled(iDragIdx))
			{
				if (iDragBasis == 0)
				{
					AddToAnimDragData(iDragIdx, float(dFactor * dFX * sqrt(double(fStepX * fStepX + fStepY * fStepY))), 0.0f, 0.0f);
				}
				else if (iDragBasis == 1)
				{
					AddToAnimDragData(iDragIdx, 0.0f, 0.0f, -float(dFactor * dFY * sqrt(double(fStepX * fStepX + fStepY * fStepY))));
				}
				else
				{
					AddToAnimDragData(iDragIdx, -float(dFactor * dFZ * sqrt(double(fStepX * fStepX + fStepY * fStepY))), 0.0f, 0.0f);
				}
			}
			else
			{
				if (iDragBasis == 0)
				{
					AddToDragData(iDragIdx, float(dFactor * dFX * sqrt(double(fStepX * fStepX + fStepY * fStepY))), 0.0f, 0.0f);
				}
				else if (iDragBasis == 1)
				{
					AddToDragData(iDragIdx, 0.0f, 0.0f, -float(dFactor * dFY * sqrt(double(fStepX * fStepX + fStepY * fStepY))));
				}
				else
				{
					AddToDragData(iDragIdx, -float(dFactor * dFZ * sqrt(double(fStepX * fStepX + fStepY * fStepY))), 0.0f, 0.0f);
				}
			}
		}
		else
		{
			//float fFac;
			COGLVertex vexDelta;
			vexDelta.Set(float(dX - dX2), float(dY - dY2), float(dZ - dZ2));
			if (iDragBasis == 0)
			{
				vexDelta = (vexDelta * vexX) * vexX;
			}
			else if (iDragBasis == 1)
			{
				vexDelta = (vexDelta * vexY) * vexY;
			}
			else if (iDragBasis == 2)
			{
				vexDelta = (vexDelta * vexZ) * vexZ;
			}

			if (IsAnimDragEnabled(iDragIdx))
			{
				AddToAnimDragData(iDragIdx, vexDelta[0], vexDelta[1], vexDelta[2]);
			}
			else
			{
				AddToDragData(iDragIdx, vexDelta[0], vexDelta[1], vexDelta[2]);
			}
		}
	}
	else
	{
		float fFactor;

		// Adjust sensitivity of mouse dragging, such that the default drag factor of 1
		// in CLUScript gives an acceptable movement. In this way also a 3D-Mouse behaves
		// similar to a standard mouse.
		if (iDragIdx == GetDragIdxTranslate())
		{
			fFactor = 0.01f;
		}
		else if ((iDragIdx == GetDragIdxRotate1()) ||
			 (iDragIdx == GetDragIdxRotate2()))
		{
			fFactor = 0.005f;
		}
		else
		{
			fFactor = 0.01f;
		}

		fStepX *= fFactor;
		fStepY *= fFactor;

		if (IsAnimDragEnabled(iDragIdx))
		{
			if (bShiftDown)
			{
				AddToAnimDragData(iDragIdx, fStepX, -fStepY, 0.0f);
			}
			else
			{
				AddToAnimDragData(iDragIdx, fStepX, 0.0f, fStepY);
			}
		}
		else
		{
			if (bShiftDown)
			{
				AddToDragData(iDragIdx, fStepX, -fStepY, 0.0f);
			}
			else
			{
				AddToDragData(iDragIdx, fStepX, 0.0f, fStepY);
			}
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Mouse 3D Move

void COGLScene::Mouse3D_Move(double dTX, double dTY, double dTZ, double dLength, double dRX, double dRY, double dRZ, double dAngle)
{
	int iTransIdx, iRotIdx;
	double dSX, dSY, dSZ;
	//double dX, dY, dZ;
	double dX2, dY2, dZ2;
	COGLVertex vexX, vexY, vexZ, vexRot, vexTrans;

	// Always Rotate/Translate relative to absolute screen coordinate system
	ProjectScreenPos(0, 0, 0, dSX, dSY, dSZ);

	UnProjectScreenPos(dSX + 100.0, dSY, dSZ, dX2, dY2, dZ2);
	vexX.Set(float(dX2), float(dY2), float(dZ2));
	vexX.Norm();

	UnProjectScreenPos(dSX, dSY + 100.0, dSZ, dX2, dY2, dZ2);
	vexY.Set(float(dX2), float(dY2), float(dZ2));
	vexY.Norm();

	vexZ = vexX ^ vexY;
	vexZ.Norm();

	if (IsDragEnabled(iTransIdx = GetDragIdxTranslate()) && (dLength > 0.0))
	{
		vexTrans = float(dTX) * vexX + float(dTY) * vexY + float(dTZ) * vexZ;

		if (IsAnimDragEnabled(iTransIdx))
		{
			//AddToAnimDragData( iTransIdx, float(dTX), float(dTY), float(dTZ) );
			AddToAnimDragData(iTransIdx, vexTrans[0], vexTrans[1], vexTrans[2]);
		}
		else
		{
			//AddToDragData( iTransIdx, float(dTX), float(dTY), float(dTZ) );
			AddToDragData(iTransIdx, vexTrans[0], vexTrans[1], vexTrans[2]);
		}
	}

	if (IsDragEnabled(iRotIdx = GetDragIdxRotate2()) && (dAngle > 0.0))
	{
		vexRot = float(dAngle * dRX) * vexX + float(dAngle * dRY) * vexY + float(dAngle * dRZ) * vexZ;

		if (IsAnimDragEnabled(iRotIdx))
		{
			//AddToAnimDragData( iRotIdx, float(dAngle * dRY), float(dAngle * dRZ), float(dAngle * dRX) );
			AddToAnimDragData(iRotIdx, vexRot[1], vexRot[2], vexRot[0]);
		}
		else
		{
			//AddToDragData( iRotIdx, float(dAngle * dRY), float(dAngle * dRZ), float(dAngle * dRX) );
			AddToDragData(iRotIdx, vexRot[1], vexRot[2], vexRot[0]);
		}
	}
}

//////////////////////////////////////////////////////////////////////
/// Reset Drag Data to zero

void COGLScene::ResetDragData()
{
	for (int i = 0; i < 4; i++)
	{
		m_mDragData[i].Set(0.0f, 0.0f, 0.0f);
		m_mDragDataStep[i].Set(0.0f, 0.0f, 0.0f);
		m_mRMain[i] = m_E3Base.vSC;
	}
}

//////////////////////////////////////////////////////////////////////
// Reset Animated Drag

void COGLScene::ResetAnimDrag(int iDataIdx)
{
	if (uint(iDataIdx) >= m_mAnimDragEnable.Count())
	{
		return;
	}

	m_mAnimDragActive[iDataIdx] = false;
	m_mAnimDragData[iDataIdx].Set(0.0f, 0.0f, 0.0f);
}

//////////////////////////////////////////////////////////////////////
// Enable/Disable aimated dragging for particular indices

void COGLScene::EnableAnimDrag(int iDataIdx, bool bVal)
{
	if (uint(iDataIdx) >= m_mAnimDragEnable.Count())
	{
		return;
	}

	if (m_mAnimDragEnable[iDataIdx] != bVal)
	{
		m_mAnimDragActive[iDataIdx] = false;
		m_mAnimDragData[iDataIdx].Set(0.0f, 0.0f, 0.0f);
	}

	m_mAnimDragEnable[iDataIdx] = bVal;
}

//////////////////////////////////////////////////////////////////////
// Enable/Disable dragging for particular indices

void COGLScene::EnableDrag(int iDataIdx, bool bVal)
{
	if (uint(iDataIdx) >= m_mAllowDrag.Count())
	{
		return;
	}

	m_mAllowDrag[iDataIdx] = bVal;
}

//////////////////////////////////////////////////////////////////////
// Enable/Disable dragging parallel to screen plane for particular indices

void COGLScene::EnableDragInScreenPlane(int iDataIdx, bool bVal)
{
	if (uint(iDataIdx) >= m_mScreenPlaneDrag.Count())
	{
		return;
	}

	m_mScreenPlaneDrag[iDataIdx] = bVal;
}

//////////////////////////////////////////////////////////////////////
// Enable/Disable dragging in projected directions for particular indices

void COGLScene::EnableDragInProjDir(int iDataIdx, bool bVal)
{
	if (uint(iDataIdx) >= m_mProjDirDrag.Count())
	{
		return;
	}

	m_mProjDirDrag[iDataIdx] = bVal;
}

//////////////////////////////////////////////////////////////////////
// Function called to change data values through dragging

bool COGLScene::SetDragData(int iDataIdx, float fX, float fY, float fZ)
{
	if (uint(iDataIdx) >= m_mAllowDrag.Count())
	{
		return false;
	}

/*
                float *pFac = m_mDragFactor[ iDataIdx ].pfData;

                fX *= pFac[0];
                fY *= pFac[1];
                fZ *= pFac[2];
*/
	// Test whether AutoScale and AutoTranslate is active and is changed by drag
	if (m_bAutoScale &&
	    m_bAutoTranslate &&
	    (iDataIdx == m_iDragIdxTranslate) &&
	    m_bAutoScaleAboutLocalOrigin)
	{
		m_AutoScaleOffset.Set(0.0f, 0.0f, 0.0f);

		fX *= (abs(m_AutoScale[0]) > 1e-6f ? m_AutoScale[0] : 1e-6f);
		fY *= (abs(m_AutoScale[2]) > 1e-6f ? m_AutoScale[2] : 1e-6f);
		fZ *= (abs(m_AutoScale[1]) > 1e-6f ? m_AutoScale[1] : 1e-6f);
	}

	m_mDragData[iDataIdx].Set(fX, fY, fZ);
	m_mDragDataStep[iDataIdx].Set(0, 0, 0);

	if (m_mUseDragRange[iDataIdx])
	{
		m_mDragRange[iDataIdx].Clamp(m_mDragData[iDataIdx]);
		// Clamping may have changed value
		m_mDragData[iDataIdx].Get(fX, fY, fZ);
	}

	if ((iDataIdx == m_iDragIdxRotate2) || (iDataIdx == m_iDragIdxRotate1))	//&& m_bAutoRotate )
	{
		COGLVertex vexA = m_mDragBasis[iDataIdx].LinComb(fZ, fX, fY);
		float fX2, fY2, fZ2;

		fX2 = vexA[0];
		fY2 = vexA[1];
		fZ2 = vexA[2];

		m_vU[4] = fX2;
		m_vU[5] = fY2;
		m_vU[6] = fZ2;

		float fAngle = float(sqrt(fX * fX + fY * fY + fZ * fZ)) * m_fRadPerDeg;
		m_mRMain[iDataIdx] = m_E3Base.Rotor(m_vU, fAngle, false);
	}

	// Test whether AutoScale is enabled and is changed by drag
	if ((m_bAutoScale || m_bAutoPixelZoom) &&
	    (iDataIdx == m_iDragIdxScale))
	{
		// Estimate Auto Scale
		EvalAutoScale();
	}

	UpdateAutoAdjustFrame();
	DragDataChanged(iDataIdx);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Function called to change animated draggin attenuation values

bool COGLScene::SetAnimDragAttenuation(int iDataIdx, float fX, float fY, float fZ)
{
	if (uint(iDataIdx) >= m_mAnimDragAttenuation.Count())
	{
		return false;
	}

	m_mAnimDragAttenuation[iDataIdx].Set(fX, fY, fZ);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Function called to change data values through dragging

bool COGLScene::AddToAnimDragData(int iDataIdx, float fX, float fY, float fZ)
{
	if (uint(iDataIdx) >= m_mAllowDrag.Count())
	{
		return false;
	}

	float fActX, fActY, fActZ;

	m_mAnimDragData[iDataIdx].Get(fActX, fActY, fActZ);

	fActX = 0.2f * fActX + 0.8f * fX;
	fActY = 0.2f * fActY + 0.8f * fY;
	fActZ = 0.2f * fActZ + 0.8f * fZ;

	m_mAnimDragData[iDataIdx].Set(fActX, fActY, fActZ);
	m_mAnimDragActive[iDataIdx] = true;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Step Animated Dragging
///
/// returns true if dragging has to be animated.

bool COGLScene::StepAnimDrag()
{
	float fX, fY, fZ, fAX, fAY, fAZ;
	bool bNeedAnimate = false;
	int iIdx, iCnt = int(m_mAnimDragEnable.Count());

	for (iIdx = 0; iIdx < iCnt; ++iIdx)
	{
		if (!m_mAnimDragEnable[iIdx] || !m_mAnimDragActive[iIdx])
		{
			continue;
		}

		bNeedAnimate = true;
		m_mAnimDragData[iIdx].Get(fX, fY, fZ);
		m_mAnimDragAttenuation[iIdx].Get(fAX, fAY, fAZ);

		AddToDragData(iIdx, fX, fY, fZ);

		fX *= fAX;
		fY *= fAY;
		fZ *= fAZ;

		if ((Mag(fX) < 1e-4f) && (Mag(fY) < 1e-4f) && (Mag(fZ) < 1e-4f))
		{
			fX                      = 0.0f;
			fY                      = 0.0f;
			fZ                      = 0.0f;
			m_mAnimDragActive[iIdx] = false;
		}

		m_mAnimDragData[iIdx].Set(fX, fY, fZ);
	}

	return bNeedAnimate;
}

//////////////////////////////////////////////////////////////////////
// Function called to change data values through dragging

bool COGLScene::AddToDragData(int iDataIdx, float fX, float fY, float fZ)
{
	if (uint(iDataIdx) >= m_mAllowDrag.Count())
	{
		return false;
	}

	//if ( !m_mAllowDrag[ iDataIdx ] )
	//	return true;

	float fOX, fOY, fOZ;
	//char *pStr;
	float* pFac = m_mDragFactor[iDataIdx].pfData;

	// Test whether AutoScale and AutoTranslate is active and is changed by drag
	if (m_bAutoScale &&
	    m_bAutoTranslate &&
	    (iDataIdx == m_iDragIdxTranslate) &&
	    !m_bAutoScaleAboutLocalOrigin)
	{
		fX *= pFac[0] / (abs(m_AutoScale[0]) > 1e-6f ? m_AutoScale[0] : 1e-6f);
		fY *= pFac[1] / (abs(m_AutoScale[2]) > 1e-6f ? m_AutoScale[2] : 1e-6f);
		fZ *= pFac[2] / (abs(m_AutoScale[1]) > 1e-6f ? m_AutoScale[1] : 1e-6f);
	}
	else
	{
		fX *= pFac[0];
		fY *= pFac[1];
		fZ *= pFac[2];
	}

	m_mDragData[iDataIdx].Get(fOX, fOY, fOZ);
	m_mDragDataStep[iDataIdx].Set(fX, fY, fZ);
	m_mDragData[iDataIdx].Add(fX, fY, fZ);

	if (m_mUseDragRange[iDataIdx])
	{
		m_mDragRange[iDataIdx].Clamp(m_mDragData[iDataIdx]);

		// Clamping may have changed value
		m_mDragData[iDataIdx].Get(fX, fY, fZ);

		// Estimate actual steps needed for rotation update
		fX -= fOX;
		fY -= fOY;
		fZ -= fOZ;

		m_mDragDataStep[iDataIdx].Set(fX, fY, fZ);
	}

	// Test whether AutoScale is enabled and is changed by drag
	if ((m_bAutoScale || m_bAutoPixelZoom) &&
	    (iDataIdx == m_iDragIdxScale))
	{
		SDragData CurScale;

		CurScale = m_AutoScale;

		// Estimate Auto Scale
		EvalAutoScale();

		if (m_bAutoScaleAboutMouseOrigin)
		{
			float fDX, fDY, fDZ;
			float fX, fY, fZ;

			fX = float(m_pdMouseDragStartLocal[0]);
			fY = float(m_pdMouseDragStartLocal[1]);
			fZ = float(m_pdMouseDragStartLocal[2]);

			fDX = fX * (CurScale[0] - m_AutoScale[0]);
			fDY = fY * (CurScale[1] - m_AutoScale[1]);
			fDZ = fZ * (CurScale[2] - m_AutoScale[2]);

			m_AutoScaleOffset.Add(fDX, fDY, fDZ);
		}
	}

	if ((iDataIdx == m_iDragIdxRotate2) ||
	    (iDataIdx == m_iDragIdxRotate1))		//&& m_bAutoRotate )
	{
		COGLVertex vexA = m_mDragBasis[iDataIdx].LinComb(fZ, fX, fY);
		float fX2, fY2, fZ2;

		fX2 = vexA[0];
		fY2 = vexA[1];
		fZ2 = vexA[2];

		m_vU[4] = fX2;
		m_vU[5] = fY2;
		m_vU[6] = fZ2;

		float fAngle = float(sqrt(fX * fX + fY * fY + fZ * fZ));// * m_fRadPerDeg;
		m_mRMain[iDataIdx] = m_E3Base.Rotor(m_vU, fAngle, false) & m_mRMain[iDataIdx];
		//pStr = m_mRMain[iDataIdx].Str();
	}

	UpdateAutoAdjustFrame();
	DragDataChanged(iDataIdx);

	SetContentChanged(true, true, false);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Enable/Disable drag range for particular indices

void COGLScene::EnableDragRange(int iDataIdx, bool bVal)
{
	if (uint(iDataIdx) >= m_mAllowDrag.Count())
	{
		return;
	}

	m_mUseDragRange[iDataIdx] = bVal;
}

//////////////////////////////////////////////////////////////////////
// Function called to change data values through dragging

bool COGLScene::SetDragRange(int iDataIdx,               float fMinX, float fMaxX, float fMinY, float fMaxY, float fMinZ, float fMaxZ)
{
	if (uint(iDataIdx) >= m_mAllowDrag.Count())
	{
		return false;
	}

	m_mDragRange[iDataIdx].Set(fMinX, fMaxX, fMinY, fMaxY, fMinZ, fMaxZ);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Update Auto Adjust Frame

void COGLScene::UpdateAutoAdjustFrame()
{
	if (m_bAutoAdjustFrame && m_refAutoFrame.IsValid())
	{
		float fX, fY, fZ;
		double dAngle;
		COGLVertex vexA;

		fX = fY = fZ = 0;
		//fX = vexA[0];
		//fY = vexA[1];
		//fZ = vexA[2];

		COGLFrame* pFrame = dynamic_cast< COGLFrame* >((COGLBaseElement*) m_refAutoFrame);
		if (pFrame)
		{
			Matrix<double> matID(4, 4);
			matID.Identity();
			pFrame->Set(matID);

			// Right Mouse Button & CTRL
			if (!m_bAutoScaleAboutLocalOrigin)
			{
				if (m_bAutoScale)
				{
					pFrame->Scale(m_AutoScale[0], m_AutoScale[1], m_AutoScale[2]);
				}

				if (m_bAutoPixelZoom)
				{
					pFrame->PixelZoom(m_AutoScale[0], m_AutoScale[1]);
				}
			}

			// Left Mouse Button & CTRL
			if (m_bAutoRotate1)
			{
				dAngle = 2.0 * acos(Scalar(m_mRMain[m_iDragIdxRotate1]));
				if (abs(dAngle) > 1e-4)
				{
					m_vAxis = (m_mRMain[m_iDragIdxRotate1](2)) & m_E3Base.vI;
					pFrame->Rotate(dAngle, m_vAxis[1], m_vAxis[2], m_vAxis[3]);
				}
			}

			if (m_bAutoTranslate)
			{
				// Right Mouse Button
				m_mDragData[m_iDragIdxTranslate].Get(fX, fY, fZ);

				if (IsScreenPlaneDragEnabled(m_iDragIdxTranslate))
				{
					pFrame->Translate(fX, fY, fZ);
				}
				else if (IsProjDirDragEnabled(m_iDragIdxTranslate))
				{
					pFrame->Translate(fX, fY, fZ);
				}
				else
				{
					vexA = m_mDragBasis[m_iDragIdxTranslate].LinComb(fX, fY, fZ);
					pFrame->Translate(double(vexA[0]),
							double(vexA[1]),
							double(vexA[2]));
				}
			}

			// Right Mouse Button & CTRL
			if (m_bAutoScaleAboutLocalOrigin)
			{
				if (m_bAutoScale)
				{
					pFrame->Scale(m_AutoScale[0], m_AutoScale[1], m_AutoScale[2]);
				}

				if (m_bAutoPixelZoom)
				{
					pFrame->PixelZoom(m_AutoScale[0], m_AutoScale[1]);
				}
			}

			if (m_bAutoRotate2)
			{
				// Left Mouse Button
				dAngle = 2.0 * acos(Scalar(m_mRMain[m_iDragIdxRotate2]));
				if (abs(dAngle) > 1e-4)
				{
					m_vAxis = (m_mRMain[m_iDragIdxRotate2](2)) & m_E3Base.vI;
					pFrame->Rotate(dAngle, m_vAxis[1], m_vAxis[2], m_vAxis[3]);
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////
// Function called to get data values through dragging

bool COGLScene::GetDragData(int iDataIdx, float& fX, float& fY, float& fZ)
{
	if (uint(iDataIdx) >= m_mAllowDrag.Count())
	{
		return false;
	}

	m_mDragData[iDataIdx].Get(fX, fY, fZ);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Function called to get data step values through dragging

bool COGLScene::GetDragDataStep(int iDataIdx, float& fX, float& fY, float& fZ)
{
	if (uint(iDataIdx) >= m_mAllowDrag.Count())
	{
		return false;
	}

	m_mDragDataStep[iDataIdx].Get(fX, fY, fZ);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set factors that are multiplied with drag data values

bool COGLScene::SetDragFactor(int iDataIdx, float fX, float fY, float fZ)
{
	if (uint(iDataIdx) >= m_mDragFactor.Count())
	{
		return false;
	}

	m_mDragFactor[iDataIdx].Set(fX, fY, fZ);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Get factors that are multiplied with drag data values

bool COGLScene::GetDragFactor(int iDataIdx, float& fX, float& fY, float& fZ)
{
	if (uint(iDataIdx) >= m_mDragFactor.Count())
	{
		return false;
	}

	m_mDragFactor[iDataIdx].Get(fX, fY, fZ);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Search for element with given UID also in PickList

COGLBEReference COGLScene::GetElementWithUID(uint uUID)
{
	COGLBEReference Ref = COGLBaseElementList::GetElementWithUID(uUID);
	COGLBEReference SubRef;
	COGLBaseElementList* pList;

	if (Ref.IsValid())
	{
		return Ref;
	}

	int i;
	for (i = 0; i < 8; ++i)
	{
		SubRef = m_mPickSceneRef[i];
		if (SubRef.IsValid())
		{
			if (SubRef->GetUID() == uUID)
			{
				return SubRef;
			}

			if ((pList = dynamic_cast<COGLBaseElementList*>((COGLBaseElement*) SubRef)))
			{
				SubRef = pList->GetElementWithUID(uUID);
				if (SubRef.IsValid())
				{
					return SubRef;
				}
			}
		}

		SubRef = m_mSinglePickSceneRef[i];
		if (SubRef.IsValid())
		{
			if (SubRef->GetUID() == uUID)
			{
				return SubRef;
			}

			if ((pList = dynamic_cast<COGLBaseElementList*>((COGLBaseElement*) SubRef)))
			{
				SubRef = pList->GetElementWithUID(uUID);
				if (SubRef.IsValid())
				{
					return SubRef;
				}
			}
		}
	}

	return Ref;
}

//////////////////////////////////////////////////////////////////////
// Set Auto Translation Basis

bool COGLScene::SetDragBasis(int iDataIdx, const COGLVertex& vexX, const COGLVertex& vexY, const COGLVertex& vexZ)
{
	if (uint(iDataIdx) >= m_mDragFactor.Count())
	{
		return false;
	}

	m_mDragBasis[iDataIdx].Set(vexX, vexY, vexZ);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Get Auto Translation Basis

bool COGLScene::GetDragBasis(int iDataIdx, COGLVertex& vexX, COGLVertex& vexY, COGLVertex& vexZ)
{
	if (uint(iDataIdx) >= m_mDragFactor.Count())
	{
		return false;
	}

	m_mDragBasis[iDataIdx].Get(vexX, vexY, vexZ);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set List of Scenes that trigger pick scene of this scene.
// This also replaces the current pick scene.

void COGLScene::SetPickTriggerScenes(vector< COGLBEReference >& vecTrigScene)
{
	vector< COGLBEReference >::iterator it_El;

	m_mapPickUID.clear();

	for (it_El = vecTrigScene.begin();
	     it_El != vecTrigScene.end();
	     ++it_El)
	{
		if (it_El->IsValid())
		{
			m_mapPickUID[(*it_El)->GetUID()] = 1;
		}
	}
}

//////////////////////////////////////////////////////////////////////
/// Eval Auto Scale

void COGLScene::EvalAutoScale()
{
	COGLVertex vexTrans;
	float fX, fY, fZ;
	int i;

	m_mDragData[m_iDragIdxScale].Get(fX, fY, fZ);
	vexTrans = m_mDragBasis[m_iDragIdxScale].LinComb(fX, fY, fZ);

	if (m_eScaleFunc == /*EScaleFunc::*/ LINEAR)
	{
		for (i = 0; i < 3; ++i)
		{
			m_AutoScale[i] = vexTrans[i] + 1;
		}
	}
	else if (m_eScaleFunc == /*EScaleFunc::*/ EXP)
	{
		for (i = 0; i < 3; ++i)
		{
			m_AutoScale[i] = exp(vexTrans[i]);
		}
	}
}

void COGLScene::GetProjectionMatrices(double* pdProjectMatrix, double* pdFrameMatrix, double* pdLocalFrameMatrix, int* piViewport)
{
	COGLScene* pScene = this;

	if (m_refFrameContextScene.IsValid())
	{
		COGLScene* pVal = dynamic_cast< COGLScene* >((COGLBaseElement*) m_refFrameContextScene);
		if (pVal)
		{
			pScene = pVal;
		}
	}

	memcpy(pdProjectMatrix, pScene->m_pdMouseProjMat, 16 * sizeof(double));
	memcpy(pdFrameMatrix, pScene->m_pdMouseFrameMat, 16 * sizeof(double));
	memcpy(pdLocalFrameMatrix, pScene->m_pdATMouseFrameMat, 16 * sizeof(double));
	memcpy(piViewport, pScene->m_piMouseViewport, 4 * sizeof(int));
}

//////////////////////////////////////////////////////////////////////
// Project Screen Pos

void COGLScene::ProjectScreenPos(double dX, double dY, double dZ, double& dSX, double& dSY, double& dSZ)
{
	COGLScene* pScene = this;

	if (m_refFrameContextScene.IsValid())
	{
		COGLScene* pVal = dynamic_cast< COGLScene* >((COGLBaseElement*) m_refFrameContextScene);
		if (pVal)
		{
			pScene = pVal;
		}
	}

	gluProject(dX, dY, dZ,
			pScene->m_pdMouseFrameMat, pScene->m_pdMouseProjMat, pScene->m_piMouseViewport,
			&dSX, &dSY, &dSZ);
}

//////////////////////////////////////////////////////////////////////
// UnProject Screen Position

void COGLScene::UnProjectScreenPos(double dSX, double dSY, double dSZ, double& dX, double& dY, double& dZ)
{
	COGLScene* pScene = this;

	if (m_refFrameContextScene.IsValid())
	{
		COGLScene* pVal = dynamic_cast< COGLScene* >((COGLBaseElement*) m_refFrameContextScene);
		if (pVal)
		{
			pScene = pVal;
		}
	}

	gluUnProject(dSX, dSY, dSZ,
			pScene->m_pdMouseFrameMat, pScene->m_pdMouseProjMat, pScene->m_piMouseViewport,
			&dX, &dY, &dZ);
}

//////////////////////////////////////////////////////////////////////
// Project Screen Pos

void COGLScene::ATProjectScreenPos(double dX, double dY, double dZ, double& dSX, double& dSY, double& dSZ)
{
	COGLScene* pScene = this;

	if (m_refFrameContextScene.IsValid())
	{
		COGLScene* pVal = dynamic_cast< COGLScene* >((COGLBaseElement*) m_refFrameContextScene);
		if (pVal)
		{
			pScene = pVal;
		}
	}

	gluProject(dX, dY, dZ,
			pScene->m_pdATMouseFrameMat, pScene->m_pdMouseProjMat, pScene->m_piMouseViewport,
			//pScene->m_pdMouseFrameMat, pScene->m_pdMouseProjMat, pScene->m_piMouseViewport,
			&dSX, &dSY, &dSZ);
}

//////////////////////////////////////////////////////////////////////
// UnProject Screen Position

void COGLScene::ATUnProjectScreenPos(int iX, int iY, double dSZ, double& dX, double& dY, double& dZ)
{
	COGLScene* pScene = this;

	if (m_refFrameContextScene.IsValid())
	{
		COGLScene* pVal = dynamic_cast< COGLScene* >((COGLBaseElement*) m_refFrameContextScene);
		if (pVal)
		{
			pScene = pVal;
		}
	}

	gluUnProject(double(iX), double(iY), dSZ,
			pScene->m_pdATMouseFrameMat, pScene->m_pdMouseProjMat, pScene->m_piMouseViewport,
			//pScene->m_pdMouseFrameMat, pScene->m_pdMouseProjMat, pScene->m_piMouseViewport,
			&dX, &dY, &dZ);
}

//////////////////////////////////////////////////////////////////////
// Project Screen Pos

void COGLScene::ARProjectScreenPos(double dX, double dY, double dZ, double& dSX, double& dSY, double& dSZ)
{
	COGLScene* pScene = this;

	if (m_refFrameContextScene.IsValid())
	{
		COGLScene* pVal = dynamic_cast< COGLScene* >((COGLBaseElement*) m_refFrameContextScene);
		if (pVal)
		{
			pScene = pVal;
		}
	}

	gluProject(dX, dY, dZ,
			//pScene->m_pdARMouseFrameMat, pScene->m_pdMouseProjMat, pScene->m_piMouseViewport,
			pScene->m_pdMouseFrameMat, pScene->m_pdMouseProjMat, pScene->m_piMouseViewport,
			&dSX, &dSY, &dSZ);
}

//////////////////////////////////////////////////////////////////////
// UnProject Screen Position

void COGLScene::ARUnProjectScreenPos(int iX, int iY, double dSZ, double& dX, double& dY, double& dZ)
{
	COGLScene* pScene = this;

	if (m_refFrameContextScene.IsValid())
	{
		COGLScene* pVal = dynamic_cast< COGLScene* >((COGLBaseElement*) m_refFrameContextScene);
		if (pVal)
		{
			pScene = pVal;
		}
	}

	gluUnProject(double(iX), double(iY), dSZ,
			//pScene->m_pdARMouseFrameMat, pScene->m_pdMouseProjMat, pScene->m_piMouseViewport,
			pScene->m_pdMouseFrameMat, pScene->m_pdMouseProjMat, pScene->m_piMouseViewport,
			&dX, &dY, &dZ);
}

//////////////////////////////////////////////////////////////////////
// Enable adaptive redraw

void COGLScene::EnableAdaptiveRedraw(bool bVal)
{
	m_bAdaptiveRedraw = bVal;
	m_bHasSceneImage  = false;

	// Set all child BaseElementLists accordingly
	EnableContentChangedInfo(bVal);
}

//////////////////////////////////////////////////////////////////////
/// Tell parents that content has changed

void COGLScene::TellParentContentChanged()
{
	list<COGLBaseElement*>::iterator itEl;

	for (itEl = m_listParent.begin();
	     itEl != m_listParent.end();
	     ++itEl)
	{
		COGLBaseElementList* pList = dynamic_cast<COGLBaseElementList*>((COGLBaseElement*) (*itEl));
		if (pList)
		{
			pList->SetContentChanged(true, true, false);
		}
	}
}

//////////////////////////////////////////////////////////////////////
// Either read current projection matrix from OpenGL or copy
// previously read matrix.

void COGLScene::GetProjMat()
{
	if (m_bProjMatCurrent)
	{
		return;
	}

	glGetDoublev(GL_PROJECTION_MATRIX, m_pdCurProjMat);
	m_bProjMatCurrent = true;
}

//////////////////////////////////////////////////////////////////////
// Either read current projection matrix from OpenGL or copy
// previously read matrix.

void COGLScene::GetProjMat(GLdouble* pMat)
{
	if (!m_bProjMatCurrent)
	{
		GetProjMat();
	}

	memcpy(pMat, m_pdCurProjMat, 16 * sizeof(GLdouble));
}

//////////////////////////////////////////////////////////////////////
// Either read current model view matrix from OpenGL or copy
// previously read matrix.

void COGLScene::GetFrameMat()
{
	if (m_bFrameMatCurrent)
	{
		return;
	}

	glGetDoublev(GL_MODELVIEW_MATRIX, m_pdCurFrameMat);
	m_bFrameMatCurrent = true;
}

//////////////////////////////////////////////////////////////////////
// Either read current model view matrix from OpenGL or copy
// previously read matrix.

void COGLScene::GetFrameMat(GLdouble* pMat)
{
	if (!m_bFrameMatCurrent)
	{
		GetFrameMat();
	}

	memcpy(pMat, m_pdCurFrameMat, 16 * sizeof(GLdouble));
}

//////////////////////////////////////////////////////////////////////
// Either read current viewport from OpenGL or copy
// previously read viewport.

void COGLScene::GetViewport()
{
	if (m_bViewportCurrent)
	{
		return;
	}

	glGetIntegerv(GL_VIEWPORT, m_piCurViewport);
	m_bViewportCurrent = true;
}

//////////////////////////////////////////////////////////////////////
// Either read current model view matrix from OpenGL or copy
// previously read matrix.

void COGLScene::GetViewport(GLint* pViewport)
{
	if (!m_bViewportCurrent)
	{
		GetViewport();
	}

	memcpy(pViewport, m_piCurViewport, 4 * sizeof(GLint));
}

//////////////////////////////////////////////////////////////////////
// Get clipping planes for picking mode

void COGLScene::GetPickClipPlanes(GLdouble pdClipPlanes[4][4])
{
	glGetClipPlane(GL_CLIP_PLANE0, pdClipPlanes[0]);
	glGetClipPlane(GL_CLIP_PLANE1, pdClipPlanes[1]);
	glGetClipPlane(GL_CLIP_PLANE2, pdClipPlanes[2]);
	glGetClipPlane(GL_CLIP_PLANE3, pdClipPlanes[3]);
}

//////////////////////////////////////////////////////////////////////
// Set clipping planes for picking mode

void COGLScene::SetPickClipPlanes(GLdouble pdClipPlanes[4][4])
{
	glClipPlane(GL_CLIP_PLANE0, pdClipPlanes[0]);
	glClipPlane(GL_CLIP_PLANE1, pdClipPlanes[1]);
	glClipPlane(GL_CLIP_PLANE2, pdClipPlanes[2]);
	glClipPlane(GL_CLIP_PLANE3, pdClipPlanes[3]);
}

//////////////////////////////////////////////////////////////////////
// Evaluate Pick Clip Planes

void COGLScene::EvalPickClipPlanes(GLdouble pdClipPlanes[4][4],        int iX, int iY, int iPickW, int iPickH, GLint* piViewport, GLdouble* pdProj, GLdouble* pdFrame)
{
	double dXL, dXR, dYB, dYT;
	double dX, dY, dZ;
	COGLVertex xA0, xA1, xA2, xA3;
	COGLVertex xB0, xB2, xCenter;
	COGLVertex xDir, xDirFrontRight, xDirFrontUp;

	dXL = double(iX - iPickW);
	dXR = double(iX + iPickW);
	dYB = double(iY - iPickH);
	dYT = double(iY + iPickH);

	// Center
	gluUnProject(double(iX), double(iY), 0.2,
			pdFrame, pdProj, piViewport,
			&dX, &dY, &dZ);
	xCenter.Set(float(dX), float(dY), float(dZ));

	// Left-Bottom-Front
	gluUnProject(dXL, dYB, 0.2,
			pdFrame, pdProj, piViewport,
			&dX, &dY, &dZ);
	xA0.Set(float(dX), float(dY), float(dZ));

	// Right-Bottom-Frontk
	gluUnProject(dXR, dYB, 0.2,
			pdFrame, pdProj, piViewport,
			&dX, &dY, &dZ);
	xA1.Set(float(dX), float(dY), float(dZ));

	// Front right direction
	xDirFrontRight = xA1 - xA0;
	xDirFrontRight.Norm();

	// Right-Top-Front
	gluUnProject(dXR, dYT, 0.2,
			pdFrame, pdProj, piViewport,
			&dX, &dY, &dZ);
	xA2.Set(float(dX), float(dY), float(dZ));

	// Left-Top-Front
	gluUnProject(dXL, dYT, 0.2,
			pdFrame, pdProj, piViewport,
			&dX, &dY, &dZ);
	xA3.Set(float(dX), float(dY), float(dZ));

	// Front up direction
	xDirFrontUp = xA3 - xA0;
	xDirFrontUp.Norm();

	// Left-Bottom-Back
	gluUnProject(dXL, dYB, 0.8,
			pdFrame, pdProj, piViewport,
			&dX, &dY, &dZ);
	xB0.Set(float(dX), float(dY), float(dZ));

	// Right-Top-Back
	gluUnProject(dXR, dYT, 0.8,
			pdFrame, pdProj, piViewport,
			&dX, &dY, &dZ);
	xB2.Set(float(dX), float(dY), float(dZ));

	// Clip plane: left
	// We have to flip the direction value when ortho projection is setup to flip a direction
	xDir = ((xB0 - xA0) ^ (xA3 - xA0));
	xDir.Norm();
	if (xDirFrontRight * xDir < 0.0f)
	{
		xDir = -xDir;
	}

	pdClipPlanes[0][0] = double(((float*) xDir)[0]);
	pdClipPlanes[0][1] = double(((float*) xDir)[1]);
	pdClipPlanes[0][2] = double(((float*) xDir)[2]);
	pdClipPlanes[0][3] = -double(xDir * xA0);

	// Clip plane: right
	xDir = ((xB2 - xA2) ^ (xA1 - xA2));
	xDir.Norm();
	if (xDirFrontRight * xDir > 0.0f)
	{
		xDir = -xDir;
	}

	pdClipPlanes[1][0] = double(((float*) xDir)[0]);
	pdClipPlanes[1][1] = double(((float*) xDir)[1]);
	pdClipPlanes[1][2] = double(((float*) xDir)[2]);
	pdClipPlanes[1][3] = -double(xDir * xA2);

	// Clip plane: bottom
	xDir = ((xA1 - xA0) ^ (xB0 - xA0));
	xDir.Norm();
	if (xDirFrontUp * xDir < 0.0f)
	{
		xDir = -xDir;
	}

	pdClipPlanes[2][0] = double(((float*) xDir)[0]);
	pdClipPlanes[2][1] = double(((float*) xDir)[1]);
	pdClipPlanes[2][2] = double(((float*) xDir)[2]);
	pdClipPlanes[2][3] = -double(xDir * xA0);

	// Clip plane: top
	xDir = ((xA3 - xA2) ^ (xB2 - xA2));
	xDir.Norm();
	if (xDirFrontUp * xDir > 0.0f)
	{
		xDir = -xDir;
	}

	pdClipPlanes[3][0] = double(((float*) xDir)[0]);
	pdClipPlanes[3][1] = double(((float*) xDir)[1]);
	pdClipPlanes[3][2] = double(((float*) xDir)[2]);
	pdClipPlanes[3][3] = -double(xDir * xA2);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool COGLScene::Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData& rData)
{
	try
	{
		// If is not to be drawn, return immediately
		if (!m_bDrawScene ||
		    ((eMode == COGLBaseElement::DRAW) && ((!rData.bInTransparencyPass && !m_bDrawOpaque) || (rData.bInTransparencyPass && !m_bDrawTransparent))) ||
		    ((eMode == COGLBaseElement::PICK) && !m_bDoPickDraw))
		{
			return true;
		}

		// Reset Flag for restoring pick clip planes
		bool bRestorePickClipPlanes = false;

		bool bAutoAdaptFrontFace  = false;
		bool bInvertFrontFace     = false;
		bool bShowPickScene       = false;
		bool bShowSinglePickScene = false;
		bool bCurNeedAnimate      = false;
		bool bPickStart           = false;
		int piCurViewport[4];
		int piViewport[4], piScissorBox[4];
		double pdDepthRange[2];
		double dDepthLevelIdx   = 0.0;
		double dCurPickTime     = 0.0;
		double dCurPickTimeStep = 0.0;

		COGLScene* pPickScene       = 0;
		COGLScene* pSinglePickScene = 0;
		m_iPickIdx = -1;

		GLboolean bUseScissor, bDoNormalize = GLboolean(0);
		GLfloat fPixelZoomX, fPixelZoomY;

		// Store current pick state
		bool bOutsidePickState;

		// Reset Flags that matrices are current
		ClearProjMatCurrent();
		ClearFrameMatCurrent();
		ClearViewportCurrent();

		// If in first render pass animate dragging if necessary
		if ((eMode == COGLBaseElement::DRAW) && rData.bInFirstRenderPass && (rData.iRepeatIdx == 0))
		{
			// Function StepAnimDrag() returns true if dragging need to be animated
			rData.bNeedAnimate |= StepAnimDrag();
		}

		if ((eMode == COGLBaseElement::DRAW) && rData.bInFirstRenderPass && (rData.iRepeatIdx == 0))
		{
			// Store current need animate flag
			bCurNeedAnimate = rData.bNeedAnimate;
			// Reset global need animate flag
			rData.bNeedAnimate = false;
		}

		if (m_bIsPickable && (eMode == COGLBaseElement::DRAW))
		{
			// Call this for each render pass and each repeat to get the uPickDrawFlag which indicates whether a single pick scene has already been drawn
			rData.IsPicked(m_mapPickUID, m_uPickDrawFlag);
		}

		if (m_bIsPickable && rData.bInFirstRenderPass)
		{
			if (m_dIntroTimer > 0.0)
			{
				m_iPickIdx = m_iCurPickIdx = m_iIntroPickIdx;
			}
			else if (m_dOutroTimer > 0.0)
			{
				m_iPickIdx = m_iCurPickIdx = -1;
			}
			else if (eMode == COGLBaseElement::DRAW)
			{
				m_bPickIntro = false;
				m_bPickOutro = false;

				// Check whether this scene is picked.
				// Call this for each render pass and each repeat to get the uPickDrawFlag which indicates whether a single pick scene has already been drawn
				m_bIsPicked = rData.IsPicked(m_mapPickUID, m_uPickDrawFlag);

				if (rData.iRepeatIdx == 0)
				{
					if (m_bIsPicked)
					{
						m_iPickIdx = GetPickSceneIdx(rData.bShiftDown, rData.bCtrlDown, rData.bAltDown, rData.bDoDrag);

						// Check whether asked for pick scene exists. If not default to standard pick scene
						if (!m_mPickSceneRef[m_iPickIdx].IsValid() && !m_mSinglePickSceneRef[m_iPickIdx].IsValid())
						{
							m_iPickIdx = 0;
						}

						if (m_iPickIdx != m_iCurPickIdx)
						{
							m_iCurPickIdx     = m_iPickIdx;
							m_bCurPickChanged = true;

							m_dIntroTimer      = m_mIntroPickTimer[m_iPickIdx];
							m_dIntroTimeFactor = m_mIntroPickTimeFactor[m_iPickIdx];
							m_iIntroPickIdx    = m_iPickIdx;

							bPickStart = true;
						}
					}
					else if (!m_bIsPicked)
					{
						m_iPickIdx = -1;
						if (m_iCurPickIdx != -1)
						{
							m_dOutroTimer      = m_mOutroPickTimer[m_iCurPickIdx];
							m_dOutroTimeFactor = m_mOutroPickTimeFactor[m_iCurPickIdx];
							m_iOutroPickIdx    = m_iCurPickIdx;
							m_iCurPickIdx      = -1;
							m_bCurPickChanged  = true;
						}
					}
				}
			}
		}

		if (m_bLocalView)
		{
			double dWLeft, dWBottom, dWDepth, dWRight, dWTop;

			GetViewport(piCurViewport);
			glGetIntegerv(GL_SCISSOR_BOX, piScissorBox);
			glGetBooleanv(GL_SCISSOR_TEST, &bUseScissor);

			int iLeft     = piCurViewport[0];
			int iBottom   = piCurViewport[1];
			float fWidth  = (float) piCurViewport[2];
			float fHeight = (float) piCurViewport[3];

			if (m_Viewport.refSceneOrigin.IsValid())
			{
				COGLScene* pScene = dynamic_cast<COGLScene*>((COGLBaseElement*) m_Viewport.refSceneOrigin);
				if (pScene)
				{
					if (m_Viewport.bSceneBottom)
					{
						iBottom = pScene->m_piViewport[1];
					}
					else
					{
						iBottom = pScene->m_piViewport[1] + pScene->m_piViewport[3];
					}

					if (m_Viewport.bSceneLeft)
					{
						iLeft = pScene->m_piViewport[0];
					}
					else
					{
						iLeft = pScene->m_piViewport[0] + pScene->m_piViewport[2];
					}
				}
			}

			if (m_Viewport.bLocalOrigin)
			{
				GetFrameMat(m_pdFrameMat);
				GetProjMat(m_pdProjMat);

				gluProject(double(m_Viewport.fLeft), double(m_Viewport.fBottom), double(m_Viewport.fDepth),
						m_pdFrameMat, m_pdProjMat, piCurViewport, &dWLeft, &dWBottom, &dWDepth);

				iLeft   = (int) floor(dWLeft);
				iBottom = (int) floor(dWBottom);
			}
			else
			{
				if (m_Viewport.bRelLeft)
				{
					iLeft += int(m_Viewport.fLeft * fWidth);
				}
				else
				{
					iLeft += int(m_Viewport.fLeft);
				}

				if (m_Viewport.bRelBottom)
				{
					iBottom += int(m_Viewport.fBottom * fHeight);
				}
				else
				{
					iBottom += int(m_Viewport.fBottom);
				}
			}

			int iWidth, iHeight;
			if (m_Viewport.bLocalOrigin && m_Viewport.bLocalExtent)
			{
				gluProject(double(m_Viewport.fLeft + m_Viewport.fWidth),
						double(m_Viewport.fBottom + m_Viewport.fHeight),
						double(m_Viewport.fDepth),
						m_pdFrameMat, m_pdProjMat, piCurViewport,
						&dWRight, &dWTop, &dWDepth);

				iWidth  = int(floor(dWRight - dWLeft));
				iHeight = int(floor(dWTop - dWBottom));
			}
			else
			{
				if (m_Viewport.bRelWidth)
				{
					iWidth = int(m_Viewport.fWidth * fWidth);
				}
				else
				{
					iWidth = int(m_Viewport.fWidth);
				}

				if (m_Viewport.bRelHeight)
				{
					iHeight = int(m_Viewport.fHeight * fHeight);
				}
				else
				{
					iHeight = int(m_Viewport.fHeight);
				}
			}

			int iBL = m_Viewport.iBorderLeft;
			int iBR = m_Viewport.iBorderRight;
			int iBB = m_Viewport.iBorderBottom;
			int iBT = m_Viewport.iBorderTop;

			if (abs(iWidth) <= iBL + iBR)
			{
				iWidth = iBL + iBR + 1;
			}

			if (iHeight <= iBB + iBT)
			{
				iHeight = iBB + iBT + 1;
			}

			if (iWidth < 0)
			{
				iLeft += iWidth;
				iWidth = -iWidth;
			}

			if (iHeight < 0)
			{
				iBottom += iHeight;
				iHeight  = -iHeight;
			}

			iLeft   -= int(m_Viewport.fAlignX * float(iWidth));
			iBottom -= int(m_Viewport.fAlignY * float(iHeight));

			iLeft   += iBL;
			iBottom += iBB;
			iWidth  -= iBL + iBR;
			iHeight -= iBB + iBT;

			// Check whether mouse is outside the current viewport.
			// If yes do not draw anything in picking mode.
			if ((eMode == COGLBaseElement::PICK) &&
			    ((rData.iMouseX < iLeft) || (rData.iMouseX > iLeft + iWidth) || (rData.iMouseY < iBottom) || (rData.iMouseY > iBottom + iHeight)))
			{
				// Store the viewport that would have been used by this scene, in case this scene is used as reference for the viewport alignment of another scene
				m_piViewport[0] = iLeft - iBL;
				m_piViewport[1] = iBottom - iBB;
				m_piViewport[2] = iWidth + iBL + iBR;
				m_piViewport[3] = iHeight + iBB + iBT;

				return true;
			}

			// Store current depth range
			glGetDoublev(GL_DEPTH_RANGE, pdDepthRange);
			// Store current depth level index
			dDepthLevelIdx = rData.dDepthLevelIdx;
			// Standard depth range
			m_dDepthNear = pdDepthRange[0];
			m_dDepthFar  = pdDepthRange[1];

			// Set depth range of scene.
			if (m_Viewport.eDepthLevelType == SViewport::DL_Relative)
			{
				// New depth level is relative to current one negative depth levels lie in front of the standard scene
				rData.dDepthLevelIdx += m_Viewport.dDepthLevel;
				m_dDepthNear          = rData.dDepthLevelOrig - rData.dDepthLevelIdx * rData.dDepthLevelWidth;
				m_dDepthFar           = m_dDepthNear + rData.dDepthLevelWidth - rData.dDepthLevelGap;
			}
			else if (m_Viewport.eDepthLevelType == SViewport::DL_Absolute)
			{
				// New depth level. Negative depth levels lie in front of the standard scene
				rData.dDepthLevelIdx = m_Viewport.dDepthLevel;
				m_dDepthNear         = rData.dDepthLevelOrig - rData.dDepthLevelIdx * rData.dDepthLevelWidth;
				m_dDepthFar          = m_dDepthNear + rData.dDepthLevelWidth - rData.dDepthLevelGap;
			}
			else if (m_Viewport.bLocalOrigin && (m_Viewport.eDepthLevelType == SViewport::DL_Local))
			{
				// Depth level depends on local depth of viewport origin in screen coordinate system
				double dWX = double(piCurViewport[0]) + double(piCurViewport[2]) / 2.0;
				double dWY = double(piCurViewport[1]) + double(piCurViewport[3]) / 2.0;

				double dX1, dY1, dZ1;
				gluUnProject(dWX, dWY, 0.2, m_pdFrameMat, m_pdProjMat, piCurViewport, &dX1, &dY1, &dZ1);

				double dX2, dY2, dZ2;
				gluUnProject(dWX, dWY, 0.8, m_pdFrameMat, m_pdProjMat, piCurViewport, &dX2, &dY2, &dZ2);

				// Make depth axis point out of the screen
				double dDX    = dX1 - dX2;
				double dDY    = dY1 - dY2;
				double dDZ    = dZ1 - dZ2;
				double dDNorm = sqrt(dDX * dDX + dDY * dDY + dDZ * dDZ);

				dDX /= dDNorm;
				dDY /= dDNorm;
				dDZ /= dDNorm;

				double dDLLocal = dDX * double(m_Viewport.fLeft) + dDY * double(m_Viewport.fBottom) + dDZ * double(m_Viewport.fDepth);

				if (dDLLocal > m_Viewport.dDLLocalNear)
				{
					dDLLocal = m_Viewport.dDLLocalNear;
				}
				else if (dDLLocal < m_Viewport.dDLLocalFar)
				{
					dDLLocal = m_Viewport.dDLLocalFar;
				}

				rData.dDepthLevelIdx = (dDLLocal - m_Viewport.dDLLocalFar)
						       * (m_Viewport.dDLNear - m_Viewport.dDLFar) / (m_Viewport.dDLLocalNear - m_Viewport.dDLLocalFar) + m_Viewport.dDLFar;

				m_dDepthNear = rData.dDepthLevelOrig - rData.dDepthLevelIdx * rData.dDepthLevelWidth;
				m_dDepthFar  = m_dDepthNear + rData.dDepthLevelWidth - rData.dDepthLevelGap;
			}

			glDepthRange(m_dDepthNear, m_dDepthFar);

			// Set new viewport
			CLU_OGL_CALL(glViewport(iLeft, iBottom, iWidth, iHeight));

			if (m_bUseViewScissor)
			{
				glScissor(iLeft, iBottom, iWidth, iHeight);
				glEnable(GL_SCISSOR_TEST);
			}

			ClearViewportCurrent();

			piViewport[0] = iLeft;
			piViewport[1] = iBottom;
			piViewport[2] = iWidth;
			piViewport[3] = iHeight;

			m_piViewport[0] = iLeft - iBL;
			m_piViewport[1] = iBottom - iBB;
			m_piViewport[2] = iWidth + iBL + iBR;
			m_piViewport[3] = iHeight + iBB + iBT;

			// If we are in drawing mode, adaptive drawing is switched on and image of scene has been stored, then simply draw the pixel
			if ((eMode == COGLBaseElement::DRAW) && m_bAdaptiveRedraw && m_bHasSceneImage)
			{
				int iWidth, iHeight;
				m_imgScene.GetSize(iWidth, iHeight);

				if (m_bCurPickChanged || (iWidth != piViewport[2]) || (iHeight != piViewport[3]))
				{
					m_bHasSceneImage = false;
				}
				// If content has not changed draw image
				else if (!IsContentChanged())
				{
					if (glWindowPos3f)
					{
						glWindowPos3f(float(iLeft), float(iBottom), float(m_dDepthNear));
					}
					else
					{
						rData.xMatrixStack.Push(Clu::CMatrixStack::Projection);
						gluOrtho2D(0.0, float(iWidth), 0.0, float(iHeight));

						rData.xMatrixStack.Push(Clu::CMatrixStack::ModelView);
						glLoadIdentity();
						glRasterPos3f(0.0, 0.0, 0.0);

						rData.xMatrixStack.Pop(Clu::CMatrixStack::ModelView);
						rData.xMatrixStack.Pop(Clu::CMatrixStack::Projection);

						glMatrixMode(GL_MODELVIEW);
					}

					glPixelStorei(GL_UNPACK_ROW_LENGTH, iWidth);
					glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
					glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
					glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

					::LockImageAccess();
					glDrawPixels(iWidth, iHeight, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*) m_imgScene.GetDataPtr());
					::UnlockImageAccess();

					// Reset Viewport
					ClearViewportCurrent();
					glViewport(piCurViewport[0], piCurViewport[1], piCurViewport[2], piCurViewport[3]);

					if (m_bUseViewScissor)
					{
						glScissor(piScissorBox[0], piScissorBox[1], piScissorBox[2], piScissorBox[3]);
						if (bUseScissor)
						{
							glEnable(GL_SCISSOR_TEST);
						}
						else
						{
							glDisable(GL_SCISSOR_TEST);
						}
					}

					glDepthRange(pdDepthRange[0], pdDepthRange[1]);

					// Finished drawing
					return true;
				}

				// If content has changed reset content changed flag
				if (IsContentChanged() && rData.bInLastRenderPass)
				{
					SetContentChanged(false, true, false);
				}
			}

			// If a new viewport but no new projection matrix is defined
			// then adapt current projection matrix to aspect ratio of
			// new viewport.
			if (!m_bLocalProj)
			{
				GetProjMat(m_pdProjMat);
				GetFrameMat(m_pdFrameMat);

				glMatrixMode(GL_PROJECTION);
				ClearProjMatCurrent();
				glScalef(float(piCurViewport[2]) / float(piViewport[2]), float(piCurViewport[3]) / float(piViewport[3]), 1.0f);
				GetProjMat();
				glMatrixMode(GL_MODELVIEW);

				// If we are in picking mode and there is no local
				// projection defined, then need to adapt pick clip planes.
				if (eMode == COGLBaseElement::PICK)
				{
					// Signal that pick clip planes need to be restored
					bRestorePickClipPlanes = true;

					// Store current pick clip planes
					glPushAttrib(GL_TRANSFORM_BIT);

					// Evaluate new pick clip planes
					EvalPickClipPlanes(m_pdCurPickClipPlanes, rData.iMouseX, rData.iMouseY, rData.iPickW, rData.iPickH, piViewport, m_pdCurProjMat,
							m_pdFrameMat);

					// Set new pick clip planes
					SetPickClipPlanes(m_pdCurPickClipPlanes);
				}
			}
		}

		if (m_bLocalProj)
		{
			GetViewport(piViewport);
			GetFrameMat(m_pdFrameMat);
			GetProjMat(m_pdProjMat);

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			ClearProjMatCurrent();

			if (m_eProjType == /*EProjType::*/ ORTHO)
			{
				float fLeft   = m_OrthoProj.fLeft;
				float fRight  = m_OrthoProj.fRight;
				float fBottom = m_OrthoProj.fBottom;
				float fTop    = m_OrthoProj.fTop;
				float fOW, fOH, fVW, fVH, fVAspect, fOAspect, fCenter;

				// Adapt Orthographic projection according to
				// viewport aspect ratio
				if (m_bOrthoViewportAspectRatio)
				{
					fOW      = (fRight - fLeft);
					fOH      = (fTop - fBottom);
					fVW      = float(piViewport[2]);
					fVH      = float(piViewport[3]);
					fOAspect = fOW / fOH;
					fVAspect = fVW / fVH;

					float fOAspectSign = fOAspect > 0 ? 1.f : -1.f;

					if (abs(fVAspect) < abs(fOAspect))
					{
						fOH     = (fOW / fVAspect) * fOAspectSign;
						fCenter = (fBottom + fTop) / 2.0f;
						fBottom = fCenter - fOH / 2.0f;
						fTop    = fCenter + fOH / 2.0f;
					}
					else
					{
						fOW     = fOH * fVAspect * fOAspectSign;
						fCenter = (fLeft + fRight) / 2.0f;
						fLeft   = fCenter - fOW / 2.0f;
						fRight  = fCenter + fOW / 2.0f;
					}
				}

				glOrtho(GLdouble(fLeft), GLdouble(fRight),
						GLdouble(fBottom), GLdouble(fTop),
						GLdouble(m_OrthoProj.fNear), GLdouble(m_OrthoProj.fFar));
			}
			else if (m_eProjType ==	/*EProjType::*/ CENTRAL)
			{
				if (m_Perspective.bUseAngle)
				{
					float fHWidth, fHHeight, fAspect, fTA;

					fTA                   = tan(0.5f * m_Perspective.fAngle * m_fRadPerDeg);
					fAspect               = float(piViewport[2]) / float(piViewport[3]);
					m_Perspective.fAspect = fAspect;

					if (fAspect < 1.0f)
					{
						fHWidth  = fTA * m_Perspective.fNear;
						fHHeight = fHWidth / fAspect;
					}
					else
					{
						fHHeight = fTA * m_Perspective.fNear;
						fHWidth  = fHHeight * fAspect;
					}

					glFrustum(GLdouble(-fHWidth), GLdouble(fHWidth),
							GLdouble(-fHHeight), GLdouble(fHHeight),
							GLdouble(m_Perspective.fNear),
							GLdouble(m_Perspective.fFar));
				}
				else
				{
					glFrustum(
							GLdouble(m_Perspective.fLeft),
							GLdouble(m_Perspective.fRight),
							GLdouble(m_Perspective.fBottom),
							GLdouble(m_Perspective.fTop),
							GLdouble(m_Perspective.fNear),
							GLdouble(m_Perspective.fFar));
				}

				if (rData.bUseColorStereo)
				{
					glTranslatef(rData.fColorStereoSep, 0, 0);
					glRotatef(rData.fColorStereoDegAngle, 0, 1, 0);
				}
			}
			else if (m_eProjType ==	/*EProjType::*/ PIXEL)
			{
				int iLeft   = piViewport[2] >> 1;
				int iBottom = piViewport[3] >> 1;
				int iRight  = piViewport[2] - iLeft - 1;
				int iTop    = piViewport[3] - iBottom - 1;

				glOrtho(-GLdouble(iLeft), GLdouble(iRight),
						-GLdouble(iBottom), GLdouble(iTop),
						-1.01, 1.01);
			}

			glMatrixMode(GL_MODELVIEW);

			// Update pick clip planes if in picking mode
			if (eMode == COGLBaseElement::/*EApplyMode::*/ PICK)
			{
				// Get current projection matrix
				GetProjMat();

				// Signal that pick clip planes need to be restored
				bRestorePickClipPlanes = true;

				// Store current pick clip planes
				glPushAttrib(GL_TRANSFORM_BIT);

				// Evaluate new pick clip planes
				EvalPickClipPlanes(m_pdCurPickClipPlanes, rData.iMouseX, rData.iMouseY, rData.iPickW, rData.iPickH,
						piViewport, m_pdCurProjMat, m_pdFrameMat);

				// Set new pick clip planes
				SetPickClipPlanes(m_pdCurPickClipPlanes);
			}
		}

		if (m_bLocalFrame)
		{
			rData.xMatrixStack.Push(Clu::CMatrixStack::Texture);
			rData.xMatrixStack.Push(Clu::CMatrixStack::ModelView);

			glGetFloatv(GL_ZOOM_X, &fPixelZoomX);
			glGetFloatv(GL_ZOOM_Y, &fPixelZoomY);
		}

		if (m_bResetFrame)
		{
			glLoadIdentity();
			ClearFrameMatCurrent();

			if (m_eProjType == /*EProjType::*/ CENTRAL)
			{
				glTranslatef(0.0f, 0.0f, -5.0f);
			}

			glPixelZoom(1.0f, 1.0f);
		}

		if (eMode == COGLBaseElement::PICK)
		{
			rData.PushPickName(GetUID());

			// Make sure this scene gets an entry in the select buffer independent of whether the mouse pointer is above something that is drawn
			if (m_bLocalView && m_bPickableView)
			{
				double pdProj[16], pdFrame[16];
				int piView[4];

				GetProjMat(pdProj);
				GetFrameMat(pdFrame);
				GetViewport(piView);

				double dX1, dY1, dZ1;
				gluUnProject(double(rData.iMouseX - rData.iPickW), double(rData.iMouseY - rData.iPickH), 1.0, pdFrame, pdProj, piView, &dX1, &dY1, &dZ1);

				double dX2, dY2, dZ2;
				gluUnProject(double(rData.iMouseX + rData.iPickW), double(rData.iMouseY + rData.iPickH), 1.0, pdFrame, pdProj, piView, &dX2, &dY2, &dZ2);

				// Get current pick color
				uint uPickColor = rData.ConvertNameToColor(rData.StorePickNameStack(), 0);

				glBegin(GL_QUADS);
				glColor4ubv((GLubyte*) &uPickColor);
				glVertex3d(dX1, dY1, dZ1);
				glVertex3d(dX2, dY1, dZ1);
				glVertex3d(dX2, dY2, dZ1);
				glVertex3d(dX1, dY2, dZ1);

				// Note that glGetError returns GL_INVALID_OPERATION if it is called between a call to glBegin and its corresponding call to glEnd.
				CLU_OGL_CALL(glEnd());
			}
		}

		// Store current auto adapt front face and set new value
		bInvertFrontFace          = rData.bInvertFrontFace;
		bAutoAdaptFrontFace       = rData.bAutoAdaptFrontFace;
		rData.bAutoAdaptFrontFace = m_bAutoAdaptFrontFace;

		// If in drawing mode store current coordinate system data for estimation of mouse position.
		bool bStoreFrame = (m_bIsPickable && eMode == COGLBaseElement::DRAW);

		if (bStoreFrame)
		{
			GetProjMat(m_pdMouseProjMat);
			GetFrameMat(m_pdMouseFrameMat);
			GetViewport(m_piMouseViewport);
		}

		// Only apply auto transformations if auto adjust frame is switched off
		if (!m_bAutoAdjustFrame)
		{
			if ((m_bAutoScale || m_bAutoPixelZoom) && !m_bAutoScaleAboutLocalOrigin)
			{
				ClearFrameMatCurrent();

				if (m_bAutoScale)
				{
					glScalef(m_AutoScale[0], m_AutoScale[1], m_AutoScale[2]);
				}

				if (m_bAutoPixelZoom)
				{
					glPixelZoom(m_AutoScale[0], m_AutoScale[1]);
				}
			}

			if (m_bAutoRotate1)
			{
				ClearFrameMatCurrent();

				double dAngle = 2.0 * acos(Scalar(m_mRMain[m_iDragIdxRotate1])) / m_fRadPerDeg;
				if (abs(dAngle) > 1e-4)
				{
					m_vAxis = (m_mRMain[m_iDragIdxRotate1](2)) & m_E3Base.vI;
					glRotatef(float(dAngle), m_vAxis[1], m_vAxis[2], m_vAxis[3]);
				}
			}

			if (m_bAutoTranslate)
			{
				ClearFrameMatCurrent();

				float fX, fY, fZ;
				m_mDragData[m_iDragIdxTranslate].Get(fX, fY, fZ);

				if (IsScreenPlaneDragEnabled(m_iDragIdxTranslate))
				{
					glTranslatef(fX, fY, fZ);
				}
				else if (IsProjDirDragEnabled(m_iDragIdxTranslate))
				{
					glTranslatef(fX, fY, fZ);
				}
				else
				{
					COGLVertex vexTrans = m_mDragBasis[m_iDragIdxTranslate].LinComb(fX, fY, fZ);
					glTranslatef(vexTrans[0], vexTrans[1], vexTrans[2]);
				}
			}

			if (m_bAutoRotate2)
			{
				ClearFrameMatCurrent();

				float fAngle = float(2.0 * acos(Scalar(m_mRMain[m_iDragIdxRotate2]))) / m_fRadPerDeg;

				m_vAxis = (m_mRMain[m_iDragIdxRotate2](2)) & m_E3Base.vI;
				glRotatef(fAngle, m_vAxis[1], m_vAxis[2], m_vAxis[3]);
			}

			if ((m_bAutoScale || m_bAutoPixelZoom) && (m_bAutoScaleAboutLocalOrigin || m_bAutoScaleAboutMouseOrigin))
			{
				ClearFrameMatCurrent();

				if (m_bAutoScaleAboutMouseOrigin)
				{
					glTranslatef(m_AutoScaleOffset[0], m_AutoScaleOffset[1], m_AutoScaleOffset[2]);
				}

				if (m_bAutoScale)
				{
					glScalef(m_AutoScale[0], m_AutoScale[1], m_AutoScale[2]);
				}

				if (m_bAutoPixelZoom)
				{
					glPixelZoom(m_AutoScale[0], m_AutoScale[1]);
				}
			}
		}

		// Store Modelview Frame after auto transformation
		if (bStoreFrame)
		{
			GetFrameMat(m_pdATMouseFrameMat);
		}

		if (m_bIsPickable)
		{
			COGLBEReference refPickScene;
			COGLBEReference refSinglePickScene;
			bool bUsePickScene = false;

			if (m_bIsPicked)
			{
				bUsePickScene = true;
				if ((eMode == COGLBaseElement::DRAW) && rData.bInFirstRenderPass && (rData.iRepeatIdx == 0))
				{
					if (m_dIntroTimer > 0.0)
					{
						m_bPickIntro = true;
					}
					else
					{
						m_bPickIntro = false;
					}
				}

				if (m_bPickIntro)
				{
					if (m_bNeedAnimate && !bPickStart &&
					    (eMode == COGLBaseElement::DRAW) &&
					    rData.bInFirstRenderPass && (rData.iRepeatIdx == 0))
					{
						m_dIntroTimer -= Mag(rData.dTimeStep);
					}

					if (m_dIntroTimer > 0.0)
					{
						rData.bNeedAnimate = true;
					}

					refPickScene       = m_mIntroPickSceneRef[m_iIntroPickIdx];
					refSinglePickScene = m_mSingleIntroPickSceneRef[m_iIntroPickIdx];
				}
				else
				{
					// Switch off animation of intro sequence
					//rData.bNeedAnimate = false;

					refPickScene       = m_mPickSceneRef[m_iCurPickIdx];
					refSinglePickScene = m_mSinglePickSceneRef[m_iCurPickIdx];

					//if ( m_iCurPickIdx != 0 && !refPickScene.IsValid() )
					//	refPickScene = m_mPickSceneRef[ 0 ];

					//if ( m_iCurPickIdx != 0 && !refSinglePickScene.IsValid() )
					//	refSinglePickScene = m_mSinglePickSceneRef[ 0 ];
				}
			}
			else	// if m_bIsPicked
			{
				if ((eMode == COGLBaseElement::DRAW) && rData.bInFirstRenderPass && (rData.iRepeatIdx == 0))
				{
					if (m_dOutroTimer > 0.0)
					{
						m_bPickOutro = true;
					}
					else
					{
						m_bPickOutro = false;
					}
				}

				if (m_bPickOutro)
				{
					if (m_bNeedAnimate &&
					    (eMode == COGLBaseElement::DRAW) &&
					    rData.bInFirstRenderPass && (rData.iRepeatIdx == 0))
					{
						m_dOutroTimer -= Mag(rData.dTimeStep);
					}

					if (m_dOutroTimer > 0.0)
					{
						rData.bNeedAnimate = true;
					}

					refPickScene       = m_mOutroPickSceneRef[m_iOutroPickIdx];
					refSinglePickScene = m_mSingleOutroPickSceneRef[m_iOutroPickIdx];
					bUsePickScene      = true;
				}
			}

			if (bUsePickScene)
			{
				if (refPickScene.IsValid())
				{
					pPickScene = dynamic_cast< COGLScene* >((COGLBaseElement*) refPickScene);
					if (pPickScene)
					{
						//itBegin = pPickScene->GetElementList().begin();
						//itEnd = pPickScene->GetElementList().end();

						bShowPickScene = true;
						//iRepeatCnt = pPickScene->GetRepeatCount();
					}
				}

				if (refSinglePickScene.IsValid() && (m_uPickDrawFlag == 0))
				{
					pSinglePickScene = dynamic_cast< COGLScene* >((COGLBaseElement*) refSinglePickScene);
					if (pSinglePickScene)
					{
						//itSingleBegin = pSinglePickScene->GetElementList().begin();
						//itSingleEnd = pSinglePickScene->GetElementList().end();

						// Only flag
						rData.FlagPickDraw(m_mapPickUID);

						bShowSinglePickScene = true;
						//iSingleRepeatCnt = pSinglePickScene->GetRepeatCount();
					}
				}
			}

			if (bShowSinglePickScene || bShowPickScene)
			{
				if ((eMode == COGLBaseElement::DRAW) &&
				    rData.bInFirstRenderPass &&
				    (rData.iRepeatIdx == 0))
				{
					if (bPickStart)
					{
						m_dPickTime     = 0.0;
						m_dPickTimeStep = 0.0;	//rData.dTimeStep;

						// Reset local times of pick scenes if they use local time
						COGLScene* pScene;
						if ((pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) m_mPickSceneRef[m_iCurPickIdx])))
						{
							if (pScene->m_bUseLocalTime)
							{
								pScene->m_dLocalTime = 0.0;
							}
						}

						if ((pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) m_mSinglePickSceneRef[m_iCurPickIdx])))
						{
							if (pScene->m_bUseLocalTime)
							{
								pScene->m_dLocalTime = 0.0;
							}
						}

						if ((pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) m_mIntroPickSceneRef[m_iCurPickIdx])))
						{
							if (pScene->m_bUseLocalTime)
							{
								pScene->m_dLocalTime = 0.0;
							}
						}

						if ((pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) m_mSingleIntroPickSceneRef[m_iCurPickIdx])))
						{
							if (pScene->m_bUseLocalTime)
							{
								pScene->m_dLocalTime = 0.0;
							}
						}

						if ((pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) m_mOutroPickSceneRef[m_iCurPickIdx])))
						{
							if (pScene->m_bUseLocalTime)
							{
								pScene->m_dLocalTime = 0.0;
							}
						}

						if ((pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) m_mSingleOutroPickSceneRef[m_iCurPickIdx])))
						{
							if (pScene->m_bUseLocalTime)
							{
								pScene->m_dLocalTime = 0.0;
							}
						}
					}
					else if (m_bNeedAnimate)
					{
						if (pPickScene)
						{
							pPickScene->m_bNeedAnimate = true;
						}

						if (pSinglePickScene)
						{
							pSinglePickScene->m_bNeedAnimate = true;
						}

						if (m_bPickIntro)
						{
							m_dPickTimeStep = m_dIntroTimeFactor * rData.dTimeStep;
							// Make sure Pick time does not increase by too much
							if (m_dIntroTimer < 0.0)
							{
								m_dPickTimeStep += m_dIntroTimeFactor * m_dIntroTimer;
							}
						}
						else if (m_bPickOutro)
						{
							m_dPickTimeStep = m_dOutroTimeFactor * rData.dTimeStep;
							if (m_dOutroTimer < 0.0)
							{
								m_dPickTimeStep += m_dOutroTimeFactor * m_dOutroTimer;
							}
						}
						else
						{
							m_dPickTimeStep = rData.dTimeStep;
						}

						m_dPickTime += m_dPickTimeStep;
					}
				}

				dCurPickTime     = rData.dTime;
				dCurPickTimeStep = rData.dTimeStep;

				rData.dTime     = m_dPickTime;
				rData.dTimeStep = m_dPickTimeStep;
				//rData.SwapPickTime();
			}
			//else
			//{
			//	//rData.bNeedAnimate = false;
			//}
		}

		// Store the pick state of the outside scene
		bOutsidePickState = rData.bInPickState;
		// if this scene is pickable, then only set the pick state
		// if this scene is actually picked.
		if (m_bIsPickable)
		{
			rData.bInPickState = m_bIsPicked;
		}
		// otherwise use the pick state of the outside scene.
		else
		{
			rData.bInPickState = bOutsidePickState;
		}

		if (m_bNormalize)
		{
			glGetBooleanv(GL_NORMALIZE, &bDoNormalize);
			glEnable(GL_NORMALIZE);
		}

		if (bShowSinglePickScene && pSinglePickScene)
		{
			if (!pSinglePickScene->ApplyElementList(eMode, rData))
			{
				return false;
			}
		}

		if (bShowPickScene && pPickScene)
		{
			if (!pPickScene->ApplyElementList(eMode, rData))
			{
				return false;
			}
		}

		if (!bShowPickScene)
		{
			if (!ApplyElementList(eMode, rData))
			{
				return false;
			}
		}

		rData.bInPickState = bOutsidePickState;

		if ((eMode == COGLBaseElement::DRAW) && rData.bInFirstRenderPass && (rData.iRepeatIdx == 0))
		{
			// Set local need animate flag
			m_bNeedAnimate = rData.bNeedAnimate;
			// Set global need animate flag
			rData.bNeedAnimate = rData.bNeedAnimate || bCurNeedAnimate;
		}

		if (bShowPickScene || bShowSinglePickScene)
		{
			rData.dTime     = dCurPickTime;
			rData.dTimeStep = dCurPickTimeStep;
		}

		// Restore auto adapt front face flag
		rData.bAutoAdaptFrontFace = bAutoAdaptFrontFace;
		rData.bInvertFrontFace    = bInvertFrontFace;

		if (m_bNormalize && !bDoNormalize)
		{
			glDisable(GL_NORMALIZE);
		}

		if (eMode == COGLBaseElement::PICK)
		{
			rData.PopPickName();
		}

		if (m_bLocalFrame)
		{
			rData.xMatrixStack.Pop(Clu::CMatrixStack::Texture);
			rData.xMatrixStack.Pop(Clu::CMatrixStack::ModelView);

			glPixelZoom(fPixelZoomX, fPixelZoomY);
		}

		if (m_bLocalProj)
		{
			glMatrixMode(GL_PROJECTION);
			glLoadMatrixd(m_pdProjMat);
			glMatrixMode(GL_MODELVIEW);
		}

		if (m_bLocalView)
		{
			// If we are in drawing mode, adaptive drawing is switched on
			// and
			if ((eMode == COGLBaseElement::DRAW) &&
			    m_bAdaptiveRedraw)
			{
				// Only store vieport image if in last render pass
				if (rData.bInLastRenderPass)
				{
					m_imgScene.ResizeCanvas(piViewport[2], piViewport[3]);
					glReadBuffer(GL_BACK);

					::LockImageAccess();
					glReadPixels(piViewport[0], piViewport[1], piViewport[2], piViewport[3], GL_RGBA, GL_UNSIGNED_BYTE, m_imgScene.GetDataPtr());
					::UnlockImageAccess();

					m_bHasSceneImage = true;
				}
			}
			else if (!m_bAdaptiveRedraw)
			{
				m_bHasSceneImage = false;
			}

			//glDisable( GL_SCISSOR_TEST );
			glViewport(piCurViewport[0], piCurViewport[1], piCurViewport[2], piCurViewport[3]);

			if (m_bUseViewScissor)
			{
				glScissor(piScissorBox[0], piScissorBox[1], piScissorBox[2], piScissorBox[3]);
				if (bUseScissor)
				{
					glEnable(GL_SCISSOR_TEST);
				}
				else
				{
					glDisable(GL_SCISSOR_TEST);
				}
			}

			// Restore depth range
			glDepthRange(pdDepthRange[0], pdDepthRange[1]);
			// Restore current depth level index
			rData.dDepthLevelIdx = dDepthLevelIdx;

			// If no local projection matrix had been defined
			// restore the previous one.
			if (!m_bLocalProj)
			{
				glMatrixMode(GL_PROJECTION);
				glLoadMatrixd(m_pdProjMat);
				glMatrixMode(GL_MODELVIEW);
			}
		}

		// Restore pick clip planes if in picking mode
		if ((eMode == COGLBaseElement::/*EApplyMode::*/ PICK) && bRestorePickClipPlanes)
		{
			glPopAttrib();
		}

		if (rData.bInLastRenderPass)
		{
			m_bCurPickChanged = false;
		}

		return true;
	}
	catch (Clu::CIException& ex)
	{
		throw CLU_EXCEPTION_NEST("Error applying scene", std::move(ex));
	}
}

//////////////////////////////////////////////////////////////////////
// Apply Base List

bool COGLScene::ApplyElementList(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData& rData)
{
	try
	{
		double dCurTime;

		// Check whether local time is to be used and
		if (m_bUseLocalTime)
		{
			COGLScene* pScene = 0;
			if (m_refLocalTimeScene.IsValid())
			{
				pScene = dynamic_cast< COGLScene* >((COGLBaseElement*) m_refLocalTimeScene);
			}

			if (m_bNeedAnimate &&	// Check whether there is anything to animate in this scene
			    (eMode == COGLBaseElement::DRAW) &&
			    rData.bInFirstRenderPass && (rData.iRepeatIdx == 0))
			{
				if (pScene)
				{
					pScene->m_dLocalTime += rData.dTimeStep;
				}
				else
				{
					m_dLocalTime += rData.dTimeStep;
				}
			}

			dCurTime = rData.dTime;
			if (pScene)
			{
				rData.dTime = pScene->m_dLocalTime;
			}
			else
			{
				rData.dTime = m_dLocalTime;
			}
		}

		int iCurRepeatIdx = rData.iRepeatIdx;
		for (int iRepeat = 0; iRepeat < m_iRepeatCount; iRepeat++)
		{
			// Apply list. Returns false if one element in list couldn't not be applied
			if (!COGLBaseElementList::ApplyList(m_listElement, eMode, rData))
			{
				return false;
			}

			rData.iRepeatIdx++;
		}

		rData.iRepeatIdx = iCurRepeatIdx;

		// Restore time
		if (m_bUseLocalTime)
		{
			rData.dTime = dCurTime;
		}

		return true;
	}
	catch (Clu::CIException& ex)
	{
		throw CLU_EXCEPTION_NEST("Error applying scene element list", std::move(ex));
	}
}
