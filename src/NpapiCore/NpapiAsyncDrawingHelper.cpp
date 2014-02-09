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

#include "NpapiAsyncDrawingHelper.h"

using namespace FB::Npapi;

NpapiAsyncDrawingHelper::NpapiAsyncDrawingHelper(NpapiBrowserHost *theBrowserHost, NpAsyncDrawing mode) :
#ifdef _WINDOWS
	D3d10Helper(),
#endif
	browserHost(theBrowserHost), asyncDrawing(mode), frontBuffer(NULL), backBuffer(NULL)
{
}

NpapiAsyncDrawingHelper::~NpapiAsyncDrawingHelper()
{
}

void NpapiAsyncDrawingHelper::freeResources()
{	
	if (browserHost) {
		if (frontBuffer) {
			browserHost->FinalizeAsyncSurface(frontBuffer);
			browserHost->MemFree(frontBuffer);
			frontBuffer = NULL;
		}
		if (backBuffer) {
			browserHost->FinalizeAsyncSurface(backBuffer);
			browserHost->MemFree(backBuffer);
			backBuffer = NULL;
		}
	}
}

NPError NpapiAsyncDrawingHelper::setAsyncDrawingWindow(NPWindow* newWindow)
{
	FBLOG_INFO("NpapiAsyncDrawingHelper::setAsyncDrawingWindow", "newWindow: " << newWindow);

	if(asyncDrawing == AD_BITMAP) {
		// TBD not implemented
		return NPERR_GENERIC_ERROR;
	}
	else if(asyncDrawing == AD_NONE) {
		return NPERR_NO_ERROR;
	}

	if (frontBuffer &&
		frontBuffer->size.width == newWindow->width &&
		frontBuffer->size.height == newWindow->height) {
			return NPERR_NO_ERROR;
	}

//	releaseBuffers();
	
	if (frontBuffer) {
		browserHost->FinalizeAsyncSurface(frontBuffer);
		browserHost->MemFree(frontBuffer);
		frontBuffer = NULL;
	}
	if (backBuffer) {
		browserHost->FinalizeAsyncSurface(backBuffer);
		browserHost->MemFree(backBuffer);
		backBuffer = NULL;
	}
	
	HRESULT hr = initDevice();
	if(FAILED(hr)) {
		FBLOG_ERROR("NpapiAsyncDrawingHelper::setAsyncDrawingWindow", "initDevice failed");
		return NPERR_GENERIC_ERROR; 
	}

	FBLOG_INFO("NpapiAsyncDrawingHelper::setAsyncDrawingWindow", "allocating buffers");
	frontBuffer = (NPAsyncSurface*)browserHost->MemAlloc(sizeof(NPAsyncSurface));
	backBuffer = (NPAsyncSurface*)browserHost->MemAlloc(sizeof(NPAsyncSurface));

	NPSize size;
	size.width = newWindow->width;
	size.height = newWindow->height;

	memset(frontBuffer, 0, sizeof(NPAsyncSurface));
	memset(backBuffer, 0, sizeof(NPAsyncSurface));

	browserHost->InitAsyncSurface(&size, NPImageFormatBGRA32, NULL, frontBuffer);
	browserHost->InitAsyncSurface(&size, NPImageFormatBGRA32, NULL, backBuffer);

	hr = openSharedResources(frontBuffer->sharedHandle, backBuffer->sharedHandle);
	if(FAILED(hr)) {
		FBLOG_ERROR("NpapiAsyncDrawingHelper::setAsyncDrawingWindow", "OpenSharedResources failed");
		return NPERR_GENERIC_ERROR; 
	}

	FBLOG_INFO("NpapiAsyncDrawingHelper::setAsyncDrawingWindow", "setAsyncDrawingWindow done");
	return NPERR_NO_ERROR;
}

bool NpapiAsyncDrawingHelper::endDrawAsync()
{
	if(D3d10Helper::endDrawAsync()) {
		browserHost->SetCurrentAsyncSurface(backBuffer, NULL);
		NPAsyncSurface *oldFront = frontBuffer;
		frontBuffer = backBuffer;
		backBuffer = oldFront;
		FBLOG_INFO("NpapiAsyncDrawingHelper::endDrawAsync", "done");
		return true;
	}
	FBLOG_INFO("NpapiAsyncDrawingHelper::endDrawAsync", "D3d10Helper::endDrawAsync failed");
	return false;
}

void NpapiAsyncDrawingHelper::resized(uint32_t width, uint32_t height)
{
}
