/**********************************************************\

  Auto-generated FBTestPlugin.cpp

  This file contains the auto-generated main plugin object
  implementation for the ${PLUGIN.name} project

\**********************************************************/

#include <sstream>
#include "FBTestPluginAPI.h"
#include "SimpleMathAPI.h"
#include <stdio.h>

#include "FBTestPlugin.h"
#include "DOM/Window.h"
#include "URI.h"

#ifdef FB_WIN
#include <mshtml.h>
#include "PluginWindowWin.h"
#include "PluginWindowlessWin.h"
#include "Win/D3d10DrawingContext.h"
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

bool FBTestPlugin::onAttached( FB::AttachedEvent *evt, FB::PluginWindow* )
{
    // This is called when the window is attached; don't start drawing before this!
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
    FB::PluginWindow::DrawingModel drawingModel = win->getDrawingModel();
    FBLOG_INFO("FBTestPlugin::draw", "PluginWindow::DrawingModel() : " << drawingModel );

#if FB_WIN
    HDC hDC;
    FB::PluginWindowlessWin *wndLess = dynamic_cast<FB::PluginWindowlessWin*>(win);
    FB::PluginWindowWin *wnd = dynamic_cast<FB::PluginWindowWin*>(win);
    PAINTSTRUCT ps;
    if (wndLess) {
        if (drawingModel == FB::PluginWindow::DrawingModelActiveXSurfacePresenter ||
            drawingModel == FB::PluginWindow::DrawingModelNpapiAsyncDXGI) 
        {
            FBLOG_INFO("FBTestPlugin::draw", "calling drawAsync().  win: " << win << "; pos: " << pos.left << "," << pos.top);
            if (drawAsync(win, pos)) {
                return true;
            }
            wndLess->setDrawingModel(FB::PluginWindow::DrawingModelWindowless); // oh oh: in npapi you can call the function to set this only during Initialization TBD!
        }

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

bool FBTestPlugin::drawAsync(FB::PluginWindow* win, FB::Rect pos)
{
#if FB_WIN
    FBLOG_INFO("FBTestPlugin::drawAsync", "PluginCore::drawAsync() pos: " << pos.left << "," << pos.top );

    FB::PluginWindowlessWin *wndLess = dynamic_cast<FB::PluginWindowlessWin*>(win);
    if(wndLess == NULL)
        return false;

    FB::AsyncDrawingService *drawingService;
    drawingService = dynamic_cast<FB::AsyncDrawingService *>(wndLess->getPlatformAsyncDrawingService());

    if(!drawingService)
        return false;

    void *asyncDrawingContext;
    FB::D3d10DrawingContext *drawingContext;

    FBLOG_INFO("FBTestPlugin::drawAsync", "calling beginDrawAsync...");
    if(drawingService->beginDrawAsync(pos, &asyncDrawingContext)==false)
        return false;

    drawingContext = reinterpret_cast<FB::D3d10DrawingContext *>(asyncDrawingContext);
    if(drawingContext == NULL)
        return false;

    uint32_t rgba = asyncTestBgColor();

    unsigned char subpixels[4];
    subpixels[0] = rgba & 0xFF;
    subpixels[1] = (rgba & 0xFF00) >> 8;
    subpixels[2] = (rgba & 0xFF0000) >> 16;
    subpixels[3] = (rgba & 0xFF000000) >> 24;

    float color[4];
    color[2] = float(subpixels[3] * subpixels[0]) / 0xFE01;
    color[1] = float(subpixels[3] * subpixels[1]) / 0xFE01;
    color[0] = float(subpixels[3] * subpixels[2]) / 0xFE01;
    color[3] = float(subpixels[3]) / 0xFF;
    drawingContext->dev->ClearRenderTargetView(drawingContext->rtView, color);

    FBLOG_INFO("FBTestPlugin::drawAsync", "calling endDrawAsync...");
    if(!drawingService->endDrawAsync()) {
        return false;
    }
#endif    
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

void FBTestPlugin::onPluginReady()
{
    FB::DOM::WindowPtr window = m_host->getDOMWindow();
    if (!window)
        return;

    FB::URI uri = FB::URI::fromString(window->getLocation());
    bool log = uri.query_data.find("log") != uri.query_data.end();
    m_host->setEnableHtmlLog(log);
}
