////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      CodeLoop.cpp
//
// summary:   Implements the code loop class
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

#include "StdAfx.h"
#include "CodeLoop.h"
#include "CodeBase.h"
#include "CodeVar.h"

#include <iostream>

using namespace std;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCodeLoop::CCodeLoop()
{
}

CCodeLoop::~CCodeLoop()
{

}


//////////////////////////////////////////////////////////////////////
/// Apply

bool CCodeLoop::Apply(CCodeBase* pCodeBase, SCodeData *pData)
{
	if (!pCodeBase)
		return false;

	bool bBreak = false;
	int iLine, iPos;
	if (pData)
	{
		iLine = pData->iTextLine;
		iPos = pData->iTextPos;
	}

	int iStackCount = pCodeBase->ActStackCount();

	TCodeVarPtr pVarLoop;

	if (iStackCount == 1)
	{
		if (!pCodeBase->Pop(pVarLoop))
			return false;

		if (!pVarLoop)
		{
			pCodeBase->m_ErrorList.GeneralError("No loop code available.", iLine, iPos);
			return false;
		}

		//pVarLoop->ConvertSELtoSE();

		// Is variable of type CodePtr then execute code
		if (pVarLoop && pVarLoop->Type() == PDT_CODEPTR)
		{
			// Execute code
			CCodeElementList *pCode = dynamic_cast<CCodeElementList*>
				(*((CCodeElement **) pVarLoop->Val()));
			if (pCode)
			{
				//int iTempVarCount, iCurTempVarCount;
				int iCL, iCount = pCode->Count();
				CCodeVar *pDVar;

				// Lock Stack
				pCodeBase->LockStack();

				for(iCL=0;iCL<iCount;iCL++)
				{
					// Empty Stack up to lock
					// while(pCodeBase->Pop(pDVar));

					//iTempVarCount = pCodeBase->m_mTempVarList.Count();
					if (!(*pCode)[iCL]->Apply(pCodeBase, pData))
					{
						return false;
					}
					//iCurTempVarCount = pCodeBase->m_mTempVarList.Count();
					//pCodeBase->m_mTempVarList.Del(iTempVarCount, iCurTempVarCount-iTempVarCount);
				}

				// Empty stack up to lock
				while(pCodeBase->Pop(pDVar))
				{
					// If CODEPTR variable is left on stack
					// then execute it since it is a code block
					// in curly brackets.
					if (pDVar && pDVar->Type() == PDT_CODEPTR)
					{
						// Execute code
						CCodeElementList *pCode = dynamic_cast<CCodeElementList*>
							(*((CCodeElement **) pDVar->Val()));
						if (pCode)
						{
							int iTempVarCount, iCurTempVarCount;
							int iCL, iCount = pCode->Count();
							CCodeVar *pDVar;

							// Lock Stack
							pCodeBase->LockStack();

							int iMaxLoopCount = pCodeBase->GetLoopCountLimit();
							int iLoop = 0;
							do
							{
								for(iCL=0;iCL<iCount;iCL++)
								{
									// Empty Stack up to lock
									while(pCodeBase->Pop(pDVar));

									iTempVarCount = pCodeBase->TempVarCount();
									if (!(*pCode)[iCL]->Apply(pCodeBase, pData))
									{
										if (pCodeBase->m_ErrorList.Last().iLevel == CEL_INTERNAL &&
											pCodeBase->m_ErrorList.Last().iNo == CERR_BREAK)
										{
											bBreak = true;
											break;
										}
										return false;
									}
									iCurTempVarCount = pCodeBase->TempVarCount();
									pCodeBase->DeleteTempVar(iTempVarCount, iCurTempVarCount-iTempVarCount);
									//pCodeBase->m_mTempVarList.Del(iTempVarCount, iCurTempVarCount-iTempVarCount);
								}

								if (++iLoop >= iMaxLoopCount)
								{
									pCodeBase->m_ErrorList.LoopMaxCountReached(iLine, iPos);
									return false;
									//bBreak = true;
								}
							} while (!bBreak);

							// Empty stack up to lock
							while(pCodeBase->Pop(pDVar));

							// Unlock Stack
							pCodeBase->UnlockStack();
						}
					}
				} // while

				// Unlock Stack
				pCodeBase->UnlockStack();
			}
		}
	}
	else
	{
		// Error
		pCodeBase->m_ErrorList.GeneralError("CCodeLoop::Apply: Too many elements on stack.", iLine, iPos);
		return false;
	}

	return true;
}

