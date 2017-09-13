#if ! defined(__ANDROID__) && defined(__linux__) && defined(FSL_WINDOWSYSTEM_X11)
/****************************************************************************************************************************************************
* Copyright (c) 2014 Freescale Semiconductor, Inc.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*    * Redistributions of source code must retain the above copyright notice,
*      this list of conditions and the following disclaimer.
*
*    * Redistributions in binary form must reproduce the above copyright notice,
*      this list of conditions and the following disclaimer in the documentation
*      and/or other materials provided with the distribution.
*
*    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
*      its contributors may be used to endorse or promote products derived from
*      this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
****************************************************************************************************************************************************/

#include <FslNativeWindowPlatform/X11/PlatformNativeWindowX11.hpp>
#include <FslNativeWindowPlatform/X11/PlatformNativeWindowSystemX11.hpp>
#include <FslNativeWindow/INativeWindowEventQueue.hpp>
#include <FslNativeWindow/NativeWindowEventHelper.hpp>
#include <FslNativeWindow/NativeWindowSetup.hpp>
#include <FslNativeWindow/NativeWindowSystemSetup.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <X11/extensions/Xrandr.h>
#include <X11/keysym.h>
#include <cassert>
#include <cmath>
#include <iostream>


#if 0
#define LOCAL_LOG(X) FSLLOG("PlatformNativeWindowX11: " << X)
#else
#define LOCAL_LOG(X) {}
#endif

// This file is a good reference for how X11 works https://cgit.freedesktop.org/xorg/app/xev/tree/xev.c
// This code is not a reproduction of the code at the link, but only here to give others a
// good reference source.

namespace Fsl
{
  namespace
  {
    const int MAGIC_DEFAULT_DPI = 96;

    std::weak_ptr<INativeWindowEventQueue> g_eventQueue;
    int WaitForMap(Display* display, XEvent* event, XPointer arg)
    {
      return (event->type == MapNotify && event->xmap.window == *reinterpret_cast<Window*>(arg));
    }

    VirtualMouseButton::Enum MouseToVirtualMouse(unsigned int button)
    {
      switch (button)
      {
      case Button1:
        return VirtualMouseButton::Left;
      case Button2:
        return VirtualMouseButton::Middle;
      case Button3:
        return VirtualMouseButton::Right;
      default:
        return VirtualMouseButton::Left;
      }
    }


