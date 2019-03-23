////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_N3_ErrorProp.cpp
//
// summary:   Implements the function 3 error property class
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

#include "Func_N3_ErrorProp.h"


//////////////////////////////////////////////////////////////////////
/// Error Propagating Embedding of Vector in Conformal space
//
// Expects one vector and one covariance Matrix
// or one Mouse Mode ID and cov. mat.
// or three Euclidean coordinates and cov. mat.


bool  ErrorPropVecN3Func(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList *pmVars = rPars.GetVarListPtr();

	int iVarCount = int(pmVars->Count());
	TCVScalar pfVec[3];
	int iGAID;

	if (iVarCount == 1 && (*pmVars)(0).BaseType() == PDT_VARLIST)
	{
		pmVars = (*pmVars)(0).GetVarListPtr();
		iVarCount = int(pmVars->Count());
	}

	TVarList& mVars = *pmVars;

	if (iVarCount == 4)
	{
		int i;
		
		for(i=0;i<3;i++)
		{
			if ( !(mVars(i).CastToScalar(pfVec[i], rCB.GetSensitivity())) )
			{
				rCB.GetErrorList().InvalidType(mVars(i), iLine, iPos);
				return false;
			}
		}

		iGAID = ID_E3GA;
	}
	else if (iVarCount == 2)
	{
		int i;
		TCVCounter iVal;
		
		if (mVars(0).CastToCounter(iVal))
		{
			if (iVal < 0 || iVal > 9)
			{
				CStrMem csText;
				csText = iVal;
				rCB.GetErrorList().InvalidParVal(csText.Str(), 1, iLine, iPos);
				return false;
			}

			for(i=0;i<3;i++)
				pfVec[i] = (TCVScalar) rCB.GetTransform()[iVal].pfTrans[i];
	
			iGAID = ID_E3GA;
		}
		else if (mVars(0).BaseType() == PDT_MULTIV)
		{
			TMultiV &vA = *mVars(0).GetMultiVPtr();
			iGAID = int(vA.GetBase().BaseID());

			if (iGAID == ID_E3GA)
			{
				pfVec[0] = vA[rCB.GetE3GABase().iE1];
				pfVec[1] = vA[rCB.GetE3GABase().iE2];
				pfVec[2] = vA[rCB.GetE3GABase().iE3];
			}
			else if (iGAID == ID_PGA)
			{
				rCB.GetErrorList().GeneralError(
					"Embedding of projective vector in conformal space "
					"with error propagation not implemented, yet.", iLine, iPos);
				return false;
			}
			else if (iGAID == ID_CONFGA)
			{
				rCB.GetErrorList().GeneralError(
					"Embedding of conformal vector in conformal space "
					"with error propagation not implemented, yet.", iLine, iPos);
				return false;
			}
			else
			{
				rCB.GetErrorList().GeneralError(
					"Multivector is of unknown type.", iLine, iPos);
				return false;
			}
		}
		else
		{
			rCB.GetErrorList().InvalidType(mVars(0), iLine, iPos);
			return false;
		}	
	}
	else
	{
		int piVal[] = { 2, 4 };
		rCB.GetErrorList().WrongNoOfParams(piVal, 2, iLine, iPos);
		return false;
	}


	if (mVars(iVarCount-1).BaseType() != PDT_MATRIX)
	{
		rCB.GetErrorList().InvalidType(mVars(iVarCount-1), iLine, iPos);
		return false;			
	}

	TMatrix &CovMat = *mVars(iVarCount-1).GetMatrixPtr();

	int i, iRow, iCol;
	TMultiV vX;
	TMatrix xX, xM, xC;

	if (iGAID == ID_E3GA)
	{
		if (CovMat.Rows() != 8 || CovMat.Cols() != 8)
		{
			char pcText[300];
			sprintf_s(pcText, "Covariance matrix has wrong dimensions %dx%d. Expect 8x8 matrix.",
				CovMat.Rows(), CovMat.Cols());
			rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
			return false;
		}
		
		xX.Resize(8, 1);
		xC.Resize(32, 32);

		int iE[3];
		iE[0] = rCB.GetE3GABase().iE1 + 1;
		iE[1] = rCB.GetE3GABase().iE2 + 1;
		iE[2] = rCB.GetE3GABase().iE3 + 1;

		int iNE[5];
		iNE[0] = m_pN3GABase->iE1 + 1;
		iNE[1] = m_pN3GABase->iE2 + 1;
		iNE[2] = m_pN3GABase->iE3 + 1;
		iNE[3] = m_pN3GABase->iEp + 1;
		iNE[4] = m_pN3GABase->iEm + 1;

		// Embed Vector
		vX = m_pN3GABase->NullVec(pfVec[0], pfVec[1], pfVec[2]);
		vX += ( TCVScalar(0.5) * 
			  (CovMat(iE[0],iE[0]) + CovMat(iE[1],iE[1]) + CovMat(iE[2],iE[2])) ) & 
			  m_pN3GABase->vE;

		// Evaluate Covariance Matrix in conformal space of vector.
		xX = TCVScalar(0);

		for(i = 0; i < 3; i++)
			xX(iE[i], 1) = pfVec[i];

		xC = TCVScalar(0);
		for( iRow = 0; iRow < 3; iRow++ )
		{
			for( iCol = 0; iCol < 3; iCol++ )
			{
				xC(iNE[iRow],iNE[iCol]) = CovMat(iE[iRow], iE[iCol]);
			}
		}

		xM = CovMat * xX;
		for( iRow = 0; iRow < 3; iRow++ )
		{
			// Write column vector into column for e+ and e- since
			// it should be written in column for e.
			xC(iNE[iRow], iNE[3]) = xC(iNE[iRow], iNE[4]) = 
			xC(iNE[3], iNE[iRow]) = xC(iNE[4], iNE[iRow]) = xM(iE[iRow], 1);
		}

		xM = (~xX) * xM;
		xC(iNE[3], iNE[4]) = xC(iNE[4], iNE[3]) = 
		xC(iNE[3], iNE[3]) = xC(iNE[4], iNE[4]) = xM(1,1);

	}
	else
	{
		rCB.GetErrorList().GeneralError("Invalid multivector space.", iLine, iPos);
		return false;
	}

	TVarList ResList;

	ResList.Set(2);
	ResList[0] = vX;
	ResList[1] = xC;

	rVar = ResList;

	return true;
}



