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

#include "win_common.h"
#include <mshtml.h>
#include <atlctl.h>
#include <map>
#include "BrowserHost.h"
#include "APITypes.h"
#include "FBPointers.h"
#include "SafeQueue.h"
#include "ShareableReference.h"
#include "ActiveXFactoryDefinitions.h"

#include "Win/D3d10DrawingContext.h"
#include "AsyncDrawingService.h"

namespace FB {
    namespace ActiveX {
        FB_FORWARD_PTR(ActiveXBrowserHost);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @class  ActiveXBrowserHost
        ///
        /// @brief  Provides a BrowserHost implementation for ActiveX
        ////////////////////////////////////////////////////////////////////////////////////////////////////
		class ActiveXAsyncDrawingService : public AsyncDrawingService
		{
			public:
				ActiveXAsyncDrawingService(ActiveXBrowserHost* pActiveXBrowserHost) :  m_pActiveXBrowserHost(pActiveXBrowserHost)
				{
				}
				
				virtual bool beginDrawAsync(const FB::Rect &posRect, void **asyncDrawingContext) {
					return m_pActiveXBrowserHost->beginDrawAsync(posRect, asyncDrawingContext);
				}

				virtual bool endDrawAsync() {
					return m_pActiveXBrowserHost->endDrawAsync();
				}

			private:
				ActiveXBrowserHost* m_pActiveXBrowserHost;
		};
	}
}
#endif // H_ACTIVEXASYNCDRAWINGSERVICE
