/**********************************************************\ 
Original Author: Gil Gonen

Created:    Jan 28, 2013
License:    Dual license model; choose one of two:
New BSD License
http://www.opensource.org/licenses/bsd-license.php
- or -
GNU Lesser General Public License, version 2.1
http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2013 Gil Gonen and the Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_FB_DIRECT3DHELPER
#define H_FB_DIRECT3DHELPER

#include "win_common.h"
#include <d3d10_1.h>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include "logging.h"
#include "APITypes.h"
#include "D3d10DrawingContext.h"

// todo: split into cpp

//----------------------------------------------------------------------------------------------- 
// D3D10 functions
//
#ifdef _WINDOWS

typedef HRESULT (WINAPI*D3D10CreateDevice1Func)(
	IDXGIAdapter *pAdapter,
	D3D10_DRIVER_TYPE DriverType,
	HMODULE Software,
	UINT Flags,
	D3D10_FEATURE_LEVEL1 HardwareLevel,
	UINT SDKVersion,
	ID3D10Device1 **ppDevice
	);

typedef HRESULT(WINAPI*CreateDXGIFactory1Func)(
	REFIID riid,
	void **ppFactory
	);

static HRESULT getD3D10Device(ID3D10Device1** returnedDevice)
{
	HRESULT hr = S_FALSE;

	HMODULE d3d10module = LoadLibraryA("d3d10_1.dll");
	if(!d3d10module) {
		FBLOG_ERROR("getD3D10Device", "LoadLibraryA d3d10_1.dll failed");
		return hr;
	}	

	D3D10CreateDevice1Func createD3DDevice = (D3D10CreateDevice1Func)GetProcAddress(d3d10module, "D3D10CreateDevice1");
	if (createD3DDevice) {
		HMODULE dxgiModule = LoadLibraryA("dxgi.dll");
		CreateDXGIFactory1Func createDXGIFactory1 = (CreateDXGIFactory1Func)GetProcAddress(dxgiModule, "CreateDXGIFactory1");

		// Try to use a DXGI 1.1 adapter in order to share resources across processes.
		IDXGIAdapter1 *adapter1;
		if (createDXGIFactory1) {
			IDXGIFactory1 *factory1;
			hr = createDXGIFactory1(__uuidof(IDXGIFactory1),(void**)&factory1);

			if (FAILED(hr) || !factory1) {
				FBLOG_ERROR("getD3D10Device", "createDXGIFactory1 failed");
				return hr;
			}

			hr = factory1->EnumAdapters1(0, &adapter1);
			if (FAILED(hr) || !adapter1) {
				FBLOG_ERROR("getD3D10Device", "EnumAdapters1 failed");
				return hr;
			}

			hr = adapter1->CheckInterfaceSupport(__uuidof(ID3D10Device), NULL);
			if (FAILED(hr)) {
				adapter1 = NULL;
				FBLOG_ERROR("getD3D10Device", "CheckInterfaceSupport failed");
				factory1->Release();
				return hr;
			}
			factory1->Release();
		} else {
			FBLOG_ERROR("getD3D10Device", "GetProcAddress CreateDXGIFactory1 failed");
			return hr;
		}

		hr = createD3DDevice(
			adapter1, 
			D3D10_DRIVER_TYPE_HARDWARE,
			NULL,
			D3D10_CREATE_DEVICE_BGRA_SUPPORT |
			D3D10_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS,
			D3D10_FEATURE_LEVEL_10_0,
			D3D10_1_SDK_VERSION,
			returnedDevice);

		adapter1->Release();
	} else {
		FBLOG_ERROR("getD3D10Device", "GetProcAddress D3D10CreateDevice1 failed");
	}

	return hr;
}

//
// D3D10 functions
//----------------------------------------------------------------------------------------------- 

namespace FB
{
	class D3d10DrawingContext;

	class D3d10Helper
	{
	public:
		D3d10Helper();		
		virtual ~D3d10Helper();
		
		HRESULT initDevice();
		HRESULT OpenSharedResources(HANDLE hFrontBufferSharedHandle, HANDLE hBackBufferSharedHandle);

		virtual bool beginDrawAsync(const FB::Rect &posRect, void **asyncDrawingContext);
		virtual bool endDrawAsync();
		virtual void releaseBuffers();
		virtual void freeResources();

	protected:
		ID3D10Device1 *m_device;
		ID3D10Texture2D *m_frontBuffer;
		ID3D10Texture2D *m_backBuffer;
		D3d10DrawingContext m_D3d10DrawingContext;
	};  
}
#endif
#endif