//////////////////////////////////////////////////////////////////////
/// Error Propagating Embedding of Cross-Covariance of two Vectors in Conformal space
//
// Expects two vectors and one cross-covariance Matrix
// or two Mouse Mode IDs and cross.-cov. mat.
// or six Euclidean coordinates and cross.-cov. mat.


bool  ErrorPropCrossCovVecN3Func(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = int(mVars.Count());
	TCVScalar pfVec[6];
	int iGAID;

	if (iVarCount == 7)
	{
		int i;
		
		for(i=0;i<6;i++)
		{
			if ( !(mVars(i).CastToScalar(pfVec[i], rCB.GetSensitivity())) )
			{
				rCB.GetErrorList().InvalidType(mVars(i), iLine, iPos);
				return false;
			}
		}

		iGAID = ID_E3GA;
	}
	else if (iVarCount == 3)
	{
		int i, iVar;
		TCVCounter iVal;
		
		for( iVar = 0; iVar < 2; iVar++ )
		{
			if (mVars(iVar).CastToCounter(iVal))
			{
				if (iVal < 0 || iVal > 9)
				{
					CStrMem csText;
					csText = iVal;
					rCB.GetErrorList().InvalidParVal(csText.Str(), iVar+1, iLine, iPos);
					return false;
				}

				for(i=0;i<3;i++)
					pfVec[3*iVar + i] = (TCVScalar) rCB.GetTransform()[iVal].pfTrans[i];

				iGAID = ID_E3GA;
			}
			else if (mVars(iVar).BaseType() == PDT_MULTIV)
			{
				TMultiV &vA = *mVars(iVar).GetMultiVPtr();
				iGAID = int(vA.GetBase().BaseID());

				if (iGAID == ID_E3GA)
				{
					pfVec[3*iVar + 0] = vA[rCB.GetE3GABase().iE1];
					pfVec[3*iVar + 1] = vA[rCB.GetE3GABase().iE2];
					pfVec[3*iVar + 2] = vA[rCB.GetE3GABase().iE3];
				}
				else if (iGAID == ID_PGA)
				{
					rCB.GetErrorList().GeneralError(
						"Embedding of projective vector in conformal space "
						"with error propagation not implemented, yet.", iLine, iPos);
					return false;
				}
				else if (iGAID == ID_CONFGA)
				{
					rCB.GetErrorList().GeneralError(
						"Embedding of conformal vector in conformal space "
						"with error propagation not implemented, yet.", iLine, iPos);
					return false;
				}
				else
				{
					rCB.GetErrorList().GeneralError(
						"Multivector is of unknown type.", iLine, iPos);
					return false;
				}
			}
			else
			{
				rCB.GetErrorList().InvalidType(mVars(iVar), iLine, iPos);
				return false;
			}	
		}
	}
	else
	{
		int piVal[] = { 3, 7 };
		rCB.GetErrorList().WrongNoOfParams(piVal, 2, iLine, iPos);
		return false;
	}


	if (mVars(iVarCount-1).BaseType() != PDT_MATRIX)
	{
		rCB.GetErrorList().InvalidType(mVars(iVarCount-1), iLine, iPos);
		return false;			
	}

	TMatrix &CovMat = *mVars(iVarCount-1).GetMatrixPtr();

	int i, iRow, iCol;
	TMatrix xX, xY, xM, xC;

	if (iGAID == ID_E3GA)
	{
		if (CovMat.Rows() != 8 || CovMat.Cols() != 8)
		{
			char pcText[300];
			sprintf_s(pcText, "Covariance matrix has wrong dimensions %dx%d. Expect 8x8 matrix.",
				CovMat.Rows(), CovMat.Cols());
			rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
			return false;
		}
		
		xX.Resize(8, 1);
		xY.Resize(8, 1);
		xC.Resize(32, 32);

		int iE[3];
		iE[0] = rCB.GetE3GABase().iE1 + 1;
		iE[1] = rCB.GetE3GABase().iE2 + 1;
		iE[2] = rCB.GetE3GABase().iE3 + 1;

		int iNE[5];
		iNE[0] = m_pN3GABase->iE1 + 1;
		iNE[1] = m_pN3GABase->iE2 + 1;
		iNE[2] = m_pN3GABase->iE3 + 1;
		iNE[3] = m_pN3GABase->iEp + 1;
		iNE[4] = m_pN3GABase->iEm + 1;

		// Evaluate Covariance Matrix in conformal space of vector.
		xX = TCVScalar(0);
		xY = TCVScalar(0);

		for(i = 0; i < 3; i++)
		{
			xX(iE[i], 1) = pfVec[i];
			xY(iE[i], 1) = pfVec[3 + i];
		}

		xC = TCVScalar(0);
		for( iRow = 0; iRow < 3; iRow++ )
		{
			for( iCol = 0; iCol < 3; iCol++ )
			{
				xC(iNE[iRow],iNE[iCol]) = CovMat(iE[iRow], iE[iCol]);
			}
		}

		xM = CovMat * xY;
		for( iRow = 0; iRow < 3; iRow++ )
		{
			// Write column vector into column for e+ and e- since
			// it should be written in column for e.
			xC(iNE[iRow], iNE[3]) = xC(iNE[iRow], iNE[4]) = xM(iE[iRow], 1);
		}

		xM = (~xX) * CovMat;
		for( iCol = 0; iCol < 3; iCol++ )
		{
			// Write row vector into row for e+ and e- since
			// it should be written in column for e.
			xC(iNE[3], iNE[iCol]) = xC(iNE[4], iNE[iCol]) = xM(1, iE[iCol]);
		}

		xM = xM * xY;
		xC(iNE[3], iNE[4]) = xC(iNE[4], iNE[3]) = 
		xC(iNE[3], iNE[3]) = xC(iNE[4], iNE[4]) = xM(1,1);

	}
	else
	{
		rCB.GetErrorList().GeneralError("Invalid multivector space.", iLine, iPos);
		return false;
	}

	rVar = xC;

	return true;
}




