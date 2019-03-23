////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_List.cpp
//
// summary:   Implements the function list class
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
#include "Func_List.h"

#include "CluTec.Viz.Base\TensorOperators.h"

#include <vector>
#include <algorithm>
#include <functional>		// For greater<int>( )

using namespace std;

//////////////////////////////////////////////////////////////////////
/// The Argument FUNCTION
/// Returns a list of arguments, for which the element passed with these arguments is true.

bool  ArgTrueFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars             = *rPars.GetVarListPtr();
	int iVarCount               = int(mVars.Count());
	TCVCounter iRetainStructure = 0;

	if ((iVarCount == 0) || (iVarCount > 2))
	{
		int piPar[] = { 1, 2 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 2, iLine, iPos);
		return false;
	}

	if (iVarCount == 2)
	{
		if (!mVars(1).CastToCounter(iRetainStructure))
		{
			rCB.GetErrorList().GeneralError("Expect integer value 0 or 1 as second parameter.", iLine, iPos);
			return false;
		}
	}

	ECodeDataType eType = mVars(0).BaseType();

	if (eType == PDT_MATRIX)
	{
		int iR, iC, iRows, iCols;
		TMatrix& xM = *mVars(0).GetMatrixPtr();
		iRows = xM.Rows();
		iCols = xM.Cols();

		rVar.New(PDT_VARLIST);
		TVarList& rList = *rVar.GetVarListPtr();

		for (iR = 1; iR <= iRows; iR++)
		{
			/*
			rList.Add(1);
			rList.Last().New(PDT_VARLIST);
			TVarList &rRowList = *rList.Last().GetVarListPtr();
			*/
			for (iC = 1; iC <= iCols; iC++)
			{
				if (xM(iR, iC) != TCVScalar(0))
				{
					rList.Add(1);
					rList.Last().New(PDT_VARLIST);

					TVarList& rArg = *rList.Last().GetVarListPtr();
					rArg.Set(2);
					rArg[0] = iR;
					rArg[1] = iC;
				}
			}

			/*
			if (rRowList.Count() == 0)
			{
			rList.Sub(1);
			}
			*/
		}
	}
	else if (eType == PDT_VARLIST)
	{
		/*
		TVarList& rList = *mVars(0).GetVarListPtr();
		int i, iCount = rList.Count();
		*/
		rVar.New(PDT_VARLIST);
		TVarList& rRetList = *rVar.GetVarListPtr();

		TVarList IdxList;

		if (!ArgTrueList(rCB, rRetList, IdxList, mVars(0), (iRetainStructure ? true : false),
				    iLine, iPos))
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}
	}
	else
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// ArgTrueList. Return arguments of list where content is not equal to zero

