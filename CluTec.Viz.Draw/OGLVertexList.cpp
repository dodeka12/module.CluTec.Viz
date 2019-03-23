////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLVertexList.cpp
//
// summary:   Implements the ogl vertex list class
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
#include "CluTec.Viz.OpenGL.Extensions\Extensions.h"
#include "CluTec.Viz.OpenGL\Api.h"

#include "OGLVertexList.h"
#include "OGLShader.h"
#include "CluTec.Base/Exception.h"

#undef M_PI_4
#undef M_1_PI
#undef M_2_PI
#undef M_SQRT1_2
#include "CluTec.Math/Matrix.h"
#include "CluTec.Math/Matrix.Algo.SVD.h"

//// IMPORTANT IMPORTANT IMPORTANT
/*
		Never write this:

				m_VexList.AddVex(m_VexList[0]);

		since internally the memory of m_VexList is enlarged, such that m_VexList[0]
		may not be a valid reference anymore!!!
*/

// OGLVertexList.cpp: Implementierung der Klasse COGLVertexList.
//
//////////////////////////////////////////////////////////////////////

#define BUFFER_OFFSET(i) ((char*) NULL + (i))

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COGLVertexList::COGLVertexList()
{
	m_sTypeName = "Object";

	m_bScaleBackColor = false;
	m_fBackColorScale = 1.0f;

	m_bLineStipple        = false;
	m_iLineStippleFactor  = 1;
	m_iLineStipplePattern = 0xFFFF;

	m_bOverrideAlpha      = false;
	m_fOverrideAlphaValue = 1.0f;

	m_bPolygonOffsetFill = false;
	m_fPOFX              = 0.0f;
	m_fPOFY              = 0.0f;

	m_bUsePolyCull                = false;
	m_bUsePolyCullWhenTransparent = false;
	m_bUsePolygonMode             = false;
	m_ePolyFace                   = GL_FRONT_AND_BACK;
	m_ePolyMode                   = GL_FILL;
	m_ePolyFrontFace              = GL_CCW;

	m_bUseTransform    = false;
	m_bUseGenTransform = false;
	m_bUseScaling      = false;

	m_iInstanceCount = 1;

	m_matTrans.Resize(4, 4);
	m_matTrans.Identity();

	m_vexTrans1.Set(0, 0, 0);
	m_vexTrans2.Set(0, 0, 0);

	m_vexRotAxis.Set(1, 0, 0);
	m_fRotAngle = 0.0f;

	m_vexScale.Set(1.0f, 1.0f, 1.0f);

	Reset();

	m_bVexModified = false;
	m_uVexBufID    = 0;
	m_uIdxBufID    = 0;

	m_bExternalVBO    = false;
	m_bKeepDataOnHost = true;

	m_matProjection.SetZero();
	m_matModelView.SetZero();
	m_matModelViewProj.SetZero();

	m_matInvProjection.SetZero();
	m_matInvModelView.SetZero();
	m_matInvModelViewProj.SetZero();

	m_bMatrixChangedProjection = true;
	m_bMatrixChangedModelView  = true;
}

COGLVertexList::~COGLVertexList()
{
	// do not delete VBOs if they are external ones
	if (m_bExternalVBO == false)
	{
		if (glDeleteBuffers)
		{
			Clu::OpenGL::DeleteBuffers(1, &m_uVexBufID);
			Clu::OpenGL::DeleteBuffers(1, &m_uIdxBufID);
		}
	}
}

COGLVertexList::COGLVertexList(const COGLVertexList& rVexList)
{
	m_sTypeName = "Object";
	Reset();

	m_bVexModified = false;
	m_uVexBufID    = 0;
	m_uIdxBufID    = 0;
	m_bExternalVBO = false;

	*this = rVexList;
}

//////////////////////////////////////////////////////////////////////
/// Operator =

