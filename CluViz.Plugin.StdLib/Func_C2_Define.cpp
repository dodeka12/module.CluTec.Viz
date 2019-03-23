////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_C2_Define.cpp
//
// summary:   Implements the function c 2 define class
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
#include "Func_C2_Define.h"

//////////////////////////////////////////////////////////////////////
/// Define Variables specific to Ellipsoidal 2d space

bool  DefVarsEl2(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
//

	int iVarCount = int(mVars.Count());

	if (iVarCount != 0)
	{
		rCB.GetErrorList().WrongNoOfParams(0, iLine, iPos);
		return false;
	}

	if (rCB.GetCurSpaceVars() == CCLUCodeBase::SPACEVARS_C2)
	{
		return true;
	}

	rCB.ResetSpaceVars();

	(rCB.NewConstVar("id", PDT_MULTIV) = m_pEl2GABase->vSC).EnableProtect();
	(rCB.NewConstVar("e1", PDT_MULTIV) = m_pEl2GABase->vE1).EnableProtect();
	(rCB.NewConstVar("e2", PDT_MULTIV) = m_pEl2GABase->vE2).EnableProtect();
	(rCB.NewConstVar("e3", PDT_MULTIV) = m_pEl2GABase->vE3).EnableProtect();
	(rCB.NewConstVar("e4", PDT_MULTIV) = m_pEl2GABase->vE4).EnableProtect();
	(rCB.NewConstVar("e5", PDT_MULTIV) = m_pEl2GABase->vE5).EnableProtect();
	(rCB.NewConstVar("e6", PDT_MULTIV) = m_pEl2GABase->vE6).EnableProtect();
	(rCB.NewConstVar("I", PDT_MULTIV) = m_pEl2GABase->vI).EnableProtect();

	rCB.GetCurSpaceVars() = CCLUCodeBase::SPACEVARS_C2;

	return true;
}

//////////////////////////////////////////////////////////////////////
// Create El2 Vector

