////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      Func_Serial.cpp
//
// summary:   Implements the function serial class
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

#include "CluTec.Viz.Parse\CLUCodeBase.h"

#include "Func_Serial.h"

//////////////////////////////////////////////////////////////////////
/// Functions relating to Serial Port I/O

#ifdef WIN32

//////////////////////////////////////////////////////////////////////
/// Open the serial port
///
/// Parameters:
///		1. Port Name (string)
///		2. Baud Rate (int)
///		3. Time Out in milli-sec. (int)
///		4. (opt) Number of bits per byte; default 8; possible (7, 8) (int)
///		5. (opt) Number of stop bits; default 1; possible (1, 1.5, 2) (float)
///		6. (opt) Partiy; default "none"; possible ("even", "odd", "none") (string)

	bool  OpenSerialPortFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
	{
		TVarList& mVars = *rPars.GetVarListPtr();

		int iVarCount = int(mVars.Count());

		int i;
		char pcText[100];
		string sName, sParity;
		TCVCounter piVal[2], iByteSize = 8;
		TCVScalar dStopBits = 1.0;

		sParity = "none";

		if ((iVarCount != 3) && (iVarCount != 6))
		{
			int piParNo[] = { 3, 6 };
			rCB.GetErrorList().WrongNoOfParams(piParNo, 2, iLine, iPos);
			return false;
		}

		if (mVars(0).BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		sName = mVars(0).ValStr();

		for (i = 0; i < 2; i++)
		{
			if (!(mVars(i + 1).CastToCounter(piVal[i])))
			{
				rCB.GetErrorList().InvalidParType(mVars(i + 1), i + 2, iLine, iPos);
				return false;
			}
		}

		if (iVarCount >= 6)
		{
			if (!(mVars(3).CastToCounter(iByteSize)))
			{
				rCB.GetErrorList().InvalidParType(mVars(3), 4, iLine, iPos);
				return false;
			}

			if (!(mVars(4).CastToScalar(dStopBits)))
			{
				rCB.GetErrorList().InvalidParType(mVars(4), 5, iLine, iPos);
				return false;
			}

			if (mVars(5).BaseType() != PDT_STRING)
			{
				rCB.GetErrorList().InvalidParType(mVars(5), 6, iLine, iPos);
				return false;
			}

			sParity = mVars(5).ValStr().Str();
		}

		if ((sParity != "none") && (sParity != "even") && (sParity != "odd"))
		{
			rCB.GetErrorList().GeneralError("Invalid parity. Has to be 'none', 'even' or 'odd'.", iLine, iPos);
			return false;
		}

		if ((dStopBits != 1.0) && (dStopBits != 1.5) && (dStopBits != 2.0))
		{
			rCB.GetErrorList().GeneralError("Invalid number of stop bits. Has to be 1, 1.5 or 2.", iLine, iPos);
			return false;
		}

		if ((iByteSize != 7) && (iByteSize != 8))
		{
			rCB.GetErrorList().GeneralError("Invalid byte size. Has to be 7 or 8.", iLine, iPos);
			return false;
		}

		if (piVal[0] < 100)
		{
			rCB.GetErrorList().GeneralError("Invalid Baud Rate.", iLine, iPos);
			return false;
		}

		if (piVal[1] <= 0)
		{
			rCB.GetErrorList().GeneralError("Timeout time has to be greater than zero.", iLine, iPos);
			return false;
		}

		unsigned uByteSize, uStopBits, uParity;

		uByteSize = (unsigned) iByteSize;

		if (dStopBits == 1.0)
		{
			uStopBits = ONESTOPBIT;
		}
		else if (dStopBits == 1.5)
		{
			uStopBits = ONE5STOPBITS;
		}
		else if (dStopBits == 2)
		{
			uStopBits = TWOSTOPBITS;
		}

		if (sParity == "none")
		{
			uParity = NOPARITY;
		}
		else if (sParity == "even")
		{
			uParity = EVENPARITY;
		}
		else if (sParity == "odd")
		{
			uParity = ODDPARITY;
		}

		CCLUCodeBase::TSerialIOMap::iterator itEl;
		CSyncSerialComm* pIO;

		// Check whether instance for given serial port name does already exist
		if ((itEl = rCB.GetMapSerialIO().find(sName)) != rCB.GetMapSerialIO().end())
		{
			// Yes it does exist. So get a reference to it.
			pIO = &(itEl->second);

			// Check whether the port is currently open
			if (pIO->IsOpen())
			{
				// Has the Baud rate, or the timeout changed?
				if ((pIO->GetBaudRate() != piVal[0]) ||
				    (pIO->GetTimeOut() != piVal[1]) ||
				    (pIO->GetByteSize() != uByteSize) ||
				    (pIO->GetStopBits() != uStopBits) ||
				    (pIO->GetParity() != uParity))
				{
					// if yes, we need to close port and open it again with
					// new settings.
					pIO->Close();
				}
				else
				{
					// Port is already open and nothing has changed
					return true;
				}
			}
		}
		else
		{
			// If port instance does not exist, then create it
			pIO = &(rCB.GetMapSerialIO()[sName]);
		}

		// Open Port
		if (!pIO->Open(sName.c_str()))
		{
			sprintf_s(pcText, "Error opening port '%s'.", sName.c_str());
			rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
			return false;
		}

		// Configure Port
		if (!pIO->ConfigPort(piVal[0], piVal[1], uByteSize, uParity, uStopBits))
		{
			sprintf_s(pcText, "Error configuring port '%s'.", sName.c_str());
			rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
			return false;
		}

		return true;
	}

//////////////////////////////////////////////////////////////////////
/// Write to serial port
///
/// Parameters:
///		1. Port Name (string)
///		2. String to write (string)
///

	bool  WriteToSerialPortFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
	{
		TVarList& mVars = *rPars.GetVarListPtr();

		int iVarCount = int(mVars.Count());

		char pcText[100];
		string sName, sText;

		if (iVarCount != 2)
		{
			int piParNo[] = { 2 };
			rCB.GetErrorList().WrongNoOfParams(piParNo, 1, iLine, iPos);
			return false;
		}

		if (mVars(0).BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		if (mVars(1).BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().InvalidParType(mVars(1), 2, iLine, iPos);
			return false;
		}

		sName = mVars(0).ValStr();
		sText = mVars(1).ValStr();

		CCLUCodeBase::TSerialIOMap::iterator itEl;

		// Check whether instance for given serial port name does already exist
		if ((itEl = rCB.GetMapSerialIO().find(sName)) == rCB.GetMapSerialIO().end())
		{
			// If not, then error
			sprintf_s(pcText, "Port '%s' is not open.", sName.c_str());
			rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
			return false;
		}

		// Yes it does exist. So get a reference to it.
		CSyncSerialComm& rIO = itEl->second;

		if (!rIO.IsOpen())
		{
			// If not, then error
			sprintf_s(pcText, "Port '%s' is not open.", sName.c_str());
			rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
			return false;
		}

		if (!rIO.Write(sText.c_str(), (DWORD) sText.size()))
		{
			sprintf_s(pcText, "Error writing to serial port '%s'.", sName.c_str());
			rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
			return false;
		}

		return true;
	}

//////////////////////////////////////////////////////////////////////
/// Read from serial port
///
/// Parameters:
///		1. Port Name (string)
///
/// Return:
///		String read from serial port
///

	bool  ReadFromSerialPortFunc(CCLUCodeBase& rCB, CCodeVar& rVar, CCodeVar& rPars, int iLine, int iPos)
	{
		TVarList& mVars = *rPars.GetVarListPtr();

		int iVarCount = int(mVars.Count());

		char pcText[100];
		string sName, sText;

		if (iVarCount != 1)
		{
			int piParNo[] = { 1 };
			rCB.GetErrorList().WrongNoOfParams(piParNo, 1, iLine, iPos);
			return false;
		}

		if (mVars(0).BaseType() != PDT_STRING)
		{
			rCB.GetErrorList().InvalidParType(mVars(0), 1, iLine, iPos);
			return false;
		}

		sName = mVars(0).ValStr();

		CCLUCodeBase::TSerialIOMap::iterator itEl;

		// Check whether instance for given serial port name does already exist
		if ((itEl = rCB.GetMapSerialIO().find(sName)) == rCB.GetMapSerialIO().end())
		{
			// If not, then error
			sprintf_s(pcText, "Port '%s' is not open.", sName.c_str());
			rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
			return false;
		}

		// Yes it does exist. So get a reference to it.
		CSyncSerialComm& rIO = itEl->second;

		if (!rIO.IsOpen())
		{
			// If not, then error
			sprintf_s(pcText, "Port '%s' is not open.", sName.c_str());
			rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
			return false;
		}

		if (!rIO.Read(sText))
		{
			// If not, then error
			sprintf_s(pcText, "Cannot read from port '%s'.", sName.c_str());
			rCB.GetErrorList().GeneralError(pcText, iLine, iPos);
			return false;
		}

		rVar.New(PDT_STRING);
		(*rVar.GetStringPtr()) = sText.c_str();

		return true;
	}

#endif