COGLVertexList& COGLVertexList::operator=(const COGLVertexList& rVexList)
{
	COGLBaseElement::operator=(rVexList);

	m_bKeepDataOnHost = rVexList.m_bKeepDataOnHost;

	m_bVexModified = true;
	m_bIdxModified = true;

	m_eMode          = rVexList.m_eMode;
	m_mDataList      = rVexList.m_mDataList;
	m_iVexCnt        = rVexList.m_iVexCnt;
	m_iTexCnt        = rVexList.m_iTexCnt;
	m_iNormCnt       = rVexList.m_iNormCnt;
	m_iColCnt        = rVexList.m_iColCnt;
	m_iEdgeCnt       = rVexList.m_iEdgeCnt;
	m_iPartIdCnt     = rVexList.m_iPartIdCnt;
	m_iInstanceCount = rVexList.m_iInstanceCount;

	m_mIdxList = rVexList.m_mIdxList;

	m_bScaleBackColor = rVexList.m_bScaleBackColor;
	m_fBackColorScale = rVexList.m_fBackColorScale;

	m_bPolygonOffsetFill = rVexList.m_bPolygonOffsetFill;
	m_fPOFX              = rVexList.m_fPOFX;
	m_fPOFY              = rVexList.m_fPOFY;

	m_bLineStipple        = rVexList.m_bLineStipple;
	m_iLineStippleFactor  = rVexList.m_iLineStippleFactor;
	m_iLineStipplePattern = rVexList.m_iLineStipplePattern;

	m_bOverrideAlpha      = rVexList.m_bOverrideAlpha;
	m_fOverrideAlphaValue = rVexList.m_fOverrideAlphaValue;

	m_bUsePolygonMode             = rVexList.m_bUsePolygonMode;
	m_bUsePolyCull                = rVexList.m_bUsePolyCull;
	m_bUsePolyCullWhenTransparent = rVexList.m_bUsePolyCullWhenTransparent;
	m_ePolyFace                   = rVexList.m_ePolyFace;
	m_ePolyMode                   = rVexList.m_ePolyMode;
	m_ePolyFrontFace              = rVexList.m_ePolyFrontFace;

	m_bUseTransform    = rVexList.m_bUseTransform;
	m_bUseGenTransform = rVexList.m_bUseGenTransform;
	m_bUseScaling      = rVexList.m_bUseScaling;

	m_vexTrans1 = rVexList.m_vexTrans1;
	m_vexTrans2 = rVexList.m_vexTrans2;

	m_vexRotAxis = rVexList.m_vexRotAxis;
	m_fRotAngle  = rVexList.m_fRotAngle;

	m_vexScale = rVexList.m_vexScale;

	m_matTrans = rVexList.m_matTrans;

	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void COGLVertexList::ApplyVertexAndIndexData(const COGLVertexList& xSource)
{
	try
	{
		// Set the vertex data
		ApplyVertexData(xSource.m_eMode, xSource.m_mDataList.Count(), xSource.m_mDataList.Data(), (xSource.m_iVexCnt != 0), (xSource.m_iTexCnt != 0),
				(xSource.m_iNormCnt != 0), (xSource.m_iColCnt != 0), false, (xSource.m_iEdgeCnt != 0), (xSource.m_iPartIdCnt != 0), xSource.m_uVexBufID);

		// Set the index data
		m_mIdxList = xSource.m_mIdxList;
	}
	catch (Clu::CIException& ex)
	{
		throw CLU_EXCEPTION_NEST("Error setting vertex data", std::move(ex));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void COGLVertexList::ApplyVertexData(GLenum eMode,
		size_t nCount,
		const SData* pData,
		bool bUseVex,
		bool bUseTex,
		bool bUseNorm,
		bool bUseCol,
		bool bUseFogZ,
		bool bUseEdge,
		bool bEnablePartPicking,
		unsigned uVertexBufferID)
{
	// If pData is a valid pointer, copy the data
	if (pData != nullptr)
	{
		if (nCount != m_mDataList.Count())
		{
			if (!m_mDataList.Set(nCount))
			{
				throw CLU_EXCEPTION("Error creating target memory");
			}
		}

		memcpy(m_mDataList.Data(), pData, nCount * sizeof(SData));
		m_bVexModified = true;
	}
	else
	{
		// There is no valid pData pointer, so we assume to have an external VertexBufferID
		m_bExternalVBO = true;
		m_uVexBufID    = uVertexBufferID;
		m_bVexModified = false;
	}

	m_eMode      = eMode;
	m_iVexCnt    = (bUseVex ? int(nCount) : 0);
	m_iTexCnt    = (bUseTex ? int(nCount) : 0);
	m_iNormCnt   = (bUseNorm ? int(nCount) : 0);
	m_iColCnt    = (bUseCol ? int(nCount) : 0);
	m_iEdgeCnt   = (bUseEdge ? int(nCount) : 0);
	m_iPartIdCnt = (bEnablePartPicking ? int(nCount) : 0);
}

//////////////////////////////////////////////////////////////////////
// Set Index list from arrays
bool COGLVertexList::SetIndexLists(const unsigned** const ppuIndexLists, const unsigned* const puIndexListLengths, unsigned uIndexListCount, unsigned uIndexBufferID)
{
	// if there is no index List len array is set, or the count is zero, we cannot continue
	if ((puIndexListLengths == nullptr) || (uIndexListCount == 0))
	{
		return false;
	}

	//if the index data is a nullptr AND no buffer is given, we cannot continue
	if ((ppuIndexLists == nullptr) && (uIndexBufferID == 0))
	{
		return false;
	}

	if (ppuIndexLists != nullptr)
	{
		// Clear current index lists
		m_mIdxList.Set(uIndexListCount);

		unsigned uIndexList;
		for (uIndexList = 0; uIndexList < uIndexListCount; ++uIndexList)
		{
			Mem<unsigned>& mList = m_mIdxList[uIndexList];
			unsigned uCnt        = puIndexListLengths[uIndexList];

			if (!mList.Set(uCnt))
			{
				return false;
			}

			const unsigned* puList = ppuIndexLists[uIndexList];
			if (puList == nullptr)
			{
				return false;
			}
			memcpy(mList.Data(), puList, uCnt * sizeof(unsigned));
		}

		m_bIdxModified = true;
	}
	else
	{
		//we have an external Index list !
		m_mIdxList.Set(uIndexListCount);

		m_uIdxBufID = uIndexBufferID;
		m_mIdxListLengths.Set(uIndexListCount);

		for (unsigned i = 0; i < uIndexListCount; i++)
		{
			m_mIdxListLengths[i] = puIndexListLengths[i];
		}

		m_bIdxModified = false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add Vertex

bool COGLVertexList::AddVex(const COGLVertex& rVex)
{
	if (m_iVexCnt == m_mDataList.Count())
	{
		if (!(m_mDataList.Add(1))) { return false; } }

	m_mDataList[m_iVexCnt].xVex = rVex;
	m_mDataList[m_iVexCnt].xVex.Clamp();
	++m_iVexCnt;
	m_bVexModified = true;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add Vertex

bool COGLVertexList::AddVex(const float* m_pfVex)
{
	if (m_iVexCnt == m_mDataList.Count())
	{
		if (!(m_mDataList.Add(1))) { return false; } }

	m_mDataList[m_iVexCnt].xVex = m_pfVex;
	m_mDataList[m_iVexCnt].xVex.Clamp();
	++m_iVexCnt;
	m_bVexModified = true;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add Vertex

bool COGLVertexList::AddVex(float fX, float fY, float fZ)
{
	if (m_iVexCnt == m_mDataList.Count())
	{
		if (!(m_mDataList.Add(1))) { return false; } }

	m_mDataList[m_iVexCnt].xVex.Set(fX, fY, fZ);
	m_mDataList[m_iVexCnt].xVex.Clamp();
	++m_iVexCnt;
	m_bVexModified = true;

	return true;
}

//////////////////////////////////////////////////////////////////////
bool COGLVertexList::AddVexRange(const TVexList& mVex)
{
	size_t iNewCnt = mVex.Count();
	if (size_t(m_iVexCnt + iNewCnt) > m_mDataList.Count())
	{
		if (!(m_mDataList.Set(m_iVexCnt + iNewCnt)))
		{
			return false;
		}
	}

	for (size_t nIdx = 0; nIdx < iNewCnt; ++nIdx)
	{
		m_mDataList[m_iVexCnt + nIdx].xVex = mVex[nIdx];
	}

	m_iVexCnt     += int(iNewCnt);
	m_bVexModified = true;
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add Texture Coordinates

bool COGLVertexList::AddTex(const COGLVertex& rVex)
{
	if (m_iTexCnt == m_mDataList.Count())
	{
		if (!(m_mDataList.Add(1)))
		{
			return false;
		}
	}

	m_mDataList[m_iTexCnt].xTex = rVex;
	m_mDataList[m_iTexCnt].xTex.Clamp();
	++m_iTexCnt;
	m_bVexModified = true;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add Texture Coordinates

bool COGLVertexList::AddTex(const float* m_pfVex)
{
	if (m_iTexCnt == m_mDataList.Count())
	{
		if (!(m_mDataList.Add(1)))
		{
			return false;
		}
	}

	m_mDataList[m_iTexCnt].xTex = m_pfVex;
	m_mDataList[m_iTexCnt].xTex.Clamp();
	++m_iTexCnt;
	m_bVexModified = true;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add Texture Coordinates

bool COGLVertexList::AddTex(float fX, float fY, float fZ)
{
	if (m_iTexCnt == m_mDataList.Count())
	{
		if (!(m_mDataList.Add(1))) { return false; } }

	m_mDataList[m_iTexCnt].xTex.Set(fX, fY, fZ);
	m_mDataList[m_iTexCnt].xTex.Clamp();
	++m_iTexCnt;
	m_bVexModified = true;

	return true;
}

//////////////////////////////////////////////////////////////////////
bool COGLVertexList::AddTexRange(const TVexList& mVex)
{
	if (m_iTexCnt + mVex.Count() > m_mDataList.Count())
	{
		if (!(m_mDataList.Set(m_iTexCnt + mVex.Count())))
		{
			return false;
		}
	}

	for (int i = 0; i < (int) mVex.Count(); ++i)
	{
		m_mDataList[m_iTexCnt + i].xTex = mVex[i];
	}

	m_iTexCnt     += int(mVex.Count());
	m_bVexModified = true;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add Normal

bool COGLVertexList::AddNormal(const COGLVertex& rVex)
{
	if (m_iNormCnt == m_mDataList.Count())
	{
		if (!(m_mDataList.Add(1))) { return false; } }

	m_mDataList[m_iNormCnt].xNorm = rVex;
	m_mDataList[m_iNormCnt].xNorm.Clamp();
	++m_iNormCnt;
	m_bVexModified = true;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add Normal

bool COGLVertexList::AddNormal(const float* m_pfVex)
{
	if (m_iNormCnt == m_mDataList.Count())
	{
		if (!(m_mDataList.Add(1))) { return false; } }

	m_mDataList[m_iNormCnt].xNorm = m_pfVex;
	m_mDataList[m_iNormCnt].xNorm.Clamp();
	++m_iNormCnt;
	m_bVexModified = true;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add Normal

bool COGLVertexList::AddNormal(float fX, float fY, float fZ)
{
	if (m_iNormCnt == m_mDataList.Count())
	{
		if (!(m_mDataList.Add(1))) { return false; } }

	m_mDataList[m_iNormCnt].xNorm.Set(fX, fY, fZ);
	m_mDataList[m_iNormCnt].xNorm.Clamp();
	++m_iNormCnt;
	m_bVexModified = true;

	return true;
}

//////////////////////////////////////////////////////////////////////
bool COGLVertexList::AddNormalRange(const TVexList& mVex)
{
	if (m_iNormCnt + mVex.Count() > m_mDataList.Count())
	{
		if (!(m_mDataList.Set(m_iNormCnt + mVex.Count()))) { return false; } }

	for (int i = 0; i < (int) mVex.Count(); ++i)
	{
		m_mDataList[m_iNormCnt + i].xNorm = mVex[i];
	}

	m_iNormCnt    += int(mVex.Count());
	m_bVexModified = true;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add Color

bool COGLVertexList::AddCol(const COGLColor& rCol)
{
	if (m_iColCnt == m_mDataList.Count())
	{
		if (!(m_mDataList.Add(1))) { return false; } }

	m_mDataList[m_iColCnt].xCol = rCol.Data();
	++m_iColCnt;
	m_bVexModified = true;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add Color

bool COGLVertexList::AddCol(const float* pfCol)
{
	if (m_iColCnt == m_mDataList.Count())
	{
		if (!(m_mDataList.Add(1))) { return false; } }

	m_mDataList[m_iColCnt].xCol = pfCol;
	++m_iColCnt;
	m_bVexModified = true;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add Color

bool COGLVertexList::AddCol(float fR, float fG, float fB, float fA)
{
	if (m_iColCnt == m_mDataList.Count())
	{
		if (!(m_mDataList.Add(1))) { return false; } }

	TColor& xCol = m_mDataList[m_iColCnt].xCol;
	xCol[0] = fR;
	xCol[1] = fG;
	xCol[2] = fB;
	xCol[3] = fA;
	++m_iColCnt;
	m_bVexModified = true;

	return true;
}

//////////////////////////////////////////////////////////////////////
bool COGLVertexList::AddColRange(const MemObj<COGLColor>& mCol)
{
	if (m_iColCnt + mCol.Count() > m_mDataList.Count())
	{
		if (!(m_mDataList.Set(m_iColCnt + mCol.Count()))) { return false; } }

	for (int i = 0; i < (int) mCol.Count(); ++i)
	{
		m_mDataList[m_iColCnt + i].xCol = mCol[i].Data();
	}

	m_iColCnt     += int(mCol.Count());
	m_bVexModified = true;

	return true;
}

//////////////////////////////////////////////////////////////////////
bool COGLVertexList::AddPartIdRange(const TPartIdList& mPartIdList)
{
	if (m_iPartIdCnt + mPartIdList.Count() > m_mDataList.Count())
	{
		if (!(m_mDataList.Set(m_iPartIdCnt + mPartIdList.Count())))
		{
			return false;
		}
	}

	for (int i = 0; i < (int) mPartIdList.Count(); ++i)
	{
		m_mDataList[m_iPartIdCnt + i].uPartId = mPartIdList[i];
	}

	m_iPartIdCnt  += int(mPartIdList.Count());
	m_bVexModified = true;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Index List

bool COGLVertexList::SetIdxList(size_t nNo, unsigned* pIdx)
{
	m_mIdxList.Set(1);
	Mem<unsigned>& rIdx = m_mIdxList[0];

	if ((nNo < 0) || !rIdx.Set(nNo))
	{
		return false;
	}

	memcpy(rIdx.Data(), pIdx, nNo * sizeof(unsigned));
	m_bIdxModified = true;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Index List

bool COGLVertexList::SetIdxList(Mem<unsigned>& mIdx)
{
	m_mIdxList.Set(1);
	m_mIdxList[0]  = mIdx;
	m_bIdxModified = true;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add Index List

bool COGLVertexList::AddIdxList(size_t nNo, unsigned* pIdx)
{
	m_mIdxList.Add(1);
	Mem<unsigned>& rIdx = m_mIdxList.Last();

	if ((nNo < 0) || !rIdx.Set(nNo))
	{
		return false;
	}

	memcpy(rIdx.Data(), pIdx, nNo * sizeof(unsigned));
	m_bIdxModified = true;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add Index List

bool COGLVertexList::AddIdxList(Mem<unsigned>& mIdx)
{
	m_mIdxList.Add(1);
	m_mIdxList.Last() = mIdx;
	m_bIdxModified    = true;

	return true;
}

//////////////////////////////////////////////////////////////////////
// Invert Normals
// Invert direction of all normals for backface of objects

void COGLVertexList::InvertNormals(float fFac)
{
	m_bVexModified = true;
	size_t i, n = m_mDataList.Count();

	for (i = 0; i < n; i++)
	{
		m_mDataList[i].xNorm = -m_mDataList[i].xNorm * fFac;
	}
}

//////////////////////////////////////////////////////////////////////
void COGLVertexList::_LoadMatrixProjection()
{
	float pfMatrix[16];

	glGetFloatv(GL_PROJECTION_MATRIX, pfMatrix);

	if ((m_bMatrixChangedProjection = (memcmp(m_matProjection.DataPointer(), pfMatrix, 16 * sizeof(float)) != 0)) == true)
	{
		memcpy(m_matProjection.DataPointer(), pfMatrix, 16 * sizeof(float));
	}
}

//////////////////////////////////////////////////////////////////////
void COGLVertexList::_LoadMatrixModelView()
{
	float pfMatrix[16];

	glGetFloatv(GL_MODELVIEW_MATRIX, pfMatrix);

	if ((m_bMatrixChangedModelView = (memcmp(m_matModelView.DataPointer(), pfMatrix, 16 * sizeof(float)) != 0)) == true)
	{
		memcpy(m_matModelView.DataPointer(), pfMatrix, 16 * sizeof(float));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
COGLVertexList::TMat4 COGLVertexList::InvertMatrix(const TMat4& _mA)
{
	TMat4 mInvA;

	Clu::CMatrix<float> mA(_mA);
	mA = Clu::CMatrixAlgoSVD<float>::Inverse(mA, 1e-5f);
	mA.ToMatrix(mInvA);

	return mInvA;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool COGLVertexList::Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData& rData)
{
	if (m_iVexCnt == 0)
	{
		return true;
	}

	if (m_uVexBufID == 0)
	{
		CLU_OGL_CALL(glGenBuffers(1, &m_uVexBufID));
	}

	if (m_uIdxBufID == 0)
	{
		CLU_OGL_CALL(glGenBuffers(1, &m_uIdxBufID));
	}

	unsigned uShaderID = 0;
	// True if a shader is active
	bool bShaderOn = false;
	// This is true if a shader is active and its version is greater or equal to GLSL version 1.4
	bool bShader_1_4 = false;

	COGLShader* pxShader = dynamic_cast<COGLShader*>(rData.pCurShader);
	if (pxShader != nullptr)
	{
		bShaderOn   = true;
		bShader_1_4 = pxShader->GetFragmentShaderVersion() >= 140 || pxShader->GetVertexShaderVersion() >= 140 || pxShader->GetGeometryShaderVersion() >= 140;
		uShaderID   = pxShader->GetShaderGlId();

		// TODO: Shader version test correct? Not && required?
	}

	// Flag that we are in pick mode
	bool bPickMode = (eMode == COGLBaseElement::PICK);

	bool bUsePartIdAsColor = false;
	if (bPickMode)
	{
		// Push Object ID onto name stack
		rData.PushPickName(GetUID());

		// Now store current pick name stack in list
		uint uPickName  = rData.StorePickNameStack();
		uint uPickColor = rData.ConvertNameToColor(uPickName, 0);

		if ((m_iPartIdCnt > 0) && m_bKeepDataOnHost)
		{
			SData* pData = m_mDataList.Data();
			// Only update the part ID names with uPickName if necessary
			if (pData->uPartId != rData.ConvertNameToColor(uPickName, pData->uPartId))
			{
				for (int iVexIdx = 0; iVexIdx < m_iVexCnt; ++iVexIdx, ++pData)
				{
					// Use the lower 20 bits for the part ID and the upper 12 bits for the pick name
					pData->uPartId = rData.ConvertNameToColor(uPickName, pData->uPartId);
				}

				m_bVexModified = true;
			}

			bUsePartIdAsColor = true;
		}
		else
		{
			//Set current color to picking ID color
			CLU_OGL_CALL(glColor4ubv((const GLubyte*) &uPickColor));
			rData.pfCurColor[0] = float(uPickColor & 0x000000FF) / 255.0f;
			rData.pfCurColor[1] = float((uPickColor & 0x0000FF00) >> 8) / 255.0f;
			rData.pfCurColor[2] = float((uPickColor & 0x00FF0000) >> 16) / 255.0f;
			rData.pfCurColor[3] = float((uPickColor & 0xFF000000) >> 24) / 255.0f;
		}
	}

	int piUsedTexUnit[OGL_MAX_TEX_UNITS];
	int iUsedTexUnitCnt = 0;

	if (m_iTexCnt != 0)
	{
		for (int iTexUnit = 0; iTexUnit < OGL_MAX_TEX_UNITS; iTexUnit++)
		{
			if (rData.pbActTexUnit[iTexUnit])
			{
				piUsedTexUnit[iUsedTexUnitCnt++] = iTexUnit;
			}
		}
	}

	bool bUseTex    = (m_iTexCnt != 0 && (uShaderID > 0 || iUsedTexUnitCnt > 0));
	bool bUseNorm   = (m_iNormCnt != 0);
	bool bUseColor  = (!bPickMode && (m_iColCnt != 0));
	bool bUsePartId = (!bPickMode && (m_iPartIdCnt > 0));

	float pfCurrentColor[4];
	float pfFrontColor[4];

	if (!bPickMode)
	{
		// Get current color
		CLU_OGL_CALL(glGetFloatv(GL_CURRENT_COLOR, pfCurrentColor));

		// Copy current color to front color
		memcpy(pfFrontColor, pfCurrentColor, 4 * sizeof(float));

		if (m_bOverrideAlpha)
		{
			// Override alpha channel of front color
			pfFrontColor[3] = m_fOverrideAlphaValue;

			// Set front color
			CLU_OGL_CALL(glColor4fv(pfFrontColor));

			// Copy front color to apply data
			memcpy(rData.pfCurColor, pfFrontColor, 4 * sizeof(float));
		}

		if (m_bScaleBackColor)
		{
			// Back color is scaled front color
			float pfBackColor[4];
			pfBackColor[0] = m_fBackColorScale * pfFrontColor[0];
			pfBackColor[1] = m_fBackColorScale * pfFrontColor[1];
			pfBackColor[2] = m_fBackColorScale * pfFrontColor[2];
			pfBackColor[3] = m_fBackColorScale * pfFrontColor[3];

			CLU_OGL_CALL(glDisable(GL_COLOR_MATERIAL));
			CLU_OGL_CALL(glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, pfFrontColor));
			CLU_OGL_CALL(glMaterialfv(GL_BACK, GL_AMBIENT_AND_DIFFUSE, pfBackColor));
		}

		if (m_bLineStipple)
		{
			CLU_OGL_CALL(glEnable(GL_LINE_STIPPLE));
			CLU_OGL_CALL(glLineStipple(m_iLineStippleFactor, (GLushort) m_iLineStipplePattern));
		}
	}
	else
	{
		pfCurrentColor[0] = 0.0f;
		pfCurrentColor[1] = 0.0f;
		pfCurrentColor[2] = 0.0f;
		pfCurrentColor[3] = 0.0f;

		pfFrontColor[3] = 0.0f;
	}

	if (m_bUseTransform || m_bUseGenTransform || m_bUseScaling)
	{
		rData.xMatrixStack.Push(Clu::CMatrixStack::ModelView);
	}

	if (m_bUseGenTransform)
	{
		CLU_OGL_CALL(glMultMatrixf(m_matTrans.Data()));
		CLU_OGL_CALL(glEnable(GL_NORMALIZE));
	}

	if (m_bUseTransform)
	{
		CLU_OGL_CALL(glTranslatef(m_vexTrans1[0], m_vexTrans1[1], m_vexTrans1[2]));
		CLU_OGL_CALL(glRotatef(m_fRotAngle, m_vexRotAxis[0], m_vexRotAxis[1], m_vexRotAxis[2]));
		CLU_OGL_CALL(glTranslatef(m_vexTrans2[0], m_vexTrans2[1], m_vexTrans2[2]));
	}

	if (m_bUseScaling)
	{
		CLU_OGL_CALL(glScalef(m_vexScale[0], m_vexScale[1], m_vexScale[2]));
		CLU_OGL_CALL(glEnable(GL_NORMALIZE));
	}

	bool bCullFace = (m_bUsePolyCull || (m_bUsePolyCullWhenTransparent && (pfFrontColor[3] < 0.9999f)));
	if (bCullFace)
	{
		CLU_OGL_CALL(glEnable(GL_CULL_FACE));
	}

	if (m_bPolygonOffsetFill)
	{
		CLU_OGL_CALL(glEnable(GL_POLYGON_OFFSET_FILL));
		CLU_OGL_CALL(glPolygonOffset(m_fPOFX, m_fPOFY));
	}

	if (m_bUsePolygonMode)
	{
		CLU_OGL_CALL(glPolygonMode(m_ePolyFace, m_ePolyMode));
	}
	else
	{
		CLU_OGL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
	}

	// Store front face
	int iFrontFace;
	CLU_OGL_CALL(glGetIntegerv(GL_FRONT_FACE, &iFrontFace));

	// Check whether frame has negative determinant. If yes, have to use inverted front face.
	if (rData.bAutoAdaptFrontFace && rData.bInvertFrontFace)
	{
		CLU_OGL_CALL(glFrontFace((m_ePolyFrontFace == GL_CCW) ? GL_CW : GL_CCW));
	}
	else
	{
		CLU_OGL_CALL(glFrontFace(m_ePolyFrontFace));
	}

	// /////////////////////////////////////////////////////////////////////////////////////////
	// /////////////////////////////////////////////////////////////////////////////////////////
	// /////////////////////////////////////////////////////////////////////////////////////////

	// If a shader of version >= 1.4 is active, set standard uniform variables
	if (bShader_1_4)
	{
		bool bProjMatLoaded  = false;
		bool bModelMatLoaded = false;

		int iUniformLocation;
		/************************************************************************/
		/* clu_matProjection                                                    */
		/************************************************************************/
		if ((iUniformLocation = glGetUniformLocation(uShaderID, "clu_matProjection")) >= 0)
		{
			_LoadMatrixProjection();
			glUniformMatrix4fv(iUniformLocation, 1, GL_FALSE, (const GLfloat*) m_matProjection.DataPointer());
			bProjMatLoaded = true;
		}

		/************************************************************************/
		/* clu_matInvProjection                                                 */
		/************************************************************************/
		if ((iUniformLocation = glGetUniformLocation(uShaderID, "clu_matInvProjection")) >= 0)
		{
			if (!bProjMatLoaded)
			{
				_LoadMatrixProjection();
				bProjMatLoaded = true;
			}

			if (m_bMatrixChangedProjection)
			{
				m_matInvProjection = InvertMatrix(m_matProjection);
			}
			glUniformMatrix4fv(iUniformLocation, 1, GL_FALSE, (const GLfloat*) m_matInvProjection.DataPointer());
		}

		/************************************************************************/
		/* clu_matModelView                                                     */
		/************************************************************************/
		if ((iUniformLocation = glGetUniformLocation(uShaderID, "clu_matModelView")) >= 0)
		{
			_LoadMatrixModelView();
			glUniformMatrix4fv(iUniformLocation, 1, GL_FALSE, (const GLfloat*) m_matModelView.DataPointer());
			bModelMatLoaded = true;
		}

		/************************************************************************/
		/* clu_matInvModelView                                                  */
		/************************************************************************/
		if ((iUniformLocation = glGetUniformLocation(uShaderID, "clu_matInvModelView")) >= 0)
		{
			if (!bModelMatLoaded)
			{
				_LoadMatrixModelView();
				bModelMatLoaded = true;
			}

			if (m_bMatrixChangedModelView)
			{
				m_matInvModelView = InvertMatrix(m_matModelView);
			}
			glUniformMatrix4fv(iUniformLocation, 1, GL_FALSE, (const GLfloat*) m_matInvModelView.DataPointer());
		}

		int iLocMVP    = glGetUniformLocation(uShaderID, "clu_matModelViewProjection");
		int iLocInvMVP = glGetUniformLocation(uShaderID, "clu_matInvModelViewProjection");
		int iLocOC     = glGetUniformLocation(uShaderID, "clu_vOpticalCenter");

		if ((iLocMVP >= 0) || (iLocInvMVP >= 0) || (iLocOC >= 0))
		{
			if (!bModelMatLoaded)
			{
				_LoadMatrixModelView();
			}

			if (!bProjMatLoaded)
			{
				_LoadMatrixProjection();
			}

			// Matrix product for column-major matrices
			for (int iLeftRow = 0; iLeftRow < 4; ++iLeftRow)
			{
				for (int iRightCol = 0; iRightCol < 4; ++iRightCol)
				{
					float* pfLeft   = &m_matProjection(0, iLeftRow);
					float* pfRight  = &m_matModelView(iRightCol, 0);
					float* pfResult = &(m_matModelViewProj(iRightCol, iLeftRow));

					(*pfResult) = 0.0f;

					for (int i = 0; i < 4; ++i, pfLeft += 4, ++pfRight)
					{
						(*pfResult) += (*pfLeft) * (*pfRight);
					}
				}
			}

			if (((iLocInvMVP >= 0) || (iLocOC >= 0)) && (m_bMatrixChangedModelView || m_bMatrixChangedProjection))
			{
				m_matInvModelViewProj = InvertMatrix(m_matModelViewProj);
			}

			if (iLocOC >= 0)
			{
				// Fourth column of inverse model view projection is position of optical center in model view frame.
				glUniform4fv(iLocOC, 1, (const GLfloat*) &(m_matInvModelViewProj(3, 0)));
			}

			if (iLocInvMVP >= 0)
			{
				glUniformMatrix4fv(iLocInvMVP, 1, GL_FALSE, (const GLfloat*) m_matInvModelViewProj.DataPointer());
			}

			if (iLocMVP >= 0)
			{
				glUniformMatrix4fv(iLocMVP, 1, GL_FALSE, (const GLfloat*) m_matModelViewProj.DataPointer());
			}
		}

		const char* ppcTexMatrixName[] = { "clu_matTex0", "clu_matTex1", "clu_matTex2", "clu_matTex3", "clu_matTex4", "clu_matTex5", "clu_matTex6", "clu_matTex7" };

		for (int iUsedTexUnitIdx = 0; iUsedTexUnitIdx < iUsedTexUnitCnt; ++iUsedTexUnitIdx)
		{
			int iTexUnit = piUsedTexUnit[iUsedTexUnitIdx];	// TODO: This is different than all other loops below

			glActiveTexture(GL_TEXTURE0 + iTexUnit);

			if ((iUniformLocation = glGetUniformLocation(uShaderID, ppcTexMatrixName[iTexUnit])) >= 0)
			{
				float pfMatrix[16];
				glGetFloatv(GL_TEXTURE_MATRIX, pfMatrix);
				glUniformMatrix4fv(iUniformLocation, 1, GL_TRUE, (const GLfloat*) pfMatrix);
			}
		}

		if ((iUniformLocation = glGetUniformLocation(uShaderID, "clu_piViewport")) >= 0)
		{
			GLint piViewport[4];
			glGetIntegerv(GL_VIEWPORT, piViewport);
			glUniform1iv(iUniformLocation, 4, (const GLint*) piViewport);
		}

		if ((iUniformLocation = glGetUniformLocation(uShaderID, "clu_pfDepthRange")) >= 0)
		{
			GLfloat pfDepthRange[2];
			glGetFloatv(GL_DEPTH_RANGE, pfDepthRange);
			glUniform1fv(iUniformLocation, 2, (const GLfloat*) pfDepthRange);
		}

		if ((iUniformLocation = glGetUniformLocation(uShaderID, "clu_vColor")) >= 0)
		{
			glUniform4fv(iUniformLocation, 1, (const GLfloat*) rData.pfCurColor);
		}

		if ((iUniformLocation = glGetUniformLocation(uShaderID, "clu_fPointSize")) >= 0)
		{
			float fValue;
			glGetFloatv(GL_POINT_SIZE, &fValue);
			glUniform1fv(iUniformLocation, 1, (const GLfloat*) &fValue);
		}

		if ((iUniformLocation = glGetUniformLocation(uShaderID, "clu_pfPointSizeRange")) >= 0)
		{
			float pfValues[2];
			glGetFloatv(GL_POINT_SIZE_RANGE, pfValues);
			glUniform1fv(iUniformLocation, 2, (const GLfloat*) pfValues);
		}

		if ((iUniformLocation = glGetUniformLocation(uShaderID, "clu_pfPointDistanceAttenuation")) >= 0)
		{
			float pfValues[3];
			glGetFloatv(GL_POINT_DISTANCE_ATTENUATION, pfValues);
			glUniform1fv(iUniformLocation, 3, (const GLfloat*) pfValues);
		}

		if ((iUniformLocation = glGetUniformLocation(uShaderID, "clu_fPointFadeThresholdSize")) >= 0)
		{
			float fValue;
			glGetFloatv(GL_POINT_FADE_THRESHOLD_SIZE, &fValue);
			glUniform1fv(iUniformLocation, 1, (const GLfloat*) &fValue);
		}

		if ((iUniformLocation = glGetUniformLocation(uShaderID, "clu_fLineWidth")) >= 0)
		{
			float fValue;
			glGetFloatv(GL_LINE_WIDTH, &fValue);
			glUniform1fv(iUniformLocation, 1, (const GLfloat*) &fValue);
		}

		if ((iUniformLocation = glGetUniformLocation(uShaderID, "clu_pvClipPlanes")) >= 0)
		{
			const int iMaxClipPlanes = int(rData.mClipPlanes.Count() / 4);
			double pdValue[4];

			for (int iClipPlane = 0; iClipPlane < iMaxClipPlanes; ++iClipPlane)
			{
				if (glIsEnabled(GL_CLIP_PLANE0 + iClipPlane))
				{
					glGetClipPlane(GL_CLIP_PLANE0 + iClipPlane, pdValue);
					for (int i = 0, iArrayPos = iClipPlane * 4; i < 4; ++i, ++iArrayPos)
					{
						rData.mClipPlanes[iArrayPos] = float(pdValue[i]);
					}
				}
				else
				{
					memset(&rData.mClipPlanes[iClipPlane * 4], 0, 4 * sizeof(float));
				}
			}

			glUniform4fv(iUniformLocation, iMaxClipPlanes, rData.mClipPlanes.Data());
		}
	}

	// /////////////////////////////////////////////////////////////////////////////////////////
	// /////////////////////////////////////////////////////////////////////////////////////////
	// /////////////////////////////////////////////////////////////////////////////////////////

	// Draw vertex array
	CLU_OGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_uVexBufID));

	// Only copy vertex data to device if it has been modified
	if (m_bVexModified)
	{
		m_bVexModified = false;
		Clu::OpenGL::BufferData(GL_ARRAY_BUFFER, m_iVexCnt * sizeof(SData), m_mDataList.Data(), GL_STATIC_DRAW);

		if (!m_bKeepDataOnHost)
		{
			m_mDataList.Set(0);
			m_mDataList.Prune();
		}
	}

	if (bShader_1_4)
	{
		glEnableVertexAttribArray(CLUGL_VAA_VERTEX);
		glVertexAttribPointer(CLUGL_VAA_VERTEX, 3, GL_FLOAT, GL_FALSE, sizeof(SData), BUFFER_OFFSET(SData::iOffsetVex));

		if (bUseColor)
		{
			glEnableVertexAttribArray(CLUGL_VAA_COLOR);
			glVertexAttribPointer(CLUGL_VAA_COLOR, 4, GL_FLOAT, GL_TRUE, sizeof(SData), BUFFER_OFFSET(SData::iOffsetCol));
		}
		else if (bUsePartIdAsColor)
		{
			glEnableVertexAttribArray(CLUGL_VAA_COLOR);
			glVertexAttribPointer(CLUGL_VAA_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(SData), BUFFER_OFFSET(SData::iOffsetPartId));
		}
		else
		{
			glDisableVertexAttribArray(CLUGL_VAA_COLOR);
		}

		if (bUseNorm)
		{
			glEnableVertexAttribArray(CLUGL_VAA_NORMAL);
			glVertexAttribPointer(CLUGL_VAA_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(SData), BUFFER_OFFSET(SData::iOffsetNorm));
		}
		else
		{
			glDisableVertexAttribArray(CLUGL_VAA_NORMAL);
		}
	}
	else
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, sizeof(SData), BUFFER_OFFSET(SData::iOffsetVex));

		if (bUseColor)
		{
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(4, GL_FLOAT, sizeof(SData), BUFFER_OFFSET(SData::iOffsetCol));
		}
		else if (bUsePartIdAsColor)
		{
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(SData), BUFFER_OFFSET(SData::iOffsetPartId));
		}
		else
		{
			glDisableClientState(GL_COLOR_ARRAY);
		}

		if (bUseNorm)
		{
			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(GL_FLOAT, sizeof(SData), BUFFER_OFFSET(SData::iOffsetNorm));
		}
		else
		{
			glDisableClientState(GL_NORMAL_ARRAY);
		}
	}

	if (bUsePartId && (glVertexAttribIPointer != nullptr))
	{
		glEnableVertexAttribArray(CLUGL_VAA_PARTID);
		glVertexAttribIPointer(CLUGL_VAA_PARTID, 1, GL_INT, sizeof(SData), BUFFER_OFFSET(SData::iOffsetPartId));
	}
	else
	{
		glDisableVertexAttribArray(CLUGL_VAA_PARTID);
	}

	if (bUseTex)
	{
		if (bShader_1_4)
		{
			const char* ppcTexUnitName[] = { "clu_texUnit0", "clu_texUnit1", "clu_texUnit2", "clu_texUnit3", "clu_texUnit4", "clu_texUnit5", "clu_texUnit6", "clu_texUnit7" };
			if (glActiveTexture)
			{
				for (int iUsedTexUnitIdx = 0; iUsedTexUnitIdx < OGL_MAX_TEX_UNITS; ++iUsedTexUnitIdx)
				{
					glActiveTexture(GL_TEXTURE0 + iUsedTexUnitIdx);
					glEnable(GL_TEXTURE_2D);

					int iLoc = 0;
					if ((iLoc = glGetUniformLocation(uShaderID, ppcTexUnitName[iUsedTexUnitIdx])) >= 0)
					{
						glUniform1i(iLoc, iUsedTexUnitIdx);
					}

					glEnableVertexAttribArray(CLUGL_VAA_TEX0 + iUsedTexUnitIdx);
					glVertexAttribPointer(CLUGL_VAA_TEX0 + iUsedTexUnitIdx, 3, GL_FLOAT, GL_TRUE, sizeof(SData), BUFFER_OFFSET(SData::iOffsetTex));
				}

				// If there is a shader but no texture then still enable the texture coordinates
				if ((uShaderID > 0) && (iUsedTexUnitCnt == 0))
				{
					glEnableVertexAttribArray(CLUGL_VAA_TEX0);
					glVertexAttribPointer(CLUGL_VAA_TEX0, 3, GL_FLOAT, GL_TRUE, sizeof(SData), BUFFER_OFFSET(SData::iOffsetTex));
				}
			}
			else
			{
				glEnable(GL_TEXTURE_2D);

				int iLoc = 0;
				if ((iLoc = glGetUniformLocation(uShaderID, ppcTexUnitName[0])) >= 0)
				{
					glUniform1i(iLoc, 0);
				}

				glEnableVertexAttribArray(CLUGL_VAA_TEX0);
				glVertexAttribPointer(CLUGL_VAA_TEX0, 3, GL_FLOAT, GL_TRUE, sizeof(SData), BUFFER_OFFSET(SData::iOffsetTex));
			}
		}
		else
		{
			if (glActiveTexture)
			{
				for (int iUsedTexUnitIdx = 0; iUsedTexUnitIdx < OGL_MAX_TEX_UNITS; ++iUsedTexUnitIdx)
				{
					glActiveTexture(GL_TEXTURE0 + iUsedTexUnitIdx);
					glEnable(GL_TEXTURE_2D);

					glClientActiveTexture(GL_TEXTURE0 + iUsedTexUnitIdx);
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glTexCoordPointer(2, GL_FLOAT, sizeof(SData), BUFFER_OFFSET(SData::iOffsetTex));
				}

				// If there is a shader but no texture then still enable the texture coordinates
				if ((uShaderID > 0) && (iUsedTexUnitCnt == 0))
				{
					glClientActiveTexture(GL_TEXTURE0);
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glTexCoordPointer(2, GL_FLOAT, sizeof(SData), BUFFER_OFFSET(SData::iOffsetTex));
				}
			}
			else
			{
				glEnable(GL_TEXTURE_2D);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(2, GL_FLOAT, sizeof(SData), BUFFER_OFFSET(SData::iOffsetTex));
			}
		}
	}

	size_t nIdxListCnt = m_mIdxList.Count();
	if (nIdxListCnt)
	{
		CLU_OGL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uIdxBufID));

		if (m_bExternalVBO)
		{
			m_mIdxListPointers.Set(nIdxListCnt);
			m_mIdxListPointers[0] = BUFFER_OFFSET(0);
		}

		if (m_bIdxModified)
		{
			m_bIdxModified = false;
			m_mIdxListLengths.Set(nIdxListCnt);
			m_mIdxListPointers.Set(nIdxListCnt);

			size_t nTotalIdxCnt = 0;
			for (size_t nIdxListIdx = 0; nIdxListIdx < nIdxListCnt; ++nIdxListIdx)
			{
				size_t nLen = m_mIdxList[nIdxListIdx].Count();
				m_mIdxListLengths[nIdxListIdx] = (GLsizei) nLen;
				nTotalIdxCnt                  += nLen;
			}

			Clu::OpenGL::BufferData(GL_ELEMENT_ARRAY_BUFFER, nTotalIdxCnt * sizeof(unsigned), NULL, GL_STATIC_DRAW);

			size_t nIdxListPos = 0;
			for (size_t nIdxListIdx = 0; nIdxListIdx < nIdxListCnt; ++nIdxListIdx)
			{
				glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
						nIdxListPos * sizeof(unsigned),
						m_mIdxListLengths[nIdxListIdx] * sizeof(unsigned),
						m_mIdxList[nIdxListIdx].Data());

				m_mIdxListPointers[nIdxListIdx] = BUFFER_OFFSET(nIdxListPos * sizeof(unsigned));
				nIdxListPos                    += m_mIdxListLengths[nIdxListIdx];
			}
		}

		if (m_iInstanceCount == 1)
		{
			glMultiDrawElements(m_eMode, (GLsizei*) m_mIdxListLengths.Data(), GL_UNSIGNED_INT,
					(const GLvoid**) m_mIdxListPointers.Data(), (GLsizei) m_mIdxListPointers.Count());
		}
		else if (m_iInstanceCount > 1)
		{
			for (size_t nIdxListIdx = 0; nIdxListIdx < nIdxListCnt; ++nIdxListIdx)
			{
				glDrawElementsInstanced(m_eMode, (GLsizei) m_mIdxListLengths[nIdxListIdx], GL_UNSIGNED_INT, (const GLvoid*) m_mIdxListPointers[nIdxListIdx],
						(GLsizei) m_iInstanceCount);
			}
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	else
	{
		if (m_iInstanceCount == 1)
		{
			glDrawArrays(m_eMode, 0, Count());
		}
		else if (m_iInstanceCount > 1)
		{
			glDrawArraysInstanced(m_eMode, 0, Count(), m_iInstanceCount);
		}
	}

	if (bShader_1_4)
	{
		glDisableVertexAttribArray(CLUGL_VAA_VERTEX);
	}
	else
	{
		glDisableClientState(GL_VERTEX_ARRAY);
	}

	if (bUseColor || bUsePartIdAsColor)
	{
		if (bShader_1_4)
		{
			glDisableVertexAttribArray(CLUGL_VAA_COLOR);
		}
		else
		{
			glDisableClientState(GL_COLOR_ARRAY);
		}
	}

	if (bUsePartId)
	{
		glDisableVertexAttribArray(CLUGL_VAA_PARTID);
	}

	if (bUseNorm)
	{
		if (bShader_1_4)
		{
			glDisableVertexAttribArray(CLUGL_VAA_NORMAL);
		}
		else
		{
			glDisableClientState(GL_NORMAL_ARRAY);
		}
	}

	if (bUseTex)
	{
		if (bShader_1_4)
		{
			if (glActiveTexture)
			{
				for (int iUsedTexUnitIdx = 0; iUsedTexUnitIdx < OGL_MAX_TEX_UNITS; ++iUsedTexUnitIdx)
				{
					glActiveTexture(GL_TEXTURE0 + iUsedTexUnitIdx);
					glDisable(GL_TEXTURE_2D);

					glDisableVertexAttribArray(CLUGL_VAA_TEX0 + iUsedTexUnitIdx);
				}

				// If there is a shader but no texture then disable the texture coordinates
				if ((uShaderID > 0) && (iUsedTexUnitCnt == 0))
				{
					glDisableVertexAttribArray(CLUGL_VAA_TEX0);
				}
			}
			else
			{
				glDisable(GL_TEXTURE_2D);
				glDisableVertexAttribArray(CLUGL_VAA_TEX0);
			}
		}
		else
		{
			if (glActiveTexture)
			{
				for (int iUsedTexUnitIdx = 0; iUsedTexUnitIdx < OGL_MAX_TEX_UNITS; ++iUsedTexUnitIdx)
				{
					glActiveTexture(GL_TEXTURE0 + iUsedTexUnitIdx);
					glDisable(GL_TEXTURE_2D);

					glClientActiveTexture(GL_TEXTURE0 + iUsedTexUnitIdx);
					glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				}

				// If there is a shader but no texture then disable the texture coordinates
				if ((uShaderID > 0) && (iUsedTexUnitCnt == 0))
				{
					glClientActiveTexture(GL_TEXTURE0);
					glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				}
			}
			else
			{
				glDisable(GL_TEXTURE_2D);
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			}
		}
	}

	CLU_OGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));

	// /////////////////////////////////////////////////////////////////////////////////////////
	// /////////////////////////////////////////////////////////////////////////////////////////
	// /////////////////////////////////////////////////////////////////////////////////////////

	CLU_OGL_CALL(glFrontFace(iFrontFace));

	if (bCullFace)
	{
		glDisable(GL_CULL_FACE);
	}

	if (m_bUsePolygonMode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (m_bPolygonOffsetFill)
	{
		glDisable(GL_POLYGON_OFFSET_FILL);
	}

	if (m_bUseScaling || m_bUseGenTransform)
	{
		glDisable(GL_NORMALIZE);
	}

	if (m_bUseTransform || m_bUseGenTransform || m_bUseScaling)
	{
		rData.xMatrixStack.Pop(Clu::CMatrixStack::ModelView);
	}

	if (!bPickMode)
	{
		if (m_bLineStipple)
		{
			glDisable(GL_LINE_STIPPLE);
		}

		if (m_bScaleBackColor)
		{
			glEnable(GL_COLOR_MATERIAL);
			glColor4fv(pfCurrentColor);
			memcpy(rData.pfCurColor, pfCurrentColor, 4 * sizeof(float));
		}
	}

	if (bPickMode)
	{
		// Pop pick name from pick name stack
		rData.PopPickName();
	}

	return true;
}
