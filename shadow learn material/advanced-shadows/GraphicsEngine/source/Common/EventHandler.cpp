#include "stdafx.h"
#include "EventHandler.h"


/* EventHandler::EventListener ***********************************************/
void EventHandler::EventListener::KeyEvent(EventHandler&, unsigned int, State)
{
	// do nothing
}

void EventHandler::EventListener::MouseEvent(EventHandler&, int, int, Button, State)
{
	// do nothing
}

void EventHandler::EventListener::MouseWheelEvent(EventHandler&, int)
{
	// do nothing
}

void EventHandler::EventListener::ReshapeEvent(EventHandler&, int, int)
{
	// do nothing
}

void EventHandler::EventListener::QuitEvent(EventHandler&)
{
	// do nothing
}


/* EventHandler **************************************************************/
EventHandler::EventHandler() :
	_eventListeners(COUNT)
{
}

EventHandler::~EventHandler()
{
}

void EventHandler::Register(EventTypes type, EventListener& listener)
{
	_eventListeners[type].push_back(&listener);
}

void EventHandler::UnRegister(EventTypes type, EventListener& listener)
{
	for (tListenerArray::iterator i = _eventListeners[type].begin();
		i != _eventListeners[type].end();
		i++)
	{
		if (*i == &listener)
		{
			_eventListeners[type].erase(i);
			break;
		}
	}
}

void EventHandler::DispatchMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_DESTROY:
			SendQuitEvent();
			break;
		case WM_KEYDOWN:
			SendKeyEvent(static_cast<unsigned int>(wParam), EventListener::Down);
			break;
		case WM_KEYUP:
			SendKeyEvent(static_cast<unsigned int>(wParam), EventListener::Up);
			break;
		case WM_MOUSEMOVE:
			SendMouseEvent(LOWORD(lParam), HIWORD(lParam), EventListener::None, EventListener::Up);
			break;
		case WM_LBUTTONDOWN:
			SendMouseEvent(LOWORD(lParam), HIWORD(lParam), EventListener::Left, EventListener::Down);
			break;
		case WM_LBUTTONUP:
			SendMouseEvent(LOWORD(lParam), HIWORD(lParam), EventListener::Left, EventListener::Up);
			break;
		case WM_LBUTTONDBLCLK:
			SendMouseEvent(LOWORD(lParam), HIWORD(lParam), EventListener::Left, EventListener::DblClk);
			break;
		case WM_RBUTTONDOWN:
			SendMouseEvent(LOWORD(lParam), HIWORD(lParam), EventListener::Right, EventListener::Down);
			break;
		case WM_RBUTTONUP:
			SendMouseEvent(LOWORD(lParam), HIWORD(lParam), EventListener::Right, EventListener::Up);
			break;
		case WM_RBUTTONDBLCLK:
			SendMouseEvent(LOWORD(lParam), HIWORD(lParam), EventListener::Right, EventListener::DblClk);
			break;
		case WM_SIZE:
			SendReshapeEvent(LOWORD(lParam), HIWORD(lParam));
			break;
		case WM_MOUSEWHEEL:
			SendMouseWheelEvent(GET_WHEEL_DELTA_WPARAM(wParam));
			break;
		default:
			break;

	}
}

void EventHandler::SendKeyEvent(unsigned int key, EventListener::State state)
{
	for (unsigned int i = 0; i < _eventListeners[KeyEvent].size(); ++i)
		_eventListeners[KeyEvent][i]->KeyEvent(*this, key, state);
}

void EventHandler::SendMouseEvent(int x, int y, EventListener::Button button, EventListener::State state)
{
	for (unsigned int i = 0; i < _eventListeners[MouseEvent].size(); ++i)
		_eventListeners[MouseEvent][i]->MouseEvent(*this, x, y, button, state);
}

void EventHandler::SendMouseWheelEvent(int delta)
{
	for (unsigned int i = 0; i < _eventListeners[MouseWheelEvent].size(); ++i)
		_eventListeners[MouseWheelEvent][i]->MouseWheelEvent(*this, delta);
}

void EventHandler::SendReshapeEvent(int wndWidth, int wndHeight)
{
	for (unsigned int i = 0; i < _eventListeners[ReshapeEvent].size(); ++i)
		_eventListeners[ReshapeEvent][i]->ReshapeEvent(*this, wndWidth, wndHeight);
}

void EventHandler::SendQuitEvent()
{
	for (unsigned int i = 0; i < _eventListeners[QuitEvent].size(); ++i)
		_eventListeners[QuitEvent][i]->QuitEvent(*this);
}

void EventHandler::SetMousePosition(int x, int y)
{
	::SetCursorPos(x, y);
}

bool EventHandler::GetStateDown(unsigned int key, HWND hWnd)
{
	if (::GetFocus() != hWnd)
		return false;
  
	return (::GetAsyncKeyState(key) & 0x8000) ? true : false;
}