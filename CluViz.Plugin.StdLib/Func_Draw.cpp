////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_Draw.cpp
//
// summary:   Implements the function draw class
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
#include "Func_Draw.h"

//////////////////////////////////////////////////////////////////////
/// The Plot FUNCTION
/// first parameter is function, or list of two functions [ func, col_func ]
/// second parameter is list containing:
///		free variable, start, stop, steps.

bool PlotFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());
	TCVScalar fMin, fMax, fStep;
	TCVCounter iSteps, i;
	bool bError       = false;
	bool bIsFuncList  = false;
	bool bUseColList  = false;
	bool bUseNormList = false;
	bool bUseTexList  = false;
	int iListSize     = 0;
	int iColListPos   = -1;
	int iNormListPos  = -1;
	int iTexListPos   = -1;

	//TScene refScene;
	//rVar = refScene;
	//return true;

	CCodeElementList* pCodeList = mVars.GetCodeList();

	if (!pCodeList)
	{
		rCB.GetErrorList().Internal(iLine, iPos);
		return false;
	}

	if (iVarCount == 2)
	{
		if (mVars(0).BaseType() != PDT_MULTIV)
		{
			if (mVars(0).BaseType() != PDT_VARLIST)
			{
				rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
				return false;
			}
			iListSize = int(mVars(0).GetVarListPtr()->Count());
			if (iListSize > 1)
			{
				bUseColList = true;
			}
			bIsFuncList = true;
		}

		if (mVars(1).BaseType() != PDT_VARLIST)
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		// List containing free variable, plot range and step.
		TVarList& mFVList = *mVars(1).GetVarListPtr();

		if (mFVList.Count() != 4)
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		// Free variable to be used
		CCodeVar& FreeVar = rCB.GetVar(mFVList(0).Name().c_str());

		if (FreeVar.BaseType() == PDT_NOTYPE)
		{
			rCB.GetErrorList().GeneralError("No free variable given.", iLine, iPos);
			return false;
		}

		if (!mFVList(1).CastToScalar(fMin, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Minimum value is not a scalar.", iLine, iPos);
			return false;
		}

		if (!mFVList(2).CastToScalar(fMax, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Maximum value is not a scalar.", iLine, iPos);
			return false;
		}

		if (!mFVList(3).CastToCounter(iSteps))
		{
			rCB.GetErrorList().GeneralError("The number of steps must be an integer value.", iLine, iPos);
			return false;
		}

		if ((fMin >= fMax) || (iSteps < 1))
		{
			rCB.GetErrorList().GeneralError("Minimum value has to be smaller than maximum value.", iLine, iPos);
			return false;
		}

		fStep = (fMax - fMin) / TCVScalar(iSteps);

		SCodeData sData;
		sData.Set(iLine, iPos, 0);

		/*
		// Initialize result variable
		if (!rVar.New(PDT_VEXLIST, "Constant"))
		{
		rCB.GetErrorList().OutOfMemory(iLine, iPos);
		return false;
		}

		TVexList& rVexList = *rVar.GetVexListPtr();
		rVexList.SetMode((GLenum)rCB.GetPlotMode());
		*/
		// Initialize Point List
		CCodeVar Par;
		Par.New(PDT_VARLIST);

		TVarList& rParList = *Par.GetVarListPtr();

		rParList.Set(3);

		// Set DoDraw flag to false, i.e. do not draw directly.
		rParList[2] = TCVCounter(0);

		rParList[0].New(PDT_VARLIST);
		TVarList& rPointList = *rParList[0].GetVarListPtr();
		rPointList.Set(iSteps + 1);

		TVarList* pColList = 0;
		if (bUseColList)
		{
			rParList[1].New(PDT_VARLIST);
			pColList = rParList[1].GetVarListPtr();
			pColList->Set(iSteps + 1);
		}

		// Initialize free variable
		FreeVar = fMin;

		// Get pointer to free variable
		TCVScalar& rX = *FreeVar.GetScalarPtr();

		// Result variable
		CCodeVar* pR, varR;
		TMultiV vR;
		char pcText[200];

		rCB.LockStack();
		for (i = 0; i <= iSteps; i++, rX += fStep)
		{
			if (!(*pCodeList)[0]->Apply(&rCB, &sData))
			{
				bError = true;
				break;
			}

			if (!rCB.Pop(pR))
			{
				sprintf_s(pcText, "Function did not return a value at step %d.", i + 1);
				rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
				bError = true;
				break;
			}

			varR = pR->DereferenceVarPtr(true);

			if (!bIsFuncList)
			{
				if (varR.BaseType() != PDT_MULTIV)
				{
					sprintf_s(pcText, "Value of function at step %d is not a multivector.", i + 1);
					rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
					bError = true;
					break;
				}

				rPointList[i] = *(varR.GetMultiVPtr());
			}
			else
			{
				if (varR.BaseType() != PDT_VARLIST)
				{
					sprintf_s(pcText, "No list returned at step %d.", i + 1);
					rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
					bError = true;
					break;
				}

				TVarList& rResList = *(varR.GetVarListPtr());
				int iSize          = int(rResList.Count());
				if (iSize != iListSize)
				{
					sprintf_s(pcText, "No value returned at step %d.", i + 1);
					rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
					bError = true;
					break;
				}

				if (rResList[0].BaseType() != PDT_MULTIV)
				{
					sprintf_s(pcText, "Value of function at step %d is not a multivector.", i + 1);
					rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
					bError = true;
					break;
				}

				rPointList[i] = rResList[0];

				if (bUseColList)
				{
					if (rResList[1].BaseType() != PDT_COLOR)
					{
						sprintf_s(pcText, "Invalid color value at step %d.", i + 1);
						rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
						bError = true;
						break;
					}

					(*pColList)[i] = rResList[1];
				}
			}
		}

		while (rCB.Pop(pR))
			;		// Empty Stack

		rCB.UnlockStack();

		if (bError)
		{
			return false;
		}

		CMVInfo<float> Info;

		// Only analyze multivector
		rCB.GetFilter()->DrawMV(*rPointList[0].GetMultiVPtr(), true);

		// Get MVInfo
		Info = rCB.GetFilter()->GetMVInfo();
		bool bRes         = false;
		bool bSurfacePlot = true;

		if ((Info.m_eType == GA_POINT) ||
		    (Info.m_eType == GA_SCALAR))
		{
			//TScene refScene;
			//rVar = refScene;
			//bRes = true;
			bRes         = DrawPointListFunc(rCB, rVar, Par, iLine, iPos);
			bSurfacePlot = false;
		}
		else if (Info.m_eType == GA_LINE)
		{
			bRes = DrawLineSurfaceFunc(rCB, rVar, Par, iLine, iPos);
		}
		else if (Info.m_eType == GA_CIRCLE)
		{
			bRes = DrawCircleSurfaceFunc(rCB, rVar, Par, iLine, iPos);
		}
		else
		{
			rCB.GetErrorList().GeneralError("Unable to plot function of multivectors of given type.", iLine, iPos);
			return false;
		}

		if (!bRes)
		{
			return false;
		}

		if (rVar.BaseType() == PDT_SCENE)
		{
			TScene& rScene           = *rVar.GetScenePtr();
			COGLVertexList* pVexList = dynamic_cast< COGLVertexList* >((COGLBaseElement*) rScene);

			if (pVexList)
			{
				if (bSurfacePlot)
				{
					if (rCB.GetPlotMode() == PLOT_POINTS)
					{
						pVexList->SetPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
						pVexList->EnablePolygonMode(true);
					}
					else if (rCB.GetPlotMode() == PLOT_LINES)
					{
						pVexList->SetPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
						pVexList->EnablePolygonMode(true);
					}
					else if (rCB.GetPlotMode() == PLOT_FILL)
					{
						pVexList->SetPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
						pVexList->EnablePolygonMode(true);
					}
				}
				else
				{
					if (rCB.GetPlotMode() == PLOT_POINTS)
					{
						pVexList->SetMode(GL_POINTS);
					}
				}
			}
		}

		return true;
	}
	///////////////////////////////////////////////////////////////////////
	//// Surface Plot
	else if (iVarCount >= 3)
	{
		TCVScalar fNormScale = 0;

		if (mVars(0).BaseType() != PDT_MULTIV)
		{
			if (mVars(0).BaseType() != PDT_VARLIST)
			{
				rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
				return false;
			}

			TVarList& rList = *mVars(0).GetVarListPtr();
			iListSize = int(rList.Count());

			if (iListSize == 0)
			{
				rCB.GetErrorList().GeneralError("No function given.", iLine, iPos);
				return false;
			}

			if (iListSize >= 1)
			{
				if (rList[0].BaseType() != PDT_MULTIV)
				{
					rCB.GetErrorList().GeneralError("First element in function list must be multivector valued.", iLine, iPos);
					return false;
				}
			}

			if (iListSize >= 2)
			{
				if (rList[1].BaseType() == PDT_MULTIV)
				{
					bUseNormList = true;
					iNormListPos = 1;
				}
				else if (rList[1].BaseType() == PDT_COLOR)
				{
					bUseColList = true;
					iColListPos = 1;
				}
				// Counter is a dummy for 'no function given'
				else if (rList[1].BaseType() != PDT_COUNTER)
				{
					rCB.GetErrorList().GeneralError(
							"Expect second element in function list to be either color or multivector (normal).",
							iLine, iPos);
					return false;
				}
			}

			if (iListSize >= 3)
			{
				if (rList[2].BaseType() == PDT_MULTIV)
				{
					if (bUseNormList)
					{
						rCB.GetErrorList().GeneralError(
								"Invalid third element in function list. Surface normals already specified as second element of function list.",
								iLine, iPos);
						return false;
					}
					bUseNormList = true;
					iNormListPos = 2;
				}
				else if (rList[2].BaseType() == PDT_COLOR)
				{
					if (bUseColList)
					{
						rCB.GetErrorList().GeneralError(
								"Invalid third element in function list. Color already specified as second element of function list.",
								iLine, iPos);
						return false;
					}
					bUseColList = true;
					iColListPos = 2;
				}
				else if (rList[2].BaseType() != PDT_COUNTER)
				{
					rCB.GetErrorList().GeneralError(
							"Expect third element in function list to be either color or multivector (normal).",
							iLine, iPos);
					return false;
				}
			}

			if (iListSize >= 4)
			{
				if (rList[3].BaseType() == PDT_MULTIV)
				{
					bUseTexList = true;
					iTexListPos = 3;
				}
				else
				{
					rCB.GetErrorList().GeneralError(
							"Expect fourth element in function list to be a multivector (texture coordinate).",
							iLine, iPos);
					return false;
				}
			}

			bIsFuncList = true;
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

		if (iVarCount == 4)
		{
			if (!mVars(3).CastToScalar(fNormScale, rCB.GetSensitivity()))
			{
				rCB.GetErrorList().InvalidParType(mVars(3), 4, iLine, iPos);
				return false;
			}
		}

		// List containing first free variable, plot range and step.
		TVarList& mFV1List = *mVars(1).GetVarListPtr();

		// List containing second free variable, plot range and step.
		TVarList& mFV2List = *mVars(2).GetVarListPtr();

		if (mFV1List.Count() != 4)
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		if (mFV2List.Count() != 4)
		{
			rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}

		TCVScalar fMin1, fMax1, fStep1;
		TCVScalar fMin2, fMax2, fStep2;
		TCVCounter iSteps1, iSteps2, i, j;

		// Free variable to be used
		CCodeVar& FreeVar1 = rCB.GetVar(mFV1List(0).Name().c_str());
		CCodeVar& FreeVar2 = rCB.GetVar(mFV2List(0).Name().c_str());

		if (FreeVar1.BaseType() == PDT_NOTYPE)
		{
			rCB.GetErrorList().GeneralError("No free variable given for first dimension.", iLine, iPos);
			return false;
		}

		if (!mFV1List(1).CastToScalar(fMin1, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Minimum value is not a scalar.", iLine, iPos);
			return false;
		}

		if (!mFV1List(2).CastToScalar(fMax1, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Maximum value is not a scalar.", iLine, iPos);
			return false;
		}

		if (!mFV1List(3).CastToCounter(iSteps1))
		{
			rCB.GetErrorList().GeneralError("The number of steps must be an integer value.", iLine, iPos);
			return false;
		}

		if ((fMin1 >= fMax1) || (iSteps1 < 1))
		{
			rCB.GetErrorList().GeneralError("Minimum value has to be smaller than maximum value.", iLine, iPos);
			return false;
		}

		fStep1 = (fMax1 - fMin1) / TCVScalar(iSteps1);

		if (FreeVar2.BaseType() == PDT_NOTYPE)
		{
			rCB.GetErrorList().GeneralError("No free variable given for second dimension.", iLine, iPos);
			return false;
		}

		if (!mFV2List(1).CastToScalar(fMin2, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Minimum value is not a scalar.", iLine, iPos);
			return false;
		}

		if (!mFV2List(2).CastToScalar(fMax2, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Maximum value is not a scalar.", iLine, iPos);
			return false;
		}

		if (!mFV2List(3).CastToCounter(iSteps2))
		{
			rCB.GetErrorList().GeneralError("The number of steps must be an integer value.", iLine, iPos);
			return false;
		}

		if ((fMin2 >= fMax2) || (iSteps2 < 1))
		{
			rCB.GetErrorList().GeneralError("Minimum value has to be smaller than maximum value.", iLine, iPos);
			return false;
		}

		fStep2 = (fMax2 - fMin2) / TCVScalar(iSteps2);

		SCodeData sData;
		sData.Set(iLine, iPos, 0);

		/*
		// Initialize result variable
		if (!rVar.New(PDT_VEXLIST, "Constant"))
		{
		rCB.GetErrorList().OutOfMemory(iLine, iPos);
		return false;
		}

		TVexList& rVexList = *rVar.GetVexListPtr();
		rVexList.SetMode((GLenum)rCB.GetPlotMode());
		*/
		// Initialize Point List
		CCodeVar Par;
		Par.New(PDT_VARLIST);

		TVarList& rParList = *Par.GetVarListPtr();

		rParList.Set(8);

		// Set DoDraw flag to false
		rParList[4] = TCVCounter(0);

		// Set Norm Scale
		rParList[6] = fNormScale;

		rParList[0] = TCVCounter(iSteps1 + 1);
		rParList[1] = TCVCounter(iSteps2 + 1);
		rParList[2].New(PDT_VARLIST);
		TVarList& rPointList = *rParList[2].GetVarListPtr();
		rPointList.Set((iSteps1 + 1) * (iSteps2 + 1));

		TVarList* pColList  = 0;
		TVarList* pNormList = 0;
		TVarList* pTexList  = 0;

		if (bUseColList)
		{
			rParList[3].New(PDT_VARLIST);
			pColList = rParList[3].GetVarListPtr();
			pColList->Set((iSteps1 + 1) * (iSteps2 + 1));
		}

		if (bUseNormList)
		{
			rParList[5].New(PDT_VARLIST);
			pNormList = rParList[5].GetVarListPtr();
			pNormList->Set((iSteps1 + 1) * (iSteps2 + 1));
		}

		if (bUseTexList)
		{
			rParList[7].New(PDT_VARLIST);
			pTexList = rParList[7].GetVarListPtr();
			pTexList->Set((iSteps1 + 1) * (iSteps2 + 1));
		}

		// Initialize free variable
		FreeVar1 = fMin1;
		FreeVar2 = fMin2;

		// Get pointer to free variable
		TCVScalar& rX1 = *FreeVar1.GetScalarPtr();
		TCVScalar& rX2 = *FreeVar2.GetScalarPtr();

		// Result variable
		CCodeVar* pR;
		TMultiV vR;
		int iPos = 0;
		char pcText[200];

		rCB.LockStack();
		for (j = 0; j <= iSteps2; j++, rX2 += fStep2)
		{
			rX1 = fMin1;
			for (i = 0; i <= iSteps1; i++, rX1 += fStep1, iPos++)
			{
				if (!(*pCodeList)[0]->Apply(&rCB, &sData))
				{
					bError = true;
					break;
				}

				if (!rCB.Pop(pR))
				{
					sprintf_s(pcText, "Function did not return a value at step %d.", i + 1);
					rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
					bError = true;
					break;
				}

				CCodeVar& rRes = pR->DereferenceVarPtr(true);

				if (!bIsFuncList)
				{
					if (rRes.BaseType() != PDT_MULTIV)
					{
						sprintf_s(pcText, "Value of function at step %d is not a multivector.", i + 1);
						rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
						bError = true;
						break;
					}

					rPointList[iPos] = *(rRes.GetMultiVPtr());
				}
				else
				{
					if (rRes.BaseType() != PDT_VARLIST)
					{
						sprintf_s(pcText, "No list returned at step %d.", i + 1);
						rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
						bError = true;
						break;
					}

					TVarList& rResList = *(rRes.GetVarListPtr());
					int iSize          = int(rResList.Count());
					if (iSize != iListSize)
					{
						sprintf_s(pcText, "No value returned at step %d.", i + 1);
						rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
						bError = true;
						break;
					}

					CCodeVar& rResMV = rResList[0].DereferenceVarPtr(true);
					if (rResMV.BaseType() != PDT_MULTIV)
					{
						sprintf_s(pcText, "Value of function at step %d is not a multivector.", i + 1);
						rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
						bError = true;
						break;
					}

					rPointList[iPos] = rResMV;

					if (bUseColList)
					{
						CCodeVar& rResCol = rResList[iColListPos].DereferenceVarPtr(true);
						if (rResCol.BaseType() != PDT_COLOR)
						{
							sprintf_s(pcText, "Invalid color value at step %d.", i + 1);
							rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
							bError = true;
							break;
						}

						(*pColList)[iPos] = rResCol;
					}

					if (bUseNormList)
					{
						CCodeVar& rResNorm = rResList[iNormListPos].DereferenceVarPtr(true);
						if (rResNorm.BaseType() != PDT_MULTIV)
						{
							sprintf_s(pcText, "Invalid normal at step %d.", i + 1);
							rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
							bError = true;
							break;
						}

						(*pNormList)[iPos] = rResNorm;
					}

					if (bUseTexList)
					{
						CCodeVar& rResNorm = rResList[iTexListPos].DereferenceVarPtr(true);
						if (rResNorm.BaseType() != PDT_MULTIV)
						{
							sprintf_s(pcText, "Invalid texture coordinate at step %d.", i + 1);
							rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
							bError = true;
							break;
						}

						(*pTexList)[iPos] = rResNorm;
					}
				}
			}
		}

		while (rCB.Pop(pR))
			;		// Empty Stack

		rCB.UnlockStack();

		if (bError)
		{
			return false;
		}

		bool bRes = DrawPointSurfaceFunc(rCB, rVar, Par, iLine, iPos);

		if (!bRes)
		{
			return false;
		}

		if (rVar.BaseType() == PDT_VARLIST)
		{
			TVarList& rList = *rVar.GetVarListPtr();

			TScene& rScene           = *rList(rList.Count() - 1).GetScenePtr();
			COGLVertexList* pVexList = dynamic_cast< COGLVertexList* >((COGLBaseElement*) rScene);

			if (pVexList)
			{
				if (rCB.GetPlotMode() == PLOT_POINTS)
				{
					pVexList->SetPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
					pVexList->EnablePolygonMode(true);
				}
				else if (rCB.GetPlotMode() == PLOT_LINES)
				{
					pVexList->SetPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					pVexList->EnablePolygonMode(true);
				}
				else if (rCB.GetPlotMode() == PLOT_FILL)
				{
					pVexList->SetPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					pVexList->EnablePolygonMode(true);
				}
			}
		}

		return true;
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(2, iLine, iPos);
		return false;
	}
}

//////////////////////////////////////////////////////////////////////
// Flush the current scene graph to screen

bool FlushDrawFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount != 0)
	{
		int piPar[] = { 0 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	rCB.GetCLUDrawBase()->FlushDraw();

	return true;
}

//////////////////////////////////////////////////////////////////////
// Draw multivector with given offset and scale
// expects two or three parameters:
// 1: Multivector to be drawn
// 2: offset vector
// 3: scale (optional)

bool OffsetFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	CMVInfo<float> MVInfo;
	TMultiV vA, vB;
	TCVScalar dScale = 0;

	if ((iVarCount < 2) || (iVarCount > 3))
	{
		int piIdx[] = { 2, 3 };
		rCB.GetErrorList().WrongNoOfParams(piIdx, 2, iLine, iPos);
		return false;
	}

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

	if (iVarCount == 3)
	{
		if (!mVars(2).CastToScalar(dScale, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}
	}

	vA = *mVars(0).GetMultiVPtr();

	if (!rCB.CastMVtoE3(*mVars(1).GetMultiVPtr(), vB))
	{
		CStrMem csText = mVars(1).GetMultiVPtr()->Str();
		rCB.GetErrorList().InvalidParVal(csText, 2, iLine, iPos);
		return false;
	}

	// Only Analyze Multivector
	rCB.GetFilter()->DrawMV(vA, true);
	MVInfo = rCB.GetFilter()->GetMVInfo();

	if (MVInfo.m_eType == GA_LINE)
	{
		int i;
		float fScale;
		COGLVertex xB, xD;

		// Offset vector
		for (i = 0; i < 3; i++)
		{
			xB[i] = (float) vB[i + 1];
		}

		xD = MVInfo.m_mVex[1].Data();	// direction of line

		// Scale
		if (dScale != 0)
		{
			fScale = 0.5f * (MVInfo.m_mVal[0] = (float) dScale);
		}
		else
		{
			fScale = 0.5f * MVInfo.m_mVal[0];
		}

		xB = xB + fScale * xD;

		MVInfo.m_mVex[0] = xB;
	}
	else if (MVInfo.m_eType == GA_PLANE)
	{
		int i;
		COGLVertex xB, xD;

		// Offset vector
		for (i = 0; i < 3; i++)
		{
			xB[i] = (float) vB[i + 1];
		}

		// Scale
		if (dScale != 0)
		{
			MVInfo.m_mVal[0] = (float) dScale;
		}

		MVInfo.m_mVex[0] = xB;
	}
	else
	{
		CStrMem csText = mVars(0).GetMultiVPtr()->Str();
		rCB.GetErrorList().InvalidParVal(csText, 1, iLine, iPos);
		return false;
	}

	switch (rCB.GetFilter()->GetLastMVID())
	{
	case ID_E3GA:
		rCB.GetFilter()->m_E3Filter.DrawMVInfo(MVInfo);
		break;

	case ID_PGA:
		rCB.GetFilter()->m_P3Filter.DrawMVInfo(MVInfo);
		break;

	case ID_CONFGA:
		rCB.GetFilter()->m_N3Filter.DrawMVInfo(MVInfo);
		break;

	default:
		rCB.GetErrorList().Internal(iLine, iPos);
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper function to create single Frame Grid

COGLBEReference _CreateFrameGrid(CCLUCodeBase& rCB, float fSegSize, int iSegCntX, int iSegCntY, int iIdxX, int iIdxY, int iIdxZ, const COGLColor& colGrid)
{
	unsigned uVexCnt = unsigned((iSegCntX + 1) * (iSegCntY + 1));

	COGLVertexList* pVexList     = new COGLVertexList();
	COGLVertexList::SData* pData = new COGLVertexList::SData[uVexCnt];

	pVexList->EnablePolygonCull(true);
	pVexList->EnablePolygonMode(true);
	pVexList->SetPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	unsigned uVexIdx = 0;

	float fSizeX = fSegSize * float(iSegCntX);
	float fSizeY = fSegSize * float(iSegCntY);

	float fHalfSizeX = fSizeX / 2.0f;
	float fHalfSizeY = fSizeY / 2.0f;

	float fY = -fHalfSizeY;
	for (int iY = 0; iY <= iSegCntY; ++iY, fY += fSegSize)
	{
		float fX = -fHalfSizeX;
		for (int iX = 0; iX <= iSegCntX; ++iX, fX += fSegSize, ++uVexIdx)
		{
			COGLVertexList::SData& xVex = pData[uVexIdx];
			xVex.xVex[iIdxX] = fX;
			xVex.xVex[iIdxY] = fY;
			xVex.xVex[iIdxZ] = 0;

			xVex.xTex[iIdxX] = float(iX) / float(iSegCntX);
			xVex.xTex[iIdxY] = float(iY) / float(iSegCntY);
			xVex.xTex[iIdxZ] = 0.0f;

			xVex.xNorm[iIdxX] = 0.0f;
			xVex.xNorm[iIdxY] = 0.0f;
			xVex.xNorm[iIdxZ] = 1.0f;

			xVex.xCol[0] = colGrid[0];
			xVex.xCol[1] = colGrid[1];
			xVex.xCol[2] = colGrid[2];
			xVex.xCol[3] = colGrid[3];
		}
	}

	unsigned uIdxListLen  = 4 * iSegCntX * iSegCntY;
	unsigned* puIndexList = new unsigned[uIdxListLen];
	unsigned* puIdx       = puIndexList;
	int iRowLen           = iSegCntX + 1;

	for (int iY = 0; iY < iSegCntY; ++iY)
	{
		int iRowOffset = iY * iRowLen;

		for (int iX = 0; iX < iSegCntX; ++iX)
		{
			*puIdx = iRowOffset + iX;
			++puIdx;

			*puIdx = iRowOffset + iX + 1;
			++puIdx;

			*puIdx = iRowOffset + iRowLen + iX + 1;
			++puIdx;

			*puIdx = iRowOffset + iRowLen + iX;
			++puIdx;
		}
	}

	pVexList->ApplyVertexData(GL_QUADS, uVexCnt, pData, true, true, true, true, false, false, false, 0);
	pVexList->SetIndexLists((const unsigned int** const) &puIndexList, &uIdxListLen, 1, 0);

	delete[] puIndexList;
	delete[] pData;

	return rCB.GetOGLDrawBase()->GetSceneRepository()->New(pVexList);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper function to create single Frame Plane

COGLBEReference _CreateFramePlane(CCLUCodeBase& rCB, float fSegSize, int iSegCntX, int iSegCntY, int iIdxX, int iIdxY, int iIdxZ, const COGLColor& colBoxSurface)
{
	COGLVertexList* pVexList     = new COGLVertexList();
	COGLVertexList::SData* pData = new COGLVertexList::SData[4];

	pVexList->EnablePolygonCull(true);
	pVexList->EnablePolygonMode(false);
	pVexList->SetPolygonOffsetFill(1.0f, 1.0f);

	unsigned uVexIdx = 0;

	float fSizeX = fSegSize * float(iSegCntX);
	float fSizeY = fSegSize * float(iSegCntY);

	float fHalfSizeX = fSizeX / 2.0f;
	float fHalfSizeY = fSizeY / 2.0f;

	float fY = -fHalfSizeY;
	for (int iY = 0; iY < 2; ++iY, fY += fSizeY)
	{
		float fX = -fHalfSizeX;
		for (int iX = 0; iX < 2; ++iX, fX += fSizeX, ++uVexIdx)
		{
			COGLVertexList::SData& xVex = pData[uVexIdx];
			xVex.xVex[iIdxX] = fX;
			xVex.xVex[iIdxY] = fY;
			xVex.xVex[iIdxZ] = 0;

			xVex.xTex[iIdxX] = float(iX);
			xVex.xTex[iIdxY] = float(iY);
			xVex.xTex[iIdxZ] = 0.0f;

			xVex.xNorm[iIdxX] = 0.0f;
			xVex.xNorm[iIdxY] = 0.0f;
			xVex.xNorm[iIdxZ] = 1.0f;

			xVex.xCol[0] = colBoxSurface[0];
			xVex.xCol[1] = colBoxSurface[1];
			xVex.xCol[2] = colBoxSurface[2];
			xVex.xCol[3] = colBoxSurface[3];
		}
	}

	unsigned uIdxListLen2  = 4;
	unsigned* puIndexList2 = new unsigned[4];
	puIndexList2[0] = 0;
	puIndexList2[1] = 1;
	puIndexList2[2] = 3;
	puIndexList2[3] = 2;

	pVexList->ApplyVertexData(GL_QUADS, 4, pData, true, true, true, true, false, false, false, 0);
	pVexList->SetIndexLists((const unsigned int** const) &puIndexList2, &uIdxListLen2, 1, 0);

	delete[] pData;
	delete[] puIndexList2;

	return rCB.GetOGLDrawBase()->GetSceneRepository()->New(pVexList);
}

//////////////////////////////////////////////////////////////////////
/// Draw Frame Function

bool DrawFrameFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!rCB.GetOGLDrawBase())
	{
		rCB.GetErrorList().GeneralError("Internal error.", iLine, iPos);
		return false;
	}

	TVarList& mVars = *rPars.GetVarListPtr();
	TCVScalar dSize = 3.0;
	TString csType  = "box";

	int iVarCount = int(mVars.Count());

	if (iVarCount > 5)
	{
		int piPar[] = { 0, 1, 2, 3, 4, 5 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 5, iLine, iPos);
		return false;
	}

	if (iVarCount >= 1)
	{
		if (!mVars(0).CastToScalar(dSize, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("First parameter has to give size of frame.", iLine, iPos);
			return false;
		}
	}

	if (iVarCount >= 2)
	{
		if (mVars(1).BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().GeneralError("Second parameter has to give type of box.", iLine, iPos);
			return false;
		}

		csType = *mVars(1).GetStringPtr();
	}

	if (csType == "box")
	{
		float fS = float(dSize / 2);

		COGLVertexList* pVexList = new COGLVertexList();

		pVexList->SetMode(GL_LINES);

		// X-Axis
		pVexList->AddCol(1.0f, 0.0f, 0.0f, 1.0f);
		pVexList->AddNormal(-1.0f, 0.0f, 0.0f);
		pVexList->AddVex(-fS, -fS, -fS);
		pVexList->AddCol(1.0f, 0.0f, 0.0f, 1.0f);
		pVexList->AddNormal(1.0f, 0.0f, 0.0f);
		pVexList->AddVex(fS, -fS, -fS);

		// Box
		pVexList->AddCol(1.0f, 1.0f, 1.0f, 1.0f);
		pVexList->AddNormal(0.0f, -1.0f, 0.0f);
		pVexList->AddVex(fS, -fS, -fS);
		pVexList->AddCol(1.0f, 1.0f, 1.0f, 1.0f);
		pVexList->AddNormal(0.0f, 1.0f, 0.0f);
		pVexList->AddVex(fS, fS, -fS);

		pVexList->AddCol(1.0f, 1.0f, 1.0f, 1.0f);
		pVexList->AddNormal(1.0f, 0.0f, 0.0f);
		pVexList->AddVex(fS, fS, -fS);
		pVexList->AddCol(1.0f, 1.0f, 1.0f, 1.0f);
		pVexList->AddNormal(-1.0f, 0.0f, 0.0f);
		pVexList->AddVex(-fS, fS, -fS);

		// Y-Axis
		pVexList->AddCol(0.0f, 1.0f, 0.0f, 1.0f);
		pVexList->AddNormal(0.0f, 1.0f, 0.0f);
		pVexList->AddVex(-fS, fS, -fS);
		pVexList->AddCol(0.0f, 1.0f, 0.0f, 1.0f);
		pVexList->AddNormal(0.0f, -1.0f, 0.0f);
		pVexList->AddVex(-fS, -fS, -fS);

		// Box
		pVexList->AddCol(1.0f, 1.0f, 1.0f, 1.0f);
		pVexList->AddNormal(-1.0f, 0.0f, 0.0f);
		pVexList->AddVex(-fS, -fS, fS);
		pVexList->AddCol(1.0f, 1.0f, 1.0f, 1.0f);
		pVexList->AddNormal(1.0f, 0.0f, 0.0f);
		pVexList->AddVex(fS, -fS, fS);

		pVexList->AddCol(1.0f, 1.0f, 1.0f, 1.0f);
		pVexList->AddNormal(0.0f, -1.0f, 0.0f);
		pVexList->AddVex(fS, -fS, fS);
		pVexList->AddCol(1.0f, 1.0f, 1.0f, 1.0f);
		pVexList->AddNormal(0.0f, 1.0f, 0.0f);
		pVexList->AddVex(fS, fS, fS);

		pVexList->AddCol(1.0f, 1.0f, 1.0f, 1.0f);
		pVexList->AddNormal(1.0f, 0.0f, 0.0f);
		pVexList->AddVex(fS, fS, fS);
		pVexList->AddCol(1.0f, 1.0f, 1.0f, 1.0f);
		pVexList->AddNormal(-1.0f, 0.0f, 0.0f);
		pVexList->AddVex(-fS, fS, fS);

		pVexList->AddCol(1.0f, 1.0f, 1.0f, 1.0f);
		pVexList->AddNormal(0.0f, 1.0f, 0.0f);
		pVexList->AddVex(-fS, fS, fS);
		pVexList->AddCol(1.0f, 1.0f, 1.0f, 1.0f);
		pVexList->AddNormal(0.0f, -1.0f, 0.0f);
		pVexList->AddVex(-fS, -fS, fS);

		// Z-Axis
		pVexList->AddCol(0.0f, 0.0f, 1.0f, 1.0f);
		pVexList->AddNormal(0.0f, 0.0f, -1.0f);
		pVexList->AddVex(-fS, -fS, -fS);
		pVexList->AddCol(0.0f, 0.0f, 1.0f, 1.0f);
		pVexList->AddNormal(0.0f, 0.0f, 1.0f);
		pVexList->AddVex(-fS, -fS, fS);

		// Box
		pVexList->AddCol(1.0f, 1.0f, 1.0f, 1.0f);
		pVexList->AddNormal(0.0f, 0.0f, -1.0f);
		pVexList->AddVex(fS, -fS, -fS);
		pVexList->AddCol(1.0f, 1.0f, 1.0f, 1.0f);
		pVexList->AddNormal(0.0f, 0.0f, 1.0f);
		pVexList->AddVex(fS, -fS, fS);

		pVexList->AddCol(1.0f, 1.0f, 1.0f, 1.0f);
		pVexList->AddNormal(0.0f, 0.0f, -1.0f);
		pVexList->AddVex(fS, fS, -fS);
		pVexList->AddCol(1.0f, 1.0f, 1.0f, 1.0f);
		pVexList->AddNormal(0.0f, 0.0f, 1.0f);
		pVexList->AddVex(fS, fS, fS);

		pVexList->AddCol(1.0f, 1.0f, 1.0f, 1.0f);
		pVexList->AddNormal(0.0f, 0.0f, -1.0f);
		pVexList->AddVex(-fS, fS, -fS);
		pVexList->AddCol(1.0f, 1.0f, 1.0f, 1.0f);
		pVexList->AddNormal(0.0f, 0.0f, 1.0f);
		pVexList->AddVex(-fS, fS, fS);

		rCB.GetOGLDrawBase()->DrawBaseElement(rCB.GetOGLDrawBase()->GetSceneRepository()->New(pVexList));
	}
	else if (csType == "axes")
	{
		COGLVertex xO, xA, xB, xC;
		xO.Set(0.0f, 0.0f, 0.0f);
		xA.Set(float(dSize), 0.0f, 0.0f);
		xB.Set(0.0f, float(dSize), 0.0f);
		xC.Set(0.0f, 0.0f, float(dSize));

		rCB.GetOGLDrawBase()->DrawBaseElement(rCB.GetOGLDrawBase()->GetSceneRepository()->New(new COGLColor(1.0f, 0.0f, 0.0f)));
		rCB.GetOGLDrawBase()->DrawVector(xO, xA);

		rCB.GetOGLDrawBase()->DrawBaseElement(rCB.GetOGLDrawBase()->GetSceneRepository()->New(new COGLColor(0.0f, 1.0f, 0.0f)));
		rCB.GetOGLDrawBase()->DrawVector(xO, xB);

		rCB.GetOGLDrawBase()->DrawBaseElement(rCB.GetOGLDrawBase()->GetSceneRepository()->New(new COGLColor(0.0f, 0.0f, 1.0f)));
		rCB.GetOGLDrawBase()->DrawVector(xO, xC);
	}
	else if (csType == "box_coord")
	{
		TCVCounter iSegCnt = 10, piSegCnt[3];
		COGLColor colBoxSurface, colGrid;

		piSegCnt[0] = piSegCnt[1] = piSegCnt[2] = iSegCnt;

		if (iVarCount >= 3)
		{
			if (mVars(2).CastToCounter(iSegCnt))
			{
				piSegCnt[0] = piSegCnt[1] = piSegCnt[2] = iSegCnt;
			}
			else if (mVars(2).BaseType() == PDT_VARLIST)
			{
				TVarList& rList = *mVars(2).GetVarListPtr();
				if (rList.Count() != 3)
				{
					rCB.GetErrorList().GeneralError("Third parameter has to give the number of segments or a 3-list of segment numbers.", iLine, iPos);
					return false;
				}

				for (int i = 0; i < 3; ++i)
				{
					if (!rList(i).CastToCounter(piSegCnt[i]))
					{
						rCB.GetErrorList().GeneralError("List of segment counts contains invalid members.", iLine, iPos);
						return false;
					}
				}
			}
			else
			{
				rCB.GetErrorList().GeneralError("Third parameter has to give the number of segments or a 3-list of segment numbers.", iLine, iPos);
				return false;
			}
		}

		if (iVarCount >= 4)
		{
			if (mVars(3).BaseType() != PDT_COLOR)
			{
				rCB.GetErrorList().GeneralError("Fourth parameter has to give the box surface color.", iLine, iPos);
				return false;
			}

			colBoxSurface = *mVars(3).GetOGLColorPtr();
		}
		else
		{
			colBoxSurface.Set(0.9f, 0.9f, 0.9f, 1.0f);
		}

		if (iVarCount >= 5)
		{
			if (mVars(4).BaseType() != PDT_COLOR)
			{
				rCB.GetErrorList().GeneralError("Fifth parameter has to give the grid color.", iLine, iPos);
				return false;
			}

			colGrid = *mVars(4).GetOGLColorPtr();
		}
		else
		{
			colGrid.Set(0.3f, 0.3f, 0.3f, 1.0f);
		}

		float fSegSize = float(dSize) / float(piSegCnt[0]);
		float pfSize[3], pfHalfSize[3];

		for (int i = 0; i < 3; ++i)
		{
			pfSize[i]     = fSegSize * float(piSegCnt[i]);
			pfHalfSize[i] = pfSize[i] / 2.0f;
		}

		COGLBEReference refGridXY = _CreateFrameGrid(rCB, fSegSize, piSegCnt[0], piSegCnt[1], 0, 1, 2, colGrid);
		COGLBEReference refGridYZ = _CreateFrameGrid(rCB, fSegSize, piSegCnt[1], piSegCnt[2], 1, 2, 0, colGrid);
		COGLBEReference refGridZX = _CreateFrameGrid(rCB, fSegSize, piSegCnt[2], piSegCnt[0], 2, 0, 1, colGrid);

		// ///////////////////////////////////////////////////////////////////////////////////////

		COGLBEReference refPlaneXY = _CreateFramePlane(rCB, fSegSize, piSegCnt[0], piSegCnt[1], 0, 1, 2, colBoxSurface);
		COGLBEReference refPlaneYZ = _CreateFramePlane(rCB, fSegSize, piSegCnt[1], piSegCnt[2], 1, 2, 0, colBoxSurface);
		COGLBEReference refPlaneZX = _CreateFramePlane(rCB, fSegSize, piSegCnt[2], piSegCnt[0], 2, 0, 1, colBoxSurface);

		// ////////////////////////////////////////////////////////////////////////////////////////////

		double dPi2 = acos(0.0);

		COGLFrame* pFrame;
		COGLFrameStack* pFrameStack;

		// ///////////////////////////////////////////////////////////////////////////////////////////////
		// XY-Planes
		pFrameStack = new COGLFrameStack();
		pFrameStack->DoPush();
		rCB.GetOGLDrawBase()->DrawBaseElement(rCB.GetOGLDrawBase()->GetSceneRepository()->New(pFrameStack));

		// Far XY-Plane
		pFrame = new COGLFrame;
		pFrame->Translate(0.0, 0.0, double(-pfHalfSize[2]));
		rCB.GetOGLDrawBase()->DrawBaseElement(rCB.GetOGLDrawBase()->GetSceneRepository()->New(pFrame));

		rCB.GetOGLDrawBase()->DrawBaseElement(refGridXY);
		rCB.GetOGLDrawBase()->DrawBaseElement(refPlaneXY);

		// Near XY-Plane
		pFrame = new COGLFrame;
		pFrame->Translate(0.0, 0.0, double(2.0f * pfHalfSize[2]));
		pFrame->Rotate(2.0 * dPi2, 1.0, 0.0, 0.0);
		rCB.GetOGLDrawBase()->DrawBaseElement(rCB.GetOGLDrawBase()->GetSceneRepository()->New(pFrame));

		rCB.GetOGLDrawBase()->DrawBaseElement(refGridXY);
		rCB.GetOGLDrawBase()->DrawBaseElement(refPlaneXY);

		pFrameStack = new COGLFrameStack();
		pFrameStack->DoPop();
		rCB.GetOGLDrawBase()->DrawBaseElement(rCB.GetOGLDrawBase()->GetSceneRepository()->New(pFrameStack));

		// ///////////////////////////////////////////////////////////////////////////////////////////////
		// YZ-Planes
		pFrameStack = new COGLFrameStack();
		pFrameStack->DoPush();
		rCB.GetOGLDrawBase()->DrawBaseElement(rCB.GetOGLDrawBase()->GetSceneRepository()->New(pFrameStack));

		// Left YZ-Plane
		pFrame = new COGLFrame;
		pFrame->Translate(double(-pfHalfSize[0]), 0.0, 0.0);
		rCB.GetOGLDrawBase()->DrawBaseElement(rCB.GetOGLDrawBase()->GetSceneRepository()->New(pFrame));

		rCB.GetOGLDrawBase()->DrawBaseElement(refGridYZ);
		rCB.GetOGLDrawBase()->DrawBaseElement(refPlaneYZ);

		// Right YZ-Plane
		pFrame = new COGLFrame;
		pFrame->Translate(double(2.0f * pfHalfSize[0]), 0.0, 0.0);
		pFrame->Rotate(2.0 * dPi2, 0.0, 1.0, 0.0);
		rCB.GetOGLDrawBase()->DrawBaseElement(rCB.GetOGLDrawBase()->GetSceneRepository()->New(pFrame));

		rCB.GetOGLDrawBase()->DrawBaseElement(refGridYZ);
		rCB.GetOGLDrawBase()->DrawBaseElement(refPlaneYZ);

		pFrameStack = new COGLFrameStack();
		pFrameStack->DoPop();
		rCB.GetOGLDrawBase()->DrawBaseElement(rCB.GetOGLDrawBase()->GetSceneRepository()->New(pFrameStack));

		// ///////////////////////////////////////////////////////////////////////////////////////////////
		// ZX-Planes
		pFrameStack = new COGLFrameStack();
		pFrameStack->DoPush();
		rCB.GetOGLDrawBase()->DrawBaseElement(rCB.GetOGLDrawBase()->GetSceneRepository()->New(pFrameStack));

		// Bottom ZX-Plane
		pFrame = new COGLFrame;
		pFrame->Translate(0.0, double(-pfHalfSize[1]), 0.0);
		rCB.GetOGLDrawBase()->DrawBaseElement(rCB.GetOGLDrawBase()->GetSceneRepository()->New(pFrame));

		rCB.GetOGLDrawBase()->DrawBaseElement(refGridZX);
		rCB.GetOGLDrawBase()->DrawBaseElement(refPlaneZX);

		// Top ZX-Plane
		pFrame = new COGLFrame;
		pFrame->Translate(0.0, double(2.0f * pfHalfSize[1]), 0.0);
		pFrame->Rotate(2.0 * dPi2, 1.0, 0.0, 0.0);
		rCB.GetOGLDrawBase()->DrawBaseElement(rCB.GetOGLDrawBase()->GetSceneRepository()->New(pFrame));

		rCB.GetOGLDrawBase()->DrawBaseElement(refGridZX);
		rCB.GetOGLDrawBase()->DrawBaseElement(refPlaneZX);

		pFrameStack = new COGLFrameStack();
		pFrameStack->DoPop();
		rCB.GetOGLDrawBase()->DrawBaseElement(rCB.GetOGLDrawBase()->GetSceneRepository()->New(pFrameStack));

		COGLVertex xO, xA, xB, xC;
		xO.Set(-pfHalfSize[0], -pfHalfSize[1], -pfHalfSize[2]);
		xA.Set(pfSize[0], 0.0f, 0.0f);
		xB.Set(0.0f, pfSize[1], 0.0f);
		xC.Set(0.0f, 0.0f, pfSize[2]);

		rCB.GetOGLDrawBase()->DrawBaseElement(rCB.GetOGLDrawBase()->GetSceneRepository()->New(new COGLColor(0.9f, 0.05f, 0.05f)));
		rCB.GetOGLDrawBase()->DrawVector(xO, xO + xA);

		rCB.GetOGLDrawBase()->DrawBaseElement(rCB.GetOGLDrawBase()->GetSceneRepository()->New(new COGLColor(0.05f, 0.9f, 0.05f)));
		rCB.GetOGLDrawBase()->DrawVector(xO, xO + xB);

		rCB.GetOGLDrawBase()->DrawBaseElement(rCB.GetOGLDrawBase()->GetSceneRepository()->New(new COGLColor(0.05f, 0.05f, 0.9f)));
		rCB.GetOGLDrawBase()->DrawVector(xO, xO + xC);
	}
	else
	{
		rCB.GetErrorList().GeneralError("Frame type has to be: \"box\" or \"axes\" or \"box_coord\".", iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Plane Function

bool DrawPlaneFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!rCB.GetOGLDrawBase())
	{
		return false;
	}

	int iVar;
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVScalar dVal[9];
	TCVScalar dRad = 0;

	if ((iVarCount == 9) || (iVarCount == 10))
	{
		for (iVar = 0; iVar < 9; iVar++)
		{
			if (!mVars(iVar).CastToScalar(dVal[iVar], rCB.GetSensitivity()))
			{
				rCB.GetErrorList().InvalidParType(mVars(iVar), iVar + 1, iLine, iPos);
				return false;
			}
		}

		if (iVarCount == 10)
		{
			if (!mVars(9).CastToScalar(dRad))
			{
				rCB.GetErrorList().InvalidParType(mVars(9), 10, iLine, iPos);
				return false;
			}
		}

		COGLVertex xP, xA, xB;

		xP.Set((float) dVal[0], (float) dVal[1], (float) dVal[2]);
		xA.Set((float) dVal[3], (float) dVal[4], (float) dVal[5]);
		xB.Set((float) dVal[6], (float) dVal[7], (float) dVal[8]);

		if (dRad == 0)
		{
			rCB.GetOGLDrawBase()->DrawPlane(xP, xA, xB, false, false);
		}
		else
		{
			rCB.GetOGLDrawBase()->DrawDisk(xP, xA, xB, float(Mag(dRad)), 5.0f, false);
		}

		//if (!rVar.New(PDT_VARLIST, "Constant"))
		//{
		//	rCB.GetErrorList().Internal(iLine, iPos);
		//	return false;
		//}
	}
	else if ((iVarCount == 3) || (iVarCount == 4))
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

		if (mVars(2).BaseType() != PDT_MULTIV)
		{
			rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}

		TMultiV& vA = *mVars(0).GetMultiVPtr();
		TMultiV& vB = *mVars(1).GetMultiVPtr();
		TMultiV& vC = *mVars(2).GetMultiVPtr();
		TMultiV vX, vY, vZ;
		COGLVertex xA, xB, xC;

		if (!rCB.CastMVtoE3(vA, vX))
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (!rCB.CastMVtoE3(vB, vY))
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		if (!rCB.CastMVtoE3(vC, vZ))
		{
			rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}

		xA.Set((float) vX[E3GA < TCVScalar > ::iE1],
				(float) vX[E3GA < TCVScalar > ::iE2],
				(float) vX[E3GA < TCVScalar > ::iE3]);

		xB.Set((float) vY[E3GA < TCVScalar > ::iE1],
				(float) vY[E3GA < TCVScalar > ::iE2],
				(float) vY[E3GA < TCVScalar > ::iE3]);

		xC.Set((float) vZ[E3GA < TCVScalar > ::iE1],
				(float) vZ[E3GA < TCVScalar > ::iE2],
				(float) vZ[E3GA < TCVScalar > ::iE3]);

		if (iVarCount >= 4)
		{
			if (mVars(3).BaseType() == PDT_MULTIV)
			{
				/// Draw Quad
				TMultiV& vD = *mVars(3).GetMultiVPtr();
				TMultiV vW;
				COGLVertex xD;

				if (!rCB.CastMVtoE3(vD, vW))
				{
					rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
					return false;
				}

				xD.Set((float) vW[E3GA < TCVScalar > ::iE1],
						(float) vW[E3GA < TCVScalar > ::iE2],
						(float) vW[E3GA < TCVScalar > ::iE3]);

				rCB.GetOGLDrawBase()->DrawQuad(xA, xB, xC, xD);
			}
			else if (mVars(3).CastToScalar(dRad))
			{
				if (dRad == 0)
				{
					rCB.GetOGLDrawBase()->DrawPlane(xA, xB, xC, false, false);
				}
				else
				{
					rCB.GetOGLDrawBase()->DrawDisk(xA, xB, xC, float(Mag(dRad)), 5.0f, false);
				}
			}
			else
			{
				rCB.GetErrorList().InvalidParType(mVars(3), 4, iLine, iPos);
				return false;
			}
		}
		else
		{
			rCB.GetOGLDrawBase()->DrawPlane(xA, xB, xC, false, false);
		}
	}
	else
	{
		int piPar[] = { 3, 4, 9, 10 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 4, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Polygon Function

bool DrawPolygonFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!rCB.GetOGLDrawBase())
	{
		return false;
	}

	TVarList& mVars      = *rPars.GetVarListPtr();
	int iVarCount        = int(mVars.Count());
	TCVCounter iDirected = 0;

	if ((iVarCount < 2) || (iVarCount > 3))
	{
		int piPar[] = { 2, 3 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_VARLIST)
	{
		rCB.GetErrorList().GeneralError("Expect as first parameter a list of at least three vectors.", iLine, iPos);
		return false;
	}

	TVarList& rList = *mVars(0).GetVarListPtr();
	int iVex, iVexCnt = int(rList.Count());

	if (iVexCnt < 3)
	{
		rCB.GetErrorList().GeneralError("List of vertices must have at least three elements.", iLine, iPos);
		return false;
	}

	if (mVars(1).BaseType() != PDT_VARLIST)
	{
		rCB.GetErrorList().GeneralError("Expect as second parameter a list of normal vectors.", iLine, iPos);
		return false;
	}

	TVarList& rNorm = *mVars(1).GetVarListPtr();
	if (iVexCnt != rNorm.Count())
	{
		rCB.GetErrorList().GeneralError("Number of polygon vertices has to be the same as number of normals.", iLine, iPos);
		return false;
	}

	if (iVarCount >= 3)
	{
		if (!mVars(2).CastToCounter(iDirected))
		{
			rCB.GetErrorList().GeneralError("Expect as third optional parameter a bool to indicate whether polygon is directed.",
					iLine, iPos);
			return false;
		}
	}

	TMultiV vX;
	COGLVertex xX, xN;
	Mem<COGLVertex> mVex, mNorm;
	mVex.Set(iVexCnt);
	mNorm.Set(iVexCnt);

	for (iVex = 0; iVex < iVexCnt; ++iVex)
	{
		if (rList(iVex).BaseType() != PDT_MULTIV)
		{
			rCB.GetErrorList().GeneralError("Element in vertex list is not a multivector.", iLine, iPos);
			return false;
		}

		TMultiV& vA = *rList(iVex).GetMultiVPtr();

		if (!rCB.CastMVtoE3(vA, vX))
		{
			rCB.GetErrorList().GeneralError("Multivector in vertex list cannot be interpreted as a point.", iLine, iPos);
			return false;
		}

		mVex[iVex].Set(
				(float) vX[E3GA < TCVScalar > ::iE1],
				(float) vX[E3GA < TCVScalar > ::iE2],
				(float) vX[E3GA < TCVScalar > ::iE3]);

		if (rNorm(iVex).BaseType() != PDT_MULTIV)
		{
			rCB.GetErrorList().GeneralError("Element in normal list is not a multivector.", iLine, iPos);
			return false;
		}

		TMultiV& vN = *rNorm(iVex).GetMultiVPtr();

		if (!rCB.CastMVtoE3(vN, vX))
		{
			rCB.GetErrorList().GeneralError("Multivector in normal list cannot be interpreted as a point.", iLine, iPos);
			return false;
		}

		mNorm[iVex].Set(
				(float) vX[E3GA < TCVScalar > ::iE1],
				(float) vX[E3GA < TCVScalar > ::iE2],
				(float) vX[E3GA < TCVScalar > ::iE3]);
	}

	rCB.GetOGLDrawBase()->DrawPolygon(mVex, mNorm, (iDirected ? true : false));

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Box Function

bool DrawBoxFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!rCB.GetOGLDrawBase())
	{
		return false;
	}

	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVScalar dVal[10];
	/*
	if (iVarCount == 10)
	{
	for (iVar = 0; iVar < 10; iVar++)
	{
	if (!mVars(iVar).CastToScalar(dVal[iVar], rCB.GetSensitivity()))
	{
	rCB.GetErrorList().InvalidParType(mVars(iVar), 1, iLine, iPos);
	return false;
	}
	}

	if (dVal[9] < 0)
	{
	rCB.GetErrorList().InvalidParVal(mVars(9), 10, iLine, iPos);
	return false;
	}

	COGLVertex xP, xA, xB;

	xP.Set((float) dVal[0], (float) dVal[1], (float) dVal[2]);
	xA.Set((float) dVal[3], (float) dVal[4], (float) dVal[5]);
	xB.Set((float) dVal[6], (float) dVal[7], (float) dVal[8]);

	rCB.GetOGLDrawBase()->DrawBox(xP, xA, xB, dVal[9], false, false);

	//if (!rVar.New(PDT_VARLIST, "Constant"))
	//{
	//	rCB.GetErrorList().Internal(iLine, iPos);
	//	return false;
	//}
	}
	else */

	if ((iVarCount == 4) || (iVarCount == 5))
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

		if (mVars(2).BaseType() != PDT_MULTIV)
		{
			rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}

		MemObj<COGLColor> mColList;

		if (iVarCount == 5)
		{
			if (mVars(4).BaseType() != PDT_VARLIST)
			{
				rCB.GetErrorList().InvalidParType(mVars(4), 5, iLine, iPos);
				return false;
			}

			TVarList& rColList = *mVars(4).GetVarListPtr();
			if (rColList.Count() != 8)
			{
				rCB.GetErrorList().GeneralError("Expect 8 colors in color list.", iLine, iPos);
				return false;
			}

			mColList.Set(8);

			int i;
			for (i = 0; i < 8; i++)
			{
				if (rColList[i].BaseType() != PDT_COLOR)
				{
					char pcText[100];
					sprintf_s(pcText, "Element at position %d in color list is not a color.", i + 1);
					rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
					return false;
				}

				mColList[i] = *rColList[i].GetOGLColorPtr();
			}
		}

		TMultiV& vP = *mVars(0).GetMultiVPtr();
		TMultiV& vA = *mVars(1).GetMultiVPtr();
		TMultiV& vB = *mVars(2).GetMultiVPtr();
		TMultiV vW, vX, vY;

		if (!rCB.CastMVtoE3(vP, vW))
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (!rCB.CastMVtoE3(vA, vX))
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		if (!rCB.CastMVtoE3(vB, vY))
		{
			rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}

		COGLVertex xP, xA, xB, xC;
		xP.Set((float) vW[E3GA < TCVScalar > ::iE1],
				(float) vW[E3GA < TCVScalar > ::iE2],
				(float) vW[E3GA < TCVScalar > ::iE3]);

		xA.Set((float) vX[E3GA < TCVScalar > ::iE1],
				(float) vX[E3GA < TCVScalar > ::iE2],
				(float) vX[E3GA < TCVScalar > ::iE3]);

		xB.Set((float) vY[E3GA < TCVScalar > ::iE1],
				(float) vY[E3GA < TCVScalar > ::iE2],
				(float) vY[E3GA < TCVScalar > ::iE3]);

		if (mVars(3).CastToScalar(dVal[0], rCB.GetSensitivity()))
		{
			if (dVal[0] <= 0)
			{
				rCB.GetErrorList().GeneralError("Box depth has to be greater than zero.", iLine, iPos);
				return false;
			}

			xC = xA ^ xB;								// vector cross product
			xC.Norm();
			xC *= float(dVal[0]);
		}
		else if (mVars(3).BaseType() == PDT_MULTIV)
		{
			TMultiV vZ;
			TMultiV& vC = *mVars(3).GetMultiVPtr();

			if (!rCB.CastMVtoE3(vC, vZ))
			{
				rCB.GetErrorList().InvalidParType(mVars(3), 4, iLine, iPos);
				return false;
			}

			xC.Set((float) vZ[E3GA < TCVScalar > ::iE1],
					(float) vZ[E3GA < TCVScalar > ::iE2],
					(float) vZ[E3GA < TCVScalar > ::iE3]);
		}
		else
		{
			rCB.GetErrorList().InvalidParType(mVars(3), 4, iLine, iPos);
			return false;
		}

		rVar.New(PDT_VEXLIST);
		rCB.GetOGLDrawBase()->DrawBox(xP, xA, xB, xC, mColList);
	}
	else
	{
		int piPar[] = { 4, 10 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Line Function

bool DrawLineFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!rCB.GetOGLDrawBase())
	{
		return false;
	}

	int iVar;
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVScalar dVal[6];
	MemObj<COGLBEReference> mScene;

	if (iVarCount == 6)
	{
		for (iVar = 0; iVar < 6; iVar++)
		{
			if (!mVars(iVar).CastToScalar(dVal[iVar], rCB.GetSensitivity()))
			{
				rCB.GetErrorList().InvalidParType(mVars(iVar), 1, iLine, iPos);
				return false;
			}
		}

		COGLVertex xA, xB;

		xA.Set((float) dVal[0], (float) dVal[1], (float) dVal[2]);
		xB.Set((float) dVal[3], (float) dVal[4], (float) dVal[5]);

		rCB.GetOGLDrawBase()->DrawLine(xA, xB, true, 0, 1, &mScene);

		//if (!rVar.New(PDT_VARLIST, "Constant"))
		//{
		//	rCB.GetErrorList().Internal(iLine, iPos);
		//	return false;
		//}
	}
	else if (iVarCount == 2)
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
		TMultiV vX, vY;

		if (!rCB.CastMVtoE3(vA, vX))
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (!rCB.CastMVtoE3(vB, vY))
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		COGLVertex xA, xB;
		xA.Set((float) vX[E3GA < TCVScalar > ::iE1],
				(float) vX[E3GA < TCVScalar > ::iE2],
				(float) vX[E3GA < TCVScalar > ::iE3]);

		xB.Set((float) vY[E3GA < TCVScalar > ::iE1],
				(float) vY[E3GA < TCVScalar > ::iE2],
				(float) vY[E3GA < TCVScalar > ::iE3]);

		rCB.GetOGLDrawBase()->DrawLine(xA, xB, true, 0, 1, &mScene);
	}
	else
	{
		int piPar[] = { 2, 6 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	rVar.New(PDT_VARLIST);
	TVarList& rList = *rVar.GetVarListPtr();
	int iIdx, iCnt = int(mScene.Count());
	rList.Set(iCnt);
	for (iIdx = 0; iIdx < iCnt; ++iIdx)
	{
		rList(iIdx) = mScene[iIdx];
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Point Function

bool DrawPointFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!rCB.GetOGLDrawBase())
	{
		return false;
	}

	int iVar;
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVScalar dVal[3];

	if (iVarCount == 3)
	{
		for (iVar = 0; iVar < 3; iVar++)
		{
			if (!mVars(iVar).CastToScalar(dVal[iVar], rCB.GetSensitivity()))
			{
				rCB.GetErrorList().InvalidParType(mVars(iVar), 1, iLine, iPos);
				return false;
			}
		}

		COGLVertex xA;

		xA.Set((float) dVal[0], (float) dVal[1], (float) dVal[2]);

		rCB.GetOGLDrawBase()->DrawPoint(xA);

		rCB.GetBMPPos() = xA;

		//if (!rVar.New(PDT_VARLIST, "Constant"))
		//{
		//	rCB.GetErrorList().Internal(iLine, iPos);
		//	return false;
		//}
	}
	// If a single multivector is given, cast it to E3
	// and use that position.
	else if (iVarCount == 1)
	{
		if (mVars(0).BaseType() == PDT_MULTIV)
		{
			TMultiV& vA = *mVars(0).GetMultiVPtr();
			TMultiV vB;
			if (!rCB.CastMVtoE3(vA, vB))
			{
				rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
				return false;
			}

			COGLVertex xA;
			xA.Set((float) vB[E3GA < TCVScalar > ::iE1],
					(float) vB[E3GA < TCVScalar > ::iE2],
					(float) vB[E3GA < TCVScalar > ::iE3]);

			rCB.GetOGLDrawBase()->DrawPoint(xA);
			rCB.GetBMPPos() = xA;
		}
		else
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}
	}
	else
	{
		int piPar[] = { 1, 3 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Arrow Function

bool DrawArrowFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!rCB.GetOGLDrawBase())
	{
		return false;
	}

	int iVar;
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVScalar dVal[6];

	if (iVarCount == 6)
	{
		for (iVar = 0; iVar < 6; iVar++)
		{
			if (!mVars(iVar).CastToScalar(dVal[iVar], rCB.GetSensitivity()))
			{
				rCB.GetErrorList().InvalidParType(mVars(iVar), iVar + 1, iLine, iPos);
				return false;
			}
		}

		COGLVertex xA, xB;

		xA.Set((float) dVal[0], (float) dVal[1], (float) dVal[2]);
		xB.Set((float) dVal[3], (float) dVal[4], (float) dVal[5]);

		rCB.GetOGLDrawBase()->DrawVector(xA, xB);

		//if (!rVar.New(PDT_VARLIST, "Constant"))
		//{
		//	rCB.GetErrorList().Internal(iLine, iPos);
		//	return false;
		//}
	}
	else if (iVarCount == 2)
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
		TMultiV vX, vY;

		if (!rCB.CastMVtoE3(vA, vX))
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (!rCB.CastMVtoE3(vB, vY))
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		COGLVertex xA, xB;
		xA.Set((float) vX[E3GA < TCVScalar > ::iE1],
				(float) vX[E3GA < TCVScalar > ::iE2],
				(float) vX[E3GA < TCVScalar > ::iE3]);

		xB.Set((float) vY[E3GA < TCVScalar > ::iE1],
				(float) vY[E3GA < TCVScalar > ::iE2],
				(float) vY[E3GA < TCVScalar > ::iE3]);

		rCB.GetOGLDrawBase()->DrawVector(xA, xB);
	}
	else
	{
		int piPar[] = { 2, 6 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Arc Function

bool DrawArcFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!rCB.GetOGLDrawBase())
	{
		return false;
	}

	int iVar;
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVScalar dVal;
	bool bShort;

	if ((iVarCount == 4) || (iVarCount == 5))
	{
		for (iVar = 0; iVar < 3; iVar++)
		{
			if (mVars(iVar).BaseType() != PDT_MULTIV)
			{
				rCB.GetErrorList().InvalidParType(mVars(iVar), iVar + 1, iLine, iPos);
				return false;
			}
		}

		if (!mVars(3).CastToScalar(dVal, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(mVars(3), 4, iLine, iPos);
			return false;
		}

		if (iVarCount == 5)
		{
			int iVal;

			if (!mVars(4).CastToCounter(iVal))
			{
				rCB.GetErrorList().InvalidParType(mVars(4), 5, iLine, iPos);
				return false;
			}

			if (iVal)
			{
				bShort = true;
			}
			else
			{
				bShort = false;
			}
		}
		else
		{
			bShort = true;
		}

		TMultiV& vP = *mVars(0).GetMultiVPtr();
		TMultiV& vA = *mVars(1).GetMultiVPtr();
		TMultiV& vB = *mVars(2).GetMultiVPtr();
		TMultiV vX, vY, vZ;

		if (!rCB.CastMVtoE3(vP, vX))
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (!rCB.CastMVtoE3(vA, vY))
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		if (!rCB.CastMVtoE3(vB, vZ))
		{
			rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}

		COGLVertex xA, xB, xP;
		xP.Set((float) vX[E3GA < TCVScalar > ::iE1],
				(float) vX[E3GA < TCVScalar > ::iE2],
				(float) vX[E3GA < TCVScalar > ::iE3]);

		xA.Set((float) vY[E3GA < TCVScalar > ::iE1],
				(float) vY[E3GA < TCVScalar > ::iE2],
				(float) vY[E3GA < TCVScalar > ::iE3]);

		xB.Set((float) vZ[E3GA < TCVScalar > ::iE1],
				(float) vZ[E3GA < TCVScalar > ::iE2],
				(float) vZ[E3GA < TCVScalar > ::iE3]);

		rCB.GetOGLDrawBase()->DrawArc(xP, xA, xB, float(dVal), bShort);
	}
	else
	{
		int piPar[] = { 4 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Disk Function

bool DrawDiskFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!rCB.GetOGLDrawBase())
	{
		return false;
	}

	int iVar;
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVScalar dVal;
	bool bShort;

	if ((iVarCount == 4) || (iVarCount == 5))
	{
		for (iVar = 0; iVar < 3; iVar++)
		{
			if (mVars(iVar).BaseType() != PDT_MULTIV)
			{
				rCB.GetErrorList().InvalidParType(mVars(iVar), iVar + 1, iLine, iPos);
				return false;
			}
		}

		if (!mVars(3).CastToScalar(dVal, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(mVars(3), 4, iLine, iPos);
			return false;
		}

		if (iVarCount == 5)
		{
			int iVal;

			if (!mVars(4).CastToCounter(iVal))
			{
				rCB.GetErrorList().InvalidParType(mVars(4), 5, iLine, iPos);
				return false;
			}

			if (iVal)
			{
				bShort = true;
			}
			else
			{
				bShort = false;
			}
		}
		else
		{
			bShort = true;
		}

		TMultiV& vP = *mVars(0).GetMultiVPtr();
		TMultiV& vA = *mVars(1).GetMultiVPtr();
		TMultiV& vB = *mVars(2).GetMultiVPtr();
		TMultiV vX, vY, vZ;

		if (!rCB.CastMVtoE3(vP, vX))
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (!rCB.CastMVtoE3(vA, vY))
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		if (!rCB.CastMVtoE3(vB, vZ))
		{
			rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}

		COGLVertex xA, xB, xP;
		xP.Set((float) vX[E3GA < TCVScalar > ::iE1],
				(float) vX[E3GA < TCVScalar > ::iE2],
				(float) vX[E3GA < TCVScalar > ::iE3]);

		xA.Set((float) vY[E3GA < TCVScalar > ::iE1],
				(float) vY[E3GA < TCVScalar > ::iE2],
				(float) vY[E3GA < TCVScalar > ::iE3]);

		xB.Set((float) vZ[E3GA < TCVScalar > ::iE1],
				(float) vZ[E3GA < TCVScalar > ::iE2],
				(float) vZ[E3GA < TCVScalar > ::iE3]);

		rCB.GetOGLDrawBase()->DrawArc(xP, xA, xB, float(dVal), bShort, true);
	}
	else
	{
		int piPar[] = { 4 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Ellipse Function

bool DrawEllipseFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!rCB.GetOGLDrawBase())
	{
		return false;
	}

	int iVar;
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if (iVarCount == 3)
	{
		for (iVar = 0; iVar < 3; iVar++)
		{
			if (mVars(iVar).BaseType() != PDT_MULTIV)
			{
				rCB.GetErrorList().InvalidParType(mVars(iVar), iVar + 1, iLine, iPos);
				return false;
			}
		}

		TMultiV& vP = *mVars(0).GetMultiVPtr();
		TMultiV& vA = *mVars(1).GetMultiVPtr();
		TMultiV& vB = *mVars(2).GetMultiVPtr();
		TMultiV vC, vX, vY;

		if (!rCB.CastMVtoE3(vP, vC))
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (!rCB.CastMVtoE3(vA, vX))
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		if (!rCB.CastMVtoE3(vB, vY))
		{
			rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}

		COGLVertex xP, xA, xB;
		xP.Set((float) vC[E3GA < TCVScalar > ::iE1],
				(float) vC[E3GA < TCVScalar > ::iE2],
				(float) vC[E3GA < TCVScalar > ::iE3]);

		xA.Set((float) vX[E3GA < TCVScalar > ::iE1],
				(float) vX[E3GA < TCVScalar > ::iE2],
				(float) vX[E3GA < TCVScalar > ::iE3]);

		xB.Set((float) vY[E3GA < TCVScalar > ::iE1],
				(float) vY[E3GA < TCVScalar > ::iE2],
				(float) vY[E3GA < TCVScalar > ::iE3]);

		rCB.GetOGLDrawBase()->DrawEllipse(xP, xA, xB);
	}
	else
	{
		int piPar[] = { 3 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Ellipsoid Function

bool DrawEllipsoidFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!rCB.GetOGLDrawBase())
	{
		return false;
	}

	int iVar;
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	if ((iVarCount == 4) || (iVarCount == 5))
	{
		TCVCounter iSolid = 1;

		for (iVar = 0; iVar < 4; iVar++)
		{
			if (mVars(iVar).BaseType() != PDT_MULTIV)
			{
				rCB.GetErrorList().InvalidParType(mVars(iVar), iVar + 1, iLine, iPos);
				return false;
			}
		}

		if (iVarCount >= 5)
		{
			if (!mVars(4).CastToCounter(iSolid))
			{
				rCB.GetErrorList().GeneralError("Expect as fifth parameter a boolean value indicating whether "
								"ellipsoid is to be drawn as solid object (true, default) or as wire frame (false).",
						iLine, iPos);
				return false;
			}
		}

		TMultiV& vP = *mVars(0).GetMultiVPtr();
		TMultiV& vA = *mVars(1).GetMultiVPtr();
		TMultiV& vB = *mVars(2).GetMultiVPtr();
		TMultiV& vC = *mVars(3).GetMultiVPtr();
		TMultiV vQ, vX, vY, vZ;

		if (!rCB.CastMVtoE3(vP, vQ))
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (!rCB.CastMVtoE3(vA, vX))
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		if (!rCB.CastMVtoE3(vB, vY))
		{
			rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}

		if (!rCB.CastMVtoE3(vC, vZ))
		{
			rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}

		COGLVertex xP, xA, xB, xC;
		xP.Set((float) vQ[E3GA < TCVScalar > ::iE1],
				(float) vQ[E3GA < TCVScalar > ::iE2],
				(float) vQ[E3GA < TCVScalar > ::iE3]);

		xA.Set((float) vX[E3GA < TCVScalar > ::iE1],
				(float) vX[E3GA < TCVScalar > ::iE2],
				(float) vX[E3GA < TCVScalar > ::iE3]);

		xB.Set((float) vY[E3GA < TCVScalar > ::iE1],
				(float) vY[E3GA < TCVScalar > ::iE2],
				(float) vY[E3GA < TCVScalar > ::iE3]);

		xC.Set((float) vZ[E3GA < TCVScalar > ::iE1],
				(float) vZ[E3GA < TCVScalar > ::iE2],
				(float) vZ[E3GA < TCVScalar > ::iE3]);

		rCB.GetOGLDrawBase()->DrawEllipsoid(xP, xA, xB, xC, (iSolid ? true : false));
	}
	else
	{
		int piPar[] = { 4, 5 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Circle Function

bool DrawCircleFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!rCB.GetOGLDrawBase())
	{
		return false;
	}

	int iVar;
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVScalar dVal;

	if (iVarCount == 3)
	{
		for (iVar = 0; iVar < 2; iVar++)
		{
			if (mVars(iVar).BaseType() != PDT_MULTIV)
			{
				rCB.GetErrorList().InvalidParType(mVars(iVar), iVar + 1, iLine, iPos);
				return false;
			}
		}

		if (!mVars(2).CastToScalar(dVal, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}

		TMultiV& vP = *mVars(0).GetMultiVPtr();
		TMultiV& vN = *mVars(1).GetMultiVPtr();
		TMultiV vX, vY;

		if (!rCB.CastMVtoE3(vP, vX))
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (!rCB.CastMVtoE3(vN, vY))
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		COGLVertex xN, xP;
		xP.Set((float) vX[E3GA < TCVScalar > ::iE1],
				(float) vX[E3GA < TCVScalar > ::iE2],
				(float) vX[E3GA < TCVScalar > ::iE3]);

		xN.Set((float) vY[E3GA < TCVScalar > ::iE1],
				(float) vY[E3GA < TCVScalar > ::iE2],
				(float) vY[E3GA < TCVScalar > ::iE3]);

		rCB.GetOGLDrawBase()->DrawCircle(xP, xN, float(dVal));
	}
	else
	{
		int piPar[] = { 3 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Circle Surface Function
///
/// Expects a list of multivectors representing circles
///
/// Parameters:
///		1. List of circles as multivectors in N3
///		2. List of colors (optional)
///		3. Flag indicating whether to draw surface immediately (optional)
///
///	Returns:
///		Vertex List representing object.

bool DrawCircleSurfaceFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!rCB.GetOGLDrawBase())
	{
		return false;
	}

	if (!rCB.GetFilter())
	{
		rCB.GetErrorList().GeneralError("Internal multivector analysis filter not set.", iLine, iPos);
		return false;
	}

	TVarList* pColList = 0;
	TVarList& mVars    = *rPars.GetVarListPtr();

	int iVarCount            = int(mVars.Count());
	bool bUseCol             = false;
	bool bDoDraw             = true;
	TCVCounter iAngleStepCnt = 20;

	if ((iVarCount < 1) || (iVarCount > 4))
	{
		int piPar[] = { 1, 2, 3, 4 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 4, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_VARLIST)
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 0, iLine, iPos);
		return false;
	}

	if (iVarCount >= 2)
	{
		if (mVars(1).BaseType() == PDT_VARLIST)
		{
			bUseCol  = true;
			pColList = mVars(1).GetVarListPtr();
		}
		/*
		else
		{
		rCB.GetErrorList().InvalidParType(mVars(1), 1, iLine, iPos);
		return false;
		}
		*/
	}

	if (iVarCount >= 3)
	{
		int iVal;
		if (!mVars(2).CastToCounter(iVal))
		{
			rCB.GetErrorList().InvalidParType(mVars(2), 2, iLine, iPos);
			return false;
		}

		if (iVal)
		{
			bDoDraw = true;
		}
		else
		{
			bDoDraw = false;
		}
	}

	if (iVarCount >= 4)
	{
		if (!mVars(3).CastToCounter(iAngleStepCnt))
		{
			rCB.GetErrorList().GeneralError("Expect as fourth parameter the number of steps per circle.", iLine, iPos);
			return false;
		}

		if ((iAngleStepCnt < 3) || (iAngleStepCnt > 720))
		{
			rCB.GetErrorList().GeneralError("The number of steps per circle have to lie between 3 and 720.", iLine, iPos);
			return false;
		}
	}

	CMVInfo<float> MVInfo;
	Mem<COGLVertex> mCenter, mNormal;
	Mem<float> mRadius;
	MemObj<COGLColor> mColor;

	TVarList& CList = *mVars(0).GetVarListPtr();
	int iP, iCount = int(CList.Count());

	mCenter.Set(iCount);
	mNormal.Set(iCount);
	mRadius.Set(iCount);

	if (bUseCol)
	{
		mColor.Set(iCount);
	}

	for (iP = 0; iP < iCount; iP++)
	{
		if (CList[iP].BaseType() != PDT_MULTIV)
		{
			rCB.GetErrorList().GeneralError("Element in list is not a multivector.", iLine, iPos);
			return false;
		}

		if (bUseCol)
		{
			if ((*pColList)[iP].BaseType() != PDT_COLOR)
			{
				rCB.GetErrorList().GeneralError("Element in color list is not a color.", iLine, iPos);
				return false;
			}

			mColor[iP] = *((COGLColor*) (*pColList)[iP].Val());
		}

		TMultiV& vC = *((TMultiV*) CList[iP].Val());

		// Only analyze multivector
		rCB.GetFilter()->DrawMV(vC, true);

		// Get MVInfo
		MVInfo = rCB.GetFilter()->GetMVInfo();

		if (MVInfo.m_eType != GA_CIRCLE)
		{
			char pcText[200];

			sprintf_s(pcText, "The multivectors at position %d in list does not represent a circle.", iP + 1);
			rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
			return false;
		}

		mCenter[iP] = MVInfo.m_mVex[0].Data();
		mNormal[iP] = MVInfo.m_mVex[1].Data();
		mRadius[iP] = MVInfo.m_mVal[0];
	}

	float fAngleStep = float(360.0f / double(iAngleStepCnt));

	TScene refScene;
	rCB.GetOGLDrawBase()->DrawCircleSurface(mCenter, mNormal, mRadius, mColor, refScene, fAngleStep, bDoDraw);

	rVar = refScene;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Ellipse Surface Function
///
/// Expects a list of 1-vector triplets representing
///	the center and the two axes of the ellipsed.
///
/// Parameters:
///		1. List of vector triplets (as sublists)
///		2. List of colors (optional)
///		3. Angle step (optional)
///		4. Flag indicating whether to draw surface immediately (optional)
///
///	Returns:
///		Vertex List representing object.

bool DrawEllipseSurfaceFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!rCB.GetOGLDrawBase())
	{
		return false;
	}

	if (!rCB.GetFilter())
	{
		rCB.GetErrorList().GeneralError("Internal multivector analysis filter not set.", iLine, iPos);
		return false;
	}

	TVarList* pColList = 0;
	TVarList& mVars    = *rPars.GetVarListPtr();

	int iVarCount    = int(mVars.Count());
	bool bUseCol     = false;
	bool bDoDraw     = true;
	float fAngleStep = 15.0f;

	if ((iVarCount < 1) || (iVarCount > 4))
	{
		int piPar[] = { 1, 2, 3, 4 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 4, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_VARLIST)
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 0, iLine, iPos);
		return false;
	}

	if (iVarCount >= 2)
	{
		if (mVars(1).BaseType() == PDT_VARLIST)
		{
			bUseCol  = true;
			pColList = mVars(1).GetVarListPtr();
		}
	}

	if (iVarCount >= 3)
	{
		TCVScalar dVal;
		if (!mVars(2).CastToScalar(dVal))
		{
			rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}

		fAngleStep = float(dVal);
		if ((fAngleStep > 90) || (fAngleStep < 1))
		{
			rCB.GetErrorList().GeneralError("Angle step has to be in range [1, 90].", iLine, iPos);
			return false;
		}
	}

	if (iVarCount >= 4)
	{
		int iVal;
		if (!mVars(3).CastToCounter(iVal))
		{
			rCB.GetErrorList().InvalidParType(mVars(3), 4, iLine, iPos);
			return false;
		}

		if (iVal)
		{
			bDoDraw = true;
		}
		else
		{
			bDoDraw = false;
		}
	}

	CMVInfo<float> MVInfo;
	Mem<COGLVertex> mCenter, mEX, mEY;
	MemObj<COGLColor> mColor;

	TVarList& CList = *mVars(0).GetVarListPtr();
	int iP, iCount = int(CList.Count());

	mCenter.Set(iCount);
	mEX.Set(iCount);
	mEY.Set(iCount);

	if (bUseCol)
	{
		mColor.Set(iCount);
	}

	for (iP = 0; iP < iCount; iP++)
	{
		if (CList[iP].BaseType() != PDT_VARLIST)
		{
			rCB.GetErrorList().GeneralError("Expect list of sublists.", iLine, iPos);
			return false;
		}

		TVarList& rVecList = *CList[iP].GetVarListPtr();
		if (rVecList.Count() != 3)
		{
			rCB.GetErrorList().GeneralError("Expect sublists of three entities.", iLine, iPos);
			return false;
		}

		if (bUseCol)
		{
			if ((*pColList)[iP].BaseType() != PDT_COLOR)
			{
				rCB.GetErrorList().GeneralError("Element in color list is not a color.", iLine, iPos);
				return false;
			}

			mColor[iP] = *((COGLColor*) (*pColList)[iP].Val());
		}

		TMultiV& vC = *rVecList(0).GetMultiVPtr();
		rCB.GetFilter()->DrawMV(vC, true);
		MVInfo = rCB.GetFilter()->GetMVInfo();
		if ((MVInfo.m_eType != GA_POINT) && (MVInfo.m_eType != GA_DIR1D))
		{
			char pcText[200];

			sprintf_s(pcText, "The multivector at position %d in list does not represent a point or direction.", iP + 1);
			rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
			return false;
		}
		mCenter[iP] = MVInfo.m_mVex[0].Data();

		TMultiV& vEX = *rVecList(1).GetMultiVPtr();
		rCB.GetFilter()->DrawMV(vEX, true);
		MVInfo = rCB.GetFilter()->GetMVInfo();
		if ((MVInfo.m_eType != GA_POINT) && (MVInfo.m_eType != GA_DIR1D))
		{
			char pcText[200];

			sprintf_s(pcText, "The multivector at position %d in list does not represent a point or direction.", iP + 1);
			rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
			return false;
		}
		mEX[iP] = MVInfo.m_mVex[0].Data();

		TMultiV& vEY = *rVecList(2).GetMultiVPtr();
		rCB.GetFilter()->DrawMV(vEY, true);
		MVInfo = rCB.GetFilter()->GetMVInfo();
		if ((MVInfo.m_eType != GA_POINT) && (MVInfo.m_eType != GA_DIR1D))
		{
			char pcText[200];

			sprintf_s(pcText, "The multivector at position %d in list does not represent a point or direction.", iP + 1);
			rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
			return false;
		}
		mEY[iP] = MVInfo.m_mVex[0].Data();
	}

	TScene refScene;
	rCB.GetOGLDrawBase()->DrawEllipseSurface(mCenter, mEX, mEY, mColor, refScene, fAngleStep, bDoDraw);

	rVar = refScene;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Cylinder Function
///
/// Expects:
///		1. MV giving center of bottom
///		2. MV giving direction and length of cylinder
///		3. Scalar giving radius of cylinder
///		4. (optional) number of slices

bool DrawCylinderFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!rCB.GetOGLDrawBase())
	{
		return false;
	}

	if (!rCB.GetFilter())
	{
		rCB.GetErrorList().GeneralError("Internal multivector analysis filter not set.", iLine, iPos);
		return false;
	}

	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount   = int(mVars.Count());
	bool bClosed    = false;
	int iAngleSteps = 72;

	if ((iVarCount < 3) || (iVarCount > 5))
	{
		int piPar[] = { 3, 4, 5 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 3, iLine, iPos);
		return false;
	}

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

	TCVScalar dR;
	if (!mVars(2).CastToScalar(dR, rCB.GetSensitivity()))
	{
		rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
		return false;
	}

	if (iVarCount >= 4)
	{
		int iVal;
		if (!mVars(3).CastToCounter(iVal))
		{
			rCB.GetErrorList().InvalidParType(mVars(3), 4, iLine, iPos);
			return false;
		}

		if ((iVal < 3) || (iVal > 360))
		{
			rCB.GetErrorList().GeneralError("Number of slices needs to be between 3 and 360.", iLine, iPos);
			return false;
		}

		iAngleSteps = iVal;
	}

	if (iVarCount >= 5)
	{
		TCVScalar dVal;
		if (!mVars(4).CastToScalar(dVal, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Expect as optional fifth parameter boolean value whether cylinder is closed.", iLine,
					iPos);
			return false;
		}

		if (dVal)
		{
			bClosed = true;
		}
		else
		{
			bClosed = false;
		}
	}

	float fAngle = 360.0f / float(iAngleSteps);

	TMultiV vC, vX;
	COGLVertex xCenter, xAxis;

	TMultiV& vA = *mVars(0).GetMultiVPtr();
	TMultiV& vB = *mVars(1).GetMultiVPtr();

	rCB.CastMVtoE3(vA, vC);
	rCB.CastMVtoE3(vB, vX);

	xCenter.Set(float(vC[1]), float(vC[2]), float(vC[3]));
	xAxis.Set(float(vX[1]), float(vX[2]), float(vX[3]));

	rCB.GetOGLDrawBase()->DrawCylinder(xCenter, xAxis, float(dR), bClosed, fAngle);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Line Surface Function
///
/// Expects a list of multivectors representing lines

bool DrawLineSurfaceFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!rCB.GetOGLDrawBase())
	{
		return false;
	}

	if (!rCB.GetFilter())
	{
		rCB.GetErrorList().GeneralError("Internal multivector analysis filter not set.", iLine, iPos);
		return false;
	}

	TVarList* pColList = 0;
	TVarList& mVars    = *rPars.GetVarListPtr();

	int iVarCount       = int(mVars.Count());
	bool bUseCol        = false;
	bool bDoDraw        = true;
	TCVCounter iStepCnt = 20;

	if ((iVarCount < 1) || (iVarCount > 4))
	{
		int piPar[] = { 1, 2, 3, 4 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 4, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_VARLIST)
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 0, iLine, iPos);
		return false;
	}

	if (iVarCount >= 2)
	{
		if (mVars(1).BaseType() == PDT_VARLIST)
		{
			bUseCol  = true;
			pColList = mVars(1).GetVarListPtr();
		}
	}

	if (iVarCount >= 3)
	{
		int iVal;
		if (!mVars(2).CastToCounter(iVal))
		{
			rCB.GetErrorList().InvalidParType(mVars(2), 2, iLine, iPos);
			return false;
		}

		if (iVal)
		{
			bDoDraw = true;
		}
		else
		{
			bDoDraw = false;
		}
	}

	if (iVarCount >= 4)
	{
		if (!mVars(3).CastToCounter(iStepCnt))
		{
			rCB.GetErrorList().GeneralError("Expect as fourth parameter the number of visualization steps along each line.", iLine,
					iPos);
			return false;
		}
	}

	CMVInfo<float> MVInfo;
	Mem<COGLVertex> mCenter, mDir;
	Mem<float> mLen;
	MemObj<COGLColor> mColor;

	TVarList& CList = *mVars(0).GetVarListPtr();
	int iP, iCount = int(CList.Count());

	mCenter.Set(iCount);
	mDir.Set(iCount);
	mLen.Set(iCount);

	if (bUseCol)
	{
		mColor.Set(iCount);
	}

	for (iP = 0; iP < iCount; iP++)
	{
		if (CList[iP].BaseType() != PDT_MULTIV)
		{
			rCB.GetErrorList().GeneralError("Element in list is not a multivector.", iLine, iPos);
			return false;
		}

		if (bUseCol)
		{
			if ((*pColList)[iP].BaseType() != PDT_COLOR)
			{
				rCB.GetErrorList().GeneralError("Element in color list is not a color.", iLine, iPos);
				return false;
			}

			mColor[iP] = *((COGLColor*) (*pColList)[iP].Val());
		}

		TMultiV& vC = *((TMultiV*) CList[iP].Val());

		// Only analyze multivector
		rCB.GetFilter()->DrawMV(vC, true);

		// Get MVInfo
		MVInfo = rCB.GetFilter()->GetMVInfo();

		if (MVInfo.m_eType != GA_LINE)
		{
			char pcText[200];

			sprintf_s(pcText, "The multivectors at position %d in list does not represent a line.", iP + 1);
			rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
			return false;
		}

		mCenter[iP] = MVInfo.m_mVex[0].Data();
		mDir[iP]    = MVInfo.m_mVex[1].Data();
		mLen[iP]    = MVInfo.m_mVal[0];
	}

	TScene refScene;
	rCB.GetOGLDrawBase()->DrawLineSurface(mCenter, mDir, mLen, mColor, refScene, iStepCnt, bDoDraw);

	rVar = refScene;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Point Surface Function
///
/// Expects:
/// 1. Number of columns, ie width in x-direction
/// 2. Number of rows, ie width in y-direction
/// 3. List of multivectors representing points in E3
/// 4. List of colors for each point (optional)
/// 5. Bool flag indicating whether surface is to be drawn directly (optional)
/// 6. List of multivectors representing corresponding normals (optional)
/// 7. Scale of normal lines to be drawn (optional). By default this is 0, i.e. nothing
///		is drawn.
/// 8. List of multivectors representing 2D-texture coordinates. (optional)
///
/// Returns:
/// List of base element references (scenes) that may be drawn later on with :-operator

bool DrawPointSurfaceFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!rCB.GetOGLDrawBase())
	{
		return false;
	}

	if (!rCB.GetFilter())
	{
		rCB.GetErrorList().GeneralError("Internal multivector analysis filter not set.", iLine, iPos);
		return false;
	}

	TVarList* pColList    = 0;
	TVarList* pNormalList = 0;
	TVarList* pTexList    = 0;
	TVarList& mVars       = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	int iRowCount, iColCount;
	bool bUseCol         = false;
	bool bUseNormals     = false;
	bool bUseTex         = false;
	bool bDoDraw         = true;
	TCVScalar fNormScale = 0;

	if ((iVarCount < 1) || (iVarCount > 9))
	{
		int piPar[] = { 3, 4, 5, 6, 7, 8, 9 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 7, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToCounter(iColCount))
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToCounter(iRowCount))
	{
		rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}

	if (mVars(2).BaseType() != PDT_VARLIST)
	{
		rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
		return false;
	}

	if (iVarCount >= 4)
	{
		if (mVars(3).BaseType() == PDT_VARLIST)
		{
			bUseCol  = true;
			pColList = mVars(3).GetVarListPtr();
		}
		/*
		else
		{
		rCB.GetErrorList().InvalidParType(mVars(3), 4, iLine, iPos);
		return false;
		}
		*/
	}

	if (iVarCount >= 5)
	{
		int iVal;
		if (!mVars(4).CastToCounter(iVal))
		{
			rCB.GetErrorList().InvalidParType(mVars(4), 5, iLine, iPos);
			return false;
		}

		if (iVal)
		{
			bDoDraw = true;
		}
		else
		{
			bDoDraw = false;
		}
	}

	if (iVarCount >= 6)
	{
		if (mVars(5).BaseType() == PDT_VARLIST)
		{
			bUseNormals = true;
			pNormalList = mVars(5).GetVarListPtr();
		}
		/*
		else
		{
		rCB.GetErrorList().InvalidParType(mVars(6), 7, iLine, iPos);
		return false;
		}
		*/
	}

	if (iVarCount >= 7)
	{
		if (!mVars(6).CastToScalar(fNormScale, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(mVars(6), 7, iLine, iPos);
			return false;
		}
	}

	if (iVarCount >= 8)
	{
		if (mVars(7).BaseType() == PDT_VARLIST)
		{
			bUseTex  = true;
			pTexList = mVars(7).GetVarListPtr();
		}
	}

	CMVInfo<float> MVInfo;
	Mem<COGLVertex> mPoint, mNormal, mTex;
	MemObj<COGLColor> mColor;
	TMultiV vZero;
	TCVScalar dVal;
	bool bIsOrigin;

	TVarList& PList = *mVars(2).GetVarListPtr();
	int iP, iCount = int(PList.Count());

	if (iRowCount * iColCount != iCount)
	{
		rCB.GetErrorList().GeneralError("Number of points given is incompatible to row and column dimensions.",
				iLine, iPos);
		return false;
	}

	if (pNormalList && (iCount != pNormalList->Count()))
	{
		rCB.GetErrorList().GeneralError("List of points and list of normals are of different length.", iLine, iPos);
		return false;
	}

	mPoint.Set(iCount);

	if (bUseNormals)
	{
		mNormal.Set(iCount);
	}

	if (bUseTex)
	{
		mTex.Set(iCount);
	}

	if (bUseCol)
	{
		mColor.Set(iCount);
	}

	for (iP = 0; iP < iCount; iP++)
	{
		bIsOrigin = false;
		if (PList(iP).BaseType() != PDT_MULTIV)
		{
			if (PList(iP).CastToScalar(dVal))
			{
				bIsOrigin = true;
			}
			else
			{
				char pcText[200];
				sprintf_s(pcText, "Element at position %d with value %s is not a multivector.",
						iP + 1, PList[iP].ValStr().Str());
				rCB.GetErrorList().GeneralError(pcText /*"Element in list is not a multivector."*/, iLine, iPos);
				return false;
			}
		}

		if (bUseNormals)
		{
			if ((*pNormalList)(iP).BaseType() != PDT_MULTIV)
			{
				rCB.GetErrorList().GeneralError("Element in normals list is not a multivector.", iLine, iPos);
				return false;
			}

			/////////////////////////////////////////////////
			// Get Normal
			TMultiV& vN = *((TMultiV*) (*pNormalList)[iP].Val());
			// Only analyze multivector
			rCB.GetFilter()->DrawMV(vN, true);
			// Get MVInfo
			MVInfo = rCB.GetFilter()->GetMVInfo();

			if ((MVInfo.m_eType != GA_POINT) && (MVInfo.m_eType != GA_DIR1D))
			{
				rCB.GetErrorList().GeneralError("One of the multivectors passed does not represent a point or a direction.",
						iLine, iPos);
				return false;
			}
			mNormal[iP] = MVInfo.m_mVex[0].Data();
			mNormal[iP].Norm();
		}

		if (bUseTex)
		{
			if ((*pTexList)[iP].BaseType() != PDT_MULTIV)
			{
				rCB.GetErrorList().GeneralError("Element in texture coordinate list is not a multivector.", iLine, iPos);
				return false;
			}

			/////////////////////////////////////////////////
			// Get Texture Coordinate
			TMultiV& vN = *((TMultiV*) (*pTexList)[iP].Val());
			// Only analyze multivector
			rCB.GetFilter()->DrawMV(vN, true);
			// Get MVInfo
			MVInfo = rCB.GetFilter()->GetMVInfo();

			if ((MVInfo.m_eType == GA_POINT) ||
			    (MVInfo.m_eType == GA_DIR1D))
			{
				mTex[iP]    = MVInfo.m_mVex[0].Data();
				mTex[iP][1] = float(1) - mTex[iP][1];
			}
			else if ((MVInfo.m_eType == GA_SCALAR) || (vN == vZero))
			{
				mTex[iP].Set(0, 1, 0);
			}
			else
			{
				rCB.GetErrorList().GeneralError(
						"One of the multivectors passed as texture coordinate does not represent a point or a direction.",
						iLine, iPos);
				return false;
			}
		}

		if (bUseCol)
		{
			if ((*pColList)[iP].BaseType() != PDT_COLOR)
			{
				rCB.GetErrorList().GeneralError("Element in color list is not a color.", iLine, iPos);
				return false;
			}

			mColor[iP] = *((COGLColor*) (*pColList)[iP].Val());
		}

		/////////////////////////////////////////////////
		// Get Point
		if (bIsOrigin)
		{
			mPoint[iP].Set(0, 0, 0);
		}
		else
		{
			TMultiV& vP = *(PList[iP].GetMultiVPtr());
			vZero.SetStyle(vP.GetStyle());
			vZero = 0;

			// Only analyze multivector
			rCB.GetFilter()->DrawMV(vP, true);
			// Get MVInfo
			MVInfo = rCB.GetFilter()->GetMVInfo();

			if (MVInfo.m_eType == GA_POINT)
			{
				mPoint[iP] = MVInfo.m_mVex[0].Data();
			}
			else if ((MVInfo.m_eType == GA_SCALAR) || (vP == vZero))
			{
				mPoint[iP].Set(0, 0, 0);
			}
			else
			{
				char pcText[200];
				sprintf_s(pcText, "The multivector at position %d in list does not represent a point.", iP + 1);
				rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
				return false;
			}
		}
	}

	TScene refSurface, refNormals;
	rCB.GetOGLDrawBase()->DrawPointSurface(iRowCount, iColCount,
			mPoint, mNormal, mTex, mColor,
			float(fNormScale),
			refSurface, refNormals,
			bDoDraw);

	rVar.New(PDT_VARLIST, "List of Scenes");
	TVarList& rVarList = *rVar.GetVarListPtr();

	if (fNormScale != 0)
	{
		rVarList.Set(2);
		rVarList[0] = refNormals;
		rVarList[1] = refSurface;
	}
	else
	{
		rVarList.Set(1);
		rVarList[0] = refSurface;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Point Grid Function
///
/// Expects:
/// 1. Number of columns, ie width in x-direction
/// 2. Number of rowas, ie width in y-direction
/// 3. List of multivectors representing points in E3
/// 4. List of colors for each point (optional)
/// 5. Bool flag indicating whether surface is to be drawn directly (optional)
/// 6. Bool flag indicating whether surface normals are to be negated (optional)
/// 7. List of multivectors representing corresponding normals (optional)
///
/// Returns:
/// List of vertices that may be drawn later on with :-operator

bool DrawPointGridFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!rCB.GetOGLDrawBase())
	{
		return false;
	}

	if (!rCB.GetFilter())
	{
		rCB.GetErrorList().GeneralError("Internal multivector analysis filter not set.", iLine, iPos);
		return false;
	}

	TVarList* pColList    = 0;
	TVarList* pNormalList = 0;
	TVarList& mVars       = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	int iRowCount, iColCount;
	bool bUseCol        = false;
	bool bUseNormals    = false;
	bool bNegateNormals = false;
	bool bDoDraw        = true;

	if ((iVarCount < 1) || (iVarCount > 7))
	{
		int piPar[] = { 3, 4, 5, 6, 7 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 5, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToCounter(iColCount))
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToCounter(iRowCount))
	{
		rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}

	if (mVars(2).BaseType() != PDT_VARLIST)
	{
		rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
		return false;
	}

	if (iVarCount >= 4)
	{
		if (mVars(3).BaseType() != PDT_VARLIST)
		{
			rCB.GetErrorList().InvalidParType(mVars(3), 4, iLine, iPos);
			return false;
		}

		bUseCol  = true;
		pColList = mVars(3).GetVarListPtr();
	}

	if (iVarCount >= 5)
	{
		int iVal;
		if (!mVars(4).CastToCounter(iVal))
		{
			rCB.GetErrorList().InvalidParType(mVars(4), 5, iLine, iPos);
			return false;
		}

		if (iVal)
		{
			bDoDraw = true;
		}
		else
		{
			bDoDraw = false;
		}
	}

	if (iVarCount >= 6)
	{
		int iVal;
		if (!mVars(5).CastToCounter(iVal))
		{
			rCB.GetErrorList().InvalidParType(mVars(5), 6, iLine, iPos);
			return false;
		}

		if (iVal)
		{
			bNegateNormals = true;
		}
		else
		{
			bNegateNormals = false;
		}
	}

	if (iVarCount >= 7)
	{
		if (mVars(6).BaseType() != PDT_VARLIST)
		{
			rCB.GetErrorList().InvalidParType(mVars(6), 7, iLine, iPos);
			return false;
		}

		bUseNormals = true;
		pNormalList = mVars(6).GetVarListPtr();
	}

	CMVInfo<float> MVInfo;
	Mem<COGLVertex> mPoint, mNormal;
	MemObj<COGLColor> mColor;
	TMultiV vZero;
	TCVScalar dVal;
	bool bIsOrigin;

	TVarList& PList = *mVars(2).GetVarListPtr();
	int iP, iCount = int(PList.Count());

	if (iRowCount * iColCount != iCount)
	{
		rCB.GetErrorList().GeneralError("Number of points given is incompatible to row and column dimensions.",
				iLine, iPos);
		return false;
	}

	if (pNormalList && (iCount != pNormalList->Count()))
	{
		rCB.GetErrorList().GeneralError("List of points and list of normals are of different length.", iLine, iPos);
		return false;
	}

	mPoint.Set(iCount);

	if (bUseNormals)
	{
		mNormal.Set(iCount);
	}

	if (bUseCol)
	{
		mColor.Set(iCount);
	}

	for (iP = 0; iP < iCount; iP++)
	{
		bIsOrigin = false;
		if (PList(iP).BaseType() != PDT_MULTIV)
		{
			if (PList(iP).CastToScalar(dVal))
			{
				bIsOrigin = true;
			}
			else
			{
				rCB.GetErrorList().GeneralError("Element in list is not a multivector.", iLine, iPos);
				return false;
			}
		}

		if (bUseNormals)
		{
			if ((*pNormalList)[iP].BaseType() != PDT_MULTIV)
			{
				rCB.GetErrorList().GeneralError("Element in normals list is not a multivector.", iLine, iPos);
				return false;
			}

			/////////////////////////////////////////////////
			// Get Normal
			TMultiV& vN = *((TMultiV*) (*pNormalList)[iP].Val());
			// Only analyze multivector
			rCB.GetFilter()->DrawMV(vN, true);
			// Get MVInfo
			MVInfo = rCB.GetFilter()->GetMVInfo();

			if ((MVInfo.m_eType != GA_POINT) && (MVInfo.m_eType != GA_DIR1D))
			{
				rCB.GetErrorList().GeneralError("One of the multivectors passed does not represent a point or a direction.",
						iLine, iPos);
				return false;
			}
			mNormal[iP] = MVInfo.m_mVex[0].Data();
		}

		if (bUseCol)
		{
			if ((*pColList)[iP].BaseType() != PDT_COLOR)
			{
				rCB.GetErrorList().GeneralError("Element in color list is not a color.", iLine, iPos);
				return false;
			}

			mColor[iP] = *((COGLColor*) (*pColList)[iP].Val());
		}

		/////////////////////////////////////////////////
		// Get Point
		if (bIsOrigin)
		{
			mPoint[iP].Set(0, 0, 0);
		}
		else
		{
			TMultiV& vP = *((TMultiV*) PList[iP].Val());
			vZero.SetStyle(vP.GetStyle());
			vZero = 0;

			// Only analyze multivector
			rCB.GetFilter()->DrawMV(vP, true);
			// Get MVInfo
			MVInfo = rCB.GetFilter()->GetMVInfo();

			if (MVInfo.m_eType == GA_POINT)
			{
				mPoint[iP] = MVInfo.m_mVex[0].Data();
			}
			else if ((MVInfo.m_eType == GA_SCALAR) || (vP == vZero))
			{
				mPoint[iP].Set(0, 0, 0);
			}
			else
			{
				char pcText[200];
				sprintf_s(pcText, "The multivector at position %d in list does not represent a point.", iP + 1);
				rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
				return false;
			}
		}
	}

	TScene refScene;

	if (!rCB.GetOGLDrawBase()->DrawPointGrid(iRowCount, iColCount, mPoint, mNormal, mColor, refScene, bNegateNormals, bDoDraw))
	{
		rCB.GetErrorList().GeneralError("Error drawing point list.", iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Point List Function
///
/// Expects:
/// 1. List of multivectors representing points in E3
/// 2. List of colors for each point (optional)
/// 3. Bool flag indicating whether surface is to be drawn directly (optional)
/// 4. Bool flag indicating whether surface normals are to be negated (optional)
/// 5. List of multivectors representing corresponding normals (optional)
///
/// Returns:
/// List of vertices that may be drawn later on with :-operator

bool DrawPointListFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!rCB.GetOGLDrawBase())
	{
		return false;
	}

	if (!rCB.GetFilter())
	{
		rCB.GetErrorList().GeneralError("Internal multivector analysis filter not set.", iLine, iPos);
		return false;
	}

	TVarList* pColList    = 0;
	TVarList* pNormalList = 0;
	TVarList& mVars       = *rPars.GetVarListPtr();

	int iVarCount       = int(mVars.Count());
	bool bUseCol        = false;
	bool bUseNormals    = false;
	bool bNegateNormals = false;
	bool bDoDraw        = true;

	if ((iVarCount < 1) || (iVarCount > 5))
	{
		int piPar[] = { 1, 2, 3, 4, 5 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 5, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_VARLIST)
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (iVarCount >= 2)
	{
		if (mVars(1).BaseType() == PDT_VARLIST)
		{
			bUseCol  = true;
			pColList = mVars(1).GetVarListPtr();
		}
		/*
		else
		{
		rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
		}
		*/
	}

	if (iVarCount >= 3)
	{
		int iVal;
		if (!mVars(2).CastToCounter(iVal))
		{
			rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}

		if (iVal)
		{
			bDoDraw = true;
		}
		else
		{
			bDoDraw = false;
		}
	}

	if (iVarCount >= 4)
	{
		int iVal;
		if (!mVars(3).CastToCounter(iVal))
		{
			rCB.GetErrorList().InvalidParType(mVars(3), 4, iLine, iPos);
			return false;
		}

		if (iVal)
		{
			bNegateNormals = true;
		}
		else
		{
			bNegateNormals = false;
		}
	}

	if (iVarCount >= 5)
	{
		if (mVars(4).BaseType() != PDT_VARLIST)
		{
			rCB.GetErrorList().InvalidParType(mVars(4), 5, iLine, iPos);
			return false;
		}

		bUseNormals = true;
		pNormalList = mVars(5).GetVarListPtr();
	}

	CMVInfo<float> MVInfo;
	COGLVertex xOrigin;
	TMultiV vZero;
	Mem<COGLVertex> mPoint, mNormal;
	MemObj<COGLColor> mColor;
	TCVScalar dVal;
	bool bIsOrigin;

	xOrigin.Set(0, 0, 0);

	TVarList& PList = *mVars(0).GetVarListPtr();
	int iP, iCount = int(PList.Count());

	if (pNormalList && (iCount != pNormalList->Count()))
	{
		rCB.GetErrorList().GeneralError("List of points and list of normals are of different length.", iLine, iPos);
		return false;
	}

	mPoint.Set(iCount);

	if (bUseNormals)
	{
		mNormal.Set(iCount);
	}

	if (bUseCol)
	{
		mColor.Set(iCount);
	}

	for (iP = 0; iP < iCount; iP++)
	{
		bIsOrigin = false;
		if (PList(iP).BaseType() != PDT_MULTIV)
		{
			if (PList(iP).CastToScalar(dVal))
			{
				bIsOrigin = true;
			}
			else
			{
				rCB.GetErrorList().GeneralError("Element in list is not a multivector.", iLine, iPos);
				return false;
			}
		}

		if (bUseNormals)
		{
			if ((*pNormalList)[iP].BaseType() != PDT_MULTIV)
			{
				rCB.GetErrorList().GeneralError("Element in normals list is not a multivector.", iLine, iPos);
				return false;
			}

			/////////////////////////////////////////////////
			// Get Normal
			TMultiV& vN = *((TMultiV*) (*pNormalList)[iP].Val());
			// Only analyze multivector
			rCB.GetFilter()->DrawMV(vN, true);
			// Get MVInfo
			MVInfo = rCB.GetFilter()->GetMVInfo();

			if ((MVInfo.m_eType != GA_POINT) && (MVInfo.m_eType != GA_DIR1D))
			{
				rCB.GetErrorList().GeneralError("One of the multivectors passed does not represent a point or a direction.",
						iLine, iPos);
				return false;
			}
			mNormal[iP] = MVInfo.m_mVex[0].Data();
		}

		if (bUseCol)
		{
			if ((*pColList)[iP].BaseType() != PDT_COLOR)
			{
				rCB.GetErrorList().GeneralError("Element in color list is not a color.", iLine, iPos);
				return false;
			}

			mColor[iP] = *((COGLColor*) (*pColList)[iP].Val());
		}

		/////////////////////////////////////////////////
		// Get Point
		if (bIsOrigin)
		{
			mPoint[iP].Set(0, 0, 0);
		}
		else
		{
			TMultiV& vP = *((TMultiV*) PList[iP].Val());
			// Only analyze multivector
			rCB.GetFilter()->DrawMV(vP, true);
			// Get MVInfo
			MVInfo = rCB.GetFilter()->GetMVInfo();

			if (MVInfo.m_eType == GA_SCALAR)
			{
				mPoint[iP] = xOrigin;
			}
			else if (MVInfo.m_eType == GA_POINT)
			{
				CPointND<float, 3> rVex = MVInfo.m_mVex[0];
				mPoint[iP] = MVInfo.m_mVex[0].Data();
			}
			else
			{
				rCB.GetErrorList().GeneralError("One of the multivectors passed does not represent a point.", iLine, iPos);
				return false;
			}
		}
	}

	TScene refScene;
	rCB.GetOGLDrawBase()->DrawPointList(mPoint, mNormal, mColor, refScene, bNegateNormals, bDoDraw);

	rVar = refScene;

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Triangle Function

bool DrawTriangleFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!rCB.GetOGLDrawBase())
	{
		return false;
	}

	int iVar;
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVScalar dVal[9];

	if (iVarCount == 9)
	{
		for (iVar = 0; iVar < 9; iVar++)
		{
			if (!mVars(iVar).CastToScalar(dVal[iVar], rCB.GetSensitivity()))
			{
				rCB.GetErrorList().InvalidParType(mVars(iVar), 1, iLine, iPos);
				return false;
			}
		}

		COGLVertex xA, xB, xC;

		xA.Set((float) dVal[0], (float) dVal[1], (float) dVal[2]);
		xB.Set((float) dVal[3], (float) dVal[4], (float) dVal[5]);
		xC.Set((float) dVal[6], (float) dVal[7], (float) dVal[8]);

		rCB.GetOGLDrawBase()->DrawTriangle(xA, xB, xC, true);
	}
	else if (iVarCount == 3)
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

		if (mVars(2).BaseType() != PDT_MULTIV)
		{
			rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}

		TMultiV& vA = *mVars(0).GetMultiVPtr();
		TMultiV& vB = *mVars(1).GetMultiVPtr();
		TMultiV& vC = *mVars(2).GetMultiVPtr();
		TMultiV vX, vY, vZ;

		if (!rCB.CastMVtoE3(vA, vX))
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (!rCB.CastMVtoE3(vB, vY))
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		if (!rCB.CastMVtoE3(vC, vZ))
		{
			rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}

		COGLVertex xA, xB, xC;
		xA.Set((float) vX[E3GA < TCVScalar > ::iE1],
				(float) vX[E3GA < TCVScalar > ::iE2],
				(float) vX[E3GA < TCVScalar > ::iE3]);

		xB.Set((float) vY[E3GA < TCVScalar > ::iE1],
				(float) vY[E3GA < TCVScalar > ::iE2],
				(float) vY[E3GA < TCVScalar > ::iE3]);

		xC.Set((float) vZ[E3GA < TCVScalar > ::iE1],
				(float) vZ[E3GA < TCVScalar > ::iE2],
				(float) vZ[E3GA < TCVScalar > ::iE3]);

		rCB.GetOGLDrawBase()->DrawTriangle(xA, xB, xC, true);
	}
	else
	{
		int piPar[] = { 3, 9 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Cone Function

bool DrawConeFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!rCB.GetOGLDrawBase())
	{
		return false;
	}

	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVScalar dVal, dBool = 0;
	TCVCounter iStepCnt = 16;
	MemObj<COGLColor> mColor;

	if ((iVarCount >= 3) && (iVarCount <= 6))
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

		if (!mVars(2).CastToScalar(dVal, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}

		if (iVarCount >= 4)
		{
			if (!mVars(3).CastToCounter(iStepCnt))
			{
				rCB.GetErrorList().GeneralError(
						"Expect optional fourth parameter to give the number of steps around rim of cone.", iLine, iPos);
				return false;
			}
		}

		if ((iStepCnt <= 2) || (iStepCnt > 360))
		{
			rCB.GetErrorList().GeneralError("Number of visualization steps have to lie in the range 3 to 360.", iLine, iPos);
			return false;
		}

		if (iVarCount >= 5)
		{
			if (!mVars(4).CastToScalar(dBool, rCB.GetSensitivity()))
			{
				rCB.GetErrorList().GeneralError(
						"Expect optional fifth parameter to be boolean indicating whether cone is closed.", iLine, iPos);
				return false;
			}
		}

		if (iVarCount >= 6)
		{
			if (mVars(5).BaseType() != PDT_VARLIST)
			{
				rCB.GetErrorList().GeneralError("Expect as sixth otional parameter a list of three colors.", iLine, iPos);
				return false;
			}

			TVarList& rList = *mVars(5).GetVarListPtr();
			int iCnt        = int(rList.Count());

			if (iCnt != 3)
			{
				rCB.GetErrorList().GeneralError("Expect a list of exactly three colors.", iLine, iPos);
				return false;
			}

			mColor.Set(iCnt);
			for (int i = 0; i < iCnt; ++i)
			{
				if (rList(i).BaseType() != PDT_COLOR)
				{
					rCB.GetErrorList().GeneralError("Element in color list is not a color.", iLine, iPos);
					return false;
				}

				mColor[i] = *rList(i).GetOGLColorPtr();
			}
		}

		TMultiV& vA = *mVars(0).GetMultiVPtr();
		TMultiV& vB = *mVars(1).GetMultiVPtr();
		TMultiV vX, vY;

		if (!rCB.CastMVtoE3(vA, vX))
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (!rCB.CastMVtoE3(vB, vY))
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		COGLVertex xA, xB;
		xA.Set((float) vX[E3GA < TCVScalar > ::iE1],
				(float) vX[E3GA < TCVScalar > ::iE2],
				(float) vX[E3GA < TCVScalar > ::iE3]);

		xB.Set((float) vY[E3GA < TCVScalar > ::iE1],
				(float) vY[E3GA < TCVScalar > ::iE2],
				(float) vY[E3GA < TCVScalar > ::iE3]);

		MemObj<TScene> mScene;

		if (!rCB.GetOGLDrawBase()->DrawCone(xA, xB, float(dVal), mColor,
				    iStepCnt, (dBool ? true : false),
				    &mScene))
		{
			rCB.GetErrorList().GeneralError("Error creating cone object", iLine, iPos);
			return false;
		}

		rVar.New(PDT_VARLIST);
		TVarList& rList = *rVar.GetVarListPtr();

		int iIdx, iCnt = int(mScene.Count());
		rList.Set(iCnt);
		for (iIdx = 0; iIdx < iCnt; ++iIdx)
		{
			rList(iIdx) = mScene[iIdx];
		}
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(3, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Sphere Function

bool DrawSphereFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!rCB.GetOGLDrawBase())
	{
		return false;
	}

	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVScalar dVal, dSolid = 1.0;

	if ((iVarCount < 2) || (iVarCount > 3))
	{
		int piPar[] = { 2, 3 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_MULTIV)
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToScalar(dVal, rCB.GetSensitivity()))
	{
		rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}

	if (iVarCount > 2)
	{
		if (!mVars(2).CastToScalar(dSolid, rCB.GetSensitivity()))
		{
			rCB.GetErrorList().GeneralError("Third parameter has to be boolean to indicate solid/wireframe sphere.", iLine, iPos);
			return false;
		}
	}

	TMultiV& vP = *mVars(0).GetMultiVPtr();
	TMultiV vX;

	if (!rCB.CastMVtoE3(vP, vX))
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	COGLVertex xP;
	xP.Set((float) vX[E3GA < TCVScalar > ::iE1],
			(float) vX[E3GA < TCVScalar > ::iE2],
			(float) vX[E3GA < TCVScalar > ::iE3]);

	// Draw solid sphere
	rCB.GetOGLDrawBase()->DrawSphere(xP, float(dVal), (dSolid ? true : false), false);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Icosahedron Function

bool DrawIcosahedronFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!rCB.GetOGLDrawBase())
	{
		return false;
	}

	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVScalar dRadius;
	TCVCounter iPower, iSolid = 1;

	if ((iVarCount != 3) && (iVarCount != 4))
	{
		int piPar[] = { 3, 4 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_MULTIV)
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToScalar(dRadius, rCB.GetSensitivity()))
	{
		rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}

	if (dRadius < 1e-4)
	{
		rCB.GetErrorList().GeneralError("Radius has to be larger than zero.", iLine, iPos);
		return false;
	}

	if (!mVars(2).CastToCounter(iPower))
	{
		rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
		return false;
	}

	if (iPower > 6)
	{
		rCB.GetErrorList().GeneralError("Evaluation of Icosahedron for a power > 6 will take too long :-(", iLine, iPos);
		return false;
	}

	if (iPower < 0)
	{
		rCB.GetErrorList().GeneralError("A negative power makes no sense here.", iLine, iPos);
		return false;
	}

	if (iVarCount >= 4)
	{
		if (!mVars(3).CastToCounter(iSolid))
		{
			rCB.GetErrorList().InvalidParType(mVars(3), 4, iLine, iPos);
			return false;
		}
	}

	TMultiV& vP = *mVars(0).GetMultiVPtr();
	TMultiV vX;

	if (!rCB.CastMVtoE3(vP, vX))
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	COGLVertex xP;
	xP.Set((float) vX[E3GA < TCVScalar > ::iE1],
			(float) vX[E3GA < TCVScalar > ::iE2],
			(float) vX[E3GA < TCVScalar > ::iE3]);

	// Draw Icosahedron
	rCB.GetOGLDrawBase()->DrawIcosahedron(xP, float(dRadius), iPower, (iSolid ? true : false));

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Quantum Fractal Function
///
/// This is an implementation of 'Quantum Jumps, EEQT and the Five Platonic Fractals'
/// by A. Jadczyk and R. �berg.
///
/// Parameters:
///		- List of detector positions
///		- The value of 'alpha'
///		- The number of points to generate
///		- The start point.
///

bool DrawQuantumFractalFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!rCB.GetOGLDrawBase())
	{
		return false;
	}

	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	TCVScalar dVal;
	float dAlpha;
	TCVCounter iPntCnt, iPnt;
	CMVInfo<TCVScalar> Info;

	if (iVarCount != 4)
	{
		int piPar[] = { 4 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_VARLIST)
	{
		rCB.GetErrorList().GeneralError("Expect list of detector positions as first parameter.", iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToScalar(dVal))
	{
		rCB.GetErrorList().GeneralError("Expect value of 'alpha' as second parameter.", iLine, iPos);
		return false;
	}
	dAlpha = float(dVal);

	if (!mVars(2).CastToCounter(iPntCnt))
	{
		rCB.GetErrorList().GeneralError("Expect number of iterations as third parameters.", iLine, iPos);
		return false;
	}

	if (iPntCnt <= 0)
	{
		rCB.GetErrorList().GeneralError("Point count has to be larger than zero.", iLine, iPos);
		return false;
	}

	if (mVars(3).BaseType() != PDT_MULTIV)
	{
		rCB.GetErrorList().GeneralError("Expect start point as fourth parameter.", iLine, iPos);
		return false;
	}

	rVar.New(PDT_VEXLIST);
	TVexList& rVexList = *rVar.GetVexListPtr();
	//Mem<COGLVertex> &rVex = rVexList.GetVexList();

	TVarList& rList = *mVars(0).GetVarListPtr();
	int iDetector, iDetectorCount = int(rList.Count());
	COGLVertex xR;
	Mem<COGLVertex> mxN;

	rVexList.SetMode(GL_POINTS);

	if (!rVexList.Reserve(iPntCnt))
	{
		rCB.GetErrorList().GeneralError("Insufficient memory available.", iLine, iPos);
		return false;
	}

	// Analyze Multivector
	if (!rCB.GetFilter()->DrawMV(*mVars(3).GetMultiVPtr(), true))
	{
		rCB.GetErrorList().GeneralError("Start point given is not a point.", iLine, iPos);
		return false;
	}

	rCB.GetFilter()->GetMVInfo(Info);

	if (Info.m_eType != GA_POINT)
	{
		rCB.GetErrorList().GeneralError("Start point given is not a point.", iLine, iPos);
		return false;
	}

	CPointND<TCVScalar, 3>& rPnt = Info.m_mVex[0];

	xR.Set(float(rPnt[0]), float(rPnt[1]), float(rPnt[2]));

	mxN.Set(iDetectorCount);

	for (iDetector = 0; iDetector < iDetectorCount; iDetector++)
	{
		if (rList(iDetector).BaseType() != PDT_MULTIV)
		{
			rCB.GetErrorList().GeneralError("Element in detector position list is not a multivector.", iLine, iPos);
			return false;
		}

		// Analyze Multivector
		if (!rCB.GetFilter()->DrawMV(*rList(iDetector).GetMultiVPtr(), true))
		{
			rCB.GetErrorList().GeneralError("Multivector in detector position list is not a point.", iLine, iPos);
			return false;
		}

		rCB.GetFilter()->GetMVInfo(Info);

		if (Info.m_eType != GA_POINT)
		{
			rCB.GetErrorList().GeneralError("Multivector in detector position list is not a point.", iLine, iPos);
			return false;
		}

		CPointND<TCVScalar, 3>& rPnt = Info.m_mVex[0];

		mxN[iDetector].Set(float(rPnt[0]), float(rPnt[1]), float(rPnt[2]));
	}

	///////////////////////////////////////////////////////////////////////

	//	COGLVertexList vlIco;
	float dZ;
	float dAlpha2, dOPAlpha2, dOMAlpha2, dProb, dProbNorm, dNdR, dFac;

	//#define TEST
	#undef TEST

	#ifdef TEST
		COGLVertexList vlIco;
		COGLVertex xX, xY, xZ;

		rCB.GetOGLDrawBase()->GenVexIcosahedron(vlIco, 1.0f, 7);
		Mem<COGLVertex>& rIcoVex  = vlIco.GetVexList();
		Mem<COGLVertex>& rIcoNorm = vlIco.GetNormList();
		Mem<TColor>& rIcoCol      = vlIco.GetColList();
		Mem<int>& rIcoIdx         = vlIco.GetIdxList();
		Mem<float> mIcoHist;
		MemObj< Mem<int> > mIcoIdx;
		CArray2D<int> mPntIdx;
		double dTheta, dPhi;
		int iTheta, iPhi, iIdx, iIdxCount;

		int iIcoBoxCount   = 500;
		double dRatioTheta = double(iIcoBoxCount) / rCB.GetPi();
		double dRatioPhi   = double(iIcoBoxCount) / (2.0 * rCB.GetPi());
		mIcoIdx.Set(iIcoBoxCount * iIcoBoxCount);

		int iIco, iIcoCnt = rIcoVex.Count();
		rIcoCol.Set(iIcoCnt);
		mIcoHist.Set(iIcoCnt);
		mPntIdx.SetDim(iIcoCnt, 13);

		for (iIco = 0; iIco < iIcoCnt; iIco++)
		{
			mIcoHist[iIco]   = 0;
			mPntIdx(iIco, 0) = 0;

			// find theta,phi coordinates of ico corner
			COGLVertex& rVex = rIcoVex[iIco];

			dTheta = acos(rVex[2]);
			if ((rVex[0] == 0) && (rVex[1] == 0))
			{
				dPhi = 0;
			}
			else
			{
				dPhi = atan2(rVex[1], rVex[0]) + rCB.GetPi();
			}

			iTheta = int(floor(dTheta * dRatioTheta));
			iPhi   = int(floor(dPhi * dRatioPhi));

			// Add index of ico corner to index array
			iIdx = iTheta * iIcoBoxCount + iPhi;
			mIcoIdx[iIdx].Add(1);
			mIcoIdx[iIdx].Last() = iIco;

			if (iPhi - 1 < 0)
			{
				iIdx = iTheta * iIcoBoxCount + iIcoBoxCount - 1;
			}
			else
			{
				iIdx = iTheta * iIcoBoxCount + iPhi - 1;
			}

			mIcoIdx[iIdx].Add(1);
			mIcoIdx[iIdx].Last() = iIco;

			if (iPhi + 1 >= iIcoBoxCount)
			{
				iIdx = iTheta * iIcoBoxCount;
			}
			else
			{
				iIdx = iTheta * iIcoBoxCount + iPhi + 1;
			}

			mIcoIdx[iIdx].Add(1);
			mIcoIdx[iIdx].Last() = iIco;

			if (iTheta - 1 < 0)
			{
				iIdx = (iIcoBoxCount - 1) * iIcoBoxCount + iPhi;
			}
			else
			{
				iIdx = (iTheta - 1) * iIcoBoxCount + iPhi;
			}

			mIcoIdx[iIdx].Add(1);
			mIcoIdx[iIdx].Last() = iIco;

			if (iTheta + 1 >= iIcoBoxCount)
			{
				iIdx = iPhi;
			}
			else
			{
				iIdx = (iTheta + 1) * iIcoBoxCount + iPhi;
			}

			mIcoIdx[iIdx].Add(1);
			mIcoIdx[iIdx].Last() = iIco;
		}

		// Create index list that connects index of point with
		// indices of surrounding five points.
		// This is used later to evaluate normals.
		iIdxCount = rIcoIdx.Count();
		for (iIdx = 0; iIdx < iIdxCount; iIdx += 3)
		{
			int iVal, i;

			{
				iIco = rIcoIdx[iIdx];
				int& iFirst = mPntIdx(iIco, 0);
				iVal = rIcoIdx[iIdx + 1];
				//			for (i = 1; i <= iFirst; i++)
				//				if (mPntIdx( iIco, i) == iVal)
				//					break;
				//			if (i > iFirst)
				mPntIdx(iIco, ++iFirst) = iVal;

				iVal = rIcoIdx[iIdx + 2];
				//			for (i = 1; i <= iFirst; i++)
				//				if (mPntIdx( iIco, i) == iVal)
				//					break;
				//			if (i > iFirst)
				mPntIdx(iIco, ++iFirst) = iVal;
			}

			{
				iIco = rIcoIdx[iIdx + 1];
				int& iFirst = mPntIdx(iIco, 0);

				iVal = rIcoIdx[iIdx];
				//			for (i = 1; i <= iFirst; i++)
				//				if (mPntIdx( iIco, i) == iVal)
				//					break;
				//			if (i > iFirst)
				mPntIdx(iIco, ++iFirst) = iVal;

				iVal = rIcoIdx[iIdx + 2];
				//			for (i = 1; i <= iFirst; i++)
				//				if (mPntIdx( iIco, i) == iVal)
				//					break;
				//			if (i > iFirst)
				mPntIdx(iIco, ++iFirst) = iVal;
			}

			{
				iIco = rIcoIdx[iIdx + 2];
				int& iFirst = mPntIdx(iIco, 0);

				iVal = rIcoIdx[iIdx];
				//			for (i = 1; i <= iFirst; i++)
				//				if (mPntIdx( iIco, i) == iVal)
				//					break;
				//			if (i > iFirst)
				mPntIdx(iIco, ++iFirst) = iVal;

				iVal = rIcoIdx[iIdx + 1];
				//			for (i = 1; i <= iFirst; i++)
				//				if (mPntIdx( iIco, i) == iVal)
				//					break;
				//			if (i > iFirst)
				mPntIdx(iIco, ++iFirst) = iVal;
			}
		}

	#endif

	dAlpha2   = dAlpha * dAlpha;
	dOPAlpha2 = 1 + dAlpha2;
	dOMAlpha2 = 1 - dAlpha2;
	dProbNorm = 1.0f / (float(iDetectorCount) * dOPAlpha2);

	// Evaluate Points
	for (iPnt = 0; iPnt < iPntCnt; iPnt++)
	{
		rVexList[iPnt].xVex = xR;

		// Generate Random Number
		dZ = float(rCB.GetRandom().ran3());

		// Find Detector to use
		dProb = 0.0;
		for (iDetector = 0; iDetector < iDetectorCount; iDetector++)
		{
			dNdR   = xR * mxN[iDetector];								// Inner product
			dFac   = dOPAlpha2 + 2 * dAlpha * dNdR;
			dProb += dFac * dProbNorm;
			if (dZ <= dProb)
			{
				break;
			}
		}

		// iDetector has now the value of the detector chosen
		xR = (dOMAlpha2 * xR + 2.0f * dAlpha * (1.0f + dAlpha * dNdR) * mxN[iDetector]);							// / dFac;
		xR.Norm();

		#ifdef TEST
			dTheta = acos(xR[2]);
			if ((xR[0] == 0) && (xR[1] == 0))
			{
				dPhi = 0;
			}
			else
			{
				dPhi = atan2(xR[1], xR[0]) + rCB.GetPi();
			}

			iTheta = int(floor(dTheta * dRatioTheta));
			iPhi   = int(floor(dPhi * dRatioPhi));
			iIdx   = iTheta * iIcoBoxCount + iPhi;

			Mem<int>& rIdx = mIcoIdx[iIdx];
			iIdxCount = rIdx.Count();

			for (iIdx = 0; iIdx < iIdxCount; iIdx++)
			{
				iIco = rIdx[iIdx];

				xX   = rIcoVex[iIco] - xR;
				fVal = xX * xX;

				fVal = 1.0f - fVal / 1e-4f;
				if (fVal > 0.0f)
				{
					mIcoHist[iIco] += fVal;
				}
			}

		#endif
	}

	#ifdef TEST
		float fMax = 0;
		float fCol;
		for (iIco = 0; iIco < iIcoCnt; iIco++)
		{
			if (mIcoHist[iIco] > fMax)
			{
				fMax = mIcoHist[iIco];
			}
		}

		// Generate Color and Heights
		for (iIco = 0; iIco < iIcoCnt; iIco++)
		{
			fCol = float(log(1.0 + log(1.0 + double(mIcoHist[iIco])) / log(1.0 + double(fMax))));
			//		fCol = float( log(1.0 + double(mIcoHist[ iIco ])/double(fMax)) );
			rIcoCol[iIco][0] = fCol;
			rIcoCol[iIco][2] = 1.0f - fCol;

			fVal           = 1.0f + 0.2 * fCol;
			rIcoVex[iIco] *= fVal;
		}

		// Generate Normals
		for (iIco = 0; iIco < iIcoCnt; iIco++)
		{
			int iMax = mPntIdx(iIco, 0);

			xR.Set(0, 0, 0);
			xX = rIcoVex[iIco];
			for (iIdx = 1; iIdx <= iMax; iIdx += 2)
			{
				xY = rIcoVex[mPntIdx(iIco, iIdx)] - xX;
				xZ = rIcoVex[mPntIdx(iIco, iIdx + 1)] - xX;
				xY = xY^xZ;
				if (xY * xX < 0)
				{
					xY = -xY;
				}

				xR = xR + xY;
			}

			xY = xR.Norm();
			//if (xX * xY < 0.0f)
			//rIcoNorm[ iIco ] = -xY;
			//else
			rIcoNorm[iIco] = xY;
		}

		vlIco.Apply();
	#endif

	rVexList.UpdateVexCount();
	//rVexList.Apply();

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Draw Quantum Fractal Surface Function
///
/// This is an implementation of 'Quantum Jumps, EEQT and the Five Platonic Fractals'
/// by A. Jadczyk and R. �berg.
///
/// Parameters:
///		- List of detector positions
///		- The value of 'alpha'
///		- The number of points to generate
///		- The start point.
///		- The subdivision power of the icosahedron to use
///		- The dimension of the sphere index array
///		- Scale of influence of fractal point on icosahedron points
///		- Height Scale
///		- Min Color
///		- Mean Color
///		- Max Color

bool DrawQuantumFractalSurfaceFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	if (!rCB.GetOGLDrawBase())
	{
		return false;
	}

	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());
	int iIcoBoxCount, iIcoPower;

	TCVScalar dVal, dScale;
	float dAlpha, fVal, fHeight;
	TCVCounter iPntCnt, iPnt;
	TColor colMin, colMax, colMean;
	TOGLColor colVal;
	CMVInfo<TCVScalar> Info;

	if (iVarCount != 11)
	{
		int piPar[] = { 11 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_VARLIST)
	{
		rCB.GetErrorList().GeneralError("Expect list of detector positions as first parameter.", iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToScalar(dVal))
	{
		rCB.GetErrorList().GeneralError("Expect value of 'alpha' as second parameter.", iLine, iPos);
		return false;
	}
	dAlpha = float(dVal);

	if (!mVars(2).CastToCounter(iPntCnt))
	{
		rCB.GetErrorList().GeneralError("Expect number of iterations as third parameters.", iLine, iPos);
		return false;
	}

	if (iPntCnt <= 0)
	{
		rCB.GetErrorList().GeneralError("Point count has to be larger than zero.", iLine, iPos);
		return false;
	}

	if (mVars(3).BaseType() != PDT_MULTIV)
	{
		rCB.GetErrorList().GeneralError("Expect start point as fourth parameter.", iLine, iPos);
		return false;
	}

	if (!mVars(4).CastToCounter(iIcoPower))
	{
		rCB.GetErrorList().GeneralError("Expect number of icosahedron subdivisions.", iLine, iPos);
		return false;
	}

	if (iIcoPower <= 0)
	{
		rCB.GetErrorList().GeneralError("Icosahedron subdivision power needs to be higher than zero.", iLine, iPos);
		return false;
	}

	if (!mVars(5).CastToCounter(iIcoBoxCount))
	{
		rCB.GetErrorList().GeneralError("Expect number of elements along each dimension of sphere histogram.", iLine, iPos);
		return false;
	}

	if (iIcoBoxCount < 10)
	{
		rCB.GetErrorList().GeneralError("Number of elements along each dimension of sphere histogram should be larger than 10.", iLine,
				iPos);
		return false;
	}

	if (!mVars(6).CastToScalar(dVal))
	{
		rCB.GetErrorList().GeneralError("Expect value of 'scale' as 7th parameter.", iLine, iPos);
		return false;
	}
	dScale = 1.0 / dVal;

	if (!mVars(7).CastToScalar(dVal))
	{
		rCB.GetErrorList().GeneralError("Expect value of 'height' as 8th parameter.", iLine, iPos);
		return false;
	}
	fHeight = float(dVal);

	if (mVars(8).BaseType() != PDT_COLOR)
	{
		rCB.GetErrorList().GeneralError("Expect minimum height color as 9th parameter.", iLine, iPos);
		return false;
	}
	colMin = mVars(8).GetOGLColorPtr()->Data();

	if (mVars(9).BaseType() != PDT_COLOR)
	{
		rCB.GetErrorList().GeneralError("Expect maximum height color as 10th parameter.", iLine, iPos);
		return false;
	}
	colMean = mVars(9).GetOGLColorPtr()->Data();

	if (mVars(10).BaseType() != PDT_COLOR)
	{
		rCB.GetErrorList().GeneralError("Expect maximum height color as 10th parameter.", iLine, iPos);
		return false;
	}
	colMax = mVars(10).GetOGLColorPtr()->Data();

	rVar.New(PDT_VEXLIST);
	TVexList& vlIco = *rVar.GetVexListPtr();
	//Mem<COGLVertex> &rVex = rVexList.GetVexList();

	TVarList& rList = *mVars(0).GetVarListPtr();
	int iDetector, iDetectorCount = int(rList.Count());
	COGLVertex xR;
	Mem<COGLVertex> mxN;
	/*
	rVexList.SetMode( GL_POINTS );

	if ( !rVex.Set( iPntCnt ) )
	{
	rCB.GetErrorList().GeneralError("Insufficient memory available.", iLine, iPos);
	return false;
	}
	*/

	// Analyze Multivector
	if (!rCB.GetFilter()->DrawMV(*mVars(3).GetMultiVPtr(), true))
	{
		rCB.GetErrorList().GeneralError("Start point given is not a point.", iLine, iPos);
		return false;
	}

	rCB.GetFilter()->GetMVInfo(Info);

	if (Info.m_eType != GA_POINT)
	{
		rCB.GetErrorList().GeneralError("Start point given is not a point.", iLine, iPos);
		return false;
	}

	CPointND<TCVScalar, 3>& rPnt = Info.m_mVex[0];

	xR.Set(float(rPnt[0]), float(rPnt[1]), float(rPnt[2]));

	mxN.Set(iDetectorCount);

	for (iDetector = 0; iDetector < iDetectorCount; iDetector++)
	{
		if (rList(iDetector).BaseType() != PDT_MULTIV)
		{
			rCB.GetErrorList().GeneralError("Element in detector position list is not a multivector.", iLine, iPos);
			return false;
		}

		// Analyze Multivector
		if (!rCB.GetFilter()->DrawMV(*rList(iDetector).GetMultiVPtr(), true))
		{
			rCB.GetErrorList().GeneralError("Multivector in detector position list is not a point.", iLine, iPos);
			return false;
		}

		rCB.GetFilter()->GetMVInfo(Info);

		if (Info.m_eType != GA_POINT)
		{
			rCB.GetErrorList().GeneralError("Multivector in detector position list is not a point.", iLine, iPos);
			return false;
		}

		CPointND<TCVScalar, 3>& rPnt = Info.m_mVex[0];

		mxN[iDetector].Set(float(rPnt[0]), float(rPnt[1]), float(rPnt[2]));
	}

	///////////////////////////////////////////////////////////////////////

	//	COGLVertexList vlIco;
	float dZ;
	float dAlpha2, dOPAlpha2, dOMAlpha2, dProb, dProbNorm, dNdR, dFac;

	#define TEST
	//#undef TEST

	#ifdef TEST
		//COGLVertexList vlIco;
		COGLVertex xX, xY, xZ;

		rCB.GetOGLDrawBase()->GenVexIcosahedron(vlIco, 1.0f, iIcoPower);
		//Mem<COGLVertex> &rIcoVex = vlIco.GetVexList();
		//Mem<COGLVertex> &rIcoNorm = vlIco.GetNormList();
		//Mem<TColor> &rIcoCol = vlIco.GetColList();
		MemObj<Mem<unsigned> >& rIdxList = vlIco.GetIdxList();
		rIdxList.Set(1);
		Mem<unsigned>& rIcoIdx = rIdxList[0];
		Mem<float> mIcoHist;
		MemObj< Mem<int> > mIcoIdx;
		CArray2D<int> mPntIdx;
		double dTheta, dPhi;
		int iTheta, iPhi, iIdx, iIdxCount;

		double dRatioTheta = double(iIcoBoxCount) / rCB.GetPi();
		double dRatioPhi   = double(iIcoBoxCount) / (2.0 * rCB.GetPi());
		mIcoIdx.Set(iIcoBoxCount * iIcoBoxCount);

		int iIco, iIcoCnt = vlIco.Count();
		//rIcoCol.Set( iIcoCnt );
		mIcoHist.Set(iIcoCnt);
		mPntIdx.SetDim(iIcoCnt, 13);

		for (iIco = 0; iIco < iIcoCnt; iIco++)
		{
			mIcoHist[iIco]   = 0;
			mPntIdx(iIco, 0) = 0;

			// find theta,phi coordinates of ico corner
			COGLVertex& rVex = vlIco[iIco].xVex;

			dTheta = acos(rVex[2]);
			if ((rVex[0] == 0) && (rVex[1] == 0))
			{
				dPhi = 0;
			}
			else
			{
				dPhi = atan2(rVex[1], rVex[0]) + rCB.GetPi();
			}

			iTheta = int(floor(dTheta * dRatioTheta));
			iPhi   = int(floor(dPhi * dRatioPhi));

			// Add index of ico corner to index array
			iIdx = iTheta * iIcoBoxCount + iPhi;
			mIcoIdx[iIdx].Add(1);
			mIcoIdx[iIdx].Last() = iIco;

			if (iPhi - 1 < 0)
			{
				iIdx = iTheta * iIcoBoxCount + iIcoBoxCount - 1;
			}
			else
			{
				iIdx = iTheta * iIcoBoxCount + iPhi - 1;
			}

			mIcoIdx[iIdx].Add(1);
			mIcoIdx[iIdx].Last() = iIco;

			if (iPhi + 1 >= iIcoBoxCount)
			{
				iIdx = iTheta * iIcoBoxCount;
			}
			else
			{
				iIdx = iTheta * iIcoBoxCount + iPhi + 1;
			}

			mIcoIdx[iIdx].Add(1);
			mIcoIdx[iIdx].Last() = iIco;

			if (iTheta - 1 >= 0)
			{
				//iIdx = (iIcoBoxCount - 1) * iIcoBoxCount + iPhi;
				//else
				iIdx = (iTheta - 1) * iIcoBoxCount + iPhi;

				mIcoIdx[iIdx].Add(1);
				mIcoIdx[iIdx].Last() = iIco;
			}

			if (iTheta + 1 < iIcoBoxCount)
			{
				//iIdx = iPhi;
				//else
				iIdx = (iTheta + 1) * iIcoBoxCount + iPhi;

				mIcoIdx[iIdx].Add(1);
				mIcoIdx[iIdx].Last() = iIco;
			}
		}

		// Create index list that connects index of point with
		// indices of surrounding five points.
		// This is used later to evaluate normals.
		iIdxCount = int(rIcoIdx.Count());
		for (iIdx = 0; iIdx < iIdxCount; iIdx += 3)
		{
			int iVal;

			{
				iIco = rIcoIdx[iIdx];
				int& iFirst = mPntIdx(iIco, 0);
				iVal = rIcoIdx[iIdx + 1];
				//			for (i = 1; i <= iFirst; i++)
				//				if (mPntIdx( iIco, i) == iVal)
				//					break;
				//			if (i > iFirst)
				mPntIdx(iIco, ++iFirst) = iVal;

				iVal = rIcoIdx[iIdx + 2];
				//			for (i = 1; i <= iFirst; i++)
				//				if (mPntIdx( iIco, i) == iVal)
				//					break;
				//			if (i > iFirst)
				mPntIdx(iIco, ++iFirst) = iVal;
			}

			{
				iIco = rIcoIdx[iIdx + 1];
				int& iFirst = mPntIdx(iIco, 0);

				iVal = rIcoIdx[iIdx];
				//			for (i = 1; i <= iFirst; i++)
				//				if (mPntIdx( iIco, i) == iVal)
				//					break;
				//			if (i > iFirst)
				mPntIdx(iIco, ++iFirst) = iVal;

				iVal = rIcoIdx[iIdx + 2];
				//			for (i = 1; i <= iFirst; i++)
				//				if (mPntIdx( iIco, i) == iVal)
				//					break;
				//			if (i > iFirst)
				mPntIdx(iIco, ++iFirst) = iVal;
			}

			{
				iIco = rIcoIdx[iIdx + 2];
				int& iFirst = mPntIdx(iIco, 0);

				iVal = rIcoIdx[iIdx];
				//			for (i = 1; i <= iFirst; i++)
				//				if (mPntIdx( iIco, i) == iVal)
				//					break;
				//			if (i > iFirst)
				mPntIdx(iIco, ++iFirst) = iVal;

				iVal = rIcoIdx[iIdx + 1];
				//			for (i = 1; i <= iFirst; i++)
				//				if (mPntIdx( iIco, i) == iVal)
				//					break;
				//			if (i > iFirst)
				mPntIdx(iIco, ++iFirst) = iVal;
			}
		}

	#endif

	dAlpha2   = dAlpha * dAlpha;
	dOPAlpha2 = 1 + dAlpha2;
	dOMAlpha2 = 1 - dAlpha2;
	dProbNorm = 1.0f / (float(iDetectorCount) * dOPAlpha2);

	// Evaluate Points
	for (iPnt = 0; iPnt < iPntCnt; iPnt++)
	{
		//rVex[ iPnt ] = 1.05f * xR;

		// Generate Random Number
		dZ = float(rCB.GetRandom().ran3());

		// Find Detector to use
		dProb = 0.0;
		for (iDetector = 0; iDetector < iDetectorCount; iDetector++)
		{
			dNdR   = xR * mxN[iDetector];								// Inner product
			dFac   = dOPAlpha2 + 2 * dAlpha * dNdR;
			dProb += dFac * dProbNorm;
			if (dZ <= dProb)
			{
				break;
			}
		}

		// iDetector has now the value of the detector chosen
		xR = (dOMAlpha2 * xR + 2.0f * dAlpha * (1.0f + dAlpha * dNdR) * mxN[iDetector]);							// / dFac;
		xR.Norm();

		#ifdef TEST
			dTheta = acos(xR[2]);
			if ((xR[0] == 0) && (xR[1] == 0))
			{
				dPhi = 0;
			}
			else
			{
				dPhi = atan2(xR[1], xR[0]) + rCB.GetPi();
			}

			iTheta = int(floor(dTheta * dRatioTheta));
			iPhi   = int(floor(dPhi * dRatioPhi));
			iIdx   = iTheta * iIcoBoxCount + iPhi;

			Mem<int>& rIdx = mIcoIdx[iIdx];
			iIdxCount = int(rIdx.Count());

			for (iIdx = 0; iIdx < iIdxCount; iIdx++)
			{
				iIco = rIdx[iIdx];

				xX   = vlIco[iIco].xVex - xR;
				fVal = xX * xX;

				//fVal = 1.0f - fVal / 1e-4f;
				fVal = float(exp(-pow(double(fVal) * dScale, 2)));
				//if (fVal > 0.0f)
				mIcoHist[iIco] += fVal;
			}

		#endif
	}

	#ifdef TEST
		float fMax = 0;
		float fCol;
		for (iIco = 0; iIco < iIcoCnt; iIco++)
		{
			if (mIcoHist[iIco] > fMax)
			{
				fMax = mIcoHist[iIco];
			}
		}

		//double dNorm = 1.0 / log(2.0);
		// Generate Color and Heights
		for (iIco = 0; iIco < iIcoCnt; iIco++)
		{
			fCol = float(log(1.0 + double(mIcoHist[iIco])) / log(1.0 + double(fMax)));
			//		fCol = float( dNorm * log( 1.0 + log(1.0 + double(mIcoHist[ iIco ])) / log(1.0 + double(fMax)) ) );
			//		fCol = float( log(1.0 + double(mIcoHist[ iIco ])/double(fMax)) );
			vlIco[iIco].xCol = colMax * fCol + colMin * (1.0f - fCol)
					   + colMean * (1.0f - fCol) * fCol * 4.0f;
			//rIcoCol[ iIco ] = colMin * (1.0f - fCol);

			fVal              = 1.0f + fHeight * fCol;
			vlIco[iIco].xVex *= fVal;
		}

		// Generate Normals
		for (iIco = 0; iIco < iIcoCnt; iIco++)
		{
			int iMax = mPntIdx(iIco, 0);

			xR.Set(0, 0, 0);
			xX = vlIco[iIco].xVex;
			for (iIdx = 1; iIdx <= iMax; iIdx += 2)
			{
				xY = vlIco[mPntIdx(iIco, iIdx)].xVex - xX;
				xZ = vlIco[mPntIdx(iIco, iIdx + 1)].xVex - xX;
				xY = xY^xZ;
				if (xY * xX < 0)
				{
					xY = -xY;
				}

				xR = xR + xY;
			}

			xY = xR.Norm();
			//if (xX * xY < 0.0f)
			//rIcoNorm[ iIco ] = -xY;
			//else
			vlIco[iIco].xNorm = xY;
		}

		//vlIco.Apply();
	#endif

	return true;
}
