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
#ifndef H_FB_DIRECT3DHELPER
#define H_FB_DIRECT3DHELPER

#include "win_common.h"
#include <d3d10_1.h>
#include "APITypes.h"
#include "D3d10DrawingContext.h"
#include "AsyncDrawingService.h"

namespace FB
{
    class D3d10Helper
        : public AsyncDrawingService
    {
    public:
        virtual ~D3d10Helper();
        HRESULT initDevice();
        HRESULT openSharedResources(HANDLE hFrontBufferSharedHandle, HANDLE hBackBufferSharedHandle);

        virtual bool beginDrawAsync();
        virtual bool endDrawAsync();

    protected:
        CComPtr<ID3D10Device1> m_device;
        CComPtr<ID3D10Texture2D> m_buffer[2];
    };  
}

#endif