    VirtualKey::Enum KeyToVirtualKey(XKeyEvent * kEvent)
    {
      KeySym keyId = XLookupKeysym(kEvent, 0);

      switch(keyId)
      {
      case XK_A:
      case XK_a:
        return VirtualKey::A;
      case XK_B:
      case XK_b:
        return VirtualKey::B;
      case XK_C:
      case XK_c:
        return VirtualKey::C;
      case XK_D:
      case XK_d:
        return VirtualKey::D;
      case XK_E:
      case XK_e:
        return VirtualKey::E;
      case XK_F:
      case XK_f:
        return VirtualKey::F;
      case XK_G:
      case XK_g:
        return VirtualKey::G;
      case XK_H:
      case XK_h:
        return VirtualKey::H;
      case XK_I:
      case XK_i:
        return VirtualKey::I;
      case XK_J:
      case XK_j:
        return VirtualKey::J;
      case XK_K:
      case XK_k:
        return VirtualKey::K;
      case XK_L:
      case XK_l:
        return VirtualKey::L;
      case XK_M:
      case XK_m:
        return VirtualKey::M;
      case XK_N:
      case XK_n:
        return VirtualKey::N;
      case XK_O:
      case XK_o:
        return VirtualKey::O;
      case XK_P:
      case XK_p:
        return VirtualKey::P;
      case XK_Q:
      case XK_q:
        return VirtualKey::Q;
      case XK_R:
      case XK_r:
        return VirtualKey::R;
      case XK_S:
      case XK_s:
        return VirtualKey::S;
      case XK_T:
      case XK_t:
        return VirtualKey::T;
      case XK_U:
      case XK_u:
        return VirtualKey::U;
      case XK_V:
      case XK_v:
        return VirtualKey::V;
      case XK_W:
      case XK_w:
        return VirtualKey::W;
      case XK_X:
      case XK_x:
        return VirtualKey::X;
      case XK_Y:
      case XK_y:
        return VirtualKey::Y;
      case XK_Z:
      case XK_z:
        return VirtualKey::Z;
      case XK_0:
        return VirtualKey::Code0;
      case XK_1:
        return VirtualKey::Code1;
      case XK_2:
        return VirtualKey::Code2;
      case XK_3:
        return VirtualKey::Code3;
      case XK_4:
        return VirtualKey::Code4;
      case XK_5:
        return VirtualKey::Code5;
      case XK_6:
        return VirtualKey::Code6;
      case XK_7:
        return VirtualKey::Code7;
      case XK_8:
        return VirtualKey::Code8;
      case XK_9:
        return VirtualKey::Code9;
      case XK_Escape:
        return VirtualKey::Escape;
      case XK_Tab:
        return VirtualKey::Tab;
      case XK_Return:
        return VirtualKey::Return;
      case XK_space:
        return VirtualKey::Space;
      case XK_BackSpace:
        return VirtualKey::Backspace;
      case XK_Up:
        return VirtualKey::UpArrow;
      case XK_Down:
        return VirtualKey::DownArrow;
      case XK_Left:
        return VirtualKey::LeftArrow;
      case XK_Right:
        return VirtualKey::RightArrow;
      case XK_F1:
        return VirtualKey::F1;
      case XK_F2:
        return VirtualKey::F2;
      case XK_F3:
        return VirtualKey::F3;
      case XK_F4:
        return VirtualKey::F4;
      case XK_F5:
        return VirtualKey::F5;
      case XK_F6:
        return VirtualKey::F6;
      case XK_F7:
        return VirtualKey::F7;
      case XK_F8:
        return VirtualKey::F8;
      case XK_F9:
        return VirtualKey::F9;
      case XK_F10:
        return VirtualKey::F10;
      case XK_F11:
        return VirtualKey::F11;
      case XK_F12:
        return VirtualKey::F12;
      case XK_Insert:
        return VirtualKey::Insert;
      case XK_Delete:
        return VirtualKey::Delete;
      case XK_Home:
        return VirtualKey::Home;
      case XK_End:
        return VirtualKey::End;
      case XK_Page_Up:
        return VirtualKey::PageUp;
      case XK_Page_Down:
        return VirtualKey::PageDown;
      case XK_Print:
        return VirtualKey::PrintScreen;
      case XK_Pause:
        return VirtualKey::ScrollLock;
      case XK_Scroll_Lock:
        return VirtualKey::ScrollLock;
      default:
        return VirtualKey::Tab;
      }
    }


    //! @note dont call this very often, its probably slow as hell
    bool TryLookupDisplayScreenSize(PlatformNativeDisplayType platformDisplay, PlatformNativeWindowType platformWindow, XRRScreenSize& rScreenSize)
    { // Cache some window info
      bool found = false;
      XRRScreenConfiguration* pScreenInfo = XRRGetScreenInfo(platformDisplay, platformWindow);
      {
        Rotation currentRotation;
        const auto currentSize = XRRConfigCurrentConfiguration (pScreenInfo, &currentRotation);
        int  sizeCount;
        XRRScreenSize* pSizes = XRRConfigSizes(pScreenInfo, &sizeCount);
        if( currentSize <= sizeCount )
        {
          found = true;
          rScreenSize = pSizes[currentSize];
        }
        else
        {
          rScreenSize = XRRScreenSize{};
          found = false;
        }
      }
      XRRFreeScreenConfigInfo(pScreenInfo);
      return found;
    }


