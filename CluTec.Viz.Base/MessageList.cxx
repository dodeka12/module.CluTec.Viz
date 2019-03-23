////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      MessageList.cxx
//
// summary:   message list class
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

#include "MessageList.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Dmstruktion
//////////////////////////////////////////////////////////////////////

template<class CType>
MessageList<CType>::MessageList()
{

}

template<class CType>
MessageList<CType>::~MessageList()
{

}


//////////////////////////////////////////////////////////////////////
/// Operator =

template<class CType>
MessageList<CType>& MessageList<CType>::operator= (const MessageList<CType>& ml)
{
	msgList = ml.msgList;
	msgData = ml.msgData;

	return *this;
}


//////////////////////////////////////////////////////////////////////
/// Operator += : Joins two Message lists

template<class CType>
MessageList<CType>& MessageList<CType>::operator<< (const MessageList<CType>& ml)
{
	uint mno = uint(ml.msgList.Count());
	uint no = uint(msgList.Count());
	uint i,j;

	if (!(msgList += mno)) return *this;

	for(i=0,j=no;i<mno;i++,j++)
	{
		msgList[j] = ml.msgList[i];
	}

	return *this;
}



//////////////////////////////////////////////////////////////////////
// HasMessage
//////////////////////////////////////////////////////////////////////

template<class CType>
uint MessageList<CType>::HasMessage(CType *mdata)
{
	uint i;

	if (i = uint(msgList.Count()))
	{
		if (mdata) *mdata = msgList[i-1].data;

		return msgList[i-1].no;
	}

	return 0;
}


//////////////////////////////////////////////////////////////////////
// GetLastMessage
//////////////////////////////////////////////////////////////////////

template<class CType>
uint MessageList<CType>::GetLastMessage(CType *mdata)
{
	uint i, mno;

	if (i = uint(msgList.Count()))
	{
		if (mdata) *mdata = msgList[i-1].data;
		mno =  msgList[i-1].no;
		msgList--;

		return mno;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////
// GetFirstMessage
//////////////////////////////////////////////////////////////////////

template<class CType>
uint MessageList<CType>::GetFirstMessage(CType *mdata)
{
	uint mno;

	if (msgList.Count())
	{
		if (mdata) *mdata = msgList[0].data;
		mno =  msgList[0].no;
		msgList.Del(0);

		return mno;
	}

	return 0;
}



//////////////////////////////////////////////////////////////////////
// AddMessage
//////////////////////////////////////////////////////////////////////

template<class CType>
void MessageList<CType>::AddMessage(uint mno, CType *mdata)
{
	uint i = uint (msgData.Count());

	if (msgData++)
	{
		msgData[i].no = mno;

		if (mdata) msgData[i].data = *mdata;
	}
}


//////////////////////////////////////////////////////////////////////
// SetMessage
//////////////////////////////////////////////////////////////////////

template<class CType>
void MessageList<CType>::SetMessage(uint mno, CType *mdata)
{
	uint i = uint(msgList.Count());

	if (msgList++)
	{
		msgList[i].no = mno;
		if (!mdata) msgList[i].data = msgData[GetMessageIdx(mno)].data;
		else msgList[i].data = *mdata;
	}
}



//////////////////////////////////////////////////////////////////////
// GetMessageIdx
//////////////////////////////////////////////////////////////////////

template<class CType>
uint MessageList<CType>::GetMessageIdx(uint mno)
{
	uint n = uint(msgData.Count());

	for(uint i=0;i<n;i++)
	{
		if (msgData[i].no == mno)
			return i;
	}

	return 0;
}
