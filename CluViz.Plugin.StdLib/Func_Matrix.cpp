////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_Matrix.cpp
//
// summary:   Implements the function matrix class
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

#include <time.h>
#include <algorithm>

#include "CluTec.Viz.Base\TensorOperators.h"

using std::sort;

//////////////////////////////////////////////////////////////////////
/// Reduce row space of matrix to set of linearly independent rows
//
// Parameters:
//	1. Matrix to be reduced
//	2. Precision to be used. That is, magnitude of rejection below which
//		two vectors are parallel.
//
// Returns:
//	List of
//		1. reduced matrix
//		2. index list of indicating movement of rows.

typedef pair<int, TCVScalar> TIdxValPair;

bool PairGreater(TIdxValPair v1, TIdxValPair v2)
{
	if (v1.second > v2.second)
	{
		return true;
	}

	return false;
}

bool  ReduceMatrixToRowSpaceFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	TCVScalar dPrecision = 1e-12;
	int iVarCount        = int(mVars.Count());

	if ((iVarCount < 1) || (iVarCount > 2))
	{
		int piParNo[] = { 1, 2 };
		rCB.GetErrorList().WrongNoOfParams(piParNo, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_MATRIX)
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (iVarCount > 1)
	{
		if (!mVars(1).CastToScalar(dPrecision, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}
		//printf("Prec.: %g\n", dPrecision);
		dPrecision = Mag(dPrecision);
		//printf("Prec.: %g\n", dPrecision);
	}

	TMatrix& mA = *mVars(0).GetMatrixPtr();

	int i, j, iRows, iCols;
	TCVScalar dNorm;

	iRows = mA.Rows();
	iCols = mA.Cols();

	vector<TIdxValPair> vecRowNorm;

	// Evaluate L2 norm of rows
	for (i = 0; i < iRows; i++)
	{
		dNorm = 0;
		for (j = 0; j < iCols; j++)
		{
			dNorm += pow(mA(i + 1, j + 1), 2.0);
		}

		dNorm = sqrt(dNorm);
		//printf("%d: Norm = %g\n", i, dNorm);
		vecRowNorm.push_back(TIdxValPair(i, dNorm));
	}

	// Sort row indices in descending order
	sort(vecRowNorm.begin(), vecRowNorm.end(), PairGreater);

	// Create matrix with normalized rows.
	int iRowIdx;
	TMatrix mB(1, iCols);
	vector<TMatrix> vecRows;

	for (i = 0; i < iRows; i++)
	{
		iRowIdx = vecRowNorm[i].first + 1;
		dNorm   = vecRowNorm[i].second;
		if (dNorm > TCVScalar(0))
		{
			for (j = 0; j < iCols; j++)
			{
				mB(1, j + 1) = mA(iRowIdx, j + 1) / dNorm;
			}

			//printMat(mB, "B",0);
			vecRows.push_back(mB);
		}
	}

	// Create reduced matrix
	int iBasisDim;
	TMatrix mX, mVal;
	vector<TMatrix> vecBasis;
	vector<int> vecBasisIdx;

	vecBasis.push_back(vecRows[0]);
	vecBasisIdx.push_back(vecRowNorm[0].first);
	for (i = 1; i < iRows; i++)
	{
		// Evaluate rejection of a row on basis found so far
		iBasisDim = int(vecBasis.size());
		mX        = vecRows[i];
		for (j = 0; j < iBasisDim; j++)
		{
			mVal = vecBasis[j] * ~mX;
			mX  -= mVal(1, 1) * vecBasis[j];
		}

		// Eval. norm of mX
		dNorm = 0;
		for (int iC = 1; iC <= iCols; iC++)
		{
			dNorm += pow(mX(1, iC), 2.0);
		}

		dNorm = sqrt(dNorm);

		//printMat(mX, "Rejection", 0);
		//printf("Norm: %g,  Prec.: %g\n", dNorm, dPrecision);

		if (dNorm >= dPrecision)
		{
			//mX = (1.0 / dNorm) * mX;
			dNorm = 1.0 / dNorm;
			mX    = dNorm * mX;
			vecBasis.push_back(mX);
			vecBasisIdx.push_back(vecRowNorm[i].first);
		}
	}

	// Construct final matrix
	int iRowCount = int(vecBasisIdx.size());

	//printMat(mA, "A", 0);
	mB.Resize(iRowCount, iCols);
	for (i = 0; i < iRowCount; i++)
	{
		iRowIdx = vecBasisIdx[i] + 1;
		//printf("Row: %d,  RowIdx: %d\n", i+1, iRowIdx);
		for (j = 0; j < iCols; j++)
		{
			mB(i + 1, j + 1) = mA(iRowIdx, j + 1);
		}
	}

	//printMat(mB, "B", 0);

	// Generate output
	rVar.New(PDT_VARLIST);
	TVarList& rList = *rVar.GetVarListPtr();

	if (!rList.Add(2))
	{
		rCB.GetErrorList().OutOfMemory(iLine, iPos);
		return false;
	}

	rList[0] = mB;

	rList[1].New(PDT_VARLIST);
	TVarList& rIdxList = *rList[1].GetVarListPtr();

	if (!rIdxList.Add(iRowCount))
	{
		rCB.GetErrorList().OutOfMemory(iLine, iPos);
		return false;
	}

	for (i = 0; i < iRowCount; i++)
	{
		rIdxList[i] = vecBasisIdx[i] + 1;
	}

	return true;
}

bool  ReduceMatrixToMajorRowsFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iNumb;

	int iVarCount = int(mVars.Count());

	if (iVarCount != 2)
	{
		rCB.GetErrorList().WrongNoOfParams(2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_MATRIX)
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToCounter(iNumb))
	{
		rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}
	iNumb = Mag(iNumb);

	TMatrix& mA = *mVars(0).GetMatrixPtr();

	int i, j, iRows, iCols;
	TCVScalar dNorm;

	iRows = mA.Rows();
	iCols = mA.Cols();

	if (iNumb > iRows)
	{
		rCB.GetErrorList().InconsistentRowLength(iRows, iLine, iPos);
		return false;
	}

	vector<TIdxValPair> vecRowNorm, idxAndNorm;

	// Evaluate L2 norm of rows
	for (i = 0; i < iRows; i++)
	{
		dNorm = 0;
		for (j = 0; j < iCols; j++)
		{
			dNorm += pow(mA(i + 1, j + 1), 2.0);
		}

		dNorm = sqrt(dNorm);
		// printf("%d: Norm = %g\n", i, dNorm);
		vecRowNorm.push_back(TIdxValPair(i, dNorm));
	}

	// Sort row indices in descending order
	sort(vecRowNorm.begin(), vecRowNorm.end(), PairGreater);

	// Create matrix with normalized rows.
	int iRowIdx;
	TMatrix mB(1, iCols);
	vector<TMatrix> vecRows;

	for (i = 0; i < iRows; i++)
	{
		iRowIdx = vecRowNorm[i].first + 1;
		dNorm   = vecRowNorm[i].second;
		if (dNorm > TCVScalar(0))
		{
			for (j = 0; j < iCols; j++)
			{
				mB(1, j + 1) = mA(iRowIdx, j + 1) / dNorm;
			}

			//printMat(mB, "B",0);
			vecRows.push_back(mB);
		}
	}

	// Create reduced matrix
	int iBasisDim;
	TMatrix mX, mVal;
	vector<TMatrix> vecBasis;
	vector<int> vecBasisIdx;

	vecBasis.push_back(vecRows[0]);
	vecBasisIdx.push_back(vecRowNorm[0].first);
	idxAndNorm.push_back(TIdxValPair(vecRowNorm[0].first, 1.0));
	for (i = 1; i < iRows; i++)
	{
		// Evaluate rejection of a row on basis found so far
		iBasisDim = int(vecBasis.size());
		mX        = vecRows[i];
		for (j = 0; j < iBasisDim; j++)
		{
			mVal = vecBasis[j] * ~mX;
			mX  -= mVal(1, 1) * vecBasis[j];
		}

		// Eval. norm of mX
		dNorm = 0;
		for (int iC = 1; iC <= iCols; iC++)
		{
			dNorm += pow(mX(1, iC), 2.0);
		}

		dNorm = sqrt(dNorm);

		if (dNorm > 0)
		{
			dNorm = 1.0 / dNorm;
			mX    = dNorm * mX;
			vecBasis.push_back(mX);
			idxAndNorm.push_back(TIdxValPair(vecRowNorm[i].first, 1.0 / dNorm));
		}
	}

	// Sort row indices in descending order
	sort(idxAndNorm.begin(), idxAndNorm.end(), PairGreater);

	vecBasisIdx.clear();
	for (i = 0; i < iNumb; i++)
	{
		vecBasisIdx.push_back(idxAndNorm[i].first + 1);
	}

	sort(vecBasisIdx.begin(), vecBasisIdx.end());

	// Generate output
	rVar.New(PDT_VARLIST);
	TVarList& rIdxList = *rVar.GetVarListPtr();

	if (!rIdxList.Add(iNumb))
	{
		rCB.GetErrorList().OutOfMemory(iLine, iPos);
		return false;
	}

	for (i = 0; i < iNumb; i++)
	{
		rIdxList[i] = vecBasisIdx[i];
	}

	return true;
}

