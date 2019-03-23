////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_C2_ErrorProp.cpp
//
// summary:   Implements the function c 2 error property class
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

#include "Func_C2_ErrorProp.h"

//////////////////////////////////////////////////////////////////////
/// Error Propagating Embedding of Vector in 2d projective conic space
//
// Expects one vector and one covariance Matrix
// or one Mouse Mode ID and cov. mat.
// or three Euclidean coordinates and cov. mat.


bool  ErrorPropVecC2Func(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
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
					"Embedding of projective vector in conic space "
					"with error propagation not implemented, yet.", iLine, iPos);
				return false;
			}
			else if (iGAID == ID_CONFGA)
			{
				rCB.GetErrorList().GeneralError(
					"Embedding of conformal vector in conic space "
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

	TMultiV vX;
	TMatrix xX, xM, xC;
	TCVScalar dSqrt2 = TCVScalar(sqrt(2.0));

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
		xC.Resize(64, 64);

		int iE[3];
		iE[0] = rCB.GetE3GABase().iE1 + 1;
		iE[1] = rCB.GetE3GABase().iE2 + 1;
		iE[2] = rCB.GetE3GABase().iE3 + 1;

		int iNE[6];
		iNE[0] = rCB.GetEl2GABase().iE1 + 1;
		iNE[1] = rCB.GetEl2GABase().iE2 + 1;
		iNE[2] = rCB.GetEl2GABase().iE3 + 1;
		iNE[3] = rCB.GetEl2GABase().iE4 + 1;
		iNE[4] = rCB.GetEl2GABase().iE5 + 1;
		iNE[5] = rCB.GetEl2GABase().iE6 + 1;

		// Embed Vector
		vX = rCB.GetEl2GABase().Vec(pfVec[0], pfVec[1]);
		vX[rCB.GetEl2GABase().iE4] += CovMat(iE[0], iE[0]) / dSqrt2;
		vX[rCB.GetEl2GABase().iE5] += CovMat(iE[1], iE[1]) / dSqrt2;
		vX[rCB.GetEl2GABase().iE6] += CovMat(iE[0], iE[1]);

		// Evaluate Conic Matrix in conformal space of vector.
		TCVScalar dS11, dS22, dS12, dX1, dX2;

		dS11 = CovMat(iE[0], iE[0]);
		dS22 = CovMat(iE[1], iE[1]);
		dS12 = CovMat(iE[0], iE[1]);
		dX1 = pfVec[0];
		dX2 = pfVec[1];

		xC = TCVScalar(0);
		xC(iNE[0], iNE[0]) = dS11;
		xC(iNE[0], iNE[1]) = dS12;
		xC(iNE[1], iNE[0]) = dS12;
		xC(iNE[1], iNE[1]) = dS22;
		
		xC(iNE[0], iNE[3]) = xC(iNE[3], iNE[0]) = dSqrt2 * dX1 * dS11;
		xC(iNE[0], iNE[4]) = xC(iNE[4], iNE[0]) = dSqrt2 * dX2 * dS12;
		xC(iNE[0], iNE[5]) = xC(iNE[5], iNE[0]) = dX2 * dS11 + dX1 * dS12;

		xC(iNE[1], iNE[3]) = xC(iNE[3], iNE[1]) = dSqrt2 * dX1 * dS12;
		xC(iNE[1], iNE[4]) = xC(iNE[4], iNE[1]) = dSqrt2 * dX2 * dS22;
		xC(iNE[1], iNE[5]) = xC(iNE[5], iNE[1]) = dX2 * dS12 + dX1 * dS22;

		xC(iNE[3], iNE[3]) = 2.0 * dX1 * dX1 * dS11;
		xC(iNE[3], iNE[4]) = xC(iNE[4], iNE[3]) = 2.0 * dX1 * dX2 * dS12;
		xC(iNE[3], iNE[5]) = xC(iNE[5], iNE[3]) = dSqrt2 * (dX1 * dX2 * dS11 + dX1 * dX1 * dS12);

		xC(iNE[4], iNE[4]) = 2.0 * dX2 * dX2 * dS22;
		xC(iNE[4], iNE[5]) = xC(iNE[5], iNE[4]) = dSqrt2 * (dX2 * dX2 * dS12 + dX1 * dX2 * dS22);

		xC(iNE[5], iNE[5]) = dX1 * dX1 * dS22 + dX2 * dX2 * dS11 + 2.0 * dX1 * dX2 * dS12;
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


