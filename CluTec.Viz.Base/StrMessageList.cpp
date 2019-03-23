////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      StrMessageList.cpp
//
// summary:   Implements the message list class
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

// StrMessageList.cpp: Implementierung der Klasse StrMessageList.
//
//////////////////////////////////////////////////////////////////////

#include "StrMessageList.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

StrMessageList::StrMessageList()
{
	uStdMsgNo = 1;
}

StrMessageList::~StrMessageList()
{

}



//////////////////////////////////////////////////////////////////////
// AddMessage
//////////////////////////////////////////////////////////////////////

void StrMessageList::AddMessage(uint mno, const char *mdata)
{
	uint i = uint(msgData.Count());

	if (msgData++)
	{
		msgData[i].no = mno;

		if (mdata) msgData[i].data = mdata;
	}
}


//////////////////////////////////////////////////////////////////////
// SetMessage
//////////////////////////////////////////////////////////////////////

void StrMessageList::SetMessage(uint mno, const char *mdata)
{
	uint i = uint(msgList.Count());

	if (msgList++)
	{
		msgList[i].no = mno;
		if (!mdata) msgList[i].data = msgData[GetMessageIdx(mno)].data;
		else msgList[i].data = mdata;
	}
}

//////////////////////////////////////////////////////////////////////
// SetMessage const char*

void StrMessageList::SetMessage(const char *mdata)
{
	if (!mdata) return;

	uint i = uint(msgList.Count());

	if (msgList++)
	{
		msgList[i].no = uStdMsgNo;
		msgList[i].data = mdata;
	}
}

//////////////////////////////////////////////////////////////////////
// SetMessage cStr&

void StrMessageList::SetMessage(CStrMem& mStr)
{
	uint i = uint(msgList.Count());

	if (msgList++)
	{
		msgList[i].no = uStdMsgNo;
		msgList[i].data = mStr;
	}
}


//////////////////////////////////////////////////////////////////////
// SetMessage Matrix<double>&

void StrMessageList::SetMessage(Matrix<double>& mMat)
{
	size_t i, n = msgList.Count();

	if (!msgList++) return;

	size_t rows = mMat.Rows();
	CStrMem str;

	for(i=1;i<=rows;i++)
	{
		str << mMat.Str(uint(i)) << "\n";
	}

	msgList[n].no = uStdMsgNo;
	msgList[n].data = str;
}






