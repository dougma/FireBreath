/**********************************************************\
Original Author: Gil Gonen

Created:    Jan 3, 2013
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2013 Gil Gonen and the Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_FB_ASYNCDRAWINGSERVICE
#define H_FB_ASYNCDRAWINGSERVICE

namespace FB {
	struct Rect;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  AsyncDrawingService
    ///
    /// @brief  asynchronous drawing service base class. 
    ///
    /// This is the base class for all asynchronous drawing services that are used in the plugin.  
    ////////////////////////////////////////////////////////////////////////////////////////////////////
	class AsyncDrawingService
	{
		public:
			virtual bool beginDrawAsync(const FB::Rect &posRect, void **asyncDrawingContext) = 0;
			virtual bool endDrawAsync() = 0;
	};
};

#endif

