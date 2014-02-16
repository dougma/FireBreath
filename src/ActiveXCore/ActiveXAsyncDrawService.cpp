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

#include <boost/bind.hpp>
#include "BrowserHost.h"
#include "ActiveXAsyncDrawService.h"
#include "logging.h"

using namespace FB::ActiveX;

ActiveXAsyncDrawService::ActiveXAsyncDrawService(FB::BrowserHostPtr host, IViewObjectPresentSite* vops)
    : m_host(host)
    , m_pViewObjectPresentSite(vops)
    , m_width(0), m_height(0)
    , m_dimsChanged(false)
{
    assert(vops);
    initDevice();
}

void ActiveXAsyncDrawService::resized(uint32_t width, uint32_t height)
{
    m_dimsChanged = (m_width != width) || (m_height != height);

    uint32_t was = m_width * m_width;
    uint32_t now = width * height;

    m_width = width; m_height = height;

    if (was == 0 && now != 0) {
        present(true);
    }
}

void ActiveXAsyncDrawService::present(bool initOnly)
{
    if (!m_pViewObjectPresentSite || !m_device)
        return;

    if (m_pSurfacePresenter) {
        BOOL isCurrent = 0;
        m_pSurfacePresenter->IsCurrent(&isCurrent);
        if (!isCurrent || m_dimsChanged) {
            // don't bother presenting, because the new 
            // SurfacePresenter will replace it with blank
            m_pSurfacePresenter = 0;
        } else {
            if (!initOnly) {
                m_pSurfacePresenter->Present(0, NULL);
            }
        }
    } 
    
    if (!m_pSurfacePresenter) {
        // (re)create surface presenter
        m_dimsChanged = false;
        UINT backBuffers = 1;
        DXGI_FORMAT format = DXGI_FORMAT_B8G8R8A8_UNORM;    // IE9 only supports this one
        HRESULT hr = m_pViewObjectPresentSite->CreateSurfacePresenter(
            m_device, m_width, m_height, backBuffers, format, VIEW_OBJECT_ALPHA_MODE_PREMULTIPLIED, &m_pSurfacePresenter);
        if (FAILED(hr) || !m_pSurfacePresenter) {
            FBLOG_ERROR("ActiveXAsyncDrawService::present", "CreateSurfacePresenter failed");
            return;
        }
    } 

    // send the back buffer to the render thread
    {
        boost::lock_guard<boost::mutex> lock(m_mut);
        REFIID riid = __uuidof(ID3D10Texture2D);
        m_pSurfacePresenter->GetBuffer(0, riid, (void**) &m_pBackBuffer.p);
    }
    m_cond.notify_one();
}



void ActiveXAsyncDrawService::render(RenderCallback cb)
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
    }

    BrowserHostPtr host(m_host.lock());
    if (host) {
        if (host->isMainThread()) {
            present(false);
        } else {
            host->ScheduleOnMainThread(this->shared_from_this(), boost::bind(&ActiveXAsyncDrawService::present, this, false));
        }
    }
}

