#include "Window.h"
#include <Windowsx.h>
#include <string>
#include <assert.h>
#include <strsafe.h>

using namespace std;

Window::Window(int width, int height, std::wstring CLassName, std::wstring WndName)
	:Hinstace(GetModuleHandle(nullptr)), width(width), height(height),CLASS_NAME(CLassName)
{
	WNDCLASSEX  wc{};
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.hInstance = GetModuleHandle(nullptr);
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = CLassName.c_str();
	wc.hIconSm = nullptr;

	RegisterClassEx(&wc);

	DWORD style = WS_BORDER | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU;

	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	AdjustWindowRect(&wr, style, FALSE);

	 CreateWindowEx(0, CLassName.c_str(), WndName.c_str(),  style, CW_USEDEFAULT, CW_USEDEFAULT,
	 wr.right - wr.left, wr.bottom - wr.top, nullptr, nullptr, GetModuleHandle(nullptr), this);


	ShowWindow(hwnd, SW_SHOWDEFAULT);

	KeyStates.reset();
}

int Window::ProcessMessages() noexcept
{
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{

		if (msg.message == WM_QUIT)
		{
			return msg.wParam;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

void Window::DoFrame()
{
	while (!IsMouseEventEmpty())
	{
		auto e = ReadMouseEvent();

		if (e.Type == MouseEvent::Event::Move)
		{
			lol = L"(" + to_wstring(e.x) + L',' + to_wstring(e.y) + L')' + L"(" +
				to_wstring(GetMousePosXNormalized()) + L', ' + to_wstring(GetMousePosYNormalized());
			SetWindowText(hwnd, lol.c_str());
		}

	}

}

void Window::DrawMessageBox(std::wstring mes)
{
	MessageBox(hwnd, mes.c_str(), NULL, MB_OK);
}

Window::~Window()
{
	if (pDx11 != nullptr)
	{
		delete pDx11;
		pDx11 = nullptr;
	}
}


bool Window::IsKeyPressed(unsigned char key)
{
	return KeyStates[key];
}

wchar_t Window::GetChar()
{
	if (CharQueue.size() > 0)
	{
		const wchar_t c = CharQueue.front();
		CharQueue.pop();
		return c;
	}
	else return 0;
}

bool Window::IsCharEmpty()
{
	return CharQueue.empty();
}

bool Window::IsKeyboardEventEmpty()
{
	return KeyQueue.empty();
}

Window::KeyEvent Window::ReadKeyEvent()
{
	if (KeyQueue.size() > 0)
	{
		KeyEvent k = KeyQueue.front();
		KeyQueue.pop();
		return k;
	}
	else return KeyEvent(0, KeyEvent::Event::Invalid);
}

void Window::ClearKeyEvent()
{
	while (!IsKeyboardEventEmpty())
	{
		KeyQueue.pop();
	}
}

void Window::ClearCharQueue()
{
	while (!IsCharEmpty())
	{
		CharQueue.pop();
	}
}

bool Window::IsRightPressed() noexcept
{
	return RightPress;
}

bool Window::IsLeftPressed() noexcept
{
	return LeftPress;
}

bool Window::IsMiddlePressed() noexcept
{
	return MiddlePress;
}

int Window::GetMousePosX() noexcept
{
	return PosX;
}

int Window::GetMousePosY() noexcept
{
	return PosY;
}

float Window::GetMousePosXNormalized() noexcept
{
	return  ((float)PosX / (width / 2.0f) - 1.0f);
}

float Window::GetMousePosYNormalized() noexcept
{
	return  (-(float)PosY / (height / 2.0f) + 1.0f);
}

bool Window::IsMouseEventEmpty()
{
	return MouseQueue.empty();
}

Window::MouseEvent Window::ReadMouseEvent()
{
	if (MouseQueue.size() > 0)
	{
		MouseEvent m = MouseQueue.front();
		MouseQueue.pop();
		return m;
	}
	else return MouseEvent(0, 0, MouseEvent::Event::Invalid);
}

void Window::ClearMouseQueue()
{
	while (!IsMouseEventEmpty())
	{
		MouseQueue.pop();
	}
}

void Window::InitDx11()
{
	assert(pDx11 == nullptr);
	pDx11 = new Graphics(hwnd, height, width);
}

void Window::BeginFrame()
{
	pDx11->BeginFrame();
}

void Window::EndFrame()
{
	pDx11->EndFrame();
}


LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	Window* pWnd = nullptr;
	if (msg == WM_NCCREATE)
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		pWnd->hwnd = hWnd;
	}
	else
	{
		pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	}


	if (pWnd)
		return pWnd->HandleMsg(msg, wParam, lParam);
	else
		return DefWindowProc(hWnd, msg, wParam, lParam);
}


LRESULT Window::HandleMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(2);
		return 0;
		// Keboard------------------------------------------------------------------------
	case WM_KILLFOCUS:
		KeyStates.reset();
		break;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		KeyStates[(unsigned char)wParam] = true;
		KeyQueue.emplace((unsigned char)wParam, KeyEvent::Event::Press);
		break;
	case WM_SYSKEYUP:
	case WM_KEYUP:
		KeyStates[(unsigned char)wParam] = false;
		KeyQueue.emplace((unsigned char)wParam, KeyEvent::Event::Release);
		break;
	case WM_CHAR:
		CharQueue.push((wchar_t)wParam);
		break;
		// Mouse------------------------------------------------------------------------
	case WM_LBUTTONDOWN:
		LeftPress = true;
		PosX = GET_X_LPARAM(lParam);
		PosY = GET_Y_LPARAM(lParam);
		MouseQueue.emplace(PosX, PosY, MouseEvent::Event::LeftPress);
		break;
	case WM_LBUTTONUP:
		LeftPress = false;
		PosX = GET_X_LPARAM(lParam);
		PosY = GET_Y_LPARAM(lParam);
		MouseQueue.emplace(PosX, PosY, MouseEvent::Event::LeftRelease);
		break;
	case WM_MBUTTONDOWN:
		MiddlePress = true;
		PosX = GET_X_LPARAM(lParam);
		PosY = GET_Y_LPARAM(lParam);
		MouseQueue.emplace(PosX, PosY, MouseEvent::Event::WheelPrees);
		break;
	case WM_MBUTTONUP:
		MiddlePress = true;
		PosX = GET_X_LPARAM(lParam);
		PosY = GET_Y_LPARAM(lParam);
		MouseQueue.emplace(PosX, PosY, MouseEvent::Event::WheelRelease);
		break;
	case WM_RBUTTONDOWN:
		RightPress = true;
		PosX = GET_X_LPARAM(lParam);
		PosY = GET_Y_LPARAM(lParam);
		MouseQueue.emplace(PosX, PosY, MouseEvent::Event::RightPress);
		break;
	case WM_RBUTTONUP:
		RightPress = true;
		PosX = GET_X_LPARAM(lParam);
		PosY = GET_Y_LPARAM(lParam);
		MouseQueue.emplace(PosX, PosY, MouseEvent::Event::RightRelease);
		break;
	case WM_MOUSEMOVE:
		PosX = GET_X_LPARAM(lParam);
		PosY = GET_Y_LPARAM(lParam);
		MouseQueue.emplace(PosX, PosY, MouseEvent::Event::Move);
		break;
	case WM_MOUSEWHEEL:
		ScrollDelta = 0;
		ScrollAcumulate = GET_WHEEL_DELTA_WPARAM(wParam);
		if ((ScrollAcumulate / 120 >= 1) || (ScrollAcumulate / 120 <= -1))
		{
			ScrollDelta = ScrollAcumulate / 120;
			MouseEvent::Event t;
			if (ScrollDelta > 0) t = MouseEvent::Event::WheelUp;
			else  t = MouseEvent::Event::WheelDown;
			MouseQueue.emplace(ScrollDelta, 0, t);
			ScrollAcumulate = 0;
		}
		break;
		//-----------------------------------------------------------------
	case WM_SIZE:
		width = LOWORD(lParam);
		height = HIWORD(lParam);
		if (pDx11 != nullptr) pDx11->Resize(hwnd, width, height);
		break;
	}

	if (KeyQueue.size() > QueueLimit) KeyQueue.pop();
	if (CharQueue.size() > QueueLimit) CharQueue.pop();
	if (MouseQueue.size() > QueueLimit)MouseQueue.pop();


	return DefWindowProc(this->hwnd, msg, wParam, lParam);
}
