////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLDrawBase.h
//
// summary:   Declares the ogl draw base class
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

// OGLDrawBase.h: Schnittstelle f�r die Klasse COGLDrawBase.
//
// Base Class for drawing to scenes
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLDRAWBASE_H__3D0F9FE0_C4C7_4C2C_9881_815CE941C428__INCLUDED_)
	#define AFX_OGLDRAWBASE_H__3D0F9FE0_C4C7_4C2C_9881_815CE941C428__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif	// _MSC_VER > 1000

#include "OGLColor.h"
#include "OGLMaterial.h"
#include "OGLVertex.h"
#include "OGLVertexList.h"
#include "OGLDisplayList.h"
#include "OGLBaseElementList.h"

//#include "CluTec.Viz.Base.GA/e3ga.h"
//#include "CluTec.Viz.Base.GA/pga.h"
//#include "CluTec.Viz.Base.GA/confga.h"
//#include "CluTec.Viz.Base.GA/el2ga.h"

#include "OGLBERepository.h"

	#define DRAW_POINT_AS_DOT               (ID_ALL << 16) + 0x0100
	#define DRAW_POINT_AS_SPHERE    (ID_ALL << 16) + 0x0101
	#define DRAW_POINT_AS_STD               DRAW_POINT_AS_DOT

	#define DRAW_LINE_AS_LINE               (ID_ALL << 16) + 0x0110
	#define DRAW_LINE_AS_CYLINDER   (ID_ALL << 16) + 0x0111
	#define DRAW_LINE_AS_STD                DRAW_LINE_AS_LINE

	#define MAT_STD         0
	#define MAT_PLANE       1
	#define MAT_CUBE        2
	#define MAT_SPHERE      3
	#define MAT_ROTOR       4
	#define MAT_CIRCLE      5
	#define MAT_LINE        6
	#define MAT_POINT       7
	#define MAT_VECTOR      8
	#define MAT_RHOMBUS     9
	#define MAT_COUNT       10

	class CCLUCodeBase;

	class CLUDRAW_API COGLDrawBase
	{
	public:

		enum TDrawPointType
		{
			PntStd = 0, PntDot = 0, PntSphere
		};

		enum TDrawLineType
		{
			LineStd = 0, LineAsLine = 0, LineAsCylinder
		};

		struct STriplet
		{
		public:

			unsigned& operator[](int iPos) { return m_pData[iPos]; }

			unsigned* Data() { return m_pData; }

		protected:

			unsigned m_pData[3];
		};

		typedef unsigned TTriplet[3];

	public:

		COGLDrawBase();
		virtual ~COGLDrawBase();

		void SetSceneRepository(COGLBERepository* pSceneRep)
		{ m_pSceneRep = pSceneRep; }

		COGLBERepository* GetSceneRepository()
		{ return m_pSceneRep; }

		// Set Scene into which is drawn.
		// rSceneRef has to point to a COGLBaseElementList
		bool SetScene(COGLBEReference& rSceneRef);

		// Get scene to which is currently drawn
		COGLBEReference& GetScene() { return m_SceneRef; }

		bool SetMode(int iMode);
		bool ResetModes();

		bool SetColor(COGLColor& rCol);
		COGLColor GetColor() { return m_ActiveColor; }

		bool SetLineWidth(float fWidth);
		bool SetPointSize(float fSize);
		bool SetPointSize(float fSize, float fSizeMin, float fSizeMax, float fFadeSize, float fConst, float fLin, float fQuad);
		bool EnablePointSprites(bool bValue);

		bool SetSphereDetailLevel(int iLevel)
		{ if ((iLevel < 0) || (iLevel > 9)) { return false; } m_iSphereDetailLevel = iLevel; return true; }

		bool SetCylinderDetailLevel(int iLevel)
		{ if ((iLevel < 3) || (iLevel > 360)) { return false; } m_iCylinderDetailLevel = iLevel; return true; }

		bool EnableMultisample(bool bVal = true);

		bool Init();
		void EnableLineStipple(bool bSet = true);

		//bool SetDrawPos( const COGLVertex& xA );

		bool DrawBaseElement(COGLBEReference& rElement);
		bool DrawVertexList(COGLVertexList& VexList);

		bool DrawVector(const COGLVertex& xA, const COGLVertex& xB, int iStipple = 0, int iStippleFactor = 0);

		bool DrawLine(const COGLVertex& xP, const COGLVertex& xD, bool bEndPoints = false, int iStipple = 0, int iStippleFactor = 1, MemObj<COGLBEReference>* pmScene = 0);

		bool DrawCube(float fSize, const float* pX = 0, const float* pY = 0, const float* pZ = 0);

		bool DrawRhombus(float fSize);

		bool DrawSphere(const COGLVertex& xC, float fRadius, bool bSolid = false, bool bDirected = false);

		bool DrawEllipsoid(const COGLVertex& xC, const COGLVertex& xX, const COGLVertex& xY, const COGLVertex& xZ, bool bSolid = false);

		bool DrawPlane(const COGLVertex& xP, const COGLVertex& xA, const COGLVertex& xB, bool bCornerVecs = false, bool bDirected = true);

		bool DrawPolygon(Mem<COGLVertex>& mVex, Mem<COGLVertex>& mNorm, bool bDirected);

		bool DrawQuad(const COGLVertex& xA, const COGLVertex& xB, const COGLVertex& xC, const COGLVertex& xD);

		//bool DrawBox(const COGLVertex& xP, const COGLVertex& xA,
		//	const COGLVertex& xB, float fWidth,
		//	bool bCornerVecs, bool bDirected,
		//	COGLVertexList *_pVexList = 0,
		//	bool bDoDraw = true);

		bool DrawBox(const COGLVertex& xP, const COGLVertex& xA, const COGLVertex& xB, const COGLVertex& xC, const MemObj<COGLColor>& rColList);

		bool DrawTriangle(const COGLVertex& xA, const COGLVertex& xB, const COGLVertex& xC, bool bDirected = true);

		bool DrawCone(const COGLVertex& xP, const COGLVertex& xD, float fR, MemObj<COGLColor>& mColor, int iStepCnt = 16, bool bClosed = false, MemObj<COGLBEReference>* pmScene = 0);

		bool DrawCircle(const COGLVertex& xC, const COGLVertex& xN, float fR, float fAngleStep = 5.0f);

		bool DrawCylinder(const COGLVertex& xC, const COGLVertex& xN, float fR, bool bClosed = false, float fAngleStep = 5.0f);

		bool DrawCircleSurface(const Mem<COGLVertex>& mC,
				const Mem<COGLVertex>& mN,
				const Mem<float>& mR,
				const MemObj<COGLColor>& mCol,
				COGLBEReference& refScene,
				float fAngleStep = 5.0f,
				bool bDoDraw = true);

		bool DrawEllipseSurface(const Mem<COGLVertex>& mC,
				const Mem<COGLVertex>& mX,
				const Mem<COGLVertex>& mY,
				const MemObj<COGLColor>& mCol,
				COGLBEReference& refScene,
				float fAngleStep,
				bool bDoDraw = true);

		bool DrawLineSurface(const Mem<COGLVertex>& mC,
				const Mem<COGLVertex>& mD,
				const Mem<float>& mL,
				const MemObj<COGLColor>& mCol,
				COGLBEReference& refScene,
				int iSteps = 20,
				bool bDoDraw = true);

		bool DrawEllipse(const COGLVertex& xC, const COGLVertex& xA, const COGLVertex& xB, float fAngleStep = 5.0f);

		bool DrawHyperbola(const COGLVertex& xC, const COGLVertex& xA, const COGLVertex& xB, float fAngleStep = 5.0f);

		bool GenVexPointSurface(int iRowCount,
				int iColCount,
				const Mem<COGLVertex>& mP,
				const Mem<COGLVertex>& mN,
				const Mem<COGLVertex>& mT,
				const MemObj<COGLColor>& mCol,
				float fNormScale,
				COGLVertexList* pVexListSurf,
				COGLVertexList* pVexListNorm);

		bool DrawPointSurface(int iRowCount,
				int iColCount,
				const Mem<COGLVertex>& mP,
				const Mem<COGLVertex>& mN,
				const Mem<COGLVertex>& mT,
				const MemObj<COGLColor>& mCol,
				float fNormScale,
				COGLBEReference& refSurface,
				COGLBEReference& refNormals,
				bool bDoDraw = true);

		bool DrawPointGrid(int iRowCount,
				int iColCount,
				const Mem<COGLVertex>& mP,
				const Mem<COGLVertex>& mN,
				const MemObj<COGLColor>& mCol,
				COGLBEReference& refScene,
				bool bNegateNormals = false,
				bool bDoDraw = true);

		bool DrawPointList(const Mem<COGLVertex>& mP, const Mem<COGLVertex>& mN, const MemObj<COGLColor>& mCol, COGLBEReference& refScene, bool bNegateNormals = false, bool bDoDraw = true);

		bool DrawDisk(const COGLVertex& xC, const COGLVertex& xA, const COGLVertex& xB, float fR, float fAngleStep = 5.0f, bool bDirected = true);

		bool DrawRotor(const COGLVertex& xP, const COGLVertex& xR, float fAngle, float fDegAngleStep = 5.0f);

		bool DrawTwist(const COGLVertex& xP, const COGLVertex& xT, const COGLVertex& xR, float fAngle, float fDegAngleStep = 5.0f);

		// Draws an arc.
		// xP is offset vector. Arc is drawn from (xA-xP) to (xB-xP) with radius 'fRadius'.
		// if bFilled = true then a pie-slice is drawn.
		// if bShort = true, then the arc along the shortest path from xA to xB is drawn.
		// otherwise the arc along the longest path.
		bool DrawArc(const COGLVertex& xP, const COGLVertex& xA, const COGLVertex& xB, float fRadius, bool bShort = true, bool bFilled = false, float fDegAngleStep = 5.0f);

		// Draw a single point
		bool DrawPoint(const COGLVertex& xP);

		// Draws an Icosahedron at pos. xP, with radius fRadius and 2^iPower subdivisions.
		// This approximates a sphere for higher powers
		bool DrawIcosahedron(const COGLVertex& xP, float fRadius, int iPower, bool bSolid = true);

		// Draw Elements in an evenly spaced grid
		// vexObject: Vertex List of Object to draw
		// mCount: Number of instances along respective vector in mDir
		// mDir: Directions along which to draw vexObject
		//bool DrawObjectGrid( COGLBaseElementList &mObject,
		//	const Mem<int>& mCount,
		//	const Mem<COGLVertex>& mDir);

		bool TranslateFrame(float fX, float fY, float fZ);

		void SetSensitivity(double dSens) { m_fSensitivity = (float) dSens; m_dSensitivity = dSens; }
		float GetFloatSens() { return m_fSensitivity; }
		double GetDoubleSens() { return m_dSensitivity; }

		float Round(float fVal)
		{ return ::Round(fVal, m_fSensitivity); }

		double Round(double dVal)
		{ return ::Round(dVal, m_dSensitivity); }

		void EnableLighting(bool bLight = true) { m_bUseLighting = bLight; }
		void EnableTranslucency(bool bVal = true) { m_bUseTranslucency = bVal; }
		//void EnableTextures(bool bVal = true) { m_bUseTextures = bVal; }

		void SetTranslucencySeed(int iSeed) { m_Random.seed3(iSeed); }

		void SetTextureRepeatFactor(float fVal) { m_fTexRepFac = fVal; }
		void EnableAbsTexCoords(bool bVal = true) { m_bUseAbsTexCoords = bVal; }

		void SetArrowShape(float fSize, float fAngle, float fLineRadius)
		{
			m_fArrowLength     = fSize;
			m_fArrowAngle      = fAngle;
			m_fArrowLineRadius = fLineRadius;
		}

		bool GenVexIcosahedron(COGLVertexList& rVexList, float fRadius, int iPower);
		bool GenCubeSphere(MemObj<COGLVertexList>& mVexList, int iRowCnt, int iColCnt);

	protected:

		//bool GenerateSphereVL(double dAngleStepTheta, double dAngleStepPhi);
		bool MakeRandPolyStipple(double dLevel);

		bool InitSphereVexLists();

	protected:

		E3GA<float> m_E3Base;
		PGA<float> m_P3Base;
		ConfGA<float> m_N3Base;
		El2GA<float> m_El2Base;

		E3GA<double> m_dE3Base;
		PGA<double> m_dP3Base;
		ConfGA<double> m_dN3Base;
		El2GA<double> m_dEl2Base;

		// Scene Repository, needed to generate BaseElement References
		COGLBERepository* m_pSceneRep;

		COGLBEReference m_SceneRef;
		COGLBaseElementList* m_pScene;	// Current scene into which is drawn

		//COGLBEReference m_refCurOverlay; // Current overlay

		COGLColor m_ActiveColor;
		COGLVertexList m_VexList;
		COGLVertexList m_NormVexList;

		//COGLVertex m_vexDrawPos;

		MemObj<COGLVertexList> m_Sphere[10];
		MemObj<COGLVertexList> m_CubeSphere[10];
		int m_iSphereDetailLevel;
		int m_iCylinderDetailLevel;

		MemObj<COGLMaterial> m_mMaterial;

		float m_fPi;
		float m_fBackFaceColFac;
		float m_fArrowColFac;

		float m_fLineWidth;
		float m_fLineAsCylinderFac;

		float m_fPointSize;
		float m_fPointAsSphereFac;

		float m_fTexRepFac;	// Texture Repeat Factor

		// Arrow Shape: Length of arrow and opening angle
		float m_fArrowLength, m_fArrowAngle;	// Arrow Shape
		float m_fArrowLineRadius;

		float m_fSensitivity;	// Used as TinyToZero parameter
		double m_dSensitivity;

		Rand m_Random;
		unsigned char m_pcPolyStipple[1024];	// Polygon Stipple pattern

		bool m_bUseLighting;
		bool m_bUseTranslucency;
		bool m_bShowImaginaryObjects;
		bool m_bUseAbsTexCoords;
		//bool m_bUseTextures;

		TDrawPointType m_eDrawPointType;
		TDrawLineType m_eDrawLineType;
	};

#endif	// !defined(AFX_OGLMVFILTERBASE_H__3D0F9FE0_C4C7_4C2C_9881_815CE941C428__INCLUDED_)
