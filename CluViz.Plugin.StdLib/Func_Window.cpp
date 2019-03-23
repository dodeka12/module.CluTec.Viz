////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_Window.cpp
//
// summary:   Implements the function Windows Form
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

#include "Func_Window.h"

//////////////////////////////////////////////////////////////////////
/// Set OpenGL Window Size Function

bool  SetOGLSizeFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());
	TCVScalar dWidth, dHeight;

	if (iVarCount != 2)
	{
		rCB.GetErrorList().WrongNoOfParams(2, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToScalar(dWidth))
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (!mVars(1).CastToScalar(dHeight))
	{
		rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
		return false;
	}

	rCB.GetCLUDrawBase()->SetOGLSize(dWidth, dHeight);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set OpenGL Window Size Function

bool  GetOGLSizeFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());
	TCVScalar dWidth, dHeight;

	if (iVarCount != 0)
	{
		rCB.GetErrorList().WrongNoOfParams(0, iLine, iPos);
		return false;
	}

	rCB.GetCLUDrawBase()->GetOGLSize(dWidth, dHeight);

	rVar.New(PDT_VARLIST);
	TVarList& rList = *rVar.GetVarListPtr();

	rList.Set(2);
	rList(0) = dWidth;
	rList(1) = dHeight;

	return true;
}

//////////////////////////////////////////////////////////////////////
// Enable Menu
//

bool  EnableMenuFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());
	TCVCounter iVal;

	if (iVarCount != 1)
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToCounter(iVal))
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	rCB.GetCLUDrawBase()->EnableMenu((iVal ? true : false));

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Menu
///
/// Parameter: Expects a nested list detailing the menu
///
/// [
///		{[ "Name", {"key+ctrl+alt+shift"|0}, {"invisible|inactive|divider"|0},
///				{"checkbox|radio"|0}, selected: {true|false}, "callback name" ]
///		|[ "Name", [ { Submenu Item List } ] ] }
/// ]
///
/// Allowed callback names are:
///		CB::Animate, CB::MouseAnimate, CB::LocalRotation, CB::FullScreen,
///		CB::ResetView, CB::ResetMouseModes, CB::SaveImage, CB::QuickSaveImage
///		CB::Quit
///

