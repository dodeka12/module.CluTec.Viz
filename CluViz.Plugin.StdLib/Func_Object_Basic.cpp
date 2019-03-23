////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_Object_Basic.cpp
//
// summary:   Implements the function object basic class
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
#include "Func_Object_Basic.h"

#include "CluTec.Viz.Draw\OGLVertexList.h"
#include "CluTec.Viz.Base\TensorOperators.h"
#include "CluTec.Viz.Draw\OGLDrawBase.h"

//////////////////////////////////////////////////////////////////////
// Create a Vertex List Variable
//
// Pars:
// 1. (string) name of vertex list
// 2. (opt)(int) type of vertex list
//
// Return:
//	Vertex list object

bool  VexListFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVLT      = GL_POINTS;
	int iVarCount = int(mVars.Count());

	if ((iVarCount != 1) && (iVarCount != 2))
	{
		int piPar[] = { 1, 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("Expect name of object as first parameter.", iLine, iPos);
		return false;
	}

	COGLVertexList* pVexList = new COGLVertexList;

	if (!pVexList)
	{
		rCB.GetErrorList().GeneralError("Out of memory while creating object.", iLine, iPos);
		return false;
	}

	if (iVarCount >= 2)
	{
		if (!mVars(1).CastToCounter(iVLT))
		{
			rCB.GetErrorList().GeneralError("Expect object mode as second parameter.", iLine, iPos);
			return false;
		}

		if ((iVLT != GL_POINTS)
		    && (iVLT != GL_LINES) && (iVLT != GL_LINE_STRIP) && (iVLT != GL_LINE_LOOP)
		    && (iVLT != GL_TRIANGLES) && (iVLT != GL_TRIANGLE_STRIP) && (iVLT != GL_TRIANGLE_FAN)
		    && (iVLT != GL_QUADS) && (iVLT != GL_QUAD_STRIP)
		    && (iVLT != GL_POLYGON)
		    && (iVLT != GL_LINES_ADJACENCY) && (iVLT != GL_LINE_STRIP_ADJACENCY)
		    && (iVLT != GL_TRIANGLES_ADJACENCY) && (iVLT != GL_TRIANGLE_STRIP_ADJACENCY))
		{
			rCB.GetErrorList().GeneralError("Unknown object mode. Please use one of the predefined 'OM_' constants.", iLine, iPos);
			return false;
		}
	}

	pVexList->SetName(mVars(0).GetStringPtr()->Str());
	pVexList->SetMode(iVLT);

	COGLBEReference SceneRef = rCB.GetOGLDrawBase()->GetSceneRepository()->New(pVexList);

	rVar = SceneRef;

	return true;
}

