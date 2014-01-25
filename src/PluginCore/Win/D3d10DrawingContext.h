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
#ifndef H_FB_D3DD10RAWINGCONTEXT
#define H_FB_D3D10DRAWINGCONTEXT

#ifndef _WINDOWS
#define ID3D10Device1 void *
#define ID3D10RenderTargetView void *
#define IDXGIKeyedMutex void *
#else
#include "win_common.h"
#include <d3d10_1.h>
#endif

#include "AsyncDrawingContext.h"

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

namespace FB
{
	class D3d10DrawingContext : public AsyncDrawingContext
    {
		public:
			D3d10DrawingContext()
				: dev(NULL), rtView(NULL), mutex(NULL) 
			{
			}

			D3d10DrawingContext(ID3D10Device1 *_dev, ID3D10RenderTargetView *_rtView, IDXGIKeyedMutex *_mutex)
				: dev(_dev), rtView(_rtView), mutex(_mutex)
			{ }
		public:
			ID3D10Device1 *dev;
			ID3D10RenderTargetView *rtView;
			IDXGIKeyedMutex *mutex;
	};  
}
#endif