    int CalcDPI(const int width, const int millimeterWidth)
    {
      assert(width > 0);
      assert(millimeterWidth > 0);
      // 1mm = 0.0393701f inches
      const double w = static_cast<double>(width);
      const double inchesWidth = static_cast<double>(millimeterWidth) * 0.0393701;
      return static_cast<int>(std::round(w / inchesWidth));
    }


    void UpdateDPIIfPossible(PlatformNativeDisplayType platformDisplay, PlatformNativeWindowType platformWindow, Point2& rScreenDPI)
    {
      XRRScreenSize screenSize{};
      if( TryLookupDisplayScreenSize(platformDisplay, platformWindow, screenSize) )
      {
        if(screenSize.width > 0 && screenSize.height > 0 && screenSize.mwidth > 0 && screenSize.mheight > 0)
        {
          LOCAL_LOG("screenSize.width" << screenSize.width);
          LOCAL_LOG("screenSize.height" << screenSize.height);
          LOCAL_LOG("screenSize.mwidth" << screenSize.mwidth);
          LOCAL_LOG("screenSize.mheight" << screenSize.mheight);
          rScreenDPI = Point2(CalcDPI(screenSize.width, screenSize.mwidth), CalcDPI(screenSize.height, screenSize.mheight));
          LOCAL_LOG("DPI: " << rScreenDPI.X << ", " << rScreenDPI.Y);
        }
        else
        {
          FSLLOG_WARNING("Failed to acquire proper DPI information, using defaults");
        }
      }
      else
      {
        FSLLOG_WARNING("Failed to acquire real screen DPI using defaults");
      }
    }


    std::shared_ptr<INativeWindow> AllocateWindow(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& windowParams, const PlatformNativeWindowAllocationParams*const pPlatformCustomWindowAllocationParams)
    {
      return std::make_shared<PlatformNativeWindowX11>(nativeWindowSetup, windowParams, pPlatformCustomWindowAllocationParams);
    }
  }


  PlatformNativeWindowSystemX11::PlatformNativeWindowSystemX11(const NativeWindowSystemSetup& setup, const PlatformNativeWindowAllocationFunction& allocateWindowFunction, const PlatformNativeWindowSystemParams& systemParams)
    : PlatformNativeWindowSystem(setup, nullptr)
    , m_visualId(0)
    , m_allocationFunction(allocateWindowFunction ? allocateWindowFunction : AllocateWindow)
    , m_extensionRREnabled(false)
    , m_rrEventBase(0)
    , m_rrErrorBase(0)
  {
    FSLLOG_WARNING_IF(setup.GetConfig().GetDisplayId() != 0, "X11 only supports the main display. Using DisplayId 0 instead of " << setup.GetConfig().GetDisplayId());
    LOCAL_LOG("XOpenDisplay");
    m_platformDisplay = XOpenDisplay(nullptr);
    if (!m_platformDisplay)
      throw GraphicsException("Could not open display");

    // Lookup the default visual
    auto defaultVisual = DefaultVisual(m_platformDisplay, 0);
    m_visualId = XVisualIDFromVisual(defaultVisual);

    // Get some extension info
    m_extensionRREnabled = (XRRQueryExtension(m_platformDisplay, &m_rrEventBase, &m_rrErrorBase) != 0);
    LOCAL_LOG("Extension RR: " << m_extensionRREnabled);
    if( m_extensionRREnabled )
    {
      int major, minor;
      if (XRRQueryVersion (m_platformDisplay, &major, &minor))
      {
        LOCAL_LOG("RR version: " << major << "." << minor);
        if (major == 1 && minor <= 1)
        {
          FSLLOG_WARNING("PlatformNativeWindowSystemX11: Due to the XRR version some features have been disabled.");
          m_extensionRREnabled = false;
        }
      }
    }

    LOCAL_LOG("XOpenDisplay: Completed");
  }


  PlatformNativeWindowSystemX11::~PlatformNativeWindowSystemX11()
  {
    LOCAL_LOG("XCloseDisplay begin");
    XCloseDisplay(m_platformDisplay);
    LOCAL_LOG("XCloseDisplay done");
  }


