#ifndef _BASE_FORM_H
#define _BASE_FORM_H

#include "stdafx.h"

#include<Windows.h>
#include<tchar.h>
#include<ShObjIdl.h>

struct WND_MSG;

class BaseForm {
private:
	HINSTANCE m_hInstance;
	HWND      m_hwnd;
	LPCWSTR   m_className;
	LPCWSTR   m_windowsTitle;
	WND_MSG*  m_extra_data;

	bool InitializeWindow();
public:
	BaseForm() = delete;
	BaseForm(HINSTANCE app_hinstance, LPCWSTR className, LPCWSTR windowsTitle);

	~BaseForm();

	void ShowWindow();

	static LRESULT CALLBACK WndProc(_In_ HWND, _In_ UINT, _In_ WPARAM, _In_ LPARAM);
};

struct WND_MSG {
	BaseForm* m_form;
	WND_MSG(BaseForm* pData): m_form(pData){}
};

#endif // _BASE_FORM_H