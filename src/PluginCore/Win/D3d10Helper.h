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
#include "AsyncDrawService.h"

namespace FB
{
    class D3d10Helper
        : public AsyncDrawService
    {
    public:
        virtual ~D3d10Helper();
        HRESULT initDevice();

    protected:
        CComPtr<ID3D10Device1> m_device;
    };  
}

#endif