//////////////////////////////////////////////////////////////////
/// Extract Rows given by an index list from matrix
//
// Parameters:
//	1. matrix from which rows are to be extracted
//	2. list of row indices
//
// Returns:
//	Matrix constructed from extracted rows.
//

bool  ExtractMatrixRowsFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount != 2)
	{
		int piParNo[] = { 2 };
		rCB.GetErrorList().WrongNoOfParams(piParNo, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_MATRIX)
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_VARLIST)
	{
		rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}

	TMatrix& mA        = *mVars(0).GetMatrixPtr();
	TVarList& rIdxList = *mVars(1).GetVarListPtr();

	int iRows, iCols, iIdx;
	int i, j, iIdxCount = int(rIdxList.Count());
	vector<int> vecIdxList;

	iRows = mA.Rows();
	iCols = mA.Cols();
	vecIdxList.resize(iIdxCount);

	// Copy list of row indices
	for (i = 0; i < iIdxCount; i++)
	{
		if (!rIdxList[i].CastToCounter(iIdx))
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		if ((iIdx > iRows) || (iIdx <= 0))
		{
			rCB.GetErrorList().GeneralError("Index out of range.", iLine, iPos);
			return false;
		}

		vecIdxList[i] = iIdx;
	}

	// Copy rows
	rVar.New(PDT_MATRIX);
	TMatrix& mB = *rVar.GetMatrixPtr();

	mB.Resize(iIdxCount, iCols);

	for (i = 0; i < iIdxCount; i++)
	{
		iIdx = vecIdxList[i];
		for (j = 0; j < iCols; j++)
		{
			mB(i + 1, j + 1) = mA(iIdx, j + 1);
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add Rows to matrix

bool  AddMatrixRowsFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount != 2)
	{
		int piParNo[] = { 2 };
		rCB.GetErrorList().WrongNoOfParams(piParNo, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_MATRIX)
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_MATRIX)
	{
		rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}

	TMatrix& mA = *mVars(0).GetMatrixPtr();
	TMatrix& mB = *mVars(1).GetMatrixPtr();

	rVar = mA;
	TMatrix& mR = *rVar.GetMatrixPtr();

	if (mA.Cols() != mB.Cols())
	{
		rCB.GetErrorList().GeneralError("The matrices do not have the same number of columns.", iLine, iPos);
		return false;
	}

	if (!mR.AddRows(mR.Rows() + 1, 1, mB, mB.Rows()))
	{
		rCB.GetErrorList().GeneralError("Error adding rows.", iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add Cols to matrix

bool  AddMatrixColsFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount != 2)
	{
		int piParNo[] = { 2 };
		rCB.GetErrorList().WrongNoOfParams(piParNo, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_MATRIX)
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_MATRIX)
	{
		rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}

	TMatrix& mA = *mVars(0).GetMatrixPtr();
	TMatrix& mB = *mVars(1).GetMatrixPtr();

	rVar = mA;
	TMatrix& mR = *rVar.GetMatrixPtr();

	if (mA.Rows() != mB.Rows())
	{
		rCB.GetErrorList().GeneralError("The matrices do not have the same number of rows.", iLine, iPos);
		return false;
	}

	if (!mR.AddCols(mR.Cols() + 1, 1, mB, mB.Cols()))
	{
		rCB.GetErrorList().GeneralError("Error adding columns.", iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Create Identity Matrix of given Dimensions

bool  IdentityMatrixFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount == 2)
	{
		// Dimension of Identity-Matrix given
		int iRows = 0, iCols = 0;

		if (!mVars(0).CastToCounter(iRows))
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (iRows <= 0)
		{
			CStrMem csText;
			csText = iRows;
			rCB.GetErrorList().InvalidParVal(csText, 1, iLine, iPos);
			return false;
		}

		if (!mVars(1).CastToCounter(iCols))
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		if (iCols <= 0)
		{
			CStrMem csText;
			csText = iCols;
			rCB.GetErrorList().InvalidParVal(csText, 2, iLine, iPos);
			return false;
		}

		TMatrix xA(iRows, iCols);
		xA.Identity();

		rVar = xA;
	}
	else
	{
		int piParNo[] = { 2 };
		rCB.GetErrorList().WrongNoOfParams(piParNo, 1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Resize Matrix and keep data

bool  ResizeMatrixFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount == 3)
	{
		// Dimension of Identity-Matrix given
		int iRows = 0, iCols = 0;

		if (mVars(0).BaseType() != PDT_MATRIX)
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (!mVars(1).CastToCounter(iRows))
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		if (iRows <= 0)
		{
			CStrMem csText;
			csText = iRows;
			rCB.GetErrorList().InvalidParVal(csText, 2, iLine, iPos);
			return false;
		}

		if (!mVars(2).CastToCounter(iCols))
		{
			rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}

		if (iCols <= 0)
		{
			CStrMem csText;
			csText = iCols;
			rCB.GetErrorList().InvalidParVal(csText, 3, iLine, iPos);
			return false;
		}

		// The Matrix
		TMatrix Mat = *mVars(0).GetMatrixPtr();
		;

		if (Mat.Resize(iRows, iCols) == 0)
		{
			rCB.GetErrorList().GeneralError("Error resizing matrix.", iLine, iPos);
			return false;
		}
		rVar = Mat;
	}
	else
	{
		int piParNo[] = { 3 };
		rCB.GetErrorList().WrongNoOfParams(piParNo, 1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Reshape Matrix and keep data

bool  ReshapeMatrixFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount == 3)
	{
		// Dimension of Identity-Matrix given
		int iRows = 0, iCols = 0;

		if (mVars(0).BaseType() != PDT_MATRIX)
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (!mVars(1).CastToCounter(iRows))
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		if (iRows <= 0)
		{
			CStrMem csText;
			csText = iRows;
			rCB.GetErrorList().InvalidParVal(csText, 2, iLine, iPos);
			return false;
		}

		if (!mVars(2).CastToCounter(iCols))
		{
			rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}

		if (iCols <= 0)
		{
			CStrMem csText;
			csText = iCols;
			rCB.GetErrorList().InvalidParVal(csText, 3, iLine, iPos);
			return false;
		}

		// The Matrix
		TMatrix Mat = *mVars(0).GetMatrixPtr();
		;

		if (Mat.Reshape(iRows, iCols) == 0)
		{
			rCB.GetErrorList().GeneralError("Error reshaping matrix.", iLine, iPos);
			return false;
		}
		rVar = Mat;
	}
	else
	{
		int piParNo[] = { 3 };
		rCB.GetErrorList().WrongNoOfParams(piParNo, 1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Make Diagonal of Matrix into a Row Vector

bool  DiagToVectorFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount == 1)
	{
		if (mVars(0).BaseType() == PDT_MATRIX)
		{
			TMatrix M = *mVars(0).GetMatrixPtr();
			M.Trace();
			rVar = M;
		}
		else
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Make Vector into diagonal of Matrix

bool  VectorToDiagFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount == 1)
	{
		if (mVars(0).BaseType() == PDT_MATRIX)
		{
			TMatrix M = *mVars(0).GetMatrixPtr();
			M.Diagonal();
			rVar = M;
		}
		else
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Get Multivector from Matrix (column vector) in algebra

bool  GetMatrixMVFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if ((iVarCount == 1) || (iVarCount == 2))
	{
		if (mVars(0).BaseType() != PDT_MATRIX)
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		TVarList mRet;
		TMatrix& M = *mVars(0).GetMatrixPtr();
		MemObj<TMultiV> mvA;
		Mem<int> mMask;

		int iGADim;
		int iMVCount = int(M.Cols());
		int iMatRows = int(M.Rows());

		if (iVarCount == 2)
		{
			if (mVars(1).BaseType() != PDT_VARLIST)
			{
				rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
				return false;
			}

			TVarList& rList = *mVars(1).GetVarListPtr();
			int i, iVal;

			iGADim = int(rList.Count());
			mMask.Set(iGADim);
			for (i = 0; i < iGADim; i++)
			{
				if (!rList[i].CastToCounter(iVal))
				{
					rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
					return false;
				}

				mMask[i] = iVal;
			}
		}
		else
		{
			iGADim = int(M.Rows());
			int i;

			mMask.Set(iGADim);
			for (i = 0; i < iGADim; i++)
			{
				mMask[i] = i + 1;
			}
		}

		if ((iGADim != 8) && (iGADim != 16) && (iGADim != 32) && (iGADim != 64))
		{
			rCB.GetErrorList().GeneralError("Matrix has invalid row dimension. Expect 8, 16, 32 or 64.", iLine, iPos);
			return false;
		}

		mvA.Set(iMVCount);
		mRet.Set(iMVCount);
		for (int iMV = 0; iMV < iMVCount; iMV++)
		{
			TMultiV& vA = mvA[iMV];

			if (iGADim == 8)
			{
				vA = E3GA<TCVScalar>::vSC;
			}
			else if (iGADim == 16)
			{
				vA = PGA<TCVScalar>::vSC;
			}
			else if (iGADim == 32)
			{
				vA = ConfGA<TCVScalar>::vSC;
			}
			else if (iGADim == 64)
			{
				vA = El2GA<TCVScalar>::vSC;
			}

			for (int iDim = 0; iDim < iGADim; iDim++)
			{
				int iRow = mMask[iDim];
				if (iRow == 0)
				{
					vA[iDim] = 0;
				}
				else
				{
					if (::abs(iRow) > iMatRows)
					{
						rCB.GetErrorList().GeneralError(CLU_S "Multivector mask indexes invalid matrix row. Matrix has " << iMatRows << " rows. Index is " << iRow << ".", iLine, iPos);
						return false;
					}

					vA[iDim] = (iRow > 0) ? M(iRow, iMV + 1) : -M(::abs(iRow), iMV + 1);
				}
			}

			mRet[iMV] = vA;
		}

		rVar = mRet;
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Get Matrix (column vector) of multivector in algebra

bool  GetMVMatrixFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	bool bMVList  = false;
	int iVarCount = int(mVars.Count());

	if ((iVarCount == 1) || (iVarCount == 2))
	{
		TMatrix M;
		Mem<int> mMask;
		TMultiV* pvA      = 0;
		TVarList* pMVList = 0;
		int i, j, iGADim;	// = int(vA.GetGADim());
		int iRowDim, iMVCount;
		uint uBaseID;

		if (mVars(0).BaseType() == PDT_MULTIV)
		{
			bMVList  = false;
			iMVCount = 1;
			pvA      = mVars(0).GetMultiVPtr();
			iGADim   = pvA->GetGADim();
			uBaseID  = pvA->GetBase().BaseID();
		}
		else if (mVars(0).BaseType() == PDT_VARLIST)
		{
			bMVList = true;
			pMVList = mVars(0).GetVarListPtr();
			if ((iMVCount = int(pMVList->Count())) < 1)
			{
				rCB.GetErrorList().GeneralError("List of multivectors is empty.", iLine, iPos);
				return false;
			}

			if ((*pMVList)[0].BaseType() != PDT_MULTIV)
			{
				rCB.GetErrorList().GeneralError("First element in list is not a multivector.", iLine, iPos);
				return false;
			}

			iGADim  = int((*pMVList)[0].GetMultiVPtr()->GetGADim());
			uBaseID = (*pMVList)[0].GetMultiVPtr()->GetBase().BaseID();
		}
		else
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		mMask.Set(iGADim);

		if (iVarCount == 2)
		{
			if (mVars(1).BaseType() != PDT_VARLIST)
			{
				rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
				return false;
			}

			TVarList& rList = *mVars(1).GetVarListPtr();
			int i, iVal;

			if (iGADim != rList.Count())
			{
				rCB.GetErrorList().GeneralError("Invalid mask dimension.", iLine, iPos);
				return false;
			}

			iRowDim = 0;
			for (i = 0; i < iGADim; i++)
			{
				if (!rList[i].CastToCounter(iVal))
				{
					rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
					return false;
				}

				if (iVal < 0)
				{
					rCB.GetErrorList().GeneralError("Invalid mask. Negative index in mask.",
							iLine, iPos);
					return false;
				}

				mMask[i] = iVal;
				if (iVal > iRowDim)
				{
					iRowDim = iVal;
				}
			}

			if (iRowDim == 0)
			{
				rCB.GetErrorList().GeneralError("Invalid mask. There is no value larger than zero.",
						iLine, iPos);
				return false;
			}
		}
		else
		{
			int i;

			for (i = 0; i < iGADim; i++)
			{
				mMask[i] = i + 1;
			}

			iRowDim = iGADim;
		}

		if (bMVList)
		{
			M.Resize(iRowDim, iMVCount);
			M = (TCVScalar) 0;

			for (j = 0; j < iMVCount; j++)
			{
				if ((*pMVList)[j].BaseType() != PDT_MULTIV)
				{
					char pcText[200];
					sprintf_s(pcText, "Element at position %d is not a multivector.", j + 1);
					rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
					return false;
				}

				pvA = (*pMVList)[j].GetMultiVPtr();
				if (pvA->GetBase().BaseID() != uBaseID)
				{
					char pcText[200];
					sprintf_s(pcText, "Multivectors at positions 1 and %d are from different spaces.", j + 1);
					rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
					return false;
				}

				for (i = 0; i < iGADim; i++)
				{
					if (mMask[i] > 0)
					{
						M(mMask[i], j + 1) = (*pvA)[i];
					}
				}
			}
		}
		else
		{
			M.Resize(iRowDim, 1);
			M = (TCVScalar) 0;
			for (i = 0; i < iGADim; i++)
			{
				if (mMask[i] > 0)
				{
					M(mMask[i], 1) = (*pvA)[i];
				}
			}
		}

		rVar = M;
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Get Multivector Geometric Product Matrix

bool  GetMVProductMatrixFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if ((iVarCount >= 1) && (iVarCount <= 4))
	{
		int iProductType    = 0;
		bool bPruneNullRows = false;
		Mem<int> mMask;

		if (iVarCount >= 2)
		{
			if (!mVars(1).CastToCounter(iProductType))
			{
				rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
				return false;
			}

			if ((iProductType < 0) || (iProductType > 2))
			{
				rCB.GetErrorList().InvalidParVal(mVars(1), 2, iLine, iPos);
				return false;
			}
		}

		CCodeVar& rMVVar = mVars(0);
		MemObj<TMultiV> mvA;
		int iMV, iMVCount;

		if (rMVVar.BaseType() == PDT_MULTIV)
		{
			mvA.Set(1);
			mvA[0] = *rMVVar.GetMultiVPtr();
		}
		else if (rMVVar.BaseType() == PDT_VARLIST)
		{
			TVarList& rList = *rMVVar.GetVarListPtr();

			if (!(iMVCount = int(rList.Count())))
			{
				rCB.GetErrorList().GeneralError("Multivector List is empty.", iLine, iPos);
				return false;
			}

			mvA.Set(iMVCount);
			for (iMV = 0; iMV < iMVCount; iMV++)
			{
				if (rList(iMV).BaseType() != PDT_MULTIV)
				{
					rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
					return false;
				}
				mvA[iMV] = *rList(iMV).GetMultiVPtr();
			}
		}
		else
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		TMatrix M;
		int i, iGADim = int(mvA[0].GetGADim());
		int iMaskParPos;

		if ((iVarCount >= 3) && (mVars(2).BaseType() != PDT_VARLIST) && (mVars(2).BaseType() != PDT_INT))
		{
			rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}

		if ((iVarCount >= 4) && (mVars(3).BaseType() != PDT_VARLIST) && (mVars(3).BaseType() != PDT_INT))
		{
			rCB.GetErrorList().InvalidParType(mVars(3), 4, iLine, iPos);
			return false;
		}

		if ((iVarCount >= 4) && (mVars(2).BaseType() == mVars(3).BaseType()))
		{
			rCB.GetErrorList().GeneralError("Third and fourth parameter cannot be of same type.", iLine, iPos);
			return false;
		}

		// Check whether Mask is 3rd or 4th parameter
		if ((iVarCount >= 3) && (mVars(2).BaseType() == PDT_VARLIST))
		{
			iMaskParPos    = 2;
			bPruneNullRows = true;
		}
		else if ((iVarCount >= 4) && (mVars(3).BaseType() == PDT_VARLIST))
		{
			iMaskParPos    = 3;
			bPruneNullRows = true;
		}
		else
		{
			iMaskParPos = 0;
		}

		// If Mask is given either at 3rd or 4th position then read it.
		if (iMaskParPos)
		{
			if (mVars(iMaskParPos).BaseType() != PDT_VARLIST)
			{
				rCB.GetErrorList().InvalidParType(mVars(iMaskParPos), iMaskParPos + 1, iLine, iPos);
				return false;
			}

			TVarList& rList = *mVars(iMaskParPos).GetVarListPtr();
			int i, iVal, iCount = int(rList.Count());

			if (iCount != iGADim)
			{
				rCB.GetErrorList().GeneralError("Invalid mask dimension.", iLine, iPos);
				return false;
			}

			mMask.Set(iCount);
			for (i = 0; i < iCount; i++)
			{
				if (!rList(i).CastToCounter(iVal))
				{
					rCB.GetErrorList().InvalidParType(mVars(iMaskParPos), iMaskParPos + 1, iLine, iPos);
					return false;
				}

				mMask[i] = iVal;
			}
		}

		// If explicit mask was not given, create a standard mask here.
		if (mMask.Count() == 0)
		{
			mMask.Set(iGADim);
			for (i = 0; i < iGADim; i++)
			{
				mMask[i] = i + 1;
			}
		}

		// Check whether left/right flag was given either in third or in fourth position
		int iLRFlagParPos;
		tMVPos eProdDir = Left;

		if ((iVarCount >= 3) && (mVars(2).BaseType() == PDT_INT))
		{
			iLRFlagParPos = 2;
		}
		else if ((iVarCount >= 4) && (mVars(3).BaseType() == PDT_INT))
		{
			iLRFlagParPos = 3;
		}
		else
		{
			iLRFlagParPos = 0;
		}

		if (iLRFlagParPos)
		{
			int iVal = 0;

			mVars(iLRFlagParPos).CastToCounter(iVal);
			if (iVal)
			{
				eProdDir = Left;
			}
			else
			{
				eProdDir = Right;
			}
		}

		if (!CalcMVListGPMatMask(mvA, M, eProdDir, mMask, EMVOpType(iProductType), bPruneNullRows))
		{
			rCB.GetErrorList().GeneralError("Multivector Product Matrix could not be evaluated.", iLine, iPos);
			return false;
		}

		rVar = M;
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Determinant of Matrix

bool  DetFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList* pmVars = rPars.GetVarListPtr();
	int iVarCount    = int(pmVars->Count());

	if (iVarCount == 1)
	{
		TMatrix* pMat, Mat;

		if ((*pmVars)(0).BaseType() == PDT_MATRIX)
		{
			pMat = (*pmVars)(0).GetMatrixPtr();
		}
		else if (rCB.CastToMatrix(Mat, (*pmVars)(0), iLine, iPos))
		{
			pMat = &Mat;
		}
		else{ return false; }

		TMatrix& xA = *pMat;

		rVar = xA.Det();
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// SVD of Matrix

bool  SVDFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList* pmVars = rPars.GetVarListPtr();
	int iVarCount    = int(pmVars->Count());

	if (iVarCount == 1)
	{
		TMatrix* pMat, Mat;
		bool bTensor = false;

		if ((*pmVars)(0).BaseType() == PDT_MATRIX)
		{
			pMat = (*pmVars)(0).GetMatrixPtr();
		}
		else if (rCB.CastToMatrix(Mat, (*pmVars)(0), iLine, iPos))
		{
			pMat    = &Mat;
			bTensor = true;
		}
		else{ return false; }

		TMatrix& xA = *pMat;
		TMatrix xU, xD, xV;

		xA.SVD(xU, xD, xV);
		SVDOrder(xD, xU, xV, false);

		xU.TinyToZero(rCB.GetSensitivity());
		xV.TinyToZero(rCB.GetSensitivity());
		xD.TinyToZero(rCB.GetSensitivity());
		xD.Diagonal();

		rVar.New(PDT_VARLIST);
		TVarList& rList = *rVar.GetVarListPtr();

		if (!rList.Add(3))
		{
			rCB.GetErrorList().OutOfMemory(iLine, iPos);
			return false;
		}

		rList[0] = xU;
		rList[1] = xD;
		rList[2] = xV;
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Eigenvalues of Matrix

bool  EigenValuesFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList* pmVars = rPars.GetVarListPtr();
	int iVarCount    = int(pmVars->Count());

	if (iVarCount == 1)
	{
		TMatrix* pMat, Mat;

		if ((*pmVars)(0).BaseType() == PDT_MATRIX)
		{
			pMat = (*pmVars)(0).GetMatrixPtr();
		}
		else if (rCB.CastToMatrix(Mat, (*pmVars)(0), iLine, iPos))
		{
			pMat = &Mat;
		}
		else{ return false; }

		TMatrix& xA = *pMat;
		TMatrix xREVal, xIEVal;

		if (!EigenValues(xA, xREVal, xIEVal))
		{
			rCB.GetErrorList().GeneralError("Error evaluating Eigenvalues", iLine, iPos);
			return false;
		}

		xREVal.Round(rCB.GetSensitivity());
		xIEVal.Round(rCB.GetSensitivity());

		rVar.New(PDT_VARLIST);
		TVarList& rList = *rVar.GetVarListPtr();

		if (!rList.Add(2))
		{
			rCB.GetErrorList().OutOfMemory(iLine, iPos);
			return false;
		}

		rList[0] = xREVal;
		rList[1] = xIEVal;
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Eigenvectors and -values of Matrix

bool  EigenFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList* pmVars = rPars.GetVarListPtr();
	int iVarCount    = int(pmVars->Count());

	if (iVarCount == 1)
	{
		TMatrix* pMat, Mat;

		if ((*pmVars)(0).BaseType() == PDT_MATRIX)
		{
			pMat = (*pmVars)(0).GetMatrixPtr();
		}
		else if (rCB.CastToMatrix(Mat, (*pmVars)(0), iLine, iPos))
		{
			pMat = &Mat;
		}
		else{ return false; }

		TMatrix& xA = *pMat;
		TMatrix xREVal, xIEVal, xREVecs, xIEVecs;

		if (!Eigen(xA, xREVal, xIEVal, xREVecs, xIEVecs))
		{
			rCB.GetErrorList().GeneralError("Error evaluating Eigenvalues and -vectors.", iLine, iPos);
			return false;
		}

		xREVal.Round(rCB.GetSensitivity());
		xIEVal.Round(rCB.GetSensitivity());
		xREVecs.Round(rCB.GetSensitivity());
		xIEVecs.Round(rCB.GetSensitivity());

		rVar.New(PDT_VARLIST);
		TVarList& rList = *rVar.GetVarListPtr();

		if (!rList.Add(4))
		{
			rCB.GetErrorList().OutOfMemory(iLine, iPos);
			return false;
		}

		rList[0] = xREVal;
		rList[1] = xIEVal;
		rList[2] = xREVecs;
		rList[3] = xIEVecs;
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Create Matrix
bool  MatrixFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList* pmVars = rPars.GetVarListPtr();
	int iVarCount    = int(pmVars->Count());

	if (iVarCount == 1)
	{
		rVar.New(PDT_MATRIX);
		TMatrix& rMat = *rVar.GetMatrixPtr();

		return rCB.CastToMatrix(rMat, (*pmVars)(0), iLine, iPos);
	}	// iVarCount == 1
	else if (iVarCount == 2)
	{
		// Dimension of Null-Matrix given
		int iRows = 0, iCols = 0;

		if (!(*pmVars)(0).CastToCounter(iRows))
		{
			rCB.GetErrorList().InvalidParType((*pmVars)(0), 1, iLine, iPos);
			return false;
		}

		if (iRows <= 0)
		{
			CStrMem csText;
			csText = iRows;
			rCB.GetErrorList().InvalidParVal(csText, 1, iLine, iPos);
			return false;
		}

		if (!(*pmVars)(1).CastToCounter(iCols))
		{
			rCB.GetErrorList().InvalidParType((*pmVars)(1), 2, iLine, iPos);
			return false;
		}

		if (iCols <= 0)
		{
			CStrMem csText;
			csText = iCols;
			rCB.GetErrorList().InvalidParVal(csText, 2, iLine, iPos);
			return false;
		}

		TMatrix xA(iRows, iCols);

		rVar = xA;
	}
	else
	{
		int piParNo[] = { 1, 2 };
		rCB.GetErrorList().WrongNoOfParams(piParNo, 2, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Create a single column matrix from a symmetric matrix

bool  Sym2ColMatrixFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount != 1)
	{
		int piParNo[] = { 1 };
		rCB.GetErrorList().WrongNoOfParams(piParNo, 1, iLine, iPos);
		return false;
	}

	TMatrix* pMat, Mat;

	if (mVars(0).BaseType() == PDT_MATRIX)
	{
		pMat = mVars(0).GetMatrixPtr();
	}
	else if (rCB.CastToMatrix(Mat, mVars(0), iLine, iPos))
	{
		pMat = &Mat;
	}
	else{ return false; }

	TMatrix& rM = *pMat;

	if (rM.Rows() != rM.Cols())
	{
		rCB.GetErrorList().GeneralError("Matrix has to be symmetric.", iLine, iPos);
		return false;
	}

	rVar.New(PDT_MATRIX);
	TMatrix& MRes = *rVar.GetMatrixPtr();

	int iDim1, iDimCount, iDim2, iTrgPos, iSrcPos;
	iDimCount = rM.Rows();
	int iSize = iDimCount * iDimCount;

	MRes.Resize(((iSize - iDimCount) >> 1) + iDimCount, 1);

	TCVScalar dFac      = sqrt(2.0);
	TCVScalar* pData    = MRes.Data();
	TCVScalar* pSrcData = rM.Data();

	for (iDim1 = 0, iTrgPos = 0, iSrcPos = 0; iDim1 < iDimCount; iDim1++)
	{
		iSrcPos += iDim1;
		for (iDim2 = iDim1; iDim2 < iDimCount; iDim2++, iTrgPos++, iSrcPos++)
		{
			if (iDim1 == iDim2)
			{
				pData[iTrgPos] = pSrcData[iSrcPos];
			}
			else
			{
				pData[iTrgPos] = dFac * pSrcData[iSrcPos];
			}
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Create a symmetric matrix from a single column matrix

bool  Col2SymMatrixFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount != 1)
	{
		int piParNo[] = { 1 };
		rCB.GetErrorList().WrongNoOfParams(piParNo, 1, iLine, iPos);
		return false;
	}

	TMatrix* pMat, Mat;

	if (mVars(0).BaseType() == PDT_MATRIX)
	{
		pMat = mVars(0).GetMatrixPtr();
	}
	else if (rCB.CastToMatrix(Mat, mVars(0), iLine, iPos))
	{
		pMat = &Mat;
	}
	else{ return false; }

	TMatrix& rM = *pMat;
	int iRows, iCols;

	iRows = rM.Rows();
	iCols = rM.Cols();

	if (iCols != 1)
	{
		rCB.GetErrorList().GeneralError("Matrix has to consist of single column.", iLine, iPos);
		return false;
	}

	double dVal = -0.5 + sqrt(0.25 + 2.0 * double(iRows));
	// dVal should be an integer.

	if ((dVal < 0) || !::IsZero(fmod(dVal, 1), 1e-4))
	{
		rCB.GetErrorList().GeneralError("Column vector cannot be transformed into symmetric matrix.", iLine, iPos);
		return false;
	}

	rVar.New(PDT_MATRIX);
	TMatrix& MRes = *rVar.GetMatrixPtr();
	int iDim1, iDimCount, iDim2, iSrcPos;
	iDimCount = int(dVal);
	MRes.Resize(iDimCount, iDimCount);

	TCVScalar dFac      = 1.0 / sqrt(2.0);
	TCVScalar* pData    = MRes.Data();
	TCVScalar* pSrcData = rM.Data();

	for (iDim1 = 0, iSrcPos = 0; iDim1 < iDimCount; iDim1++)
	{
		for (iDim2 = iDim1; iDim2 < iDimCount; iDim2++, iSrcPos++)
		{
			if (iDim1 == iDim2)
			{
				pData[iDim1 * iDimCount + iDim2] = pSrcData[iSrcPos];
			}
			else
			{
				pData[iDim1 * iDimCount + iDim2] = dFac * pSrcData[iSrcPos];
				pData[iDim2 * iDimCount + iDim1] = dFac * pSrcData[iSrcPos];
			}
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Cast to Matrix

//bool CastToMatrix(CCLUCodeBase &rCB, TMatrix& rMat, CCodeVar& rVar, int iLine, int iPos)
//{
//	if (rVar.BaseType() == PDT_VARLIST)
//	{
//		int i, j;
//		int iRows = 0, iCols = 0;
//		TCVScalar fVal;
//		TVarList &rRowList = *rVar.GetVarListPtr();
//
//		if (rRowList[0].BaseType() == PDT_VARLIST)
//		{
//			// Nested List defines Matrix
//			iRows = rRowList.Count();
//
//			for(i=0;i<iRows;i++)
//			{
//				if (rRowList[i].BaseType() != PDT_VARLIST)
//				{
//					rCB.GetErrorList().InvalidType(rRowList[i], iLine, iPos);
//					return false;
//				}
//
//				TVarList &rRow = *((TVarList *) rRowList[i].GetVarListPtr());
//
//				if (i > 0 && (int) rRow.Count() != iCols)
//				{
//					rCB.GetErrorList().InconsistentRowLength(i+1, iLine, iPos);
//					return false;
//				}
//				else if (i == 0)
//				{
//					// Initialize Matrix Size
//					iCols = rRow.Count();
//					rMat.Resize(iRows, iCols);
//				}
//
//				// Walk along row
//				for(j=0;j<iCols;j++)
//				{
//					if (!rRow[j].CastToScalar(fVal, rCB.GetSensitivity()))
//					{
//						rCB.GetErrorList().InvalidType(rRow[j], iLine, iPos);
//						return false;
//					}
//
//					rMat(i+1, j+1) = fVal;
//				} // for j
//			} // for i
//		}
//		else if (rRowList[0].CastToScalar(fVal, rCB.GetSensitivity()))
//		{
//			// Single List gives column vector
//			iRows = rRowList.Count();
//			rMat.Resize(iRows, 1);
//
//			rMat(1,1) = fVal;
//
//			for(i=1;i<iRows;i++)
//			{
//				if (!rRowList[i].CastToScalar(fVal, rCB.GetSensitivity()))
//				{
//					rCB.GetErrorList().InvalidType(rRowList[i], iLine, iPos);
//					return false;
//				}
//
//				rMat(i+1, 1) = fVal;
//			}
//		}
//		else
//		{
//			rCB.GetErrorList().InvalidParType(rRowList[0], 1, iLine, iPos);
//			return false;
//		}
//	} // type == PDT_VARLIST
//	else if (rVar.BaseType() == PDT_MULTIV)
//	{
//		TMultiV &vA = *rVar.GetMultiVPtr();
//
//		if (vA.BasePtr()->BaseID() == ID_EL2GA)
//		{
//			// Embed conic vector as symmetric 3x3 matrix.
//			rMat = rCB.GetEl2GABase().Mat(vA);
//		}
//#ifdef USE_GA_CC2
//		else if (vA.BasePtr()->BaseID() == ID_CC2GA)
//		{
//			// Embed conic vector as symmetric 3x3 matrix.
//			rMat = m_CC2GABase.Mat(vA);
//		}
//#endif
//		else
//		{
//			rCB.GetErrorList().GeneralError("Multivector is not in a conic space.", iLine, iPos);
//			return false;
//		}
//
//
//	} // type == PDT_MULTIV
//	else if (rVar.BaseType() == PDT_TENSOR_IDX ||
//			 rVar.BaseType() == PDT_TENSOR)
//	{
//		TTensor T, *pT;
//
//		if (rVar.BaseType() == PDT_TENSOR_IDX)
//		{
//			TTensorIdx &rTIdx = *rVar.GetTensorIdxPtr();
//
//			if (!MakeTensor(T, rTIdx))
//			{
//				rCB.GetErrorList().GeneralError("Error creating tensor from index.", iLine, iPos);
//				return false;
//			}
//
//			pT = &T;
//		}
//		else
//		{
//			pT = rVar.GetTensorPtr();
//		}
//
//		TTensor &rT = *pT;
//
//		if (rT.Valence() == 1)
//		{
//			rMat.Resize(rT.DimSize(0), 1);
//
//			memcpy(rMat.Data(), rT.Data(), rT.Size()*sizeof(TCVScalar));
//		}
//		else if (rT.Valence() == 2)
//		{
//			rMat.Resize(rT.DimSize(0), rT.DimSize(1));
//
//			memcpy(rMat.Data(), rT.Data(), rT.Size()*sizeof(TCVScalar));
//		}
//		else
//		{
//			rCB.GetErrorList().GeneralError("Tensor needs to be of valence 1 or 2.", iLine, iPos);
//			return false;
//		}
//	}
//	else
//	{
//		rCB.GetErrorList().InvalidParType(rVar, 1, iLine, iPos);
//		return false;
//	}
//
//	return true;
//}
//
