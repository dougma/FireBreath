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

#include "logging.h"
#include "D3d10AsyncDrawService.h"
#include "BrowserHost.h"

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

D3d10AsyncDrawService::D3d10AsyncDrawService(BrowserHostPtr host)
    : m_weakHost(host)
    , m_width(0)
    , m_height(0)
    , m_dimsChanged(false)
{
    initDevice();
}

D3d10AsyncDrawService::~D3d10AsyncDrawService()
{
    if (m_device) {
        m_device->ClearState();
    }
}

void D3d10AsyncDrawService::resized(uint32_t width, uint32_t height)
{
    m_dimsChanged = (m_width != width) || (m_height != height);

    uint32_t was = m_width * m_height;
    uint32_t now = width * height;

    m_width = width; m_height = height;

    if (was == 0 && now != 0) {
        present(true);
    }
}

void D3d10AsyncDrawService::render(RenderCallback cb)
{
    if (!m_device)
        return;

    boost::unique_lock<boost::mutex> lock(m_mut);
    while (!m_pBackBuffer)
    {
        m_cond.wait(lock);
    }

    CComPtr<ID3D10RenderTargetView> rtView;
    HRESULT hr = m_device->CreateRenderTargetView(m_pBackBuffer, NULL, &rtView);
    if (SUCCEEDED(hr)) {
        D3D10_TEXTURE2D_DESC desc;
        m_pBackBuffer->GetDesc(&desc);

        cb(m_device, rtView, desc.Width, desc.Height);

        BrowserHostPtr host(m_weakHost.lock());
        if (host) {
            if (host->isMainThread()) {
                present(false);
            } else {
                host->ScheduleOnMainThread(this->shared_from_this(), boost::bind(&D3d10AsyncDrawService::present, this, false));
            }
        }
    }
}

HRESULT D3d10AsyncDrawService::initDevice()
{
    return m_device ? S_OK : getD3D10Device(&m_device);
}


