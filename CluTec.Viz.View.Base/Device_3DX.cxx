////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.View.Base
// file:      Device_3DX.cxx
//
// summary:   device 3D x coordinate class
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

// CDevice_3DX.cpp: Implementierung der Klasse CDevice_3DX.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#include "Device_3DX.h"

// Includes and defines for 3dx 3D-Mouse
//#ifdef WIN32
//#	define _WIN32_DCOM
//#	define _ATL_APARTMENT_THREADED
//#	define _ATL_NO_AUTOMATIC_NAMESPACE
//
//#	define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit
//#	define _ATL_ATTRIBUTES 1
//
//#	include <atlbase.h>
//#	include <atlcom.h>
//#	include <atlwin.h>
//#	include <atltypes.h>
//#	include <atlctl.h>
//#	include <atlhost.h>
//
//	using namespace ATL;
//
//	struct IKeyboard;
//	struct ISensor;
//#	import "progid:TDxInput.Device" embedded_idl no_namespace
//
//#endif

#ifdef WIN32
    #import "progid:TDxInput.Device" embedded_idl no_namespace
#endif

bool CDevice_3DX::sm_bCOMInitOK = false;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CDevice_3DX::CDevice_3DX()
{
	m_b3DMouseDriver = false;
	m_d3DMouseTime   = 0.0;

	m_pfMouse3D_KeyDown = 0;
	m_pfMouse3D_KeyUp   = 0;
	m_pfMouse3D_Move    = 0;
}

//////////////////////////////////////////////////////////////////////
/// Init 3D Mouse Driver

bool CDevice_3DX::Init()
{
	//if ( !sm_bCOMInitOK )
	//{
	#ifdef WIN32
		HRESULT hr = ::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
		sm_bCOMInitOK = SUCCEEDED(hr);
	#endif
	//}

	if (sm_bCOMInitOK && !m_b3DMouseDriver)
	{
		#ifdef WIN32
			HRESULT hr;
			CComPtr<IUnknown> _3DxDevice;

			// Create the device object
			hr = _3DxDevice.CoCreateInstance(__uuidof(Device));
			if (!SUCCEEDED(hr))
			{
				return false;
			}

			CComPtr<ISimpleDevice> _3DxSimpleDevice;

			hr = _3DxDevice.QueryInterface(&_3DxSimpleDevice);
			if (!SUCCEEDED(hr))
			{
				return false;
			}

			hr = __hook(&_ISimpleDeviceEvents::DeviceChange, _3DxSimpleDevice,
				&CDevice_3DX::On3DX_DeviceChange);

			// Get the interfaces to the sensor and the keyboard;
			hr = _3DxSimpleDevice->get_Sensor(&m_p3DX_ISensor);

			hr = __hook(&_ISensorEvents::SensorInput, m_p3DX_ISensor,
				&CDevice_3DX::On3DX_SensorInput);

			hr = _3DxSimpleDevice->get_Keyboard(&m_p3DX_IKeyboard);

			hr = __hook(&_IKeyboardEvents::KeyDown, m_p3DX_IKeyboard,
				&CDevice_3DX::On3DX_KeyDown);

			hr = __hook(&_IKeyboardEvents::KeyUp, m_p3DX_IKeyboard,
				&CDevice_3DX::On3DX_KeyUp);

			// Set the preferences we want to use
			_3DxSimpleDevice->LoadPreferences(BSTR("CLUViz"));

			// Connect to the driver
			_3DxSimpleDevice->Connect();

			m_b3DMouseDriver = true;
		#endif
	}

	return m_b3DMouseDriver;
}

//////////////////////////////////////////////////////////////////////
/// Finalize 3D Mouse Driver

bool CDevice_3DX::Finalize()
{
	if (m_b3DMouseDriver)
	{
		#ifdef WIN32
			HRESULT hr;
			CComPtr<IDispatch> _3DxDevice;

			if (m_p3DX_ISensor)
			{
				hr = m_p3DX_ISensor->get_Device(&_3DxDevice);
			}
			else if (m_p3DX_IKeyboard)
			{
				hr = m_p3DX_IKeyboard->get_Device(&_3DxDevice);
			}

			if (SUCCEEDED(hr))
			{
				CComPtr<ISimpleDevice> _3DxSimpleDevice;
				hr = _3DxDevice.QueryInterface(&_3DxSimpleDevice);
				if (SUCCEEDED(hr))
				{
					_3DxSimpleDevice->Disconnect();
				}
			}

			if (m_p3DX_ISensor)
			{
				// unhook (unadvise) the sensor event sink
				__unhook(&_ISensorEvents::SensorInput, m_p3DX_ISensor,
						&CDevice_3DX::On3DX_SensorInput);

				m_p3DX_ISensor.Release();
			}

			if (m_p3DX_IKeyboard)
			{
				__unhook(&_IKeyboardEvents::KeyDown, m_p3DX_IKeyboard,
						&CDevice_3DX::On3DX_KeyDown);

				__unhook(&_IKeyboardEvents::KeyUp, m_p3DX_IKeyboard,
						&CDevice_3DX::On3DX_KeyUp);

				m_p3DX_IKeyboard.Release();
			}

			m_b3DMouseDriver = false;
		#endif
	}

//	if ( sm_bCOMInitOK )
//	{
//#ifdef WIN32
//		::CoUninitialize();
//#endif
	sm_bCOMInitOK = false;
	//}

	return true;
}

