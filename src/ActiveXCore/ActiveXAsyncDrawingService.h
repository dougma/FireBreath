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
#include "AsyncDrawingService.h"
#include "Win\D3d10Helper.h"

namespace FB {
    namespace ActiveX {

        FB_FORWARD_PTR(ActiveXAsyncDrawingService);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @class  ActiveXAsyncDrawingService
        ///
        /// @brief  Provides a AsyncDrawingService implementation for ActiveX
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class ActiveXAsyncDrawingService : public D3d10Helper
        {
        public:
            typedef boost::function<bool(ID3D10Device1*, ID3D10RenderTargetView*, uint32_t, uint32_t)> RenderCallback;

            ActiveXAsyncDrawingService(FB::BrowserHostPtr host, IViewObjectPresentSite*);
            virtual void resized(uint32_t width, uint32_t height);
            void render(RenderCallback cb);

        private:
            void present(bool init);

            BrowserHostWeakPtr m_host;
            CComPtr<IViewObjectPresentSite> m_pViewObjectPresentSite;
            CComPtr<ISurfacePresenter> m_pSurfacePresenter;
            CComPtr<ID3D10Texture2D> m_pBackBuffer;
            boost::mutex m_mut;
            boost::condition_variable m_cond;
            uint32_t m_width, m_height;
            bool m_dimsChanged;
        };
    }
}
#endif // H_ACTIVEXASYNCDRAWINGSERVICE