  std::shared_ptr<INativeWindow> PlatformNativeWindowSystemX11::CreateNativeWindow(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowAllocationParams*const pPlatformCustomWindowAllocationParams)
  {
    const auto window = m_allocationFunction(nativeWindowSetup, PlatformNativeWindowParams(m_platformDisplay, m_visualId, m_extensionRREnabled), pPlatformCustomWindowAllocationParams);

    auto ptr = std::dynamic_pointer_cast<PlatformNativeWindowX11>(window);
    if (!ptr)
      throw NotSupportedException("Allocation function did not allocate a PlatformNativeWindowX11 as required");

    SetCreatedWindow(ptr);
    return window;
  }


  bool PlatformNativeWindowSystemX11::ProcessMessages(const NativeWindowProcessMessagesArgs& args)
  {
    const std::shared_ptr<PlatformNativeWindowX11> window = m_window.lock();
    VirtualKey::Enum keyCode;
    std::shared_ptr<INativeWindowEventQueue> eventQueue = g_eventQueue.lock();
    Point2 mousePosition;
    VirtualMouseButton::Enum mouseButton;
    bool bQuit = false;
    XEvent event;
    static int wheelEvents = 0;

    while (XPending(m_platformDisplay) > 0)
    {
      XNextEvent(m_platformDisplay, &event);

      switch(event.type)
      {
      case ConfigureNotify:
        if( window && event.xconfigure.window == window->GetPlatformWindow() )
        {
          // FSLLOG("Window moved or resized");
          window->OnConfigureNotify(event.xconfigure, eventQueue);
        }
        break;
      case DestroyNotify:
        bQuit = true;
        break;
      case ClientMessage:
        if( window )
        {
          if ((Atom)event.xclient.data.l[0] == window->m_wm_delete_window)
          {
            bQuit = true;
          }
          break;
        }
      case ButtonPress:
      {
        mousePosition.X = event.xbutton.x;
        mousePosition.Y = event.xbutton.y;
        if(event.xbutton.button<Button4)
        {
          //Regular Mouse Buttons
          mouseButton = MouseToVirtualMouse(event.xbutton.button);
          if( eventQueue )
            eventQueue->PostEvent(NativeWindowEventHelper::EncodeInputMouseButtonEvent(mouseButton, true, mousePosition));
        }
        else
        {
          //Wheel Event
          if(Button4==event.xbutton.button)
            wheelEvents++;
          else if(Button5==event.xbutton.button)
            wheelEvents--;
          if( eventQueue )
            eventQueue->PostEvent(NativeWindowEventHelper::EncodeInputMouseWheelEvent(wheelEvents, mousePosition));
        }
        break;
      }
      case ButtonRelease:
      {
        mousePosition.X = event.xbutton.x;
        mousePosition.Y = event.xbutton.y;
        mouseButton = MouseToVirtualMouse(event.xbutton.button);
        if( eventQueue )
          eventQueue->PostEvent(NativeWindowEventHelper::EncodeInputMouseButtonEvent(mouseButton, false, mousePosition));
        break;
      }
      case KeyPress:
      {
        keyCode = KeyToVirtualKey(&event.xkey);
        if( eventQueue )
          eventQueue->PostEvent(NativeWindowEventHelper::EncodeInputKeyEvent(keyCode, true));
        break;
      }
      case KeyRelease:
      {
        keyCode = KeyToVirtualKey(&event.xkey);
        if( eventQueue )
          eventQueue->PostEvent(NativeWindowEventHelper::EncodeInputKeyEvent(keyCode, false));
        break;
      }
      case MotionNotify:
      {
        mousePosition.X = event.xmotion.x;
        mousePosition.Y = event.xmotion.y;
        if( eventQueue )
          eventQueue->PostEvent(NativeWindowEventHelper::EncodeInputMouseMoveEvent(mousePosition));
        break;
      }
      default:
        if (window && m_extensionRREnabled)
        {
          if (event.type == m_rrEventBase + RRScreenChangeNotify)
            window->OnRRScreenChangeNotify(&event, eventQueue);
        }
        break;
      }

    }
    return !bQuit;
  }


