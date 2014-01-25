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
#ifndef H_FB_ASYNCDRAWINGCONTEXT
#define H_FB_ASYNCDRAWINGCONTEXT

namespace FB {

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  AsyncDrawingContext
    ///
    /// @brief  asynchronous drawing context base class. 
    ///
    /// This is the base class for all asynchronous drawing contexts that are used in the plugin.  
    ////////////////////////////////////////////////////////////////////////////////////////////////////
	class AsyncDrawingContext
    {
    public:
        AsyncDrawingContext() { };
        virtual ~AsyncDrawingContext() { };
    };
};

#endif

