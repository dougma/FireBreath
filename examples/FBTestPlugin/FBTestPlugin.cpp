/**********************************************************\

  Auto-generated FBTestPlugin.cpp

  This file contains the auto-generated main plugin object
  implementation for the ${PLUGIN.name} project

\**********************************************************/

#include <sstream>
#include <stdio.h>
#include "boost/bind.hpp" 

#include "FBTestPluginAPI.h"
#include "SimpleMathAPI.h"
#include "FBTestPlugin.h"
#include "DOM/Window.h"
#include "URI.h"

#ifdef FB_WIN
#include <mshtml.h>
#include "PluginWindowWin.h"
#include "PluginWindowlessWin.h"
#include "ActiveXAsyncDrawService.h"
#include "Win/NpapiAsyncDrawService.h"
#endif

void FBTestPlugin::StaticInitialize()
{
    FBLOG_INFO("StaticInit", "Static Initialize");
    // Place one-time initialization stuff here; As of FireBreath 1.4 this should only
    // be called once per process
}

void FBTestPlugin::StaticDeinitialize()
{
    FBLOG_INFO("StaticInit", "Static Deinitialize");
    // Place one-time deinitialization stuff here. This should be called just before
    // the plugin library is unloaded
}

FBTestPlugin::FBTestPlugin(const std::string& mimetype) :
    m_mimetype(mimetype)
{
}

FBTestPlugin::~FBTestPlugin()
{
    // By resetting the api and then telling the host that the plugin instance is shutting down,
    // we control the lifecycle. As long as m_api isn't stored anywhere else, telling host to
    // shutdown should free the object
    releaseRootJSAPI();
    m_host->freeRetainedObjects();
}

FB::JSAPIPtr FBTestPlugin::createJSAPI()
{
    // Return a SimpleMath object instead of a FBTestPluginAPI:
    if (!m_mimetype.compare("application/x-fbtestplugin-math")) {
        // Create a new simplemath object each time        
        return boost::make_shared<SimpleMathAPI>(m_host);
    }

    // By default, return a FBTestPluginAPI object:
    // m_host is the BrowserHost
    return boost::make_shared<FBTestPluginAPI>(FB::ptr_cast<FBTestPlugin>(shared_from_this()), m_host);
}

bool FBTestPlugin::onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindow*)
{
    // These are just examples; if you don't need them, remove them, as htmlLog
    // does have a performance penalty!
    std::ostringstream oStr;
    oStr << "Mouse down at: " << evt->m_x << ", " << evt->m_y;
    this->m_host->htmlLog(oStr.str());
    return false;
}

bool FBTestPlugin::onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow*)
{
    // These are just examples; if you don't need them, remove them, as htmlLog
    // does have a performance penalty!
    std::ostringstream oStr;
    oStr << "Mouse up at: " << evt->m_x << ", " << evt->m_y;
    this->m_host->htmlLog(oStr.str());
    return false;
}

bool FBTestPlugin::onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow*)
{
    // These are just examples; if you don't need them, remove them, as htmlLog
    // does have a performance penalty!
    std::ostringstream oStr;
    oStr << "Mouse move at: " << evt->m_x << ", " << evt->m_y;
    this->m_host->htmlLog(oStr.str());
    return false;
}

bool FBTestPlugin::onAttached( FB::AttachedEvent *evt, FB::PluginWindow* win)
{
    // This is called when the window is attached; don't start drawing before this!
    FB::PluginWindowlessWin* windowless = dynamic_cast<FB::PluginWindowlessWin*>(win);
    if (windowless) {
        FB::AsyncDrawServicePtr ads = windowless->getAsyncDrawService();
        if (ads) {
            startDrawAsync(ads);
        }
    }
    return false;
}

bool FBTestPlugin::onDetached( FB::DetachedEvent *evt, FB::PluginWindow* )
{
    // This is called when the window is detached! You must not draw after this!
    return false;
}

