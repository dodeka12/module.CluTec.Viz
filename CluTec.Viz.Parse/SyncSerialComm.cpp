////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Parse
// file:      SyncSerialComm.cpp
//
// summary:   Implements the synchronise serial communications class
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

#ifdef WIN32

    #include "SyncSerialComm.h"

    #include <string.h>
    #include <assert.h>

    #include <sstream>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

	CSyncSerialComm::CSyncSerialComm()
		: m_hSerialComm(INVALID_HANDLE_VALUE)
	{
		m_pszPortName = 0;
		m_dwBaudRate  = 0;
		m_dwTimeOut   = 0;
		m_dwByteSize  = 0;
		m_dwParity    = 0;
		m_dwStopBits  = 0;
	}

	CSyncSerialComm::~CSyncSerialComm()
	{
		if (m_pszPortName)
		{
			delete[] m_pszPortName;
		}

		Close();
	}

//////////////////////////////////////////////////////////////////////
// Name: Open
// Version: 1.0
// Return: HRESULT
// Comment: This function is used open a connection with a serial port.
// Uses non-overlapped i/o, and allows for reading & writing to the
// port.
//////////////////////////////////////////////////////////////////////

	bool CSyncSerialComm::Open(const char* pszPortName)
	{
		bool bResult;
		assert(pszPortName);

		m_pszPortName = new char[strlen(pszPortName) + 1];
		strcpy_s(m_pszPortName, strlen(pszPortName) + 1, pszPortName);

		m_hSerialComm = CreateFile(m_pszPortName,	/* Port Name */
				GENERIC_READ | GENERIC_WRITE,				/* Desired Access */
				0,				/* Shared Mode */
				NULL,				/* Security */
				OPEN_EXISTING,					/* Creation Disposition */
				0,
				NULL);					/* Non Overlapped */

		if (m_hSerialComm == INVALID_HANDLE_VALUE)
		{
			bResult = false;
		}
		else
		{
			bResult = true;
		}

		m_dwBaudRate = 0;
		m_dwTimeOut  = 0;
		m_dwByteSize = 0;
		m_dwParity   = 0;
		m_dwStopBits = 0;

		return bResult;
	}

