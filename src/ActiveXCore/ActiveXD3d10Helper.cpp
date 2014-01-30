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

#include "ActiveXD3d10Helper.h"

#include <boost/smart_ptr/make_shared.hpp>

using namespace FB;
using namespace FB::ActiveX;

ActiveXD3d10Helper::ActiveXD3d10Helper(CComPtr<IViewObjectPresentSite> pViewObjectPresentSite, const FB::Rect &posRect)
    : m_pViewObjectPresentSite(pViewObjectPresentSite)
    , m_currentBufferIndex(0)
    , m_posRect(posRect)
{
}

void ActiveXD3d10Helper::freeResources()
{
    D3d10Helper::freeResources();
}

HRESULT FB::ActiveX::ActiveXD3d10Helper::_renewAsyncDrawing(const FB::Rect &posRect)
{
    HRESULT hr;

    if (!m_pViewObjectPresentSite) {
        FBLOG_ERROR("ActiveXD3d10Helper::_renewAsyncDrawing", "m_pIViewObjectPresentSite is NULL");
        return S_FALSE;
    }

    hr = initDevice();
    if (FAILED(hr)) {
        FBLOG_ERROR("ActiveXD3d10Helper::_renewAsyncDrawing", "m_device==null");
        return S_FALSE;
    }

    BOOL posRectChanged = false;
    if (m_posRect.left != posRect.left || m_posRect.right != posRect.right || m_posRect.top != posRect.top || m_posRect.bottom != posRect.bottom) {
        FBLOG_INFO("ActiveXD3d10Helper::_renewAsyncDrawing", "posRectChanged. Releasing...");
        posRectChanged = true;

        m_posRect.left = posRect.left;
        m_posRect.right = posRect.right;
        m_posRect.top = posRect.top;
        m_posRect.bottom = posRect.bottom;
    }

    BOOL isCurrent;
    if (m_pSurfacePresenter) {
        // check if the device that is used to create the surface presenter front buffer is still the current device that is used to compose with.
        hr = m_pSurfacePresenter->IsCurrent(&isCurrent);
        if(FAILED(hr)) {
            FBLOG_ERROR("ActiveXD3d10Helper::_renewAsyncDrawing", "ISurfacePresenter->IsCurrent failed");
            return hr;
        }
        if(!isCurrent || posRectChanged) {
            FBLOG_INFO("ActiveXD3d10Helper::_renewAsyncDrawing", "ISurfacePresenter->IsCurrent = false || posRectChanged. Releasing...");
            m_pSurfacePresenter = NULL;
        } 
    }

    UINT window_width, window_height;
    window_width = m_posRect.right - m_posRect.left;
    window_height = m_posRect.bottom - m_posRect.top;

    UINT backBufferCount = 2;


    if (!m_pSurfacePresenter) {
        m_currentBufferIndex = 0;
        hr = m_pViewObjectPresentSite->CreateSurfacePresenter(
            m_device, window_width, window_height, backBufferCount, DXGI_FORMAT_B8G8R8A8_UNORM, VIEW_OBJECT_ALPHA_MODE_PREMULTIPLIED, &m_pSurfacePresenter);
        if (FAILED(hr) || !m_pSurfacePresenter) {
            FBLOG_ERROR("ActiveXD3d10Helper::_renewAsyncDrawing", "CreateSurfacePresenter failed");
            return hr;
        }
    }

    BOOL IsHardwareComposition;
    hr = m_pViewObjectPresentSite->IsHardwareComposition(&IsHardwareComposition);
    if (FAILED(hr || !IsHardwareComposition)) {
        FBLOG_ERROR("ActiveXD3d10Helper::_renewAsyncDrawing", "IsHardwareComposition failed");
        return hr;
    }

    hr = m_pViewObjectPresentSite->SetCompositionMode(VIEW_OBJECT_COMPOSITION_MODE_SURFACEPRESENTER);
    if (FAILED(hr)) {
        FBLOG_ERROR("ActiveXD3d10Helper::_renewAsyncDrawing", "SetCompositionMode failed");
        return hr;
    }

    REFIID riid = __uuidof( ID3D10Texture2D );
    hr = m_pSurfacePresenter->GetBuffer( m_currentBufferIndex, riid, (LPVOID*)&m_backBuffer);
    if (FAILED(hr)) {
        FBLOG_ERROR("ActiveXD3d10Helper::_renewAsyncDrawing", "ISurfacePresenter GetBuffer failed");
        return hr;
    }

    FBLOG_INFO("ActiveXD3d10Helper::_renewAsyncDrawing", "_renewAsyncDrawing done");
    return S_OK;
}

bool FB::ActiveX::ActiveXD3d10Helper::beginDrawAsync(const FB::Rect &posRect, void **asyncDrawingContext)
{
    HRESULT hr;

    hr = _renewAsyncDrawing(posRect);
    if (FAILED(hr)) {
        FBLOG_ERROR("ActiveXD3d10Helper::beginDrawAsync", "_renewAsyncDrawing failed");
        return false;
    }
    return D3d10Helper::beginDrawAsync(posRect, asyncDrawingContext);
}

bool FB::ActiveX::ActiveXD3d10Helper::endDrawAsync()
{
    HRESULT hr = m_pSurfacePresenter->Present(m_currentBufferIndex, NULL/*&m_posRect*/);

    D3d10Helper::endDrawAsync();

    if (FAILED(hr)) {
        FBLOG_ERROR("ActiveXD3d10Helper::endDrawAsync", "ISurfacePresenter Present failed");
        return FALSE;
    }

    m_currentBufferIndex = 1 - m_currentBufferIndex;

    FBLOG_INFO("ActiveXD3d10Helper::endDrawAsync", "done");
    return TRUE;
}

