////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_ErrorProp.cpp
//
// summary:   Implements the function error property class
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

#include "Func_ErrorProp.h"


//////////////////////////////////////////////////////////////////////
/// Error Propagating Operation between two Multivectors
///
/// Expects Prod.Type, (mean, covar), (mean, covar), [cross-covar], [(Cxz, Cyz, Cxw, Cyw, etc.)]

bool  ErrorPropagationOperationFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	

	int iVarCount = int(mVars.Count());
	int i;
	
	if (iVarCount < 3 || iVarCount > 5)
	{
		int piPar[] = {3, 4, 5};
		rCB.GetErrorList().WrongNoOfParams(piPar, 3, iLine, iPos);
		return false;
	}

	int iOpType = 0;
	Mem<int> mMask;

	if (!mVars(0).CastToCounter(iOpType))
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (iOpType < int(MVOP_GEO) || iOpType > int(MVOP_SUB))
	{
		rCB.GetErrorList().InvalidParVal(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_VARLIST)
	{
		rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}

	if (mVars(2).BaseType() != PDT_VARLIST)
	{
		rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
		return false;
	}

	if (iVarCount >= 4 && mVars(3).BaseType() != PDT_MATRIX)
	{
		rCB.GetErrorList().InvalidParType(mVars(3), 4, iLine, iPos);
		return false;
	}

	// Get Lists of Multivector and Covariance Matrix
	TVarList& mListA = *mVars(1).GetVarListPtr();
	TVarList& mListB = *mVars(2).GetVarListPtr();

	if (mListA.Count() != 2 ||
		mListA(0).BaseType() != PDT_MULTIV ||
		mListA(1).BaseType() != PDT_MATRIX)
	{
		rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}

	if (mListB.Count() != 2 ||
		mListB(0).BaseType() != PDT_MULTIV ||
		mListB(1).BaseType() != PDT_MATRIX)
	{
		rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
		return false;
	}

	// Get Multivectors
	TMultiV &vA = *mListA(0).GetMultiVPtr();
	TMultiV &vB = *mListB(0).GetMultiVPtr();

	if (vA.GetBase().BaseID() != vB.GetBase().BaseID())
	{
		rCB.GetErrorList().GeneralError("Multivectors are from different spaces.", iLine, iPos);
		return false;
	}

	int iGADim = int(vA.GetGADim());

	TMatrix &Ca = *mListA(1).GetMatrixPtr();
	if (int(Ca.Rows()) != iGADim || int(Ca.Cols()) != iGADim)
	{
		char pcText[300];
		sprintf_s(pcText, "Dimensions of matrix in parameter two are wrong. Expect %dx%d matrices.",
						iGADim, iGADim);
		rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
		return false;
	}

	TMatrix &Cb = *mListB(1).GetMatrixPtr();
	if (int(Cb.Rows()) != iGADim || int(Cb.Cols()) != iGADim)
	{
		char pcText[300];
		sprintf_s(pcText, "Dimensions of matrix in parameter three are wrong. Expect %dx%d matrices.",
						iGADim, iGADim);
		rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
		return false;
	}

	TMatrix *pCab, CabNull;
	if (iVarCount >= 4)
	{
		pCab = mVars(3).GetMatrixPtr();
		if (int(pCab->Rows()) != iGADim || int(pCab->Cols()) != iGADim)
		{
			char pcText[300];
			sprintf_s(pcText, "Dimensions of cross-covariance matrix in parameter four are wrong. Expect %dx%d matrices.",
				iGADim, iGADim);
			rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
			return false;
		}
	}
	else
	{
		CabNull.Resize(iGADim, iGADim);
		CabNull = TCVScalar(0);
		pCab = &CabNull;
	}

	// Cross-Covariance Matrix of MVs A and B.
	TMatrix &Cab = *pCab;

	// Result Multivector
	TMultiV vC;

	// Result Covariance Matrix
	TMatrix Cuu;

	// Evaluate Mean of MV product
	if (!EvalEPMeanProd(vC, EMVOpType(iOpType), vA, vB, Cab))
	{
		rCB.GetErrorList().GeneralError("Error evaluating mean of product of multivectors.", iLine, iPos);
		return false;
	}

	// Evaluate Covariance Matrix of Result
	if (!EvalEPCovMatProd(Cuu, EMVOpType(iOpType), vA, vB, Ca, Cb, Cab))
	{
		rCB.GetErrorList().GeneralError("Error evaluating covariance matrix of product of multivectors.", iLine, iPos);
		return false;
	}

	// Create Code Variable List which is returned
	TVarList ResList, List;

	List.Set(2);
	List[0] = vC;
	List[1] = Cuu;

	ResList.Set(1);
	ResList[0] = List;

	// Cross-Covariance Matrix List
	TVarList CrossCovList;

	if (iVarCount == 5)
	{
		int iCCCount;
		TMatrix Cuz;

		// Get Cross-Covariance Matrix List
		TVarList &rCCovMatList = *mVars(4).GetVarListPtr();

		if ((iCCCount = int(rCCovMatList.Count())) % 2 != 0)
		{
			rCB.GetErrorList().GeneralError("Not an even number of cross-covariance matrices given.", iLine, iPos);
			return false;
		}

		CrossCovList.Set(iCCCount / 2);

		for ( i = 0; i < iCCCount; i += 2 )
		{
			if (rCCovMatList(i).BaseType() != PDT_MATRIX)
			{
				rCB.GetErrorList().GeneralError("Expect list of cross-covariance matrices as fifth parameter.", iLine, iPos);
				return false;
			}

			TMatrix &Cxz = *rCCovMatList(i).GetMatrixPtr();
			if (int(Cxz.Rows()) != iGADim || int(Cxz.Cols()) != iGADim)
			{
				char pcText[300];
				sprintf_s(pcText, "Dimensions of matrix %d in cross-covariance list are wrong. Expect %dx%d matrices.",
					i+1, iGADim, iGADim);
				rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
				return false;
			}

			TMatrix &Cyz = *rCCovMatList(i+1).GetMatrixPtr();
			if (int(Cyz.Rows()) != iGADim || int(Cyz.Cols()) != iGADim)
			{
				char pcText[300];
				sprintf_s(pcText, "Dimensions of matrix %d in cross-covariance list are wrong. Expect %dx%d matrices.",
					i+2, iGADim, iGADim);
				rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
				return false;
			}

			if (!EvalEPCrossCovMatProd(Cuz, EMVOpType(iOpType), vA, vB, Cxz, Cyz))
			{
				rCB.GetErrorList().GeneralError("Error evaluating cross-covariance matrix of product of multivectors.", iLine, iPos);
				return false;
			}

			CrossCovList[i/2] = Cuz;
		}

		ResList.Add(1);
		ResList.Last() = CrossCovList;
	}

	rVar = ResList;

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Error Propagating Evaluation of square root of scalar
//
// Expects mean value of square of value and corresponding variance.
// Returns initial value squared and initial variance, however, not necessarily
// in this order. This function uses particular approximation which cannot
// distinguish between mean value part and variance.

bool  ErrorPropSQRTFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList &mVars = *rPars.GetVarListPtr();
	

	int iVarCount = int(mVars.Count());
	TCVScalar pfVec[2], fVal1, fVal2, fVal;

	if (iVarCount != 1 && iVarCount != 2)
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
	int i;

	for(i=0;i<2;i++)
	{
		if ( !(mList(i).CastToScalar(pfVec[i], rCB.GetSensitivity())) )
		{
			rCB.GetErrorList().InvalidType(mVars(0), iLine, iPos);
			return false;
		}
	}

	if ((fVal = pfVec[0]*pfVec[0] - pfVec[1]) < TCVScalar(0))
	{
		rCB.GetErrorList().GeneralError("The given parameters have invalid values.", iLine, iPos);
		return false;
	}

	fVal1 = TCVScalar(0.5) * (pfVec[0] + TCVScalar(sqrt(fVal)));
	fVal2 = TCVScalar(0.5) * (pfVec[0] - TCVScalar(sqrt(fVal)));

	TVarList ResList, List1, List2;

	List1.Set(2);
	List1[0] = sqrt(fVal1);
	List1[1] = fVal2;

/*
	List2.Set(2);
	List2[0] = sqrt(fVal2);
	List2[1] = fVal1;
*/

	ResList.Set(1);
	ResList[0] = List1;

	// Cross covariance matrices given
	// matrices may be of any dimension
	if (iVarCount == 2)
	{
		fVal = TCVScalar(0.5) / TCVScalar(sqrt(pfVec[0]));

		int iCol, iColNo, iMat, iMatNo;	
		TCVScalar fCov;

		TVarList ResMList;
		TVarList &MList = *mVars(1).GetVarListPtr();
		iMatNo = int(MList.Count());

		ResMList.Set(iMatNo);

		for (iMat = 0; iMat < iMatNo; iMat++)
		{
			if (MList(iMat).BaseType() == PDT_MATRIX)
			{
				TMatrix &xA = *MList(iMat).GetMatrixPtr();
				TMatrix xB;

				xB.Resize(xA.Rows(), xA.Cols());
				xB = TCVScalar(0);

				iColNo = xA.Cols();
				for(iCol = 1; iCol <= iColNo; iCol++)
				{
					xB(1,iCol) = fVal * xA(1,iCol);
				}

				ResMList[iMat] = xB;
			}
			else if (MList[iMat].CastToScalar(fCov, rCB.GetSensitivity()))
			{
				ResMList[iMat] = fVal * fCov;
			}
			else
			{
				rCB.GetErrorList().InvalidType(mVars(1), iLine, iPos);
				return false;		
			}
		}

		ResList.Add(1);
		ResList.Last() = ResMList;
	}


	rVar = ResList;

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Error Propagating Evaluation of inverse of scalar
//
// Expects mean value of value and corresponding variance.
// Returns inverse value and corresponding variance.

bool  ErrorPropINVFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList &mVars = *rPars.GetVarListPtr();
	

	int iVarCount = int(mVars.Count());
	TCVScalar pfVec[2], fVal1, fVal2, fVal;

	if (iVarCount < 1 || iVarCount > 2)
	{
		int piPars[] = { 1, 2 };
		rCB.GetErrorList().WrongNoOfParams(piPars, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_VARLIST)
	{
		rCB.GetErrorList().InvalidType(mVars(0), iLine, iPos);
		return false;		
	}

	TVarList& mList = *mVars(0).GetVarListPtr();
	int i;

	for(i=0;i<2;i++)
	{
		if ( !(mList(i).CastToScalar(pfVec[i], rCB.GetSensitivity())) )
		{
			rCB.GetErrorList().InvalidType(mVars(0), iLine, iPos);
			return false;
		}
	}

	if (pfVec[0] == TCVScalar(0))
	{
		rCB.GetErrorList().GeneralError("The given parameters have invalid values.", iLine, iPos);
		return false;
	}

	fVal = TCVScalar(1) / pfVec[0];

	fVal1 = fVal + pow(fVal, 3) * pfVec[1];
	fVal2 = pow(fVal, 4) * pfVec[1];

	TVarList ResList, PList;
	
	PList.Set(2);
	PList[0] = fVal1;
	PList[1] = fVal2;

	ResList.Set(1);
	ResList[0] = PList;

	if (iVarCount == 2)
	{
		mVars(1).ConvertSEtoSEL();

		TVarList& mList = *mVars(1).GetVarListPtr();
		int i, iCol, iColNo, iCount = int(mList.Count());

		TVarList CList;
		CList.Set(iCount);
		fVal1 = -fVal*fVal;

		for(i=0;i<iCount;i++)
		{
			if (mList(i).CastToScalar(fVal2, rCB.GetSensitivity()))
			{
				CList[i] = fVal1 * fVal2;
			}
			else if (mList(i).BaseType() == PDT_MATRIX)
			{
				TMatrix &xA = *mList(i).GetMatrixPtr();
				TMatrix xB;

				xB.Resize(xA.Rows(), xA.Cols());
				xB = TCVScalar(0);

				iColNo = xA.Cols();
				for(iCol = 1; iCol <= iColNo; iCol++)
				{
					xB(1,iCol) = fVal * xA(1,iCol);
				}

				CList[i] = xB;
			}
			else
			{
				rCB.GetErrorList().InvalidType(mVars(1), iLine, iPos);
				return false;
			}
		}

		ResList.Add(1);
		ResList.Last() = CList;
	}

	rVar = ResList;

	return true;
}


//////////////////////////////////////////////////////////////////////
/// Error Propagating Evaluation of product of two scalars
//
// Expects mean value and corresponding variance for both values as two lists.
// Third parameter gives optionally the cross-covariance.
// Returns mean value of product and corresponding variance.

bool  ErrorPropMULFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList &mVars = *rPars.GetVarListPtr();
	

	int iVarCount = int(mVars.Count());
	TCVScalar pfVec[2][2], fCC, fVal1, fVal2;

	if (iVarCount < 2 || iVarCount > 4)
	{
		int piVars[] = { 2, 3, 4 };
		rCB.GetErrorList().WrongNoOfParams(piVars, 3, iLine, iPos);
		return false;
	}

	int i, j;

	for(i=0;i<2;i++)
	{
		if (mVars(i).BaseType() != PDT_VARLIST)
		{
			rCB.GetErrorList().InvalidType(mVars(i), iLine, iPos);
			return false;
		}

		TVarList& mList = *mVars(i).GetVarListPtr();
		if (mList.Count() != 2)
		{
			rCB.GetErrorList().GeneralError("List is not of length two.", iLine, iPos);
			return false;
		}

		for (j = 0; j < 2; j++)
		{
			if ( !(mList(j).CastToScalar(pfVec[i][j], rCB.GetSensitivity())) )
			{
				rCB.GetErrorList().InvalidType(mVars(i), iLine, iPos);
				return false;
			}
		}
	}

	// Is cross-covariance given?
	if (iVarCount == 3)
	{
		if ( !(mVars(2).CastToScalar(fCC, rCB.GetSensitivity())) )
		{
			rCB.GetErrorList().InvalidType(mVars(2), iLine, iPos);
			return false;
		}
	}
	else
	{
		fCC = TCVScalar(0);
	}

	fVal1 = pfVec[0][0] * pfVec[1][0] + fCC;
	fVal2 = pfVec[1][0] * pfVec[1][0] * pfVec[0][1] + 
			pfVec[0][0] * pfVec[0][0] * pfVec[1][1] + 
			TCVScalar(2) * pfVec[0][0] * pfVec[1][0] * fCC;

	TVarList ResList, List;

	List.Set(2);
	List[0] = fVal1;
	List[1] = fVal2;

	ResList.Set(1);
	ResList[0] = List;

	// Check whether cross.cov.mat. pairs are given.
	if (iVarCount == 4)
	{
		if (mVars(3).BaseType() != PDT_VARLIST)
		{
			rCB.GetErrorList().InvalidType(mVars(3), iLine, iPos);
			return false;		
		}		

		TVarList& mList = *mVars(3).GetVarListPtr();
		int i, iCount = int(mList.Count());

		if (iCount%2 != 0)
		{
			rCB.GetErrorList().GeneralError("Expect even number of cross-covariance matrices.", iLine, iPos);
			return false;
		}

		TVarList CList;
		CList.Set(iCount/2);

		for(i = 0;i < iCount;i += 2)
		{
			if (mList(i).CastToScalar(fVal1, rCB.GetSensitivity()) &&
				mList(i+1).CastToScalar(fVal2, rCB.GetSensitivity()))
			{
				CList[i/2] = pfVec[1][0] * fVal1 + pfVec[0][0] * fVal2;
			}
			else if (mList(i).BaseType() == PDT_MATRIX &&
					 mList(i+1).BaseType() == PDT_MATRIX)
			{
				TMatrix &Cxz = *mList(i).GetMatrixPtr();
				TMatrix &Cyz = *mList(i+1).GetMatrixPtr();
				TMatrix Cuz;

				CList[i/2] = pfVec[1][0] * Cxz + pfVec[0][0] * Cyz;
			}
			else
			{
				rCB.GetErrorList().InvalidType(mVars(3), iLine, iPos);
				return false;
			}
		}

		ResList.Add(1);
		ResList.Last() = CList;
	}

	rVar = ResList;

	return true;
}



