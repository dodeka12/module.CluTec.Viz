////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_Math.cpp
//
// summary:   Implements the function mathematics class
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
#include <float.h>

#include "CluTec.Viz.Parse\CLUCodeBase.h"
#include "CluTec.Viz.Base\TensorOperators.h"

#ifndef WIN32

    #include <math.h>
	#define _isnan isnan

#endif

#include "Func_Math.h"

//////////////////////////////////////////////////////////////////////
// Set Evaluation Precision FUNCTION
//

bool  SetEvalPrecFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());
	TCVCounter iPow;

	if (iVarCount != 1)
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToCounter(iPow))
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if ((iPow < 0) || (iPow > 12))
	{
		rCB.GetErrorList().GeneralError("Value for precision has to be in the range {0,...,12}", iLine, iPos);
		return false;
	}

	rCB.GetSensitivity() = TCVScalar(pow(0.1, double(iPow)));

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Factorial FUNCTION
///
/// evaluates factorial up to 170!

bool  FactFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	int iVal;
	TCVScalar dRes;

	if (iVarCount == 1)
	{
		if (!mVars(0).CastToCounter(iVal))
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if ((iVal < 0) || (iVal > 170))
		{
			CStrMem csText;
			csText = iVal;
			rCB.GetErrorList().InvalidParVal(csText, 1, iLine, iPos);
			return false;
		}

		dRes = 1;
		for (; iVal > 1; iVal--)
		{
			dRes *= (TCVScalar) iVal;
		}

		rVar = dRes;
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Sign FUNCTION
///
/// evaluates the sign of a scalar

bool  SignFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount == 1)
	{
		if (!Sign(rCB, rVar, mVars(0), iLine, iPos))
		{
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

bool  Sign(CCLUCodeBase& rCB,  CCodeVar& rVar, CCodeVar& rPar, int iLine, int iPos)
{
	TCVScalar dVal, dRes;

	if (rPar.BaseType() == PDT_VARLIST)
	{
		TVarList& rList = *rPar.GetVarListPtr();
		int iPos, iCnt = int(rList.Count());

		rVar.New(PDT_VARLIST);
		TVarList& rResList = *rVar.GetVarListPtr();
		rResList.Set(iCnt);

		for (iPos = 0; iPos < iCnt; ++iPos)
		{
			if (!Sign(rCB, rResList(iPos), rList(iPos), iLine, iPos))
			{
				return false;
			}
		}

		return true;
	}

	if (!rPar.CastToScalar(dVal))
	{
		rCB.GetErrorList().GeneralError("Expect scalar value or list.", iLine, iPos);
		return false;
	}

	if (dVal >= 0)
	{
		dRes = 1;
	}
	else
	{
		dRes = -1;
	}

	rVar = dRes;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// The floor FUNCTION

bool  FloorFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount == 1)
	{
		return Floor(rCB, rVar, mVars(0), iLine, iPos);
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}
}

//////////////////////////////////////////////////////////////////////
/// Evaluate floor

bool  Floor(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rDataVar, int iLine, int iPos)
{
	ECodeDataType eType = rDataVar.BaseType();
	TCVScalar fVal;

	if (eType == PDT_VARLIST)
	{
		TVarList& rList = *rDataVar.GetVarListPtr();
		int i, iCount = int(rList.Count());

		rVar.New(PDT_VARLIST);
		TVarList& rFloorList = *rVar.GetVarListPtr();
		rFloorList.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!Floor(rCB, rFloorList[i], rList(i), iLine, iPos))
			{
				return false;
			}
		}
	}
	else if (rDataVar.CastToScalar(fVal, rCB.GetSensitivity()))
	{
		rVar = (TCVScalar) floor((double) fVal);
	}
	else if (rDataVar.BaseType() == PDT_MULTIV)
	{
		rVar.New(PDT_MULTIV);
		TMultiV& vA = *rVar.GetMultiVPtr();
		vA = *rDataVar.GetMultiVPtr();
		vA.Floor();
	}
	else
	{
		rCB.GetErrorList().InvalidParType(rDataVar, 1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// The ceiling FUNCTION

bool  CeilFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount == 1)
	{
		return Ceil(rCB, rVar, mVars(0), iLine, iPos);
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}
}

//////////////////////////////////////////////////////////////////////
/// Evaluate ceiling