bool FBTestPlugin::draw( FB::RefreshEvent *evt, FB::PluginWindow* win )
{
    FB::Rect pos = win->getWindowPosition();

#if FB_WIN
    HDC hDC;
    FB::PluginWindowlessWin *wndLess = dynamic_cast<FB::PluginWindowlessWin*>(win);
    FB::PluginWindowWin *wnd = dynamic_cast<FB::PluginWindowWin*>(win);
    PAINTSTRUCT ps;
    if (wndLess) {
        hDC = wndLess->getHDC();
    } else if (wnd) {
        hDC = BeginPaint(wnd->getHWND(), &ps);
        pos.right -= pos.left;
        pos.left = 0;
        pos.bottom -= pos.top;
        pos.top = 0;
    }

    ::SetTextAlign(hDC, TA_CENTER|TA_BASELINE);
    LPCTSTR pszText = _T("FireBreath Plugin!");
    if (!m_mimetype.compare("application/x-fbtestplugin-math")) {
        // Mark 2nd mimetype differently than main mimetype
        pszText =  _T("FireBreath Plugin (Math)!");
    }
    ::TextOut(hDC, pos.left + (pos.right - pos.left) / 2, pos.top + (pos.bottom - pos.top) / 2, pszText, lstrlen(pszText));

    if (wnd) {
        // Release the device context
        EndPaint(wnd->getHWND(), &ps);
    }
#endif

    return true;
}

//////////////////

#include "Scene.h"

//////////////////

void FBTestPlugin::renderThread(FB::AsyncDrawServicePtr ads)
{
    FB::ActiveX::ActiveXAsyncDrawServicePtr aads = 
        FB::ptr_cast<FB::ActiveX::ActiveXAsyncDrawService>(ads);

    FB::Npapi::NpapiAsyncDrawServicePtr nads = 
        FB::ptr_cast<FB::Npapi::NpapiAsyncDrawService>(ads);

    if (!aads && !nads)
        return;

    try {
        Scene scene(asyncTestBgColor(), FB::utf8_to_wstring(getFSPath()));
        do {
            if (aads)
                aads->render(boost::bind(&Scene::render, &scene, _1, _2, _3, _4));
            if (nads)
                nads->render(boost::bind(&Scene::render, &scene, _1, _2, _3, _4));
            boost::this_thread::sleep_for(boost::chrono::milliseconds(20));
        } while(true);
    }
    catch (const std::exception& e) {
    }
    catch (boost::thread_interrupted) {
    }
    catch (...) {
    }
}

void FBTestPlugin::onPluginReady()
{
    FB::DOM::WindowPtr window = m_host->getDOMWindow();
    if (!window)
        return;

    FB::URI uri = FB::URI::fromString(window->getLocation());
    bool log = uri.query_data.find("log") != uri.query_data.end();
    m_host->setEnableHtmlLog(log);
}

//virtual
void FBTestPlugin::ClearWindow()
{
    // the window is about to go away, so we need to stop our render thread
    m_thread.interrupt();
    m_thread.join();
}

bool FBTestPlugin::startDrawAsync(FB::AsyncDrawServicePtr ads)
{
    m_thread = boost::thread(&FBTestPlugin::renderThread, this, ads);
    return true;
}

uint32_t FBTestPlugin::asyncTestBgColor()
{
    if (m_asyncTestBgColor) {
        return *m_asyncTestBgColor;
    }

    FB::VariantMap::iterator itr = m_params.find("color");
    if (itr != m_params.end()) {
        try {
            uint32_t v;
            std::stringstream ss;
            ss << std::hex << itr->second.convert_cast<std::string>();
            ss >> v;
            m_asyncTestBgColor = v;
            return *m_asyncTestBgColor;
        } catch (const FB::bad_variant_cast& ex) {
            FB_UNUSED_VARIABLE(ex);
        }
    }
    m_asyncTestBgColor = 0x7F00FF00; // if no param is given set the default to semi-transparent green (format is 0xAARRGGBB)
    return *m_asyncTestBgColor;
}

