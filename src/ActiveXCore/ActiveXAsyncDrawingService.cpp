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

#include "ActiveXAsyncDrawingService.h"
#include "ActiveXD3d10Helper.h"

using namespace FB::ActiveX;

ActiveXAsyncDrawingService::ActiveXAsyncDrawingService(CComPtr<IViewObjectPresentSite> pViewObjectPresentSite, const FB::Rect &posRect)
    : m_D3d10Helper(boost::make_shared<ActiveXD3d10Helper>(pViewObjectPresentSite, posRect))
{
    assert(pViewObjectPresentSite);
}

bool ActiveXAsyncDrawingService::beginDrawAsync(const FB::Rect &posRect, void **asyncDrawingContext)
{
    return m_D3d10Helper->beginDrawAsync(posRect, asyncDrawingContext);
}

bool ActiveXAsyncDrawingService::endDrawAsync()
{
    return m_D3d10Helper->endDrawAsync();
}