bool  Ceil(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rDataVar, int iLine, int iPos)
{
	ECodeDataType eType = rDataVar.BaseType();
	TCVScalar fVal;

	if (eType == PDT_VARLIST)
	{
		TVarList& rList = *rDataVar.GetVarListPtr();
		int i, iCount = int(rList.Count());

		rVar.New(PDT_VARLIST);
		TVarList& rFloorList = *rVar.GetVarListPtr();
		rFloorList.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!Ceil(rCB, rFloorList[i], rList(i), iLine, iPos))
			{
				return false;
			}
		}
	}
	else if (rDataVar.CastToScalar(fVal, rCB.GetSensitivity()))
	{
		rVar = (TCVScalar) ceil((double) fVal);
	}
	else if (rDataVar.BaseType() == PDT_MULTIV)
	{
		rVar.New(PDT_MULTIV);
		TMultiV& vA = *rVar.GetMultiVPtr();
		vA = *rDataVar.GetMultiVPtr();
		vA.Ceil();
	}
	else
	{
		rCB.GetErrorList().InvalidParType(rDataVar, 1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// The Round FUNCTION

bool  RoundFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	TCVCounter iPrec;
	TCVScalar dPrec;
	int iVarCount = int(mVars.Count());

	if (iVarCount >= 1)
	{
		if (iVarCount == 2)
		{
			if (!mVars(1).CastToCounter(iPrec))
			{
				rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
				return false;
			}

			dPrec = pow(0.1, double(iPrec));
			if (dPrec == 0.0)
			{
				rCB.GetErrorList().GeneralError("Precision value out of range.", iLine, iPos);
				return false;
			}
		}
		else
		{
			dPrec = rCB.GetSensitivity();
		}

		return Round(rCB, rVar, mVars(0), dPrec, iLine, iPos);
	}
	else
	{
		int piParNo[] = { 1, 2 };
		rCB.GetErrorList().WrongNoOfParams(piParNo, 2, iLine, iPos);
		return false;
	}
}

//////////////////////////////////////////////////////////////////////
/// Evaluate floor

bool  Round(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rDataVar, TCVScalar dPrec, int iLine, int iPos)
{
	ECodeDataType eType = rDataVar.BaseType();
	TCVScalar fVal;

	if (eType == PDT_VARLIST)
	{
		TVarList& rList = *rDataVar.GetVarListPtr();
		int i, iCount = int(rList.Count());

		rVar.New(PDT_VARLIST);
		TVarList& rFloorList = *rVar.GetVarListPtr();
		rFloorList.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!Round(rCB, rFloorList[i], rList(i), dPrec, iLine, iPos))
			{
				return false;
			}
		}
	}
	else if (rDataVar.CastToScalar(fVal, rCB.GetSensitivity()))
	{
		rVar = (TCVScalar)::Round(fVal, dPrec);
	}
	else if (rDataVar.BaseType() == PDT_MULTIV)
	{
		rVar.New(PDT_MULTIV);
		TMultiV& vA = *rVar.GetMultiVPtr();
		vA = *rDataVar.GetMultiVPtr();
		vA.Round(dPrec);
	}
	else if (rDataVar.BaseType() == PDT_MATRIX)
	{
		rVar.New(PDT_MATRIX);
		TMatrix& mA = *rVar.GetMatrixPtr();
		mA = *rDataVar.GetMatrixPtr();
		mA.Round(dPrec);
	}
	else
	{
		rCB.GetErrorList().InvalidParType(rDataVar, 1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// The Exponential FUNCTION

bool  ExpFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount == 1)
	{
		if (!Exponential(rCB, rVar, mVars(0), iLine, iPos))
		{
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
/// The Exponential

bool  Exponential(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rData, int iLine, int iPos)
{
	ECodeDataType eType = rData.BaseType();
	TCVScalar fVal;

	if (rData.CastToScalar(fVal, rCB.GetSensitivity()))
	{
		rVar = (TCVScalar) exp((double) fVal);
	}
	else if (eType == PDT_MULTIV)
	{
		TMultiV& vA = *rData.GetMultiVPtr();

		rVar = Exp(vA, 12);
	}
	else if (eType == PDT_VARLIST)
	{
		// Loop over all elements of list and call this function recursively.
		TVarList& List = *rData.GetVarListPtr();
		int i, iCount = int(List.Count());

		rVar.New(PDT_VARLIST);
		TVarList& ResultList = *rVar.GetVarListPtr();

		ResultList.Set(iCount);
		for (i = 0; i < iCount; i++)
		{
			if (!Exponential(rCB, ResultList[i], List[i], iLine, iPos))
			{
				return false;
			}
		}
	}
	else if (eType == PDT_MATRIX)
	{
		rVar = *rData.GetMatrixPtr();
		TMatrix& rMat = *rVar.GetMatrixPtr();

		// Take exp of matrix components separately
		rMat.ExpComps();
	}
	else
	{
		rCB.GetErrorList().InvalidParType(rData, 1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Log FUNCTION
bool  LogFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount == 1)
	{
		return Log(rCB, rVar, mVars(0), iLine, iPos);
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}
}

//////////////////////////////////////////////////////////////////////
/// Cosine
bool  Log(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rData, int iLine, int iPos)
{
	TCVScalar dVal;

	if (rData.BaseType() == PDT_VARLIST)
	{
		TVarList& rList = *rData.GetVarListPtr();
		int i, iCount = int(rList.Count());

		rVar.New(PDT_VARLIST);
		TVarList& rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!Log(rCB, rRetList[i], rList(i), iLine, iPos))
			{
				return false;
			}
		}
	}
	else if (rData.BaseType() == PDT_MATRIX)
	{
		rVar = *rData.GetMatrixPtr();
		TMatrix& rMat = *rVar.GetMatrixPtr();

		// Take log of matrix components separately
		rMat.LogComps();
	}
	else
	{
		if (!rData.CastToScalar(dVal, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(rData, 1, iLine, iPos);
			return false;
		}

		if (dVal > 0.0)
		{
			rVar = (TCVScalar) log((double) dVal);
		}
		else
		{
			rCB.GetErrorList().GeneralError("Parameter has to be greater than zero.", iLine, iPos);
			return false;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Absolute FUNCTION

bool  AbsFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount == 1)
	{
		return Abs(rCB, rVar, mVars(0), iLine, iPos);
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}
}

//////////////////////////////////////////////////////////////////////
/// Absolute Value

bool  Abs(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rData, int iLine, int iPos)
{
	TCVScalar dVal;
	MultiV<TCVScalar> MV;
	TMatrix Mat;
	ECodeDataType eType = rData.BaseType();

	if (eType == PDT_VARLIST)
	{
		TVarList& rList = *rData.GetVarListPtr();
		int i, iCount = int(rList.Count());

		rVar.New(PDT_VARLIST);
		TVarList& rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!Abs(rCB, rRetList[i], rList(i), iLine, iPos))
			{
				return false;
			}
		}
	}
	else
	{
		switch (eType)
		{
		case PDT_INT:
			rVar = Mag(*((int*) rData.GetIntPtr()));
			break;

		case PDT_UINT:
			rVar = *((uint*) rData.GetUIntPtr());
			break;

		case PDT_LONG:
			rVar = Mag(*((long*) rData.GetLongPtr()));
			break;

		case PDT_FLOAT:
			rVar = Mag(*((float*) rData.GetFloatPtr()));
			break;

		case PDT_DOUBLE:
			rVar = Mag(*((double*) rData.GetDoublePtr()));
			break;

		case PDT_MULTIV:
			MV = *((MultiV<TCVScalar>*)rData.GetMultiVPtr());
			if (!rData.CastToScalar(dVal, rCB.GetSensitivity()))
			{
				rVar = TCVScalar(sqrt(double(Mag(Scalar(MV * (~MV))))));
			}
			else
			{
				rVar = Mag(dVal);
			}
			break;

		case PDT_MATRIX:
			rVar = *rData.GetMatrixPtr();
			rVar.GetMatrixPtr()->AbsComps();
			break;

		default:
			rCB.GetErrorList().InvalidType(rData, iLine, iPos);
			return false;
		}
	}

	return true;
}

/*
//////////////////////////////////////////////////////////////////////
/// Argument FUNCTION

bool  ArgFunc(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
        TVarList& mVars = *rPars.GetVarListPtr();
        int iVarCount = mVars.Count();

        if (iVarCount == 1)
        {
                return Arg(rVar, mVars(0), iLine, iPos);
        }
        else
        {
                rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
                return false;
        }

        return true;
}


//////////////////////////////////////////////////////////////////////
/// Argument Value

bool  Arg(CCLUCodeBase &rCB, CCodeVar& rVar, CCodeVar& rData, int iLine, int iPos)
{
        TCVScalar dVal;
        MultiV<TCVScalar> MV;
        TMatrix Mat;
        ECodeDataType eType = rData.BaseType();

        if (eType == PDT_VARLIST)
        {
                TVarList &rList = *rData.GetVarListPtr();
                int i, iCount = rList.Count();

                rVar.New(PDT_VARLIST);
                TVarList &rRetList = *rVar.GetVarListPtr();
                rRetList.Set(iCount);

                for ( i = 0; i < iCount; i++)
                {
                        if (!Arg(rRetList[i], rList(i), iLine, iPos))
                                return false;
                }
        }
        else
        {
                switch(eType)
                {
                case PDT_INT:
                        rVar = Mag( *((int *) rData.GetIntPtr()) );
                        break;

                case PDT_UINT:
                        rVar = *((uint *) rData.GetUIntPtr());
                        break;

                case PDT_LONG:
                        rVar = Mag( *((long *) rData.GetLongPtr()) );
                        break;

                case PDT_FLOAT:
                        rVar = Mag( *((float *) rData.GetFloatPtr()) );
                        break;

                case PDT_DOUBLE:
                        rVar = Mag( *((double *) rData.GetDoublePtr()) );
                        break;

                case PDT_MULTIV:
                        MV = *((MultiV<TCVScalar> *) rData.GetMultiVPtr());
                        if (!rData.CastToScalar(dVal, rCB.GetSensitivity()))
                        {
                                rVar = TCVScalar(sqrt(double(Mag(Scalar(MV * (~MV))))));
                        }
                        else
                        {
                                rVar = Mag(dVal);
                        }
                        break;

                case PDT_MATRIX:
                        Mat = *rData.GetMatrixPtr();
                        rVar = Mat.AbsComps();
                        break;

                default:
                        rCB.GetErrorList().InvalidType(rData, iLine, iPos);
                        return false;
                }
        }

        return true;
}

*/

//////////////////////////////////////////////////////////////////////
/// Summation FUNCTION

bool  SumFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount == 1)
	{
		if (mVars(0).BaseType() == PDT_VARLIST)
		{
			return Sum(rCB, rVar, *mVars(0).GetVarListPtr(), iLine, iPos);
		}
		else if (mVars(0).BaseType() == PDT_MATRIX)
		{
			TMatrix& mA = *mVars(0).GetMatrixPtr();

			TCVScalar dSum;
			int iR, iC, iRows, iCols;
			iRows = mA.Rows();
			iCols = mA.Cols();

			if (!iRows || !iCols)
			{
				rCB.GetErrorList().GeneralError("Matrix passed is empty.", iLine, iPos);
				return false;
			}

			// Only sum rows separately.
			rVar.New(PDT_MATRIX);
			TMatrix& rColMat = *rVar.GetMatrixPtr();
			rColMat.Resize(iRows, 1);

			for (iR = 1; iR <= iRows; iR++)
			{
				dSum = mA(iR, 1);
				for (iC = 2; iC <= iCols; iC++)
				{
					dSum += mA(iR, iC);
				}

				rColMat(iR, 1) = dSum;
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
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Sum of  List

bool  Sum(CCLUCodeBase& rCB, CCodeVar& rVar, TVarList& rList, int iLine, int iPos)
{
	MultiV<TCVScalar> MV;
	TMatrix Mat;

	int i, iCount = int(rList.Count());
	if (!iCount)
	{
		return true;
	}

	if (rList(0).BaseType() == PDT_VARLIST)
	{
		rVar.New(PDT_VARLIST);
		TVarList& rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			CCodeVar& rSubVar = rList(i);
			if (rSubVar.BaseType() == PDT_VARLIST)
			{
				TVarList& rSubList = *rSubVar.GetVarListPtr();

				if (!Sum(rCB, rRetList[i], rSubList, iLine, iPos))
				{
					return false;
				}
			}
			else
			{
				rCB.GetErrorList().GeneralError("Expect nested lists.", iLine, iPos);
				return false;
			}
		}
	}
	else
	{
		CCodeVar ValVar;
		rVar = rList(0);

		for (i = 1; i < iCount; i++)
		{
			if (!rCB.OpAdd(rVar, rList(i), ValVar, iLine, iPos))
			{
				return false;
			}

			rVar = ValVar;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Subtraction FUNCTION

bool  SubFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount == 1)
	{
		if (mVars(0).BaseType() == PDT_VARLIST)
		{
			return Sub(rCB, rVar, *mVars(0).GetVarListPtr(), iLine, iPos);
		}
		else if (mVars(0).BaseType() == PDT_MATRIX)
		{
			TMatrix& mA = *mVars(0).GetMatrixPtr();

			TCVScalar dSum;
			int iR, iC, iRows, iCols;
			iRows = mA.Rows();
			iCols = mA.Cols();

			if (!iRows || !iCols)
			{
				rCB.GetErrorList().GeneralError("Matrix passed is empty.", iLine, iPos);
				return false;
			}

			// Only sum rows separately.
			rVar.New(PDT_MATRIX);
			TMatrix& rColMat = *rVar.GetMatrixPtr();
			rColMat.Resize(iRows, 1);

			for (iR = 1; iR <= iRows; iR++)
			{
				dSum = mA(iR, 1);
				for (iC = 2; iC <= iCols; iC++)
				{
					dSum -= mA(iR, iC);
				}

				rColMat(iR, 1) = dSum;
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
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Subtraction of  List
/// From first element the rest is subtracted.

bool  Sub(CCLUCodeBase& rCB, CCodeVar& rVar, TVarList& rList, int iLine, int iPos)
{
	MultiV<TCVScalar> MV;
	TMatrix Mat;

	int i, iCount = int(rList.Count());
	if (!iCount)
	{
		return true;
	}

	if (rList(0).BaseType() == PDT_VARLIST)
	{
		rVar.New(PDT_VARLIST);
		TVarList& rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			CCodeVar& rSubVar = rList(i);
			if (rSubVar.BaseType() == PDT_VARLIST)
			{
				TVarList& rSubList = *rSubVar.GetVarListPtr();

				if (!Sub(rCB, rRetList[i], rSubList, iLine, iPos))
				{
					return false;
				}
			}
			else
			{
				rCB.GetErrorList().GeneralError("Expect nested lists.", iLine, iPos);
				return false;
			}
		}
	}
	else
	{
		CCodeVar ValVar;
		rVar = rList(0);

		for (i = 1; i < iCount; i++)
		{
			if (!rCB.OpSubtract(rVar, rList(i), ValVar, iLine, iPos))
			{
				return false;
			}

			rVar = ValVar;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Product FUNCTION

bool  ProdFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount == 1)
	{
		if (mVars(0).BaseType() == PDT_VARLIST)
		{
			return Prod(rCB, rVar, *mVars(0).GetVarListPtr(), iLine, iPos);
		}
		else if (mVars(0).BaseType() == PDT_MATRIX)
		{
			TMatrix& mA = *mVars(0).GetMatrixPtr();

			TCVScalar dProd;
			int iR, iC, iRows, iCols;
			iRows = mA.Rows();
			iCols = mA.Cols();

			if (!iRows || !iCols)
			{
				rCB.GetErrorList().GeneralError("Matrix passed is empty.", iLine, iPos);
				return false;
			}

			// Only take product of rows separately.
			rVar.New(PDT_MATRIX);
			TMatrix& rColMat = *rVar.GetMatrixPtr();
			rColMat.Resize(iRows, 1);

			for (iR = 1; iR <= iRows; iR++)
			{
				dProd = mA(iR, 1);
				for (iC = 2; iC <= iCols; iC++)
				{
					dProd *= mA(iR, iC);
				}

				rColMat(iR, 1) = dProd;
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
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Product of  List

bool  Prod(CCLUCodeBase& rCB, CCodeVar& rVar, TVarList& rList, int iLine, int iPos)
{
	MultiV<TCVScalar> MV;
	TMatrix Mat;

	int i, iCount = int(rList.Count());
	if (!iCount)
	{
		return true;
	}

	if (rList(0).BaseType() == PDT_VARLIST)
	{
		rVar.New(PDT_VARLIST);
		TVarList& rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			CCodeVar& rSubVar = rList(i);
			if (rSubVar.BaseType() == PDT_VARLIST)
			{
				TVarList& rSubList = *rSubVar.GetVarListPtr();

				if (!Prod(rCB, rRetList[i], rSubList, iLine, iPos))
				{
					return false;
				}
			}
			else
			{
				rCB.GetErrorList().GeneralError("Expect nested lists.", iLine, iPos);
				return false;
			}
		}
	}
	else
	{
		CCodeVar ValVar;
		rVar = rList(0);

		for (i = 1; i < iCount; i++)
		{
			if (!rCB.OpProd(rVar, rList(i), ValVar, iLine, iPos))
			{
				return false;
			}

			rVar = ValVar;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Minimum Value FUNCTION

bool  MinFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if ((iVarCount >= 1) && (mVars(0).BaseType() == PDT_IMAGE))
	{
		double dPart = 1.0;
		TImage& rImg = *mVars(0).GetImagePtr();

		if (!rImg.IsValid())
		{
			rCB.GetErrorList().GeneralError("Invalid image.", iLine, iPos);
			return false;
		}

		if (iVarCount >= 2)
		{
			if (!mVars(1).CastToScalar(dPart, rCB.GetSensitivity()))
			{
				rCB.GetErrorList().GeneralError("Expect second parameter to be scalar giving percentage of upper pixels for which minimum is to be found.",
						iLine,
						iPos);
				return false;
			}
		}

		int piX[4], piY[4];
		double pdCol[4];

		if (!rImg->GetPixelMinLum(piX, piY, pdCol, dPart))
		{
			rCB.GetErrorList().GeneralError("Error finding pixel with minimal luminance.", iLine, iPos);
			return false;
		}

		TOGLColor colMax;
		colMax.Set(float(pdCol[0]), float(pdCol[1]), float(pdCol[2]), float(pdCol[3]));

		rVar = colMax;
	}
	else if (iVarCount == 1)
	{
		if (mVars(0).BaseType() == PDT_VARLIST)
		{
			return Min(rCB, rVar, *mVars(0).GetVarListPtr(), iLine, iPos);
		}
		else if (mVars(0).BaseType() == PDT_MATRIX)
		{
			TMatrix& mA = *mVars(0).GetMatrixPtr();

			TCVScalar dMinVal;
			int iR, iC, iRows, iCols;
			iRows = mA.Rows();
			iCols = mA.Cols();

			if (!iRows || !iCols)
			{
				rCB.GetErrorList().GeneralError("Matrix passed is empty.", iLine, iPos);
				return false;
			}

			rVar.New(PDT_VARLIST);
			TVarList& rRowList = *rVar.GetVarListPtr();
			rRowList.Set(iRows);

			for (iR = 1; iR <= iRows; iR++)
			{
				dMinVal = mA(iR, 1);
				for (iC = 2; iC <= iCols; iC++)
				{
					if (mA(iR, iC) < dMinVal)
					{
						dMinVal = mA(iR, iC);
					}
				}

				rRowList[iR - 1] = dMinVal;
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
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Minimum Value in List

bool  Min(CCLUCodeBase& rCB, CCodeVar& rVar, TVarList& rList, int iLine, int iPos)
{
	MultiV<TCVScalar> MV;
	TMatrix Mat;

	int i, iCount = int(rList.Count());
	if (!iCount)
	{
		return true;
	}

	if (rList(0).BaseType() == PDT_VARLIST)
	{
		rVar.New(PDT_VARLIST);
		TVarList& rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			CCodeVar& rSubVar = rList(i);
			if (rSubVar.BaseType() == PDT_VARLIST)
			{
				TVarList& rSubList = *rSubVar.GetVarListPtr();

				if (!Min(rCB, rRetList[i], rSubList, iLine, iPos))
				{
					return false;
				}
			}
			else
			{
				rCB.GetErrorList().GeneralError("Expect nested lists.", iLine, iPos);
				return false;
			}
		}
	}
	else
	{
		TCVScalar dVal, dMinVal;
		if (!rList(0).CastToScalar(dMinVal, rCB.GetSensitivity()))
		{
			if (rList(0).BaseType() == PDT_STRING)
			{
				dMinVal = TCVScalar(rList(0).GetStringPtr()->Len());
			}
			else
			{
				CStrMem csText;
				csText << "Cannot cast element at position " << 1 << " in (sub-)list to scalar.";
				rCB.GetErrorList().GeneralError(csText.Str(), iLine, iPos);
				return false;
			}
		}

		for (i = 1; i < iCount; i++)
		{
			if (!rList(i).CastToScalar(dVal, rCB.GetSensitivity()))
			{
				if (rList(i).BaseType() == PDT_STRING)
				{
					dVal = TCVScalar(rList(i).GetStringPtr()->Len());
				}
				else
				{
					CStrMem csText;
					csText << "Cannot cast element at position " << (i + 1) << " in (sub-)list to scalar.";
					rCB.GetErrorList().GeneralError(csText.Str(), iLine, iPos);
					return false;
				}
			}

			if (dVal < dMinVal)
			{
				dMinVal = dVal;
			}
		}

		rVar = dMinVal;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Maximum Value FUNCTION

bool  MaxFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if ((iVarCount >= 1) && (mVars(0).BaseType() == PDT_IMAGE))
	{
		double dPart = 1.0;
		TImage& rImg = *mVars(0).GetImagePtr();

		if (!rImg.IsValid())
		{
			rCB.GetErrorList().GeneralError("Invalid image.", iLine, iPos);
			return false;
		}

		if (iVarCount >= 2)
		{
			if (!mVars(1).CastToScalar(dPart, rCB.GetSensitivity()))
			{
				rCB.GetErrorList().GeneralError("Expect second parameter to be scalar giving percentage of lower pixels for which maximum is to be found.",
						iLine,
						iPos);
				return false;
			}
		}

		int piX[4], piY[4];
		double pdCol[4];

		if (!rImg->GetPixelMaxLum(piX, piY, pdCol, dPart))
		{
			rCB.GetErrorList().GeneralError("Error finding pixel with maximal luminance.", iLine, iPos);
			return false;
		}

		TOGLColor colMax;
		colMax.Set(float(pdCol[0]), float(pdCol[1]), float(pdCol[2]), float(pdCol[3]));

		rVar = colMax;
	}
	else if (iVarCount == 1)
	{
		if (mVars(0).BaseType() == PDT_VARLIST)
		{
			return Max(rCB, rVar, *mVars(0).GetVarListPtr(), iLine, iPos);
		}
		else if (mVars(0).BaseType() == PDT_MATRIX)
		{
			TMatrix& mA = *mVars(0).GetMatrixPtr();

			TCVScalar dMaxVal;
			int iR, iC, iRows, iCols;
			iRows = mA.Rows();
			iCols = mA.Cols();

			if (!iRows || !iCols)
			{
				rCB.GetErrorList().GeneralError("Matrix passed is empty.", iLine, iPos);
				return false;
			}

			rVar.New(PDT_VARLIST);
			TVarList& rRowList = *rVar.GetVarListPtr();
			rRowList.Set(iRows);

			for (iR = 1; iR <= iRows; iR++)
			{
				dMaxVal = mA(iR, 1);
				for (iC = 2; iC <= iCols; iC++)
				{
					if (mA(iR, iC) > dMaxVal)
					{
						dMaxVal = mA(iR, iC);
					}
				}

				rRowList[iR - 1] = dMaxVal;
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
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Maximum Value in List

bool  Max(CCLUCodeBase& rCB, CCodeVar& rVar, TVarList& rList, int iLine, int iPos)
{
	MultiV<TCVScalar> MV;
	TMatrix Mat;

	int i, iCount = int(rList.Count());
	if (!iCount)
	{
		return true;
	}

	if (rList(0).BaseType() == PDT_VARLIST)
	{
		rVar.New(PDT_VARLIST);
		TVarList& rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			CCodeVar& rSubVar = rList(i);
			if (rSubVar.BaseType() == PDT_VARLIST)
			{
				TVarList& rSubList = *rSubVar.GetVarListPtr();

				if (!Max(rCB, rRetList[i], rSubList, iLine, iPos))
				{
					return false;
				}
			}
			else
			{
				rCB.GetErrorList().GeneralError("Expect nested lists.", iLine, iPos);
				return false;
			}
		}
	}
	else
	{
		TCVScalar dVal, dMaxVal;
		if (!rList(0).CastToScalar(dMaxVal, rCB.GetSensitivity()))
		{
			if (rList(0).BaseType() == PDT_STRING)
			{
				dMaxVal = TCVScalar(rList(0).GetStringPtr()->Len());
			}
			else
			{
				CStrMem csText;
				csText << "Cannot cast element at position " << 1 << " in (sub-)list to scalar.";
				rCB.GetErrorList().GeneralError(csText.Str(), iLine, iPos);
				return false;
			}
		}

		for (i = 1; i < iCount; i++)
		{
			if (!rList(i).CastToScalar(dVal, rCB.GetSensitivity()))
			{
				if (rList(i).BaseType() == PDT_STRING)
				{
					dVal = TCVScalar(rList(i).GetStringPtr()->Len());
				}
				else
				{
					CStrMem csText;
					csText << "Cannot cast element at position " << (i + 1) << " in (sub-)list to scalar.";
					rCB.GetErrorList().GeneralError(csText.Str(), iLine, iPos);
					return false;
				}
			}

			if (dVal > dMaxVal)
			{
				dMaxVal = dVal;
			}
		}

		rVar = dMaxVal;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Argument of Minimum Value FUNCTION

bool  ArgMinFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if ((iVarCount >= 1) && (mVars(0).BaseType() == PDT_IMAGE))
	{
		double dPart = 1.0;
		TImage& rImg = *mVars(0).GetImagePtr();

		if (!rImg.IsValid())
		{
			rCB.GetErrorList().GeneralError("Invalid image.", iLine, iPos);
			return false;
		}

		if (iVarCount >= 2)
		{
			if (!mVars(1).CastToScalar(dPart, rCB.GetSensitivity()))
			{
				rCB.GetErrorList().GeneralError("Expect second parameter to be scalar giving percentage of upper pixels for which minimum is to be found.",
						iLine,
						iPos);
				return false;
			}
		}

		int piX[4], piY[4];
		double pdCol[4];

		if (!rImg->GetPixelMinLum(piX, piY, pdCol, dPart))
		{
			rCB.GetErrorList().GeneralError("Error finding pixel with minimum luminance.", iLine, iPos);
			return false;
		}

		rVar.New(PDT_VARLIST);
		TVarList& rList = *rVar.GetVarListPtr();
		rList.Set(4);

		for (int i = 0; i < 4; ++i)
		{
			CCodeVar& rVarPos = rList(i);
			rVarPos.New(PDT_VARLIST);
			TVarList& rListPos = *rVarPos.GetVarListPtr();
			rListPos.Set(2);

			rListPos(0) = piX[i];
			rListPos(1) = piY[i];
		}
	}
	else if (iVarCount == 1)
	{
		if (mVars(0).BaseType() == PDT_VARLIST)
		{
			rVar.New(PDT_VARLIST);
			TVarList& rIdxList = *rVar.GetVarListPtr();

			return ArgMin(rCB, rIdxList, *mVars(0).GetVarListPtr(), iLine, iPos);
		}
		else if (mVars(0).BaseType() == PDT_MATRIX)
		{
			TMatrix& mA = *mVars(0).GetMatrixPtr();

			TCVScalar dMinVal;
			TCVCounter iMinIdx;
			int iR, iC, iRows, iCols;
			iRows = mA.Rows();
			iCols = mA.Cols();

			if (!iRows || !iCols)
			{
				rCB.GetErrorList().GeneralError("Matrix passed is empty.", iLine, iPos);
				return false;
			}

			rVar.New(PDT_VARLIST);
			TVarList& rRowList = *rVar.GetVarListPtr();
			rRowList.Set(iRows);

			for (iR = 1; iR <= iRows; iR++)
			{
				dMinVal = mA(iR, 1);
				iMinIdx = 1;
				for (iC = 2; iC <= iCols; iC++)
				{
					if (mA(iR, iC) < dMinVal)
					{
						dMinVal = mA(iR, iC);
						iMinIdx = TCVCounter(iC);
					}
				}

				rRowList[iR - 1].New(PDT_VARLIST);
				TVarList& rMatIdx = *rRowList[iR - 1].GetVarListPtr();
				rMatIdx.Set(2);
				rMatIdx[0] = iR;
				rMatIdx[1] = iMinIdx;
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
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Argument of Minimum Value in List

bool  ArgMin(CCLUCodeBase& rCB, TVarList& rIdxList, TVarList& rList, int iLine, int iPos)
{
	MultiV<TCVScalar> MV;
	TMatrix Mat;

	rIdxList.Set(0);

	int i, iCount = int(rList.Count());
	if (!iCount)
	{
		return true;
	}

	if (rList(0).BaseType() == PDT_VARLIST)
	{
		TVarList SubIdxList;

		for (i = 0; i < iCount; i++)
		{
			CCodeVar& rSubVar = rList(i);
			if (rSubVar.BaseType() == PDT_VARLIST)
			{
				TVarList& rSubList = *rSubVar.GetVarListPtr();

				if (!ArgMin(rCB, SubIdxList, rSubList, iLine, iPos))
				{
					return false;
				}

				int j, iSubCount = int(SubIdxList.Count());
				for (j = 0; j < iSubCount; j++)
				{
					rIdxList.Add(1);
					rIdxList.Last() = SubIdxList(j);
					TVarList& rIdx = *rIdxList.Last().GetVarListPtr();
					rIdx.Insert(0);
					rIdx(0) = i + 1;
				}
			}
			else
			{
				rCB.GetErrorList().GeneralError("Expect nested lists.", iLine, iPos);
				return false;
			}
		}
	}
	else
	{
		TCVScalar dVal, dMinVal;
		int iMinIdx = 0;

		if (!rList(0).CastToScalar(dMinVal, rCB.GetSensitivity()))
		{
			if (rList(0).BaseType() == PDT_STRING)
			{
				dMinVal = TCVScalar(rList(0).GetStringPtr()->Len());
			}
			else
			{
				CStrMem csText;
				csText << "Cannot cast element at position " << 1 << " in (sub-)list to scalar.";
				rCB.GetErrorList().GeneralError(csText.Str(), iLine, iPos);
				return false;
			}
		}

		for (i = 1; i < iCount; i++)
		{
			if (!rList(i).CastToScalar(dVal, rCB.GetSensitivity()))
			{
				if (rList(i).BaseType() == PDT_STRING)
				{
					dVal = TCVScalar(rList(i).GetStringPtr()->Len());
				}
				else
				{
					CStrMem csText;
					csText << "Cannot cast element at position " << (i + 1) << " in (sub-)list to scalar.";
					rCB.GetErrorList().GeneralError(csText.Str(), iLine, iPos);
					return false;
				}
			}

			if (dVal < dMinVal)
			{
				dMinVal = dVal;
				iMinIdx = i;
			}
		}

		rIdxList.Add(1);
		rIdxList.Last().New(PDT_VARLIST);
		TVarList& rIdx = *rIdxList.Last().GetVarListPtr();
		rIdx.Add(1);
		rIdx[0] = TCVCounter(iMinIdx + 1);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Argument of Maximum Value FUNCTION

bool  ArgMaxFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if ((iVarCount >= 1) && (mVars(0).BaseType() == PDT_IMAGE))
	{
		double dPart = 1.0;
		TImage& rImg = *mVars(0).GetImagePtr();

		if (!rImg.IsValid())
		{
			rCB.GetErrorList().GeneralError("Invalid image.", iLine, iPos);
			return false;
		}

		if (iVarCount >= 2)
		{
			if (!mVars(1).CastToScalar(dPart, rCB.GetSensitivity()))
			{
				rCB.GetErrorList().GeneralError("Expect second parameter to be scalar giving percentage of lower pixels for which maximum is to be found.",
						iLine,
						iPos);
				return false;
			}
		}

		int piX[4], piY[4];
		double pdCol[4];

		if (!rImg->GetPixelMaxLum(piX, piY, pdCol, dPart))
		{
			rCB.GetErrorList().GeneralError("Error finding pixel with maximal luminance.", iLine, iPos);
			return false;
		}

		rVar.New(PDT_VARLIST);
		TVarList& rList = *rVar.GetVarListPtr();
		rList.Set(4);

		for (int i = 0; i < 4; ++i)
		{
			CCodeVar& rVarPos = rList(i);
			rVarPos.New(PDT_VARLIST);
			TVarList& rListPos = *rVarPos.GetVarListPtr();
			rListPos.Set(2);

			rListPos(0) = piX[i];
			rListPos(1) = piY[i];
		}
	}
	else if (iVarCount == 1)
	{
		if (mVars(0).BaseType() == PDT_VARLIST)
		{
			rVar.New(PDT_VARLIST);
			TVarList& rIdxList = *rVar.GetVarListPtr();

			return ArgMax(rCB, rIdxList, *mVars(0).GetVarListPtr(), iLine, iPos);
		}
		else if (mVars(0).BaseType() == PDT_MATRIX)
		{
			TMatrix& mA = *mVars(0).GetMatrixPtr();

			TCVScalar dMaxVal;
			TCVCounter iMaxIdx;
			int iR, iC, iRows, iCols;
			iRows = mA.Rows();
			iCols = mA.Cols();

			if (!iRows || !iCols)
			{
				rCB.GetErrorList().GeneralError("Matrix passed is empty.", iLine, iPos);
				return false;
			}

			rVar.New(PDT_VARLIST);
			TVarList& rRowList = *rVar.GetVarListPtr();
			rRowList.Set(iRows);

			for (iR = 1; iR <= iRows; iR++)
			{
				dMaxVal = mA(iR, 1);
				iMaxIdx = 1;
				for (iC = 2; iC <= iCols; iC++)
				{
					if (mA(iR, iC) > dMaxVal)
					{
						dMaxVal = mA(iR, iC);
						iMaxIdx = TCVCounter(iC);
					}
				}

				rRowList[iR - 1].New(PDT_VARLIST);
				TVarList& rMatIdx = *rRowList[iR - 1].GetVarListPtr();
				rMatIdx.Set(2);
				rMatIdx[0] = iR;
				rMatIdx[1] = iMaxIdx;
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
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Argument of Maximum Value in List

bool  ArgMax(CCLUCodeBase& rCB, TVarList& rIdxList, TVarList& rList, int iLine, int iPos)
{
	MultiV<TCVScalar> MV;
	TMatrix Mat;

	rIdxList.Set(0);

	int i, iCount = int(rList.Count());
	if (!iCount)
	{
		return true;
	}

	if (rList(0).BaseType() == PDT_VARLIST)
	{
		TVarList SubIdxList;

		for (i = 0; i < iCount; i++)
		{
			CCodeVar& rSubVar = rList(i);
			if (rSubVar.BaseType() == PDT_VARLIST)
			{
				TVarList& rSubList = *rSubVar.GetVarListPtr();

				if (!ArgMax(rCB, SubIdxList, rSubList, iLine, iPos))
				{
					return false;
				}

				int j, iSubCount = int(SubIdxList.Count());
				for (j = 0; j < iSubCount; j++)
				{
					rIdxList.Add(1);
					rIdxList.Last() = SubIdxList(j);
					TVarList& rIdx = *rIdxList.Last().GetVarListPtr();
					rIdx.Insert(0);
					rIdx(0) = i + 1;
				}
			}
			else
			{
				rCB.GetErrorList().GeneralError("Expect nested lists.", iLine, iPos);
				return false;
			}
		}
	}
	else
	{
		TCVScalar dVal, dMaxVal;
		int iMaxIdx = 0;

		if (!rList(0).CastToScalar(dMaxVal, rCB.GetSensitivity()))
		{
			if (rList(0).BaseType() == PDT_STRING)
			{
				dMaxVal = TCVScalar(rList(0).GetStringPtr()->Len());
			}
			else
			{
				CStrMem csText;
				csText << "Cannot cast element at position " << 1 << " in (sub-)list to scalar.";
				rCB.GetErrorList().GeneralError(csText.Str(), iLine, iPos);
				return false;
			}
		}

		for (i = 1; i < iCount; i++)
		{
			if (!rList(i).CastToScalar(dVal, rCB.GetSensitivity()))
			{
				if (rList(i).BaseType() == PDT_STRING)
				{
					dVal = TCVScalar(rList(i).GetStringPtr()->Len());
				}
				else
				{
					CStrMem csText;
					csText << "Cannot cast element at position " << (i + 1) << " in (sub-)list to scalar.";
					rCB.GetErrorList().GeneralError(csText.Str(), iLine, iPos);
					return false;
				}
			}

			if (dVal > dMaxVal)
			{
				dMaxVal = dVal;
				iMaxIdx = i;
			}
		}

		rIdxList.Add(1);
		rIdxList.Last().New(PDT_VARLIST);
		TVarList& rIdx = *rIdxList.Last().GetVarListPtr();
		rIdx.Add(1);
		rIdx[0] = TCVCounter(iMaxIdx + 1);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Inverse FUNCTION

bool  InvFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount != 1)
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() == PDT_MATRIX)
	{
		rVar = *mVars(0).GetMatrixPtr();
		TMatrix& rMat = *rVar.GetMatrixPtr();

		rMat.InvComps(0, rCB.GetSensitivity());
	}
	else if (mVars(0).BaseType() == PDT_TENSOR_IDX)
	{
		try
		{
			TTensorIdx& rTIdxData = *mVars(0).GetTensorIdxPtr();

			// Create a temporary variable that contains tensor.
			// Do this, so that we can return a tensor index.
			CCodeVar& rTemp = rCB.NewTempVar(PDT_TENSOR);
			TTensor& rT     = *rTemp.GetTensorPtr();

			rVar.New(PDT_TENSOR_IDX);
			TTensorIdx& rTIdx = *rVar.GetTensorIdxPtr();

			if (!TensorIdxInvertComps(rT, rTIdx, rTIdxData, rCB.GetSensitivity()))
			{
				rCB.GetErrorList().GeneralError("Error inverting tensor components.", iLine, iPos);
				return false;
			}
		}
		catch (CCluOutOfMemory&)
		{
			rCB.GetErrorList().OutOfMemory(iLine, iPos);
			return false;
		}
		catch (CCluException& xEx)
		{
			rCB.GetErrorList().GeneralError(xEx.PrintError().c_str(), iLine, iPos);
			return false;
		}
	}
	else if (mVars(0).BaseType() == PDT_TENSOR)
	{
		TTensor& rTData = *mVars(0).GetTensorPtr();

		rVar.New(PDT_TENSOR);
		TTensor& rT = *rVar.GetTensorPtr();

		if (!TensorInvertComps(rT, rTData))
		{
			rCB.GetErrorList().GeneralError("Error inverting tensor components.", iLine, iPos);
			return false;
		}
	}
	else
	{
		rCB.GetErrorList().GeneralError("Inv() function can only be applied to matrices and tensors, where it inverts the components separately. "
						"Use the operator '!' for a general inverse.", iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Square Root FUNCTION

bool  SqrtFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount == 1)
	{
		if (!Sqrt(rCB, rVar, mVars(0), iLine, iPos))
		{
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
/// Square Root Value

bool  Sqrt(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rData, int iLine, int iPos)
{
	TCVScalar fVal;

	if (rData.CastToScalar(fVal, rCB.GetSensitivity()))
	{
		if ((fVal < 0.0) && (fVal >= -rCB.GetSensitivity()))
		{
			rVar = TCVScalar(0);
		}
		else if (fVal >= 0.0)
		{
			rVar = (TCVScalar) sqrt((double) fVal);
		}	// check NaN
		else if (fVal != fVal)
		{
			rVar = (TCVScalar) nan("");
		}
		else
		{
			rCB.GetErrorList().GeneralError("Parameter has to be greater or equal to zero.", iLine, iPos);
			return false;
		}
	}
	else if (rData.BaseType() == PDT_MATRIX)
	{
		rVar = *rData.GetMatrixPtr();
		TMatrix& rMat = *rVar.GetMatrixPtr();

		if (!rMat.SqrtComps(rCB.GetSensitivity()))
		{
			rCB.GetErrorList().MatrixIsNAN(iLine, iPos);
			return false;
		}
	}
	else if (rData.BaseType() == PDT_VARLIST)
	{
		// Loop over all elements of list and call this function recursively.
		TVarList& List = *rData.GetVarListPtr();
		int i, iCount = int(List.Count());

		rVar.New(PDT_VARLIST);
		TVarList& ResultList = *rVar.GetVarListPtr();

		ResultList.Set(iCount);
		for (i = 0; i < iCount; i++)
		{
			if (!Sqrt(rCB, ResultList[i], List[i], iLine, iPos))
			{
				return false;
			}
		}
	}
	else
	{
		rCB.GetErrorList().InvalidType(rData, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Power FUNCTION

bool  PowFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount == 2)
	{
		if (!rCB.Pow(rVar, mVars(0), mVars(1), iLine, iPos))
		{
			return false;
		}
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(2, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Sin FUNCTION
bool  SinFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount == 1)
	{
		return Sin(rCB, rVar, mVars(0), iLine, iPos);
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}
}

//////////////////////////////////////////////////////////////////////
/// Sine
bool  Sin(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rData, int iLine, int iPos)
{
	TCVScalar dVal;

	if (rData.BaseType() == PDT_VARLIST)
	{
		TVarList& rList = *rData.GetVarListPtr();
		int i, iCount = int(rList.Count());

		rVar.New(PDT_VARLIST);
		TVarList& rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!Sin(rCB, rRetList[i], rList(i), iLine, iPos))
			{
				return false;
			}
		}
	}
	else if (rData.BaseType() == PDT_MATRIX)
	{
		rVar = *rData.GetMatrixPtr();
		TMatrix& rMat = *rVar.GetMatrixPtr();

		// Take sin of matrix components separately
		rMat.SinComps();
	}
	else
	{
		if (!rData.CastToScalar(dVal, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(rData, 1, iLine, iPos);
			return false;
		}

		rVar = (TCVScalar) sin(dVal);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Cos FUNCTION
bool  CosFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount == 1)
	{
		return Cos(rCB, rVar, mVars(0), iLine, iPos);
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}
}

//////////////////////////////////////////////////////////////////////
/// Cosine
bool  Cos(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rData, int iLine, int iPos)
{
	TCVScalar dVal;

	if (rData.BaseType() == PDT_VARLIST)
	{
		TVarList& rList = *rData.GetVarListPtr();
		int i, iCount = int(rList.Count());

		rVar.New(PDT_VARLIST);
		TVarList& rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!Cos(rCB, rRetList[i], rList(i), iLine, iPos))
			{
				return false;
			}
		}
	}
	else if (rData.BaseType() == PDT_MATRIX)
	{
		rVar = *rData.GetMatrixPtr();
		TMatrix& rMat = *rVar.GetMatrixPtr();

		// Take sin of matrix components separately
		rMat.CosComps();
	}
	else
	{
		if (!rData.CastToScalar(dVal, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(rData, 1, iLine, iPos);
			return false;
		}

		rVar = (TCVScalar) cos(dVal);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Tan FUNCTION
bool  TanFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount == 1)
	{
		return Tan(rCB, rVar, mVars(0), iLine, iPos);
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}
}

//////////////////////////////////////////////////////////////////////
/// Tan
bool  Tan(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rData, int iLine, int iPos)
{
	TCVScalar dVal;

	if (rData.BaseType() == PDT_VARLIST)
	{
		TVarList& rList = *rData.GetVarListPtr();
		int i, iCount = int(rList.Count());

		rVar.New(PDT_VARLIST);
		TVarList& rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!Tan(rCB, rRetList[i], rList(i), iLine, iPos))
			{
				return false;
			}
		}
	}
	else if (rData.BaseType() == PDT_MATRIX)
	{
		rVar = *rData.GetMatrixPtr();
		TMatrix& rMat = *rVar.GetMatrixPtr();

		// Take sin of matrix components separately
		rMat.TanComps();
	}
	else
	{
		if (!rData.CastToScalar(dVal, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(rData, 1, iLine, iPos);
			return false;
		}

		rVar = (TCVScalar) tan(dVal);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// ASin FUNCTION
bool  AsinFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount == 1)
	{
		return Asin(rCB, rVar, mVars(0), iLine, iPos);
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}
}

//////////////////////////////////////////////////////////////////////
/// Asin
bool  Asin(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rData, int iLine, int iPos)
{
	TCVScalar dVal;

	if (rData.BaseType() == PDT_VARLIST)
	{
		TVarList& rList = *rData.GetVarListPtr();
		int i, iCount = int(rList.Count());

		rVar.New(PDT_VARLIST);
		TVarList& rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!Asin(rCB, rRetList[i], rList(i), iLine, iPos))
			{
				return false;
			}
		}
	}
	else if (rData.BaseType() == PDT_MATRIX)
	{
		rVar = *rData.GetMatrixPtr();
		TMatrix& rMat = *rVar.GetMatrixPtr();

		// Take sin of matrix components separately
		if (!rMat.ArcSinComps(rCB.GetSensitivity()))
		{
			rCB.GetErrorList().MatrixIsNAN(iLine, iPos);
			return false;
		}
	}
	else
	{
		if (!rData.CastToScalar(dVal, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(rData, 1, iLine, iPos);
			return false;
		}

		if (!rCB.IsInRange(dVal, -1, 1, iLine, iPos))
		{
			return false;
		}

		rVar = (TCVScalar) asin(dVal);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Acos FUNCTION
bool  AcosFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount == 1)
	{
		return Acos(rCB, rVar, mVars(0), iLine, iPos);
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}
}

//////////////////////////////////////////////////////////////////////
/// Cosine
bool  Acos(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rData, int iLine, int iPos)
{
	TCVScalar dVal;

	if (rData.BaseType() == PDT_VARLIST)
	{
		TVarList& rList = *rData.GetVarListPtr();
		int i, iCount = int(rList.Count());

		rVar.New(PDT_VARLIST);
		TVarList& rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!Acos(rCB, rRetList[i], rList(i), iLine, iPos))
			{
				return false;
			}
		}
	}
	else if (rData.BaseType() == PDT_MATRIX)
	{
		rVar = *rData.GetMatrixPtr();
		TMatrix& rMat = *rVar.GetMatrixPtr();

		// Take sin of matrix components separately
		if (!rMat.ArcCosComps(rCB.GetSensitivity()))
		{
			rCB.GetErrorList().MatrixIsNAN(iLine, iPos);
			return false;
		}
	}
	else
	{
		if (!rData.CastToScalar(dVal, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(rData, 1, iLine, iPos);
			return false;
		}

		if (!rCB.IsInRange(dVal, -1, 1, iLine, iPos))
		{
			return false;
		}

		rVar = (TCVScalar) acos(dVal);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Atan FUNCTION
bool  AtanFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount == 1)
	{
		return Atan(rCB, rVar, mVars(0), iLine, iPos);
	}
	else if (iVarCount == 2)
	{
		return Atan(rCB, rVar, mVars(0), mVars(1), iLine, iPos);
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}
}

//////////////////////////////////////////////////////////////////////
/// Arcus Tangent

bool  Atan(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rData, int iLine, int iPos)
{
	TCVScalar dVal;

	if (rData.BaseType() == PDT_VARLIST)
	{
		TVarList& rList = *rData.GetVarListPtr();
		int i, iCount = int(rList.Count());

		rVar.New(PDT_VARLIST);
		TVarList& rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!Atan(rCB, rRetList[i], rList(i), iLine, iPos))
			{
				return false;
			}
		}
	}
	else if (rData.BaseType() == PDT_MATRIX)
	{
		rVar = *rData.GetMatrixPtr();
		TMatrix& rMat = *rVar.GetMatrixPtr();

		// Take sin of matrix components separately
		rMat.ArcTanComps();
	}
	else
	{
		if (!rData.CastToScalar(dVal, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(rData, 1, iLine, iPos);
			return false;
		}

		rVar = (TCVScalar) atan(dVal);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Arcus Tangent Complete

bool  Atan(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rLData, CCodeVar& rRData, int iLine, int iPos)
{
	TCVScalar dVal, dLVal, dRVal;

	if ((rLData.BaseType() == PDT_VARLIST) &&
	    (rRData.BaseType() != PDT_VARLIST))
	{
		TVarList& rList = *rLData.GetVarListPtr();
		int i, iCount = int(rList.Count());

		rVar.New(PDT_VARLIST);
		TVarList& rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!Atan(rCB, rRetList[i], rList(i), rRData, iLine, iPos))
			{
				return false;
			}
		}
	}
	else if ((rLData.BaseType() != PDT_VARLIST) &&
		 (rRData.BaseType() == PDT_VARLIST))
	{
		TVarList& rList = *rRData.GetVarListPtr();
		int i, iCount = int(rList.Count());

		rVar.New(PDT_VARLIST);
		TVarList& rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!Atan(rCB, rRetList[i], rLData, rList(i), iLine, iPos))
			{
				return false;
			}
		}
	}
	else if ((rLData.BaseType() == PDT_VARLIST) &&
		 (rRData.BaseType() == PDT_VARLIST))
	{
		TVarList& rLList = *rLData.GetVarListPtr();
		TVarList& rRList = *rRData.GetVarListPtr();
		int i, iCount = int(rLList.Count());

		if (iCount != rRList.Count())
		{
			rCB.GetErrorList().GeneralError("Lists are not of same length.", iLine, iPos);
			return false;
		}

		rVar.New(PDT_VARLIST);
		TVarList& rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!Atan(rCB, rRetList[i], rLList(i), rRList(i), iLine, iPos))
			{
				return false;
			}
		}
	}
/*
        else if (rData.BaseType() == PDT_MATRIX)
        {
                rVar = *rData.GetMatrixPtr();
                TMatrix &rMat = *rVar.GetMatrixPtr();

                // Take sin of matrix components separately
                rMat.ArcTanComps();
        }
*/
	else
	{
		if (!rLData.CastToScalar(dLVal, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Exepcted scalar value as first parameter.", iLine, iPos);
			return false;
		}

		if (!rRData.CastToScalar(dRVal, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Exepcted scalar value as second parameter.", iLine, iPos);
			return false;
		}

		if (dRVal > 0)
		{
			if (dLVal >= 0)
			{
				dVal = TCVScalar(atan(dLVal / dRVal));
			}
			else
			{
				dVal = 2 * rCB.GetPi() + TCVScalar(atan(dLVal / dRVal));
			}
		}
		else if (dRVal < 0)
		{
			dVal = rCB.GetPi() + TCVScalar(atan(dLVal / dRVal));
		}
		else if (::IsZero(dRVal, rCB.GetSensitivity()) &&
			 !::IsZero(dLVal, rCB.GetSensitivity()))
		{
			if (dLVal > 0)
			{
				dVal = rCB.GetPi() / 2.0;
			}
			else
			{
				dVal = 1.5 * rCB.GetPi();
			}
		}
		else
		{
			//rCB.GetErrorList().GeneralError("Undefined result.", iLine, iPos);
			//return false;
			dVal = TCVScalar(0);
		}

		rVar = dVal;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// SinHyp FUNCTION
bool  SinHypFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount == 1)
	{
		return SinHyp(rCB, rVar, mVars(0), iLine, iPos);
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}
}

//////////////////////////////////////////////////////////////////////
/// Sine
bool  SinHyp(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rData, int iLine, int iPos)
{
	TCVScalar dVal;

	if (rData.BaseType() == PDT_VARLIST)
	{
		TVarList& rList = *rData.GetVarListPtr();
		int i, iCount = int(rList.Count());

		rVar.New(PDT_VARLIST);
		TVarList& rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!SinHyp(rCB, rRetList[i], rList(i), iLine, iPos))
			{
				return false;
			}
		}
	}
	//else if (rData.BaseType() == PDT_MATRIX)
	//{
	//	rVar = *rData.GetMatrixPtr();
	//	TMatrix &rMat = *rVar.GetMatrixPtr();

	//	// Take sin of matrix components separately
	//	rMat.SinComps();
	//}
	else
	{
		if (!rData.CastToScalar(dVal, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(rData, 1, iLine, iPos);
			return false;
		}

		rVar = (TCVScalar) sinh(dVal);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Cos FUNCTION
bool  CosHypFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount == 1)
	{
		return CosHyp(rCB, rVar, mVars(0), iLine, iPos);
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}
}

//////////////////////////////////////////////////////////////////////
/// Cosine
bool  CosHyp(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rData, int iLine, int iPos)
{
	TCVScalar dVal;

	if (rData.BaseType() == PDT_VARLIST)
	{
		TVarList& rList = *rData.GetVarListPtr();
		int i, iCount = int(rList.Count());

		rVar.New(PDT_VARLIST);
		TVarList& rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!CosHyp(rCB, rRetList[i], rList(i), iLine, iPos))
			{
				return false;
			}
		}
	}
	//else if (rData.BaseType() == PDT_MATRIX)
	//{
	//	rVar = *rData.GetMatrixPtr();
	//	TMatrix &rMat = *rVar.GetMatrixPtr();

	//	// Take sin of matrix components separately
	//	rMat.CosComps();
	//}
	else
	{
		if (!rData.CastToScalar(dVal, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(rData, 1, iLine, iPos);
			return false;
		}

		rVar = (TCVScalar) cosh(dVal);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Tan FUNCTION
bool  TanHypFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount == 1)
	{
		return TanHyp(rCB, rVar, mVars(0), iLine, iPos);
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}
}

//////////////////////////////////////////////////////////////////////
/// Tan
bool  TanHyp(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rData, int iLine, int iPos)
{
	TCVScalar dVal;

	if (rData.BaseType() == PDT_VARLIST)
	{
		TVarList& rList = *rData.GetVarListPtr();
		int i, iCount = int(rList.Count());

		rVar.New(PDT_VARLIST);
		TVarList& rRetList = *rVar.GetVarListPtr();
		rRetList.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (!TanHyp(rCB, rRetList[i], rList(i), iLine, iPos))
			{
				return false;
			}
		}
	}
	//else if (rData.BaseType() == PDT_MATRIX)
	//{
	//	rVar = *rData.GetMatrixPtr();
	//	TMatrix &rMat = *rVar.GetMatrixPtr();

	//	// Take sin of matrix components separately
	//	rMat.TanComps();
	//}
	else
	{
		if (!rData.CastToScalar(dVal, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(rData, 1, iLine, iPos);
			return false;
		}

		rVar = (TCVScalar) tanh(dVal);
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 	returns an NaN value
/// </summary>
///
/// <param name="rCB">   [in,out] The cb. </param>
/// <param name="rVar">  [in,out] The variable. </param>
/// <param name="rPars"> [in,out] The pars. </param>
/// <param name="iLine"> Zero-based index of the line. </param>
/// <param name="iPos">  Zero-based index of the position. </param>
///
/// <returns> True if it succeeds, false if it fails. </returns>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool NaNFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	// We expect no parameter
	if (iVarCount != 0)
	{
		rCB.GetErrorList().WrongNoOfParams(0, iLine, iPos);
		return false;
	}

	// Std lib nan function
	rVar = TCVScalar(std::nan(""));

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 	Query if this floating point is NaN value.
/// </summary>
///
/// <param name="rCB">   [in,out] The cb. </param>
/// <param name="rVar">  [in,out] The variable. </param>
/// <param name="rPars"> [in,out] The pars. </param>
/// <param name="iLine"> Zero-based index of the line. </param>
/// <param name="iPos">  Zero-based index of the position. </param>
///
/// <returns> True if NaN function, false if not. </returns>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool IsNaNFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	TCVScalar fValue;

	// we expect one parameter, the value to test
	if (iVarCount == 1)
	{
		if (mVars(0).BaseType() == PDT_SCALAR)
		{
			fValue = *(mVars(0).GetScalarPtr());
		}
		else if (mVars(0).BaseType() == PDT_FLOAT)
		{
			fValue = *(mVars(0).GetFloatPtr());
		}
		else if (mVars(0).BaseType() == PDT_INT)
		{
			int iVal = *(mVars(0).GetIntPtr());
			fValue = (TCVScalar) iVal;
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

	// NaN check by c++ std lib
	if (isnan(fValue))
	{
		rVar = TCVCounter(1);
	}
	else
	{
		// return false on NOT-NAN
		rVar = TCVCounter(0);
	}

	return true;
}
