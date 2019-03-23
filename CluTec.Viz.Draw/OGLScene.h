////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLScene.h
//
// summary:   Declares the ogl scene class
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

// Class written by Vladimir Banarer, 3.12.2001.

// OGLBaseElementList.h: Schnittstelle f�r die Klasse COGLBaseElementList.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLSCENE_H__5746B287_573C_4800_9DE1_B801F31D0EB4__INCLUDED_)
	#define AFX_OGLSCENE_H__5746B287_573C_4800_9DE1_B801F31D0EB4__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif	// _MSC_VER > 1000

#include "OGLBaseElementList.h"
#include "OGLBEReference.h"
#include "OGLVertex.h"
#include "OGLImage.h"

#include "time.h"
#include "sys/timeb.h"

#include <vector>
#include <map>
#include <string>
#include <deque>

	using namespace std;

#ifdef WIN32
		typedef struct _timeb TTimeStruct;
		#define GETTIME(S) _ftime_s(S)
#else
		typedef struct timeb TTimeStruct;
		#define GETTIME(S) ftime(S)
#endif

	class CLUDRAW_API COGLScene : public COGLBaseElementList
	{
	public:

		struct SDragData
		{
			void Set(float fX, float fY, float fZ)
			{ pfData[0] = fX; pfData[1] = fY; pfData[2] = fZ; }

			void Add(float fX, float fY, float fZ)
			{ pfData[0] += fX; pfData[1] += fY; pfData[2] += fZ; }

			void Get(float& fX, float& fY, float& fZ)
			{ fX = pfData[0]; fY = pfData[1]; fZ = pfData[2]; }

			float& operator[](int iIdx)
			{ return pfData[iIdx]; }

			float pfData[3];
		};

		struct SDragRange
		{
			void Set(float fMinX, float fMaxX, float fMinY, float fMaxY, float fMinZ, float fMaxZ)
			{
				pfDataMin[0] = fMinX;
				pfDataMin[1] = fMinY;
				pfDataMin[2] = fMinZ;
				pfDataMax[0] = fMaxX;
				pfDataMax[1] = fMaxY;
				pfDataMax[2] = fMaxZ;
			}

			void Clamp(SDragData& rData)
			{
				for (int i = 0; i < 3; ++i)
				{
					if (rData[i] < pfDataMin[i])
					{
						rData[i] = pfDataMin[i];
					}
					else if (rData[i] > pfDataMax[i])
					{
						rData[i] = pfDataMax[i];
					}
				}
			}

			float pfDataMin[3];
			float pfDataMax[3];
		};

		struct SDragBasis
		{
			void Set(const COGLVertex& rvexX, const COGLVertex& rvexY, const COGLVertex& rvexZ)
			{ pvexBasis[0] = rvexX; pvexBasis[1] = rvexY; pvexBasis[2] = rvexZ; }

			void Get(COGLVertex& rvexX, COGLVertex& rvexY, COGLVertex& rvexZ)
			{ rvexX = pvexBasis[0]; rvexY = pvexBasis[1]; rvexZ = pvexBasis[2]; }

			COGLVertex LinComb(float fX, float fY, float fZ)
			{       return fX * pvexBasis[0] + fY * pvexBasis[1] + fZ * pvexBasis[2]; }

			COGLVertex pvexBasis[3];
		};

		struct SPerspective
		{
			float fAngle;
			float fAspect;
			float fNear;
			float fFar;

			float fLeft;
			float fRight;
			float fBottom;
			float fTop;

			bool bUseAngle;

			void Set(float _fAngle, float _fAspect, float _fNear, float _fFar)
			{
				fAngle    = _fAngle;
				fAspect   = _fAspect;
				fNear     = _fNear;
				fFar      = _fFar;
				bUseAngle = true;
			}

			void SetVolume(float _fLeft, float _fRight, float _fBottom, float _fTop, float _fNear, float _fFar)
			{
				fLeft     = _fLeft;
				fRight    = _fRight;
				fBottom   = _fBottom;
				fTop      = _fTop;
				fNear     = _fNear;
				fFar      = _fFar;
				bUseAngle = false;
			}
		};

		struct SOrthoProj
		{
			float fLeft;
			float fRight;
			float fBottom;
			float fTop;
			float fNear;
			float fFar;

			void Set(float _fLeft, float _fRight, float _fBottom, float _fTop, float _fNear, float _fFar)
			{
				fLeft   = _fLeft;
				fRight  = _fRight;
				fBottom = _fBottom;
				fTop    = _fTop;
				fNear   = _fNear;
				fFar    = _fFar;
			}
		};

		struct SViewport
		{
			float fLeft;
			float fBottom;
			float fDepth;
			float fWidth;
			float fHeight;

			// If true, the corresponding above values are percentages of the current viewport size
			bool bRelLeft, bRelBottom;
			bool bRelWidth, bRelHeight;

			// If true, the above values are given in local coordinate system values
			bool bLocalOrigin;
			bool bLocalExtent;

			// Alignment of viewport to local coordinates
			float fAlignX, fAlignY;

			// Depth level data
			// The viewport depth level index. Need not be an integer.
			double dDepthLevel;
			// DL_Relative: Depth level is relative to previous scene
			// DL_Absolute: Depth level is absolute
			// DL_Local: Depth level depends on local depth coordinate
			enum EDepthLevelType { DL_Relative = 0, DL_Absolute, DL_Local } eDepthLevelType;
			// Range of local depth variable that is mapped to
			// depth level range
			double dDLLocalNear, dDLLocalFar;
			// Range of depth levels to which local depth value range is mapped
			double dDLNear, dDLFar;

			// Pixel Offset
			int iBorderLeft, iBorderRight;
			int iBorderTop, iBorderBottom;

			// Reference to Scene whose viewport is used as origin reference
			COGLBEReference refSceneOrigin;
			bool bSceneLeft, bSceneBottom;

			void Set(float _fLeft,
					float _fBottom,
					float _fWidth,
					float _fHeight,
					bool _bRelLeft,
					bool _bRelBottom,
					bool _bRelWidth,
					bool _bRelHeight,
					float _fDepth = 0.0f,
					bool _bLocalOrigin = false,
					bool _bLocalExtent = false,
					float _fAlignX = 0.0f,
					float _fAlignY = 0.0f,
					int _iBLeft = 0,
					int _iBRight = 0,
					int _iBBottom = 0,
					int _iBTop = 0)
			{
				fLeft         = _fLeft;
				fBottom       = _fBottom;
				fWidth        = _fWidth;
				fHeight       = _fHeight;
				bRelLeft      = _bRelLeft;
				bRelBottom    = _bRelBottom;
				bRelWidth     = _bRelWidth;
				bRelHeight    = _bRelHeight;
				fDepth        = _fDepth;
				bLocalOrigin  = _bLocalOrigin;
				bLocalExtent  = _bLocalExtent;
				fAlignX       = _fAlignX;
				fAlignY       = _fAlignY;
				iBorderLeft   = _iBLeft;
				iBorderRight  = _iBRight;
				iBorderBottom = _iBBottom;
				iBorderTop    = _iBTop;
			}
		};

		enum EProjType
		{
			CENTRAL = 0,
			ORTHO,
			PIXEL
		};

		enum EScaleFunc
		{
			LINEAR = 0,
			EXP
		};

		enum EPickType
		{
			PICK_STANDARD = 0,
			PICK_INTRO,
			PICK_OUTRO
		};

	public:

		COGLScene();
		COGLScene(const COGLScene& rList);
		virtual ~COGLScene();

		COGLScene& operator=(const COGLScene& rList);

		virtual COGLBaseElement* Copy()
		{
			return (COGLBaseElement*) new COGLScene(*this);
		}

		void SetPickScene(COGLBEReference& rScene,
				bool bSingle = false,
				bool bShift = false,
				bool bCtrl = false,
				bool bAlt = false,
				bool bDrag = false,
				EPickType eType = PICK_STANDARD,
				double dTimer = 0.0,
				double dFactor = 1.0)
		{
			int iIdx = GetPickSceneIdx(bShift, bCtrl, bAlt, bDrag);

			if (eType == PICK_STANDARD)
			{
				if (bSingle) { m_mSinglePickSceneRef[iIdx] = rScene; }
				else{ m_mPickSceneRef[iIdx] = rScene; }
			}
			else if (eType == PICK_INTRO)
			{
				if (bSingle) { m_mSingleIntroPickSceneRef[iIdx] = rScene; }
				else{ m_mIntroPickSceneRef[iIdx] = rScene; }

				m_mIntroPickTimer[iIdx]      = dTimer;
				m_mIntroPickTimeFactor[iIdx] = dFactor;
			}
			else if (eType == PICK_OUTRO)
			{
				if (bSingle) { m_mSingleOutroPickSceneRef[iIdx] = rScene; }
				else{ m_mOutroPickSceneRef[iIdx] = rScene; }

				m_mOutroPickTimer[iIdx]      = dTimer;
				m_mOutroPickTimeFactor[iIdx] = dFactor;
			}
		}

		void ClearPickScene()
		{
			for (int i = 0; i < 16; ++i)
			{
				m_mPickSceneRef[i].Clear();
				m_mIntroPickSceneRef[i].Clear();
				m_mOutroPickSceneRef[i].Clear();

				m_mSinglePickSceneRef[i].Clear();
				m_mSingleIntroPickSceneRef[i].Clear();
				m_mSingleOutroPickSceneRef[i].Clear();

				m_mIntroPickTimer[i] = 0.0;
				m_mOutroPickTimer[i] = 0.0;
			}
		}

		virtual COGLBEReference GetElementWithUID(uint uUID);

		// Enable/Disable Drawing in opaque pass
		void EnableOpaqueDraw(bool bVal = true)
		{ m_bDrawOpaque = bVal; }

		// Enable/Disable Drawing in transparency pass
		void EnableTransparentDraw(bool bVal = true)
		{ m_bDrawTransparent = bVal; }

		// Enable/Disable automatic adaptation of vertex list front face
		// according to whether current frame is reflected (determinant < 0)
		void EnableAutoAdaptFrontFace(bool bVal = true)
		{ m_bAutoAdaptFrontFace = bVal; }

		// Make scene pickable
		void EnablePick(bool bVal = true)
		{ m_bIsPickable = bVal; }

		// Draw scene in pick mode
		void EnablePickDraw(bool bVal = true)
		{
			m_bDoPickDraw = bVal;
		}

		// Notify Script on actions
		void EnableNotify(bool bVal = true)
		{
			m_bDoNotify           = bVal;
			m_bDoNotifyMouseClick = bVal;
			m_bDoNotifyMouseOver  = bVal;
			m_bDoNotifyMouseDrag  = bVal;
		}

		// Notify Script on particular actions
		void EnableNotifyMouseClick(bool bVal = true)
		{
			m_bDoNotifyMouseClick = bVal;
			m_bDoNotify           = (m_bDoNotifyMouseClick ||
						 m_bDoNotifyMouseOver ||
						 m_bDoNotifyMouseDrag);
		}

		// Notify Script on particular actions
		void EnableNotifyMouseOver(bool bVal = true)
		{
			m_bDoNotifyMouseOver = bVal;
			m_bDoNotify          = (m_bDoNotifyMouseClick ||
						m_bDoNotifyMouseOver ||
						m_bDoNotifyMouseDrag);
		}

		// Notify Script on particular actions
		void EnableNotifyMouseDrag(bool bVal = true)
		{
			m_bDoNotifyMouseDrag = bVal;
			m_bDoNotify          = (m_bDoNotifyMouseClick ||
						m_bDoNotifyMouseOver ||
						m_bDoNotifyMouseDrag);
		}

		// Use local transformation
		void EnableLocalFrame(bool bVal = true)
		{ m_bLocalFrame = bVal; }

		void EnableLocalProj(bool bVal = true)
		{ m_bLocalProj = bVal; }

		void EnableLocalView(bool bVal = true)
		{ m_bLocalView = bVal; }

		void EnableViewScissor(bool bVal = true)
		{ m_bUseViewScissor = bVal; }

		void EnableResetFrame(bool bVal = true)
		{ m_bResetFrame = bVal; }

		void EnablePickableView(bool bVal = true)
		{ m_bPickableView = bVal; }

		void EnableOrthoViewportAspectRatio(bool bVal = true)
		{ m_bOrthoViewportAspectRatio = bVal; }

		void SetProjType(EProjType eType)
		{ m_eProjType = eType; }

		// Enable/Disable auto translation
		void EnableAutoTranslate(bool bVal = true)
		{ m_bAutoTranslate = bVal; }

		// Enable/Disable auto rotation
		void EnableAutoRotate1(bool bVal = true)
		{ m_bAutoRotate1 = bVal; }

		// Enable/Disable auto rotation
		void EnableAutoRotate2(bool bVal = true)
		{ m_bAutoRotate2 = bVal; }

		// Enable/Disable auto scale
		void EnableAutoScale(bool bVal = true)
		{ m_bAutoScale = bVal; EnableNormalize(bVal); if (bVal) { EvalAutoScale(); } }

		// Enable/Disable auto pixel zoom
		void EnableAutoPixelZoom(bool bVal = true)
		{ m_bAutoPixelZoom = bVal; if (bVal) { EvalAutoScale(); } }

		// Enable/Disable auto scale about local origin, i.e. after local translate
		void EnableAutoScaleAboutLocalOrigin(bool bVal = true)
		{ m_bAutoScaleAboutLocalOrigin = bVal; }

		// Enable/Disable auto scale about local origin at mouse position after local translate
		void EnableAutoScaleAboutMouseOrigin(bool bVal = true)
		{ m_bAutoScaleAboutMouseOrigin = bVal; }

		// Enable/Disable auto adjusting of frame
		void EnableAutoAdjustFrame(bool bVal = true)
		{ m_bAutoAdjustFrame = bVal; }

		// Enable/Disable buffer scene
		void EnableBufferScene(bool bVal = true)
		{ m_bBufferScene = bVal; }

		// Enable/Disable draw scene
		void EnableDrawScene(bool bVal = true)
		{ m_bDrawScene = bVal; }

		void EnableNormalize(bool bVal = true)
		{ m_bNormalize = bVal; }

		// Is Scene to be drawn?
		bool DoDrawScene()
		{ return m_bDrawScene; }

		// Enable Adaptive Redraw
		void EnableAdaptiveRedraw(bool bVal = true);

		// Enforce Redraw in adaptive redraw mode
		void DoAdaptiveRedraw()
		{ TellParentContentChanged(); }

		// Enable/Disable animated dragging for particular indices
		void EnableAnimDrag(int iDataIdx, bool bVal = true);

		// Reset Animated Drag
		void ResetAnimDrag(int iDataIdx);

		// Enable/Disable dragging for particular indices
		void EnableDrag(int iDataIdx, bool bVal = true);

		// Enable/Disable dragging parallel to screen plane for particular indices
		void EnableDragInScreenPlane(int iDataIdx, bool bVal = true);

		// Enable/Disable dragging in projected direction for particular indices
		void EnableDragInProjDir(int iDataIdx, bool bVal = true);

		// Set Drag Idx for Translation
		void SetDragIdxTranslate(int iIdx)
		{ if ((iIdx >= 0) && (iIdx <= 3)) { m_iDragIdxTranslate = iIdx; } }

		// Set Drag Idx for Rotation BEFORE Translation
		void SetDragIdxRotate1(int iIdx)
		{ if ((iIdx >= 0) && (iIdx <= 3)) { m_iDragIdxRotate1 = iIdx; } }

		// Set Drag Idx for Rotation AFTER Translation
		void SetDragIdxRotate2(int iIdx)
		{ if ((iIdx >= 0) && (iIdx <= 3)) { m_iDragIdxRotate2 = iIdx; } }

		// Set Drag Idx for Scaling
		void SetDragIdxScale(int iIdx)
		{ if ((iIdx >= 0) && (iIdx <= 3)) { m_iDragIdxScale = iIdx; } }

		int GetDragIdxTranslate()
		{ return m_iDragIdxTranslate; }

		int GetDragIdxRotate1()
		{ return m_iDragIdxRotate1; }

		int GetDragIdxRotate2()
		{ return m_iDragIdxRotate2; }

		int GetDragIdxScale()
		{ return m_iDragIdxScale; }

		// Set Scale Function
		void SetScaleFunc(EScaleFunc eScaleFunc)
		{ m_eScaleFunc = eScaleFunc; }

		bool IsPickable()
		{ return m_bIsPickable; }

		bool DoNotify()
		{ return m_bDoNotify; }

		bool DoNotifyMouseClick()
		{ return m_bDoNotifyMouseClick; }

		bool DoNotifyMouseOver()
		{ return m_bDoNotifyMouseOver; }

		bool DoNotifyMouseDrag()
		{ return m_bDoNotifyMouseDrag; }

		// Is Animated Drag Enabled?
		bool IsAnimDragEnabled(int iDataIdx)
		{
			if (uint(iDataIdx) > m_mAnimDragEnable.Count()) { return false; }
			return m_mAnimDragEnable[iDataIdx];
		}

		// Is Animated Drag Active?
		bool IsAnimDragActive(int iDataIdx)
		{
			if (uint(iDataIdx) > m_mAnimDragActive.Count()) { return false; }
			return m_mAnimDragActive[iDataIdx];
		}

		// Is Drag Enabled?
		bool IsDragEnabled(int iDataIdx)
		{
			if (uint(iDataIdx) > m_mAllowDrag.Count()) { return false; }
			return m_mAllowDrag[iDataIdx];
		}

		bool IsScreenPlaneDragEnabled(int iDataIdx)
		{
			if (uint(iDataIdx) > m_mScreenPlaneDrag.Count()) { return false; }
			return m_mScreenPlaneDrag[iDataIdx];
		}

		bool IsProjDirDragEnabled(int iDataIdx)
		{
			if (uint(iDataIdx) > m_mProjDirDrag.Count()) { return false; }
			return m_mProjDirDrag[iDataIdx];
		}

		bool IsDragEnabled()
		{
			bool bDrag;
			int i, iCnt = int(m_mAllowDrag.Count());
			for (bDrag = false, i = 0;
			     i < iCnt;
			     (bDrag = (bDrag || m_mAllowDrag[i])), ++i) {; }

			return bDrag;
		}

		// Get number of drag data groups
		int GetDragDataCount()
		{ return int(m_mAllowDrag.Count()); }

		// Reset the drag position data to zero
		virtual void ResetDragData();

		// Function called when mouse movement influences scene
		virtual bool ActiveMouseMove(int iDragIdx, int iMouseX, int iMouseY, int iStepX, int iStepY, bool bShiftDown, bool bCtrlDown, bool bAltDown);
		// Mouse 3D Move
		virtual void Mouse3D_Move(double dTX, double dTY, double dTZ, double dLength, double dRX, double dRY, double dRZ, double dAngle);

		// Function called to change data values through dragging
		virtual bool SetDragData(int iDataIdx, float fX, float fY, float fZ);

		// Function called to change data values through dragging
		virtual bool AddToDragData(int iDataIdx, float fX, float fY, float fZ);

		// Function called to change data values through dragging
		virtual bool AddToAnimDragData(int iDataIdx, float fX, float fY, float fZ);

		// Function called to change animated draggin attenuation values
		virtual bool SetAnimDragAttenuation(int iDataIdx, float fX, float fY, float fZ);

		// Function called to animate dragging
		virtual bool StepAnimDrag();

		// Function called to get data values from dragging
		virtual bool GetDragData(int iDataIdx, float& fX, float& fY, float& fZ);

		// Function called to get data values from dragging
		virtual bool GetDragDataStep(int iDataIdx, float& fX, float& fY, float& fZ);

		// Set factors that are multiplied with drag data values
		bool SetDragFactor(int iDataIdx, float fX, float fY, float fZ);

		// Get factors that are multiplied with drag data values
		bool GetDragFactor(int iDataIdx, float& fX, float& fY, float& fZ);

		// Set Auto Translation Basis
		bool SetDragBasis(int iDataIdx, const COGLVertex& vexX, const COGLVertex& vexY, const COGLVertex& vexZ);

		// Get Auto Translation Basis
		bool GetDragBasis(int iDataIdx, COGLVertex& vexX, COGLVertex& vexY, COGLVertex& vexZ);

		// Enable Drag Range
		void EnableDragRange(int iDataIdx, bool bVal);

		// Set Drag Range
		bool SetDragRange(int iDataIdx,  float fMinX, float fMaxX, float fMinY, float fMaxY, float fMinZ, float fMaxZ);

		// Set Projection Parameters
		void SetProj(float fAngle, float fAspect, float fNear, float fFar)
		{ m_Perspective.Set(fAngle, fAspect, fNear, fFar); }

		// Set Projection Volume Parameters
		void SetProjVolume(float fLeft, float fRight, float fBottom, float fTop, float fNear, float fFar)
		{ m_Perspective.SetVolume(fLeft, fRight, fBottom, fTop, fNear, fFar); }

		// Set Orthographic Projection Parameters
		void SetOrthoProj(float fLeft,  float fRight, float fBottom, float fTop, float fNear,  float fFar)
		{ m_OrthoProj.Set(fLeft, fRight, fBottom, fTop, fNear, fFar); }

		// Get View Parameters
		void GetViewportSize(int& iWidth, int& iHeight)
		{
			iWidth  = m_piViewport[2];
			iHeight = m_piViewport[3];
		}

		// Set View Parameters
		void SetView(float fLeft, float fBottom, float fWidth, float fHeight, bool bRelLeft, bool bRelBottom, bool bRelWidth, bool bRelHeight)
		{
			m_Viewport.Set(fLeft, fBottom, fWidth, fHeight,
					bRelLeft, bRelBottom, bRelWidth, bRelHeight);
		}

		void SetViewOriginRef(COGLBEReference& refScene, bool bLeft, bool bBottom)
		{
			m_Viewport.refSceneOrigin = refScene;
			m_Viewport.bSceneLeft     = bLeft;
			m_Viewport.bSceneBottom   = bBottom;
		}

		void SetViewLocal(bool bLocalOrigin, bool bLocalExtent, float fDepth)
		{
			m_Viewport.bLocalOrigin = bLocalOrigin;
			m_Viewport.bLocalExtent = bLocalExtent;
			m_Viewport.fDepth       = fDepth;
		}

		void SetViewAlign(float fAlignX, float fAlignY)
		{
			m_Viewport.fAlignX = fAlignX;
			m_Viewport.fAlignY = fAlignY;
		}

		void SetViewBorder(int iBLeft, int iBRight, int iBBottom, int iBTop)
		{
			m_Viewport.iBorderLeft   = iBLeft;
			m_Viewport.iBorderRight  = iBRight;
			m_Viewport.iBorderBottom = iBBottom;
			m_Viewport.iBorderTop    = iBTop;
		}

		// Set Depth Range
		void SetDepthRange(double dNear, double dFar)
		{
			m_dDepthNear = GLdouble((dNear < 0.0 ? 0.0 : (dNear > 1.0 ? 1.0 : dNear)));
			m_dDepthFar  = GLdouble((dFar < 0.0 ? 0.0 : (dFar > 1.0 ? 1.0 : dFar)));
		}

		// Set Depth Level
		void SetDepthLevel(double dDL)
		{
			if (dDL < -10.0) { dDL = -10.0; }
			else if (dDL > 10.0) { dDL = 10.0; }
			m_Viewport.dDepthLevel = dDL;
		}

		// Set Depth Level Type
		void SetDepthLevelType(SViewport::EDepthLevelType eDLT)
		{
			m_Viewport.eDepthLevelType = eDLT;
		}

		// SetDepthLevelLocal
		void SetDepthLevelLocalRange(double dLocalMin, double dLocalMax, double dLevelMin, double dLevelMax)
		{
			m_Viewport.dDLLocalNear = dLocalMin;
			m_Viewport.dDLLocalFar  = dLocalMax;

			if (dLevelMin < -10.0) { dLevelMin = -10.0; }
			else if (dLevelMin > 10.0) { dLevelMin = 10.0; }

			if (dLevelMax < -10.0) { dLevelMax = -10.0; }
			else if (dLevelMax > 10.0) { dLevelMax = 10.0; }

			m_Viewport.dDLNear = dLevelMin;
			m_Viewport.dDLFar  = dLevelMax;
		}

		// Enable/Disable the use of local time
		void EnableLocalTime(bool bVal = true)
		{ m_bUseLocalTime = bVal; }

		// Set the local time
		void SetLocalTime(double dVal)
		{ m_dLocalTime = dVal; }

		// Get the local time
		double GetLocalTime()
		{ return m_dLocalTime; }

		void SetLocalTimeSceneRef(COGLBEReference refScene)
		{ m_refLocalTimeScene = refScene; }

		void ClearLocalTimeSceneRef()
		{ m_refLocalTimeScene.Clear(); }

		// Set Repeat Count
		void SetRepeatCount(int iCount)
		{ m_iRepeatCount = (iCount < 0 ? 0 : iCount); }

		// Get Repeat Count
		int GetRepeatCount()
		{ return m_iRepeatCount; }

		// Set Auto Adjust Frame Reference
		void SetAutoAdjustFrame(COGLBEReference& rFrameRef)
		{ m_refAutoFrame = rFrameRef; }

		// Set Scene whose frame context is used for auto rotation and translation
		void SetFrameContextScene(COGLBEReference& rFrameContext)
		{
			if (rFrameContext.IsValid())
			{
				COGLScene* pScene = dynamic_cast<COGLScene*>((COGLBaseElement*) rFrameContext);
				if (pScene)
				{
					m_refFrameContextScene        = rFrameContext;
					pScene->m_refFrameContextUser = rFrameContext.GetRepositoryPtr()->New(this);
				}
			}
			else
			{
				m_refFrameContextScene = rFrameContext;
			}
		}

		// Set Drag Start Mouse World Coordinates
		void SetMouseDragStartWorld(double dX, double dY, double dZ)
		{ m_pdMouseDragStartWorld[0] = dX; m_pdMouseDragStartWorld[1] = dY; m_pdMouseDragStartWorld[2] = dZ; }

		// Set Drag Start Mouse Local Coordinates
		void SetMouseDragStartLocal(double dX, double dY, double dZ)
		{ m_pdMouseDragStartLocal[0] = dX; m_pdMouseDragStartLocal[1] = dY; m_pdMouseDragStartLocal[2] = dZ; }

		// Set List of Scenes that trigger pick scene of this scene.
		// This also replaces the current pick scene.
		void SetPickTriggerScenes(vector< COGLBEReference >& vecTrigScene);

		// Project screen position
		void ProjectScreenPos(double dX, double dY, double dZ, double& dMX, double& dMY, double& dMZ);

		// Unproject screen position
		void UnProjectScreenPos(double dSX, double dSY, double dSZ, double& dX, double& dY, double& dZ);

		// Project screen position
		void ATProjectScreenPos(double dX, double dY, double dZ, double& dMX, double& dMY, double& dMZ);

		// Unproject screen position
		void ATUnProjectScreenPos(int iX, int iY, double dSZ, double& dX, double& dY, double& dZ);

		// Project screen position
		void ARProjectScreenPos(double dX, double dY, double dZ, double& dMX, double& dMY, double& dMZ);

		// Unproject screen position
		void ARUnProjectScreenPos(int iX, int iY, double dSZ, double& dX, double& dY, double& dZ);

		void GetProjectionMatrices(double* pdProjectMatrix, double* pdFrameMatrix, double* pdLocalFrameMatrix, int* piViewport);

		// Apply list
		virtual bool Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData& rData);

		bool ApplyElementList(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData& rData);

	protected:

		virtual void DragDataChanged(int iDataIdx) {         }

		void UpdateAutoAdjustFrame();
		void EvalAutoScale();

		int GetPickSceneIdx(bool bShift, bool bCtrl, bool bAlt, bool bDrag)
		{
			return (bShift ? 1 : 0) | (bCtrl ? 2 : 0) | (bAlt ? 4 : 0) | (bDrag ? 8 : 0);
		}

		void TellParentContentChanged();

		// Either read current projection matrix from OpenGL or copy
		// previously read matrix.
		void GetProjMat();
		void GetProjMat(GLdouble* pMat);
		void ClearProjMatCurrent() { m_bProjMatCurrent = false; }

		// Either read current model view matrix from OpenGL or copy
		// previously read matrix.
		void GetFrameMat();
		void GetFrameMat(GLdouble* pMat);
		void ClearFrameMatCurrent() { m_bFrameMatCurrent = false; }

		// Either read current viewport from OpenGL or copy
		// previously read viewport.
		void GetViewport();
		void GetViewport(GLint* pViewport);
		void ClearViewportCurrent() { m_bViewportCurrent = false; }

		// Get current clip planes for picking mode
		void GetPickClipPlanes(GLdouble pdClipPlanes[4][4]);
		// Set the clip planes for picking mode
		void SetPickClipPlanes(GLdouble pdClipPlanes[4][4]);
		// Evaluate Pick Clip Planes
		void EvalPickClipPlanes(GLdouble pdClipPlanes[4][4],
					int iX, int iY, int iPickW, int iPickH,
					GLint * piViewport, GLdouble * pdProj, GLdouble * pdFrame);

	protected:

		// Scene that is drawn instead of this scene when it is selected,
		// for all repeated instances of scene.
		// List of 8 pick scenes for all combinations of shift, ctrl and alt pressed.
		MemObj< COGLBEReference > m_mPickSceneRef;

		// Scene that is drawn instead of this scene when it is selected,
		// only once.
		// List of 8 pick scenes for all combinations of shift, ctrl and alt pressed.
		MemObj< COGLBEReference > m_mSinglePickSceneRef;
		MemObj< COGLBEReference > m_mSingleIntroPickSceneRef;
		MemObj< COGLBEReference > m_mSingleOutroPickSceneRef;

		// Intro pick scenes
		MemObj< COGLBEReference > m_mIntroPickSceneRef;

		// Outro pick scenes
		MemObj< COGLBEReference > m_mOutroPickSceneRef;

		// Intro pick scene timer. Gives times in seconds the intro scene is shown
		// before the standard pick scene is shown.
		Mem< double > m_mIntroPickTimer;

		// Outro pick scene timer. Gives time in second the outro scene is shown
		// before the standard scene is shown.
		Mem< double > m_mOutroPickTimer;

		// Intro pick scene time factor
		Mem< double > m_mIntroPickTimeFactor;

		// Outro pick scene time factor
		Mem< double > m_mOutroPickTimeFactor;

		// Automatic checking whether det(frame) < 0
		bool m_bAutoAdaptFrontFace;

		// Does scene react at all to picking?
		bool m_bIsPickable;

		// Draw scene in pick mode?
		bool m_bDoPickDraw;

		// Should script be notified on actions?
		bool m_bDoNotify;
		bool m_bDoNotifyMouseOver;
		bool m_bDoNotifyMouseClick;
		bool m_bDoNotifyMouseDrag;

		// Should Frame be pushed/popped before/after Apply()?
		bool m_bLocalFrame;

		// Local projection matrix
		bool m_bLocalProj;

		// Local viewport
		bool m_bLocalView;

		// Enable Scissor box of viewport
		bool m_bUseViewScissor;

		// Reset Frame at start of scene
		bool m_bResetFrame;

		// Enable drawing of scene
		bool m_bDrawScene;

		// Draw in opaque pass
		bool m_bDrawOpaque;

		// Draw in transparency pass
		bool m_bDrawTransparent;

		// Type of projection
		EProjType m_eProjType;

		// Use Orthographic projection with aspect ratio of viewport
		bool m_bOrthoViewportAspectRatio;

		// Area of viewport is pickable independet
		// of whether something is drawn in it
		bool m_bPickableView;

		// Translation by dragging enabled?
		bool m_bAutoTranslate;

		// Rotation BEFORE Translation by Dragging enabled?
		bool m_bAutoRotate1;

		// Rotation AFTER Translation by Dragging enabled?
		bool m_bAutoRotate2;

		// Scaling by Dragging enabled?
		bool m_bAutoScale;

		// Image Scaling by Dragging enables?
		bool m_bAutoPixelZoom;

		// Scale about local origin, i.e. after auto translate
		bool m_bAutoScaleAboutLocalOrigin;

		// Scale about local position of mouse at start of dragging
		bool m_bAutoScaleAboutMouseOrigin;

		// Only redraw when children have changed. Otherwise draw stored pixel map
		bool m_bAdaptiveRedraw;

		// Used by adaptive redraw to flag whether image of scene has been stored
		bool m_bHasSceneImage;

		// Used by adaptive redraw to store image of scene
		COGLImage m_imgScene;

		// Scaling function
		EScaleFunc m_eScaleFunc;

		// Buffer scene in display list or some other way
		bool m_bBufferScene;

		// Adjust given frame through dragging
		bool m_bAutoAdjustFrame;

		// Enable normalization of normals
		bool m_bNormalize;

		// This scene has elements that need to be animated
		bool m_bNeedAnimate;

		// Scene is picked
		bool m_bIsPicked;

		// Has local time which is not automatically reset to zero after an animation
		bool m_bUseLocalTime;

		// The local time
		double m_dLocalTime;

		// Local time reference scene
		COGLBEReference m_refLocalTimeScene;

		// The time since start of picking
		double m_dPickTime;

		// The pick time step
		double m_dPickTimeStep;

		// Repeated visualization count
		int m_iRepeatCount;

		// Drag Index for auto translation
		int m_iDragIdxTranslate;

		// Drag Index for auto rotation BEFORE auto translation
		int m_iDragIdxRotate1;

		// Drag Index for auto rotation AFTER auto translation
		int m_iDragIdxRotate2;

		// Drag Index for auto scale
		int m_iDragIdxScale;

		// Has this scene been picked previously
		unsigned int m_uPickDrawFlag;

		// Current Pick Idx
		int m_iCurPickIdx;
		int m_iPickIdx;

		bool m_bPickIntro;
		bool m_bPickOutro;

		// Current intro pick idx
		int m_iIntroPickIdx;

		// Current outro pick idx
		int m_iOutroPickIdx;

		// Current Pick has changed
		bool m_bCurPickChanged;

		// Intro timer
		double m_dIntroTimer;

		// Outro timer
		double m_dOutroTimer;

		// Intro Time Factor
		double m_dIntroTimeFactor;

		// Outro Time Factor
		double m_dOutroTimeFactor;

		// Which dragging is allowed
		Mem<bool> m_mAllowDrag;

		// Drag parallel to screen plane
		Mem<bool> m_mScreenPlaneDrag;

		// Drag along projected directions
		Mem<bool> m_mProjDirDrag;

		// Animation Drag Data
		Mem< SDragData > m_mAnimDragData;

		// Animation Drag Data Attenuation
		Mem< SDragData > m_mAnimDragAttenuation;

		// Enable/Disable Anim Drag
		Mem< bool > m_mAnimDragEnable;

		// Animated Drag Active Flags
		Mem< bool > m_mAnimDragActive;

		// Drag Data
		Mem< SDragData > m_mDragData;

		// Last Drag Data Step
		Mem< SDragData > m_mDragDataStep;

		// Drag factors
		Mem< SDragData > m_mDragFactor;

		// Auto translate basis
		Mem< SDragBasis > m_mDragBasis;

		// Drag Value Range
		Mem< SDragRange > m_mDragRange;

		// Use Drag Value Range
		Mem< bool > m_mUseDragRange;

		// Start drag position of mouse in world coordinates
		double m_pdMouseDragStartWorld[3];

		// Start drag position of mouse in local coordinates
		double m_pdMouseDragStartLocal[3];

		// Mouse Movement Queue
		deque<float> m_qMouseStepX, m_qMouseStepY;

		// Time of Mouse Movement Step
		deque<double> m_qMouseTime;

		// Smooth Mouse Movement?
		bool m_bMouseMoveSmooth;

		// Max. number of entries in Mouse smoothing queue
		int m_iMouseMoveQueueLen;

		// Mouse move smoothing time factor in seconds
		float m_fMouseMoveSmoothTime;

		// Current scale when auto scale is active.
		// This is used to adjust translation drag factor.
		SDragData m_AutoScale;

		// Offset for scaling
		SDragData m_AutoScaleOffset;

		// List of UIDs that switch scene to pick scene
		// Typically this list only contains the UID of the scene itself.
		// However, an arbitrary list of UIDs is allowed.
		map<uint, uint> m_mapPickUID;

		// Perspective Projection Data
		SPerspective m_Perspective;

		// Orthographic projection data
		SOrthoProj m_OrthoProj;

		// Viewport data
		SViewport m_Viewport;

		// Variables storing rotation
		MemObj<MultiV<float> > m_mRMain;
		MultiV<float> m_vU, m_vAxis;

		// 3D Space for rotor evaluation
		E3GA<float> m_E3Base;

		// General constants
		float m_fPi, m_fRadPerDeg;

		// true if m_pdProjMat contains current projection matrix
		bool m_bProjMatCurrent;

		// true if m_pdFrameMat contains current model view matrix
		bool m_bFrameMatCurrent;

		// true if m_piViewport contains current viewport
		bool m_bViewportCurrent;

		// Projection matrix store
		GLdouble m_pdProjMat[16];

		// Modelview matrix store
		GLdouble m_pdFrameMat[16];

		// Projection matrix store for calculation of mouse position
		GLdouble m_pdMouseProjMat[16];

		// Modelview matrix store for calculation of mouse position
		GLdouble m_pdMouseFrameMat[16];

		// Modelview matrix store for calculation of mouse position
		GLdouble m_pdATMouseFrameMat[16];

		// Modelview matrix store for calculation of mouse position
		GLdouble m_pdARMouseFrameMat[16];

		// Viewport store for calculation of mouse position
		GLint m_piMouseViewport[4];

		// Viewport of this scene
		GLint m_piViewport[4];

		// Projection matrix store
		GLdouble m_pdCurProjMat[16];

		// Modelview matrix store
		GLdouble m_pdCurFrameMat[16];

		// Current Viewport of this scene
		GLint m_piCurViewport[4];

		// Depth range used by scene
		GLdouble m_dDepthNear, m_dDepthFar;

		// Clip Plane Store for picking mode
		GLdouble m_pdPrevPickClipPlanes[4][4];
		// Current clip planes for this scene
		GLdouble m_pdCurPickClipPlanes[4][4];

		// Display List ID
		uint m_uDispListID;

		// Reference to frame for auto adjusting
		COGLBEReference m_refAutoFrame;

		// Reference to scene whose frame context is used for auto translation and rotation
		COGLBEReference m_refFrameContextScene;

		// Reference to scene that uses frame context of this scene
		COGLBEReference m_refFrameContextUser;
	};

#endif	// !defined(AFX_OGLBASEELEMENTLIST_H__5746B287_573C_4800_9DE1_B801F31D0EB4__INCLUDED_)
