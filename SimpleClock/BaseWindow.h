#ifndef _BASE_WINDOW_H
#define _BASE_WINDOW_H

#include "stdafx.h"
#include "logger.h"
#include<Windows.h>
#include<cassert>

template<class DerivedWindow>
class BaseWND {
protected:
	HWND m_hwnd;

public:
	logger* _logger;

	static LRESULT CALLBACK WND_PROC(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
		// forward ... don't directly return a value
		DerivedWindow* pBW;
		if (msg == WM_NCCREATE) {
			CREATESTRUCT* pCs = reinterpret_cast<CREATESTRUCT*>(lp);
			pBW               = reinterpret_cast<DerivedWindow*>(pCs->lpCreateParams);
			::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pBW);
			pBW->m_hwnd = hwnd;
		}
		else {
			pBW = reinterpret_cast<DerivedWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		}

		if (pBW) {
			return pBW->handleProc(msg, wp, lp);
		}
		else {
			return ::DefWindowProc(hwnd, msg, wp, lp);
		}
	}
	BaseWND(): m_hwnd(nullptr){
	}
	virtual ~BaseWND() {
	}

	virtual bool Create(
		LPCWSTR WindowName,
		DWORD dwStyle = WS_SYSMENU | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SIZEBOX,
		int x        = CW_USEDEFAULT,
		int y        = CW_USEDEFAULT,
		int width    = CW_USEDEFAULT,
		int height   = CW_USEDEFAULT,
		HWND hParent = NULL,
		HMENU hMenu  = NULL
	) {
		WNDCLASSEX wcx;
		::ZeroMemory(&wcx, sizeof(WNDCLASSEX));

		wcx.cbSize        = sizeof(WNDCLASSEX);
		wcx.hCursor       = LoadCursor(GetModuleHandle(NULL), IDC_ARROW);
		wcx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcx.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
		wcx.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
		wcx.lpszClassName = className();
		wcx.lpfnWndProc   = BaseWND::WND_PROC;
		wcx.style         = CS_HREDRAW | CS_VREDRAW;
		wcx.hInstance     = GetModuleHandle(NULL);

		if(!::RegisterClassEx(&wcx)) return false;
		this->m_hwnd = ::CreateWindowEx(NULL, className(), WindowName,
									    dwStyle, x, y, width, height, 
									    hParent, hMenu, GetModuleHandle(NULL), this);
		return this->m_hwnd ? true : false;
	}

	inline HWND window() { return this->m_hwnd; }

	void showWindow() {
		if (m_hwnd != nullptr) {
			::ShowWindow(this->m_hwnd, SW_NORMAL);
			::UpdateWindow(this->m_hwnd);

			// message loop
			MSG msg;
			while (GetMessage(&msg, this->m_hwnd, 0, 0)) {
				TranslateMessage(&msg);
				if (msg.message == WM_NULL)
					break;
				DispatchMessage(&msg);
			}
		}
		return;
	}

	virtual LPCWSTR className() = 0;
	virtual LRESULT handleProc(UINT _msg, WPARAM wp, LPARAM lp) = 0;
};

#endif // _BASE_WINDOW_H