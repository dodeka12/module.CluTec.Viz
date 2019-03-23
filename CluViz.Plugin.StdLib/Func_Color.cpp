////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_Color.cpp
//
// summary:   Implements the function color class
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
#include "Func_Color.h"

//////////////////////////////////////////////////////////////////////
/// The Color FUNCTION

bool ColorFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount == 1)
	{
		if (mVars(0).BaseType() != PDT_MULTIV)
		{
			rCB.GetErrorList().GeneralError("Expect vector as single parameter.", iLine, iPos);
			return false;
		}

		TMultiV& rA = *mVars(0).GetMultiVPtr();

		// Analyze Multivector
		if (!rCB.GetFilter()->DrawMV(rA, true))
		{
			rCB.GetErrorList().GeneralError("Multivector does not represent point.", iLine, iPos);
			return false;
		}

		CMVInfo<TCVScalar> Info;
		rCB.GetFilter()->GetMVInfo(Info);
		if (Info.m_eType != GA_POINT)
		{
			rCB.GetErrorList().GeneralError("Multivector does not represent point.", iLine, iPos);
			return false;
		}

		if (!rVar.New(PDT_COLOR, "Constant"))
		{
			rCB.GetErrorList().OutOfMemory(iLine, iPos);
			return false;
		}

		COGLColor& rCol = *rVar.GetOGLColorPtr();

		rCol.Set((float) Info.m_mVex[0][0], (float) Info.m_mVex[0][1],
				(float) Info.m_mVex[0][2]);
	}
	else if (iVarCount == 3)
	{
		TCVScalar fR, fG, fB;

		if (!mVars(0).CastToScalar(fR, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (!mVars(1).CastToScalar(fG, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		if (!mVars(2).CastToScalar(fB, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}
/*
                if (fR < 0) fR = 0.0;
                else if (fR > 1) fR = 1.0;

                if (fG < 0) fG = 0.0;
                else if (fG > 1) fG = 1.0;

                if (fB < 0) fB = 0.0;
                else if (fB > 1) fB = 1.0;
*/
		if (!rVar.New(PDT_COLOR, "Constant"))
		{
			rCB.GetErrorList().OutOfMemory(iLine, iPos);
			return false;
		}

		COGLColor& rCol = *rVar.GetOGLColorPtr();

		rCol.Set((float) fR, (float) fG, (float) fB);
	}
	else if (iVarCount == 4)
	{
		TCVScalar fR, fG, fB, fA;

		if (!mVars(0).CastToScalar(fR, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (!mVars(1).CastToScalar(fG, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		if (!mVars(2).CastToScalar(fB, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}

		if (!mVars(3).CastToScalar(fA, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(mVars(3), 4, iLine, iPos);
			return false;
		}
/*
                if (fR < 0) fR = 0.0;
                else if (fR > 1) fR = 1.0;

                if (fG < 0) fG = 0.0;
                else if (fG > 1) fG = 1.0;

                if (fB < 0) fB = 0.0;
                else if (fB > 1) fB = 1.0;

                if (fA < 0) fA = 0.0;
                else if (fA > 1) fA = 1.0;
*/
		if (!rVar.New(PDT_COLOR, "Constant"))
		{
			rCB.GetErrorList().OutOfMemory(iLine, iPos);
			return false;
		}

		COGLColor& rCol = *rVar.GetOGLColorPtr();

		rCol.Set((float) fR, (float) fG, (float) fB, (float) fA);
	}
	else
	{
		int piParCount[3] = { 1, 3, 4 };

		rCB.GetErrorList().WrongNoOfParams(piParCount, 3, iLine, iPos);
		return false;
	}

	return true;
}

bool SetColorFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (mVars(0).BaseType() != PDT_COLOR)
	{
		rCB.GetErrorList().GeneralError("Expect first parameter to be a Color object.", iLine, iPos);
		return false;
	}

	COGLColor& rColTrg = *mVars(0).GetOGLColorPtr();

	if (iVarCount == 2)
	{
		if (mVars(1).BaseType() != PDT_COLOR)
		{
			rCB.GetErrorList().GeneralError("Expect second parameter to be a Color object.", iLine, iPos);
			return false;
		}

		COGLColor& rColSrc = *mVars(1).GetOGLColorPtr();

		rColTrg = rColSrc;
	}
	else if ((iVarCount == 4) || (iVarCount == 5))
	{
		TCVScalar fR, fG, fB, fA;

		if (!mVars(1).CastToScalar(fR, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		if (!mVars(2).CastToScalar(fG, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}

		if (!mVars(3).CastToScalar(fB, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(mVars(3), 4, iLine, iPos);
			return false;
		}

		fA = 1.0;
		if (iVarCount == 5)
		{
			if (!mVars(4).CastToScalar(fA, rCB.GetSensitivity()))
			{
				rCB.GetErrorList().InvalidParType(mVars(4), 5, iLine, iPos);
				return false;
			}
		}

		rColTrg.Set((float) fR, (float) fG, (float) fB, (float) fA);
	}
	else
	{
		int piParCount[3] = { 2, 4, 5 };

		rCB.GetErrorList().WrongNoOfParams(piParCount, 3, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Create an animated color element

bool AnimColorFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount < 2)
	{
		rCB.GetErrorList().GeneralError("Expect at least the color to animate and the type of animation.", iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_COLOR)
	{
		rCB.GetErrorList().GeneralError("Expect a color as first parameter.", iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().GeneralError("Expect an animation type string as second parameter.", iLine, iPos);
		return false;
	}

	TOGLColor colBase = *mVars(0).GetOGLColorPtr();
	TString sType     = *mVars(1).GetStringPtr();
	sType.ToLowerCase();

	if ((sType == "sinus") || (sType == "sinus2") || (sType == "constant"))
	{
		if (iVarCount != 4)
		{
			rCB.GetErrorList().GeneralError("Expect color direction and frequency as parameters for 'sinus' animation.", iLine, iPos);
			return false;
		}

		TCVScalar dFreq;

		if (mVars(2).BaseType() != PDT_COLOR)
		{
			rCB.GetErrorList().GeneralError("Expect a color as third parameter.", iLine, iPos);
			return false;
		}

		if (!mVars(3).CastToScalar(dFreq, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Expect an animation frequency as fourth parameter.", iLine, iPos);
			return false;
		}

		TOGLColor colDir = *mVars(2).GetOGLColorPtr();

		COGLAnimColor* pAnimColor = new COGLAnimColor;

		pAnimColor->Set(colBase);
		pAnimColor->SetDir(colDir);
		pAnimColor->SetFreq(float(dFreq));

		if (sType == "constant")
		{
			pAnimColor->SetMode(COGLAnimColor::/*EAnimMode::*/ CONSTANT);
		}
		else if (sType == "sinus")
		{
			pAnimColor->SetMode(COGLAnimColor::/*EAnimMode::*/ SINUS);
		}
		else if (sType == "sinus2")
		{
			pAnimColor->SetMode(COGLAnimColor::/*EAnimMode::*/ SINUS2);
		}

		COGLBEReference AnimColRef = rCB.GetOGLDrawBase()->GetSceneRepository()->New(pAnimColor);
		rCB.GetOGLDrawBase()->DrawBaseElement(AnimColRef);
		rVar = AnimColRef;
	}
	else
	{
		COGLAnimColor* pAnimColor = new COGLAnimColor;

		pAnimColor->Set(colBase);
		pAnimColor->SetMode(COGLAnimColor::NONE);

		COGLBEReference AnimColRef = rCB.GetOGLDrawBase()->GetSceneRepository()->New(pAnimColor);
		rCB.GetOGLDrawBase()->DrawBaseElement(AnimColRef);
		rVar = AnimColRef;
	}

	return true;
}
