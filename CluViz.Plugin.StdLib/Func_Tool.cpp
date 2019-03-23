////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_Tool.cpp
//
// summary:   Implements the function tool class
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

#include "Func_Tool.h"

//////////////////////////////////////////////////////////////////////
/// Add a slider tool

bool  ToolSliderFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	int i;
	string sName, sToolTip;
	const char* pcToolTip = 0;
	TCVScalar dVal[4];

	if ((iVarCount != 5) && (iVarCount != 6) && (iVarCount != 1))
	{
		int piParNo[] = { 1, 5, 6 };
		rCB.GetErrorList().WrongNoOfParams(piParNo, 3, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	sName = mVars(0).ValStr();

	if (iVarCount >= 5)
	{
		for (i = 0; i < 4; i++)
		{
			if (!(mVars(i + 1).CastToScalar(dVal[i], rCB.GetSensitivity())))
			{
				rCB.GetErrorList().InvalidParType(mVars(i + 1), i + 2, iLine, iPos);
				return false;
			}
		}

		if (iVarCount >= 6)
		{
			if (mVars(5).BaseType() != PDT_STRING)
			{
				rCB.GetErrorList().InvalidParType(mVars(5), 6, iLine, iPos);
				return false;
			}

			sToolTip  = mVars(5).ValStr();
			pcToolTip = sToolTip.c_str();
		}

		if (!rCB.GetCLUDrawBase()->AddToolSlider(sName.c_str(), dVal[0], dVal[1], dVal[2], dVal[3], false, pcToolTip))
		{
			rCB.GetErrorList().GeneralError("Cannot create slider", iLine, iPos);
			return false;
		}
	}

	double dValue;
	rCB.GetCLUDrawBase()->GetToolValue(sName.c_str(), dValue);

	rVar = TCVScalar(dValue);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add a counter tool

bool  ToolCounterFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	rCB.GetErrorList().GeneralError("Function not supported", iLine, iPos);
	return false;
}

//////////////////////////////////////////////////////////////////////
/// Add a input tool

bool  ToolInputFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	int i;
	string sName, sToolTip;
	const char* pcToolTip = 0;
	TCVScalar dVal[3];

	if ((iVarCount != 4) && (iVarCount != 5) && (iVarCount != 1))
	{
		int piParNo[] = { 1, 4, 5 };
		rCB.GetErrorList().WrongNoOfParams(piParNo, 3, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	sName = mVars(0).ValStr();

	if (iVarCount >= 4)
	{
		for (i = 0; i < 3; i++)
		{
			if (!(mVars(i + 1).CastToScalar(dVal[i], rCB.GetSensitivity())))
			{
				rCB.GetErrorList().InvalidParType(mVars(i + 1), i + 2, iLine, iPos);
				return false;
			}
		}

		if (iVarCount >= 5)
		{
			if (mVars(4).BaseType() != PDT_STRING)
			{
				rCB.GetErrorList().InvalidParType(mVars(4), 5, iLine, iPos);
				return false;
			}

			sToolTip  = mVars(4).ValStr();
			pcToolTip = sToolTip.c_str();
		}

		if (!rCB.GetCLUDrawBase()->AddToolInput(sName.c_str(), dVal[0], dVal[1], dVal[2], false, pcToolTip))
		{
			rCB.GetErrorList().GeneralError("Cannot create input", iLine, iPos);
			return false;
		}
	}

	double dValue;
	rCB.GetCLUDrawBase()->GetToolValue(sName.c_str(), dValue);

	rVar = TCVScalar(dValue);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add a input text tool

bool  ToolInputTextFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	string sName, sToolTip;
	const char* pcToolTip = 0;

	string sVal;
	TCVScalar dVal;

	if ((iVarCount != 3) && (iVarCount != 4) && (iVarCount != 1))
	{
		int piParNo[] = { 1, 3, 4 };
		rCB.GetErrorList().WrongNoOfParams(piParNo, 3, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	sName = mVars(0).ValStr();

	if (iVarCount >= 3)
	{
		if (mVars(2).BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}

		sVal = mVars(2).ValStr();

		if (!(mVars(1).CastToScalar(dVal, rCB.GetSensitivity())))
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		if (iVarCount >= 4)
		{
			if (mVars(3).BaseType() != PDT_STRING)
			{
				rCB.GetErrorList().InvalidParType(mVars(3), 4, iLine, iPos);
				return false;
			}

			sToolTip  = mVars(3).ValStr();
			pcToolTip = sToolTip.c_str();
		}

		if (!rCB.GetCLUDrawBase()->AddToolInputText(sName.c_str(), (dVal ? true : false), sVal.c_str(), false, pcToolTip))
		{
			rCB.GetErrorList().GeneralError("Cannot create text input.", iLine, iPos);
			return false;
		}
	}

	rCB.GetCLUDrawBase()->GetToolValue(sName.c_str(), sVal);

	rVar = sVal.c_str();

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add a check box tool

bool  ToolCheckBoxFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	string sName, sToolTip;
	const char* pcToolTip = 0;
	TCVScalar dVal;

	if ((iVarCount != 2) && (iVarCount != 3) && (iVarCount != 1))
	{
		int piParNo[] = { 1, 2, 3 };
		rCB.GetErrorList().WrongNoOfParams(piParNo, 3, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	sName = mVars(0).ValStr();

	if (iVarCount >= 2)
	{
		if (!(mVars(1).CastToScalar(dVal, rCB.GetSensitivity())))
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		if (iVarCount >= 3)
		{
			if (mVars(2).BaseType() != PDT_STRING)
			{
				rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
				return false;
			}

			sToolTip  = mVars(2).ValStr();
			pcToolTip = sToolTip.c_str();
		}

		if (!rCB.GetCLUDrawBase()->AddToolCheckBox(sName.c_str(), ((dVal != TCVScalar(0)) ? true : false), false, pcToolTip))
		{
			rCB.GetErrorList().GeneralError("Cannot create check box.", iLine, iPos);
			return false;
		}
	}

	double dValue;
	rCB.GetCLUDrawBase()->GetToolValue(sName.c_str(), dValue);

	rVar = TCVScalar(dValue);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add a button tool

bool  ToolButtonFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	string sName, sToolTip;
	const char* pcToolTip = 0;

	if ((iVarCount != 1) && (iVarCount != 2))
	{
		int piParNo[] = { 1, 2 };
		rCB.GetErrorList().WrongNoOfParams(piParNo, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	sName = mVars(0).ValStr();

	if (iVarCount >= 2)
	{
		if (mVars(1).BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		sToolTip  = mVars(1).ValStr();
		pcToolTip = sToolTip.c_str();
	}

	if (!rCB.GetCLUDrawBase()->AddToolButton(sName.c_str(), false, pcToolTip))
	{
		rCB.GetErrorList().GeneralError("Cannot create button.", iLine, iPos);
		return false;
	}

	double dValue;
	rCB.GetCLUDrawBase()->GetToolValue(sName.c_str(), dValue);

	rVar = TCVScalar(dValue);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add an image button tool

bool ToolImageButtonFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	rCB.GetErrorList().GeneralError("Function not supported", iLine, iPos);
	return false;
}

//////////////////////////////////////////////////////////////////////
/// Add a Choice tool

bool  ToolChoiceFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	string sName, sToolTip;
	const char* pcToolTip = 0;
	TCVCounter iVal;

	if ((iVarCount != 3) && (iVarCount != 4) && (iVarCount != 1))
	{
		int piParNo[] = { 1, 3, 4 };
		rCB.GetErrorList().WrongNoOfParams(piParNo, 3, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	sName = mVars(0).ValStr();

	if (iVarCount >= 3)
	{
		if (mVars(1).BaseType() != PDT_VARLIST)
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		TVarList& rData = *mVars(1).GetVarListPtr();
		TString csChoice;

		if (mVars(2).CastToCounter(iVal))
		{
			if ((iVal > 0) && (iVal <= int(rData.Count())))
			{
				csChoice = rData[iVal - 1].ValStr();
			}
			else
			{
				rCB.GetErrorList().GeneralError("Index value is out of range.", iLine, iPos);
				return false;
			}
		}
		else if (mVars(2).BaseType() == PDT_STRING)
		{
			csChoice = *mVars(2).GetStringPtr();
		}
		else
		{
			rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}

		vector<string> vecData;
		int i, iCount = int(rData.Count());
		vecData.resize(iCount);

		for (i = 0; i < iCount; i++)
		{
			vecData[i] = rData[i].ValStr().Str();
		}

		if (iVarCount >= 4)
		{
			if (mVars(3).BaseType() != PDT_STRING)
			{
				rCB.GetErrorList().InvalidParType(mVars(3), 4, iLine, iPos);
				return false;
			}

			sToolTip  = mVars(3).ValStr();
			pcToolTip = sToolTip.c_str();
		}

		if (!rCB.GetCLUDrawBase()->AddToolChoice(sName.c_str(), vecData, csChoice.Str(), false, pcToolTip))
		{
			rCB.GetErrorList().GeneralError("Cannot create choice tool.", iLine, iPos);
			return false;
		}
	}

	double dValue;
	rCB.GetCLUDrawBase()->GetToolValue(sName.c_str(), dValue);

	rVar = TCVScalar(dValue);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add an image button toolbar object

bool  ToolbarImageButtonFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	string sName, sToolTip;
	TImage* pImgAct;//, *pImgDeact;
	const char* pcToolTip = 0;

	if ((iVarCount < 1) || (iVarCount > 3))
	{
		int piParNo[] = { 1, 2, 3 };
		rCB.GetErrorList().WrongNoOfParams(piParNo, 3, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	sName = mVars(0).ValStr();

	if (iVarCount > 1)
	{
		if (mVars(1).BaseType() != PDT_IMAGE)
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		pImgAct = mVars(1).GetImagePtr();

		if (iVarCount >= 3)
		{
			if (mVars(2).BaseType() != PDT_STRING)
			{
				rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
				return false;
			}

			sToolTip  = mVars(2).ValStr();
			pcToolTip = sToolTip.c_str();
		}

		if (!rCB.GetCLUDrawBase()->AddToolbarImageButton(sName.c_str(), *pImgAct, 0, true, pcToolTip))
		{
			rCB.GetErrorList().GeneralError("Cannot create image button.", iLine, iPos);
			return false;
		}
	}

	double dValue;
	rCB.GetCLUDrawBase()->GetToolValue(sName.c_str(), dValue);

	rVar = TCVScalar(dValue);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add a counter tool

bool  ToolbarCounterFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	int i;
	string sName, sToolTip, sUnit;
	const char* pcToolTip = 0;
	TCVScalar dVal[5];
	bool bActive = true;

	if ((iVarCount != 6) && (iVarCount != 7) && (iVarCount != 8) && (iVarCount != 9) && (iVarCount != 1))
	{
		int piParNo[] = { 1, 6, 7, 8, 9 };
		rCB.GetErrorList().WrongNoOfParams(piParNo, 5, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	sName = mVars(0).ValStr();

	if (iVarCount >= 6)
	{
		for (i = 0; i < 5; i++)
		{
			if (!(mVars(i + 1).CastToScalar(dVal[i], rCB.GetSensitivity())))
			{
				rCB.GetErrorList().InvalidParType(mVars(i + 1), i + 2, iLine, iPos);
				return false;
			}
		}

		if (iVarCount >= 7)
		{
			if (mVars(6).BaseType() != PDT_STRING)
			{
				rCB.GetErrorList().InvalidParType(mVars(6), 7, iLine, iPos);
				return false;
			}

			sUnit = mVars(6).ValStr();
		}

		if (iVarCount >= 8)
		{
			if (mVars(7).BaseType() != PDT_STRING)
			{
				rCB.GetErrorList().InvalidParType(mVars(6), 7, iLine, iPos);
				return false;
			}

			sToolTip  = mVars(7).ValStr();
			pcToolTip = sToolTip.c_str();
		}

		if (iVarCount >= 9)
		{
			TCVCounter iVal;
			if (!mVars(8).CastToCounter(iVal))
			{
				rCB.GetErrorList().GeneralError("Expect parameter no. 9 to be true/false.", iLine, iPos);
				return false;
			}

			bActive = (iVal ? true : false);
		}

		if (!rCB.GetCLUDrawBase()->AddToolbarCounter(sName.c_str(), sUnit.c_str(), dVal[0], dVal[1], dVal[2],
				    dVal[3], dVal[4], bActive, true, pcToolTip))
		{
			rCB.GetErrorList().GeneralError("Cannot create stepper for toolbar.", iLine, iPos);
			return false;
		}
	}

	double dValue;
	rCB.GetCLUDrawBase()->GetToolValue(sName.c_str(), dValue);

	rVar = TCVScalar(dValue);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Add toolbar label

bool  ToolbarLabelFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	string sName, sToolTip;
	const char* pcToolTip = 0;

	if ((iVarCount != 1) && (iVarCount != 2))
	{
		int piParNo[] = { 1, 2 };
		rCB.GetErrorList().WrongNoOfParams(piParNo, 2, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	sName = mVars(0).ValStr();
	if (sName.length() == 0)
	{
		rCB.GetErrorList().GeneralError("Tool name cannot be empty.", iLine, iPos);
		return false;
	}

	if (iVarCount >= 2)
	{
		if (mVars(1).BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		sToolTip  = mVars(1).ValStr();
		pcToolTip = sToolTip.c_str();
	}

	if (!rCB.GetCLUDrawBase()->AddToolbarLabel(sName.c_str(), true, pcToolTip))
	{
		rCB.GetErrorList().GeneralError("Cannot create label for toolbar.", iLine, iPos);
		return false;
	}

	return true;
}