bool  VecEl2Func(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList* pmVars = rPars.GetVarListPtr();

	int iVarCount = int(pmVars->Count());

	if (iVarCount == 2)
	{
		TCVScalar pfVec[2];
		int i;

		for (i = 0; i < 2; i++)
		{
			if (!((*pmVars)(i).CastToScalar(pfVec[i], rCB.GetSensitivity())))
			{
				rCB.GetErrorList().InvalidType((*pmVars)(i), iLine, iPos);
				return false;
			}
		}

		if (!VecEl2(rCB, rVar, pfVec, iLine, iPos))
		{
			return false;	// VecE3 sets error
		}
	}
	else if (iVarCount == 1)
	{
		TCVCounter iVal;

		// If Parameter is a list of values
		// call VecEl2 with this list of values.
		if ((*pmVars)(0).BaseType() == PDT_VARLIST)
		{
			int i, iCount;
			TVarList& rParList = *((*pmVars)(0).GetVarListPtr());

			iCount = int(rParList.Count());
			if ((rParList(0).BaseType() == PDT_SCALAR) ||
			    (rParList(0).BaseType() == PDT_COUNTER))
			{
				return VecEl2Func(rCB, rVar, (*pmVars)(0), iLine, iPos);
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
						if (!VecEl2Func(rCB, rList[i], rParList(i), iLine, iPos))
						{
							return false;
						}
					}
					else
					{
						if (rParList(i).BaseType() == PDT_MULTIV)
						{
							if (!VecEl2(rCB, rList[i], *((TMultiV*) rParList(i).GetMultiVPtr()), iLine, iPos))
							{
								return false;
							}
						}
						else if (rParList(i).BaseType() == PDT_MATRIX)
						{
							if (!VecEl2(rCB, rList[i], *((TMatrix*) rParList(i).GetMatrixPtr()), iLine, iPos))
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
			if (!VecEl2(rCB, rVar, iVal, iLine, iPos))
			{
				return false;	// VecE3 sets error
			}
		}
		else if ((*pmVars)(0).BaseType() == PDT_MULTIV)
		{
			if (!VecEl2(rCB, rVar, *((TMultiV*) (*pmVars)(0).GetMultiVPtr()), iLine, iPos))
			{
				return false;
			}
		}
		else if ((*pmVars)(0).BaseType() == PDT_MATRIX)
		{
			if (!VecEl2(rCB, rVar, *((TMatrix*) (*pmVars)(0).GetMatrixPtr()), iLine, iPos))
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
		int piVal[] = { 1, 2 };
		rCB.GetErrorList().WrongNoOfParams(piVal, 2, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Create El2 Vector

bool  VecEl2(CCLUCodeBase& rCB, CCodeVar& rLVar, TCVScalar* pfVec, int iLine, int iPos)
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

	rLVar = m_pEl2GABase->Vec(pfVec);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Create El2 Vector from transform variable

bool  VecEl2(CCLUCodeBase& rCB, CCodeVar& rLVar, int iVal, int iLine, int iPos)
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

	TCVScalar pVec[2];

	if (rCB.GetCLUDrawBase()->Is2dView())
	{
		pVec[0] = (TCVScalar) rCB.GetTransform()[iVal].pfTrans[0];
		pVec[1] = (TCVScalar) rCB.GetTransform()[iVal].pfTrans[1];
	}
	else
	{
		pVec[0] = (TCVScalar) rCB.GetTransform()[iVal].pfTrans[0];
		pVec[1] = -(TCVScalar) rCB.GetTransform()[iVal].pfTrans[2];
	}
	rLVar = m_pEl2GABase->Vec(pVec);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Convert N3,P3, E3 vector into El2 Vector

bool  VecEl2(CCLUCodeBase& rCB, CCodeVar& rLVar, TMultiV& vA, int iLine, int iPos)
{
	TMultiV vR;

	if (!rCB.CastMVtoEl2(vA, vR))
	{
		rCB.GetErrorList().Internal(iLine, iPos);
		return false;
	}

	rLVar = vR.TinyToZero(rCB.GetSensitivity());

	return true;
}

//////////////////////////////////////////////////////////////////////
// Convert 3x3 matrix to vector

bool  VecEl2(CCLUCodeBase& rCB, CCodeVar& rLVar, TMatrix& xM, int iLine, int iPos)
{
	TMultiV vR;

	if ((xM.Rows() != 3) || (xM.Cols() != 3))
	{
		rCB.GetErrorList().GeneralError("Matrix is not of dimension 3x3.", iLine, iPos);
		return false;
	}

	vR    = m_pEl2GABase->Vec(xM);
	rLVar = vR.TinyToZero(rCB.GetSensitivity());

	return true;
}

//////////////////////////////////////////////////////////////////////
// Create C2 Rotor from Angle

bool  RotorC2Func(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount  = int(mVars.Count());
	TCVScalar fVal = 0;

	if (iVarCount == 1)
	{
		if (!mVars(0).CastToScalar(fVal, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}
	}
	else
	{
		int piParNo[] = { 1 };
		rCB.GetErrorList().WrongNoOfParams(piParNo, 1, iLine, iPos);
		return false;
	}

	TCVScalar fS, fC, fSh, fCh, fFac;
	TMultiV R1(m_pEl2GABase->GetMVStyle()), R2(m_pEl2GABase->GetMVStyle());

	fFac = TCVScalar(1.0 / sqrt(2.0));
	fS   = TCVScalar(sin(double(fVal)));
	fC   = TCVScalar(cos(double(fVal)));
	fSh  = TCVScalar(sin(double(fVal) / 2.0));
	fCh  = TCVScalar(cos(double(fVal) / 2.0));

	R1 = (fC ^ m_pEl2GABase->vSC) + ((fFac * fS) ^ (m_pEl2GABase->vE5 - m_pEl2GABase->vE4) ^ m_pEl2GABase->vE6);
	R2 = (fCh  ^ m_pEl2GABase->vSC) + (fSh ^ m_pEl2GABase->vE2 ^ m_pEl2GABase->vE1);

	rVar = (R2 & R1);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Create C2 Reflector from Angle

bool  ReflectorC2Func(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount  = int(mVars.Count());
	TCVScalar fVal = 0;

	if (iVarCount == 1)
	{
		if (!mVars(0).CastToScalar(fVal, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}
	}
	else
	{
		int piParNo[] = { 1 };
		rCB.GetErrorList().WrongNoOfParams(piParNo, 1, iLine, iPos);
		return false;
	}

	TCVScalar fS, fC, fSh, fCh, fFac;
	TMultiV R1(m_pEl2GABase->GetMVStyle()), R2(m_pEl2GABase->GetMVStyle());

	fFac = TCVScalar(1.0 / sqrt(2.0));
	fS   = TCVScalar(sin(double(fVal) * 2.0));
	fC   = TCVScalar(cos(double(fVal) * 2.0));
	fSh  = TCVScalar(sin(double(fVal)));
	fCh  = TCVScalar(cos(double(fVal)));

	R1 = (-fC ^ m_pEl2GABase->vE6) + ((fFac * fS) ^ (m_pEl2GABase->vE4 - m_pEl2GABase->vE5));
	R2 = (fCh  ^ m_pEl2GABase->vE2) - (fSh ^ m_pEl2GABase->vE1);

	rVar = (R2 & R1);

	return true;
}
