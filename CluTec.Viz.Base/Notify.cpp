////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Base
// file:      Notify.cpp
//
// summary:   Implements the notify class
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

// Notify.cpp: Implementierung der Klasse CNotify.
//
//////////////////////////////////////////////////////////////////////

#include "Notify.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CNotify::CNotify()
{
	SetStdMsgNo(NT_NOTIFY);
}

CNotify::~CNotify()
{

}


//////////////////////////////////////////////////////////////////////
/// Get Notify Range

void CNotify::GetNotifyRange(eNotifyType ntLow, eNotifyType ntHigh, CStrMem &csMsg)
{
	uint i, n = uint(msgList.Count());
	int iLevel;

	for(i=0;i<n;i++)
	{
		iLevel = int(msgList[i].no);

		if (iLevel >= int(ntLow) && iLevel <= int(ntHigh))
		{
			csMsg << msgList[i].data << "\n";
		}
	}
}
