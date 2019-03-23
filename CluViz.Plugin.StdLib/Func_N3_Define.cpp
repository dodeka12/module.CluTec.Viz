////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_N3_Define.cpp
//
// summary:   Implements the function 3 define class
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
#include "Func_N3_Define.h"

//////////////////////////////////////////////////////////////////////
/// Define Variables specific to N3

bool  DefVarsN3(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList* pmVars = rPars.GetVarListPtr();

	int iVarCount = int(pmVars->Count());

	if (iVarCount != 0)
	{
		rCB.GetErrorList().WrongNoOfParams(0, iLine, iPos);
		return false;
	}

	if (rCB.GetCurSpaceVars() == CCLUCodeBase::SPACEVARS_N3)
	{
		return true;
	}

	rCB.ResetSpaceVars();

	(rCB.NewConstVar("id", PDT_MULTIV) = m_pN3GABase->vSC).EnableProtect();
	(rCB.NewConstVar("e1", PDT_MULTIV) = m_pN3GABase->vE1).EnableProtect();
	(rCB.NewConstVar("e2", PDT_MULTIV) = m_pN3GABase->vE2).EnableProtect();
	(rCB.NewConstVar("e3", PDT_MULTIV) = m_pN3GABase->vE3).EnableProtect();
	(rCB.NewConstVar("ep", PDT_MULTIV) = m_pN3GABase->vEp).EnableProtect();
	(rCB.NewConstVar("em", PDT_MULTIV) = m_pN3GABase->vEm).EnableProtect();
	(rCB.NewConstVar("e", PDT_MULTIV) = m_pN3GABase->vE).EnableProtect();
	(rCB.NewConstVar("einf", PDT_MULTIV) = m_pN3GABase->vE).EnableProtect();
	(rCB.NewConstVar("e0", PDT_MULTIV) = m_pN3GABase->vE0).EnableProtect();
	(rCB.NewConstVar("ni", PDT_MULTIV) = m_pN3GABase->vN).EnableProtect();
	(rCB.NewConstVar("no", PDT_MULTIV) = m_pN3GABase->vNb).EnableProtect();
	(rCB.NewConstVar("n", PDT_MULTIV) = m_pN3GABase->vN).EnableProtect();
	(rCB.NewConstVar("nb", PDT_MULTIV) = m_pN3GABase->vNb).EnableProtect();
	(rCB.NewConstVar("I", PDT_MULTIV) = m_pN3GABase->vI).EnableProtect();

	rCB.GetCurSpaceVars() = CCLUCodeBase::SPACEVARS_N3;

	return true;
}

//////////////////////////////////////////////////////////////////////
// Create N3 Vector

