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
#ifndef H_NPAPIASYNCDRAWINGHELPER
#define H_NPAPIASYNCDRAWINGHELPER

#include "NpapiTypes.h"
#include "NpapiBrowserHost.h"

#ifdef _WINDOWS
#include "Win/D3d10Helper.h"
#endif

namespace FB
{
#ifdef _WINDOWS
    FB_FORWARD_PTR(D3d10Helper);
#endif
	
	namespace Npapi 
	{
	    FB_FORWARD_PTR(NpapiAsyncDrawingHelper);
	    typedef boost::shared_ptr<NpapiAsyncDrawingHelper> NpapiAsyncDrawingHelperPtr;    
	}
}

namespace FB { namespace Npapi {

	class NpapiBrowserHost;

	class NpapiAsyncDrawingHelper
#ifdef _WINDOWS
		: public D3d10Helper
#endif
	{
	public:
		NpapiAsyncDrawingHelper(NpapiBrowserHost *theBrowserHost, NpAsyncDrawing mode);		
		virtual ~NpapiAsyncDrawingHelper();

		virtual bool endDrawAsync();
		virtual void freeResources();  // this must be called instead of destructor!

	public: 
		NPError setAsyncDrawingWindow(NPWindow* newWindow);
		NpAsyncDrawing getAsyncDrawingMode() { return asyncDrawing; }

	protected:
	    NpapiBrowserHost *browserHost;

		FB::Npapi::NpAsyncDrawing asyncDrawing;
		NPAsyncSurface *frontBuffer;
		NPAsyncSurface *backBuffer;
	};

}; };

#endif