//////////////////////////////////////////////////////////////////////
/// Error Propagating Evaluation of projection of Affine bivector
/// in N3 of the form A^e into E3.
//
// Expects mean bivector and corresponding covariance matrix.
// Third parameter may also be a list of ....
// Returns E3 vector with covariance matrix

bool  ErrorPropPrjVecN3ToE3Func(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	int i;
	TVarList &mVars = *rPars.GetVarListPtr();
	

	int iVarCount = int(mVars.Count());

	if (iVarCount < 1 || iVarCount > 2)
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_VARLIST)
	{
		rCB.GetErrorList().InvalidType(mVars(0), iLine, iPos);
		return false;		
	}

	TVarList& mList = *mVars(0).GetVarListPtr();

	if (mList(0).BaseType() != PDT_MULTIV || mList(1).BaseType() != PDT_MATRIX)
	{
		rCB.GetErrorList().InvalidType(mVars(0), iLine, iPos);
		return false;
	}

	TMultiV &vA = *mList(0).GetMultiVPtr();
	TMatrix &CAA = *mList(1).GetMatrixPtr();

	if (vA.GetBase().BaseID() != ID_CONFGA)
	{
		rCB.GetErrorList().GeneralError("Expect multivector from conformal space.", iLine, iPos);
		return false;
	}

	int iGADim = int(vA.GetGADim());

	if (CAA.Rows() != iGADim || CAA.Cols() != iGADim)
	{
		char pcText[300];
		sprintf_s(pcText, "Covariance matrix is of wrong dimension. Expect %dx%d matrix.", 
						iGADim, iGADim);
		rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
		return false;
	}

	MemObj<TMatrix> mCCList;
	if (iVarCount == 2)
	{
		mVars(1).ConvertSEtoSEL();

		TVarList &MatList = *mVars(1).GetVarListPtr();
		int iMat, iMatCount = int(MatList.Count());
		
		mCCList.Set(iMatCount);
		for (iMat = 0; iMat < iMatCount; iMat++)
		{
			if (MatList(iMat).BaseType() != PDT_MATRIX)
			{
				rCB.GetErrorList().GeneralError("Expect list of cross covariance matrices.", iLine, iPos);
				return false;
			}

			TMatrix &rMat = *MatList(iMat).GetMatrixPtr();

			if (rMat.Rows() != iGADim || rMat.Cols() != iGADim)
			{
				char pcText[300];
				sprintf_s(pcText, "Covariance matrix is of wrong dimension. Expect %dx%d matrix.", 
					iGADim, iGADim);
				rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
				return false;
			}

			mCCList[iMat] = rMat;
		}
	}


	TMultiV vE0 = -m_pN3GABase->vE0;
	TMatrix CNull(iGADim, iGADim);
	CNull = TCVScalar(0);
	
	// Result Multivector andcovariance matrix
	TMultiV vB;
	TMatrix CBB;

	// Evaluate inner product of bivector with -e0
	if (!EvalEPMeanProd(vB, MVOP_INNER, vA, vE0, CNull))
	{
		rCB.GetErrorList().GeneralError("Error evaluating error propagating product of multivector with e0.",
									iLine, iPos);
		return false;
	}

	// Evaluate cov. mat. of inner product of bivector with -e0
	if (!EvalEPCovMatProd(CBB, MVOP_INNER, vA, vE0, CAA, CNull, CNull))
	{
		rCB.GetErrorList().GeneralError("Error evaluating covariance matrix of error propagating product of multivector with e0.",
									iLine, iPos);
		return false;
	}

	// Evaluate cross covariances
	TMatrix CDummy;
	int iMat, iMatCount = int(mCCList.Count());

	for (iMat = 0; iMat < iMatCount; iMat++)
	{
		if (!EvalEPCrossCovMatProd(CDummy, MVOP_INNER, vA, vE0, mCCList[iMat], CNull))
		{
			rCB.GetErrorList().GeneralError("Error evaluating cross-covariance matrix of product of multivectors.", iLine, iPos);
			return false;
		}

		mCCList[iMat] = CDummy;
	}

	TCVScalar fH, fH2, fH3, fH4;

	// Get homogeneous component of vector
	fH = -Scalar(vB * m_pN3GABase->vE);
