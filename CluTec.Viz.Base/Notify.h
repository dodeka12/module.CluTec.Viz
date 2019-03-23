////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      Notify.h
//
// summary:   Declares the notify class
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

// Notify.h: Schnittstelle f�r die Klasse CNotify.
//
// Class to collect notifications from program routines
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NOTIFY_H__87B4B452_A5ED_4CB5_803E_34CB9056F45C__INCLUDED_)
#define AFX_NOTIFY_H__87B4B452_A5ED_4CB5_803E_34CB9056F45C__INCLUDED_

#include "StrMessageList.h"


enum eNotifyType
{
	NT_NOTIFY, NT_ERROR, NT_WARN0, NT_WARN1, NT_WARN2, NT_WARN3, 
	NT_TRACE0, NT_TRACE1, NT_TRACE2, NT_TRACE3, NT_TRACE4, NT_TRACE5
};


class CNotify : public StrMessageList  
{
public:
	CNotify();
	virtual ~CNotify();

	CNotify& operator= (const CNotify& sml)
	{ StrMessageList::operator = (StrMessageList(sml)); return *this; }

	CNotify& operator<< (const CNotify& sml)
	{ StrMessageList::operator << (StrMessageList(sml)); return *this; }


	void Notify(const char *psMsg) { SetMessage(NT_NOTIFY, psMsg); }
	void Error(const char *psMsg) { SetMessage(NT_ERROR, psMsg); }
	void Warn(const char *psMsg) { SetMessage(NT_WARN0, psMsg); }
	void Warn(int i, const char *psMsg) 
	{ if (i < 0) i=0; else if (i > 3) i=3; SetMessage(NT_WARN0 + i, psMsg); }

	void Trace(const char *psMsg) { SetMessage(NT_TRACE0, psMsg); }
	void Trace(int iLevel, const char *psMsg) 
	{ if (iLevel < 0) iLevel=0; else if (iLevel > 5) iLevel=5; 
	  SetMessage(NT_TRACE0 + iLevel, psMsg); }

	CStrMem GetNotify() { CStrMem csMsg; GetNotify(csMsg); return csMsg; }
	void GetNotify(CStrMem &csMsg) { GetNotifyRange(NT_NOTIFY, NT_NOTIFY, csMsg); }

	CStrMem GetError() { CStrMem csMsg; GetError(csMsg); return csMsg; }
	void GetError(CStrMem &csMsg) { GetNotifyRange(NT_ERROR, NT_ERROR, csMsg); }

	CStrMem GetWarn(int iLevel) { CStrMem csMsg; GetWarn(iLevel, csMsg); return csMsg; }
	void GetWarn(int iLevel, CStrMem &csMsg) 
	{	if (iLevel < 0) iLevel=0; else if (iLevel>3) iLevel = 3;
		GetNotifyRange(NT_WARN0, eNotifyType(NT_WARN0+iLevel), csMsg); }

	CStrMem GetTrace(int iLevel) { CStrMem csMsg; GetWarn(iLevel, csMsg); return csMsg; }
	void GetTrace(int iLevel, CStrMem &csMsg) 
	{	if (iLevel < 0) iLevel=0; else if (iLevel>5) iLevel = 5;
		GetNotifyRange(NT_TRACE0, eNotifyType(NT_TRACE0+iLevel), csMsg); }

	void GetNotifyRange(eNotifyType ntLow, eNotifyType ntHigh, CStrMem &csMsg);
};

#endif // !defined(AFX_NOTIFY_H__87B4B452_A5ED_4CB5_803E_34CB9056F45C__INCLUDED_)
