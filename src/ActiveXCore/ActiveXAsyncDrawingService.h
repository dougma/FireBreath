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
#ifndef H_ACTIVEXASYNCDRAWINGSERVICE
#define H_ACTIVEXASYNCDRAWINGSERVICE

#include <atlbase.h>
#include <mshtml.h>
#include "APITypes.h"
#include "FBPointers.h"
#include "AsyncDrawingService.h"

namespace FB {
    namespace ActiveX {

        FB_FORWARD_PTR(ActiveXD3d10Helper);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @class  ActiveXAsyncDrawingService
        ///
        /// @brief  Provides a AsyncDrawingService implementation for ActiveX
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class ActiveXAsyncDrawingService : public AsyncDrawingService
        {
        public:
            ActiveXAsyncDrawingService(CComPtr<IViewObjectPresentSite> pViewObjectPresentSite, const FB::Rect &posRect);
            virtual bool beginDrawAsync(const FB::Rect &posRect, void **asyncDrawingContext);
            virtual bool endDrawAsync();

            ActiveXD3d10HelperPtr m_D3d10Helper;
        };
    }
}
#endif // H_ACTIVEXASYNCDRAWINGSERVICE
