////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLVertexList.h
//
// summary:   Declares the ogl vertex list class
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

// OGLVertexList.h: Schnittstelle f�r die Klasse COGLVertexList.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLVERTEXLIST_H__C262712E_1840_43F3_A75E_F76904823DE9__INCLUDED_)
	#define AFX_OGLVERTEXLIST_H__C262712E_1840_43F3_A75E_F76904823DE9__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif	// _MSC_VER > 1000

#include "OGLVertex.h"
#include "OGLColor.h"
#include "OGLBaseElement.h"
#include "OGLBEReference.h"

#include "GL\GL.h"

#include "CluTec.Math/Static.Matrix.h"
#include "CluTec.Math/Static.Matrix.Math.h"

	typedef CPointND<float, 4> TColor;

	class CLUDRAW_API COGLVertexList : public COGLBaseElement
	{
	public:
		using TMat4 = Clu::SMatrix<float, 4>;

		typedef Mem<COGLVertex>         TVexList;
		typedef Mem<TColor>                     TColList;
		typedef Mem<GLboolean>          TEdgeList;
		typedef MemObj< Mem<unsigned> > TIdxList;
		typedef Mem<unsigned>           TPartIdList;

		struct SData
		{
			COGLVertex      xVex;	// 12 bytes; @0
			COGLVertex      xTex;	// 12 bytes; @12
			COGLVertex      xNorm;	// 12 bytes; @24
			TColor          xCol;	// 16 bytes; @36
			GLfloat         fFogZ;	//  4 bytes; @52
			GLuint          uPartId;//  4 bytes; @56
			GLboolean       bEdge;	//  1 byte;  @60
			GLbyte          pbPad[3];	// 3 bytes; @61
			// = 64 bytes

			static const int iOffsetVex    = 0;
			static const int iOffsetTex    = 12;
			static const int iOffsetNorm   = 24;
			static const int iOffsetCol    = 36;
			static const int iOffsetFog    = 52;
			static const int iOffsetPartId = 56;
			static const int iOffsetEdge   = 60;
		};

		typedef Mem<SData> TDataList;

	public:

		COGLVertexList();
		COGLVertexList(const COGLVertexList& rVexList);
		virtual ~COGLVertexList();

		virtual COGLBaseElement* Copy()
		{
			return (COGLBaseElement*) new COGLVertexList(*this);
		}

		COGLVertexList& operator=(const COGLVertexList& rVexList);

		void Reset()
		{
			m_mDataList.Set(0);
			m_mIdxList.Set(0);
			m_iVexCnt         = m_iNormCnt = m_iTexCnt = m_iColCnt = m_iEdgeCnt = m_iPartIdCnt = 0;
			m_bVexModified    = true;
			m_bIdxModified    = true;
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

		void ResetVexList() { m_iVexCnt = 0; AdjustDataListSize(); }
		void ResetTexList() { m_iTexCnt = 0; AdjustDataListSize(); }
		void ResetNormList() { m_iNormCnt = 0; AdjustDataListSize(); }
		void ResetColList() { m_iColCnt = 0; AdjustDataListSize(); }
		void ResetEdgeList() { m_iEdgeCnt = 0; AdjustDataListSize(); }
		void ResetPartIdList() { m_iPartIdCnt = 0; AdjustDataListSize(); }

		TMat4 InvertMatrix(const TMat4& mA);

		// Copy from memory, or if dataPtr is null -> take external vertexBufferID
		void ApplyVertexData(GLenum eMode,
				size_t iCount,
				const SData* pData,
				bool bUseVex,
				bool bUseTex,
				bool bUseNorm,
				bool bUseCol,
				bool bUseFogZ,
				bool bUseEdge,
				bool bEnablePartPicking,
				unsigned uVertexBufferID);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Sets the vertex and the index data using the given vertex list.
		/// </summary>
		///
		/// <param name="xSource"> The data source. </param>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void ApplyVertexAndIndexData(const COGLVertexList& xSource);

		// Set Index list from arrays
		bool SetIndexLists(const unsigned** const ppuIndexLists, const unsigned* const puIndexListLengths, unsigned uIndexListCount, unsigned puIndexBufferID);

		// Reserve memory for the given number of vertices
		bool Reserve(size_t iCnt) { return m_mDataList.Set(iCnt); }
		// Reserve memory for the given number of vertices
		bool ReserveAdd(int iCnt) { return m_mDataList.Add(iCnt); }

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Return the number of used elements in data list. The total count of elements in m_mDataList is only the number of
		/// 	reserved vertices not the number of elements that should be drawn.
		/// </summary>
		///
		/// <returns> . </returns>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		int Count() const
		{
			return m_iVexCnt;
		}

		bool IsKeepDataOnHostEnabled()
		{
			return m_bKeepDataOnHost;
		}

		void EnableKeepDataOnHost(bool bEnable)
		{
			m_bKeepDataOnHost = bEnable;
		}

		int GetVexCount() { return m_iVexCnt; }
		int GetTexCount() { return m_iTexCnt; }
		int GetNormCount() { return m_iNormCnt; }
		int GetColCount() { return m_iColCnt; }
		int GetEdgeCount() { return m_iEdgeCnt; }

		// Update internal normal count, so that normals are drawn
		void UpdateNormCount() { m_iNormCnt = int(m_mDataList.Count()); }
		void UpdateVexCount() { m_iVexCnt = int(m_mDataList.Count()); }
		void UpdateTexCount() { m_iTexCnt = int(m_mDataList.Count()); }
		void UpdateColCount() { m_iColCnt = int(m_mDataList.Count()); }
		void UpdateEdgeCount() { m_iEdgeCnt = int(m_mDataList.Count()); }
		void UpdatePartIdCount() { m_iPartIdCnt = int(m_mDataList.Count()); }

		COGLVertexList& operator<<(const COGLVertex& rVex)
		{ AddVex(rVex); return *this; }

		COGLVertexList& operator<<(const COGLColor& rCol)
		{ AddCol(rCol); return *this; }

		COGLVertexList& operator<<(Mem<unsigned>& mIdx)
		{ SetIdxList(mIdx); return *this; }

		COGLVertexList& operator<<(GLenum eMode)
		{ SetMode(eMode); return *this; }

		SData& operator[](size_t i) { m_bVexModified = true; return m_mDataList[i]; }
		//COGLVertex& GetTex(int iPos) { return m_mDataList[(uint)iPos].xTex; }
		//COGLVertex& GetNormal(int iPos) { return m_mDataList[(uint)iPos].xNorm; }
		//TColor& GetColor(int iPos) { return m_mColList[(uint)iPos]; }
		unsigned& GetIndex(int iList, int iPos) { m_bIdxModified = true; return m_mIdxList[(uint) iList][(uint) iPos]; }

		bool AddVex(const COGLVertex& rVex);
		bool AddVex(const float* pfVex);
		bool AddVex(float fX, float fY, float fZ);
		bool AddVexRange(const TVexList& mVex);

		bool AddTex(const COGLVertex& rVex);
		bool AddTex(const float* pfVex);
		bool AddTex(float fX, float fY, float fZ);
		bool AddTexRange(const TVexList& mVex);

		bool AddCol(const COGLColor& rCol);
		bool AddCol(const float* pfCol);
		bool AddCol(float fR, float fG, float fB, float fA = 0.0f);
		bool AddColRange(const MemObj<COGLColor>& mCol);

		bool AddNormal(const COGLVertex& rVex);
		bool AddNormal(const float* pfVex);
		bool AddNormal(float fX, float fY, float fZ);
		bool AddNormalRange(const TVexList& mVex);

		bool AddPartIdRange(const TPartIdList& mPartIdList);

		bool SetIdxList(size_t nNo, unsigned* pIdx);
		bool SetIdxList(Mem<unsigned>& mIdx);
		bool AddIdxList(size_t nNo, unsigned* pIdx);
		bool AddIdxList(Mem<unsigned>& mIdx);

		void SetMode(GLenum eMode) { m_eMode = eMode; }
		GLenum GetMode() const { return m_eMode; }

		void SetPolygonOffsetFill(float fX, float fY)
		{ m_bPolygonOffsetFill = (fX != 0.0f || fY != 0.0f); m_fPOFX = fX; m_fPOFY = fY; }

		void GetPolygonOffsetFill(float& fX, float& fY)
		{ fX = m_fPOFX; fY = m_fPOFY; }

		void InvertNormals(float fFac = 1.0f);

		void EnableScaleBackColor(bool bVal = true)
		{ m_bScaleBackColor = bVal; }

		bool UseScaleBackColor()
		{ return m_bScaleBackColor; }

		void SetBackColorScale(float fVal)
		{ m_fBackColorScale = fVal; }

		void GetBackColorScale(float& fVal)
		{ fVal = m_fBackColorScale; }

		void EnableLineStipple(bool bVal = true)
		{ m_bLineStipple = bVal; }

		bool UseLineStipple()
		{ return m_bLineStipple; }

		void SetLineStipple(int iFactor, int iPattern)
		{ m_iLineStippleFactor = iFactor; m_iLineStipplePattern = iPattern; }

		void GetLineStipple(int& iFactor, int& iPattern)
		{ iFactor = m_iLineStippleFactor; iPattern = m_iLineStipplePattern; }

		void EnableOverrideAlpha(bool bVal = true)
		{ m_bOverrideAlpha = bVal; }

		bool UseOverrideAlpha()
		{ return m_bOverrideAlpha; }

		void SetOverrideAlphaValue(float fValue)
		{ m_fOverrideAlphaValue = fValue; }

		void GetOverrideAlpha(float& fValue)
		{ fValue = m_fOverrideAlphaValue; }

		void EnableTransform(bool bVal = true)
		{ m_bUseTransform = bVal; }

		bool UseTransform()
		{ return m_bUseTransform; }

		void SetTranslation1(const COGLVertex& rVex)
		{ m_vexTrans1 = rVex; }

		void GetTranslation1(COGLVertex& rVex)
		{ rVex = m_vexTrans1; }

		void SetTranslation2(const COGLVertex& rVex)
		{ m_vexTrans2 = rVex; }

		void GetTranslation2(COGLVertex& rVex)
		{ rVex = m_vexTrans2; }

		void SetRotation(float fAngle, const COGLVertex& rAxis)
		{ m_fRotAngle = fAngle; m_vexRotAxis = rAxis; }

		void GetRotation(float& fAngle, COGLVertex& rAxis)
		{ fAngle = m_fRotAngle; rAxis = m_vexRotAxis; }

		void EnableScaling(bool bVal = true)
		{ m_bUseScaling = bVal; }

		void SetScaling(const COGLVertex& rVex)
		{ m_vexScale = rVex; }

		bool UseScaling()
		{ return m_bUseScaling; }

		void GetScaling(COGLVertex& rVex)
		{ rVex = m_vexScale; }

		void EnableGenTransform(bool bVal = true)
		{ m_bUseGenTransform = bVal; }

		bool UseGenTransform()
		{ return m_bUseGenTransform; }

		void SetGenTransformMatrix(const Matrix<float>& matTrans)
		{ m_matTrans = matTrans; m_matTrans = ~m_matTrans; }

		void GetGenTransformMatrix(Matrix<float>& matTrans)
		{ matTrans = m_matTrans; matTrans = ~matTrans; }

		void EnablePolygonCull(bool bVal = true)
		{ m_bUsePolyCull = bVal; }

		void EnablePolygonCullWhenTransparent(bool bVal = true)
		{ m_bUsePolyCullWhenTransparent = bVal; }

		void EnablePolygonMode(bool bVal = true)
		{ m_bUsePolygonMode = bVal; }

		void SetPolygonMode(GLenum eFace, GLenum eMode)
		{ m_ePolyFace = eFace; m_ePolyMode = eMode; }

		bool UsePolygonMode()
		{ return m_bUsePolygonMode; }

		void GetPolygonMode(GLenum& eFace, GLenum& eMode)
		{ eFace = m_ePolyFace; eMode = m_ePolyMode; }

		void SetFrontFace(GLenum eFrontFace)
		{ m_ePolyFrontFace = eFrontFace; }

		int GetFrontFace()
		{ return m_ePolyFrontFace; }

		void SetInstanceCount(int iCount)
		{
			m_iInstanceCount = iCount;
		}

		int GetInstanceCount()
		{
			return m_iInstanceCount;
		}

		bool Apply(COGLBaseElement::EApplyMode eMode, COGLBaseElement::SApplyData& rData);

		//TDataList& GetDataList() { return m_mDataList; }
		//TVexList& GetVexList() { return m_mVexList; }
		//TVexList& GetNormList() { return m_mNormalList; }
		//TVexList& GetTexList() { return m_mTexList; }
		//TColList& GetColList() { return m_mColList; }
		TIdxList& GetIdxList() { m_bIdxModified = true; return m_mIdxList; }

		//Set / Get the VertexBuffer ID
		unsigned GetVertexBufferID() { return m_uVexBufID; };
		void SetVertexBufferID(unsigned int uVboID) { m_uVexBufID = uVboID; };

	protected:

		void AdjustDataListSize()
		{
			int iCnt = std::max(std::max(std::max(std::max(std::max(m_iVexCnt, m_iNormCnt), m_iTexCnt), m_iColCnt), m_iEdgeCnt), m_iPartIdCnt);
			m_mDataList.Set(iCnt);
		}

		void _LoadMatrixProjection();
		void _LoadMatrixModelView();

	protected:

		// flag to determine an external created VBO
		bool m_bExternalVBO;

		// Flag to indicate whether the vertex list data should be kept
		// in host memory after drawing. This is usually not necessary,
		// since the data is copied to the graphics device and kept there.
		bool m_bKeepDataOnHost;

		GLenum m_eMode;
		TDataList m_mDataList;
		int m_iVexCnt, m_iNormCnt, m_iTexCnt, m_iColCnt, m_iEdgeCnt, m_iPartIdCnt;

		// ID of buffer object used by vertex list
		unsigned m_uVexBufID;
		unsigned m_uIdxBufID;

		bool m_bVexModified;
		bool m_bIdxModified;

		TIdxList m_mIdxList;
		// These two arrays are used to store information for
		// glMultiDrawElements calls.
		Mem<GLsizei> m_mIdxListLengths;
		Mem<GLvoid*> m_mIdxListPointers;

		// If true the color of the back facing polygons is scaled
		bool m_bScaleBackColor;

		// Scale of color for back facing polygons
		float m_fBackColorScale;

		// If true then line stipple is used
		bool m_bLineStipple;

		// Line Stipple Pattern
		int m_iLineStipplePattern;

		// Line Stipple Factor
		int m_iLineStippleFactor;

		// Override Alpha Value
		bool m_bOverrideAlpha;

		// Applied alpha value
		float m_fOverrideAlphaValue;

		// Use transformation
		bool m_bUseTransform;

		// Use general transformation
		bool m_bUseGenTransform;

		// Use Polygon Culling
		bool m_bUsePolyCull;

		// Use Polygon Culling if the current color is transparent
		bool m_bUsePolyCullWhenTransparent;

		// Instance Count for instanced drawing.
		int m_iInstanceCount;

		// General transformation matrix
		Matrix<float> m_matTrans;

		// Translations of Vertex List
		COGLVertex m_vexTrans1, m_vexTrans2;

		// Rotation axis of Vertex List
		COGLVertex m_vexRotAxis;

		// Rotation angle
		float m_fRotAngle;

		// Use scaling
		bool m_bUseScaling;

		// Scale of Vertex List
		COGLVertex m_vexScale;

		// Polygon Mode and Face
		bool m_bUsePolygonMode;
		GLenum m_ePolyMode, m_ePolyFace;

		// Polygon Offset Fill data
		bool m_bPolygonOffsetFill;
		float m_fPOFX, m_fPOFY;

		// Polygon Face
		GLenum m_ePolyFrontFace;

		Clu::SMatrix<float, 4> m_matModelViewProj;
		Clu::SMatrix<float, 4> m_matInvModelViewProj;

		Clu::SMatrix<float, 4> m_matProjection;
		Clu::SMatrix<float, 4> m_matInvProjection;

		Clu::SMatrix<float, 4> m_matModelView;
		Clu::SMatrix<float, 4> m_matInvModelView;

		bool m_bMatrixChangedProjection;
		bool m_bMatrixChangedModelView;
	};

#endif	// !defined(AFX_OGLVERTEXLIST_H__C262712E_1840_43F3_A75E_F76904823DE9__INCLUDED_)
