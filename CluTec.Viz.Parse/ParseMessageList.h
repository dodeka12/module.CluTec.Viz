////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      ParseMessageList.h
//
// summary:   Declares the parse message list class
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

// MessageList.h: Schnittstelle f�r die Klasse CMessageList.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESSAGELIST_H__51963663_24D3_4AB2_B8B0_5A688881FD3C__INCLUDED_)
	#define AFX_MESSAGELIST_H__51963663_24D3_4AB2_B8B0_5A688881FD3C__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif	// _MSC_VER > 1000

	struct SMsg
	{
		int iNo;
		int iLevel;
		int iLine;
		int iTextLineOffset;
		int iPos;
		CStrMem csText;

		SMsg() { iNo = 0; iLevel = 0; }
	};

	class CMessageList : public MemObj<SMsg>
	{
	public:

		CMessageList();
		virtual ~CMessageList();

		bool Reset() { return Set(0); }

		bool AddMsg(const char* pcText, int iLine = -1, int iPos = -1, int iNo = 0, int iLevel = 0, int iTextLineOffset = 0)
		{
			if (!Add(1)) { return false; }
			Last().iLine           = iLine;
			Last().iPos            = iPos;
			Last().iNo             = iNo;
			Last().iLevel          = iLevel;
			Last().iTextLineOffset = iTextLineOffset;
			Last().csText          = pcText;
			return true;
		}
	};

#endif	// !defined(AFX_MESSAGELIST_H__51963663_24D3_4AB2_B8B0_5A688881FD3C__INCLUDED_)
