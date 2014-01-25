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
#ifndef H_ACTIVEXD3D10HELPER
#define H_ACTIVEXD3D10HELPER

#include "win_common.h"
#include <mshtml.h>
#include <atlctl.h>
#include "BrowserHost.h"
#include "APITypes.h"
#include "FBPointers.h"

#include "ActiveXBrowserHost.h"
#include "Win/D3d10Helper.h"

struct ISurfacePresenter;

namespace FB {

    namespace ActiveX {
	    class ActiveXBrowserHost;
    
		FB_FORWARD_PTR(ActiveXD3d10Helper);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @class  ActiveXD3d10Helper
        ///
        /// @brief  Provides a D3D10 async drawing implementation for ActiveX
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class ActiveXD3d10Helper :
            public FB::D3d10Helper
        {
        public:
            ActiveXD3d10Helper(ActiveXBrowserHost *theBrowserHost);
            virtual ~ActiveXD3d10Helper(void);

			virtual bool beginDrawAsync(const FB::Rect &posRect, void **asyncDrawingContext);
			virtual bool endDrawAsync();
			virtual void freeResources();  // this must be called instead of destructor!
		
		public:
			HRESULT setAsyncDrawingWindow(void *pIViewObjectPresentSite, const FB::Rect &posRect);

        protected:
			HRESULT _renewAsyncDrawing(const FB::Rect &posRect);

        protected:
			ActiveXBrowserHost *browserHost;
   		    void *m_pIViewObjectPresentSite;
			ISurfacePresenter* m_spISurfacePresenter;

			int m_currentBufferIndex;
	        FB::Rect m_posRect;
        };
    }
}

#endif // H_ACTIVEXD3D10HELPER

