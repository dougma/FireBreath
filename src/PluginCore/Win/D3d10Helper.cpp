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

D3d10Helper::D3d10Helper() : m_device(NULL), m_frontBuffer(NULL), m_backBuffer(NULL)
{ 
}

D3d10Helper::~D3d10Helper() 
{
}	

HRESULT D3d10Helper::initDevice()
{
	if(m_device)
		return S_OK;

	HRESULT hr = getD3D10Device(&m_device);
	return hr;
}

HRESULT D3d10Helper::OpenSharedResources(HANDLE hFrontBufferSharedHandle, HANDLE hBackBufferSharedHandle) 
{
	HRESULT hr;
	if(!m_device)
		return S_FALSE;
	hr = m_device->OpenSharedResource(hFrontBufferSharedHandle, __uuidof(ID3D10Texture2D), (void**)&m_frontBuffer);
	if(SUCCEEDED(hr)) {
		hr = m_device->OpenSharedResource(hBackBufferSharedHandle, __uuidof(ID3D10Texture2D), (void**)&m_backBuffer);
	}
	return hr;
}

bool D3d10Helper::beginDrawAsync(const FB::Rect &posRect, void **asyncDrawingContext)
{
	FBLOG_INFO("D3d10Helper::beginDrawAsync", "entering");

	if(!m_device) {
		FBLOG_INFO("D3d10Helper::beginDrawAsync", "m_device==null");
		return false;
	}

	if(!m_backBuffer) {
		FBLOG_INFO("D3d10Helper::beginDrawAsync", "m_backBuffer==null");
		return false;
	}

	IDXGIKeyedMutex *mutex;
	m_backBuffer->QueryInterface(&mutex);

	mutex->AcquireSync(0, INFINITE);
	assert(mutex);
	if(!mutex)
		return false;
	ID3D10RenderTargetView *rtView;
	m_device->CreateRenderTargetView(m_backBuffer, NULL, &rtView);
	assert(rtView);
	if(!rtView)
		return false;

	m_D3d10DrawingContext.dev = m_device;
	m_D3d10DrawingContext.rtView = rtView;
	m_D3d10DrawingContext.mutex = mutex;

	*asyncDrawingContext = (void *)&m_D3d10DrawingContext;
	FBLOG_INFO("D3d10Helper::beginDrawAsync", "done");
	return true;
}

bool D3d10Helper::endDrawAsync()
{
	FBLOG_INFO("D3d10Helper::endDrawAsync", "entering");

	m_D3d10DrawingContext.rtView->Release();
	m_D3d10DrawingContext.mutex->ReleaseSync(0);
	m_D3d10DrawingContext.mutex->Release();

	m_D3d10DrawingContext.dev = NULL;
	m_D3d10DrawingContext.mutex = NULL;
	m_D3d10DrawingContext.rtView = NULL;

	ID3D10Texture2D *oldFrontT = m_frontBuffer;
	m_frontBuffer = m_backBuffer;
	m_backBuffer = oldFrontT;

	FBLOG_INFO("D3d10Helper::endDrawAsync", "done");
	return true;
}

void D3d10Helper::releaseBuffers() 
{
	if (m_frontBuffer) {
		m_frontBuffer->Release();
		m_frontBuffer = NULL;
	}
	if (m_backBuffer) {
		m_backBuffer->Release();
		m_backBuffer = NULL;
	}
}

void D3d10Helper::freeResources()
{
	FBLOG_INFO("D3d10Helper::freeResources", "release D3D10 resources");
	releaseBuffers();

	if (m_device) {
		m_device->ClearState();
		m_device = NULL;
	}
}