//////////////////////////////////////////////////////////////////////
// Copy the contents of a Vertex List Variable to another vertex list variable
//
// Pars:
// 1. (vexlist) the destination vertex list
// 2. (vexlist) the source vertex list
//
// Return:
//	void
bool CopyFromVexListFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount != 2)
	{
		int piPar[] = { 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("First parameter has to be a object.", iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("second parameter has to be a object.", iLine, iPos);
		return false;
	}

	TScene scDst = *mVars(0).GetScenePtr();
	if (!scDst.IsValid())
	{
		rCB.GetErrorList().GeneralError("Invalid object", iLine, iPos);
		return false;
	}

	TScene scSrc = *mVars(1).GetScenePtr();
	if (!scSrc.IsValid())
	{
		rCB.GetErrorList().GeneralError("Invalid object", iLine, iPos);
		return false;
	}

	COGLVertexList* pVexListDst = dynamic_cast<COGLVertexList*>((COGLBaseElement*) scDst);
	COGLVertexList* pVexListSrc = dynamic_cast<COGLVertexList*>((COGLBaseElement*) scSrc);

	if (!pVexListDst)
	{
		rCB.GetErrorList().GeneralError("First parameter is not an object.", iLine, iPos);
		return false;
	}

	if (!pVexListSrc)
	{
		rCB.GetErrorList().GeneralError("Second parameter is not an object.", iLine, iPos);
		return false;
	}

	// Create copy of object
	pVexListDst->ApplyVertexAndIndexData(*pVexListSrc);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Copy the contents of a Vertex List Variable
//
// Pars:
// 1. (vexlist) the vertex list to be copied
//
// Return:
//	The copy of the Vertex list object

bool  CopyVexListFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount != 1)
	{
		int piPar[] = { 1 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("First parameter has to be a object.", iLine, iPos);
		return false;
	}

	TScene scVL = *mVars(0).GetScenePtr();
	if (!scVL.IsValid())
	{
		rCB.GetErrorList().GeneralError("Invalid object", iLine, iPos);
		return false;
	}

	COGLVertexList* pVexList = dynamic_cast<COGLVertexList*>((COGLBaseElement*) scVL);

	if (!pVexList)
	{
		rCB.GetErrorList().GeneralError("First parameter is not an object.", iLine, iPos);
		return false;
	}

	// Create copy of object
	rVar = scVL.Copy();

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set vertex list type
//
// Pars:
// 1. (vexlist) the vertex list
// 2. (opt)(int) type of vertex list
//
// Return:
//	the vertex list

bool  SetVexListTypeFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVLT;
	int iVarCount = int(mVars.Count());

	if ((iVarCount != 1) && (iVarCount != 2))
	{
		int piPar[] = { 1, 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("First parameter has to be a object.", iLine, iPos);
		return false;
	}

	TScene scVL              = *mVars(0).GetScenePtr();
	COGLVertexList* pVexList = dynamic_cast<COGLVertexList*>((COGLBaseElement*) scVL);

	if (!pVexList)
	{
		rCB.GetErrorList().GeneralError("First parameter is not an object.", iLine, iPos);
		return false;
	}

	if (iVarCount >= 2)
	{
		if (!mVars(1).CastToCounter(iVLT))
		{
			rCB.GetErrorList().GeneralError("Expect object mode as second parameter.", iLine, iPos);
			return false;
		}

		if ((iVLT != GL_POINTS)
		    && (iVLT != GL_LINES) && (iVLT != GL_LINE_STRIP) && (iVLT != GL_LINE_LOOP)
		    && (iVLT != GL_TRIANGLES) && (iVLT != GL_TRIANGLE_STRIP) && (iVLT != GL_TRIANGLE_FAN)
		    && (iVLT != GL_QUADS) && (iVLT != GL_QUAD_STRIP)
		    && (iVLT != GL_POLYGON) && (iVLT != GL_LINES_ADJACENCY) && (iVLT != GL_LINE_STRIP_ADJACENCY))
		{
			rCB.GetErrorList().GeneralError("Unknown object mode. Please use one of the predefined 'OM_' constants.", iLine, iPos);
			return false;
		}

		pVexList->SetMode(iVLT);

		rVar = scVL;
	}
	else
	{
		rVar = TCVCounter(pVexList->GetMode());
	}

	return true;
}

bool EnableVexListKeepDataFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iKeepData;
	int iVarCount = int(mVars.Count());

	if ((iVarCount != 1) && (iVarCount != 2))
	{
		int piPar[] = { 1, 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("First parameter has to be a object.", iLine, iPos);
		return false;
	}

	TScene scVL              = *mVars(0).GetScenePtr();
	COGLVertexList* pVexList = dynamic_cast<COGLVertexList*>((COGLBaseElement*) scVL);

	if (!pVexList)
	{
		rCB.GetErrorList().GeneralError("First parameter is not an object.", iLine, iPos);
		return false;
	}

	if (iVarCount >= 2)
	{
		if (!mVars(1).CastToCounter(iKeepData))
		{
			rCB.GetErrorList().GeneralError("Expect true/false as second parameter.", iLine, iPos);
			return false;
		}

		pVexList->EnableKeepDataOnHost(iKeepData != 0);

		rVar = scVL;
	}
	else
	{
		rVar = TCVCounter(pVexList->IsKeepDataOnHostEnabled() ? 1 : 0);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set vertex list instance count
//
// Pars:
// 1. (vexlist) the vertex list
// 2. (int) the instance count
//
// Return:
//	the vertex list

bool  SetVexListInstanceCountFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iInstanceCount;
	int iVarCount = int(mVars.Count());

	if ((iVarCount != 2))
	{
		int piPar[] = { 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("First parameter has to be a object.", iLine, iPos);
		return false;
	}

	TScene scVL              = *mVars(0).GetScenePtr();
	COGLVertexList* pVexList = dynamic_cast<COGLVertexList*>((COGLBaseElement*) scVL);

	if (!pVexList)
	{
		rCB.GetErrorList().GeneralError("First parameter is not an object.", iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToCounter(iInstanceCount))
	{
		rCB.GetErrorList().GeneralError("Expect instance count as second parameter.", iLine, iPos);
		return false;
	}

	if (iInstanceCount < 1)
	{
		rCB.GetErrorList().GeneralError("The instance count has to be greater or equal to 1.", iLine, iPos);
		return false;
	}

	pVexList->SetInstanceCount(iInstanceCount);

	rVar = scVL;

	return true;
}

//////////////////////////////////////////////////////////////////////
// Get vertex list data
//
// Pars:
// 1. (vexlist) the vertex list
// 2. (list) the data (return value)
//
// Return:
//	Reference to the object

bool  GetVexListDataFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount != 2)
	{
		int piPar[] = { 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("First parameter has to be an object.", iLine, iPos);
		return false;
	}

	TScene scVL              = *mVars(0).GetScenePtr();
	COGLVertexList* pVexList = dynamic_cast<COGLVertexList*>((COGLBaseElement*) scVL);

	if (!pVexList)
	{
		rCB.GetErrorList().GeneralError("First parameter is not an object.", iLine, iPos);
		return false;
	}

	if (mVars(1).IsProtected())
	{
		rCB.GetErrorList().GeneralError("Second parameter is a read only variable.", iLine, iPos);
		return false;
	}

	int iVexPos, iVex, iVexCnt;
	int iTexPos, iTex, iTexCnt;
	int iNormPos, iNorm, iNormCnt;
	int iColPos, iCol, iColCnt;
	int iIdx, iIdxCnt, iIdxList, iIdxListCnt;

	Mem<int> mDimVex, mDimTex, mDimNorm, mDimCol, mDimIdx;
	COGLVertexList::TVexList mVex, mTex, mNorm;
	COGLVertexList::TColList mCol;
	COGLVertexList::TIdxList mIdx;

	//mVex = pVexList->GetVexList();
	//mTex = pVexList->GetTexList();
	//mNorm = pVexList->GetNormList();
	//mCol = pVexList->GetColList();
	mIdx = pVexList->GetIdxList();

	mDimVex.Set(2);
	mDimVex[0] = ((iVexCnt = pVexList->GetVexCount()) ? iVexCnt : 1);
	mDimVex[1] = 3;

	mDimTex.Set(2);
	mDimTex[0] = ((iTexCnt = pVexList->GetTexCount()) ? iTexCnt : 1);
	mDimTex[1] = 2;

	mDimNorm.Set(2);
	mDimNorm[0] = ((iNormCnt = pVexList->GetNormCount()) ? iNormCnt : 1);
	mDimNorm[1] = 3;

	mDimCol.Set(2);
	mDimCol[0] = ((iColCnt = pVexList->GetColCount()) ? iColCnt : 1);
	mDimCol[1] = 4;

	mDimIdx.Set(1);
	mDimIdx[0] = 1;

	TTensor tVex(mDimVex), tTex(mDimTex), tNorm(mDimNorm), tCol(mDimCol), tIdx(mDimIdx);
	TCVScalar* pdVex  = tVex.Data();
	TCVScalar* pdTex  = tTex.Data();
	TCVScalar* pdNorm = tNorm.Data();
	TCVScalar* pdCol  = tCol.Data();

	COGLVertexList& rVexList = *pVexList;

	// Get vertex data
	for (iVex = 0, iVexPos = 0; iVex < iVexCnt; ++iVex, iVexPos += 3)
	{
		COGLVertex& rVex = rVexList[iVex].xVex;
		pdVex[iVexPos]     = rVex[0];
		pdVex[iVexPos + 1] = rVex[1];
		pdVex[iVexPos + 2] = rVex[2];
	}

	// Get texture coordinate data
	for (iTex = 0, iTexPos = 0; iTex < iTexCnt; ++iTex, iTexPos += 2)
	{
		COGLVertex& rTex = rVexList[iTex].xTex;
		pdTex[iTexPos]     = rTex[0];
		pdTex[iTexPos + 1] = rTex[1];
	}

	// Get normal data
	for (iNorm = 0, iNormPos = 0; iNorm < iNormCnt; ++iNorm, iNormPos += 3)
	{
		COGLVertex& rNorm = rVexList[iNorm].xNorm;
		pdNorm[iNormPos]     = rNorm[0];
		pdNorm[iNormPos + 1] = rNorm[1];
		pdNorm[iNormPos + 2] = rNorm[2];
	}

	// Get color data
	for (iCol = 0, iColPos = 0; iCol < iColCnt; ++iCol, iColPos += 4)
	{
		TColor& rCol = rVexList[iCol].xCol;
		pdCol[iColPos]     = rCol[0];
		pdCol[iColPos + 1] = rCol[1];
		pdCol[iColPos + 2] = rCol[2];
		pdCol[iColPos + 3] = rCol[3];
	}

	// Create return variable
	CCodeVar& rData = mVars(1);
	rData.New(PDT_VARLIST, rData.Name().c_str());

	TVarList& rList = *rData.GetVarListPtr();
	rList.Set(5);

	// Vertex List Data
	rList(0).New(PDT_VARLIST, "Vex");
	TVarList& rListVex = *rList(0).GetVarListPtr();
	rListVex.Set(2);
	rListVex(0) = TString("vex");
	if (iVexCnt > 0)
	{
		rListVex(1) = tVex;
	}
	else
	{
		rListVex(1) = TCVScalar(0);
	}

	// Texture Coordinate List Data
	rList(1).New(PDT_VARLIST, "Tex");
	TVarList& rListTex = *rList(1).GetVarListPtr();
	rListTex.Set(2);
	rListTex(0) = TString("tex");
	if (iTexCnt > 0)
	{
		rListTex(1) = tTex;
	}
	else
	{
		rListTex(1) = TCVScalar(0);
	}

	// Normal List Data
	rList(2).New(PDT_VARLIST, "Norm");
	TVarList& rListNorm = *rList(2).GetVarListPtr();
	rListNorm.Set(2);
	rListNorm(0) = TString("norm");
	if (iNormCnt > 0)
	{
		rListNorm(1) = tNorm;
	}
	else
	{
		rListNorm(1) = TCVScalar(0);
	}

	// Color List Data
	rList(3).New(PDT_VARLIST, "Color");
	TVarList& rListCol = *rList(3).GetVarListPtr();
	rListCol.Set(2);
	rListCol(0) = TString("col");
	if (iColCnt > 0)
	{
		rListCol(1) = tCol;
	}
	else
	{
		rListCol(1) = TCVScalar(0);
	}

	// Index Lists
	iIdxListCnt = int(mIdx.Count());

	rList(4).New(PDT_VARLIST, "Index");
	TVarList& rListIdx = *rList(4).GetVarListPtr();
	rListIdx.Set(2);
	rListIdx(0) = TString("idx");
	rListIdx(1).New(PDT_VARLIST, "indexlists");

	TVarList& rIdxListList = *rListIdx(1).GetVarListPtr();
	rIdxListList.Set(iIdxListCnt);

	for (iIdxList = 0; iIdxList < iIdxListCnt; ++iIdxList)
	{
		Mem<unsigned>& mIdxList = mIdx[iIdxList];
		iIdxCnt    = int(mIdxList.Count());
		mDimIdx[0] = iIdxCnt;

		tIdx.Reset(mDimIdx);
		TCVScalar* pdIdx = tIdx.Data();

		for (iIdx = 0; iIdx < iIdxCnt; ++iIdx)
		{
			pdIdx[iIdx] = TCVScalar(mIdxList[iIdx] + 1);
		}

		rIdxListList(iIdxList) = tIdx;
	}

	// Return reference to object
	rVar = scVL;

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set vertex list polygon mode
//
// Pars:
// 1. (vexlist) the vertex list
// 2. (opt)(string) polygon mode
//
// Return:
//	the vertex list

bool  SetVexListPolyModeFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if ((iVarCount != 1) && (iVarCount != 2))
	{
		int piPar[] = { 1, 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("First parameter has to be a object.", iLine, iPos);
		return false;
	}

	TScene scVL              = *mVars(0).GetScenePtr();
	COGLVertexList* pVexList = dynamic_cast<COGLVertexList*>((COGLBaseElement*) scVL);

	if (!pVexList)
	{
		rCB.GetErrorList().GeneralError("First parameter is not an object.", iLine, iPos);
		return false;
	}

	if (iVarCount == 2)
	{
		if (mVars(1).BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().GeneralError("Expect object structure mode as second parameter. Has to be one of 'points', 'lines', 'fill'.", iLine, iPos);
			return false;
		}

		TString csMode = *mVars(1).GetStringPtr();

		if (csMode == "points")
		{
			pVexList->SetPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			pVexList->EnablePolygonMode(true);
		}
		else if (csMode == "lines")
		{
			pVexList->SetPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			pVexList->EnablePolygonMode(true);
		}
		else if (csMode == "fill")
		{
			pVexList->SetPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			pVexList->EnablePolygonMode(true);
		}
		else
		{
			rCB.GetErrorList().GeneralError("Unknown object structure mode. Has to be one of: 'points', 'lines', 'fill'.", iLine, iPos);
			return false;
		}

		rVar = scVL;
	}
	else
	{
		GLenum eFace, eMode;

		pVexList->GetPolygonMode(eFace, eMode);
		if (eMode == GL_POINT)
		{
			rVar = "points";
		}
		else if (eMode == GL_LINE)
		{
			rVar = "lines";
		}
		else if (eMode == GL_FILL)
		{
			rVar = "fill";
		}
		else
		{
			rVar = "fill";
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set vertex list scaling
//
// Pars:
// 1. (vexlist) the vertex list
// 2. (scalar) scale X
// 3. (scalar) scale Y
// 4. (scalar) scale Z
//
// Return:
//	the vertex list

bool  SetVexListScaleFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	double dX, dY, dZ;
	COGLVertex vexScale;

	if ((iVarCount != 1) && (iVarCount != 4))
	{
		int piPar[] = { 1, 4 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("First parameter has to be a object.", iLine, iPos);
		return false;
	}

	TScene scVL              = *mVars(0).GetScenePtr();
	COGLVertexList* pVexList = dynamic_cast<COGLVertexList*>((COGLBaseElement*) scVL);

	if (!pVexList)
	{
		rCB.GetErrorList().GeneralError("First parameter is not an object.", iLine, iPos);
		return false;
	}

	if (iVarCount == 4)
	{
		if (!mVars(1).CastToScalar(dX, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Second parameter has to give x-scale.", iLine, iPos);
			return false;
		}

		if (!mVars(2).CastToScalar(dY, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Third parameter has to give y-scale.", iLine, iPos);
			return false;
		}

		if (!mVars(3).CastToScalar(dZ, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Fourth parameter has to give z-scale.", iLine, iPos);
			return false;
		}

		vexScale.Set(float(dX), float(dY), float(dZ));

		pVexList->SetScaling(vexScale);

		if ((dX == 1.0) && (dY == 1.0) && (dZ == 1.0))
		{
			pVexList->EnableScaling(false);
		}
		else
		{
			pVexList->EnableScaling(true);
		}

		rVar = scVL;
	}
	else
	{
		pVexList->GetScaling(vexScale);

		rVar.New(PDT_VARLIST);
		TVarList& rList = *rVar.GetVarListPtr();

		rList.Set(3);
		rList(0) = TCVScalar(vexScale[0]);
		rList(1) = TCVScalar(vexScale[1]);
		rList(2) = TCVScalar(vexScale[2]);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Clear Vertex List
//
// Pars:
// 1. (vexlist) the vertex list
// (2.) (string) what should be cleared. Elements can be combined by ','.
//		- (default) "all": clear all lists
//		- "vex": vertex list
//		- "norm": normal list
//		- "tex": texture list
//		- "col": color list
//		- "idx": index lists
//
// Return:
//	the vertex list

bool  ClearVexListFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount  = int(mVars.Count());
	TString csType = "all";

	if ((iVarCount != 1) && (iVarCount != 2))
	{
		int piPar[] = { 1, 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("First parameter has to be a object.", iLine, iPos);
		return false;
	}

	if (iVarCount > 1)
	{
		if (mVars(1).BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().GeneralError("Second parameter has to give one or a combination of the clear types: 'all', 'vex', 'norm', 'tex', 'col', 'idx'.",
					iLine,
					iPos);
			return false;
		}

		csType = *mVars(1).GetStringPtr();
	}

	TScene scVL              = *mVars(0).GetScenePtr();
	COGLVertexList* pVexList = dynamic_cast<COGLVertexList*>((COGLBaseElement*) scVL);

	if (!pVexList)
	{
		rCB.GetErrorList().GeneralError("First parameter is not an object.", iLine, iPos);
		return false;
	}

	char* pcType = csType.Str();

	if (strstr(pcType, "all") || strstr(pcType, "vex"))
	{
		pVexList->ResetVexList();
	}

	if (strstr(pcType, "all") || strstr(pcType, "norm"))
	{
		pVexList->ResetNormList();
	}

	if (strstr(pcType, "all") || strstr(pcType, "tex"))
	{
		pVexList->ResetTexList();
	}

	if (strstr(pcType, "all") || strstr(pcType, "col"))
	{
		pVexList->ResetColList();
	}

	if (strstr(pcType, "all") || strstr(pcType, "idx"))
	{
		pVexList->GetIdxList().Set(0);
	}

	rVar = scVL;

	return true;
}

//////////////////////////////////////////////////////////////////////
// Set Vertex List Line Stipple
//
// Set stipple pattern and factor for lines in vertex list.
// If the stipple pattern is >= 0xFFFF or == 0 then stippling is switched off.
//
// Pars:
// 1. (vexlist) the vertex list
// 2. (counter) the pattern
// 3. (counter) the factor
//
// Return:
//	reference to vertex list

bool SetVexListLineStippleFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVCounter iPattern, iFactor;

	if ((iVarCount != 1) && (iVarCount != 3))
	{
		int piPar[] = { 1, 3 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_SCENE)
	{
		rCB.GetErrorList().GeneralError("First parameter has to be an object.", iLine, iPos);
		return false;
	}

	TScene scVL              = *mVars(0).GetScenePtr();
	COGLVertexList* pVexList = dynamic_cast<COGLVertexList*>((COGLBaseElement*) scVL);

	if (!pVexList)
	{
		rCB.GetErrorList().GeneralError("First parameter is not an object.", iLine, iPos);
		return false;
	}

	if (iVarCount >= 3)
	{
		if (!mVars(1).CastToCounter(iPattern, true))
		{
			rCB.GetErrorList().GeneralError("Second parameter has to give the stipple pattern.", iLine, iPos);
			return false;
		}

		if (!mVars(2).CastToCounter(iFactor, true))
		{
			rCB.GetErrorList().GeneralError("Third parameter has to give the stipple factor.", iLine, iPos);
			return false;
		}

		if ((unsigned(iPattern) >= 0xFFFF) || (iPattern == 0))
		{
			pVexList->EnableLineStipple(false);
		}
		else
		{
			if (iFactor < 1)
			{
				iFactor = 1;
			}
			else if (iFactor > 256)
			{
				iFactor = 256;
			}

			pVexList->EnableLineStipple(true);
			pVexList->SetLineStipple(iFactor, iPattern);
		}

		rVar = scVL;
	}
	else
	{
		pVexList->GetLineStipple(iFactor, iPattern);

		rVar.New(PDT_VARLIST);
		TVarList& rList = *rVar.GetVarListPtr();

		rList.Set(2);
		rList(0) = iFactor;
		rList(1) = iPattern;
	}

	return true;
}
