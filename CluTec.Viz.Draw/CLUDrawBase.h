////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      CLUDrawBase.h
//
// summary:   Declares the clu draw base class
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

// cluDrawBase.h: Schnittstelle f�r die Klasse CCLUDrawBase.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLUDRAWBASE_H__6EB9C283_EE5B_11D5_BA34_00E07D8AFD6A__INCLUDED_)
	#define AFX_CLUDRAWBASE_H__6EB9C283_EE5B_11D5_BA34_00E07D8AFD6A__INCLUDED_

#if _MSC_VER > 1000
	#pragma once
#endif	// _MSC_VER > 1000

#include "CluTec.Viz.OpenGL/Api.h"

#include "OGLBERepository.h"
#include "OGLColor.h"
#include "OGLLight.h"
#include "OGLMaterial.h"
#include "OGLBitmap.h"
#include "IOGLWinBase.h"

#include "CameraTransform.h"

#include <vector>
#include <string>

	using namespace std;

	#define MRHLEN  10
	#define CLUDRAWBASE_MAXLIGHTCOUNT 7

	struct SPerspective
	{
		GLfloat fAngle;
		GLfloat fAspect;
		GLfloat fNear;
		GLfloat fFar;

		void Set(GLfloat _fAngle, GLfloat _fAspect, GLfloat _fNear, GLfloat _fFar)
		{ fAngle = _fAngle; fAspect = _fAspect; fNear = _fNear; fFar = _fFar; }
	};

	struct SOrtho
	{
		GLfloat fLeft;
		GLfloat fRight;
		GLfloat fBottom;
		GLfloat fTop;
		GLfloat fNear;
		GLfloat fFar;

		void Set(GLfloat _fLeft, GLfloat _fRight, GLfloat _fBottom, GLfloat _fTop, GLfloat _fNear, GLfloat _fFar)
		{
			fLeft   = _fLeft;
			fRight  = _fRight;
			fBottom = _fBottom;
			fTop    = _fTop;
			fNear   = _fNear;
			fFar    = _fFar;
		}
	};

	struct SMousePos
	{
		int iX, iY;
	};

	struct STransform
	{
		GLfloat pfRot[3];
		GLfloat pfTrans[3];

		int iFlags;

		void Reset()
		{
			for (int i = 0; i < 3; i++)
			{
				pfRot[i]   = 0.0f;
				pfTrans[i] = 0.0f;
			}

			iFlags = 0;
		}

		bool HasRotChanged(int i)
		{
			if ((i >= 0) && (i < 3)) { return (iFlags & (1 << i)) ? true : false; }
			else{ return false; }
		}

		bool HasTransChanged(int i)
		{
			if ((i >= 0) && (i < 3)) { return (iFlags & (0x10 << i)) ? true : false; }
			else{ return false; }
		}

		void SetRotChanged(int i)
		{ if ((i >= 0) && (i < 3)) { iFlags |= (1 << i); } }

		void SetTransChanged(int i)
		{ if ((i >= 0) && (i < 3)) { iFlags |= (0x10 << i); } }

		void SetAllRotChanged()
		{
			iFlags |= 0x0F;
		}

		void SetAllTransChanged()
		{
			iFlags |= 0xF0;
		}

		void ResetChanged()
		{
			iFlags = 0;
		}

		void ResetTransChanged()
		{ iFlags &= 0x0F; }

		void ResetRotChanged()
		{ iFlags &= 0xF0; }
	};


	class CLUDRAW_API CCLUDrawBase : public IOGLWinBase
	{
	public:
		using TVec3 = Clu::SVector3<float>;

	public:

		enum EMousePickType
		{
			MOUSEPICK_NONE = 0,
			MOUSEPICK_CLICK,
			MOUSEPICK_OVER
		};

		enum EMenuItemType
		{
			MI_END = 0,
			MI_TEXT,
			MI_CHECKBOX,
			MI_RADIO,
			MI_SUBMENU
		};

		enum EMenuKeyModifier
		{
			MKEY_NONE  = 0,
			MKEY_ALT   = 0x01,
			MKEY_CTRL  = 0x02,
			MKEY_SHIFT = 0x04,
		};

		enum EMenuKeySpecial
		{
			MKEY_ZERO = 0,
			MKEY_ESCAPE,
			MKEY_F1,
			MKEY_F2,
			MKEY_F3,
			MKEY_F4,
			MKEY_F5,
			MKEY_F6,
			MKEY_F7,
			MKEY_F8,
			MKEY_F9,
			MKEY_F10,
			MKEY_F11,
			MKEY_F12,
			MKEY_FEND,
			MKEY_MAX
		};

	
		struct SMenuItem
		{
			string sName;

			char cKey;
			EMenuKeyModifier eModifier;

			string sCallback;
			EMenuItemType eType;
			bool bInactive;
			bool bInvisible;
			bool bDivider;
			bool bSelected;

			SMenuItem()
			{
				Reset();
			}

			SMenuItem(string& sTheName,
					EMenuItemType eTheType = /*EMenuItemType::*/ MI_TEXT,
					char cTheKey = 0,
					EMenuKeyModifier eTheModifier =	/*EMenuKeyModifier::*/ MKEY_NONE,
					const string& sTheCallback = string(""),
					bool bIsInactive = false,
					bool bIsInvisible = false,
					bool bHasDivider = false,
					bool bIsSelected = false)
			{
				Set(sTheName, eTheType, cTheKey, eTheModifier, sTheCallback,
						bIsInactive, bIsInvisible, bHasDivider, bIsSelected);
			}

			void Reset()
			{
				sName      = "";
				sCallback  = "";
				cKey       = 0;
				eModifier  = /*EMenuKeyModifier::*/ MKEY_NONE;
				eType      = /*EMenuItemType::*/ MI_END;
				bInactive  = false;
				bInvisible = false;
				bDivider   = false;
				bSelected  = false;
			}

			void Set(string& sTheName,
					EMenuItemType eTheType = /*EMenuItemType::*/ MI_TEXT,
					char cTheKey = 0,
					EMenuKeyModifier eTheModifier =	/*EMenuKeyModifier::*/ MKEY_NONE,
					const string& sTheCallback = string(""),
					bool bIsInactive = false,
					bool bIsInvisible = false,
					bool bHasDivider = false,
					bool bIsSelected = false)
			{
				sName      = sTheName;
				eType      = eTheType;
				cKey       = cTheKey;
				eModifier  = eTheModifier;
				sCallback  = sTheCallback;
				bInactive  = bIsInactive;
				bInvisible = bIsInvisible;
				bDivider   = bHasDivider;
				bSelected  = bIsSelected;
			}
		};

	public:

		CCLUDrawBase();
		virtual ~CCLUDrawBase();

		void InitTime();
		double GetTime();
		__int64 GetTime64();
		double GetTimeStart() { return m_dTimeStart; }

		void SetPerspectiveAngle(GLfloat fAngle, bool bUpdateMatrices = true);
		void SetPerspective(GLfloat fAngle, GLfloat fNear, GLfloat fFar);
		void SetOrtho(GLfloat fLeft, GLfloat fRight, GLfloat fBottom, GLfloat fTop, GLfloat fNear, GLfloat fFar);

		void GetViewport(int& iX, int& iY, int& iW, int& iH);
		bool SetViewport(int iX, int iY, int iW, int iH);

		void AdjustRotAngle(float& fDeg);
		float DegToRad(float fDeg)
		{ return fDeg * m_fRadPerDeg; }

		void SetRotFac(float fRotFac) { m_fRotFac = fRotFac; }
		void SetTransFac(float fTransFac) { m_fTransFac = fTransFac; }

		float GetRotFac() { return m_fRotFac; }
		float GetTransFac() { return m_fTransFac; }

		bool SetRTMode(int iMode);
		int GetActRTMode() { return m_bCtrlDown ? 0 : m_iRTMode;  }

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Enables/Disables the RTView Mode (flight mode)
		/// </summary>
		///
		/// <param name="bVal"> true to value. </param>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void EnableRTViewMode(bool bVal) { if (bVal) { m_iStdRTMode = 10; } else{ m_iStdRTMode = 0; } }

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Set the Camera to look at the specified Bounding Sphere.
		/// </summary>
		///
		/// <param name="fBoundingCenterX"> The BoundingCenter x coordinate. </param>
		/// <param name="fBoundingCenterY"> The BoundingCenter y coordinate. </param>
		/// <param name="fBoundingCenterZ"> The BoundingCenter z coordinate. </param>
		/// <param name="fBoundingRadius">  The Bounding Sphere radius. </param>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void RTViewLookAt(float fBoundingCenterX, float fBoundingCenterY, float fBoundingCenterZ, float fBoundingRadius)
		{
			// Reset the model transformation
			m_vxTransform[0].Reset();

			// Reset all view parameter
			m_vRMain     = m_E3Base.vSC;
			m_vRViewMain = m_E3Base.vSC;
			m_vU         = m_E3Base.vSC;
			m_vAxis      = m_E3Base.vSC;
			m_vU         = 0;
			m_vAxis      = 0;

			TVec3 vTarget               = TVec3(fBoundingCenterX, fBoundingCenterY, fBoundingCenterZ);
			float fCameraFieldOfViewRad = DegToRad(m_xPers.fAngle);

			// Default mouse sensitivity is 0.02 for ~5 Unit bounding volume
			float fTranslateFactor = (0.02f * fBoundingRadius) / 5.0f;
			SetTransFac(fTranslateFactor);

			// Call LookAt function for camera transform
			m_xCameraTransform.LookAtBoundingSphere(vTarget, fBoundingRadius, fCameraFieldOfViewRad);
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Set the Camera to look at the specified target point.
		/// </summary>
		///
		/// <param name="fX"> The f x coordinate. </param>
		/// <param name="fY"> The f y coordinate. </param>
		/// <param name="fZ"> The f z coordinate. </param>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void RTViewLookAt(float fX, float fY, float fZ)
		{
			// Reset the model transformation
			m_vxTransform[0].Reset();

			// Reset all view parameter
			m_vRMain     = m_E3Base.vSC;
			m_vRViewMain = m_E3Base.vSC;
			m_vU         = m_E3Base.vSC;
			m_vAxis      = m_E3Base.vSC;
			m_vU         = 0;
			m_vAxis      = 0;

			m_xCameraTransform.LookAt(TVec3(fX, fY, fZ));
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Set camera view position.
		/// </summary>
		///
		/// <param name="fx"> The x coordinate. </param>
		/// <param name="fy"> The y coordinate. </param>
		/// <param name="fz"> The z coordinate. </param>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void RTViewSetPosition(float fx, float fy, float fz)
		{
			m_xCameraTransform.SetCameraPosition(TVec3(fx, fy, fz));
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Set camera rotation axis inversion.
		/// </summary>
		///
		/// <param name="bAxisX"> true to invert X-Axis coordinates. </param>
		/// <param name="bAxisY"> true to invert Y-Axis coordinates. </param>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void RTViewSetRotationAxisInversion(bool bAxisX, bool bAxisY)
		{
			m_xCameraTransform.InvertXAxis(bAxisX);
			m_xCameraTransform.InvertYAxis(bAxisY);
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Resets the 3D view to the initial state.
		/// </summary>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void Reset3dView();

		bool IsAnimated() { return m_bIsAnimated; }
		bool Is2dView() { return m_b2DViewEnabled; }

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Enables the 2D view.
		/// </summary>
		///
		/// <param name="bEnable"> True to enable, false to disable. </param>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void Enable2dView(bool bEnable);

		void EnableLighting(bool bLight = true) { m_bUseLighting = bLight; }
		void EnableLocalRot(bool bVal = true) { m_bUseLocalRot = bVal; }
		void EnablePickMouseOver(bool bVal = true) { m_bPickMouseOver = bVal; }
		void EnablePickMouseClick(bool bVal = true) { m_bPickMouseClick = bVal; }
		void EnableOGLMouse(bool bVal = true) { m_bOGLMouse = bVal; }
		void EnableColorStereo(bool bVal = true) { m_bDrawColorStereo = bVal; }
		void EnableTransparency(bool bVal = true) { m_bDrawTransparency = bVal; }

		void EnableSendControlKeyEvents(bool bVal = true) { m_bSendControlKeyEvents = bVal; };
		void EnableSendFunctionKeyEvents(bool bVal = true) { m_bSendFunctionKeyEvents = bVal; };

		void SetColorStereoMask(int piMaskLeft[4], int piMaskRight[4]);
		void SetColorStereoSep(float fVal) { m_fColorStereoSep = fVal; }
		void SetColorStereoDegAngle(float fVal) { m_fColorStereoDegAngle = fVal; }

		void ResetTransformVars() { for (int i = 1; i < 10; i++) { m_vxTransform[i].Reset(); } }
		void SetTransformVarsChanged() { for (int i = 1; i < 10; i++) { m_vxTransform[i].iFlags = 0xFF; } }

		COGLLight& GetStdLight() { return m_StdLight; }

		virtual bool ResetStdLight();

		// Tool Functions do nothing in standard implementation
		virtual bool GetToolValue(const char* pcName, double& rVal);
		virtual bool GetToolValue(const char* pcName, string& rVal);
		virtual bool AddToolSlider(const char* pcName, double dMin, double dMax, double dStep, double dVal, bool bFixed = false, const char* pcDesc = 0);
		virtual bool AddToolCounter(const char* pcName, double dMin, double dMax, double dStep, double dLStep, double dVal, bool bFixed = false, const char* pcDesc = 0);
		virtual bool AddToolInput(const char* pcName, double dMin, double dMax, double dVal, bool bFixed = false, const char* pcDesc = 0);
		virtual bool AddToolInputText(const char* pcName,
				bool bImmediateCallback,
				const char* pcInitText,
				bool bFixed = false,
				const char* pcDesc = 0,
				bool bDoUpdate = true);
		virtual bool AddToolCheckBox(const char* pcName, bool bSet, bool bFixed = false, const char* pcDesc = 0);
		virtual bool AddToolButton(const char* pcName, bool bFixed = false, const char* pcDesc = 0);
		virtual bool AddToolImageButton(const char* pcName, COGLImage* pImgAct, COGLImage* pImgDeact = 0, bool bFixed = false, const char* pcDesc = 0);
		virtual bool AddToolChoice(const char* pcName, vector<string>& vecData, const char* pcCoice, bool bFixed = false, const char* pcDesc = 0, bool bDoUpdate = true);

		virtual bool AddToolbarImageButton(const char* pcName, COGLImage* pImgAct, COGLImage* pImgDeact = 0, bool bFixed = false, const char* pcDesc = 0);

		virtual bool AddToolbarCounter(const char* pcName,
				const char* pcUnit,
				double dMin,
				double dMax,
				double dStep,
				double dLStep,
				double dVal,
				bool bActive = true,
				bool bFixed = false,
				const char* pcDesc = 0);

		virtual bool AddToolbarLabel(const char* pcName, bool bFixed = false, const char* pcDesc = 0);

		// Info Window Functions
		virtual bool SetInfoText(const char* pcText);
		virtual bool SetInfoWidth(double dWidth);

		virtual bool SetOGLSize(double dWidth, double dHeight);
		virtual bool GetOGLSize(double& dWidth, double& dHeight);

		// Flush everything that has currently been drawn to the screen
		virtual bool FlushDraw();

		virtual bool EnableMenu(bool bVal);
		virtual bool EnableStdCtrl(bool bVal);
		virtual bool SetMenu(vector<SMenuItem>& vecMenuItem);

		virtual bool SetMouseMode(int iMode);
		virtual bool GetMouseMode(int& iMode);

		virtual bool SetAnimationTimeStep(int iTimeStep);
		virtual bool SetVisualizationTimeStep(int iTimeStep);

		virtual bool Init(bool bFirst);

		// the following virtual functions are defined in the window system dependent derived classes
		virtual bool BeginDraw();
		virtual void Display();
		virtual bool EndDraw();
		// Transformation of the world called in BeginDraw
		virtual void WorldTransform();

		virtual COGLColor GetBGColor();
		virtual void SetBGColor(COGLColor& rBGCol);
		virtual void SetBoxColor(COGLColor& rCol, COGLColor& rXCol, COGLColor& rYCol, COGLColor& rZCol);

		virtual unsigned char CLU_LEFT_BUTTON() { return 99; };
		virtual unsigned char CLU_RIGHT_BUTTON() { return 99; };
		virtual unsigned char CLU_DOWN() { return 99; };
		virtual unsigned char CLU_UP() { return 99; };

		virtual bool SaveScreen2PGM(const char* pcFilename);
		virtual bool SaveScreen2BMP(const char* pcFilename);

		virtual bool LockVis(int iWait = 5000) { return true; }
		virtual void UnlockVis() {         }

	protected:

		virtual void Reshape(int iWidth, int iHeight);
		virtual void Mouse(int iButton, int iState, int iX, int iY);
		virtual void Key(unsigned char cKey, int iX, int iY);
		virtual void ActiveMouseMove(int iX, int iY);
		virtual void PassiveMouseMove(int iX, int iY);

		virtual void Mouse3D_KeyDown(int iKeyCode);
		virtual void Mouse3D_KeyUp(int iKeyCode);
		virtual void Mouse3D_Move(double dTX, double dTY, double dTZ, double dLength, double dRX, double dRY, double dRZ, double dAngle);

		virtual void Menu(int iIdx);
		virtual void IdleFunc() { }

		virtual void GetModifiers() { }
		virtual bool StartAnimation(unsigned int uiTime) { return true; }
		virtual bool StopAnimation() { return true; }

		virtual bool TimedFunc();
		virtual bool AtAnimStart() { return true; }
		virtual bool AtAnimEnd() { return true; }

		// Called to process picking
		// if bMouseClick == true, then called because of mouse click
		// otherwise called because of mouse over picking
		virtual bool Picking(EMousePickType ePickType, int iX, int iY, int iPickW, int iPickH);

		// Called to draw for picking
		virtual bool PickDraw() { return true; }
		// Process the picking data
		virtual bool PickProcessHits(EMousePickType ePickType, uint uPickColor, float fDepth) { return true; }

		// returns milliseconds since last call of idlefunc
		float timeElapsed() { return m_fTimeStep; }
		void UpdateMatrices();

		void DrawBase();
		void DrawBox(float fSize = 4.0f);

		MultiV<float> GetMouseRotHistSum()
		{
			MultiV<float> vSum = m_pvMouseRotHist[0];
			for (int i = 1; i < MRHLEN; i++)
			{
				vSum += m_pvMouseRotHist[i];
			}

			return vSum;
		}

		void ResetMouseRotHist()
		{
			for (int i = 0; i < MRHLEN; i++)
			{
				m_pvMouseRotHist[i] = 0;
			}

			m_iMRHPos     = 0;
			m_bMRHChanged = true;
		}

		bool HasMouseRotHistChanged() { bool bVal = m_bMRHChanged; m_bMRHChanged = false; return bVal; }

		// ArcCos with clamped operand
		double _acos(double dVal) { return acos((dVal < -1.0 ? -1 : (dVal > 1.0 ? 1.0 : dVal))); }

	protected:

		__int64 m_liTimeFreq;
		double m_dTimeStart, m_dTimePrev;

		int m_iPosX, m_iPosY, m_iSizeX, m_iSizeY;

		bool m_bMouseLDown;
		bool m_bMouseRDown;
		volatile bool m_bShiftDown;
		volatile bool m_bCtrlDown;
		volatile bool m_bAltDown;
		volatile bool m_bNumLock;
		volatile bool m_bCapsLock;
		volatile bool m_bScrollLock;

		bool m_b2DViewEnabled;
		bool m_bPickMouseOver;	// If true then picking through mouse over is activated
		bool m_bPickMouseClick;	// If true then picking through mouse click is activated
		bool m_bOGLMouse;	// Show OpenGL Mouse
		bool m_bDrawColorStereo;// If true scene is drawn in color stereo
		bool m_bDrawTransparency;	// If true double pass is used for drawing transparent objects

		bool m_bSendControlKeyEvents;	// Send control key events to script
		bool m_bSendFunctionKeyEvents;	// Send function key events to script

		bool m_pbColorMaskLeft[4];	// Color mask for left stereo image
		bool m_pbColorMaskRight[4];	// Color mask for right stereo image
		float m_fColorStereoSep;// Separation of cameras in color stereo
		float m_fColorStereoDegAngle;	// Angle in degree of color stereo cameras to central axis

		float m_fTimeStep;	// Time past since last animation call
		float m_fAnimTimeStep;	// Time past since last animation call
		float m_fTotalTime;	// Total time past since start of animation
		unsigned int m_uAnimateTimeStep;
		bool m_bIsAnimated;
		int m_iAnimRotType;

		// The number of slices the total depth range [0,1]
		// is split into to draw viewports at different
		// depth levels.
		int m_iDepthLevelCount;
		// The total width of a depth level including the gap to the next one
		double m_dDepthLevelWidth;
		// The gap between consecutive depth levels.
		double m_dDepthLevelGap;
		// The near value of the central depth level
		double m_dDepthLevelOrig;

		SOrtho m_xOrtho;
		SPerspective m_xPers;

		bool m_bUseLighting;
		bool m_bUseLocalRot;

		SMousePos m_sMousePos;

		int m_iRTMode, m_iStdRTMode;
		float m_fFrameAngle;

		unsigned m_uMaxTransforms;	// the maximum amount of transforms available
		std::vector<STransform> m_vxTransform;

		/// <summary> The camera transform. </summary>
		CCameraTransform m_xCameraTransform;

		// Background Color
		COGLColor m_BGCol;

		// Color for Box
		COGLColor m_BoxCol, m_BoxXCol, m_BoxYCol, m_BoxZCol;

		// Global Colors
		COGLColor Red, Green, Blue, Cyan, Magenta, Yellow;
		COGLColor DRed, DGreen, DBlue, DCyan, DMagenta, DYellow;
		COGLColor MRed, MGreen, MBlue, MCyan, MMagenta, MYellow;
		COGLColor Orange, DOrange, MOrange, White, Black, DGray, MGray;

		// if bool value is true, then the position of the light is relative to the view frame.
		// otherwise relative to the model-view frame.
		COGLLight m_StdLight;

		COGLMaterial m_StdMaterial;

		float m_fPi, m_fRadPerDeg;

		float m_fRotFac, m_fTransFac;
		float m_fOrthoFac, m_fOrthoZoomFac;

		int m_iMainMenu;

		// 3D Space for rotor evaluation
		E3GA<float> m_E3Base;

		// Main Rotor. Gives rotation of space.
		MultiV<float> m_vRMain, m_vRViewMain;
		MultiV<float> m_vU, m_vAxis;
		MultiV<float> m_pvMouseRotHist[MRHLEN];
		int m_iMRHPos;
		bool m_bMRHChanged;

		int m_iSaveScreenNo;
		int m_iAnimPause;
		int m_iVisTimeStep;

		int m_iGLVMinor, m_iGLVMajor;
		int m_iGLSLVMinor, m_iGLSLVMajor;
		bool m_bGLHas_MultiTexture;
		bool m_bGLHas_Shader;

		bool m_bGLCan_Multisample;

		// Scene Apply Data
		COGLBaseElement::SApplyData m_SceneApplyData;
	};

#endif	// !defined(AFX_CLUDRAWBASE_H__6EB9C283_EE5B_11D5_BA34_00E07D8AFD6A__INCLUDED_)