  void PlatformNativeWindowSystemX11::SetCreatedWindow(const std::weak_ptr<PlatformNativeWindowX11>& window)
  {
    LOCAL_LOG("PlatformNativeWindowSystemX11: SetCreatedWindow");
    auto currentWindow = m_window.lock();
    if( currentWindow )
      throw NotSupportedException("We only support one active window");

    m_window = window;
  }


  PlatformNativeWindowX11::PlatformNativeWindowX11(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& platformWindowParams, const PlatformNativeWindowAllocationParams*const pPlatformCustomWindowAllocationParams)
    : PlatformNativeWindow(nativeWindowSetup, platformWindowParams, pPlatformCustomWindowAllocationParams)
    , m_pVisual(nullptr)
    , m_cachedWindowSize()
    , m_cachedScreenDPI(MAGIC_DEFAULT_DPI, MAGIC_DEFAULT_DPI)
  {
    LOCAL_LOG("PlatformNativeWindowSystemX11: 1");

    const NativeWindowConfig nativeWindowConfig = nativeWindowSetup.GetConfig();
    int windowWidth, windowHeight, windowX, windowY;
    g_eventQueue = nativeWindowSetup.GetEventQueue();

    XVisualInfo visualInfo{};
    visualInfo.visualid = platformWindowParams.VisualId;

    int visualItemCount;
    m_pVisual = XGetVisualInfo(m_platformDisplay, VisualIDMask, &visualInfo, &visualItemCount);
    if( m_pVisual == nullptr )
      throw GraphicsException("Could not get native visual");

    const int screen = DefaultScreen(m_platformDisplay);
    const Window rootwindow =RootWindow(m_platformDisplay, screen);
    XWindowAttributes windowATTR;
    XGetWindowAttributes(m_platformDisplay,rootwindow,&windowATTR);
    //GET SCREEN RESOURCES FOR THE DEVICE
    //XRRScreenResources * screenResources = XRRGetScreenResources(m_platformDisplay, rootwindow);

    //We now have info on the Screen Resources, get information about the available CRTs
    //XRRCrtcInfo * crtDeviceInformation = XRRGetCrtcInfo(m_platformDisplay, screenResources, screenResources->crtcs[1]);

    //printf("Specific CRT information screen count %d,  %dx%d   window %dx%d\n", screenResources->noutput, crtDeviceInformation->width, crtDeviceInformation->height, windowATTR.width, windowATTR.height);

    if (nativeWindowConfig.GetWindowMode() != WindowMode::Window)
    {
      windowWidth  = windowATTR.width ;
      windowHeight = windowATTR.height;
      windowX = 0;
      windowY = 0;
    }
    else
    {
      const Rectangle windowRectangle = nativeWindowConfig.GetWindowRectangle();
      windowWidth = windowRectangle.Width();
      windowHeight = windowRectangle.Height();
      windowX = windowRectangle.X();
      windowY = windowRectangle.Y();
    }

    m_colormap = XCreateColormap(m_platformDisplay, rootwindow, m_pVisual->visual, AllocNone);

    const unsigned long mask = CWBackPixel | CWBorderPixel | CWEventMask | CWColormap;

    XSetWindowAttributes windowAttributes;
    windowAttributes.colormap = m_colormap;
    windowAttributes.background_pixel = 0xFFFFFFFF;
    windowAttributes.border_pixel = 0;
    windowAttributes.event_mask = StructureNotifyMask | ExposureMask;

    m_platformWindow = XCreateWindow(m_platformDisplay, rootwindow, windowX, windowY, windowWidth, windowHeight,
                                     0, m_pVisual->depth, InputOutput, m_pVisual->visual, mask, &windowAttributes);

    XMapWindow(m_platformDisplay, m_platformWindow);

    XEvent event;
    XIfEvent(m_platformDisplay, &event, WaitForMap, reinterpret_cast<XPointer>(&m_platformWindow));

    XSetWMColormapWindows(m_platformDisplay, m_platformWindow, &m_platformWindow, 1);

    XFlush(m_platformDisplay);

    // Hook up and listen for window close
    m_wm_delete_window=XInternAtom(m_platformDisplay, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(m_platformDisplay, m_platformWindow, &m_wm_delete_window, 1);

    // Listen for various events
    const long evMask = ExposureMask | StructureNotifyMask | VisibilityChangeMask |
      ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask |
      EnterWindowMask | LeaveWindowMask | PointerMotionMask;
    XSelectInput(m_platformDisplay, m_platformWindow, evMask);

    // Enable some XRR events.
    if( platformWindowParams.ExtensionRREnabled )
      XRRSelectInput(m_platformDisplay, m_platformWindow, RRScreenChangeNotifyMask);

    m_cachedWindowSize = Point2(windowWidth, windowHeight);
    UpdateDPIIfPossible(m_platformDisplay, m_platformWindow, m_cachedScreenDPI);

    { // Post the activation message to let the framework know we are ready
      std::shared_ptr<INativeWindowEventQueue> eventQueue = g_eventQueue.lock();
      if( eventQueue )
        eventQueue->PostEvent(NativeWindowEventHelper::EncodeWindowActivationEvent(true));
    }
    LOCAL_LOG("PlatformNativeWindowSystemX11: 2");
  }


  PlatformNativeWindowX11::~PlatformNativeWindowX11()
  {
    // X windows clean up
    XDestroyWindow(m_platformDisplay, m_platformWindow);
    XFreeColormap(m_platformDisplay, m_colormap);

    if( m_pVisual != nullptr )
      XFree(m_pVisual);
  }


  bool PlatformNativeWindowX11::TryGetDPI(Vector2& rDPI) const
  {
    rDPI = Vector2(m_cachedScreenDPI.X, m_cachedScreenDPI.Y);
    return true;
  }

  bool PlatformNativeWindowX11::TryGetSize(Point2& rSize) const
  {
    rSize = m_cachedWindowSize;
    return true;
  }


  void PlatformNativeWindowX11::OnConfigureNotify(const XConfigureEvent& event, const std::shared_ptr<INativeWindowEventQueue>& eventQueue)
  {
    Point2 newSize(event.width, event.height);
    if( newSize == m_cachedWindowSize )
      return;

    m_cachedWindowSize = newSize;
    LOCAL_LOG("Updating cached size to " << m_cachedWindowSize.X << ", " << m_cachedWindowSize.Y);

    if( eventQueue )
      eventQueue->PostEvent(NativeWindowEventHelper::EncodeWindowResizedEvent());
  }


  void PlatformNativeWindowX11::OnRRScreenChangeNotify(XEvent* pEvent, const std::shared_ptr<INativeWindowEventQueue>& eventQueue)
  {
    XRRUpdateConfiguration(pEvent);
    XRRScreenChangeNotifyEvent* pSpecificEvent = reinterpret_cast<XRRScreenChangeNotifyEvent*>(pEvent);

    Point2 newDPI(CalcDPI(pSpecificEvent->width, pSpecificEvent->mwidth), CalcDPI(pSpecificEvent->height, pSpecificEvent->mheight));
    if( newDPI == m_cachedScreenDPI )
      return;

    m_cachedScreenDPI = newDPI;
    LOCAL_LOG("Cached DPI updated: " << m_cachedScreenDPI.X << ", " << m_cachedScreenDPI.Y);
    if( eventQueue )
      eventQueue->PostEvent(NativeWindowEventHelper::EncodeWindowDPIChanged());
  }

}
#endif