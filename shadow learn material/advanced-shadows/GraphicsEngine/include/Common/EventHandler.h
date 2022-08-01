#pragma once

#include <vector>

#include "CommonExports.h"

#include "Singleton.h"

class COMMON_API EventHandler : public Singleton<EventHandler>
{
public:
	enum EventTypes
	{
		KeyEvent = 0,
		MouseEvent,
		MouseWheelEvent,
		ReshapeEvent,
		QuitEvent,
		COUNT
	};

	class COMMON_API EventListener
	{
	public:
		enum State
		{
			Down = 0,
			Up,
			DblClk
		};

		enum Button
		{
			None = 0,
			Left,
			Right
		};

		virtual void KeyEvent(EventHandler& handler, unsigned int key, State state);

		virtual void MouseEvent(EventHandler& handler, int x, int y, Button button, State state);

		virtual void MouseWheelEvent(EventHandler& handler, int delta);

		virtual void ReshapeEvent(EventHandler& handler, int wndWidth, int wndHeight);

		virtual void QuitEvent(EventHandler& handler);
	};

	EventHandler();
	virtual ~EventHandler();

	void Register(EventTypes type, EventListener& listener);
	void UnRegister(EventTypes type, EventListener& listener);

	void SendKeyEvent(unsigned int key, EventListener::State state);

	void SendMouseEvent(int x, int y, EventListener::Button button, EventListener::State state);

	void SendMouseWheelEvent(int delta);

	void SendReshapeEvent(int wndWidth, int wndHeight);

	void SendQuitEvent();

	void DispatchMsg(UINT message, WPARAM wParam, LPARAM lParam);

	void SetMousePosition(int x, int y);

	bool GetStateDown(unsigned int key, HWND hWnd);

private:
	typedef std::vector<EventListener*> tListenerArray;
	typedef std::vector<tListenerArray> tEventListeners;

#pragma warning(push)
#pragma warning(disable : 4251)
	tEventListeners _eventListeners;
#pragma warning(pop)	
};