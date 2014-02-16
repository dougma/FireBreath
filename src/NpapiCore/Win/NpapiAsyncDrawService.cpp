#include "NpapiAsyncDrawService.h"

using namespace FB::Npapi;


NpapiAsyncDrawService::NpapiAsyncDrawService(NpapiBrowserHostPtr host)
    : m_weakHost(host)
    , m_current(0)
    , m_dimsChanged(false)
{
    m_dims.height = m_dims.width = 0;
    initDevice();
}

void 
NpapiAsyncDrawService::resized(uint32_t width, uint32_t height)
{
    m_dimsChanged = (m_dims.width != width) || (m_dims.height != height);

    uint32_t was = m_dims.width * m_dims.height;
    uint32_t now = width * height;

    m_dims.width = width; m_dims.height = height;

    if (was == 0 && now != 0) {
        present(true);
    }
}

void NpapiAsyncDrawService::present(bool initOnly)
{
    if (m_pBufferMutex) {
        m_pBufferMutex->ReleaseSync(0);
        m_pBufferMutex = 0;
    }

    NpapiBrowserHostPtr hostPtr = m_weakHost.lock();
    NpapiBrowserHost* host = hostPtr.get();

    if (!m_device || !host)
        return;

    int next = 1 - m_current;
    m_device->Flush();
    m_surface[next].makeCurrent(host);
    int prev = m_current;
    m_current = next;

    // create or (if the size has changed) recreate the back buffer
    if (m_surface[prev].sizeIsDifferent(m_dims)) {
        if (m_dims.width && m_dims.height) {
            m_surface[prev].init(host, &m_dims);
        }
    }
    
    // send the back buffer to the render thread
    {
        boost::lock_guard<boost::mutex> lock(m_mut);
        REFIID riid = __uuidof(ID3D10Texture2D);
        if (m_surface[prev].valid) {
            HRESULT hr = m_device->OpenSharedResource(m_surface[prev].sharedHandle, __uuidof(ID3D10Texture2D), (void**) &m_pBackBuffer.p);
            if (SUCCEEDED(hr)) {
                m_pBackBuffer->QueryInterface(&m_pBufferMutex);
                if (m_pBufferMutex) {
                    hr = m_pBufferMutex->AcquireSync(0, 50);
                    if (FAILED(hr)) {
                        m_pBufferMutex = 0;
                    }
                }

                m_cond.notify_one();
            }
        }
    }
}


void NpapiAsyncDrawService::render(RenderCallback cb)
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

    BrowserHostPtr host(m_weakHost.lock());
    if (host) {
        if (host->isMainThread()) {
            present(false);
        } else {
            host->ScheduleOnMainThread(this->shared_from_this(), boost::bind(&NpapiAsyncDrawService::present, this, false));
        }
    }
}


NpapiAsyncDrawService::Surface::Surface()
    : valid(false)
{
    version = 0;
}

NPError NpapiAsyncDrawService::Surface::init(NpapiBrowserHost* host, NPSize* size)
{
    if (valid) {
        finalize(host);
    }
    NPError result = host->InitAsyncSurface(size, NPImageFormatBGRA32, NULL /*initData*/, this);
    valid = (0 == result);
    return result;
}

void NpapiAsyncDrawService::Surface::finalize(NpapiBrowserHost* host)
{
    if (valid) {
        host->FinalizeAsyncSurface(this);
        valid = false;
    }
}

void NpapiAsyncDrawService::Surface::makeCurrent(NpapiBrowserHost* host)
{
    host->SetCurrentAsyncSurface(valid ? this : 0, NULL);
}

bool NpapiAsyncDrawService::Surface::sizeIsDifferent(const NPSize& b) const
{
    return !valid || b.height != size.height || b.width != size.width;
}
