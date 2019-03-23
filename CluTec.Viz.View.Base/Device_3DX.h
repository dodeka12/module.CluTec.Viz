////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.View.Base
// file:      Device_3DX.h
//
// summary:   Declares the device 3D x coordinate class
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

// Device_3DX.h: Schnittstelle f�r die Klasse Device_3DX.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_DEVICE_3DX_H__INCLUDED_)
#define _DEVICE_3DX_H__INCLUDED_

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000


#include "time.h"
#include "sys/timeb.h"

#include <map>
#include <string>
#include <vector>

using namespace std;

#ifdef WIN32
	// Macro that is used by Windows and only confuses us here.
	#ifdef CreateWindow
		#undef CreateWindow
	#endif
	typedef struct _timeb TTimeStruct;
#	ifndef GETTIME
#		define GETTIME(S) _ftime_s(S)
#	endif
#else
	typedef struct timeb TTimeStruct;
	#define GETTIME(S) ftime(S)
#endif

// Includes and defines for 3dx 3D-Mouse
#ifdef WIN32
#	define _WIN32_DCOM 
#	define _ATL_APARTMENT_THREADED
#	define _ATL_NO_AUTOMATIC_NAMESPACE

#	define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit
#	define _ATL_ATTRIBUTES 1

#	include <atlbase.h>
#	include <atlcom.h>
#	include <atlwin.h>
#	include <atltypes.h>
#	include <atlctl.h>
#	include <atlhost.h>

	using namespace ATL;

	struct IKeyboard;
	struct ISensor;
//#	import "progid:TDxInput.Device" embedded_idl no_namespace

#endif


[ event_receiver(com) ]
class CDevice_3DX
{
public:
	CDevice_3DX();
	virtual ~CDevice_3DX()
	{
		Finalize();
	}

	// Initialize 3D Mouse if one is connected.
	bool Init();
	bool Finalize();
	
	void SetCallback_KeyDown( void (*f)(int) )
	{ m_pfMouse3D_KeyDown = f; }

	void SetCallback_KeyUp( void (*f)(int) )
	{ m_pfMouse3D_KeyUp = f; }

	void SetCallback_Move( void (*f)( double, double, double, double, double, double, double, double ) )
	{ m_pfMouse3D_Move = f; }

	// COM 3DX Event Handler
#ifdef WIN32
	HRESULT On3DX_DeviceChange (long lReserved);
	HRESULT On3DX_KeyDown (int iKeyCode);
	HRESULT On3DX_KeyUp (int iKeyCode);
	HRESULT On3DX_SensorInput(void);
#endif

	double GetTime();

protected:

	void (*m_pfMouse3D_KeyDown)( int iKeyCode );
	void (*m_pfMouse3D_KeyUp)( int iKeyCode );
	void (*m_pfMouse3D_Move)( double dTX, double dTY, double dTZ, double dLength,
							   double dRX, double dRY, double dRZ, double dAngle );

	// true if COM could be initialized
	static bool sm_bCOMInitOK;

	// true if 3D Mouse driver could be initialized
	bool m_b3DMouseDriver;

	// 3D Mouse Sensor Event Time
	double m_d3DMouseTime;

#ifdef WIN32
	ATL::CComPtr<ISensor> m_p3DX_ISensor;
	ATL::CComPtr<IKeyboard> m_p3DX_IKeyboard;
#endif


};


#endif // !defined(AFX_OGLWINGLUT_H__6EB9C282_EE5B_11D5_BA34_00E07D8AFD6A__INCLUDED_)
