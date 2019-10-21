#include <thread>
#include <chrono>
#include "OperatingSystem.h"

namespace ApiWithoutSecrets {
	namespace OS {
		Window::Window() : Parametes() {}

		WindowParameters Window::GetParametes()const {
			return Parametes;
		}
#if defined(VK_USE_PLATFORM_WIN32_KHR)
#define SERIES_NAME "API without Secrets: Introducion to Vulkan"
#endif
		LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
			switch (message)
			{
			case WM_SIZE:
			case WM_EXITSIZEMOVE:
				PostMessage(hWnd, WM_USER + 1, wParam, lParam);
				break;
			case WM_KEYDOWN:
			case WM_CLOSE:
				PostMessage(hWnd, WM_USER + 2, wParam, lParam);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			return 0;
		}
		Window::~Window() {
			if (Parametes.Handle)
				DestroyWindow(Parametes.Handle);
			if (Parametes.Instance)
				UnregisterClass((LPCWSTR)SERIES_NAME, Parametes.Instance);
		}
		bool Window::Create(const char *title)
		{
			Parametes.Instance = GetModuleHandle(nullptr);
			WNDCLASSEX wcex;

			wcex.cbSize = sizeof(WNDCLASSEX);
			wcex.style = CS_HREDRAW | CS_VREDRAW;
			wcex.lpfnWndProc = WndProc;
			wcex.cbClsExtra = 0;
			wcex.cbWndExtra = 0;
			wcex.hInstance = Parametes.Instance;
			wcex.hIcon = NULL;
			wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
			wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
			wcex.lpszMenuName = NULL;
			wcex.lpszClassName = (LPCWCH)SERIES_NAME;
			wcex.hIconSm = NULL;

			if (!RegisterClassEx(&wcex))
				return false;
			Parametes.Handle = CreateWindow((LPCWSTR)SERIES_NAME, title, WS_OVERLAPPEDWINDOW, 20, 20,
				500, 500, nullptr, nullptr, Parametes.Instance, nullptr);
			if (!Parametes.Handle)
				return false;
			return true;
		}

		bool Window::RenderingLoop(ProjectBase &project)const
		{
			ShowWindow(Parametes.Handle, SW_SHOWNORMAL);
			UpdateWindow(Parametes.Handle);

			MSG message;
			bool loop = true;
			bool resize = false;
			bool result = true;

			while (loop)
			{
				if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
				{
					switch (message.message)
					{
					case WM_USER + 1:
						resize = true;
						break;
					case WM_USER + 2:
						loop = false;
						break;
					}
					TranslateMessage(&message);
					DispatchMessage(&message);
				}
				else
				{
					if (resize)
					{
						resize = false;
						if (!project.OnWindowSizeChanged())
						{
							result = false;
							break;
						}
					}
					if (project.ReadyToDraw()) {
						if (!project.Draw()) {
							result = false;
							break;
						}
					}
					else
					{
						std::this_thread::sleep_for(std::chrono::microseconds(100));
					}
				}
			}

			return result;
		}
	}
}