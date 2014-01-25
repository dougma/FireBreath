/**********************************************************\

  Auto-generated FBTestPlugin.cpp

  This file contains the auto-generated main plugin object
  implementation for the ${PLUGIN.name} project

\**********************************************************/
#ifndef H_TEMPLATEPLUGIN
#define H_TEMPLATEPLUGIN

#include <boost/scoped_ptr.hpp>

#include "PluginEvents/MouseEvents.h"
#include "PluginEvents/DrawingEvents.h"
#include "PluginEvents/AttachedEvent.h"

#include "PluginCore.h"
#include "PluginWindow.h"


//----------------------------------------------------------------------------
// Adapted from Mozilla sample plugin code
//

#if defined(WIN32) || defined(OS2)
  /*
   * Win32 and OS/2 don't know C99, so define [u]int_16/32/64 here. The bool
   * is predefined tho, both in C and C++.
   */
  typedef unsigned int uint32_t;
#elif defined(_AIX) || defined(__sun) || defined(__osf__) || defined(IRIX) || defined(HPUX)
  /*
   * AIX and SunOS ship a inttypes.h header that defines [u]int32_t,
   * but not bool for C.
   */
  #include <inttypes.h>
#elif defined(bsdi) || defined(FREEBSD) || defined(OPENBSD)
  /*
   * BSD/OS, FreeBSD, and OpenBSD ship sys/types.h that define int32_t and
   * u_int32_t.
   */
  #include <sys/types.h>

  /*
   * BSD/OS ships no header that defines uint32_t, nor bool (for C)
   */
  #if defined(bsdi)
  typedef u_int32_t uint32_t;
  typedef u_int64_t uint64_t;

  #else
  /*
   * FreeBSD and OpenBSD define uint32_t and bool.
   */
    #include <inttypes.h>
    #include <stdbool.h>
  #endif
#elif defined(BEOS)
  #include <inttypes.h>
#else
  /*
   * For those that ship a standard C99 stdint.h header file, include
   * it. Can't do the same for stdbool.h tho, since some systems ship
   * with a stdbool.h file that doesn't compile!
   */
  #include <stdint.h>
#endif
//
// --> Adapted from Mozilla sample plugin code
//----------------------------------------------------------------------------

#if 1
#undef HAS_LEAKFINDER
#endif

class LeakFinderXmlOutput;

class FBTestPlugin : public FB::PluginCore
{
public:
    static void StaticInitialize();
    static void StaticDeinitialize();

public:
    FBTestPlugin(const std::string& mimetype);
    virtual ~FBTestPlugin();

public:
	bool drawAsync(FB::PluginWindow* win, FB::Rect pos);

public:
    virtual FB::JSAPIPtr createJSAPI();
    
    std::string getPluginPath() { return m_filesystemPath; }

    bool isWindowless();
	uint32_t asyncTestBgColor();
    void onPluginReady();
	virtual void SetWindow( FB::PluginWindow* win);

    BEGIN_PLUGIN_EVENT_MAP()
        EVENTTYPE_CASE(FB::MouseDownEvent, onMouseDown, FB::PluginWindow)
        EVENTTYPE_CASE(FB::MouseUpEvent, onMouseUp, FB::PluginWindow)
        EVENTTYPE_CASE(FB::MouseMoveEvent, onMouseMove, FB::PluginWindow)
        EVENTTYPE_CASE(FB::AttachedEvent, onAttached, FB::PluginWindow)
        EVENTTYPE_CASE(FB::DetachedEvent, onDetached, FB::PluginWindow)
        EVENTTYPE_CASE(FB::RefreshEvent, draw, FB::PluginWindow)
    END_PLUGIN_EVENT_MAP()

    /** BEGIN EVENTDEF -- DON'T CHANGE THIS LINE **/
    virtual bool onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindow*);
    virtual bool onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow*);
    virtual bool onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow*);
    virtual bool onAttached(FB::AttachedEvent *evt, FB::PluginWindow*);
    virtual bool onDetached(FB::DetachedEvent *evt, FB::PluginWindow*);
    virtual bool draw(FB::RefreshEvent *evt, FB::PluginWindow*);
    /** END EVENTDEF -- DON'T CHANGE THIS LINE **/
#ifdef HAS_LEAKFINDER
    static boost::scoped_ptr<LeakFinderXmlOutput> pOut;
#endif
  private:
    std::string m_mimetype;
    uint32_t m_asyncTestBgColor;

};

#endif

