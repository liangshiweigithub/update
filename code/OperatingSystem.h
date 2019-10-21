#pragma once
#if !defined(OPERATING_SYSTEM_HEADER)
#define OPERATING_SYSTEM_HEADER
#endif
#define VK_USE_PLATFORM_WIN32_KHR

#if defined(VK_USE_PLATFORM_WIN32_KHR)
#include <Windows.h>

#elif defined(VK_USE_PLATFORM_XCB_KHR)
#include <xcb/xcb.h>
#include <dlfcn.h>
#include <cstdlib>

#elif defined(VK_USE_PLATFORM_XLIB_KHR)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <dlfcn.h>
#include <cstdlib>

#endif

#include <cstring>
#include <iostream>

namespace ApiWithoutSecrets {
	namespace OS {
#if defined(VK_USE_PLATFORM_WIN32_KHR)
		typedef HMODULE LibraryHandle;
#elif defined(VK_USE_PLATFORM_XCB_KHR) || defined(VK_USE_PLATFORM_XLIB_KHR)
		typedef void *LibraryHandle;
#endif

		class ProjectBase {
		protected:
			bool CanRender;
		public:
			virtual bool OnWindowSizeChanged() = 0;
			virtual bool Draw() = 0;
			virtual bool ReadyToDraw() const final {
				return CanRender;
			}
			ProjectBase() :CanRender(false) {}
			virtual ~ProjectBase() {}
		};

		struct WindowParameters
		{
#if defined(VK_USE_PLATFORM_WIN32_KHR)
			HINSTANCE Instance;
			HWND Handle;
			WindowParameters() :Instance(), Handle() {}
#elif defined(VK_USE_PLATFORM_XCB_KHR)
			xcb_connection_t *Connection;
			xcb_window_t Handle;
			WindowParameters() : Connection(), Handle() {}
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
			Display *DisplayPtr;
			Window Handle;

			WindowParameters() : DisplayPtr(), Handle() {}
#endif
		};

		class Window {
		private: WindowParameters Parametes;
		public:
			Window();
			~Window();
			
			bool Create(const char *title);
			bool RenderingLoop(ProjectBase &project)const;
			WindowParameters GetParametes()const;
		};
	}
}