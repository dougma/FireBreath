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
#include "logging.h"

using namespace FB;
using namespace FB::ActiveX;

ActiveXD3d10Helper::ActiveXD3d10Helper(IViewObjectPresentSite* vops)
    : m_pViewObjectPresentSite(vops)
    , m_currentBufferIndex(0)
    , m_width(0)
    , m_height(0)
{
    assert(vops);
    initDevice();
}

HRESULT FB::ActiveX::ActiveXD3d10Helper::renewAsyncDrawing()
{
    if (!m_pViewObjectPresentSite) {
        FBLOG_ERROR("ActiveXD3d10Helper::_renewAsyncDrawing", "m_pIViewObjectPresentSite is NULL");
        return S_FALSE;
    }

/* do this on the swap?

    BOOL isCurrent;
    if (m_pSurfacePresenter) {
        // check if the device that is used to create the surface presenter front buffer is still the current device that is used to compose with.
        hr = m_pSurfacePresenter->IsCurrent(&isCurrent);
        if (FAILED(hr)) {
            FBLOG_ERROR("ActiveXD3d10Helper::_renewAsyncDrawing", "ISurfacePresenter->IsCurrent failed");
            return hr;
        }
        if (!isCurrent || sizeChanged) {
            FBLOG_INFO("ActiveXD3d10Helper::_renewAsyncDrawing", "ISurfacePresenter->IsCurrent = false || posRectChanged. Releasing...");
            m_pSurfacePresenter = 0;
        } 
    }
*/

/* done much earlier...

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
*/


    FBLOG_INFO("ActiveXD3d10Helper::_renewAsyncDrawing", "_renewAsyncDrawing done");
    return S_OK;
}

bool FB::ActiveX::ActiveXD3d10Helper::beginDrawAsync(void **asyncDrawingContext)
{
    renewAsyncDrawing();
    return D3d10Helper::beginDrawAsync();
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