bool  VecN3Func(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList* pmVars = rPars.GetVarListPtr();
	int iVarCount    = int(pmVars->Count());
	TCVCounter iVal;
	TCVScalar dVal;

	if ((iVarCount == 2) || (iVarCount == 3))
	{
		if (!(*pmVars)(0).CastToScalar(dVal))
		{
			rCB.GetErrorList().GeneralError("Expect scalar values if more than one parameter is specified.", iLine, iPos);
			return false;
		}

		if (!VecN3(rCB, rVar, *pmVars, iLine, iPos))
		{
			return false;
		}
	}
	else if (iVarCount == 1)
	{
		// If Parameter is a list of values
		// call VecN3 with this list of values.
		if ((*pmVars)(0).BaseType() == PDT_VARLIST)
		{
			if (!VecN3(rCB, rVar, *((*pmVars)(0).GetVarListPtr()), iLine, iPos))
			{
				return false;
			}
		}
		else if ((*pmVars)(0).CastToCounter(iVal))
		{
			if (!VecN3(rCB, rVar, iVal, iLine, iPos))
			{
				return false;	// VecE3 sets error
			}
		}
		else if ((*pmVars)(0).BaseType() == PDT_MULTIV)
		{
			if (!VecN3(rCB, rVar, *((TMultiV*) (*pmVars)(0).GetMultiVPtr()), iLine, iPos))
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
		int piVal[] = { 1, 2, 3 };
		rCB.GetErrorList().WrongNoOfParams(piVal, 3, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Create N3 Vector

bool  VecN3(CCLUCodeBase& rCB, CCodeVar& rVar, TVarList& rVarList, int iLine, int iPos)
{
	int i, iCount;

	iCount = int(rVarList.Count());

	if ((rVarList(0).BaseType() == PDT_SCALAR) ||
	    (rVarList(0).BaseType() == PDT_COUNTER))
	{
		TCVScalar pfVec[3];
		int i;

		for (i = 0; i < iCount; i++)
		{
			if (!(rVarList(i).CastToScalar(pfVec[i], rCB.GetSensitivity())))
			{
				rCB.GetErrorList().InvalidType(rVarList(i), iLine, iPos);
				return false;
			}
		}

		if (iCount == 2)
		{
			pfVec[2] = TCVScalar(0);
		}

		if (!VecN3(rCB, rVar, pfVec, iLine, iPos))
		{
			return false;	// VecE3 sets error
		}
	}
	else
	{
		rVar.New(PDT_VARLIST);
		TVarList& rList = *rVar.GetVarListPtr();
		rList.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (rVarList(i).BaseType() == PDT_VARLIST)
			{
				if (!VecN3(rCB, rList[i], *rVarList(i).GetVarListPtr(), iLine, iPos))
				{
					return false;
				}
			}
			else
			{
				if (rVarList(i).BaseType() == PDT_MULTIV)
				{
					if (!VecN3(rCB, rList[i], *((TMultiV*) rVarList(i).GetMultiVPtr()), iLine, iPos))
					{
						return false;
					}
				}
				else
				{
					rCB.GetErrorList().InvalidType(rVarList(i), iLine, iPos);
					return false;
				}
			}
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Create N3 Vector

bool  VecN3(CCLUCodeBase& rCB, CCodeVar& rLVar, TCVScalar* pfVec, int iLine, int iPos)
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

	rLVar = m_pN3GABase->NullVec(pfVec);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Create N3 Vector from transform variable

bool  VecN3(CCLUCodeBase& rCB, CCodeVar& rLVar, int iVal, int iLine, int iPos)
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

	TCVScalar pVec[3];

	if (rCB.GetCLUDrawBase()->Is2dView())
	{
		pVec[0] = (TCVScalar) rCB.GetTransform()[iVal].pfTrans[0];
		pVec[1] = (TCVScalar) rCB.GetTransform()[iVal].pfTrans[1];
		pVec[2] = (TCVScalar) 0;
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			pVec[i] = (TCVScalar) rCB.GetTransform()[iVal].pfTrans[i];
		}
	}

	rLVar = m_pN3GABase->NullVec(pVec);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Convert P3,E3 vector into N3 Vector

bool  VecN3(CCLUCodeBase& rCB, CCodeVar& rLVar, TMultiV& vA, int iLine, int iPos)
{
	TMultiV vR(m_pN3GABase->GetMVStyle());
	int iID = (int) vA.GetBase().BaseID();

	if (iID == ID_E3GA)
	{
		vR = ConfGA<TCVScalar>::NullVec(vA[1], vA[2], vA[3]);
	}
	#ifdef USE_GA_E8
		else if (iID == ID_E8GA)
		{
			vR = ConfGA<TCVScalar>::NullVec(vA[1], vA[2], vA[3]);
		}
	#endif
	else if (iID == ID_PGA)
	{
		if (vA[4] != 0)
		{
			vA = PGA<TCVScalar>::Vec(vA);
			vR = ConfGA<TCVScalar>::NullVec(vA[1], vA[2], vA[3]);
		}
		else
		{
			vR = ConfGA<TCVScalar>::DirVec(vA[1], vA[2], vA[3]);
		}
	}
	else if (iID == ID_CONFGA)
	{
		vR = vA;
	}
	else
	{
		rCB.GetErrorList().Internal(iLine, iPos);
		return false;
	}

	rLVar = vR.TinyToZero(rCB.GetSensitivity());

	return true;
}

//////////////////////////////////////////////////////////////////////
// Create N3 Rotor from Multi-Vector FUNCTION
bool  RotorN3Func(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
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

		if (!RotorN3(rCB, rVar, pfVec, iLine, iPos))
		{
			return false;	// VecE3 sets error
		}
	}
	else if (iVarCount == 2)
	{
		TCVScalar fDeg;
		TMultiV vA;

		if ((*pmVars)(0).BaseType() != PDT_MULTIV)
		{
			rCB.GetErrorList().InvalidType((*pmVars)(0), iLine, iPos);
			return false;
		}

		vA = *((TMultiV*) (*pmVars)(0).GetMultiVPtr());

		if (!((*pmVars)(1).CastToScalar(fDeg, rCB.GetSensitivity())))
		{
			rCB.GetErrorList().InvalidType((*pmVars)(1), iLine, iPos);
			return false;
		}

		if (!RotorN3(rCB, rVar, vA, fDeg, iLine, iPos))
		{
			return false;	// VecE3 sets error
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
// Create N3 Rotor from Multi-Vector
bool  RotorN3(CCLUCodeBase& rCB, CCodeVar& rLVar, TMultiV& rMV, TCVScalar fRad, int iLine, int iPos)
{
	if (!rLVar.New(PDT_MULTIV, "Constant"))
	{
		rCB.GetErrorList().Internal(iLine, iPos);
		return false;
	}

	bool bAxis = true;
	//TCVScalar fRad = fDeg * m_fRadPerDeg;

	if (rMV(2) != rCB.GetE3GABase().vZero)
	{
		bAxis = false;
	}

	rLVar = m_pN3GABase->Rotor(rMV, fRad, bAxis);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Create N3 Rotor from axis
bool  RotorN3(CCLUCodeBase& rCB, CCodeVar& rLVar, TCVScalar* pfVec, int iLine, int iPos)
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

	rLVar = m_pN3GABase->Rotor(pfVec[0], pfVec[1], pfVec[2], fRad);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Create N3 Sphere from center and radius in OPNS
bool  SphereN3(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList* pmVars = rPars.GetVarListPtr();

	int iVarCount = int(pmVars->Count());

	TCVScalar pfVec[4];

	if (iVarCount == 2)
	{
		TMultiV vA;

		if ((*pmVars)(0).BaseType() != PDT_MULTIV)
		{
			rCB.GetErrorList().InvalidType((*pmVars)(0), iLine, iPos);
			return false;
		}

		if (!rCB.CastMVtoE3(*((TMultiV*) (*pmVars)(0).GetMultiVPtr()), vA))
		{
			rCB.GetErrorList().InvalidParVal((*pmVars)(0), iLine, iPos);
			return false;
		}

		if (!((*pmVars)(1).CastToScalar(pfVec[3], rCB.GetSensitivity())))
		{
			rCB.GetErrorList().InvalidType((*pmVars)(1), iLine, iPos);
			return false;
		}

		pfVec[0] = vA[rCB.GetE3GABase().iE1];
		pfVec[1] = vA[rCB.GetE3GABase().iE2];
		pfVec[2] = vA[rCB.GetE3GABase().iE3];
	}
	else if (iVarCount == 4)
	{
		int i;

		for (i = 0; i < 4; i++)
		{
			if (!((*pmVars)(i).CastToScalar(pfVec[i], rCB.GetSensitivity())))
			{
				rCB.GetErrorList().InvalidType((*pmVars)(i), iLine, iPos);
				return false;
			}
		}
	}
	else
	{
		int piParNo[] = { 2, 4 };
		rCB.GetErrorList().WrongNoOfParams(piParNo, 2, iLine, iPos);
		return false;
	}

	if (!rVar.New(PDT_MULTIV, "Constant"))
	{
		rCB.GetErrorList().Internal(iLine, iPos);
		return false;
	}
	rVar = *m_pN3GABase->Sphere(pfVec[0], pfVec[1], pfVec[2], pfVec[3]);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Create N3 Translator from 3 scalars or take vector part of multivector
bool  TranslatorN3(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
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

		rVar = m_pN3GABase->Translator(pfVec[0], pfVec[1], pfVec[2]);
	}
	else if (iVarCount == 1)
	{
		TMultiV vA;

		if ((*pmVars)(0).BaseType() != PDT_MULTIV)
		{
			rCB.GetErrorList().InvalidType((*pmVars)(0), iLine, iPos);
			return false;
		}

		vA = *((TMultiV*) (*pmVars)(0).GetMultiVPtr());

		if (!rVar.New(PDT_MULTIV, "Constant"))
		{
			rCB.GetErrorList().Internal(iLine, iPos);
			return false;
		}

		rVar = m_pN3GABase->Translator(vA);
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
/// Create N3 Dilator
bool  DilatorN3(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList* pmVars = rPars.GetVarListPtr();

	int iVarCount = int(pmVars->Count());

	if (iVarCount == 1)
	{
		TCVScalar pfVec;

		if (!((*pmVars)(0).CastToScalar(pfVec, rCB.GetSensitivity())))
		{
			rCB.GetErrorList().InvalidType((*pmVars)(0), iLine, iPos);
			return false;
		}

		if (!rVar.New(PDT_MULTIV, "Constant"))
		{
			rCB.GetErrorList().Internal(iLine, iPos);
			return false;
		}

		rVar = m_pN3GABase->Dilator(pfVec);
	}
	else
	{
		int piParNo[] = { 1 };
		rCB.GetErrorList().WrongNoOfParams(piParNo, 1, iLine, iPos);
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////
/// Create N3 Direction Vector
bool  DirVecN3(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& rVars = *rPars.GetVarListPtr();

	int iVarCount = int(rVars.Count());
	if (iVarCount == 1)
	{
		ECodeDataType eType = rVars(0).BaseType();

		if (eType == PDT_MULTIV)
		{
			TMultiV vA;

			if (!rCB.CastMVtoE3(*rVars(0).GetMultiVPtr(), vA))
			{
				rCB.GetErrorList().InvalidParVal(rVars(0), iLine, iPos);
				return false;
			}

			rVar = m_pN3GABase->DirVec(vA[E3GA < TCVScalar > ::iE1], vA[E3GA < TCVScalar > ::iE2], vA[E3GA < TCVScalar > ::iE3]);
		}
		else if (eType == PDT_VARLIST)
		{
			TVarList& rList = *rVars(0).GetVarListPtr();

			int iIdx, iCnt = int(rList.Count());

			if (iCnt != 3)
			{
				rCB.GetErrorList().GeneralError("Expect a list of three scalar values.", iLine, iPos);
				return false;
			}

			TCVScalar pfVec[3];

			for (iIdx = 0; iIdx < 3; ++iIdx)
			{
				if (!rList(iIdx).CastToScalar(pfVec[iIdx], rCB.GetSensitivity()))
				{
					rCB.GetErrorList().GeneralError("Expect a list of three scalar values.", iLine, iPos);
					return false;
				}
			}

			rVar = m_pN3GABase->DirVec(pfVec[0], pfVec[1], pfVec[2]);
		}
		else
		{
			rCB.GetErrorList().GeneralError("Expect as single parameter a multivector or a list of three scalar values.", iLine, iPos);
			return false;
		}
	}
	else if (iVarCount == 3)
	{
		TCVScalar pfVec[3];
		int i;

		for (i = 0; i < 3; i++)
		{
			if (!(rVars(i).CastToScalar(pfVec[i], rCB.GetSensitivity())))
			{
				rCB.GetErrorList().InvalidType(rVars(i), iLine, iPos);
				return false;
			}
		}

		if (!rVar.New(PDT_MULTIV, "Constant"))
		{
			rCB.GetErrorList().Internal(iLine, iPos);
			return false;
		}

		rVar = m_pN3GABase->DirVec(pfVec[0], pfVec[1], pfVec[2]);
	}
	else
	{
		int piParNo[] = { 1, 3 };
		rCB.GetErrorList().WrongNoOfParams(piParNo, 2, iLine, iPos);
		return false;
	}
	return true;
}
