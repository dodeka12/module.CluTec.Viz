////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLDrawBase.cpp
//
// summary:   Implements the ogl draw base class
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

//// IMPORTANT IMPORTANT IMPORTANT
/*
        Never write this:

                m_VexList.AddVex(m_VexList[0]);

        since internally the memory of m_VexList is enlarged, such that m_VexList[0]
        may not be a valid reference anymore!!!
*/

// OGLFilterBase.cpp: Implementierung der Klasse COGLFilterBase.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "OGLDrawBase.h"
#include "OGLVexListE3.h"
#include "OGLAnimColor.h"
#include "OGLTranslation.h"
#include "OGLLineWidth.h"
#include "OGLPeek.h"
#include "OGLPointParameter.h"
#include "OGLPointSprites.h"
#include "OGLMultisample.h"

//#include "OGLObjWireSphere.h"
#include "CluTec.Viz.Fltk\Fl_math.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLDrawBase::COGLDrawBase()
{
	m_pSceneRep = 0;
	m_pScene    = 0;

	m_fBackFaceColFac = 0.5f;
	m_fArrowColFac    = 0.8f;

	m_fLineWidth         = 1.5f;
	m_fLineAsCylinderFac = 0.005f;

	m_fPointSize        = 6.0f;
	m_fPointAsSphereFac = 0.003f;

	m_fPi = 2.0f * (float) asin(1.0);

	m_fArrowLength     = 0.15f;
	m_fArrowAngle      = 15.0f / 180.0f * m_fPi;
	m_fArrowLineRadius = 0.01f;

	m_fSensitivity = 1e-7f;

	m_fTexRepFac       = 0;	// Textures are not repeated
	m_bUseAbsTexCoords = false;

	m_bUseLighting     = false;
	m_bUseTranslucency = false;

	m_iSphereDetailLevel = 1;

	//m_vexDrawPos.Set( 0.0f, 0.0f, 0.0f );

	m_mMaterial.Set(MAT_COUNT);

	m_mMaterial[MAT_STD].Face(GL_FRONT);
	m_mMaterial[MAT_STD].Specular(0.8f, 0.8f, 0.8f, 0.0f);
	m_mMaterial[MAT_STD].Shininess(70.0f);

	for (int i = 1; i < MAT_COUNT; i++)
	{
		m_mMaterial[i] = m_mMaterial[MAT_STD];
	}

	m_mMaterial[MAT_SPHERE].Specular(0.6f, 0.6f, 0.6f, 0.0f);
	m_mMaterial[MAT_SPHERE].Ambient(0.0f, 0.0f, 0.0f, 0.0f);
	m_mMaterial[MAT_SPHERE].Diffuse(1.0f, 1.0f, 1.0f, 1.0f);

	ResetModes();
}

COGLDrawBase::~COGLDrawBase()
{
}

//////////////////////////////////////////////////////////////////////
// Set Scene into which is drawn.
// rSceneRef has to point to a COGLBaseElementList

