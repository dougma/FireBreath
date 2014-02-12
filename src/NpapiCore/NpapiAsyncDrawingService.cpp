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

#include "NpapiAsyncDrawingService.h"

using namespace FB::Npapi;

NpapiAsyncDrawingService::NpapiAsyncDrawingService(FB::BrowserHostPtr host)
    : m_host(host)
{
}

/*
	NPError res;

	FBLOG_INFO("NpapiBrowserHost::_initAsyncDrawing", "npAsyncDrawingMode: " << npAsyncDrawingMode);
	if (npAsyncDrawingMode == AD_BITMAP && pluginSupportsAsyncBitmapDrawing()) {
		//NPBool supportsAsyncBitmap = false;
		//res = GetValue(NPNVsupportsAsyncWindowsDXGISurfaceBool, &supportsAsyncBitmap);
		//if (res == NPERR_NO_ERROR && supportsAsyncBitmap) {
		//	NPN_SetValue(instance, NPPVpluginDrawingModel, (void*)NPDrawingModelAsyncBitmapSurface);
		//} else {
		//	return NPERR_GENERIC_ERROR;
		//}
		// GG: not implemented TBD
		return NPERR_GENERIC_ERROR;
	}
	else if(npAsyncDrawingMode == AD_DXGI && pluginSupportsAsyncDXGIDrawing()) {
		NPBool supportsAsyncDXGI = false;
		res = GetValue(NPNVsupportsAsyncWindowsDXGISurfaceBool, &supportsAsyncDXGI);
		if (res == NPERR_NO_ERROR && supportsAsyncDXGI) {
			SetValue(NPPVpluginDrawingModel, (void*)NPDrawingModelAsyncWindowsDXGISurface);
		} else {
			return NPERR_GENERIC_ERROR;
		}
	}
	else {
		return NPERR_GENERIC_ERROR;
	}

	m_asyncDrawingHelper = NpapiAsyncDrawingHelperPtr(new NpapiAsyncDrawingHelper(this, npAsyncDrawingMode));
	if (!m_asyncDrawingHelper)
		return NPERR_OUT_OF_MEMORY_ERROR;

	return NPERR_NO_ERROR;

*/

void NpapiAsyncDrawingService::resized(uint32_t width, uint32_t height)
{
}

bool NpapiAsyncDrawingService::beginDrawAsync()
{
    return true;
}

bool NpapiAsyncDrawingService::endDrawAsync()
{
    return true;
}
