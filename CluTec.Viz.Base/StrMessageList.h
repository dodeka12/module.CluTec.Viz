////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      StrMessageList.h
//
// summary:   Declares the message list class
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

// StrMessageList.h: Schnittstelle f�r die Klasse StrMessageList.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STRMESSAGELIST_H__2321E060_528F_11D3_9065_444553540001__INCLUDED_)
#define AFX_STRMESSAGELIST_H__2321E060_528F_11D3_9065_444553540001__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CStrMem.h"
#include "matrix.h"
#include "MessageList.h"


class StrMessageList : public MessageList<CStrMem>  
{
public:
	StrMessageList();
	virtual ~StrMessageList();

	StrMessageList& operator= (const StrMessageList& sml)
	{ MessageList<CStrMem>::operator = (MessageList<CStrMem>(sml)); return *this; }

	StrMessageList& operator<< (const StrMessageList& sml)
	{ MessageList<CStrMem>::operator << (MessageList<CStrMem>(sml)); return *this; }

	// Add message no and a text to msgData
	void AddMessage(uint mno, const char *mdata);

	// Set message with number and optionally some other data
	// than the one given in msgData
	void SetMessage(uint mno, const char *mdata = 0);

	// Set Standard Message Number
	void SetStdMsgNo(uint mno) { if (mno > 0) uStdMsgNo = mno; }

	// Set Messages with standard message number
	void SetMessage(const char *mdata);
	void SetMessage(CStrMem& mStr);
	void SetMessage(Matrix<double>& mMat);

	// Set Standard Message Number
	StrMessageList& operator << (uint mno) { SetStdMsgNo(mno); return *this; }

	// Set Messages with standard message number
	StrMessageList& operator << (const char *mdata) { SetMessage(mdata); return *this; }
	StrMessageList& operator << (CStrMem& mStr) { SetMessage(mStr); return *this; }
	StrMessageList& operator << (Matrix<double>& mMat) { SetMessage(mMat); return *this; }

protected:
	uint uStdMsgNo; // Std Message Number. Is used if no Msg. No. is given.
};

#endif // !defined(AFX_STRMESSAGELIST_H__2321E060_528F_11D3_9065_444553540001__INCLUDED_)
