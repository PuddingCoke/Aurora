#include<Aurora/Window/Win32Form.h>

Win32Form::Win32Form(const std::wstring& title, const UINT& startX, const UINT& startY, const UINT& width, const UINT& height, const DWORD& windowStyle, LRESULT(*windowCallBack)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)):
	hwnd(nullptr),msg{}
{
	const HINSTANCE hInstance = GetModuleHandle(0);

	WNDCLASSEX wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wcex.hIcon = LoadIcon(0, IDI_APPLICATION);
	wcex.hIconSm = LoadIcon(0, IDI_APPLICATION);
	wcex.lpszClassName = L"MyWindowClass";
	wcex.hInstance = hInstance;
	wcex.lpfnWndProc = windowCallBack;

	RegisterClassEx(&wcex);

	RECT rect = { 0,0,(LONG)width,(LONG)height };

	AdjustWindowRect(&rect, windowStyle, false);

	hwnd = CreateWindow(L"MyWindowClass", title.c_str(), windowStyle, startX, startY, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, hInstance, nullptr);

	if (!hwnd)
	{
		throw "Create Window Failed!";
	}

	ShowWindow(hwnd, SW_SHOW);
}

bool Win32Form::pollEvents()
{
	if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.message != WM_QUIT;
}

const HWND& Win32Form::getHWND() const
{
	return hwnd;
}
