////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_GA.cpp
//
// summary:   Implements the function ga class
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

#include "Func_GA.h"

#define VAR_LIST_PAIR(rCodeVar, Val1, Val2) \
	rCodeVar.New(PDT_VARLIST); \
	{ \
		TVarList& rVex = *rCodeVar.GetVarListPtr(); \
		rVex.Set(2); \
		rVex[0] = Val1; \
		rVex[1] = Val2; \
	}

//////////////////////////////////////////////////////////////////////
/// The Commutator Product FUNCTION

bool  CPFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount == 2)
	{
		if ((mVars(0).BaseType() == PDT_MULTIV) && (mVars(1).BaseType() == PDT_MULTIV))
		{
			TMultiV& vA = *mVars(0).GetMultiVPtr();
			TMultiV& vB = *mVars(1).GetMultiVPtr();

			rVar = (0.5 & ((vA & vB) - (vB & vA)));
		}
		else
		{
			if (mVars(0).BaseType() != PDT_MULTIV)
			{
				rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			}
			else if (mVars(1).BaseType() != PDT_MULTIV)
			{
				rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			}

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
/// The Anti-Commutator Product FUNCTION

bool  ACPFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount == 2)
	{
		if ((mVars(0).BaseType() == PDT_MULTIV) && (mVars(1).BaseType() == PDT_MULTIV))
		{
			TMultiV& vA = *mVars(0).GetMultiVPtr();
			TMultiV& vB = *mVars(1).GetMultiVPtr();

			rVar = (0.5 & ((vA & vB) + (vB & vA)));
		}
		else
		{
			if (mVars(0).BaseType() != PDT_MULTIV)
			{
				rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			}
			else if (mVars(1).BaseType() != PDT_MULTIV)
			{
				rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			}

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
/// The Projection FUNCTION
/// Project par 1 onto par 2

bool  ProjectFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount == 2)
	{
		if (mVars(0).BaseType() != PDT_MULTIV)
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (mVars(1).BaseType() != PDT_MULTIV)
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		TMultiV& vA = *mVars(0).GetMultiVPtr();
		TMultiV& vB = *mVars(1).GetMultiVPtr();

		rVar = Project(vB, vA);
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(2, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// The Rejection FUNCTION
/// Reject par 1 from par 2

bool  RejectFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount == 2)
	{
		if (mVars(0).BaseType() != PDT_MULTIV)
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (mVars(1).BaseType() != PDT_MULTIV)
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		TMultiV& vA = *mVars(0).GetMultiVPtr();
		TMultiV& vB = *mVars(1).GetMultiVPtr();

		rVar = Reject(vB, vA);
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(2, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Outer Product FUNCTION

bool  OuterProdFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount == 1)
	{
		if (mVars(0).BaseType() == PDT_VARLIST)
		{
			return OuterProd(rCB, rVar, *mVars(0).GetVarListPtr(), iLine, iPos);
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
}

//////////////////////////////////////////////////////////////////////
/// Product of  List

bool  OuterProd(CCLUCodeBase& rCB, CCodeVar& rVar, TVarList& rList, int iLine, int iPos)
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

				if (!OuterProd(rCB, rRetList[i], rSubList, iLine, iPos))
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
			if (!rCB.OpWedge(rVar, rList(i), ValVar, iLine, iPos))
			{
				return false;
			}

			rVar = ValVar;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Inner Product FUNCTION

bool  InnerProdFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount == 1)
	{
		if (mVars(0).BaseType() == PDT_VARLIST)
		{
			return InnerProd(rCB, rVar, *mVars(0).GetVarListPtr(), iLine, iPos);
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
}

//////////////////////////////////////////////////////////////////////
/// Inner Product of  List

bool  InnerProd(CCLUCodeBase& rCB, CCodeVar& rVar, TVarList& rList, int iLine, int iPos)
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

				if (!InnerProd(rCB, rRetList[i], rSubList, iLine, iPos))
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
			if (!rCB.OpInnerProd(rVar, rList(i), ValVar, iLine, iPos))
			{
				return false;
			}

			rVar = ValVar;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// The Grade List FUNCTION
/// Returns list of grades contained in multivector

bool  GradeListFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount == 1)
	{
		TCVScalar dVal;

		if (!rVar.New(PDT_VARLIST, "Constant"))
		{
			rCB.GetErrorList().OutOfMemory(iLine, iPos);
			return false;
		}

		TVarList& rList = *rVar.GetVarListPtr();

		if (mVars(0).CastToScalar(dVal))
		{
			rList.Set(1);
			rList[0] = 0;
			return true;
		}
		else if (mVars(0).BaseType() != PDT_MULTIV)
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		TMultiV& vA = *mVars(0).GetMultiVPtr();
		Mem<uint> mGList;

		GradeList(vA, mGList, rCB.GetSensitivity());
		uint uCount = uint(mGList.Count());

		if (!rList.Set(uCount))
		{
			rCB.GetErrorList().OutOfMemory(iLine, iPos);
			return false;
		}

		for (uint u = 0; u < uCount; u++)
		{
			rList[u] = (TCVCounter) mGList[u];
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
/// Analyze a Multivector and return information about it.
///
/// params: 1. Multivector to be analyzed
///			2. (optional) boolean entry: if 1 draw mv, otherwise only analyze (default)
/// return: a list containing information about the mv.

bool  AnalyzeMVFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount   = int(mVars.Count());
	TCVCounter iVal = 0;

	if ((iVarCount < 1) || (iVarCount > 2))
	{
		int piVal[] = { 1, 2 };
		rCB.GetErrorList().WrongNoOfParams(piVal, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_MULTIV)
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if ((iVarCount == 2) && !mVars(1).CastToCounter(iVal))
	{
		rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}

	bool bAnalyzeOnly = (iVal ? false : true);
	TMultiV& rA       = *mVars(0).GetMultiVPtr();

	rVar.New(PDT_VARLIST);
	TVarList& rList = *rVar.GetVarListPtr();

	// Analyze Multivector
	if (!rCB.GetFilter()->DrawMV(rA, bAnalyzeOnly))
	{
		// Could not be analyzed
		rList.Set(1);
		rList[0] = "unknown";
		return true;
	}

	CMVInfo<TCVScalar> Info;
	rCB.GetFilter()->GetMVInfo(Info);
	EMVType mvType = Info.m_eType;

/*
        GA_NONE, GA_SCALAR,
        GA_POINT, GA_LINE, GA_PLANE, GA_SPACE,
        GA_POINTPAIR, GA_CIRCLE, GA_SPHERE,
        GA_DIR1D, GA_DIR2D, GA_DIR3D,
        GA_ROTOR, GA_MOTOR, GA_TWIST, GA_TRANSLATOR,
        GA_ELLIPSE, GA_HYPERBOLA, GA_PARLINEPAIR,
        GA_LINEPAIR, GA_POINTSET
*/

	if (mvType == GA_NONE)
	{
		rList.Set(1);
		rList[0] = "unknown";
	}
	else if (mvType == GA_SCALAR)
	{
		rList.Set(2);
		rList[0] = "scalar";

		VAR_LIST_PAIR(rList[1], "value", Info.m_mVal[0]);
	}
	else if (mvType == GA_POINT)
	{
		rList.Set(2);
		rList[0] = "point";

		VAR_LIST_PAIR(rList[1], "pos", rCB.GetE3GABase().Vec(Info.m_mVex[0].Data()));
	}
	else if (mvType == GA_POINTSET)
	{
		int i, iCount = int(Info.m_mVex.Count());

		rList.Set(2);
		rList[0] = "pointset";

		rList[1].New(PDT_VARLIST);
		TVarList& rPointSet = *rList[1].GetVarListPtr();
		rPointSet.Set(2);
		rPointSet[0] = "pos";

		rPointSet[1].New(PDT_VARLIST);
		TVarList& rVex = *rPointSet[1].GetVarListPtr();
		rVex.Set(iCount);
		for (i = 0; i < iCount; i++)
		{
			rVex[i] = rCB.GetE3GABase().Vec(Info.m_mVex[i].Data());
		}
	}
	else if (mvType == GA_LINE)
	{
		rList.Set(4);
		rList[0] = "line";

		VAR_LIST_PAIR(rList[1], "mag", Info.m_mVal[0]);
		VAR_LIST_PAIR(rList[2], "pos", rCB.GetE3GABase().Vec(Info.m_mVex[0].Data()));
		VAR_LIST_PAIR(rList[3], "dir", rCB.GetE3GABase().Vec(Info.m_mVex[1].Data()));
	}
	else if (mvType == GA_INF_POINTPAIR)
	{
		rList.Set(4);
		rList[0] = "inf_point_pair";

		VAR_LIST_PAIR(rList[1], "mag", Info.m_mVal[0]);
		VAR_LIST_PAIR(rList[2], "pos", rCB.GetE3GABase().Vec(Info.m_mVex[0].Data()));
		VAR_LIST_PAIR(rList[3], "dir", rCB.GetE3GABase().Vec(Info.m_mVex[1].Data()));
	}
	else if (mvType == GA_PLANE)
	{
		rList.Set(5);
		rList[0] = "plane";

		VAR_LIST_PAIR(rList[1], "mag", Info.m_mVal[0]);
		VAR_LIST_PAIR(rList[2], "pos", rCB.GetE3GABase().Vec(Info.m_mVex[0].Data()));
		VAR_LIST_PAIR(rList[3], "dir1", rCB.GetE3GABase().Vec(Info.m_mVex[1].Data()));
		VAR_LIST_PAIR(rList[4], "dir2", rCB.GetE3GABase().Vec(Info.m_mVex[2].Data()));
	}
	else if (mvType == GA_INF_LINE)
	{
		rList.Set(5);
		rList[0] = "inf_line";

		VAR_LIST_PAIR(rList[1], "mag", Info.m_mVal[0]);
		VAR_LIST_PAIR(rList[2], "pos", rCB.GetE3GABase().Vec(Info.m_mVex[0].Data()));
		VAR_LIST_PAIR(rList[3], "dir1", rCB.GetE3GABase().Vec(Info.m_mVex[1].Data()));
		VAR_LIST_PAIR(rList[4], "dir2", rCB.GetE3GABase().Vec(Info.m_mVex[2].Data()));
	}
	else if (mvType == GA_SPACE)
	{
		rList.Set(2);
		rList[0] = "space";

		VAR_LIST_PAIR(rList[1], "value", Info.m_mVal[0]);
	}
	else if (mvType == GA_POINTPAIR)
	{
		TCVScalar fMag;
		TMultiV vA, vD;

		vA   = rCB.GetE3GABase().Vec(Info.m_mVex[0].Data());
		vD   = rCB.GetE3GABase().Vec(Info.m_mVex[1].Data());
		fMag = Info.m_mVal[0];

		rList.Set(4);
		rList[0] = "point_pair";

		VAR_LIST_PAIR(rList[1], "mag", fMag);
		VAR_LIST_PAIR(rList[2], "pos1", vA + (Mag(fMag)^vD));
		VAR_LIST_PAIR(rList[3], "pos2", vA - (Mag(fMag)^vD));
	}
	else if (mvType == GA_DIR1D)
	{
		rList.Set(3);
		rList[0] = "dir_1d";

		VAR_LIST_PAIR(rList[1], "mag", Info.m_mVal[0]);
		VAR_LIST_PAIR(rList[2], "dir", rCB.GetE3GABase().Vec(Info.m_mVex[0].Data()));
	}
	else if (mvType == GA_DIR2D)
	{
		rList.Set(4);
		rList[0] = "dir_2d";

		VAR_LIST_PAIR(rList[1], "mag", Info.m_mVal[0]);
		VAR_LIST_PAIR(rList[2], "dir1", rCB.GetE3GABase().Vec(Info.m_mVex[0].Data()));
		VAR_LIST_PAIR(rList[3], "dir2", rCB.GetE3GABase().Vec(Info.m_mVex[1].Data()));
	}
	else if (mvType == GA_DIR3D)
	{
		rList.Set(5);
		rList[0] = "dir_3d";

		VAR_LIST_PAIR(rList[1], "mag", Info.m_mVal[0]);
		VAR_LIST_PAIR(rList[2], "dir1", rCB.GetE3GABase().Vec(Info.m_mVex[0].Data()));
		VAR_LIST_PAIR(rList[3], "dir2", rCB.GetE3GABase().Vec(Info.m_mVex[1].Data()));
		VAR_LIST_PAIR(rList[4], "dir3", rCB.GetE3GABase().Vec(Info.m_mVex[2].Data()));
	}
	else if (mvType == GA_CIRCLE)
	{
		TCVScalar fMag;
		TMultiV vC, vN, vU;
		MemObj<TMultiV> mvList;

		vC = rCB.GetE3GABase().Vec(Info.m_mVex[0].Data());
		vN = rCB.GetE3GABase().Vec(Info.m_mVex[1].Data());

		fMag = Info.m_mVal[0];

		vU = vN & rCB.GetE3GABase().vI;
		if (!FactorBlade<TCVScalar>(vU, mvList, true, 0, 0, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError(
					"\nUnfortunately there still seems\n"
					"to be a bug in the analysis of\n"
					"multivectors in conformal space.\n"
					"Apparently a multivector was \n"
					"incorrectly analyzed as a circle.\n\n"
					"Please send me an email with the\n"
					"script where the error occured.\n"
					"I will try to fix this problem\n"
					"as soon as possible.\n", iLine, iPos);
			return false;
		}

		rList.Set(5);
		rList[0] = "circle";

		VAR_LIST_PAIR(rList[1], "radius", Info.m_mVal[0]);
		VAR_LIST_PAIR(rList[2], "pos", vC);
		VAR_LIST_PAIR(rList[3], "dir1", mvList[0]);
		VAR_LIST_PAIR(rList[4], "dir2", mvList[1]);
	}
	else if (mvType == GA_SPHERE)
	{
		rList.Set(3);
		rList[0] = "sphere";

		VAR_LIST_PAIR(rList[1], "radius", Info.m_mVal[0]);
		VAR_LIST_PAIR(rList[2], "pos", rCB.GetE3GABase().Vec(Info.m_mVex[0].Data()));
	}
	else if (mvType == GA_ELLIPSE)
	{
		rList.Set(4);
		rList[0] = "ellipse";

		VAR_LIST_PAIR(rList[1], "pos", rCB.GetE3GABase().Vec(Info.m_mVex[2].Data()));
		VAR_LIST_PAIR(rList[2], "dir1", rCB.GetE3GABase().Vec(Info.m_mVex[0].Data()));
		VAR_LIST_PAIR(rList[3], "dir2", rCB.GetE3GABase().Vec(Info.m_mVex[1].Data()));
	}
	else if (mvType == GA_HYPERBOLA)
	{
		rList.Set(4);
		rList[0] = "hyperbola";

		VAR_LIST_PAIR(rList[1], "pos", rCB.GetE3GABase().Vec(Info.m_mVex[2].Data()));
		VAR_LIST_PAIR(rList[2], "dir1", rCB.GetE3GABase().Vec(Info.m_mVex[0].Data()));
		VAR_LIST_PAIR(rList[3], "dir2", rCB.GetE3GABase().Vec(Info.m_mVex[1].Data()));
	}
	else if (mvType == GA_LINEPAIR)
	{
		rList.Set(4);
		rList[0] = "line_pair";

		//VAR_LIST_PAIR(rList[1], "mag", Info.m_mVal[0]);
		VAR_LIST_PAIR(rList[1], "pos", rCB.GetE3GABase().Vec(Info.m_mVex[0].Data()));
		VAR_LIST_PAIR(rList[2], "dir1", rCB.GetE3GABase().Vec(Info.m_mVex[1].Data()));
		VAR_LIST_PAIR(rList[3], "dir2", rCB.GetE3GABase().Vec(Info.m_mVex[2].Data()));
	}
	else if (mvType == GA_PARLINEPAIR)
	{
		TCVScalar fMag;
		TMultiV vA, vD;

		vA   = rCB.GetE3GABase().Vec(Info.m_mVex[2].Data());
		vD   = rCB.GetE3GABase().Vec(Info.m_mVex[0].Data());
		fMag = Info.m_mVal[0];

		rList.Set(5);
		rList[0] = "line_pair_parallel";

		VAR_LIST_PAIR(rList[1], "mag", fMag);
		VAR_LIST_PAIR(rList[2], "pos1", vA + (Mag(fMag) ^ vD));
		VAR_LIST_PAIR(rList[3], "pos2", vA - (Mag(fMag) ^ vD));
		VAR_LIST_PAIR(rList[4], "dir", rCB.GetE3GABase().Vec(Info.m_mVex[1].Data()));
	}
	else if (mvType == GA_TRANSLATOR)
	{
		TMultiV vA;

		vA = rCB.GetE3GABase().Vec(Info.m_mVex[0].Data());

		rList.Set(2);
		rList[0] = "translator";

		VAR_LIST_PAIR(rList[1], "dir", vA);
	}
	else if (mvType == GA_MOTOR)
	{
		TCVScalar fMag;
		TMultiV vA, vD;

		vA   = rCB.GetE3GABase().Vec(Info.m_mVex[0].Data());
		vD   = rCB.GetE3GABase().Vec(Info.m_mVex[1].Data());
		fMag = Info.m_mVal[0];

		rList.Set(4);
		rList[0] = "motor";

		VAR_LIST_PAIR(rList[1], "angle", fMag);
		VAR_LIST_PAIR(rList[2], "axis", vD);
		VAR_LIST_PAIR(rList[3], "offset", vA);
	}
	else if (mvType == GA_TWIST)
	{
		TCVScalar fMag;
		TMultiV vTp, vTr, vR;

		vTp  = rCB.GetE3GABase().Vec(Info.m_mVex[0].Data());
		vTr  = rCB.GetE3GABase().Vec(Info.m_mVex[1].Data());
		vR   = rCB.GetE3GABase().Vec(Info.m_mVex[2].Data());
		fMag = Info.m_mVal[0];

		rList.Set(5);
		rList[0] = "twist";

		VAR_LIST_PAIR(rList[1], "angle", fMag);
		VAR_LIST_PAIR(rList[2], "axis", vR);
		VAR_LIST_PAIR(rList[3], "offset", vTp);
		VAR_LIST_PAIR(rList[4], "trans", vTr);
	}
	else if (mvType == GA_ROTOR)
	{
		TCVScalar fMag;
		TMultiV vA, vD;

		vA   = rCB.GetE3GABase().Vec(Info.m_mVex[0].Data());
		fMag = Info.m_mVal[0];

		rList.Set(3);
		rList[0] = "rotor";

		VAR_LIST_PAIR(rList[1], "angle", fMag);
		VAR_LIST_PAIR(rList[2], "axis", vA);
	}
	else
	{
		rList.Set(1);
		rList[0] = "unknown";
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Generate a Mask as needed for GetMVProductMatrix from a multivector.
///
/// params: Multivector whose non-zero components give elements of mask.
/// return: a list containing the mask

bool  GenerateMaskFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount != 1)
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_MULTIV)
	{
		rCB.GetErrorList().InvalidType(mVars(0), iLine, iPos);
		return false;
	}

	TMultiV& vA = *mVars(0).GetMultiVPtr();

	rVar.New(PDT_VARLIST, "Mask");
	TVarList& IdxList = *rVar.GetVarListPtr();

	vA.TinyToZero();
	int iIdx, iMaskIdx, iGADim = vA.GetGADim();
	IdxList.Set(iGADim);

	for (iIdx = 0, iMaskIdx = 0; iIdx < iGADim; iIdx++)
	{
		if (vA[iIdx] != 0)
		{
			IdxList[iIdx] = ++iMaskIdx;
		}
		else
		{
			IdxList[iIdx] = TCVCounter(0);
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Get indices of those components of a multivector that are
/// non-zero.
///
/// params: multivector
/// return: list of indices

bool  GetMVIndicesFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount != 1)
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_MULTIV)
	{
		rCB.GetErrorList().InvalidType(mVars(0), iLine, iPos);
		return false;
	}

	TMultiV& vA = *mVars(0).GetMultiVPtr();

	rVar.New(PDT_VARLIST, "IndexList");
	TVarList& IdxList = *rVar.GetVarListPtr();

	vA.TinyToZero();
	int iIdx, iGADim = vA.GetGADim();

	for (iIdx = 0; iIdx < iGADim; iIdx++)
	{
		if (vA[iIdx] != 0)
		{
			IdxList.Add(1);
			IdxList.Last() = iIdx + 1;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Factorize a multivector blade
// TODO: Update docu for FactorizeBlade. Now also for null blades.

bool  FactorBladeFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList* pmVars = rPars.GetVarListPtr();

	int iVarCount = int(pmVars->Count());

	if (iVarCount == 1)
	{
		MemObj<TMultiV > memResultList, memBasisList;
		TMultiV vA;

		if ((*pmVars)(0).BaseType() != PDT_MULTIV)
		{
			rCB.GetErrorList().InvalidType((*pmVars)(0), iLine, iPos);
			return false;
		}

		vA = *((TMultiV*) (*pmVars)(0).GetMultiVPtr());

		// If multivector is element of conformal space
		// then use special FactorBlade function that can
		// also factorize null blades.
		if (vA.BasePtr()->BaseID() == ID_CONFGA)
		{
			FactorBlade<TCVScalar>(vA, rCB.GetN3GABase().vEm, memResultList, true, 0, 0, rCB.GetSensitivity());
		}
		else
		{
			FactorBlade<TCVScalar>(vA, memResultList, true, 0, 0, rCB.GetSensitivity());
		}

		//if (iVarCount == 1)
		/*else
		{
		        if ((*pmVars)(1).BaseType() == PDT_VARLIST)
		        { // get the MultiVectors from PDT_VARLIST
		                TVarList *pmBasisVars = (TVarList *) ((*pmVars)(1).Val());
		                int iBasisVarCount = pmBasisVars->Count();
		                for (int j=0;j<iBasisVarCount;j++)
		                {
		                        if ((*pmBasisVars)[j].BaseType() != PDT_MULTIV)
		                        {
		                                rCB.GetErrorList().InvalidType((*pmBasisVars)[j], iLine, iPos);
		                                return false;
		                        }
		                        memBasisList++;
		                        memBasisList.Last()= *((TMultiV*) (*pmBasisVars)[j].Val());
		                }
		                FactorBlade(vA,memResultList,memBasisList);
		        }
		        else
		        {
		                if (iVarCount == 2)
		                {
		                        if ((*pmVars)(1).BaseType()!=PDT_INT)
		                        {
		                                rCB.GetErrorList().InvalidType((*pmVars)(1), iLine, iPos);
		                                return false;
		                        }
		                        int iNorm=*((int *) (*pmVars)(1).Val());
		                        bool bNorm = ( iNorm>0 );
		                        FactorBlade(vA,memResultList,bNorm);
		                }
		                if (iVarCount == 3)
		                {
		                }
		        }
		}*/

		if (!rVar.New(PDT_VARLIST, "Constant"))
		{
			rCB.GetErrorList().Internal(iLine, iPos);
			return false;
		}

		/*
		if (iVarCount == 1) // only a multivector?
		{

		  TCVScalar pfVec[4];
		  int i;

		        for(i=0;i<iVarCount;i++)
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

		          rVar = rCB.GetN3GABase().Sphere(pfVec[0], pfVec[1], pfVec[2], pfVec[3]);
		*/
		for (unsigned int l = 0; l < memResultList.Count(); l++)
		{
			if (!rVar.GetVarListPtr()->Add(1))
			{
				rCB.GetErrorList().OutOfMemory(iLine, iPos);
				return false;
			}
			rVar.GetVarListPtr()->Last() = memResultList[l];
		}
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	return true;
}
