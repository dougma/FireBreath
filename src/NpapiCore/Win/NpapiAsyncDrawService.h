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
#ifndef H_NPAPIASYNCDRAWSERVICE
#define H_NPAPIASYNCDRAWSERVICE

#include "NpapiBrowserHost.h"
#include "Win\D3d10Helper.h"

namespace FB {
    namespace Npapi {

        FB_FORWARD_PTR(NpapiAsyncDrawService);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @class  NpapiAsyncDrawService
        ///
        /// @brief  Provides an AsyncDrawService implementation for NPAPI
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class NpapiAsyncDrawService : public D3d10Helper
        {
            // wrap the NPAsyncSurface to track init/finalize
            struct Surface : NPAsyncSurface
            {
                Surface();
                NPError init(NpapiBrowserHost*, NPSize*);
                void finalize(NpapiBrowserHost*);
                void makeCurrent(NpapiBrowserHost*);
                bool sizeIsDifferent(const NPSize&) const;
                bool valid;
            };

            NpapiBrowserHostWeakPtr m_weakHost;
            Surface m_surface[2];
            int m_current;
            NPSize m_dims;
            bool m_dimsChanged;         // since last call to present()

            boost::mutex m_mut;
            boost::condition_variable m_cond;
            CComPtr<ID3D10Texture2D> m_pBackBuffer;
            CComPtr<IDXGIKeyedMutex> m_pBufferMutex;

            void present(bool init);

        public:
            typedef boost::function<bool(ID3D10Device1*, ID3D10RenderTargetView*, uint32_t, uint32_t)> RenderCallback;

            NpapiAsyncDrawService(NpapiBrowserHostPtr host);
            virtual ~NpapiAsyncDrawService() {};

            void render(RenderCallback cb);

            virtual void resized(uint32_t width, uint32_t height);
        };
    };
};

#endif