//	fH = -Scalar(vB * m_pN3GABase->vEm);

	if (fH == TCVScalar(0))
	{
		rCB.GetErrorList().GeneralError("The homogeneous component of the given vector is zero.", 
									iLine, iPos);
		return false;
	}

	int iE[3];
	iE[0] = rCB.GetE3GABase().iE1 + 1;
	iE[1] = rCB.GetE3GABase().iE2 + 1;
	iE[2] = rCB.GetE3GABase().iE3 + 1;

	int iNE[5];
	iNE[0] = m_pN3GABase->iE1 + 1;
	iNE[1] = m_pN3GABase->iE2 + 1;
	iNE[2] = m_pN3GABase->iE3 + 1;
	iNE[3] = m_pN3GABase->iEp + 1;
	iNE[4] = m_pN3GABase->iEm + 1;

	// Evaluate mean vector in E3
	TCVScalar fVal1, fVal2;
	TMultiV vX(rCB.GetE3GABase().GetMVStyle()); // Resultant vector in E3
	TMatrix CXX(8, 8); // Resultant cov. mat.

	// Reset vector and matrix
	vX = TCVScalar(0);
	CXX = TCVScalar(0);

	// Constant values that are needed in following calculation
	fH = TCVScalar(1) / fH;
	fH2 = fH * fH;
	fH3 = fH2 * fH;
	fH4 = fH3 * fH;

	fVal1 = (CBB(iNE[3], iNE[3]) + CBB(iNE[4], iNE[4]) - TCVScalar(2) * CBB(iNE[3], iNE[4]));
