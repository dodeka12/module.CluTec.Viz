////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_E8_Define.cpp
//
// summary:   Implements the function e 8 define class
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
#include "Func_E8_Define.h"

//////////////////////////////////////////////////////////////////////
/// Define Variables specific to Euclidean 8D-Space
#ifdef USE_GA_E8

	bool  DefVarsE8(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
	{
		TVarList& mVars = *rPars.GetVarListPtr();
//

		int iVarCount = int(mVars.Count());

		if (iVarCount != 0)
		{
			rCB.GetErrorList().WrongNoOfParams(0, iLine, iPos);
			return false;
		}

		if (rCB.GetCurSpaceVars() == CCLUCodeBase::SPACEVARS_E8)
		{
			return true;
		}

		rCB.ResetSpaceVars();

		(rCB.NewConstVar("id", PDT_MULTIV) = m_pE8GABase->vSC).EnableProtect();
		(rCB.NewConstVar("e1", PDT_MULTIV) = m_pE8GABase->vE1).EnableProtect();
		(rCB.NewConstVar("e2", PDT_MULTIV) = m_pE8GABase->vE2).EnableProtect();
		(rCB.NewConstVar("e3", PDT_MULTIV) = m_pE8GABase->vE3).EnableProtect();
		(rCB.NewConstVar("e4", PDT_MULTIV) = m_pE8GABase->vE4).EnableProtect();
		(rCB.NewConstVar("e5", PDT_MULTIV) = m_pE8GABase->vE5).EnableProtect();
		(rCB.NewConstVar("e6", PDT_MULTIV) = m_pE8GABase->vE6).EnableProtect();
		(rCB.NewConstVar("e7", PDT_MULTIV) = m_pE8GABase->vE7).EnableProtect();
		(rCB.NewConstVar("e8", PDT_MULTIV) = m_pE8GABase->vE8).EnableProtect();
		(rCB.NewConstVar("I", PDT_MULTIV) = m_pE8GABase->vI).EnableProtect();

		rCB.GetCurSpaceVars() = CCLUCodeBase::SPACEVARS_E8;

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Create E8 Vector

	bool  VecE8Func(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
	{
		TVarList* pmVars = rPars.GetVarListPtr();

		int iVarCount = int(pmVars->Count());

		if (iVarCount == 8)
		{
			TCVScalar pfVec[8];
			int i;

			for (i = 0; i < 8; i++)
			{
				if (!((*pmVars)(i).CastToScalar(pfVec[i], rCB.GetSensitivity())))
				{
					rCB.GetErrorList().InvalidType((*pmVars)(i), iLine, iPos);
					return false;
				}
			}

			if (!VecE8(rCB, rVar, pfVec, iLine, iPos))
			{
				return false;	// VecE8 sets error
			}
		}
		else if (iVarCount == 1)
		{
			TCVCounter iVal;

			// If Parameter is a list of values
			// call VecE8 with this list of values.
			if ((*pmVars)(0).BaseType() == PDT_VARLIST)
			{
				int i, iCount;
				TVarList& rParList = *((*pmVars)(0).GetVarListPtr());

				iCount = int(rParList.Count());
				if ((rParList(0).BaseType() == PDT_SCALAR) ||
				    (rParList(0).BaseType() == PDT_COUNTER))
				{
					return VecE8Func(rCB, rVar, (*pmVars)(0), iLine, iPos);
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
							if (!VecE8Func(rCB, rList[i], rParList(i), iLine, iPos))
							{
								return false;
							}
						}
						else
						{
							if (rParList(i).BaseType() == PDT_MULTIV)
							{
								if (!VecE8(rCB, rList[i], *((TMultiV*) rParList(i).GetMultiVPtr()), iLine, iPos))
								{
									return false;
								}
							}
							//else if (rParList(i).BaseType() == PDT_MATRIX)
							//{
							//	if (!VecE8(rList[i], *((TMatrix *) rParList(i).GetMatrixPtr()), iLine, iPos))
							//		return false;
							//}
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
				if (!VecE8(rCB, rVar, iVal, iLine, iPos))
				{
					return false;	// VecE3 sets error
				}
			}
			else if ((*pmVars)(0).BaseType() == PDT_MULTIV)
			{
				if (!VecE8(rCB, rVar, *((TMultiV*) (*pmVars)(0).GetMultiVPtr()), iLine, iPos))
				{
					return false;
				}
			}
			//else if ((*pmVars)(0).BaseType() == PDT_MATRIX)
			//{
			//	if (!VecE8(rVar, *((TMatrix *) (*pmVars)(0).GetMatrixPtr()), iLine, iPos))
			//		return false;
			//}
			else
			{
				rCB.GetErrorList().InvalidType((*pmVars)(0), iLine, iPos);
				return false;
			}
		}
		else
		{
			int piVal[] = { 1, 8 };
			rCB.GetErrorList().WrongNoOfParams(piVal, 2, iLine, iPos);
			return false;
		}

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Create E8 Vector

	bool  VecE8(CCLUCodeBase& rCB, CCodeVar& rLVar, TCVScalar* pfVec, int iLine, int iPos)
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

		rLVar = m_pE8GABase->Vec(pfVec);

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Create E8 Vector from transform variable

	bool  VecE8(CCLUCodeBase& rCB, CCodeVar& rLVar, int iVal, int iLine, int iPos)
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

		TCVScalar pVec[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

		if (rCB.GetCLUDrawBase()->Is2dView())
		{
			pVec[0] = (TCVScalar) rCB.GetTransform()[iVal].pfTrans[0];
			pVec[1] = (TCVScalar) rCB.GetTransform()[iVal].pfTrans[1];
			pVec[2] = (TCVScalar) 0;
		}
		else
		{
			pVec[0] = (TCVScalar) rCB.GetTransform()[iVal].pfTrans[0];
			pVec[1] = (TCVScalar) rCB.GetTransform()[iVal].pfTrans[1];
			pVec[2] = (TCVScalar) rCB.GetTransform()[iVal].pfTrans[2];
		}

		rLVar = m_pE8GABase->Vec(pVec);

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Convert N3,P3, E3 vector into E8 Vector

	bool  VecE8(CCLUCodeBase& rCB, CCodeVar& rLVar, TMultiV& vA, int iLine, int iPos)
	{
		TMultiV vR;

		if (!rCB.CastMVtoE8(vA, vR))
		{
			rCB.GetErrorList().Internal(iLine, iPos);
			return false;
		}

		rLVar = vR.TinyToZero(rCB.GetSensitivity());

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Create E8 Rotor from Multi-Vector FUNCTION

	bool  RotorE8Func(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
	{
		TVarList* pmVars = rPars.GetVarListPtr();

		int iVarCount = int(pmVars->Count());

		if (iVarCount == 2)
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

			if (!RotorE8(rCB, rVar, vA, fDeg, iLine, iPos))
			{
				return false;
			}
		}
		else
		{
			int piParNo[] = { 2 };
			rCB.GetErrorList().WrongNoOfParams(piParNo, 2, iLine, iPos);
			return false;
		}

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Create E8 Rotor from Multi-Vector

	bool  RotorE8(CCLUCodeBase& rCB, CCodeVar& rLVar, TMultiV& rMV, TCVScalar fRad, int iLine, int iPos)
	{
		if (!rLVar.New(PDT_MULTIV, "Constant"))
		{
			rCB.GetErrorList().Internal(iLine, iPos);
			return false;
		}

		rLVar = m_pE8GABase->Rotor(rMV, fRad);

		return true;
	}

#endif
