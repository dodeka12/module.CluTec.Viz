////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_P3_Define.cpp
//
// summary:   Implements the function p 3 define class
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
#include "Func_P3_Define.h"

//////////////////////////////////////////////////////////////////////
/// Normalize homogeneous vectors

bool  AffineVecP3Func(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!rCB.GetFilter())
	{
		rCB.GetErrorList().Internal(iLine, iPos);
		return false;
	}

	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	// Recurse on list
	if ((iVarCount == 1) && (mVars(0).BaseType() == PDT_VARLIST))
	{
		return AffineVecP3Func(rCB, rVar, mVars(0), iLine, iPos);
	}

	if (!rVar.New(PDT_VARLIST, "Constant"))
	{
		rCB.GetErrorList().OutOfMemory(iLine, iPos);
		return false;
	}

	TVarList& VarList = *rVar.GetVarListPtr();

	if (!VarList.Set(iVarCount))
	{
		rCB.GetErrorList().OutOfMemory(iLine, iPos);
		return false;
	}

	TMultiV vX;
	TCVScalar dH;
	int i;

	for (i = 0; i < iVarCount; i++)
	{
		if (mVars(i).BaseType() != PDT_MULTIV)
		{
			rCB.GetErrorList().InvalidType(mVars(i), iLine, iPos);
			return false;
		}

		vX = *mVars(i).GetMultiVPtr();
		dH = vX[4];

		if (dH != 0.0)
		{
			vX /= dH;
		}

		VarList[i] = vX;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Define Variables specific to P3

bool  DefVarsP3(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList* pmVars = rPars.GetVarListPtr();

	int iVarCount = int(pmVars->Count());

	if (iVarCount != 0)
	{
		rCB.GetErrorList().WrongNoOfParams(0, iLine, iPos);
		return false;
	}

	if (rCB.GetCurSpaceVars() == CCLUCodeBase::SPACEVARS_P3)
	{
		return true;
	}

	rCB.ResetSpaceVars();

	(rCB.NewConstVar("id", PDT_MULTIV) = m_pP3GABase->vSC).EnableProtect();
	(rCB.NewConstVar("e1", PDT_MULTIV) = m_pP3GABase->vE1).EnableProtect();
	(rCB.NewConstVar("e2", PDT_MULTIV) = m_pP3GABase->vE2).EnableProtect();
	(rCB.NewConstVar("e3", PDT_MULTIV) = m_pP3GABase->vE3).EnableProtect();
	(rCB.NewConstVar("e4", PDT_MULTIV) = m_pP3GABase->vE4).EnableProtect();
	(rCB.NewConstVar("I", PDT_MULTIV) = m_pP3GABase->vI).EnableProtect();

	rCB.GetCurSpaceVars() = CCLUCodeBase::SPACEVARS_P3;

	return true;
}

//////////////////////////////////////////////////////////////////////
// Create P3 Vector

bool  VecP3Func(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList* pmVars = rPars.GetVarListPtr();

	int iVarCount = int(pmVars->Count());

	if ((iVarCount >= 2) && (iVarCount <= 4))
	{
		TCVScalar pfVec[4];
		int i;

		for (i = 0; i < iVarCount; i++)
		{
			if (!((*pmVars)(i).CastToScalar(pfVec[i], rCB.GetSensitivity())))
			{
				rCB.GetErrorList().InvalidType((*pmVars)(i), iLine, iPos);
				return false;
			}
		}

		if (iVarCount < 3)
		{
			pfVec[2] = TCVScalar(0);
		}

		if (iVarCount < 4)
		{
			pfVec[3] = TCVScalar(1);
		}

		if (!VecP3(rCB, rVar, pfVec, iLine, iPos))
		{
			return false;	// VecE3 sets error
		}
	}
	else if (iVarCount == 1)
	{
		TCVCounter iVal;

		// If Parameter is a list of values
		// call VecE3 with this list of values.
		if ((*pmVars)(0).BaseType() == PDT_VARLIST)
		{
			int i, iCount;
			TVarList& rParList = *((*pmVars)(0).GetVarListPtr());

			iCount = int(rParList.Count());
			if ((rParList(0).BaseType() == PDT_SCALAR) ||
			    (rParList(0).BaseType() == PDT_COUNTER))
			{
				return VecP3Func(rCB, rVar, (*pmVars)(0), iLine, iPos);
			}
			else
			{
				rVar.New(PDT_VARLIST);
				TVarList& rList = *rVar.GetVarListPtr();
				rList.Set(iCount);

				for (i = 0; i < iCount; i++)
				{
					if (rParList(i).BaseType() == PDT_VARLIST)
					{
						if (!VecP3Func(rCB, rList[i], rParList(i), iLine, iPos))
						{
							return false;
						}
					}
					else
					{
						if (rParList(i).BaseType() == PDT_MULTIV)
						{
							if (!VecP3(rCB, rList[i], *((TMultiV*) rParList(i).GetMultiVPtr()), iLine, iPos))
							{
								return false;
							}
						}
						else
						{
							rCB.GetErrorList().InvalidType((*pmVars)(0), iLine, iPos);
							return false;
						}
					}
				}
			}

			return true;
		}

		if ((*pmVars)(0).CastToCounter(iVal))
		{
			if (!VecP3(rCB, rVar, iVal, iLine, iPos))
			{
				return false;	// VecE3 sets error
			}
		}
		else if ((*pmVars)(0).BaseType() == PDT_MULTIV)
		{
			if (!VecP3(rCB, rVar, *((TMultiV*) (*pmVars)(0).GetMultiVPtr()), iLine, iPos))
			{
				return false;
			}
		}
		else if ((*pmVars)(0).BaseType() == PDT_COLOR)
		{
			TOGLColor& rCol = *((*pmVars)(0).GetOGLColorPtr());
			rVar = m_pP3GABase->hVec(TCVScalar(rCol[0]), TCVScalar(rCol[1]), TCVScalar(rCol[2]), TCVScalar(rCol[3]));
		}
		else
		{
			rCB.GetErrorList().InvalidType((*pmVars)(0), iLine, iPos);
			return false;
		}
	}
	else
	{
		int piVal[] = { 1, 2, 3, 4 };
		rCB.GetErrorList().WrongNoOfParams(piVal, 2, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Create P3 Vector

bool  VecP3(CCLUCodeBase& rCB, CCodeVar& rLVar, TCVScalar* pfVec, int iLine, int iPos)
{
	if (!pfVec)
	{
		return false;
	}

	if (!rLVar.New(PDT_MULTIV, "Constant"))
	{
		rCB.GetErrorList().Internal(iLine, iPos);
		return false;
	}

	rLVar = m_pP3GABase->hVec(pfVec);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Create P3 Vector from transform variable

bool  VecP3(CCLUCodeBase& rCB, CCodeVar& rLVar, int iVal, int iLine, int iPos)
{
	if (!(&rCB.GetTransform()))
	{
		return false;
	}

	if (!rLVar.New(PDT_MULTIV, "Constant"))
	{
		rCB.GetErrorList().Internal(iLine, iPos);
		return false;
	}

	if ((iVal < 0) || (iVal > 9))
	{
		CStrMem csText;
		csText = iVal;
		rCB.GetErrorList().InvalidParVal(csText.Str(), 1, iLine, iPos);
		return false;
	}

	float* pfTrans = rCB.GetTransform()[iVal].pfTrans;
	TCVScalar pfVec[4];

	if (rCB.GetCLUDrawBase()->Is2dView())
	{
		pfVec[0] = (TCVScalar) pfTrans[0];
		pfVec[1] = (TCVScalar) pfTrans[1];
		pfVec[2] = (TCVScalar) 0.0;
		pfVec[3] = (TCVScalar) 1.0;
	}
	else
	{
		pfVec[0] = (TCVScalar) pfTrans[0];
		pfVec[1] = (TCVScalar) pfTrans[1];
		pfVec[2] = (TCVScalar) pfTrans[2];
		pfVec[3] = (TCVScalar) 1.0;
	}

	rLVar = m_pP3GABase->hVec(pfVec);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Convert N3,E3 vector into P3 Vector

bool  VecP3(CCLUCodeBase& rCB, CCodeVar& rLVar, TMultiV& vA, int iLine, int iPos)
{
	TMultiV vR;

	if (!rCB.CastMVtoP3(vA, vR))
	{
		rCB.GetErrorList().Internal(iLine, iPos);
		return false;
	}

	rLVar = vR.TinyToZero(rCB.GetSensitivity());

/*
        TMultiV vR(m_pP3GABase->GetMVStyle());
        int iID = (int) vA.GetBase().BaseID();

        if (iID == ID_E3GA)
        {
                vR = PGA<TCVScalar>::hVec(vA[1], vA[2], vA[3], TCVScalar(1));
        }
        else if (iID == ID_PGA)
        {
                vR = vA;
        }
        else if (iID == ID_CONFGA)
        {
                if (::Round(Scalar(vA * rCB.GetN3GABase().vE), m_pFilter->GetDoubleSens()) != 0)
                {
                        vA = ConfGA<TCVScalar>::MakeEuclidVec(vA);
                        vR = PGA<TCVScalar>::hVec(vA[1], vA[2], vA[3], TCVScalar(1));
                }
                else
                {
                        vA = ConfGA<TCVScalar>::MakeEuclidVec(vA);
                        vR = PGA<TCVScalar>::hVec(vA[1], vA[2], vA[3], TCVScalar(0));
                }
        }
        else
        {
                rCB.GetErrorList().Internal(iLine, iPos);
                return false;
        }

        rLVar = vR.TinyToZero(rCB.GetSensitivity());
*/
	return true;
}

//////////////////////////////////////////////////////////////////////
// Create P3 Rotor from Multi-Vector FUNCTION
bool  RotorP3Func(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList* pmVars = rPars.GetVarListPtr();

	int iVarCount = int(pmVars->Count());

	if (iVarCount == 4)
	{
		TCVScalar pfVec[4];
		int i;
		for (i = 0; i < 4; i++)
		{
			if (!((*pmVars)(i).CastToScalar(pfVec[i], rCB.GetSensitivity())))
			{
				rCB.GetErrorList().InvalidType((*pmVars)(i), iLine, iPos);
				return false;
			}
		}

		if (!RotorP3(rCB, rVar, pfVec, iLine, iPos))
		{
			return false;	// VecP3 sets error
		}
	}
	else if (iVarCount == 2)
	{
		TCVScalar fDeg;
		TMultiV vA, vB;

		if ((*pmVars)(0).BaseType() != PDT_MULTIV)
		{
			rCB.GetErrorList().InvalidType((*pmVars)(0), iLine, iPos);
			return false;
		}

		vA = *((TMultiV*) (*pmVars)(0).GetMultiVPtr());
		rCB.CastMVtoP3(vA, vB);

		if (!((*pmVars)(1).CastToScalar(fDeg, rCB.GetSensitivity())))
		{
			rCB.GetErrorList().InvalidType((*pmVars)(1), iLine, iPos);
			return false;
		}

		if (!RotorP3(rCB, rVar, vB, fDeg, iLine, iPos))
		{
			return false;	// VecP3 sets error
		}
	}
	else
	{
		int piParNo[] = { 2, 4 };
		rCB.GetErrorList().WrongNoOfParams(piParNo, 2, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Create P3 Rotor from Multi-Vector
bool  RotorP3(CCLUCodeBase& rCB, CCodeVar& rLVar, TMultiV& rMV, TCVScalar fRad, int iLine, int iPos)
{
	if (!rLVar.New(PDT_MULTIV, "Constant"))
	{
		rCB.GetErrorList().Internal(iLine, iPos);
		return false;
	}

	bool bAxis = true;
	//TCVScalar fRad = fDeg * m_fRadPerDeg;

	if (rMV(2) != m_pP3GABase->vZero)
	{
		bAxis = false;
	}

	rLVar = m_pP3GABase->Rotor(rMV, fRad, bAxis);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Create P3 Rotor from axis
bool  RotorP3(CCLUCodeBase& rCB, CCodeVar& rLVar, TCVScalar* pfVec, int iLine, int iPos)
{
	if (!pfVec)
	{
		return false;
	}

	if (!rLVar.New(PDT_MULTIV, "Constant"))
	{
		rCB.GetErrorList().Internal(iLine, iPos);
		return false;
	}

	TCVScalar fRad = pfVec[3];	// * m_fRadPerDeg;

	rLVar = m_pP3GABase->Rotor(pfVec[0], pfVec[1], pfVec[2], fRad);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Create P3 Direction Vector
bool  DirVecP3Func(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList* pmVars = rPars.GetVarListPtr();

	int iVarCount = int(pmVars->Count());

	if (iVarCount == 3)
	{
		TCVScalar pfVec[3];
		int i;

		for (i = 0; i < 3; i++)
		{
			if (!((*pmVars)(i).CastToScalar(pfVec[i], rCB.GetSensitivity())))
			{
				rCB.GetErrorList().InvalidType((*pmVars)(i), iLine, iPos);
				return false;
			}
		}

		if (!rVar.New(PDT_MULTIV, "Constant"))
		{
			rCB.GetErrorList().Internal(iLine, iPos);
			return false;
		}
		rVar = m_pP3GABase->Vec(pfVec[0], pfVec[1], pfVec[2]);
	}
	else if (iVarCount == 1)
	{
		TCVCounter iVal;

		// If Parameter is a list of values
		// call VecE3 with this list of values.
		if ((*pmVars)(0).BaseType() == PDT_VARLIST)
		{
			int i, iCount;
			TVarList& rParList = *((*pmVars)(0).GetVarListPtr());

			iCount = int(rParList.Count());
			if ((rParList(0).BaseType() == PDT_SCALAR) ||
			    (rParList(0).BaseType() == PDT_COUNTER))
			{
				return VecP3Func(rCB, rVar, (*pmVars)(0), iLine, iPos);
			}
			else
			{
				rVar.New(PDT_VARLIST);
				TVarList& rList = *rVar.GetVarListPtr();
				rList.Set(iCount);

				for (i = 0; i < iCount; i++)
				{
					if (rParList(i).BaseType() == PDT_VARLIST)
					{
						if (!DirVecP3Func(rCB, rList[i], rParList(i), iLine, iPos))
						{
							return false;
						}
					}
					else
					{
						if (rParList(i).BaseType() == PDT_MULTIV)
						{
							if (!DirVecP3(rCB, rList[i], *((TMultiV*) rParList(i).GetMultiVPtr()), iLine, iPos))
							{
								return false;
							}
						}
						else
						{
							rCB.GetErrorList().InvalidType((*pmVars)(0), iLine, iPos);
							return false;
						}
					}
				}
			}

			return true;
		}

		if ((*pmVars)(0).CastToCounter(iVal))
		{
			if (!DirVecP3(rCB, rVar, iVal, iLine, iPos))
			{
				return false;	// VecE3 sets error
			}
		}
		else if ((*pmVars)(0).BaseType() == PDT_MULTIV)
		{
			TMultiV& rvA = *((*pmVars)(0).GetMultiVPtr());

			if (!DirVecP3(rCB, rVar, rvA, iLine, iPos))
			{
				return false;
			}
		}
		else
		{
			rCB.GetErrorList().InvalidType((*pmVars)(0), iLine, iPos);
			return false;
		}
	}
	else
	{
		int piParNo[] = { 1, 3 };
		rCB.GetErrorList().WrongNoOfParams(piParNo, 2, iLine, iPos);
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////
// Create P3 Dir.-Vector from transform variable

bool  DirVecP3(CCLUCodeBase& rCB, CCodeVar& rLVar, int iVal, int iLine, int iPos)
{
	if (!(&rCB.GetTransform()))
	{
		return false;
	}

	if (!rLVar.New(PDT_MULTIV, "Constant"))
	{
		rCB.GetErrorList().Internal(iLine, iPos);
		return false;
	}

	if ((iVal < 0) || (iVal > 9))
	{
		CStrMem csText;
		csText = iVal;
		rCB.GetErrorList().InvalidParVal(csText.Str(), 1, iLine, iPos);
		return false;
	}

	float* pfTrans = rCB.GetTransform()[iVal].pfTrans;
	TCVScalar pfVec[3];

	if (rCB.GetCLUDrawBase()->Is2dView())
	{
		pfVec[0] = (TCVScalar) pfTrans[0];
		pfVec[1] = -(TCVScalar) pfTrans[2];
		pfVec[2] = (TCVScalar) 0.0;
	}
	else
	{
		pfVec[0] = (TCVScalar) pfTrans[0];
		pfVec[1] = (TCVScalar) pfTrans[1];
		pfVec[2] = (TCVScalar) pfTrans[2];
	}

	rLVar = m_pP3GABase->Vec(pfVec);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Convert N3,E3 vector into P3 Vector

bool  DirVecP3(CCLUCodeBase& rCB, CCodeVar& rLVar, TMultiV& vA, int iLine, int iPos)
{
	TMultiV vR;

	if (!rCB.CastMVtoE3(vA, vR))
	{
		rCB.GetErrorList().Internal(iLine, iPos);
		return false;
	}

	vR.TinyToZero(rCB.GetSensitivity());
	rLVar = m_pP3GABase->Vec(vR[1], vR[2], vR[3]);

	return true;
}