//	fVal1 = CBB(iNE[4], iNE[4]);
	fVal2 = fH + fH3 * fVal1;

	// Mean vector in E3
	for (i = 0; i < 3; i++)
	{
		vX[iE[i]-1] =		vB[iNE[i]-1] * fVal2 
						+	fH2 * (CBB(iNE[i], iNE[3]) - CBB(iNE[i], iNE[4]));
	}

	fVal2 = fH4 * fVal1;

	// Covariance Matrix of vX
	int iRow, iCol;
	for ( iRow = 0; iRow < 3; iRow++ )
	{
		for ( iCol = 0; iCol < 3; iCol++ )
		{
			CXX(iE[iRow], iE[iCol]) = 
					fH2 * CBB(iNE[iRow], iNE[iCol]) 
				-	vB[iNE[iRow]-1] * fH3 * ( CBB(iNE[4], iNE[iCol]) - CBB(iNE[3], iNE[iCol]) )
				-	vB[iNE[iCol]-1] * fH3 * ( CBB(iNE[iRow], iNE[4]) - CBB(iNE[iRow], iNE[3]) )
				+	vB[iNE[iRow]-1] * vB[iNE[iCol]-1] * fVal2;
		}
	}

	TVarList ResList, PList;
	
	PList.Set(2);
	PList[0] = vX;
	PList[1] = CXX;

	ResList.Set(1);
	ResList[0] = PList;

	if (iMatCount > 0)
	{
		int iRow, iCol;
		TVarList CList;
		TMatrix CrossCov(8, 32);
		CrossCov = TCVScalar(0);

		CList.Set(iMatCount);

		for (iMat = 0; iMat < iMatCount; iMat++)
		{
			TMatrix &rMat = mCCList[iMat];
			for ( iRow = 0; iRow < 3; iRow++ )
			{
				for ( iCol = 0; iCol < 32; iCol++ )
				{
					CrossCov(iE[iRow], iCol+1) = 
							fH * rMat(iNE[iRow], iCol+1)
						-	fH2 * vB[iNE[iRow]-1] * (rMat(iNE[4], iCol+1) - rMat(iNE[3], iCol+1));
				}
			}

			CList[iMat] = CrossCov;
		}

		ResList.Add(1);
		ResList.Last() = CList;
	}

	rVar = ResList;

	return true;
}
