#pragma once

#ifndef _WIN32FORM_H_
#define _WIN32FORM_H_

#define NOMINMAX

#include<Windows.h>
#include<string>

class Win32Form
{
public:

	Win32Form(const std::wstring& title, const UINT& startX, const UINT& startY, const UINT& width, const UINT& height, const DWORD& windowStyle, LRESULT(*windowCallBack)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam));

	bool pollEvents();

	const HWND& getHWND() const;

private:

	HWND hwnd;

	MSG msg;

};

#endif // !_WIN32FORM_H_
