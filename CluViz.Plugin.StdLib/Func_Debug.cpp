////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_Debug.cpp
//
// summary:   Implements the function debug class
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
#include "Func_Debug.h"

//////////////////////////////////////////////////////////////////////
// Get Content List of Base Element Repository

bool  GetBaseElementRepositoryContentListFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount != 0)
	{
		rCB.GetErrorList().WrongNoOfParams(0, iLine, iPos);
		return false;
	}

	COGLBERepository* pRep = rCB.GetOGLDrawBase()->GetSceneRepository();

	const COGLBERepository::TObjToRefMap* pmapObjToRef = pRep->GetObjToRefMap();

	COGLBERepository::TObjToRefMap::const_iterator itEl;

	char pcPtr[32];
	rVar.New(PDT_VARLIST);
	TVarList& rList = *rVar.GetVarListPtr();

	for (itEl = pmapObjToRef->begin();
	     itEl != pmapObjToRef->end();
	     ++itEl)
	{
		rList.Add(1);
		CCodeVar& rObjVar = rList.Last();
		rObjVar.New(PDT_VARLIST);
		TVarList& rObjList = *rObjVar.GetVarListPtr();

		const COGLBaseElement* pEl = itEl->first;
		rObjList.Add(4);
		rObjList(0) = pEl->GetUID();
		rObjList(1) = pEl->GetTypeName().c_str();
		rObjList(2) = pEl->GetName().c_str();

		rObjList(3).New(PDT_VARLIST);
		TVarList& rRefList = *rObjList(3).GetVarListPtr();

		const COGLBERepository::TObjRefList& listRef = itEl->second;
		rRefList.Add(listRef.size());

		COGLBERepository::TObjRefList::const_iterator itRef;

		int i = 0;
		for (itRef = listRef.begin();
		     itRef != listRef.end();
		     ++itRef)
		{
			sprintf_s(pcPtr, 32, "%p", (*itRef));
			rRefList(i) = pcPtr;
			++i;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Get Content List of Image Repository

bool  GetImageRepositoryContentListFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
{
	TVarList& mVars = *rPars.GetVarListPtr();
	int iVarCount   = int(mVars.Count());

	if (iVarCount != 0)
	{
		rCB.GetErrorList().WrongNoOfParams(0, iLine, iPos);
		return false;
	}

	CImageRepository& rImgRep = *((CImageRepository*)::GetImageRepositoryPtr());
	rImgRep.Lock();

	const CImageRepository::TImgToRefMap* pmapImgToRef = rImgRep.GetImgToRefMap();

	CImageRepository::TImgToRefMap::const_iterator itEl;

	char pcPtr[32];
	CStrMem csVal;
	int iWidth, iHeight;
	rVar.New(PDT_VARLIST);
	TVarList& rList = *rVar.GetVarListPtr();

	for (itEl = pmapImgToRef->begin();
	     itEl != pmapImgToRef->end();
	     ++itEl)
	{
		rList.Add(1);
		CCodeVar& rObjVar = rList.Last();
		rObjVar.New(PDT_VARLIST);
		TVarList& rObjList = *rObjVar.GetVarListPtr();

		const COGLImage* pEl = itEl->first;
		rObjList.Add(4);
		pEl->GetFilename(csVal);
		pEl->GetSize(iWidth, iHeight);
		rObjList(0) = csVal;
		rObjList(1) = iWidth;
		rObjList(2) = iHeight;

		rObjList(3).New(PDT_VARLIST);
		TVarList& rRefList = *rObjList(3).GetVarListPtr();

		const CImageRepository::TImgRefList& listRef = itEl->second;
		rRefList.Add(listRef.size());

		CImageRepository::TImgRefList::const_iterator itRef;

		int i = 0;
		for (itRef = listRef.begin();
		     itRef != listRef.end();
		     ++itRef)
		{
			sprintf_s(pcPtr, 32, "%p", (*itRef));
			rRefList(i) = pcPtr;
			++i;
		}
	}

	rImgRep.Unlock();
	return true;
}