bool  SetMenuFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount != 1)
	{
		int piParNo[] = { 1 };
		rCB.GetErrorList().WrongNoOfParams(piParNo, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_VARLIST)
	{
		rCB.GetErrorList().GeneralError("Expect a list detailing the menu as parameter.", iLine, iPos);
		return false;
	}

	vector< CCLUDrawBase::SMenuItem > vecMenu;

	if (!ParseMenu(rCB, vecMenu, *mVars(0).GetVarListPtr(), iLine, iPos))
	{
		return false;
	}

	rCB.GetCLUDrawBase()->SetMenu(vecMenu);

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Parse Menu Item Description from nested list to SMenuItem elements

bool  ParseMenu(CCLUCodeBase& rCB,  vector< CCLUDrawBase::SMenuItem >& vecMenu, TVarList& rList, int iLine, int iPos)
{
	int iIdx, iCnt = int(rList.Count());
	CCLUDrawBase::SMenuItem MenuItem;

	for (iIdx = 0; iIdx < iCnt; ++iIdx)
	{
		if (rList(iIdx).BaseType() != PDT_VARLIST)
		{
			char pcText[500];
			sprintf_s(pcText, "Element no. %d in list has to be a list giving the menu item definition.", iIdx + 1);
			rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
			return false;
		}

		TVarList& rItem = *rList(iIdx).GetVarListPtr();
		MenuItem.Reset();

		if (rItem.Count() == 0)
		{
			rCB.GetErrorList().GeneralError("Empty menu items are not allowed.", iLine, iPos);
			return false;
		}

		if (rItem(0).BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().GeneralError("First element of menu item has to be a string giving its name.", iLine, iPos);
			return true;
		}

		// Set Menu Name
		MenuItem.sName = rItem(0).GetStringPtr()->Str();
		MenuItem.eType = CCLUDrawBase::/*EMenuItemType::*/ MI_TEXT;

		// If there are no more entries to the menu item parse them
		if (rItem.Count() >= 2)
		{
			// Check for submenu
			if (rItem(1).BaseType() == PDT_VARLIST)
			{
				MenuItem.eType = CCLUDrawBase::/*EMenuItemType::*/ MI_SUBMENU;
				vecMenu.push_back(MenuItem);

				// Parse Submenu
				if (!ParseMenu(rCB, vecMenu, *rItem(1).GetVarListPtr(), iLine, iPos))
				{
					return false;
				}
			}
			else
			{
				if (rItem(1).BaseType() == PDT_STRING)
				{
					// Parse shortcut key
					TString csKey = *rItem(1).GetStringPtr();

					if (csKey.Len() > 0)
					{
						if ((csKey[1] == 0) || (csKey[1] == '+'))
						{
							// First character in string is the key itself
							MenuItem.cKey = csKey[0];
						}
						else
						{
							if (strstr(csKey.Str(), "esc"))
							{
								MenuItem.cKey = char(CCLUDrawBase::MKEY_ESCAPE);
							}
							else if (csKey[0] == 'f')
							{
								if (isdigit(csKey[1]) && !isdigit(csKey[2]))
								{
									char pcDig[2];
									pcDig[0] = csKey[1];
									pcDig[1] = 0;

									MenuItem.cKey = char(CCLUDrawBase::MKEY_F1) + (char) atoi(pcDig) - 1;
								}
								else if (isdigit(csKey[1]) && isdigit(csKey[2]))
								{
									char pcDig[3];
									pcDig[0] = csKey[1];
									pcDig[1] = csKey[2];
									pcDig[2] = 0;
									int iFNo = atoi(pcDig);
									if (iFNo > 12)
									{
										iFNo = 12;
									}

									MenuItem.cKey = char(CCLUDrawBase::MKEY_F1) + (char) iFNo - 1;
								}
							}
						}

						// Check for modifiers
						if (strstr(csKey.Str(), "ctrl"))
						{
							MenuItem.eModifier
								= CCLUDrawBase::EMenuKeyModifier(int(MenuItem.eModifier) |
									int(CCLUDrawBase::/*EMenuKeyModifier::*/ MKEY_CTRL));
						}

						if (strstr(csKey.Str(), "alt"))
						{
							MenuItem.eModifier
								= CCLUDrawBase::EMenuKeyModifier(int(MenuItem.eModifier) |
									int(CCLUDrawBase::/*EMenuKeyModifier::*/ MKEY_ALT));
						}

						if (strstr(csKey.Str(), "shift"))
						{
							MenuItem.eModifier
								= CCLUDrawBase::EMenuKeyModifier(int(MenuItem.eModifier) |
									int(CCLUDrawBase::/*EMenuKeyModifier::*/ MKEY_SHIFT));
						}
					}
				}

				// Check whether properties are given
				if ((rItem.Count() >= 3) && (rItem(2).BaseType() == PDT_STRING))
				{
					// Parse property string
					TString csProp = *rItem(2).GetStringPtr();

					if (strstr(csProp.Str(), "inactive"))
					{
						MenuItem.bInactive = true;
					}

					if (strstr(csProp.Str(), "invisible"))
					{
						MenuItem.bInvisible = true;
					}

					if (strstr(csProp.Str(), "divider"))
					{
						MenuItem.bDivider = true;
					}
				}

				bool bCanBeSelected = false;

				// Check whether menu item type is specified
				if ((rItem.Count() >= 4) && (rItem(3).BaseType() == PDT_STRING))
				{
					// Parse menu item type
					TString csType = *rItem(3).GetStringPtr();

					if (strstr(csType.Str(), "checkbox"))
					{
						MenuItem.eType = CCLUDrawBase::/*EMenuItemType::*/ MI_CHECKBOX;
						bCanBeSelected = true;
					}
					else if (strstr(csType.Str(), "radio"))
					{
						MenuItem.eType = CCLUDrawBase::/*EMenuItemType::*/ MI_RADIO;
						bCanBeSelected = true;
					}
				}

				// Check whether checkbox or radiobutton is selected
				if ((rItem.Count() >= 5) && bCanBeSelected)
				{
					TCVCounter iVal;

					if (!rItem(4).CastToCounter(iVal))
					{
						rCB.GetErrorList().GeneralError(
								"The fifth entry of a menu item specification has to specify whether "
								"a checkbox or radio button entry is selected.", iLine, iPos);
						return false;
					}

					MenuItem.bSelected = (iVal ? true : false);
				}

				// Check whether callback function is specified
				if ((rItem.Count() >= 6) && (rItem(5).BaseType() == PDT_STRING))
				{
					// Parse menu item type
					MenuItem.sCallback = *rItem(5).GetStringPtr();
				}

				// Store Menu Item
				vecMenu.push_back(MenuItem);
			}
		}
		else
		{
			vecMenu.push_back(MenuItem);
		}
	}

	// Push End Menu Item
	MenuItem.Reset();
	vecMenu.push_back(MenuItem);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Enable Standard Ctrl
//

bool  EnableStdCtrlFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());
	TCVCounter iVal;

	if (iVarCount != 1)
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToCounter(iVal))
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	rCB.GetCLUDrawBase()->EnableStdCtrl((iVal ? true : false));

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Info Text

bool  SetInfoTextFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	string sText;

	if (iVarCount != 1)
	{
		int piParNo[] = { 1 };
		rCB.GetErrorList().WrongNoOfParams(piParNo, 1, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_STRING)
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	sText = mVars(0).ValStr();

	if (!rCB.GetCLUDrawBase()->SetInfoText(sText.c_str()))
	{
		rCB.GetErrorList().GeneralError("Cannot set info text.", iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Set Info Width

bool  SetInfoWidthFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();

	int iVarCount = int(mVars.Count());

	TCVScalar dVal;
	string sText;

	if (iVarCount != 1)
	{
		int piParNo[] = { 1 };
		rCB.GetErrorList().WrongNoOfParams(piParNo, 1, iLine, iPos);
		return false;
	}

	if (!mVars(0).CastToScalar(dVal, rCB.GetSensitivity()))
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	if (dVal < 0)
	{
		rCB.GetErrorList().GeneralError("Width of information window has to be greater than zero.", iLine, iPos);
		return false;
	}

	if (dVal != rCB.GetCurrentInfoWidth())
	{
		if (!rCB.GetCLUDrawBase()->SetInfoWidth(dVal))
		{
			rCB.GetErrorList().GeneralError("Cannot set width of information window.", iLine, iPos);
			return false;
		}

		rCB.GetCurrentInfoWidth() = dVal;
	}

	return true;
}