//////////////////////////////////////////////////////////////////////
// Name: Close
// Version: 1.0
// Return: HRESULT
// Comment: This function is used to close the serial port connection
// Note: This function is called with the destructor
//////////////////////////////////////////////////////////////////////

	bool CSyncSerialComm::Close()
	{
		if (m_hSerialComm != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_hSerialComm);
			m_hSerialComm = INVALID_HANDLE_VALUE;
		}

		m_dwBaudRate = 0;
		m_dwTimeOut  = 0;
		m_dwByteSize = 0;
		m_dwParity   = 0;
		m_dwStopBits = 0;

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Name: ConfigPort
// Version: 1.0
// Parameter: dwBaudRate - This must be set to the baud rate of the
// serial port connection otherwise invalid reads occur.
// dwTimeOut - Specifies the timeout for read and write of the serial
// port connection in milli-seconds
// Return: HRESULT
// Comment: This function is used configure the serial port connection.
//////////////////////////////////////////////////////////////////////

	bool CSyncSerialComm::ConfigPort(DWORD dwBaudRate, DWORD dwTimeOut, DWORD dwByteSize, DWORD dwParity, DWORD dwStopBits)
	{
		if (!SetupComm(m_hSerialComm, 1024, 1024))
		{
			return false;
		}

		DCB dcbConfig;

		if (GetCommState(m_hSerialComm, &dcbConfig))	/* Configuring Serial Port Settings */
		{
			dcbConfig.BaudRate = dwBaudRate;
			dcbConfig.ByteSize = BYTE(dwByteSize);
			dcbConfig.Parity   = BYTE(dwParity);
			dcbConfig.StopBits = BYTE(dwStopBits);
			dcbConfig.fBinary  = TRUE;
			dcbConfig.fParity  = TRUE;
		}
		else
		{
			return false;
		}

		if (!SetCommState(m_hSerialComm, &dcbConfig))
		{
			return false;
		}

		COMMTIMEOUTS commTimeout;

		if (GetCommTimeouts(m_hSerialComm, &commTimeout))	/* Configuring Read & Write Time Outs */
		{
			commTimeout.ReadIntervalTimeout         = dwTimeOut;
			commTimeout.ReadTotalTimeoutConstant    = dwTimeOut;
			commTimeout.ReadTotalTimeoutMultiplier  = 0;
			commTimeout.WriteTotalTimeoutConstant   = dwTimeOut;
			commTimeout.WriteTotalTimeoutMultiplier = 0;
		}
		else
		{
			return false;
		}

		if (!SetCommTimeouts(m_hSerialComm, &commTimeout))
		{
			return false;
		}

		m_dwBaudRate = dwBaudRate;
		m_dwTimeOut  = dwTimeOut;
		m_dwByteSize = dwByteSize;
		m_dwParity   = dwParity;
		m_dwStopBits = dwStopBits;

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Name: Read
// Version: 1.0
// Parameter: ppszBuf - The buffer that will have the value that was
// read in from the serial port.
// dwSize - The size of the buffer
// Return: HRESULT
// Comment: This function sets an event that will be signalled if the
// any byte is buffered internally. Once this occurs, the function keeps
// reading multiple a single byte at a time until there is no more furthur
// byte to read from the input stream
//////////////////////////////////////////////////////////////////////

	bool CSyncSerialComm::Read(string& sBuf)
	{
		bool bResult = true;
		std::stringbuf sb;
		DWORD /*dwEventMask,*/ dwBytesAvail;

//	if(!SetCommMask(m_hSerialComm, EV_RXCHAR)) /* Setting Event Type */
//		return false;

		PeekNamedPipe(m_hSerialComm, NULL, NULL, NULL, &dwBytesAvail, NULL);

//	if(WaitCommEvent(m_hSerialComm, &dwEventMask, NULL)) /* Waiting For Event to Occur */
		if (dwBytesAvail > 0)
		{
			char szBuf;
			DWORD dwIncommingReadSize;

			do
			{
				if (ReadFile(m_hSerialComm, &szBuf, 1, &dwIncommingReadSize, NULL) != 0)
				{
					if (dwIncommingReadSize > 0)
					{
						//dwSize += dwIncommingReadSize;
						sb.sputn(&szBuf, dwIncommingReadSize);
					}
				}
				else
				{
					bResult = false;
					break;
				}
			}
			while (dwIncommingReadSize > 0);

			sBuf = sb.str();
			return bResult;
		}

		return true;
	}

//////////////////////////////////////////////////////////////////////
// Name: Write
// Version: 1.0
// Parameter: szBuf - The buffer holding the bytes to write to the serial
// port connection
// dwSize - The size of the buffer
// Return: HRESULT
// Comment: This function writes one byte at a time until all the bytes
// in the buffer is sent out
//////////////////////////////////////////////////////////////////////

	bool CSyncSerialComm::Write(const char* pszBuf, DWORD dwSize)
	{
		bool bResult = true;

		assert(pszBuf);

		unsigned long dwNumberOfBytesSent = 0;

		while (dwNumberOfBytesSent < dwSize)
		{
			unsigned long dwNumberOfBytesWritten;

			if (WriteFile(m_hSerialComm, &pszBuf[dwNumberOfBytesSent], 1, &dwNumberOfBytesWritten, NULL) != 0)
			{
				if (dwNumberOfBytesWritten > 0)
				{
					++dwNumberOfBytesSent;
				}
				else
				{
					bResult = false;
					break;
				}
			}
			else
			{
				bResult = false;
				break;
			}
		}

		return bResult;
	}

//////////////////////////////////////////////////////////////////////
// Name: Flush
// Version: 1.0
// Parameter: dwFlag - The flag specifying if the input/output buffer
// to be flushed
// Return: HRESULT
// Comment: This function is flushes the specfied buffer
// Note: By default, both the input and output buffers are flushed
//////////////////////////////////////////////////////////////////////

	bool CSyncSerialComm::Flush(DWORD dwFlag)
	{
		if (PurgeComm(m_hSerialComm, dwFlag))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

#endif