bool COGLDrawBase::SetScene(COGLBEReference& rSceneRef)
{
	COGLBaseElementList* pElList
		= dynamic_cast<COGLBaseElementList*>((COGLBaseElement*) rSceneRef);

	if (!pElList)
	{
		return false;
	}

	m_SceneRef = rSceneRef;
	m_pScene   = pElList;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Init OpenGL with Filter Data

bool COGLDrawBase::Init()
{
	InitSphereVexLists();

	return true;
}

//////////////////////////////////////////////////////////////////////
// Reset Modes

bool COGLDrawBase::ResetModes()
{
	m_eDrawPointType = PntDot;
	m_eDrawLineType  = LineAsLine;

	m_bShowImaginaryObjects = true;

	m_iSphereDetailLevel   = 3;
	m_iCylinderDetailLevel = 18;
	m_fPointSize           = 6.0f;
	m_fLineWidth           = 1.5f;

	//m_vexDrawPos.Set( 0.0f, 0.0f, 0.0f );

	return true;
}

//////////////////////////////////////////////////////////////////////
// Make Random Polygon Stipple Patterns

bool COGLDrawBase::MakeRandPolyStipple(double dLevel)
{
	int i;
	unsigned char ucVal;

	for (i = 0; i < 1024; i++)
	{
		ucVal  = (m_Random.ran3() < dLevel ? 0x01 : 0);
		ucVal += (m_Random.ran3() < dLevel ? 0x02 : 0);
		ucVal += (m_Random.ran3() < dLevel ? 0x04 : 0);
		ucVal += (m_Random.ran3() < dLevel ? 0x08 : 0);

		ucVal += (m_Random.ran3() < dLevel ? 0x10 : 0);
		ucVal += (m_Random.ran3() < dLevel ? 0x20 : 0);
		ucVal += (m_Random.ran3() < dLevel ? 0x40 : 0);
		ucVal += (m_Random.ran3() < dLevel ? 0x80 : 0);

		m_pcPolyStipple[i] = ucVal;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Color

bool COGLDrawBase::SetColor(COGLColor& rCol)
{
	m_ActiveColor = rCol;

	if (!m_SceneRef.IsValid())
	{
		return false;
	}

	COGLColor* pCol        = new COGLColor(rCol);
	COGLBEReference ColRef = m_pSceneRep->New(pCol);

	m_pScene->Add(ColRef);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Line Width

bool COGLDrawBase::SetLineWidth(float fWidth)
{
	if (!m_SceneRef.IsValid())
	{
		return false;
	}

	COGLLineWidth* pLineWidth = new COGLLineWidth(fWidth);
	COGLBEReference Ref       = m_pSceneRep->New(pLineWidth);

	m_pScene->Add(Ref);

	m_fLineWidth = fWidth;

	return true;
}

//////////////////////////////////////////////////////////////////////
bool COGLDrawBase::SetPointSize(float fSize)
{
	return SetPointSize(fSize, 1.0f, 10.0f, 1.0f, 1.0f, 0.0f, 0.0f);
}

//////////////////////////////////////////////////////////////////////
/// Set Point Size and parameters

bool COGLDrawBase::SetPointSize(float fSize, float fSizeMin, float fSizeMax, float fFadeSize, float fConst, float fLin, float fQuad)
{
	if (!m_SceneRef.IsValid())
	{
		return false;
	}

	COGLPointParameter* pPnt = new COGLPointParameter();

	pPnt->SetSize(fSize);
	pPnt->SetSizeRange(fSizeMin, fSizeMax);
	pPnt->SetFadeSize(fFadeSize);
	pPnt->SetDistanceAttenuation(fConst, fLin, fQuad);

	m_pScene->Add(m_pSceneRep->New(pPnt));

	m_fPointSize = fSize;

	return true;
}

//////////////////////////////////////////////////////////////////////
// Enable Point Sprites
bool COGLDrawBase::EnablePointSprites(bool bValue)
{
	if (!m_SceneRef.IsValid())
	{
		return false;
	}

	COGLPointSprites* pPnt = new COGLPointSprites();

	pPnt->Enable(bValue);

	m_pScene->Add(m_pSceneRep->New(pPnt));

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Enable/Disable Multisampling

bool COGLDrawBase::EnableMultisample(bool bVal)
{
	if (!m_SceneRef.IsValid())
	{
		return false;
	}

	COGLMultisample* pMultisample = new COGLMultisample(bVal);
	if (pMultisample == 0)
	{
		return false;
	}

	m_pScene->Add(m_pSceneRep->New(pMultisample));

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Mode

bool COGLDrawBase::SetMode(int iMode)
{
	switch (iMode)
	{
	case DRAW_POINT_AS_DOT:
		m_eDrawPointType = PntDot;
		break;

	case DRAW_POINT_AS_SPHERE:
		m_eDrawPointType = PntSphere;
		break;

	case DRAW_LINE_AS_LINE:
		m_eDrawLineType = LineAsLine;
		break;

	case DRAW_LINE_AS_CYLINDER:
		m_eDrawLineType = LineAsCylinder;
		break;

	default:
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Enable Line Stipple

void COGLDrawBase::EnableLineStipple(bool bSet)
{
	//if (bSet)
	//{
	//	glEnable(GL_LINE_STIPPLE);
	//	glLineWidth(0.5);
	//	glLineStipple(1, 0x0F0F);
	//}
	//else
	//{
	//	glLineWidth(m_fLineWidth);
	//	glDisable(GL_LINE_STIPPLE);
	//}
}

//////////////////////////////////////////////////////////////////////
/// Draw Base Element

bool COGLDrawBase::DrawBaseElement(COGLBEReference& rElement)
{
	if (!m_pScene || !rElement.IsValid() ||
	    (m_pScene == (COGLBaseElement*) rElement))
	{
		return false;
	}

	m_pScene->Add(rElement);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Translate Frame

bool COGLDrawBase::TranslateFrame(float fX, float fY, float fZ)
{
	m_pScene->Add(m_pSceneRep->New(new COGLTranslation(fX, fY, fZ)));

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Vertex List

bool COGLDrawBase::DrawVertexList(COGLVertexList& VexList)
{
	m_pScene->Add(m_pSceneRep->New(VexList.Copy()));

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Point

bool COGLDrawBase::DrawPoint(const COGLVertex& xP)
{
	if (m_eDrawPointType == PntSphere)
	{
		DrawSphere(xP, m_fPointSize * m_fPointAsSphereFac, true);
	}
	else
	{
		COGLVertexList* pVexList = new COGLVertexList;
		if (!pVexList)
		{
			return false;
		}

		COGLVertexList& rVexList = *pVexList;

		rVexList.SetMode(GL_POINTS);
		rVexList.AddVex(xP);

		m_pScene->Add(m_pSceneRep->New(pVexList));
	}

	//m_vexDrawPos = xP;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Cube

bool COGLDrawBase::DrawCube(float fSize, const float* pX, const float* pY, const float* pZ)
{
	COGLVertex X, Y, Z, A;
	float fHSize          = 0.5f * fSize;
	unsigned puIdxQuads[] = {
		0, 1, 2, 3, 4, 5, 6, 7,
		0, 7, 6, 1, 1, 6, 5, 2, 2, 5, 4, 3, 3, 4, 7, 0
	};
	int i;

	if (!m_SceneRef.IsValid() || (m_pSceneRep == 0))
	{
		return false;
	}

	COGLVertexList* pVexList = new COGLVertexList;

	if (!pVexList)
	{
		return false;
	}

	COGLVertexList& rVexList = *pVexList;

	//rVexList.Reset();
	rVexList.SetMode(GL_QUADS);

	// Reserve memory for 24 vertices
	rVexList.Reserve(24);

	if (pX && pY && pZ)
	{
		float fSign = 1.0f;
		MultiV<float> vX, vY, vZ, vD;

		vX = m_E3Base.Vec(pX);
		vY = m_E3Base.Vec(pY);
		vZ = m_E3Base.Vec(pZ);

		vD  = vX;
		vD ^= vY;
		vD *= vZ;
		if (-vD[0] < 0)
		{
			fSign = -1.0f;
		}

		X = pX;
		Y = pY;
		Z = pZ;

		X = X * fHSize;
		Y = Y * fHSize;
		Z = Z * fSign * fHSize;
	}
	else
	{
		X.Set(fHSize, 0, 0);
		Y.Set(0, fHSize, 0);
		Z.Set(0, 0, fHSize);
		/*
		                rVexList.AddVex(-fHSize, - fHSize, - fHSize);
		                rVexList.AddVex( fHSize, - fHSize, - fHSize);
		                rVexList.AddVex( fHSize, - fHSize,   fHSize);
		                rVexList.AddVex(-fHSize, - fHSize,   fHSize);

		                rVexList.AddVex(-fHSize,  fHSize,   fHSize);
		                rVexList.AddVex( fHSize,  fHSize,   fHSize);
		                rVexList.AddVex( fHSize,  fHSize, - fHSize);
		                rVexList.AddVex(-fHSize,  fHSize, - fHSize);
		*/
	}

	rVexList.AddVex(-X - Y - Z);
	rVexList.AddVex(X - Y - Z);
	rVexList.AddVex(X - Y + Z);
	rVexList.AddVex(-X - Y + Z);

	rVexList.AddVex(-X + Y + Z);
	rVexList.AddVex(X + Y + Z);
	rVexList.AddVex(X + Y - Z);
	rVexList.AddVex(-X + Y - Z);

	if (m_bUseLighting)
	{
		m_mMaterial[MAT_CUBE].Diffuse(1.0, 1.0, 1.0, m_ActiveColor.Data()[3]);
		m_mMaterial[MAT_CUBE].Ambient(m_ActiveColor);
		//		m_mMaterial[MAT_CUBE].Apply();

		//		FILE *pFile;
		//		if ((pFile = fopen("Cube.dat", "w")))
		//		{

		for (i = 0; i < 4; i++)
		{
			rVexList.AddNormal(-Y);
			//			fprintf(pFile, "%d (%f %f %f)\n", i, rVexList[0][0], rVexList[0][1], rVexList[0][2]);
		}

		for (i = 4; i < 8; i++)
		{
			rVexList.AddNormal(Y);
			//			fprintf(pFile, "%d (%f %f %f)\n", i, rVexList[0][0], rVexList[0][1], rVexList[0][2]);
		}

		for (i = 8; i < 12; i++)
		{
			A = rVexList[puIdxQuads[i]].xVex;
			rVexList.AddVex(A);
			rVexList.AddNormal(-Z);
			//			fprintf(pFile, "%d (%f %f %f)\n", i, rVexList[0][0], rVexList[0][1], rVexList[0][2]);
		}

		for (i = 12; i < 16; i++)
		{
			A = rVexList[puIdxQuads[i]].xVex;
			rVexList.AddVex(A);
			rVexList.AddNormal(X);
			//			fprintf(pFile, "%d (%f %f %f)\n", i, rVexList[0][0], rVexList[0][1], rVexList[0][2]);
		}

		for (i = 16; i < 20; i++)
		{
			A = rVexList[puIdxQuads[i]].xVex;
			rVexList.AddVex(A);
			rVexList.AddNormal(Z);
			//			fprintf(pFile, "%d (%f %f %f)\n", i, rVexList[0][0], rVexList[0][1], rVexList[0][2]);
		}

		for (i = 20; i < 24; i++)
		{
			A = (COGLVertex) rVexList[puIdxQuads[i]].xVex;
			rVexList.AddVex(A);
			rVexList.AddNormal(-X);
			//			fprintf(pFile, "%d (%f %f %f)\n", i, rVexList[0][0], rVexList[0][1], rVexList[0][2]);
		}

		//		fprintf(pFile, "Last (%f %f %f)\n", rVexList[23][0], rVexList[23][1], rVexList[23][2]);

		//		fclose(pFile);
		//		}
	}
	else
	{
		rVexList.SetIdxList(24, puIdxQuads);
	}

	//glEnable(GL_POLYGON_OFFSET_FILL);
	//glPolygonOffset(1.0, 1.0);

	//rVexList.Apply();

	//glDisable(GL_POLYGON_OFFSET_FILL);

	m_pScene->Add(m_pSceneRep->New(pVexList));

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Cube

bool COGLDrawBase::DrawRhombus(float fSize)
{
	COGLVertex pVex[3];
	float fHSize     = 0.5f * fSize;
	unsigned puIdx[] = { 0, 5, 4, 3, 2, 5, 1, 2, 3, 4, 5, 2 };

	COGLVertexList* pVexList = new COGLVertexList;
	if (!pVexList)
	{
		return false;
	}

	COGLVertexList& rVexList = *pVexList;

	rVexList.SetMode(GL_TRIANGLE_FAN);
	rVexList.Reserve(6);
	rVexList.SetPolygonOffsetFill(1.0f, 1.0f);

	rVexList.AddVex(0.0, fHSize, 0.0);
	rVexList.AddVex(0.0, -fHSize, 0.0);

	rVexList.AddVex(fHSize, 0.0, 0.0);
	rVexList.AddVex(0.0, 0.0, fHSize);
	rVexList.AddVex(-fHSize, 0.0, 0.0);
	rVexList.AddVex(0.0, 0.0, -fHSize);

	if (m_bUseLighting)
	{
		rVexList.AddNormal(0.0, 1.0, 0.0);
		rVexList.AddNormal(0.0, -1.0, 0.0);
		rVexList.AddNormal(1.0, 0.0, 0.0);
		rVexList.AddNormal(0.0, 0.0, 1.0);
		rVexList.AddNormal(-1.0, 0.0, 0.0);
		rVexList.AddNormal(0.0, 0.0, -1.0);
	}

	rVexList.SetIdxList(6, puIdx);
	rVexList.AddIdxList(6, &puIdx[6]);

	m_pScene->Add(m_pSceneRep->New(pVexList));

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Initialize Sphere Display Lists

bool COGLDrawBase::InitSphereVexLists()
{
	for (int i = 0; i < 10; ++i)
	{
		m_Sphere[i].Set(1);
	}

	GenVexIcosahedron(m_Sphere[0][0], 1, 1);
	GenVexIcosahedron(m_Sphere[1][0], 1, 2);
	GenVexIcosahedron(m_Sphere[2][0], 1, 3);
	GenVexIcosahedron(m_Sphere[3][0], 1, 4);
	GenVexIcosahedron(m_Sphere[4][0], 1, 5);
	m_Sphere[5][0] = m_Sphere[4][0];
	m_Sphere[6][0] = m_Sphere[4][0];
	m_Sphere[7][0] = m_Sphere[4][0];
	m_Sphere[8][0] = m_Sphere[4][0];
	m_Sphere[9][0] = m_Sphere[4][0];

	for (int i = 0; i < 10; ++i)
	{
		GenCubeSphere(m_CubeSphere[i], 2 * i + 3, 2 * i + 3);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Generate Vertices of a sphere as projection of a cube

bool COGLDrawBase::GenCubeSphere(MemObj<COGLVertexList>& mVexList, int iRowCnt, int iColCnt)
{
	Mem<COGLVertex> mVex, mTex, mNorm;
	MemObj<COGLColor> mCol;
	int iVexCnt, iVex, iRow, iCol;
	double dX, dY, dZ, dStepX, dStepY, dFac;

	if ((iRowCnt < 2) || (iColCnt < 2))
	{
		return false;
	}

	iVexCnt = iRowCnt * iColCnt;
	mVex.Set(iVexCnt);
	mTex.Set(iVexCnt);
	mNorm.Set(iVexCnt);

	dStepX = 2.0 / double(iRowCnt - 1);
	dStepY = 2.0 / double(iColCnt - 1);
	dZ     = 1.0;

	dY = -1.0;
	for (iRow = 0, iVex = 0; iRow < iRowCnt; ++iRow, dY += dStepY)
	{
		dX = -1.0;
		for (iCol = 0; iCol < iColCnt; ++iCol, dX += dStepX, ++iVex)
		{
			mVex[iVex].Set(float(dX), float(dY), float(dZ));

			dFac = 1.0 / (1.0 + 0.2 * sqrt(dX * dX + dY * dY));
			mTex[iVex].Set(float((dX * dFac + 1.0) / 2.0),
					float((dY * dFac + 1.0) / 2.0),
					0.0f);

			mVex[iVex].Norm();
			mNorm[iVex] = mVex[iVex];
		}
	}

	mVexList.Set(6);

	GenVexPointSurface(iRowCnt, iColCnt, mVex, mNorm, mTex, mCol,
			0.0f, &mVexList[0], 0);

	int iRot, iRotCnt = 3;

	for (iRot = 0; iRot < iRotCnt; ++iRot)
	{
		// Rotate by 90� about y-axis
		mVexList[iRot + 1] = mVexList[iRot];
		COGLVertexList& xCurVexList = mVexList[iRot + 1];
		//COGLVertexList::TVexList &rCurVexList = mVexList[iRot + 1].GetVexList();
		iVexCnt = xCurVexList.Count();

		for (iVex = 0; iVex < iVexCnt; ++iVex)
		{
			COGLVertex& rVex = xCurVexList[iVex].xVex;
			xCurVexList[iVex].xVex.Set(rVex[2], rVex[1], -rVex[0]);
			xCurVexList[iVex].xNorm = xCurVexList[iVex].xVex;
		}

		//mVexList[ iRot + 1 ].GetNormList() = rCurVexList;
	}

	{
		// Rotate by 90� about x-axis
		mVexList[4] = mVexList[0];
		COGLVertexList& xCurVexList = mVexList[4];
		//COGLVertexList::TVexList &rCurVexList = mVexList[ 4 ].GetVexList();
		iVexCnt = xCurVexList.Count();

		for (iVex = 0; iVex < iVexCnt; ++iVex)
		{
			COGLVertex& rVex = xCurVexList[iVex].xVex;
			xCurVexList[iVex].xVex.Set(rVex[0], rVex[2], -rVex[1]);
			xCurVexList[iVex].xNorm = xCurVexList[iVex].xVex;
		}

		//mVexList[ 4 ].GetNormList() = rCurVexList;
	}

	{
		// Rotate by -90� about x-axis
		mVexList[5] = mVexList[0];
		COGLVertexList& xCurVexList = mVexList[5];
		//COGLVertexList::TVexList &rCurVexList = mVexList[ 5 ].GetVexList();
		iVexCnt = xCurVexList.Count();

		for (iVex = 0; iVex < iVexCnt; ++iVex)
		{
			COGLVertex& rVex = xCurVexList[iVex].xVex;
			xCurVexList[iVex].xVex.Set(rVex[0], -rVex[2], rVex[1]);
			xCurVexList[iVex].xNorm = xCurVexList[iVex].xVex;
		}

		//mVexList[ 5 ].GetNormList() = rCurVexList;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Generates vertices of an Icosahedron with radius fRadius and 2^iPower subdivisions.
// This approximates a sphere for higher powers

bool COGLDrawBase::GenVexIcosahedron(COGLVertexList& rVexList, float fRadius, int iPower)
{
	int i, iStep, iTriCount, iTri, iVexCount, iIdxPos;
	float fTau = float((sqrt(5.0) - 1.0) / 2.0);
	double dTh, dPhi, dLen, dX, dY, dZ, dPi, dHPi, d2Pi;
	//	double dMX, dMY, dMZ, dFac;
	//	float fS, fT;
	MultiV<double> pvX[12], vR, vRr, vD;

	dHPi = acos(0.0);
	dPi  = 2.0 * dHPi;
	d2Pi = 2.0 * dPi;

	Mem<STriplet> mIdxList, mNewIdxList;
	TTriplet pInitTriplets[] =
	{
		{ 0, 1, 10 },
		{ 0, 10, 7 },
		{ 0, 7, 6 },
		{ 0, 6, 2 },
		{ 0, 2, 1 },

		{ 11, 10, 9 },
		{ 11, 9, 4 },
		{ 11, 4, 5 },
		{ 11, 5, 7 },
		{ 11, 7, 10 },

		{ 8, 1, 2 },
		{ 8, 2, 3 },
		{ 8, 3, 4 },
		{ 8, 4, 9 },
		{ 8, 9, 1 },

		{ 2, 6, 3 },
		{ 6, 7, 5 },
		{ 5, 3, 6 },
		{ 5, 4, 3 },
		{ 1, 9, 10 }
	};

	rVexList.Reset();
	rVexList.SetMode(GL_TRIANGLES);

	//COGLVertexList::TVexList &rVex = rVexList.GetVexList();
	//COGLVertexList::TVexList &rNorm = rVexList.GetNormList();
	//COGLVertexList::TVexList &rTex = rVexList.GetTexList();

	// Icosahedron constructed from golden squares
	pvX[0]  = E3GA<double>::Vec(0, 1, fTau);
	pvX[1]  = E3GA<double>::Vec(0, 1, -fTau);
	pvX[2]  = E3GA<double>::Vec(1, fTau, 0);
	pvX[3]  = E3GA<double>::Vec(1, -fTau, 0);
	pvX[4]  = E3GA<double>::Vec(0, -1, -fTau);
	pvX[5]  = E3GA<double>::Vec(0, -1, fTau);
	pvX[6]  = E3GA<double>::Vec(fTau, 0, 1);
	pvX[7]  = E3GA<double>::Vec(-fTau, 0, 1);
	pvX[8]  = E3GA<double>::Vec(fTau, 0, -1);
	pvX[9]  = E3GA<double>::Vec(-fTau, 0, -1);
	pvX[10] = E3GA<double>::Vec(-1, fTau, 0);
	pvX[11] = E3GA<double>::Vec(-1, -fTau, 0);

	vR = E3GA<double>::Rotor(1, 0, 0, acos(pvX[0][2] / sqrt(1 + fTau * fTau)));
	rVexList.Reserve(12);
	for (i = 0; i < 12; i++)
	{
		vD     = vR;
		vD    &= pvX[i];
		vD    &= vR.Reverse();
		pvX[i] = vD;
		//pvX[i] = ((vR & pvX[i]) & (~vR));
		rVexList[i].xVex.Set(float(pvX[i][1]), float(pvX[i][2]), float(pvX[i][3]));
		rVexList[i].xVex.Norm();
	}

	mIdxList.Set(20);
	memcpy(mIdxList.Data(), pInitTriplets, 60 * sizeof(int));

	rVexList.UpdateVexCount();

	// Subdivide Triangles
	iVexCount = rVexList.Count();
	for (iStep = 0; iStep < iPower; iStep++)
	{
		iTriCount = (int) (mIdxList.Count());	// Number of Triangles
		mNewIdxList.Set(0);

		rVexList.ReserveAdd(iTriCount * 3);

		for (iTri = 0; iTri < iTriCount; iTri++)
		{
			unsigned* pTriIdx;
			TTriplet pIdx;
			TTriplet pNewIdx;

			memcpy(pIdx, mIdxList[iTri].Data(), 3 * sizeof(int));

			rVexList[iVexCount++].xVex = rVexList[pIdx[0]].xVex + rVexList[pIdx[1]].xVex;
			rVexList[iVexCount++].xVex = rVexList[pIdx[1]].xVex + rVexList[pIdx[2]].xVex;
			rVexList[iVexCount++].xVex = rVexList[pIdx[2]].xVex + rVexList[pIdx[0]].xVex;

			rVexList[iVexCount - 1].xVex.Norm();
			rVexList[iVexCount - 2].xVex.Norm();
			rVexList[iVexCount - 3].xVex.Norm();

			pNewIdx[0] = iVexCount - 2;
			pNewIdx[1] = iVexCount - 1;
			pNewIdx[2] = iVexCount - 3;

			iIdxPos = (int) (mNewIdxList.Count());
			mNewIdxList.Add(4);

			pTriIdx    = mNewIdxList[iIdxPos].Data();
			pTriIdx[0] = pIdx[0];
			pTriIdx[1] = pNewIdx[2];
			pTriIdx[2] = pNewIdx[1];

			pTriIdx    = mNewIdxList[iIdxPos + 1].Data();
			pTriIdx[0] = pIdx[1];
			pTriIdx[1] = pNewIdx[0];
			pTriIdx[2] = pNewIdx[2];

			pTriIdx    = mNewIdxList[iIdxPos + 2].Data();
			pTriIdx[0] = pIdx[2];
			pTriIdx[1] = pNewIdx[1];
			pTriIdx[2] = pNewIdx[0];

			pTriIdx    = mNewIdxList[iIdxPos + 3].Data();
			pTriIdx[0] = pNewIdx[2];
			pTriIdx[1] = pNewIdx[0];
			pTriIdx[2] = pNewIdx[1];
		}

		mIdxList = mNewIdxList;
	}

	rVexList.UpdateVexCount();

	// The normals are the vertices themselves
	for (i = 0; i < rVexList.Count(); ++i)
	{
		rVexList[i].xNorm = rVexList[i].xVex;
	}

	// Generate texture coordinates and scale vertices to radius
	iVexCount = rVexList.Count();
	for (i = 0; i < iVexCount; i++)
	{
		COGLVertex& rTheVex = rVexList[i].xVex;

		dX = rTheVex[0];
		dY = rTheVex[1];
		dZ = rTheVex[2];

		// Scale vertices to radius
		rTheVex *= fRadius;

		/*
		                dMX = Mag( dX );
		                dMY = Mag( dY );
		                dMZ = Mag( dZ );

		                if ( dMX >= dMY && dMX >= dMZ )
		                {
		                        dFac = 1.0 / dMX;
		                        if ( dX > 0.0 )
		                                fS = float( dZ * dFac );
		                        else
		                                fS = -float( dZ * dFac );

		                        fT = float( dY * dFac );
		                }
		                else if ( dMY >= dMX && dMY >= dMZ )
		                {
		                        dFac = 1.0 / dMY;

		                        fS = float( dZ * dFac );

		                        if ( dY > 0.0 )
		                                fT = -float( dX * dFac );
		                        else
		                                fT = float( dX * dFac );
		                }
		                else
		                {
		                        dFac = 1.0 / dMZ;

		                        fT = float( dY * dFac );

		                        if ( dZ > 0.0 )
		                                fS = -float( dX * dFac );
		                        else
		                                fS = float( dX * dFac );
		                }

		                fS = (fS + 1.0f) / 2.0f;
		                fT = (fT + 1.0f) / 2.0f;

		                if ( fS > 1.0f ) fS = 1.0f;
		                else if ( fS < 0.0f ) fS = 0.0f;

		                if ( fT > 1.0f ) fT = 1.0f;
		                else if ( fT < 0.0f ) fT = 0.0f;

		                rTex[i].Set( fS, fT, 0.0f );
		*/

		dLen = sqrt(dX * dX + dY * dY);
		if (!::IsZero(dLen, m_dSensitivity))
		{
			if (Mag(dY) < Mag(dX))
			{
				if (dX >= 0)
				{
					if (dY >= 0)
					{
						dTh = atan(dY  / dX);
					}
					else
					{
						dTh = 2 * dPi + atan(dY / dX);
					}
				}
				else
				{
					dTh = atan(dY / dX) + dPi;
				}
			}
			else
			{
				if (dY >= 0)
				{
					dTh = dHPi - atan(dX  / dY);
				}
				else
				{
					dTh = 3 * dHPi - atan(dX / dY);
				}
			}
		}

		if (dLen < Mag(dZ))
		{
			if (dZ >= 0)
			{
				dPhi = atan(dLen / dZ);
			}
			else
			{
				dPhi = atan(dLen / dZ) + dPi;
			}
		}
		else
		{
			dPhi = dHPi - atan(dZ / dLen);
		}

		//		dTh = dTh - dHPi;
		//		if (dTh < 0)
		//			dTh += d2Pi;

		rVexList[i].xTex.Set(float(Mag(dTh / dPi - 1.0)), float(dPhi / dPi), 0);
		//rVexList.AddCol(Mag(dTh/dPi - 1), 0, dPhi/dPi);
	}

	// Set the index list
	rVexList.SetIdxList(mIdxList.Count() * 3, &mIdxList[0][0]);

	rVexList.UpdateVexCount();
	rVexList.UpdateNormCount();
	rVexList.UpdateTexCount();

	return true;
}

//////////////////////////////////////////////////////////////////////
// Draws an Icosahedron at pos. xP, with radius fRadius and 2^iPower subdivisions.
// This approximates a sphere for higher powers

bool COGLDrawBase::DrawIcosahedron(const COGLVertex& xP, float fRadius, int iPower, bool bSolid)
{
	bool bNegRadius = false;

	if (fRadius == 0.0)
	{
		return false;
	}

	if (fRadius < 0.0)
	{
		if (!m_bShowImaginaryObjects)
		{
			return false;
		}

		fRadius    = -fRadius;
		bNegRadius = true;
	}

	COGLVertexList* pVexList = new COGLVertexList();
	if (!pVexList)
	{
		return false;
	}

	COGLVertexList& VexList = *pVexList;

	GenVexIcosahedron(VexList, fRadius, iPower);

	VexList.SetName("Icosahedron");
	VexList.EnableTransform();
	VexList.SetTranslation1(xP);

	if (bNegRadius)
	{
		VexList.EnableOverrideAlpha(true);
		VexList.SetOverrideAlphaValue(0.5f);
	}

	if (!bSolid)
	{
		VexList.EnablePolygonMode(true);
		VexList.SetPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		if (bNegRadius)
		{
			VexList.EnableLineStipple(true);
			VexList.SetLineStipple(1, 0x5555);
		}
	}
	else
	{
		VexList.SetPolygonOffsetFill(1.0, 1.0);
		VexList.EnablePolygonCullWhenTransparent(true);
	}

	m_pScene->Add(m_pSceneRep->New(pVexList));

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Sphere

bool COGLDrawBase::DrawSphere(const COGLVertex& xC, float fRadius, bool bSolid, bool bDirected)
{
	//int iSphereID = 0;
	bool bNegRadius = false;

	if (fRadius == 0.0)
	{
		return false;
	}

	if (fRadius < 0.0)
	{
		if (!m_bShowImaginaryObjects)
		{
			return false;
		}

		fRadius    = -fRadius;
		bNegRadius = true;
	}

	//if (fRadius < 0.25f)
	//	iSphereID = 0;
	//else
	//	iSphereID = 1;

	MemObj<COGLVertexList>* pList;
	if (bSolid)
	{
		pList = &m_CubeSphere[m_iSphereDetailLevel];
	}
	else
	{
		pList = &m_Sphere[m_iSphereDetailLevel];
	}

	MemObj<COGLVertexList>& rList = *pList;

	size_t iVexList, iVexListCnt = rList.Count();
	COGLVertexList* pVexList;

	for (iVexList = 0; iVexList < iVexListCnt; ++iVexList)
	{
		pVexList = dynamic_cast<COGLVertexList*>(rList[iVexList].Copy());

		if (bDirected)
		{
			pVexList->EnableScaleBackColor(true);
			pVexList->SetBackColorScale(m_fBackFaceColFac);
		}

		if (bNegRadius)
		{
			pVexList->EnableOverrideAlpha(true);
			pVexList->SetOverrideAlphaValue(0.5f);
		}

		pVexList->EnableTransform(true);
		pVexList->SetTranslation1(xC);

		pVexList->EnableScaling(true);
		pVexList->SetScaling(COGLVertex(fRadius, fRadius, fRadius));

		if (!bSolid)
		{
			pVexList->EnablePolygonMode(true);
			pVexList->SetPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			if (bNegRadius)
			{
				pVexList->EnableLineStipple(true);
				pVexList->SetLineStipple(1, 0x5555);
			}
		}
		else
		{
			pVexList->SetPolygonOffsetFill(1.0, 1.0);
			pVexList->EnablePolygonCullWhenTransparent(true);
		}

		m_pScene->Add(m_pSceneRep->New(pVexList));
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Ellipsoid

bool COGLDrawBase::DrawEllipsoid(const COGLVertex& xC,           const COGLVertex& xX, const COGLVertex& xY, const COGLVertex& xZ, bool bSolid)
{
	int i;

	MemObj<COGLVertexList>* pList;
	if (bSolid)
	{
		pList = &m_CubeSphere[m_iSphereDetailLevel];
	}
	else
	{
		pList = &m_Sphere[m_iSphereDetailLevel];
	}

	MemObj<COGLVertexList>& rList = *pList;

	Matrix<float> matRot(4, 4), matTrans(4, 4);
	COGLVertex xNX = xX;
	COGLVertex xYp, xNYp;
	COGLVertex xZp, xNZp;

	xNX.Norm();
	xYp  = xY - (xNX * xY) * xNX;
	xNYp = xYp;
	xNYp.Norm();

	xNZp = xNX ^ xNYp;
	xZp  = (xZ * xNZp) * xNZp;

	matRot.Identity();
	matTrans.Identity();

	for (i = 0; i < 3; i++)
	{
		matRot(i + 1, 1)   = xX[i];
		matRot(i + 1, 2)   = xYp[i];
		matRot(i + 1, 3)   = xZp[i];
		matTrans(i + 1, 4) = xC[i];
	}

	matTrans = matTrans * matRot;

	size_t iVexList, iVexListCnt = rList.Count();
	COGLVertexList* pVexList;

	for (iVexList = 0; iVexList < iVexListCnt; ++iVexList)
	{
		pVexList = dynamic_cast<COGLVertexList*>(rList[iVexList].Copy());

		pVexList->EnableGenTransform(true);
		pVexList->SetGenTransformMatrix(matTrans);

		if (!bSolid)
		{
			pVexList->EnablePolygonMode(true);
			pVexList->SetPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			pVexList->SetPolygonOffsetFill(1.0, 1.0);
			pVexList->EnablePolygonCullWhenTransparent(true);
		}

		m_pScene->Add(m_pSceneRep->New(pVexList));
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool COGLDrawBase::DrawPlane(const COGLVertex& xP, const COGLVertex& xA, const COGLVertex& xB, bool bCornerVecs, bool bDirected)
{
	COGLVertexList* pxVexList = new COGLVertexList();
	COGLVertexList& xVexList  = *pxVexList;

	xVexList.Reset();
	xVexList.SetName("Plane");
	xVexList.SetMode(GL_QUADS);
	xVexList.SetPolygonOffsetFill(1.0f, 1.0f);
	xVexList.Reserve(4);

	if (m_bUseLighting)
	{
		COGLVertex xN = xA ^ xB;
		xN.Norm();

		xVexList[0].xNorm = xN;
		xVexList[1].xNorm = xN;
		xVexList[2].xNorm = xN;
		xVexList[3].xNorm = xN;
		xVexList.UpdateNormCount();
	}

	float fMaxX = (m_bUseAbsTexCoords) ? xA.Mag() : 1.0f;
	float fMaxY = (m_bUseAbsTexCoords) ? xB.Mag() : 1.0f;

	float fTexMaxX = (m_fTexRepFac != 0.0f) ? fMaxX / m_fTexRepFac : 1.0f;
	float fTexMaxY = (m_fTexRepFac != 0.0f) ? fMaxY / m_fTexRepFac : 1.0f;

	xVexList[0].xTex.Set(0.0f, 0.0f, 0);
	xVexList[1].xTex.Set(fTexMaxX, 0.0f, 0);
	xVexList[2].xTex.Set(fTexMaxX, fTexMaxY, 0);
	xVexList[3].xTex.Set(0.0f, fTexMaxY, 0);

	COGLVertex xU = 0.5f * xA;
	COGLVertex xV = 0.5f * xB;

	if (bCornerVecs)
	{
		xVexList[0].xVex = xP - xU;
		xVexList[1].xVex = xP - xV;
		xVexList[2].xVex = xP + xU;
		xVexList[3].xVex = xP + xV;
	}
	else
	{
		xVexList[0].xVex = xP - xU - xV;
		xVexList[1].xVex = xP + xU - xV;
		xVexList[2].xVex = xP + xU + xV;
		xVexList[3].xVex = xP - xU + xV;
	}

	xVexList.UpdateVexCount();
	xVexList.UpdateTexCount();

	if (bDirected)
	{
		xVexList.EnableScaleBackColor(true);
		xVexList.SetBackColorScale(m_fBackFaceColFac);
	}

	m_pScene->Add(m_pSceneRep->New(pxVexList));
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Polygon

bool COGLDrawBase::DrawPolygon(Mem<COGLVertex>& mVex, Mem<COGLVertex>& mNorm, bool bDirected)
{
	size_t iVex, iVexCnt = mVex.Count();

	if (iVexCnt < 3)
	{
		return false;
	}

	COGLVertexList* pVexList = new COGLVertexList;

	if (!pVexList)
	{
		return false;
	}

	COGLVertexList& rVexList = *pVexList;

	rVexList.Reset();

	rVexList.SetMode(GL_POLYGON);
	rVexList.SetPolygonOffsetFill(1.0f, 1.0f);

	//COGLVertexList::TVexList &rVex = rVexList.GetVexList();
	//COGLVertexList::TVexList &rNorm = rVexList.GetNormList();
	//COGLVertexList::TVexList &rTex = rVexList.GetTexList();

	rVexList.Reserve(int(mVex.Count()));

	for (iVex = 0; iVex < iVexCnt; ++iVex)
	{
		rVexList[iVex].xVex = mVex[iVex];
	}

	if (m_bUseLighting)
	{
		for (iVex = 0; iVex < iVexCnt; ++iVex)
		{
			rVexList[iVex].xNorm = mNorm[iVex].Norm();
		}

		rVexList.UpdateNormCount();
	}

	rVexList.UpdateVexCount();

	/*
	        float fTexMinX = 0.0f;
	        float fTexMinY = 0.0f;
	        float fTexMaxX = 1.0f;
	        float fTexMaxY = 1.0f;
	        float fMaxX = 1.0f, fMaxY = 1.0f;

	        if (m_bUseAbsTexCoords)
	        {
	                fMaxX = xA.Mag();
	                fMaxY = xB.Mag();
	        }

	        if (m_fTexRepFac != 0.0f)
	        {
	                fTexMaxX = fMaxX / m_fTexRepFac;
	                fTexMaxY = fMaxY / m_fTexRepFac;
	        }

	        rTex.Set( 4 );
	        rTex[0].Set(fTexMinX, fTexMaxY, 0);
	        rTex[1].Set(fTexMaxX, fTexMaxY, 0);
	        rTex[2].Set(fTexMaxX, fTexMinY, 0);
	        rTex[3].Set(fTexMinX, fTexMinY, 0);
	*/

	pVexList->SetName("Polygon");

	if (bDirected)
	{
		pVexList->EnableScaleBackColor(true);
		pVexList->SetBackColorScale(m_fBackFaceColFac);
	}
	m_pScene->Add(m_pSceneRep->New(pVexList));

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Plane

bool COGLDrawBase::DrawQuad(const COGLVertex& xA, const COGLVertex& xB, const COGLVertex& xC, const COGLVertex& xD)
{
	COGLColor Dark;
	COGLVertex xN, xX, xY;
	COGLVertexList* pVexList;

	pVexList = new COGLVertexList();
	if (!pVexList)
	{
		return false;
	}

	COGLVertexList& VexList = *pVexList;

	xX = xB - xA;
	xY = xD - xA;
	xN = xX ^ xY;

	Dark = m_fBackFaceColFac * m_ActiveColor;

	VexList.SetMode(GL_QUADS);
	VexList.Reserve(4);

	if (m_bUseLighting)
	{
		VexList.AddNormal(xN);
		VexList.AddNormal(xN);
		VexList.AddNormal(xN);
		VexList.AddNormal(xN);
	}

	float fTexMaxX = 1.0f;
	float fTexMaxY = 1.0f;

	VexList.AddTex(0, fTexMaxY, 0);
	VexList.AddTex(fTexMaxX, fTexMaxY, 0);
	VexList.AddTex(fTexMaxX, 0, 0);
	VexList.AddTex(0, 0, 0);

	VexList.AddVex(xA);
	VexList.AddVex(xB);
	VexList.AddVex(xC);
	VexList.AddVex(xD);

	VexList.SetName("Quad");
	VexList.SetPolygonOffsetFill(1.0f, 1.0f);

	m_pScene->Add(m_pSceneRep->New(&VexList));

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Box

//bool COGLDrawBase::DrawBox(const COGLVertex& xP, const COGLVertex& xA,
//								 const COGLVertex& xB, float fWidth,
//								 bool bCornerVecs, bool bDirected,
//								 COGLVertexList *_pVexList,
//								 bool bDoDraw)
//{
//	int piIdx[] = { 3, 2, 1, 0 };
//	COGLColor Dark;
//	COGLVertex xU, xV, xN, xW, xNa, xNb, xC1, xC2, xC3, xC4;
//	COGLVertexList *pVexList = 0;
//
//	if (_pVexList != 0)
//	{
//		pVexList = _pVexList;
//	}
//	else
//	{
//		pVexList = &m_VexList;
//	}
//
//	COGLVertexList &VexList = *pVexList;
//
//	xN[0] = xA[1]*xB[2] - xA[2]*xB[1];
//	xN[1] = xA[2]*xB[0] - xA[0]*xB[2];
//	xN[2] = xA[0]*xB[1] - xA[1]*xB[0];
//	xN.Norm();
//
//	xNa = xA;
//	xNa.Norm();
//
//	xNb = xB;
//	xNb.Norm();
//
//	xU = 0.5f * xA;
//	xV = 0.5f * xB;
//	xW = 0.5f * fWidth * xN;
//
//	Dark = m_fBackFaceColFac * m_ActiveColor;
//
//	VexList.Reset();
//
//	VexList.SetMode(GL_QUADS);
//
//	float fTexMaxX = 1.0f;
//	float fTexMaxY = 1.0f;
//	float fMaxX = 1.0f, fMaxY = 1.0f;
//
//	if (m_bUseAbsTexCoords)
//	{
//		fMaxX = xA.Mag();
//		fMaxY = xB.Mag();
//	}
//
//	if (m_fTexRepFac != 0.0f)
//	{
//		fTexMaxX = fMaxX / m_fTexRepFac;
//		fTexMaxY = fMaxY / m_fTexRepFac;
//	}
//
//	if (m_bUseLighting)
//	{
//		m_mMaterial[MAT_PLANE].Ambient(m_ActiveColor);
//		m_mMaterial[MAT_PLANE].Diffuse(1.0, 1.0, 1.0, m_ActiveColor.Data()[3]);
////		m_mMaterial[MAT_PLANE].Apply();
//	}
//
//	if (bCornerVecs)
//	{
//		VexList.AddVex(xP - xU);
//		VexList.AddVex(xP - xV);
//		VexList.AddVex(xP + xU);
//		VexList.AddVex(xP + xV);
//	}
//	else
//	{
//		xC1 = xP - xU - xV;
//		xC2 = xP + xU - xV;
//		xC3 = xP + xU + xV;
//		xC4 = xP - xU + xV;
//
//		if (m_bUseLighting)
//		{
//			VexList.AddNormal(xN);
//			VexList.AddNormal(xN);
//			VexList.AddNormal(xN);
//			VexList.AddNormal(xN);
//
//			VexList.AddNormal(-xN);
//			VexList.AddNormal(-xN);
//			VexList.AddNormal(-xN);
//			VexList.AddNormal(-xN);
//
//			VexList.AddNormal(xNa);
//			VexList.AddNormal(xNa);
//			VexList.AddNormal(xNa);
//			VexList.AddNormal(xNa);
//
//			VexList.AddNormal(-xNa);
//			VexList.AddNormal(-xNa);
//			VexList.AddNormal(-xNa);
//			VexList.AddNormal(-xNa);
//
//			VexList.AddNormal(xNb);
//			VexList.AddNormal(xNb);
//			VexList.AddNormal(xNb);
//			VexList.AddNormal(xNb);
//
//			VexList.AddNormal(-xNb);
//			VexList.AddNormal(-xNb);
//			VexList.AddNormal(-xNb);
//			VexList.AddNormal(-xNb);
//		}
//
//		// Top
//		VexList.AddVex(xC1 + xW);
//		VexList.AddVex(xC2 + xW);
//		VexList.AddVex(xC3 + xW);
//		VexList.AddVex(xC4 + xW);
//
//		VexList.AddTex(0,fTexMaxY,0);
//		VexList.AddTex(fTexMaxX,fTexMaxY,0);
//		VexList.AddTex(fTexMaxX,0,0);
//		VexList.AddTex(0,0,0);
//
//		// Bottom
//		VexList.AddVex(xC4 - xW);
//		VexList.AddVex(xC3 - xW);
//		VexList.AddVex(xC2 - xW);
//		VexList.AddVex(xC1 - xW);
//
//		VexList.AddTex(0,fTexMaxY,0);
//		VexList.AddTex(fTexMaxX,fTexMaxY,0);
//		VexList.AddTex(fTexMaxX,0,0);
//		VexList.AddTex(0,0,0);
//
//		// xA Normal side
//		VexList.AddVex(xC2 - xW);
//		VexList.AddVex(xC3 - xW);
//		VexList.AddVex(xC3 + xW);
//		VexList.AddVex(xC2 + xW);
//
//		VexList.AddTex(0,fTexMaxY,0);
//		VexList.AddTex(fTexMaxX,fTexMaxY,0);
//		VexList.AddTex(fTexMaxX,0,0);
//		VexList.AddTex(0,0,0);
//
//		// -xA Normal side
//		VexList.AddVex(xC1 + xW);
//		VexList.AddVex(xC4 + xW);
//		VexList.AddVex(xC4 - xW);
//		VexList.AddVex(xC1 - xW);
//
//		VexList.AddTex(0,fTexMaxY,0);
//		VexList.AddTex(fTexMaxX,fTexMaxY,0);
//		VexList.AddTex(fTexMaxX,0,0);
//		VexList.AddTex(0,0,0);
//
//		// xB Normal side
//		VexList.AddVex(xC3 - xW);
//		VexList.AddVex(xC4 - xW);
//		VexList.AddVex(xC4 + xW);
//		VexList.AddVex(xC3 + xW);
//
//		VexList.AddTex(0,fTexMaxY,0);
//		VexList.AddTex(fTexMaxX,fTexMaxY,0);
//		VexList.AddTex(fTexMaxX,0,0);
//		VexList.AddTex(0,0,0);
//
//		// -xB Normal side
//		VexList.AddVex(xC1 - xW);
//		VexList.AddVex(xC2 - xW);
//		VexList.AddVex(xC2 + xW);
//		VexList.AddVex(xC1 + xW);
//
//		VexList.AddTex(0,fTexMaxY,0);
//		VexList.AddTex(fTexMaxX,fTexMaxY,0);
//		VexList.AddTex(fTexMaxX,0,0);
//		VexList.AddTex(0,0,0);
//	}
//
//
//	glEnable(GL_POLYGON_OFFSET_FILL);
//	glPolygonOffset(1.0, 1.0);
//
//	if ( bDoDraw )
////		VexList.Apply();
//
//	glDisable(GL_POLYGON_OFFSET_FILL);
//
//	return true;
//}

//////////////////////////////////////////////////////////////////////
/// Draw Box

bool COGLDrawBase::DrawBox(const COGLVertex& xP, const COGLVertex& xA, const COGLVertex& xB, const COGLVertex& _xC, const MemObj<COGLColor>& rColList)
{
	bool bIsLH = false;
	COGLColor Dark;
	COGLVertex xC, xU, xV, xN, xW, xNa, xNb, xNc, xC1, xC2, xC3, xC4;
	float fTexMaxX = 1.0f;
	float fTexMaxY = 1.0f;
	float fMaxX    = 1.0f, fMaxY = 1.0f;

	COGLVertexList* pVexList = new COGLVertexList;
	COGLVertexList& VexList  = *pVexList;

	if (m_bUseAbsTexCoords)
	{
		fMaxX = xA.Mag();
		fMaxY = xB.Mag();
	}

	if (m_fTexRepFac != 0.0f)
	{
		fTexMaxX = fMaxX / m_fTexRepFac;
		fTexMaxY = fMaxY / m_fTexRepFac;
	}

	xNc = xA ^ xB;	// vector cross product
	xNc.Norm();
	if (xNc * _xC < 0.0f)
	{
		xNc   = -xNc;
		xC    = -_xC;
		bIsLH = true;	// left handed system
	}
	else
	{
		xC = _xC;
	}

	xNa = xB ^ xC;	// vector cross product
	xNa.Norm();

	xNb = xC ^ xA;	// vector cross product
	xNb.Norm();

	xU = 0.5f * xA;
	xV = 0.5f * xB;
	xW = 0.5f * xC;

	VexList.Reset();
	VexList.SetMode(GL_QUADS);

	xC1 = xP - xU - xV;
	xC2 = xP + xU - xV;
	xC3 = xP + xU + xV;
	xC4 = xP - xU + xV;

	int iTex, iVex = 0;
	//COGLVertexList::TVexList &mVex = VexList.GetVexList();
	//COGLVertexList::TVexList &mTex = VexList.GetTexList();
	//COGLVertexList::TVexList &mNorm = VexList.GetNormList();
	//COGLVertexList::TColList &mCol = VexList.GetColList();

	VexList.Reserve(24);

	if (m_bUseLighting)
	{
		VexList[iVex++].xNorm = xNc;
		VexList[iVex++].xNorm = xNc;
		VexList[iVex++].xNorm = xNc;
		VexList[iVex++].xNorm = xNc;

		VexList[iVex++].xNorm = -xNc;
		VexList[iVex++].xNorm = -xNc;
		VexList[iVex++].xNorm = -xNc;
		VexList[iVex++].xNorm = -xNc;

		VexList[iVex++].xNorm = xNa;
		VexList[iVex++].xNorm = xNa;
		VexList[iVex++].xNorm = xNa;
		VexList[iVex++].xNorm = xNa;

		VexList[iVex++].xNorm = -xNa;
		VexList[iVex++].xNorm = -xNa;
		VexList[iVex++].xNorm = -xNa;
		VexList[iVex++].xNorm = -xNa;

		VexList[iVex++].xNorm = xNb;
		VexList[iVex++].xNorm = xNb;
		VexList[iVex++].xNorm = xNb;
		VexList[iVex++].xNorm = xNb;

		VexList[iVex++].xNorm = -xNb;
		VexList[iVex++].xNorm = -xNb;
		VexList[iVex++].xNorm = -xNb;
		VexList[iVex++].xNorm = -xNb;

		VexList.UpdateNormCount();
	}

	if (rColList.Count() == 8)
	{
		iVex = 0;

		if (bIsLH)
		{
			VexList[iVex++].xCol = rColList[4].Data();
			VexList[iVex++].xCol = rColList[5].Data();
			VexList[iVex++].xCol = rColList[6].Data();
			VexList[iVex++].xCol = rColList[7].Data();

			VexList[iVex++].xCol = rColList[3].Data();
			VexList[iVex++].xCol = rColList[2].Data();
			VexList[iVex++].xCol = rColList[1].Data();
			VexList[iVex++].xCol = rColList[0].Data();

			VexList[iVex++].xCol = rColList[1].Data();
			VexList[iVex++].xCol = rColList[2].Data();
			VexList[iVex++].xCol = rColList[6].Data();
			VexList[iVex++].xCol = rColList[5].Data();

			VexList[iVex++].xCol = rColList[4].Data();
			VexList[iVex++].xCol = rColList[7].Data();
			VexList[iVex++].xCol = rColList[3].Data();
			VexList[iVex++].xCol = rColList[0].Data();

			VexList[iVex++].xCol = rColList[2].Data();
			VexList[iVex++].xCol = rColList[3].Data();
			VexList[iVex++].xCol = rColList[7].Data();
			VexList[iVex++].xCol = rColList[6].Data();

			VexList[iVex++].xCol = rColList[0].Data();
			VexList[iVex++].xCol = rColList[1].Data();
			VexList[iVex++].xCol = rColList[5].Data();
			VexList[iVex++].xCol = rColList[4].Data();
		}
		else
		{
			VexList[iVex++].xCol = rColList[0].Data();
			VexList[iVex++].xCol = rColList[1].Data();
			VexList[iVex++].xCol = rColList[2].Data();
			VexList[iVex++].xCol = rColList[3].Data();

			VexList[iVex++].xCol = rColList[7].Data();
			VexList[iVex++].xCol = rColList[6].Data();
			VexList[iVex++].xCol = rColList[5].Data();
			VexList[iVex++].xCol = rColList[4].Data();

			VexList[iVex++].xCol = rColList[5].Data();
			VexList[iVex++].xCol = rColList[6].Data();
			VexList[iVex++].xCol = rColList[2].Data();
			VexList[iVex++].xCol = rColList[1].Data();

			VexList[iVex++].xCol = rColList[0].Data();
			VexList[iVex++].xCol = rColList[3].Data();
			VexList[iVex++].xCol = rColList[7].Data();
			VexList[iVex++].xCol = rColList[4].Data();

			VexList[iVex++].xCol = rColList[6].Data();
			VexList[iVex++].xCol = rColList[7].Data();
			VexList[iVex++].xCol = rColList[3].Data();
			VexList[iVex++].xCol = rColList[2].Data();

			VexList[iVex++].xCol = rColList[4].Data();
			VexList[iVex++].xCol = rColList[5].Data();
			VexList[iVex++].xCol = rColList[1].Data();
			VexList[iVex++].xCol = rColList[0].Data();
		}

		VexList.UpdateColCount();
	}

	iTex = iVex = 0;

	// Top
	VexList[iVex++].xVex = xC1 + xW;
	VexList[iVex++].xVex = xC2 + xW;
	VexList[iVex++].xVex = xC3 + xW;
	VexList[iVex++].xVex = xC4 + xW;

	VexList[iTex++].xTex.Set(0, 0, 0);
	VexList[iTex++].xTex.Set(fTexMaxX, 0, 0);
	VexList[iTex++].xTex.Set(fTexMaxX, fTexMaxY, 0);
	VexList[iTex++].xTex.Set(0, fTexMaxY, 0);

	// Bottom
	VexList[iVex++].xVex = xC4 - xW;
	VexList[iVex++].xVex = xC3 - xW;
	VexList[iVex++].xVex = xC2 - xW;
	VexList[iVex++].xVex = xC1 - xW;

	VexList[iTex++].xTex.Set(fTexMaxX, fTexMaxY, 0);
	VexList[iTex++].xTex.Set(0, fTexMaxY, 0);
	VexList[iTex++].xTex.Set(0, 0, 0);
	VexList[iTex++].xTex.Set(fTexMaxX, 0, 0);

	// xA Normal side
	VexList[iVex++].xVex = xC2 - xW;
	VexList[iVex++].xVex = xC3 - xW;
	VexList[iVex++].xVex = xC3 + xW;
	VexList[iVex++].xVex = xC2 + xW;

	VexList[iTex++].xTex.Set(fTexMaxX, 0, 0);
	VexList[iTex++].xTex.Set(fTexMaxX, fTexMaxY, 0);
	VexList[iTex++].xTex.Set(0, fTexMaxY, 0);
	VexList[iTex++].xTex.Set(0, 0, 0);

	// -xA Normal side
	VexList[iVex++].xVex = xC1 + xW;
	VexList[iVex++].xVex = xC4 + xW;
	VexList[iVex++].xVex = xC4 - xW;
	VexList[iVex++].xVex = xC1 - xW;

	VexList[iTex++].xTex.Set(fTexMaxX, 0, 0);
	VexList[iTex++].xTex.Set(fTexMaxX, fTexMaxY, 0);
	VexList[iTex++].xTex.Set(0, fTexMaxY, 0);
	VexList[iTex++].xTex.Set(0, 0, 0);

	// xB Normal side
	VexList[iVex++].xVex = xC3 - xW;
	VexList[iVex++].xVex = xC4 - xW;
	VexList[iVex++].xVex = xC4 + xW;
	VexList[iVex++].xVex = xC3 + xW;

	VexList[iTex++].xTex.Set(fTexMaxX, fTexMaxY, 0);
	VexList[iTex++].xTex.Set(0, fTexMaxY, 0);
	VexList[iTex++].xTex.Set(0, 0, 0);
	VexList[iTex++].xTex.Set(fTexMaxX, 0, 0);

	// -xB Normal side
	VexList[iVex++].xVex = xC1 - xW;
	VexList[iVex++].xVex = xC2 - xW;
	VexList[iVex++].xVex = xC2 + xW;
	VexList[iVex++].xVex = xC1 + xW;

	VexList[iTex++].xTex.Set(0, 0, 0);
	VexList[iTex++].xTex.Set(fTexMaxX, 0, 0);
	VexList[iTex++].xTex.Set(fTexMaxX, fTexMaxY, 0);
	VexList[iTex++].xTex.Set(0, fTexMaxY, 0);

	VexList.UpdateVexCount();
	VexList.UpdateTexCount();

	pVexList->SetPolygonOffsetFill(1.0f, 1.0f);
	//pVexList->SetFrontFace( GL_CW );

	m_pScene->Add(m_pSceneRep->New(pVexList));

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Triangle

bool COGLDrawBase::DrawTriangle(const COGLVertex& xA, const COGLVertex& xB, const COGLVertex& xC, bool bDirected)
{
	COGLColor Dark;
	COGLVertex xU, xV, xN, xD;
	COGLVertexList* pVexList;

	pVexList = new COGLVertexList();
	if (!pVexList)
	{
		return false;
	}

	COGLVertexList& rVexList = *pVexList;

	xU = xB - xA;
	xV = xC - xA;

	Dark = m_fBackFaceColFac * m_ActiveColor;

	rVexList.SetMode(GL_TRIANGLES);
	rVexList.Reserve(3);

	if (m_bUseLighting)
	{
		MultiV<float> vN;

		vN  = m_E3Base.Vec(xU[0], xU[1], xU[2]);
		vN ^= m_E3Base.Vec(xV[0], xV[1], xV[2]);
		vN  = vN.Dual();
		xN  = &vN[1];
		// We need to normalize the normal!
		xN.Norm();

		rVexList.AddNormal(xN);
		rVexList.AddNormal(xN);
		rVexList.AddNormal(xN);
	}

	rVexList.AddVex(xA);
	rVexList.AddVex(xB);
	rVexList.AddVex(xC);

	//rVexList.AddCol( m_ActiveColor );
	//rVexList.AddCol( m_ActiveColor );
	//rVexList.AddCol( m_ActiveColor );

	float fTexMaxX = 1.0f;
	float fTexMaxY = 1.0f;

	rVexList.AddTex(0, fTexMaxY, 0);
	rVexList.AddTex(fTexMaxX, fTexMaxY, 0);
	rVexList.AddTex(fTexMaxX / 2, 0, 0);

	rVexList.SetName("Triangle");
	rVexList.SetPolygonOffsetFill(1.0f, 1.0f);

	if (bDirected)
	{
		rVexList.EnableScaleBackColor();
		rVexList.SetBackColorScale(0.8f);
	}

	m_pScene->Add(m_pSceneRep->New(&rVexList));

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Circle

bool COGLDrawBase::DrawCircle(const COGLVertex& xC, const COGLVertex& xN, float fR, float fAngleStep)
{
	MemObj<MultiV<float> > mvList;
	MultiV<float> vU, vX, vR;
	COGLVertex xX, xA, xB;

	double dTheta = 0;
	double dRadStep;
	double dPi = acos(-1.0);

	bool bNegRadius = false;

	if ((fR == 0.0f) || (fR > 1000.0f) || (fAngleStep <= 0.0f) || (fAngleStep > 180.0f) ||
	    (xN.Mag() == 0))
	{
		return false;
	}

	if (fR < 0.0)
	{
		if (!m_bShowImaginaryObjects)
		{
			return false;
		}

		fR         = -fR;
		bNegRadius = true;
	}

	int i, iSteps = int(360.0f / fAngleStep);
	dRadStep = 2.0 * dPi / double(iSteps);

	vU = m_E3Base.Vec(xN[0], xN[1], xN[2]) & m_E3Base.vI;
	//vR = m_E3Base.Rotor(vU, (fAngleStep * m_fPi)/180.0f, false);

	FactorBlade<float>(vU, mvList, bool(true), (MultiV<float>*) 0, (float*) 0, m_fSensitivity);
	if (mvList.Count() == 0)
	{
		return false;
	}

	vX = fR & mvList[0];
	xA.Set(vX[1], vX[2], vX[3]);

	vX = fR & mvList[1];
	xB.Set(vX[1], vX[2], vX[3]);

	COGLVertexList* pVexList = new COGLVertexList;
	if (!pVexList)
	{
		return false;
	}

	COGLVertexList& rVexList = *pVexList;

	rVexList.Reset();
	rVexList.SetMode(GL_LINE_STRIP);

	//COGLVertexList::TVexList &mVex = rVexList.GetVexList();
	//COGLVertexList::TVexList &mNorm = rVexList.GetNormList();

	rVexList.Reserve(iSteps + 1);
	//mVex.Set( iSteps + 1);
	//mNorm.Set( iSteps + 1);

	if (bNegRadius)
	{
		rVexList.EnableLineStipple(true);
		rVexList.SetLineStipple(1, 0x5555);
	}

	for (i = 0, dTheta = 0; i < iSteps; i++, dTheta += dRadStep)
	{
		xX = (float(cos(dTheta)) * xA) + (float(sin(dTheta)) * xB);

		rVexList[i].xVex  = xX + xC;
		rVexList[i].xNorm = xN;
	}

	rVexList[i].xVex  = xA + xC;
	rVexList[i].xNorm = xN;

	rVexList.UpdateVexCount();
	rVexList.UpdateNormCount();

	m_pScene->Add(m_pSceneRep->New(pVexList));

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Ellipse

bool COGLDrawBase::DrawEllipse(const COGLVertex& xC,       const COGLVertex& xA, const COGLVertex& xB, float fAngleStep)
{
	MemObj<MultiV<float> > mvList;
	MultiV<float> vA, vB, vX, vN;
	COGLVertex xN, xX;

	double dTheta = 0;
	double dRadStep;
	double dPi = acos(-1.0);

	if ((fAngleStep <= 0.0f) || (fAngleStep > 180.0f) ||
	    (xA.Mag() == 0) || (xB.Mag() == 0) || (xA == xB))
	{
		return false;
	}

	int i, iSteps = int(360.0f / fAngleStep);
	dRadStep = 2.0 * dPi / double(iSteps);

	vA = m_E3Base.Vec(xA[0], xA[1], xA[2]);
	vB = m_E3Base.Vec(xB[0], xB[1], xB[2]);
	vN = *(vA^vB);
	xN.Set(vN[1], vN[2], vN[3]);
	xN.Norm();

	COGLVertexList* pVexList = new COGLVertexList;
	if (!pVexList)
	{
		return false;
	}

	COGLVertexList& rVexList = *pVexList;

	rVexList.Reset();
	rVexList.SetMode(GL_LINE_STRIP);

	//COGLVertexList::TVexList &mVex = rVexList.GetVexList();
	//COGLVertexList::TVexList &mNorm = rVexList.GetNormList();

	rVexList.Reserve(iSteps + 1);

	for (i = 0, dTheta = 0; i < iSteps; i++, dTheta += dRadStep)
	{
		xX = (float(cos(dTheta)) * xA) + (float(sin(dTheta)) * xB);

		rVexList[i].xVex  = xX + xC;
		rVexList[i].xNorm = xN;
	}

	rVexList[i].xVex  = xA + xC;
	rVexList[i].xNorm = xN;

	rVexList.UpdateVexCount();
	rVexList.UpdateNormCount();

	m_pScene->Add(m_pSceneRep->New(pVexList));

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Hyperbola

bool COGLDrawBase::DrawHyperbola(const COGLVertex& xC, const COGLVertex& xA, const COGLVertex& xB, float fAngleStep)
{
	//MemObj<MultiV<float> > mvList;
	MultiV<float> vA, vB, vX, vN, vD;
	COGLVertex xN;

	double dTheta = 0, dAsymptoteLength = 100.0;
	double dRadStep;
	double dPi = acos(-1.0);

	if ((fAngleStep <= 0.0f) || (fAngleStep > 90.0f) ||
	    (xA.Mag() == 0) || (xB.Mag() == 0) || (xA == xB))
	{
		return false;
	}

	int i, iSteps = int(180.0f / fAngleStep);
	dRadStep = 0.98 * dPi / double(iSteps);

	//dAMag = sqrt(xA[0]*xA[0] + xA[1]*xA[1] + xA[2]*xA[2]);
	//dBMag = sqrt(xB[0]*xB[0] + xB[1]*xB[1] + xB[2]*xB[2]);
	//dBAFac = dBMag / dAMag;

	vA = m_E3Base.Vec(xA[0], xA[1], xA[2]);
	vB = m_E3Base.Vec(xB[0], xB[1], xB[2]);
	vN = *(vA^vB);
	xN.Set(vN[1], vN[2], vN[3]);

	COGLVertexList* pVexList = new COGLVertexList;
	if (!pVexList)
	{
		return false;
	}

	COGLVertexList& rVexList = *pVexList;

	rVexList.Reset();
	rVexList.SetMode(GL_LINE_STRIP);

	//COGLVertexList::TVexList &mVex = rVexList.GetVexList();
	//COGLVertexList::TVexList &mNorm = rVexList.GetNormList();

	rVexList.Reserve(iSteps + 2);

	// Draw first branch
	dTheta = -0.49 * dPi;
	// Add Asymptotic vector
	vX  = (float(1.0 / cos(dTheta)) & vA) + (float(tan(dTheta)) & vB);
	vD  = (float(1.0 / cos(-0.495 * dPi)) & vA) + (float(tan(-0.495 * dPi)) & vB);
	vD -= vX;
	vD /= float(sqrt(vD.Mag2()));
	vX += float(dAsymptoteLength) ^ vD;

	rVexList[0].xVex.Set(vX[1] + xC[0], vX[2] + xC[1], vX[3] + xC[2]);
	rVexList[0].xNorm = xN;

	// Draw branch
	for (i = 0; i < iSteps; i++, dTheta += dRadStep)
	{
		vX = (float(1.0 / cos(dTheta)) & vA) + (float(tan(dTheta)) & vB);

		rVexList[i + 1].xVex.Set(vX[1] + xC[0], vX[2] + xC[1], vX[3] + xC[2]);
		rVexList[i + 1].xNorm = xN;
	}

	// Add Asymptotic vector
	dTheta = 0.49 * dPi;
	vX     = (float(1.0 / cos(dTheta)) & vA) + (float(tan(dTheta)) & vB);
	vD     = (float(1.0 / cos(0.495 * dPi)) & vA) + (float(tan(0.495 * dPi)) & vB);
	vD    -= vX;
	vD    /= float(sqrt(vD.Mag2()));
	vX    += float(dAsymptoteLength) ^ vD;

	rVexList[i + 1].xVex.Set(vX[1] + xC[0], vX[2] + xC[1], vX[3] + xC[2]);
	rVexList[i + 1].xNorm = xN;

	rVexList.UpdateVexCount();
	rVexList.UpdateNormCount();

	m_pScene->Add(m_pSceneRep->New(pVexList));

	/////////////////////////////////////////////////
	// Draw second branch
	{
		pVexList = new COGLVertexList;
		if (!pVexList)
		{
			return false;
		}

		COGLVertexList& rVexList = *pVexList;

		rVexList.Reset();
		rVexList.SetMode(GL_LINE_STRIP);

		//COGLVertexList::TVexList &mVex = rVexList.GetVexList();
		//COGLVertexList::TVexList &mNorm = rVexList.GetNormList();

		rVexList.Reserve(iSteps + 2);

		dTheta = -0.49 * dPi;
		// Add Asymptotic vector
		vX  = (float(1.0 / -cos(dTheta)) & vA) + (float(tan(dTheta)) & vB);
		vD  = (float(1.0 / -cos(-0.495 * dPi)) & vA) + (float(tan(-0.495 * dPi)) & vB);
		vD -= vX;
		vD /= float(sqrt(vD.Mag2()));
		vX += float(dAsymptoteLength) ^ vD;

		rVexList[0].xVex.Set(vX[1] + xC[0], vX[2] + xC[1], vX[3] + xC[2]);
		rVexList[0].xNorm = xN;

		// Draw branch
		for (i = 0; i < iSteps; i++, dTheta += dRadStep)
		{
			vX = (float(1.0 / -cos(dTheta)) & vA) + (float(tan(dTheta)) & vB);

			rVexList[i + 1].xVex.Set(vX[1] + xC[0], vX[2] + xC[1], vX[3] + xC[2]);
			rVexList[i + 1].xNorm = xN;
		}

		// Add Asymptotic vector
		dTheta = 0.49 * dPi;
		vX     = (float(1.0 / -cos(dTheta)) & vA) + (float(tan(dTheta)) & vB);
		vD     = (float(1.0 / -cos(0.495 * dPi)) & vA) + (float(tan(0.495 * dPi)) & vB);
		vD    -= vX;
		vD    /= float(sqrt(vD.Mag2()));
		vX    += float(dAsymptoteLength) ^ vD;

		rVexList[i + 1].xVex.Set(vX[1] + xC[0], vX[2] + xC[1], vX[3] + xC[2]);
		rVexList[i + 1].xNorm = xN;

		rVexList.UpdateVexCount();
		rVexList.UpdateNormCount();

		m_pScene->Add(m_pSceneRep->New(pVexList));
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Cylinder
///
/// Draw cylinder as surface spanned by two circles
/// xC: Center of bottom circle
/// xN: Vector pointing towards other end of cylinder giving length of it
/// fR: Radius of Cylinder
/// bClosed: Are ends of cylinder closed?
/// fAngleStep: angle step for drawing circles

bool COGLDrawBase::DrawCylinder(const COGLVertex& xC,           const COGLVertex& xN, float fR, bool bClosed, float fAngleStep)
{
	if ((fAngleStep <= 0.0f) || (fAngleStep > 180.0f))
	{
		return false;
	}

	fR = Mag(fR);
	if (fR < 1e-5f)
	{
		return DrawLine(xC, xN, false);
	}

	COGLVertexList* pVexListFront = 0;
	COGLVertexList* pVexListBack  = 0;
	//COGLVertexList::TVexList *pmVexF;
	//COGLVertexList::TVexList *pmTexF;
	//COGLVertexList::TVexList *pmNormF;

	COGLVertexList* pVexList = new COGLVertexList;
	if (!pVexList)
	{
		return false;
	}

	if (bClosed)
	{
		pVexListFront = new COGLVertexList;
		if (!pVexListFront)
		{
			delete pVexList;
			return false;
		}

		pVexListBack = new COGLVertexList;
		if (!pVexListBack)
		{
			delete pVexList;
			delete pVexListFront;
			return false;
		}

		pVexListFront->SetMode(GL_POLYGON);

		//pmVexF = &pVexListFront->GetVexList();
		//pmTexF = &pVexListFront->GetTexList();
		//pmNormF = &pVexListFront->GetNormList();
	}

	pVexList->SetMode(GL_QUAD_STRIP);

	//COGLVertexList::TVexList &mVex = pVexList->GetVexList();
	//COGLVertexList::TVexList &mTex = pVexList->GetTexList();
	//COGLVertexList::TVexList &mNorm = pVexList->GetNormList();
	COGLVertexList& rVexList = *pVexList;

	int iVexCnt, iVex;
	int iStep, iStepCnt = int(360.0f / fAngleStep + 0.5f) + 1;

	double dRadStep = double(fAngleStep * m_fPi) / 180.0;
	double dTexStep = 1.0 / double(iStepCnt - 1);
	double dAngle, dRadius = double(fR);
	double dLen = double(xN.Mag());
	double dX, dY, dCos, dSin, dTexX;

	COGLVertex xAxis, xDir;
	float fRotAngle;

	iVexCnt = 2 * iStepCnt;
	rVexList.Reserve(iVexCnt);
	//mVex.Set( iVexCnt );
	//mTex.Set( iVexCnt );
	//mNorm.Set( iVexCnt );

	if (bClosed)
	{
		pVexListFront->Reserve(iStepCnt);
		//pmVexF->Set( iStepCnt );
		//pmTexF->Set( iStepCnt );
		//pmNormF->Set( iStepCnt );
	}

	for (iStep = 0, dAngle = 0, dTexX = 0, iVex = 0;
	     iStep < iStepCnt;
	     ++iStep, dAngle += dRadStep, dTexX += dTexStep, iVex += 2)
	{
		dCos = cos(dAngle);
		dSin = sin(dAngle);
		dX   = dRadius * dCos;
		dY   = dRadius * dSin;

		rVexList[iVex].xVex.Set(float(dX), float(dY), 0.0f);
		rVexList[iVex + 1].xVex.Set(float(dX), float(dY), float(dLen));

		rVexList[iVex].xNorm.Set(float(dCos), float(dSin), 0.0f);
		rVexList[iVex + 1].xNorm = rVexList[iVex].xNorm;

		rVexList[iVex].xTex.Set(float(dTexX), 0.0f, 0.0f);
		rVexList[iVex + 1].xTex.Set(float(dTexX), 1.0f, 0.0f);

		if (bClosed)
		{
			(*pVexListFront)[iStep].xVex.Set(float(dX), float(-dY), 0.0f);
			(*pVexListFront)[iStep].xNorm.Set(0.0f, 0.0f, 1.0f);
			(*pVexListFront)[iStep].xTex.Set(float((dCos + 1.0) / 2.0),
					float((-dSin + 1.0) / 2.0),
					0.0f);
		}
	}

	rVexList.UpdateVexCount();
	rVexList.UpdateTexCount();
	rVexList.UpdateNormCount();

	pVexList->EnableTransform(true);
	pVexList->SetTranslation1(xC);

	xDir = xN;
	xDir.Norm();
	xAxis     = COGLVertex(0.0f, 0.0f, 1.0f) ^ xDir;
	fRotAngle = float(acos(double(xDir[2]))) * 180.0f / m_fPi;
	pVexList->SetRotation(fRotAngle, xAxis);
	// Set front face to clock wise due to indexing
	pVexList->SetFrontFace(GL_CW);

	m_pScene->Add(m_pSceneRep->New(pVexList));

	if (bClosed)
	{
		(*pVexListBack) = (*pVexListFront);

		pVexListBack->EnableTransform(true);
		pVexListBack->SetTranslation1(xC);
		pVexListBack->SetRotation(fRotAngle, xAxis);
		pVexListBack->SetFrontFace(GL_CW);

		pVexListFront->EnableTransform(true);
		pVexListFront->SetTranslation1(xC);
		pVexListFront->SetRotation(fRotAngle, xAxis);
		pVexListFront->SetTranslation2(COGLVertex(0.0f, 0.0f, float(dLen)));
		pVexListFront->SetFrontFace(GL_CW);

		pVexListFront->UpdateVexCount();
		pVexListFront->UpdateTexCount();
		pVexListFront->UpdateNormCount();

		pVexListBack->UpdateVexCount();
		pVexListBack->UpdateTexCount();
		pVexListBack->UpdateNormCount();

		m_pScene->Add(m_pSceneRep->New(pVexListFront));
		m_pScene->Add(m_pSceneRep->New(pVexListBack));
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Circle Surface
///
/// Draw surface spanned by a set of circles
/// mC: List of circle centers
/// mN: List of circle normals
/// mR: List of circle radii
/// mCol: List of colors for circles
/// fAngleStep: angle step for drawing circles

bool COGLDrawBase::DrawCircleSurface(const Mem<COGLVertex>& mC,
		const Mem<COGLVertex>& mN,
		const Mem<float>& mR,
		const MemObj<COGLColor>& mCol,
		COGLBEReference& refScene,
		float fAngleStep,
		bool bDoDraw)
{
	Mem<unsigned> mIdxList;
	MemObj<MultiV<float> > mvList;
	MultiV<float> vU, vX, vR, vN, vNPrev, vB, vR12, vXN;
	COGLVertex xC;
	COGLVertexList* pVexList = 0;
	float fR, fAngle, fMinAngle, fMaxAngle;
	bool bUseCol;

	if ((fAngleStep <= 0.0f) || (fAngleStep > 180.0f))
	{
		return false;
	}

	pVexList = new COGLVertexList();
	if (!pVexList)
	{
		return false;
	}

	COGLVertexList& VexList = *pVexList;

	//fAngleStep = 60.0f;
	float fTexX, fTexY, fTexStepX, fTexStepY;
	int i, iSteps = int(360.0f / fAngleStep) + 1;
	size_t iCircle, iCircleCount = mC.Count();
	size_t nIdx;
	int iPos;

	if (iCircleCount < 2)
	{
		return false;
	}

	if (mCol.Count() == 0)
	{
		bUseCol = false;
	}
	else
	{
		bUseCol = true;
	}

	float fTexMaxX = 1.0f;
	float fTexMaxY = 1.0f;
	float fMaxX    = 1.0f, fMaxY = 1.0f;

	if (m_bUseAbsTexCoords)
	{
		COGLVertex xD;
		fMaxX = 0.0f;
		fMaxY = 0.0f;

		for (nIdx = 0; nIdx < iCircleCount - 1; nIdx++)
		{
			if (float(mR[nIdx]) > fMaxY)
			{
				fMaxY = mR[nIdx];
			}

			xD     = mC[nIdx + 1] - mC[nIdx];
			fMaxX += xD.Mag();
		}

		if (float(mR[nIdx]) > fMaxY)
		{
			fMaxY = mR[nIdx];
		}
	}

	if (m_fTexRepFac != 0.0f)
	{
		fTexMaxX = fMaxX / m_fTexRepFac;
		fTexMaxY = fMaxY / m_fTexRepFac;
	}

	fTexStepY = fTexMaxY / float(iSteps - 1);
	fTexStepX = fTexMaxX / float(iCircleCount - 1);

	mIdxList.Set((iCircleCount - 1) * 2 * iSteps);

	vN = m_E3Base.Vec(mN[0][0], mN[0][1], mN[0][2]);
	if (vN.IsZero(m_fSensitivity))
	{
		return false;
	}

	vNPrev     = vN;
	vU         = vN & m_E3Base.vI;
	fAngleStep = (fAngleStep * m_fPi) / 180.0f;
	vR         = m_E3Base.Rotor(vU, fAngleStep, false);

	FactorBlade<float>(vU, mvList, true, 0, 0, m_fSensitivity);
	if (mvList.Count() == 0)
	{
		return false;
	}

	vB = mvList[0];
	xC = mC[0];
	fR = mR[0];
	vX = fR & vB;

	VexList.Reset();
	VexList.SetMode(GL_TRIANGLE_STRIP);
	VexList.Reserve(int(iCircleCount) * iSteps);

	fTexX = 0.0f;
	for (i = 0, fTexY = 1; i < iSteps; i++, fTexY -= fTexStepY)
	{
		if (m_bUseLighting)
		{
			vXN = vX / float(sqrt(Scalar(vX * vX)));
			VexList.AddNormal(vXN[1], vXN[2], vXN[3]);
		}
		if (bUseCol)
		{
			VexList.AddCol(mCol[0]);
		}

		VexList.AddVex(vX[1] + xC[0], vX[2] + xC[1], vX[3] + xC[2]);
		VexList.AddTex(fTexX, fTexY, 0);

		vX = (vR & vX) & (~vR);
	}

	nIdx      = iPos = 0;
	fMinAngle = -m_fPi / 1800.0f;
	fMaxAngle =  m_fPi / 1800.0f;
	fTexX     = fTexStepX;

	for (iCircle = 1; iCircle < iCircleCount; iCircle++, fTexX += fTexStepX)
	{
		const COGLColor& rCol = mCol[iCircle];

		xC = mC[iCircle];
		vN = m_E3Base.Vec(mN[iCircle][0], mN[iCircle][1], mN[iCircle][2]);
		if (vN.IsZero(m_fSensitivity))
		{
			return false;
		}

		vU = vN & m_E3Base.vI;
		vR = m_E3Base.Rotor(vU, fAngleStep, false);

		fAngle = acos(Scalar(vNPrev * vN));
		if ((fAngle <= fMinAngle) || (fAngle >= fMaxAngle))
		{
			vR12 = m_E3Base.Rotor(vNPrev ^ vN, fAngle, false);
			vB   = vR12 & (vB & (~vR12));
		}

		vX = mR[iCircle] & vB;

		for (i = 0, fTexY = 1; i < iSteps; i++, fTexY -= fTexStepY)
		{
			if (m_bUseLighting)
			{
				vXN = vX / float(sqrt(Scalar(vX * vX)));
				VexList.AddNormal(vXN[1], vXN[2], vXN[3]);
			}
			if (bUseCol)
			{
				VexList.AddCol(rCol);
			}

			VexList.AddVex(vX[1] + xC[0], vX[2] + xC[1], vX[3] + xC[2]);
			VexList.AddTex(fTexX, fTexY, 0);

			mIdxList[nIdx++] = iPos;
			mIdxList[nIdx++] = iPos + iSteps;
			iPos++;

			vX = (vR & vX) & (~vR);
		}

		vNPrev = vN;
	}

	VexList.SetName("CircleSurface");
	VexList.SetIdxList(mIdxList);
	VexList.SetPolygonOffsetFill(1.0f, 1.0f);
	refScene = m_pSceneRep->New(&VexList);

	if (bDoDraw)
	{
		m_pScene->Add(refScene);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Ellipse Surface
///
/// Draw surface spanned by a set of ellipses
/// mC: List of ellipse centers
/// mX: List of first axes
/// mY: List of second axes
/// mCol: List of colors for circles
/// fAngleStep: angle step for drawing ellipses

bool COGLDrawBase::DrawEllipseSurface(const Mem<COGLVertex>& mC,
		const Mem<COGLVertex>& mX,
		const Mem<COGLVertex>& mY,
		const MemObj<COGLColor>& mCol,
		COGLBEReference& refScene,
		float fAngleStep,
		bool bDoDraw)
{
	Mem<unsigned> mIdxList;
	MemObj<MultiV<float> > mvList;
	MultiV<float> vU, vX, vR, vN, vNPrev, vB, vR12, vXN;
	COGLVertex xC, xX, xY, xVex;
	COGLVertexList* pVexList = 0;
	float fAngle;
	bool bUseCol;

	if ((fAngleStep <= 0.0f) || (fAngleStep > 180.0f))
	{
		return false;
	}

	pVexList = new COGLVertexList();
	if (!pVexList)
	{
		return false;
	}

	COGLVertexList& VexList = *pVexList;

	//fAngleStep = 60.0f;
	float fTexX, fTexY, fTexStepX, fTexStepY;
	int i, iSteps = int(360.0f / fAngleStep) + 1;
	size_t iCircle, iCircleCount = mC.Count();
	size_t nIdx;
	int iPos;

	if (iCircleCount < 2)
	{
		return false;
	}

	if (mCol.Count() == 0)
	{
		bUseCol = false;
	}
	else
	{
		bUseCol = true;
	}

	float fTexMaxX = 1.0f;
	float fTexMaxY = 1.0f;
	float fMaxX    = 1.0f, fMaxY = 1.0f;

	if (m_bUseAbsTexCoords)
	{
		COGLVertex xD;
		float fX, fY, fR;
		fMaxX = 0.0f;
		fMaxY = 0.0f;

		for (nIdx = 0; nIdx < iCircleCount - 1; nIdx++)
		{
			fX = mX[nIdx].Mag();
			fY = mY[nIdx].Mag();
			if (fX > fY)
			{
				fR = fX;
			}
			else
			{
				fR = fY;
			}

			if (fR > fMaxY)
			{
				fMaxY = fR;
			}

			xD     = mC[nIdx + 1] - mC[nIdx];
			fMaxX += xD.Mag();
		}

		fX = mX[nIdx].Mag();
		fY = mY[nIdx].Mag();
		if (fX > fY)
		{
			fR = fX;
		}
		else
		{
			fR = fY;
		}

		if (fR > fMaxY)
		{
			fMaxY = fR;
		}
	}

	if (m_fTexRepFac != 0.0f)
	{
		fTexMaxX = fMaxX / m_fTexRepFac;
		fTexMaxY = fMaxY / m_fTexRepFac;
	}

	fTexStepY = fTexMaxY / float(iSteps - 1);
	fTexStepX = fTexMaxX / float(iCircleCount - 1);

	mIdxList.Set((iCircleCount - 1) * 2 * iSteps);

	//	vEX = m_E3Base.Vec(mX[0][0], mX[0][1], mX[0][2]);
	//	vEY = m_E3Base.Vec(mY[0][0], mY[0][1], mY[0][2]);

	fAngleStep = (fAngleStep * m_fPi) / 180.0f;

	xC = mC[0];
	xX = mX[0];
	xY = mY[0];

	VexList.Reset();
	VexList.SetMode(GL_TRIANGLE_STRIP);
	VexList.Reserve(iCircleCount * iSteps);

	fTexX  = 0.0f;
	fAngle = 0.0f;
	for (i = 0, fTexY = 1; i < iSteps; i++, fTexY -= fTexStepY, fAngle += fAngleStep)
	{
		xVex = cos(fAngle) * xX + sin(fAngle) * xY;

		if (m_bUseLighting)
		{
			VexList.AddNormal(xVex / xVex.Mag());
		}
		if (bUseCol)
		{
			VexList.AddCol(mCol[0]);
		}

		VexList.AddVex(xVex + xC);
		VexList.AddTex(fTexX, fTexY, 0);
	}

	nIdx  = iPos = 0;
	fTexX = fTexStepX;

	for (iCircle = 1; iCircle < iCircleCount; iCircle++, fTexX += fTexStepX)
	{
		xC = mC[iCircle];
		xX = mX[iCircle];
		xY = mY[iCircle];

		fAngle = 0.0f;
		for (i = 0, fTexY = 1; i < iSteps; i++, fTexY -= fTexStepY, fAngle += fAngleStep)
		{
			xVex = cos(fAngle) * xX + sin(fAngle) * xY;

			if (m_bUseLighting)
			{
				VexList.AddNormal(xVex / xVex.Mag());
			}
			if (bUseCol)
			{
				VexList.AddCol(mCol[0]);
			}

			VexList.AddVex(xVex + xC);
			VexList.AddTex(fTexX, fTexY, 0);

			mIdxList[nIdx++] = iPos;
			mIdxList[nIdx++] = iPos + iSteps;
			iPos++;
		}
	}

	VexList.SetName("EllipseSurface");
	VexList.SetIdxList(mIdxList);
	VexList.SetPolygonOffsetFill(1.0f, 1.0f);
	refScene = m_pSceneRep->New(&VexList);

	if (bDoDraw)
	{
		m_pScene->Add(refScene);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Line Surface
///
/// Draw surface spanned by a set of lines
/// mC: List of line centers
/// mD: List of line directions
/// mL: List of line lengths
/// mCol: List of colors for lines
/// iSteps: number of separations for drawing lines

bool COGLDrawBase::DrawLineSurface(const Mem<COGLVertex>& mC,
		const Mem<COGLVertex>& mD,
		const Mem<float>& mL,
		const MemObj<COGLColor>& mCol,
		COGLBEReference& refScene,
		int iSteps,
		bool bDoDraw)
{
	Mem<unsigned> mIdxList;
	MemObj<MultiV<float> > mvList;
	MultiV<float> vX, vD[2], vN, vNPrev, vC[3], vU;
	COGLVertex xC, xD, xX, xN;
	COGLVertexList* pVexList = 0;
	float fL, fPosStep, fMinPos;
	bool bUseCol, bClosedSurface;

	if ((iSteps <= 1) || (iSteps > 1000))
	{
		return false;
	}

	pVexList = new COGLVertexList();
	if (!pVexList)
	{
		return false;
	}

	COGLVertexList& VexList = *pVexList;

	int i;
	size_t iLine, iNextLine, iLineCount = mC.Count();
	size_t iIdx, iPos;
	float fTexX, fTexY, fTexStepX, fTexStepY;
	float fTexMaxX = 1.0f;
	float fTexMaxY = 1.0f;
	float fMaxX    = 1.0f, fMaxY = 1.0f;

	if (m_bUseAbsTexCoords)
	{
		COGLVertex xD;
		fMaxX = 0.0f;
		fMaxY = 0.0f;

		for (iIdx = 0; iIdx < iLineCount - 1; iIdx++)
		{
			if (float(mL[iIdx]) > fMaxX)
			{
				fMaxX = mL[iIdx];
			}

			xD     = mC[iIdx + 1] - mC[iIdx];
			fMaxY += xD.Mag();
		}

		if (float(mL[iIdx]) > fMaxX)
		{
			fMaxX = mL[iIdx];
		}
	}

	if (m_fTexRepFac != 0.0f)
	{
		fTexMaxX = fMaxX / m_fTexRepFac;
		fTexMaxY = fMaxY / m_fTexRepFac;
	}

	fTexStepX = fTexMaxX / float(iSteps - 1);
	fTexStepY = fTexMaxY / float(iLineCount - 1);

	if (iLineCount < 2)
	{
		return false;
	}

	if (mCol.Count() == 0)
	{
		bUseCol = false;
	}
	else
	{
		bUseCol = true;
	}

	if ((mC[0] == mC[iLineCount - 1]) && (mD[0] == mD[iLineCount - 1]))
	{
		bClosedSurface = true;
	}
	else
	{
		bClosedSurface = false;
	}

	mIdxList.Set((iLineCount - 1) * 2 * iSteps + 4 * (iLineCount - 2));

	vC[0] = m_E3Base.Vec(mC[0][0], mC[0][1], mC[0][2]);
	vC[1] = m_E3Base.Vec(mC[1][0], mC[1][1], mC[1][2]);
	vD[0] = m_E3Base.Vec(mD[0][0], mD[0][1], mD[0][2]);

	vN = -*(vD[0] ^ (vC[1] - vC[0]));

	vN     = vN / float(sqrt(Scalar(vN * vN)));
	vNPrev = vN;

	xC       = mC[0];
	fL       = mL[0];
	fPosStep = fL / float(iSteps - 1);
	fMinPos  = -fL * 0.5f;

	xD = mD[0];
	xX = xC - 0.5f * fL * xD;
	xD = fPosStep * xD;

	VexList.Reset();
	VexList.SetMode(GL_TRIANGLE_STRIP);
	VexList.Reserve(int(iLineCount) * iSteps);

	fTexX = 0;
	fTexY = 0;
	xN.Set(vN[1], vN[2], vN[3]);
	for (i = 0; i < iSteps; i++, fTexX += fTexStepX)
	{
		if (m_bUseLighting)
		{
			VexList.AddNormal(xN);
		}
		if (bUseCol)
		{
			VexList.AddCol(mCol[0]);
		}

		VexList.AddVex(xX);
		VexList.AddTex(fTexX, fTexY, 0);

		xX = xX + xD;
	}

	iIdx = iPos = 0;

	for (iLine = 1, fTexY = fTexStepY; iLine < iLineCount; iLine++, fTexY += fTexStepY)
	{
		const COGLColor& rCol = mCol[iLine];

		xC = mC[iLine];

		if ((iLine < iLineCount - 1) || bClosedSurface)
		{
			if (bClosedSurface)
			{
				iNextLine = 0;
			}
			else
			{
				iNextLine = iLine + 1;
			}

			vC[1] = m_E3Base.Vec(mC[iLine][0], mC[iLine][1], mC[iLine][2]);
			vC[2] = m_E3Base.Vec(mC[iNextLine][0], mC[iNextLine][1], mC[iNextLine][2]);

			vD[1] = m_E3Base.Vec(mD[iLine][0], mD[iLine][1], mD[iLine][2]);

			vU = *((vC[1] - vC[0]) ^ (vC[2] - vC[1]));
			if (Scalar(vD[0] * vU) > 0.0)
			{
				vD[0] = -vD[0];
			}
			if (Scalar(vD[1] * vU) > 0.0)
			{
				vD[1] = -vD[1];
			}

			vNPrev = -*(vD[0]^(vC[1] - vC[0]));
			vN     = -*(vD[1]^(vC[2] - vC[1]));

			vN     = vN / float(sqrt(Scalar(vN * vN)));
			vNPrev = vNPrev / float(sqrt(Scalar(vNPrev * vNPrev)));
			vX     = vN + vNPrev;
			vN     = vX / float(sqrt(Scalar(vX * vX)));
			vNPrev = vN;

			vC[0] = vC[1];
			vC[1] = vC[2];
			vD[0] = vD[1];

			if (iLine == 1)
			{
				VexList[0].xNorm.Set(vN[1], vN[2], vN[3]);
			}

			if (bClosedSurface && (iLine == iLineCount - 1))
			{
				VexList[0].xNorm.Set(vN[1], vN[2], vN[3]);
			}
		}
		else
		{
			vN = vNPrev;
		}

		xC       = mC[iLine];
		fL       = mL[iLine];
		fPosStep = fL / float(iSteps - 1);
		fMinPos  = -fL * 0.5f;

		xD = mD[iLine];

		xX = xC - 0.5f * fL * xD;
		xD = fPosStep * xD;
		xN.Set(vN[1], vN[2], vN[3]);

		for (i = 0, fTexX = 0; i < iSteps; i++, fTexX += fTexStepX)
		{
			if (m_bUseLighting)
			{
				VexList.AddNormal(xN);
			}
			if (bUseCol)
			{
				VexList.AddCol(rCol);
			}

			VexList.AddVex(xX);
			VexList.AddTex(fTexX, fTexY, 0);

			mIdxList[iIdx++] = int(iPos);
			mIdxList[iIdx++] = int(iPos + iSteps);

			iPos++;

			xX = xX + xD;
		}

		if (iLine < iLineCount - 1)
		{
			mIdxList[iIdx++] = int(iPos - 1 + iSteps);
			mIdxList[iIdx++] = int(iPos);
			mIdxList[iIdx++] = int(iPos);
			mIdxList[iIdx++] = int(iPos);
		}
	}

	VexList.SetName("LineSurface");
	VexList.SetIdxList(mIdxList);
	VexList.SetPolygonOffsetFill(1.0f, 1.0f);
	refScene = m_pSceneRep->New(&VexList);

	if (bDoDraw)
	{
		m_pScene->Add(refScene);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Point Surface
///
/// Draw surface spanned by a set of Points
/// Points are ordered left to right, rowwise.
/// iRowCount: Number of rows (bottom to top)
/// iColCount: Number of cols (left to right)
/// mP: List of points
/// mN: List of normals
/// mT: List of texture coordinates
/// mCol: List of colors
/// fNormScale: Scale of normals drawn.
/// bDoDraw: Draw directly or not.

bool COGLDrawBase::DrawPointSurface(int iRowCount,
		int iColCount,
		const Mem<COGLVertex>& mP,
		const Mem<COGLVertex>& mN,
		const Mem<COGLVertex>& mT,
		const MemObj<COGLColor>& mCol,
		float fNormScale,
		COGLBEReference& refSurface,
		COGLBEReference& refNormals,
		bool bDoDraw)
{
	COGLVertexList* pVexListSurf = new COGLVertexList;
	if (!pVexListSurf)
	{
		return false;
	}

	COGLVertexList* pVexListNorm = 0;

	if (fNormScale != 0.0f)
	{
		pVexListNorm = new COGLVertexList;
		if (!pVexListNorm)
		{
			delete pVexListSurf;
			return false;
		}
	}

	if (!GenVexPointSurface(iRowCount, iColCount,
			    mP, mN, mT, mCol, fNormScale,
			    pVexListSurf, pVexListNorm))
	{
		delete pVexListSurf;

		if (pVexListNorm)
		{
			delete pVexListNorm;
		}

		return false;
	}

	pVexListSurf->SetName("Surface");
	pVexListSurf->SetPolygonOffsetFill(1.0f, 1.0f);
	refSurface = m_pSceneRep->New(pVexListSurf);

	if (pVexListNorm)
	{
		pVexListNorm->SetName("Normals");
		refNormals = m_pSceneRep->New(pVexListNorm);
	}

	if (bDoDraw)
	{
		if (pVexListNorm)
		{
			m_pScene->Add(refNormals);
		}

		m_pScene->Add(refSurface);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Generate VertexList of Point Surface
///
/// Draw surface spanned by a set of Points
/// Points are ordered left to right, rowwise.
/// iRowCount: Number of rows (bottom to top)
/// iColCount: Number of cols (left to right)
/// mP: List of points
/// mN: List of normals
/// mT: List of texture coordinates
/// mCol: List of colors
/// fNormScale: Scale of normals drawn.
///	pVexListSurf: Vertex list of surface
/// pVexListNorm: Vertex list of normals

bool COGLDrawBase::GenVexPointSurface(int iRowCount,
		int iColCount,
		const Mem<COGLVertex>& mP,
		const Mem<COGLVertex>& mN,
		const Mem<COGLVertex>& mT,
		const MemObj<COGLColor>& mCol,
		float fNormScale,
		COGLVertexList* pVexListSurf,
		COGLVertexList* pVexListNorm)
{
	if (!pVexListSurf || ((fNormScale != 0.0f) && !pVexListNorm))
	{
		return false;
	}

	Mem<unsigned> mIdxList;
	bool bUseCol, bUseNormals, bUseTex, bDrawNormLines;

	if (fNormScale != 0.0f)
	{
		bDrawNormLines = true;
	}
	else
	{
		bDrawNormLines = false;
	}

	COGLVertexList& rVexListSurf = *pVexListSurf;
	COGLVertexList& rVexListNorm = *pVexListNorm;

	int iPointCount = (int) mP.Count();

	if ((iRowCount * iColCount > iPointCount) ||
	    (iRowCount < 2) || (iColCount < 2))
	{
		return false;
	}

	if (mCol.Count() == 0)
	{
		bUseCol = false;
	}
	else
	{
		bUseCol = true;
	}

	if (mN.Count() == 0)
	{
		bUseNormals = false;
	}
	else
	{
		bUseNormals = true;
	}

	if (mT.Count() == 0)
	{
		bUseTex = false;
	}
	else
	{
		bUseTex = true;
	}

	int iRow, iCol;
	int iIdx, iPos;
	int iColPos, iRowPos;
	int iPosL, iPosR, iPosT, iPosB;
	COGLVertex dX, dY, xN;

	iColPos = 0;
	iRowPos = 0;

	if (bDrawNormLines)
	{
		rVexListNorm.Reset();
		rVexListNorm.SetMode(GL_LINES);
	}

	rVexListSurf.Reset();
	rVexListSurf.SetMode(GL_TRIANGLE_STRIP);

	bool bResult = rVexListSurf.AddVexRange(mP);
	if (bResult == false)
	{
		throw CLU_EXCEPTION("cannot add vertices (out of memory)");
	}

	Mem<unsigned> mPartId(iPointCount);
	unsigned* puPartId = mPartId.Data();
	for (unsigned i = 0; i < unsigned(iPointCount); ++i, ++puPartId)
	{
		*puPartId = i;
	}

	rVexListSurf.AddPartIdRange(mPartId);

	if (bUseTex)
	{
		rVexListSurf.AddTexRange(mT);
	}

	if (bUseCol)
	{
		rVexListSurf.AddColRange(mCol);
	}

	if (m_bUseLighting)
	{
		if (bUseNormals)
		{
			rVexListSurf.AddNormalRange(mN);

			if (bDrawNormLines)
			{
				for (iPos = 0; iPos < iPointCount; ++iPos)
				{
					rVexListNorm.AddVex(mP[iPos]);
					rVexListNorm.AddVex(mP[iPos] + fNormScale * mN[iPos]);
				}
			}
		}
		else
		{
			iColPos = 0;
			iRowPos = 0;

			for (iPos = 0; iPos < iPointCount; ++iPos)
			{
				if ((iPosL = iColPos - 1) < 0)
				{
					iPosL = 0;
				}
				if ((iPosR = iColPos + 1) >= iColCount)
				{
					iPosR = iColCount - 1;
				}

				if ((iPosT = iRowPos + 1) >= iRowCount)
				{
					iPosT = iRowCount - 1;
				}
				if ((iPosB = iRowPos - 1) < 0)
				{
					iPosB = 0;
				}

				iPosL += iRowPos * iColCount;
				iPosR += iRowPos * iColCount;
				iPosT  = iColPos + iPosT * iColCount;
				iPosB  = iColPos + iPosB * iColCount;

				dX = 0.5f * (mP[iPosR] - mP[iPosL]);
				dY = 0.5f * (mP[iPosT] - mP[iPosB]);

				xN[0] = dX[1] * dY[2] - dX[2] * dY[1];
				xN[1] = dX[2] * dY[0] - dX[0] * dY[2];
				xN[2] = dX[0] * dY[1] - dX[1] * dY[0];

				xN.Norm();
				rVexListSurf.AddNormal(xN);

				if (bDrawNormLines)
				{
					rVexListNorm.AddVex(mP[iPos]);
					rVexListNorm.AddVex(mP[iPos] + fNormScale * xN);
				}

				if (++iColPos >= iColCount)
				{
					iColPos = 0;
					iRowPos++;
				}
			}
		}
	}

	mIdxList.Set((iRowCount - 1) * 2 * iColCount + (iRowCount - 2));

	// If no texture coordinates are given, then calculate some.
	if (!bUseTex)
	{
		float fTexRow, fTexCol, fTexRowStep, fTexColStep;
		float fTexMaxX = 1.0f;
		float fTexMaxY = 1.0f;
		float fMaxX    = 1.0f, fMaxY = 1.0f;

		if (m_bUseAbsTexCoords)
		{
			COGLVertex xD;
			fMaxX = 0.0f;
			fMaxY = 0.0f;

			for (iCol = 0; iCol < iColCount - 1; iCol++)
			{
				xD     = mP[iCol + 1] - mP[iCol];
				fMaxX += xD.Mag();
			}

			for (iRow = 0, iPos = 0; iRow < iRowCount - 1; iRow++, iPos += iColCount)
			{
				xD     = mP[iPos + iColCount] - mP[iPos];
				fMaxY += xD.Mag();
			}
		}

		if (m_fTexRepFac != 0.0f)
		{
			fTexMaxX = fMaxX / m_fTexRepFac;
			fTexMaxY = fMaxY / m_fTexRepFac;
		}

		fTexRowStep = fTexMaxX / float(iRowCount - 1);
		fTexColStep = fTexMaxY / float(iColCount - 1);

		for (iRow = 0, fTexRow = 0; iRow < iRowCount; iRow++, fTexRow += fTexRowStep)
		{
			for (iCol = 0, fTexCol = 0; iCol < iColCount; iCol++, iPos++, fTexCol += fTexColStep)
			{
				rVexListSurf.AddTex(fTexCol, fTexRow, 0);
			}
		}
	}

	iIdx = iPos = 0;
	for (iRow = 0; iRow < iRowCount - 1; iRow++)
	{
		for (iCol = 0; iCol < iColCount; iCol++, iPos++)
		{
			mIdxList[iIdx++] = iPos;
			mIdxList[iIdx++] = iPos + iColCount;
		}

		if (++iRow >= iRowCount - 1)
		{
			break;
		}

		iPos            += iColCount - 1;
		mIdxList[iIdx++] = iPos;

		for (iCol = iColCount; iCol > 0; iCol--, iPos--)
		{
			mIdxList[iIdx++] = iPos;
			mIdxList[iIdx++] = iPos + iColCount;
		}

		if (iRow + 1 >= iRowCount - 1)
		{
			break;
		}

		iPos            += iColCount + 1;
		mIdxList[iIdx++] = iPos;
	}

	// Set front face to clock wise due to indexing
	rVexListSurf.SetFrontFace(GL_CW);
	rVexListSurf.SetIdxList(mIdxList);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Point Grid
///
/// Draw grid spanned by a set of Points
/// Points are ordered left to right, rowwise.
/// iRowCount: Number of rows (bottom to top)
/// iColCount: Number of cols (left to right)
/// mP: List of points
/// mN: List of normals
/// mCol: List of colors

bool COGLDrawBase::DrawPointGrid(int iRowCount,
		int iColCount,
		const Mem<COGLVertex>& mP,
		const Mem<COGLVertex>& mN,
		const MemObj<COGLColor>& mCol,
		COGLBEReference& refScene,
		bool bNegateNormals,
		bool bDoDraw)
{
	Mem<unsigned> mIdxList;
	COGLVertexList* pVexList = 0;
	bool bUseCol, bUseNormals;

	pVexList = new COGLVertexList;

	if (!pVexList)
	{
		return false;
	}

	refScene = m_pSceneRep->New(pVexList);
	COGLVertexList& VexList = *pVexList;

	int iPointCount = (int) mP.Count();

	if ((iRowCount * iColCount > iPointCount) ||
	    (iRowCount < 2) || (iColCount < 2))
	{
		return false;
	}

	if (mCol.Count() == 0)
	{
		bUseCol = false;
	}
	else
	{
		bUseCol = true;
	}

	if (mN.Count() == 0)
	{
		bUseNormals = false;
	}
	else
	{
		bUseNormals = true;
	}

	int iRow, iCol;
	int iIdx, iPos;
	int iColPos, iRowPos;
	int iPosL, iPosR, iPosT, iPosB;
	COGLVertex dX, dY, xN;

	iColPos = 0;
	iRowPos = 0;

	VexList.Reset();
	VexList.SetMode(GL_LINES);
	VexList.Reserve(iPointCount);

	for (iPos = 0; iPos < iPointCount; iPos++)
	{
		VexList.AddVex(mP[iPos]);

		if (m_bUseLighting)
		{
			if (bUseNormals)
			{
				if (bNegateNormals)
				{
					VexList.AddNormal(-mN[iPos]);
				}
				else
				{
					VexList.AddNormal(mN[iPos]);
				}
			}
			else
			{
				if ((iPosL = iColPos - 1) < 0)
				{
					iPosL = 0;
				}
				if ((iPosR = iColPos + 1) >= iColCount)
				{
					iPosR = iColCount - 1;
				}

				if ((iPosT = iRowPos + 1) >= iRowCount)
				{
					iPosT = iRowCount - 1;
				}
				if ((iPosB = iRowPos - 1) < 0)
				{
					iPosB = 0;
				}

				iPosL += iRowPos * iColCount;
				iPosR += iRowPos * iColCount;
				iPosT  = iColPos + iPosT * iColCount;
				iPosB  = iColPos + iPosT * iColCount;

				dX = 0.5f * (mP[iPosR] - mP[iPosL]);
				dY = 0.5f * (mP[iPosT] - mP[iPosB]);

				xN[0] = dX[1] * dY[2] - dX[2] * dY[1];
				xN[1] = dX[2] * dY[0] - dX[0] * dY[2];
				xN[2] = dX[0] * dY[1] - dX[1] * dY[0];

				xN.Norm();
				if (bNegateNormals)
				{
					VexList.AddNormal(-xN);
				}
				else
				{
					VexList.AddNormal(xN);
				}
			}
		}

		if (bUseCol)
		{
			VexList.AddCol(mCol[iPos]);
		}

		if (++iColPos >= iColCount)
		{
			iColPos = 0;
			iRowPos++;
		}
	}

	mIdxList.Set((iColCount - 1) * ((iRowCount - 1) *  4 + 4));

	iIdx = iPos = 0;
	for (iRow = 0; iRow < iRowCount; iRow++)
	{
		if (iRow < iRowCount - 1)
		{
			for (iCol = 0; iCol < iColCount - 1; iCol++, iPos++)
			{
				mIdxList[iIdx++] = iPos;
				mIdxList[iIdx++] = iPos + iColCount;

				mIdxList[iIdx++] = iPos;
				mIdxList[iIdx++] = iPos + 1;
			}
		}
		else
		{
			for (iCol = 0; iCol < iColCount - 1; iCol++, iPos++)
			{
				mIdxList[iIdx++] = iPos;
				mIdxList[iIdx++] = iPos + 1;
			}

			break;
		}

		++iRow;
		mIdxList[iIdx++] = iPos;
		iPos            += iColCount;
		mIdxList[iIdx++] = iPos;

		if (iRow < iRowCount - 1)
		{
			for (iCol = iColCount; iCol > 1; iCol--, iPos--)
			{
				mIdxList[iIdx++] = iPos;
				mIdxList[iIdx++] = iPos + iColCount;

				mIdxList[iIdx++] = iPos;
				mIdxList[iIdx++] = iPos - 1;
			}
		}
		else
		{
			for (iCol = iColCount; iCol > 1; iCol--, iPos--)
			{
				mIdxList[iIdx++] = iPos;
				mIdxList[iIdx++] = iPos - 1;
			}

			break;
		}

		mIdxList[iIdx++] = iPos;
		iPos            += iColCount;
		mIdxList[iIdx++] = iPos;
	}

	VexList.SetIdxList(mIdxList);
	VexList.SetName("PointGrid");
	refScene = m_pSceneRep->New(&VexList);

	if (bDoDraw)
	{
		m_pScene->Add(refScene);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Point List (as line)
///
/// Draw line spanned by a set of Points
/// Points are ordered left to right
/// mP: List of points
/// mN: List of normals
/// mCol: List of colors

bool COGLDrawBase::DrawPointList(const Mem<COGLVertex>& mP, const Mem<COGLVertex>& mN, const MemObj<COGLColor>& mCol, COGLBEReference& refScene, bool bNegateNormals, bool bDoDraw)
{
	Mem<unsigned> mIdxList;
	COGLVertexList* pVexList = 0;
	bool bUseCol, bUseNormals;

	pVexList = new COGLVertexList;

	if (!pVexList)
	{
		return false;
	}

	refScene = m_pSceneRep->New(pVexList);
	COGLVertexList& VexList = *pVexList;

	int iPointCount = (int) mP.Count();

	if (mCol.Count() == 0)
	{
		bUseCol = false;
	}
	else
	{
		bUseCol = true;
	}

	if (mN.Count() == 0)
	{
		bUseNormals = false;
	}
	else
	{
		bUseNormals = true;
	}

	int iPos;
	COGLVertex dA, dB, xN;

	VexList.Reset();
	VexList.SetMode(GL_LINE_STRIP);
	VexList.Reserve(iPointCount);

	for (iPos = 0; iPos < iPointCount; iPos++)
	{
		VexList.AddVex(mP[iPos]);

		if (m_bUseLighting)
		{
			if (bUseNormals)
			{
				if (bNegateNormals)
				{
					VexList.AddNormal(-mN[iPos]);
				}
				else
				{
					VexList.AddNormal(mN[iPos]);
				}
			}
			else
			{
				if ((iPos > 0) && (iPos < iPointCount - 1))
				{
					dA = mP[iPos] - mP[iPos - 1];
					dB = mP[iPos + 1] - mP[iPos];

					xN[0] = dA[1] * dB[2] - dA[2] * dB[1];
					xN[1] = dA[2] * dB[0] - dA[0] * dB[2];
					xN[2] = dA[0] * dB[1] - dA[1] * dB[0];

					xN.Norm();

					if ((iPos == 1) || (iPos == iPointCount - 1))
					{
						if (bNegateNormals)
						{
							VexList.AddNormal(-xN);
						}
						else
						{
							VexList.AddNormal(xN);
						}
					}
					if (bNegateNormals)
					{
						VexList.AddNormal(-xN);
					}
					else
					{
						VexList.AddNormal(xN);
					}
				}
			}
		}

		if (bUseCol)
		{
			VexList.AddCol(mCol[iPos]);
		}
	}

	VexList.SetName("PointList");

	if (bDoDraw)
	{
		m_pScene->Add(refScene);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Disk

bool COGLDrawBase::DrawDisk(const COGLVertex& xC,       const COGLVertex& xA, const COGLVertex& xB, float fR, float fAngleStep, bool bDirected)
{
	COGLColor Dark;
	COGLVertex xN;
	MemObj<MultiV<float> > mvList;
	MultiV<float> vU, vX, vR, vN;
	Mem<int> mIdx;
	float fAngle = 0.0f;

	if ((fR <= 0.0f) || (fR > 1000.0f) || (fAngleStep <= 0.0f) || (fAngleStep > 180.0f))
	{
		return false;
	}

	COGLVertexList* pVexList = new COGLVertexList;

	if (!pVexList)
	{
		return false;
	}

	COGLVertexList& rVexList = *pVexList;

	int i, iSteps = int(360.0f / fAngleStep);

	mIdx.Set(iSteps + 2);
	//	vU = m_E3Base.Vec(xN[0], xN[1], xN[2]) & m_E3Base.vI;
	vU = m_E3Base.Vec(xA[0], xA[1], xA[2]) ^ m_E3Base.Vec(xB[0], xB[1], xB[2]);

	fAngleStep = (fAngleStep * m_fPi) / 180.0f;
	vR         = m_E3Base.Rotor(vU, fAngleStep, false);

	FactorBlade<float>(vU, mvList, true, 0, 0, m_fSensitivity);

	if (mvList.Count() == 0)
	{
		return false;
	}

	vX = fR & mvList[0];

	rVexList.Reset();
	rVexList.SetMode(GL_TRIANGLE_FAN);
	rVexList.Reserve(iSteps + 2);

	if (m_bUseLighting)
	{
		vN  = *vU;
		vN /= float(sqrt(Scalar(vN * vN)));
		xN  = &vN[m_E3Base.iE1];

		//m_mMaterial[MAT_PLANE].Ambient(m_ActiveColor);
		//m_mMaterial[MAT_PLANE].Diffuse(1.0, 1.0, 1.0, m_ActiveColor.Data()[3]);
		//		m_mMaterial[MAT_PLANE].Apply();

		rVexList.AddNormal(xN);
		rVexList.AddNormal(xN);
	}

	mIdx[0] = 0;
	mIdx[1] = iSteps + 1;
	rVexList.AddVex(xC[0], xC[1], xC[2]);
	rVexList.AddVex(vX[1] + xC[0], vX[2] + xC[1], vX[3] + xC[2]);

	float fTexMaxR = 0.5f;
	float fMaxR    = 0.5f;

	if (m_bUseAbsTexCoords)
	{
		fMaxR = fR;
	}

	if (m_fTexRepFac != 0.0f)
	{
		fTexMaxR = fMaxR / m_fTexRepFac;
	}

	rVexList.AddTex(0.5f, 0.5f, 0);
	rVexList.AddTex(0.5f + fTexMaxR, 0.5f, 0);

	for (i = 0; i < iSteps; i++)
	{
		fAngle += fAngleStep;
		vX      = (vR & vX) & (~vR);
		rVexList.AddVex(vX[1] + xC[0], vX[2] + xC[1], vX[3] + xC[2]);
		rVexList.AddTex(0.5f + fTexMaxR * cos(fAngle), 0.5f + fTexMaxR * sin(fAngle), 0);

		if (m_bUseLighting)
		{
			rVexList.AddNormal(xN);
		}

		mIdx[i + 2] = iSteps - i;
	}

	pVexList->SetName("Disk");

	if (bDirected)
	{
		pVexList->EnableScaleBackColor(true);
		pVexList->SetBackColorScale(m_fBackFaceColFac);
	}
	m_pScene->Add(m_pSceneRep->New(pVexList));

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Line

bool COGLDrawBase::DrawLine(const COGLVertex& xP, const COGLVertex& xD, bool bEndPoints, int iStipple, int iStippleFactor, MemObj<COGLBEReference>* pmScene)
{
	if (pmScene)
	{
		pmScene->Set(0);
	}

	if (m_eDrawLineType == LineAsCylinder)
	{
		float fAngleStep = float(360.0f / float(m_iCylinderDetailLevel));

		if (bEndPoints)
		{
			return DrawCylinder(xP, xD - xP, m_fLineWidth * m_fLineAsCylinderFac, true, fAngleStep);
		}
		else
		{
			return DrawCylinder(xP - 0.5 * xD, xD, m_fLineWidth * m_fLineAsCylinderFac, true, fAngleStep);
		}
	}

	COGLVertexList* pVexList = new COGLVertexList;

	if (!pVexList)
	{
		return false;
	}

	COGLVertexList& rVexList = *pVexList;

	rVexList.Reset();
	rVexList.SetMode(GL_LINES);

	//COGLVertexList::TVexList &rVex = rVexList.GetVexList();
	//COGLVertexList::TVexList &rNorm = rVexList.GetNormList();

	rVexList.Reserve(2);

	if (bEndPoints)
	{
		if (m_bUseLighting)
		{
			rVexList[0].xVex = xP;
			rVexList[1].xVex = xD;

			//rNorm[0] = xP - xD;
			//rNorm[1] = xD - xP;
		}
		else
		{
			rVexList[0].xVex = xP;
			rVexList[1].xVex = xD;
		}
	}
	else
	{
		if (m_bUseLighting)
		{
			rVexList[0].xVex = xP - 0.5f * xD;
			rVexList[1].xVex = xP + 0.5f * xD;

			//rNorm[0] = -xD;
			//rNorm[1] = xD;
		}
		else
		{
			rVexList[0].xVex = xP - 0.5f * xD;
			rVexList[1].xVex = xP + 0.5f * xD;
		}
	}

	rVexList.UpdateVexCount();

	if (iStipple != 0)
	{
		pVexList->EnableLineStipple(true);
		pVexList->SetLineStipple(iStippleFactor, iStipple);
	}

	COGLBEReference scLine;

	pVexList->SetName("Line");
	scLine = m_pSceneRep->New(pVexList);
	m_pScene->Add(scLine);

	if (pmScene)
	{
		pmScene->Set(1);
		(*pmScene)[0] = scLine;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Vector
///
/// fTipSize: length of tip
/// fLtoW: Ratio of length to width

bool COGLDrawBase::DrawVector(const COGLVertex& xA, const COGLVertex& xB, int iStipple, int iStippleFactor)
{
	COGLVertex xDir;
	float fArrowHeight;
	MemObj<COGLColor> mColor;

	xDir = xA - xB;
	xDir.Norm();

	fArrowHeight = m_fArrowLength * float(tan(double(m_fArrowAngle)));

	DrawCone(xB, m_fArrowLength * xDir, fArrowHeight, mColor, 16, true);
	DrawCylinder(xA, (xB - xA) + m_fArrowLength * xDir, m_fArrowLineRadius, true, 20.0f);

	//	DrawLine( xA, xB + m_fArrowLength * xDir, true, iStipple, iStippleFactor );

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Cone
///
/// xP: Origin
/// xD: Direction and length of cone
/// fR: Radius of cone at end of direction vector

bool COGLDrawBase::DrawCone(const COGLVertex& xP, const COGLVertex& xD, float fR, MemObj<COGLColor>& mColor, int iStepCnt, bool bClosed, MemObj<COGLBEReference>* pmScene)
{
	bool bUseCol = false;

	if (pmScene)
	{
		pmScene->Set(0);
	}

	if (iStepCnt <= 2)
	{
		return false;
	}

	if (mColor.Count() >= 3)
	{
		bUseCol = true;
	}

	++iStepCnt;

	COGLVertexList* pVexListFan = new COGLVertexList;
	if (!pVexListFan)
	{
		return false;
	}

	COGLVertexList* pVexListBack = 0;

	if (bClosed)
	{
		pVexListBack = new COGLVertexList;
		if (!pVexListBack)
		{
			delete pVexListFan;
			return false;
		}
	}

	int iStep;
	double dAngle, dAngleStep, dArrowHeight, dArrowLen, dArrowAngle;
	double dCos, dSin;
	float fRotAngle;
	COGLVertex xDir, xAxis;
	COGLColor colStd;

	// Draw Fan
	{
		COGLVertexList& rVexList = *pVexListFan;

		rVexList.Reset();
		rVexList.SetMode(GL_TRIANGLE_FAN);

		//COGLVertexList::TVexList &mVex = rVexList.GetVexList();
		//COGLVertexList::TVexList &mNorm = rVexList.GetNormList();
		//COGLVertexList::TVexList &mTex = rVexList.GetTexList();
		//COGLVertexList::TColList &mCol = rVexList.GetColList();

		rVexList.Reserve(iStepCnt + 1);
		rVexList.UpdateVexCount();
		rVexList.UpdateTexCount();
		rVexList.UpdateNormCount();

		//mVex.Set( iStepCnt + 1);
		//mTex.Set( iStepCnt + 1);
		//mNorm.Set( iStepCnt + 1);

		dAngleStep   = 2.0f * double(m_fPi) / double(iStepCnt - 1);
		dArrowLen    = double(xD.Mag());
		dArrowHeight = double(fR);
		dArrowAngle  = atan(dArrowHeight / dArrowLen);

		rVexList[0].xVex.Set(0.0f, 0.0f, 0.0f);
		rVexList[0].xNorm.Set(0.0f, 0.0f, 1.0f);
		rVexList[0].xTex.Set(0.5f, 0.5f, 0.0f);

		if (bUseCol)
		{
			rVexList.UpdateColCount();
			colStd           = mColor[1];
			rVexList[0].xCol = mColor[0].Data();
		}

		for (iStep = 0, dAngle = 0.0f; iStep < iStepCnt; ++iStep, dAngle += dAngleStep)
		{
			rVexList[iStep + 1].xVex.Set(float(dArrowHeight * (dCos = cos(dAngle))),
					float(dArrowHeight * (dSin = sin(dAngle))),
					float(-dArrowLen));

			rVexList[iStep + 1].xNorm.Set(float(dCos * cos(dArrowAngle)),
					float(dSin * cos(dArrowAngle)),
					float(sin(dArrowAngle)));

			rVexList[iStep + 1].xTex.Set(float((dCos + 1.0) / 2.0),
					float((dSin + 1.0) / 2.0),
					0);

			if (bUseCol)
			{
				rVexList[iStep + 1].xCol = colStd.Data();
			}
		}
	}

	// Draw Back
	if (bClosed)
	{
		COGLVertexList& rVexList = *pVexListBack;
		rVexList = *pVexListFan;

		//COGLVertexList::TVexList &mVex = rVexList.GetVexList();
		//COGLVertexList::TVexList &mNorm = rVexList.GetNormList();
		//COGLVertexList::TColList &mCol = rVexList.GetColList();

		if (bUseCol)
		{
			colStd           = mColor[2];
			rVexList[0].xCol = colStd.Data();
		}

		rVexList[0].xVex[2] = float(-dArrowLen);

		for (iStep = 0; iStep < iStepCnt + 1; ++iStep)
		{
			rVexList[iStep].xNorm.Set(0.0f, 0.0f, -1.0f);

			//if ( bUseCol )
			//	mCol[ iStep ] = colStd.Data();
		}
	}

	// Rotate and Translate Arrow
	xDir = xD;
	xDir.Norm();

	xAxis     = COGLVertex(0.0f, 0.0f, -1.0f) ^ xDir;
	fRotAngle = float(acos(double(-xDir[2]))) * 180.0f / m_fPi;
	//	fRotAngle = float( asin( double( xAxis.Mag() ) ) ) * 180.0f / m_fPi;
	xAxis.Norm();

	pVexListFan->EnableTransform(true);
	pVexListFan->SetRotation(fRotAngle, xAxis);
	pVexListFan->SetTranslation1(xP);
	pVexListFan->SetPolygonOffsetFill(1.0f, 1.0f);

	COGLBEReference scFan, scBack;

	scFan = m_pSceneRep->New(pVexListFan);
	m_pScene->Add(scFan);

	if (bClosed)
	{
		pVexListBack->EnableTransform(true);
		pVexListBack->SetRotation(fRotAngle, xAxis);
		pVexListBack->SetTranslation1(xP);
		pVexListBack->SetPolygonOffsetFill(1.0f, 1.0f);
		pVexListBack->SetFrontFace(GL_CW);

		scBack = m_pSceneRep->New(pVexListBack);
		m_pScene->Add(scBack);

		if (pmScene)
		{
			pmScene->Set(2);
			(*pmScene)[0] = scFan;
			(*pmScene)[1] = scBack;
		}
	}
	else
	{
		if (pmScene)
		{
			pmScene->Set(1);
			(*pmScene)[0] = scFan;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Rotor

bool COGLDrawBase::DrawRotor(const COGLVertex& xP, const COGLVertex& xR, float fAngle, float fDegAngleStep)
{
	COGLVertex xA;
	MultiV<float> vR, vA, vU;
	MemObj<MultiV<float> > mvList;
	float fAngleStep;
	int i, iAngleSteps;

	COGLVertexList* pVexList = new COGLVertexList();
	if (!pVexList)
	{
		return false;
	}

	COGLVertexList& VexList = *pVexList;

	fAngleStep = fDegAngleStep * m_fPi / 180.0f;

	if (!(iAngleSteps = int(fAngle / fAngleStep + 0.5)))
	{
		iAngleSteps = 1;
	}

	fAngleStep = fAngle / float(iAngleSteps);

	vA = m_E3Base.Vec(xR[0], xR[1], xR[2]);
	vR = m_E3Base.Rotor(vA, fAngleStep);

	vU = vA & m_E3Base.vI;
	FactorBlade<float>(vU, mvList, true, 0, 0, m_fSensitivity);
	if (mvList.Count() == 0)
	{
		return false;
	}

	vA = mvList[0];

	DrawLine(xP, xP + xR, true);

	VexList.SetMode(GL_TRIANGLE_FAN);
	VexList.Reserve(iAngleSteps + 2);

	if (m_bUseLighting)
	{
		VexList.AddNormal(xR);
		VexList.AddNormal(xR);
	}

	xA = &vA[1];

	float fTexMaxR = 0.5f;
	float fTexAngle;

	VexList.AddTex(0.5f, 0.5f, 0);
	VexList.AddTex(0.5f + fTexMaxR, 0.5f, 0);

	VexList.AddVex(xP);
	VexList.AddVex(xP + xA);

	for (i = 0, fTexAngle = fAngleStep; i < iAngleSteps; i++, fTexAngle += fAngleStep)
	{
		vA = (vR & vA) & (~vR);
		xA = &vA[1];
		VexList.AddVex(xP + xA);
		VexList.AddTex(0.5f + fTexMaxR * cos(fTexAngle), 0.5f + fTexMaxR * sin(fTexAngle), 0);

		if (m_bUseLighting)
		{
			VexList.AddNormal(xR);
		}
	}

	VexList.SetName("Rotor");
	VexList.SetPolygonOffsetFill(1.0f, 1.0f);
	VexList.EnableScaleBackColor();
	VexList.SetBackColorScale(0.5f);

	m_pScene->Add(m_pSceneRep->New(pVexList));

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Twist
/// xP : offset of rot. axis
/// xT : translation along rot. axis
/// xR : rot. axis

bool COGLDrawBase::DrawTwist(const COGLVertex& xP, const COGLVertex& xT, const COGLVertex& xR, float fAngle, float fDegAngleStep)
{
	COGLVertex xA;
	MultiV<float> vR, vA, vU;
	MemObj<MultiV<float> > mvList;
	float fAngleStep;
	int i, iAngleSteps;

	COGLVertexList* pVexList = new COGLVertexList();
	if (!pVexList)
	{
		return false;
	}

	COGLVertexList& VexList = *pVexList;

	fAngleStep = fDegAngleStep * m_fPi / 180.0f;

	if (!(iAngleSteps = int(fAngle / fAngleStep + 0.5)))
	{
		iAngleSteps = 1;
	}

	fAngleStep = fAngle / float(iAngleSteps);

	vA = m_E3Base.Vec(xR[0], xR[1], xR[2]);
	vR = m_E3Base.Rotor(vA, fAngleStep);

	vU = vA & m_E3Base.vI;
	FactorBlade<float>(vU, mvList, true, 0, 0, m_fSensitivity);
	if (mvList.Count() == 0)
	{
		return false;
	}

	vA = mvList[0];

	DrawLine(xP, xP + xR, true);
	DrawVector(xP, xP + xT);

	VexList.SetMode(GL_TRIANGLE_FAN);
	VexList.Reserve(iAngleSteps + 2);

	if (m_bUseLighting)
	{
		VexList.AddNormal(xR);
		VexList.AddNormal(xR);
	}

	xA = &vA[1];

	float fTexMaxR = 0.5f;
	float fTexAngle;

	VexList.AddTex(0.5f, 0.5f, 0);
	VexList.AddTex(0.5f + fTexMaxR, 0.5f, 0);

	VexList.AddVex(xP);
	VexList.AddVex(xP + xA);

	for (i = 0, fTexAngle = 0; i < iAngleSteps; i++, fTexAngle += fAngleStep)
	{
		vA = (vR & vA) & (~vR);
		xA = &vA[1];
		VexList.AddVex(xP + xA);
		VexList.AddTex(0.5f + fTexMaxR * cos(fTexAngle), 0.5f - fTexMaxR * sin(fTexAngle), 0);

		if (m_bUseLighting)
		{
			VexList.AddNormal(xR);
		}
	}

	VexList.SetName("Rotor");
	VexList.SetPolygonOffsetFill(1.0f, 1.0f);
	VexList.EnableScaleBackColor();
	VexList.SetBackColorScale(0.5f);

	m_pScene->Add(m_pSceneRep->New(pVexList));

	return true;
}

//////////////////////////////////////////////////////////////////////
// Draws an arc.
// xP is offset vector. Arc is drawn from (xA-xP) to (xB-xP) with radius 'fRadius'.
// if bFilled = true then a pie-slice is drawn.

bool COGLDrawBase::DrawArc(const COGLVertex& xP, const COGLVertex& xV1, const COGLVertex& xV2, float fRadius, bool bShort, bool bFilled, float fDegAngleStep)
{
	COGLVertex xA, xB, xN;
	MultiV<float> vR, vrR, vA, vB, vU, vP;
	MemObj<MultiV<float> > mvList;
	float fAngleStep, fAngle;
	int i, iAngleSteps;
	COGLColor Dark;

	xA = xV1 - xP;
	xB = xV2 - xP;
	xA.Norm();
	xB.Norm();
	vP = m_E3Base.Vec(xP[0], xP[1], xP[2]);
	vA = m_E3Base.Vec(xA[0], xA[1], xA[2]);
	vB = m_E3Base.Vec(xB[0], xB[1], xB[2]);

	if ((vA == m_E3Base.vZero) ||
	    (vB == m_E3Base.vZero) ||
	    (vA == vB))
	{
		// Do not draw anything
		return true;
	}

	fAngle = float(acos(Scalar(vA * vB)));
	if (!bShort)
	{
		fAngle = 2.0f * m_fPi - fAngle;
	}

	fAngleStep = fDegAngleStep * m_fPi / 180.0f;
	Dark       = m_fBackFaceColFac * m_ActiveColor;

	if (!(iAngleSteps = int(fAngle / fAngleStep + 0.5)))
	{
		iAngleSteps = 1;
	}

	fAngleStep = fAngle / float(iAngleSteps);

	if (bShort)
	{
		vU = vA ^ vB;
	}
	else
	{
		vU = vB ^ vA;
	}

	vR  = m_E3Base.Rotor(vU, fAngleStep, false);
	vrR = ~vR;

	// Get Normal to rotation plane
	vU = *vU;
	xN = &vU[1];
	xN.Norm();

	COGLVertexList* pVexList = new COGLVertexList;

	if (!pVexList)
	{
		return false;
	}

	COGLVertexList& rVexList = *pVexList;

	rVexList.Reset();
	rVexList.Reserve(iAngleSteps + 2);

	//COGLVertexList::TVexList &mVex = rVexList.GetVexList();
	//COGLVertexList::TVexList &mNorm = rVexList.GetNormList();
	//COGLVertexList::TVexList &mTex = rVexList.GetTexList();

	float fTexMaxR = 0.5f;
	float fMaxR    = 0.5f;
	float fTexAngle;

	if (bFilled)
	{
		rVexList.SetMode(GL_TRIANGLE_FAN);
		rVexList.AddVex(xP);

		if (m_bUseAbsTexCoords)
		{
			fMaxR = fRadius;
		}

		if (m_fTexRepFac != 0.0f)
		{
			fTexMaxR = fMaxR / m_fTexRepFac;
		}

		rVexList.AddTex(0.5f, 0.5f, 0);
		rVexList.AddTex(0.5f + fTexMaxR, 0.5f, 0);
	}
	else
	{
		rVexList.SetMode(GL_LINE_STRIP);
	}

	if (m_bUseLighting)
	{
		rVexList.AddNormal(xN);

		if (bFilled)
		{
			rVexList.AddNormal(xN);
		}
	}

	vA &= fRadius;
	xA  = &vA[1];
	rVexList.AddVex(xP + xA);

	for (i = 0, fTexAngle = fAngleStep;
	     i < iAngleSteps;
	     i++, fTexAngle += fAngleStep)
	{
		vA = (vR & vA) & vrR;

		xA = &vA[1];
		rVexList.AddVex(xP + xA);

		if (bFilled)
		{
			rVexList.AddTex(0.5f + fTexMaxR * cos(fTexAngle), 0.5f - fTexMaxR * sin(fTexAngle), 0);
		}

		if (m_bUseLighting)
		{
			rVexList.AddNormal(xN);
		}
	}

	if (bFilled)
	{
		rVexList.SetPolygonOffsetFill(1.0f, 1.0f);
	}

	m_pScene->Add(m_pSceneRep->New(pVexList));

	return true;
}
