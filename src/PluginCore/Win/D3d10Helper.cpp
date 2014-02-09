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
#include <boost/smart_ptr/make_shared.hpp>
#include "logging.h"

#include "D3d10DrawingContext.h"
#include "D3d10Helper.h"

using namespace FB;


//----------------------------------------------------------------------------------------------- 
// D3D10 functions
//

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

D3d10Helper::~D3d10Helper()
{
    if (m_device) {
        m_device->ClearState();
    }
}

HRESULT D3d10Helper::initDevice()
{
    return m_device ? S_OK : getD3D10Device(&m_device);
}

HRESULT D3d10Helper::openSharedResources(HANDLE hFrontBufferSharedHandle, HANDLE hBackBufferSharedHandle) 
{
    if (!m_device)
        return S_FALSE;

    HRESULT hr = m_device->OpenSharedResource(hFrontBufferSharedHandle, __uuidof(ID3D10Texture2D), (void**)&m_buffer[0]);
    if (SUCCEEDED(hr)) {
        hr = m_device->OpenSharedResource(hBackBufferSharedHandle, __uuidof(ID3D10Texture2D), (void**)&m_buffer[1]);
    }
    return hr;
}

bool D3d10Helper::beginDrawAsync()
{
    return false;
}

bool D3d10Helper::endDrawAsync()
{
    return false;
}