//////////////////////////////////////////////////////////////////////
/// Get Time as double

double CDevice_3DX::GetTime()
{
	TTimeStruct Time;
	GETTIME(&Time);

	return double(Time.time) + 1e-3 * double(Time.millitm);
}

#ifdef WIN32
//////////////////////////////////////////////////////////////////////
/// 3D Mouse Device Changed

	HRESULT CDevice_3DX::On3DX_DeviceChange(long lReserved)
	{
		HRESULT hResult = S_OK;
		//AtlTrace (_T("CCubeWindow::OnDeviceChange(reserved =0x%x)\n"), reserved);

		//HRESULT hr;
		//ISimpleDevicePtr _p3DxDevice;
		//hr = m_p3DX_IKeyboard->get_Device((IDispatch**)&_p3DxDevice);

		//if (SUCCEEDED(hr))
		//{
		//   long device;
		//   _p3DxDevice->get_Type(&device);
		//   //AtlTrace (_T("Attached device=%d\n"), device);

		//   _p3DxDevice.Release();

		//   long keys, programmableKeys;
		//   m_pIKeyboard->get_Keys(&keys);
		//   m_pIKeyboard->get_ProgrammableKeys(&programmableKeys);
		//   AtlTrace (_T("Number of Keys=%d\tProgrammable keys=%d\n"), keys, programmableKeys);
		//   for (long i=1; i<=keys; i++)
		//   {
		//      BSTR bstrLabel;
		//      BSTR bstrName;
		//      m_pIKeyboard->GetKeyLabel(i, &bstrLabel);
		//      m_pIKeyboard->GetKeyName(i, &bstrName);
		//      CString strLabel(bstrLabel);
		//      CString strName(bstrName);

		//      AtlTrace (_T("Key Label=%s\tKey Name=%s\n"), (const TCHAR *)strLabel, (const TCHAR *)strName);
		//   }
		//}

		return hResult;
	}

//////////////////////////////////////////////////////////////////////
/// 3D Mouse Key Down

	HRESULT CDevice_3DX::On3DX_KeyDown(int iKeyCode)
	{
		HRESULT hResult = S_OK;

		if (m_pfMouse3D_KeyDown)
		{
			(*m_pfMouse3D_KeyDown)(iKeyCode);
		}

		return hResult;
	}

	HRESULT CDevice_3DX::On3DX_KeyUp(int iKeyCode)
	{
		HRESULT hResult = S_OK;

		if (m_pfMouse3D_KeyUp)
		{
			(*m_pfMouse3D_KeyUp)(iKeyCode);
		}

		return hResult;
	}

	HRESULT CDevice_3DX::On3DX_SensorInput(void)
	{
		HRESULT hResult = S_OK;

		if (!m_pfMouse3D_Move)
		{
			return hResult;
		}

		try
		{
			CComPtr<IAngleAxis> pRotation;
			CComPtr<IVector3D> pTranslation;

			if (!SUCCEEDED(m_p3DX_ISensor->get_Rotation(&pRotation)))
			{
				return S_OK;
			}

			if (!SUCCEEDED(m_p3DX_ISensor->get_Translation(&pTranslation)))
			{
				pRotation.Release();
				return S_OK;
			}

			double dAngle, dLength;
			double dPeriod, dTimeFactor, dTimeNow;
			double dTX, dTY, dTZ;
			double dRX, dRY, dRZ;

			pTranslation->get_Length(&dLength);
			pRotation->get_Angle(&dAngle);

			if ((dAngle != 0.0) || (dLength != 0.0))
			{
				m_p3DX_ISensor->get_Period(&dPeriod);

				dTimeFactor = 0.0001;
				dTimeNow    = GetTime();
				if (m_d3DMouseTime != 0.0)
				{
					dTimeFactor = (dTimeNow - m_d3DMouseTime) / dPeriod;
				}
				m_d3DMouseTime = dTimeNow;

				dLength *= dTimeFactor;	// / 512.0;
				pTranslation->put_Length(dLength);

				pTranslation->get_X(&dTX);
				pTranslation->get_Y(&dTY);
				pTranslation->get_Z(&dTZ);

				dAngle *= dTimeFactor;	// / 1024.0;
				pRotation->get_X(&dRX);
				pRotation->get_Y(&dRY);
				pRotation->get_Z(&dRZ);

				(*m_pfMouse3D_Move)(dTX, dTY, dTZ, dLength, dRX, dRY, dRZ, dAngle);
			}
			else
			{
				m_d3DMouseTime = 0.0;
			}

			pRotation.Release();
			pTranslation.Release();
		}
		catch (...)
		{
			// Some sort of exception handling
		}

		return hResult;
	}

#endif
