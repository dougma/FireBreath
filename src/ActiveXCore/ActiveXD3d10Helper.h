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
#include <mshtml.h> // IViewObjectPresentSite, ISurfacePresenter
#include "APITypes.h"

#include "Win/D3d10Helper.h"

namespace FB {

    namespace ActiveX {

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @class  ActiveXD3d10Helper
        ///
        /// @brief  Provides a D3D10 async drawing implementation for ActiveX
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class ActiveXD3d10Helper :
            public FB::D3d10Helper
        {
        public:
            ActiveXD3d10Helper(IViewObjectPresentSite*);

            virtual bool beginDrawAsync(void **asyncDrawingContext);
            virtual bool endDrawAsync();

        protected:
            HRESULT renewAsyncDrawing();

            CComPtr<IViewObjectPresentSite> m_pViewObjectPresentSite;
            CComPtr<ISurfacePresenter> m_pSurfacePresenter;

            int m_currentBufferIndex;
            unsigned m_width, m_height;
        };
    }
}

#endif // H_ACTIVEXD3D10HELPER

