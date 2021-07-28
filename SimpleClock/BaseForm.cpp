#include "stdafx.h"
#include "BaseForm.h"

BaseForm::BaseForm(HINSTANCE app_instance, LPCWSTR className, LPCWSTR titleName)
	:m_className(className), m_windowsTitle(titleName), m_hInstance(app_instance), m_extra_data(new WND_MSG(this))
{
	this->InitializeWindow();
}

BaseForm::~BaseForm() {
	delete this->m_extra_data;
}

bool BaseForm::InitializeWindow()
{
	WNDCLASSEX wcx;

	wcx.cbClsExtra    = NULL;
	wcx.cbSize        = sizeof(WNDCLASSEX);
	wcx.cbWndExtra    = NULL;
	wcx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcx.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wcx.hIcon         = LoadIcon(this->m_hInstance, IDI_APPLICATION);
	wcx.hIconSm       = LoadIcon(this->m_hInstance, IDI_APPLICATION);
	wcx.hInstance     = this->m_hInstance;
	wcx.lpfnWndProc   = BaseForm::WndProc;
	wcx.lpszClassName = this->m_className;
	wcx.lpszMenuName  = NULL;
	wcx.style         = CS_HREDRAW | CS_VREDRAW;

	// register window class
	if (!::RegisterClassEx(&wcx)) {
		MessageBox(NULL, L"Fail to register windows class.", this->m_className, MB_OK);
		return false;
	}

	// create windows
	if (!(this->m_hwnd = ::CreateWindowEx(NULL,
										  this->m_className, this->m_windowsTitle, NULL,
										  100, 100, 500, 500, NULL, NULL, this->m_hInstance, 
										  (LPVOID)this->m_extra_data))) {
		MessageBox(NULL, L"Fail to create windows.", this->m_className, MB_OK);
		return false;
	}

	return true;
}

void BaseForm::ShowWindow() {
	::ShowWindow(this->m_hwnd, SW_SHOW);
	::UpdateWindow(this->m_hwnd);

	MSG _msg;
	while (GetMessage(&_msg, this->m_hwnd, 0, 0)) {
		TranslateMessage(&_msg);
		if (_msg.message == WM_NULL)
			break;
		DispatchMessage(&_msg);
	}
}

LRESULT CALLBACK BaseForm::WndProc(_In_ HWND w_ins, _In_ UINT _msg,
	_In_ WPARAM _wp, _In_ LPARAM _lp) {
	switch (_msg) {
	case WM_CREATE: {
		CREATESTRUCT* lp = reinterpret_cast<CREATESTRUCT*>(_lp);
		WND_MSG* extra_data = reinterpret_cast<WND_MSG*>(lp->lpCreateParams);
		::SetWindowLongPtr(w_ins, GWLP_USERDATA, (LONG_PTR)extra_data);
	} return 0;
	case WM_CLOSE: {
		if (MessageBox(NULL, L"Are you sure to close this windows?", L"Message", MB_OKCANCEL) == IDOK) {
			DestroyWindow(w_ins);
		}
	} return 0;
	case WM_DESTROY: {
		PostQuitMessage(0);
	} return 0;
	}

	return ::DefWindowProc(w_ins, _msg, _wp, _lp);
}