bool  ArgTrueList(CCLUCodeBase& rCB, TVarList& rArgList, TVarList& rIdxList, CCodeVar& rData, bool bRetainStructure, int iLine, int iPos)
{
	TCVScalar dVal;

	if (rData.BaseType() == PDT_VARLIST)
	{
		TVarList& rList = *rData.GetVarListPtr();
		int i, iCount = int(rList.Count());
		TVarList* pSubArgList;

		if (rIdxList.Count() == 0)
		{
			pSubArgList = &rArgList;
		}
		else
		{
			rArgList.Add(1);
			rArgList.Last().New(PDT_VARLIST);
			pSubArgList = rArgList.Last().GetVarListPtr();
		}

		TVarList CurIdxList = rIdxList;
		CurIdxList.Add(1);

		for (i = 0; i < iCount; i++)
		{
			CurIdxList.Last() = i + 1;

			if (!ArgTrueList(rCB, *pSubArgList, CurIdxList, rList(i), bRetainStructure, iLine, iPos))
			{
				return false;
			}
		}

		// If sublist of arguments is empty, then remove it again.
		if (!bRetainStructure && (pSubArgList->Count() == 0) && (rIdxList.Count() > 0))
		{
			rArgList.Sub(1);
		}
	}
	else if (rData.CastToScalar(dVal, rCB.GetSensitivity()))
	{
		if (dVal != TCVScalar(0))
		{
			rArgList.Add(1);
			rArgList.Last() = rIdxList;
		}
	}
	else
	{
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Sorting

bool  SortFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());
	int iAscend, iExecute;
	vector<int> vecSortIdx;
	bool bSortExecute = true;

	rCB.m_pSortCode   = 0;
	rCB.m_bSortAscend = true;
	rCB.m_iSortLine   = iLine;
	rCB.m_iSortPos    = iPos;

	if ((iVarCount < 1) || (iVarCount > 3))
	{
		int piPar[] = { 1, 2, 3 };

		rCB.GetErrorList().WrongNoOfParams(piPar, 3, iLine, iPos);
		return false;
	}

	if (iVarCount > 1)
	{
		if (!mVars(1).CastToCounter(iAscend))
		{
			if (mVars(1).BaseType() != PDT_CODEPTR)
			{
				rCB.GetErrorList().GeneralError("Invalid sorting type.", iLine, iPos);
				return false;
			}

			rCB.m_pSortCode = *mVars(1).GetCodePtrPtr();
		}

		rCB.m_bSortAscend = (iAscend > 0 ? true : false);
	}

	if (iVarCount > 2)
	{
		if (!mVars(2).CastToCounter(iExecute))
		{
			rCB.GetErrorList().GeneralError("Expect 'true' or 'false' as third parameter.", iLine, iPos);
			return false;
		}

		bSortExecute = (iExecute > 0 ? true : false);
	}

	if (mVars(0).BaseType() == PDT_VARLIST)
	{
		rCB.sm_pCurCodeBase = &rCB;
		rCB.m_pSortVarList  = mVars(0).GetVarListPtr();

		int iPos, iCount = int(rCB.m_pSortVarList->Count());
		vecSortIdx.resize(iCount);

		for (iPos = 0; iPos < iCount; iPos++)
		{
			vecSortIdx[iPos] = iPos;
		}

		try
		{
			/// Compare Function uses sm_pCurCodeBase to sort m_pSortVarList.
			stable_sort(vecSortIdx.begin(), vecSortIdx.end(), CCLUCodeBase::StaticListSortCompare);
		}
		catch (CCLUCodeBase::SortError& rEx)
		{
			rEx.GetErrorLevel();
			return false;
		}
		catch (CCluException& rEx)
		{
			rCB.GetErrorList().GeneralError(rEx.PrintError().c_str(), iLine, iPos);
			return false;
		}

		// Return value of sort is index list of sorted elements
		rVar.New(PDT_VARLIST);
		TVarList& rIdxList = *rVar.GetVarListPtr();

		rIdxList.Set(iCount);
		for (iPos = 0; iPos < iCount; iPos++)
		{
			CCodeVar& rEl = rIdxList[iPos];
			rEl.New(PDT_VARLIST);
			TVarList& rSubList = *rEl.GetVarListPtr();

			rSubList.Set(1);
			rSubList[0] = vecSortIdx[iPos] + 1;
		}

		if (bSortExecute)
		{
			rCB.m_pSortVarList->Order(vecSortIdx);
		}
	}
	else
	{
		rCB.GetErrorList().GeneralError("Cannot sort variable of given type.", iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Cast To List

bool  ListFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());
	int iVal;

	if ((iVarCount < 1) || (iVarCount > 2))
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	if (mVars(0).CastToCounter(iVal))
	{
		// iVal gives length of new list
		if (iVal < 0)
		{
			rCB.GetErrorList().GeneralError("Lenght of new list has to be >= 0.", iLine, iPos);
			return false;
		}

		rVar.New(PDT_VARLIST);
		TVarList& rList = *rVar.GetVarListPtr();

		rList.Set(iVal);
		for (int i = 0; i < iVal; i++)
		{
			rList[i] = TCVScalar(0);
		}
	}
	else if (mVars(0).BaseType() == PDT_MULTIV)
	{
		TMultiV& vA = *mVars(0).GetMultiVPtr();
		int i, iDim = vA.GetGADim();

		rVar.New(PDT_VARLIST);
		TVarList& rList = *rVar.GetVarListPtr();

		rList.Set(iDim);

		for (i = 0; i < iDim; i++)
		{
			rList[i] = vA[i];
		}
	}
	else if (mVars(0).BaseType() == PDT_MATRIX)
	{
		TMatrix& rM = *mVars(0).GetMatrixPtr();
		int iR, iRows = rM.Rows();
		int iC, iCols = rM.Cols();
		int iMPos;
		TCVScalar* pData = rM.Data();

		rVar.New(PDT_VARLIST);
		TVarList& rList = *rVar.GetVarListPtr();

		rList.Set(iRows);

		for (iR = 0, iMPos = 0; iR < iRows; iR++)
		{
			rList[iR].New(PDT_VARLIST);
			TVarList& rRow = *rList[iR].GetVarListPtr();
			rRow.Set(iCols);

			for (iC = 0; iC < iCols; iC++, iMPos++)
			{
				rRow[iC] = pData[iMPos];
			}
		}
	}
	else if (mVars(0).BaseType() == PDT_STRING)
	{
		if (iVarCount < 2)
		{
			rCB.GetErrorList().GeneralError("Expect delimiter string as second parameter.", iLine, iPos);
			return false;
		}

		if (mVars(1).BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().GeneralError("Expect delimiter string as second parameter.", iLine, iPos);
			return false;
		}

		TString rStr  = *mVars(0).GetStringPtr();	// Copy String!
		TString& rDel = *mVars(1).GetStringPtr();

		rVar.New(PDT_VARLIST);
		TVarList& rList = *rVar.GetVarListPtr();

		int iDelCount = int(rDel.Len());

		if (iDelCount == 0)
		{
			rCB.GetErrorList().GeneralError("Delimiter string cannot be empty.", iLine, iPos);
			return false;
		}

		char* pcStr = rStr.Str();
		char* pcDel = rDel.Str();
		char* pcCur = pcStr;
		char* pcEnd;

		while ((pcEnd = strstr(pcCur, pcDel)))
		{
			*pcEnd = 0;
			rList.Add(1);
			rList.Last() = pcCur;
			pcCur        = &pcEnd[iDelCount];
		}

		// Copy rest of string which was not ended
		// by a delimiter.
		if (pcCur)
		{
			rList.Add(1);
			rList.Last() = pcCur;
		}
	}
	else
	{
		rCB.GetErrorList().GeneralError("Casting of given element to list is not implemented.", iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Transpose Elements in List
/// This can transform index lists into lists of numbers and vice versa.
/// Parameters:
///		1. The list
///		2. The sublevel at which transpose is to be executed.
///
// TODO: Documentation for this function. Called 'TransList' in CLUScript.

bool  TransposeListElementsFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());
	TCVCounter iRecurseDepth;

	if ((iVarCount != 1) && (iVarCount != 2))
	{
		rCB.GetErrorList().WrongNoOfParams(1, iLine, iPos);
		return false;
	}

	if (iVarCount == 2)
	{
		if (!mVars(1).CastToCounter(iRecurseDepth))
		{
			rCB.GetErrorList().GeneralError("Second parameter has to be counter if given.", iLine, iPos);
			return false;
		}
	}
	else
	{
		iRecurseDepth = 0;
	}

	if (mVars(0).BaseType() != PDT_VARLIST)
	{
		rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
		return false;
	}

	rVar.New(PDT_VARLIST);
	TVarList& rTransList = *rVar.GetVarListPtr();
	TVarList& rList      = *mVars(0).GetVarListPtr();

	return TransposeListElements(rCB, rTransList, rList, iRecurseDepth, iLine, iPos);
}

bool  TransposeListElements(CCLUCodeBase& rCB, TVarList& rTransList, TVarList& rList, int iRecurseDepth, int iLine, int iPos)
{
	Mem<TVarList*> mSubList;
	int i, j, iListCount = int(rList.Count());
	int iCnt, iMaxCnt = 0;

	if (iListCount == 0)
	{
		return true;
	}

	if (iRecurseDepth > 0)
	{
		rTransList.Set(iListCount);
		for (i = 0; i < iListCount; i++)
		{
			rTransList(i).New(PDT_VARLIST);
			TVarList& rSubTransList = *rTransList(i).GetVarListPtr();

			if (rList(i).BaseType() != PDT_VARLIST)
			{
				rCB.GetErrorList().GeneralError("Transpose on given level not possible.", iLine, iPos);
				return false;
			}
			TVarList& rSubList = *rList(i).GetVarListPtr();

			if (!TransposeListElements(rCB, rSubTransList, rSubList, iRecurseDepth - 1, iLine, iPos))
			{
				return false;
			}
		}

		return true;
	}

	// Find Maximum row length
	for (i = 0; i < iListCount; i++)
	{
		if (rList(i).BaseType() == PDT_VARLIST)
		{
			iCnt = int(rList(i).GetVarListPtr()->Count());
			if (iCnt > iMaxCnt)
			{
				iMaxCnt = iCnt;
			}
		}
		else if (1 > iMaxCnt)
		{
			iMaxCnt = 1;
		}
	}

	rTransList.Set(iMaxCnt);
	mSubList.Set(iMaxCnt);
	for (i = 0; i < iMaxCnt; i++)
	{
		rTransList(i).New(PDT_VARLIST);
		mSubList[i] = rTransList(i).GetVarListPtr();
	}

	for (i = 0; i < iListCount; i++)
	{
		if (rList(i).BaseType() == PDT_VARLIST)
		{
			TVarList& rSubList = *rList(i).GetVarListPtr();

			iCnt = int(rSubList.Count());
			for (j = 0; j < iCnt; j++)
			{
				mSubList[j]->Add(1);
				mSubList[j]->Last() = rSubList(j);
			}
		}
		else
		{
			mSubList[0]->Add(1);
			mSubList[0]->Last() = rList(i);
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Extract Elements from List
// TODO: Documentation for this function. Called 'SubList' in CLUScript.

bool  ExtractListElementsFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	TCVCounter iFirst, iCount;

	if (iVarCount == 2)
	{
		if (mVars(0).BaseType() != PDT_VARLIST)
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (mVars(1).BaseType() != PDT_VARLIST)
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		rVar.New(PDT_VARLIST);
		TVarList& rSubList = *rVar.GetVarListPtr();

		TVarList& rList    = *mVars(0).GetVarListPtr();
		TVarList& rIdxList = *mVars(1).GetVarListPtr();

		TCVCounter iIdx;
		int i, iCount = int(rIdxList.Count());
		int iListCount = int(rList.Count());
		rSubList.Set(iCount);

		for (i = 0; i < iCount; i++)
		{
			if (rIdxList(i).BaseType() == PDT_VARLIST)
			{
				if (!rCB.GetVarListElement(rSubList[i], rList, *rIdxList(i).GetVarListPtr(), 0, iLine, iPos))
				{
					return false;
				}

				// GetVarListElement returns pointer to element. Need to copy this
				// to a new element, since pointer will not be valid after initial
				// list does not exist anymore.
				rSubList[i].DereferencePtr();
			}
			else if (rIdxList(i).CastToCounter(iIdx))
			{
				if ((iIdx <= 0) || (iIdx > iListCount))
				{
					CStrMem csText;
					csText << "Index out of range at position " << (i + 1) << " in index list.";
					rCB.GetErrorList().GeneralError(csText.Str(), iLine, iPos);
					return false;
				}

				rSubList[i] = rList(iIdx - 1);
			}
			else
			{
				CStrMem csText;
				csText << "Invalid index at position " << (i + 1) << " in index list.";
				rCB.GetErrorList().GeneralError(csText.Str(), iLine, iPos);
				return false;
			}
		}
	}
	else if (iVarCount == 3)
	{
		if (mVars(0).BaseType() != PDT_VARLIST)
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (!mVars(1).CastToCounter(iFirst))
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		if (!mVars(2).CastToCounter(iCount))
		{
			rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}

		// List containing free variable, plot range and step.
		rVar = *mVars(0).GetVarListPtr();
		TVarList& List = *rVar.GetVarListPtr();

		int iLCnt = (int) List.Count();

		if ((iFirst < 1) || (iFirst > iLCnt))
		{
			rCB.GetErrorList().InvalidParVal(mVars(0), 1, iLine, iPos);
			return false;
		}
		iFirst--;

		if (iCount == 0)
		{
			rVar.New(PDT_VARLIST);
			return true;
		}

		if (iCount < 0)
		{
			iCount = iLCnt - iFirst;
		}
		else if (iFirst + iCount > iLCnt)
		{
			rCB.GetErrorList().InvalidParVal(mVars(1), 2, iLine, iPos);
			return false;
		}

		if (iFirst > 0)
		{
			List.Del((unsigned) 0, (unsigned) iFirst);
		}

		iFirst += iCount;
		if (iFirst < iLCnt)
		{
			List.Del((unsigned) iFirst, (unsigned) (iLCnt - iFirst));
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
/// Remove Elements from List
// TODO: Documentation for this function. Called 'RemList' in CLUScript.

bool  RemoveListElementsFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	TCVCounter iFirst, iCount;

	if (iVarCount == 2)
	{
		TCVCounter iIdx;

		if (mVars(0).BaseType() != PDT_VARLIST)
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (mVars(1).CastToCounter(iIdx))
		{
			TVarList& rList = *mVars(0).GetVarListPtr();
			int iListCount  = int(rList.Count());

			if ((iIdx <= 0) || (iIdx > iListCount))
			{
				rCB.GetErrorList().GeneralError("Index out of range.", iLine, iPos);
				return false;
			}

			rList.Del(iIdx - 1);
		}
		else if (mVars(1).BaseType() == PDT_VARLIST)
		{
			TVarList& rList    = *mVars(0).GetVarListPtr();
			TVarList& rIdxList = *mVars(1).GetVarListPtr();

			TCVCounter iIdx;
			int i, iCount = int(rIdxList.Count());
			int iListCount = int(rList.Count());
			vector<int> vecIdx;
			vecIdx.resize(iCount);

			for (i = 0; i < iCount; i++)
			{
				if (!rIdxList(i).CastToCounter(iIdx))
				{
					CStrMem csText;
					csText << "Invalid index at position " << (i + 1) << " in index list.";
					rCB.GetErrorList().GeneralError(csText.Str(), iLine, iPos);
					return false;
				}

				vecIdx[i] = iIdx;
			}

			// Sort indices in descending order
			sort(vecIdx.begin(), vecIdx.end(), greater<int>());

			for (i = 0; i < iCount; i++)
			{
				iListCount = int(rList.Count());
				iIdx       = vecIdx[i];

				if ((iIdx <= 0) || (iIdx > iListCount))
				{
					CStrMem csText;
					csText << "Index out of range at position " << (i + 1) << " in index list.";
					rCB.GetErrorList().GeneralError(csText.Str(), iLine, iPos);
					return false;
				}

				rList.Del(iIdx - 1);

				while (i + 1 < iCount && iIdx == vecIdx[i + 1])
					i++;

				/*
				if (rIdxList(i).BaseType() == PDT_VARLIST)
				{
				        if (!RemoveVarListElement(rList, *rIdxList(i).GetVarListPtr(), 0, iLine, iPos))
				                return false;
				}
				else
				if (rIdxList(i).CastToCounter(iIdx))
				{
				        if (iIdx <= 0 || iIdx > iListCount)
				        {
				                cStr csText;
				                csText << "Index out of range at position " << (i+1) << " in index list.";
				                rCB.GetErrorList().GeneralError(csText.Str(), iLine, iPos);
				                return false;
				        }

				        rList.Del(iIdx-1);
				}
				else
				{
				        cStr csText;
				        csText << "Invalid index at position " << (i+1) << " in index list.";
				        rCB.GetErrorList().GeneralError(csText.Str(), iLine, iPos);
				        return false;
				}
				*/
			}
		}
		else
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}
	}
	else if (iVarCount == 3)
	{
		if (mVars(0).BaseType() != PDT_VARLIST)
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (!mVars(1).CastToCounter(iFirst))
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		if (!mVars(2).CastToCounter(iCount))
		{
			rCB.GetErrorList().InvalidParType(mVars(2), 3, iLine, iPos);
			return false;
		}

		TVarList& List = *mVars(0).GetVarListPtr();

		int iLCnt = (int) List.Count();

		if ((iFirst < 1) || (iFirst > iLCnt))
		{
			rCB.GetErrorList().InvalidParVal(mVars(0), 1, iLine, iPos);
			return false;
		}
		iFirst--;

		if (iCount == 0)
		{
			rVar.New(PDT_VARLIST);
			return true;
		}

		if (iCount < 0)
		{
			iCount = iLCnt - iFirst;
		}
		else if (iFirst + iCount > iLCnt)
		{
			rCB.GetErrorList().InvalidParVal(mVars(1), 2, iLine, iPos);
			return false;
		}

		List.Del((unsigned) iFirst, (unsigned) iCount);

		//rVar = List;
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(3, iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Insert Elements into List
///
/// Parameters:
///		1. List where to insert elements
///		2. Position before which to insert
///		3. Either:
///			a) Number of zero elements to insert.
///			b) List of elements to insert.
///
///	Return:
///		Void
//////////////////////////////////////////////////////////////////////

bool  InsertListElementsFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());
	TCVCounter iInsPos, iInsCount;

	if (iVarCount != 3)
	{
		rCB.GetErrorList().WrongNoOfParams(3, iLine, iPos);
		return false;
	}

	if (mVars(0).BaseType() != PDT_VARLIST)
	{
		rCB.GetErrorList().GeneralError("First parameter has to be a variable list.", iLine, iPos);
		return false;
	}

	TVarList& rList       = *mVars(0).GetVarListPtr();
	TCVCounter iListCount = int(rList.Count());

	if (!mVars(1).CastToCounter(iInsPos))
	{
		rCB.GetErrorList().GeneralError("Second parameter has to be the position before which to insert.", iLine, iPos);
		return false;
	}

	if ((iInsPos < 1) || (iInsPos > iListCount + 1))
	{
		rCB.GetErrorList().GeneralError("Invalid insert position.", iLine, iPos);
		return false;
	}

	if (mVars(2).CastToCounter(iInsCount))
	{
		// Insert zero elements

		if (iInsCount < 0)
		{
			rCB.GetErrorList().GeneralError("Invalid number of elements to insert.", iLine, iPos);
			return false;
		}

		if (iInsCount == 0)
		{
			return true;
		}

		--iInsPos;
		rList.Insert((uint) iInsPos, (uint) iInsCount);

		for (int iPos = 0; iPos < iInsCount; iPos++)
		{
			rList[iInsPos + iPos] = TCVCounter(0);
		}
	}
	else if (mVars(2).BaseType() == PDT_VARLIST)
	{
		// Insert list elements

		TVarList& rInsList = *mVars(2).GetVarListPtr();

		if (&rList == &rInsList)
		{
			rCB.GetErrorList().GeneralError("Cannot insert list into itself.", iLine, iPos);
			return false;
		}

		iInsCount = int(rInsList.Count());
		if (iInsCount == 0)
		{
			return true;
		}

		--iInsPos;
		rList.Insert((uint) iInsPos, (uint) iInsCount);

		for (int iPos = 0; iPos < iInsCount; iPos++)
		{
			rList[iInsPos + iPos] = rInsList[iPos];
		}
	}
	else
	{
		rCB.GetErrorList().GeneralError("Third parameter has to be either a number or a list.", iLine, iPos);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Index list of combinations
// Expects number of elements to choose from and number of elements to combine
// Returns list of indices giving combinations.

bool  CombinationIndexListFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	TCVCounter iChoose, iCount;

	if (iVarCount == 2)
	{
		if (!mVars(0).CastToCounter(iCount))
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (!mVars(1).CastToCounter(iChoose))
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		if ((iCount <= 0) || (iCount > 50))
		{
			rCB.GetErrorList().GeneralError("Number of elements out of range.", iLine, iPos);
			return false;
		}

		if ((iChoose <= 0) || (iChoose > iCount))
		{
			rCB.GetErrorList().GeneralError("Number of elements to choose out of range.", iLine, iPos);
			return false;
		}

		int* piIdxList = new int[iChoose];
		if (!piIdxList)
		{
			rCB.GetErrorList().OutOfMemory(iLine, iPos);
			return false;
		}

		MemObj<Mem<int> > mCombIdx;
		if (!GenerateCombinationIndices(mCombIdx, iCount, iChoose, 1, piIdxList, 0))
		{
			rCB.GetErrorList().GeneralError("Error generating combination indices.", iLine, iPos);
			delete[] piIdxList;
			return false;
		}

		delete[] piIdxList;

		int i, j, iCombCount = int(mCombIdx.Count());
		rVar.New(PDT_VARLIST);
		TVarList& rCList = *rVar.GetVarListPtr();
		rCList.Set(iCombCount);

		for (i = 0; i < iCombCount; i++)
		{
			rCList[i].New(PDT_VARLIST);
			TVarList& rIdxList = *(rCList[i].GetVarListPtr());
			rIdxList.Set(iChoose);

			Mem<int>& rmIdx = mCombIdx[i];

			for (j = 0; j < iChoose; j++)
			{
				rIdxList[j].New(PDT_VARLIST);
				TVarList& rTheIdx = *rIdxList[j].GetVarListPtr();
				rTheIdx.Set(1);

				rTheIdx[0] = TCVCounter(rmIdx[j]);
			}
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
/// Index list of permutations
// Expects number of elements to choose from and number of elements to permute
// Returns list of indices giving combinations.

bool  PermutationIndexListFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	TCVCounter iChoose, iCount;

	if (iVarCount == 2)
	{
		if (!mVars(0).CastToCounter(iCount))
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (!mVars(1).CastToCounter(iChoose))
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		if ((iCount <= 0) || (iCount > 50))
		{
			rCB.GetErrorList().GeneralError("Number of elements out of range.", iLine, iPos);
			return false;
		}

		if ((iChoose <= 0) || (iChoose > iCount))
		{
			rCB.GetErrorList().GeneralError("Number of elements to choose out of range.", iLine, iPos);
			return false;
		}

		int* piIdxList = new int[iChoose];
		if (!piIdxList)
		{
			rCB.GetErrorList().OutOfMemory(iLine, iPos);
			return false;
		}

		// First generate combinations. Then permute those combinations.
		MemObj<Mem<int> > mCombIdx;
		if (!GenerateCombinationIndices(mCombIdx, iCount, iChoose, 1, piIdxList, 0))
		{
			rCB.GetErrorList().GeneralError("Error generating combination indices.", iLine, iPos);
			delete[] piIdxList;
			return false;
		}

		delete[] piIdxList;

		int i, j, iCombCount = int(mCombIdx.Count());
		MemObj<Mem<int> > mPermIdx, mPermIdxList;
		Mem<int> mCurPermIdx;

		// Now permute all combinations
		for (i = 0; i < iCombCount; i++)
		{
			mPermIdxList.Set(0);

			if (!GeneratePermutationIndices(mPermIdxList, mCurPermIdx, mCombIdx[i], iChoose))
			{
				rCB.GetErrorList().GeneralError("Error generating permutations.", iLine, iPos);
				return false;
			}

			int iPermCount  = int(mPermIdxList.Count());
			int iPermOffset = int(mPermIdx.Count());
			mPermIdx.Add(iPermCount);
			for (j = 0; j < iPermCount; j++)
			{
				mPermIdx[iPermOffset + j] = mPermIdxList[j];
			}
		}

		int iPermCount = int(mPermIdx.Count());

		rVar.New(PDT_VARLIST);
		TVarList& rCList = *rVar.GetVarListPtr();
		rCList.Set(iPermCount);

		for (i = 0; i < iPermCount; i++)
		{
			rCList[i].New(PDT_VARLIST);
			TVarList& rIdxList = *(rCList[i].GetVarListPtr());
			rIdxList.Set(iChoose);

			Mem<int>& rmIdx = mPermIdx[i];

			for (j = 0; j < iChoose; j++)
			{
				rIdxList[j].New(PDT_VARLIST);
				TVarList& rTheIdx = *rIdxList[j].GetVarListPtr();
				rTheIdx.Set(1);

				rTheIdx[0] = TCVCounter(rmIdx[j]);
			}
		}
	}
	else
	{
		rCB.GetErrorList().WrongNoOfParams(2, iLine, iPos);
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////
// Generates all combinations of indices using recursion.

bool  GenerateCombinationIndices(MemObj<Mem<int> >& mCombIdx, int iCount, int iChoose, int iFirstIdx, int* piIdxList, int iIdxPos)
{
	int iIdx;

	// Assume that indices start counting with 1
	if ((iChoose < 0) || (iChoose > iCount) || (iFirstIdx < 1))
	{
		return false;
	}

	// Scalar blade
	if (iChoose == 0)
	{
		// Save Index List
		mCombIdx.Add(1);
		Mem<int>& mIdxList = mCombIdx.Last();
		if (iIdxPos > 0)
		{
			mIdxList.Set(iIdxPos);
			for (int i = 0; i < iIdxPos; i++)
			{
				mIdxList[i] = piIdxList[i];
			}
		}
		else
		{
			mIdxList.Set(0);
		}

		return true;
	}

	// Blade of higher grade
	for (iIdx = iFirstIdx; iIdx <= iCount; iIdx++)
	{
		piIdxList[iIdxPos] = iIdx;
		if (!GenerateCombinationIndices(mCombIdx, iCount,
				    iChoose - 1, iIdx + 1, piIdxList, iIdxPos + 1))
		{
			return false;
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////
// Generates all permutations of indices using recursion.

bool  GeneratePermutationIndices(MemObj<Mem<int> >& mPermIdx, Mem<int>& mCurPermIdx, Mem<int>& mIdxList, int iRest)
{
	if (iRest < 0)
	{
		return false;
	}

	if (iRest == 0)
	{
		mPermIdx.Add(1);
		mPermIdx.Last() = mCurPermIdx;
		return true;
	}

	int iIdx, iIdxCount = int(mIdxList.Count());
	Mem<int> mSubPermIdx;
	Mem<int> mSubIdxList;

	for (iIdx = 0; iIdx < iIdxCount; iIdx++)
	{
		mSubPermIdx = mCurPermIdx;
		mSubIdxList = mIdxList;
		mSubPermIdx.Add(1);
		mSubPermIdx.Last() = mIdxList[iIdx];
		mSubIdxList.Del(iIdx);

		if (!GeneratePermutationIndices(mPermIdx, mSubPermIdx, mSubIdxList, iRest - 1))
		{
			return false;
		}
	}

	return true;
}
