////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      CLUCodeBase_VexList.cpp
//
// summary:   Implements the clu code base vex list class
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

// CLUCodeBase.cpp: Implementierung der Klasse CCLUCodeBase.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
//#include "CvFltk\glut.h"

#include "CLUCodeBase.h"
#include "CluTec.Viz.Draw\OGLVertexList.h"
#include "CluTec.Viz.Base\TensorOperators.h"
#include "CluTec.Viz.Draw\OGLDrawBase.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Get Vex List count
//
// 1. (vexlist) the vertex list
// 2. (var) A list containing the count of [vex, norm, tex, col, idx] will be written into here
//
// Return:
//	true/false

bool CCLUCodeBase::VexListGetSize(COGLVertexList& rVexList, CCodeVar& rVar, int iLine, int iPos)
{
	rVar.New(PDT_VARLIST, "Constant");
	TVarList& rList = *rVar.GetVarListPtr();
	rList.Set(5);

	rList(0) = TCVCounter(rVexList.GetVexCount());
	rList(1) = TCVCounter(rVexList.GetNormCount());
	rList(2) = TCVCounter(rVexList.GetTexCount());
	rList(3) = TCVCounter(rVexList.GetColCount());
	rList(4) = TCVCounter(rVexList.GetIdxList().Count());

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Create Object Form: Grid
///
/// Parameters:
///		1. x-extent
///		2. y-extent
///		3. x-count vertices
///		4. y-count vertices

bool CCLUCodeBase::VexListCreateFormGrid(COGLVertexList& rVexList, TVarList& rPar, int iLine, int iPos)
{
	int iParCnt = int(rPar.Count());
	int iCol, iRow, iColCnt, iRowCnt;
	TCVScalar dW, dH;

	if (iParCnt != 4)
	{
		int piPar[] = { 4 };

		m_ErrorList.WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (!rPar(0).CastToScalar(dW))
	{
		m_ErrorList.GeneralError("First parameter of grid form has to give the extent along the x-direction.", iLine, iPos);
		return false;
	}

	if (!rPar(1).CastToScalar(dH))
	{
		m_ErrorList.GeneralError("Second parameter of grid form has to give the extent along the y-direction.", iLine, iPos);
		return false;
	}

	if (!rPar(2).CastToCounter(iColCnt))
	{
		m_ErrorList.GeneralError("Third parameter of grid form has to give the number of vertices along the x-direction.", iLine, iPos);
		return false;
	}

	if (!rPar(3).CastToCounter(iRowCnt))
	{
		m_ErrorList.GeneralError("Fourth parameter of grid form has to give the number of vertices along the y-direction.", iLine, iPos);
		return false;
	}

	if (dW <= 1e-6)
	{
		m_ErrorList.GeneralError("The x-extent has to be greater than zero.", iLine, iPos);
		return false;
	}

	if (dH <= 1e-6)
	{
		m_ErrorList.GeneralError("The y-extent has to be greater than zero.", iLine, iPos);
		return false;
	}

	if (iColCnt < 2)
	{
		m_ErrorList.GeneralError("The number of vertices along the x-direction has to be greater than 1.", iLine, iPos);
		return false;
	}

	if (iRowCnt < 2)
	{
		m_ErrorList.GeneralError("The number of vertices along the y-direction has to be greater than 1.", iLine, iPos);
		return false;
	}

	Mem<COGLVertex> mVex;
	Mem<COGLVertex> mNorm;
	Mem<COGLVertex> mTex;
	MemObj<COGLColor> mCol;

	int iVex, iVexCnt = iRowCnt * iColCnt;

	mVex.Set(iVexCnt);
	mNorm.Set(iVexCnt);
	mTex.Set(iVexCnt);

	float fX, fY, fStepX, fStepY;
	float fTX, fTY, fStepTX, fStepTY;

	fStepX = float(dW / double(iColCnt - 1));
	fStepY = float(dH / double(iRowCnt - 1));

	fStepTX = float(1.0f / double(iColCnt - 1));
	fStepTY = float(1.0f / double(iRowCnt - 1));

	fY  = 0.0;
	fTY = 0.0;
	for (iRow = 0, iVex = 0; iRow < iRowCnt; ++iRow, fY += fStepY, fTY += fStepTY)
	{
		fX  = 0.0;
		fTX = 0.0;
		for (iCol = 0; iCol < iColCnt; ++iCol, fX += fStepX, fTX += fStepTX, iVex++)
		{
			mVex[iVex].Set(fX, fY, 0.0f);
			mNorm[iVex].Set(0.0f, 0.0f, 1.0f);
			mTex[iVex].Set(fTX, fTY, 0.0f);
		}
	}

	if (!m_pDrawBase->GenVexPointSurface(iRowCnt, iColCnt,
			    mVex, mNorm, mTex, mCol,
			    0.0f, &rVexList, 0))
	{
		m_ErrorList.GeneralError("Error generating grid object.", iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Create Object Form: Line Strip
///
/// Parameters:
///		1. x-component of line vector
///		2. y-component of line vector
///		3. number of divisions along line

bool CCLUCodeBase::VexListCreateFormLineStrip(COGLVertexList& rVexList, TVarList& rPar, int iLine, int iPos)
{
	int iParCnt = int(rPar.Count());
	int iEl, iElCnt;
	TCVScalar dX;

	if (iParCnt != 2)
	{
		int piPar[] = { 2 };

		m_ErrorList.WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (!rPar(0).CastToScalar(dX))
	{
		m_ErrorList.GeneralError("First parameter of grid form has to give the length of the line strip along the x-axis.", iLine, iPos);
		return false;
	}

	if (!rPar(1).CastToCounter(iElCnt))
	{
		m_ErrorList.GeneralError("Second parameter of grid form has to give the number of vertices along line strip.", iLine, iPos);
		return false;
	}

	if (Mag(dX) <= 1e-6)
	{
		m_ErrorList.GeneralError("The line strip length has to be greater than zero.", iLine, iPos);
		return false;
	}

	if (iElCnt < 2)
	{
		m_ErrorList.GeneralError("The number of vertices along the the line strip has to be greater than 1.", iLine, iPos);
		return false;
	}

	Mem<COGLVertex> mVex;
	Mem<COGLVertex> mNorm;
	Mem<COGLVertex> mTex;
	MemObj<COGLColor> mCol;

	int iVex, iVexCnt = iElCnt;

	mVex.Set(iVexCnt);
	mNorm.Set(iVexCnt);
	mTex.Set(iVexCnt);

	float fX, fStepX;
	float fTX, fStepTX;

	fStepX  = float(dX / double(iElCnt - 1));
	fStepTX = float(1.0f / double(iElCnt - 1));

	fX  = 0.0;
	fTX = 0.0;
	for (iEl = 0, iVex = 0; iEl < iElCnt; ++iEl, fX += fStepX, fTX += fStepTX, iVex++)
	{
		mVex[iVex].Set(fX, 0.0f, 0.0f);
		mNorm[iVex].Set(0.0f, 0.0f, 1.0f);
		mTex[iVex].Set(fTX, 0.0f, 0.0f);
	}

	rVexList.Reset();
	rVexList.SetMode(GL_LINE_STRIP);
	rVexList.AddNormalRange(mNorm);
	rVexList.AddVexRange(mVex);
	rVexList.AddTexRange(mTex);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add Data to vertex list
/// This function is e.g. called by the operator '<<'.
/// iDataType: Type of data entered. 0: Vertex, 1: Normal, 2: Texture, 3: Color, 4: Index
/// iIdxListID: ID of active index list

bool CCLUCodeBase::VexListAddData(COGLVertexList* pVexList, CCodeVar& rVar, int iDataType, int iIdxListID, int iLine, int iPos)
{
	ECodeDataType eType, eSubType;
	TCVScalar dVal = 0.0;
	TCVScalar pdVal[4];

	eType = rVar.Type();

	if (eType == PDT_VARLIST)
	{
		TVarList& rList = *rVar.GetVarListPtr();

		int iVLCnt = int(rList.Count());
		if (iVLCnt > 0)
		{
			eSubType = rList(0).Type();
			if (eSubType == PDT_STRING)
			{
				// The first element in list gives the type of data
				// 'vex': Vertex
				// 'norm': Normal
				// 'tex': Texture coordinate
				// 'col': Color
				// 'idx': Index
				// 'partid': Part ID for vertex

				TString csType = *rList(0).GetStringPtr();
				if (csType == "vex")
				{
					iDataType = 0;
				}
				else if (csType == "norm")
				{
					iDataType = 1;
				}
				else if (csType == "tex")
				{
					iDataType = 2;
				}
				else if (csType == "col")
				{
					iDataType = 3;
				}
				else if (csType == "idx")
				{
					iDataType = 4;
				}
				else if (csType == "partid")
				{
					iDataType = 5;
				}
				else
				{
					m_ErrorList.GeneralError("Invalid object data type. Has to be one of 'vex', 'norm', 'tex', 'col' or 'idx'.", iLine, iPos);
					return false;
				}

				// If a single scalar is given as data element, then there is no data
				// for the respective type.
				if ((rList(1).BaseType() == PDT_COUNTER) ||
				    (rList(1).BaseType() == PDT_SCALAR))
				{
					return true;
				}

				if (!VexListAddData(pVexList, rList(1), iDataType, iIdxListID, iLine, iPos))
				{
					return false;
				}
			}
			else if ((rList(0).BaseType() != PDT_MULTIV) && (rList(0).CastToScalar(dVal) || (rList(0).BaseType() == PDT_TENSOR)))
			{
				// This is a list of scalars giving the data
				if ((iDataType >= 0) && (iDataType <= 2))
				{
					// vertex
					if ((iVLCnt < 2) || (iVLCnt > 3))
					{
						m_ErrorList.GeneralError("Either 2 or 3 coordinates have to be given.", iLine, iPos);
						return false;
					}

					for (int iVL = 0; iVL < iVLCnt; ++iVL)
					{
						if (!rList(iVL).CastToScalar(pdVal[iVL]))
						{
							m_ErrorList.GeneralError("Element in list is not a scalar.", iLine, iPos);
							return false;
						}
					}

					if (iVLCnt == 2)
					{
						pdVal[2] = 0.0;
					}

					if (iDataType == 0)
					{
						pVexList->AddVex(float(pdVal[0]), float(pdVal[1]), float(pdVal[2]));
					}
					else if (iDataType == 1)
					{
						pVexList->AddNormal(float(pdVal[0]), float(pdVal[1]), float(pdVal[2]));
					}
					else if (iDataType == 2)
					{
						pVexList->AddTex(float(pdVal[0]), float(pdVal[1]), float(pdVal[2]));
					}
				}
				else if (iDataType == 3)
				{
					// color
					if ((iVLCnt < 3) || (iVLCnt > 4))
					{
						m_ErrorList.GeneralError("A color has to be defined either by 3 or 4 values.", iLine, iPos);
						return false;
					}

					for (int iVL = 0; iVL < iVLCnt; ++iVL)
					{
						if (!rList(iVL).CastToScalar(pdVal[iVL]))
						{
							m_ErrorList.GeneralError("Element in list is not a scalar.", iLine, iPos);
							return false;
						}
					}

					if (iVLCnt == 3)
					{
						pdVal[3] = 0.0;
					}

					pVexList->AddCol(float(pdVal[0]), float(pdVal[1]), float(pdVal[2]), float(pdVal[3]));
				}
				else if (iDataType == 4)
				{
					// List of counters giving indices
					// or list of tensors giving index lists
					Mem<unsigned> mIdxList;
					int iVexIdx;

					if (rList(0).BaseType() == PDT_TENSOR)
					{
						// Expect list of tensors giving index lists
						for (int iVL = 0; iVL < iVLCnt; ++iVL)
						{
							if (rList(iVL).BaseType() != PDT_TENSOR)
							{
								m_ErrorList.GeneralError("Element in list of index lists is not a tensor.", iLine, iPos);
								return false;
							}

							TTensor& tIdx    = *rList(iVL).GetTensorPtr();
							int iIdxCnt      = tIdx.DimSize(0);
							TCVScalar* pdIdx = tIdx.Data();
							mIdxList.Set(iIdxCnt);

							for (int iIdx = 0; iIdx < iIdxCnt; ++iIdx)
							{
								iVexIdx = int(floor(pdIdx[iIdx] + 0.5));
								if (iVexIdx <= 0)
								{
									m_ErrorList.GeneralError("Invalid index in index list", iLine, iPos);
									return false;
								}
								mIdxList[iIdx] = iVexIdx - 1;
							}

							if (iVL == 0)
							{
								pVexList->SetIdxList(mIdxList);
							}
							else
							{
								pVexList->AddIdxList(mIdxList);
							}
						}
					}
					else
					{
						// Expect list of counters giving indices
						mIdxList.Set(iVLCnt);

						for (int iVL = 0; iVL < iVLCnt; ++iVL)
						{
							if (!rList(iVL).CastToCounter(iVexIdx, false))
							{
								m_ErrorList.GeneralError("Element in list is not a counter.", iLine, iPos);
								return false;
							}

							if (iVexIdx <= 0)
							{
								m_ErrorList.GeneralError("Invalid index in index list.", iLine, iPos);
								return false;
							}

							mIdxList[iVL] = iVexIdx - 1;
						}

						pVexList->SetIdxList(mIdxList);
					}
				}
				else if (iDataType == 5)
				{
					// List of counters giving part id's
					Mem<unsigned> mPartIdList;
					int iPartId;

					// Expect list of counters giving indices
					mPartIdList.Set(iVLCnt);

					for (int iVL = 0; iVL < iVLCnt; ++iVL)
					{
						if (!rList(iVL).CastToCounter(iPartId, false))
						{
							m_ErrorList.GeneralError("Element in list is not a counter.", iLine, iPos);
							return false;
						}

						if (iPartId <= 0)
						{
							m_ErrorList.GeneralError("Part ID must be greater than zero.", iLine, iPos);
							return false;
						}

						mPartIdList[iVL] = iPartId - 1;
					}

					pVexList->AddPartIdRange(mPartIdList);
				}
				else
				{
					m_ErrorList.GeneralError("Unknown object data type.", iLine, iPos);
					return false;
				}
			}
			else
			{
				// Call this function for each element of list
				for (int iVL = 0; iVL < iVLCnt; ++iVL)
				{
					if (!VexListAddData(pVexList, rList(iVL), iDataType, iIdxListID, iLine, iPos))
					{
						return false;
					}
				}
			}
		}
	}
	else if (eType == PDT_MULTIV)
	{
		TMultiV& rMV = *rVar.GetMultiVPtr();
		TMultiV mvVec;

		if (!CastMVtoE3(rMV, mvVec))
		{
			m_ErrorList.GeneralError("Given multivector cannot be interpreted as a point.", iLine, iPos);
			return false;
		}

		if (iDataType == 0)
		{
			pVexList->AddVex(float(mvVec[1]), float(mvVec[2]), float(mvVec[3]));
		}
		else if (iDataType == 1)
		{
			pVexList->AddNormal(float(mvVec[1]), float(mvVec[2]), float(mvVec[3]));
		}
		else if (iDataType == 2)
		{
			pVexList->AddTex(float(mvVec[1]), float(mvVec[2]), float(mvVec[3]));
		}
		else if (iDataType == 3)
		{
			pVexList->AddCol(float(mvVec[1]), float(mvVec[2]), float(mvVec[3]));
		}
		else
		{
			m_ErrorList.GeneralError("Unknown object data type.", iLine, iPos);
			return false;
		}
	}
	else if (eType == PDT_COLOR)
	{
		TOGLColor& rCol = *rVar.GetOGLColorPtr();

		pVexList->AddCol(rCol);
	}
	else if ((eType == PDT_TENSOR) || (eType == PDT_TENSOR_IDX))
	{
		TTensor* pT, T1;

		if (eType == PDT_TENSOR_IDX)
		{
			::MakeTensor(T1, *rVar.GetTensorIdxPtr());
			pT = &T1;
		}
		else
		{
			pT = rVar.GetTensorPtr();
		}

		TTensor& rT = *pT;

		if ((iDataType >= 0) && (iDataType <= 4) && (rT.Valence() != 2))
		{
			m_ErrorList.GeneralError("Tensor must have a valence of 2.", iLine, iPos);
			return false;
		}
		else if ((iDataType == 5) && (rT.Valence() != 1))
		{
			m_ErrorList.GeneralError("Tensor for part-id must have a valence of 1.", iLine, iPos);
			return false;
		}

		Mem<int> mIdxSteps = rT.GetIdxSteps();
		int iRow, iRowCnt = rT.DimSize(0);
		int iCol, iColCnt = rT.DimSize(1);
		int iStepRow     = mIdxSteps[0];
		TCVScalar* pData = rT.Data();
		int iIdx         = 0;

		if (iDataType == 0)
		{
			// Vertex
			Mem<COGLVertex> memVertex;
			memVertex.Set(iRowCnt);

			if (iColCnt == 2)
			{
				for (iRow = 0; iRow < iRowCnt; ++iRow, iIdx += iStepRow)
				{
					memVertex[iRow].Set(float(pData[iIdx]), float(pData[iIdx + 1]), 0.0f);
				}
			}
			else if (iColCnt == 3)
			{
				for (iRow = 0; iRow < iRowCnt; ++iRow, iIdx += iStepRow)
				{
					memVertex[iRow].Set(float(pData[iIdx]), float(pData[iIdx + 1]), float(pData[iIdx + 2]));
				}
			}
			else
			{
				m_ErrorList.GeneralError("Tensor must have either 2 or 3 columns.", iLine, iPos);
				return false;
			}

			pVexList->AddVexRange(memVertex);
		}
		else if (iDataType == 1)
		{
			// Normal
			Mem<COGLVertex> memVertex;
			memVertex.Set(iRowCnt);

			if (iColCnt == 2)
			{
				for (iRow = 0; iRow < iRowCnt; ++iRow, iIdx += iStepRow)
				{
					memVertex[iRow].Set(float(pData[iIdx]), float(pData[iIdx + 1]), 0.0f);
				}
			}
			else if (iColCnt == 3)
			{
				for (iRow = 0; iRow < iRowCnt; ++iRow, iIdx += iStepRow)
				{
					memVertex[iRow].Set(float(pData[iIdx]), float(pData[iIdx + 1]), float(pData[iIdx + 2]));
				}
			}
			else
			{
				m_ErrorList.GeneralError("Tensor must have either 2 or 3 columns.", iLine, iPos);
				return false;
			}

			pVexList->AddNormalRange(memVertex);
		}
		else if (iDataType == 2)
		{
			// Texture
			Mem<COGLVertex> memVertex;
			memVertex.Set(iRowCnt);

			if (iColCnt == 2)
			{
				for (iRow = 0; iRow < iRowCnt; ++iRow, iIdx += iStepRow)
				{
					memVertex[iRow].Set(float(pData[iIdx]), float(pData[iIdx + 1]), 0.0f);
				}
			}
			else if (iColCnt == 3)
			{
				for (iRow = 0; iRow < iRowCnt; ++iRow, iIdx += iStepRow)
				{
					memVertex[iRow].Set(float(pData[iIdx]), float(pData[iIdx + 1]), float(pData[iIdx + 2]));
				}
			}
			else
			{
				m_ErrorList.GeneralError("Tensor must have either 2 or 3 columns.", iLine, iPos);
				return false;
			}

			pVexList->AddTexRange(memVertex);
		}
		else if (iDataType == 3)
		{
			// Color
			MemObj<COGLColor> memColor;
			memColor.Set(iRowCnt);

			if (iColCnt == 3)
			{
				for (iRow = 0; iRow < iRowCnt; ++iRow, iIdx += iStepRow)
				{
					memColor[iRow].Set(float(pData[iIdx]), float(pData[iIdx + 1]), float(pData[iIdx + 2]), 1.0f);
				}
			}
			else if (iColCnt == 4)
			{
				for (iRow = 0; iRow < iRowCnt; ++iRow, iIdx += iStepRow)
				{
					memColor[iRow].Set(float(pData[iIdx]), float(pData[iIdx + 1]), float(pData[iIdx + 2]), float(pData[iIdx + 3]));
				}
			}
			else
			{
				m_ErrorList.GeneralError("Tensor must have either 3 or 4 columns.", iLine, iPos);
				return false;
			}

			pVexList->AddColRange(memColor);
		}
		else if (iDataType == 4)
		{
			// Index
			Mem<unsigned> mIdxList;
			int iVexIdx;
			mIdxList.Set(iColCnt);

			for (iRow = 0; iRow < iRowCnt; ++iRow)
			{
				for (iCol = 0; iCol < iColCnt; ++iCol, iIdx++)
				{
					iVexIdx = int(floor(pData[iIdx] + 0.5));
					if (iVexIdx <= 0)
					{
						m_ErrorList.GeneralError("Invalid index in index list", iLine, iPos);
						return false;
					}
					mIdxList[iCol] = iVexIdx - 1;
				}

				if (iRow == 0)
				{
					pVexList->SetIdxList(mIdxList);
				}
				else
				{
					pVexList->AddIdxList(mIdxList);
				}
			}
		}
		else if (iDataType == 5)
		{
			// List of counters giving part id's
			Mem<unsigned> mPartIdList;
			int iPartId;

			// Expect list of counters giving indices
			mPartIdList.Set(iRowCnt);

			for (iRow = 0; iRow < iRowCnt; ++iRow)
			{
				iPartId = int(pData[iRow]);

				if (iPartId <= 0)
				{
					m_ErrorList.GeneralError("Part ID must be greater than zero.", iLine, iPos);
					return false;
				}

				mPartIdList[iRow] = iPartId - 1;
			}

			pVexList->AddPartIdRange(mPartIdList);
		}
		else
		{
			m_ErrorList.GeneralError("Unknown object data type.", iLine, iPos);
			return false;
		}
	}
	else
	{
		m_ErrorList.InvalidRVal(rVar, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Adapt Object coordinates by image
///
/// Parameters:
///		1. (scalar) value to which black maps to
///		2. (scalar) value to which white maps to
///		3. (scalar) x-offset in image in pixels
///		4. (scalar) y-offset in image in pixels
///		5. (scalar) x-component of vector in image in pixels
///		6. (scalar) y-component of vector in image in pixels

bool CCLUCodeBase::VexListAdaptCoordImage1D(COGLVertexList& rVexList,
		TImage& rImg,
		int iCoord,
		TVarList& rPar,
		double& dValueMin,
		double& dValueMax,
		double dValueOffset,
		int iLine,
		int iPos)
{
	dValueMin = 0.0;
	dValueMax = 0.0;

	int iParCnt = int(rPar.Count());
	if ((iParCnt != 6) && (iParCnt != 7))
	{
		m_ErrorList.GeneralError("Adapting 1D-object with image expects 6 or 7 parameters.", iLine, iPos);
		return false;
	}

	if (iCoord < 0)
	{
		iCoord = 0;
	}
	else if (iCoord > 2)
	{
		iCoord = 2;
	}

	double dMin;
	if (!rPar(0).CastToScalar(dMin))
	{
		m_ErrorList.GeneralError("First parameter of adaptation has to give the coordinate value 'black' maps to.", iLine, iPos);
		return false;
	}

	double dMax;
	if (!rPar(1).CastToScalar(dMax))
	{
		m_ErrorList.GeneralError("Second parameter of adaptation has to give the coordinate value 'white' maps to.", iLine, iPos);
		return false;
	}

	double dOffX;
	if (!rPar(2).CastToScalar(dOffX))
	{
		m_ErrorList.GeneralError("Third parameter of adaptation has to give the x-offset in the image in pixels.", iLine, iPos);
		return false;
	}

	double dOffY;
	if (!rPar(3).CastToScalar(dOffY))
	{
		m_ErrorList.GeneralError("Fourth parameter of adaptation has to give the y-offset in the image in pixels.", iLine, iPos);
		return false;
	}

	double dVecX;
	if (!rPar(4).CastToScalar(dVecX))
	{
		m_ErrorList.GeneralError("Fifth parameter of adaptation has to give the x-component of vector in the image in pixels.", iLine, iPos);
		return false;
	}

	double dVecY;
	if (!rPar(5).CastToScalar(dVecY))
	{
		m_ErrorList.GeneralError("Fifth parameter of adaptation has to give the y-component of vector in the image in pixels.", iLine, iPos);
		return false;
	}

	int iPixelOperatorID = 0;
	if (iParCnt >= 7)
	{
		TString csOp = *rPar(6).GetStringPtr();
		if (csOp == "max")
		{
			iPixelOperatorID = 0;
		}
		else if (csOp == "r")
		{
			iPixelOperatorID = 1;
		}
		else if (csOp == "g")
		{
			iPixelOperatorID = 2;
		}
		else if (csOp == "b")
		{
			iPixelOperatorID = 3;
		}
		else if (csOp == "a")
		{
			iPixelOperatorID = 4;
		}
		else
		{
			m_ErrorList.GeneralError("Seventh parameter of adaptation has to give the pixel operator name: 'max', 'r', 'g', 'b', 'a'.", iLine, iPos);
			return false;
		}
	}

	dValueMin = 1e32;
	dValueMax = -1e32;

	int iWidth, iHeight;
	rImg->GetSize(iWidth, iHeight);

	if ((dOffX < 0.0) || (dOffX >= iWidth - 0.5) || (dOffX + dVecX < 0.0) || (dOffX + dVecX >= iWidth - 0.5) ||
	    (dOffY < 0.0) || (dOffY >= iHeight - 0.5) || (dOffY + dVecY < 0.0) || (dOffY + dVecY >= iHeight - 0.5))
	{
		m_ErrorList.GeneralError("Specified line in image out of range.", iLine, iPos);
		return false;
	}

	if (rVexList.GetVexCount() != rVexList.GetTexCount())
	{
		m_ErrorList.GeneralError("Number of vertices and texture coordinates does not agree.", iLine, iPos);
		return false;
	}

	// Move vertices in given coordinate by image intensity
	for (int iVertexID = 0; iVertexID < rVexList.GetVexCount(); ++iVertexID)
	{
		COGLVertex& rPos       = rVexList[iVertexID].xVex;
		const COGLVertex& rRel = rVexList[iVertexID].xTex;

		// Position along line in image in pixels
		double dRelPos = double(rRel[0]);

		int iX = int(dRelPos * dVecX + dOffX);
		int iY = int(dRelPos * dVecY + dOffY);

		double dColorR, dColorG, dColorB, dColorA;
		rImg->GetPixel(iX, iY, dColorR, dColorG, dColorB, dColorA);

		double dOperatorResult;
		switch (iPixelOperatorID)
		{
		case 0: dOperatorResult = max(max(dColorR, dColorG), dColorB);
			break;
		case 1: dOperatorResult = dColorR;
			break;
		case 2: dOperatorResult = dColorG;
			break;
		case 3: dOperatorResult = dColorB;
			break;
		case 4: dOperatorResult = dColorA;
			break;
		}

		double dValue = dMin + dOperatorResult * (dMax - dMin);

		dValue -= dValueOffset;

		dValueMin = min(dValueMin, dValue);
		dValueMax = max(dValueMax, dValue);

		// Adapt coordinate
		rPos[iCoord] = float(dValue);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Adapt Object coordinates by image
///
/// Parameters:
///		1. (scalar) value to which black maps to
///		2. (scalar) value to which white maps to
///		3. (scalar) scale of normal z-component

bool CCLUCodeBase::VexListAdaptCoordImage2D(COGLVertexList& rVexList, TImage& rImg, int iCoord, TVarList& rPar, int iLine, int iPos)
{
	int iParCnt = int(rPar.Count());
	TCVScalar dMin, dMax, dRange, dNScale;

	if (iParCnt != 3)
	{
		m_ErrorList.GeneralError("Adaption object with image expects 3 parameters.", iLine, iPos);
		return false;
	}

	if (iCoord < 0)
	{
		iCoord = 0;
	}
	else if (iCoord > 2)
	{
		iCoord = 2;
	}

	if (!rPar(0).CastToScalar(dMin))
	{
		m_ErrorList.GeneralError("First parameter of adaptation has to give the coordinate value 'black' maps to.", iLine, iPos);
		return false;
	}

	if (!rPar(1).CastToScalar(dMax))
	{
		m_ErrorList.GeneralError("Second parameter of adaptation has to give the coordinate value 'white' maps to.", iLine, iPos);
		return false;
	}

	if (!rPar(2).CastToScalar(dNScale))
	{
		m_ErrorList.GeneralError("Third parameter of adaptation has to give the scale of the image normal z-component.", iLine, iPos);
		return false;
	}

	dRange = dMax - dMin;

	//Mem<COGLVertex> &rVex = rVexList.GetVexList();
	//Mem<COGLVertex> &rTex = rVexList.GetTexList();
	//Mem<COGLVertex> &rNorm = rVexList.GetNormList();

	int iVex, iVexCnt = rVexList.GetVexCount();
	int iX, iY, iWidth, iHeight;
	int iNX, iNY, iNZ;
	double dWidth, dHeight;
	double dI, dR, dG, dB, dA;
	double dX, dY, dZ;
	double dNorm;

	rImg->GetSize(iWidth, iHeight);
	dWidth  = double(iWidth);
	dHeight = double(iHeight);

	if (iCoord == 0)
	{
		iNX = 1;
		iNY = 2;
		iNZ = 0;
	}
	else if (iCoord == 1)
	{
		iNX = 2;
		iNY = 0;
		iNZ = 1;
	}
	else if (iCoord == 2)
	{
		iNX = 0;
		iNY = 1;
		iNZ = 2;
	}

	if (iVexCnt != rVexList.GetTexCount())
	{
		m_ErrorList.GeneralError("Number of vertices and texture coordinates does not agree.", iLine, iPos);
		return false;
	}

	int iImgType, iDataType, iBPP;
	int iIdxC, iIdxX, iIdxY;
	float fIC, fIX, fIY;
	double dDIX, dDIY;

	rImg->GetType(iImgType, iDataType, iBPP);
	if ((iImgType == CLUVIZ_IMG_LUMINANCE) && (iDataType == CLUVIZ_IMG_FLOAT))
	{
		::LockImageAccess();

		float* pfImg = (float*) rImg->GetDataPtr();

		// move vertices in given coordinate by image intensity
		for (iVex = 0; iVex < iVexCnt; ++iVex)
		{
			COGLVertex& rPos = rVexList[iVex].xVex;
			COGLVertex& rRel = rVexList[iVex].xTex;
			COGLVertex& rN   = rVexList[iVex].xNorm;

			iX = int(dWidth * double(rRel[0]) + 0.5);
			iY = int(dHeight * double(rRel[1]) + 0.5);

			if (iX < 0) { iX = 0; }
			else if (iX >= iWidth - 1)
			{
				iX = iWidth - 2;
			}

			if (iY < 0) { iY = 0; }
			else if (iY >= iHeight - 1)
			{
				iY = iHeight - 2;
			}

			iIdxC = (iHeight - iY - 1) * iWidth + iX;
			iIdxX = iIdxC + 1;
			iIdxY = iIdxC - iWidth;

			fIC = pfImg[iIdxC];
			fIX = pfImg[iIdxX];
			fIY = pfImg[iIdxY];

			dDIX = double(fIX - fIC);
			dDIY = double(fIY - fIC);

			// Adapt coordinate
			rPos[iCoord] = float(dMin + double(fIC) * dRange);

			// Adapt normal
			dNorm = sqrt(dDIX * dDIX + dDIY * dDIY + dNScale * dNScale);

			rN[iNX] = float(-dDIX / dNorm);
			rN[iNY] = float(-dDIY / dNorm);
			rN[iNZ] = float(dNScale / dNorm);
		}

		::UnlockImageAccess();
	}
	else
	{
		// move vertices in given coordinate by image intensity
		for (iVex = 0; iVex < iVexCnt; ++iVex)
		{
			COGLVertex& rPos = rVexList[iVex].xVex;
			COGLVertex& rRel = rVexList[iVex].xTex;
			COGLVertex& rN   = rVexList[iVex].xNorm;

			iX = int(dWidth * double(rRel[0]) + 0.5);
			iY = int(dHeight * double(rRel[1]) + 0.5);

			rImg->GetPixel(iX, iY, dR, dG, dB, dA);
			rImg->GetPixelNormalLum(iX, iY, dX, dY, dZ);

			dI = max(max(dR, dG), dB);

			// Adapt coordinate
			rPos[iCoord] = float(dMin + dI * dRange);

			// Adapt normal
			dZ   *= dNScale;
			dNorm = sqrt(dX * dX + dY * dY + dZ * dZ);

			rN[iNX] = float(dX / dNorm);
			rN[iNY] = float(dY / dNorm);
			rN[iNZ] = float(dZ / dNorm);
		}
	}

	return true;
}
