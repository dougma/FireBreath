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
#ifndef H_FB_NPAPI_NPAPIASYNCDRAWINGSERVICE
#define H_FB_NPAPI_NPAPIASYNCDRAWINGSERVICE

#include "NpapiTypes.h"
#include "NpapiBrowserHost.h"

namespace FB {   
    namespace Npapi {

	class NpapiAsyncDrawingService : public AsyncDrawingService
	{
		public:
			NpapiAsyncDrawingService(NpapiBrowserHost* pNpapiBrowserHost) : m_pNpapiBrowserHost(pNpapiBrowserHost) 
			{
			}

			virtual bool beginDrawAsync(const FB::Rect &posRect, void **asyncDrawingContext)
			{
				return m_pNpapiBrowserHost->beginDrawAsync(posRect, asyncDrawingContext);
			}

			virtual bool endDrawAsync()
			{
				return m_pNpapiBrowserHost->endDrawAsync();
			}
		private:
			NpapiBrowserHost* m_pNpapiBrowserHost;
	};
}; }; // FB::Npapi

#endif
