////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      SyncSerialComm.h
//
// summary:   Declares the synchronise serial communications class
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

#ifdef WIN32

#include <windows.h>

#if !defined(AFX_SYNCSERIALCOMM_H__D1CAB621_DF4B_4729_82AB_31D5B9EFE8A9__INCLUDED_)
#define AFX_SYNCSERIALCOMM_H__D1CAB621_DF4B_4729_82AB_31D5B9EFE8A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// Name: CSyncSerialComm
// Version: 1.0
// Comment: This class is responsible for provide I/O operation with
// a serial port. It is implemented with Synchronous I/O viz. both
// input and output operations block. Both read and write operations
// are supported.
//////////////////////////////////////////////////////////////////////

class CSyncSerialComm
{
public:
	bool Flush(DWORD dwFlag = PURGE_TXCLEAR | PURGE_RXCLEAR);
	bool Write(const char *pszBuf, DWORD dwSize);
	bool Read(string &sBuf);
	bool ConfigPort(DWORD dwBaudRate = CBR_19200, DWORD dwTimeOutInSec = 5,
					DWORD dwByteSize = 8, DWORD dwParity = NOPARITY,
					DWORD dwStopBits = ONESTOPBIT);
	bool Close();	
	bool Open(const char *pszPortName);

	CSyncSerialComm();
	virtual ~CSyncSerialComm();

	const char* GetPortName() const { return m_pszPortName; }
	unsigned GetBaudRate() { return m_dwBaudRate; }
	unsigned GetTimeOut() { return m_dwTimeOut; }
	unsigned GetByteSize() { return m_dwByteSize; }
	unsigned GetParity() { return m_dwParity; }
	unsigned GetStopBits() { return m_dwStopBits; }

	bool IsOpen() { return (m_hSerialComm == INVALID_HANDLE_VALUE ? false : true); }

private:
	char *m_pszPortName;
	DWORD m_dwBaudRate;
	DWORD m_dwTimeOut;
	DWORD m_dwByteSize;
	DWORD m_dwParity;
	DWORD m_dwStopBits;
	HANDLE m_hSerialComm;
};

#endif // !defined(AFX_SYNCSERIALCOMM_H__D1CAB621_DF4B_4729_82AB_31D5B9EFE8A9__INCLUDED_)

#endif
